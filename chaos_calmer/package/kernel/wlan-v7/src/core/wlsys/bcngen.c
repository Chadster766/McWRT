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


/*
* 
* Purpose: 
*    This file contains the implementations of the beacon update functions. 
* 
*/


#include "wltypes.h"		   
#include "IEEE_types.h"
#include "osif.h"
#include "mib.h"
#include "ds.h"
#include "tkip.h"
#include "StaDb.h"
#include "macmgmtap.h"
#include "qos.h"
#include "wlmac.h"

#include "bcngen.h"

#include "macMgmtMlme.h"

#include "wl_macros.h"
#include "wl_mib.h"
#include "wl_hal.h"
#include "ap8xLnxFwcmd.h"
#include "domain.h"



#define bcngen_TBTT_EVENT   (1 >> 0 )
#define BCNGEN_EVENT_TRIGGERS bcngen_TBTT_EVENT


typedef enum
{
	COPY_FULL,
	COPY_END,
	COPY_BYTE
} CopyMode_e;



macmgmtQ_MgmtMsg_t Bcn; // __attribute__ ((section (".sdbuf")));
macmgmtQ_MgmtMsg_t * BcnBuffer_p = (macmgmtQ_MgmtMsg_t *)&Bcn;
macmgmtQ_MgmtMsg_t Bcn2; // __attribute__ ((section (".sdbuf")));
macmgmtQ_MgmtMsg_t * BcnBuffer_p2 = (macmgmtQ_MgmtMsg_t *)&Bcn2;

extern IEEEtypes_DataRate_t OpRateSet[IEEEtypes_MAX_DATA_RATES_G];
extern IEEEtypes_DataRate_t BasicRateSet[IEEEtypes_MAX_DATA_RATES_G];
extern UINT32 BasicRateSetLen;
extern UINT32 OpRateSetLen;
UINT8 *BcnErpInfoLocation_p,*BcnErpInfoLocation_p2;
UINT8 *PrbrspErpInfoLocation_p,*PrbrspErpInfoLocation_p2;

#ifdef FLEX_TIME
UINT8 *BcnIntervalLocation_p,*BcnIntervalLocation_p2;
UINT8 *ProbeIntervalLocation_p, *ProbeIntervalLocation_p2;
#endif



UINT8 *Bcnchannel;
UINT8 *Bcnchannel2;
extern UINT8 freq54g;

#ifdef IEEE80211H
typedef struct _CHANNELSWITCH_CTRL
{
	BOOLEAN isActivated;    
	UINT8 targetChannel;
} CHANNELSWITCH_CTRL;

static CHANNELSWITCH_CTRL ChannelSwitchCtrl = {FALSE, 0};
IEEEtypes_StartCmd_t StartCmd_update;
SINT8 BcnTxPwr=0xd;
SINT8 ProbeRspTxPwr=0xd;
UINT8 *BcnCSACount;
UINT8 *ProbeCSACount;
UINT8 bcn_reg_domain;
#endif /* IEEE80211H */

IEEEtypes_Tim_t Tim,Tim2;
IEEEtypes_Tim_t *TimPtr,*TimPtr2;
UINT8 TrafficMap[251],TrafficMap2[251];

IEEEtypes_MacAddr_t BcastAddr = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
extern UINT32 CBP_QID;
extern UINT32 BRATE_QID;
extern UINT8 BarkerPreambleSet;
extern UINT8 RfSwitchChanA, RfSwitchChanG;
UINT32 EDCA_Beacon_Counter;
UINT8 *BcnWMEInfoElemLocation_p;
UINT8 *BcnWMEParamElemLocation_p;
UINT8 *PrbWMEParamElemLocation_p;
UINT8 *PrbWMEInfoElemLocation_p;
#ifdef QOS_FEATURE
extern mib_QAPEDCATable_t mib_QAPEDCATable[4];
#endif


#ifdef FLEX_TIME
UINT8 AGinterval=20;
extern UINT32 flexmode_duration;
#else
UINT8 AGinterval=34; /*46*/
#endif



#ifdef COUNTRY_INFO_SUPPORT
#ifndef IEEE80211_DH

typedef  struct _DomainChannelEntry
{
	UINT8 FirstChannelNo;
	UINT8 NoofChannel;
	UINT8 MaxTransmitPw;   
}
PACK_END DomainChannelEntry;

typedef  struct _DomainCountryInfo
{
	UINT8 CountryString[3];
	UINT8 GChannelLen;
	DomainChannelEntry DomainEntryG[1]; /** Assume only 1 G zone **/
	UINT8 AChannelLen;
	DomainChannelEntry DomainEntryA[20]; /** Assume max of 5 A zone **/
}

PACK_END DomainCountryInfo;
#endif //IEEE80211_DH
#endif

#ifdef UR_WPA
IEEEtypes_RSN_IE_t thisStaRsnIEUr;
IEEEtypes_RSN_IE_WPA2_t thisStaRsnIEWPA2Ur;
extern UINT8 mib_urMode;
extern UINT8 mib_wbMode;
#endif

UINT32 txbfcap = 0x1807ff1f;
extern BOOLEAN wlSet11aRFChan(UINT32 chan);
extern void wlQosChangeSlotTimeMode(void);
extern void wlChangeSlotTimeMode(void);


/**
* Fill Beacon Buffer with ERP parameters
* 
* @param pBcnBuf Pointer to start of ERP parameters in beacon buffer
* @return The number of bytes written
*/

#ifdef ERP

inline UINT16 ErpBufUpdate(UINT8 *pBcnBuf, IEEEtypes_StartCmd_t *StartCmd, UINT16 MsgSubType, IEEEtypes_ExtSuppRatesElement_t *ExtSuppRateSet_p)
{
	UINT8 * pNextElement = pBcnBuf;
	IEEEtypes_ExtSuppRatesElement_t *pExtSuppRate;
	IEEEtypes_ERPInfoElement_t *pErpInfo;

	UINT16 byteCnt = 0;
	UINT16 totalLen = 0;

	/* ERP Info*/
	pErpInfo = (IEEEtypes_ERPInfoElement_t *)pNextElement;
	pErpInfo->ElementId = ERP_INFO;
	pErpInfo->Len = 1;
	*(UINT8 *)&pErpInfo->ERPInfo = 0;
	if ( MsgSubType == IEEE_MSG_PROBE_RSP)
	{
		PrbrspErpInfoLocation_p = (UINT8 *) & pErpInfo->ERPInfo;  /* Will be used later probe rsp update when
																  * b only stations associate */
	}
	else
	{
		BcnErpInfoLocation_p = (UINT8 *) & pErpInfo->ERPInfo;
	}
	byteCnt = sizeof(IEEEtypes_InfoElementHdr_t) + pErpInfo->Len;

	totalLen += byteCnt;
	pNextElement += byteCnt;
	pExtSuppRate = (IEEEtypes_ExtSuppRatesElement_t *)pNextElement;
	*pExtSuppRate = *ExtSuppRateSet_p;

	byteCnt = sizeof(IEEEtypes_InfoElementHdr_t) + ExtSuppRateSet_p->Len;

	totalLen += byteCnt;
	return totalLen;
}


/* ErpInfo is updated when the first NonErp station associates and when the last NonErp station
* leaves the BSS */

extern UINT32 BStnAroundCnt;
extern UINT32 BAPCount;
void bcngen_UpdateBeaconErpInfo(vmacApInfo_t      *vmacSta_p, BOOLEAN SetFlag)
{
	static int erpval;
	int val = 0;
	if (SetFlag)
	{
#ifdef DISABLE_B_AP_CHECK
		if(vmacSta_p->bOnlyStnCnt)
#else
		if((vmacSta_p->bOnlyStnCnt) || (BAPCount> 0))
#endif
		{
			if(BarkerPreambleSet)
			{
				val  = 0x7;
			} else
			{
				val = 0x3;
			}
		} else if(BStnAroundCnt && !vmacSta_p->bOnlyStnCnt)
		{
			val = 0x2;
		}
	} else
	{
		val = 0x0;
	}
	if(val != erpval)
	{
		erpval = val;
		wlFwSetGProt(vmacSta_p->dev,val);
	}
}
#endif

#ifdef QOS_FEATURE
/******************************************************************************
* 
* Name: bcngen_UpdateQBSSLoad 
* 
* Description: Will append the QBSS Load Info Element to the Beacon.
*     
* 
* Conditions For Use: 
*    When the QoSOptImpl and the QBSSLoadOptImpl are true. 
* 
* Arguments: 
*    None. 
*                                 
* Return Value: 
*    The offset to the beacon buffer after the new information element is added. 
* 
* Notes: 
*    None. 
* 
* PDL: 
*
* END PDL 
* 
*****************************************************************************/
inline UINT16 bcngen_UpdateQBSSLoad(vmacApInfo_t *vmacSta_p,UINT8 *pBcnBuf)
{
	MIB_STA_CFG  *mib_StaCfg_p=vmacSta_p->Mib802dot11->StationConfig;
	QBSS_load_t *pQBSSLoad = (QBSS_load_t *)pBcnBuf;

	if (!*(vmacSta_p->Mib802dot11->QoSOptImpl) || !mib_StaCfg_p->QBSSLoadOptImpl)
		return 0;
	pQBSSLoad->ElementId =  QBSS_LOAD;
	pQBSSLoad->Len = 5;
	pQBSSLoad->sta_cnt = AssocStationsCnt;
	//Currently assign a default channel utilization of zero.
	pQBSSLoad->channel_util=0;
	pQBSSLoad->avail_admit_cap = GetChannelCapacity();
	return sizeof(QBSS_load_t);
}

/******************************************************************************
* 
* Name: bcngen_UpdateEDCAParamSet 
* 
* Description: Will append the QBSS Load Info Element to the Beacon.
*     
* 
* Conditions For Use: 
*    When the QoSOptImpl and the QBSSLoadOptImpl are true and for 2 DTIM intervals after EDCA has been 
*    changed. 
* 
* Arguments: 
*    None. 
*                                 
* Return Value: 
*    The offset to the beacon buffer after the new information element is added. 
* 
* Notes: 
*    None. 
* 
* PDL: 
*
* END PDL 
* 
*****************************************************************************/
inline UINT16 bcngen_AppendEDCAParamSet(vmacApInfo_t *vmacSta_p,UINT8 *pBcnBuf)
{
	MIB_STA_CFG  *mib_StaCfg_p=vmacSta_p->Mib802dot11->StationConfig;
	EDCA_param_set_t *pEDCA_param_set = (EDCA_param_set_t *)pBcnBuf;
	if (!*(vmacSta_p->Mib802dot11->QoSOptImpl))
		return 0;
	pEDCA_param_set->ElementId =  EDCA_PARAM_SET;
	pEDCA_param_set->Len = 18;
#ifdef WMM_PS_SUPPORT
	pEDCA_param_set->QoS_info.EDCA_param_set_update_cnt = EDCA_param_set_update_cnt;
	pEDCA_param_set->QoS_info.U_APSD = 1;
	pEDCA_param_set->QoS_info.Reserved = 0;
#else
	pEDCA_param_set->QoS_info.EDCA_param_set_update_cnt = EDCA_param_set_update_cnt;
	pEDCA_param_set->QoS_info.Q_ack = mib_StaCfg_p->QAckOptImpl;
	pEDCA_param_set->QoS_info.TXOP_req = PROCESS_TXOP_REQ; //We can process TxOp Request.
	pEDCA_param_set->QoS_info.Q_req = PROCESS_QUEUE_REQ; //We can process non-zero Queue Size.
#endif
	//Update EDCA for BE
	pEDCA_param_set->AC_BE.ACI_AIFSN.AIFSN = mib_QAPEDCATable[0].QAPEDCATblAIFSN;
	pEDCA_param_set->AC_BE.ACI_AIFSN.ACI = 0;
	pEDCA_param_set->AC_BE.ECW_min_max.ECW_min = 
		GetLog(mib_QAPEDCATable[0].QAPEDCATblCWmin);
	pEDCA_param_set->AC_BE.ECW_min_max.ECW_max = 
		GetLog(mib_QAPEDCATable[0].QAPEDCATblCWmax);
	pEDCA_param_set->AC_BE.TXOP_lim = mib_QAPEDCATable[0].QAPEDCATblTXOPLimit;
	//Update EDCA for BK
	pEDCA_param_set->AC_BK.ACI_AIFSN.AIFSN = mib_QAPEDCATable[1].QAPEDCATblAIFSN;
	pEDCA_param_set->AC_BK.ACI_AIFSN.ACI = 1;
	pEDCA_param_set->AC_BK.ECW_min_max.ECW_min = 
		GetLog(mib_QAPEDCATable[1].QAPEDCATblCWmin);
	pEDCA_param_set->AC_BK.ECW_min_max.ECW_max = 
		GetLog(mib_QAPEDCATable[1].QAPEDCATblCWmax);
	pEDCA_param_set->AC_BK.TXOP_lim = mib_QAPEDCATable[1].QAPEDCATblTXOPLimit;
	//Update EDCA for VI
	pEDCA_param_set->AC_VI.ACI_AIFSN.AIFSN = mib_QAPEDCATable[2].QAPEDCATblAIFSN;
	pEDCA_param_set->AC_VI.ACI_AIFSN.ACI = 2;
	pEDCA_param_set->AC_VI.ECW_min_max.ECW_min = 
		GetLog(mib_QAPEDCATable[2].QAPEDCATblCWmin);
	pEDCA_param_set->AC_VI.ECW_min_max.ECW_max = 
		GetLog(mib_QAPEDCATable[2].QAPEDCATblCWmax);
	pEDCA_param_set->AC_VI.TXOP_lim = mib_QAPEDCATable[2].QAPEDCATblTXOPLimit;
	//Update EDCA for VO
	pEDCA_param_set->AC_VO.ACI_AIFSN.AIFSN = mib_QAPEDCATable[3].QAPEDCATblAIFSN;
	pEDCA_param_set->AC_VO.ACI_AIFSN.ACI = 3;
	pEDCA_param_set->AC_VO.ECW_min_max.ECW_min = 
		GetLog(mib_QAPEDCATable[3].QAPEDCATblCWmin);
	pEDCA_param_set->AC_VO.ECW_min_max.ECW_max = 
		GetLog(mib_QAPEDCATable[3].QAPEDCATblCWmax);
	pEDCA_param_set->AC_VO.TXOP_lim = mib_QAPEDCATable[3].QAPEDCATblTXOPLimit;

	return sizeof(EDCA_param_set_t);
}
#endif //QOS_FEATURE

