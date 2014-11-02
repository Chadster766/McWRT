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

#ifdef WL_KERNEL_26
//todo for 2.6

#include "wltypes.h"
#include "IEEE_types.h"
#include "wl_macros.h"
#include "wl_mib.h"

#include "mib.h"
#include "osif.h"
#include "qos.h"
#include "ds.h"
#include "keyMgmtCommon.h"
#include "keyMgmt.h"
#include "tkip.h"
#include "StaDb.h"
#include "macmgmtap.h"
#include "macMgmtMlme.h"
#include "wlmac.h"
#include "wl_hal.h"





#define HW_SI_TIMER
UINT32 currentSI; //var which tells what the current SI is
Status_e DelTxOpEntry(UINT32 Tid)
{
	return SUCCESS;
}
Status_e AddTxOpEntry(vmacApInfo_t *vmacSta_p,UINT32 Indx, UINT16 Aid, UINT8 ClientMode)
{
	return SUCCESS;
}
#else


#include "wltypes.h"
#include "IEEE_types.h"
#include "wl_macros.h"
#include "wl_mib.h"

#include "mib.h"
#include "osif.h"
#include "wl_mib.h"
#include "wl_hal.h"
#include "qos.h"
#include "ds.h"
#include "keyMgmtCommon.h"
#include "keyMgmt.h"
#include "tkip.h"
#include "StaDb.h"
#include "macmgmtap.h"
#include "macMgmtMlme.h"
#include "wlmac.h"
#include "wl_hal.h"





#define HW_SI_TIMER
UINT32 currentSI; //var which tells what the current SI is
UINT32 gGCD;
extern TXOP_t TxOpDb[MAX_QOS_STRMS];
extern TSPECEntry_t TSpecEntryDb[MAX_QOS_STRMS];
extern UINT32 SItimerID; //var to hold the timer ID of the Service Interval
extern UINT8 (*GetStationRateFp)(AssocReqData_t* pAssocTableOfStation, UINT8 ClientMode, UINT8 Aid, UINT8 ApMode) ;

UINT32 GetMySI(vmacApInfo_t *vmacSta_p,UINT32 SI)
{
	UINT32 mySI, BcnPeriod;

#ifndef NEW_SCHEDULER
	BcnPeriod = *(vmacSta_p->Mib802dot11->mib_BcnPeriod) * 1000;
	/*
	while (mySI > SI)
	mySI >>= 1;
	*/
	if ((mySI = BcnPeriod/2) <= SI)//50
		;
	else if ((mySI = BcnPeriod/4) <= SI)//25
		;
	else if ((mySI = BcnPeriod/5) <= SI)//20
		;
	else if ((mySI = BcnPeriod/10) <= SI)//10
		;
	else
		mySI = 0;

	mySI = SI;//hardcode
#else
	if ((mySI = (SI/10000)*10000) == 0)
	{//if the SI is less than 10ms then we need to do this way                   
		//mySI = ((float)SI/10000)*10000;
		return mySI;
	}
#endif

	//mySI = ((float)SI/10000)*10;
	return mySI;
}

void CalcSI_Slot(UINT32 TxOpIndx)
{
	TxOpDb[TxOpIndx].mySI_slot = TxOpDb[TxOpIndx].mySI/gGCD;  
}

UINT32 CalculateGCD(UINT32 inp, UINT32 inp2)
{
	UINT32 a,b, tmp;

	if (gGCD > inp)
	{
		a = inp2;
		b = inp;
	}
	else
	{
		b = inp2;
		a = inp;
	}
	while (b != 0)
	{
		tmp = a;
		a = b;
		b = tmp % b;
	}
	return a;
}

UINT32 ReCalculateGCD(void)
{
	UINT32 newGCD,i;

	newGCD = TxOpDb[0].mySI;
	for (i=1; TxOpDb[i].not_free; i++)
	{
		newGCD = CalculateGCD(TxOpDb[i].mySI, newGCD);
	}
	return newGCD;
}

