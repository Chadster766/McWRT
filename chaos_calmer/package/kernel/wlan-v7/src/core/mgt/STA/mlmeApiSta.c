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

/*******************************************************************************************
*
* File: mlmeStaInterface.c
*        Client MLME Module Interface
* Description:  Handle all the other modules' api coming in and out of the MLME 
*
*******************************************************************************************/


#include "wltypes.h"
#include "mhsm.h"
#include "mlmeSta.h"

#include "mlmeApi.h"
#include "wl_mib.h"
#include "wl_hal.h"
#include "wlvmac.h"

#include "wl_macros.h"
#include "StaDb.h"

#include "idList.h"


#ifdef PORT_TO_LINUX_OS
#include <linux/skbuff.h>
#include "ap8xLnxIntf.h"
#include "ap8xLnxFwcmd.h"

#include "mlmeParent.h"
struct ieee80211_frame
{
	IEEEtypes_FrameCtl_t FrmCtl;
	UINT8	dur[2];
	UINT8	addr1[IEEEtypes_ADDRESS_SIZE];
	UINT8	addr2[IEEEtypes_ADDRESS_SIZE];
	UINT8	addr3[IEEEtypes_ADDRESS_SIZE];
	UINT8	seq[2];
	UINT8   addr4[IEEEtypes_ADDRESS_SIZE];
} PACK;

#define TEST_SCAN_FLOW  1
#ifdef TEST_SCAN_FLOW
UINT8   curChannel = 11;
#endif /* TEST_SCAN_FLOW */

#define SK_BUF_RESERVED_PAD    6
#define MGT_FRAME_BUF_SIZE   1024
#define SME_CMD_BUF_Q_LIMIT     8
macmgmtQ_CmdBuf_t smeCmdBuf_q[SME_CMD_BUF_Q_LIMIT];
UINT8   smeCmdBufCnt = 0;

/* Extern for LINUS OS Platform */
extern struct sk_buff * ieee80211_getmgtframe(UINT8 **frm, unsigned int pktlen);
extern WL_STATUS txMgmtMsg(struct net_device *dev,struct sk_buff *skb);
extern void InitClientInfo(UINT8 *macAddr_p, dot11MgtFrame_t *MgmtMsg_p, vmacEntry_t *clientVMacEntry_p, BOOLEAN isApMrvl);
extern int	wlchannelSet(struct net_device *netdev, int channel, CHNL_FLAGS chanflag, UINT8 initRateTable);
extern BOOLEAN wlUpdateAutoChan(vmacApInfo_t *vmacSta_p,UINT32 chan,UINT8 shadowMIB);
#else /* PORT_TO_LINUX_OS */
extern pool_ID_t txMgmtPoolIdPerMac[];
extern pool_ID_t smeCmdPoolId;
#endif /* PORT_TO_LINUX_OS */


/*importing this because it is not allowing me to include the macMgmtMlme.h file*/
extern BOOLEAN macMgtMlme_seturAid(phyMacId_t phyMacId);
extern BOOLEAN macMgmtMlme_UpdateRateInfoForPeerStationWithRateSetInfo(UINT32 Aid,
																	   IEEEtypes_DataRate_t *bOpRateSet_p,
																	   IEEEtypes_DataRate_t *gOpRateSet_p);
extern void hw_ResetForSta(UINT32 phymac);
extern BOOLEAN msi_wl_EnableReceiver(UINT32 macid);
extern void msi_wl_SetAckRate(UINT32 macid, UINT8 *rateset, UINT32 len);
extern BOOLEAN macMgmtMlme_UpdateRateInfoForPeerStation(UINT32 Aid,
														IEEEtypes_SuppRatesElement_t *Rates_p,
														IEEEtypes_ExtSuppRatesElement_t *ExtRates_p);
extern UINT32 hw_GetPhyRxRateIndex(UINT8 RateCode, UINT16 RxParam);
extern void    msi_wl_StaTsfUpdate(UINT32 phymac, UINT32 enable);
extern int     msi_wl_DisableMBSSAndMSTAMode(UINT32 macid);
extern void    msi_wl_ConfigBeaconUpdates( UINT32 macid, UINT32 fEnable );
extern void    msi_wl_ConfigBeaconBalanceParams( UINT32 macid );
extern void    msi_wl_ConfigMulticastRx(UINT32 macid, UINT32 fEnable);
extern int msi_wl_GetMaxAPs(UINT32 phymac);
extern WL_STATUS tx80211Q_MgmtWriteNoBlock( tx80211_MgmtMsg_t *MgmtMsg_p );
extern void RxBeaconIsr(void);
extern void KeyMgmtSta_InitSession(vmacEntry_t * vmacEntry_p);
extern void keyMgmtSta_StartSession(vmacEntry_t * vmacEntry_p);
extern UINT32 rand(void);

extern UINT8  mib_StaMode[NUM_OF_WLMACS];
/*Private Variables*/
static IEEEtypes_SuppRatesElement_t mlmeApiClientModeSupportedRates[NUM_OF_WLMACS];
static IEEEtypes_ExtSuppRatesElement_t mlmeApiClientModeExtSupportedRates[NUM_OF_WLMACS];
static BOOLEAN mlmeApiIsThisRateSupported(IEEEtypes_DataRate_t Rate, 
										  IEEEtypes_SuppRatesElement_t *peer_stn_Rates_p, 
										  IEEEtypes_SuppRatesElement_t *peer_stn_ExtRates_p);
void wep_encrypt(vmacEntry_t *vmacEntry_p, unsigned char *buf, int len);
extern void DeleteClientInfo(UINT8 *macAddr_p, vmacEntry_t *clientVMacEntry_p);

#ifdef PORT_TO_LINUX_OS
typedef struct driverNotificationMsg_t
{
	UINT8                       dirty;
	Timer 						notificationTimer;
	UINT8                       *vMacEntry_p;
	UINT8                       eventId;
	UINT32                      data1;
}driverNotificationMsg_t;

driverNotificationMsg_t linuxdrvNotificationMsg[NUM_OF_WLMACS] = {{0}};


/*************************************************************************
* Function:
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern SINT32 mlmeApi_NotificationActTimeOut(UINT8 *data)
{ 
	driverNotificationMsg_t *linuxdrvMsg_p = (driverNotificationMsg_t *)data;
	vmacEntry_t *vmacEntry_p = (vmacEntry_t *)linuxdrvMsg_p->vMacEntry_p;
	vmacStaInfo_t *vStaInfo_p = (vmacStaInfo_t *)vmacEntry_p->info_p;

	if(data && linuxdrvMsg_p->dirty)
	{
		vStaInfo_p->mlmeCallBack_fp(linuxdrvMsg_p->eventId, 
			(UINT8 *)vmacEntry_p, 
			linuxdrvMsg_p->data1);
		linuxdrvMsg_p->dirty = 0;
	}
	return 0;
}
#endif /* PORT_TO_LINUX_OS */


/*************************************************************************
* Function:
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern UINT16 mlmeApiGetMacId( vmacStaInfo_t *vStaInfo_p)
{
	vmacEntry_t  *vmacEntry_p;
	UINT16 targetMacId = 0;

	if((vmacEntry_p = (vmacEntry_t *)vStaInfo_p->vMacEntry_p) == NULL)
	{
		return TOMACID(targetMacId);
	}
	targetMacId = vmacEntry_p->macId;
	return targetMacId;
}

/*************************************************************************
* Function:
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern UINT8 mlmeApiGetMacIndex( vmacStaInfo_t *vStaInfo_p)
{
	vmacEntry_t  *vmacEntry_p;
	UINT8 targetMacId = 0;

	if((vmacEntry_p = (vmacEntry_t *)vStaInfo_p->vMacEntry_p) == NULL)
	{
		return targetMacId;
	}
	return vmacEntry_p->phyHwMacIndx;
}

/*************************************************************************
* Function:
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern BOOLEAN mlmeApiHalAccessAllow( vmacStaInfo_t *vStaInfo_p)
{   
	if(vStaInfo_p->mib_WB_p->opMode)
	{
		return TRUE;
	}
	return FALSE;
}

/*************************************************************************
* Function:
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern void mlmeApiSetBSSIDFilter(vmacStaInfo_t *vStaInfo_p, 
								  UINT8 mode)
{
	UINT16 macId;
	UINT8  macIndx;

	if(!mlmeApiHalAccessAllow(vStaInfo_p))
	{
		return;
	}

	macId = mlmeApiGetMacId(vStaInfo_p);
	macIndx = mlmeApiGetMacIndex(vStaInfo_p);

#ifdef PORT_TO_LINUX_OS

#else /* PORT_TO_LINUX_OS */
#ifdef MULTI_CAST_SUPPORT
	msi_wl_DisableBssidFilterAndEnableMsgReceipt(macIndx);
#else /* MULTI_CAST_SUPPORT */
	switch(mode)
	{
	case 0:
		msi_wl_EnableBssidFilterAndMsgReceiptAndBrdcstSsid(macIndx);
		break;

	case 1:
		msi_wl_EnableBssidFilterAndMsgReceipt(macIndx);
		break;

	default:
		break;
	}
#endif /* MULTI_CAST_SUPPORT */
#endif //* PORT_TO_LINUX_OS */
}

/*************************************************************************
* Function:
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern void mlmeApiHwSetShortSlotTime(vmacStaInfo_t *vStaInfo_p, 
									  UINT8 opSet)
{
	UINT16 macId;

	if(!mlmeApiHalAccessAllow(vStaInfo_p))
	{
		return;
	}
	macId = mlmeApiGetMacId(vStaInfo_p);
#ifdef PORT_TO_LINUX_OS

#else /* PORT_TO_LINUX_OS */
	if(opSet)
	{
		msi_wl_setDcfSlotTime(macId,0x9);
		msi_wl_setAifsTime6(macId,0x1c);
		msi_wl_setAifsTime7(macId,0x1c);
	}
	else
	{
		msi_wl_setDcfSlotTime(macId,0x14);
		msi_wl_setAifsTime6(macId,0x32);
		msi_wl_setAifsTime7(macId,0x32);
	}
#endif /* PORT_TO_LINUX_OS */
}

/*************************************************************************
* Function:
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern void mlmeApiSetAIdToMac(vmacStaInfo_t *vStaInfo_p, 
							   UINT32 AId)
{
	UINT16 macId;

	if(!mlmeApiHalAccessAllow(vStaInfo_p))
	{
		return;
	}
	macId = mlmeApiGetMacId(vStaInfo_p);
#ifdef PORT_TO_LINUX_OS

#else /* PORT_TO_LINUX_OS */
	msi_wl_SetStaAID(macId,AId);
#endif /* PORT_TO_LINUX_OS */
}

/*************************************************************************
* Function:
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern void mlmeApiSetBssidToMac(vmacStaInfo_t *vStaInfo_p,
								 UINT8 * bssid_p)
{
	IEEEtypes_MacAddr_t zero = {0};
	UINT32   valHi;
	UINT32   valLo;
	UINT16 macId;

	if(!mlmeApiHalAccessAllow(vStaInfo_p))
	{
		return;
	}
	macId = mlmeApiGetMacId(vStaInfo_p);
#ifdef PORT_TO_LINUX_OS
	memset(&zero, 0, sizeof(IEEEtypes_MacAddr_t));
	valHi = 0;
	valLo = 0;
#else /* PORT_TO_LINUX_OS */
	if(bssid_p == NULL)
	{
		msi_wl_SetBSSID(macId,(UINT8*)&zero);
	}
	else
	{
		valLo = *(bssid_p + 3);
		valLo = (valLo << 8) | *(bssid_p + 2);
		valLo = (valLo << 8) | *(bssid_p + 1);
		valLo = (valLo << 8) | *(bssid_p);
		valHi = *(bssid_p + 5);
		valHi = (valHi << 8) | *(bssid_p + 4);
		msi_wl_SetBSSID2(macId,*(UINT32 *)&valHi[0], *(UINT32 *)&valLo[0]);
	}
#endif /* PORT_TO_LINUX_OS */
}

/*************************************************************************
* Function:
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern void mlmeApiSetSsIdToMac(vmacStaInfo_t *vStaInfo_p, 
								UINT8 * ssid_p, 
								UINT8 ssid_len)
{
	UINT16 macId;

	if(!mlmeApiHalAccessAllow(vStaInfo_p))
	{
		return;
	}
	macId = mlmeApiGetMacId(vStaInfo_p);
#ifdef PORT_TO_LINUX_OS

#else /* PORT_TO_LINUX_OS */
	msi_wl_SetSSID(macId,ssid_p, ssid_len);
#endif /* PORT_TO_LINUX_OS */
}

/*************************************************************************
* Function:
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern void mlmeApiGetTimerTxTSF( vmacStaInfo_t *vStaInfo_p,
								 UINT32 *valueHi_p,
								 UINT32 *valueLo_p)
{
	UINT16 macId;
	UINT8  macIndx;

	if(!mlmeApiHalAccessAllow(vStaInfo_p))
	{
		return;
	}
	macId = mlmeApiGetMacId(vStaInfo_p);
	macIndx = mlmeApiGetMacIndex(vStaInfo_p);
#ifdef PORT_TO_LINUX_OS

#else /* PORT_TO_LINUX_OS */
	msi_wl_GetMCUCoreTimerTxTSF(macId,valueHi_p, valueLo_p);
#endif /* PORT_TO_LINUX_OS */
}