#ifdef WPA

// Init this sta's RSN IE
void InitThisStaRsnIE(vmacApInfo_t *vmacSta_p)
{
	MIB_RSNCONFIG	*mib_RSNConfig_p=vmacSta_p->Mib802dot11->RSNConfig ;
	MIB_RSNCONFIG_UNICAST_CIPHERS	*mib_RSNConfigUnicastCiphers_p=vmacSta_p->Mib802dot11->UnicastCiphers;
	MIB_RSNCONFIG_AUTH_SUITES		*mib_RSNConfigAuthSuites_p=vmacSta_p->Mib802dot11->RSNConfigAuthSuites;
	MIB_RSNCONFIGWPA2			   *mib_RSNConfigWPA2_p=vmacSta_p->Mib802dot11->RSNConfigWPA2;
	MIB_RSNCONFIGWPA2_AUTH_SUITES		*mib_RSNConfigWPA2AuthSuites_p=vmacSta_p->Mib802dot11->WPA2AuthSuites;
	MIB_RSNCONFIGWPA2_UNICAST_CIPHERS	*mib_RSNConfigWPA2UnicastCiphers_p=vmacSta_p->Mib802dot11->WPA2UnicastCiphers;
	MIB_RSNCONFIGWPA2_UNICAST_CIPHERS	*mib_RSNConfigWPA2UnicastCiphers2_p=vmacSta_p->Mib802dot11->WPA2UnicastCiphers2;
	IEEEtypes_RSN_IE_t *thisStaRsnIE_p = vmacSta_p->Mib802dot11->thisStaRsnIE;
	IEEEtypes_RSN_IE_WPA2_t *thisStaRsnIEWPA2_p = vmacSta_p->Mib802dot11->thisStaRsnIEWPA2;
	IEEEtypes_RSN_IE_WPA2MixedMode_t *thisStaRsnIEWPA2MixedMode_p = vmacSta_p->Mib802dot11->thisStaRsnIEWPA2MixedMode;
	thisStaRsnIE_p->ElemId = 221;
	thisStaRsnIE_p->Len = 22;
	thisStaRsnIE_p->OuiType[0] = 0x0;
	thisStaRsnIE_p->OuiType[1] = 0x50;
	thisStaRsnIE_p->OuiType[2] = 0xf2;
	thisStaRsnIE_p->OuiType[3] = 0x01;
	thisStaRsnIE_p->Ver[0] = 0x01;
	thisStaRsnIE_p->Ver[1] = 0x0;
	memcpy(thisStaRsnIE_p->GrpKeyCipher, mib_RSNConfig_p->MulticastCipher, 4);
	thisStaRsnIE_p->PwsKeyCnt[0] = 0x01;
	thisStaRsnIE_p->PwsKeyCnt[1] = 0x0;
	memcpy(thisStaRsnIE_p->PwsKeyCipherList, mib_RSNConfigUnicastCiphers_p->UnicastCipher, 4);
	thisStaRsnIE_p->AuthKeyCnt[0] = 0x01;
	thisStaRsnIE_p->AuthKeyCnt[1] = 0x0;
	memcpy(thisStaRsnIE_p->AuthKeyList, mib_RSNConfigAuthSuites_p->AuthSuites, 4);
	//thisStaRsnIE_p->RsnCap[0] = 0x0;
	//thisStaRsnIE_p->RsnCap[1] = 0x0;
#ifdef AP_WPA2
	/* WPA2 Only */
	thisStaRsnIEWPA2_p->ElemId = 48;
	thisStaRsnIEWPA2_p->Len = 20;
	//thisStaRsnIEWPA2_p->OuiType[0] = 0x0;
	//thisStaRsnIEWPA2_p->OuiType[1] = 0x50;
	//thisStaRsnIEWPA2_p->OuiType[2] = 0xf2;
	//thisStaRsnIEWPA2_p->OuiType[3] = 0x01;
	thisStaRsnIEWPA2_p->Ver[0] = 0x01;
	thisStaRsnIEWPA2_p->Ver[1] = 0x0;
	memcpy(thisStaRsnIEWPA2_p->GrpKeyCipher, mib_RSNConfigWPA2_p->MulticastCipher, 4);
	thisStaRsnIEWPA2_p->PwsKeyCnt[0] = 0x01;
	thisStaRsnIEWPA2_p->PwsKeyCnt[1] = 0x0;
	memcpy(thisStaRsnIEWPA2_p->PwsKeyCipherList, mib_RSNConfigWPA2UnicastCiphers_p->UnicastCipher, 4);
	thisStaRsnIEWPA2_p->AuthKeyCnt[0] = 0x01;
	thisStaRsnIEWPA2_p->AuthKeyCnt[1] = 0x0;
	memcpy(thisStaRsnIEWPA2_p->AuthKeyList, mib_RSNConfigWPA2AuthSuites_p->AuthSuites, 4);
	if (mib_RSNConfigWPA2_p->WPA2PreAuthEnabled) 
		thisStaRsnIEWPA2_p->RsnCap[0] = 0x01;
	else
		thisStaRsnIEWPA2_p->RsnCap[0] = 0x0;
	thisStaRsnIEWPA2_p->RsnCap[1] = 0x0;

	/* WPA2 Mixed Mode */
	thisStaRsnIEWPA2MixedMode_p->ElemId = 48;
	thisStaRsnIEWPA2MixedMode_p->Len = 24; //38;
	//thisStaRsnIEWPA2MixedMode_p->OuiType[0] = 0x0;
	//thisStaRsnIEWPA2MixedMode_p->OuiType[1] = 0x50;
	//thisStaRsnIEWPA2MixedMode_p->OuiType[2] = 0xf2;
	//thisStaRsnIEWPA2MixedMode_p->OuiType[3] = 0x01;
	thisStaRsnIEWPA2MixedMode_p->Ver[0] = 0x01;
	thisStaRsnIEWPA2MixedMode_p->Ver[1] = 0x0;
	memcpy(thisStaRsnIEWPA2MixedMode_p->GrpKeyCipher, mib_RSNConfigWPA2_p->MulticastCipher, 4);
	thisStaRsnIEWPA2MixedMode_p->PwsKeyCnt[0] = 0x02;
	thisStaRsnIEWPA2MixedMode_p->PwsKeyCnt[1] = 0x0;
	memcpy(thisStaRsnIEWPA2MixedMode_p->PwsKeyCipherList, mib_RSNConfigWPA2UnicastCiphers_p->UnicastCipher, 4);

	if (mib_RSNConfigWPA2UnicastCiphers_p->UnicastCipher[3] != 
		mib_RSNConfigWPA2UnicastCiphers2_p->UnicastCipher[3])
	{
		memcpy(thisStaRsnIEWPA2MixedMode_p->PwsKeyCipherList2, mib_RSNConfigWPA2UnicastCiphers2_p->UnicastCipher, 4);
		thisStaRsnIEWPA2MixedMode_p->AuthKeyCnt[0] = 0x01;
		thisStaRsnIEWPA2MixedMode_p->AuthKeyCnt[1] = 0x0;
		memcpy(thisStaRsnIEWPA2MixedMode_p->AuthKeyList, mib_RSNConfigWPA2AuthSuites_p->AuthSuites, 4);
		if (mib_RSNConfigWPA2_p->WPA2PreAuthEnabled) 
			thisStaRsnIEWPA2MixedMode_p->RsnCap[0] = 0x01;
		else
			thisStaRsnIEWPA2MixedMode_p->RsnCap[0] = 0x0;
		thisStaRsnIEWPA2MixedMode_p->RsnCap[1] = 0x0;
	}
	else
	{
		thisStaRsnIEWPA2MixedMode_p->Len = 20; //38;
		thisStaRsnIEWPA2MixedMode_p->PwsKeyCnt[0] = 0x01;
		*(thisStaRsnIEWPA2MixedMode_p->PwsKeyCipherList2) = 0x01;   //AuthKeyCnt[0]
		*(thisStaRsnIEWPA2MixedMode_p->PwsKeyCipherList2+1) = 0x0;   //AuthKeyCnt[1]
		memcpy(thisStaRsnIEWPA2MixedMode_p->PwsKeyCipherList2+2, 
			mib_RSNConfigWPA2AuthSuites_p->AuthSuites, 4);
		if (mib_RSNConfigWPA2_p->WPA2PreAuthEnabled) 
			*(thisStaRsnIEWPA2MixedMode_p->PwsKeyCipherList2+6) = 0x01;   //RsnCap[0]
		else
			*(thisStaRsnIEWPA2MixedMode_p->PwsKeyCipherList2+6) = 0x0;   //RsnCap[0]
		*(thisStaRsnIEWPA2MixedMode_p->PwsKeyCipherList2+7) = 0x0;   //RsnCap[1]
	}
#endif

}

#ifdef UR_WPA
void InitThisStaRsnIEUr(vmacApInfo_t *vmacSta_p)
{
	MIB_RSNCONFIG	*mib_RSNConfig_p=vmacSta_p->Mib802dot11->RSNConfig ;
	MIB_RSNCONFIG_UNICAST_CIPHERS	*mib_RSNConfigUnicastCiphers_p=vmacSta_p->Mib802dot11->UnicastCiphers;
	MIB_RSNCONFIG_AUTH_SUITES		*mib_RSNConfigAuthSuites_p=vmacSta_p->Mib802dot11->RSNConfigAuthSuites;
	MIB_RSNCONFIGWPA2			   *mib_RSNConfigWPA2_p=vmacSta_p->Mib802dot11->RSNConfigWPA2;
	MIB_RSNCONFIGWPA2_AUTH_SUITES		*mib_RSNConfigWPA2AuthSuites_p=vmacSta_p->Mib802dot11->WPA2AuthSuites;
	MIB_RSNCONFIGWPA2_UNICAST_CIPHERS	*mib_RSNConfigWPA2UnicastCiphers_p=vmacSta_p->Mib802dot11->WPA2UnicastCiphers;
	thisStaRsnIEUr.ElemId = 221;
	thisStaRsnIEUr.Len = sizeof(IEEEtypes_RSN_IE_t) - 2;
	thisStaRsnIEUr.OuiType[0] = 0x0;
	thisStaRsnIEUr.OuiType[1] = 0x50;
	thisStaRsnIEUr.OuiType[2] = 0xf2;
	thisStaRsnIEUr.OuiType[3] = 0x01;
	thisStaRsnIEUr.Ver[0] = 0x01;
	thisStaRsnIEUr.Ver[1] = 0x0;
	memcpy(thisStaRsnIEUr.GrpKeyCipher, mib_RSNConfig_p->MulticastCipher, 4);
	thisStaRsnIEUr.PwsKeyCnt[0] = 0x01;
	thisStaRsnIEUr.PwsKeyCnt[1] = 0x0;
	memcpy(thisStaRsnIEUr.PwsKeyCipherList, mib_RSNConfigUnicastCiphers_p->UnicastCipher, 4);
	thisStaRsnIEUr.AuthKeyCnt[0] = 0x01;
	thisStaRsnIEUr.AuthKeyCnt[1] = 0x0;
	memcpy(thisStaRsnIEUr.AuthKeyList, mib_RSNConfigAuthSuites_p->AuthSuites, 4);
	//thisStaRsnIEUr.RsnCap[0] = 0x0;
	//thisStaRsnIEUr.RsnCap[1] = 0x0;

	/* WPA2 */
	thisStaRsnIEWPA2Ur.ElemId = 48;
	thisStaRsnIEWPA2Ur.Len = 20;
	thisStaRsnIEWPA2Ur.Ver[0] = 0x01;
	thisStaRsnIEWPA2Ur.Ver[1] = 0x0;
	memcpy(thisStaRsnIEWPA2Ur.GrpKeyCipher, mib_RSNConfigWPA2_p->MulticastCipher, 4);
	thisStaRsnIEWPA2Ur.PwsKeyCnt[0] = 0x01;
	thisStaRsnIEWPA2Ur.PwsKeyCnt[1] = 0x0;
	memcpy(thisStaRsnIEWPA2Ur.PwsKeyCipherList, mib_RSNConfigWPA2UnicastCiphers_p->UnicastCipher, 4);
	thisStaRsnIEWPA2Ur.AuthKeyCnt[0] = 0x01;
	thisStaRsnIEWPA2Ur.AuthKeyCnt[1] = 0x0;
	memcpy(thisStaRsnIEWPA2Ur.AuthKeyList, mib_RSNConfigWPA2AuthSuites_p->AuthSuites, 4);
	thisStaRsnIEWPA2Ur.RsnCap[0] = 0x0;
	thisStaRsnIEWPA2Ur.RsnCap[1] = 0x0;
}
#endif

// Add RSN IE to a frame body
UINT16 AddRSN_IE(vmacApInfo_t *vmacSta_p, IEEEtypes_RSN_IE_t* pNextElement)
{
	IEEEtypes_RSN_IE_t *thisStaRsnIE_p = vmacSta_p->Mib802dot11->thisStaRsnIE;
#ifdef UR_WPA
	if((mib_urMode == 0) && (mib_wbMode == 0)) 	//Ap
		memcpy( pNextElement, &thisStaRsnIE, sizeof(IEEEtypes_RSN_IE_t) );
	else //WB or UR
		memcpy( pNextElement, &thisStaRsnIEUr, sizeof(IEEEtypes_RSN_IE_t) );
#else
	memcpy( pNextElement, thisStaRsnIE_p, sizeof(IEEEtypes_RSN_IE_t) );
#endif
	return ( sizeof(IEEEtypes_RSN_IE_t) );
}
UINT16 AddRSN_IE_TO(IEEEtypes_RSN_IE_t *thisStaRsnIE_p, IEEEtypes_RSN_IE_t* pNextElement)
{
	memcpy( pNextElement, thisStaRsnIE_p, sizeof(IEEEtypes_RSN_IE_t) );
	return ( sizeof(IEEEtypes_RSN_IE_t) );
}
#endif 