#ifndef NEW_SCHEDULER
void ReCalcAllSI_Slot(void)
{
	UINT32 i;

	for (i=0; TxOpDb[i].not_free; i++)
	{
		TxOpDb[i].mySI_slot = TxOpDb[i].mySI/TxOpDb[0].mySI;
	}
}
#else
void ReCalcAllSI_Slot(void)
{
	UINT32 i;

	for (i=0; TxOpDb[i].not_free; i++)
	{
		CalcSI_Slot(i);
	}
}
#endif

Status_e CalcTxOp(UINT32 TSpecIndx, UINT32 TxOpIndx)
{
	UINT32 nMeanDRPckts, TxOp;
	UINT32 bytesMeanDR; //time to tx nMeanDRPckts at PhyRate
	UINT32 bytesMaxDR;
	UINT64 bytes;  //time to tx one max size MSDU at PhyRate
	UINT32 npckts,i;//# of pckta to be tx in the TxOp
	UINT16 IntegPart,DecPart;
	//float DecVal,floati,DecTxOp;
	UINT32 DecVal,floati,DecTxOp;
	UINT32 TempTxop;

#ifdef NEW_SCHEDULER
	//nMeanDRPckts = (TxOpDb[TxOpIndx].mySI * ((float)TSpecEntryDb[TSpecIndx].TSpec.mean_data_rate / 1000000)) / 8;
	nMeanDRPckts = ((TxOpDb[TxOpIndx].mySI * TSpecEntryDb[TSpecIndx].TSpec.mean_data_rate) / 1000000) / 8;
#else
	//nMeanDRPckts = (currentSI * ((float)TSpecEntryDb[TSpecIndx].TSpec.mean_data_rate / 1000000)) / 8;
	nMeanDRPckts = ((currentSI * TSpecEntryDb[TSpecIndx].TSpec.mean_data_rate) / 1000000) / 8;
#endif

	//nMeanDRPckts += 1; //get the ceiling
	bytesMeanDR = nMeanDRPckts;// / TSpecEntryDb[TSpecIndx].TSpec.min_phy_rate;
	bytesMaxDR = TSpecEntryDb[TSpecIndx].TSpec.max_msdu_size;
	bytes = bytesMeanDR > bytesMaxDR? bytesMeanDR : bytesMaxDR;
	TxOp = (bytes * 8 * 1000000)/ TSpecEntryDb[TSpecIndx].TSpec.min_phy_rate;
	IntegPart = TSpecEntryDb[TSpecIndx].TSpec.srpl_bw_allow >> 13;
	if(!IntegPart)
		IntegPart = 1;
	DecPart =   TSpecEntryDb[TSpecIndx].TSpec.srpl_bw_allow << 3;
	//DecVal = 0.0;
	//floati = 1.0;
	//for (i = 0; i < 13; i++)
	//{   floati /= 2.0;
	//    DecVal = DecVal + (( DecPart & 0x8000) ? floati : 0.0); 
	//    DecPart = DecPart << 1;
	//}
	DecVal = 0;
	floati = 1;
	for (i = 0; i < 13; i++)
	{   //floati /= 2;  // or do a >> 1 equiv to /2.
		floati = floati >> 1;
		DecVal = DecVal + (( DecPart & 0x8000) ? floati : 0); 
		DecPart = DecPart << 1;
	}
	DecTxOp = TxOp * DecVal;

	TxOp *= IntegPart; 
	TxOp += DecTxOp;
	TempTxop = TxOp >> 5;
	if(TempTxop>0xff)
		TxOpDb[TxOpIndx].TXOP=0xff;
	else
		TxOpDb[TxOpIndx].TXOP = TxOp >> 5; // (TxOp/32)in units of 32 musec
	npckts = (TxOp * (TSpecEntryDb[TSpecIndx].TSpec.min_phy_rate/1000000)) / (TSpecEntryDb[TSpecIndx].TSpec.max_msdu_size * 8);
	npckts += 1;
	TxOpDb[TxOpIndx].npckts = npckts; //# of pckts to be tx HARDCODE
	/*Check for Admission control- TBD*/
	return SUCCESS;
}