/*************************************************************************
* Function:
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern void mlmeApiPrepHwStartIBss( vmacStaInfo_t *vStaInfo_p,
								   IEEEtypes_StartCmd_t *StartCmd_p)
{
	UINT32 valueLo, valueHi;
	UINT8 nullMac[6];
	UINT16 macId;
	UINT8  macIndx;

	if(!mlmeApiHalAccessAllow(vStaInfo_p))
	{
		return;
	}
	macId = mlmeApiGetMacId(vStaInfo_p);
	macIndx = mlmeApiGetMacIndex(vStaInfo_p);
	memset (&nullMac[0], 0, 6);
#ifdef PORT_TO_LINUX_OS
	valueLo = 0;
	valueHi = 0;
#else /* PORT_TO_LINUX_OS */
	if(!memcmp(&vStaInfo_p->macMgmtMlme_ThisStaData.BssId, nullMac, sizeof(IEEEtypes_MacAddr_t)))
	{
		msi_wl_GetMCUCoreTimerTxTSF(macId,&valueHi, &valueLo);
		valueLo += rand();
		valueHi += rand();
		valueLo = ((valueLo<<8)+0x200) | 0x2;
		memcpy(&(vStaInfo_p->macMgmtMlme_ThisStaData.BssId),
			&valueLo,
			sizeof(UINT32));
		valueHi = (~valueLo+0x111)&0xFFFF;
		memcpy(&(vStaInfo_p->macMgmtMlme_ThisStaData.BssId[4]),
			&valueHi,
			sizeof(IEEEtypes_MacAddr_t) - sizeof(UINT32));
		msi_wl_SetBSSID2(macId,valueHi, valueLo);
	}  
	mlmeApiSetBSSIDFilter(vStaInfo_p, 0);
	mlmeApiSetSsIdToMac(vStaInfo_p,
		&StartCmd_p->SsId[0], 
		vStaInfo_p->macMgmtMlme_ThisStaData.BssSsIdLen);
	msi_wl_setBcnPeriod(macId, StartCmd_p->BcnPeriod, 0);
	msi_wl_setDtimPeriod(macId, StartCmd_p->DtimPeriod, 0);
	msi_wl_SetMCUCoreTimerTxTSF(macId, 0, 0);
#endif /* PORT_TO_LINUX_OS */
}

/*************************************************************************
* Function:
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern void mlmeApiPrepHwJoin(vmacStaInfo_t *vStaInfo_p,
							  IEEEtypes_JoinCmd_t *JoinCmd_p)
{
	UINT16 macId;
	UINT8  macIndx;

	if(!mlmeApiHalAccessAllow(vStaInfo_p))
	{
		return;
	}
	macId = mlmeApiGetMacId(vStaInfo_p);
	macIndx = mlmeApiGetMacIndex(vStaInfo_p);
#ifdef PORT_TO_LINUX_OS

#else /* PORT_TO_LINUX_OS */
	msi_wl_setBcnPeriod(macId, JoinCmd_p->BssDesc.BcnPeriod, 0);
	msi_wl_setDtimPeriod(macId, JoinCmd_p->BssDesc.DtimPeriod, 0);
	msi_wl_SetMCUCoreTimerTxTSF(macId, 0, 0);
#endif /* PORT_TO_LINUX_OS */
}

/*************************************************************************
* Function:
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern void mlmeApiHwSetSTAMode(vmacStaInfo_t *vStaInfo_p)
{
	UINT16 macId;

	if(!mlmeApiHalAccessAllow(vStaInfo_p))
	{
		return;
	}
	macId = mlmeApiGetMacId(vStaInfo_p);
#ifdef PORT_TO_LINUX_OS

#else /* PORT_TO_LINUX_OS */
	msi_wl_SetOpModeMCU(macId,MCU_MODE_STA_INFRA);
#endif /* PORT_TO_LINUX_OS */
}

/*************************************************************************
* Function:
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern void mlmeApiHwSetIBssMode(vmacStaInfo_t *vStaInfo_p)
{
	UINT16 macId;

	if(!mlmeApiHalAccessAllow(vStaInfo_p))
	{
		return;
	}
	macId = mlmeApiGetMacId(vStaInfo_p);
#ifdef PORT_TO_LINUX_OS

#else /* PORT_TO_LINUX_OS */
	msi_wl_SetOpModeMCU(macId,MCU_MODE_STA_ADHOC);
#endif /* PORT_TO_LINUX_OS */
}

/*************************************************************************
* Function:
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern void mlmeApiHwSetTxTimerRun(vmacStaInfo_t *vStaInfo_p,
								   UINT32 val)
{
	UINT16 macId;

	if(!mlmeApiHalAccessAllow(vStaInfo_p))
	{
		return;
	}
	macId = mlmeApiGetMacId(vStaInfo_p);
#ifdef PORT_TO_LINUX_OS

#else /* PORT_TO_LINUX_OS */
	msi_wl_SetTimerRun(macId,val);
#endif /* PORT_TO_LINUX_OS */
}

/*************************************************************************
* Function:
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern SINT32 mlmeApiSetIbssDefaultFilter( vmacStaInfo_t *vStaInfo_p,
										  BOOLEAN enable)
{
	UINT32 rxModeValue;
	UINT16 macId;
	UINT8  macIndx;

	if(!mlmeApiHalAccessAllow(vStaInfo_p))
	{
		return -1;
	}
	macId = mlmeApiGetMacId(vStaInfo_p);
	macIndx = mlmeApiGetMacIndex(vStaInfo_p);
	if(macIndx == MAC_0)
	{
		/* Due to current limitation: Don't do this for MAC_0 */
		return -1;
	}
#ifdef PORT_TO_LINUX_OS
	rxModeValue = 0;
#else /* PORT_TO_LINUX_OS */
	rxModeValue = msi_wl_GetRxModeMCU(macId);
	if(enable)
	{
		rxModeValue &= ~(RX_CORE_rx_beacon|RX_CORE_BSSIDFltMode);
		msi_wl_SetRxModeMCU(macId, (rxModeValue|RX_CORE_blk_probe_rsp|RX_CORE_adhoc_bssid_filter|RX_CORE_BrdCstSSIDEn));
	}
	else
	{
		msi_wl_SetRxModeMCU(macId, (rxModeValue|RX_CORE_rx_beacon|RX_CORE_BSSIDFltMode|RX_CORE_BrdCstSSIDEn));
	}
#endif /* PORT_TO_LINUX_OS */
	return 0;
}

/*************************************************************************
* Function:
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern void mlmeApiHwStartIBssMode(vmacStaInfo_t *vStaInfo_p)
{
	UINT16 macId;

	if(!mlmeApiHalAccessAllow(vStaInfo_p))
	{
		return;
	}
	macId = mlmeApiGetMacId(vStaInfo_p);
#ifdef PORT_TO_LINUX_OS

#else /* PORT_TO_LINUX_OS */
	msi_wl_SetOpModeMCU(macId,MCU_MODE_STA_ADHOC);
	msi_wl_DisableMBSSAndMSTAMode(macId);
	msi_wl_StaTsfUpdate(PHYMAC(macId), 1);
	msi_wl_ConfigBeaconUpdates( macId, 0 );
	msi_wl_ConfigBeaconBalanceParams( macId );
	msi_wl_ConfigMulticastRx(macId, 1);
	msi_wl_EnableAdHocBSS(macId);
	msi_wl_timerCommitAll(macId);
	msi_wl_SetTimerRun(macId,1);
#endif /* PORT_TO_LINUX_OS */
	mlmeApiSetIbssDefaultFilter(vStaInfo_p, TRUE);
}

/*************************************************************************
* Function:
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern void mlmeApiResetTimerSync(vmacStaInfo_t *vStaInfo_p)
{
	static UINT32 TsfLoInit;
	static UINT32 TsfHiInit;
	UINT16 macId;

	if(!mlmeApiHalAccessAllow(vStaInfo_p))
	{
		return;
	}
	macId = mlmeApiGetMacId(vStaInfo_p);
#ifdef PORT_TO_LINUX_OS
	TsfLoInit = 0;
	TsfHiInit = 0;
#else /* PORT_TO_LINUX_OS */
	msi_wl_SetMCUCoreTimerTxTSF(macId, TsfHiInit, TsfLoInit);
#endif /* PORT_TO_LINUX_OS */
}

/*************************************************************************
* Function:
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern void mlmeApiNotifyNextBcnTime(vmacStaInfo_t *vStaInfo_p,
									 UINT64   nextBcnTime)
{
	UINT32   lowHalf;
	UINT32   hiHalf;
	UINT16 macId;

	if(!mlmeApiHalAccessAllow(vStaInfo_p))
	{
		return;
	}
	macId = mlmeApiGetMacId(vStaInfo_p);
	memcpy(&lowHalf, &nextBcnTime, sizeof(UINT32));
	nextBcnTime = nextBcnTime >> 32;
	memcpy(&hiHalf, &nextBcnTime, sizeof(UINT32));
#ifdef PORT_TO_LINUX_OS
	lowHalf = 0;
	hiHalf = 0;
#else /* PORT_TO_LINUX_OS */
	msi_wl_SetMCUCoreTimerAccIntvl(macId,hiHalf, lowHalf);
#endif /* PORT_TO_LINUX_OS */
}

/*************************************************************************
* Function:
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern void mlmeApiSetTimerSync(vmacStaInfo_t *vStaInfo_p,
								UINT32 val_lo, 
								UINT32 val_hi)
{
	UINT16 macId;

	if(!mlmeApiHalAccessAllow(vStaInfo_p))
	{
		return;
	}
	macId = mlmeApiGetMacId(vStaInfo_p);
#ifdef PORT_TO_LINUX_OS

#else /* PORT_TO_LINUX_OS */
	msi_wl_SetMCUCoreTimerTxTSF(macId, val_hi, val_lo);
#endif /* PORT_TO_LINUX_OS */
}


/*************************************************************************
* Function: mlmeApiDisconnect
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern void mlmeApiDisconnect (vmacStaInfo_t *vStaInfo_p)
{
	UINT16 macId;
	UINT8  macIndx;

	if(!mlmeApiHalAccessAllow(vStaInfo_p))
	{
		return;
	}
	macId = mlmeApiGetMacId(vStaInfo_p);
	macIndx = mlmeApiGetMacIndex(vStaInfo_p);
#ifdef PORT_TO_LINUX_OS

#else /* PORT_TO_LINUX_OS */
	msi_wl_DisableBssidFilterAndEnableMsgReceipt(macIndx);
#endif /* PORT_TO_LINUX_OS */
	mlmeApiResetTimerSync(vStaInfo_p);
}

/*************************************************************************
* Function:
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern void mlmeApiPrepHwToScan(vmacStaInfo_t *vStaInfo_p)
{
	UINT16 macId;
	UINT8  macIndx;

	if(!mlmeApiHalAccessAllow(vStaInfo_p))
	{
		return;
	}
	macId = mlmeApiGetMacId(vStaInfo_p);
	macIndx = mlmeApiGetMacIndex(vStaInfo_p);
#ifdef PORT_TO_LINUX_OS

#else /* PORT_TO_LINUX_OS */
	msi_wl_SetTimerRun(macId,0);
	msi_wl_DisableBssidFilterAndEnableMsgReceipt(macIndx);
	mlmeApiResetTimerSync(vStaInfo_p);
	msi_wl_SetOpModeMCU(macId,MCU_MODE_STA_INFRA);
#endif /* PORT_TO_LINUX_OS */
}

/*************************************************************************
* Function:
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern void mlmeApiSetRfChannel(vmacStaInfo_t *vStaInfo_p,
								UINT8 channel, UINT8 initRateTable, BOOLEAN scanning)
{
#ifdef PORT_TO_LINUX_OS
	vmacEntry_t  *vMacEntry_p = (vmacEntry_t *)vStaInfo_p->vMacEntry_p;
	struct net_device *clientDev_p;
	struct wlprivate	*priv_p;
	BOOLEAN result;
	UINT32 lchan;
	MIB_PHY_DSSS_TABLE *PhyDSSSTable;
	CHNL_FLAGS chanflag;
	UINT8 nullAddr[6] = {0,0,0,0,0,0};		

	clientDev_p = mainNetdev_p[vMacEntry_p->phyHwMacIndx];
	if(clientDev_p == NULL)
	{
		return;
	}
	priv_p = NETDEV_PRIV_P(struct wlprivate, clientDev_p);
	lchan = channel;
	PhyDSSSTable=priv_p->vmacSta_p->Mib802dot11->PhyDSSSTable;
	Disable_extStaDb_ProcessKeepAliveTimer(priv_p->vmacSta_p);
	Disable_MonitorTimerProcess(priv_p->vmacSta_p);

	wlUpdateAutoChan(priv_p->vmacSta_p, lchan,0); 
#ifndef CLIENTONLY
	wlUpdateAutoChan(priv_p->vmacSta_p, lchan,1);
#endif

	chanflag = PhyDSSSTable->Chanflag;

	/* Channel 165 currently only supports 20 MHz BW.*/
	if (scanning || (lchan == 165))
	{
		chanflag.ChnlWidth = CH_20_MHz_WIDTH;
		chanflag.ExtChnlOffset = NO_EXT_CHANNEL;
	}
    else /* Only when joining set 20/40 MHz Channel width according to AP's BSS descriptor */
    {
		/*When Bssid is 0, there is no reason to update ChnlWidth*/
		/*After RestorePreScanParameters, there is no bssDescProfile yet and if we don't have this condition check, */
		/*we could update ChnlWidth wrongly*/
		if(memcmp(vStaInfo_p->bssDescProfile_p->BssId,nullAddr,6)!=0)
		{
			chanflag.ExtChnlOffset = NO_EXT_CHANNEL;		
			chanflag.ChnlWidth = CH_20_MHz_WIDTH;			
			
			if((*(priv_p->vmacSta_p->Mib802dot11->mib_ApMode) == AP_MODE_N_ONLY)
				|| (*(priv_p->vmacSta_p->Mib802dot11->mib_ApMode) == AP_MODE_BandN)
				|| (*(priv_p->vmacSta_p->Mib802dot11->mib_ApMode) == AP_MODE_GandN)
				|| (*(priv_p->vmacSta_p->Mib802dot11->mib_ApMode) == AP_MODE_BandGandN)
				|| (*(priv_p->vmacSta_p->Mib802dot11->mib_ApMode) == AP_MODE_5GHZ_N_ONLY)
#ifdef SOC_W8864
				|| (*(priv_p->vmacSta_p->Mib802dot11->mib_ApMode) == AP_MODE_2_4GHZ_11AC_MIXED)
				|| (*(priv_p->vmacSta_p->Mib802dot11->mib_ApMode) == AP_MODE_5GHZ_Nand11AC)
				|| (*(priv_p->vmacSta_p->Mib802dot11->mib_ApMode) == AP_MODE_5GHZ_11AC_ONLY)
#endif
				|| (*(priv_p->vmacSta_p->Mib802dot11->mib_ApMode) == AP_MODE_AandN)
			)
			{
				if (vStaInfo_p->bssDescProfile_p->ADDHTElement.Len)
					chanflag.ExtChnlOffset = vStaInfo_p->bssDescProfile_p->ADDHTElement.AddChan.ExtChanOffset;
				else
					chanflag.ExtChnlOffset = NO_EXT_CHANNEL;
			
			
				if (vStaInfo_p->bssDescProfile_p->HTElement.HTCapabilitiesInfo.SupChanWidth)
            		chanflag.ChnlWidth = CH_40_MHz_WIDTH;
        		else
            		chanflag.ChnlWidth = CH_20_MHz_WIDTH;
			}
#ifdef SOC_W8864
			if(*(priv_p->vmacSta_p->Mib802dot11->mib_ApMode) >= AP_MODE_5GHZ_11AC_ONLY)
			{	
        		if(vStaInfo_p->bssDescProfile_p->VHTOp.ch_width == 1){
					chanflag.ChnlWidth = CH_80_MHz_WIDTH;
        		}
				else {
					if (vStaInfo_p->bssDescProfile_p->HTElement.HTCapabilitiesInfo.SupChanWidth)
            			chanflag.ChnlWidth = CH_40_MHz_WIDTH;
        			else
            			chanflag.ChnlWidth = CH_20_MHz_WIDTH;
				}
        		
			}	
#endif
			PhyDSSSTable->Chanflag.ChnlWidth = chanflag.ChnlWidth;
			PhyDSSSTable->Chanflag.ExtChnlOffset = chanflag.ExtChnlOffset;		
		}		
    }


	result = wlchannelSet(priv_p->vmacSta_p->dev, lchan, chanflag, initRateTable);
	/* FIX: need to set txpower table again after setting channel becasue f/w will reset power table when setting channel */
	wlFwSetTxPower(priv_p->master?priv_p->master:clientDev_p, HostCmd_ACT_GEN_SET_LIST, 0);

	/* Setup the basic rates */
	/* Set once when moving from 5 Ghz to 2.5 Ghz */
	/* Change rates only if fixed rate is not enabled */
	if (*(priv_p->vmacSta_p->Mib802dot11->mib_enableFixedRateTx) == 0)
	{
		if (lchan < 14)
		{
			*(priv_p->vmacSta_p->Mib802dot11->mib_ManagementRate) = 2;
			*(priv_p->vmacSta_p->Mib802dot11->mib_MulticastRate) = 2;
			*(priv_p->vmacSta_p->Mib802dot11->mib_MultiRateTxType) = 0;
			wlFwSetRate(clientDev_p, 0);   
		}
		else
		{
			*(priv_p->vmacSta_p->Mib802dot11->mib_ManagementRate) = 12;
			*(priv_p->vmacSta_p->Mib802dot11->mib_MulticastRate) = 12;
			*(priv_p->vmacSta_p->Mib802dot11->mib_MultiRateTxType) = 0;
			wlFwSetRate(clientDev_p, 0);   
		}
	}

	extStaDb_ProcessKeepAliveTimerInit(priv_p->vmacSta_p);
	MonitorTimerInit(priv_p->vmacSta_p);