#ifdef AP_WPA2
UINT16 AddRSN_IEWPA2(vmacApInfo_t *vmacSta_p,IEEEtypes_RSN_IE_WPA2_t* pNextElement)
{
	IEEEtypes_RSN_IE_WPA2_t *thisStaRsnIEWPA2_p = vmacSta_p->Mib802dot11->thisStaRsnIEWPA2;
#ifdef UR_WPA
	memcpy( pNextElement, &thisStaRsnIEWPA2Ur, thisStaRsnIEWPA2Ur.Len + 2 );
	//return ( sizeof(IEEEtypes_RSN_IE_WPA2_t) );
	return (thisStaRsnIEWPA2Ur.Len + 2);
#else
	memcpy( pNextElement, thisStaRsnIEWPA2_p, thisStaRsnIEWPA2_p->Len + 2 );
	//return ( sizeof(IEEEtypes_RSN_IE_WPA2_t) );
	return (thisStaRsnIEWPA2_p->Len + 2);
#endif
}
UINT16 AddRSN_IEWPA2_TO(IEEEtypes_RSN_IE_WPA2_t *thisStaRsnIEWPA2_p,IEEEtypes_RSN_IE_WPA2_t* pNextElement)
{
	memcpy( pNextElement, thisStaRsnIEWPA2_p, thisStaRsnIEWPA2_p->Len + 2 );
	return (thisStaRsnIEWPA2_p->Len + 2);
}

UINT16 AddRSN_IEWPA2MixedMode(vmacApInfo_t *vmacSta_p,IEEEtypes_RSN_IE_WPA2MixedMode_t* pNextElement)
{
	IEEEtypes_RSN_IE_WPA2MixedMode_t *thisStaRsnIEWPA2MixedMode_p = vmacSta_p->Mib802dot11->thisStaRsnIEWPA2MixedMode;
	memcpy( pNextElement, thisStaRsnIEWPA2MixedMode_p, thisStaRsnIEWPA2MixedMode_p->Len + 2 );
	//return ( sizeof(IEEEtypes_RSN_IE_WPA2_t) );
	return (thisStaRsnIEWPA2MixedMode_p->Len + 2);
}
#endif

#ifdef COUNTRY_INFO_SUPPORT


IEEEtypes_COUNTRY_IE_t thisStaCountryIE;

#ifdef COUNTRY_INFO_SUPPORT
void InitThisCountry_IE(vmacApInfo_t *vmacSta_p)
{
	MIB_802DOT11 *mib=vmacSta_p->Mib802dot11;

	DomainCountryInfo DomainInfo[1];


	domainGetPowerInfo((UINT8 *)DomainInfo);

	thisStaCountryIE.ElemId = COUNTRY;
	thisStaCountryIE.CountryCode[0]=DomainInfo->CountryString[0];
	thisStaCountryIE.CountryCode[1]= DomainInfo->CountryString[1];
	thisStaCountryIE.CountryCode[2]=DomainInfo->CountryString[2];

	if(*(mib->mib_ApMode)==AP_MODE_A_ONLY || *(mib->mib_ApMode)==AP_MODE_AandG)
	{
		thisStaCountryIE.Len=DomainInfo->AChannelLen+3;  /** include 3 byte of country code here **/
		memcpy(thisStaCountryIE.DomainEntry,DomainInfo->DomainEntryA,DomainInfo->AChannelLen);
	}
	else
	{
		thisStaCountryIE.Len=DomainInfo->GChannelLen+3;  /** include 3 byte of country code here **/
		memcpy(thisStaCountryIE.DomainEntry,DomainInfo->DomainEntryG,DomainInfo->GChannelLen);
	}

}
/** DUPLICATE OF ABOVE, this is just use for ag mode , g channel **/
void InitThisCountry_IE2(UINT8 mib_ApMode)  
{

	DomainCountryInfo DomainInfo[1];


	domainGetPowerInfo((UINT8 *)DomainInfo);

	thisStaCountryIE.ElemId = COUNTRY;
	thisStaCountryIE.CountryCode[0]=DomainInfo->CountryString[0];
	thisStaCountryIE.CountryCode[1]= DomainInfo->CountryString[1];
	thisStaCountryIE.CountryCode[2]=DomainInfo->CountryString[2];

	thisStaCountryIE.Len=DomainInfo->GChannelLen+3;  /** include 3 byte of country code here **/
	memcpy(thisStaCountryIE.DomainEntry,DomainInfo->DomainEntryG,DomainInfo->GChannelLen);


}
#endif
UINT16 AddCountry_IE(IEEEtypes_COUNTRY_IE_t* pNextElement)
{
	memcpy( pNextElement, &thisStaCountryIE, thisStaCountryIE.Len+2 ); /** 2 for size of length and elementId **/
	return (  thisStaCountryIE.Len+2);
}


#endif

#ifdef IEEE80211H


static IEEEtypes_PowerConstraintElement_t PowerConstraintIE;
IEEEtypes_ChannelSwitchAnnouncementElement_t ChannelSwitchAnnouncementIE;
IEEEtypes_QuietElement_t QuietIE;
#ifdef IEEE80211H_NOTWIFI
static IEEEtypes_TPCRepElement_t TPCRepIE;
#endif

static void InitPowerConstraint_IE(vmacApInfo_t *vmacSta_p,UINT8 channel)
{
#ifndef IEEE80211_DH
	PowerConstraintIE.value = (SINT8)mib_SpectrumMagament_p->mitigationRequirement;

	PowerConstraintIE.ElementId = PWR_CONSTRAINT;	
	PowerConstraintIE.Len = 1; 
#endif //IEEE80211_DH

	return;
}

static UINT16 AddPowerConstraint_IE(UINT8* pNextElement)
{
	memcpy( pNextElement, &PowerConstraintIE, PowerConstraintIE.Len+2 ); /** 2 for size of length and elementId **/
	return ( PowerConstraintIE.Len+2);
}



void bcngen_AddChannelSwithcAnnouncement_IE(vmacApInfo_t *vmacSta_p,IEEEtypes_ChannelSwitchAnnouncementElement_t *pChannelSwitchAnnouncementIE)
{
	ChannelSwitchAnnouncementIE.ElementId = CSA;
	ChannelSwitchAnnouncementIE.Mode = pChannelSwitchAnnouncementIE->Mode;
	ChannelSwitchAnnouncementIE.Channel = pChannelSwitchAnnouncementIE->Channel;
	ChannelSwitchAnnouncementIE.Count = pChannelSwitchAnnouncementIE->Count;
	ChannelSwitchAnnouncementIE.Len = 3;

	/* update beacon immediately */
	bcngen_UpdateBeaconBuffer(vmacSta_p, &StartCmd_update);

	ChannelSwitchCtrl.isActivated = TRUE;
	ChannelSwitchCtrl.targetChannel = pChannelSwitchAnnouncementIE->Channel;
	StartCmd_update.PhyParamSet.DsParamSet.CurrentChan=pChannelSwitchAnnouncementIE->Channel;  /** update channel number here **/

	return;
}


void bcngen_RemoveChannelSwithcAnnouncement_IE(vmacApInfo_t *vmacSta_p)
{
	ChannelSwitchCtrl.isActivated = FALSE;
	ChannelSwitchCtrl.targetChannel = 0;

	ChannelSwitchAnnouncementIE.Len = 0;    
	memset( &ChannelSwitchAnnouncementIE, 0,sizeof(ChannelSwitchAnnouncementIE));        

	/* update beacon immediately */
	bcngen_UpdateBeaconBuffer(vmacSta_p,&StartCmd_update);

	return;
}


static UINT16 AddChannelSwithcAnnouncement_IE(UINT8* pNextElement)
{   
	if (ChannelSwitchAnnouncementIE.Len)
	{
		memcpy( pNextElement, &ChannelSwitchAnnouncementIE, ChannelSwitchAnnouncementIE.Len+2 ); /** 2 for size of length and elementId **/
		return ( ChannelSwitchAnnouncementIE.Len+2);
	}
	else
		return (0);
}


void bcngen_AddQuiet_IE(vmacApInfo_t *vmacSta_p,IEEEtypes_QuietElement_t *pQuietIE)
{
	QuietIE.ElementId = QUIET;
	QuietIE.Count = pQuietIE->Count;
	QuietIE.Period = pQuietIE->Period;
	QuietIE.Duration = pQuietIE->Duration;
	QuietIE.Offset = pQuietIE->Offset;
	QuietIE.Len = 6;   

	/* update beacon immediately */
	bcngen_UpdateBeaconBuffer(vmacSta_p,&StartCmd_update);

	return;
}

void bcngen_RemoveQuiet_IE(vmacApInfo_t *vmacSta_p)
{
	QuietIE.Len = 0;    
	memset( &QuietIE, 0, sizeof(QuietIE)); 

	/* update beacon immediately */
	bcngen_UpdateBeaconBuffer(vmacSta_p,&StartCmd_update);

	return;
}

static UINT16 AddQuiet_IE(UINT8* pNextElement)
{   
	if (QuietIE.Len)
	{
		memcpy( pNextElement, &QuietIE, QuietIE.Len+2 ); /** 2 for size of length and elementId **/
		return ( QuietIE.Len+2);
	}
	else
		return (0);
}
#ifdef IEEE80211H_NOTWIFI
static void InitTPCRep_IE(void)
{

	TPCRepIE.ElementId = TPC_REP;	
	TPCRepIE.Len = 2; 
	TPCRepIE.TxPwr= BcnTxPwr;        
	TPCRepIE.LinkMargin = 0;
	return;
}

static UINT16 AddTPCRep_IE(UINT8* pNextElement)
{    
	memcpy( pNextElement, &TPCRepIE, TPCRepIE.Len+2 ); /** 2 for size of length and elementId **/
	return ( TPCRepIE.Len+2);
}
#endif
#ifdef FLEX_TIME
void UpdateBeaconInterval(UINT8 mode)
{
	if(mode==0)  //scanning mode
	{
		*ProbeIntervalLocation_p=*BcnIntervalLocation_p=20;
		*ProbeIntervalLocation_p2=*BcnIntervalLocation_p2=20;


	}
	else
	{
		*ProbeIntervalLocation_p=*BcnIntervalLocation_p=40;
		*ProbeIntervalLocation_p2=*BcnIntervalLocation_p2=40;

	}

}
#endif


