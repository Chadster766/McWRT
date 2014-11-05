/** 
  * Copyright (C) 2008-2014, Marvell International Ltd. 
  * 
  * This software file (the "File") is distributed by Marvell International 
  * Ltd. under the terms of the GNU General Public License Version 2, June 1991 
  * (the "License").  You may use, redistribute and/or modify this File in 
  * accordance with the terms and conditions of the License, a copy of which 
  * is available by writing to the Free Software Foundation, Inc.,
  * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA or on the
  * worldwide web at http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt.
  *
  * THE FILE IS DISTRIBUTED AS-IS, WITHOUT WARRANTY OF ANY KIND, AND THE 
  * IMPLIED WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE 
  * ARE EXPRESSLY DISCLAIMED.  The License provides additional details about 
  * this warranty disclaimer.
  *
  */


#include "ap8xLnxIntf.h"

#include "wltypes.h"
#include "IEEE_types.h"
#include "osif.h"

#include "mib.h"
#include "wl_mib.h"
#include "wl_hal.h"
#include "qos.h"
#include "wlmac.h"
#include "ds.h"
#include "keyMgmtCommon.h"
#include "keyMgmt.h"
#include "tkip.h"
#include "StaDb.h"
#include "macmgmtap.h"
#include "wldebug.h"

#include "macMgmtMlme.h"
#define MCBC_STN_ID         (MAX_STNS)
#ifdef MRV_8021X
#include <net/iw_handler.h>
#endif

#define MACADDR_CPY(macaddr1,macaddr2) { *(UINT16*)macaddr1 = *(UINT16*)macaddr2; \
	*(UINT16 *)((UINT16*)macaddr1+1) = *(UINT16 *)((UINT16*)macaddr2+1); \
	*(UINT16 *)((UINT16*)macaddr1+2) = *(UINT16 *)((UINT16*)macaddr2+2);}

inline static UINT32 pad(UINT8 *data,UINT32 length) ;

inline void block_function(UINT32 *L,UINT32 *R) ;
MIC_Fail_State_e gMICFailFlagVal;//indicates if 60 sec have elapsed since the last MIC failure
UINT32 gMICFailTimerId;//holds the timerID of the MIC failure timer
UINT32 gMICFailRstTimerId;//holds the timerID of the MIC failure reset timer
UINT32 uppTSC;
extern const UINT16 Sbox[2][256];

void TKIPInit(vmacApInfo_t *vmacSta_p)
{
	MIB_RSNSTATS *mib_RSNStats_p=vmacSta_p->Mib802dot11->RSNStats;
	gMICFailFlagVal = NO_MIC_FAILURE;
	vmacSta_p->MIC_ErrordisableStaAsso = 0;
	gMICFailRstTimerId = 0;
	vmacSta_p->MICCounterMeasureEnabled = 1;
	mib_RSNStats_p->TKIPCounterMeasuresInvoked = 0;
	mib_RSNStats_p->TKIPICVErrors = 0;
	mib_RSNStats_p->TKIPRemoteMICFailures = 0;
	os_SemaphoreInit(sysinfo_MIC_FAIL_SEM, 1); 
}