#ifdef ETH_DEBUG
	eprintf("**** Finished changing channel with result = %d\n", result);
#endif /* ETH_DEBUG */

#ifdef TEST_SCAN_FLOW
	curChannel = channel;
#endif /* TEST_SCAN_FLOW */

#else /* PORT_TO_LINUX_OS */
	msi_wl_SetRFChan(channel);
#endif /* PORT_TO_LINUX_OS */
}

/*************************************************************************
* Function:
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern UINT8 mlmeApiGetRfChannel(vmacStaInfo_t *vStaInfo_p)
{
#ifdef PORT_TO_LINUX_OS
	vmacEntry_t  *vMacEntry_p = (vmacEntry_t *)vStaInfo_p->vMacEntry_p;
	struct net_device *clientDev_p;
	struct wlprivate	*priv_p;
	MIB_PHY_DSSS_TABLE *PhyDSSSTable;

	clientDev_p = mainNetdev_p[vMacEntry_p->phyHwMacIndx];
	if(clientDev_p == NULL)
	{
		return 0xff;
	}
	priv_p = NETDEV_PRIV_P(struct wlprivate, clientDev_p);

	PhyDSSSTable=priv_p->vmacSta_p->Mib802dot11->PhyDSSSTable;

	return (PhyDSSSTable->CurrChan);
#else /* PORT_TO_LINUX_OS */
	return ((UINT8)msi_wl_GetRFChan());
#endif /* PORT_TO_LINUX_OS */
}

/*************************************************************************
* Function:
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern void mlmePrepDefaultMgtMsg_Sta( vmacStaInfo_t *vStaInfo_p,
									  dot11MgtFrame_t *mgtFrame_p, 
									  IEEEtypes_MacAddr_t *DestAddr, 
									  UINT32 Subtype,
									  IEEEtypes_MacAddr_t *BssId_p )
{
	vmacEntry_t  *vmacEntry_p = (vmacEntry_t	*)vStaInfo_p->vMacEntry_p;

	mgtFrame_p->Hdr.FrmCtl.ProtocolVersion   = IEEEtypes_PROTOCOL_VERSION;
	mgtFrame_p->Hdr.FrmCtl.Type              = IEEE_TYPE_MANAGEMENT;
	mgtFrame_p->Hdr.FrmCtl.Subtype           = Subtype;
	mgtFrame_p->Hdr.FrmCtl.ToDs              = 0;
	mgtFrame_p->Hdr.FrmCtl.FromDs            = 0;
	mgtFrame_p->Hdr.FrmCtl.MoreFrag          = 0;
	mgtFrame_p->Hdr.FrmCtl.Retry             = 0;
#ifdef PORT_TO_LINUX_OS
	mgtFrame_p->Hdr.FrmCtl.PwrMgmt           = 0;
#else
	mgtFrame_p->Hdr.FrmCtl.PwrMgmt           = vStaInfo_p->staSystemMibs.mib_StaCfg_p->PwrMgtMode;
#endif /* PORT_TO_LINUX_OS */
	mgtFrame_p->Hdr.FrmCtl.MoreData          = 0;
	mgtFrame_p->Hdr.FrmCtl.Wep               = 0;
	mgtFrame_p->Hdr.FrmCtl.Order             = 0;
	mgtFrame_p->Hdr.Duration                 = 0;
	mgtFrame_p->Hdr.SeqCtl                   = 0;
	memcpy(&mgtFrame_p->Hdr.DestAddr,
		DestAddr,
		sizeof(IEEEtypes_MacAddr_t));
	memcpy(&mgtFrame_p->Hdr.SrcAddr,
		&vmacEntry_p->vmacAddr[0],
		sizeof(IEEEtypes_MacAddr_t));
	memcpy(&mgtFrame_p->Hdr.BssId,
		BssId_p,
		sizeof(IEEEtypes_MacAddr_t));
}

/*************************************************************************
* Function:
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern dot11MgtFrame_t *mlmeApiAllocMgtMsg(UINT8 phymac)
{
	tx80211_MgmtMsg_t *txFrame_p;
	dot11MgtFrame_t   *MgmtMsg_p;
	UINT8 *frm;
#ifdef PORT_TO_LINUX_OS
	struct sk_buff *skb_p;

	txFrame_p = NULL;
	MgmtMsg_p = NULL;

	if ((skb_p = ieee80211_getmgtframe(&frm, MGT_FRAME_BUF_SIZE+SK_BUF_RESERVED_PAD)) == NULL)
	{
		return NULL;
	}

#ifdef ETH_DEBUG
	eprintf("mlmeApiAllocMgtMsg:: skbuf = %x  -->data = %x\n", skb_p, skb_p->data);
#endif /* ETH_DEBUG */

	MgmtMsg_p = (dot11MgtFrame_t *)skb_p->data;
	MgmtMsg_p->priv_p = (void *)skb_p;

#ifdef ETH_DEBUG
	eprintf("mlmeApiAllocMgtMsg:: MgmtMsg_p = %x -->priv_p =  %x\n", MgmtMsg_p, MgmtMsg_p->priv_p);
#endif /* ETH_DEBUG */

#else /* PORT_TO_LINUX_OS */
	if ((txFrame_p = (tx80211_MgmtMsg_t *)pool_GetBuf(txMgmtPoolIdPerMac[phymac])) != NULL)
	{
		MgmtMsg_p = (dot11MgtFrame_t *)&txFrame_p->MgmtFrame;
		MgmtMsg_p->Hdr.FrmCtl.Type = IEEE_TYPE_MANAGEMENT;
		MgmtMsg_p->Hdr.FrmCtl.Retry=0;
		MgmtMsg_p->Hdr.Duration = 300;
	}
	else
	{
		MgmtMsg_p = NULL;
	}
#endif /* PORT_TO_LINUX_OS */
	return MgmtMsg_p;
}

/*************************************************************************
* Function:
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern BOOLEAN mlmeApiSendSmeMsg( macmgmtQ_CmdReq_t *smeCmd_p )
{
	vmacEntry_t  *targetVMacEntry_p;

#ifdef PORT_TO_LINUX_OS

#ifdef ETH_DEBUG
	eprintf("mlmeApiSendSmeMsg:: allocBuf_p = %x\n", smeCmd_p);
#endif /* ETH_DEBUG */

	if((targetVMacEntry_p = vmacGetVMacEntryByAddr((UINT8 *)&smeCmd_p->targetAddr)) == NULL)
	{
		return FALSE;
	}
	targetVMacEntry_p->mlmeMsgEvt((UINT8 *)smeCmd_p, 
		NULL, 
		targetVMacEntry_p->info_p);
#else /* PORT_TO_LINUX_OS */
	if(macMgmtQ_SmeWriteNoBlock( smeCmd_p ) == OS_FAIL)
	{
		return FALSE;
	}
#endif /* def PORT_TO_LINUX_OS */
	return TRUE;
}

/*************************************************************************
* Function:
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern macmgmtQ_CmdBuf_t* mlmeApiAllocSmeMsg( void )
{
	macmgmtQ_CmdBuf_t* allocBuf_p = NULL;

#ifdef PORT_TO_LINUX_OS
	allocBuf_p = &smeCmdBuf_q[smeCmdBufCnt%SME_CMD_BUF_Q_LIMIT];
	smeCmdBufCnt++;

#ifdef ETH_DEBUG
	eprintf("mlmeApiAllocSmeMsg:: allocBuf_p = %x\n", allocBuf_p);
#endif /* ETH_DEBUG */

#else /* PORT_TO_LINUX_OS */
	allocBuf_p = (macmgmtQ_CmdBuf_t *)pool_GetBuf(smeCmdPoolId);
#endif /* def PORT_TO_LINUX_OS */
	return allocBuf_p; 
}


/*************************************************************************
* Function:
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern void mlmeApiFreeMgtMsg(dot11MgtFrame_t *mgtFrame_p)
{
#ifdef PORT_TO_LINUX_OS
	struct sk_buff *skb_p;

#ifdef ETH_DEBUG
	eprintf("mlmeApiFreeMgtMsg:: MgmtMsg_p = %x  priv_p = %x \n", mgtFrame_p, mgtFrame_p->priv_p);
#endif /* ETH_DEBUG */

	skb_p = (struct sk_buff *)mgtFrame_p->priv_p;

#ifdef ETH_DEBUG
	eprintf("mlmeApiFreeMgtMsg:: skb_p = %x \n", skb_p);
#endif /* ETH_DEBUG */

	dev_kfree_skb_any(skb_p);

#else /* def PORT_TO_LINUX_OS */
	pool_FreeBuf((UINT8 *)mgtFrame_p);
#endif /* PORT_TO_LINUX_OS */
}

/*************************************************************************
* Function:
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern void mlmeApiFreeSmeMsg(macmgmtQ_CmdBuf_t *cmdMsg)
{
	memset((void *)&cmdMsg->Cmd.cmdReq.targetAddr[0], 
		0, 
		sizeof(IEEEtypes_MacAddr_t));
#ifdef PORT_TO_LINUX_OS

#else /* PORT_TO_LINUX_OS */
	pool_FreeBuf((UINT8 *)cmdMsg);
#endif /* PORT_TO_LINUX_OS */
}

/*************************************************************************
* Function:
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern SINT32 mlmeApiSendMgtMsg_Sta(vmacStaInfo_t *vStaInfo_p,
									dot11MgtFrame_t *mgtFrame_p,
									UINT8 *pRxSign)
{
	UINT8       macIndx;
	UINT8       rate;
	UINT8       preamble;
	//RxSign_t    *pRxInfo;
	vmacEntry_t *vMacEntry_p = (vmacEntry_t *)vStaInfo_p->vMacEntry_p;

	macIndx = mlmeApiGetMacIndex(vStaInfo_p);
#ifdef PORT_TO_LINUX_OS
	{
		struct sk_buff *skb_p;
		UINT32  adjust_len;

#ifdef ETH_DEBUG
		eprintf("mlmeApiSendMgtMsg_Sta:: MgmtMsg_p = %x  priv_p = %x \n", mgtFrame_p, mgtFrame_p->priv_p);
#endif /* ETH_DEBUG */

		rate = 0;
		preamble = 0;
		skb_p = (struct sk_buff *)mgtFrame_p->priv_p;

#ifdef ETH_DEBUG
		eprintf("mlmeApiSendMgtMsg_Sta:: skb_p = %x \n", skb_p);
		eprintf("mlmeApiSendMgtMsg_Sta:: MgmtMsg_p->len = %x  skb_p->len = %x \n", mgtFrame_p->Hdr.FrmBodyLen, skb_p->len);
#endif /* ETH_DEBUG */

		adjust_len = sizeof(struct ieee80211_frame) + SK_BUF_RESERVED_PAD + mgtFrame_p->Hdr.FrmBodyLen;
		skb_trim(skb_p, adjust_len);

#ifdef ETH_DEBUG
		eprintf("mlmeApiSendMgtMsg_Sta:: MgmtMsg_p->len_2 = %x  skb_p->len_2 = %x \n", mgtFrame_p->Hdr.FrmBodyLen, skb_p->len);
#endif /* ETH_DEBUG */

		if(skb_pull(skb_p, SK_BUF_RESERVED_PAD) == NULL)
		{
#ifdef ETH_DEBUG
			eprintf("mlmeApiSendMgtMsg_Sta:: BIG ERROR\n");
#endif /* ETH_DEBUG */
			return MLME_FAILURE;
		}

		if (txMgmtMsg(mainNetdev_p[vMacEntry_p->phyHwMacIndx], skb_p) != OS_SUCCESS )
		{
			dev_kfree_skb_any(skb_p);
			return MLME_FAILURE;
		}
	}