#endif /* IEEE80211H */
/******************************************************************************
* 
* Name: bcngen_UpdateBeaconBuffer 
* 
* Description: 
*     
* 
* Conditions For Use: 
*    None. 
* 
* Arguments: 
*    None. 
* 
* Return Value: 
*    Status indicating success or failure. 
* 
* Notes: 
*    None. 
* 
* PDL: 
*
* END PDL 
* 
*****************************************************************************/
void bcngen_UpdateBeaconBuffer(vmacApInfo_t *vmacSta_p,IEEEtypes_StartCmd_t *StartCmd)
{
	MIB_802DOT11 *mib=vmacSta_p->Mib802dot11;
	MIB_STA_CFG  *mib_StaCfg_p=vmacSta_p->Mib802dot11->StationConfig;
	MIB_PRIVACY_TABLE *mib_PrivacyTable_p=vmacSta_p->Mib802dot11->Privacy;
	MIB_RSNCONFIGWPA2			   *mib_RSNConfigWPA2_p=vmacSta_p->Mib802dot11->RSNConfigWPA2;
	UINT8 * NextElementPtr;
	IEEEtypes_SsIdElement_t *SsIdPtr;
	IEEEtypes_SuppRatesElement_t *SuppRatesPtr;
	IEEEtypes_PhyParamSet_t *PhyParamSetPtr;
	IEEEtypes_SsParamSet_t *SsParamSetPtr;
	UINT32 RateCnt;
	UINT16 totalLen = 0;
	UINT16 byteCnt;

	IEEEtypes_SuppRatesElement_t *SuppRateSet_p=&(vmacSta_p->SuppRateSet);			
#ifdef ERP
	IEEEtypes_ExtSuppRatesElement_t *ExtSuppRateSet_p=&(vmacSta_p->ExtSuppRateSet);
#endif


#ifdef IEEE80211H
	/* snapshot the startcmd */
	StartCmd_update = *StartCmd;
#endif

	BcnBuffer_p->Hdr.Duration = 0;
	BcnBuffer_p->Hdr.FrmCtl.ProtocolVersion = 0;
	BcnBuffer_p->Hdr.FrmCtl.Type = IEEE_TYPE_MANAGEMENT;
	BcnBuffer_p->Hdr.FrmCtl.Subtype = IEEE_MSG_BEACON;
	BcnBuffer_p->Hdr.FrmCtl.ToDs = 0;
	BcnBuffer_p->Hdr.FrmCtl.FromDs = 0;
	BcnBuffer_p->Hdr.FrmCtl.MoreFrag = 0;
	BcnBuffer_p->Hdr.FrmCtl.Retry = 0;
	BcnBuffer_p->Hdr.FrmCtl.PwrMgmt = 0;
	BcnBuffer_p->Hdr.FrmCtl.MoreData = 0;
	BcnBuffer_p->Hdr.FrmCtl.Wep = 0;
	BcnBuffer_p->Hdr.FrmCtl.Order = 0;
	memcpy(&BcnBuffer_p->Hdr.DestAddr, &BcastAddr, sizeof(IEEEtypes_MacAddr_t));
	memcpy(&BcnBuffer_p->Hdr.SrcAddr, &vmacSta_p->macBssId, sizeof(IEEEtypes_MacAddr_t));
	memcpy(&BcnBuffer_p->Hdr.BssId, &vmacSta_p->macBssId, sizeof(IEEEtypes_MacAddr_t));


	if(*(mib->mib_ApMode)==AP_MODE_AandG)
	{
#ifdef FLEX_TIME
		BcnIntervalLocation_p=(UINT8 *)&BcnBuffer_p->Body.Bcn.BcnInterval;
#endif

		BcnBuffer_p->Body.Bcn.BcnInterval = AGinterval;
	}
	else
	{
		BcnBuffer_p->Body.Bcn.BcnInterval = StartCmd->BcnPeriod;
	}

	BcnBuffer_p->Body.Bcn.CapInfo = StartCmd->CapInfo;

	NextElementPtr = (UINT8 *) & BcnBuffer_p->Body.Bcn.SsId;
	SsIdPtr = &BcnBuffer_p->Body.Bcn.SsId;

	if (*(mib->mib_broadcastssid) == TRUE)
	{
		*SsIdPtr = vmacSta_p->macSsId;
	}
	else
	{ /** FALSE CASE: NEED TO BLANK OFF SSID **/

		*SsIdPtr = vmacSta_p->macSsId;
		memset(SsIdPtr->SsId, 0, SsIdPtr->Len);
	}


	NextElementPtr = NextElementPtr + 2 + SsIdPtr->Len;
	SuppRatesPtr = (IEEEtypes_SuppRatesElement_t *)NextElementPtr;
	SuppRatesPtr->ElementId = SUPPORTED_RATES;
	RateCnt = 0;

	*SuppRatesPtr = *SuppRateSet_p;
	NextElementPtr = NextElementPtr + 2 + SuppRateSet_p->Len;		
	
	PhyParamSetPtr = (IEEEtypes_PhyParamSet_t *)NextElementPtr;
	if (*(UINT8 *)&StartCmd->PhyParamSet == FH_PARAM_SET )
	{
		PhyParamSetPtr->FhParamSet =
			StartCmd->PhyParamSet.FhParamSet;
		NextElementPtr = NextElementPtr + sizeof(IEEEtypes_FhParamSet_t);
	}

	else if (*(UINT8 *)&StartCmd->PhyParamSet == DS_PARAM_SET)
	{
		PhyParamSetPtr->DsParamSet =
			StartCmd->PhyParamSet.DsParamSet;
		Bcnchannel=(UINT8 *)&PhyParamSetPtr->DsParamSet.CurrentChan;
		NextElementPtr = NextElementPtr + sizeof(IEEEtypes_DsParamSet_t);
	}

#ifdef AP_WPA2
	if (mib_PrivacyTable_p->RSNEnabled)
	{
		// Set capability info.privacy bit for Beacon and probe rsp.
		BcnBuffer_p->Body.Bcn.CapInfo.Privacy = 1;

		InitThisStaRsnIE(vmacSta_p);

		if (!mib_RSNConfigWPA2_p->WPA2OnlyEnabled)
		{

			// add IE to beacon.
			NextElementPtr += AddRSN_IE( vmacSta_p, (IEEEtypes_RSN_IE_t*)NextElementPtr );
		}
	}
	else
	{
		// ReSet capability info.privacy bit for Beacon and probe rsp.
		if ( (mib_StaCfg_p->PrivOption && mib_PrivacyTable_p->PrivInvoked ) )
			BcnBuffer_p->Body.Bcn.CapInfo.Privacy = 1;
		else
			BcnBuffer_p->Body.Bcn.CapInfo.Privacy = 0;
	}

#else
	if (mib_PrivacyTable_p->RSNEnabled)
	{
		// Set capability info.privacy bit for Beacon and probe rsp.
		BcnBuffer_p->Body.Bcn.CapInfo.Privacy = 1;

		InitThisStaRsnIE(vmacSta_p);

		// add IE to beacon.
		NextElementPtr += AddRSN_IE( vmacSta_p, (IEEEtypes_RSN_IE_t*)NextElementPtr );
	}
	else
	{
		// ReSet capability info.privacy bit for Beacon and probe rsp.
		if ( (mib_StaCfg_p->PrivOption && mib_PrivacyTable_p->PrivInvoked ) )
			BcnBuffer_p->Body.Bcn.CapInfo.Privacy = 1;
		else
			BcnBuffer_p->Body.Bcn.CapInfo.Privacy = 0;
	}
#endif


	SsParamSetPtr = (IEEEtypes_SsParamSet_t *)NextElementPtr;
	if (StartCmd->BssType == BSS_INFRASTRUCTURE )
	{
		SsParamSetPtr->CfParamSet =
			StartCmd->SsParamSet.CfParamSet;
		NextElementPtr = NextElementPtr + sizeof(IEEEtypes_CfParamSet_t);
	}
	else if (StartCmd->BssType == BSS_INDEPENDENT)
	{
		SsParamSetPtr->IbssParamSet =
			StartCmd->SsParamSet.IbssParamSet;
		NextElementPtr = NextElementPtr + sizeof(IEEEtypes_IbssParamSet_t);
	}

	TimPtr = (IEEEtypes_Tim_t *)NextElementPtr;
	TimPtr->ElementId = TIM;

	if(*(mib->mib_ApMode)==AP_MODE_AandG)   /** Dtim period can only be 0 for A and G mode **/
		TimPtr->DtimPeriod = 1;
	else
		TimPtr->DtimPeriod = StartCmd->DtimPeriod;

	TimPtr->BitmapCtl = 0;
	TimPtr->PartialVirtualBitmap[0] = 0;
	TimPtr->Len = 4;
	// Also, update the local TIM buffer
	Tim.ElementId = TIM;

	if(*(mib->mib_ApMode)==AP_MODE_AandG)   /** Dtim period can only be 0 for A and G mode **/
		Tim.DtimPeriod = 1 ;
	else
		Tim.DtimPeriod =StartCmd->DtimPeriod;

	Tim.BitmapCtl = 0;
	Tim.PartialVirtualBitmap[0] = 0;
	Tim.Len = 4;
	byteCnt = sizeof(IEEEtypes_InfoElementHdr_t) + TimPtr->Len;
	NextElementPtr += byteCnt;

#ifdef COUNTRY_INFO_SUPPORT
	InitThisCountry_IE(vmacSta_p);
	NextElementPtr +=AddCountry_IE((IEEEtypes_COUNTRY_IE_t *) NextElementPtr);

#endif

#ifdef IEEE80211H
	if(mib_StaCfg_p->SpectrumManagementRequired && (*(mib->mib_ApMode) == AP_MODE_A_ONLY||*(mib->mib_ApMode) == AP_MODE_AandG))
	{
		InitPowerConstraint_IE(vmacSta_p,*Bcnchannel);
		NextElementPtr += AddPowerConstraint_IE(NextElementPtr);    
		/* the following 2 IEs MAY BE present */

#ifdef IEEE80211H_NOTWIFI
		InitTPCRep_IE();
		NextElementPtr += AddTPCRep_IE(NextElementPtr);    
#endif
		BcnCSACount = &((IEEEtypes_ChannelSwitchAnnouncementElement_t *)NextElementPtr)->Count;
		NextElementPtr += AddChannelSwithcAnnouncement_IE(NextElementPtr);            
		NextElementPtr += AddQuiet_IE(NextElementPtr);    


	}
#endif /* IEEE80211H */


	totalLen = (UINT32)NextElementPtr - (UINT32)BcnBuffer_p -
		sizeof(IEEEtypes_MgmtHdr_t);


#ifdef ERP
	if (*(mib->mib_ApMode) != AP_MODE_B_ONLY && *(mib->mib_ApMode)!= AP_MODE_A_ONLY && *(mib->mib_ApMode)!= AP_MODE_AandG)
	{
		byteCnt = ErpBufUpdate(NextElementPtr, StartCmd, IEEE_MSG_BEACON, ExtSuppRateSet_p);
		totalLen += byteCnt;
		NextElementPtr += byteCnt;
	}
	BcnBuffer_p->Hdr.FrmBodyLen = totalLen;

#endif

#ifdef AP_WPA2
	if (mib_RSNConfigWPA2_p->WPA2Enabled || mib_RSNConfigWPA2_p->WPA2OnlyEnabled) 
	{
		if (mib_RSNConfigWPA2_p->WPA2Enabled) 
			byteCnt = AddRSN_IEWPA2MixedMode( vmacSta_p, (IEEEtypes_RSN_IE_WPA2MixedMode_t*)NextElementPtr );
		else
			byteCnt = AddRSN_IEWPA2( vmacSta_p, (IEEEtypes_RSN_IE_WPA2_t*)NextElementPtr );
		NextElementPtr += byteCnt;
		totalLen += byteCnt;
	}

#endif


	/*Adding the QoS Information of beacon here.*/
#ifdef QOS_FEATURE

	//(UINT32)NextElementPtr =  totalLen + (UINT32)BcnBuffer_p +
	//            sizeof(IEEEtypes_MgmtHdr_t);
	if (*(mib->QoSOptImpl))
	{
		BcnBuffer_p->Body.Bcn.CapInfo.QoS = 1;
		NextElementPtr += bcngen_UpdateQBSSLoad(vmacSta_p,NextElementPtr);
#ifdef QOS_WSM_FEATURE
		NextElementPtr += Qos_UpdateWSMQosCapElem(vmacSta_p,NextElementPtr);

#else //QOS_WSM_FEATURE

		//Send EDCA Param or QosCapElem. Not both.
		if (EDCA_Beacon_Counter)
			NextElementPtr += bcngen_AppendEDCAParamSet(vmacSta_p,NextElementPtr);
		else
			NextElementPtr += Qos_UpdateQosCapElem(vmacSta_p,NextElementPtr);
#endif //QOS_WSM_FEATURE

#ifdef QOS_WSM_FEATURE
		//Always Append the WME Parameter Element.

		{
			BcnWMEParamElemLocation_p = NextElementPtr;
			BcnWMEInfoElemLocation_p=NULL;
			NextElementPtr += QoS_AppendWMEParamElem(vmacSta_p,NextElementPtr);
		}


#endif

		totalLen = (UINT32)NextElementPtr - (UINT32)BcnBuffer_p -
			sizeof(IEEEtypes_MgmtHdr_t);
	}


#endif //QOS_FEATURE


	BcnBuffer_p->Hdr.FrmBodyLen = totalLen;

	/*--------------------------------------------------------------*/
	/* The following offset to the CF Parameters in the beacon is   */
	/* calculated for the MAC transmit hardware as follows:         */
	/*                                                              */
	/* 1) First, the offset to be calculated is the number of bytes */
	/*    the transmitter will transmit, starting from the frame    */
	/*    control field in the IEEE header, up to the point of the  */
	/*    CF parameter set information in the beacon body.          */
	/*                                                              */
	/* 2) Since the beacon body byte length field is not to be      */
	/*    transmitted, we must subtract out the length of this      */
	/*    field.                                                    */
	/*                                                              */
	/* 3) Since the 4th address in the IEEE header is not to be     */
	/*    transmitted (since there is no 4th address for management */
	/*    messages), the transmitter skips over this field - hence, */
	/*    we must subtract out the length of this field as well.    */
	/*                                                              */
	/* So, taking the difference between pointers to the CF         */
	/* parameter set and the beacon buffer pointer, and subtracting */
	/* out the additional fields mentioned above, yields the        */
	/* number of bytes tranmitted up to the point of the CF         */
	/* paramters.                                                   */
	/*--------------------------------------------------------------*/
}

/******************************************************************************
* 
* Name:  bcngen_EnableBcnFreeIntr
* 
* Description:  
* Conditions For Use: 
*    None. 
* 
* Arguments: 
*    None. 
* 
* Return Value: 
*    Status indicating success or failure. 
* 
* Notes: 
*    None. 
* 
* PDL: 
*
* END PDL 
* 
*****************************************************************************/
void bcngen_EnableTbttFreeIntr(void)
{
	return ;
}

void bcngen_EnableBcnFreeIntr(void)
{
	return ;
}

/******************************************************************************
* 
* Name:  bcngen_DisableBcnFreeIntr
* 
* Description:  
* Conditions For Use: 
*    None. 
* 
* Arguments: 
*    None. 
* 
* Return Value: 
*    Status indicating success or failure. 
* 
* Notes: 
*    None. 
* 
* PDL: 
*
* END PDL 
* 
*****************************************************************************/
void bcngen_DisableTbttFreeIntr(void)
{
	return ;
}

void bcngen_DisableBcnFreeIntr(void)
{
	return ;
}


/******************************************************************************
* 
* Name: bcn_BeaconFreeIsr  
* 
* Description: This ISR updates the TIM in the Beacon buffer. When the 
*    BCN_BUSY interrupt is raised, the HW MAC is just done with using the
*    beacon buffer and it is safe for the MAC SW to update the TIM fields.
*    After updating the beacon buffer, the interrupt is disabled.  
* 
* Conditions For Use: 
*    None. 
* 
* Arguments: 
*    None. 
* 
* Return Value: 
*    Status indicating success or failure. 
* 
* Notes: 
*    None. 
* 
* PDL: 
*
* END PDL 
* 
*****************************************************************************/

