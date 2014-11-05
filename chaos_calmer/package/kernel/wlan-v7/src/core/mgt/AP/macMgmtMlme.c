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


/******************** (c) Marvell Semiconductor, Inc., 2001 *******************
*
* Public Procedures:
*    macMgmtMlme_AssociateCmd      Process a cmd to associate with an AP
*    macMgmtMlme_AssociateRsp      Process an associate rsp from an AP
*    macMgmtMlme_Atim              Process an ATIM msg from another STA
*    macMgmtMlme_AuthenticateCmd   Process a cmd to authenticate with
*                                  another station or an AP
*    macMgmtMlme_AutheticateMsg    Process an authentication msg from a
*                                  station or an AP
*    macMgmtMlme_BeaconMsg         Process a beacon msg from an AP or a
*                                  station
*    macMgmtMlme_DeauthenticateMsg Process a deauthentication msg from a
*                                  station or an AP
*    macMgmtMlme_DisassociateCmd   Process a cmd to disassociate with an AP
*    macMgmtMlme_DisassociateMsg   Process a disassociation msg from an AP
*    macMgmtMlme_JoinCmd           Process a cmd to join a BSS
*    macMgmtMlme_ProbeRqst         Process a probe request from another
*                                  station in an IBSS
*    macMgmtMlme_ProbeRsp          Process a probe response from a station
*                                  or an AP
*    macMgmtMlme_ReassociateCmd    Process a cmd to reassociate with a new AP
*    macMgmtMlme_ReassociateRsp    Process a reassociation rsp from an AP
*    macMgmtMlme_ResetCmd          Process a cmd to peform a reset
*    macMgmtMlme_ScanCmd           Process a cmd to perform a scan for BSSs
*    macMgmtMlme_StartCmd          Process a cmd to start an IBSS
*                                  timers
*
* Notes:
*    None.
*
*****************************************************************************/

/*============================================================================= */
/*                               INCLUDE FILES */
/*============================================================================= */
#include <linux/if_arp.h>   
#include <linux/wireless.h>

#include "ap8xLnxIntf.h"

#include "wltypes.h"
#include "IEEE_types.h"
#include "mib.h"
#include "wl_hal.h"

#include "StaDb.h"
#include "qos.h"
#include "wlmac.h"

#include "ds.h"
#include "osif.h"
#include "keyMgmtCommon.h"
#include "keyMgmt.h"
#include "timer.h"
#include "tkip.h"

#include "smeMain.h"
#include "macmgmtap.h"
#include "macMgmtMlme.h"

#include "bcngen.h"
#include "idList.h"
#include "wl_macros.h"
#include "wpa.h"
#include "buildModes.h"
#include "wldebug.h"
#include "ap8xLnxWlLog.h"
#include "ap8xLnxIntf.h"
#include "domain.h"
#include "ap8xLnxFwcmd.h"
#include "ap8xLnxIntf.h"
#include "dfs.h"
#include "keyMgmtSta.h"

#define MCBC_STN_ID         (MAX_STNS)

#define MONITER_PERIOD_1SEC 10
static UINT32 counterHtProt[2] = {0,0};
static UINT8 legacyAPCount[2] = {0,0};

#ifdef INTOLERANT40
static UINT32 sHt30minStaIntolerant = 0;
#endif
#ifdef AP_URPTR
#include "wlvmac.h"
extern UINT8 mib_urMode;
extern UINT8 mib_wbMode;
extern UINT8 mib_urModeConfig;
extern UINT8 mib_StaMode;
UINT8  g_urClientMode = GONLY_MODE;
#ifdef AP_URPTR_NEW_RATE
AssocReqData_t      *g_urAssocInfo; //ur_todo - rate info
UINT32              urAid;
#else
AssocReqData_t      g_urAssocInfo; //ur_todo - rate info
#endif
#endif

#ifdef CAP_MAX_RATE
u_int32_t MCSCapEnable=0;
u_int32_t MCSCap;
#endif

UINT8 dfs_test_mode = 0;

UINT16 RxBeaconMaxCnt;
UINT32 BStnAroundCnt = 0;
#define HIGHEST_11B_RATE_REG_INDEX   4

#define SET_ERP_PROTECTION 1
#define RESET_ERP_PROTECTION 0


#define AID_PREFIX 0xC000 
#ifdef MRVL_DFS
extern int wlreset_mbss(struct net_device *netdev);
extern int  wlchannelSet(struct net_device *netdev, int channel, CHNL_FLAGS chanflag, UINT8 initRateTable);
#endif
#ifdef MRVL_WSC //MRVL_WSC_IE
static UINT8 WSC_OUI[4] = {0x00,0x50,0xf2,0x04};
#endif


typedef enum
{
	CMD_STATE_IDLE,
	CMD_STATE_AP
} CmdState_e;


extern UINT32 gMICFailRstTimerId; //holds the timerID of the MIC failure reset timer

extern UINT32 gMICFailTimerId; //holds the timerID of the MIC failure timer

AssocReqData_t AssocTable[MAX_AID + 1];
UINT16 PhyRates[IEEEtypes_MAX_DATA_RATES_G] = {2,4,11,22,44,12,18,24,36,48,72,96,108,144};
UINT16 PhyRatesA[IEEEtypes_MAX_DATA_RATES_A]={12,18,24,36,48,72,96,108,144};


#if defined(AP_SITE_SURVEY) || defined(AUTOCHANNEL)
/* Added for Site Survey */
#define SITE_SURVEY_ENTRY_MAX   50

/*---------------------------------*/
/* Client MLME local Management Messages */
/*---------------------------------*/
struct ieee80211_frame
{
	IEEEtypes_FrameCtl_t FrmCtl;
	UINT8   dur[2];
	UINT8   addr1[IEEEtypes_ADDRESS_SIZE];
	UINT8   addr2[IEEEtypes_ADDRESS_SIZE];
	UINT8   addr3[IEEEtypes_ADDRESS_SIZE];
	UINT8   seq[2];
	UINT8   addr4[IEEEtypes_ADDRESS_SIZE];
} PACK;

typedef struct dot11MgtFrame_t
{
	void *priv_p;
	IEEEtypes_MgmtHdr_t  Hdr;
	union
	{
		IEEEtypes_Bcn_t          Bcn;
		IEEEtypes_DisAssoc_t     DisAssoc;
		IEEEtypes_AssocRqst_t    AssocRqst;
		IEEEtypes_AssocRsp_t     AssocRsp;
		IEEEtypes_ReassocRqst_t  ReassocRqst;
		IEEEtypes_ReassocRsp_t   ReassocRsp;
		IEEEtypes_ProbeRqst_t    ProbeRqst;
		IEEEtypes_ProbeRsp_t     ProbeRsp;
		IEEEtypes_Auth_t         Auth;
		IEEEtypes_Deauth_t       Deauth;
#ifdef IEEE80211H
		IEEEtypes_ActionField_t Action;
#endif /* IEEE80211H */
#ifdef QOS_FEATURE
		IEEEtypes_ADDTS_Req_t    AddTSReq;
		IEEEtypes_ADDTS_Rsp_t    AddTSRsp;
		IEEEtypes_DELTS_Req_t    DelTSReq;
		WSM_DELTS_Req_t          DelWSMTSReq;
		IEEEtypes_ADDBA_Req_t    AddBAReq;
		IEEEtypes_ADDBA_Rsp_t    AddBAResp;
		IEEEtypes_DELBA_t        DelBA;
		IEEEtypes_DlpReq_t       DlpReq;
		IEEEtypes_DlpResp_t      DlpResp;
		IEEEtypes_DlpTearDown_t  DlpTearDown;
#endif
	} Body;
	UINT32  FCS;
} dot11MgtFrame_t;


#define MAX_B_DATA_RATES    4

#define SCAN_TIME  5    /* in unit of 100ms */
//static UINT8 scaningTimerInitialized =0;

#if defined(AP_SITE_SURVEY)
API_SURVEY_ENTRY  siteSurveyInfo[SITE_SURVEY_ENTRY_MAX];
extern void AccumulateSiteSurveyResults( void *BssData_p, UINT8 *rfHdr_p);
#endif /* AP_SITE_SURVEY */
#endif
/*============================================================================= */
/*                         IMPORTED PUBLIC VARIABLES */
/*============================================================================= */


UINT8 WiFiOUI[3] = {0x00,0x50,0xf2};
UINT8 B_COMP_OUI[3] = {0x00,0x90,0x4c};
UINT8 I_COMP_OUI[3] = {0x00,0x17,0x35};
#ifdef INTOLERANT40
#define INTOLERANTCHEKCOUNTER	1500 //25 MIN
static UINT32 sMonitorcnt30min = 0;
#endif

IEEEtypes_DataRate_t OpRateSet[IEEEtypes_MAX_DATA_RATES_G];
IEEEtypes_DataRate_t BasicRateSet[IEEEtypes_MAX_DATA_RATES_G];
UINT32 BasicRateSetLen;
UINT32 OpRateSetLen;

static UINT8 OpRateSetIndex[IEEEtypes_MAX_DATA_RATES_G];

static UINT8 BasicRateSetIndex[IEEEtypes_MAX_DATA_RATES_G];
static UINT32 LowestBasicRate;
static UINT32 LowestBasicRateIndex;
static UINT32 HighestBasicRate;
UINT32 HighestBasicRateIndex;
static UINT32 HighestBasicRateB;
static UINT32 LowestBasicRateB;
UINT32 HighestBasicRateIndexB;
static UINT32 LowestBasicRateIndexB;

static UINT32 LowestOpRate;
static UINT32 LowestOpRateIndex;
static UINT32 HighestOpRate;
static UINT32 HighestOpRateIndex;

volatile UINT32 ResetDone = 0;

extern macmgmtQ_MgmtMsg_t * BcnBuffer_p;
#ifdef ENABLE_RATE_ADAPTATION
extern UINT32 AssocStationsCnt;
#endif

IEEEtypes_MacAddr_t bcast= {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

#ifdef ENABLE_ERP_PROTECTION
UINT32 macErpProtModeEnabled = 1;
#else
UINT32 macErpProtModeEnabled = 0;
#endif

extern macmgmtQ_MgmtMsg_t * PrbRspBuf_p;
UINT32 macChangeSlotTimeModeTo = SLOT_TIME_MODE_SHORT;
UINT32 macCurSlotTimeMode = SLOT_TIME_MODE_LONG;

UINT8 BarkerPreambleSet=0;
UINT8 PreviousBAroundStnCount;
extern UINT8 AGinterval;
extern UINT8 freq54g;
UINT8 RfSwitchChanA, RfSwitchChanG;
extern UINT32 AGintOffset;

#ifdef AMPDU_SUPPORT
extern void disableAmpduTx(vmacApInfo_t *vmacSta_p,UINT8 *macaddr, UINT8 tid);
#endif

/*============================================================================= */
/*                          MODULE LEVEL VARIABLES */
/*============================================================================= */
IEEEtypes_BssDesc_t BssDesc;

/* data for testing */
smeQ_MgmtMsg_t ResetCfrm;
/*============================================================================= */
/*                   PRIVATE PROCEDURES (ANSI Prototypes) */
/*============================================================================= */
WL_STATUS isSsIdMatch(IEEEtypes_SsIdElement_t *SsId1,
					  IEEEtypes_SsIdElement_t *SsId2);

WL_STATUS isCapInfoSupported(IEEEtypes_CapInfo_t *CapInfo1,
							 IEEEtypes_CapInfo_t *CapInfo2);
UINT32 GetHighestRateIndex(vmacApInfo_t *vmacSta_p,IEEEtypes_SuppRatesElement_t * SuppRates,
						   IEEEtypes_ExtSuppRatesElement_t *ExtSuppRates,BOOLEAN *gRatePresent);
WL_STATUS isBasicRatesSupported(IEEEtypes_SuppRatesElement_t * SuppRates,
								IEEEtypes_ExtSuppRatesElement_t *ExtSuppRates,BOOLEAN gRatePresent);
UINT32 SetSuppRateSetRegMap(vmacApInfo_t *vmacSta_p,IEEEtypes_SuppRatesElement_t *SuppRates,
							UINT32 * SuppRateSetRegMap);
void PrepareRateElements(vmacApInfo_t *vmacSta_p,IEEEtypes_StartCmd_t *StartCmd_p );
void FixedRateCtl(extStaDb_StaInfo_t *pStaInfo, PeerInfo_t	*PeerInfo, MIB_802DOT11 *mib);
#ifdef BRS_SUPPORT
int isClientRateMatchAP(vmacApInfo_t *vmacSta_p,UINT8  *Rates);
#endif
#ifdef INTOLERANT40
static void HT40MIntolerantHandler(vmacApInfo_t *vmacSta_p,UINT8 soon);
BOOLEAN macMgmtMlme_SendBeaconReqMeasureReqAction(struct net_device *dev,IEEEtypes_MacAddr_t *Addr);
BOOLEAN macMgmtMlme_SendExtChanSwitchAnnounceAction(struct net_device *dev,IEEEtypes_MacAddr_t *Addr);
#endif
extern UINT32 CopySsId(UINT8 * SsId1, UINT8 * SsId2);
extern UINT8 RFInit(void);
extern void hw_Init(void);
extern void extStaDb_ProcessAgeTick(UINT8 *data);
extern void macMgmtMlme_SendDeauthenticateMsg(vmacApInfo_t *vmacSta_p,
											  IEEEtypes_MacAddr_t *Addr, UINT16 StnId, UINT16 Reason );
extern void macMgmtMlme_SendDisassociateMsg(vmacApInfo_t *vmacSta_p,
											IEEEtypes_MacAddr_t *Addr, UINT16 StnId, UINT16 Reason );
extern void macMgmtCleanUp(vmacApInfo_t *vmacSta_p,extStaDb_StaInfo_t *StaInfo_p);
#ifdef AP_MAC_LINUX
extern struct sk_buff *mlmeApiPrepMgtMsg(UINT32 Subtype, IEEEtypes_MacAddr_t *DestAddr, IEEEtypes_MacAddr_t *SrcAddr);
#else
extern tx80211_MgmtMsg_t *mlmeApiPrepMgtMsg(UINT32 Subtype, IEEEtypes_MacAddr_t *DestAddr, IEEEtypes_MacAddr_t *SrcAddr);
#endif
#ifdef AP_MAC_LINUX
extern struct sk_buff *mlmeApiPrepMgtMsg2(UINT32 Subtype, IEEEtypes_MacAddr_t *DestAddr, IEEEtypes_MacAddr_t *SrcAddr, UINT16 size);
#else
extern tx80211_MgmtMsg_t *mlmeApiPrepMgtMsg2(UINT32 Subtype, IEEEtypes_MacAddr_t *DestAddr, IEEEtypes_MacAddr_t *SrcAddr, UINT16 size);
#endif
extern void KeyMgmtHskCtor(vmacApInfo_t *vmacSta_p, extStaDb_StaInfo_t *pStaInfo);
extern void ProcessAddBAReq(macmgmtQ_MgmtMsg_t *);
extern void ProcessDelBA(macmgmtQ_MgmtMsg_t *);
extern void ProcessDlpReq(vmacApInfo_t *vmacSta_p, macmgmtQ_MgmtMsg_t *);
extern void ProcessDlpTeardown(vmacApInfo_t *vmacSta_p, macmgmtQ_MgmtMsg_t *);
extern void ProcessDlpRsp(vmacApInfo_t *vmacSta_p,macmgmtQ_MgmtMsg_t *);
extern void hw_Init2(void);
extern UINT8 BBPInit(void);
extern void wlQosSetQAsCAPQ(UINT32 );
extern BOOLEAN wlInitPHY(void);
extern void set_11a_mode(void);
extern void set_11g_mode(void);
extern UINT32 hw_GetPhyRateIndex(vmacApInfo_t *, UINT32 );
extern UINT32 hw_GetPhyRateIndex2(UINT32 );
extern BOOLEAN wlSet11aRFChan(UINT32);
extern BOOLEAN wlEnableReceiver(void);
#ifdef WMM_PS_SUPPORT
extern Status_e NotifyPwrModeChange(IEEEtypes_MacAddr_t *mac, IEEEtypes_PwrMgmtMode_e mode,UINT8 flag,UINT8 flag ){return FAIL;};
#else
extern Status_e NotifyPwrModeChange(IEEEtypes_MacAddr_t *mac, IEEEtypes_PwrMgmtMode_e mode){return FAIL;};
#endif
extern void DisableBlockTrafficMode(vmacApInfo_t *vmacSta_p);
extern UINT32 Rx_Traffic_Cnt(vmacApInfo_t *vmacSta_p);
extern UINT32 Rx_Traffic_Err_Cnt(vmacApInfo_t *vmacSta_p);

extern UINT32 Rx_Traffic_BBU(vmacApInfo_t *vmacSta_p);

extern void wlQosSetQosBackoffRegs(BOOLEAN );
#ifdef WMM_PS_SUPPORT
extern extStaDb_Status_e extStaDb_SetQosInfo (vmacApInfo_t *vmacSta_p, IEEEtypes_MacAddr_t *, QoS_WmeInfo_Info_t  *);
#endif



extern Status_e NotifyAid(UINT16 StnId, UINT16 Aid, Boolean PollFlag);

extern void syncSrv_SetNextChannel (vmacApInfo_t *vmacSta_p);


//extern void apiSetHiPower();

extern WL_STATUS mlmeAuthInit(UINT16 NoStns);
extern void mlmeAuthCleanup(vmacApInfo_t *vmacSta_p);
extern void StnIdListCleanup(void);
extern void EnableBlockTrafficMode(vmacApInfo_t *vmacSta_p);
#ifdef AUTOCHANNEL
extern BOOLEAN wlUpdateAutoChan(vmacApInfo_t *vmacSta_p,UINT32 chan,UINT8 shadowMIB);
#endif
extern BOOLEAN isMcIdIE(UINT8 *data_p);
extern BOOLEAN isM_RptrIdIE(UINT8 *data_p);
extern void keyMgmt_msg(vmacApInfo_t *vmacSta_p,DistTaskMsg_t *pDistMsg);
extern int wlFwSetAPUpdateTim(struct net_device *, u_int16_t, Bool_e);
extern int wlFwSetChannelSwitchIE(struct net_device *netdev, UINT32 nextChannel, UINT32 mode, UINT32 count, CHNL_FLAGS Chanflag);
extern UINT16 extStaDb_AggrCk(vmacApInfo_t *vmacSta_p);
extern void extStaDb_Cleanup(vmacApInfo_t *vmacSta_p);
extern UINT16 AddAddHT_IE(vmacApInfo_t *vmacSta_p,IEEEtypes_Add_HT_Element_t* pNextElement);
int wlFwSetSecurity(struct net_device *netdev,	u_int8_t *staaddr);
int wlFwSetHTGF( struct net_device *netdev, UINT32 mode);
void sendLlcExchangeID(struct net_device *dev, IEEEtypes_MacAddr_t *src);
void HandleNProtectionMode(vmacApInfo_t *vmacSta_p );
#ifdef COEXIST_20_40_SUPPORT
void Handle20_40_Channel_switch(vmacApInfo_t *vmacSta_p , UINT8);
extern int Check_40MHz_Affected_range(vmacApInfo_t *,int, int);
#define FortyMIntolerantRSSIThres	50			//RSSI threshold so AP is less sensitive to switch from 40 to 20MHz	
#endif

void checkLegDevOutBSS(vmacApInfo_t *vmacSta_p );
#ifdef MBSS
extern vmacApInfo_t  *vmacGetMBssByAddr(vmacApInfo_t *vmacSta_p,UINT8 *macAddr_p);
#endif
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

static UINT32 GetLegacyRateBitMap(vmacApInfo_t *vmacSta_p, IEEEtypes_SuppRatesElement_t *SuppRates, IEEEtypes_ExtSuppRatesElement_t *ExtSuppRates);
/*============================================================================= */
/*                         CODED PUBLIC PROCEDURES */
/*============================================================================= */
//static void dummy(void){}

extern int wlFwSetNProtOpMode(struct net_device *netdev, UINT8 mode);

extern WL_STATUS macMgmtMlme_Init(vmacApInfo_t *vmacSta_p,UINT32 maxStns, IEEEtypes_MacAddr_t *stnMacAddr)
{
	static int idlistinit=0;
#ifdef ENABLE_RATE_ADAPTATION
	UINT32 i;
	for (i = 0;i <= MAX_AID;i++)
	{
		AssocTable[i].RateToBeUsedForTx = 1;  /* index of the Rate reg 0 */
		AssocTable[i].HighestRateIndex = 1;
	}
	AssocStationsCnt = 0;
#endif

	/*-----------------------------------------------------------*/
	/* Initialize the random number generator used in generating */
	/* challenge text.                                           */
	/*-----------------------------------------------------------*/
	if (idlistinit == 0)
	{
		if (mlmeAuthInit(maxStns) == OS_FAIL)
			return (OS_FAIL);
		idlistinit++;
		InitAidList();
		if (InitStnIdList(maxStns) == OS_FAIL)
			return (OS_FAIL);
	}

	memcpy(&vmacSta_p->macStaAddr, stnMacAddr, sizeof(IEEEtypes_MacAddr_t));
	memcpy(&vmacSta_p->macBssId, stnMacAddr, sizeof(IEEEtypes_MacAddr_t));


	memcpy(&vmacSta_p->macBssId2, stnMacAddr, sizeof(IEEEtypes_MacAddr_t));
	vmacSta_p->macBssId2[5]=vmacSta_p->macBssId2[5]+1;
	return(OS_SUCCESS);
}

#ifdef AUTOCHANNEL
/*************************************************************************
* Function: syncSrv_RestorePreScanSettings
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
static void syncSrv_RestorePreScanSettings(vmacApInfo_t *vmacSta_p)
{   
	/* Set MAC back to AP Mode */
#ifdef AUTOCHANNEL
	UINT8 cur_channel;
#ifdef COEXIST_20_40_SUPPORT
	if(*(vmacSta_p->ShadowMib802dot11->mib_HT40MIntoler) || *(vmacSta_p->ShadowMib802dot11->mib_autochannel))
#else
	if(*(vmacSta_p->ShadowMib802dot11->mib_autochannel))
#endif
	{
		vmacSta_p->autochannelstarted = 1;
		cur_channel = channelSelected(vmacSta_p,(*(vmacSta_p->Mib802dot11->mib_ApMode)&AP_MODE_BAND_MASK) >=AP_MODE_A_ONLY);
#ifdef MRVL_DFS
		if( DfsPresentInNOL(  vmacSta_p->dev, cur_channel ) )
		{
			cur_channel = 36 ;
		}
#endif
#ifdef COEXIST_20_40_SUPPORT
		if( *(vmacSta_p->ShadowMib802dot11->mib_autochannel))
		{
			wlUpdateAutoChan(vmacSta_p,cur_channel,1);
		}
#endif
		wlSetOpModeMCU(vmacSta_p,MCU_MODE_AP);
		wlResetTask(vmacSta_p->dev);
		DisableBlockTrafficMode(vmacSta_p);

	}
#else  
	MIB_PHY_DSSS_TABLE *PhyDSSSTable_p=vmacSta_p->Mib802dot11->PhyDSSSTable;
	wlSetRFChan(vmacSta_p,PhyDSSSTable_p->CurrChan);
#endif
#ifdef AP_URPTR
	if (mib_wbMode == 1)
		wlSetOpModeMCU(vmacSta_p,MCU_MODE_STA_INFRA);
	else
#endif
}
extern SINT32 syncSrv_ScanActTimeOut(UINT8 *data)
{ 
	vmacApInfo_t *vmacSta_p = (vmacApInfo_t *)data;
#ifdef AUTOCHANNEL
	UINT32 t_cnt, bbu_cnt;
	int channel = 0;
	if(vmacSta_p->ChanIdx>=0)
		channel = vmacSta_p->ScanParams.ChanList[vmacSta_p->ChanIdx];
	if(channel)
	{
		t_cnt  = Rx_Traffic_Cnt(vmacSta_p);
		bbu_cnt = Rx_Traffic_BBU(vmacSta_p);
		WLDBG_INFO(DBG_LEVEL_7, "finished scanning channel %d, traffic cnt %d, err cnt %d\n", channel, t_cnt, bbu_cnt);
		vmacSta_p->autochannel[vmacSta_p->ChanIdx] = t_cnt*100 + bbu_cnt;
	}
#endif
	syncSrv_SetNextChannel(vmacSta_p);
	return 0;
}

extern void syncSrv_SetNextChannel ( vmacApInfo_t *vmacSta_p )
{   
	/* Increment the index keeping track of which channel scanning for */
	vmacSta_p->ChanIdx++;

	if (vmacSta_p->ChanIdx > vmacSta_p->NumScanChannels)
	{
		vmacSta_p->busyScanning = 0;
		syncSrv_RestorePreScanSettings(vmacSta_p);
		/* Reset Mac and Start AP Services */
#ifdef AP_MAC_LINUX
		Disable_ScanTimerProcess(vmacSta_p);
#else
		SendResetCmd(vmacSta_p,0);
#endif

		return;
	}

#if 1//def HARRIER
	while((vmacSta_p->ChanIdx < vmacSta_p->NumScanChannels) && (vmacSta_p->ScanParams.ChanList[vmacSta_p->ChanIdx] ==0) )
	{
		vmacSta_p->ChanIdx++;
	}
#endif

	//	etherBugSend("channel[%d] %d\n", ChanIdx, ScanParams.ChanList[ChanIdx]);
	/* Set the rf parameters */
	if(vmacSta_p->ChanIdx < vmacSta_p->NumScanChannels)
	{
		wlSetRFChan(vmacSta_p,vmacSta_p->ScanParams.ChanList[vmacSta_p->ChanIdx]);
		Rx_Traffic_Cnt(vmacSta_p);
		Rx_Traffic_BBU(vmacSta_p);
	}

	/* Start the scan timer with duration of the maximum channel time */
	TimerRearm(&vmacSta_p->scaningTimer, SCAN_TIME);

}


extern void resetautochanneldata(vmacApInfo_t *vmacSta_p)
{
	vmacSta_p->autochannelstarted = 0;
	memset(vmacSta_p->autochannel, 0, sizeof(UINT32)*(IEEEtypes_MAX_CHANNELS+IEEEtypes_MAX_CHANNELS_A) );
} 
extern int channelSelected(vmacApInfo_t *vmacSta_p,int mode)
{
	int i;
	int channel =0;
	UINT32 traffic = -1;
	UINT8 band = mode?FREQ_BAND_5GHZ:FREQ_BAND_2DOT4GHZ;
	/* for AUTOCHANNEL_G_BAND_1_6_11 */
	UINT32 sum1 = 0, sum2 = 0, sum3 = 0;

#ifdef AP_URPTR
	if ( mib_urMode == 1)
	{
		channel = urSelectedChannel();
		if(channel)
			return channel;
	}
#endif
	if(!vmacSta_p->autochannelstarted)
		return 0;
	i = 0;
	while(vmacSta_p->ScanParams.ChanList[i])
	{
		if(vmacSta_p->ScanParams.ChanList[i] && domainChannelValid(vmacSta_p->ScanParams.ChanList[i], band))
		{
			if(vmacSta_p->autochannel[i] < *(vmacSta_p->Mib802dot11->mib_acs_threshold))
			{
				if((vmacSta_p->ScanParams.ChanList[i] == 1) || (vmacSta_p->ScanParams.ChanList[i] == 11))
				{
					//give the priority to other channels other than low power 1 & 11
					vmacSta_p->autochannel[i] = *(vmacSta_p->Mib802dot11->mib_acs_threshold);
				}
				if((vmacSta_p->ScanParams.ChanList[i] >= 36) && (vmacSta_p->ScanParams.ChanList[i] <= 48))
				{
					//36, 40, 44, 48 power are low, give the other channels more priority
					vmacSta_p->autochannel[i] = *(vmacSta_p->Mib802dot11->mib_acs_threshold);
				}
				if((vmacSta_p->ScanParams.ChanList[i] >48 ) && (vmacSta_p->ScanParams.ChanList[i] < 149))
				{
					//52  - 140 channel yield to 149 - 161, but prior to 36 - 48
					vmacSta_p->autochannel[i] = *(vmacSta_p->Mib802dot11->mib_acs_threshold) - 1;
				}
			}
			if(vmacSta_p->autochannel[i] < traffic)
			{
				traffic = vmacSta_p->autochannel[i];
				channel = vmacSta_p->ScanParams.ChanList[i];
				WLDBG_INFO(DBG_LEVEL_7,"%s channel %d rx traffic byte count=%d\n",mode?"A":"G", channel, traffic); 
			}

			/* AUTOCHANNEL_G_BAND_1_6_11 */
			if (band == FREQ_BAND_2DOT4GHZ) {
				if ((vmacSta_p->ScanParams.ChanList[i] >= 1) && (vmacSta_p->ScanParams.ChanList[i] <= 5))
					sum1 += vmacSta_p->autochannel[i];
				if ((vmacSta_p->ScanParams.ChanList[i] >= 4) && (vmacSta_p->ScanParams.ChanList[i] <= 8))
					sum2 += vmacSta_p->autochannel[i];
				if ((vmacSta_p->ScanParams.ChanList[i] >= 7) && (vmacSta_p->ScanParams.ChanList[i] <= 11))
					sum3 += vmacSta_p->autochannel[i];
			}
		}	
		i++;
	}

#if 1 /* to enable using only CH1,6, or 11 for ACS. ACSAUTOCHANNEL_G_BAND_1_6_11 */
	if (band == FREQ_BAND_2DOT4GHZ) {
		WLDBG_INFO(DBG_LEVEL_7, "%s channel %d selected before G-Band 1/6/11 optimization, sum1=%d, sum2=%d, sum3=%d\n", mode?"A":"G", channel, sum1, sum2, sum3);
		if ((sum1 <= sum2) && (sum1 <= sum3))
			channel = 1;
		else if ((sum2 <= sum1) && (sum2 <= sum3))
			channel = 6;
		else
			channel = 11;
	}	
#endif

	WLDBG_INFO(DBG_LEVEL_7,"%s channel %d selected\n", mode?"A":"G", channel);
	// do not print for now
	//WLSYSLOG(WLSYSLOG_CLASS_ALL, WLSYSLOG_MSG_GEN_AUTOCHANNEL "%d\n", channel);
	return channel; 
}
#endif

#ifdef IEEE80211H
static WL_STATUS macMgmtMlme_asso_ind(vmacApInfo_t *vmacSta_p,IEEEtypes_MacAddr_t *addr) UNUSED;
//static WL_STATUS macMgmtMlme_reasso_ind(vmacApInfo_t *vmacSta_p,IEEEtypes_MacAddr_t *addr);
#endif /* IEEE80211H */


#ifdef ERP
extern void macMgmtMlme_IncrBonlyStnCnt(vmacApInfo_t *vmacSta_p,UINT8 option)
{
	MIB_802DOT11 *mib=vmacSta_p->Mib802dot11;

	if ( *(mib->mib_ApMode) != AP_MODE_B_ONLY && *(mib->mib_ApMode) != AP_MODE_A_ONLY)
	{
		if (!option)
			vmacSta_p->bOnlyStnCnt++;
#if 1//FIX NOW ????  def FIX_LATER /* Remove for Sfly, need to inform firmware of protection mode change. */
#ifdef ENABLE_ERP_PROTECTION
		if ( *(mib->mib_ErpProtEnabled) && (vmacSta_p->bOnlyStnCnt == 1 || BStnAroundCnt))
		{
			bcngen_UpdateBeaconErpInfo(vmacSta_p,SET_ERP_PROTECTION);
		}
#endif
#endif
		if ( *(mib->mib_shortSlotTime) && (vmacSta_p->bOnlyStnCnt == 1 || BStnAroundCnt) )
		{
			macChangeSlotTimeModeTo = SLOT_TIME_MODE_LONG; 
			//bcngen_EnableBcnFreeIntr();
		}
	}

}
extern void macMgmtMlme_DecrBonlyStnCnt(vmacApInfo_t      *vmacSta_p,UINT8 option)
{
	MIB_802DOT11 *mib=vmacSta_p->Mib802dot11;

	if ( *(mib->mib_ApMode) != AP_MODE_B_ONLY && *(mib->mib_ApMode) != AP_MODE_A_ONLY)
	{
		if ( !option && vmacSta_p->bOnlyStnCnt )
			vmacSta_p->bOnlyStnCnt--;
#ifdef ENABLE_ERP_PROTECTION
		if ( *(mib->mib_ErpProtEnabled) && !vmacSta_p->bOnlyStnCnt && !BStnAroundCnt )
		{
			bcngen_UpdateBeaconErpInfo(vmacSta_p, RESET_ERP_PROTECTION);
		}
#endif
		if ( *(mib->mib_shortSlotTime) && !vmacSta_p->bOnlyStnCnt && !BStnAroundCnt)
		{
			macChangeSlotTimeModeTo = SLOT_TIME_MODE_SHORT; 
			//bcngen_EnableBcnFreeIntr();
		}
		if (vmacSta_p->bOnlyStnCnt==0)
			BarkerPreambleSet=0;
	}
}
#endif

#ifdef APCFGUR
typedef struct IE_UR_Hdr_t
{
	UINT8 ElemId;
	UINT8 Len;
	UINT8 OUI[3];    /*00:50:43*/
	UINT8 OUI_Type;
	UINT8 OUI_Subtype;
	UINT8 Version;
}PACK_END IE_UR_Hdr_t;

BOOLEAN isUrClientIE(UINT8 *data_p)
{
	IE_UR_Hdr_t *ie;
	ie = (IE_UR_Hdr_t*)data_p;
	if (ie->ElemId != RSN_IE)
	{
		return FALSE;
	}
	if((ie->OUI[0] != 0x00) || (ie->OUI[1] != 0x50) || (ie->OUI[2] != 0x43))
		return FALSE;
	if(ie->OUI_Type !=1)
		return FALSE;
	if(ie->OUI_Subtype !=1)
		return FALSE;     
	if(ie->Version !=1)
		return FALSE;
	return TRUE;
}
#endif                

/******************************************************************************
*
* Name: macMgmtMlme_AssociateReq
*
* Description:
*    This routine handles a response from an AP to a prior associate request.
*
* Conditions For Use:
*    All software components have been initialized and started.
*
* Arguments:
*    Arg1 (i  ): MgmtMsg_p - Pointer to an 802.11 management message
*                            containing an associate response
*
* Return Value:
*    None.
*
* Notes:
*    None.
*
* PDL:
*        Check if the state is authenticated
*        Check if the SSID matches with AP's SSID
*        Check if the Capability Info could be supported
*        Check if the Basic rates are in the Supported rates
*        Assign Aid and store the information
*        Send AssociateRsp message back 
* END PDL
*
*****************************************************************************/

#define OUT_OF_BOUNDARDY_MESSAGE(x, y) printk("%s[line %d] out of boundary (%d %d)\n", __FUNCTION__, __LINE__, x, y)

static BOOLEAN macMgmtMlme_AssocReAssocIESanityCheck(UINT8 *buf, SINT32 len)
{
	SINT32 VarLen = 0;
	UINT8 *VariableElements_p=buf;
	UINT8 *end = (UINT8 *)((UINT32)buf +len);

	while(VarLen < len)
	{
		VarLen += (2 +*(VariableElements_p + 1));  /* value in the length field */
		VariableElements_p += 1;
		VariableElements_p += *VariableElements_p;
		VariableElements_p += 1;
		if(VariableElements_p > end)
			return FALSE;
	}
	if(VarLen == len)
		return TRUE;
	return FALSE;
}