#else /* PORT_TO_LINUX_OS */
	if( pRxInfo )
	{
		rate = hw_GetPhyRxRateIndex(pRxInfo->Rate, pRxInfo->RxParam);
		preamble = pRxInfo->AntPream & 0x1;

		txFrame_p->RateAndPreamble = (rate & 0x7F);
		txFrame_p->RateAndPreamble |= (preamble<<7);
	}
	else
	{
		txFrame_p->RateAndPreamble = 0xFF;
	}
	if (tx80211Q_MgmtWriteNoBlock(txFrame_p) == OS_FAIL)
	{
#ifdef ETH_DEBUG
		eprintf("mlmeApiSendMgtMsg_Sta:: fail to tx mgt frame\n");
#endif /* ETH_DEBUG */
		mlmeApiFreeMgtMsg(mgtFrame_p);
		return MLME_FAILURE;
	}
#endif /* PORT_TO_LINUX_OS */
	return MLME_SUCCESS;
}

/*************************************************************************
* Function:
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern SINT32 mlmeApiSndScanNotificationOnly(vmacStaInfo_t *vStaInfo_p,
											 UINT16 scanResult,
											 UINT8 numSet,
											 UINT16 bufSize,
											 UINT8  *BssDescSet_p)
{
	macmgmtQ_CmdRsp_t *smeMsg_p;
	vmacEntry_t  *vmacEntry_p;
	unsigned char           *scanCfrm_p;
	IEEEtypes_ScanResult_t  *scanResult_p;

	if((vmacEntry_p = (vmacEntry_t *)vStaInfo_p->vMacEntry_p) == NULL)
	{
		return MLME_FAILURE;
	}

	if ( (smeMsg_p = (macmgmtQ_CmdRsp_t *)mlmeApiAllocSmeMsg()) == NULL )
	{
		return MLME_FAILURE;
	}
	memset((void *)smeMsg_p, 0, sizeof(macmgmtQ_CmdRsp_t));
	memcpy((void *)&smeMsg_p->targetAddr[0], 
		(const void *)&vmacEntry_p->vmacAddr[0],
		sizeof(IEEEtypes_MacAddr_t));
	smeMsg_p->MsgType = MlmeScan_Cnfm;
	smeMsg_p->Msg.ScanCfrm.BufSize  = bufSize;
	smeMsg_p->Msg.ScanCfrm.Rsp.BssDescSet.NumSets = numSet;
	smeMsg_p->Msg.ScanCfrm.Rsp.Result   = scanResult;
	if(bufSize)
	{
		memcpy(&(smeMsg_p->Msg.ScanCfrm.Rsp.BssDescSet.BssDesc),
			BssDescSet_p,
			bufSize);

		scanCfrm_p = (unsigned char *)
			&(smeMsg_p->Msg.ScanCfrm.Rsp.BssDescSet.BssDesc) +
			bufSize;

		scanResult_p  = (IEEEtypes_ScanResult_t *)scanCfrm_p;
		*scanResult_p = SCAN_RESULT_SUCCESS;
	}
	else
	{
		smeMsg_p->Msg.ScanCfrm.Rsp.Result   = scanResult;
	}
	if (!mlmeApiSendSmeMsg((macmgmtQ_CmdReq_t *)smeMsg_p))
	{
		/* free buffer */
		mlmeApiFreeSmeMsg((macmgmtQ_CmdBuf_t *)smeMsg_p);
		return MLME_FAILURE;
	}
	return MLME_SUCCESS;
}

/*************************************************************************
* Function:
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
UINT32 gDebug_SmeQueWriteFail = 0;
UINT32 gDebug_SmeAllocFail    = 0;

extern SINT32 mlmeApiSndNotification(vmacStaInfo_t *vStaInfo_p,
									 UINT8 *evtMsg_p, 
									 UINT8 mlmeEvt)
{
	macmgmtQ_CmdRsp_t *smeMsg_p;
	vmacEntry_t  *vmacEntry_p;

	if((vmacEntry_p = (vmacEntry_t *)vStaInfo_p->vMacEntry_p) == NULL)
	{
		return MLME_FAILURE;
	}

	if ( (smeMsg_p = (macmgmtQ_CmdRsp_t *)mlmeApiAllocSmeMsg()) == NULL )
	{
		gDebug_SmeAllocFail ++;
		return MLME_FAILURE;
	}
	memcpy((void *)&smeMsg_p->targetAddr[0], 
		(const void *)&vmacEntry_p->vmacAddr[0],
		sizeof(IEEEtypes_MacAddr_t));

	switch(mlmeEvt)
	{
	case MlmeAuth_Cnfm:
		{
			smeMsg_p->MsgType = MlmeAuth_Cnfm;
			memcpy(&smeMsg_p->Msg.AuthCfrm, evtMsg_p, sizeof(IEEEtypes_AuthCfrm_t));
		}
		break;

	case MlmeAuth_Ind:
		{
			smeMsg_p->MsgType              = MlmeAuth_Ind;
			memcpy(&smeMsg_p->Msg.AuthInd,
				evtMsg_p,
				sizeof(IEEEtypes_AuthCfrm_t));
		}
		break;

	case MlmeDeAuth_Ind:
		{
			smeMsg_p->MsgType              = MlmeDeAuth_Ind;
			memcpy(&smeMsg_p->Msg.AuthInd,
				evtMsg_p,
				sizeof(IEEEtypes_AuthCfrm_t));  
		}
		break;

	case MlmeDeAuth_Cnfm:
		{
			smeMsg_p->MsgType               = MlmeDeAuth_Cnfm;
			memcpy(&smeMsg_p->Msg.DeauthCfrm,
				evtMsg_p,
				sizeof(IEEEtypes_DeauthCfrm_t));

		}
		break;

	case MlmeAssoc_Cnfm:
		{
			smeMsg_p->MsgType        = MlmeAssoc_Cnfm;
			memcpy(&smeMsg_p->Msg.AssocCfrm,
				evtMsg_p,
				sizeof(IEEEtypes_AssocCfrm_t));
		}
		break;

	case MlmeAssoc_Ind:
		{
			smeMsg_p->MsgType = MlmeAssoc_Ind;
			memcpy(&smeMsg_p->Msg.AssocInd,
				evtMsg_p,
				sizeof(IEEEtypes_AuthCfrm_t));
		}
		break;

	case MlmeReAssoc_Cnfm:
		{
			smeMsg_p->MsgType = MlmeReAssoc_Cnfm;
			memcpy(&smeMsg_p->Msg.ReassocCfrm,
				evtMsg_p,
				sizeof(IEEEtypes_ReassocCfrm_t));
		}
		break;

	case MlmeReAssoc_Ind:
		{
			smeMsg_p->MsgType              = MlmeReAssoc_Ind;
			memcpy(&smeMsg_p->Msg.ReassocInd,
				evtMsg_p,
				sizeof(IEEEtypes_ReassocInd_t));
		}
		break;

	case MlmeDisAssoc_Cnfm:
		{
			smeMsg_p->MsgType = MlmeDisAssoc_Cnfm;

			memcpy(&smeMsg_p->Msg.DisassocCfrm,
				evtMsg_p,
				sizeof(IEEEtypes_ReassocCfrm_t));
		}
		break;

	case MlmeDisAssoc_Ind:
		{
			smeMsg_p->MsgType              = MlmeDisAssoc_Ind;
			memcpy(&smeMsg_p->Msg.DisassocInd,
				evtMsg_p,
				sizeof(IEEEtypes_DisassocInd_t));
		}
		break;

	case MlmeScan_Cnfm:
		{
			//memset(smeMsg_p, 0, sizeof(macmgmtQ_CmdRsp_t));

			smeMsg_p->MsgType = MlmeScan_Cnfm;

			memcpy(&smeMsg_p->Msg.ScanCfrm,
				evtMsg_p,
				sizeof(smeQ_ScanRsp_t));
		}
		break;

	case MlmeJoin_Cnfm:
		{
			smeMsg_p->MsgType = MlmeJoin_Cnfm;
			memcpy(&smeMsg_p->Msg.JoinCfrm,
				evtMsg_p,
				sizeof(IEEEtypes_JoinCfrm_t));
		}
		break;

	case MlmeStart_Cnfm:
		{
			smeMsg_p->MsgType = MlmeStart_Cnfm;
			memcpy(&smeMsg_p->Msg.StartCfrm,
				evtMsg_p,
				sizeof(IEEEtypes_StartCfrm_t));
		}
		break;

	case MlmeReset_Cnfm:
		{
			smeMsg_p->MsgType                = MlmeReset_Cnfm;

			memcpy(&smeMsg_p->Msg.ResetCfrm,
				evtMsg_p,
				sizeof(IEEEtypes_ResetCfrm_t));
		}
		break;

	case Tbcn:
		smeMsg_p->MsgType = Tbcn;
		break;

	default:
		/* free buffer */
		mlmeApiFreeSmeMsg((macmgmtQ_CmdBuf_t *)smeMsg_p);
		return MLME_FAILURE;
	}

#ifdef ETH_DEBUG
	eprintf("mlmeApiSndNotification :: smeMsg_p event %d\n", smeMsg_p->MsgType);
	eprintf("mlmeApiSndNotification :: macmgmtQ_CmdReq_t event %d\n", ((macmgmtQ_CmdReq_t *)smeMsg_p)->CmdType);
#endif /* #ifdef ETH_DEBUG */

	if (!mlmeApiSendSmeMsg((macmgmtQ_CmdReq_t *)smeMsg_p))
	{
		gDebug_SmeQueWriteFail ++;
		/* free buffer */
		/* pool_FreeBuf((UINT8 *)smeMsg_p); */
		mlmeApiFreeSmeMsg((macmgmtQ_CmdBuf_t *)smeMsg_p);
		return MLME_FAILURE;
	}

	return MLME_SUCCESS;
}

/*************************************************************************
* Function:
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern SINT32 mlmeApiWepEncrypt(vmacStaInfo_t *vStaInfo_p, UINT8 * outBuf, UINT8 *data, SINT32 dataLen)
{

	UINT8 tempBuf[16];

	memcpy(tempBuf, outBuf, 16);
	memcpy(outBuf + 4, tempBuf, 16);
	memcpy(outBuf + 4 + 8, data, dataLen);
	// SW encryption.
	wep_encrypt((vmacEntry_t *) vStaInfo_p->vMacEntry_p, outBuf, dataLen + 8);
	return 0;
}

/* Perform WEP encryption on given buffer. Buffer needs to has 4 bytes of
* extra space (IV) in the beginning, then len bytes of data, and finally
* 4 bytes of extra space (ICV). Both IV and ICV will be transmitted, so the
* payload length increases with 8 bytes.
*
* WEP frame payload: IV + TX key idx, RC4(data), ICV = RC4(CRC32(data))
*/