void bcngen_BeaconFreeIsr()
{
	/*----------------------------------------------------------------*/
	/* The beacon body length is calculated as the difference between */
	/* the TIM pointer and the beacon buffer pointer, less the size   */
	/* of the header since that is obviously not part of the body and   */
	/* addition of  size of TIM field                                  */
	/*----------------------------------------------------------------*/
}
/******************************************************************************
* 
* Name:  
* 
* Description: 
*     
* 
* Conditions For Use: 
*    None. 
* 
* Arguments: 
*    None. 
* 
* Return Value: 
*    Status indicating success or failure. 
* 
* Notes: 
*    Refer to Protocol spec for the variables used.Section 7.3.2.6 page 57-58 
* 
* PDL: 
*
* END PDL 
* 
*****************************************************************************/
void bcngen_UpdateBitInTim(UINT16 Aid, BOOLEAN Set)
{

	static UINT32 LowByte = 0, HighByte = 0;
	UINT32 N1, N2;
	UINT32 ByteNum, BitNum, Len, i, j;

	os_EnterCriticalSection;


	if ( Aid == 0)
	{
		if (Set == TRUE)
		{
			/* Set bit 0 in the bit map control */
			Tim.BitmapCtl = Tim.BitmapCtl | 0x01;
		}
		else
		{
			Tim.BitmapCtl = Tim.BitmapCtl & 0xFE ;

		}
		bcngen_EnableBcnFreeIntr();
		os_ExitCriticalSection;
		return ;
	}

	/* Set the corresponding bit in the traffic Map */
	ByteNum = Aid / 8;	
	BitNum = Aid % 8;
	if ( Set)
	{
		if(HighByte < ByteNum)
		{
			HighByte = ByteNum;
			//HighByte +=(HighByte%2);
		}
		TrafficMap[ByteNum] = TrafficMap[ByteNum] | ( 1 << BitNum);
	}
	else
	{
		if ((ByteNum < LowByte) || (ByteNum > HighByte))
		{
			//			printk("ByteNum=%d, LowByte=%d, HighByte=%d\n", ByteNum, LowByte, HighByte);
			//os_ExitCriticalSection;
			//return ;
		}
		TrafficMap[ByteNum] = TrafficMap[ByteNum] & (( 1 << BitNum) ^ 0xff);
	}
	N1 = 0;
	N2 = 0;
	for (i = 0; i <= HighByte; i+=2)
	{
		if(TrafficMap[i] == 0 && TrafficMap[i+1] == 0)
			N1+=2;
		else
			break;
	}
	for (j = HighByte; j > i; j--)
	{
		if(TrafficMap[j] )
		{
			HighByte= j;
			//HighByte -=(HighByte%2);
			break;
		}
	}

	N2 = j;
	if(N2 > N1)
		Len = (N2 - N1 ) + 4;
	else
		Len = 4;
	/* if the lowest byte did not change, copy only the changed byte,
	* if the lowest byte changes, copy the entire map */
	/* copy from byte N1 *2 on to partial bit map bytes */
	TimPtr = &Tim2;
	Tim.PartialVirtualBitmap[0] = 0;

	Tim.Len = Len;

	for (i = 0; i <= N2; i++)
	{
		Tim.PartialVirtualBitmap[i] = TrafficMap[i+N1];
	}
	if((N2 == N1||Len==4)&& Tim.PartialVirtualBitmap[0] == 0)
		N1 = 0;
	TimPtr->BitmapCtl = (TimPtr->BitmapCtl & 0x01) |N1;
	/* Safer update, the beacon should be updated only when beacon free interrupt
	* occurs */
	/* Enable Beacon Free interrupt */
	bcngen_EnableBcnFreeIntr();
	os_ExitCriticalSection;


}

/******************************************************************************
* 
* Name:  
* 
* Description: 
*     
* 
* Conditions For Use: 
*    None. 
* 
* Arguments: 
*    None. 
* 
* Return Value: 
*    Status indicating success or failure. 
* 
* Notes: 
*    Refer to Protocol spec for the variables used.Section 7.3.2.6 page 57-58 
* 
* PDL: 
*
* END PDL 
* 
*****************************************************************************/
void bcngen_UpdateBitInTim2(UINT16 Aid, BOOLEAN Set)
{

	static UINT32 LowByte = 0, HighByte = 0;
	CopyMode_e CopyMode;
	static UINT32 N1, N2, prevN2;
	UINT32 ByteNum, HwordNum, BitNum, Len, i;
	if ( Aid == 0)
	{
		if (Set == TRUE)
		{
			/* Set bit 0 in the bit map control */
			Tim2.BitmapCtl = Tim2.BitmapCtl | 0x01;
		}
		else
		{
			//disable for now     Tim2.BitmapCtl = Tim2.BitmapCtl & 0xFE ;
		}
		bcngen_EnableBcnFreeIntr();

		return ;
	}

	/* Set the corresponding bit in the traffic Map */
	ByteNum = Aid / 8;
	HwordNum = Aid / 16;
	BitNum = Aid % 8;
	CopyMode = COPY_BYTE;
	if ( Set)
	{
		if (ByteNum < N1*2 )
		{
			N1 = Aid / 16;
			CopyMode = COPY_FULL;
		}
		if (ByteNum > N2)
		{
			prevN2 = N2;
			N2 = Aid / 8;
			CopyMode = COPY_END;
		}
		TrafficMap2[ByteNum] = TrafficMap2[ByteNum] | ( 1 << BitNum);
	}
	else
	{
		if ((ByteNum < LowByte) || (ByteNum > HighByte))
			return ;
		TrafficMap2[ByteNum] = TrafficMap2[ByteNum] & (( 1 << BitNum) ^ 0xff);
		if ( HwordNum == N1 && *((UINT16*)TrafficMap2 + HwordNum) == 0)
		{
			CopyMode = COPY_FULL;
			/* Find the next non zero halfword */
			for ( i = N1 + 2; i <= HighByte; i += 2)
			{
				if ((*(UINT16 *)TrafficMap2) + i != 0)
				{
					N1 = i / 2;
					break;
				}
			}
		}
		else if ( ByteNum == N2 && TrafficMap2[ByteNum] == 0)
		{
			CopyMode = COPY_END;
			/* find the previous non zero byte */
			for (i = N2 - 1; i >= LowByte;i--)
			{
				if (TrafficMap2[i] != 0)
				{
					N2 = i;
				}
			}
		}
	}
	Len = (N2 - N1 * 2) + 4;

	/* if the lowest byte did not change, copy only the changed byte,
	* if the lowest byte changes, copy the entire map */
	/* copy from byte N1 *2 on to partial bit map bytes */
	TimPtr2->BitmapCtl = TimPtr2->BitmapCtl & (0x80 | N1);
	Tim2.PartialVirtualBitmap[0] = 0;
	Tim2.Len = Len;
	if (CopyMode == COPY_BYTE)
		Tim2.PartialVirtualBitmap[ByteNum] = TrafficMap2[ByteNum];
	else if (CopyMode == COPY_FULL)
	{
		for (i = N1 * 2; i <= N2; i++)
		{
			Tim2.PartialVirtualBitmap[i] = TrafficMap2[i];
		}
	}
	else if ( CopyMode == COPY_END)
	{
		for (i = prevN2 + 1; i <= N2; i++)
		{
			Tim2.PartialVirtualBitmap[i] = TrafficMap2[i];
		}
	}
	memcpy((UINT8*)TimPtr2, (UINT8*)&Tim2, sizeof(IEEEtypes_InfoElementHdr_t) + Tim2.Len);
	/* Enable Beacon Free interrupt */

}

/******************************************************************************
* 
* Name:  
* 
* Description: 
*     
* 
* Conditions For Use: 
*    None. 
* 
* Arguments: 
*    None. 
* 
* Return Value: 
*    Status indicating success or failure. 
* 
* Notes: 
*    None. 
* 
* PDL: 
*
* END PDL 
* 
*****************************************************************************/
/**
* Update the Probe Response buffer
* Similar to updating beacon buffer
* 
* @param StartCmd Pointer to start command parameters
* @return Number of bytes in the probe response
*/
static void InitHT_IE(vmacApInfo_t *vmacSta_p,IEEEtypes_HT_Element_t *elment)
{
#if 1 //def COMMON_PHYDSSS
	MIB_PHY_DSSS_TABLE *PhyDSSSTable=vmacSta_p->Mib802dot11->PhyDSSSTable;
	UINT8 *mib_guardInterval_p = vmacSta_p->Mib802dot11->mib_guardInterval;
#else
	MIB_PHY_DSSS_TABLE *PhyDSSSTable=&vmacSta_p->Mib802dot11->PhyDSSSTable;
#endif
	memset(elment, 0, sizeof(IEEEtypes_HT_Element_t));
	elment->ElementId = HT;
	elment->Len = 26; //foo 25; //fixed

	elment->HTCapabilitiesInfo.MIMOPwSave = 0x3;
	if(*mib_guardInterval_p == 2)
	{
		elment->HTCapabilitiesInfo.SGI20MHz = 0;
		elment->HTCapabilitiesInfo.SGI40MHz = 0;
	}else
	{
		elment->HTCapabilitiesInfo.SGI20MHz = 1;
		elment->HTCapabilitiesInfo.SGI40MHz = 1;
	}
	if(PhyDSSSTable->Chanflag.ChnlWidth ==CH_20_MHz_WIDTH)
		elment->HTCapabilitiesInfo.SupChanWidth = 0;
	else
		elment->HTCapabilitiesInfo.SupChanWidth = 1;
	if ((*(vmacSta_p->Mib802dot11->mib_ApMode) == AP_MODE_N_ONLY) 
        ||(*(vmacSta_p->Mib802dot11->mib_ApMode) ==AP_MODE_A_ONLY) 
        ||(*(vmacSta_p->Mib802dot11->mib_ApMode) ==AP_MODE_AandG ) 
        || (*(vmacSta_p->Mib802dot11->mib_ApMode) ==AP_MODE_AandN)
        || (*(vmacSta_p->Mib802dot11->mib_ApMode) ==AP_MODE_5GHZ_N_ONLY)
        || (*(vmacSta_p->Mib802dot11->mib_ApMode)&AP_MODE_11AC))
		elment->HTCapabilitiesInfo.DssCck40MHz = 0; //beacon sent at 6Mbps
	else
		elment->HTCapabilitiesInfo.DssCck40MHz = 1; //beacon at 1Mbps
	elment->HTCapabilitiesInfo.DelayedBA =0;
#ifdef DISABLE_AMSDU /*If AMSDU disabled, set max AMSDU Rx size to 4K, needed for V6FW */
	elment->HTCapabilitiesInfo.MaxAMSDUSize =0;
#else
	if ((*(vmacSta_p->Mib802dot11->pMib_11nAggrMode)& WL_MODE_AMSDU_TX_MASK) == WL_MODE_AMSDU_TX_8K)
	elment->HTCapabilitiesInfo.MaxAMSDUSize =1;
	else
		elment->HTCapabilitiesInfo.MaxAMSDUSize =0;
#endif
	//elment->MacHTParamInfo = 1;//for now foo 0628 rxampdu factor to 0 0x3;//0x3;//0x04;

	elment->SupportedMCSset[0] =0xff;

	/** conditon for one by one rate as suggested by ctlaw,  not auto , not 2 antenna and not 3 antenna **/
	if(!((*(vmacSta_p->Mib802dot11->mib_rxAntenna) ==  0) || (*(vmacSta_p->Mib802dot11->mib_rxAntenna) ==  3) ||
		(*(vmacSta_p->Mib802dot11->mib_rxAntenna) ==  2)))
	{
		elment->SupportedMCSset[1] =0x00;
	}
	else
	{
		elment->SupportedMCSset[1] =0xff;
	}

	/** condition for 3x3, rxantenna auto + 3 antenna and mib_3x3 rate=1 **/

	if (*(vmacSta_p->Mib802dot11->mib_3x3Rate) && ((*(vmacSta_p->Mib802dot11->mib_rxAntenna) ==  0) || 
		(*(vmacSta_p->Mib802dot11->mib_rxAntenna) ==  3)))
	{
		elment->SupportedMCSset[2] =0xff;
	}
	else
	{
		elment->SupportedMCSset[2] =0x00;
	}
	
	/* enable MCS32 support */
    elment->SupportedMCSset[4] =0x01;


	elment->MacHTParamInfo = *(vmacSta_p->Mib802dot11->mib_ampdu_factor) | ((*(vmacSta_p->Mib802dot11->mib_ampdu_density)<<2));
#if defined ( INTOLERANT40) || defined (COEXIST_20_40_SUPPORT)

	elment->HTCapabilitiesInfo.FortyMIntolerant = *(vmacSta_p->Mib802dot11->mib_FortyMIntolerant);
#endif

	if (*(vmacSta_p->Mib802dot11->mib_HtGreenField))
		elment->HTCapabilitiesInfo.GreenField = 1;
	else
		elment->HTCapabilitiesInfo.GreenField = 0;

	if (*(vmacSta_p->Mib802dot11->mib_HtStbc))
	{
		elment->HTCapabilitiesInfo.TxSTBC = 1;
		elment->HTCapabilitiesInfo.RxSTBC = 1; /* The first spatial stream */
	}
	else
	{
		elment->HTCapabilitiesInfo.TxSTBC = 0;
		elment->HTCapabilitiesInfo.RxSTBC = 0;
	}
#if defined(SOC_W8864)		
    elment->HTCapabilitiesInfo.AdvCoding = 1;
#endif
#ifdef EXPLICIT_BF
#if 0
	elment->TxBFCapabilities.ImplicitTxBFRxCapable=0;
	elment->TxBFCapabilities.RxStaggeredSoundingCapable=0;
	elment->TxBFCapabilities.TxStaggeredSoundingCapable=0;
	elment->TxBFCapabilities.RxNDPCapable=1;
	elment->TxBFCapabilities.TxNDPCapable=1;
	elment->TxBFCapabilities.ImplicitTXBfTXCapable=0;
	elment->TxBFCapabilities.Calibration=0;
	elment->TxBFCapabilities.ExplicitCSITxBFCapable=1;
	elment->TxBFCapabilities.ExplicitNonCompressedSteerCapable=1;
	elment->TxBFCapabilities.ExplicitCompressedSteeringCapable=1;
	elment->TxBFCapabilities.ExplicitTxBFCSIFB=0;
	elment->TxBFCapabilities.ExplicitNonCompressedBFFBCapable=2; // 1 for delayed fb, 2 for immediate fb, 3 for delay and immediate fb
	elment->TxBFCapabilities.ExplicitCompressedBFFBCapable=2;
	elment->TxBFCapabilities.MinimalGrouping=3;
	elment->TxBFCapabilities.CSINoBFAntSupport=3;
	elment->TxBFCapabilities.NonCompressedSteerNoAntennaSupport=3; //0 for single tx ant, 1 for 2, 2 for 3 , 3 for 4 
	elment->TxBFCapabilities.CompressedSteerNoAntennaSupport=3;
	elment->TxBFCapabilities.CSIMaxNoRowBFSupport=3;
	elment->TxBFCapabilities.ChannelEstimateCapable=3;
	elment->TxBFCapabilities.Reserved=0;
#else
	elment->TxBFCapabilities=txbfcap;
#endif
#endif

}