void macMgmtMlme_AssocReAssocReqHandler(vmacApInfo_t *vmacSta_p,macmgmtQ_MgmtMsg3_t *MgmtMsg_p, UINT32 msgSize, UINT32 flag)
{
	struct wlprivate    *wlpptr   = NETDEV_PRIV_P(struct wlprivate, vmacSta_p->dev);
	MIB_802DOT11 *mib=vmacSta_p->Mib802dot11;
	MIB_PRIVACY_TABLE *mib_PrivacyTable_p=vmacSta_p->Mib802dot11->Privacy;
	MIB_RSNCONFIGWPA2 *mib_RSNConfigWPA2_p=vmacSta_p->Mib802dot11->RSNConfigWPA2;
	extStaDb_StaInfo_t *StaInfo_p;
	macmgmtQ_MgmtMsg2_t *MgmtRsp_p;
	UINT32  frameSize = 0;
	BOOLEAN ChangeAssocParam = FALSE;
	UINT32 Aid, i; //, VarLen;
	SINT32 VarLen;
	UINT8 *VariableElements_p;
	IEEEtypes_SuppRatesElement_t *Rates_p = NULL;
	IEEEtypes_ExtSuppRatesElement_t *ExtRates_p = NULL, *TxExtRates_p = NULL;
	BOOLEAN gRatePresent;
	UINT32 HighestRate;
	IEEEtypes_SsIdElement_t *SsId_p = NULL;
	IEEEtypes_RSN_IE_t *RsnIE_p = NULL;
#ifdef AP_WPA2
	IEEEtypes_RSN_IE_WPA2_t *RsnIEWPA2_p = NULL;
#endif
	IEEEtypes_InfoElementHdr_t *InfoElemHdr_p;
	QoS_Cap_Elem_t *QosCapElem_p = NULL;
	WSM_QoS_Cap_Elem_t *WsmQosCapElem_p = NULL;
	WME_param_elem_t *pWMEParamElem = NULL;
	WME_info_elem_t *pWMEInfoElem = NULL;
#ifdef IEEE80211H
	IEEEtypes_PowerCapabilityElement_t *PowerCapability_p = NULL;
	IEEEtypes_SupportedChannelElement_t *SupportedChanne_p = NULL;
#endif /* IEEE80211H */
	DistTaskMsg_t DistMsg;
	DistTaskMsg_t *pDistMsg = &DistMsg;
	StaAssocStateMsg_t * pStaMsg;
	//UINT32 headroom;
	//UINT32 tailroom;
	unsigned char tempbuffer[1024];
	PeerInfo_t	PeerInfo;
	int HTpresent =0;
#if defined(SOC_W8864)	
	UINT8 IE191Present = 0;
	UINT8 IE192Present = 0;
	UINT8 IE199Present = 0;		
	UINT8 ht_RxChannelWidth_IE61 = 0;
	UINT8 vht_RxChannelWidth_IE192 = 0;		//In IE192, 0:20 or 40Mhz, 1:80Mhz, 2:160Mhz, 3: 80+80Mhz
	UINT8 vht_RxChannelWidth_IE199 = 0;	
	UINT8 vht_RxNss_IE199 = 0;				
	UINT8 vht_peer_RxNss = 1;				//1:Nss1, 2: Nss2, 3: Nss3 ...
	UINT16 vht_RxNss1 = 1;					//To copy bit0-bit1 of Nss1
	UINT16 vht_RxNssMask = 0;
	UINT32 vhtcap = 0;	
#endif	
	UINT32 endofmessagelocation = (UINT32)MgmtMsg_p + msgSize;
#ifdef CAP_MAX_RATE
UINT8 shiftno;
UINT8 mcscapmask;
#endif 
#ifdef UAPSD_SUPPORT
	UINT8 QosInfo=0;
	UINT8 isQosSta=0;
#endif
#if defined(MRV_8021X) && !defined(ENABLE_WLSNDEVT)
	union iwreq_data wreq; /* MRV_8021X */
#endif
	IEEEtypes_HT_Element_t *pHTIE;
	IEEEtypes_Add_HT_Element_t *pAddHTIE;
#ifdef COEXIST_20_40_SUPPORT
	MIB_PHY_DSSS_TABLE *PhyDSSSTable=mib->PhyDSSSTable;
#endif
#ifdef MRVL_WAPI
	IEEEtypes_WAPI_IE_t *WAPI_IE_p = NULL;
#endif
#ifdef MRVL_WPS2
    void * IE_p=NULL;
#endif

IEEEtypes_SuppRatesElement_t *SuppRateSet_p=&(vmacSta_p->SuppRateSet);		
#ifdef ERP
	IEEEtypes_ExtSuppRatesElement_t *ExtSuppRateSet_p=&(vmacSta_p->ExtSuppRateSet);	
#endif

#ifdef AP_MAC_LINUX
	struct sk_buff *txSkb_p;
	int extra_size=0;
#ifdef MRVL_WPS2
	if (*(mib->mib_wpaWpa2Mode) > 4)
		extra_size += sizeof(AssocResp_WSCIE_t);
#endif
#if defined(SOC_W8864)	
    extra_size += (sizeof(IEEEtypes_VhtCap_t) + sizeof(IEEEtypes_VhOpt_t));
#endif
	WLDBG_INFO(DBG_LEVEL_7, "%sassoc rxed", flag?"re":"");
	if(flag)
	{
		if ((txSkb_p = mlmeApiPrepMgtMsg2(IEEE_MSG_REASSOCIATE_RSP, &MgmtMsg_p->Hdr.SrcAddr, &MgmtMsg_p->Hdr.DestAddr, sizeof(IEEEtypes_AssocRsp_t)-4 + extra_size)) == NULL)        
			return;
	}
	else
	{
		if ((txSkb_p = mlmeApiPrepMgtMsg2(IEEE_MSG_ASSOCIATE_RSP, &MgmtMsg_p->Hdr.SrcAddr, &MgmtMsg_p->Hdr.DestAddr, sizeof(IEEEtypes_AssocRsp_t)-4 + extra_size)) == NULL)        
			return;
	}
	MgmtRsp_p = (macmgmtQ_MgmtMsg2_t *) txSkb_p->data;
#else
	/* Allocate space for response message */
	tx80211_MgmtMsg_t * TxMsg_p;

	if ((TxMsg_p = mlmeApiPrepMgtMsg(IEEE_MSG_ASSOCIATE_RSP, &MgmtMsg_p->Hdr.SrcAddr, &MgmtMsg_p->Hdr.DestAddr)) == NULL)
	{
		return;
	}
#endif
	
	/*Check if max sta limit per virtual interface is reached.  By default, it is 64 (MAX_STNS) but user can configure the max limit by
	 * using setcmd maxsta 
	**/
	if(extStaDb_entries(vmacSta_p, 0) > *(mib->mib_maxsta))
	{	
		MgmtRsp_p->Body.AssocRsp.StatusCode = IEEEtypes_STATUS_ASSOC_DENIED_UNSPEC;
		if ((txMgmtMsg(vmacSta_p->dev,txSkb_p)) != OS_SUCCESS )
			dev_kfree_skb_any(txSkb_p);
		return ;
	}

	memset((void *)&PeerInfo, 0, sizeof(PeerInfo_t));
	/* Fill the header and other values */
	MgmtRsp_p->Body.AssocRsp.CapInfo = vmacSta_p->macCapInfo;
	MgmtRsp_p->Body.AssocRsp.SuppRates.ElementId = SUPPORTED_RATES;
	MgmtRsp_p->Body.AssocRsp.SuppRates.Len = SuppRateSet_p->Len;
	memcpy((char *)&MgmtRsp_p->Body.AssocRsp.SuppRates.Rates, (char *)&SuppRateSet_p->Rates, SuppRateSet_p->Len);
	
	//MgmtRsp_p->Hdr.FrmBodyLen = 6 + MgmtRsp_p->Body.AssocRsp.SuppRates.Len + 2;
	//frameSize = MgmtRsp_p->Body.AssocRsp.SuppRates.Len + 2;
	frameSize = 30 /* sizeof(struct ieee80211_frame) */ +
		sizeof(IEEEtypes_CapInfo_t) +
		sizeof(IEEEtypes_StatusCode_t) +
		sizeof(IEEEtypes_AId_t) +
		sizeof(IEEEtypes_InfoElementHdr_t) +
		MgmtRsp_p->Body.AssocRsp.SuppRates.Len;

#ifdef ERP
	if (ExtSuppRateSet_p->Len > 0)
	{
		TxExtRates_p = (IEEEtypes_ExtSuppRatesElement_t *)((UINT8*) & MgmtRsp_p->Body.AssocRsp.SuppRates + 
			sizeof(IEEEtypes_InfoElementHdr_t) + SuppRateSet_p->Len);
		TxExtRates_p->ElementId = ExtSuppRateSet_p->ElementId;
		TxExtRates_p->Len = ExtSuppRateSet_p->Len;
		memcpy((char *)&TxExtRates_p->Rates,
			(char *)&ExtSuppRateSet_p->Rates, ExtSuppRateSet_p->Len);
		//MgmtRsp_p->Hdr.FrmBodyLen += sizeof(IEEEtypes_InfoElementHdr_t) + ExtSuppRateSet.Len;
		frameSize += sizeof(IEEEtypes_InfoElementHdr_t) + ExtSuppRateSet_p->Len;
	}
#endif

	skb_trim(txSkb_p, frameSize); 

	if ( (StaInfo_p = extStaDb_GetStaInfo(vmacSta_p,&(MgmtMsg_p->Hdr.SrcAddr), 1)) == NULL ||
		StaInfo_p->State != AUTHENTICATED || vmacSta_p->MIC_ErrordisableStaAsso!=0)
	{
#ifdef SERCOMM /* Per FAE, Sercomm needs this. This flag is only for private build */
		/* Purpose - it was said that this is for WAG511 v1 issue */
		/* This is not confirmed, and may break standard protocol so keep as private flag*/
		if(StaInfo_p->State == ASSOCIATED)
		{
			dev_kfree_skb_any(txSkb_p);
			return;
		}    
#endif

		MgmtRsp_p->Body.AssocRsp.StatusCode =
			IEEEtypes_STATUS_UNSPEC_FAILURE;
		//TxMsg_p->stnId = StaInfo_p->StnId;
		/* Send for tx */
		if (txMgmtMsg(vmacSta_p->dev,txSkb_p) != OS_SUCCESS )
			dev_kfree_skb_any(txSkb_p);
		return ;

	}
#ifdef IEEE80211H
	StaInfo_p->ListenInterval = MgmtMsg_p->Body.AssocRqst.ListenInterval;
#endif
#ifdef MRVL_WSC //MRVL_WSC_IE
	memset(&StaInfo_p->WscIEBuf, 0, sizeof(WSC_ProbeRespIE_t));
#endif
#ifdef AMPDU_SUPPORT
	cleanupAmpduTx(vmacSta_p,(UINT8 *)&StaInfo_p->Addr);
#endif

	if(flag)
	{
		//re-assoc request has 6 bytes Current AP address after ListenInterval	
		VariableElements_p = (UINT8*) & MgmtMsg_p->Body.AssocRqst.SsId+sizeof(IEEEtypes_MacAddr_t);
		/* Add 2 bytes to make up for BodyLen, ptr is adjusted to include but
		message size does not include this. */
		VarLen = msgSize -
			sizeof(IEEEtypes_MgmtHdr3_t) - sizeof(IEEEtypes_CapInfo_t) -
			sizeof(IEEEtypes_ListenInterval_t) + 2-sizeof(IEEEtypes_MacAddr_t);
	}
	else
	{
		VariableElements_p = (UINT8*) & MgmtMsg_p->Body.AssocRqst.SsId;
		/* Add 2 bytes to make up for BodyLen, ptr is adjusted to include but
		message size does not include this. */
		VarLen = msgSize -
			sizeof(IEEEtypes_MgmtHdr3_t) - sizeof(IEEEtypes_CapInfo_t) -
			sizeof(IEEEtypes_ListenInterval_t) + 2;
	}
#ifdef ERP
	ExtRates_p = NULL;
#endif

	if(macMgmtMlme_AssocReAssocIESanityCheck(VariableElements_p, VarLen) == FALSE)
	{
		MgmtRsp_p->Body.AssocRsp.StatusCode =
			IEEEtypes_STATUS_UNSPEC_FAILURE;
		if (txMgmtMsg(vmacSta_p->dev,txSkb_p) != OS_SUCCESS )
			dev_kfree_skb_any(txSkb_p);

		return ;
	}

	while (VarLen > 0)
	{
		if((UINT32)VariableElements_p >= endofmessagelocation) 
		{
			break;
		}
		switch (*VariableElements_p)
		{
		case SSID:
			SsId_p = (IEEEtypes_SsIdElement_t *)VariableElements_p;
			VariableElements_p += (sizeof(IEEEtypes_ElementId_t) +
				sizeof(IEEEtypes_Len_t) +
				SsId_p->Len);
			VarLen -= (sizeof(IEEEtypes_ElementId_t) +
				sizeof(IEEEtypes_Len_t) +
				SsId_p->Len);
			break;
		case SUPPORTED_RATES:
			Rates_p = (IEEEtypes_SuppRatesElement_t *)VariableElements_p;
			VariableElements_p += (sizeof(IEEEtypes_ElementId_t) +
				sizeof(IEEEtypes_Len_t) +
				Rates_p->Len);
			VarLen -= (sizeof(IEEEtypes_ElementId_t) +
				sizeof(IEEEtypes_Len_t) +
				Rates_p->Len);
			break;
#ifdef ERP
		case EXT_SUPPORTED_RATES:
			ExtRates_p = (IEEEtypes_ExtSuppRatesElement_t *)VariableElements_p;
			VariableElements_p += (sizeof(IEEEtypes_ElementId_t) +
				sizeof(IEEEtypes_Len_t) +
				ExtRates_p->Len);
			VarLen -= (sizeof(IEEEtypes_ElementId_t) +
				sizeof(IEEEtypes_Len_t) +
				ExtRates_p->Len);
			break;
#endif
#ifdef QOS_FEATURE
		case QOS_CAPABILITY:
			QosCapElem_p = (QoS_Cap_Elem_t *)VariableElements_p;
			VariableElements_p += (sizeof(IEEEtypes_ElementId_t) +
				sizeof(IEEEtypes_Len_t) +
				QosCapElem_p->Len);
			VarLen -= (sizeof(IEEEtypes_ElementId_t) +
				sizeof(IEEEtypes_Len_t) +
				QosCapElem_p->Len);
			break;

#endif //QOS_FEATURE
		case PROPRIETARY_IE:
#ifdef APCFGUR
			if(isUrClientIE(VariableElements_p)==TRUE)
			{
				StaInfo_p->UR = 1;
			}
#endif                
			if(isMcIdIE(VariableElements_p) == TRUE)
			{
				PeerInfo.MrvlSta = 1;
				StaInfo_p->IsStaMSTA = 1;
			}
			if((isM_RptrIdIE(VariableElements_p) == TRUE))
			{
				StaInfo_p->StaType |= 0x02; 							
			}
#ifdef MRVL_WSC //MRVL_WSC_IE
			if (!memcmp(&(( WSC_ProbeRespIE_t *)VariableElements_p)->OUI, WSC_OUI , 4))
			{
				WSC_ProbeRespIE_t *WscIE_p = (WSC_ProbeRespIE_t *)VariableElements_p;
				if( (WscIE_p->Len + 2)>sizeof(WSC_ProbeRespIE_t))
					OUT_OF_BOUNDARDY_MESSAGE((WscIE_p->Len + 2),sizeof(WSC_ProbeRespIE_t));
				else
				memcpy(&StaInfo_p->WscIEBuf, WscIE_p, WscIE_p->Len + 2);
                StaInfo_p->WSCSta = TRUE;
                /* If any RSN IE is included then ignore the RSN IE when WSC IE is included */
                RsnIEWPA2_p = NULL;
			}
#endif //MRVL_WSC_IE
#ifdef INTEROP
			if (!memcmp(&(( IEEEtypes_Generic_HT_Element_t *)VariableElements_p)->OUI, B_COMP_OUI , 3))
			{

				if (((IEEEtypes_Generic_HT_Element_t *)VariableElements_p)->OUIType == 51)
				{	
					IEEEtypes_Generic_HT_Element_t *tmp_p = (IEEEtypes_Generic_HT_Element_t *)VariableElements_p;
					HTpresent =1;
					PeerInfo.HTCapabilitiesInfo =  tmp_p->HTCapabilitiesInfo;
					PeerInfo.MacHTParamInfo = tmp_p->MacHTParamInfo;
#ifdef EXPLICIT_BF
					PeerInfo.TxBFCapabilities=tmp_p->TxBFCapabilities;
#endif
					if (*(mib->mib_3x3Rate) == 1 && (( * (mib->mib_rxAntenna) ==  0) || (*(mib->mib_rxAntenna) ==  3)))
					{
						/** 3x3 configuration **/
#ifdef CAP_MAX_RATE
						if (MCSCapEnable)		/*Enabled through mcscap debug cmd*/
						{
							if ((MCSCap>=0) && (MCSCap<8) && (tmp_p->SupportedMCSset[0]!=0))
							{
								shiftno= (MCSCap%8)+1;
								mcscapmask = 0xff << shiftno;
								tmp_p->SupportedMCSset[0]= ~(tmp_p->SupportedMCSset[0] & mcscapmask);
								tmp_p->SupportedMCSset[1]=0;	
								tmp_p->SupportedMCSset[2]=0;
								tmp_p->SupportedMCSset[3]=0;
							}
							else if ((MCSCap>7) && (MCSCap<16) && (tmp_p->SupportedMCSset[1]!=0))
							{	
								shiftno= (MCSCap%8)+1;
								mcscapmask = 0xff << shiftno;
								tmp_p->SupportedMCSset[1]= ~(tmp_p->SupportedMCSset[1] & mcscapmask);
								tmp_p->SupportedMCSset[2]=0; /*Higher MCSset[] set to 0, Lower MCSset[0] left as default*/
								tmp_p->SupportedMCSset[3]=0;
							}
							else if ((MCSCap>15) && (MCSCap<24) && (tmp_p->SupportedMCSset[2]!=0))
							{
								shiftno= (MCSCap%16)+1;
								mcscapmask = 0xff << shiftno;
								tmp_p->SupportedMCSset[2]= ~(tmp_p->SupportedMCSset[2] & mcscapmask);
								tmp_p->SupportedMCSset[3]=0;
							}
							else
							{
								printk("WRONG MCS cap value\n");
							}
							printk("MCSSet0:0x%x MCSSet1:0x%x MCSSet2:0x%x MCSSet3:0x%x \n"
							,tmp_p->SupportedMCSset[0],tmp_p->SupportedMCSset[1],tmp_p->SupportedMCSset[2]
							,tmp_p->SupportedMCSset[3]);
						}
#endif

						PeerInfo.HTRateBitMap = (tmp_p->SupportedMCSset[0] | (tmp_p->SupportedMCSset[1] << 8) |
							(tmp_p->SupportedMCSset[2] << 16) | (tmp_p->SupportedMCSset[3] << 24));
					}
					else if(!((*(mib->mib_rxAntenna) ==  0) || (*(mib->mib_rxAntenna) ==  3) ||(*(mib->mib_rxAntenna) ==  2)))

					{
						/** 1x1 configuration **/
#ifdef CAP_MAX_RATE
						if (MCSCapEnable)
						{
							if ((MCSCap>=0) && (MCSCap<8) && (tmp_p->SupportedMCSset[0]!=0))
							{
								shiftno= (MCSCap%8)+1;
								mcscapmask = 0xff << shiftno;
								tmp_p->SupportedMCSset[0]= ~(tmp_p->SupportedMCSset[0] & mcscapmask);
							}
							else 
							{
								printk("WRONG MCS cap value\n");
							}
							printk("MCSSet0:0x%x \n",tmp_p->SupportedMCSset[0]);
						}
#endif

						PeerInfo.HTRateBitMap = (tmp_p->SupportedMCSset[0]);
					}
					else
					{
#ifdef CAP_MAX_RATE
						if (MCSCapEnable)
						{
							if ((MCSCap>=0) && (MCSCap<8) && (tmp_p->SupportedMCSset[0]!=0))
							{
								shiftno= (MCSCap%8)+1;
								mcscapmask = 0xff << shiftno;
								tmp_p->SupportedMCSset[0]= ~(tmp_p->SupportedMCSset[0] & mcscapmask);
								tmp_p->SupportedMCSset[1]=0;
							}
							else if ((MCSCap>7) && (MCSCap<16) && (tmp_p->SupportedMCSset[1]!=0))
							{	
								shiftno= (MCSCap%8)+1;
								mcscapmask = 0xff << shiftno;
								tmp_p->SupportedMCSset[1]= ~(tmp_p->SupportedMCSset[1] & mcscapmask);
							}
							else 
							{
								printk("WRONG MCS cap value\n");
							}
							printk("MCSSet0:0x%x MCSSet1:0x%x \n"
							,tmp_p->SupportedMCSset[0],tmp_p->SupportedMCSset[1]);
						}
#endif

						PeerInfo.HTRateBitMap = (tmp_p->SupportedMCSset[0] | (tmp_p->SupportedMCSset[1] << 8));
					}

					StaInfo_p->HtElem.ElementId = tmp_p->ElementId;
					StaInfo_p->HtElem.Len = tmp_p->Len;
					StaInfo_p->HtElem.MacHTParamInfo = tmp_p->MacHTParamInfo;
					StaInfo_p->HtElem.ASCapabilities = tmp_p->ASCapabilities;
					StaInfo_p->HtElem.ExtHTCapabilitiesInfo = tmp_p->ExtHTCapabilitiesInfo;
#ifdef EXPLICIT_BF
					StaInfo_p->HtElem.TxBFCapabilities = tmp_p->TxBFCapabilities;
#else
					StaInfo_p->HtElem.TxBFCapabilities = tmp_p->ExtHTCapabilitiesInfo;
#endif

					StaInfo_p->HtElem.HTCapabilitiesInfo  =  tmp_p->HTCapabilitiesInfo;
					memcpy(&(StaInfo_p->HtElem.SupportedMCSset),&(tmp_p->SupportedMCSset),16);

					if((*(vmacSta_p->Mib802dot11->pMib_11nAggrMode)& WL_MODE_AMSDU_TX_MASK) && 
						!(*(vmacSta_p->Mib802dot11->pMib_11nAggrMode)& WL_MODE_AMPDU_TX))
						StaInfo_p->aggr11n.type |= WL_WLAN_TYPE_AMSDU;
					StaInfo_p->aggr11n.threshold = AGGRTHRESHOLD;
					if(*(vmacSta_p->Mib802dot11->mib_ApMode)&0x4)
						StaInfo_p->ClientMode = NONLY_MODE;
					if(tmp_p->HTCapabilitiesInfo.MaxAMSDUSize)
						StaInfo_p->aggr11n.cap = 2;
					else
						StaInfo_p->aggr11n.cap = 1;
					if( StaInfo_p->aggr11n.cap > (*(vmacSta_p->Mib802dot11->pMib_11nAggrMode)& WL_MODE_AMSDU_TX_MASK))
					{
						StaInfo_p->aggr11n.cap = (*(vmacSta_p->Mib802dot11->pMib_11nAggrMode)& WL_MODE_AMSDU_TX_MASK);
						WLDBG_INFO(DBG_LEVEL_7,"Mismatched Sta HTCapabilitiesInfo.MaxAMSDUSize=%x", tmp_p->HTCapabilitiesInfo.MaxAMSDUSize);
					}
				}
				else if(((IEEEtypes_Generic_Add_HT_Element_t *)VariableElements_p)->OUIType == 52)
				{
					IEEEtypes_Generic_Add_HT_Element_t *tmp_p = (IEEEtypes_Generic_Add_HT_Element_t *)VariableElements_p;

					StaInfo_p->AddHtElme.ElementId=tmp_p->ElementId;
					StaInfo_p->AddHtElme.Len = tmp_p->Len;
					PeerInfo.AddHtInfo.ControlChan = StaInfo_p->AddHtElme.ControlChan = tmp_p->ControlChan;
					PeerInfo.AddHtInfo.AddChan = StaInfo_p->AddHtElme.AddChan = tmp_p->AddChan;
					PeerInfo.AddHtInfo.OpMode = StaInfo_p->AddHtElme.OpMode = tmp_p->OpMode;
					PeerInfo.AddHtInfo.stbc = StaInfo_p->AddHtElme.stbc = tmp_p->stbc;

				}
				VarLen -= (sizeof(IEEEtypes_ElementId_t) + sizeof(IEEEtypes_Len_t) + *(VariableElements_p + 1));  /* value in the length field */
				VariableElements_p += (sizeof(IEEEtypes_ElementId_t) );
				VariableElements_p += *VariableElements_p;
				VariableElements_p += sizeof(IEEEtypes_Len_t);
				break;

			}


			/** FOR I_COMP ONLY, not use anymore **/
			if (!memcmp(&(( IEEEtypes_Generic_HT_Element_t2 *)VariableElements_p)->OUI, I_COMP_OUI , 3))
			{

				if (((IEEEtypes_Generic_HT_Element_t2 *)VariableElements_p)->OUIType == 51)
				{	
					IEEEtypes_Generic_HT_Element_t2 *tmp_p = (IEEEtypes_Generic_HT_Element_t2 *)VariableElements_p;
					HTpresent =1;
					PeerInfo.HTCapabilitiesInfo =  tmp_p->HTCapabilitiesInfo;
					PeerInfo.MacHTParamInfo = tmp_p->MacHTParamInfo;
#ifdef EXPLICIT_BF
					PeerInfo.TxBFCapabilities=tmp_p->TxBFCapabilities;
#endif

					if (*(mib->mib_3x3Rate) == 1)
						PeerInfo.HTRateBitMap = (tmp_p->SupportedMCSset[0] | (tmp_p->SupportedMCSset[1] << 8) |
						(tmp_p->SupportedMCSset[2] << 16) | (tmp_p->SupportedMCSset[3] << 24));
					else
						PeerInfo.HTRateBitMap = (tmp_p->SupportedMCSset[0] | (tmp_p->SupportedMCSset[1] << 8));

					StaInfo_p->HtElem.ElementId = tmp_p->ElementId;
					StaInfo_p->HtElem.Len = tmp_p->Len;
					StaInfo_p->HtElem.MacHTParamInfo = tmp_p->MacHTParamInfo;
					StaInfo_p->HtElem.ASCapabilities = tmp_p->ASCapabilities;
					StaInfo_p->HtElem.ExtHTCapabilitiesInfo = tmp_p->ExtHTCapabilitiesInfo;
#ifdef EXPLICIT_BF
					StaInfo_p->HtElem.TxBFCapabilities = tmp_p->TxBFCapabilities;
#else
					StaInfo_p->HtElem.TxBFCapabilities = tmp_p->ExtHTCapabilitiesInfo;
#endif
					StaInfo_p->HtElem.HTCapabilitiesInfo  =  tmp_p->HTCapabilitiesInfo;
					memcpy(&(StaInfo_p->HtElem.SupportedMCSset),&(tmp_p->SupportedMCSset),16);

					if((*(vmacSta_p->Mib802dot11->pMib_11nAggrMode)& WL_MODE_AMSDU_TX_MASK) && 
						!(*(vmacSta_p->Mib802dot11->pMib_11nAggrMode)& WL_MODE_AMPDU_TX))
						StaInfo_p->aggr11n.type |= WL_WLAN_TYPE_AMSDU;
					StaInfo_p->aggr11n.threshold = AGGRTHRESHOLD;//foo hack 500;
					if(*(vmacSta_p->Mib802dot11->mib_ApMode)&0x4)
						StaInfo_p->ClientMode = NONLY_MODE;
					if(tmp_p->HTCapabilitiesInfo.MaxAMSDUSize)
					{
						StaInfo_p->aggr11n.cap = 2;
						WLDBG_INFO(DBG_LEVEL_7,"Amsdu size=2");
					}
					else
					{
						StaInfo_p->aggr11n.cap = 1;
						WLDBG_INFO(DBG_LEVEL_7,"Amsdu size=1");
					}

					if( StaInfo_p->aggr11n.cap > (*(vmacSta_p->Mib802dot11->pMib_11nAggrMode)& WL_MODE_AMSDU_TX_MASK))
					{
						StaInfo_p->aggr11n.cap = (*(vmacSta_p->Mib802dot11->pMib_11nAggrMode) & WL_MODE_AMSDU_TX_MASK);
						WLDBG_INFO(DBG_LEVEL_7,"Mismatched Sta HTCapabilitiesInfo.MaxAMSDUSize=%x", tmp_p->HTCapabilitiesInfo.MaxAMSDUSize);
					}
				}
				else if(((IEEEtypes_Generic_Add_HT_Element_t2 *)VariableElements_p)->OUIType == 52)
				{
					IEEEtypes_Generic_Add_HT_Element_t2 *tmp_p = (IEEEtypes_Generic_Add_HT_Element_t2 *)VariableElements_p;

					StaInfo_p->AddHtElme.ElementId=tmp_p->ElementId;
					StaInfo_p->AddHtElme.Len = tmp_p->Len;
					PeerInfo.AddHtInfo.ControlChan = StaInfo_p->AddHtElme.ControlChan = tmp_p->ControlChan;
					PeerInfo.AddHtInfo.AddChan = StaInfo_p->AddHtElme.AddChan = tmp_p->AddChan;
					PeerInfo.AddHtInfo.OpMode = StaInfo_p->AddHtElme.OpMode = tmp_p->OpMode;
					PeerInfo.AddHtInfo.stbc = StaInfo_p->AddHtElme.stbc = tmp_p->stbc;

				}
				VarLen -= (sizeof(IEEEtypes_ElementId_t) + sizeof(IEEEtypes_Len_t) + *(VariableElements_p + 1));  /* value in the length field */
				VariableElements_p += (sizeof(IEEEtypes_ElementId_t) );
				VariableElements_p += *VariableElements_p;
				VariableElements_p += sizeof(IEEEtypes_Len_t);
				break;

			}
#endif /* INTEROP */

#ifdef QOS_WSM_FEATURE
			if (!memcmp(&((WSM_QoS_Cap_Elem_t *)VariableElements_p)->OUI, WiFiOUI, 3))
			{
				if (((WSM_QoS_Cap_Elem_t *)VariableElements_p)->OUI.Type == 0x2)
				{	
					if(((WSM_QoS_Cap_Elem_t *)VariableElements_p)->OUI.Subtype == 0)
					{
						//This is a WME Info Element
						pWMEInfoElem = (WME_info_elem_t *)VariableElements_p;
						VariableElements_p += (sizeof(IEEEtypes_ElementId_t) +
							sizeof(IEEEtypes_Len_t) +
							pWMEInfoElem->Len);
						VarLen -= (sizeof(IEEEtypes_ElementId_t) +
							sizeof(IEEEtypes_Len_t) +
							pWMEInfoElem->Len);

					}
					else if(((WSM_QoS_Cap_Elem_t *)VariableElements_p)->OUI.Subtype == 5)
					{
						//this is the WSM QoS Element
						WsmQosCapElem_p = (WSM_QoS_Cap_Elem_t *)VariableElements_p;
						VariableElements_p += (sizeof(IEEEtypes_ElementId_t) +
							sizeof(IEEEtypes_Len_t) +
							WsmQosCapElem_p->Len);
						VarLen -= (sizeof(IEEEtypes_ElementId_t) +
							sizeof(IEEEtypes_Len_t) +
							WsmQosCapElem_p->Len);
					}
					else
					{
						InfoElemHdr_p = (IEEEtypes_InfoElementHdr_t *)VariableElements_p;			 
						VariableElements_p += (sizeof(IEEEtypes_ElementId_t) +
							sizeof(IEEEtypes_Len_t) +
							InfoElemHdr_p->Len);
						VarLen -= (sizeof(IEEEtypes_ElementId_t) +
							sizeof(IEEEtypes_Len_t) +
							InfoElemHdr_p->Len);
					}
					break;
				}
				else if (((IEEEtypes_RSN_IE_t *)VariableElements_p)->OuiType[3] == 0x1)
				{//this is the WPA WiFI Element
					RsnIE_p = (IEEEtypes_RSN_IE_t *)VariableElements_p;
					VariableElements_p += (sizeof(IEEEtypes_ElementId_t) +
						sizeof(IEEEtypes_Len_t) +
						RsnIE_p->Len);
					VarLen -= (sizeof(IEEEtypes_ElementId_t) +
						sizeof(IEEEtypes_Len_t) +
						RsnIE_p->Len);
					break;
				}
				else
				{
					InfoElemHdr_p = (IEEEtypes_InfoElementHdr_t *)VariableElements_p;		     
					VariableElements_p += (sizeof(IEEEtypes_ElementId_t) +
						sizeof(IEEEtypes_Len_t) +
						InfoElemHdr_p->Len);
					VarLen -= (sizeof(IEEEtypes_ElementId_t) +
						sizeof(IEEEtypes_Len_t) +
						InfoElemHdr_p->Len);
				}
			}
			else
			{
				InfoElemHdr_p = (IEEEtypes_InfoElementHdr_t *)VariableElements_p;
				VariableElements_p += (sizeof(IEEEtypes_ElementId_t) +
					sizeof(IEEEtypes_Len_t) +
					InfoElemHdr_p->Len);
				VarLen -= (sizeof(IEEEtypes_ElementId_t) +
					sizeof(IEEEtypes_Len_t) +
					InfoElemHdr_p->Len);
			}
#else
			/*
			RsnIE_p = (IEEEtypes_RSN_IE_t *)VariableElements_p;
			VariableElements_p += (sizeof(IEEEtypes_ElementId_t) +
			sizeof(IEEEtypes_Len_t) +
			RsnIE_p->Len);
			VarLen -= (sizeof(IEEEtypes_ElementId_t) +
			sizeof(IEEEtypes_Len_t) +
			RsnIE_p->Len);
			*/
			if ( memcmp(((IEEEtypes_RSN_IE_t *)VariableElements_p)->OuiType, WPA_OUItype, 4) == 0 ) 
			{
				RsnIE_p = (IEEEtypes_RSN_IE_t *)VariableElements_p;
				VariableElements_p += (sizeof(IEEEtypes_ElementId_t) +
					sizeof(IEEEtypes_Len_t) +
					RsnIE_p->Len);
				VarLen -= (sizeof(IEEEtypes_ElementId_t) +
					sizeof(IEEEtypes_Len_t) +
					RsnIE_p->Len);
			}
			else
			{
				//RsnIE_p = NULL;
				RsnIE_Len = ((IEEEtypes_RSN_IE_t *)VariableElements_p)->Len;
				VariableElements_p += (sizeof(IEEEtypes_ElementId_t) +
					sizeof(IEEEtypes_Len_t) + RsnIE_Len);
				VarLen -= (sizeof(IEEEtypes_ElementId_t) +
					sizeof(IEEEtypes_Len_t) + RsnIE_Len);
			}

#endif
			break;
#ifdef AP_WPA2
		case RSN_IEWPA2:
			RsnIEWPA2_p = (IEEEtypes_RSN_IE_WPA2_t *)VariableElements_p;
			VariableElements_p += (sizeof(IEEEtypes_ElementId_t) +
				sizeof(IEEEtypes_Len_t) +
				RsnIEWPA2_p->Len);
			VarLen -= (sizeof(IEEEtypes_ElementId_t) +
				sizeof(IEEEtypes_Len_t) +
				RsnIEWPA2_p->Len);
			break;
#endif

#ifdef MRVL_WAPI
		case WAPI_IE:
			WAPI_IE_p = (IEEEtypes_WAPI_IE_t *)VariableElements_p;
			VariableElements_p += (sizeof(IEEEtypes_ElementId_t) +
				sizeof(IEEEtypes_Len_t) +
				WAPI_IE_p->Len);
			VarLen -= (sizeof(IEEEtypes_ElementId_t) +
				sizeof(IEEEtypes_Len_t) +
				WAPI_IE_p->Len);
			break;
#endif

#ifdef IEEE80211H
		case PWR_CAP:
			PowerCapability_p = (IEEEtypes_PowerCapabilityElement_t *)VariableElements_p;
			VariableElements_p += (sizeof(IEEEtypes_ElementId_t) +
				sizeof(IEEEtypes_Len_t) +
				PowerCapability_p->Len);
			VarLen -= (sizeof(IEEEtypes_ElementId_t) +
				sizeof(IEEEtypes_Len_t) +
				PowerCapability_p->Len);
			break;

		case SUPPORTED_CHANNEL:
			SupportedChanne_p = (IEEEtypes_SupportedChannelElement_t *)VariableElements_p;
			VariableElements_p += (sizeof(IEEEtypes_ElementId_t) +
				sizeof(IEEEtypes_Len_t) +
				SupportedChanne_p->Len);
			VarLen -= (sizeof(IEEEtypes_ElementId_t) +
				sizeof(IEEEtypes_Len_t) +
				SupportedChanne_p->Len);
			break;            
#endif /* IEEE80211H */
		case HT:
			{
				IEEEtypes_HT_Element_t *tmp_p = (IEEEtypes_HT_Element_t *)VariableElements_p;
				HTpresent =1;
				PeerInfo.HTCapabilitiesInfo = tmp_p->HTCapabilitiesInfo;
				PeerInfo.MacHTParamInfo = tmp_p->MacHTParamInfo;
#ifdef EXPLICIT_BF
				PeerInfo.TxBFCapabilities=tmp_p->TxBFCapabilities;
#endif
				if (*(mib->mib_3x3Rate) == 1 && (( * (mib->mib_rxAntenna) ==  0) || (*(mib->mib_rxAntenna) ==  3)))
				{
					/** 3x3 configuration **/
					PeerInfo.HTRateBitMap = (tmp_p->SupportedMCSset[0] | (tmp_p->SupportedMCSset[1] << 8) |
						(tmp_p->SupportedMCSset[2] << 16) | (tmp_p->SupportedMCSset[3] << 24));
				}
				else if(!((*(mib->mib_rxAntenna) == 0) || (*(mib->mib_rxAntenna) ==  3) ||(*(mib->mib_rxAntenna) ==  2)))
				{
					/** 1x1 configuration **/
					PeerInfo.HTRateBitMap = (tmp_p->SupportedMCSset[0]);
				}
				else
				{
					PeerInfo.HTRateBitMap = (tmp_p->SupportedMCSset[0] | (tmp_p->SupportedMCSset[1] << 8));
				}


				memcpy(&(StaInfo_p->HtElem), tmp_p, sizeof(IEEEtypes_HT_Element_t)); 
				if((*(vmacSta_p->Mib802dot11->pMib_11nAggrMode)& WL_MODE_AMSDU_TX_MASK) && 
					!(*(vmacSta_p->Mib802dot11->pMib_11nAggrMode)& WL_MODE_AMPDU_TX))
					StaInfo_p->aggr11n.type |= WL_WLAN_TYPE_AMSDU;
				StaInfo_p->aggr11n.threshold = AGGRTHRESHOLD;
				if(*(vmacSta_p->Mib802dot11->mib_ApMode)&0x4)
					StaInfo_p->ClientMode = NONLY_MODE;
				if(tmp_p->HTCapabilitiesInfo.MaxAMSDUSize)
					StaInfo_p->aggr11n.cap = 2;
				else
					StaInfo_p->aggr11n.cap = 1;
				if( StaInfo_p->aggr11n.cap > (*(vmacSta_p->Mib802dot11->pMib_11nAggrMode) &WL_MODE_AMSDU_TX_MASK))
				{
					StaInfo_p->aggr11n.cap = (*(vmacSta_p->Mib802dot11->pMib_11nAggrMode)&WL_MODE_AMSDU_TX_MASK);
					WLDBG_INFO(DBG_LEVEL_7,"Mismatched Sta HTCapabilitiesInfo.MaxAMSDUSize=%x", tmp_p->HTCapabilitiesInfo.MaxAMSDUSize);
				}
				StaInfo_p->PeerHTCapabilitiesInfo = tmp_p->HTCapabilitiesInfo;
				vmacSta_p->NonGFSta = 0;
				if (!tmp_p->HTCapabilitiesInfo.GreenField)
					vmacSta_p->NonGFSta++;	
			}
			VarLen -= (sizeof(IEEEtypes_ElementId_t) +
				sizeof(IEEEtypes_Len_t) +
				*(VariableElements_p + 1));  /* value in the length field */
			VariableElements_p += (sizeof(IEEEtypes_ElementId_t) );
			VariableElements_p += *VariableElements_p;
			VariableElements_p += sizeof(IEEEtypes_Len_t);
			break;
		case ADD_HT:
			{
				IEEEtypes_Add_HT_Element_t *tmp_p = (IEEEtypes_Add_HT_Element_t *)VariableElements_p;
				memcpy(&(StaInfo_p->AddHtElme), tmp_p, sizeof(IEEEtypes_Add_HT_Element_t)); 
				PeerInfo.AddHtInfo.ControlChan = tmp_p->ControlChan;
				PeerInfo.AddHtInfo.AddChan = tmp_p->AddChan;
				PeerInfo.AddHtInfo.OpMode = tmp_p->OpMode;
				PeerInfo.AddHtInfo.stbc = tmp_p->stbc;
				ht_RxChannelWidth_IE61 = tmp_p->AddChan.STAChannelWidth;	
			}
			VarLen -= (sizeof(IEEEtypes_ElementId_t) +
				sizeof(IEEEtypes_Len_t) +
				*(VariableElements_p + 1));  /* value in the length field */
			VariableElements_p += (sizeof(IEEEtypes_ElementId_t) );
			VariableElements_p += *VariableElements_p;
			VariableElements_p += sizeof(IEEEtypes_Len_t);
			break;
		//TODO: need more handling logic for IE 191&192
#if defined(SOC_W8864)	
		case VHT_CAP:
            if(*(vmacSta_p->Mib802dot11->mib_ApMode)&AP_MODE_11AC)
			{
				IEEEtypes_VhtCap_t *tmp_p = (IEEEtypes_VhtCap_t *)VariableElements_p;
                memcpy((void *)&StaInfo_p->vhtCap, tmp_p, sizeof(IEEEtypes_VhtCap_t));
				memcpy((UINT8 *)&PeerInfo.vht_cap, (UINT8 *)&tmp_p->cap, sizeof(IEEEtypes_VHT_Cap_Info_t));	
				PeerInfo.vht_MaxRxMcs = tmp_p->SupportedRxMcsSet;
			}
    		IE191Present = 1;
			vhtcap = *((UINT32*)&StaInfo_p->vhtCap.cap);	
    		printk("Received IE 191!! - vht_Cap = %x, vht_MaxRxMcs = %x\n", (unsigned int)vhtcap, (u_int32_t)PeerInfo.vht_MaxRxMcs);
			VarLen -= (sizeof(IEEEtypes_ElementId_t) +
				sizeof(IEEEtypes_Len_t) +
				*(VariableElements_p + 1));  /* value in the length field */
			VariableElements_p += (sizeof(IEEEtypes_ElementId_t) );
			VariableElements_p += *VariableElements_p;
			VariableElements_p += sizeof(IEEEtypes_Len_t);
    		break;
		case VHT_OPERATION:
			{	
				IEEEtypes_VhOpt_t *tmp_p = (IEEEtypes_VhOpt_t *)VariableElements_p;
				vht_RxChannelWidth_IE192= tmp_p->ch_width;
			}
    		IE192Present = 1;
			VarLen -= (sizeof(IEEEtypes_ElementId_t) +
				sizeof(IEEEtypes_Len_t) +
				*(VariableElements_p + 1));  /* value in the length field */
			VariableElements_p += (sizeof(IEEEtypes_ElementId_t) );
			VariableElements_p += *VariableElements_p;
			VariableElements_p += sizeof(IEEEtypes_Len_t);
    		break;
		case OP_MODE_NOTIFICATION:
			{	
				IEEEtypes_VHT_op_mode_t *tmp_p = (IEEEtypes_VHT_op_mode_t *)VariableElements_p;
				if(tmp_p->OperatingMode.RxNssType == 0){
					vht_RxChannelWidth_IE199 = tmp_p->OperatingMode.ChannelWidth;
					vht_RxNss_IE199 = tmp_p->OperatingMode.RxNss + 1;		//In IE199, 0:Nss1, 1:Nss2....So we plus 1 to become 1:Nss1, 2:Nss2
				}
				/*Beamforming related matters*/
				else{
					/*TODO: Hard coded for now to pass wifi 7/19/2013*/
					vht_RxChannelWidth_IE199 = 2; 	//0:20Mhz, 1:40Mhz, 2:80Mhz, 3:160 or 80+80Mhz
					vht_RxNss_IE199 = 3; 			//1:1Nss, 2:2Nss, 3:3Nss
				} 
			}
			IE199Present = 1;
			VarLen -= (sizeof(IEEEtypes_ElementId_t) +
				sizeof(IEEEtypes_Len_t) +
				*(VariableElements_p + 1));  /* value in the length field */
			VariableElements_p += (sizeof(IEEEtypes_ElementId_t) );
			VariableElements_p += *VariableElements_p;
			VariableElements_p += sizeof(IEEEtypes_Len_t);
			break;			
#endif    		
		default:
			VarLen -= (sizeof(IEEEtypes_ElementId_t) +
				sizeof(IEEEtypes_Len_t) +
				*(VariableElements_p + 1));  /* value in the length field */
			VariableElements_p += (sizeof(IEEEtypes_ElementId_t) );
			VariableElements_p += *VariableElements_p;
			VariableElements_p += sizeof(IEEEtypes_Len_t);
			break;
		}
	}
	
	StaInfo_p->aggr11n.thresholdBackUp = StaInfo_p->aggr11n.threshold;

	
	/*If IE199 is present, we use its channel width and Nss info to update peer info*/
	if(IE199Present){
		PeerInfo.vht_RxChannelWidth = vht_RxChannelWidth_IE199;		 

		/*Determine peer no. of Rx Nss in IE192
		* bit0-1: Nss1, bit2-3: Nss2, ..., bit14-15: Nss8. 0x3 means not supported for the Nss
		*/
		if((PeerInfo.vht_MaxRxMcs&0xc000) != 0xc000)
			vht_peer_RxNss = 8;
		else if((PeerInfo.vht_MaxRxMcs&0x3000) != 0x3000)
    	    vht_peer_RxNss = 7;	
		else if((PeerInfo.vht_MaxRxMcs&0xc00) != 0xc00)
			vht_peer_RxNss = 6;
		else if((PeerInfo.vht_MaxRxMcs&0x300) != 0x300)
    	    vht_peer_RxNss = 5;	
		else if((PeerInfo.vht_MaxRxMcs&0xc0) != 0xc0)
			vht_peer_RxNss = 4;		
		else if((PeerInfo.vht_MaxRxMcs&0x30) != 0x30)
    	    vht_peer_RxNss = 3;		
	    else if((PeerInfo.vht_MaxRxMcs&0xc) != 0xc)	    
            vht_peer_RxNss = 2;		
	    
		/*If IE199 Rx Nss > IE192 Nss, we copy two bits from IE192 Nss1 to the new Nss bits*/
		if(vht_peer_RxNss <  vht_RxNss_IE199)			
		{	
			for(; vht_peer_RxNss < vht_RxNss_IE199; vht_peer_RxNss++)
			{
				vht_RxNss1 = (UINT16)PeerInfo.vht_MaxRxMcs & 0x3;		//Copy Nss1 bits and use it in new Nss	
				vht_RxNss1 = (vht_RxNss1 << (vht_peer_RxNss*2));

				vht_RxNssMask = ~(0x3 << (vht_peer_RxNss*2));			//Set new Nss bits to zero
				PeerInfo.vht_MaxRxMcs = PeerInfo.vht_MaxRxMcs & vht_RxNssMask;
				PeerInfo.vht_MaxRxMcs = PeerInfo.vht_MaxRxMcs | vht_RxNss1;
			}
		}
		else if (vht_peer_RxNss >  vht_RxNss_IE199)
		{	
			for(; vht_peer_RxNss > vht_RxNss_IE199; vht_peer_RxNss--)
			{
				vht_RxNssMask = 0xc000 >> ((8-vht_peer_RxNss)*2);
				PeerInfo.vht_MaxRxMcs = PeerInfo.vht_MaxRxMcs | vht_RxNssMask;
			}
		}
	}
	else{
		/*If IE192 channel width bit is 0, we use IE61 channel width bit to decide 20 or 40Mhz (11ac standard section 10.39.1)*/
		if(IE192Present){
			switch(vht_RxChannelWidth_IE192)
			{
				case 0:
					if(ht_RxChannelWidth_IE61 == 1)
						PeerInfo.vht_RxChannelWidth = 1;
					else 
						PeerInfo.vht_RxChannelWidth = 0;
					break;
				case 1:
					PeerInfo.vht_RxChannelWidth = 2;
					break;
				case 2:
				case 3:
					PeerInfo.vht_RxChannelWidth = 3;
					break;
				default:
					PeerInfo.vht_RxChannelWidth = 2;
					break;
			}
		}
		else
		{
			/*In 2G, we check HT cap to decide peer bandwidth. Having VHT cap info not necessarily means can support 80 or 40MHz*/
			if(PhyDSSSTable->Chanflag.FreqBand==FREQ_BAND_2DOT4GHZ)		
			{	
				if(PeerInfo.HTCapabilitiesInfo.SupChanWidth)
					PeerInfo.vht_RxChannelWidth = 1;
				else
					PeerInfo.vht_RxChannelWidth = 0;		
			}
			else
				PeerInfo.vht_RxChannelWidth = 2;	//Set to 80Mhz if VHT client doesn't have IE192. HT client is not using vht_RxChannelWidth to determine its operating bw 
		}
	}


	/* if not Marvell station and Wep encryption then turn off aggregation  */
	if (!StaInfo_p->IsStaMSTA && mib_PrivacyTable_p->PrivInvoked)
	{
		if(StaInfo_p->aggr11n.threshold)
		{
			StaInfo_p->aggr11n.threshold = 0;
			StaInfo_p->aggr11n.thresholdBackUp = StaInfo_p->aggr11n.threshold;
		}
	}

#ifdef WFA_TKIP_NEGATIVE
	/* If HT STA and AP mode is WPA-TKIP or WPA-AES, reject the association request */
	if ((HTpresent) && ((*(vmacSta_p->Mib802dot11->mib_wpaWpa2Mode) & 0x0F) == 1))
	{
		MgmtRsp_p->Body.AssocRsp.StatusCode =
			IEEEtypes_STATUS_CAPS_UNSUPPORTED;
		/* Send for tx */
		if (txMgmtMsg(vmacSta_p->dev,txSkb_p) != OS_SUCCESS )
			dev_kfree_skb_any(txSkb_p);
		return ;
	}

	/* If HT STA and AP mode is mixed WPA2-AES and WPA-TKIP and 
	if the STA associates as WPA-TKIP reject the request */
	if ((HTpresent) && 
		(((*(vmacSta_p->Mib802dot11->mib_wpaWpa2Mode) & 0x0F) == 3) ||((*(vmacSta_p->Mib802dot11->mib_wpaWpa2Mode) & 0x0F) == 6)) && 
		(RsnIE_p != NULL) && (RsnIE_p->PwsKeyCipherList[3] == RSN_TKIP_ID))
	{
		MgmtRsp_p->Body.AssocRsp.StatusCode = IEEEtypes_STATUS_CAPS_UNSUPPORTED;
		/* Send for tx */
		if (txMgmtMsg(vmacSta_p->dev,txSkb_p) != OS_SUCCESS )
			dev_kfree_skb_any(txSkb_p);
		return ;
	}
#endif

	if ((!HTpresent) 
        && ((*(vmacSta_p->Mib802dot11->mib_ApMode) ==AP_MODE_N_ONLY)
            ||(*(vmacSta_p->Mib802dot11->mib_ApMode) ==AP_MODE_5GHZ_N_ONLY)))
	{
		MgmtRsp_p->Body.AssocRsp.StatusCode =
			IEEEtypes_STATUS_CAPS_UNSUPPORTED;
		/* Send for tx */
		if (txMgmtMsg(vmacSta_p->dev,txSkb_p) != OS_SUCCESS )
			dev_kfree_skb_any(txSkb_p);
		return ;
	}

	if (!(isSsIdMatch(SsId_p, &vmacSta_p->macSsId) ==
		OS_SUCCESS || isSsIdMatch(SsId_p, &vmacSta_p->macSsId2) ==
		OS_SUCCESS))
	{
		/* Build ASSOC_RSP msg with REFUSED status code */
		MgmtRsp_p->Body.AssocRsp.StatusCode =
			IEEEtypes_STATUS_ASSOC_DENIED_UNSPEC;
		/* Send for tx */
		if (txMgmtMsg(vmacSta_p->dev,txSkb_p) != OS_SUCCESS )
			dev_kfree_skb_any(txSkb_p);
		return ;
	}
	if (isCapInfoSupported(&MgmtMsg_p->Body.AssocRqst.CapInfo,
		&vmacSta_p->macCapInfo) != OS_SUCCESS )
	{
		/* Build ASSOC_RSP msg with cannot supp cap info (code 10) */
		MgmtRsp_p->Body.AssocRsp.StatusCode =
			IEEEtypes_STATUS_CAPS_UNSUPPORTED;

		/* Send for tx */
		if (txMgmtMsg(vmacSta_p->dev,txSkb_p) != OS_SUCCESS )
			dev_kfree_skb_any(txSkb_p);
		return ;
	}

	PeerInfo.CapInfo = MgmtMsg_p->Body.AssocRqst.CapInfo;

	if (Rates_p)
	{
		PeerInfo.LegacyRateBitMap = GetLegacyRateBitMap(vmacSta_p, Rates_p, ExtRates_p);
		HighestRate = GetHighestRateIndex(vmacSta_p,Rates_p, ExtRates_p,&gRatePresent); 
	}
	else
	{
		/* Build ASSOC_RSP msg with basic rates not supported */
		MgmtRsp_p->Body.AssocRsp.StatusCode =
			IEEEtypes_STATUS_ASSOC_DENIED_RATES;

		/* Send for tx */
		if (txMgmtMsg(vmacSta_p->dev,txSkb_p) != OS_SUCCESS )
			dev_kfree_skb_any(txSkb_p);
		return ;
	}

	if(StaInfo_p->ClientMode == NONLY_MODE)
	{
		//StaInfo_p->ClientMode = NONLY_MODE;
	}
	else if ( !gRatePresent )
	{
		StaInfo_p->ClientMode = BONLY_MODE;
	}
	else
	{
		if ( *(mib->mib_ApMode) == AP_MODE_B_ONLY )
		{
			StaInfo_p->ClientMode = BONLY_MODE;
		}
		else if (*(mib->mib_ApMode) == AP_MODE_A_ONLY)
		{
			StaInfo_p->ClientMode = AONLY_MODE;
		}
		else if (*(mib->mib_ApMode)==AP_MODE_G_ONLY)
		{
			StaInfo_p->ClientMode = GONLY_MODE;
		}        
		else if ((*(mib->mib_ApMode)==AP_MODE_BandGandN) ||
#ifdef SOC_W8864		
		    (*(mib->mib_ApMode) == AP_MODE_2_4GHZ_11AC_MIXED) ||
#endif		    
			(*(mib->mib_ApMode)==AP_MODE_MIXED)  ||
			(*(mib->mib_ApMode)==AP_MODE_GandN))
		{
			StaInfo_p->ClientMode = GONLY_MODE;
		}  
		else if ((*(mib->mib_ApMode)==AP_MODE_AandN) ||		
#ifdef SOC_W8864	
    		(*(mib->mib_ApMode) == AP_MODE_5GHZ_Nand11AC) ||
    		(*(mib->mib_ApMode) == AP_MODE_5GHZ_11AC_ONLY)
#endif    		
    		)
		{
			StaInfo_p->ClientMode = AONLY_MODE;
		}

		else 
		{
			StaInfo_p->ClientMode = MIXED_MODE;
		}
	}
	FixedRateCtl(StaInfo_p, &PeerInfo, mib);

	if ((!gRatePresent && (*(mib->mib_ApMode) == AP_MODE_G_ONLY)) || (gRatePresent && (*(mib->mib_ApMode) == AP_MODE_B_ONLY))
#ifdef BRS_SUPPORT
		|| (!PeerInfo.LegacyRateBitMap)
#endif
		)
	{
		/* Build ASSOC_RSP msg with basic rates not supported */
		MgmtRsp_p->Body.AssocRsp.StatusCode =
			IEEEtypes_STATUS_ASSOC_DENIED_RATES;

		/* Send for tx */
		if (txMgmtMsg(vmacSta_p->dev,txSkb_p) != OS_SUCCESS )
			dev_kfree_skb_any(txSkb_p);
		return ;
	}

	if (!gRatePresent && (*(mib->mib_ApMode) == AP_MODE_G_ONLY || 
	                      *(mib->mib_ApMode)==AP_MODE_A_ONLY || 
	                      *(mib->mib_ApMode)==AP_MODE_AandN || 
#ifdef SOC_W8864	
	                      *(mib->mib_ApMode) == AP_MODE_5GHZ_Nand11AC ||
	                      *(mib->mib_ApMode) == AP_MODE_5GHZ_11AC_ONLY)
#endif	                      
                          )
	{
		/* Build ASSOC_RSP msg with basic rates not supported */
		MgmtRsp_p->Body.AssocRsp.StatusCode =
			IEEEtypes_STATUS_ASSOC_DENIED_RATES;

		/* Send for tx */
		if (txMgmtMsg(vmacSta_p->dev,txSkb_p) != OS_SUCCESS )
			dev_kfree_skb_any(txSkb_p);
		return ;
	}
#ifdef IEEE80211H
	if ((*(mib->mib_ApMode) == AP_MODE_A_ONLY) ||
		((*(mib->mib_ApMode) == AP_MODE_AandG) && (StaInfo_p->ApMode == AONLY_MODE)))
		StaInfo_p->IsSpectrumMgmt = (MgmtMsg_p->Body.AssocRqst.CapInfo.SpectrumMgmt)?TRUE:FALSE;
	else
		StaInfo_p->IsSpectrumMgmt = FALSE; /* stations are not in A mode */

	/* CH 11.5 */
	if (StaInfo_p->IsSpectrumMgmt == TRUE)         
	{
        if (!dfs_test_mode)
        {
            /* Ignore for dfs testing - Veriwave does not have thest elements, need to tell Veriwave. */      
		    /* if station lacks two IEs */
		    if ((PowerCapability_p == NULL)||(SupportedChanne_p == NULL))
		    {
			    /* Build ASSOC_RSP msg with status code accordingly */
			    if (PowerCapability_p == NULL)
				    MgmtRsp_p->Body.AssocRsp.StatusCode = IEEEtypes_STATUS_ASSOC_PWE_CAP_REQUIRED;
			    else
				    MgmtRsp_p->Body.AssocRsp.StatusCode = IEEEtypes_STATUS_ASSOC_SUP_CHA_REQUIRED;;

			    /* Send for tx */
			    if (txMgmtMsg(vmacSta_p->dev,txSkb_p) != OS_SUCCESS )
				    dev_kfree_skb_any(txSkb_p);
			    return ;
		    }            

        }
	}
#endif /* IEEE80211H */

#ifdef MRVL_WSC
	/* WPS MSFT Patch require that the AP does not object if the station 
	* does not include RSN IE even when the WPA(2) is enabled.
	*/
	if ( (mib_PrivacyTable_p->RSNEnabled && !mib_RSNConfigWPA2_p->WPA2Enabled && !mib_RSNConfigWPA2_p->WPA2OnlyEnabled
		&& (RsnIE_p != NULL) && (memcmp(RsnIE_p->OuiType, 
		&(mib->thisStaRsnIE->OuiType), mib->thisStaRsnIE->Len) != 0))
		|| (vmacSta_p->WPSOn == 0 && mib_RSNConfigWPA2_p->WPA2Enabled && (RsnIEWPA2_p == NULL && RsnIE_p == NULL))
		|| (vmacSta_p->WPSOn == 0 && mib_RSNConfigWPA2_p->WPA2OnlyEnabled && (RsnIEWPA2_p == NULL)))
	{
		// Build ASSOC_RSP msg with reason code
		MgmtRsp_p->Body.AssocRsp.StatusCode = IEEEtypes_REASON_INVALID_IE;
		/* Send for tx */
		if (txMgmtMsg(vmacSta_p->dev,txSkb_p) != OS_SUCCESS )
			dev_kfree_skb_any(txSkb_p);
		return ;
	}
#endif //MRVL_WSC

	if (mib_PrivacyTable_p->RSNEnabled)
	{
		if (vmacSta_p->MIC_ErrordisableStaAsso)
		{
			MgmtRsp_p->Body.AssocRsp.StatusCode = IEEEtypes_REASON_MIC_FAILURE;

			/* Send for tx */
			if (txMgmtMsg(vmacSta_p->dev,txSkb_p) != OS_SUCCESS )
				dev_kfree_skb_any(txSkb_p);
			return;
		}

			pStaMsg = &pDistMsg->msg.StaAssocStateMsg;

			pDistMsg->MsgType = STA_ASSOMSGRECVD;
			memcpy( pStaMsg->staMACAddr, MgmtMsg_p->Hdr.SrcAddr, 6 );
			pStaMsg->assocType = WPAEVT_STA_ASSOCIATED;

		// Init keyMgmt DB
		memset(&StaInfo_p->keyMgmtStateInfo, 0, sizeof(keyMgmtInfo_t));
		// save the RSN IE into ext Sta DB
		if (mib_RSNConfigWPA2_p->WPA2OnlyEnabled)
		{
			if (RsnIEWPA2_p != NULL)
			{
				memset(StaInfo_p->keyMgmtStateInfo.RsnIEBuf, 0, MAX_SIZE_RSN_IE_BUF);
				if((RsnIEWPA2_p->Len + 2) >MAX_SIZE_RSN_IE_BUF)
					OUT_OF_BOUNDARDY_MESSAGE((RsnIEWPA2_p->Len + 2),MAX_SIZE_RSN_IE_BUF);
				else
				memcpy(StaInfo_p->keyMgmtStateInfo.RsnIEBuf, RsnIEWPA2_p, RsnIEWPA2_p->Len + 2);
			
                /* If only WPA2 is enabled check that the group cipher is AES */
                if (RsnIEWPA2_p->GrpKeyCipher[3] != RSN_AES_ID)
                {
    	    		MgmtRsp_p->Body.AssocRsp.StatusCode = IEEEtypes_STATUS_ASSOC_DENIED_INVALID_GRP_CIPHER;

                    if (txMgmtMsg(vmacSta_p->dev,txSkb_p) != OS_SUCCESS )
		            	dev_kfree_skb_any(txSkb_p);
			         return;
                }
			}
		}
		else if (mib_RSNConfigWPA2_p->WPA2Enabled)
		{
			if (RsnIEWPA2_p != NULL)
			{
				memset(StaInfo_p->keyMgmtStateInfo.RsnIEBuf, 0, MAX_SIZE_RSN_IE_BUF);
				if((RsnIEWPA2_p->Len + 2) >MAX_SIZE_RSN_IE_BUF)
					OUT_OF_BOUNDARDY_MESSAGE((RsnIEWPA2_p->Len + 2),MAX_SIZE_RSN_IE_BUF);
				else
				memcpy(StaInfo_p->keyMgmtStateInfo.RsnIEBuf, RsnIEWPA2_p, RsnIEWPA2_p->Len + 2);
                /* In mixed mode is STA is associating as a WPA2 STA check that group cipher is TKIP */
                if (RsnIEWPA2_p->GrpKeyCipher[3] != RSN_TKIP_ID)
                {
        			MgmtRsp_p->Body.AssocRsp.StatusCode = IEEEtypes_STATUS_ASSOC_DENIED_INVALID_GRP_CIPHER;
    	    		if (txMgmtMsg(vmacSta_p->dev,txSkb_p) != OS_SUCCESS )
    		        	dev_kfree_skb_any(txSkb_p);
    			    return;
                }
			}
			else if (RsnIE_p != NULL)        
			{
				memset(StaInfo_p->keyMgmtStateInfo.RsnIEBuf, 0, MAX_SIZE_RSN_IE_BUF);
				if((RsnIE_p->Len + 2) >MAX_SIZE_RSN_IE_BUF)
					OUT_OF_BOUNDARDY_MESSAGE((RsnIE_p->Len + 2),MAX_SIZE_RSN_IE_BUF);
				else
				memcpy(StaInfo_p->keyMgmtStateInfo.RsnIEBuf, RsnIE_p, RsnIE_p->Len + 2);
                /* In mixed mode is STA is associating as a WPA STA check that unicast cipher is TKIP and group is TKIP */
                if (RsnIE_p->GrpKeyCipher[3] != RSN_TKIP_ID)
                {
        			MgmtRsp_p->Body.AssocRsp.StatusCode = IEEEtypes_STATUS_ASSOC_DENIED_INVALID_GRP_CIPHER;
    	    		if (txMgmtMsg(vmacSta_p->dev,txSkb_p) != OS_SUCCESS )
    		        	dev_kfree_skb_any(txSkb_p);
    			    return;
                }
			}
		}
		else
		{
			if (RsnIE_p != NULL)
			{
				memset(StaInfo_p->keyMgmtStateInfo.RsnIEBuf, 0, MAX_SIZE_RSN_IE_BUF);
				if((RsnIE_p->Len + 2) >MAX_SIZE_RSN_IE_BUF)
					OUT_OF_BOUNDARDY_MESSAGE((RsnIE_p->Len + 2),MAX_SIZE_RSN_IE_BUF);
				else
				memcpy(StaInfo_p->keyMgmtStateInfo.RsnIEBuf, RsnIE_p, RsnIE_p->Len + 2);
			}
		}
	}

	/* Assign Aid */
	if (StaInfo_p->Aid)
	{
		Aid = StaInfo_p->Aid;
	}
	else
	{
		Aid = AssignAid();
		ChangeAssocParam = TRUE;
	}

	/********************************************/
	MgmtRsp_p->Body.AssocRsp.AId = Aid | AID_PREFIX;
	MgmtRsp_p->Body.AssocRsp.StatusCode =
		IEEEtypes_STATUS_SUCCESS;
	StaInfo_p->State = ASSOCIATED;
	StaInfo_p->Aid = Aid;


#ifdef AMPDU_SUPPORT
	free_any_pending_ampdu_pck(vmacSta_p->dev, Aid);
	for(i=0;i<3;i++)  /** Reset the ampdu reorder pck anyway **/
		wlpptr->wlpd_p->AmpduPckReorder[Aid].AddBaReceive[i]=FALSE;  /** clear Ba flag **/
	for(i=0;i<8;i++)
	{
		StaInfo_p->aggr11n.onbytid[i]=0;
		StaInfo_p->aggr11n.startbytid[i]=0;
	}

#ifdef AMPDU_SUPPORT_SBA
	disableAmpduTx(vmacSta_p,(UINT8 *)&MgmtMsg_p->Hdr.SrcAddr,i);
#endif /* _AMPDU_SUPPORT_SBA */
#endif /* _AMPDU_SUPPORT */

	if(pWMEInfoElem != NULL &&  *(mib->QoSOptImpl))
	{
#ifdef UAPSD_SUPPORT
		isQosSta=1;
#endif

		if(skb_tailroom(txSkb_p) > WME_PARAM_LEN+2)
		{
			//We have enough room at the tail
			pWMEParamElem=(WME_param_elem_t *)skb_put(txSkb_p, WME_PARAM_LEN+2);
			QoS_AppendWMEParamElem(vmacSta_p,(UINT8 *)pWMEParamElem);
		}
		else if(skb_headroom(txSkb_p) > WME_PARAM_LEN+2)
		{
			//we have enough at the head
			memcpy(&tempbuffer[0],&txSkb_p->data[0],txSkb_p->len);
			pWMEParamElem = (WME_param_elem_t *)&tempbuffer[txSkb_p->len];
			QoS_AppendWMEParamElem(vmacSta_p,(UINT8 *)pWMEParamElem);
			memset(&txSkb_p->data[0],0, txSkb_p->len);
			skb_push(txSkb_p, WME_PARAM_LEN+2);
			memcpy(&txSkb_p->data[0], &tempbuffer[0], txSkb_p->len);
			extStaDb_SetQoSOptn(vmacSta_p,&MgmtMsg_p->Hdr.SrcAddr, 1);
		}
		else
		{
			WLDBG_INFO(DBG_LEVEL_7, "panic!!!!");

		}
		extStaDb_SetQoSOptn(vmacSta_p,&MgmtMsg_p->Hdr.SrcAddr, 1);
	}
	else
	{
		extStaDb_SetQoSOptn(vmacSta_p,&MgmtMsg_p->Hdr.SrcAddr, 0);
	}


#if 1 /* The else portion of code was coded such that all capabilities of assocresp 
	are following client instead of AP capability. The 'else' code is not used 
	for now due to unknown interop risk. The code was added per bug req 16029 by AE 
	which later informed as not needed on 3/12/07 */

	/** Add Additional HT **/
	/* If client include IE45, following code should be involved, IE45 and IE61 should be added */
#ifdef COEXIST_20_40_SUPPORT
	if(*(vmacSta_p->ShadowMib802dot11->mib_HT40MIntoler))
	{
		extern void Coexist_RearmTimer(vmacApInfo_t *vmacSta_p);
		extern void  Check20_40_Channel_switch(int option, int * mode);
		MIB_PHY_DSSS_TABLE *PhyDSSSTable=mib->PhyDSSSTable;

		if(PhyDSSSTable->Chanflag.FreqBand==FREQ_BAND_2DOT4GHZ
			&&  (PhyDSSSTable->Chanflag.ChnlWidth == CH_AUTO_WIDTH || 
			PhyDSSSTable->Chanflag.ChnlWidth == CH_40_MHz_WIDTH))

		{


			if(PeerInfo.HTCapabilitiesInfo.FortyMIntolerant)
				Handle20_40_Channel_switch(vmacSta_p,0);

			/** start coexisttimer when one 40MHz client is connect **/
			/** this cover the case where initial 20/40 switch happen during AP scanning **/

			if(vmacSta_p->n40MClients==1 && StaInfo_p->HtElem.HTCapabilitiesInfo.SupChanWidth)  /** start timer only if there is 11n 40M sta **/
			{

				if((*(mib->USER_ChnlWidth )&0x0f)==0)  /** we are at 20MHz mode **/
				{
					Coexist_RearmTimer(vmacSta_p);  /** restart timer, sta already reported intolerant **/
				}

			}
		}
	}
#endif

	if (HTpresent)
	{      			
		/** Add HT IE45 **/
		if(skb_tailroom(txSkb_p) > 26 /** len of add generic ht **/ + 2)
		{
			//We have enough room at the tail
			pHTIE = (IEEEtypes_HT_Element_t *)skb_put(txSkb_p, 26 /** len of generic ht **/+2);
			memset((void *)pHTIE, 0, 26 /** len of generic ht **/+2);
			//Add_Generic_AddHT_IE(pAddHTGenericIE);
			AddHT_IE(vmacSta_p,pHTIE);
		}
		else if(skb_headroom(txSkb_p) > 26 /** len of generic ht **/ + 2)
		{
			//we have enough at the head
			memcpy(&tempbuffer[0],&txSkb_p->data[0],txSkb_p->len);
			pHTIE = (IEEEtypes_HT_Element_t *)&tempbuffer[txSkb_p->len];
			//Add_Generic_AddHT_IE(pHTGenericIE);
			AddHT_IE(vmacSta_p,pHTIE);
			memset(&txSkb_p->data[0],0, txSkb_p->len);
			skb_push(txSkb_p,26 /** len of generic ht **/+2);
			memcpy(&txSkb_p->data[0], &tempbuffer[0], txSkb_p->len);			
		}
		else
		{
			WLDBG_INFO(DBG_LEVEL_7, "panic!!!!in interop buffer alloc");
		}

		/** Add Additional HT IE61 **/
		if(skb_tailroom(txSkb_p) > 22 /** len of add generic ht **/ + 2)
		{

			//We have enough room at the tail
			pAddHTIE = (IEEEtypes_Add_HT_Element_t *)skb_put(txSkb_p, 22 /** len of generic ht **/+2);
			memset((void *)pAddHTIE, 0, 22 /** len of generic ht **/+2);
			AddAddHT_IE(vmacSta_p,pAddHTIE);

		}
		else if(skb_headroom(txSkb_p) > 22 /** len of generic ht **/ + 2)
		{
			//we have enough at the head
			memcpy(&tempbuffer[0],&txSkb_p->data[0],txSkb_p->len);
			pAddHTIE = (IEEEtypes_Add_HT_Element_t *)&tempbuffer[txSkb_p->len];
			//Add_Generic_AddHT_IE(pHTGenericIE);
			AddAddHT_IE(vmacSta_p,pAddHTIE);
			memset(&txSkb_p->data[0],0, txSkb_p->len);
			skb_push(txSkb_p,22 /** len of generic ht **/+2);
			memcpy(&txSkb_p->data[0], &tempbuffer[0], txSkb_p->len);
		}
		else
		{
			WLDBG_INFO(DBG_LEVEL_7, "panic!!!!in interop buffer alloc");
		}

#ifdef COEXIST_20_40_SUPPORT
		if(*(vmacSta_p->ShadowMib802dot11->mib_HT40MIntoler))
		{
			if(PhyDSSSTable->Chanflag.FreqBand==FREQ_BAND_2DOT4GHZ
				&&  (PhyDSSSTable->Chanflag.ChnlWidth == CH_AUTO_WIDTH || 
				PhyDSSSTable->Chanflag.ChnlWidth == CH_40_MHz_WIDTH))
			{

				/** add OBSS Scan Parameter here here **/		
				if(skb_tailroom(txSkb_p) >14 /** len of OBSS Scan Parameter IE **/ + 2)
				{
					IEEEtypes_OVERLAP_BSS_SCAN_PARAMETERS_Element_t *pOverLapBSS;
					extern UINT16 AddOverlap_BSS_Scan_Parameters_IE(vmacApInfo_t *vmacSta_p,IEEEtypes_OVERLAP_BSS_SCAN_PARAMETERS_Element_t * pNextElement);

					pOverLapBSS=(IEEEtypes_OVERLAP_BSS_SCAN_PARAMETERS_Element_t *)skb_put(txSkb_p, 14 /** len of OBSS Scan Parameter IE **/ + 2);
					memset((void *)pOverLapBSS, 0, 14 /** len of OBSS Scan Parameter IE **/ + 2);
					AddOverlap_BSS_Scan_Parameters_IE(vmacSta_p,pOverLapBSS);

				}
				else if(skb_headroom(txSkb_p) >14 /** len of OBSS Scan Parameter IE **/ + 2)
				{
					IEEEtypes_OVERLAP_BSS_SCAN_PARAMETERS_Element_t *pOverLapBSS;
					extern UINT16 AddOverlap_BSS_Scan_Parameters_IE(vmacApInfo_t *vmacSta_p,IEEEtypes_OVERLAP_BSS_SCAN_PARAMETERS_Element_t * pNextElement);

					//we have enough at the head
					memcpy(&tempbuffer[0],&txSkb_p->data[0],txSkb_p->len);
					pOverLapBSS = (IEEEtypes_OVERLAP_BSS_SCAN_PARAMETERS_Element_t *)&tempbuffer[txSkb_p->len];
					AddOverlap_BSS_Scan_Parameters_IE(vmacSta_p,pOverLapBSS);
					memset(&txSkb_p->data[0],0, txSkb_p->len);
					skb_push(txSkb_p,14 /** len of OBSS Scan Parameter IE **/ + 2);
					memcpy(&txSkb_p->data[0], &tempbuffer[0], txSkb_p->len);
				}
				else
				{
					WLDBG_INFO(DBG_LEVEL_7, "panic!!!!in OBSS Scan parameter buffer alloc");
				}


				/** add Extended Cap IE here **/		
				if(skb_tailroom(txSkb_p) >8 /** len of extended cap IE **/ + 2)		
				{
					extern UINT16 AddExtended_Cap_IE(vmacApInfo_t *vmacSta_p,IEEEtypes_Extended_Cap_Element_t * pNextElement);
					IEEEtypes_Extended_Cap_Element_t *pExtCap;

					pExtCap=(IEEEtypes_Extended_Cap_Element_t *)skb_put(txSkb_p, 8 /** len of extcap **/+2);	
					memset((void *)pExtCap, 0, 8 /** len of extcap**/+2);										
					AddExtended_Cap_IE(vmacSta_p,pExtCap);

				}
				else if(skb_headroom(txSkb_p) > 8 /** len of extended cap IE **/ + 2)		
				{
					IEEEtypes_Extended_Cap_Element_t *pExtCap;
					extern UINT16 AddExtended_Cap_IE(vmacApInfo_t *vmacSta_p,IEEEtypes_Extended_Cap_Element_t * pNextElement);

					//we have enough at the head
					memcpy(&tempbuffer[0],&txSkb_p->data[0],txSkb_p->len);
					pExtCap = (IEEEtypes_Extended_Cap_Element_t  *)&tempbuffer[txSkb_p->len];
					AddExtended_Cap_IE(vmacSta_p,pExtCap);
					memset(&txSkb_p->data[0],0, txSkb_p->len);
					skb_push(txSkb_p, 8 /** len of extended cap IE **/ + 2);				
					memcpy(&txSkb_p->data[0], &tempbuffer[0], txSkb_p->len);
				}
				else
				{
					WLDBG_INFO(DBG_LEVEL_7, "panic!!!!in extended Cap IE buffer alloc");
				}

			}
		}
#endif
		
		/*Always add Extended Cap if in 5Ghz and VHT mode to pass wifi vht operating mode test*/
		if(*(vmacSta_p->Mib802dot11->mib_ApMode) >= AP_MODE_5GHZ_11AC_ONLY)
		{
			/** add Extended Cap IE here **/		
			if(skb_tailroom(txSkb_p) >8 /** len of extended cap IE **/ + 2)		
			{
				extern UINT16 AddExtended_Cap_IE(vmacApInfo_t *vmacSta_p,IEEEtypes_Extended_Cap_Element_t * pNextElement);
				IEEEtypes_Extended_Cap_Element_t *pExtCap;

				pExtCap=(IEEEtypes_Extended_Cap_Element_t *)skb_put(txSkb_p, 8 /** len of extcap **/+2);	
				memset((void *)pExtCap, 0, 8 /** len of extcap**/+2);										
				AddExtended_Cap_IE(vmacSta_p,pExtCap);
			}
			else if(skb_headroom(txSkb_p) > 8 /** len of extended cap IE **/ + 2)		
			{
				IEEEtypes_Extended_Cap_Element_t *pExtCap;
				extern UINT16 AddExtended_Cap_IE(vmacApInfo_t *vmacSta_p,IEEEtypes_Extended_Cap_Element_t * pNextElement);

				//we have enough at the head
				memcpy(&tempbuffer[0],&txSkb_p->data[0],txSkb_p->len);
				pExtCap = (IEEEtypes_Extended_Cap_Element_t  *)&tempbuffer[txSkb_p->len];
				AddExtended_Cap_IE(vmacSta_p,pExtCap);
				memset(&txSkb_p->data[0],0, txSkb_p->len);
				skb_push(txSkb_p, 8 /** len of extended cap IE **/ + 2);				
				memcpy(&txSkb_p->data[0], &tempbuffer[0], txSkb_p->len);
			}
			else
			{
				WLDBG_INFO(DBG_LEVEL_7, "panic!!!!in extended Cap IE buffer alloc");
			}
		}

	}
#endif
	
#if defined(SOC_W8864)
    //add 11ac IEs
    if(*(vmacSta_p->Mib802dot11->mib_ApMode)&AP_MODE_11AC)
    {
        if(IE191Present)
        {
            if(skb_tailroom(txSkb_p) > sizeof(IEEEtypes_VhtCap_t))
            {
                //We have enough room at the tail
                IE_p = skb_put(txSkb_p, sizeof(IEEEtypes_VhtCap_t));
                if (IE_p != NULL)
                    Build_IE_191(vmacSta_p,(UINT8*)IE_p);
            }
            else
            {
                printk("Assoc Response not enough space for IE191\n");
                WLDBG_INFO(DBG_LEVEL_7, "panic!!!!");
            }        
        
        }
        if(IE191Present || IE192Present)
        {
            if(skb_tailroom(txSkb_p) > sizeof(IEEEtypes_VhOpt_t))
            {
                //We have enough room at the tail
                IE_p = skb_put(txSkb_p, sizeof(IEEEtypes_VhOpt_t));
                if (IE_p != NULL)
                    Build_IE_192(vmacSta_p,(UINT8*)IE_p);
            }
            else
            {
                printk("Assoc Response not enough space for IE192\n");
                WLDBG_INFO(DBG_LEVEL_7, "panic!!!!");
            }        
        
        }
    }
#endif

	if((StaInfo_p->StaType & 0x02) == 0x02)
	{
		if(skb_tailroom(txSkb_p) > 38+2)
		{
			//We have enough room at the tail
			InfoElemHdr_p=(IEEEtypes_InfoElementHdr_t *)skb_put(txSkb_p, 38+2);
			AddM_Rptr_IE(vmacSta_p,(IEEEtypes_HT_Element_t *)InfoElemHdr_p);
		}
		else if(skb_headroom(txSkb_p) > 38+2)
		{
			//we have enough at the head
			memcpy(&tempbuffer[0],&txSkb_p->data[0],txSkb_p->len);
			InfoElemHdr_p = (IEEEtypes_InfoElementHdr_t *)&tempbuffer[txSkb_p->len];
			AddM_Rptr_IE(vmacSta_p,(IEEEtypes_HT_Element_t *)InfoElemHdr_p);
			memset(&txSkb_p->data[0],0, txSkb_p->len);
			skb_push(txSkb_p, 38+2);
			memcpy(&txSkb_p->data[0], &tempbuffer[0], txSkb_p->len);
		}
		else
		{
			WLDBG_INFO(DBG_LEVEL_7, "panic!!!!");

		}
	}

#ifdef MRVL_WPS2
    if (*(mib->mib_wpaWpa2Mode) > 4)
    {
		if(skb_tailroom(txSkb_p) > sizeof(AssocResp_WSCIE_t))
		{
			//We have enough room at the tail
			IE_p = skb_put(txSkb_p, sizeof(AssocResp_WSCIE_t));
			if (IE_p != NULL)
    			Build_AssocResp_WSCIE(vmacSta_p,(AssocResp_WSCIE_t *)IE_p);
		}
		else
		{
            printk("Assoc Response not enough space for WSC IE\n");
			WLDBG_INFO(DBG_LEVEL_7, "panic!!!!");
		}        
    }
#endif

	//put here temporally foo
#ifdef UAPSD_SUPPORT
	if(pWMEInfoElem != NULL)
	{
		memcpy(&QosInfo,&pWMEInfoElem->QoS_info,1);
	}
#endif
	StaInfo_p->FwStaPtr = wlFwSetNewStn(vmacSta_p->dev,(u_int8_t *)&MgmtMsg_p->Hdr.SrcAddr, Aid, StaInfo_p->StnId, 0, &PeerInfo,QosInfo,isQosSta);  //add new station
	wlFwSetSecurity(vmacSta_p->dev,(u_int8_t *)&MgmtMsg_p->Hdr.SrcAddr);
#ifdef NPROTECTION
	extStaDb_entries(vmacSta_p, 0);
	HandleNProtectionMode(vmacSta_p);
#endif

#ifdef MRVL_WAPI
	/* inform wapid that wapi-sta associated (need to attach wie) */
	if (mib_PrivacyTable_p->WAPIEnabled)
	macMgmtMlme_WAPI_event(vmacSta_p->dev, IWEVASSOCREQIE, 0x00F1, &MgmtMsg_p->Hdr.SrcAddr, &MgmtMsg_p->Hdr.DestAddr, (char *)WAPI_IE_p);
#endif
	WLSYSLOG(vmacSta_p->dev,WLSYSLOG_CLASS_ALL, WLSYSLOG_MSG_MLME_ASSOC_SUCCESS "%02x%02x%02x%02x%02x%02x\n",MgmtMsg_p->Hdr.SrcAddr[0],
		MgmtMsg_p->Hdr.SrcAddr[1], MgmtMsg_p->Hdr.SrcAddr[2], MgmtMsg_p->Hdr.SrcAddr[3],
		MgmtMsg_p->Hdr.SrcAddr[4], MgmtMsg_p->Hdr.SrcAddr[5]);
	/* Send event to user space */
#if 0 //def MRVL_WPS2
 	if (StaInfo_p->WSCSta)
		WLSNDEVT(vmacSta_p->dev,IWEVREGISTERED, &MgmtMsg_p->Hdr.SrcAddr, NULL);
	else if ((*(mib->mib_wpaWpa2Mode) > 4) && (*(mib->mib_wpaWpa2Mode) != 16))
	WLSNDEVT(vmacSta_p->dev,IWEVREGISTERED, &MgmtMsg_p->Hdr.SrcAddr, NULL);
#else
	WLSNDEVT(vmacSta_p->dev,IWEVREGISTERED, &MgmtMsg_p->Hdr.SrcAddr, NULL);
#endif


	/* Send for tx (send assoc event to upper layer first, then xmit assoc-resp) */
	if (txMgmtMsg(vmacSta_p->dev,txSkb_p) != OS_SUCCESS )
		dev_kfree_skb_any(txSkb_p);

	/* Key Management initialized here due to station state synchronization. */
	if (mib_PrivacyTable_p->RSNEnabled)
	{
		if (*(mib->mib_wpaWpa2Mode) < 4) /* MRV_8021X For all PSK modes use internal WPA state machine*/
		{
			/*Initialize the keyMgmt state machine */
			KeyMgmtHskCtor(vmacSta_p, StaInfo_p);
			mhsm_initialize(&StaInfo_p->keyMgmtHskHsm.super,&StaInfo_p->keyMgmtHskHsm.sTop);
			//Process Key Management msg 
			keyMgmt_msg(vmacSta_p,pDistMsg);
		}
	}

	if ( !gRatePresent )   //temporary move up here for now foo, going down further might crash
	{
		if ( ChangeAssocParam == TRUE )
			macMgmtMlme_IncrBonlyStnCnt(vmacSta_p,0);
		StaInfo_p->ClientMode = BONLY_MODE;
	}
#if 1
	sendLlcExchangeID(vmacSta_p->dev,&MgmtMsg_p->Hdr.SrcAddr);
#endif    

#if defined(MRV_8021X) && !defined(ENABLE_WLSNDEVT)
	if (*(mib->mib_wpaWpa2Mode) > 3) /* All 8021x modes with external Authenticator*/
	{
		memset(&wreq, 0, sizeof(wreq));
		memcpy(wreq.addr.sa_data, &MgmtMsg_p->Hdr.SrcAddr,6);
		wreq.addr.sa_family = ARPHRD_ETHER;
		wireless_send_event(vmacSta_p->dev, IWEVREGISTERED, &wreq, NULL);
	}
#endif

#ifdef INTOLERANT40
	if (*(vmacSta_p->Mib802dot11->mib_HT40MIntoler))
	{
		if (HTpresent)
		{
			/* Start 30 min timeer */
			sMonitorcnt30min = 1;

			macMgmtMlme_SendBeaconReqMeasureReqAction(vmacSta_p->dev, (IEEEtypes_MacAddr_t *)MgmtMsg_p->Hdr.SrcAddr);
			/* handle 40-20 switch */
			if ((PeerInfo.HTCapabilitiesInfo.FortyMIntolerant) ||
				(!StaInfo_p->HtElem.HTCapabilitiesInfo.SupChanWidth))
			{
				HT40MIntolerantHandler(vmacSta_p,1);
				//printk("Assc:HT\n");
			}
		}
		else
		{
			HT40MIntolerantHandler(vmacSta_p,1);
			//printk("Assc:Legacy\n");
		}
	}
#endif //#ifdef INTOLERANT40

	return;



}