/*
**********************************************************************
* Routine: Phase 1 -- generate P1K, given TA, TK, IV32
*
* Inputs:
*     TK[]      = temporal key                         [128 bits]
*     TA[]      = transmitter's MAC address            [ 48 bits]
*     IV32      = upper 32 bits of IV                  [ 32 bits]
* Output:
*     P1K[]     = Phase 1 key                          [ 80 bits]
*
* Note:
*     This function only needs to be called every 2**16 packets,
*     although in theory it could be called every packet.
*
**********************************************************************
*/
void Phase1(UINT16 *P1K,const UINT8 *TK,const UINT8 *TA,UINT32 IV32)
{
	int  i;

	/* Initialize the 80 bits of P1K[] from IV32 and TA[0..5]     */
	P1K[0]      = Lo16(IV32);
	P1K[1]      = Hi16(IV32);
	P1K[2]      = Mk16(TA[1],TA[0]); /* use TA[] as little-endian */
	P1K[3]      = Mk16(TA[3],TA[2]);
	P1K[4]      = Mk16(TA[5],TA[4]);

	/* Now compute an unbalanced Feistel cipher with 80-bit block */
	/* size on the 80-bit block P1K[], using the 128-bit key TK[] */
	for (i=0; i < PHASE1_LOOP_CNT ;i++)
	{                 /* Each add operation here is mod 2**16 */
		P1K[0] += _S_(P1K[4] ^ TK16((i&1)+0));
		P1K[1] += _S_(P1K[0] ^ TK16((i&1)+2));
		P1K[2] += _S_(P1K[1] ^ TK16((i&1)+4));
		P1K[3] += _S_(P1K[2] ^ TK16((i&1)+6));
		P1K[4] += _S_(P1K[3] ^ TK16((i&1)+0));
		P1K[4] +=  i;                    /* avoid "slide attacks" */
	}
}

//The Block function
inline void block_function(UINT32 *L,UINT32 *R)
{
	UINT32 temp;

	//*R ^= ROL32(*L,17);
	ROL32(temp,*L,17);
	*R ^= temp;


	*L += *R;
	*R ^= XSWAP(*L);
	*L += *R;

	//*R ^= ROL32(*L,3);
	ROL32(temp,*L,3);
	*R ^= temp;

	*L += *R;
	//*R ^= ROR32(*L,2);
	ROR32(temp,*L,2);
	*R ^= temp;

	*L += *R;
}


inline MIC_Fail_State_e GetMICFailFlagVal(void)
{
	MIC_Fail_State_e val;

	//Get semaphore
	os_SemaphoreGet(sysinfo_MIC_FAIL_SEM);
	val = gMICFailFlagVal;
	//Release semaphore
	os_SemaphorePut(sysinfo_MIC_FAIL_SEM);
	return val;
}

inline void SetMICFailFlagVal(BOOLEAN val)
{
	//Get semaphore
	os_SemaphoreGet(sysinfo_MIC_FAIL_SEM);
	gMICFailFlagVal = val;
	//Release semaphore
	os_SemaphorePut(sysinfo_MIC_FAIL_SEM);
}


void MICFail_TimeoutHdlr( vmacApInfo_t *vmacSta_p)
{
	if(GetMICFailFlagVal() == SECOND_MIC_FAIL_IN_60_SEC)
	{
		vmacSta_p->MIC_ErrordisableStaAsso = 0;
	}
	SetMICFailFlagVal(NO_MIC_FAILURE); 
}


void (*MichaelTxFp)(const IEEEtypes_GenHdr_t *pHdr,
					const UINT32 *Key, UINT32 *data, UINT32 *res, UINT32 prio) = MichaelTx;

inline void MichaelTx(const IEEEtypes_GenHdr_t *pHdr,
					  const UINT32 *Key, UINT32 *data, UINT32 *res, UINT32 prio)
{
	UINT32 left, right;//cannot remove
	UINT32 i, filler[4], padded_length;

	MACADDR_CPY(&filler, pHdr->Addr1);
	MACADDR_CPY(((UINT8 *)filler + 6), pHdr->Addr3);
	filler[3] = prio;

	padded_length = pad((UINT8 *)data, pHdr->FrmBodyLen);
	padded_length = padded_length >> 2;
	left = Key[0];
	right = Key[1];

	/*Loop unrolling not recommended cause the function is in itcm
	and bloc_function is inlined */
	for (i=0; i<4; i++)
	{
		left ^= filler[i];
		block_function(&left,&right);
	}

	for (i=0; i<padded_length; i++)
	{
		left ^= data[i];
		block_function(&left,&right);
	}
	res[0] = left;
	res[1] = right;
}