#ifdef INTEROP
static void Init_Generic_HT_IE(vmacApInfo_t *vmacSta_p,IEEEtypes_Generic_HT_Element_t *elment)
{
#if 1 //def COMMON_PHYDSSS
	MIB_PHY_DSSS_TABLE *PhyDSSSTable=vmacSta_p->Mib802dot11->PhyDSSSTable;
	UINT8 *mib_guardInterval_p = vmacSta_p->Mib802dot11->mib_guardInterval;
#else
	MIB_PHY_DSSS_TABLE *PhyDSSSTable=&vmacSta_p->Mib802dot11->PhyDSSSTable;
#endif
	memset(elment, 0, sizeof(IEEEtypes_Generic_HT_Element_t));

	elment->ElementId = 221;
	elment->Len = 30; //fixed for now
	elment->OUI[0] = 0x00; 
	elment->OUI[1] = 0x90;
	elment->OUI[2] = 0x4c;
	elment->OUIType = 51; /** Temp IE for HT Capabilities Info field **/

	elment->HTCapabilitiesInfo.MIMOPwSave = 0x3; /** Mimo enable, no restriction on what may be sent to the STA  **/

	if(*mib_guardInterval_p== 2)
	{
		elment->HTCapabilitiesInfo.SGI20MHz = 0;
		elment->HTCapabilitiesInfo.SGI40MHz = 0;
	}else
	{
		elment->HTCapabilitiesInfo.SGI20MHz = 1;
		elment->HTCapabilitiesInfo.SGI40MHz = 1;
	}
	if(PhyDSSSTable->Chanflag.ChnlWidth ==CH_20_MHz_WIDTH)
		elment->HTCapabilitiesInfo.SupChanWidth = 0;
	else
		elment->HTCapabilitiesInfo.SupChanWidth = 1;
	if ((*(vmacSta_p->Mib802dot11->mib_ApMode) == AP_MODE_N_ONLY) 
        ||(*(vmacSta_p->Mib802dot11->mib_ApMode) ==AP_MODE_A_ONLY) 
        ||(*(vmacSta_p->Mib802dot11->mib_ApMode) ==AP_MODE_AandG ) 
        || (*(vmacSta_p->Mib802dot11->mib_ApMode) ==AP_MODE_AandN)
        || (*(vmacSta_p->Mib802dot11->mib_ApMode) ==AP_MODE_5GHZ_N_ONLY)
        || (*(vmacSta_p->Mib802dot11->mib_ApMode)&AP_MODE_11AC))
		elment->HTCapabilitiesInfo.DssCck40MHz = 0; //beacon sent at 6Mbps
	else
		elment->HTCapabilitiesInfo.DssCck40MHz = 1; //beacon at 1Mbps
	elment->HTCapabilitiesInfo.DelayedBA =0;
	if ((*(vmacSta_p->Mib802dot11->pMib_11nAggrMode)& WL_MODE_AMSDU_TX_MASK) == WL_MODE_AMSDU_TX_8K)
	elment->HTCapabilitiesInfo.MaxAMSDUSize =1;
	else
		elment->HTCapabilitiesInfo.MaxAMSDUSize =0;
	//	elment->MacHTParamInfo = 0x04;
	elment->SupportedMCSset[0] =0xff;
	elment->SupportedMCSset[1] =0xff;
	//elment->MacHTParamInfo = 1;//for now foo 06280x3;//0x3;//0x04;
	elment->MacHTParamInfo = *(vmacSta_p->Mib802dot11->mib_ampdu_factor) | ((*(vmacSta_p->Mib802dot11->mib_ampdu_density)<<2));

#ifdef INTOLERANT40
	elment->HTCapabilitiesInfo.FortyMIntolerant = *(vmacSta_p->Mib802dot11->mib_FortyMIntolerant);
#endif
	elment->SupportedMCSset[0] =0xff;

	/** conditon for one by one rate as suggested by ctlaw,  not auto , not 2 antenna and not 3 antenna **/
	if(!((*(vmacSta_p->Mib802dot11->mib_rxAntenna) ==  0) || (*(vmacSta_p->Mib802dot11->mib_rxAntenna) ==  3) ||
		(*(vmacSta_p->Mib802dot11->mib_rxAntenna) ==  2)))
	{
		elment->SupportedMCSset[1] =0x00;
	}
	else
	{
		elment->SupportedMCSset[1] =0xff;
	}

	/** condition for 3x3, rxantenna auto + 3 antenna and mib_3x3 rate=1 **/

	if (*(vmacSta_p->Mib802dot11->mib_3x3Rate) && ((*(vmacSta_p->Mib802dot11->mib_rxAntenna) ==  0) || 
		(*(vmacSta_p->Mib802dot11->mib_rxAntenna) ==  3)))
	{
		elment->SupportedMCSset[2] =0xff;
	}
	else
	{
		elment->SupportedMCSset[2] =0x00;
	}
	
	/* enable MCS32 support */
    elment->SupportedMCSset[4] =0x01;

	if (*(vmacSta_p->Mib802dot11->mib_HtGreenField))
		elment->HTCapabilitiesInfo.GreenField = 1;
	else
		elment->HTCapabilitiesInfo.GreenField = 0;

	if (*(vmacSta_p->Mib802dot11->mib_HtStbc))
	{
		elment->HTCapabilitiesInfo.TxSTBC = 1;
		elment->HTCapabilitiesInfo.RxSTBC = 1; /* The first spatial stream */
	}
	else
	{
		elment->HTCapabilitiesInfo.TxSTBC = 0;
		elment->HTCapabilitiesInfo.RxSTBC = 0;
	}

#ifdef EXPLICIT_BF
#if 0
	elment->TxBFCapabilities.ImplicitTxBFRxCapable=0;
	elment->TxBFCapabilities.RxStaggeredSoundingCapable=0;
	elment->TxBFCapabilities.TxStaggeredSoundingCapable=0;
	elment->TxBFCapabilities.RxNDPCapable=1;
	elment->TxBFCapabilities.TxNDPCapable=1;
	elment->TxBFCapabilities.ImplicitTXBfTXCapable=0;
	elment->TxBFCapabilities.Calibration=0;
	elment->TxBFCapabilities.ExplicitCSITxBFCapable=1;
	elment->TxBFCapabilities.ExplicitNonCompressedSteerCapable=1;
	elment->TxBFCapabilities.ExplicitCompressedSteeringCapable=1;
	elment->TxBFCapabilities.ExplicitTxBFCSIFB=0;
	elment->TxBFCapabilities.ExplicitNonCompressedBFFBCapable=2; // 1 for delayed fb, 2 for immediate fb, 3 for delay and immediate fb
	elment->TxBFCapabilities.ExplicitCompressedBFFBCapable=2;
	elment->TxBFCapabilities.MinimalGrouping=3;
	elment->TxBFCapabilities.CSINoBFAntSupport=3;
	elment->TxBFCapabilities.NonCompressedSteerNoAntennaSupport=3; //0 for single tx ant, 1 for 2, 2 for 3 , 3 for 4 
	elment->TxBFCapabilities.CompressedSteerNoAntennaSupport=3;
	elment->TxBFCapabilities.CSIMaxNoRowBFSupport=3;
	elment->TxBFCapabilities.ChannelEstimateCapable=3;
	elment->TxBFCapabilities.Reserved=0;
#else
	elment->TxBFCapabilities=txbfcap;
#endif
#endif


}
UINT16 Add_Generic_HT_IE(vmacApInfo_t *vmacSta_p,IEEEtypes_Generic_HT_Element_t* pNextElement)
{
	if((*(vmacSta_p->Mib802dot11->mib_ApMode)&0x4) && vmacSta_p->Mib802dot11->StationConfig->WSMQoSOptImpl )
	{
		Init_Generic_HT_IE(vmacSta_p, pNextElement);
		return (  pNextElement->Len+2);
	}
	else
		return 0;
}
/** for I_COMP only, not use anymore  **/
static void Init_Generic_HT_IE2(vmacApInfo_t *vmacSta_p,IEEEtypes_Generic_HT_Element_t2 *elment)
{
#if 1 //def COMMON_PHYDSSS
	MIB_PHY_DSSS_TABLE *PhyDSSSTable=vmacSta_p->Mib802dot11->PhyDSSSTable;
	UINT8 *mib_guardInterval_p = vmacSta_p->Mib802dot11->mib_guardInterval;
#else
	MIB_PHY_DSSS_TABLE *PhyDSSSTable=&vmacSta_p->Mib802dot11->PhyDSSSTable;
#endif
	memset(elment, 0, sizeof(IEEEtypes_Generic_HT_Element_t2));

	elment->ElementId = 221;
	elment->Len = 0x1f; //fixed for now
	elment->OUI[0] = 0x00; 
	elment->OUI[1] = 0x17;
	elment->OUI[2] = 0x35;
	elment->OUIType = 51; /** Temp IE for HT Capabilities Info field **/
	elment->Len2 = 0x1a; 

	elment->HTCapabilitiesInfo.MIMOPwSave = 0x3;//only static supported0x3; /** Mimo enable, no restriction on what may be sent to the STA  **/

	if(*mib_guardInterval_p== 2)
	{
		elment->HTCapabilitiesInfo.SGI20MHz = 0;
		elment->HTCapabilitiesInfo.SGI40MHz = 0;
	}else
	{
		elment->HTCapabilitiesInfo.SGI20MHz = 1;
		elment->HTCapabilitiesInfo.SGI40MHz = 1;
	}
	if(PhyDSSSTable->Chanflag.ChnlWidth ==CH_20_MHz_WIDTH)
		elment->HTCapabilitiesInfo.SupChanWidth = 0;
	else
		elment->HTCapabilitiesInfo.SupChanWidth = 1;
	if ((*(vmacSta_p->Mib802dot11->mib_ApMode) == AP_MODE_N_ONLY)
        ||(*(vmacSta_p->Mib802dot11->mib_ApMode) == AP_MODE_5GHZ_N_ONLY))
		elment->HTCapabilitiesInfo.DssCck40MHz = 0; //beacon sent at 6Mbps
	else
		elment->HTCapabilitiesInfo.DssCck40MHz = 1; //beacon at 1Mbps
	elment->HTCapabilitiesInfo.DelayedBA =1;
	if ((*(vmacSta_p->Mib802dot11->pMib_11nAggrMode)& WL_MODE_AMSDU_TX_MASK) == WL_MODE_AMSDU_TX_8K)
	elment->HTCapabilitiesInfo.MaxAMSDUSize =1;
	else
		elment->HTCapabilitiesInfo.MaxAMSDUSize =0;
	//elment->MacHTParamInfo = 0x04;
	elment->SupportedMCSset[0] =0xff;
	elment->SupportedMCSset[1] =0xff;
	elment->MacHTParamInfo = *(vmacSta_p->Mib802dot11->mib_ampdu_factor) | (*(vmacSta_p->Mib802dot11->mib_ampdu_density)<<2);
	if (*(vmacSta_p->Mib802dot11->mib_3x3Rate))
		elment->SupportedMCSset[2] =0xff;
	else
		elment->SupportedMCSset[2] =0x00;

	if (*(vmacSta_p->Mib802dot11->mib_HtGreenField))
		elment->HTCapabilitiesInfo.GreenField = 1;
	else
		elment->HTCapabilitiesInfo.GreenField = 0;

	if (*(vmacSta_p->Mib802dot11->mib_HtStbc))
	{
		elment->HTCapabilitiesInfo.TxSTBC = 1;
		elment->HTCapabilitiesInfo.RxSTBC = 1; /* The first spatial stream */
	}
	else
	{
		elment->HTCapabilitiesInfo.TxSTBC = 0;
		elment->HTCapabilitiesInfo.RxSTBC = 0;
	}

#ifdef EXPLICIT_BF
#if 0
	elment->TxBFCapabilities.ImplicitTxBFRxCapable=0;
	elment->TxBFCapabilities.RxStaggeredSoundingCapable=0;
	elment->TxBFCapabilities.TxStaggeredSoundingCapable=0;
	elment->TxBFCapabilities.RxNDPCapable=1;
	elment->TxBFCapabilities.TxNDPCapable=1;
	elment->TxBFCapabilities.ImplicitTXBfTXCapable=0;
	elment->TxBFCapabilities.Calibration=0;
	elment->TxBFCapabilities.ExplicitCSITxBFCapable=1;
	elment->TxBFCapabilities.ExplicitNonCompressedSteerCapable=1;
	elment->TxBFCapabilities.ExplicitCompressedSteeringCapable=1;
	elment->TxBFCapabilities.ExplicitTxBFCSIFB=0;
	elment->TxBFCapabilities.ExplicitNonCompressedBFFBCapable=2; // 1 for delayed fb, 2 for immediate fb, 3 for delay and immediate fb
	elment->TxBFCapabilities.ExplicitCompressedBFFBCapable=2;
	elment->TxBFCapabilities.MinimalGrouping=3;
	elment->TxBFCapabilities.CSINoBFAntSupport=3;
	elment->TxBFCapabilities.NonCompressedSteerNoAntennaSupport=3; //0 for single tx ant, 1 for 2, 2 for 3 , 3 for 4 
	elment->TxBFCapabilities.CompressedSteerNoAntennaSupport=3;
	elment->TxBFCapabilities.CSIMaxNoRowBFSupport=3;
	elment->TxBFCapabilities.ChannelEstimateCapable=3;
	elment->TxBFCapabilities.Reserved=0;
#else
	elment->TxBFCapabilities=txbfcap;
#endif
#endif

}
UINT16 Add_Generic_HT_IE2(vmacApInfo_t *vmacSta_p,IEEEtypes_Generic_HT_Element_t2* pNextElement)
{
	if((*(vmacSta_p->Mib802dot11->mib_ApMode)&0x4) && vmacSta_p->Mib802dot11->StationConfig->WSMQoSOptImpl )
	{
		Init_Generic_HT_IE2(vmacSta_p,pNextElement);
		return (  pNextElement->Len+2);
	}
	else
		return 0;
}
#endif
UINT16 AddHT_IE(vmacApInfo_t *vmacSta_p,IEEEtypes_HT_Element_t* pNextElement)
{
	if((*(vmacSta_p->Mib802dot11->mib_ApMode)&0x4) && vmacSta_p->Mib802dot11->StationConfig->WSMQoSOptImpl )
	{
		InitHT_IE(vmacSta_p,pNextElement);
		return (  pNextElement->Len+2);
	}
	else
		return 0;
}
static void InitAddHT_IE(vmacApInfo_t *vmacSta_p,IEEEtypes_Add_HT_Element_t *elment)
{
	MIB_PHY_DSSS_TABLE *PhyDSSSTable=vmacSta_p->Mib802dot11->PhyDSSSTable;
	MIB_802DOT11 *mib = vmacSta_p->ShadowMib802dot11;
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, vmacSta_p->dev);
	memset(elment, 0, sizeof(IEEEtypes_Add_HT_Element_t));
	elment->ElementId = ADD_HT;
	elment->Len = 22; //>=22
	elment->ControlChan = PhyDSSSTable->CurrChan;