/******************************************************************************
*
* Name: macMgmtMlme_ReassociateReq
*
* Description:
*    This routine handles the Reassociation Request from a Station
*
* Conditions For Use:
*    All software components have been initialized and started.
*
* Arguments:
*    Arg1 (i  ): MgmtMsg_p - Pointer to an 802.11 management message
*                            containing an associate response
*
* Return Value:
*    None.
*
* Notes:
*    None.
*
* PDL:
*        Check to see if the Station is already associated with this
*        AP. If so, change the association parameters and send a response.
*        if the station is unauthenticated, send error response.
*        if the station is 
*        Check if the state is authenticated
*        Check if the SSID matches with AP's SSID
*        Check if the Capability Info could be supported
*        Check if the Basic rates are in the Supported rates
*        Assign Aid and store the information
*        Send AssociateRsp message back 
* END PDL
*
*****************************************************************************/
void macMgmtMlme_AssociateReq(vmacApInfo_t *vmacSta_p,macmgmtQ_MgmtMsg3_t *MgmtMsg_p, UINT32 msgSize)
{
	macMgmtMlme_AssocReAssocReqHandler(vmacSta_p, MgmtMsg_p, msgSize, 0);
}

extern void macMgmtMlme_ReassociateReq(vmacApInfo_t *vmacSta_p,macmgmtQ_MgmtMsg3_t *MgmtMsg_p, UINT32 msgSize )
{
	macMgmtMlme_AssocReAssocReqHandler(vmacSta_p, MgmtMsg_p, msgSize, 1);
}