UINT32 IV = 0xaa;
UINT32 KeyId = 0;
UINT8 klen = 0;
UINT8 key[16];
UINT8 IVReset = 0;
UINT8 WS[256]  = {0};
unsigned long crc32_table[256] = {
	0x00000000L, 0x77073096L, 0xee0e612cL, 0x990951baL, 0x076dc419L,
	0x706af48fL, 0xe963a535L, 0x9e6495a3L, 0x0edb8832L, 0x79dcb8a4L,
	0xe0d5e91eL, 0x97d2d988L, 0x09b64c2bL, 0x7eb17cbdL, 0xe7b82d07L,
	0x90bf1d91L, 0x1db71064L, 0x6ab020f2L, 0xf3b97148L, 0x84be41deL,
	0x1adad47dL, 0x6ddde4ebL, 0xf4d4b551L, 0x83d385c7L, 0x136c9856L,
	0x646ba8c0L, 0xfd62f97aL, 0x8a65c9ecL, 0x14015c4fL, 0x63066cd9L,
	0xfa0f3d63L, 0x8d080df5L, 0x3b6e20c8L, 0x4c69105eL, 0xd56041e4L,
	0xa2677172L, 0x3c03e4d1L, 0x4b04d447L, 0xd20d85fdL, 0xa50ab56bL,
	0x35b5a8faL, 0x42b2986cL, 0xdbbbc9d6L, 0xacbcf940L, 0x32d86ce3L,
	0x45df5c75L, 0xdcd60dcfL, 0xabd13d59L, 0x26d930acL, 0x51de003aL,
	0xc8d75180L, 0xbfd06116L, 0x21b4f4b5L, 0x56b3c423L, 0xcfba9599L,
	0xb8bda50fL, 0x2802b89eL, 0x5f058808L, 0xc60cd9b2L, 0xb10be924L,
	0x2f6f7c87L, 0x58684c11L, 0xc1611dabL, 0xb6662d3dL, 0x76dc4190L,
	0x01db7106L, 0x98d220bcL, 0xefd5102aL, 0x71b18589L, 0x06b6b51fL,
	0x9fbfe4a5L, 0xe8b8d433L, 0x7807c9a2L, 0x0f00f934L, 0x9609a88eL,
	0xe10e9818L, 0x7f6a0dbbL, 0x086d3d2dL, 0x91646c97L, 0xe6635c01L,
	0x6b6b51f4L, 0x1c6c6162L, 0x856530d8L, 0xf262004eL, 0x6c0695edL,
	0x1b01a57bL, 0x8208f4c1L, 0xf50fc457L, 0x65b0d9c6L, 0x12b7e950L,
	0x8bbeb8eaL, 0xfcb9887cL, 0x62dd1ddfL, 0x15da2d49L, 0x8cd37cf3L,
	0xfbd44c65L, 0x4db26158L, 0x3ab551ceL, 0xa3bc0074L, 0xd4bb30e2L,
	0x4adfa541L, 0x3dd895d7L, 0xa4d1c46dL, 0xd3d6f4fbL, 0x4369e96aL,
	0x346ed9fcL, 0xad678846L, 0xda60b8d0L, 0x44042d73L, 0x33031de5L,
	0xaa0a4c5fL, 0xdd0d7cc9L, 0x5005713cL, 0x270241aaL, 0xbe0b1010L,
	0xc90c2086L, 0x5768b525L, 0x206f85b3L, 0xb966d409L, 0xce61e49fL,
	0x5edef90eL, 0x29d9c998L, 0xb0d09822L, 0xc7d7a8b4L, 0x59b33d17L,
	0x2eb40d81L, 0xb7bd5c3bL, 0xc0ba6cadL, 0xedb88320L, 0x9abfb3b6L,
	0x03b6e20cL, 0x74b1d29aL, 0xead54739L, 0x9dd277afL, 0x04db2615L,
	0x73dc1683L, 0xe3630b12L, 0x94643b84L, 0x0d6d6a3eL, 0x7a6a5aa8L,
	0xe40ecf0bL, 0x9309ff9dL, 0x0a00ae27L, 0x7d079eb1L, 0xf00f9344L,
	0x8708a3d2L, 0x1e01f268L, 0x6906c2feL, 0xf762575dL, 0x806567cbL,
	0x196c3671L, 0x6e6b06e7L, 0xfed41b76L, 0x89d32be0L, 0x10da7a5aL,
	0x67dd4accL, 0xf9b9df6fL, 0x8ebeeff9L, 0x17b7be43L, 0x60b08ed5L,
	0xd6d6a3e8L, 0xa1d1937eL, 0x38d8c2c4L, 0x4fdff252L, 0xd1bb67f1L,
	0xa6bc5767L, 0x3fb506ddL, 0x48b2364bL, 0xd80d2bdaL, 0xaf0a1b4cL,
	0x36034af6L, 0x41047a60L, 0xdf60efc3L, 0xa867df55L, 0x316e8eefL,
	0x4669be79L, 0xcb61b38cL, 0xbc66831aL, 0x256fd2a0L, 0x5268e236L,
	0xcc0c7795L, 0xbb0b4703L, 0x220216b9L, 0x5505262fL, 0xc5ba3bbeL,
	0xb2bd0b28L, 0x2bb45a92L, 0x5cb36a04L, 0xc2d7ffa7L, 0xb5d0cf31L,
	0x2cd99e8bL, 0x5bdeae1dL, 0x9b64c2b0L, 0xec63f226L, 0x756aa39cL,
	0x026d930aL, 0x9c0906a9L, 0xeb0e363fL, 0x72076785L, 0x05005713L,
	0x95bf4a82L, 0xe2b87a14L, 0x7bb12baeL, 0x0cb61b38L, 0x92d28e9bL,
	0xe5d5be0dL, 0x7cdcefb7L, 0x0bdbdf21L, 0x86d3d2d4L, 0xf1d4e242L,
	0x68ddb3f8L, 0x1fda836eL, 0x81be16cdL, 0xf6b9265bL, 0x6fb077e1L,
	0x18b74777L, 0x88085ae6L, 0xff0f6a70L, 0x66063bcaL, 0x11010b5cL,
	0x8f659effL, 0xf862ae69L, 0x616bffd3L, 0x166ccf45L, 0xa00ae278L,
	0xd70dd2eeL, 0x4e048354L, 0x3903b3c2L, 0xa7672661L, 0xd06016f7L,
	0x4969474dL, 0x3e6e77dbL, 0xaed16a4aL, 0xd9d65adcL, 0x40df0b66L,
	0x37d83bf0L, 0xa9bcae53L, 0xdebb9ec5L, 0x47b2cf7fL, 0x30b5ffe9L,
	0xbdbdf21cL, 0xcabac28aL, 0x53b39330L, 0x24b4a3a6L, 0xbad03605L,
	0xcdd70693L, 0x54de5729L, 0x23d967bfL, 0xb3667a2eL, 0xc4614ab8L,
	0x5d681b02L, 0x2a6f2b94L, 0xb40bbe37L, 0xc30c8ea1L, 0x5a05df1bL,
	0x2d02ef8dL
};


void wep_encrypt(vmacEntry_t *vmacEntry_p, unsigned char *buf, int len)
{

	unsigned long i, j, k, crc;
	unsigned char kpos, *pos;
	struct net_device *pStaDev   = (struct net_device *) vmacEntry_p->privInfo_p;
	struct wlprivate  *wlpptrSta = NETDEV_PRIV_P(struct wlprivate, pStaDev);
	vmacApInfo_t *vmacSta_p = wlpptrSta->vmacSta_p;
	MIB_802DOT11 *mib = vmacSta_p->ShadowMib802dot11;
	UINT32 WepType = mib->WepDefaultKeys[*(mib->mib_defaultkeyindex)].WepType;

	IV++;

	/* Prepend 24-bit IV to RC4 key and TX frame */
	pos = buf;
	*pos++ = key[0] = (IV >> 16) & 0xff;
	*pos++ = key[1] = (IV >> 8) & 0xff;
	*pos++ = key[2] = IV & 0xff;
	//*pos++ = mib_defaultkeyindex << 6;
	*pos++ = *(mib->mib_defaultkeyindex) << 6;

	if (WepType == 1) //40 bit
		klen = 3 + 5;
	else if (WepType == 2)
		klen = 3 + 13; //128 bit


	for (i = 0;i < klen - 3;i++)
		key[3 + i] = mib->WepDefaultKeys[*(mib->mib_defaultkeyindex)].WepDefaultKeyValue[i];
	//key[3 + i] = mib_WepDefaultKeys_p[mib_defaultkeyindex].WepDefaultKeyValue[i];
	IVReset = 1;


	/* Setup RC4 state */
	{


		for (i = 0; i < 256; i++)
			WS[i] = i;
		j = 0;
		kpos = 0;
		for (i = 0; i < 256; i++)
		{
			j = (j + WS[i] + key[kpos]) & 0xff;
			kpos++;
			if (kpos >= klen)
				kpos = 0;
			WS_SWAP(i, j);
		}

	}


	/* Compute CRC32 over unencrypted data and apply RC4 to data */
	crc = ~0;
	i = j = 0;
	for (k = 0; k < len; k++)
	{
		crc = crc32_table[(crc ^ *pos) & 0xff] ^ (crc >> 8);
		i = (i + 1) & 0xff;
		j = (j + WS[i]) & 0xff;
		WS_SWAP(i, j);
		*pos++ ^= WS[(WS[i] + WS[j]) & 0xff];
	}
	crc = ~crc;

	/* Append little-endian CRC32 and encrypt it to produce ICV */
	pos[0] = crc;
	pos[1] = crc >> 8;
	pos[2] = crc >> 16;
	pos[3] = crc >> 24;
	for (k = 0; k < 4; k++)
	{
		i = (i + 1) & 0xff;
		j = (j + WS[i]) & 0xff;
		WS_SWAP(i, j);
		*pos++ ^= WS[(WS[i] + WS[j]) & 0xff];
	}



}


#ifndef PORT_TO_LINUX_OS
/******************************************************************************
*
* Name: util_ListLen
*
* Description:
*    Routine to determine the length of a list of bytes that is represented
*    by an array where an entry of 0 indicates the end of the list if the
*    list does not contain the maximum number of entries.
*
* Conditions For Use:
*    None.
*
* Arguments:
*    Arg1 (i  ): List_p  - Pointer to the list of interest
*    Arg2 (i  ): MaxSize - The maximum allowed length of the list
*
* Return Value:
*    The length of the list.
*
* Notes:
*    The length is computed by assuming that as soon as there is a zero
*    in the array, that indicates the end of the data in the array.
*
* PDL:
*    Initialize an index that keeps track of the array location to zero
*    While the value at the index location in the array is not zero
*    and the value of the index is not greater than MaxSize
*       Increment to the index in the array
*       If the index is less than MaxSize Then
*          Increment the pointer to the array
*       End If
*    End while
*    Return the index
* END PDL
*
*****************************************************************************/
extern UINT8 util_ListLen(UINT8 *List_p, UINT32 MaxSize)
{
	UINT32  i = 0;

	while (i < MaxSize  &&  (*List_p) != 0)
	{
		i++;
		if (i < MaxSize)
		{
			List_p++;
		}
	}

	return i;

} // End util_ListLen()
#endif /* PORT_TO_LINUX_OS */


/*************************************************************************
* Function:
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern void mlmeApiEventNotification(vmacStaInfo_t *vStaInfo_p,
									 UINT32 eventInvoked,
									 UINT8 *peerMacAddr, 
									 UINT32 reasonCode)
{
	UINT8         phyIndexFromStaInfo;
	vmacEntry_t * vmacEntry_p = (vmacEntry_t *)(vStaInfo_p->vMacEntry_p);

#ifdef PORT_TO_LINUX_OS
	driverNotificationMsg_t *linuxdrvMsg_p = &linuxdrvNotificationMsg[vmacEntry_p->phyHwMacIndx];
#endif

	phyIndexFromStaInfo = vmacEntry_p->phyHwMacIndx;

#ifdef PORT_TO_LINUX_OS
	if(vStaInfo_p->mlmeCallBack_fp == NULL)
	{
		return;
	}
	switch(eventInvoked)
	{
	case MlmeScan_Cnfm:
		{
			linuxdrvMsg_p->dirty = 1;
			linuxdrvMsg_p->vMacEntry_p = (UINT8 *)vmacEntry_p;
			linuxdrvMsg_p->eventId = MlmeScan_Cnfm;
			linuxdrvMsg_p->data1 = reasonCode;
			mlmeApiStartTimer((vmacStaInfo_t *)linuxdrvMsg_p, 
				(UINT8 *)&linuxdrvMsg_p->notificationTimer,
				&mlmeApi_NotificationActTimeOut,
				1);
		}
		break;
    case MlmeReset_Cnfm:
        {
			vStaInfo_p->mlmeCallBack_fp(MlmeReset_Cnfm, 
				(UINT8 *)vmacEntry_p, 
				0);
        }
        break;
	default:
		break;
	}
#else /* PORT_TO_LINUX_OS */
	switch(eventInvoked)
	{
	case MlmeDisAssoc_Req:
		{
			/* Notify of DisAssoc Send */
			EVTBUF_STAEVT_ASSOC_SUBTYPE_DISASSOC_SEND evtBufAssoc;

			memset((void *)&evtBufAssoc, 0, sizeof(EVTBUF_STAEVT_ASSOC_SUBTYPE_DISASSOC_SEND));
			evtBufAssoc.phyIndex = phyIndexFromStaInfo;
			evtBufAssoc.bssIndex = 0;
			evtBufAssoc.reasonCode = reasonCode;
			memcpy((void *)&evtBufAssoc.apMacAddr[0],
				(const void *)peerMacAddr,
				sizeof(IEEEtypes_MacAddr_t));
			memcpy((void *)&evtBufAssoc.staMacAddr[0],
				vmacEntry_p->vmacAddr,
				sizeof(IEEEtypes_MacAddr_t));
			eventGenerate(STAEVT_ASSOC_WITH_AP, 
				STAEVT_ASSOC_SUBTYPE_DISASSOC_SEND, 
				sizeof(EVTBUF_STAEVT_ASSOC_SUBTYPE_DISASSOC_SEND), 
				(UINT8 *)&evtBufAssoc);
		}
		break;

	case MlmeDisAssoc_Ind:
		{
			/* Notify of DisAssoc Received */
			EVTBUF_STAEVT_ASSOC_SUBTYPE_DISASSOC_RECV evtBufAssoc;

			memset((void *)&evtBufAssoc, 0, sizeof(EVTBUF_STAEVT_ASSOC_SUBTYPE_DISASSOC_RECV));
			evtBufAssoc.phyIndex = phyIndexFromStaInfo;
			evtBufAssoc.bssIndex = 0;
			evtBufAssoc.reasonCode = reasonCode;
			memcpy((void *)&evtBufAssoc.apMacAddr[0],
				(const void *)peerMacAddr,
				sizeof(IEEEtypes_MacAddr_t));
			memcpy((void *)&evtBufAssoc.staMacAddr[0],
				vmacEntry_p->vmacAddr,
				sizeof(IEEEtypes_MacAddr_t));
			eventGenerate(STAEVT_ASSOC_WITH_AP, 
				STAEVT_ASSOC_SUBTYPE_DISASSOC_RECV, 
				sizeof(EVTBUF_STAEVT_ASSOC_SUBTYPE_DISASSOC_RECV), 
				(UINT8 *)&evtBufAssoc);
		}
		break;

	case MlmeDeAuth_Req:
		{
			/* Notify of DeAuth Send */
			EVTBUF_STAEVT_AUTH_SUBTYPE_DEAUTH_SEND evtBufAuth;

			memset((void *)&evtBufAuth, 0, sizeof(EVTBUF_STAEVT_AUTH_SUBTYPE_DEAUTH_SEND));
			evtBufAuth.phyIndex = phyIndexFromStaInfo;
			evtBufAuth.bssIndex = 0;
			evtBufAuth.reasonCode = reasonCode;
			memcpy((void *)&evtBufAuth.apMacAddr[0],
				(const void *)peerMacAddr,
				sizeof(IEEEtypes_MacAddr_t));
			memcpy((void *)&evtBufAuth.staMacAddr[0],
				vmacEntry_p->vmacAddr,
				sizeof(IEEEtypes_MacAddr_t));
			eventGenerate(STAEVT_AUTH_WITH_AP, 
				STAEVT_AUTH_SUBTYPE_DEAUTH_SEND, 
				sizeof(EVTBUF_STAEVT_AUTH_SUBTYPE_DEAUTH_SEND), 
				(UINT8 *)&evtBufAuth);
		}
		break;

	case MlmeDeAuth_Ind:
		{
			/* Notify of DeAuth Received */
			EVTBUF_STAEVT_AUTH_SUBTYPE_DEAUTH_RECV evtBufAuth;

			memset((void *)&evtBufAuth, 0, sizeof(EVTBUF_STAEVT_AUTH_SUBTYPE_DEAUTH_SEND));
			evtBufAuth.phyIndex = phyIndexFromStaInfo;
			evtBufAuth.bssIndex = 0;
			evtBufAuth.reasonCode = reasonCode;
			memcpy((void *)&evtBufAuth.apMacAddr[0],
				(const void *)peerMacAddr,
				sizeof(IEEEtypes_MacAddr_t));
			memcpy((void *)&evtBufAuth.staMacAddr[0],
				vmacEntry_p->vmacAddr,
				sizeof(IEEEtypes_MacAddr_t));
			eventGenerate(STAEVT_AUTH_WITH_AP, 
				STAEVT_AUTH_SUBTYPE_DEAUTH_RECV, 
				sizeof(EVTBUF_STAEVT_AUTH_SUBTYPE_DEAUTH_RECV), 
				(UINT8 *)&evtBufAuth);

		}
		break;

	case Tbcn:
		{
			/* Notify of Link lost with AP */
			EVTBUF_EVT_CLIENT_SUBTYPE_DISCONNECT evtBufLink;

			memset((void *)&evtBufLink, 0, sizeof(EVTBUF_EVT_CLIENT_SUBTYPE_DISCONNECT));
			evtBufLink.phyIndex = phyIndexFromStaInfo;
			eventGenerate(EVT_CLIENT, 
				EVT_CLIENT_SUBTYPE_DISCONNECT, 
				sizeof(EVTBUF_EVT_CLIENT_SUBTYPE_DISCONNECT), 
				(UINT8 *)&evtBufLink);
		}
		break;

	case MlmeScan_Cnfm:
		{
			/* Notify of Scan Completion */
			EVTBUF_EVT_CLIENT_SUBTYPE_SCAN_COMPLETE evtScanCompleted;

			memset((void *)&evtScanCompleted, 0, sizeof(EVTBUF_EVT_CLIENT_SUBTYPE_SCAN_COMPLETE));
			evtScanCompleted.phyIndex = phyIndexFromStaInfo;
			eventGenerate(EVT_CLIENT, 
				EVT_CLIENT_SUBTYPE_SCAN_COMPLETE, 
				sizeof(EVTBUF_EVT_CLIENT_SUBTYPE_SCAN_COMPLETE), 
				(UINT8 *)&evtScanCompleted);
		}
		break;
	case PeerDetectInCompat:
		{
			/* Notify of Incompatible Peer Detected */
			EVTBUF_EVT_CLIENT_SUBTYPE_INCOMPATIBLE_PEER_DETECTED evtIncompatiblePeerDetected;

			memset((void *)&evtIncompatiblePeerDetected, 0, sizeof(EVTBUF_EVT_CLIENT_SUBTYPE_INCOMPATIBLE_PEER_DETECTED));
			evtIncompatiblePeerDetected.phyIndex = phyIndexFromStaInfo;
			memcpy((void *)&evtIncompatiblePeerDetected.bssId[0],
				(const void *)peerMacAddr,
				sizeof(IEEEtypes_MacAddr_t));
			eventGenerate(EVT_CLIENT, 
				EVT_CLIENT_SUBTYPE_INCOMPATIBLE_PEER_DETECTED, 
				sizeof(EVTBUF_EVT_CLIENT_SUBTYPE_INCOMPATIBLE_PEER_DETECTED), 
				(UINT8 *)&evtIncompatiblePeerDetected);

		}
		break;

	case MlmeAssoc_Cnfm:
	case MlmeAuth_Cnfm:
		{
			EVTBUF_EVT_CLIENT_SUBTYPE_JOIN evtJoin;
			/* generate event */
			evtJoin.phyIndex    = phyIndexFromStaInfo;
			evtJoin.bssIndex    = 0;
			evtJoin.procType = 0;
			evtJoin.assocId = 0;
			evtJoin.joinStatus = reasonCode;
			if(eventInvoked==MlmeAssoc_Cnfm)
			{
				evtJoin.procType = 1;
				evtJoin.assocId = vStaInfo_p->aId;
			}
			memcpy(evtJoin.clientMACAddr, vmacEntry_p->vmacAddr, IEEEtypes_ADDRESS_SIZE);
			memcpy(evtJoin.apMACAddr, peerMacAddr, IEEEtypes_ADDRESS_SIZE);
			eventGenerate(EVT_CLIENT, EVT_CLIENT_SUBTYPE_JOIN, sizeof(evtJoin), (UINT8 *)&evtJoin);
		}
		break;

	case MlmeStart_Cnfm:
		{
			EVTBUF_EVT_CLIENT_SUBTYPE_IBSS_STARTED evtIbssStarted;
			memset((void *)&evtIbssStarted, 0, sizeof(EVTBUF_EVT_CLIENT_SUBTYPE_IBSS_STARTED));
			evtIbssStarted.phyIndex = mlmeApiGetMacIndex(vStaInfo_p);
			eventGenerate(EVT_CLIENT, 
				EVT_CLIENT_SUBTYPE_IBSS_STARTED, 
				sizeof(EVTBUF_EVT_CLIENT_SUBTYPE_IBSS_STARTED), 
				(UINT8 *)&evtIbssStarted);
		}
		break;

	default:
		break;
	}