#if defined ( INTOLERANT40) || defined (COEXIST_20_40_SUPPORT)
	/** fktang to check wheter 2.4G **/
	if(*(vmacSta_p->ShadowMib802dot11->mib_HT40MIntoler) && (*(mib->USER_ChnlWidth )==0))
	{

		elment->AddChan.ExtChanOffset = 0;
		elment->AddChan.STAChannelWidth = 0;

	}
	else
#endif
		if(PhyDSSSTable->Chanflag.ChnlWidth !=CH_20_MHz_WIDTH)
		{
			elment->AddChan.ExtChanOffset = PhyDSSSTable->Chanflag.ExtChnlOffset;
			elment->AddChan.STAChannelWidth = 1;
		}
		if(*(vmacSta_p->Mib802dot11->mib_rifsQNum))
			elment->AddChan.RIFSMode = 1;//pWlSysCfg->Mib802dot11->mib_rifsQNum;
		if ((*(vmacSta_p->Mib802dot11->mib_ApMode) == AP_MODE_N_ONLY)
            ||(*(vmacSta_p->Mib802dot11->mib_ApMode) == AP_MODE_5GHZ_N_ONLY))
		{
			elment->OpMode.OpMode = 0;
			elment->BscMCSSet[0] = 0xff;
			elment->BscMCSSet[1] = 0xff;
		}else
		{
			elment->OpMode.OpMode = 0;
		}
		if (*(mib->mib_HtGreenField))
		{ 
			if (vmacSta_p->NonGFSta || wlpptr->wlpd_p->NonGFSta)
				elment->OpMode.NonGFStaPresent = 1;
			else
				elment->OpMode.NonGFStaPresent = 0;
		} else
			elment->OpMode.NonGFStaPresent = 1;
}
UINT16 AddAddHT_IE(vmacApInfo_t *vmacSta_p,IEEEtypes_Add_HT_Element_t* pNextElement)
{
	if((*(vmacSta_p->Mib802dot11->mib_ApMode)&0x4) && vmacSta_p->Mib802dot11->StationConfig->WSMQoSOptImpl )
	{
		InitAddHT_IE(vmacSta_p,pNextElement);
		return (  pNextElement->Len+2);
	}
	else
		return 0;
}
#ifdef INTEROP
static void Init_Generic_AddHT_IE(vmacApInfo_t *vmacSta_p,IEEEtypes_Generic_Add_HT_Element_t *elment)
{
#if 1 //def COMMON_PHYDSSS
	MIB_PHY_DSSS_TABLE *PhyDSSSTable=vmacSta_p->Mib802dot11->PhyDSSSTable;
#else
	MIB_PHY_DSSS_TABLE *PhyDSSSTable=&vmacSta_p->Mib802dot11->PhyDSSSTable;
#endif
	memset(elment, 0, sizeof(IEEEtypes_Generic_Add_HT_Element_t));

	elment->ElementId = 221;
	elment->Len = 26; //fixed for now
	elment->OUI[0] = 0x00; 
	elment->OUI[1] = 0x90;
	elment->OUI[2] = 0x4c;
	elment->OUIType = 52; /** Temp IE for HT Capabilities Info field **/

	elment->ControlChan = PhyDSSSTable->CurrChan;
#ifdef INTOLERANT40
	if(*(vmacSta_p->Mib802dot11->mib_FortyMIntolerant))
	{
		elment->AddChan.ExtChanOffset = 0;
		elment->AddChan.STAChannelWidth = 0;
	}
	else	
#endif
		if(PhyDSSSTable->Chanflag.ChnlWidth !=CH_20_MHz_WIDTH)
		{
			elment->AddChan.ExtChanOffset = PhyDSSSTable->Chanflag.ExtChnlOffset;
			elment->AddChan.STAChannelWidth = 1;
		}
		if(*(vmacSta_p->Mib802dot11->mib_rifsQNum))
			elment->AddChan.RIFSMode = 1;
		if ((*(vmacSta_p->Mib802dot11->mib_ApMode) == AP_MODE_N_ONLY)
            ||(*(vmacSta_p->Mib802dot11->mib_ApMode) == AP_MODE_5GHZ_N_ONLY))
		{
			elment->OpMode.OpMode = 0;
			elment->BscMCSSet[0] = 0xff;
			elment->BscMCSSet[1] = 0xff;
		}else
		{
			elment->OpMode.OpMode = 0;
		}
}

UINT16 Add_Generic_AddHT_IE(vmacApInfo_t *vmacSta_p,IEEEtypes_Generic_Add_HT_Element_t* pNextElement)
{
	if((*(vmacSta_p->Mib802dot11->mib_ApMode)&0x4) && vmacSta_p->Mib802dot11->StationConfig->WSMQoSOptImpl )
	{
		Init_Generic_AddHT_IE(vmacSta_p,pNextElement);
		return (  pNextElement->Len+2);
	}
	else
		return 0;
}
static void Init_Generic_AddHT_IE2(vmacApInfo_t *vmacSta_p,IEEEtypes_Generic_Add_HT_Element_t2 *elment)
{
#if 1 //def COMMON_PHYDSSS
	MIB_PHY_DSSS_TABLE *PhyDSSSTable=vmacSta_p->Mib802dot11->PhyDSSSTable;
#else
	MIB_PHY_DSSS_TABLE *PhyDSSSTable=&vmacSta_p->Mib802dot11->PhyDSSSTable;
#endif
	memset(elment, 0, sizeof(IEEEtypes_Generic_Add_HT_Element_t2));

	elment->ElementId = 221;
	elment->Len = 27; //fixed for now
	elment->OUI[0] = 0x00; 
	elment->OUI[1] = 0x17;
	elment->OUI[2] = 0x35;
	elment->OUIType = 52; /** Temp IE for HT Capabilities Info field **/
	elment->Len2 = 27-5;

	elment->ControlChan = PhyDSSSTable->CurrChan;
    
	if(PhyDSSSTable->Chanflag.ChnlWidth !=CH_20_MHz_WIDTH)
	{
		elment->AddChan.ExtChanOffset = PhyDSSSTable->Chanflag.ExtChnlOffset;
		elment->AddChan.STAChannelWidth = 1;
	}
	elment->AddChan.RIFSMode = *(vmacSta_p->Mib802dot11->mib_rifsQNum);
	if ((*(vmacSta_p->Mib802dot11->mib_ApMode) == AP_MODE_N_ONLY)
        ||(*(vmacSta_p->Mib802dot11->mib_ApMode) == AP_MODE_5GHZ_N_ONLY))
	{
		elment->OpMode.OpMode = 0;
		elment->BscMCSSet[0] = 0xff;
		elment->BscMCSSet[1] = 0xff;
	}else
	{
		elment->OpMode.OpMode = 0;
	}
}

UINT16 Add_Generic_AddHT_IE2(vmacApInfo_t *vmacSta_p,IEEEtypes_Generic_Add_HT_Element_t2* pNextElement)
{
	if((*(vmacSta_p->Mib802dot11->mib_ApMode)&0x4) && vmacSta_p->Mib802dot11->StationConfig->WSMQoSOptImpl )
	{
		Init_Generic_AddHT_IE2(vmacSta_p,pNextElement);
		return (  pNextElement->Len+2);
	}
	else
		return 0;
}
#endif
IEEEtypes_M_Element_t M_COMP_ID_IE = {PROPRIETARY_IE, 6, {0,0x50,0x43}, 3, 0, 0};
static void InitM_IE(vmacApInfo_t *vmacSta_p,IEEEtypes_M_Element_t *element)
{
	memcpy((UINT8 *)element, (UINT8 *)&M_COMP_ID_IE, sizeof(IEEEtypes_M_Element_t));
}
UINT16 AddM_IE(vmacApInfo_t *vmacSta_p,IEEEtypes_HT_Element_t* pNextElement)
{
	if((*(vmacSta_p->Mib802dot11->mib_ApMode)&0x4) )
	{
		InitM_IE(vmacSta_p,(IEEEtypes_M_Element_t *)pNextElement);
		return (  pNextElement->Len+2);
	}
	else
		return 0;
}
BOOLEAN isMcIdIE(UINT8 *data_p)
{
	IEEEtypes_M_Element_t *ie;
	ie = (IEEEtypes_M_Element_t*)data_p;
	if(memcmp((UINT8 *)ie, (UINT8 *)&M_COMP_ID_IE, 7 ))
		return FALSE;
	if(ie->Version > M_COMP_ID_IE.Version)
	{
		//todo
	}
	return TRUE;
}

IEEEtypes_M_Rptr_Element_t M_COMP_RPTR_ID_IE = {PROPRIETARY_IE, 38, {0,0x40,0x96}, 0x27, 0x00, 0x10,{0}};

static void InitM_Rptr_IE(vmacApInfo_t *vmacSta_p,IEEEtypes_M_Rptr_Element_t *element)
{
	memcpy(M_COMP_RPTR_ID_IE.RptrDeviceType, vmacSta_p->Mib802dot11->mib_RptrDeviceType, strlen(vmacSta_p->Mib802dot11->mib_RptrDeviceType));
	memcpy((UINT8 *)element, (UINT8 *)&M_COMP_RPTR_ID_IE, sizeof(IEEEtypes_M_Rptr_Element_t));
}
UINT16 AddM_Rptr_IE(vmacApInfo_t *vmacSta_p,IEEEtypes_HT_Element_t* pNextElement)
{
	InitM_Rptr_IE(vmacSta_p,(IEEEtypes_M_Rptr_Element_t *)pNextElement);
	return (  pNextElement->Len+2);
}
BOOLEAN isM_RptrIdIE(UINT8 *data_p)
{
	IEEEtypes_M_Element_t *ie;
	ie = (IEEEtypes_M_Element_t*)data_p;
	if(memcmp((UINT8 *)ie, (UINT8 *)&M_COMP_RPTR_ID_IE, 7 ))
		return FALSE;
	if(ie->Version > M_COMP_ID_IE.Version)
	{
		//todo
	}
	return TRUE;
}

UINT8 getRegulatoryClass(vmacApInfo_t *vmacSta_p)
{
	UINT8 domainCode = DOMAIN_CODE_FCC;
	UINT8 RegulatoryClass = 12;

	domainCode = domainGetDomain();
	// TODO here
	if (domainCode == DOMAIN_CODE_FCC)
	{
		switch (vmacSta_p->Mib802dot11->PhyDSSSTable->CurrChan)
		{
		case 1:
		case 2:
		case 3:
		case 4:
			RegulatoryClass = 32;
			break;		
		case 5:
		case 6:
		case 7:
			if (vmacSta_p->Mib802dot11->PhyDSSSTable->Chanflag.ExtChnlOffset == EXT_CH_BELOW_CTRL_CH)
				RegulatoryClass = 32;
			else
				RegulatoryClass = 33;
			break;
		case 8:
		case 9:
		case 10:
		case 11:
			RegulatoryClass = 33;
			break;		
		case 36:
		case 44:
			RegulatoryClass = 22;
			break;	
		case 52:
		case 60:
			RegulatoryClass = 23;
			break;	
		case 149:
		case 157:
			RegulatoryClass = 25;
			break;	
		case 40:
		case 48:
			RegulatoryClass = 27;
			break;	
		case 56:
		case 64:
			RegulatoryClass = 28;
			break;	
		case 153:
		case 161:
			RegulatoryClass = 30;
			break;	
		default:
			break;
		}
	}
	else if (domainCode == DOMAIN_CODE_ETSI)
	{
		switch (vmacSta_p->Mib802dot11->PhyDSSSTable->CurrChan)
		{
		case 1:
		case 2:
		case 3:
		case 4:
			RegulatoryClass = 11;
			break;		
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
			if (vmacSta_p->Mib802dot11->PhyDSSSTable->Chanflag.ExtChnlOffset == EXT_CH_BELOW_CTRL_CH)
				RegulatoryClass = 11;
			else
				RegulatoryClass = 12;
			break;
		case 10:
		case 11:
		case 12:
		case 13:
			RegulatoryClass = 12;
			break;		
		case 36:
		case 44:
			RegulatoryClass = 5;
			break;	
		case 52:
		case 60:
			RegulatoryClass = 6;
			break;	
		case 40:
		case 48:
			RegulatoryClass = 8;
			break;	
		case 56:
		case 64:
			RegulatoryClass = 9;
			break;	
		default:
			break;
		}
	}
	else if (domainCode == DOMAIN_CODE_MKK)
	{
		switch (vmacSta_p->Mib802dot11->PhyDSSSTable->CurrChan)
		{
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
		case 10:
		case 11:
		case 12:
		case 13:
			RegulatoryClass = 30;
			break;		
		case 14:
			RegulatoryClass = 31;
			break;		
		case 52:
		case 56:
		case 60:
		case 64:
			RegulatoryClass = 32;
			break;	
		default:
			break;
		}
	}

	return RegulatoryClass;	
}
#ifdef INTOLERANT40
static void InitChanReport_IE(vmacApInfo_t *vmacSta_p,IEEEtypes_ChannelReportEL_t *elment)
{
	memset(elment, 0, sizeof(IEEEtypes_ChannelReportEL_t));
	elment->ElementId = CHAN_REPORT;
	elment->RegClass = getRegulatoryClass(vmacSta_p);
	domainGetInfo(elment->ChanList);
	elment->Len = 1 +  strlen(&elment->ChanList[0]);
}