void macMgmtMlme_SendDeauthenticateMsg(vmacApInfo_t *vmacAP_p,IEEEtypes_MacAddr_t *Addr, UINT16 StnId, UINT16 Reason)
{
	vmacApInfo_t * vmacSta_p = vmacAP_p;
	MIB_802DOT11 *mib=vmacSta_p->Mib802dot11;
	macmgmtQ_MgmtMsg2_t *MgmtMsg_p;
	//	tx80211_MgmtMsg_t *TxMsg_p;
	extStaDb_StaInfo_t *StaInfo_p = NULL;
	IEEEtypes_MacAddr_t SrcMacAddr;
#ifdef AP_MAC_LINUX
	struct sk_buff *txSkb_p;
#endif

	/* Deauth pkt will be sent when data pkts received from STA not in our database
	if(!IS_GROUP(Addr))
	if ((StaInfo_p = extStaDb_GetStaInfo(vmacSta_p,Addr, 1)) == NULL)
	{
	return;
	}*/

	if (*(mib->mib_ApMode)!=AP_MODE_AandG)
	{
		if ((StaInfo_p = extStaDb_GetStaInfo(vmacSta_p,Addr, 1)) == NULL)
			memcpy(SrcMacAddr, &vmacSta_p->macStaAddr, sizeof(IEEEtypes_MacAddr_t));
		else
		{
#ifdef MBSS
			vmacApInfo_t * vmactem_p; 
			vmactem_p = vmacGetMBssByAddr(vmacSta_p, (UINT8 *)(&StaInfo_p->Bssid[0]));
			if(vmactem_p)
				vmacSta_p= vmactem_p;
			mib = vmacSta_p->Mib802dot11;
#endif
			memcpy(SrcMacAddr, &vmacSta_p->macStaAddr, sizeof(IEEEtypes_MacAddr_t));
		}
	}
	else
	{
		if ((StaInfo_p = extStaDb_GetStaInfo(vmacSta_p,Addr, 1)) == NULL)
			memcpy(SrcMacAddr, &vmacSta_p->macStaAddr, sizeof(IEEEtypes_MacAddr_t));
		else
		{
#ifdef MBSS
			vmacApInfo_t * vmactem_p; 
			vmactem_p = vmacGetMBssByAddr(vmacSta_p, (UINT8 *)(&StaInfo_p->Bssid[0]));
			if(vmactem_p)
				vmacSta_p= vmactem_p;
			mib = vmacSta_p->Mib802dot11;
#endif
			if (StaInfo_p->ApMode==AONLY_MODE)
			{
				memcpy(SrcMacAddr, &vmacSta_p->macStaAddr, sizeof(IEEEtypes_MacAddr_t));
			}
			else
			{
				memcpy(SrcMacAddr, &vmacSta_p->macStaAddr2, sizeof(IEEEtypes_MacAddr_t));
			}
		}
	}
#ifdef AP_MAC_LINUX
	if ((txSkb_p = mlmeApiPrepMgtMsg(IEEE_MSG_DEAUTHENTICATE, Addr, &SrcMacAddr)) == NULL)
		return;
	//TxMsg_p = (tx80211_MgmtMsg_t *) txSkb_p->data;
	MgmtMsg_p  = (macmgmtQ_MgmtMsg2_t *) ((UINT8 *) txSkb_p->data);
	WLSYSLOG(vmacSta_p->dev,WLSYSLOG_CLASS_ALL, WLSYSLOG_MSG_MLME_DEAUTH_TOSTA "%02x%02x%02x%02x%02x%02x Reason %d\n", ((unsigned char *)Addr)[0],
		((unsigned char *)Addr)[1], ((unsigned char *)Addr)[2], ((unsigned char *)Addr)[3], 
		((unsigned char *)Addr)[4], ((unsigned char *)Addr)[5], Reason);
	/* Send event to user space */
	WLSNDEVT(vmacSta_p->dev,IWEVEXPIRED, Addr, NULL);
#endif
	{
		MgmtMsg_p->Body.Deauth.ReasonCode = Reason ;
		if (txMgmtMsg(vmacSta_p->dev, txSkb_p) != OS_SUCCESS )
			dev_kfree_skb_any(txSkb_p);
		if ((StaInfo_p = extStaDb_GetStaInfo(vmacSta_p,Addr, 1)) != NULL)
			wlFwSetAPUpdateTim(vmacSta_p->dev, StaInfo_p->Aid, RESETBIT);

		if(StaInfo_p && StaInfo_p->State != UNAUTHENTICATED) {
			wlFwSetNewStn(vmacSta_p->dev,(u_int8_t *)Addr, 0, 0, 2,NULL,0,0); //add new station
			StaInfo_p->State = UNAUTHENTICATED;
		}

	}
}
/******************************************************************************
*
* Name: macMgmtMlme_DeauthenticateMsg
*
* Description:
*    This routine handles a deauthentication notification from another
*    station or an AP.
*
* Conditions For Use:
*    All software components have been initialized and started.
*
* Arguments:
*    Arg1 (i  ): MgmtMsg_p - Pointer to an 802.11 management message
*                            containing a deauthentication message
*
* Return Value:
*    None.
*
* Notes:
*    None.
*
* PDL:
*    Remove from the External Station Info data store the entry for the
*       station that sent the message
*    Send a deauthentication indication to the SME with the reason code
* END PDL
*
*****************************************************************************/
void macMgmtMlme_DeauthenticateMsg(vmacApInfo_t *vmacSta_p,macmgmtQ_MgmtMsg3_t *MgmtMsg_p, UINT32 msgSize)
{
	//	MIB_STA_CFG  *mib_StaCfg_p=vmacSta_p->Mib802dot11->StationConfig;
	extStaDb_StaInfo_t *StaInfo_p;
	if ( (StaInfo_p = extStaDb_GetStaInfo(vmacSta_p,&(MgmtMsg_p->Hdr.SrcAddr), 0)) == NULL )
	{
		/* Station not known, do nothing */
		return ;
	}

#ifdef QOS_FEATURE_REMOVE
	if (*(mib->QoSOptImpl) && extStaDb_GetQoSOptn(vmacSta_p,(IEEEtypes_MacAddr_t * )MgmtMsg_p->Hdr.SrcAddr))
	{
		ClearQoSDB((IEEEtypes_MacAddr_t * )MgmtMsg_p->Hdr.SrcAddr);
	}
#endif
	wlFwSetAPUpdateTim(vmacSta_p->dev, StaInfo_p->Aid, RESETBIT);

	if ( StaInfo_p->State == ASSOCIATED)
	{
#ifdef ENABLE_RATE_ADAPTATION_BASEBAND_REMOVE
		UpdateAssocStnData(StaInfo_p->Aid, StaInfo_p->ApMode);
#endif //ENABLE_RATE_ADAPTATION_BASEBAND
#ifdef AMPDU_SUPPORT
		cleanupAmpduTx(vmacSta_p,(UINT8 *)&StaInfo_p->Addr);
#endif
#if 1
		/* remove the Mac address from the ethernet MAC address table */
		FreeAid(StaInfo_p->Aid);
		ResetAid(StaInfo_p->StnId, StaInfo_p->Aid);
		StaInfo_p->Aid = 0;
		if (StaInfo_p->ClientMode == BONLY_MODE)
			macMgmtMlme_DecrBonlyStnCnt(vmacSta_p, 0);

		if (StaInfo_p->PwrMode == PWR_MODE_PWR_SAVE)
		{
			if (vmacSta_p->PwrSaveStnCnt)
				vmacSta_p->PwrSaveStnCnt--;
			StaInfo_p->PwrMode=  PWR_MODE_ACTIVE;

		}
#endif
	}
#if 1
	StaInfo_p->State = UNAUTHENTICATED;
	//   FreePowerSaveQueue(StaInfo_p->StnId);
	FreeStnId(StaInfo_p->StnId);
	extStaDb_DelSta(vmacSta_p,&(MgmtMsg_p->Hdr.SrcAddr), 0);
#endif
	wlFwSetNewStn(vmacSta_p->dev,(u_int8_t *)&MgmtMsg_p->Hdr.SrcAddr, StaInfo_p->Aid, StaInfo_p->StnId, 2, NULL,0, 0); //add new station

	/* Send event to user space */
	WLSNDEVT(vmacSta_p->dev,IWEVEXPIRED, &MgmtMsg_p->Hdr.SrcAddr, NULL);

	/* No need to free the Station Id, if there are any
	* messages already queued, they  will not be removed*/
	/* Send indication to SME */
}

/******************************************************************************
*
* Name: macMgmtMlme_DisassociateCmd
*
* Description:
*    Routine to handle a command to carry out a disassociation with an AP.
*
* Conditions For Use:
*    All software components have been initialized and started.
*
* Arguments:
*    Arg1 (i  ): DisassocCmd_p - Pointer to a disassociate command
*
* Return Value:
*    None.
*
* Notes:
*    None.
*
* PDL:
*    If any of the given disassociation parameters are invalid Then
*       Send a disassociation confirmation to the SME with the failure status
*    End If
*
*    Send a disassociate message to the indicated station
*    Update the External Station Info data store if to indicate
*       AUTHENTICATED with the indicated station
*    Send a disassociation confirm message to the SME task with the result
*       code
* END PDL
*
*****************************************************************************/
void macMgmtMlme_DisassociateCmd(vmacApInfo_t *vmacSta_p,IEEEtypes_DisassocCmd_t *DisassocCmd_p)
{
	MIB_802DOT11 *mib=vmacSta_p->Mib802dot11;
	macmgmtQ_MgmtMsg2_t *MgmtMsg_p;
	extStaDb_StaInfo_t *StaInfo_p;
	IEEEtypes_MacAddr_t SrcMacAddr;
#ifdef AP_MAC_LINUX
	struct sk_buff *txSkb_p;
#endif

	if (IS_BROADCAST(&DisassocCmd_p->PeerStaAddr))
	{
		return;
	}

	if ( (StaInfo_p = extStaDb_GetStaInfo(vmacSta_p,&DisassocCmd_p->PeerStaAddr, 0)) == NULL )
	{
		return ;
	}
	StaInfo_p->State = AUTHENTICATED;
#ifdef FIX_LATER
	bcngen_UpdateBitInTim(StaInfo_p->Aid, RESETBIT);
#endif

	wlFwSetAPUpdateTim(vmacSta_p->dev, StaInfo_p->Aid, RESETBIT);
#ifdef AMPDU_SUPPORT
	cleanupAmpduTx(vmacSta_p,(UINT8 *)&StaInfo_p->Addr);
#endif

	FreeAid(StaInfo_p->Aid);
	StaInfo_p->Aid = 0;
	/* allocate buffer for message */

	if (*(mib->mib_ApMode)!=AP_MODE_AandG)
	{
		memcpy(SrcMacAddr, &vmacSta_p->macStaAddr,
			sizeof(IEEEtypes_MacAddr_t));
	}
	else
	{

		if (StaInfo_p->ApMode==AONLY_MODE)
		{
			memcpy(SrcMacAddr, &vmacSta_p->macStaAddr,
				sizeof(IEEEtypes_MacAddr_t));
		}
		else
		{
			memcpy(SrcMacAddr, &vmacSta_p->macStaAddr2,
				sizeof(IEEEtypes_MacAddr_t));
		}
	}

#ifdef QOS_FEATURE
	if (*(mib->QoSOptImpl) && extStaDb_GetQoSOptn(vmacSta_p,(IEEEtypes_MacAddr_t *)DisassocCmd_p->PeerStaAddr))
	{
		ClearQoSDB((IEEEtypes_MacAddr_t *)DisassocCmd_p->PeerStaAddr);
	}
#endif
#ifdef AP_MAC_LINUX
	if ((txSkb_p = mlmeApiPrepMgtMsg(IEEE_MSG_DISASSOCIATE, &DisassocCmd_p->PeerStaAddr, &SrcMacAddr)) == NULL)
		return;
	MgmtMsg_p = (macmgmtQ_MgmtMsg2_t *) txSkb_p->data;
#else
	if ((TxMsg_p = mlmeApiPrepMgtMsg(IEEE_MSG_DISASSOCIATE, &DisassocCmd_p->PeerStaAddr, &SrcMacAddr)) != NULL)
#endif
	{
		MgmtMsg_p->Body.DisAssoc.ReasonCode = DisassocCmd_p->Reason;
		if (txMgmtMsg(vmacSta_p->dev, txSkb_p) != OS_SUCCESS )
			dev_kfree_skb_any(txSkb_p);
	}
}

void macMgmtMlme_SendDisassociateMsg(vmacApInfo_t *vmacSta_p, IEEEtypes_MacAddr_t *Addr, UINT16 StnId, UINT16 Reason)
{
	macmgmtQ_MgmtMsg2_t *MgmtMsg_p;
	extStaDb_StaInfo_t *StaInfo_p;
	IEEEtypes_MacAddr_t SrcMacAddr;
#ifdef AP_MAC_LINUX
	struct sk_buff *txSkb_p;
#endif
	if(!IS_GROUP((UINT8 *) Addr))
	{
		if ((StaInfo_p = extStaDb_GetStaInfo(vmacSta_p,Addr, 1)) == NULL)
		{
			return;
		}
	}
	memcpy(SrcMacAddr, &vmacSta_p->macStaAddr, sizeof(IEEEtypes_MacAddr_t));
	if ((txSkb_p = mlmeApiPrepMgtMsg(IEEE_MSG_DISASSOCIATE, Addr, &SrcMacAddr)) == NULL)
		return;
	MgmtMsg_p = (macmgmtQ_MgmtMsg2_t *) txSkb_p->data;
	{
		MgmtMsg_p->Body.DisAssoc.ReasonCode = Reason;
		if (txMgmtMsg(vmacSta_p->dev,txSkb_p) != OS_SUCCESS )
			dev_kfree_skb_any(txSkb_p);
	}
	WLSYSLOG(vmacSta_p->dev,WLSYSLOG_CLASS_ALL, WLSYSLOG_MSG_MLME_DISASSOC_TOSTA "%02x%02x%02x%02x%02x%02x Reason %d\n", ((unsigned char *)Addr)[0],
		((unsigned char *)Addr)[1], ((unsigned char *)Addr)[2], ((unsigned char *)Addr)[3], 
		((unsigned char *)Addr)[4], ((unsigned char *)Addr)[5], Reason);
	/* Send event to user space */
	WLSNDEVT(vmacSta_p->dev,IWEVEXPIRED, Addr, NULL);

}
/******************************************************************************
*
* Name: macMgmtMlme_DisassociateMsg
*
* Description:
*    This routine handles a disassociation notification from an AP.
*
* Conditions For Use:
*    All software components have been initialized and started.
*
* Arguments:
*    Arg1 (i  ): MgmtMsg_p - Pointer to an 802.11 management message
*                            containing a disassociation message
*
* Return Value:
*    None.
*
* Notes:
*    None.
*
* PDL:
*    Update the External Station Info data store if to indicate
*       AUTHENTICATED with the AP that sent the message
*    Send a disassociation indication message to the SME task with the
*       reason code
* END PDL
*
*****************************************************************************/
extern void macMgmtMlme_DisassociateMsg(vmacApInfo_t *vmacSta_p,
										macmgmtQ_MgmtMsg3_t *MgmtMsg_p, UINT32 msgSize )
{
	extStaDb_StaInfo_t *StaInfo_p;
	StaInfo_p = extStaDb_GetStaInfo(vmacSta_p,&(MgmtMsg_p->Hdr.SrcAddr), 0);
	if (!StaInfo_p)
		return;

	WLSYSLOG(vmacSta_p->dev,WLSYSLOG_CLASS_ALL, WLSYSLOG_MSG_MLME_DISASSOC_FROMSTA "%02x%02x%02x%02x%02x%02x\n", MgmtMsg_p->Hdr.SrcAddr[0],
		MgmtMsg_p->Hdr.SrcAddr[1], MgmtMsg_p->Hdr.SrcAddr[2], MgmtMsg_p->Hdr.SrcAddr[3],
		MgmtMsg_p->Hdr.SrcAddr[4], MgmtMsg_p->Hdr.SrcAddr[5]);

	/* Send event to user space */
	WLSNDEVT(vmacSta_p->dev,IWEVEXPIRED, &MgmtMsg_p->Hdr.SrcAddr, NULL);

	StaInfo_p->State = AUTHENTICATED;

	if(!StaInfo_p->Aid)
		return;

#ifdef ERP
	if ( StaInfo_p->ClientMode == BONLY_MODE )
		macMgmtMlme_DecrBonlyStnCnt(vmacSta_p, 0);

#endif
	if (StaInfo_p->State == ASSOCIATED)
	{
		/*Rate Adaptation Function. */
#ifdef ENABLE_RATE_ADAPTATION_BASEBAND_REMOVE
		UpdateAssocStnData(StaInfo_p->Aid, StaInfo_p->ApMode);
#endif /*ENABLE_RATE_ADAPTATION_BASEBAND */
	}

#ifdef QOS_FEATURE
	if (*(vmacSta_p->Mib802dot11->QoSOptImpl) && extStaDb_GetQoSOptn(vmacSta_p,(IEEEtypes_MacAddr_t *)MgmtMsg_p->Hdr.SrcAddr))
	{
		ClearQoSDB((IEEEtypes_MacAddr_t *)MgmtMsg_p->Hdr.SrcAddr);
	}
#endif


	/* remove the Mac address from the ethernet MAC address table */
#ifdef FIX_LATER /* Remove for Sfly, may not be needed. */
	bcngen_UpdateBitInTim(StaInfo_p->Aid, RESETBIT);
#endif

	wlFwSetAPUpdateTim(vmacSta_p->dev, StaInfo_p->Aid, RESETBIT);
#ifdef AMPDU_SUPPORT
	cleanupAmpduTx(vmacSta_p,(UINT8 *)&StaInfo_p->Addr);
#endif
	wlFwSetNewStn(vmacSta_p->dev,(u_int8_t *)&StaInfo_p->Addr, StaInfo_p->Aid, StaInfo_p->StnId, 2, NULL,0, 0); 

	FreeAid(StaInfo_p->Aid);
	StaInfo_p->Aid = 0;
#ifdef NPROTECTION
	extStaDb_entries(vmacSta_p, 0);
	HandleNProtectionMode(vmacSta_p);
#endif
	/* Send indication to SME */
}



/******************************************************************************
*
* Name: macMgmtMlme_ProbeRqst
*
* Description:
*    This routine handles a request from another station in an IBSS to
*    respond to a probe.
*
* Conditions For Use:
*    All software components have been initialized and started.
*
* Arguments:
*    Arg1 (i  ): MgmtMsg_p - Pointer to an 802.11 management message
*                            containing a probe request
* Return Value:
*    None.
*
* Notes:
*    None.
*
* PDL:
*   
* END PDL
*
*****************************************************************************/
static void macMgmtMlme_ProbeRqst1( vmacApInfo_t *vmacSta_p,macmgmtQ_MgmtMsg3_t *MgmtMsg_p )
{
	MIB_802DOT11 *mib=vmacSta_p->Mib802dot11;
	macmgmtQ_MgmtMsg2_t * MgmtRsp_p;
	UINT8 destmacaddr[6];
#ifdef AP_MAC_LINUX
	struct sk_buff *txSkb_p;
#else
	UINT32 tsflow;
	UINT32 tsfhigh;
#endif
#ifdef MRVL_WSC
	static const char *tag = "mlme-probe_request";
#ifdef MRVL_WPS2
	/* some WPS STA (e.g. broadcom) send probe request with length > 256 */
	#define IW_CUSTOM_MAX2 512 
	unsigned char buf[IW_CUSTOM_MAX2]={0};
#else
	unsigned char buf[IW_CUSTOM_MAX]={0};
#endif
	union iwreq_data wreq;
#endif

	if (isSsIdMatch(&MgmtMsg_p->Body.ProbeRqst.SsId, &vmacSta_p->macSsId) != SUCCESS)
	{
		if (*(mib->mib_broadcastssid) == TRUE)
		{
			if (MgmtMsg_p->Body.ProbeRqst.SsId.Len != 0)
				return;
		}
		else
			return;
	}

#ifdef MRVL_WSC
    /* Send an event to upper layer with the probe request */
    /* IWEVENT mechanism restricts the size to 256 bytes */
    /* Note that the Probe Request at this point is in 4 address format and without FCS */
    /* FrmBodyLen = actual len - 4 (FCS) + 6(4 address) = actual len + 2 */
    /* In addition the Mgmt message has a 2 byte Framebody len element */
    /* Hence the length to be passed to up is FrmBodyLen + 2  + sizeof tag*/
#ifdef MRVL_WPS2
    if ((MgmtMsg_p->Hdr.FrmBodyLen + strlen(tag) + sizeof(UINT16)) <= IW_CUSTOM_MAX2) 
#else
    if ((MgmtMsg_p->Hdr.FrmBodyLen + strlen(tag) + sizeof(UINT16)) <= IW_CUSTOM_MAX) 
#endif
    {
    	snprintf(buf, sizeof(buf), "%s", tag);
        memcpy(&buf[strlen(tag)], (char *)MgmtMsg_p, MgmtMsg_p->Hdr.FrmBodyLen + sizeof(UINT16));
        memset(&wreq, 0, sizeof(wreq));
    	wreq.data.length = strlen(tag) + MgmtMsg_p->Hdr.FrmBodyLen + sizeof(UINT16);
    	wireless_send_event(vmacSta_p->dev, IWEVCUSTOM, &wreq, buf);
    }
    else
        WLDBG_INFO(DBG_LEVEL_7, "Probe Request Frame larger than allowed event buffer");         

#endif

	/* Allocate space for response message */
#ifdef AP_MAC_LINUX
	if ((txSkb_p = mlmeApiPrepMgtMsg(IEEE_MSG_PROBE_RSP,
		&MgmtMsg_p->Hdr.SrcAddr,&vmacSta_p->macBssId)) == NULL)
		return;
	MgmtRsp_p = (macmgmtQ_MgmtMsg2_t *) txSkb_p->data;
	MgmtRsp_p->Body.ProbeRsp.TimeStamp[0] =vmacSta_p->VMacEntry.macId;
#else
	if ((TxMsg_p = mlmeApiPrepMgtMsg(IEEE_MSG_PROBE_RSP,
		&MgmtMsg_p->Hdr.SrcAddr,&vmacSta_p->macBssId)) == NULL)

	{



		return ;
	}
#endif

	memcpy(destmacaddr,MgmtMsg_p->Hdr.SrcAddr,6);

#ifndef AP_MAC_LINUX

	memcpy(MgmtRsp_p, PrbRspBuf_p , PrbRspBuf_p->Hdr.FrmBodyLen+sizeof(IEEEtypes_GenHdr_t)); /** just do a copy from the probe response field **/



	/** get the current time stamp **/
	//WL_READ_WORD(TX_TSF_LO, tsflow);
	//WL_READ_WORD(TX_TSF_HI, tsfhigh);
	tsflow  = 0;
	tsfhigh = 0;



	MgmtRsp_p->Body.ProbeRsp.TimeStamp[0] = (UINT8)((tsflow & 0x000000ff));
	MgmtRsp_p->Body.ProbeRsp.TimeStamp[1] = (UINT8)((tsflow & 0x0000ff00) >> 8);
	MgmtRsp_p->Body.ProbeRsp.TimeStamp[2] = (UINT8)((tsflow & 0x00ff0000) >> 16);
	MgmtRsp_p->Body.ProbeRsp.TimeStamp[3] = (UINT8)((tsflow & 0xff000000) >> 24);
	MgmtRsp_p->Body.ProbeRsp.TimeStamp[4] = (UINT8)((tsfhigh & 0x000000ff));
	MgmtRsp_p->Body.ProbeRsp.TimeStamp[5] = (UINT8)((tsfhigh & 0x0000ff00) >> 8);
	MgmtRsp_p->Body.ProbeRsp.TimeStamp[6] = (UINT8)((tsfhigh & 0x00ff0000) >> 16);
	MgmtRsp_p->Body.ProbeRsp.TimeStamp[7] = (UINT8)((tsfhigh & 0xff000000) >> 24);
#endif
	memcpy(MgmtRsp_p->Hdr.DestAddr,destmacaddr,6);

	if (txMgmtMsg(vmacSta_p->dev,txSkb_p) != OS_SUCCESS )
		dev_kfree_skb_any(txSkb_p);
}
extern void macMgmtMlme_ProbeRqst( vmacApInfo_t *vmac_ap,macmgmtQ_MgmtMsg3_t *MgmtMsg_p )
{
	struct net_device *dev;
	struct wlprivate *wlpptr, *wlpptr1 ;
	vmacApInfo_t  *vmacSta_p;
	UINT8 bctAddr[6] = {0xff,0xff,0xff,0xff,0xff,0xff};
	int i=0;
	if(vmac_ap->master)
		vmacSta_p = vmac_ap->master;
	else
		vmacSta_p = vmac_ap;
	wlpptr = NETDEV_PRIV_P(struct wlprivate, vmacSta_p->dev);	
	if(memcmp(MgmtMsg_p->Hdr.DestAddr, bctAddr, 6) == 0)
	{
		while(i <=MAX_VMAC_INSTANCE_AP )
		{	
			if(wlpptr->vdev[i])
			{		
				dev = wlpptr->vdev[i];
				wlpptr1 = NETDEV_PRIV_P(struct wlprivate, dev);
				if(wlpptr1->vmacSta_p->VMacEntry.modeOfService == VMAC_MODE_AP)
	    			if (isMacAccessList(wlpptr1->vmacSta_p, &(MgmtMsg_p->Hdr.SrcAddr)) == SUCCESS)                
						macMgmtMlme_ProbeRqst1(wlpptr1->vmacSta_p, MgmtMsg_p);
			}
			i++;
		}
	}
	else
	{
		if(vmac_ap->VMacEntry.modeOfService == VMAC_MODE_AP)
            if (isMacAccessList(vmac_ap, &(MgmtMsg_p->Hdr.SrcAddr)) == SUCCESS)
			macMgmtMlme_ProbeRqst1(vmac_ap, MgmtMsg_p);
	}
}

#ifdef MRVL_WAPI
/* send wapi event to user space: AP/STA MAC, type, WIE (WAPI IE) etc. */
void macMgmtMlme_WAPI_event(struct net_device *dev, int event_type, u16 auth_type, IEEEtypes_MacAddr_t *sta_addr, IEEEtypes_MacAddr_t *ap_addr, char * info)
{
	asso_mt asso_mt_info ;
	union iwreq_data wreq;
	int extra_len = 0;
	struct wlprivate  *wlpptrSta = NETDEV_PRIV_P(struct wlprivate, dev);
	vmacApInfo_t *vmacSta_p = wlpptrSta->vmacSta_p;
	int i;

	asso_mt_info.type = auth_type;
	memcpy(asso_mt_info.mac, sta_addr, 6);
	memcpy(asso_mt_info.ap_mac, ap_addr, 6);
	memset(asso_mt_info.wie, 0, sizeof(asso_mt_info.wie));

	*(UINT32 *)&(asso_mt_info.gsn[0]) = *(UINT32 *)&(vmacSta_p->wapiPN_mc[12]);
	*(UINT32 *)&(asso_mt_info.gsn[4]) = *(UINT32 *)&(vmacSta_p->wapiPN_mc[8]);
	*(UINT32 *)&(asso_mt_info.gsn[8]) = *(UINT32 *)&(vmacSta_p->wapiPN_mc[4]);
	*(UINT32 *)&(asso_mt_info.gsn[12]) = *(UINT32 *)&(vmacSta_p->wapiPN_mc[0]);

	if (info) {
		IEEEtypes_WAPI_IE_t *p = (IEEEtypes_WAPI_IE_t *)info;
		extra_len = p->Len + 2;
		memcpy(asso_mt_info.wie, p, extra_len);				
		WLDBG_INFO(DBG_LEVEL_7, "### Driver (%s), extra len %d, ap mac %s,  len1=%d, len2=%d\n", __FUNCTION__, extra_len, mac_display((const UINT8 *)ap_addr), sizeof(asso_mt_info), sizeof(asso_mt_info.wie));
	}

	memset(&wreq, 0, sizeof(wreq));
	wreq.data.length = sizeof(asso_mt_info) - sizeof(asso_mt_info.wie) + extra_len;
	if (event_type == IWEVASSOCREQIE) {
		WLDBG_INFO(DBG_LEVEL_7, "### Driver (%s),wreq.data.length=%d, event_type=%x, auth_type=%x\n", __FUNCTION__, wreq.data.length, event_type, auth_type);
	}

    wireless_send_event(dev, event_type, &wreq, (char*)&asso_mt_info);
}
#endif // MRVL_WPAI

#ifdef QOS_FEATURE