#endif /* PORT_TO_LINUX_OS */
}

/** To be clean up later ftang **/
#define RX_CORE_ProModeEn (1)
#define RX_CORE_BSSIDFltMode (1<<1)
#define RX_CORE_BrdCstSSIDEn (1<<2)
#define RX_CORE_RxEn (1<<3)
#define RX_CORE_IgnoreFCS (1<<4)
#define RX_CORE_DnfBufOnly (1<<5)
#define RX_CORE_all_mcast (1<<6)
#define RX_CORE_all_ucast (1<<7)
#define RX_CORE_no_fwd_done (1<<8)
#define RX_CORE_no_defrag (1<<9)
#define RX_CORE_reserved (1<<10)
#define RX_CORE_sta_addr4 (1<<11) 
#define RX_CORE_rx_beacon (1<<12)

#define RX_CORE_blk_probe_rsp (1<<14)
#define RX_CORE_adhoc_bssid_filter (1<<21)

extern BOOLEAN WlanEnabled[2];
extern UINT8 mib_urModeConfig[NUM_OF_WLMACS];
//extern MIB_PHY_DSSS_TABLE *PhyDSSSTable_p;


/*************************************************************************
* Function:
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern void mlmeApiStaMacReset( UINT8 phyMacIndx, 
							   vmacStaInfo_t *vStaInfo_p )
{
	vmacEntry_t  *vmacEntry_p;
	UINT32 hwMacId;
	UINT8 macStaAddr[IEEEtypes_ADDRESS_SIZE];

	vmacEntry_p = (vmacEntry_t *)vStaInfo_p->vMacEntry_p;
#ifdef PORT_TO_LINUX_OS
	hwMacId = TOMACID(vmacEntry_p->phyHwMacIndx);
	memcpy((void *)macStaAddr, &vmacEntry_p->vmacAddr[0], IEEEtypes_ADDRESS_SIZE);
#else /* PORT_TO_LINUX_OS */
	msi_wl_SetOpModeMCU(TOMACID(phyMacIndx),MCU_MODE_AP);
	if(!mlmeApiHalAccessAllow(vStaInfo_p))
	{
		if(vStaInfo_p->isParentSession)
		{
			msi_wl_SetMacAddrByMacId(vmacEntry_p->phyHwMacIndx, 
				vmacEntry_p ->macId,
				&vmacEntry_p->vmacAddr[0]);
		}
		return;
	}
	vmacEntry_p ->macId = TOMACID(vmacEntry_p->phyHwMacIndx);

	if((vmacEntry_p = (vmacEntry_t *)vStaInfo_p->vMacEntry_p) == NULL)
	{
		return;
	}

	hwMacId = TOMACID(vmacEntry_p->phyHwMacIndx);
	/* Reset the HW MAC */
	hw_ResetForSta(vmacEntry_p->phyHwMacIndx);
	ReCreateTxMgmtPool(vmacEntry_p->phyHwMacIndx);
	msi_wl_SetMCUCoreTimerTxTSF(hwMacId, 0, 0);
	memcpy((void *)macStaAddr, &vmacEntry_p->vmacAddr[0], IEEEtypes_ADDRESS_SIZE);
	msi_wl_SetMACAddr(hwMacId,macStaAddr);
	msi_wl_SetOpModeMCU(hwMacId,MCU_MODE_STA_INFRA);
	/* Enable Receiver , Enable Broadcast SSID */
	/* Disable h/w defragmentation in Rx direction */
	msi_wl_SetRxModeMCU(hwMacId,RX_CORE_RxEn|RX_CORE_no_fwd_done|RX_CORE_rx_beacon|RX_CORE_no_defrag);
	msi_wl_timerCommitAll(hwMacId);
	msi_wl_SetTimerRun(hwMacId,0x1);
	msi_wl_EnableReceiver(hwMacId);

	/* BT_LOOK:: Need a hal api to set this */
	WL_MAC_WRITE_WORD(phyMacIndx, 0x8000a58c, 0x18000);/** hardcode for now to turnoff bluetooth check **/

	/* BT_LOOK:: AP code reference to this so set it for now */    
	mib_urModeConfig[MAC_0] = 1;
	/* BT_LOOK:: this is specific to MAC_0 for now */
	WlanEnabled[phyMacIndx] = TRUE;
#endif /* def PORT_TO_LINUX_OS */
	return ;
}


/*************************************************************************
* Function:
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern UINT32 mlmeApiSetStaMode(vmacStaInfo_t *vStaInfo_p, UINT8 Mode)
{
	vStaInfo_p->macMgt_StaMode = Mode;
	return 0;       

}


/*************************************************************************
* Function:
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern UINT32 mlmeApiIsValidChannel(UINT8 band, UINT8 channel)
{
	switch(band)
	{
	case BAND_B:
		if(channel < 0x1 || channel > 0xb)
		{
			return FALSE;
		}
		break;

	default:
		return FALSE;
	}
	return TRUE;       
}

/*************************************************************************
* Function:
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern UINT32 mlmeApiSetControlRates(vmacStaInfo_t *vStaInfo_p)
{
	UINT16 macId;
	UINT8 rateBuf[MAX_ALL_RATE_LEN];
	vmacEntry_t  *vmacEntry_p;

	UINT32 rateBuf_len = 0;
	UINT32 curLen = 0;

	//if(!mlmeApiHalAccessAllow(vStaInfo_p))
	//{
	//	return;
	//}
	vmacEntry_p = (vmacEntry_t *)vStaInfo_p->vMacEntry_p;

	macId = mlmeApiGetMacId(vStaInfo_p);
	memset((void *)&rateBuf[0], 0, MAX_ALL_RATE_LEN);

	/* Copy over 11B rates */
	curLen = util_ListLen(vStaInfo_p->bOpRateSet, MAX_B_DATA_RATES);
	if(curLen)
	{
		memcpy((void *)&rateBuf[rateBuf_len], 
			(void const *)&vStaInfo_p->bOpRateSet[0],
			curLen);
		rateBuf_len += curLen;
	}
	/* Copy over 11G rates */
	curLen = util_ListLen(vStaInfo_p->gOpRateSet, MAX_G_DATA_RATES);
	if(curLen)
	{
		memcpy((void *)&rateBuf[rateBuf_len], 
			(void const *)&vStaInfo_p->gOpRateSet[0],
			curLen);
		rateBuf_len += curLen;
	}


#ifdef PORT_TO_LINUX_OS

#else /* PORT_TO_LINUX_OS */
	msi_wl_SetAckRate(macId, &rateBuf[0], rateBuf_len);
#endif /* PORT_TO_LINUX_OS */
	return 0;
}
/***********************************************************
*this function will check if the following rate is supported in either
*the SupportedRates or teh ExtendedSupportedRates data structure
**************************************/ 
static BOOLEAN
mlmeApiIsThisRateSupported(IEEEtypes_DataRate_t Rate, 
						   IEEEtypes_SuppRatesElement_t *peer_stn_Rates_p, 
						   IEEEtypes_SuppRatesElement_t *peer_stn_ExtRates_p)
{
	UINT32 i;
	if (peer_stn_Rates_p != NULL) 
	{
		for (i = 0 ; i < peer_stn_Rates_p->Len; i++)
		{
			if ((peer_stn_Rates_p->Rates[i] & IEEEtypes_BASIC_RATE_MASK) == 
				(Rate & IEEEtypes_BASIC_RATE_MASK))
			{
				/*the rate has been found.*/
				return TRUE;
			}
		}
	}
	/*the rate has not been found in the supported Rates.*/
	/*so try the Extended Rates*/
	if (peer_stn_ExtRates_p != NULL) 
	{
		for (i = 0 ; i < peer_stn_ExtRates_p->Len; i++)
		{
			if ((peer_stn_ExtRates_p->Rates[i] & IEEEtypes_BASIC_RATE_MASK) == 
				(Rate & IEEEtypes_BASIC_RATE_MASK))
			{
				/*the rate has been found.*/
				return TRUE;
			}
		}
	}
	/*the rate has not been found.*/
	return FALSE;
}