Status_e ReCalcTxOp(void)
{
	UINT32 TxopIndx, TSpecIndx;//nMeanDRPckts, TxOp, 
	//	UINT32 bytesMeanDR; //time to tx nMeanDRPckts at PhyRate
	//	UINT32 bytesMaxDR, bytes;  //time to tx one max size MSDU at PhyRate
	//	UINT32 npckts;//# of pckta to be tx in the TxOp

	/*
	for (TSpecIndx=0; TSpecEntryDb[TSpecIndx].not_free; TSpecIndx++)
	{
	TxopIndx = 0;
	while ((TxOpDb[TxopIndx].gTID != TSpecIndx) &&
	(TSpecEntryDb[TxopIndx].not_free))
	{
	TxopIndx++;
	}
	*/

	for (TxopIndx=0; TxOpDb[TxopIndx].not_free; TxopIndx++)
	{
		TSpecIndx = TxOpDb[TxopIndx].gTID;
		CalcTxOp(TSpecIndx, TxopIndx);

	}

	/*Check for Admission control- TBD*/
	return SUCCESS;
}



void SI_TimeoutHdlr(UINT32 xx, os_Addrword_t Data)
{
	//send an event to the Txtask
}

Status_e SetSITimer(void)
{
	UINT16 timercontrol;
#ifndef HW_SI_TIMER
	timer_Data_t *timer_p;
	UINT32 no_of_ticks;

	if (SItimerID != 0xFFFFFFFF)
	{//A timer already exists. Disable the existing timer
		timer_Stop(SItimerID);
		timer_Return(SItimerID);
	}
	if ((timer_p = timer_Get(&SI_TimeoutHdlr)) != NULL)
	{
		SItimerID = timer_p->Id;
	}
	else
	{
		return FAIL;
	}
#ifndef NEW_SCHEDULER
	no_of_ticks = currentSI/10000;   /* how may 10 ms */
#else
	no_of_ticks = gGCD/10;   /* how may 10 ms */
#endif
	timer_Start(timer_p->Id, no_of_ticks, no_of_ticks);
#else //HW_SI_TIMER
	WL_READ_REGS16(TIMER_CONTROL, timercontrol);
	//stop the timer
	WL_WRITE_REGS16(TIMER_CONTROL, (timercontrol & (~TIMER1_ACTIVATE)));
	//load the new counter val
#ifndef NEW_SCHEDULER
	WL_WRITE_REGS16(TIMER1_LEN, currentSI/1000);/** Set to the time you want in ms **/
#else
	WL_WRITE_REGS16(TIMER1_LEN, (gGCD/1000)-1);/** Set to the time you want in ms adjust for 1ms delay in timer expiry**/
#endif

	//W81_WRITE_REGS16(TIMER1_LEN, 5000);/** Set to the time you want in ms **/
	//restart the timer. Make it a periodic timer
	WL_WRITE_REGS16(TIMER_CONTROL, (timercontrol | TIMER1_AUTORST | TIMER1_ACTIVATE | TIMER1_LD_VAL));
	//W81_WRITE_REGS16(TIMER_CONTROL, (timercontrol | TIMER1_ACTIVATE | TIMER1_LD_VAL));
#endif //HW_SI_TIMER

	//  timer_Start(timer_p->Id, 10, 10);//HARDCODE
	return SUCCESS;
}