extern void macMgmtMlme_QoSAct(vmacApInfo_t *vmacSta_p, macmgmtQ_MgmtMsg3_t *MgmtMsg_p )
{
	struct wlprivate    *wlpptr   = NETDEV_PRIV_P(struct wlprivate, vmacSta_p->dev);
	UINT8 QosAct;
	IEEEtypes_QoS_Category_e QosCategory;
	macmgmtQ_MgmtMsg2_t * MgmtRsp_p;
	extStaDb_StaInfo_t *pStaInfo;
	WSM_DELTS_Req_t *pDelTSFrm;
	IEEEtypes_ADDTS_Rsp_t *pAddTsRspFrm;
	IEEEtypes_ADDTS_Req_t *pAddTsReqFrm;
	IEEEtypes_ADDBA_Req_t *pAddBaReqFrm;
	IEEEtypes_ADDBA_Rsp_t *pAddBaRspFrm;
	IEEEtypes_DELBA_t *pDelBaReqFrm;
	IEEEtypes_VHT_op_mode_action_t *pVHTOpMode;		
	UINT8 vht_NewRxChannelWidth;	
	UINT8 vht_NewRxNss;				
	UINT32 Status, TspecDBindex;
#ifdef AP_MAC_LINUX
	struct sk_buff *txSkb_p;
#endif

	QosCategory = ((UINT8*)&MgmtMsg_p->Body)[0];//get the QoS Action
	QosAct = ((UINT8*)&MgmtMsg_p->Body)[1];//get the QoS Action

	//If ADDTS Request, send teh request to teh Scheduler.
	//check is ADDTS Request
	switch (QosCategory)
	{
	case QoS:
		switch (QosAct)
		{
		case ADDTS_REQ:
			/* Get Aid */
			pStaInfo = extStaDb_GetStaInfo(vmacSta_p,&(MgmtMsg_p->Hdr.SrcAddr), 1);

			if(pStaInfo)
			{
				/* Allocate space for response message */
#ifdef AP_MAC_LINUX
				if ((txSkb_p = mlmeApiPrepMgtMsg(IEEE_MSG_QOS_ACTION,
					&MgmtMsg_p->Hdr.SrcAddr, &vmacSta_p->macBssId)) == NULL)
					return;
				MgmtRsp_p = (macmgmtQ_MgmtMsg2_t *) txSkb_p->data;
#else
				if ((TxMsg_p = mlmeApiPrepMgtMsg(IEEE_MSG_QOS_ACTION,
					&MgmtMsg_p->Hdr.SrcAddr, &vmacSta_p->macBssId)) == NULL)
#endif
				{
					return ;
				}

				pAddTsReqFrm = (IEEEtypes_ADDTS_Req_t*)&MgmtMsg_p->Body;
				pAddTsRspFrm = (IEEEtypes_ADDTS_Rsp_t*)&MgmtRsp_p->Body;

				/*If a TSpec with the same MAC Addr and TsId exists then
				first delete it and then add a new one*/
				Status = ProcessDELTSRequest(vmacSta_p,(IEEEtypes_MacAddr_t *)MgmtMsg_p->Hdr.SrcAddr,
					pAddTsReqFrm->TSpec.ts_info.tsid);
				//Process ADDTS Request. Return a ADDTS Response.
				Status = ProcessADDTSRequest(vmacSta_p,(IEEEtypes_ADDTS_Req_t*)&MgmtMsg_p->Body,
					&MgmtMsg_p->Hdr.SrcAddr, pStaInfo->Aid,
					&TspecDBindex,pStaInfo->ClientMode);

				pAddTsRspFrm->Category = pAddTsReqFrm->Category;
				pAddTsRspFrm->Action = ADDTS_RSP;
				pAddTsRspFrm->DialogToken = pAddTsReqFrm->DialogToken;
				pAddTsRspFrm->StatusCode = Status;

				// pAddTsRspFrm->TSDelay = 0;
				memcpy(&pAddTsRspFrm->TSpec, &pAddTsReqFrm->TSpec, MgmtMsg_p->Hdr.FrmBodyLen - sizeof(IEEEtypes_MgmtHdr_t)-3 );//- sizeof(RxSign_t) - 3);
				//Now append a Schedule Element to the Response only if status is successful.
				ProcessADDTSRequestSchedule(pAddTsRspFrm, TspecDBindex);
#ifdef AP_MAC_LINUX
				if (txMgmtMsg(vmacSta_p->dev,txSkb_p) != OS_SUCCESS )
				{
					dev_kfree_skb_any(txSkb_p);
					return;
				}
#endif
			}
			break;
		case DELTS:
			pDelTSFrm = (WSM_DELTS_Req_t*)&MgmtMsg_p->Body;
			ProcessDELTSRequest(vmacSta_p,&MgmtMsg_p->Hdr.SrcAddr,
				(UINT32)pDelTSFrm->TSInfo.ts_info.tsid);
			break;
		default:
			break;

		}
		break;
	case BlkAck:
		switch (QosAct)
		{
		case ADDBA_REQ:
#if defined(AMPDU_SUPPORT) && defined(AP_MAC_LINUX)
			pStaInfo = extStaDb_GetStaInfo(vmacSta_p,&(MgmtMsg_p->Hdr.SrcAddr), 1);
			if(pStaInfo)
			{
				int i,tid;
				
				/* Allocate space for response message */
				if ((txSkb_p = mlmeApiPrepMgtMsg2(IEEE_MSG_QOS_ACTION, &MgmtMsg_p->Hdr.SrcAddr, &MgmtMsg_p->Hdr.DestAddr, sizeof(IEEEtypes_ADDBA_Rsp_t))) == NULL)        
				{
					WLDBG_INFO(DBG_LEVEL_7,"No more buffer !!!!!!!!!!!!\n");
					return;
				}
				//if ((txSkb_p = mlmeApiPrepMgtMsg(IEEE_MSG_QOS_ACTION,
				//&MgmtMsg_p->Hdr.SrcAddr, &vmacSta_p->macBssId)) == NULL)

				MgmtRsp_p = (macmgmtQ_MgmtMsg2_t *) txSkb_p->data;
				pAddBaReqFrm = (IEEEtypes_ADDBA_Req_t*)&MgmtMsg_p->Body;
				pAddBaRspFrm = (IEEEtypes_ADDBA_Rsp_t*)&MgmtRsp_p->Body;


				pAddBaRspFrm->Category = pAddBaReqFrm->Category;
				pAddBaRspFrm->Action = ADDTS_RSP;  /** same enum as addba_resp **/
				pAddBaRspFrm->DialogToken = pAddBaReqFrm->DialogToken;
				if(vmacSta_p->Ampdu_Rx_Disable_Flag)
				{
					pAddBaRspFrm->StatusCode = 37;//declined;
				}
				else
				{
					pAddBaRspFrm->StatusCode = 0;//  Status;

				}
				pAddBaRspFrm->ParamSet = pAddBaReqFrm->ParamSet;
				pAddBaRspFrm->ParamSet.amsdu = pAddBaReqFrm->ParamSet.amsdu;   //honor remote's setting
				pAddBaRspFrm->ParamSet.BufSize = 64;
				pAddBaRspFrm->Timeout_val = 0;		//Set 0 to improve thpt ramp up time by avoiding Intel6300 sending DELBA to itself after AMPDU traffic
				
				if(!vmacSta_p->Ampdu_Rx_Disable_Flag)
				{
					//	Priority= AccCategoryQ[pAddBaReqFrm->ParamSet.tid];
					tid=pAddBaReqFrm->ParamSet.tid;

					wlpptr->wlpd_p->AmpduPckReorder[pStaInfo->Aid].CurrentSeqNo[tid] =  pAddBaReqFrm->SeqControl.Starting_Seq_No;
					wlpptr->wlpd_p->AmpduPckReorder[pStaInfo->Aid].ReOrdering[tid]  = FALSE;
					wlpptr->wlpd_p->AmpduPckReorder[pStaInfo->Aid].AddBaReceive[tid]=TRUE;
					if(wlpptr->wlpd_p->AmpduPckReorder[pStaInfo->Aid].timer_init[tid]==0)
					{
						TimerInit(&wlpptr->wlpd_p->AmpduPckReorder[pStaInfo->Aid].timer[tid]);
						wlpptr->wlpd_p->AmpduPckReorder[pStaInfo->Aid].timer_init[tid]= 1;
					}

					/** Reset the current queue **/
					for(i=0;i<MAX_AMPDU_REORDER_BUFFER;i++)
					{
						wlpptr->wlpd_p->AmpduPckReorder[pStaInfo->Aid].ExpectedSeqNo[tid][i]= 0;
						if(wlpptr->wlpd_p->AmpduPckReorder[pStaInfo->Aid].pFrame[tid][i]!=NULL)
							dev_kfree_skb_any((struct sk_buff *)wlpptr->wlpd_p->AmpduPckReorder[pStaInfo->Aid].pFrame[tid][i]);
						wlpptr->wlpd_p->AmpduPckReorder[pStaInfo->Aid].pFrame[tid][i] = NULL;
					}

					wlpptr->wlpd_p->AmpduPckReorder[pStaInfo->Aid].ExpectedSeqNo[tid][0] = wlpptr->wlpd_p->AmpduPckReorder[pStaInfo->Aid].CurrentSeqNo[tid] ;
				}

				if (txMgmtMsg(vmacSta_p->dev,txSkb_p) != OS_SUCCESS )
				{
					dev_kfree_skb_any(txSkb_p);
					return;
				}
			}
#endif
			break;
		case DELBA:
#ifdef AMPDU_SUPPORT
			pStaInfo = extStaDb_GetStaInfo(vmacSta_p, &(MgmtMsg_p->Hdr.SrcAddr), 1);
			WLDBG_INFO(DBG_LEVEL_7,"Inside delba_req\n");
			if(pStaInfo)
			{
				int i,tid;

				pDelBaReqFrm = (IEEEtypes_DELBA_t*)&MgmtMsg_p->Body;
				//	pAddBaRspFrm = (IEEEtypes_ADDBA_Rsp_t*)&MgmtRsp_p->Body;




				//WLDBG_INFO(DBG_LEVEL_7,"**********************\n");
				//WLDBG_INFO(DBG_LEVEL_7,"Dialog Token %x, policy %x Bufsize %x tid %x Seqno %x\n",pAddBaReqFrm->DialogToken,pAddBaReqFrm->ParamSet.BA_policy,
				//	pAddBaReqFrm->ParamSet.BufSize,pAddBaReqFrm->ParamSet.tid,pAddBaReqFrm->SeqControl);
				// WLDBG_INFO(DBG_LEVEL_7,"Current seqno %d\n",pAddBaReqFrm->SeqControl.Starting_Seq_No);

				//WLDBG_INFO(DBG_LEVEL_7,"~~~~~~~~~~~~~~~~~~~~\n");


				//		Priority= AccCategoryQ[pDelBaReqFrm->ParamSet.tid];
				tid=pDelBaReqFrm->ParamSet.tid;

				if(pDelBaReqFrm->ParamSet.Initiator==1)  /** Initiator want to stop ampdu **/
				{

					wlpptr->wlpd_p->AmpduPckReorder[pStaInfo->Aid].ReOrdering[tid]  = FALSE;
					wlpptr->wlpd_p->AmpduPckReorder[pStaInfo->Aid].AddBaReceive[tid]=FALSE;
					//		WLDBG_INFO(DBG_LEVEL_0, " 4Value of Aid= %x Priority = %x addbareceive = %x \n",pStaInfo->Aid,Priority,AmpduPckReorder[pStaInfo->Aid].AddBaReceive[Priority]);


					/** Reset the current queue **/
					for(i=0;i<MAX_AMPDU_REORDER_BUFFER;i++)
					{
						wlpptr->wlpd_p->AmpduPckReorder[pStaInfo->Aid].ExpectedSeqNo[tid][i]= 0;
						if(wlpptr->wlpd_p->AmpduPckReorder[pStaInfo->Aid].pFrame[tid][i]!=NULL)
							dev_kfree_skb_any((struct sk_buff *)wlpptr->wlpd_p->AmpduPckReorder[pStaInfo->Aid].pFrame[tid][i]);
						wlpptr->wlpd_p->AmpduPckReorder[pStaInfo->Aid].pFrame[tid][i] = NULL;
					}


					wlpptr->wlpd_p->AmpduPckReorder[pStaInfo->Aid].ExpectedSeqNo[tid][0] = wlpptr->wlpd_p->AmpduPckReorder[pStaInfo->Aid].CurrentSeqNo[tid] ;
				}
				else   /** Receiver want to stop us from doing ampdu **/
				{
					/** check which stream is it for **/
					for(i=0;i<MAX_SUPPORT_AMPDU_TX_STREAM;i++)
					{
						if(!MACADDR_CMP(wlpptr->wlpd_p->Ampdu_tx[i].MacAddr, &(MgmtMsg_p->Hdr.SrcAddr)))
						{
							/** they are equal **/
							if(wlpptr->wlpd_p->Ampdu_tx[i].AccessCat == pDelBaReqFrm->ParamSet.tid && wlpptr->wlpd_p->Ampdu_tx[i].InUse==1)
							{
								WLDBG_INFO(DBG_LEVEL_0, "del ba !!!! They match!!!!\n");
								/* Reset the flags so that stream can be started once traffic is back on */
								pStaInfo->aggr11n.onbytid[wlpptr->wlpd_p->Ampdu_tx[i].AccessCat]=0;
								pStaInfo->aggr11n.startbytid[wlpptr->wlpd_p->Ampdu_tx[i].AccessCat]=0;
								pStaInfo->aggr11n.type &= ~WL_WLAN_TYPE_AMPDU;
								wlFwUpdateDestroyBAStream(vmacSta_p->dev, 0,0,i);
								wlpptr->wlpd_p->Ampdu_tx[i].InUse=0;
								wlpptr->wlpd_p->Ampdu_tx[i].TimeOut = 0;

							}
						}
					}
				}



			}
#endif
			break;
		case ADDBA_RESP:
#ifdef AMPDU_SUPPORT
			pStaInfo = extStaDb_GetStaInfo(vmacSta_p, &(MgmtMsg_p->Hdr.SrcAddr), 1);
			WLDBG_INFO(DBG_LEVEL_0, "Inside AddBA Response receive\n");
			if(pStaInfo)
			{
				int i;
				WLDBG_INFO(DBG_LEVEL_0, "Inside pStaInfo\n");

				pAddBaRspFrm= (IEEEtypes_ADDBA_Rsp_t*)&MgmtMsg_p->Body;


				WLDBG_INFO(DBG_LEVEL_0, "**********************\n");


				WLDBG_INFO(DBG_LEVEL_0, "Value of Category = %x\n", pAddBaRspFrm->Category);
				WLDBG_INFO(DBG_LEVEL_0, "Value of Action = %x\n",pAddBaRspFrm->Action);
				WLDBG_INFO(DBG_LEVEL_0, "Value of Dialog Token = %x\n",pAddBaRspFrm->DialogToken);
				WLDBG_INFO(DBG_LEVEL_0, "Value of StatusCode = %x\n",pAddBaRspFrm->StatusCode);
				WLDBG_INFO(DBG_LEVEL_0, "Value of ParamSet.BlockAckPolicy = %x, ParamSet.Tid = %x, BufferSize = %x\n", pAddBaRspFrm->ParamSet.BA_policy,  pAddBaRspFrm->ParamSet.tid,
					pAddBaRspFrm->ParamSet.BufSize);
				WLDBG_INFO(DBG_LEVEL_0, "Value of Reason = %x\n", pAddBaRspFrm->Timeout_val);


				for(i=0;i<MAX_SUPPORT_AMPDU_TX_STREAM;i++)
				{
					if(wlpptr->wlpd_p->Ampdu_tx[i].DialogToken == pAddBaRspFrm->DialogToken && 
						wlpptr->wlpd_p->Ampdu_tx[i].InUse && wlpptr->wlpd_p->Ampdu_tx[i].AccessCat ==pAddBaRspFrm->ParamSet.tid)
					{
						if(MACADDR_CMP(wlpptr->wlpd_p->Ampdu_tx[i].MacAddr, &(MgmtMsg_p->Hdr.SrcAddr))==0)
							break;
					}
				}

				/*Check if a ADDBA response has been received earlier, if it has then drop the response */
				if (wlpptr->wlpd_p->Ampdu_tx[i].AddBaResponseReceive == 1 && 
					wlpptr->wlpd_p->Ampdu_tx[i].DialogToken == pAddBaRspFrm->DialogToken)
				{
					break;
				}

				if(i<MAX_SUPPORT_AMPDU_TX_STREAM)  /** either stream 0 or 1 is equal **/
				{
					if(pAddBaRspFrm->StatusCode==0 )  //success
					{
						if( pAddBaRspFrm->ParamSet.tid == wlpptr->wlpd_p->Ampdu_tx[i].AccessCat)
						{
							WLDBG_INFO(DBG_LEVEL_0, "Creating blockack stream \n");
							if(wlpptr->wlpd_p->Ampdu_tx[i].initTimer==1)
								TimerDisarm(&wlpptr->wlpd_p->Ampdu_tx[i].timer);
							if(wlFwCreateBAStream(vmacSta_p->dev, pAddBaRspFrm->ParamSet.BufSize, pAddBaRspFrm->ParamSet.BufSize , (u_int8_t *)&(MgmtMsg_p->Hdr.SrcAddr), 10, wlpptr->wlpd_p->Ampdu_tx[i].AccessCat, 
								1,  i,pStaInfo->HtElem.MacHTParamInfo, NULL, wlpptr->wlpd_p->Ampdu_tx[i].start_seqno)==SUCCESS)
							{
								pStaInfo->aggr11n.type |= WL_WLAN_TYPE_AMPDU;
								//only doing amsdu over ampdu if peer supported
								if(pAddBaRspFrm->ParamSet.amsdu && (*(vmacSta_p->Mib802dot11->pMib_11nAggrMode)& WL_MODE_AMSDU_TX_MASK) )
								{
									pStaInfo->aggr11n.type |= WL_WLAN_TYPE_AMSDU;
								}else
								{
									pStaInfo->aggr11n.type &= ~WL_WLAN_TYPE_AMSDU;
								}
							}
							else
							{
								/** FW is not ready to accept addba stream **/
								//	printk("FW not ready to accept BA stream!!!\n");
								SendDelBA(vmacSta_p, (UINT8 *)&wlpptr->wlpd_p->Ampdu_tx[i].MacAddr[0], wlpptr->wlpd_p->Ampdu_tx[i].AccessCat);
								pStaInfo->aggr11n.type &= ~WL_WLAN_TYPE_AMPDU;
								wlpptr->wlpd_p->Ampdu_tx[i].InUse=0;
								wlpptr->wlpd_p->Ampdu_tx[i].TimeOut = 0;
								pStaInfo->aggr11n.onbytid[wlpptr->wlpd_p->Ampdu_tx[i].AccessCat] = 0;
								pStaInfo->aggr11n.startbytid[wlpptr->wlpd_p->Ampdu_tx[i].AccessCat] = 0;					
								//fall back to amsdu	
								if(*(vmacSta_p->Mib802dot11->pMib_11nAggrMode)& WL_MODE_AMSDU_TX_MASK)
								{
									pStaInfo->aggr11n.type |= WL_WLAN_TYPE_AMSDU;
								}
							}


						}
						else
						{
							WLDBG_INFO(DBG_LEVEL_0, "Invalid block ack response \n");
						}
						wlpptr->wlpd_p->Ampdu_tx[i].AddBaResponseReceive=1;
					}
					else
					{
						/** addba fail failure status code , clear the stream**/
						if(wlpptr->wlpd_p->Ampdu_tx[i].initTimer==1)
							TimerDisarm(&wlpptr->wlpd_p->Ampdu_tx[i].timer);

						pStaInfo->aggr11n.startbytid[wlpptr->wlpd_p->Ampdu_tx[i].AccessCat]=1;  
						wlpptr->wlpd_p->Ampdu_tx[i].DialogToken=0;
						wlpptr->wlpd_p->Ampdu_tx[i].InUse = 0;
						wlpptr->wlpd_p->Ampdu_tx[i].AccessCat = 0;
						wlpptr->wlpd_p->Ampdu_tx[i].TimeOut = 0;
					}
				}
			}
#endif
			break;

		default:
			break;
		}
		break;
	case DLP:
		switch (QosAct)
		{
		case DLP_REQ:
			ProcessDlpReq(vmacSta_p, (macmgmtQ_MgmtMsg_t *)MgmtMsg_p);
			break;
		case DLP_RESP:
			ProcessDlpRsp(vmacSta_p, (macmgmtQ_MgmtMsg_t *)MgmtMsg_p);
			break;
		case DLP_TEAR_DOWN:
			ProcessDlpTeardown(vmacSta_p, (macmgmtQ_MgmtMsg_t *)MgmtMsg_p);
			break;
		default:
			break;
		}
		break;
	
	case VHT:
		switch (QosAct)
		{
			case OPERATING_MODE_NOTIFICATION:
				pStaInfo = extStaDb_GetStaInfo(vmacSta_p, &(MgmtMsg_p->Hdr.SrcAddr), STADB_UPDATE_AGINGTIME);
				if(pStaInfo)
				{
					pVHTOpMode = (IEEEtypes_VHT_op_mode_action_t*)&MgmtMsg_p->Body;

					if(pVHTOpMode->OperatingMode.RxNssType == 0){
						vht_NewRxChannelWidth = pVHTOpMode->OperatingMode.ChannelWidth;
						vht_NewRxNss = pVHTOpMode->OperatingMode.RxNss + 1;	//In IE199, 0:Nss1, 1:Nss2....So we plus 1 to become 1:Nss1, 2:Nss2
					}
					/*Beamforming related matters*/
					else{
						/*TODO: Hard coded for now to pass wifi 7/19/2013*/
						vht_NewRxChannelWidth = 2; 		//0:20Mhz, 1:40Mhz, 2:80Mhz, 3:160 or 80+80Mhz
						vht_NewRxNss = 3; 				//1:1Nss, 2:2Nss, 3:3Nss
					} 
					
					wlFwSetVHTOpMode(vmacSta_p->dev,&(MgmtMsg_p->Hdr.SrcAddr),vht_NewRxChannelWidth,vht_NewRxNss);
				
				}
				break;
			default:
				break;
		}
		break;
	
	default:
		break;
	}
}
#endif //QOS_FEATURE

/******************************************************************************
*
* Name: macMgmtMlme_ProbeRsp
*
* Description:
*    This routine handles a response from another station in an IBSS or an
*    AP in a BSS to a prior probe request.
*
* Conditions For Use:
*    All software components have been initialized and started.
*
* Arguments:
*    Arg1 (i  ): MgmtMsg_p - Pointer to an 802.11 management message
*                            containing a probe response
*
* Return Value:
*    None.
*
* Notes:
*    None.
*
* PDL:

* END PDL
*
*****************************************************************************/
extern void macMgmtMlme_ProbeRsp( macmgmtQ_MgmtMsg_t *MgmtMsg_p )
{}



/******************************************************************************
*
* Name: macMgmtMlme_ReassociateRsp
*
* Description:
*    This routine handles a response from an AP to a prior reassociate request.
*
* Conditions For Use:
*    All software components have been initialized and started.
*
* Arguments:
*    Arg1 (i  ): MgmtMsg_p - Pointer to an 802.11 management message
*                            containing a reassociate response
*
* Return Value:
*    None.
*
* Notes:
*    None.
*
* PDL:
* END PDL
*
*****************************************************************************/
extern void macMgmtMlme_ReassociateRsp(
									   macmgmtQ_MgmtMsg_t *MgmtMsg_p )
{}

#define MAX_SUPP_RATE_SET_NUM	8

void PrepareRateElements(vmacApInfo_t *vmacSta_p,IEEEtypes_StartCmd_t *StartCmd_p )
{
	MIB_802DOT11 *mib=vmacSta_p->Mib802dot11;
	
	IEEEtypes_SuppRatesElement_t *SuppRateSet_p=&(vmacSta_p->SuppRateSet);		
#ifdef ERP
	IEEEtypes_ExtSuppRatesElement_t *ExtSuppRateSet_p=&(vmacSta_p->ExtSuppRateSet);
#endif

	UINT32 i, j, k, l;
	UINT32 Index;
	UINT8 MaxRate;

	i = j = k = l = 0;
	LowestBasicRateIndex = 0;
	HighestBasicRateIndex = 0;
	LowestBasicRate = 108;  /* 54 Mbit */

	if ((*(mib->mib_ApMode)!=AP_MODE_A_ONLY) && (*(mib->mib_ApMode)!=AP_MODE_AandN) && (*(mib->mib_ApMode)!=AP_MODE_5GHZ_N_ONLY) 
#ifdef SOC_W8864	
	    && (*(mib->mib_ApMode) != AP_MODE_5GHZ_Nand11AC) &&
	    (*(mib->mib_ApMode) != AP_MODE_5GHZ_11AC_ONLY)
#endif	    
	    )
	{
		MaxRate=IEEEtypes_MAX_DATA_RATES_G;
		HighestBasicRate = 2;  /* 1 Mbit */
	}
	else
	{
		MaxRate=IEEEtypes_MAX_DATA_RATES_A;
		HighestBasicRate = 12;
	}

	LowestBasicRateIndexB = 0;
	HighestBasicRateIndexB = 0;
	LowestBasicRateB = 22;
	HighestBasicRateB = 2;

	for ( i = 0; i < MaxRate;i++)
	{	
		if (StartCmd_p->BssBasicRateSet[i] > 1 && StartCmd_p->BssBasicRateSet[i] <= 127)
		{
			if ((Index = hw_GetPhyRateIndex(vmacSta_p,StartCmd_p->BssBasicRateSet[i]))
				< MaxRate)
				/* valid rate for the Phy */
			{

				BasicRateSetIndex[i] = Index;
				BasicRateSet[i] = StartCmd_p->BssBasicRateSet[i];

				if (BasicRateSet[i] <= LowestBasicRate)
				{
					LowestBasicRate = BasicRateSet[i];
					LowestBasicRateIndex = Index;
				}
				if (BasicRateSet[i] >= HighestBasicRate)
				{
					HighestBasicRate = BasicRateSet[i];
					HighestBasicRateIndex = Index;
				}
				
				if ((*(mib->mib_ApMode)!=AP_MODE_A_ONLY && *(mib->mib_ApMode)!=AP_MODE_AandN) && (*(mib->mib_ApMode)!=AP_MODE_5GHZ_N_ONLY) 
#ifdef SOC_W8864	
                    && (*(mib->mib_ApMode) != AP_MODE_5GHZ_Nand11AC) &&
                    (*(mib->mib_ApMode) != AP_MODE_5GHZ_11AC_ONLY)
#endif                    
                   )
				{
					if (BasicRateSet[i] == 2 || BasicRateSet[i] == 4 || BasicRateSet[i] == 11 || BasicRateSet[i] == 22)  /*It is B rate */
					{
						if (BasicRateSet[i] <= LowestBasicRateB)
						{
							LowestBasicRateB = BasicRateSet[i];
							LowestBasicRateIndexB = Index;
						}
						if (BasicRateSet[i] >= HighestBasicRateB)
						{
							HighestBasicRateB = BasicRateSet[i];
							HighestBasicRateIndexB = Index;
						}
					}
				}


			}
			else /* the rate is not a valid state for the PHY */
			{
				/* Send Invalid parameter reply */
			}
		}
		else
		{
			/* Send invalid parameters reply */
			break;
		}
	}
	BasicRateSetLen = i;
	LowestOpRateIndex = 0;
	HighestOpRateIndex = 0;

	LowestOpRate = 108;  /* 54 Mbit */
	
	if ((*(mib->mib_ApMode)!=AP_MODE_A_ONLY && *(mib->mib_ApMode)!=AP_MODE_AandN) && (*(mib->mib_ApMode)!=AP_MODE_5GHZ_N_ONLY) 
#ifdef SOC_W8864	
	    && (*(mib->mib_ApMode) != AP_MODE_5GHZ_Nand11AC) &&
	    (*(mib->mib_ApMode) != AP_MODE_5GHZ_11AC_ONLY)
#endif	    
	    )

		HighestOpRate = 2;  /* 1 Mbit */
	else
		HighestOpRate = 12;  /* 6 Mbit */


	for ( i = 0; i < MaxRate;i++)
	{	
		if (StartCmd_p->OpRateSet[i] > 1 && StartCmd_p->OpRateSet[i] <= 127)
		{
			if ((Index = hw_GetPhyRateIndex(vmacSta_p,StartCmd_p->OpRateSet[i]))
				<  MaxRate)
				/* valid rate for the Phy */
			{
				OpRateSetIndex[i] = Index;
				OpRateSet[i] = StartCmd_p->OpRateSet[i];

				if (OpRateSet[i] <= LowestOpRate)
				{
					LowestOpRate = OpRateSet[i];
					LowestOpRateIndex = Index;
				}
				if (OpRateSet[i] >= HighestOpRate)
				{
					HighestOpRate = OpRateSet[i];
					HighestOpRateIndex = Index;
				}
			}
			else /* the rate is not a valid state for the PHY */
			{
				/* Send Invalid parameter reply */
			}
		}
		else
		{
			break;
		}
	}
	OpRateSetLen = i;
	
	/* Form the supported rate set, and Ext supp rate set for sending in assoc resp msg */
	SuppRateSet_p->ElementId = SUPPORTED_RATES;
	ExtSuppRateSet_p->ElementId = EXT_SUPPORTED_RATES;		

	for (i = 0; i < OpRateSetLen; i++)
	{	
		if (*(mib->mib_ApMode) == AP_MODE_B_ONLY)
		{
			if (hw_GetPhyRateIndex(vmacSta_p,OpRateSet[i]) > HIGHEST_11B_RATE_REG_INDEX)
				continue;
		}
		for (j = 0; j < BasicRateSetLen; j++)
		{	
			if (OpRateSet[i] == BasicRateSet[j])
		{
				if (k < MAX_SUPP_RATE_SET_NUM)
			{
					SuppRateSet_p->Rates[k++]
						= OpRateSet[i] | IEEEtypes_BASIC_RATE_FLAG;
					}
					else
					{
					ExtSuppRateSet_p->Rates[l++]
						= OpRateSet[i] | IEEEtypes_BASIC_RATE_FLAG;
				}
				break;
			}
		}
		if (j == BasicRateSetLen)
		{
			if (k < MAX_SUPP_RATE_SET_NUM)
			{
				SuppRateSet_p->Rates[k++] = OpRateSet[i];
				}
				else
				{
				ExtSuppRateSet_p->Rates[l++]
					= OpRateSet[i];
				}
			}
		}
	SuppRateSet_p->Len = k;
	ExtSuppRateSet_p->Len = l;		
}
/******************************************************************************
*
* Name: macMgmtMlme_ResetCmd
*
* Description:
*    Routine to handle a command to perform a reset, which resets the MAC
*    to initial conditions.
*
* Conditions For Use:
*    All software components have been initialized and started.
*
* Arguments:
*    Arg1 (i  ): ResetCmd_p - Pointer to a reset command
*
* Return Value:
*    None.
*
* Notes:
*    None.
*
* PDL:
*    Clear the External Station Info data store of all entries
*    Set the station MAC Management state to IDLE
*    Send a reset confirmation to the SME with the result status
* END PDL
*
*****************************************************************************/
extern void macMgmtMlme_ResetCmd(vmacApInfo_t *vmacSta_p,  IEEEtypes_ResetCmd_t *ResetCmd_p )
{

	if (ResetCmd_p->SetDefaultMIB)
	{
		/* reset the MIB */
	}
	if(!ResetCmd_p->quiet)
	{
		macMgmtMlme_SendDeauthenticateMsg(vmacSta_p,&bcast, MCBC_STN_ID, IEEEtypes_REASON_DEAUTH_LEAVING);
	}
	extStaDb_RemoveAllStns(vmacSta_p,IEEEtypes_REASON_DEAUTH_LEAVING);

	if(vmacSta_p->Mib802dot11->Privacy->RSNEnabled)
	{           
		SendKeyMgmtInitEvent(vmacSta_p);
	}
	else
	{
		KeyMgmtReset(vmacSta_p);
	}

#ifdef QOS_FEATURE_REMOVE
	if (*(mib->QoSOptImpl))
	{
		wlQosSetQAsCAPQ(CAP_VO_Q);
		wlQosSetQAsCAPQ(CAP_VI_Q);
	}
#endif

	vmacSta_p->bOnlyStnCnt = 0;
	SendStartCmd(vmacSta_p);

	return ;
}

void MonitorTimerInit(vmacApInfo_t *vmacSta_p)
{
	extern void MonitorTimerProcess(UINT8 *);

	if (vmacSta_p->dev->flags & IFF_RUNNING)
	{
		TimerInit(&vmacSta_p->monTimer);
		TimerFireIn(&vmacSta_p->monTimer, 1, &MonitorTimerProcess, (unsigned char *)vmacSta_p, MONITER_PERIOD_1SEC);
	}
}

/******************************************************************************
*
* Name: macMgmtMlme_StartCmd
*
* Description:
*    Routine to handle a command to start a BSS.
*
* Conditions For Use:
*    All software components have been initialized and started.
*
* Arguments:
*    Arg1 (i  ): StartCmd_p - Pointer to a start command
*
* Return Value:
*    None.
*
* Notes:
*    None.
*
* PDL:
*    If any of the given start parameters are invalid Then
*       Send a start confirmation to the SME with the failure status
*    End If
*
* END PDL
*
*****************************************************************************/
extern void macMgmtMlme_StartCmd(vmacApInfo_t *vmacSta_p, IEEEtypes_StartCmd_t *StartCmd_p )
{
	MIB_802DOT11 *mib=vmacSta_p->Mib802dot11;
	MIB_802DOT11 *mibshadow = vmacSta_p->ShadowMib802dot11;		
	vmacSta_p->macSsId.ElementId = SSID;
	vmacSta_p->macSsId.Len = CopySsId(vmacSta_p->macSsId.SsId, StartCmd_p->SsId);

	vmacSta_p->macSsId2.ElementId = SSID;
	vmacSta_p->macSsId2.Len = CopySsId(vmacSta_p->macSsId2.SsId,StartCmd_p->SsId2);

	PrepareRateElements(vmacSta_p,StartCmd_p);


	/* Processing the Cap Info fields */
	vmacSta_p->macCapInfo = StartCmd_p->CapInfo;
	/* Make sure the reserved bits are zero */
	vmacSta_p->macCapInfo.Rsrvd1=0;

	vmacSta_p->macCapInfo.Rsrvd2=0;
	vmacSta_p->macCapInfo.DsssOfdm=0;
#ifndef QOS_FEATURE
	vmacSta_p->macCapInfo.Rsrvd3=0;
#endif    
	if (vmacSta_p->macCapInfo.CfPollable == 1 && vmacSta_p->macCapInfo.CfPollRqst == 0)
	{
	}
	if ( vmacSta_p->macCapInfo.CfPollable == 0 && vmacSta_p->macCapInfo.CfPollRqst == 1)
	{
	}
	/* Set the Cf parameter Set */
	if (StartCmd_p->SsParamSet.CfParamSet.ElementId == CF_PARAM_SET &&
		StartCmd_p->SsParamSet.CfParamSet.Len == 6 )
	{
	}
	/* Set the Phy parameter set */

	/* Update the HW MAC registers with the new values */
	/* Copy SSID and BSSID registers */

	/* Set the specified channel  */

	if (StartCmd_p->PhyParamSet.DsParamSet.ElementId == DS_PARAM_SET)
	{
	}
	if (*(mib->mib_ApMode)!=AP_MODE_B_ONLY && *(mib->mib_ApMode)!=AP_MODE_A_ONLY)
	{
		if(*(mib->mib_forceProtectiondisable)){
			*(mib->mib_ErpProtEnabled)=FALSE;
			*(mibshadow->mib_ErpProtEnabled)=FALSE;		//Update shadowmib too to prevent default shadowmib from overwriting mib during commit
		}
		else{
			*(mib->mib_ErpProtEnabled)=TRUE;
			*(mibshadow->mib_ErpProtEnabled)=TRUE;		
		}
	}

	MonitorTimerInit(vmacSta_p);

	return ;
}

void Disable_MonitorTimerProcess(vmacApInfo_t *vmacSta_p)
{
	TimerRemove(&vmacSta_p->monTimer);
}

/*============================================================================= */
/*                         CODED PRIVATE PROCEDURES */
/*============================================================================= */

/******************************************************************************
*
* Name: isSsIdMatch
*
* Description:
*
* Conditions For Use:
*    All software components have been initialized and started.
*
* Arguments:
*    Arg1 (i  ): 
* Return Value:
*    None.
*
* Notes:
*    None.
*
* PDL:

* END PDL
*
*****************************************************************************/
WL_STATUS isSsIdMatch(IEEEtypes_SsIdElement_t *SsId1,
					  IEEEtypes_SsIdElement_t *SsId2)
{
	if(SsId1 && SsId2)
	{
		if (SsId1->Len == SsId2->Len && !memcmp(SsId1->SsId, SsId2->SsId,
			SsId1->Len))
		{
			return(OS_SUCCESS);
		}
		else
		{
			return(OS_FAIL);
		}
	}
	else
		return (OS_FAIL);
}


/******************************************************************************
*
* Name: isCapInfoSupported
*
* Description:
*
* Conditions For Use:
*    All software components have been initialized and started.
*
* Arguments:
*    Arg1 (i  ): 
* Return Value:
*    None.
*
* Notes:
*    None.
*
* PDL:

* END PDL
*
*****************************************************************************/
WL_STATUS isCapInfoSupported(IEEEtypes_CapInfo_t *CapInfo1,
							 IEEEtypes_CapInfo_t *CapInfo2)
{
	if (CapInfo1->ShortPreamble==0)
		BarkerPreambleSet=1;
	return(OS_SUCCESS);
}


WL_STATUS isBasicRatesSupported(IEEEtypes_SuppRatesElement_t * SuppRates,
								IEEEtypes_ExtSuppRatesElement_t *ExtSuppRates,BOOLEAN gRatePresent)
{
	UINT32 i, j;
	UINT32 RateSetLen;
	IEEEtypes_DataRate_t *RateSet;
	if ( gRatePresent )
	{
		RateSetLen = BasicRateSetLen;
		RateSet = BasicRateSet;
	}
	else
	{
		RateSetLen = BasicRateSetLen;
		RateSet = BasicRateSet;
	}
	for ( i = 0; i < RateSetLen; i++ )
	{
		for (j = 0; j < SuppRates->Len; j++)
		{
			if ((SuppRates->Rates[j] & IEEEtypes_BASIC_RATE_MASK)
				== RateSet[i] )
			{

				break; /* Basic rate is present, no more check for this rate */
			}
		}
		if ( j < SuppRates->Len)
		{
			continue;  /* Continue for the next basic rate */
		}
		else  /* Basuc rate not in supported rate set, check in Extended Supp rate set */
		{

			for (j = 0;(ExtSuppRates && j < ExtSuppRates->Len); j++)
			{
				if ((ExtSuppRates->Rates[j] & IEEEtypes_BASIC_RATE_MASK)
					== RateSet[i] )
				{

					break;  /* basic rate present in ext supp rate set */
				}
			}
			if (j < ExtSuppRates->Len)  /* rate is present */
			{
				continue;  /* go to the next basic rate */
			}
			else  /* basic rate not present in ext rate set also */
			{
				return(OS_SUCCESS);
				/*return OS_FAIL; */
			}
		}
	}
	return(OS_SUCCESS); 
}