/*the supported rates should be a intersection of the rates supported by us*/
/*and the rates supported by the peer station*/
/*this function gets that intersection*/
extern BOOLEAN
mlmeApiGetCombinedSupportedRates(IEEEtypes_SuppRatesElement_t *my_stn_Rates_p, 
								 IEEEtypes_ExtSuppRatesElement_t *my_stn_ExtRates_p,
								 IEEEtypes_ExtSuppRatesElement_t *peer_stn_Rates_p, 
								 IEEEtypes_ExtSuppRatesElement_t *peer_stn_ExtRates_p,
								 IEEEtypes_ExtSuppRatesElement_t *combined_stn_Rates_p, 
								 IEEEtypes_ExtSuppRatesElement_t *combined_stn_ExtRates_p)
{
	UINT32 i, IE_len;


	/*for each rate that we support, we will search if the peer station also*/
	/*supports that rate. If it does, we will add that rate in the combined rates set*/

	/*get the intersection of the supported Rates first*/
	IE_len = 0;
	combined_stn_Rates_p->ElementId = SUPPORTED_RATES;
	for (i = 0 ; i < my_stn_Rates_p->Len; i++)
	{
		if (mlmeApiIsThisRateSupported(my_stn_Rates_p->Rates[i], 
			(IEEEtypes_SuppRatesElement_t *)peer_stn_Rates_p, 
			(IEEEtypes_SuppRatesElement_t *)peer_stn_ExtRates_p) ==
			TRUE)
		{
			combined_stn_Rates_p->Rates[IE_len] = my_stn_Rates_p->Rates[i];
			IE_len ++;
		}
	}
	combined_stn_Rates_p->Len = IE_len;

	/*now get the intersection of the extended supported Rates*/
	IE_len = 0;
	combined_stn_ExtRates_p->ElementId = EXT_SUPPORTED_RATES;
	for (i = 0 ; i < my_stn_ExtRates_p->Len; i++)
	{
		if (mlmeApiIsThisRateSupported(my_stn_ExtRates_p->Rates[i], 
			(IEEEtypes_SuppRatesElement_t *)peer_stn_Rates_p, 
			(IEEEtypes_SuppRatesElement_t *)peer_stn_ExtRates_p) ==
			TRUE)
		{
			combined_stn_ExtRates_p->Rates[IE_len] = my_stn_ExtRates_p->Rates[i];
			IE_len ++;
		}
	}
	combined_stn_ExtRates_p->Len = IE_len;
	return TRUE;
}
/*********************************************
* This function will create a IEEE defined Supported Rate and 
* Extended Supported Rate Element from the bOpRateSet_p and 
* gOpRateSet_p
*********************************************/
extern BOOLEAN
mlmeApiCreateSupportedRateElement(IEEEtypes_SuppRatesElement_t *Rates_p,
								  IEEEtypes_ExtSuppRatesElement_t *ExtRates_p,
								  IEEEtypes_DataRate_t *bOpRateSet_p,
								  IEEEtypes_DataRate_t *gOpRateSet_p,
								  phyMacId_t phyHwMacIndx)
{
	UINT32 i, curLen;

	/* Copy over 11B rates */
	curLen = util_ListLen(bOpRateSet_p, MAX_B_DATA_RATES);
	Rates_p->ElementId = SUPPORTED_RATES;
	Rates_p->Len = curLen;
	/*Store this information in the Supported Rates element*/
	for (i = 0 ; i < curLen; i++)
	{
		Rates_p->Rates[i] = bOpRateSet_p[i];
	}

	/* Copy over 11G rates 
	* Ensure that the client is not in B only mode*/
	if (mib_StaMode[phyHwMacIndx] != CLIENT_MODE_B) 
	{
		curLen = util_ListLen(gOpRateSet_p, MAX_G_DATA_RATES);
		ExtRates_p->ElementId = EXT_SUPPORTED_RATES;
		ExtRates_p->Len = curLen;
		/*Store this information in the Extended Supported Rates element*/
		for (i = 0 ; i < curLen; i++)
		{
			ExtRates_p->Rates[i] = gOpRateSet_p[i];
		}
	}
	else
	{
		ExtRates_p->ElementId = EXT_SUPPORTED_RATES;
		ExtRates_p->Len = 0;
	}

	return TRUE;
}

/*************************************************************************
* Function:
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern BOOLEAN
mlmeApiCreatePeerStationInfoForWBMode(IEEEtypes_MacAddr_t *staMacAddr,
									  IEEEtypes_SuppRatesElement_t *Rates_p,
									  IEEEtypes_ExtSuppRatesElement_t *ExtRates_p,
									  phyMacId_t			phyHwMacIndx)
{
	extStaDb_StaInfo_t *StaInfo = NULL;	
	extStaDb_StaInfo_t *StaInfo_p;
	IEEEtypes_SuppRatesElement_t combined_stn_Rates;
	IEEEtypes_ExtSuppRatesElement_t combined_stn_ExtRates;

	/*Use dynamic memory to prevent frame size > 1024bytes warning during compilation
	* extStaDb_StaInfo_t takes 1488bytes
	*/
	if((StaInfo = kmalloc(sizeof(extStaDb_StaInfo_t) ,GFP_KERNEL)) == NULL){
		printk("%s: Fail to allocate memory\n", __FUNCTION__);	
		return FALSE;
	}
	
#ifdef PORT_TO_LINUX_OS
	memset(StaInfo, 0, sizeof(extStaDb_StaInfo_t));		
	StaInfo_p = NULL;
	memset(&combined_stn_Rates, 0, sizeof(IEEEtypes_SuppRatesElement_t));
	memset(&combined_stn_ExtRates, 0, sizeof(IEEEtypes_ExtSuppRatesElement_t));
#else /* PORT_TO_LINUX_OS */
	/* Station not in Stn table, hence add */
	memset(StaInfo, 0, sizeof(extStaDb_StaInfo_t));		
	memcpy(&StaInfo->Addr, staMacAddr, sizeof(IEEEtypes_MacAddr_t));
	StaInfo->StnId = 0;
	StaInfo->Aid = 0;
	StaInfo->AP = FALSE; 
#ifdef APCFGUR
	StaInfo->UR = 1;
#endif
#ifdef STA_INFO_DB
	StaInfo->Sq1 = 0;
	StaInfo->Sq2 = 0;
	StaInfo->RSSI= 0;
	StaInfo->Rate= 0;
#endif

	StaInfo->vApInfo_p = NULL;
	StaInfo->phymac = phyHwMacIndx;

	/*Doing this check because the phyHwMacIndx has been found to be corrupted*/
	if (phyHwMacIndx > (NUM_OF_WLMACS - 1)) 
	{
		kfree(StaInfo);		
		return FALSE;
	}

	if (extStaDb_AddSta(StaInfo) != ADD_SUCCESS)
	{
		kfree(StaInfo);		
		return FALSE;
	}
	/*now get a handle to this newly created station*/
	StaInfo_p = extStaDb_GetStaInfo(staMacAddr, 1);
	if ((StaInfo_p->Aid = AssignAid(phyHwMacIndx)) == 0)
	{
		kfree(StaInfo);		
		return FALSE;
	}


	/*the supported rates should be a intersection of the rates supported by us*/
	/*and the rates supported by the peer station*/
	mlmeApiGetCombinedSupportedRates(&(mlmeApiClientModeSupportedRates[phyHwMacIndx]), 
		&(mlmeApiClientModeExtSupportedRates[phyHwMacIndx]),
		(IEEEtypes_ExtSuppRatesElement_t *)Rates_p, 
		ExtRates_p,
		(IEEEtypes_ExtSuppRatesElement_t *)&combined_stn_Rates, 
		&combined_stn_ExtRates);

	/*This function will update the Rate Information in the Assoc Table Database for that*/
	/*peer station that the AP/WB is going to transmit packets to*/
	macMgmtMlme_UpdateRateInfoForPeerStation(StaInfo_p->Aid,
		&combined_stn_Rates,
		&combined_stn_ExtRates);
#endif /* def PORT_TO_LINUX_OS */
	kfree(StaInfo);		
	return TRUE;
}

/*************************************************************************
* Function:  
*
* Description: This function will get the index into the station db
*   
*
* Input:
*
* Output:
*
**************************************************************************/
extern BOOLEAN
mlmeApiGetPeerStationStaInfoAndAid(IEEEtypes_MacAddr_t *staMacAddr_p,
								   extStaDb_StaInfo_t **StaInfo_pp,
								   UINT32 *Aid_p)
{

#ifdef PORT_TO_LINUX_OS
	return FALSE;
#else /* PORT_TO_LINUX_OS */
	*StaInfo_pp = extStaDb_GetStaInfo(staMacAddr_p, 0);
	if (*StaInfo_pp != NULL) 
	{
		/*the peer station info exists. Get the Aid.*/
		*Aid_p = (*StaInfo_pp)->Aid;
		return TRUE;
	}
	else
	{
		/*we need to first create a peer station info entry in */
		/*the station database first*/
		return FALSE;
	}
#endif /* PORT_TO_LINUX_OS */
}


extern BOOLEAN
mlmeApiFreePeerStationStaInfoAndAid(IEEEtypes_MacAddr_t *staMacAddr_p, vmacEntry_t *clientVMacEntry_p)
{
	extStaDb_StaInfo_t *StaInfo_p;

#ifdef PORT_TO_LINUX_OS
	StaInfo_p = NULL;
    RemoveClientFw((UINT8 *) staMacAddr_p, clientVMacEntry_p);
	return FALSE;
#else /* PORT_TO_LINUX_OS */
	if ( (StaInfo_p = extStaDb_GetStaInfo(staMacAddr_p, 0/*STADB_NO_BLOCK*/))
		== NULL )
	{
		/*This sta entry is not found*/
		return FALSE;
	}
	/*now delete that station from the peer station database*/
	if (extStaDb_DelSta(staMacAddr_p, 0) != DEL_SUCCESS)
	{
		/*could not delete this station*/
		return FALSE;
	}
	/*Free the Station ID from the Station ID list*/
	FreeStnId(StaInfo_p->StnId);
	/*ResetAid() sends an event stating that the Aid has been freed*/
	ResetAid(StaInfo_p->StnId, StaInfo_p->Aid);
	return TRUE;
#endif /* PORT_TO_LINUX_OS */
}

extern BOOLEAN
mlmeApiSetPeerStationStateForWB(extStaDb_StaInfo_t *StaInfo_p,
								extStaDb_State_e state)
{
	StaInfo_p->State = state;
	return TRUE;
}

/*************************************************************************
* Function:
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
void mlmeApiUpdateLinkStatus(vmacStaInfo_t *vStaInfo_p, UINT8 linkId)
{
	vmacEntry_t  *vmacEntry_p;
	UINT8 		 vMacAddr[6];

	vmacEntry_p = (vmacEntry_t *)vStaInfo_p->vMacEntry_p;
	memcpy(&vMacAddr[0], &vmacEntry_p->vmacAddr[0], 6);
#ifdef PORT_TO_LINUX_OS
	if(vStaInfo_p->mlmeCallBack_fp != NULL)
	{
		{
			vStaInfo_p->mlmeCallBack_fp(MmgtIndicationSignals, 
				(UINT8 *)vmacEntry_p, 
				linkId);
		}
	}
#else /* PORT_TO_LINUX_OS */
	if(vStaInfo_p->mlmeCallBack_fp != NULL)
	{
		vStaInfo_p->mlmeCallBack_fp(vmacEntry_p->phyHwMacIndx, 
			&vMacAddr[0], 
			linkId);
	}
#endif /* def PORT_TO_LINUX_OS */
}

/*************************************************************************
* Function: mlmeApiGetClientModeSupportedRates
*
* Description: This function will return a pointer to the Set of Supported Rates element
*			   that our client for teh MAC supports. 
* Input: Index of the MAC.
*
* Output:
*
**************************************************************************/
IEEEtypes_SuppRatesElement_t *mlmeApiGetClientModeSupportedRates(UINT32 phyHwMacIndx)
{
	return &(mlmeApiClientModeSupportedRates[phyHwMacIndx]);
}

/*************************************************************************
* Function: mlmeApiGetClientModeExtSupportedRates
*
* Description: This function will return a pointer to the Set of Extended Supported Rates element
*			   that our client for the MAC supports. 
* Input: Index of the MAC.
*
* Output:
*
**************************************************************************/
IEEEtypes_ExtSuppRatesElement_t *mlmeApiGetClientModeExtSupportedRates(UINT32 phyHwMacIndx)
{
	return &(mlmeApiClientModeExtSupportedRates[phyHwMacIndx]);
}

/*************************************************************************
* Function:
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern SINT32 mlmeApiQueryBeaconQueue( vmacStaInfo_t *vStaInfo_p)
{
#ifdef PORT_TO_LINUX_OS

#else /* PORT_TO_LINUX_OS */
	RxBeaconIsr();
#endif /* def PORT_TO_LINUX_OS */
	return MLME_SUCCESS;
}

#ifdef MLME_SEPERATE_SCAN_TIMER
static cyg_handle_t hSysClk_scan;
static cyg_handle_t hCounter_scan;

static cyg_handle_t hAlarm_scan;    
static UINT32 alarmData_scan;    
static cyg_alarm timerAlarm_scan;


/*************************************************************************
* Function:
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern SINT32 mlmeApiStopScanTimer( vmacStaInfo_t *vStaInfo_p)
{
#ifdef PORT_TO_LINUX_OS

#else /* PORT_TO_LINUX_OS */
	cyg_alarm_disable(hAlarm_scan);
	cyg_alarm_delete(hAlarm_scan);
#endif /* def PORT_TO_LINUX_OS */
	return MLME_SUCCESS;
}

/*************************************************************************
* Function:
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
void TimerTick_scan(UINT32 data0, cyg_addrword_t data1)
{
	vmacStaInfo_t *vStaInfo_p = (vmacStaInfo_t *)data1;
	mlmeApiStopScanTimer( vStaInfo_p);
	if(vStaInfo_p->scaningTimer.callback != NULL)
	{
		vStaInfo_p->scaningTimer.callback(vStaInfo_p->scaningTimer.userdata_p);
	}
}

/*************************************************************************
* Function:
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern SINT32 mlmeApiStartScanTimer( vmacStaInfo_t *vStaInfo_p,
									UINT32 numTick,
									void *callBackFunct_p)
{    
	ENTER_CRITICAL;
	alarmData_scan = (UINT32)vStaInfo_p;
	vStaInfo_p->scaningTimer.callback = callBackFunct_p;
	vStaInfo_p->scaningTimer.userdata_p = (UINT8 *)vStaInfo_p;
#ifdef PORT_TO_LINUX_OS

#else /* def PORT_TO_LINUX_OS */
	if(hSysClk_scan != 0)
	{
		mlmeApiStopScanTimer(vStaInfo_p);
	}
	/* Attach the timer to the real-time clock */
	hSysClk_scan = cyg_real_time_clock();
	cyg_clock_to_counter(hSysClk_scan, &hCounter_scan);
	cyg_alarm_create(hCounter_scan, (cyg_alarm_t *)&TimerTick_scan,
		(cyg_addrword_t) alarmData_scan,
		&hAlarm_scan, &timerAlarm_scan);

	/* This creates a periodic timer */
	cyg_alarm_initialize(hAlarm_scan, cyg_current_time() + numTick, numTick);
#endif /* PORT_TO_LINUX_OS */
	EXIT_CRITICAL;
	return MLME_SUCCESS;
}
#endif /* MLME_SEPERATE_SCAN_TIMER */

/*************************************************************************
* Function:
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern SINT32 mlmeApiInitKeyMgmt( vmacStaInfo_t *vStaInfo_p)
{
	vmacEntry_t  *vmacEntry_p;
	vmacEntry_p = (vmacEntry_t  *)vStaInfo_p->vMacEntry_p;
#ifdef WPA_STA
	if(vStaInfo_p && vmacEntry_p)
	{
		((KeyData_t *)vStaInfo_p->keyMgmtInfoSta_p->pKeyData)->RSNDataTrafficEnabled = 0;
		KeyMgmtSta_InitSession(vmacEntry_p);
	}
#endif /* WPA_STA */
	return 0;
}