UINT16 AddChanReport_IE(vmacApInfo_t *vmacSta_p,IEEEtypes_ChannelReportEL_t* pNextElement)
{
	InitChanReport_IE(vmacSta_p, (IEEEtypes_ChannelReportEL_t *)pNextElement);
	return (  pNextElement->Len+2);
}
#endif
#ifdef COEXIST_20_40_SUPPORT
static void InitExtended_Cap_IE(vmacApInfo_t *vmacSta_p,IEEEtypes_Extended_Cap_Element_t *elment)
{

	memset(elment, 0, sizeof(IEEEtypes_Extended_Cap_Element_t));
	elment->ElementId = EXT_CAP_IE;
	elment->ExtCap._20_40Coexistence_Support = 1;
	
	if(*(vmacSta_p->Mib802dot11->mib_ApMode) >= AP_MODE_5GHZ_11AC_ONLY){
		elment->ExtCap._20_40Coexistence_Support = 0;
		elment->ExtCap.OpModeNotification = 1;
	}
	
	elment->Len = 8;		
}

UINT16 AddExtended_Cap_IE(vmacApInfo_t *vmacSta_p,IEEEtypes_Extended_Cap_Element_t * pNextElement)
{
	InitExtended_Cap_IE(vmacSta_p, (IEEEtypes_Extended_Cap_Element_t  *)pNextElement);
	return (  pNextElement->Len+2);
}




static void Init20_40_Coexist_IE(vmacApInfo_t *vmacSta_p,IEEEtypes_20_40_BSS_COEXIST_Element_t *elment)
{

	memset(elment, 0, sizeof(IEEEtypes_20_40_BSS_COEXIST_Element_t));
	elment->ElementId = _20_40_BSSCOEXIST;
	elment->Len = 1;
	elment->Coexist.Inform_Request = 0; /* is used to indicate that a transmitting STA is requesting the recipient to transmit a 20/40 BSS Coexistence Mangagement frame with 
										transmittign STA as the recipient */
	elment->Coexist.FortyMhz_Intorant = 0; /** when set to 1, prohitbits an AP that receives this information or reports of this information from operating a 20/40 MHz BSS **/
	elment->Coexist.TwentyMhz_BSS_Width_Request=0; /** when set to 1, prohibits a receiving AP from operating its BSS as a 20/40MHz BSS **/
	elment->Coexist.OBSS_Scanning_Exemption_Grant=0;  /** when set to 1 to indicate that the transmitting non-AP STAis requesting the BSS to allow the STA
													  to be exempt from OBSS Scanning **/
	elment->Coexist.OBSS_Scanning_Exemption_Request=0; /** field is  reserved for AP **/

}

UINT16 Add20_40_Coexist_IE(vmacApInfo_t *vmacSta_p,IEEEtypes_20_40_BSS_COEXIST_Element_t * pNextElement)
{
	Init20_40_Coexist_IE(vmacSta_p, (IEEEtypes_20_40_BSS_COEXIST_Element_t  *)pNextElement);
	return (  pNextElement->Len+2);
}

static void InitOverlap_BSS_Scan_Parameters_IE(vmacApInfo_t *vmacSta_p,IEEEtypes_OVERLAP_BSS_SCAN_PARAMETERS_Element_t *elment)
{
	MIB_802DOT11 *mib = vmacSta_p->ShadowMib802dot11;

	memset(elment, 0, sizeof(IEEEtypes_OVERLAP_BSS_SCAN_PARAMETERS_Element_t));
	elment->ElementId = OVERLAPPING_BSS_SCAN_PARAMETERS;
	elment->Len = 14;
	elment->Scan_Passive = 20;  /** contain a value in TUs encoded as an integer, that a receiving STA uses as described in 11.14.5 */
	elment->Scan_Active = 10;
	elment->Channel_Width_Trigger_Scan_Interval =*(mib->mib_Channel_Width_Trigger_Scan_Interval); //180; //tbd 300; /**  value in second, as describe in 11.14.5 **/
	elment->Scan_Passive_Total_Per_Channel = 200; /** value in TU, as describe in 11.14.5 **/
	elment->Scan_Active_Total_Per_Channel=20; /** value in TU, as describe in 11.14.5 **/
	elment->Width_Channel_Transition_Delay_Factor=*(mib->mib_Channel_Transition_Delay_Factor);  /** integer value describe in 11.14.5 **/
	elment->Scan_Activity_Threshold = 25; /** contain a value in hundreds of percent encoded as unsigned integer as described in 11.14.5 **/

}

UINT16 AddOverlap_BSS_Scan_Parameters_IE(vmacApInfo_t *vmacSta_p,IEEEtypes_OVERLAP_BSS_SCAN_PARAMETERS_Element_t * pNextElement)
{
	InitOverlap_BSS_Scan_Parameters_IE(vmacSta_p, (IEEEtypes_OVERLAP_BSS_SCAN_PARAMETERS_Element_t *)pNextElement);
	return (  pNextElement->Len+2);
}
static void Init20_40Interant_Channel_Report_IE(vmacApInfo_t *vmacSta_p,IEEEtypes_20_40_INTOLERANT_CHANNEL_REPORT_Element_t *elment)
{
	extern int domainGetInfo(UINT8 *);

	memset(elment, 0, sizeof(IEEEtypes_20_40_INTOLERANT_CHANNEL_REPORT_Element_t));
	elment->ElementId = _20_40_BSS_INTOLERANT_CHANNEL_REPORT;
	elment->RegClass = getRegulatoryClass(vmacSta_p);
	domainGetInfo(elment->ChanList);
	elment->Len = 1 +  strlen(&elment->ChanList[0]);
}

UINT16 Add20_40Interant_Channel_Report_IE(vmacApInfo_t *vmacSta_p,IEEEtypes_20_40_INTOLERANT_CHANNEL_REPORT_Element_t * pNextElement)
{
	Init20_40Interant_Channel_Report_IE(vmacSta_p, (IEEEtypes_20_40_INTOLERANT_CHANNEL_REPORT_Element_t *)pNextElement);
	return (  pNextElement->Len+2);
}
#endif
#ifdef MRVL_WPS2
UINT16 Build_AssocResp_WSCIE(vmacApInfo_t *vmacSta_p, AssocResp_WSCIE_t *pNextElement)
{
    memset(pNextElement, 0, sizeof(AssocResp_WSCIE_t));
    pNextElement->ElementId = PROPRIETARY_IE;
    pNextElement->Len = sizeof(AssocResp_WSCIE_t) -2;
    pNextElement->WSC_OUI[0] = 0x00;
    pNextElement->WSC_OUI[1] = 0x50;
    pNextElement->WSC_OUI[2] = 0xF2;
    pNextElement->WSC_OUI[3] = 0x04;

    pNextElement->Version.ID = (UINT16)SHORT_SWAP(WSC_VERSION_ATTRB);
    pNextElement->Version.Len = SHORT_SWAP(0x0001);
    pNextElement->Version.Version = 0x10;
    
    pNextElement->ResponseType.ID = (UINT16)SHORT_SWAP((UINT16)WSC_RESP_TYPE_ATTRB);
    pNextElement->ResponseType.Len = SHORT_SWAP(0x0001);
    pNextElement->ResponseType.ResponseType = 0x03;

    pNextElement->VendorExtn.ID = (UINT16)SHORT_SWAP((UINT16)WSC_VENDOR_EXTN_ATTRB);
    pNextElement->VendorExtn.Len = SHORT_SWAP(6);
    pNextElement->VendorExtn.VendorID[0]= 0x00;
    pNextElement->VendorExtn.VendorID[1]= 0x37;
    pNextElement->VendorExtn.VendorID[2]= 0x2A;
    pNextElement->Version2.ID = 0x00;
    pNextElement->Version2.Len = 1;
    pNextElement->Version2.Version2 = 0x20;

    return (pNextElement->Len+2);
    
}
#endif

//for bringup; need to move to mib
UINT32 vht_cap =  0x339b7930;
UINT32 SupportedRxVhtMcsSet=0xffea;
UINT32 SupportedTxVhtMcsSet=0xffea;
UINT32 ch_width = 1;
UINT32 center_freq0 = 42;
UINT32 center_freq1 = 0;
UINT32 basic_vht_mcs = 0xffc0;		//In sniffer, 1st byte 0xc0 is decoded first to become c0 ff

UINT32 GetCenterFreq(UINT32 ch, UINT32 bw)
{
    if((bw == CH_80_MHz_WIDTH) ||  (bw == CH_AUTO_WIDTH))
    {
        switch(ch)
        {
            case 36:
            case 40:
            case 44:
            case 48:
                return(42);
            case 52:
            case 56:
            case 60:
            case 64:
                return(58);
            case 100:
            case 104:
            case 108:
            case 112:
                return(106);
            case 149:
            case 153:
            case 157:
            case 161:
                return(155);
        }
    }
    else if(bw == CH_40_MHz_WIDTH)
    {
        switch(ch)
        {
            case 36:
            case 40:
                return(38);
            case 44:
            case 48:
                return(46);
            case 52:
            case 56:
                return(54);
            case 60:
            case 64:
                return(62);
            case 100:
            case 104:
                return(102);
            case 108:
            case 112:
                return(110);
            case 149:
            case 153:
                return(151);
            case 157:
            case 161:
                return(159);
        }
    }
    return(ch);
}

//TODO: need to reprogram according to mib setting
UINT16 Build_IE_191(vmacApInfo_t *vmacSta_p, UINT8*IE_p)
{
    UINT32 SupportedRxVhtMcsSetMask = 0xffc0;
    UINT32 SupportedTxVhtMcsSetMask = 0xffc0;
    IEEEtypes_VhtCap_t *ptr = (IEEEtypes_VhtCap_t *)IE_p;
    ptr->id = 191;
    ptr->len = 12;
    memcpy((UINT8 *)&ptr->cap, &vht_cap, sizeof(IEEEtypes_VHT_Cap_Info_t));		
    
	if(!((*(vmacSta_p->Mib802dot11->mib_rxAntenna) ==  0) || (*(vmacSta_p->Mib802dot11->mib_rxAntenna) ==  3) ||
		(*(vmacSta_p->Mib802dot11->mib_rxAntenna) ==  2)))
	{   /* Rx only supports one stream 11ac rates */
    	SupportedRxVhtMcsSetMask =0xfffc;
	}
	else if(*(vmacSta_p->Mib802dot11->mib_rxAntenna) ==  2)
	{   /* Rx supports up to two stream 11ac rates */
    	SupportedRxVhtMcsSetMask =0xfff0;
	}
	else{
		/* Rx supports up to three stream 11ac rates */
    	SupportedRxVhtMcsSetMask =0xffc0;
	}
	
	if(!((*(vmacSta_p->Mib802dot11->mib_txAntenna) ==  0) || (*(vmacSta_p->Mib802dot11->mib_txAntenna) ==  3) ||
		(*(vmacSta_p->Mib802dot11->mib_txAntenna) ==  7)))
	{   /* Tx only supports one stream 11ac rates */
    	SupportedTxVhtMcsSetMask =0xfffc;
	}
	else if(*(vmacSta_p->Mib802dot11->mib_txAntenna) ==  3)
	{   /* Tx supports up to two stream 11ac rates */
    	SupportedTxVhtMcsSetMask =0xfff0;
	}
	else{
		/* Tx supports up to three stream 11ac rates */
    	SupportedTxVhtMcsSetMask =0xffc0;
	}
	
	if (*(vmacSta_p->Mib802dot11->mib_HtStbc))
	{
		ptr->cap.TxSTBC = 1;
	}
	else
	{
		ptr->cap.TxSTBC = 0;
	}

    ptr->SupportedRxMcsSet = SupportedRxVhtMcsSet | SupportedRxVhtMcsSetMask;
    ptr->SupportedTxMcsSet = SupportedTxVhtMcsSet | SupportedTxVhtMcsSetMask;

    return (sizeof(IEEEtypes_VhtCap_t));
}
UINT16 Build_IE_192(vmacApInfo_t *vmacSta_p, UINT8*IE_p)
{
	MIB_802DOT11 *mib = vmacSta_p->ShadowMib802dot11;
    MIB_PHY_DSSS_TABLE *PhyDSSSTable=mib->PhyDSSSTable;
    IEEEtypes_VhOpt_t *ptr = (IEEEtypes_VhOpt_t *)IE_p;

    ptr->id = 192;
    ptr->len = 5;
    if((PhyDSSSTable->Chanflag.ChnlWidth == CH_40_MHz_WIDTH) ||
        (PhyDSSSTable->Chanflag.ChnlWidth == CH_20_MHz_WIDTH) || 
        (PhyDSSSTable->Chanflag.FreqBand == FREQ_BAND_2DOT4GHZ))
    {
        ch_width = 0;
    }
    else
    {
        ch_width = 1;
    }
    /*
    else if(PhyDSSSTable->Chanflag.ChnlWidth == CH_80_MHz_WIDTH)
    {
        ch_width = 1;
    }
    */
    ptr->ch_width = ch_width;
    if(ch_width == 0)
    {
        /* Channel Center freq seg0 field is reserved in ht20 or ht40 */
        center_freq0 = 0;
    }
    else
    {
        center_freq0 = GetCenterFreq(PhyDSSSTable->CurrChan, 
                            PhyDSSSTable->Chanflag.ChnlWidth);
    }
    ptr->center_freq0 = center_freq0;
    ptr->center_freq1 = center_freq1;
    ptr->basic_mcs = basic_vht_mcs;
    return (sizeof(IEEEtypes_VhOpt_t));
}