/******************************************************************************
*
* Name: isRateSetValid
*
* Description:
*
* Conditions For Use:
*    All software components have been initialized and started.
*
* Arguments:
*    Arg1 (i  ): 
* Return Value:
*    None.
*
* Notes:
*    None.
*
* PDL:

* END PDL
*
*****************************************************************************/
WL_STATUS isRateSetValid(UINT8 * RateSet)
{
	int i;
	i = 0;
	while (i < IEEEtypes_MAX_DATA_RATES
		&& RateSet[i] > 1 && RateSet[i] <= 127)
	{
		i++;
	}
	/* it should have had atleast one valid value */
	if (!i)
		return(OS_SUCCESS);
	else
		return(OS_FAIL);
}



UINT32 GetHighestRateIndex(vmacApInfo_t *vmacSta_p, IEEEtypes_SuppRatesElement_t *SuppRates, 
						   IEEEtypes_ExtSuppRatesElement_t *ExtSuppRates,BOOLEAN *gRatePresent)
{
	MIB_802DOT11 *mib=vmacSta_p->Mib802dot11;
	UINT32 i, Index;
	UINT32 HighestRateIndex=0, HighestOpRate;
	UINT8 MaxRate;


	if (*(mib->mib_ApMode)==AP_MODE_A_ONLY)
	{
		MaxRate=IEEEtypes_MAX_DATA_RATES_G;
	}
	else
	{
		MaxRate=IEEEtypes_MAX_DATA_RATES_A;
	}
	/*IEEEtypes_SuppRatesElement_t *SuppRates; */
	/*IEEEtypes_ExtSuppRatesElement_t *ExtSuppRates; */

	HighestOpRate = 0;
	*gRatePresent = FALSE;
	/*	SuppRates = &AssocTable[Aid].SuppRates; */
	/*	ExtSuppRates = &AssocTable[Aid].ExtSuppRates; */
	for ( i = 0; i < SuppRates->Len && SuppRates->Rates[i];i++ )
	{


		if ((Index = hw_GetPhyRateIndex(vmacSta_p,(SuppRates->Rates[i] &IEEEtypes_BASIC_RATE_MASK)))
			< MaxRate)
			/* valid rate for the Phy */
		{

			if ( Index > HIGHEST_11B_RATE_REG_INDEX)
				* gRatePresent = TRUE;
			if ((SuppRates->Rates[i] & IEEEtypes_BASIC_RATE_MASK) >= HighestOpRate)
			{
				HighestOpRate = (SuppRates->Rates[i] & IEEEtypes_BASIC_RATE_MASK);
				HighestRateIndex = Index;
			}
		}
		else /* the rate is not a valid state for the PHY */
		{
		}
	}
	if ( ExtSuppRates && ExtSuppRates->Len )
	{
		for ( i = 0; i < IEEEtypes_MAX_DATA_RATES && ExtSuppRates->Rates[i];i++)
		{


			if ((Index = hw_GetPhyRateIndex(vmacSta_p,(ExtSuppRates->Rates[i] &IEEEtypes_BASIC_RATE_MASK)))
				< IEEEtypes_MAX_DATA_RATES_G)
				/* valid rate for the Phy */
			{

				if ( Index > HIGHEST_11B_RATE_REG_INDEX)
					* gRatePresent = TRUE;
				if ((ExtSuppRates->Rates[i] & IEEEtypes_BASIC_RATE_MASK) >= HighestOpRate)
				{
					HighestOpRate = (ExtSuppRates->Rates[i] & IEEEtypes_BASIC_RATE_MASK);
					HighestRateIndex = Index;
				}
			}
			else /* the rate is not a valid state for the PHY */
			{

			}
		}
	}

	return(HighestRateIndex);
}
UINT32 SetSuppRateSetRegMap(vmacApInfo_t *vmacSta_p,IEEEtypes_SuppRatesElement_t *SuppRates,
							UINT32 * SuppRateSetRegMap)
{
	UINT32 i, Index;
	UINT32 HighestRateIndex, HighestOpRate;

	HighestRateIndex = 0;
	HighestOpRate = 0;
	for ( i = 0; i < SuppRates->Len;i++)
	{


		if ((Index = hw_GetPhyRateIndex(vmacSta_p,(SuppRates->Rates[i] & IEEEtypes_BASIC_RATE_MASK)))
			< IEEEtypes_MAX_DATA_RATES)
			/* valid rate for the Phy */
		{


			if (SuppRates->Rates[i] >= HighestOpRate)
			{
				HighestOpRate = SuppRates->Rates[i];
				HighestRateIndex = Index;
			}
			SuppRateSetRegMap[i] = Index;
		}
		else /* the rate is not a valid rate for the PHY */
		{
			/* use one of the valid rates in this position */
			SuppRateSetRegMap[i] = 1;
		}
	}
	return(HighestRateIndex);

}

void macMgmtMlme_StepUpRate(UINT16 Aid, UINT8 LastPktRate)
{
#ifdef ENABLE_RATE_ADAPTATION
	if ( LastPktRate < (AssocTable[Aid].SuppRates.Len - 1) )
	{
		AssocTable[Aid].RateToBeUsedForTx = LastPktRate+1;
	}
#endif
	/*CurrRateRegIndex = AssocTable[Aid].RateToBeUsedForTx; */
	/*for (i = 0; i < ;i++) */
	/*{ */
	/*    if ( CurrRateRegIndex == AssocTable[Aid].SuppRateSetRegMap[i]) */
	/*    { */
	/*        if ( i < AssocTable[Aid].SuppRates.Len - 1) */
	/*        { */
	/*            AssocTable[Aid].RateToBeUsedForTx = ++i; */
	/*        } */
	/*        else */
	/*        { */
	/* Rate remains the same */
	/*        } */
	/*        break; */
	/*    } */
	/*} */
	return ;
}
void macMgmtMlme_StepDownRate(UINT16 Aid, UINT8 LastPktRate)
{

#ifdef ENABLE_RATE_ADAPTATION
	if ( LastPktRate > 0 )
	{
		AssocTable[Aid].RateToBeUsedForTx = LastPktRate-1;
	}     /*CurrRateRegIndex = AssocTable[Aid].RateToBeUsedForTx; */
#endif
	/*for (i = 0; i < AssocTable[Aid].SuppRates.Len;i++) */
	/*{ */
	/*    if ( CurrRateRegIndex == AssocTable[Aid].SuppRateSetRegMap[i]) */
	/*    { */
	/*        if ( i > 0) */
	/*        { */
	/*            AssocTable[Aid].RateToBeUsedForTx = --i; */
	/*        } */
	/*        else */
	/*        { */
	/* Rate remains the same */
	/*        } */
	/*        break; */
	/*    } */
	/*} */
	return ;
}


void macMgmtCleanUp(vmacApInfo_t *vmacSta_p,extStaDb_StaInfo_t *StaInfo_p)
{
	if ( StaInfo_p->State == ASSOCIATED )
	{
#ifdef ENABLE_RATE_ADAPTATION_BASEBAND_REMOVE
		UpdateAssocStnData(StaInfo_p->Aid, StaInfo_p->ApMode);
#endif /*ENABLE_RATE_ADAPTATION_BASEBAND */
#ifdef FIX_LATER
		bcngen_UpdateBitInTim(StaInfo_p->Aid, RESETBIT);
#endif

		wlFwSetAPUpdateTim(vmacSta_p->dev, StaInfo_p->Aid, RESETBIT);
#ifdef AMPDU_SUPPORT
		cleanupAmpduTx(vmacSta_p,(UINT8 *)&StaInfo_p->Addr);
#endif

		/* remove the Mac address from the ethernet MAC address table */
		FreeAid(StaInfo_p->Aid);
		StaInfo_p->Aid = 0;
#ifdef ERP
		if ( StaInfo_p->ClientMode == BONLY_MODE )
			macMgmtMlme_DecrBonlyStnCnt(vmacSta_p, 0);
#endif
	}
}
void macMgmtRemoveSta(vmacApInfo_t *vmacSta_p,extStaDb_StaInfo_t *StaInfo_p)
{
	if(StaInfo_p->State == ASSOCIATED )
	{
		macMgmtCleanUp(vmacSta_p, StaInfo_p);
		StaInfo_p->State = AUTHENTICATED;
	}
	StaInfo_p->State = UNAUTHENTICATED;
	FreeStnId(StaInfo_p->StnId);
	extStaDb_DelSta(vmacSta_p,(IEEEtypes_MacAddr_t *)StaInfo_p->Addr, 0);
}


UINT32 BAPCount=0, ERPCount=0;
#define BSTATION_AGECOUNT 3
#define BSTATION_AGECOUNT 3
#define BEACON_OFFSET_LEN 0x100
#define RX_BCN_BUFSIZE     (2 * 1024)


///////////////////////////////////////////////////////////////////////////////
// Check to make sure pStaInfo is valid before calling this function!!
///////////////////////////////////////////////////////////////////////////////
void macMgmtMlme_UpdatePwrMode(vmacApInfo_t *vmacSta_p,struct ieee80211_frame *Hdr_p, extStaDb_StaInfo_t *pStaInfo)
{
	if (pStaInfo->PwrMode == PWR_MODE_ACTIVE)
	{
		if (Hdr_p->FrmCtl.PwrMgmt == 1)
		{
			/* Station enters power save mode */
			pStaInfo->PwrMode=PWR_MODE_PWR_SAVE;

			vmacSta_p->PwrSaveStnCnt++;
		}
	}
	else
	{
		if (Hdr_p->FrmCtl.PwrMgmt == 0)
		{
			/* Station enters active mode */
			pStaInfo->PwrMode = PWR_MODE_ACTIVE;
			/* Inform the transmit module */
			if (vmacSta_p->PwrSaveStnCnt)
				vmacSta_p->PwrSaveStnCnt--;
		}
#ifdef WMM_PS_SUPPORT
		else   /** powersave bit is still 1 **/
		{
			if(*(mib->QoSOptImpl)&&
				((pStaInfo->Qosinfo.Uapsd_ac_vo &&   /** All AC enable **/
				pStaInfo->Qosinfo.Uapsd_ac_vi  && 
				pStaInfo->Qosinfo.Uapsd_ac_be  &&
				pStaInfo->Qosinfo.Uapsd_ac_bk) ||
				/** mixed case **/
				(AccCategoryQ[ Prio]==AC_VO_Q && pStaInfo->Qosinfo.Uapsd_ac_vo)
				|| (AccCategoryQ[ Prio]==AC_VI_Q && pStaInfo->Qosinfo.Uapsd_ac_vi)
				|| (AccCategoryQ[ Prio]==AC_BE_Q && pStaInfo->Qosinfo.Uapsd_ac_be)
				|| (AccCategoryQ[ Prio]==AC_BK_Q && pStaInfo->Qosinfo.Uapsd_ac_bk)))


			{
				if(!TriggerFrameCnt(pStaInfo->StnId,2))  /** no pending trigger frame for that station **/
				{
					NotifyPwrModeChange(&Hdr_p->Addr2, PWR_MODE_PWR_SAVE,1,Prio);  /** this is actually the trigger frame **/

				}
				else
				{
					TriggerFrameCnt(pStaInfo->StnId,0); /** decrement trigger frame cnt for that station **/
				}
			}
		}
#endif
	}
}


#if defined(AP_SITE_SURVEY) || defined(AUTOCHANNEL)
/***************************** Added for Site Survey Start *******************************/

/*************************************************************************
* Function: syncSrv_ScanCmd
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern void syncSrv_ScanCmd(vmacApInfo_t *vmacSta_p,IEEEtypes_ScanCmd_t *ScanCmd_p )
{

	/* provide list of channel to ScanParams */
	memcpy((UINT8 *)&vmacSta_p->ScanParams, (UINT8 *)ScanCmd_p, sizeof(IEEEtypes_ScanCmd_t));

	/*-----------------------------------------*/
	/* Make sure channels to scan are provided */
	/*-----------------------------------------*/
	if (vmacSta_p->ScanParams.ChanList[0] == 0)
	{
		return;
	}

	/*--------------------------------------------------------*/
	/* Determine how many channels there are to scan from the */
	/* given list.                                            */
	/*--------------------------------------------------------*/
	vmacSta_p->NumScanChannels = 0;

	while(vmacSta_p->ScanParams.ChanList[vmacSta_p->NumScanChannels])
	{
		vmacSta_p->NumScanChannels++;
	}

	/* Prepare To Scan */
	/* Broadcast DeAuth Msg */
	macMgmtMlme_SendDeauthenticateMsg(vmacSta_p,&bcast, 0, IEEEtypes_REASON_DEAUTH_LEAVING);
	/* Flush Station Database */
	extStaDb_RemoveAllStns(vmacSta_p,IEEEtypes_REASON_DEAUTH_LEAVING);
	vmacSta_p->busyScanning = 1;
	vmacSta_p->ChanIdx = -1;
#ifdef COEXIST_20_40_SUPPORT
	if(!*(vmacSta_p->ShadowMib802dot11->mib_HT40MIntoler))

#endif
		EnableBlockTrafficMode(vmacSta_p);
	wlSetOpModeMCU(vmacSta_p,MCU_MODE_STA_INFRA);
	//   wlSetRFChan(vmacSta_p,vmacSta_p->ScanParams.ChanList[ChanIdx]);
#ifdef AUTOCHANNEL
	resetautochanneldata(vmacSta_p);
#endif

	/* Get and start a scan timer with duration of the maximum channel time */
	TimerInit(&vmacSta_p->scaningTimer);
	TimerFireIn(&vmacSta_p->scaningTimer, 1, &syncSrv_ScanActTimeOut, (unsigned char *)vmacSta_p, SCAN_TIME );

}
void Disable_ScanTimerProcess(vmacApInfo_t *vmacSta_p)
{
	TimerDisarm(&vmacSta_p->scaningTimer);
}

/*************************************************************************
* Function: syncSrv_ParseAttrib
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
#endif

extern void *syncSrv_ParseAttrib( macmgmtQ_MgmtMsg_t *mgtFrame_p, UINT8 attrib, UINT16 len)
{
	void *data_p;
	UINT32 lenPacket;
	UINT32 lenOffset;

	lenOffset = sizeof(IEEEtypes_MgmtHdr2_t)
		+ sizeof(IEEEtypes_TimeStamp_t)
		+ sizeof(IEEEtypes_BcnInterval_t) 
		+ sizeof(IEEEtypes_CapInfo_t);
	lenPacket = len - lenOffset;
	data_p = (UINT8 *)mgtFrame_p + 2 + lenOffset;
	while(lenOffset < len)
	{
		if(*(UINT8 *)data_p == attrib)
		{
			return data_p;
		}

		lenOffset += 2 + *( (UINT8 *)(data_p) + 1);
		data_p += 2 + *((UINT8 *)(data_p) + 1);
	}
	return NULL;
}



#ifdef IEEE80211H

static WL_STATUS macMgmtMlme_asso_ind(vmacApInfo_t *vmacSta_p,IEEEtypes_MacAddr_t *addr)
{    
	smeQ_MgmtMsg_t *toSmeMsg = NULL;   
	WLDBG_ENTER(DBG_LEVEL_7);
	if ((toSmeMsg=(smeQ_MgmtMsg_t *)malloc(sizeof(smeQ_MgmtMsg_t))) == NULL)
	{
		WLDBG_INFO(DBG_LEVEL_7, "macMgmtMlme_asso_ind: failed to alloc msg buffer\n");        
		return WL_STATUS_ERR;
	}

	memset(toSmeMsg, 0, sizeof(smeQ_MgmtMsg_t));

	toSmeMsg->MsgType = SME_NOTIFY_ASSOC_IND;

	memcpy(&toSmeMsg->Msg.AssocInd.PeerStaAddr , addr, sizeof(IEEEtypes_MacAddr_t));
	toSmeMsg->vmacSta_p = vmacSta_p;
	smeQ_MgmtWriteNoBlock(toSmeMsg);
	free((UINT8 *)toSmeMsg);
	WLDBG_EXIT(DBG_LEVEL_7);
	return TRUE;
}
/******************************************************************************
*
* Name: macMgmtMlme_DeauthenticateCmd
*
* Description:
*    Routine to handle a command to carry out a deauthentication with another
*    station or an AP.
*
* Conditions For Use:
*    All software components have been initialized and started.
*
* Arguments:
*    Arg1 (i  ): DeauthCmd_p - Pointer to a deauthenticate command
*
* Return Value:
*    None.
*
* Notes:
*    None.
*
* PDL:
*    If any of the given deauthentication parameters are invalid Then
*       Send a deauthentication confirmation to the SME with the failure
*          status
*    End If
*
*    Send a deauthentication message to the indicated station
*    Send a deauthentication confirm message to the SME task with the
*       result code
*    Set the state of the station to UNAUTHENTICATED
* END PDL
*
*****************************************************************************/

#if 1

void macMgmtMlme_MRequestReq(vmacApInfo_t *vmacSta_p,IEEEtypes_MRequestCmd_t *MrequestCmd_p)
{
	MIB_802DOT11 *mib=vmacSta_p->Mib802dot11;
	macmgmtQ_MgmtMsg2_t *MgmtMsg_p;
	extStaDb_StaInfo_t *StaInfo_p;
	IEEEtypes_MacAddr_t SrcMacAddr;
#ifdef AP_MAC_LINUX
	struct sk_buff *txSkb_p;
#else
	tx80211_MgmtMsg_t *TxMsg_p;
#endif
	WLDBG_ENTER(DBG_LEVEL_7);

	if ((*(mib->mib_ApMode)!=AP_MODE_AandG) && (*(mib->mib_ApMode)!=AP_MODE_A_ONLY))
	{
		WLDBG_INFO(DBG_LEVEL_7, "macMgmtMlme_MRequestReq: no need to sent in current AP mode(%d)\n", *(mib->mib_ApMode)); 
		return;  /* No need for those stations not in A mode */
	}

	if (!IS_BROADCAST((UINT8 *)MrequestCmd_p->PeerStaAddr))
	{    
		if ((StaInfo_p = extStaDb_GetStaInfo(vmacSta_p,&MrequestCmd_p->PeerStaAddr, STADB_DONT_UPDATE_AGINGTIME)) == NULL)
		{
			WLDBG_INFO(DBG_LEVEL_7, "macMgmtMlme_MRequestReq: no station found\n");         
			return;
		}

		if (StaInfo_p->ClientMode == AONLY_MODE)
			memcpy(SrcMacAddr, &vmacSta_p->macStaAddr, sizeof(IEEEtypes_MacAddr_t));
		else
		{
			WLDBG_INFO(DBG_LEVEL_7, "macMgmtMlme_MRequestReq: station is not in 11A mode(%d)\n\r", StaInfo_p->ClientMode); 
			return;
		}        
	}

	/* allocate buffer for message */
#ifdef AP_MAC_LINUX
	if ((txSkb_p = mlmeApiPrepMgtMsg(IEEE_MSG_QOS_ACTION, &MrequestCmd_p->PeerStaAddr, &SrcMacAddr)) == NULL)
		return;
	MgmtMsg_p = (macmgmtQ_MgmtMsg2_t *) txSkb_p->data;
#else
	if ((TxMsg_p = mlmeApiPrepMgtMsg(IEEE_MSG_QOS_ACTION, &MrequestCmd_p->PeerStaAddr, &SrcMacAddr)) != NULL)
#endif
	{
		UINT32 loop;
		UINT32 len = 0;

		MgmtMsg_p->Body.Action.Category = SPECTRUM_MANAGEMENT;
		MgmtMsg_p->Body.Action.Action = MEASUREMENT_REQUEST;
		MgmtMsg_p->Body.Action.DialogToken = MrequestCmd_p->DiaglogToken;
		for(loop = 0; loop<MrequestCmd_p->MeasureItems; loop++)
		{            
			MgmtMsg_p->Body.Action.Data.MeasurementRequest[loop].ElementId = MEASUREMENT_REQ;
			MgmtMsg_p->Body.Action.Data.MeasurementRequest[loop].Len = 3 + sizeof(IEEEtypes_MeasurementReq_t);
			MgmtMsg_p->Body.Action.Data.MeasurementRequest[loop].Token = MrequestCmd_p->MeasureReqSet[loop].MeasurementToken;
			MgmtMsg_p->Body.Action.Data.MeasurementRequest[loop].Mode = MrequestCmd_p->MeasureReqSet[loop].Mode;
			MgmtMsg_p->Body.Action.Data.MeasurementRequest[loop].Type = MrequestCmd_p->MeasureReqSet[loop].Type;           
			MgmtMsg_p->Body.Action.Data.MeasurementRequest[loop].Request = MrequestCmd_p->MeasureReqSet[loop].Request;

			len += sizeof(IEEEtypes_MeasurementRequestElement_t);
		}

		//MgmtMsg_p->Hdr.FrmBodyLen = len + 3; /* length of Measurement IEs plus 3(Category+action+DialogToken)*/
		WLDBG_INFO(DBG_LEVEL_7, "macMgmtMlme_MRequestReq: Tx action frame\n");
		if (txMgmtMsg(vmacSta_p->dev,txSkb_p) != OS_SUCCESS )
		{
			dev_kfree_skb_any(txSkb_p);
		}
	}

	WLDBG_EXIT(DBG_LEVEL_7);
}

#endif
//void macMgmtMlme_MReportReq(vmacApInfo_t *vmacSta_p,IEEEtypes_MReportCmd_t *MreportCmd_p)
void macMgmtMlme_MReportReq(struct net_device *staDev, UINT8 *macStaAddr, IEEEtypes_MReportCmd_t *MreportCmd_p)
{    
	macmgmtQ_MgmtMsg2_t *MgmtMsg_p;
	IEEEtypes_MacAddr_t SrcMacAddr;
#ifdef AP_MAC_LINUX
	struct sk_buff *txSkb_p;
#else
	tx80211_MgmtMsg_t *TxMsg_p;
#endif
	UINT32	ielen[20];
	UINT32	totalLen = 0 ;
	UINT32 loop;

	WLDBG_ENTER(DBG_LEVEL_7);


	memcpy(SrcMacAddr, macStaAddr, sizeof(IEEEtypes_MacAddr_t));
	for(loop = 0; loop<MreportCmd_p->ReportItems; loop++)
	{
		switch(MreportCmd_p->MeasureRepSet[loop].Type)
		{
#ifdef WMON
	case TYPE_REP_APS :
		ielen[loop] = 3 + 11 + strlen( MreportCmd_p->MeasureRepSet[loop].Report.data.APS) + 1; 
		break ;
	case TYPE_REP_DFS :
		ielen[loop] = 3 + 11 + strlen( MreportCmd_p->MeasureRepSet[loop].Report.data.DFS) + 1; 
		break ;
	case TYPE_REP_PSE :
		ielen[loop] = 3 + 11 + strlen( MreportCmd_p->MeasureRepSet[loop].Report.data.PSE) + 1; 
		break ;
	case TYPE_REP_RSS :
		ielen[loop] = 3 + 11 + 1 ;
		break ;
	case TYPE_REP_NOI :
		ielen[loop] = 3 + 11 + 8 ;
		break ;
	case TYPE_REP_FCS :
	case TYPE_REP_VRX :
		ielen[loop] = 3 + 11 + 4 ;
		break ;
#endif
	default:
		ielen[loop] = 0 ;
		}
		totalLen += ielen[loop] ;
	}
	/* allocate buffer for message */
#ifdef AP_MAC_LINUX
	//	if ((txSkb_p = mlmeApiPrepMgtMsg(IEEE_MSG_QOS_ACTION, &MreportCmd_p->PeerStaAddr, &SrcMacAddr)) == NULL)
	if ((txSkb_p = mlmeApiPrepMgtMsg2(IEEE_MSG_QOS_ACTION, &MreportCmd_p->PeerStaAddr, &SrcMacAddr, totalLen)) == NULL)
		return;
	MgmtMsg_p = (macmgmtQ_MgmtMsg2_t *) txSkb_p->data;
#else
	if ((TxMsg_p = mlmeApiPrepMgtMsg(IEEE_MSG_QOS_ACTION, &MreportCmd_p->PeerStaAddr, &SrcMacAddr)) != NULL)
#endif
	{
		UINT32 len = 0;
		IEEEtypes_MeasurementReportElement_t *IE_p;
		MgmtMsg_p->Body.Action.Category = SPECTRUM_MANAGEMENT;
		MgmtMsg_p->Body.Action.Action = MEASUREMENT_REPORT;
		MgmtMsg_p->Body.Action.DialogToken = MreportCmd_p->DiaglogToken;

		IE_p = MgmtMsg_p->Body.Action.Data.MeasurementReport;        
		for(loop = 0; loop<MreportCmd_p->ReportItems; loop++)
		{   
			UINT32 subLen = 0;

			IE_p->ElementId = MEASUREMENT_REP;
			IE_p->MeasurementToken = MreportCmd_p->MeasureRepSet[loop].MeasurementToken;
			IE_p->Mode = MreportCmd_p->MeasureRepSet[loop].Mode;
			IE_p->Type = MreportCmd_p->MeasureRepSet[loop].Type; 

			if ((MreportCmd_p->MeasureRepSet[loop].Mode.Incapable) ||/* problem to produce report result */
				(MreportCmd_p->MeasureRepSet[loop].Mode.Late) ||
				(MreportCmd_p->MeasureRepSet[loop].Mode.Refused))
				IE_p->Len = 3;
			else
			{
				memcpy( &IE_p->Report, &MreportCmd_p->MeasureRepSet[loop].Report, sizeof( IEEEtypes_MeasurementRep_t ));
			}
			/* this IE's length */            
			IE_p->Len = ielen[loop] ;
			subLen = ielen[loop] + 2;

			/* point to starting address of next IE */
			IE_p = (IEEEtypes_MeasurementReportElement_t *)((UINT8 *)IE_p + subLen);

			len += subLen;
		}

		txSkb_p->len = sizeof(struct ieee80211_frame) + len + 3 ;
		//MgmtMsg_p->Hdr.FrmBodyLen = len + 3; /* length of Measurement IEs plus 3(Category+action+DialogToken)*/
		WLDBG_INFO(DBG_LEVEL_7, "macMgmtMlme_MReportReq: Tx action frame. \n"); 
		if (txMgmtMsg(staDev,txSkb_p) != OS_SUCCESS )
		{
			dev_kfree_skb_any(txSkb_p);
		}
	}
	WLDBG_EXIT(DBG_LEVEL_7);

}

void macMgmtMlme_ChannelSwitchReq(vmacApInfo_t *vmacSta_p,IEEEtypes_ChannelSwitchCmd_t *ChannelSwitchtCmd_p)
{
	MIB_802DOT11 *mib=vmacSta_p->ShadowMib802dot11;
	macmgmtQ_MgmtMsg2_t *MgmtMsg_p;
	UINT16 FrameLen = sizeof(IEEEtypes_Category_t) +
		sizeof(IEEEtypes_ActionFieldType_t) +
		sizeof(IEEEtypes_ChannelSwitchAnnouncementElement_t);
#ifdef AP_MAC_LINUX
	struct sk_buff *txSkb_p;
#endif
#if 1 //def COMMON_PHYDSSS
	MIB_PHY_DSSS_TABLE *PhyDSSSTable=mib->PhyDSSSTable;
#else
	MIB_PHY_DSSS_TABLE *PhyDSSSTable=&mib->PhyDSSSTable;
#endif

	WLDBG_ENTER(DBG_LEVEL_7);

    if (dfs_test_mode)   /* DFS test mode - Send CSA Action frame. */
	{
#ifdef AP_MAC_LINUX
		if ((txSkb_p = mlmeApiPrepMgtMsg2(IEEE_MSG_QOS_ACTION, &bcast, &vmacSta_p->macStaAddr, FrameLen)) == NULL)
			return;
		MgmtMsg_p = (macmgmtQ_MgmtMsg2_t *) txSkb_p->data;
#else
		if ((TxMsg_p = mlmeApiPrepMgtMsg(IEEE_MSG_QOS_ACTION, &bcast, &vmacSta_p->macStaAddr)) != NULL)
#endif
		{       	     
			IEEEtypes_ChannelSwitchAnnouncementElement_t *ChannelSwitchAnnouncementElement_p;

			WLDBG_INFO(DBG_LEVEL_7, "macMgmtMlme_ChannelSwitchReq: Packing the ACTION frame\n");

			MgmtMsg_p->Body.Action.Category = SPECTRUM_MANAGEMENT;
			MgmtMsg_p->Body.Action.Action = CHANNEL_SWITCH_ANNOUNCEMENT;

			/* no token in action frame */
			ChannelSwitchAnnouncementElement_p = (IEEEtypes_ChannelSwitchAnnouncementElement_t *)&MgmtMsg_p->Body.Action.DialogToken;

			ChannelSwitchAnnouncementElement_p->ElementId = CSA;
			ChannelSwitchAnnouncementElement_p->Len = sizeof(IEEEtypes_ChannelSwitchCmd_t);       
			ChannelSwitchAnnouncementElement_p->Mode = ChannelSwitchtCmd_p->Mode;
			ChannelSwitchAnnouncementElement_p->Channel = ChannelSwitchtCmd_p->ChannelNumber;          
			ChannelSwitchAnnouncementElement_p->Count = ChannelSwitchtCmd_p->ChannelSwitchCount;

			if (txMgmtMsg(vmacSta_p->dev,txSkb_p) != OS_SUCCESS )
			{
				WLDBG_INFO(DBG_LEVEL_7, "macMgmtMlme_ChannelSwitchReq: Error sending out ACTION framee\n");
				dev_kfree_skb_any(txSkb_p);
			}
		}
	}


	if (1)//sendResult == TRUE)        
	{
		/* update beacon 
		* from system's perspective, code shouldn't be here,
		* anyway, i just put it here
		*/
		IEEEtypes_ChannelSwitchAnnouncementElement_t channelSwitchAnnouncementIE;

		channelSwitchAnnouncementIE.Mode = ChannelSwitchtCmd_p->Mode;
		channelSwitchAnnouncementIE.Channel = ChannelSwitchtCmd_p->ChannelNumber;
		channelSwitchAnnouncementIE.Count = ChannelSwitchtCmd_p->ChannelSwitchCount;

#ifdef IEEE80211_DH
#ifdef MRVL_DFS
		{		
			MIB_SPECTRUM_MGMT	*mib_SpectrumMagament_p=vmacSta_p->ShadowMib802dot11->SpectrumMagament;
			/*Store the CSA Parameters in Shadow MIB */
			mib_SpectrumMagament_p->csaChannelNumber = 
				ChannelSwitchtCmd_p->ChannelNumber ;
			mib_SpectrumMagament_p->csaCount = 
				ChannelSwitchtCmd_p->ChannelSwitchCount ;
			mib_SpectrumMagament_p->csaMode = 
				ChannelSwitchtCmd_p->Mode ;
		}
        if (!dfs_test_mode)  /* DFS test mode - do not update channel list, stay in current channel. */
        {
		    UpdateCurrentChannelInMIB(vmacSta_p, ChannelSwitchtCmd_p->ChannelNumber );
		    mib_Update();
        }
#ifdef DEBUG_PRINT
		WLDBG_INFO(DBG_LEVEL_0, "RECEIVED A CHANNEL SWITCH ANNOUNCEMENT COMMAND :%d:%d:%d\n",
			ChannelSwitchtCmd_p->ChannelNumber,
			ChannelSwitchtCmd_p->Mode,
			ChannelSwitchtCmd_p->ChannelSwitchCount);
#endif
#endif //MRVL_DFS
		wlFwSetChannelSwitchIE(vmacSta_p->dev, 
			ChannelSwitchtCmd_p->ChannelNumber, 
			ChannelSwitchtCmd_p->Mode, 
			ChannelSwitchtCmd_p->ChannelSwitchCount,
			PhyDSSSTable->Chanflag);
#else
		/* update ChannelSwitchAnnouncement IE*/
		bcngen_AddChannelSwithcAnnouncement_IE(vmacSta_p,&channelSwitchAnnouncementIE);
#endif // IEEE80211_DH

		/* disable mgmt and data ISR */
		//WL_WRITE_WORD(MIR_INTR_MASK, WL_REGS32(MIR_INTR_MASK) & ~(MSK_DATA_BUF_RDY|MSK_MGMT_BUF_RDY));

		/* enbale beacon free ISR */
		//bcngen_EnableBcnFreeIntr();
	}

	WLDBG_EXIT(DBG_LEVEL_7);

}

extern SINT8 BcnTxPwr;
static void macMgmtMlme_TPCReport(vmacApInfo_t *vmacSta_p,IEEEtypes_MacAddr_t *Addr, UINT8 DialogToken, UINT8 RSSI)
{
	macmgmtQ_MgmtMsg2_t *MgmtMsg_p;
	BOOLEAN sendResult = FALSE;
#ifdef AP_MAC_LINUX
	struct sk_buff *txSkb_p;
#else
	tx80211_MgmtMsg_t *TxMsg_p;
#endif
	WLDBG_ENTER(DBG_LEVEL_7);

#ifdef AP_MAC_LINUX
	if ((txSkb_p = mlmeApiPrepMgtMsg(IEEE_MSG_QOS_ACTION, Addr, &vmacSta_p->macStaAddr)) == NULL)
		return;
	MgmtMsg_p = (macmgmtQ_MgmtMsg2_t *) txSkb_p->data;
#else
	if ((TxMsg_p = mlmeApiPrepMgtMsg(IEEE_MSG_QOS_ACTION, Addr, &vmacSta_p->macStaAddr)) != NULL)
#endif
	{   
		IEEEtypes_TPCRepElement_t *TPCRepElement_p;

		MgmtMsg_p->Body.Action.Category = SPECTRUM_MANAGEMENT;
		MgmtMsg_p->Body.Action.Action = TPC_REPORT;
		MgmtMsg_p->Body.Action.DialogToken = DialogToken;


		TPCRepElement_p = (IEEEtypes_TPCRepElement_t *)&MgmtMsg_p->Body.Action.Data.TPCReport;

		TPCRepElement_p->ElementId = TPC_REP;
		TPCRepElement_p->Len = 2;       
		TPCRepElement_p->TxPwr = BcnTxPwr; /* approx. value as the one with beacon frame */            
		TPCRepElement_p->LinkMargin = RSSI/10;    	    

		//MgmtMsg_p->Hdr.FrmBodyLen = sizeof(IEEEtypes_TPCRepElement_t) + 3; /* length of Measurement IEs plus 3(Category+action+DiaglogToken) */    

		WLDBG_INFO(DBG_LEVEL_7, "macMgmtMlme_TPCReport: send out the action frame\n"); 
		if (txMgmtMsg(vmacSta_p->dev,txSkb_p) == OS_SUCCESS )
			sendResult = TRUE;    
		else
		{
			WLDBG_INFO(DBG_LEVEL_7, "macMgmtMlme_TPCReport: Error sending out ACTION framee\n"); 
			dev_kfree_skb_any(txSkb_p);
		}    	
	}
	WLDBG_EXIT(DBG_LEVEL_7);
}


#define	MIN(a,b) (((a)<(b))?(a):(b))
void msgMrequestIndPack(IEEEtypes_MRequestInd_t *MrequestInd_p, dot11MgtFrame_t *mgtFrame_p)
{
	UINT32 lenBody ;
	UINT32 loop_d, loop_s;
	UINT32 ieNum;

	/* cast it because the sourceInsight has problem to decode the structure definition
	* and make the code more readable
	*/
	IEEEtypes_ActionField_t *Action_p = &mgtFrame_p->Body.Action;
	IEEEtypes_MeasurementRequestElement_t *MeasurementRequest_p = &Action_p->Data.MeasurementRequest[0];
	/* estimate the number of IE in this frame 
	* the number of IE is easily be calculated because length of IE is constant in MREQUEST case
	* checking the len is probably is needed
	*/
	lenBody = mgtFrame_p->Hdr.FrmBodyLen - 4 - 4 /*FCS*/ - sizeof(IEEEtypes_MgmtHdr_t) + 6 + 2 ;
	ieNum = (lenBody - 3)/sizeof(IEEEtypes_MeasurementRequestElement_t);

	/*peerstaAddr is the the station the frame coming from */
	memcpy(MrequestInd_p->PeerStaAddr, mgtFrame_p->Hdr.SrcAddr, sizeof(IEEEtypes_MacAddr_t));

	/* take the dialog token
	* the dialog token is the token associated with the action frame
	*/
	MrequestInd_p->DiaglogToken = Action_p->DialogToken;

	/* get the request command set */
	for (loop_d = loop_s = 0; loop_s<MIN(ieNum, MAX_NR_IE); loop_d ++, loop_s++)        
	{
		/* a simple semantic check */
		if ((MeasurementRequest_p[loop_s].ElementId != MEASUREMENT_REQ) ||
			(MeasurementRequest_p[loop_s].Len != 3 + sizeof(IEEEtypes_MeasurementReq_t)))
		{
			loop_d--;            
			continue;
		}

		/* pack it */
		MrequestInd_p->MeasureReqSet[loop_d].MeasurementToken = MeasurementRequest_p[loop_s].Token;
		MrequestInd_p->MeasureReqSet[loop_d].Mode = MeasurementRequest_p[loop_s].Mode;
		MrequestInd_p->MeasureReqSet[loop_d].Type = MeasurementRequest_p[loop_s].Type;
		MrequestInd_p->MeasureReqSet[loop_d].Request = MeasurementRequest_p[loop_s].Request;

		MrequestInd_p->RequestItems++;
	}

	return;
}