/*************************************************************************
* Function:  mlmeApiStartKeyMgmt
*
* Description:
*            Start RSN key handshake session.
*
* Input:
*
* Output:
*
**************************************************************************/
extern void mlmeApiStartKeyMgmt(vmacStaInfo_t * vStaInfo_p)
{
	vmacEntry_t  *vmacEntry_p;
	vmacEntry_p = (vmacEntry_t  *)vStaInfo_p->vMacEntry_p;
#ifdef WPA_STA
	if(vStaInfo_p && vmacEntry_p)
	{
		keyMgmtSta_StartSession(vmacEntry_p);
	}
#endif /* WPA_STA */
}

/*************************************************************************
* Function:
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern void mlmeApiSetMacAddrByMacId(vmacStaInfo_t * vStaInfo_p)
{
	vmacEntry_t  *vmacEntry_p;
	vmacEntry_p = (vmacEntry_t  *)vStaInfo_p->vMacEntry_p;
#ifdef PORT_TO_LINUX_OS

#else /* PORT_TO_LINUX_OS */
	msi_wl_SetMacAddrByMacId(vmacEntry_p->phyHwMacIndx,
		vmacEntry_p->macId,
		&vmacEntry_p->vmacAddr[0]);
#endif /* def PORT_TO_LINUX_OS */
}

/*************************************************************************
* Function:
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern SINT32 mlmeApiDelMacAddr(vmacStaInfo_t * vStaInfo_p, 
								UINT32 mac_id)
{
#ifdef PORT_TO_LINUX_OS
	return -1;
#else /* def PORT_TO_LINUX_OS */
	return msi_wl_StaDelMacAddr(mac_id);
#endif /* def PORT_TO_LINUX_OS */
}

/*************************************************************************
* Function:
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern SINT32 mlmeApiAddMacAddr(vmacStaInfo_t * vStaInfo_p,
								UINT8 macIndex,
								UINT8 *addr, 
								UINT8 addr_mask)
{
	UINT16 macId;
	UINT8  macIndx;

	macId = mlmeApiGetMacId(vStaInfo_p);
	macIndx = mlmeApiGetMacIndex(vStaInfo_p);

#ifdef PORT_TO_LINUX_OS
	//bt_port:: need fw api to set address filter and get assigned MAC id 
	//return -1;
	return 1;
#else /* PORT_TO_LINUX_OS */
	msi_wl_StaAddMacAddr(macIndex, addr, addr_mask);
#endif /* def PORT_TO_LINUX_OS */
}

/*************************************************************************
* Function:
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern UINT8 mlmeApiMamGetMACAddress(vmacStaInfo_t * vStaInfo_p,
									 int clientType, 
									 UINT8 *pMacAddress)
{
	vmacEntry_t  *vmacEntry_p = (vmacEntry_t  *)vStaInfo_p->vMacEntry_p;
#ifdef PORT_TO_LINUX_OS
	memcpy(pMacAddress, &vmacEntry_p->vmacAddr, IEEEtypes_ADDRESS_SIZE);
	return 1;
#else /* PORT_TO_LINUX_OS */
	return mamGetMACAddress(clientType, pMacAddress);
#endif /* def PORT_TO_LINUX_OS */
}

/*************************************************************************
* Function:
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern UINT8 mlmeApiMamGetHostMACAddress(vmacStaInfo_t *vStaInfo_p, 
										 UINT8 *pAddr )
{
	UINT8  macIndx;
	vmacEntry_t  *vmacEntry_p = (vmacEntry_t  *)vStaInfo_p->vMacEntry_p;

	macIndx = mlmeApiGetMacIndex(vStaInfo_p);

#ifdef PORT_TO_LINUX_OS
	memcpy(pAddr, &vmacEntry_p->vmacAddr, IEEEtypes_ADDRESS_SIZE);
	return 1;
#else /* PORT_TO_LINUX_OS */
	return mamGetHostMACAddress(macIndx, UINT8 *pAddr );
#endif /* PORT_TO_LINUX_OS */
}

/*************************************************************************
* Function:
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern SINT32 mlmeApiUpdateBasicRatesSetting(vmacStaInfo_t *vStaInfo_p,
											 UINT8 *rateBuf_p,
											 UINT32 rateBufLen)
{
	UINT16 macId;
	UINT8  macIndx;

	macId = mlmeApiGetMacId(vStaInfo_p);
	macIndx = mlmeApiGetMacIndex(vStaInfo_p);

#ifdef PORT_TO_LINUX_OS
	return -1;
#else /* PORT_TO_LINUX_OS */
	return smeUpdateBasicRatesSetting(macIndx,
		rateBuf_p,
		rateBufLen);
#endif /* PORT_TO_LINUX_OS */
}

/*************************************************************************
* Function:
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern void mlmeApiRateUpdateRxStats(vmacStaInfo_t *vStaInfo_p,
									 UINT8 *rfHdr_p, 
									 UINT32 length)
{
#ifdef PORT_TO_LINUX_OS
	return;
#else /* PORT_TO_LINUX_OS */
	extern void rate_UpdateRxStats(rate_RxSign_t *RxSignOfStation_p, int len);
	rate_UpdateRxStats((WLAN_RX_INFO *)rfHdr_p, length);
#endif /* def PORT_TO_LINUX_OS */
}

/*************************************************************************
* Function:
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern void mlmeApiUpdateSTAVendorIEs( vmacStaInfo_t *vStaInfo_p,
									  UINT8 fForceUpdate)
{
	UINT16 macId;
	UINT8  macIndx;

	macId = mlmeApiGetMacId(vStaInfo_p);
	macIndx = mlmeApiGetMacIndex(vStaInfo_p);

#ifdef PORT_TO_LINUX_OS

#else /* PORT_TO_LINUX_OS */
	UpdateSTAVendorIEs(fForceUpdate, macIndx);
#endif /* PORT_TO_LINUX_OS */
}
/*************************************************************************
* Function:
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern BOOLEAN mlmeApiMacMgtMlmeGetPeerStationAid( vmacStaInfo_t *vStaInfo_p,
												  extStaDb_StaInfo_t *StaInfo_p, 
												  UINT32 *Aid_p)
{
#ifdef PORT_TO_LINUX_OS
	return FALSE;
#else /* PORT_TO_LINUX_OS */
	return macMgtMlme_GetPeerStationAid(StaInfo_p, Aid_p);
#endif /* def PORT_TO_LINUX_OS */
}

/*************************************************************************
* Function:
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern WL_STATUS mlmeApiSendNullDataPktUr(vmacStaInfo_t *vStaInfo_p,
										  IEEEtypes_MacAddr_t *DestAddr,
										  IEEEtypes_MacAddr_t *SrcAddr)
{

	UINT16 macId;
	UINT8  macIndx;

	macId = mlmeApiGetMacId(vStaInfo_p);
	macIndx = mlmeApiGetMacIndex(vStaInfo_p);

#ifdef PORT_TO_LINUX_OS
	return -1;
#else /* PORT_TO_LINUX_OS */
	return sendNullDataPktUr(DestAddr,SrcAddr, macIndx);
#endif /* PORT_TO_LINUX_OS */
}

/*************************************************************************
* Function:
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern UINT8 mlmeApiHalRssiDbmGet(vmacStaInfo_t *vStaInfo_p,
								  UINT8 rawRssi, 
								  UINT8 rawSq1)
{
#ifdef PORT_TO_LINUX_OS
	return rawRssi;
#else /* PORT_TO_LINUX_OS */
	return hal_RssiDbmGet(rawRssi, rawSq1);
#endif /* PORT_TO_LINUX_OS */
}

/*************************************************************************
* Function:
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern SINT32 mlmeApiSetTrunkIdActive(vmacStaInfo_t *vStaInfo_p,
									  SINT8 trunk_id, 
									  BOOLEAN active, 
									  UINT8 trunk_mode)
{
	UINT8  macIndx;

	macIndx = mlmeApiGetMacIndex(vStaInfo_p);

#ifdef PORT_TO_LINUX_OS
	return -1;
#else /* PORT_TO_LINUX_OS */
	return EurusSetTrunkIdActive(trunk_id, macIndx, active, trunk_mode); 
#endif /* PORT_TO_LINUX_OS */
}

/*************************************************************************
* Function:
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern UINT32 mlmeApiIsTrunkIdActive(vmacStaInfo_t *vStaInfo_p,
									 SINT8 trunk_id)
{
#ifdef PORT_TO_LINUX_OS
	return -1;
#else /* PORT_TO_LINUX_OS */
	return EurusIsTrunkIdActive(trunk_id);
#endif /* PORT_TO_LINUX_OS */
}

/*************************************************************************
* Function:
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern UINT32 mlmeApiGetTime(vmacStaInfo_t *vStaInfo_p)
{
#ifdef PORT_TO_LINUX_OS
	return os_CurrentTime();
#else /* PORT_TO_LINUX_OS */
	UINT32 timeVal;
	time(&timeVal);
	return timeVal;
#endif /* PORT_TO_LINUX_OS */
}

/*************************************************************************
* Function:
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern UINT32 mlmeApiGenRandomUINT32(vmacStaInfo_t *vStaInfo_p, UINT32 seed)
{
#ifdef PORT_TO_LINUX_OS
	return net_random();
#else /* PORT_TO_LINUX_OS */
	srand(seed);
	return rand();
#endif /* PORT_TO_LINUX_OS */
}

/************************** New Timer API Wrapper *******************/

/*************************************************************************
* Function:
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern void mlmeApiStartTimer(vmacStaInfo_t *vStaInfo_p, 
							  UINT8 *timer_p,
							  void *callback,
							  UINT32 ticks)
{
	TimerRemove((Timer *)timer_p);
	TimerInit((Timer *)timer_p);
	TimerFireIn((Timer *)timer_p, 1, 
		callback, 
		(UINT8 *)vStaInfo_p, 
		ticks);
}


/*************************************************************************
* Function:
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern void mlmeApiStopTimer(vmacStaInfo_t *vStaInfo_p, UINT8 *timer_p)
{
	TimerRemove((Timer *)timer_p);
}

#ifdef STA_QOS
/*************************************************************************
* Function:
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern BOOLEAN mlmeApiIsWmeAP(vmacStaInfo_t *vStaInfo_p, UINT8 *Addr_p)
{
	if(IsWmeAP(vStaInfo_p, Addr_p))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
#endif

/*************************************************************************
* Function:
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern void mlmeApiDelStaDbEntry( vmacStaInfo_t *vStaInfo_p, UINT8 *peerAdr_p)
{
#ifdef PORT_TO_LINUX_OS /* PORT_TO_LINUX_OS */
	DeleteClientInfo(peerAdr_p, (vmacEntry_t *)vStaInfo_p->vMacEntry_p);
	return;
#else /* PORT_TO_LINUX_OS */
	/* Remove AP from station database, somtimes added by AP state machine. */
	extStaDb_DelSta((IEEEtypes_MacAddr_t *)peerAdr_p, 0);
#endif /* PORT_TO_LINUX_OS */

}

/*************************************************************************
* Function:
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern void mlmeApiAddStaDbEntry( vmacStaInfo_t *vStaInfo_p,
								 dot11MgtFrame_t *MgmtMsg_p )
{
#ifdef PORT_TO_LINUX_OS
	InitClientInfo(MgmtMsg_p->Hdr.BssId, MgmtMsg_p, (vmacEntry_t *)vStaInfo_p->vMacEntry_p, vStaInfo_p->isApMrvl);

#else /* PORT_TO_LINUX_OS */
	IEEEtypes_SuppRatesElement_t *PeerSupportedRates_p;
	IEEEtypes_ExtSuppRatesElement_t *PeerExtSupportedRates_p;
	extStaDb_StaInfo_t *wbStaInfo_p;
	UINT32 local_urAid;

	/*Also create an entry in the Peer StationInfo Database*/
	/*This will contain a pointer to the set of supported rates and other info*/
	/*needed by rate adaptation*/
	/*But first check if an entry already exist*/
	if (mlmeApiGetPeerStationStaInfoAndAid(&(MgmtMsg_p->Hdr.SrcAddr),
		&wbStaInfo_p, &local_urAid) == FALSE)
	{
		/*there is no entry in the station database for the peer station with this*/
		/*mac address. So first create one*/
		/*first get the set of supported rates of the peer station*/

		/*get a pointer to the starting IE for this Mgt frame*/
		attrib_p = (UINT8 *)&MgmtMsg_p->Body.AssocRsp.AId;
		attrib_p += sizeof(IEEEtypes_AId_t);
		/*now get a pointer to the set of Supported Rates and Ext Supported Rates*/
		PeerSupportedRates_p = syncSrv_ParseAttribWithinFrame(MgmtMsg_p, attrib_p, SUPPORTED_RATES);

		PeerExtSupportedRates_p = syncSrv_ParseAttribWithinFrame(MgmtMsg_p, attrib_p, EXT_SUPPORTED_RATES);

		if (mlmeApiCreatePeerStationInfoForWBMode(&(MgmtMsg_p->Hdr.SrcAddr),
			PeerSupportedRates_p, PeerExtSupportedRates_p, 
			vmacEntry_p->phyHwMacIndx) == 
			TRUE)
		{

			mlmeApiGetPeerStationStaInfoAndAid(&(MgmtMsg_p->Hdr.SrcAddr),
				&wbStaInfo_p, &local_urAid);
			/*Update the BSSID for this WB*/
			memcpy(&(wbStaInfo_p->Bssid[0]), 
				&(vStaInfo_p->macMgmtMlme_ThisStaData.BssId[0]),
				sizeof(IEEEtypes_MacAddr_t));
		}
		else
		{
			/*Could not create an entry for this station*/
			return MLME_FAILURE;
		}
	}
	/*Set the state of the Sta Info to WB_ASSOCIATED*/
	mlmeApiSetPeerStationStateForWB(wbStaInfo_p, WB_ASSOCIATED);
#endif /* PORT_TO_LINUX_OS */
}


extern iw_linkInfo_t *mlmeApiGetStaLinkInfo(struct net_device *dev)
{
    vmacStaInfo_t *vStaInfo_p;  //= (vmacStaInfo_t *)vmacEntry_p->info_p;
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, dev);
	vmacApInfo_t *vmacSta_p = wlpptr->vmacSta_p;

	vStaInfo_p = (vmacStaInfo_t *)vmacGetVMacStaInfo(parentGetVMacId(vmacSta_p->VMacEntry.phyHwMacIndx));
    return &vStaInfo_p->linkInfo;

}