Status_e AddTxOpEntry(vmacApInfo_t *vmacSta_p,UINT32 Indx, UINT16 Aid, UINT8 ClientMode)
{

	TXOP_t TxOpDbtmp[MAX_QOS_STRMS];
	UINT32 i, mySI, j;//, newGCD;

	i = j = 0;

	mySI = GetMySI(vmacSta_p,TSpecEntryDb[Indx].TSpec.max_SI);
	//find where to insert this new element
	while ((TxOpDb[i].mySI <= mySI) && (TxOpDb[i].not_free == TRUE))
		i++;
	//count the # of elements to copy
	while (TxOpDb[j].not_free)
		j++;

	memcpy(TxOpDbtmp, &TxOpDb[i], sizeof(TxOpDb[0]) * j);
	//insert the new element
	TxOpDb[i].mySI = mySI;
	TxOpDb[i].gTID = Indx;
	TxOpDb[i].usr_priority = TSpecEntryDb[Indx].TSpec.ts_info.usr_priority;
	TxOpDb[i].direction = TSpecEntryDb[Indx].TSpec.ts_info.direction;
	TxOpDb[i].not_free = TRUE;
	TxOpDb[i].Aid = Aid;
	TxOpDb[i].ClientMode = ClientMode;
	TSpecEntryDb[Indx].TxOpIndex = i;   /* for access by direct queueing */ 
	//copy back the old elements
	memcpy(&TxOpDb[i+1], TxOpDbtmp, sizeof(TxOpDb[0]) * j);
#ifndef NEW_SCHEDULER
	if (currentSI == TxOpDb[0].mySI)
	{//calculate TxOp only for this stream
		CalcTxOp(Indx, i);
		CalcSI_Slot(i);
	}
	else
	{//SI has changed. Calculate new Txop for all the streams
		currentSI = TxOpDb[0].mySI;
		ReCalcTxOp();
		ReCalcAllSI_Slot();
		SetSITimer();
	}
#else
	newGCD = CalculateGCD(gGCD, TxOpDb[0].mySI);
	CalcTxOp(Indx, i);
	if (newGCD == gGCD)
	{
		CalcSI_Slot(i);
	}
	else
	{
		gGCD = newGCD;
		ReCalcAllSI_Slot();
		SetSITimer();
	}
#endif
	//RefreshMySI_Slot();
	return SUCCESS;
}

Status_e DelTxOpEntry(UINT32 Tid)
{
	UINT32 i, k, active_elems;//, newGCD;
	UINT16 timercontrol;

	active_elems = 0;
	//get the entry to delete
	for (i=0;i<MAX_QOS_STRMS;i++)
	{
		if (TxOpDb[i].gTID == Tid)
		{
			//count the # of elements to be copied
			k = i+1;
			while (TxOpDb[k].not_free)
			{
				TxOpDb[k].gTID--;//Decrement by 1 because all remaining entries in the TSpec Table have been noved up by 1.
				active_elems++;
				k++;
			}
			k--;
			memcpy(&TxOpDb[i], &TxOpDb[i+1], active_elems*sizeof(TxOpDb[0]));
			TxOpDb[k].not_free = FALSE;
			if (TxOpDb[0].not_free == FALSE)
			{//if this is the last active stream then stop the timer
#ifndef HW_SI_TIMER
				timer_Stop(SItimerID);
				timer_Return(SItimerID);
				SItimerID = 0xFFFFFFFF;
#else

				/* Disable Timer interrupt */
				//W81_WRITE_WORD(W81_INTR_CLEAR, INTR_TIMER1);
				//stop the timer
				WL_READ_REGS16(TIMER_CONTROL, timercontrol);
				WL_WRITE_REGS16(TIMER_CONTROL, (timercontrol & (~TIMER1_ACTIVATE)));
#endif
				currentSI = 0;
				gGCD = 0;
				return SUCCESS;
			}
#ifndef NEW_SCHEDULER
			if (currentSI != TxOpDb[0].mySI)
			{//SI has changed. Calculate new Txop for all the streams
				ReCalcAllSI_Slot();
				ReCalcTxOp();
				SetSITimer();
			}
#else
			if ((newGCD = ReCalculateGCD()) != gGCD)
			{//SI has changed. Calculate new Txop for all the streams
				gGCD = newGCD;
				ReCalcAllSI_Slot();
				SetSITimer();
			}
#endif
			currentSI = TxOpDb[0].mySI;
			return SUCCESS;
		}
	}
	return FAIL;
}

inline void InsertQoSCtlField(txInfo_t *pTxInfo, UINT32 TSpecIndx)
{
	pTxInfo->QosControl.tid = TSpecEntryDb[TSpecIndx].TSpec.ts_info.tsid;
	pTxInfo->QosControl.ack_policy = NORMAL_ACK;
	pTxInfo->HdrAddr->Hdr.FrmCtl.Subtype = QoS_DATA;
}

UINT32 gCtr;
void ScheduleHccaPkts(void)
{
}

Boolean IsZeroMinDelayStream(txInfo_t *txInfo_p)
{
		return(SUCCESS);
}
#endif