static void msgMreportIndPack(IEEEtypes_MReportInd_t *MReportInd_p, macmgmtQ_MgmtMsg_t *MgmtMsg_p)
{    
	UINT32 idx = 0;    
	SINT32 remainLen = MgmtMsg_p->Hdr.FrmBodyLen /*- sizeof(RxSign_t)*/ -
		sizeof(IEEEtypes_GenHdr_t) - 3;

	/* cast it because the sourceInsight has problem to decode the structure definition
	* and make the code more readable
	*/
	IEEEtypes_ActionField_t *Action_p = &MgmtMsg_p->Body.Action;
	IEEEtypes_MeasurementReportElement_t *MeasurementReport_p = &Action_p->Data.MeasurementReport[0];

	/*peerstaAddr is the the station the frame coming from */
	memcpy(MReportInd_p->PeerStaAddr, MgmtMsg_p->Hdr.SrcAddr, sizeof(IEEEtypes_MacAddr_t));

	/* take the dialog token
	* the dialog token is the token associated with the action frame
	*/
	MReportInd_p->DiaglogToken = Action_p->DialogToken;

	/* get the request command set */
	while (remainLen > 0)  
	{   
		UINT32 ieLen = MeasurementReport_p->Len + 2;

		/* a simple semantic check */
		if (MeasurementReport_p->ElementId != MEASUREMENT_REP)
		{            
			/* remaining IE's total length */
			remainLen -= ieLen;

			/* mov to next IE */
			MeasurementReport_p = (IEEEtypes_MeasurementReportElement_t *)((UINT8 *)MeasurementReport_p + ieLen);
			continue;
		}

		/* pack it */       
		MReportInd_p->MeasureRepSet[idx].MeasurementToken = MeasurementReport_p->MeasurementToken;
		MReportInd_p->MeasureRepSet[idx].Mode = MeasurementReport_p->Mode;
		MReportInd_p->MeasureRepSet[idx].Type = MeasurementReport_p->Type;
		MReportInd_p->MeasureRepSet[idx].Report = MeasurementReport_p->Report;
		/* remaining IE's total length */
		remainLen -= ieLen;

		/* mov to next IE */
		MeasurementReport_p = (IEEEtypes_MeasurementReportElement_t *)((UINT8 *)MeasurementReport_p + ieLen);

		idx++;
	}

	MReportInd_p->ReportItems = idx;
	return;
}

static void msgChannelswitchIndPack(IEEEtypes_ChannelSwitchInd_t *ChannelSwitchInd_p, macmgmtQ_MgmtMsg_t *MgmtMsg_p)
{    
	/* cast it because the sourceInsight has problem to decode the structure definition
	* and make the code more readable
	*/
	IEEEtypes_ActionField_t *Action_p = &MgmtMsg_p->Body.Action;
	IEEEtypes_ChannelSwitchAnnouncementElement_t *ChannelSwitch_p = &Action_p->Data.ChannelSwitchAnnouncement;

	/* a simple semantic check */
	if ((ChannelSwitch_p->ElementId != CHANNEL_SWITCH_ANNOUNCEMENT)||(ChannelSwitch_p->Len != 3))
		return;

	/* 
	* pack the indication parameters in the primitive
	*/
	/*peerstaAddr is the the station the frame coming from */
	memcpy(ChannelSwitchInd_p->PeerStaAddr, MgmtMsg_p->Hdr.SrcAddr, sizeof(IEEEtypes_MacAddr_t));

	ChannelSwitchInd_p->Mode = ChannelSwitch_p->Mode;
	ChannelSwitchInd_p->ChannelNumber= ChannelSwitch_p->Channel;
	ChannelSwitchInd_p->ChannelSwitchCount= ChannelSwitch_p->Count;


	return;
}


extern BOOLEAN macMgmtMlme_80211hAct(vmacApInfo_t *vmacSta_p,macmgmtQ_MgmtMsg3_t *MgmtMsg_p )
{    
	MIB_802DOT11 *mib=vmacSta_p->Mib802dot11;
	MIB_STA_CFG  *mib_StaCfg_p=vmacSta_p->Mib802dot11->StationConfig;
	smeQ_MgmtMsg_t *toSmeMsg = NULL;   

	WLDBG_ENTER(DBG_LEVEL_7);
	/* whether the action frame is for 802.11h*/
	if(MgmtMsg_p->Body.Action.Category != SPECTRUM_MANAGEMENT)
		return FALSE;

	if (((*(mib->mib_ApMode) != AP_MODE_A_ONLY) && (*(mib->mib_ApMode)!=AP_MODE_AandG)) ||
		(mib_StaCfg_p->SpectrumManagementRequired != TRUE))
	{
		WLDBG_INFO(DBG_LEVEL_7, "macMgmtMlme_80211hAct : no need to do spectrum management\n");        
		return TRUE;
	}                          

	if ((toSmeMsg=(smeQ_MgmtMsg_t *)malloc(sizeof(smeQ_MgmtMsg_t))) == NULL)
	{
		WLDBG_INFO(DBG_LEVEL_7, "macMgmtMlme_80211hAct : failed to alloc msg buffer\n");        
		return TRUE;
	}

	memset(toSmeMsg, 0, sizeof(smeQ_MgmtMsg_t));

	switch(MgmtMsg_p->Body.Action.Action)
	{
		/* only for MLME-MREQUEST.ind */
	case MEASUREMENT_REQUEST:            
		toSmeMsg->MsgType = SME_NOTIFY_MREQUEST_IND;
		msgMrequestIndPack((IEEEtypes_MRequestInd_t *)&toSmeMsg->Msg.MrequestInd, (dot11MgtFrame_t *)MgmtMsg_p);           
		break;

		/* only for MLME-REPORT.ind */
	case MEASUREMENT_REPORT:
		toSmeMsg->MsgType = SME_NOTIFY_MREPORT_IND;
		msgMreportIndPack((IEEEtypes_MReportInd_t *)&toSmeMsg->Msg.MreportInd, (macmgmtQ_MgmtMsg_t *)MgmtMsg_p);
		break;

		/* start TPC protocol and processed in MLME */    
	case TPC_REQUEST:
		{
			//rx80211_MgmtMsg_t *RxBufPtr;

			//RxBufPtr = (rx80211_MgmtMsg_t *)((char *)MgmtMsg_p - sizeof(RxSign_t));

			/*
			* Call TPC protocol sub-function here
			*/
			macMgmtMlme_TPCReport(vmacSta_p,&MgmtMsg_p->Hdr.SrcAddr, MgmtMsg_p->Body.Action.DialogToken, 0/*RxBufPtr->u.RxSign.RSSI*/);
		}
		/* no msg have to be sent to SME
		* have to free the buffer
		*/
		free((UINT8 *)toSmeMsg);
		return TRUE;
		break;

		/* start TPC protocol and processed in MLME */
	case TPC_REPORT:            
		/*
		* Call TPC protocol sub-function here
		*/

		/*
		* send MLME-TPCADPT.cfm to SME
		*/
		free((UINT8 *)toSmeMsg);            
		return TRUE;
		break;

		/* only for MLME-CHANNELSWITCH.ind */    
	case CHANNEL_SWITCH_ANNOUNCEMENT:
		toSmeMsg->MsgType = SMC_NOTIFY_CHANNELSWITCH_IND;
		msgChannelswitchIndPack((IEEEtypes_ChannelSwitchInd_t *)&toSmeMsg->Msg.MrequestInd, (macmgmtQ_MgmtMsg_t *)MgmtMsg_p);              
		break;

	default:
		break;
	}
	toSmeMsg->vmacSta_p = vmacSta_p;
	smeQ_MgmtWriteNoBlock(toSmeMsg);
	free((UINT8 *)toSmeMsg);
	WLDBG_EXIT(DBG_LEVEL_7);

	return TRUE;
}
#endif /* IEEE80211H */
#ifdef COEXIST_20_40_SUPPORT
extern BOOLEAN macMgmtMlme_80211PublicAction(vmacApInfo_t *vmacSta_p,macmgmtQ_MgmtMsg3_t *MgmtMsg_p )
{    

	MIB_802DOT11 *mib =  vmacSta_p->ShadowMib802dot11;
	MIB_PHY_DSSS_TABLE *PhyDSSSTable=mib->PhyDSSSTable;
	IEEEtypes_20_40_Coexist_Act_t *CoexistAction;
	UINT8 i;


	if(!(*(vmacSta_p->ShadowMib802dot11->mib_HT40MIntoler)))
	{
		printk("Not in 20 40 mode now\n");
		return FALSE;
	}


	if(!(MgmtMsg_p->Body.Action.Category == ACTION_PUBLIC && MgmtMsg_p->Body.Action.Action == 0))
		return FALSE;

	if(!(PhyDSSSTable->Chanflag.FreqBand==FREQ_BAND_2DOT4GHZ
		&&  (PhyDSSSTable->Chanflag.ChnlWidth == CH_AUTO_WIDTH || 
		PhyDSSSTable->Chanflag.ChnlWidth == CH_40_MHz_WIDTH)))
		return FALSE;   /** accept only in 2.4G mode **/

	CoexistAction=(IEEEtypes_20_40_Coexist_Act_t *)&MgmtMsg_p->Body.Action.Category;

	if(CoexistAction->Coexist_Report.ElementId!=_20_40_BSSCOEXIST)
	{
		return FALSE;
	}




	if(CoexistAction->Coexist_Report.Coexist.FortyMhz_Intorant==1 ||
		CoexistAction->Coexist_Report.Coexist.TwentyMhz_BSS_Width_Request==1)
	{

		Handle20_40_Channel_switch( vmacSta_p,0);
	}


	if(CoexistAction->Intolerant_Report.ElementId!=_20_40_BSS_INTOLERANT_CHANNEL_REPORT)
	{
		printk("intolerant element %x does not exist\n",CoexistAction->Intolerant_Report.ElementId);
		return FALSE;

	}

	for(i=0;i<CoexistAction->Intolerant_Report.Len;i++)
	{

		if(CoexistAction->Intolerant_Report.ChanList[i]!=0)
		{
			extern int Check_40MHz_Affected_range(vmacApInfo_t *,int, int);

			if(Check_40MHz_Affected_range(vmacSta_p,CoexistAction->Intolerant_Report.ChanList[i],0))
				Handle20_40_Channel_switch( vmacSta_p,0);
			break;
		}

	}


	return TRUE;



}
#endif

UINT32 hw_GetPhyRateIndex(vmacApInfo_t *vmacSta_p,UINT32 Rate)
{
	MIB_802DOT11 *mib=vmacSta_p->Mib802dot11;
	UINT32 i; 
	UINT8 MaxRate;
	
	if(*(mib->mib_ApMode)==AP_MODE_A_ONLY || *(mib->mib_ApMode)==AP_MODE_AandN || *(mib->mib_ApMode)==AP_MODE_5GHZ_N_ONLY 
#ifdef SOC_W8864	
		|| (*(mib->mib_ApMode) == AP_MODE_5GHZ_Nand11AC) ||
		(*(mib->mib_ApMode) == AP_MODE_5GHZ_11AC_ONLY)
#endif	
	)
	{
		MaxRate=IEEEtypes_MAX_DATA_RATES_A;
	}
	else
		MaxRate=IEEEtypes_MAX_DATA_RATES_G;




	for (i=0; i < MaxRate-1; i++)
	{	
		if(*(mib->mib_ApMode)==AP_MODE_A_ONLY || *(mib->mib_ApMode)==AP_MODE_AandN || *(mib->mib_ApMode)==AP_MODE_5GHZ_N_ONLY 
#ifdef SOC_W8864	
		|| (*(mib->mib_ApMode) == AP_MODE_5GHZ_Nand11AC) ||
		(*(mib->mib_ApMode) == AP_MODE_5GHZ_11AC_ONLY)
#endif	
		)
		{
			if(PhyRatesA[i] == Rate)
				return i;
		}
		else
		{
			if( PhyRates[i] == Rate)
				return i;
		}

	}
	return(MaxRate+1);


}

UINT32 hw_GetPhyRateIndex2(UINT32 Rate)
{
	UINT32 i; 
	UINT8 MaxRate;


	MaxRate=IEEEtypes_MAX_DATA_RATES_G;




	for (i=0; i < MaxRate-1; i++)
	{
		{
			if( PhyRates[i] == Rate)
				return i;
		}

	}
	return(MaxRate+1);


}

extern WL_STATUS pool_FreeBuf ( char* ReturnedBuf_p )
{
	/* Dummy for now. */
	return SUCCESS;
}
inline void MonitorErp(vmacApInfo_t *vmacSta_p)
{
	if ((BStnAroundCnt==3) && (PreviousBAroundStnCount!=BStnAroundCnt))
	{
		macMgmtMlme_IncrBonlyStnCnt(vmacSta_p,1);
	} else if ((BStnAroundCnt==0) && (PreviousBAroundStnCount!=BStnAroundCnt))
	{
		macMgmtMlme_DecrBonlyStnCnt(vmacSta_p, 1);
	}

	PreviousBAroundStnCount=BStnAroundCnt;

	if (BAPCount>0 || ERPCount>0)
	{
		BStnAroundCnt=BSTATION_AGECOUNT;
	} else
	{
		if (BStnAroundCnt!=0)
		{
			BStnAroundCnt--;
		}
	}
	BAPCount=0;
	ERPCount=0;
}
void MonitorTimerProcess(UINT8 *data)
{
	vmacApInfo_t *vmacSta_p = (vmacApInfo_t *)data;
	extStaDb_AggrCk(vmacSta_p);
	if(vmacSta_p->monitorcnt++%3==0)
	{
		if(vmacSta_p->download == FALSE)
		{
			MonitorErp(vmacSta_p);
#ifdef NPROTECTION
			checkLegDevOutBSS(vmacSta_p);
#endif
		}
	}
#ifdef INTOLERANT40
	/* Check HT 40-20 switch in 30 min */
	if (sMonitorcnt30min)
	{
		/* Actually we set a 25 min timer */
		if (sMonitorcnt30min++%INTOLERANTCHEKCOUNTER == 0)
		{
			HT40MIntolerantHandler(vmacSta_p,0);
		}		
	}
#endif
	TimerRearm(&vmacSta_p->monTimer, MONITER_PERIOD_1SEC);
}


extern void RxBeacon(vmacApInfo_t *vmacSta_p, void *BssData_p, UINT16 len, UINT32 rssi)
{

	void     *attrib_p = NULL;
	IEEEtypes_ERPInfoElement_t *ErpInfo_p;
#ifdef COEXIST_20_40_SUPPORT
	MIB_802DOT11 *mib = vmacSta_p->ShadowMib802dot11;
	MIB_PHY_DSSS_TABLE *PhyDSSSTable=mib->PhyDSSSTable;
	IEEEtypes_DsParamSet_t *dsPSetIE_p;
	static UINT8 Scanbeacon=0;
	IEEEtypes_HT_Element_t *htSetIE_p;
	UINT8 HTCap=0, AddHtChannelOffset=0; 
#endif


	struct wlprivate    *wlpptr   = NETDEV_PRIV_P(struct wlprivate, vmacSta_p->dev);

#ifdef SOC_W8764
	{
		macmgmtQ_MgmtMsg_t *Beacon_p = NULL;
		Beacon_p = (macmgmtQ_MgmtMsg_t *)BssData_p;
		//printk("Rx Beacon from %s \n", mac_display(Beacon_p->Hdr.BssId));
	}
#endif
	if((attrib_p = syncSrv_ParseAttrib((macmgmtQ_MgmtMsg_t *)BssData_p, EXT_SUPPORTED_RATES,len)) != NULL)
	{
		if(((IEEEtypes_SuppRatesElement_t *)attrib_p)->Len > 0)
		{
		} else
			BAPCount++;
	} else if((attrib_p = syncSrv_ParseAttrib((macmgmtQ_MgmtMsg_t *)BssData_p, SUPPORTED_RATES,len)) != NULL)
	{
		if(((IEEEtypes_SuppRatesElement_t *)attrib_p)->Len > 4)
		{
		} else
			BAPCount++;
	}
	if((attrib_p = syncSrv_ParseAttrib((macmgmtQ_MgmtMsg_t *)BssData_p, ERP_INFO,len)) != NULL)
	{
		ErpInfo_p = (IEEEtypes_ERPInfoElement_t *)attrib_p;
		if (ErpInfo_p->ERPInfo.UseProtection==1 && ErpInfo_p->ERPInfo.NonERPPresent==1)
		{
			ERPCount++;
			BStnAroundCnt=BSTATION_AGECOUNT;
		}
	}
#ifdef NPROTECTION
	if((attrib_p = syncSrv_ParseAttrib((macmgmtQ_MgmtMsg_t *)BssData_p, HT,len)) == NULL)
	{
		wlpptr->wlpd_p->legAPCount = 1;
	}
#endif

#ifdef COEXIST_20_40_SUPPORT
	if(vmacSta_p->busyScanning)
		Scanbeacon=1;

	if(vmacSta_p->busyScanning && PhyDSSSTable->Chanflag.FreqBand == FREQ_BAND_2DOT4GHZ
		&& (PhyDSSSTable->Chanflag.ChnlWidth == CH_AUTO_WIDTH || 
		PhyDSSSTable->Chanflag.ChnlWidth == CH_40_MHz_WIDTH) && (rssi <= FortyMIntolerantRSSIThres))

	{	
		macmgmtQ_MgmtMsg_t * mgmt;
		extern int wlFwSet11N_20_40_Switch(struct net_device *netdev, UINT8 mode);




		mgmt=(macmgmtQ_MgmtMsg_t *)BssData_p;

		if((attrib_p = syncSrv_ParseAttrib((macmgmtQ_MgmtMsg_t *)BssData_p, HT, len)) != NULL)
		{
			htSetIE_p = (IEEEtypes_HT_Element_t *)attrib_p;


			HTCap=1;
			if(htSetIE_p->HTCapabilitiesInfo.FortyMIntolerant==1)
			{
				Handle20_40_Channel_switch( vmacSta_p,1);

			}


			if(htSetIE_p->HTCapabilitiesInfo.SupChanWidth) /** 40MHz channel capable AP found **/
			{
				void     *attrib_p2 = NULL;
				IEEEtypes_Add_HT_Element_t *AddhtSetIE_p;

				if((attrib_p2 = syncSrv_ParseAttrib((macmgmtQ_MgmtMsg_t *)BssData_p, ADD_HT,len)) != NULL)
				{

					HTCap=1;
					AddhtSetIE_p = (IEEEtypes_Add_HT_Element_t *)attrib_p2;


					if(AddhtSetIE_p->AddChan.STAChannelWidth)
					{
						AddHtChannelOffset=AddhtSetIE_p->AddChan.ExtChanOffset;
					}

				}


			}


		}


		if((attrib_p = syncSrv_ParseAttrib((macmgmtQ_MgmtMsg_t *)BssData_p, DS_PARAM_SET, len)) != NULL)
		{
			dsPSetIE_p = (IEEEtypes_DsParamSet_t *)attrib_p;


			/** if it is a 11n beacon, check for primary and extension channel ap, if not,
			only check for primary channel **/

			if(dsPSetIE_p->CurrentChan != PhyDSSSTable->CurrChan)
			{



				/** check whether channel is in interferance path , if so, do a channel switch **/
				if(Check_40MHz_Affected_range(vmacSta_p,dsPSetIE_p->CurrentChan,0))
				{

					Handle20_40_Channel_switch( vmacSta_p,1);
				}
			}

			else
			{
				if (!(HTCap &&  AddHtChannelOffset == PhyDSSSTable->Chanflag.ExtChnlOffset))
				{


					/** check whether channel is in interferance path , if so, do a channel switch **/
					if(Check_40MHz_Affected_range(vmacSta_p,dsPSetIE_p->CurrentChan,0))

						Handle20_40_Channel_switch( vmacSta_p,1);





				}
			}



		}

	}



#endif

}


/* DisableMacMgmtTimers - disable timers for removing module. */
extern void Disable_stationAgingTimer(vmacApInfo_t *vmacSta_p);
extern void Disable_GrpKeyTimer(vmacApInfo_t *vmacSta_p);

void DisableMacMgmtTimers(vmacApInfo_t *vmacSta_p)
{
	Disable_stationAgingTimer(vmacSta_p);
	Disable_MonitorTimerProcess(vmacSta_p);
	Disable_extStaDb_ProcessKeepAliveTimer(vmacSta_p);
	Disable_GrpKeyTimer(vmacSta_p);
#ifdef AUTOCHANNEL
	Disable_ScanTimerProcess(vmacSta_p);
#endif
}

void MacMgmtMemCleanup(vmacApInfo_t *vmacSta_p)
{
	extStaDb_Cleanup(vmacSta_p);
	mlmeAuthCleanup(vmacSta_p);
	StnIdListCleanup();
}


typedef struct RateConversion_t 
{
	UINT16  IEEERate;    
	UINT16  MrvlRateBitMap;
} RateToRateBitMapConversion_t;

RateToRateBitMapConversion_t IEEEToMrvlRateBitMapConversionTbl[] =
{
#define RateIndex5_1Mbps_BIT   0x00000001
#define RateIndex5_2Mbps_BIT   0x00000002
#define RateIndex5_5Mbps_BIT   0x00000004 
#define RateIndex11Mbps_BIT    0x00000008
#define RateIndex22Mbps_BIT    0x00000010
#define RateIndex6Mbps_BIT     0x00000020
#define RateIndex9Mbps_BIT     0x00000040
#define RateIndex12Mbps_BIT    0x00000080
#define RateIndex18Mbps_BIT    0x00000100
#define RateIndex24Mbps_BIT    0x00000200
#define RateIndex36Mbps_BIT    0x00000400
#define RateIndex48Mbps_BIT    0x00000800
#define RateIndex54Mbps_BIT    0x00001000
#define ENDOFTBL 0xFF

	{        2,    RateIndex5_1Mbps_BIT     },
	{        4,    RateIndex5_2Mbps_BIT     },
	{        11,   RateIndex5_5Mbps_BIT     },
	{        22,   RateIndex11Mbps_BIT      },
	{        44,   RateIndex22Mbps_BIT      },
	{        12,   RateIndex6Mbps_BIT       },
	{        18,   RateIndex9Mbps_BIT       },
	{        24,   RateIndex12Mbps_BIT      },
	{        36,   RateIndex18Mbps_BIT      },
	{        48,   RateIndex24Mbps_BIT      },
	{        72,   RateIndex36Mbps_BIT      },
	{        96,   RateIndex48Mbps_BIT      },
	{        108,  RateIndex54Mbps_BIT      },
	{        ENDOFTBL,  ENDOFTBL },
};

void IEEEToMrvlRateBitMapConversion(UINT8 SupportedIEEERate, UINT32 *pMrvlLegacySupportedRateBitMap)
{
	UINT32 i = 0;

	// Remove the highest bit which indicate if it is a basic rate.
	SupportedIEEERate = SupportedIEEERate & 0x7F;    

	while( IEEEToMrvlRateBitMapConversionTbl[i].IEEERate != ENDOFTBL)
	{
		if((IEEEToMrvlRateBitMapConversionTbl[i].IEEERate == SupportedIEEERate ))
		{
			*pMrvlLegacySupportedRateBitMap 
				= *pMrvlLegacySupportedRateBitMap | (IEEEToMrvlRateBitMapConversionTbl[i].MrvlRateBitMap);            
		}
		i++;
	}
}

static UINT32 GetLegacyRateBitMap(vmacApInfo_t *vmacSta_p,IEEEtypes_SuppRatesElement_t *SuppRates, IEEEtypes_ExtSuppRatesElement_t *ExtSuppRates)
{
	UINT16 i, j;
	//the maximum size of Rates[] should be RateIE->len + ExtSuppRateIE->len
	//for not overflow Rates[]
	UINT8  Rates[512] = {0};    
	UINT32 SupportedLegacyIEEERateBitMap = 0;

	/* Get legacy rates */    
	for ( i = 0; i < SuppRates->Len; i++ )
	{
		Rates[i] = SuppRates->Rates[i];    
	}

	if ( ExtSuppRates && ExtSuppRates->Len )
	{
		for ( j = 0; j < ExtSuppRates->Len; j++)
		{
			Rates[i+j] = ExtSuppRates->Rates[j];
		}
	}
#ifdef BRS_SUPPORT
	if (isClientRateMatchAP(vmacSta_p, Rates) == 0)
	{
		return 0;
	}
#endif
	/* Get legacy rate bit map */
	for( i = 0; i <IEEEtypes_MAX_DATA_RATES_G; i++)
	{
		IEEEToMrvlRateBitMapConversion(Rates[i], &SupportedLegacyIEEERateBitMap); 
	}

	//WLDBG_INFO(DBG_LEVEL_7,"SupportedLegacyIEEERateBitMap 0x%x\n", SupportedLegacyIEEERateBitMap);
	return SupportedLegacyIEEERateBitMap; 
}
#ifdef BRS_SUPPORT
int isClientRateMatchAP(vmacApInfo_t *vmacSta_p,UINT8  *Rates)
{
	UINT32 i, j, findRate;
	UINT32 rateMask; 
	MIB_802DOT11 *mib = vmacSta_p->Mib802dot11;

	/* Check that AP BSS Basic Rates need be supported by client all rates */
	rateMask = *(mib->BssBasicRateMask);
	i = 0;
	while(rateMask) 
	{
		if (rateMask & 0x01)
		{
			j = 0;
			findRate = 0;

			while (Rates[j])
			{
				if (mib->StationConfig->OpRateSet[i] == (Rates[j] & 0x7F))
				{
					findRate = 1;
					break;							
				}
				j++;
			}
			if(!findRate)
			{
				//WLDBG_INFO(DBG_LEVEL_7,"AP Basic match fail.\n");
				return 0;
			}
		}
		rateMask >>= 1;
		i++;
	}

	/* Check that client BSS Basic Rates need be supported by AP all rates */
	i = 0;
	while (Rates[i])
	{
		if (Rates[i] & 0x80)
		{
			j = 0;
			findRate = 0;
			rateMask = *(mib->BssBasicRateMask) | *(mib->NotBssBasicRateMask);
			while (rateMask)
			{
				if (rateMask & 0x01)
				{
					if ((Rates[i] & 0x7F) == mib->StationConfig->OpRateSet[j]) 
					{
						findRate = 1;
						break;							
					}
				}
				rateMask >>= 1;
				j++;
			}
			if(!findRate)
			{
				//WLDBG_INFO(DBG_LEVEL_7,"Client Basic match fail.\n");
				return 0;
			}
		}
		i++;
	}
	return 1;

}
#endif

void ClientStatistics(vmacApInfo_t *vmacSta_p, extStaDb_StaInfo_t *pStaInfo)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, vmacSta_p->dev);

	if (pStaInfo->State == ASSOCIATED)
	{
		vmacSta_p->numClients++;
		if(pStaInfo->ClientMode == NONLY_MODE) 
		{
			vmacSta_p->nClients++;
			wlpptr->wlpd_p->nClients++;
			if (pStaInfo->HtElem.HTCapabilitiesInfo.SupChanWidth)
				vmacSta_p->n40MClients++;    
			else
			{
				vmacSta_p->n20MClients++;
				wlpptr->wlpd_p->n20MClients++;
			}
			if (!pStaInfo->PeerHTCapabilitiesInfo.GreenField)
			{
				vmacSta_p->NonGFSta++;
				wlpptr->wlpd_p->NonGFSta++;
			}
		} else {
			vmacSta_p->legClients++;
			wlpptr->wlpd_p->legClients++;
			if(pStaInfo->ClientMode == BONLY_MODE)
				vmacSta_p->bClients++;
			else
				vmacSta_p->gaClients++;                    
		}
	}
}

void CleanCounterClient(vmacApInfo_t *vmacSta_p )
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, vmacSta_p->dev);
	vmacSta_p->numClients = 0;
	vmacSta_p->nClients = 0;
	vmacSta_p->legClients = 0;
	vmacSta_p->n40MClients = 0;
	vmacSta_p->n20MClients = 0;					
	vmacSta_p->gaClients = 0;
	vmacSta_p->bClients = 0;
	vmacSta_p->NonGFSta = 0;
	wlpptr->wlpd_p->NonGFSta = 0;
	wlpptr->wlpd_p->legClients = 0;
	wlpptr->wlpd_p->n20MClients	= 0;
	wlpptr->wlpd_p->nClients = 0;
}
#ifdef COEXIST_20_40_SUPPORT
void Coexist_TimeoutHdlr(void *pData)
{
	extern int wlFwSet11N_20_40_Switch(struct net_device *netdev, UINT8 mode);
	extern BOOLEAN macMgmtMlme_SendNotifyChannelWidthManagementAction(vmacApInfo_t *vmacSta_p, UINT8 mode);



	vmacApInfo_t *vmacSta_p = (vmacApInfo_t *)pData;
	MIB_802DOT11 *mib = vmacSta_p->ShadowMib802dot11;
	MIB_PHY_DSSS_TABLE *PhyDSSSTable=mib->PhyDSSSTable;
	UINT8 BcnAddHtAddChannel = 0;

	if(*(vmacSta_p->ShadowMib802dot11->mib_HT40MIntoler))
	{
		if(*(vmacSta_p->Mib802dot11->mib_ApMode)== AP_MODE_N_ONLY
			|| *(vmacSta_p->Mib802dot11->mib_ApMode) == AP_MODE_BandN
			|| *(vmacSta_p->Mib802dot11->mib_ApMode) == AP_MODE_GandN
#ifdef SOC_W8864			
			|| *(vmacSta_p->Mib802dot11->mib_ApMode) == AP_MODE_2_4GHZ_11AC_MIXED
#endif			
			|| *(vmacSta_p->Mib802dot11->mib_ApMode) == AP_MODE_BandGandN)
		{

			int mode=1;
			/** what to do if auto fktang **/
			if(PhyDSSSTable->Chanflag.ExtChnlOffset==EXT_CH_ABOVE_CTRL_CH)
				BcnAddHtAddChannel =5;
			else  if(PhyDSSSTable->Chanflag.ExtChnlOffset==EXT_CH_BELOW_CTRL_CH)
				BcnAddHtAddChannel=7;


			macMgmtMlme_SendNotifyChannelWidthManagementAction(vmacSta_p,1);
			/** 5 is for upper and 7 is for lower **/
			wlFwSet11N_20_40_Switch(vmacSta_p->dev, BcnAddHtAddChannel);
			*(mib->USER_ChnlWidth )=mode;

		}
	}	/** switch back to 40MHz here **/
	TimerDisarm(&vmacSta_p->CoexistTimer);

}
void Coexist_RearmTimer(vmacApInfo_t *vmacSta_p)
{
	MIB_802DOT11 *mib = vmacSta_p->ShadowMib802dot11;

	TimerRearm(&vmacSta_p->CoexistTimer, *(mib->mib_Channel_Width_Trigger_Scan_Interval)*(*(mib->mib_Channel_Transition_Delay_Factor))*10);
}


void StartCoexistTimer(vmacApInfo_t *vmacSta_p)
{
	MIB_802DOT11 *mib = vmacSta_p->Mib802dot11;
	TimerRemove(&vmacSta_p->CoexistTimer);
	TimerInit(&vmacSta_p->CoexistTimer);

	/* Coexist  in seconds, base timer is 25s. */
	TimerFireIn(&vmacSta_p->CoexistTimer, 1,Coexist_TimeoutHdlr,(unsigned char *)vmacSta_p, *(mib->mib_Channel_Width_Trigger_Scan_Interval)*(*(mib->mib_Channel_Transition_Delay_Factor))*10); 

}

void Disable_StartCoexisTimer(vmacApInfo_t *vmacSta_p)
{
	TimerRemove(&vmacSta_p->CoexistTimer);
}


int Check_40MHz_Affected_range(vmacApInfo_t *vmacSta_p, int curchannel, int extchan)
{
	// 40Mhz affected channel range = [ (fp+fs)/2-25MHz, (fp+fs)/2+25MHz]
	int lowchannel, highchannel,fp,fs=0;
	MIB_802DOT11 *mib = vmacSta_p->ShadowMib802dot11;
	MIB_PHY_DSSS_TABLE *PhyDSSSTable=mib->PhyDSSSTable;
	int channel[]={2412,2417, 2422,2427,2432,2437,2442,2447,2452,2457,2462,
		2467,2472,2484};

	fp=channel[PhyDSSSTable->CurrChan-1];

	if(PhyDSSSTable->Chanflag.ExtChnlOffset==EXT_CH_ABOVE_CTRL_CH)
		fs=channel[PhyDSSSTable->CurrChan+4-1];
	else if(PhyDSSSTable->Chanflag.ExtChnlOffset==EXT_CH_BELOW_CTRL_CH)
		fs=channel[PhyDSSSTable->CurrChan-4-1];

	lowchannel=(fp+fs)/2-25;
	highchannel=(fp+fs)/2+25;


	if(channel[curchannel-1]  >= lowchannel && channel[curchannel-1] <=highchannel)
	{
		return 1;
	}
	else
	{
		return 0;
	}

}
void Handle20_40_Channel_switch(vmacApInfo_t *vmacSta_p, UINT8 Scanning )
{
	MIB_802DOT11 *mib =vmacSta_p->ShadowMib802dot11;
	UINT8 BcnAddHtAddChannel = 0;
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, vmacSta_p->dev);


	if (*(vmacSta_p->ShadowMib802dot11->mib_HT40MIntoler) /*wlpptr->wlpd_p->BcnAddHtAddChannel!= BcnAddHtAddChannel*/)
	{
		extern int wlFwSet11N_20_40_Switch(struct net_device *netdev, UINT8 mode);
		extern BOOLEAN macMgmtMlme_SendNotifyChannelWidthManagementAction(vmacApInfo_t *vmacSta_p, UINT8 mode);

		if(((*(mib->USER_ChnlWidth ))&0x0f)==0x00)  /** we are already at 20MHz mode **/
		{
			StartCoexistTimer(vmacSta_p);  /** restart timer, sta already reported intolerant **/

			return;
		}

		wlpptr->wlpd_p->BcnAddHtAddChannel = BcnAddHtAddChannel;

		wlFwSet11N_20_40_Switch(vmacSta_p->dev, BcnAddHtAddChannel);
		/** Start timer to switch back to 40MHz **/
		if(vmacSta_p->n40MClients)  /** start timer only if there is 11n 40M sta **/
			StartCoexistTimer(vmacSta_p);
		if(Scanning)
			*(mib->USER_ChnlWidth )=0x10;  /** bit 4 use to indicate bw change during scanning **/
		else
			*(mib->USER_ChnlWidth )=0x0;	

		macMgmtMlme_SendNotifyChannelWidthManagementAction(vmacSta_p,0);
	}
}


#endif

void HandleNProtectionMode(vmacApInfo_t *vmacSta_p )
{
	MIB_802DOT11 *mib = vmacSta_p->Mib802dot11;
	UINT8 BcnAddHtOpMode = 0;
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, vmacSta_p->dev);
	UINT8 TxGf = 0;

	if(vmacSta_p->OpMode >= WL_OP_MODE_STA)
	{
		//the following will not apply to station mode operation, simply return
		return;
	}

	if (*(mib->mib_ApMode)&0x4)
	{
		/*	Draft 11, page 148, Line 65
			The HT Protection field is set to non-HT mixed mode otherwise.
		*/
		if (wlpptr->wlpd_p->legClients )
		{
			BcnAddHtOpMode = 0x03;
		} 
		else if (wlpptr->wlpd_p->nClients) 
		{
			/*  Draft 11, page 148, Line 49-54
				The HT Protection field may be set to non-member protection mode only if:
				-A non-HT STA is detected in either the primary or the secondary channel or 
				in both the primary and secondary channels, that is not known by the transmitting STA 
				to be a member of this BSS, and
				-All STAs that are known by the transmitting STA to be a member of this BSS are HT STAs*/
			if (wlpptr->wlpd_p->legAPCount)
			{
				BcnAddHtOpMode = 0x01;
			} 

            /* Draft 11, page 148, Line 57-63
			The HT Protection field may be set to 20 MHz protection mode only if:
			 -All STAs detected in the primary and all STAs detected in the secondary channel are HT STAs and    
				all STAs that are members of this BSS are HT STAs, and
			 -This BSS is a 20/40 MHz BSS, and
			 -There is at least one 20 MHz HT STA associated with this BSS
			*/
			else if (wlpptr->wlpd_p->n20MClients && ((*(mib->USER_ChnlWidth )&0x0f)==1)) 
			{
				BcnAddHtOpMode = 0x02;
			} 
			else 
			{
                /* The HT Protection field may be set to no protection mode only if the following are true:
                    — All STAs detected (by any means) in the primary or the secondary channel are HT STAs, and
                    — All STAs that are known by the transmitting STA to be a member of this BSS are either
                    — 20/40 MHz HT STAs in a 20/40 MHz BSS, or
                    — 20 MHz HT STAs in a 20 MHz BSS*/

				BcnAddHtOpMode = 0x00;
			}	
		} 
		else 
		{
            /*The HT Protection field may be set to nonmember protection mode only if the following are true:
            — A non-HT STA is detected (by any means) in either the primary or the secondary channel or in both
            the primary and secondary channels, that is not known by the transmitting STA to be a member of
            this BSS, and
            — All STAs that are known by the transmitting STA to be a member of this BSS are HT STAs */
			if (wlpptr->wlpd_p->legAPCount) 
			{
				BcnAddHtOpMode = 0x01;
			} 
			else
				BcnAddHtOpMode = 0x00;
			}
			
		if (*(mib->mib_HtGreenField))
		{ 
			if((!wlpptr->wlpd_p->NonGFSta) && ((!BcnAddHtOpMode) || (BcnAddHtOpMode == 2)))
			{
				TxGf = 1;
				BcnAddHtOpMode &= ~0x0004;  /* bit2 is non-GF present */			
			}
			else
				BcnAddHtOpMode |= 0x0004;

			if (wlpptr->wlpd_p->TxGf != TxGf)
				wlFwSetHTGF(vmacSta_p->dev, TxGf);

			wlpptr->wlpd_p->TxGf = TxGf;
		}
		else
		{
			BcnAddHtOpMode |= 0x0004;
			wlFwSetHTGF(vmacSta_p->dev, TxGf); /*Bug 46196: Added to handle disabled GF fw update*/
		}
	}

	if (wlpptr->wlpd_p->BcnAddHtOpMode != BcnAddHtOpMode)
	{
		wlpptr->wlpd_p->BcnAddHtOpMode = BcnAddHtOpMode;
		wlFwSetNProtOpMode(vmacSta_p->dev, BcnAddHtOpMode);
	}
}

void checkLegDevOutBSS(vmacApInfo_t *vmacSta_p )     
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, vmacSta_p->dev);
    UINT8 HWIndex = vmacSta_p->VMacEntry.phyHwMacIndx;

	if ((legacyAPCount[HWIndex] != wlpptr->wlpd_p->legAPCount) || (counterHtProt[HWIndex]++%10==0)) /* 30s check */
	{
		legacyAPCount[HWIndex] = wlpptr->wlpd_p->legAPCount;
		HandleNProtectionMode(vmacSta_p);
	}
	wlpptr->wlpd_p->legAPCount = 0; 
}