inline void appendMIC(UINT8 *data_ptr,UINT32 *MIC)
{
	UINT32 MIC_tmp;

	MIC_tmp = *MIC;
	*(data_ptr++) = MIC_tmp;  
	*(data_ptr++) = MIC_tmp >> 8;  
	*(data_ptr++) = MIC_tmp >> 16;  
	*(data_ptr++) = MIC_tmp >> 24;
	MIC_tmp = *(MIC+1);
	*(data_ptr++) = MIC_tmp;  
	*(data_ptr++) = MIC_tmp >> 8;  
	*(data_ptr++) = MIC_tmp >> 16;  
	*(data_ptr++) = MIC_tmp >> 24;
}

inline static UINT32 pad(UINT8 *data,UINT32 length)
{

	data[length++] = 0x5a;
	data[length++] = 0x00;
	data[length++] = 0x00;
	data[length++] = 0x00;
	data[length++] = 0x00;
	//over here len would definately be greater than 4
	while (length & 0x03)//do untill the length is a multiple of 4
	{
		data[length++] = 0x00;
	}
	return length;
}


void MrvlMICErrorHdl(vmacApInfo_t *vmacSta_p, COUNTER_MEASURE_EVENT event)
{
	MIB_802DOT11 *mib=vmacSta_p->Mib802dot11;
	MIB_RSNSTATS *mib_RSNStats_p=vmacSta_p->Mib802dot11->RSNStats;
	MIC_Fail_State_e status;
#ifdef MRV_8021X
	static const char *tag = "MLME-MICHAELMICFAILURE.indication";
	char buf[128];
	union iwreq_data wreq;
#endif

	WLDBG_INFO(DBG_LEVEL_10, "MIC Error Isr Handler. \n");

#ifdef MRV_8021X
	if (*(mib->mib_wpaWpa2Mode) < 4) /* For 8021x modes send an event to external authenticator */
#endif
	{
		if (vmacSta_p->MICCounterMeasureEnabled)
		{
			WLDBG_INFO(DBG_LEVEL_10, "COUNTER_MEASURE_EVENT. \n");

			status = GetMICFailFlagVal();

			if (status == FIRST_MIC_FAIL_IN_60_SEC)
			{   
				TimerRearm(&vmacSta_p->MicTimer, 600);

				WLDBG_INFO(DBG_LEVEL_10, "SECOND_MIC_FAIL_IN_60_SEC - send deauthenticate. \n");
				vmacSta_p->MIC_ErrordisableStaAsso = 1;                           

				mib_RSNStats_p->TKIPCounterMeasuresInvoked++;

				//send broadcast Deauthenticate msg
				macMgmtMlme_SendDeauthenticateMsg(vmacSta_p, &bcast, MCBC_STN_ID, 
					IEEEtypes_REASON_MIC_FAILURE);
				extStaDb_RemoveAllStns(vmacSta_p,IEEEtypes_REASON_MIC_FAILURE);
				SetMICFailFlagVal(SECOND_MIC_FAIL_IN_60_SEC);
			}
			else if (status == NO_MIC_FAILURE)
			{   
				//First MIC failure in 60 seconds
				//start timer for 60 seconds
				WLDBG_INFO(DBG_LEVEL_10, "FIRST MIC_FAILURE - start 60 second timer. \n");
				SetMICFailFlagVal(FIRST_MIC_FAIL_IN_60_SEC);

                TimerInit(&vmacSta_p->MicTimer);
				TimerFireIn(&vmacSta_p->MicTimer, 1, &MICFail_TimeoutHdlr, (unsigned char *)vmacSta_p, 600);
			}
			else
				return;
		}
		return;
	}
#ifdef MRV_8021X
	else
	{
		snprintf(buf, sizeof(buf), "%s(keyid=%d %scast addr=%s)", tag, 0, "uni", "00:00:00:00:00:00");
		memset(&wreq, 0, sizeof(wreq));
		wreq.data.length = strlen(buf);
		wireless_send_event(vmacSta_p->dev, IWEVCUSTOM, &wreq, buf);
	}
#endif
}


void MrvlICVErrorHdl(vmacApInfo_t *vmacSta_p)
{
	MIB_RSNSTATS *mib_RSNStats_p=vmacSta_p->Mib802dot11->RSNStats;
	mib_RSNStats_p->TKIPICVErrors++;
	return;
}