BOOLEAN macMgmtMlme_SendMimoPsHtManagementAction(vmacApInfo_t *vmacSta_p,IEEEtypes_MacAddr_t *Addr, UINT8 mode)
{
	macmgmtQ_MgmtMsg2_t *MgmtMsg_p;
	struct sk_buff *txSkb_p;
	IEEEtypes_SM_PwrCtl_t *p;

	UINT16 FrameLen = sizeof(IEEEtypes_Category_t) +
		sizeof(IEEEtypes_ActionFieldType_t) +
		sizeof(IEEEtypes_SM_PwrCtl_t);

	if ((txSkb_p = mlmeApiPrepMgtMsg2(IEEE_MSG_QOS_ACTION, Addr, (IEEEtypes_MacAddr_t *)&vmacSta_p->macStaAddr, FrameLen)) == NULL)
		return FALSE;

	MgmtMsg_p  = (macmgmtQ_MgmtMsg2_t *) ((UINT8 *) txSkb_p->data);		 

	MgmtMsg_p->Body.Act.Category = HT_CATEGORY;
	MgmtMsg_p->Body.Act.Action = ACTION_SMPS;

	p = &MgmtMsg_p->Body.Act.Field.SmPwrCtl;

	p->Mode = mode;
	if (mode == 0x03)
	{
		p->Enable = 0;
		p->Mode = 0;
	}
	else
		p->Enable = 1;	

	if (txMgmtMsg(vmacSta_p->dev, txSkb_p) != OS_SUCCESS )
	{
		dev_kfree_skb_any(txSkb_p);
		return FALSE;
	}

	return TRUE;
}
#ifdef COEXIST_20_40_SUPPORT
BOOLEAN macMgmtMlme_SendNotifyChannelWidthManagementAction(vmacApInfo_t *vmacSta_p, UINT8 mode)
{
	macmgmtQ_MgmtMsg2_t *MgmtMsg_p;
	struct sk_buff *txSkb_p;
	IEEEtypes_BWCtl_t *p;
	IEEEtypes_MacAddr_t bcastMacAddr = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};


	UINT16 FrameLen = sizeof(IEEEtypes_Category_t) +
		sizeof(IEEEtypes_ActionFieldType_t) +
		sizeof(IEEEtypes_BWCtl_t);

	if ((txSkb_p = mlmeApiPrepMgtMsg2(IEEE_MSG_QOS_ACTION, &bcastMacAddr, (IEEEtypes_MacAddr_t *)&vmacSta_p->macStaAddr, FrameLen)) == NULL)
		return FALSE;

	MgmtMsg_p  = (macmgmtQ_MgmtMsg2_t *) ((UINT8 *) txSkb_p->data);		 

	MgmtMsg_p->Body.Act.Category = HT_CATEGORY;
	MgmtMsg_p->Body.Act.Action = ACTION_NOTIFYCHANNELWIDTH;

	p = &MgmtMsg_p->Body.Act.Field.BWCtl;

	p->BandWidth= mode;


	if (txMgmtMsg(vmacSta_p->dev, txSkb_p) != OS_SUCCESS )
	{
		dev_kfree_skb_any(txSkb_p);
		return FALSE;
	}

	return TRUE;
}
#endif
#ifdef INTOLERANT40
BOOLEAN macMgmtMlme_SendInfoExchHtManagementAction(struct net_device *dev, IEEEtypes_MacAddr_t *Addr)
{
	macmgmtQ_MgmtMsg2_t *MgmtMsg_p;
	struct sk_buff *txSkb_p;
	IEEEtypes_InfoExch_t *p;

	UINT16 FrameLen = sizeof(IEEEtypes_Category_t) +
		sizeof(IEEEtypes_ActionFieldType_t) +
		sizeof(IEEEtypes_InfoExch_t);

	if ((txSkb_p = mlmeApiPrepMgtMsg2(IEEE_MSG_QOS_ACTION, Addr, (IEEEtypes_MacAddr_t *)&dev->dev_addr, FrameLen)) == NULL)
		return FALSE;

	MgmtMsg_p  = (macmgmtQ_MgmtMsg2_t *) ((UINT8 *) txSkb_p->data);		 

	/* Send event to user space */
	WLSNDEVT(dev,IWEVEXPIRED, Addr, NULL); 

	MgmtMsg_p->Body.Act.Category = HT_CATEGORY;;
	MgmtMsg_p->Body.Act.Action = ACTION_INFOEXCH;

	p = &MgmtMsg_p->Body.Act.Field.InfoExch;

	p->InfoReq = 1;

	if (txMgmtMsg(dev, txSkb_p) != OS_SUCCESS )
	{
		dev_kfree_skb_any(txSkb_p);
		return FALSE;
	}

	return TRUE;
}

BOOLEAN macMgmtMlme_SendBeaconReqMeasureReqAction(struct net_device *dev, IEEEtypes_MacAddr_t *Addr)
{
	extern UINT8 getRegulatoryClass(vmacApInfo_t *vmacSta_p);
	macmgmtQ_MgmtMsg2_t *MgmtMsg_p;
	struct sk_buff *txSkb_p;
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, dev);
	vmacApInfo_t *vmacSta_p = (vmacApInfo_t *)wlpptr->vmacSta_p;
	MIB_802DOT11 *mib = vmacSta_p->Mib802dot11;
	IEEEtypes_MeasurementRequestEl_t *p;

	UINT16 FrameLen = sizeof(IEEEtypes_Category_t) +
		sizeof(IEEEtypes_ActionFieldType_t) +
		sizeof(UINT8) +
		sizeof(UINT16) +
		sizeof(IEEEtypes_MeasurementRequestEl_t) -
		(34- strlen(&(mib->StationConfig->DesiredSsId[0])));

	if ((txSkb_p = mlmeApiPrepMgtMsg2(IEEE_MSG_QOS_ACTION, Addr, (IEEEtypes_MacAddr_t *)&dev->dev_addr, FrameLen)) == NULL)
		return FALSE;

	MgmtMsg_p  = (macmgmtQ_MgmtMsg2_t *) ((UINT8 *) txSkb_p->data);		 

	MgmtMsg_p->Body.Act.Category = RADIO_MEASURE_CATEGOTY;
	MgmtMsg_p->Body.Act.Action = MEASUREMENT_REQUEST;
	MgmtMsg_p->Body.Act.Field.Field5.DialogToken = 0;
	MgmtMsg_p->Body.Act.Field.Field5.NumRepetition = 0;

	p = &MgmtMsg_p->Body.Act.Field.Field5.Data.MeasurementRequestEl;

	p->ElementId = MEASUREMENT_REQ;
	p->Token = 0;
	p->Mode.Enable = 1;
	p->Mode.Request = 1;
	p->Mode.Report = 0;
	p->Type = TYPE_REQ_BCN;           

	p->Request.RegClass = getRegulatoryClass(vmacSta_p);
	memcpy(p->Request.BSSID, &vmacSta_p->macStaAddr, sizeof(IEEEtypes_MacAddr_t));
	p->Request.ChanNum = 255;
	p->Request.RandInterval = 0;
	p->Request.Mode = 0;
	p->Request.Duration = 10;
	p->Request.ReportCondi = 254;
	p->Request.Threshold_offset = 1;
	memcpy(p->Request.SSID, &(mib->StationConfig->DesiredSsId[0]), strlen(&(mib->StationConfig->DesiredSsId[0])));

	p->Len  = 3 + sizeof(IEEEtypes_MeasurementReqBcn_t) - (34- strlen(&(mib->StationConfig->DesiredSsId[0])));

	if (txMgmtMsg(dev, txSkb_p) != OS_SUCCESS )
	{
		dev_kfree_skb_any(txSkb_p);
		return FALSE;
	}
	//printk("MEASUREMENT_REQ %d\n", MEASUREMENT_REQ);
	return TRUE;
}

BOOLEAN macMgmtMlme_SendExtChanSwitchAnnounceAction(struct net_device *dev, IEEEtypes_MacAddr_t *Addr)
{
	extern UINT8 getRegulatoryClass(vmacApInfo_t *vmacSta_p);
	macmgmtQ_MgmtMsg2_t *MgmtMsg_p;
	struct sk_buff *txSkb_p;
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, dev);
	vmacApInfo_t *vmacSta_p = (vmacApInfo_t *)wlpptr->vmacSta_p;
	MIB_802DOT11 *mib = vmacSta_p->Mib802dot11;
	IEEEtypes_ExtendChanSwitchAnnounceEl_t *p;

	UINT16 FrameLen = sizeof(IEEEtypes_Category_t) +
		sizeof(IEEEtypes_ActionFieldType_t) +
		sizeof(IEEEtypes_ExtendChanSwitchAnnounceEl_t);

	if ((txSkb_p = mlmeApiPrepMgtMsg2(IEEE_MSG_QOS_ACTION, Addr, (IEEEtypes_MacAddr_t *)&dev->dev_addr, FrameLen)) == NULL)

		return FALSE;

	MgmtMsg_p  = (macmgmtQ_MgmtMsg2_t *) ((UINT8 *) txSkb_p->data);		 

	MgmtMsg_p->Body.Act.Category = SPECTRUM_MANAGE_CATEGOTY;
	MgmtMsg_p->Body.Act.Action = ACTION_EXTCHANSWTANNO;

	p = &MgmtMsg_p->Body.Act.Field.ExtendChanSwitchAnnounceEl;

	p->ElementId = 60; // Temp
	p->ChanSwitchMode = 1;
	p->RegClass = getRegulatoryClass(vmacSta_p);
	p->ChanNum = mib->PhyDSSSTable->CurrChan;
	p->ChanSwitchCount = 0;

	p->Len  = 4;

	if (txMgmtMsg(dev, txSkb_p) != OS_SUCCESS )
	{
		dev_kfree_skb_any(txSkb_p);
		return FALSE;
	}
	//printk("ACTION_EXTCHANSWTANNO %d\n", ACTION_EXTCHANSWTANNO);
	return TRUE;
}
#endif //#ifdef INTOLERANT40

#ifdef IEEE80211_DH
/* This function updates the shadow MIB wth the given channel*/
BOOLEAN UpdateCurrentChannelInMIB(vmacApInfo_t *vmacSta_p,  UINT32 channel)
{
	MIB_802DOT11 *mib = vmacSta_p->ShadowMib802dot11;
#if 1 //def COMMON_PHYDSSS
	MIB_PHY_DSSS_TABLE *PhyDSSSTable=mib->PhyDSSSTable;
	UINT8 *mib_extSubCh_p = mib->mib_extSubCh;
#else
	MIB_PHY_DSSS_TABLE *PhyDSSSTable=&mib->PhyDSSSTable;
	UINT8 *mib_extSubCh_p = &mib->mib_extSubCh;
#endif

	if(domainChannelValid(channel, channel <= 14?FREQ_BAND_2DOT4GHZ:FREQ_BAND_5GHZ))
	{
		PhyDSSSTable->CurrChan = channel;

		/* Currentlly, 40M is not supported for channel 14 */
		if (PhyDSSSTable->CurrChan == 14)
		{
			if ((PhyDSSSTable->Chanflag.ChnlWidth == CH_AUTO_WIDTH) ||
				(PhyDSSSTable->Chanflag.ChnlWidth == CH_40_MHz_WIDTH))
				PhyDSSSTable->Chanflag.ChnlWidth = CH_20_MHz_WIDTH;
		}

		//PhyDSSSTable->Chanflag.ChnlWidth=CH_40_MHz_WIDTH;
		PhyDSSSTable->Chanflag.ExtChnlOffset=EXT_CH_ABOVE_CTRL_CH;
		if(((PhyDSSSTable->Chanflag.ChnlWidth==CH_40_MHz_WIDTH) ||
			(PhyDSSSTable->Chanflag.ChnlWidth==CH_AUTO_WIDTH)))
		{
			switch(PhyDSSSTable->CurrChan)
			{
			case 1:
			case 2:
			case 3:
			case 4:
				PhyDSSSTable->Chanflag.ExtChnlOffset=EXT_CH_ABOVE_CTRL_CH;
				break;
			case 5:
				/* Now AutoBW use 5-1 instead of 5-9 for wifi cert convenience*/
				/*if(*mib_extSubCh_p==0)
					PhyDSSSTable->Chanflag.ExtChnlOffset=EXT_CH_ABOVE_CTRL_CH;
				else if(*mib_extSubCh_p==1)
					PhyDSSSTable->Chanflag.ExtChnlOffset=EXT_CH_BELOW_CTRL_CH;
				else if(*mib_extSubCh_p==2)
					PhyDSSSTable->Chanflag.ExtChnlOffset=EXT_CH_ABOVE_CTRL_CH;
				break;*/
			case 6:
			case 7:
			case 8:
			case 9:
			case 10:
				if(*mib_extSubCh_p==0)
					PhyDSSSTable->Chanflag.ExtChnlOffset=EXT_CH_BELOW_CTRL_CH;
				else if(*mib_extSubCh_p==1)
					PhyDSSSTable->Chanflag.ExtChnlOffset=EXT_CH_BELOW_CTRL_CH;
				else if(*mib_extSubCh_p==2)
					PhyDSSSTable->Chanflag.ExtChnlOffset=EXT_CH_ABOVE_CTRL_CH;
				break;
			case 11:
			case 12:
			case 13:
			case 14:
				PhyDSSSTable->Chanflag.ExtChnlOffset=EXT_CH_BELOW_CTRL_CH;
				break;
				/* for 5G */
			case 36:
				PhyDSSSTable->Chanflag.ExtChnlOffset=EXT_CH_ABOVE_CTRL_CH;
				break;
			case 40:
				PhyDSSSTable->Chanflag.ExtChnlOffset=EXT_CH_BELOW_CTRL_CH;
				break;
			case 44:
				PhyDSSSTable->Chanflag.ExtChnlOffset=EXT_CH_ABOVE_CTRL_CH;
				break;
			case 48:
				PhyDSSSTable->Chanflag.ExtChnlOffset=EXT_CH_BELOW_CTRL_CH;
				break;
			case 52:
				PhyDSSSTable->Chanflag.ExtChnlOffset=EXT_CH_ABOVE_CTRL_CH;
				break;
			case 56:
				PhyDSSSTable->Chanflag.ExtChnlOffset=EXT_CH_BELOW_CTRL_CH;
				break;
			case 60:
				PhyDSSSTable->Chanflag.ExtChnlOffset=EXT_CH_ABOVE_CTRL_CH;
				break;
			case 64:
				PhyDSSSTable->Chanflag.ExtChnlOffset=EXT_CH_BELOW_CTRL_CH;
				break;
			case 100:
				PhyDSSSTable->Chanflag.ExtChnlOffset=EXT_CH_ABOVE_CTRL_CH;
				break;
			case 104:
				PhyDSSSTable->Chanflag.ExtChnlOffset=EXT_CH_BELOW_CTRL_CH;
				break;
			case 108:
				PhyDSSSTable->Chanflag.ExtChnlOffset=EXT_CH_ABOVE_CTRL_CH;
				break;
			case 112:
				PhyDSSSTable->Chanflag.ExtChnlOffset=EXT_CH_BELOW_CTRL_CH;
				break;
			case 116:
				PhyDSSSTable->Chanflag.ExtChnlOffset=EXT_CH_ABOVE_CTRL_CH;
				break;
			case 120:
				PhyDSSSTable->Chanflag.ExtChnlOffset=EXT_CH_BELOW_CTRL_CH;
				break;
			case 124:
				PhyDSSSTable->Chanflag.ExtChnlOffset=EXT_CH_ABOVE_CTRL_CH;
				break;
			case 128:
				PhyDSSSTable->Chanflag.ExtChnlOffset=EXT_CH_BELOW_CTRL_CH;
				break;
			case 132:
				PhyDSSSTable->Chanflag.ExtChnlOffset=EXT_CH_ABOVE_CTRL_CH;
				break;
			case 136:
				PhyDSSSTable->Chanflag.ExtChnlOffset=EXT_CH_BELOW_CTRL_CH;
				break;
			case 140:
				PhyDSSSTable->Chanflag.ExtChnlOffset=EXT_CH_ABOVE_CTRL_CH;
				break;

			case 149:
				PhyDSSSTable->Chanflag.ExtChnlOffset=EXT_CH_ABOVE_CTRL_CH;
				break;
			case 153:
				PhyDSSSTable->Chanflag.ExtChnlOffset=EXT_CH_BELOW_CTRL_CH;
				break;
			case 157:
				PhyDSSSTable->Chanflag.ExtChnlOffset=EXT_CH_ABOVE_CTRL_CH;
				break;
			case 161:
				PhyDSSSTable->Chanflag.ExtChnlOffset=EXT_CH_BELOW_CTRL_CH;
				break;

			}
		}
		if(PhyDSSSTable->CurrChan<=14)
			PhyDSSSTable->Chanflag.FreqBand=FREQ_BAND_2DOT4GHZ;
		else
			PhyDSSSTable->Chanflag.FreqBand=FREQ_BAND_5GHZ;
	}else
	{
		WLDBG_INFO(DBG_LEVEL_0,  "invalid channel %d\n", channel);
		return FALSE;
	}
	return TRUE ;
}

void ApplyCSAChannel( struct net_device *netdev,UINT32 channel)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	vmacApInfo_t *syscfg = (vmacApInfo_t *)wlpptr->vmacSta_p;
	MIB_802DOT11 *mib = syscfg->ShadowMib802dot11;
#ifdef MRVL_DFS
	smeQ_MgmtMsg_t *toSmeMsg = NULL;
#endif
#if 1 //def COMMON_PHYDSSS
	MIB_PHY_DSSS_TABLE *PhyDSSSTable=mib->PhyDSSSTable;
#else
	MIB_PHY_DSSS_TABLE *PhyDSSSTable=&mib->PhyDSSSTable;
#endif

	{
#ifdef MRVL_DFS
		if ((toSmeMsg=(smeQ_MgmtMsg_t *)kmalloc(sizeof(smeQ_MgmtMsg_t), GFP_ATOMIC)) == NULL)
		{
			WLDBG_INFO(DBG_LEVEL_15, "wlChannelSet: failed to alloc msg buffer\n");
			return ;
		}

		memset(toSmeMsg, 0, sizeof(smeQ_MgmtMsg_t));

		toSmeMsg->vmacSta_p = wlpptr->vmacSta_p ;

		toSmeMsg->MsgType = SME_NOTIFY_CHANNELSWITCH_CFRM;

		toSmeMsg->Msg.ChanSwitchCfrm.result = 1 ;
		toSmeMsg->Msg.ChanSwitchCfrm.chInfo.channel = PhyDSSSTable->CurrChan ;
		memcpy(&toSmeMsg->Msg.ChanSwitchCfrm.chInfo.chanflag ,
			&PhyDSSSTable->Chanflag, sizeof(CHNL_FLAGS));

		smeQ_MgmtWriteNoBlock(toSmeMsg);
		kfree((UINT8 *)toSmeMsg);

#endif //MRVL_DFS
	}

}
#endif //IEEE80211_DH


#ifdef MRVL_DFS
void macMgmtMlme_StartRadarDetection(struct net_device *dev, UINT8 detectionMode)
{

	wlFwSetRadarDetection(dev, detectionMode == DFS_NORMAL_MODE ? 3 : 1 );
}

void macMgmtMlme_StopRadarDetection(struct net_device *dev, UINT8 detectionMode)
{

	wlFwSetRadarDetection(dev, detectionMode == DFS_NORMAL_MODE ? 0 : 2 );
}

void macMgmtMlme_SendChannelSwitchCmd(struct net_device *dev, 
									  IEEEtypes_ChannelSwitchCmd_t *pChannelSwitchCmd)
{
	int i ;
	struct wlprivate *wlpptr = NULL ;

	if( !dev || !pChannelSwitchCmd )
		return ;

	wlpptr = NETDEV_PRIV_P(struct wlprivate, dev);

	/* Send Channel Switch Command to all the AP virtual interfaces */
	for( i = 0 ; i <= MAX_VMAC_INSTANCE_AP; i ++ )
	{
		if( wlpptr->vdev[i] && wlpptr->vdev[i]->flags & IFF_RUNNING )
		{
			struct net_device *vdev = wlpptr->vdev[i] ;
			struct wlprivate *vpriv = NETDEV_PRIV_P(struct wlprivate, vdev);
			SendChannelSwitchCmd(vpriv->vmacSta_p, pChannelSwitchCmd);
		}
	}

}

void macMgmtMlme_GetActiveVAPs( struct net_device *dev, UINT8 *vaplist, UINT8 *vapcount_p)
{
	UINT8 j = 0 ;
	UINT8 count = 0 ;
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, dev);

	while(j <=MAX_VMAC_INSTANCE_AP )
	{
		if( wlpptr->vdev[j] && (wlpptr->vdev[j]->flags & IFF_RUNNING))
		{
			vaplist[count++] = j ;
		}
		j++ ;
	}
	*vapcount_p = count ;
}

void macMgmtMlme_Reset( struct net_device *dev, UINT8 *vaplist, UINT8 *vapcount_p)
{
	if( !dev || ! vaplist || !vapcount_p )
		return ;
	macMgmtMlme_GetActiveVAPs(dev, vaplist, vapcount_p );
	wlResetTask(dev);
}

void macMgmtMlme_MBSS_Reset(struct net_device *netdev, UINT8 *vaplist, UINT8 vapcount)
{
	int i = 0 ;
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);

	for( i = 0 ; i < vapcount; i ++ )
	{
		wlreset_mbss(wlpptr->vdev[vaplist[i]] );
	}
}

void macMgmtMlme_SwitchChannel(struct net_device *dev,  UINT8 channel, CHNL_FLAGS *chanFlag_p)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, dev);
	vmacApInfo_t *syscfg = (vmacApInfo_t *)wlpptr->vmacSta_p;
	MIB_802DOT11 *mib = syscfg->Mib802dot11;
#if 1 //def COMMON_PHYDSSS
	MIB_PHY_DSSS_TABLE *PhyDSSSTable=mib->PhyDSSSTable;
#else
	MIB_PHY_DSSS_TABLE *PhyDSSSTable=&mib->PhyDSSSTable;
#endif
	if( UpdateCurrentChannelInMIB(syscfg, channel) )
	{
		mib_Update();
		if (wlchannelSet(dev, channel, PhyDSSSTable->Chanflag, 0))
		{
			WLDBG_EXIT_INFO(DBG_LEVEL_15, "setting channel failed");
			return ;
		}

	}
	if( chanFlag_p )
	{
		memcpy( chanFlag_p, &PhyDSSSTable->Chanflag, sizeof(CHNL_FLAGS));
	}
}
BOOLEAN macMgmtMlme_DfsEnabled(struct net_device *dev )
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, dev);
	vmacApInfo_t *syscfg = (vmacApInfo_t *)wlpptr->vmacSta_p;
	MIB_802DOT11 *mib = syscfg->Mib802dot11;

	if( mib->SpectrumMagament->spectrumManagement )
		return TRUE ;
	return FALSE ;
}

void macMgmtMlme_StopDataTraffic( struct net_device *dev)
{
	struct wlprivate *wlpptr = NULL ;
	DfsAp * pdfsApMain = NULL ;

	if( !dev )
		return ;
	wlpptr = NETDEV_PRIV_P(struct wlprivate, dev);
	pdfsApMain = wlpptr->wlpd_p->pdfsApMain;
	if (dev->flags & IFF_RUNNING)
	{
		netif_stop_queue(dev);
		dev->flags &= ~IFF_RUNNING;
	}

	// Data traffic will be dropped in the data path
	if( pdfsApMain )
	{
		pdfsApMain->dropData = 1 ;
		//Disbale AMPDU streams if any
		disableAmpduTxAll( wlpptr->vmacSta_p );
	}

	return ;

}

void macMgmtMlme_RestartDataTraffic(struct net_device *dev)
{

	struct wlprivate *wlpptr = NULL ;
	DfsAp * pdfsApMain = NULL ;
	if( !dev )
		return ;

	wlpptr = NETDEV_PRIV_P(struct wlprivate, dev);
	pdfsApMain = wlpptr->wlpd_p->pdfsApMain;

	if ((dev->flags & IFF_RUNNING) == 0 )
	{
		netif_wake_queue(dev);
		dev->flags |= IFF_RUNNING;
	}

	if( pdfsApMain )
	{
		//Allow transmit traffic in the data path
		pdfsApMain->dropData = 0 ;
	}
}

UINT8 macMgmtMlme_Get40MHzExtChannelOffset( UINT8 channel )
{
	UINT8 extChnlOffset = EXT_CH_ABOVE_CTRL_CH;
	switch(channel)
	{
	case 1:
	case 2:
	case 3:
	case 4:
		extChnlOffset=EXT_CH_ABOVE_CTRL_CH;
		break;
	case 5: 
	case 6: 
	case 7: 
	case 8:
	case 9:
	case 10:
	case 11:
	case 12:
	case 13:
	case 14:
		extChnlOffset=EXT_CH_BELOW_CTRL_CH;
		break;
	case 36:
		extChnlOffset=EXT_CH_ABOVE_CTRL_CH;
		break;
	case 40:
		extChnlOffset=EXT_CH_BELOW_CTRL_CH;
		break;
	case 44:
		extChnlOffset=EXT_CH_ABOVE_CTRL_CH;
		break;
	case 48:
		extChnlOffset=EXT_CH_BELOW_CTRL_CH;
		break;
	case 52:
		extChnlOffset=EXT_CH_ABOVE_CTRL_CH;
		break;
	case 56:
		extChnlOffset=EXT_CH_BELOW_CTRL_CH;
		break;
	case 60:
		extChnlOffset=EXT_CH_ABOVE_CTRL_CH;
		break;
	case 64:
		extChnlOffset=EXT_CH_BELOW_CTRL_CH;
		break;
	case 100:
		extChnlOffset=EXT_CH_ABOVE_CTRL_CH;
		break;
	case 104:
		extChnlOffset=EXT_CH_BELOW_CTRL_CH;
		break;
	case 108:
		extChnlOffset=EXT_CH_ABOVE_CTRL_CH;
		break;
	case 112:
		extChnlOffset=EXT_CH_BELOW_CTRL_CH;
	case 116:
		extChnlOffset=EXT_CH_ABOVE_CTRL_CH;
		break;
	case 120:
		extChnlOffset=EXT_CH_BELOW_CTRL_CH;
		break;
	case 124:
		extChnlOffset=EXT_CH_ABOVE_CTRL_CH;
		break;
	case 128:
		extChnlOffset=EXT_CH_BELOW_CTRL_CH;
		break;
	case 132:
		extChnlOffset=EXT_CH_ABOVE_CTRL_CH;
		break;
	case 136:
		extChnlOffset=EXT_CH_BELOW_CTRL_CH;
		break;
	case 140:
		extChnlOffset=EXT_CH_ABOVE_CTRL_CH;
		break;

	case 149:
		extChnlOffset=EXT_CH_ABOVE_CTRL_CH;
		break;
	case 153:
		extChnlOffset=EXT_CH_BELOW_CTRL_CH;
		break;
	case 157:
		extChnlOffset=EXT_CH_ABOVE_CTRL_CH;
		break;
	case 161:
		extChnlOffset=EXT_CH_BELOW_CTRL_CH;
		break;
	default :
		extChnlOffset = EXT_CH_ABOVE_CTRL_CH ;
	}
	return extChnlOffset ;
}

#endif //MRVL_DFS

#ifdef INTOLERANT40
static void HT40MIntolerantHandler(vmacApInfo_t *vmacSta_p,UINT8 soon)
{
	extern void extStaDb_SendBeaconReqMeasureReqAction(vmacApInfo_t *vmac_p);
	MIB_802DOT11 *mib = vmacSta_p->Mib802dot11;
	MIB_802DOT11 *mibShadow = vmacSta_p->ShadowMib802dot11;
	UINT8 Addr[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

	//printk("ChnlWidth %d, USER_ChnlWidth %d\n", mib->PhyDSSSTable.Chanflag.ChnlWidth,mib->USER_ChnlWidth);

	if (!*(mib->mib_HT40MIntoler))
		return;

	if (!soon)
		extStaDb_SendBeaconReqMeasureReqAction(vmacSta_p);

	if (((mib->PhyDSSSTable->Chanflag.ChnlWidth == CH_40_MHz_WIDTH) || (mib->PhyDSSSTable->Chanflag.ChnlWidth == CH_AUTO_WIDTH)) && 
		((*(mib->USER_ChnlWidth) == CH_40_MHz_WIDTH) || (*(mib->USER_ChnlWidth) == CH_AUTO_WIDTH)))
	{
		/* Switch from 40 to 20M */
		if (soon || sHt30minStaIntolerant)
		{			
			macMgmtMlme_SendExtChanSwitchAnnounceAction(vmacSta_p->dev, (IEEEtypes_MacAddr_t *)&Addr[0]);
			*(mibShadow->mib_FortyMIntolerant) = 1;
			mibShadow->PhyDSSSTable->Chanflag.ChnlWidth = CH_20_MHz_WIDTH;
			/* Start 30 min timer */
			sMonitorcnt30min = 1;

			wlResetTask(vmacSta_p->dev);
			//printk("to 20\n");
		}
	}
	else if	((mib->PhyDSSSTable->Chanflag.ChnlWidth == CH_20_MHz_WIDTH) && 
		((*(mib->USER_ChnlWidth) == CH_40_MHz_WIDTH) || (*(mib->USER_ChnlWidth) == CH_AUTO_WIDTH)))
	{
		/* Switch from 20 to 40M */
		if (!sHt30minStaIntolerant && !vmacSta_p->legClients && !vmacSta_p->n20MClients)
		{
			*(mibShadow->mib_FortyMIntolerant) = 0;
			mibShadow->PhyDSSSTable->Chanflag.ChnlWidth = CH_40_MHz_WIDTH;
			/* Start 30 min timeer */
			sMonitorcnt30min = 1;
			wlResetTask(vmacSta_p->dev);
			//printk("to 40\n");
		}
	}
	else if	(*(mib->USER_ChnlWidth) == CH_20_MHz_WIDTH)
	{
		/* Close 30 min timeer */
		sMonitorcnt30min = 0;
	}
	//sHt30minStaIntolerant = 0;
}

void RecHTIntolerant(vmacApInfo_t *vmacSta_p,UINT8 enable)
{
	MIB_802DOT11 *mib = vmacSta_p->Mib802dot11;

	//printk("Rec:HT %d\n", enable);

	if (enable)
	{
		sHt30minStaIntolerant++;

		if (((mib->PhyDSSSTable->Chanflag.ChnlWidth == CH_40_MHz_WIDTH) || (mib->PhyDSSSTable->Chanflag.ChnlWidth == CH_AUTO_WIDTH)) && 
			((*(mib->USER_ChnlWidth) == CH_40_MHz_WIDTH) || (*(mib->USER_ChnlWidth) == CH_AUTO_WIDTH)))
		{
			sMonitorcnt30min = (INTOLERANTCHEKCOUNTER - 3);
		}
	}
	else
	{
		sMonitorcnt30min = 1;
		sHt30minStaIntolerant = 0;
	}
}
#endif //#ifdef INTOLERANT40

void FixedRateCtl(extStaDb_StaInfo_t *pStaInfo, PeerInfo_t	*PeerInfo, MIB_802DOT11 *mib)
{
	if ((pStaInfo == NULL) || (*(mib->mib_enableFixedRateTx)== 0))
		return;
#if 0 /* wlan-v5-sc2 merges: TODO LATER - FIRMWARE DEPENDENT */
	switch(*(mib->mib_ApMode))
	{
	case AP_MODE_B_ONLY:
		PeerInfo->StaMode = AP_MODE_B_ONLY;
		PeerInfo->IeeeRate = (UINT8)*(mib->mib_txDataRate);
		break;

	case AP_MODE_G_ONLY:
		PeerInfo->StaMode = AP_MODE_G_ONLY;
		PeerInfo->IeeeRate = (UINT8)*(mib->mib_txDataRateG);
		break;

	case AP_MODE_A_ONLY:
		PeerInfo->StaMode = AP_MODE_A_ONLY;
		PeerInfo->IeeeRate = (UINT8)*(mib->mib_txDataRateA);
		break;

	case AP_MODE_MIXED:
		if (pStaInfo->ClientMode ==	BONLY_MODE)	
		{
			PeerInfo->StaMode = AP_MODE_B_ONLY;
			PeerInfo->IeeeRate = (UINT8)*(mib->mib_txDataRate);
		}
		else
		{
			PeerInfo->StaMode = AP_MODE_G_ONLY;
			PeerInfo->IeeeRate = (UINT8)*(mib->mib_txDataRateG);
		}
		break;

	case AP_MODE_N_ONLY:
		if (*(mib->mib_FixedRateTxType) == 1)
			PeerInfo->StaMode = AP_MODE_N_ONLY;
		else
			PeerInfo->StaMode = AP_MODE_G_ONLY;

		PeerInfo->IeeeRate = (UINT8)*(mib->mib_txDataRateN);
		break;

	case AP_MODE_BandN:
		if (pStaInfo->ClientMode ==	NONLY_MODE)
		{
			if (*(mib->mib_FixedRateTxType) == 1)
				PeerInfo->StaMode = AP_MODE_N_ONLY;
			else
				PeerInfo->StaMode = AP_MODE_B_ONLY;

			PeerInfo->IeeeRate = (UINT8)*(mib->mib_txDataRateN);
		}
		else 
		{
			PeerInfo->StaMode = AP_MODE_B_ONLY;
			PeerInfo->IeeeRate = (UINT8)*(mib->mib_txDataRate);
		}
		break;

	case AP_MODE_GandN:
		if (pStaInfo->ClientMode ==	NONLY_MODE)
		{
			if (*(mib->mib_FixedRateTxType) == 1)
				PeerInfo->StaMode = AP_MODE_N_ONLY;
			else
				PeerInfo->StaMode = AP_MODE_G_ONLY;

			PeerInfo->IeeeRate = (UINT8)*(mib->mib_txDataRateN);
		}
		else 
		{
			PeerInfo->StaMode = AP_MODE_G_ONLY;
			PeerInfo->IeeeRate = (UINT8)*(mib->mib_txDataRateG);
		}
		break;

	case AP_MODE_BandGandN:
		if (pStaInfo->ClientMode ==	NONLY_MODE)
		{
			if (*(mib->mib_FixedRateTxType) == 1)
				PeerInfo->StaMode = AP_MODE_N_ONLY;
			else
				PeerInfo->StaMode = AP_MODE_B_ONLY;

			PeerInfo->IeeeRate = (UINT8)*(mib->mib_txDataRateN);
		}
		else if (pStaInfo->ClientMode ==	BONLY_MODE)
		{
			PeerInfo->StaMode = AP_MODE_B_ONLY;
			PeerInfo->IeeeRate = (UINT8)*(mib->mib_txDataRate);
		}
		else
		{
			PeerInfo->StaMode = AP_MODE_G_ONLY;
			PeerInfo->IeeeRate = (UINT8)*(mib->mib_txDataRateG);
		}
		break;

	case AP_MODE_5GHZ_N_ONLY:
		if (*(mib->mib_FixedRateTxType) == 1)
			PeerInfo->StaMode = AP_MODE_5GHZ_N_ONLY;
		else
			PeerInfo->StaMode = AP_MODE_A_ONLY;

		PeerInfo->IeeeRate = (UINT8)*(mib->mib_txDataRateN);
		break;

	case AP_MODE_AandN:
		if (pStaInfo->ClientMode == NONLY_MODE)
		{
			if (*(mib->mib_FixedRateTxType) == 1)
				PeerInfo->StaMode = AP_MODE_N_ONLY;
			else
				PeerInfo->StaMode = AP_MODE_A_ONLY;

			PeerInfo->IeeeRate = (UINT8)*(mib->mib_txDataRateN);
		}
		else 
		{
			PeerInfo->StaMode = AP_MODE_A_ONLY;
			PeerInfo->IeeeRate = (UINT8)*(mib->mib_txDataRateA);
		}
		break;

	default:
		if (pStaInfo->ClientMode ==	BONLY_MODE)	
		{
			PeerInfo->StaMode = AP_MODE_B_ONLY;
			PeerInfo->IeeeRate = (UINT8)*(mib->mib_txDataRate);
		}
		else
		{
			PeerInfo->StaMode = AP_MODE_G_ONLY;
			PeerInfo->IeeeRate = (UINT8)*(mib->mib_txDataRateG);
		}
		break;
	}
#endif
}


#ifdef SOC_W8864
UINT8 macMgmtMlme_Get80MHzPrimaryChannelOffset( UINT8 channel )
{
	UINT8 act_primary = ACT_PRIMARY_CHAN_0;
	switch(channel)
	{
	case 36:
		act_primary=ACT_PRIMARY_CHAN_0;
		break;
	case 40:
		act_primary=ACT_PRIMARY_CHAN_1;
		break;
	case 44:
		act_primary=ACT_PRIMARY_CHAN_2;
		break;
	case 48:
		act_primary=ACT_PRIMARY_CHAN_3;
		break;

	case 52:
		act_primary=ACT_PRIMARY_CHAN_0;
		break;
	case 56:
		act_primary=ACT_PRIMARY_CHAN_1;
		break;
	case 60:
		act_primary=ACT_PRIMARY_CHAN_2;
		break;
	case 64:
		act_primary=ACT_PRIMARY_CHAN_3;
		break;

	case 100:
		act_primary=ACT_PRIMARY_CHAN_0;
		break;
	case 104:
		act_primary=ACT_PRIMARY_CHAN_1;
		break;
	case 108:
		act_primary=ACT_PRIMARY_CHAN_2;
		break;
	case 112:
		act_primary=ACT_PRIMARY_CHAN_3;
		break;

	case 116:
		act_primary=ACT_PRIMARY_CHAN_0;
		break;
	case 120:
		act_primary=ACT_PRIMARY_CHAN_1;
		break;
	case 124:
		act_primary=ACT_PRIMARY_CHAN_2;
		break;
	case 128:
		act_primary=ACT_PRIMARY_CHAN_3;
		break;

	case 132:
		act_primary=ACT_PRIMARY_CHAN_0;
		break;
	case 136:
		act_primary=ACT_PRIMARY_CHAN_1;
		break;
	case 140:
		act_primary=ACT_PRIMARY_CHAN_2;
		break;
	case 144:
		act_primary=ACT_PRIMARY_CHAN_3;
		break;

	case 149:
		act_primary=ACT_PRIMARY_CHAN_0;
		break;
	case 153:
		act_primary=ACT_PRIMARY_CHAN_1;
		break;
	case 157:
		act_primary=ACT_PRIMARY_CHAN_2;
		break;
	case 161:
		act_primary=ACT_PRIMARY_CHAN_3;
		break;
	}

	return act_primary ;
}
#endif
