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
* Description:  Implementation of the STA MLME Module Services
*
*/

#ifndef MAC_MLME_STA
#define MAC_MLME_STA

#include "wltypes.h"
#include "IEEE_types.h"
#include "station.h"
#include "ds.h"
#include "macMgmtMlme.h"
#include "keyMgmt.h"
#include "keyMgmtSta.h"

/* For porting to LINUX_OS platform */
#define PORT_TO_LINUX_OS    1

#define buildModes_RETRIES  1
#define buildModes_AUTH_RETRIES 1

#define MLME_SW_LINK_LOST       1

/* New Timer Timeout values */
//#define SCAN_TIME  		5   /* in unit of 100ms */

//#define STATUS_TIME     300

//#define ASSOC_TIME      40
//#define AUTH_TIME       40

#define MAX_SHARED_KEY_AUTHENTICATIONS  1    /* 5 -> 2 -> 1 */
#define WEP_ENCRYPT_OVER_HDR_LEN        8
#define EVENT_NUM_MAX   64
//#define ASSOC_TIMEOUT  5 //500ms
//#define AUTH_TIMEOUT  5 //500ms

#define MAX_B_DATA_RATES    4
#define MAX_G_DATA_RATES    8

#define MLME_SUCCESS    0
#define MLME_INPROCESS 1
#define MLME_FAILURE    -1

#define MLME_SUPPORT_RATE_IE_MAX   8

#define IEEEtypes_MAX_DATA_RATES_G     14
#define INTERVAL_LOOK_AHEAD 2

#define IDLE_PROBEREQ_CNT   6

typedef struct Challenge_t
{
   BOOLEAN              Free;
   IEEEtypes_MacAddr_t  Addr;
   UINT8                Text[IEEEtypes_CHALLENGE_TEXT_SIZE];
} Challenge_t;

/*---------------------------------*/
/* Client MLME local Management Messages */
/*---------------------------------*/
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

typedef struct macmgmtQ_CmdReq_t
{
   IEEEtypes_MacAddr_t targetAddr;
   IEEEtypes_SmeCmd_t  CmdType;   
   union
   {
      macMgmtQ_ScanCmd_t       ScanCmd_sta;
      IEEEtypes_ScanCmd_t      ScanCmd_ap;
      IEEEtypes_JoinCmd_t      JoinCmd;
      IEEEtypes_AuthCmd_t      AuthCmd;
      IEEEtypes_DeauthCmd_t    DeauthCmd;
      IEEEtypes_AssocCmd_t     AssocCmd;
      IEEEtypes_ReassocCmd_t   ReassocCmd;
      IEEEtypes_DisassocCmd_t  DisassocCmd;
      IEEEtypes_ResetCmd_t     ResetCmd;
      IEEEtypes_StartCmd_t     StartCmd;
   } Body;
   UINT32 reserved[16];
} PACK_END macmgmtQ_CmdReq_t;

typedef struct macmgmtQ_CmdRsp_t
{
    IEEEtypes_MacAddr_t targetAddr;
	IEEEtypes_SmeNotify_t MsgType;
	union
	{
		IEEEtypes_PwrMgmtCfrm_t PwrMgmtCfrm;
        smeQ_ScanRsp_t            ScanCfrm;
		IEEEtypes_JoinCfrm_t JoinCfrm;
		IEEEtypes_AuthCfrm_t AuthCfrm;
		IEEEtypes_AuthInd_t AuthInd;
		IEEEtypes_DeauthCfrm_t DeauthCfrm;
		IEEEtypes_DeauthInd_t DeauthInd;
		IEEEtypes_AssocCfrm_t AssocCfrm;
		IEEEtypes_AssocInd_t AssocInd;
		IEEEtypes_ReassocCfrm_t ReassocCfrm;
		IEEEtypes_ReassocInd_t ReassocInd;
		IEEEtypes_DisassocCfrm_t DisassocCfrm;
		IEEEtypes_DisassocInd_t DisassocInd;
		IEEEtypes_ResetCfrm_t ResetCfrm;
		IEEEtypes_StartCfrm_t StartCfrm;
	} Msg;
} PACK_END macmgmtQ_CmdRsp_t;

typedef struct macmgmtQ_CmdBuf_t
{ 
   union
   {
       macmgmtQ_CmdReq_t cmdReq;
       macmgmtQ_CmdRsp_t cmdRsp;
   } Cmd;
} PACK_END macmgmtQ_CmdBuf_t;

typedef struct macMgmtMlme_StaData_t
{
   IEEEtypes_MacAddr_t           BssId;
   IEEEtypes_SsId_t              BssSsId;
   IEEEtypes_Len_t               BssSsIdLen;
   IEEEtypes_MacAddr_t           BssSourceAddr;
   IEEEtypes_Bss_t               BssType;
   IEEEtypes_CapInfo_t           CapInfo;
   IEEEtypes_SuppRatesElement_t  OpRateSet;
   IEEEtypes_SuppRatesElement_t  UsedRateSet;
   IEEEtypes_CapInfo_t           ap_CapInfo;
   UINT16                        AP_RateLen;
   UINT16                        AP_gRateLen;
#ifdef STA_QOS
   UINT8                         IsStaQosSTA;
#endif
} macMgmtMlme_StaData_t;
   //
   // Structure storing station data
   //

typedef struct macMgmtMlme_SigQltySet_t
{
   UINT8      NumReadings;
   UINT8      SigQual1;
   UINT8      SigQual2;
   UINT8      RSSI;
} macMgmtMlme_SigQltySet_t;
   //
   // Structure storing signal quality information
   //

typedef PACK_START struct _txBcnInfo_t
{
  UINT8             RetryCnt;
  UINT8             Reserved0;
  UINT16            Status;
  IEEEtypes_Frame_t *HdrAddr;
  UINT16            Service;
  UINT8             Rate;
  UINT8             Power;
  UINT8             CfOffset;
  UINT8             TimOffset;
  UINT16            ProbeRespLen;
  UINT32            Tsf0;
  UINT32            Tsf1;
  UINT16            FragBasicDurId0;
  UINT16            FragBasicDurId1;
  UINT16            TxParam;
  UINT16            Reserved1;
}
PACK_END txBcnInfo_t;

/* Added for Site Survey */
#define SITE_SURVEY_ENTRY_MAX   20

typedef struct _API_SURVEY_ENTRY
{
   UINT8 dirty;
   UINT8 BssId[6];
   UINT8 SsId[32];
   UINT8 RSSI;
   UINT8 channel;
   UINT8 IBSS:1;
   UINT8 wepEnabled:1;
   UINT8 B_Support:1;
   UINT8 G_Support:1;
   UINT8 wpaEnabled:2;
   UINT8 reserve:2;

}API_SURVEY_ENTRY;
/* Added for Site Survey end */

typedef struct siteSurveyResult_t
{
	API_SURVEY_ENTRY  siteSurveyInfo[SITE_SURVEY_ENTRY_MAX];
}siteSurveyResult_t;

typedef struct scanTableResult_t
{
	IEEEtypes_MacAddr_t 		BssSourceAddr[IEEEtypes_MAX_BSS_DESCRIPTS];
	macMgmtMlme_SigQltySet_t 	macMgmtMlme_SigQltyResults[IEEEtypes_MAX_BSS_DESCRIPTS];
	UINT8  		                ScanResults[MAX_SCAN_BUF_SIZE + BUF_PAD_NUM];
	UINT32  					ChannelMap[IEEEtypes_MAX_BSS_DESCRIPTS];
	void  						*ScanResultsMap_p[IEEEtypes_MAX_BSS_DESCRIPTS];

#ifdef WPA2
	WPA_AP_Ciphers_t 			WPA_AP_Ciphers[IEEEtypes_MAX_BSS_DESCRIPTS];
#endif	
}scanTableResult_t;

/* Structure for saving link stats and quality. */
typedef struct iw_linkInfo_t
{
    struct iw_statistics	 	wStats;
    struct iw_quality           avg_qual;
    struct iw_quality           max_qual;
}iw_linkInfo_t;

/* Definition of STA MLME information structure */
typedef struct vmacStaInfo_t
{
    /* Pointer back to parent */
    UINT8                       *vMacEntry_p;

	/* Upper layer Apps have to alloc memory and set these pointers */
	txBcnInfo_t 				*BcnTxInfo_p;
	txBcnInfo_t 				*PrbRspTxInfo_p;
	macmgmtQ_MgmtMsg_t 			*BcnBuffer_p;
	macmgmtQ_MgmtMsg_t 			*PrbRspBuf_p;
	scanTableResult_t			*scanTableResult_p;
    IEEEtypes_BssDescSet_t 		*BssDescSet_p;
	siteSurveyResult_t  		*sSurveyTable_p;

	/* MLME Call Back Function */
	void (*mlmeCallBack_fp)(UINT32 data1, UINT8 *info, UINT32 data2);

	/* State Machines */
	SyncSrvSta 					mgtStaSync;
	AssocSrvSta 				assocsrv;
	AuthReqSrvSta 				mgtStaAuthReq;

	/* Timer */
	Timer 						assocTimer;
	Timer 						authTimer;
	Timer 						scaningTimer;
	Timer 						statusTimer;
	Timer 						keepaliveTimer;
#ifdef IEEE80211_DH
	Timer 						station11hTimer;
#endif //IEEE80211_DH
#ifdef WMON
	Timer 						stationWMONTimer;
#endif //WMON

	/* Local to Mlme start here*/
	UINT8 						AssociatedFlag;
	UINT8 						IBssStartFlag;
	UINT8 						macMgt_StaMode;
	UINT8						Adhoc_Active;
	BOOLEAN						smeMain_AutoAssoc;
#ifdef IEEE80211_DH
	UINT8						station11hTimerFired ;
	UINT8						station11hChannel ;
#endif //IEEE80211_DH
#ifdef WMON
	UINT8						stationWMONTimerFired ;
#endif //WMON

	/* for Scanning */
	DECLARE_LOCK(ScanResultsLock);	
	unsigned long 				ScanResultsFlags;			
	void  						*ScanResults_p;
	BOOLEAN  					ScanForAnyBeacons;
	UINT32  					NumScanChannels;
	UINT32  					ChanIdx;
	UINT32  					NumDescripts;
	UINT32 						JoinChannel;
	UINT32  					ScanResultsLen;
	UINT32 						bcnCount;
	UINT8 						PreScanRfChannel;
	UINT8 						counterInt;
	UINT8 						misMatchBssidCount;
	UINT8 						linkQuality;
	UINT8 						rxBcnCnt;
	UINT8 						rxBcnPeriod;    /* in unit of ms */
    UINT16                      scanTime_tick;
    UINT16                      scanFilterMap;

	/* Local Data Structures */
    IEEEtypes_AId_t             aId;
    IEEEtypes_BssDesc_t         *bssDescProfile_p;
	macMgmtMlme_StaData_t  		macMgmtMlme_ThisStaData;
	macmgmtQ_CmdReq_t 			smeMain_LastScanMsg;
	IEEEtypes_MacMgmtStates_t  	macMgmtMain_State;
	IEEEtypes_MacMgmtStates_t 	smeMain_State;
	IEEEtypes_MacMgmtStates_t 	macMgmtMain_PostScanState;
	IEEEtypes_MacMgmtStates_t 	PostScanState;
	IEEEtypes_PwrMgmtMode_t 	macMgmtMain_PwrMode;
	IEEEtypes_MacAddr_t   		JoinAddr;
	IEEEtypes_ScanCmd_t  		ScanParams;
	IEEEtypes_DataRate_t 		gOpRateSet[MAX_G_DATA_RATES]; 
	IEEEtypes_DataRate_t 		bOpRateSet[MAX_B_DATA_RATES];
	BOOLEAN 					ContinueScanning;
	UINT32  					JoinRetryCount;
	UINT32  					AssocRetryCount;
	macmgmtQ_CmdReq_t  			LastJoinMsg ;
	UINT32  					AuthRetryCount;
	macmgmtQ_CmdReq_t  			LastAuthMsg ;
    UINT32                      cmdHistory;

	/* For Parent/Child Session */
	BOOLEAN						isParentSession;
	UINT32						childControlParam;

	
	/* Exported from other Modules start here */
	/* Mib Related */
	UINT8 						*mib_StaMode_p;
	Station_t 					*Station_p;
    STA_SYSTEM_MIBS             staSystemMibs;
    STA_SECURITY_MIBS           staSecurityMibs;
	MIB_WB                      *mib_WB_p;
    UINT8                       *mib_defaultkeyindex_p;
    UINT8                       *WepType_p;
#ifdef WPA_STA
	/* WPA Related */
	IEEEtypes_RSN_IE_t			*thisStaRsnIE_p;
    keyMgmtInfoSta_t            *keyMgmtInfoSta_p;
	IEEEtypes_RSN_IE_WPA2_t 	*thisStaRsnIEWPA2_p;
#endif /* WPA_STA */

#ifdef STA_QOS
    QoS_Cap_Elem_t              *thisStaQoSCapElem_p;
#endif /* STA_QOS */

    UINT32                      urProbeRspMissedCnt;
    UINT8                       g_rcvdProbeRsp;
    void                        *peerInfo_p;    
    BOOLEAN                     isApMrvl;
    iw_linkInfo_t               linkInfo;
    UINT32                      staProbeReqCntIdle;
}vmacStaInfo_t;

typedef struct scanDescptHdr_t
{
    UINT16 length;
    UINT8 bssId[IEEEtypes_ADDRESS_SIZE];
    UINT8 rssi;
    IEEEtypes_TimeStamp_t TimeStamp;
    IEEEtypes_BcnInterval_t BcnInterval;
    IEEEtypes_CapInfo_t CapInfo;

}
PACK_END scanDescptHdr_t;

/* Functions for Sync State Machine Services */
extern void SyncSrvCtorSta(SyncSrvSta *me);
extern UINT32 syncSrv_AddAttrib( dot11MgtFrame_t *mgtMsg_p, 
                                   UINT8 attribType,
                                   UINT8 *attribData,
                                   UINT8 attribLen);
extern void syncSrv_StartCmd( vmacStaInfo_t *vStaInfo_p,
							  IEEEtypes_StartCmd_t *StartCmd_p );
extern void syncSrv_JoinCmd( vmacStaInfo_t *vStaInfo_p,
							 IEEEtypes_JoinCmd_t *JoinCmd_p );
extern void syncSrv_SetInitValues( vmacStaInfo_t *vStaInfo_p );
extern void syncSrvSta_ScanCmd( vmacStaInfo_t *vStaInfo_p, 
								macMgmtQ_ScanCmd_t *ScanCmd_p );
extern void syncSrv_BncRecvAssociatedHandler(vmacStaInfo_t *vStaInfo_p,
											 dot11MgtFrame_t *MgmtMsg_p,
                                             UINT8 *rfHdr_p);
extern void syncSrv_ProbeRspRcvd( vmacStaInfo_t *vStaInfo_p,
								  dot11MgtFrame_t *MgmtMsg_p, 
								  UINT8 *rfHdr_p );
extern void syncSrvSta_SetNextChannel ( vmacStaInfo_t *vStaInfo_p );
extern SINT32 syncSrv_LinkLostHandler( vmacStaInfo_t *vStaInfo_p );
extern void syncSrv_BncRecvJoiningHandler(vmacStaInfo_t *vStaInfo_p,
										  dot11MgtFrame_t *MgmtMsg_p,
                                          UINT8 *rfHdr_p);
extern SINT32 syncSrvSta_ScanActTimeOut(UINT8 *data);
extern SINT32 syncSrv_JoinActTimeOut(UINT8 *data);
extern SINT32 syncSrv_ReJoinActTimeOut(UINT8 *data);
extern BOOLEAN syncSrv_IsLinkConnected( UINT8 *data_p);
extern BOOLEAN syncSrv_IsIbssMode( UINT8 *data_p);
extern void syncSrv_ScanFilter( vmacStaInfo_t *vStaInfo_p,
                                dot11MgtFrame_t *MgmtMsg_p, 
								UINT8 *rfHdr_p );
extern SINT32 syncSrv_SetStatusTimer(vmacStaInfo_t *vStaInfo_p,
                                     UINT8 status);
extern SINT32 syncSrv_SetKeepAliveTimer(vmacStaInfo_t *vStaInfo_p,
                                     UINT8 status);
extern void syncSrv_PrbeRspNoBssHandler( vmacStaInfo_t *vStaInfo_p,
                                  dot11MgtFrame_t *MgmtMsg_p, 
                                  UINT8 *rfHdr_p );
extern void *syncSrv_ParseAttribWithinFrame(dot11MgtFrame_t *mgtFrame_p, 
                                            UINT8 *data_p, 
                                            UINT8 attrib);
extern SINT8 syncStaMsgInit( vmacStaInfo_t *vStaInfo_p,
                             SyncSrvStaMsg *syncMsg_p, 
                             UINT8 *message);
extern void syncSrv_ProbeReqRcvd( vmacStaInfo_t *vStaInfo_p, 
                                  dot11MgtFrame_t *MgmtMsg_p, 
                                  UINT8 *rfHdr_p);
extern SINT32 syncSrv_CmdExceptionHandler(vmacStaInfo_t *vStaInfo_p);
extern void syncSrv_SndLinkLostInd(vmacStaInfo_t *vStaInfo_p);
extern void syncSrv_UpdateJoinStatus(vmacStaInfo_t *vStaInfo_p,
								UINT16 status);
extern void syncSrv_ResetCmd( vmacStaInfo_t *vStaInfo_p, 
							  IEEEtypes_ResetCmd_t *ResetCmd_p );

/* Functions for Auth State Machine Services */
extern void AuthReqSrvStaCtor(AuthReqSrvSta *me);
extern UINT32 authSrv_AuthCmd( vmacStaInfo_t *vStaInfo_p,
							   IEEEtypes_AuthCmd_t *AuthCmd_p );
extern SINT32 authSrv_RecvMsgRsp(vmacStaInfo_t *vStaInfo_p,
								 dot11MgtFrame_t *MgmtMsg_p );
extern SINT32 authSrv_RecvMsgDeAuth(vmacStaInfo_t *vStaInfo_p,
									dot11MgtFrame_t *MgmtMsg_p );
extern SINT32 authSrv_DeAuthCmd(vmacStaInfo_t *vStaInfo_p,
								IEEEtypes_DeauthCmd_t *DeauthCmd_p );
extern SINT32 authSrv_AuthActTimeOut(UINT8 *data);
extern SINT32 authSrv_Reset(vmacStaInfo_t *vStaInfo_p);
extern SINT32 authSrv_SndDeAuthMsg( vmacStaInfo_t *vStaInfo_p, 
                                    IEEEtypes_MacAddr_t *destMac_p,
                                    IEEEtypes_MacAddr_t *bssId_p,
                                    UINT16 reasonCode);

/* Function for Assoc State Machine Services */
extern void AssocSrvStaCtor(AssocSrvSta *me);
extern SINT32 assocSrv_AssocCmd( vmacStaInfo_t *vStaInfo_p, 
								 IEEEtypes_AssocCmd_t *AssocCmd_p );
extern SINT32 assocSrv_ReAssocCmd( vmacStaInfo_t *vStaInfo_p,
								   IEEEtypes_ReassocCmd_t *ReassocCmd_p );
extern SINT32 assocSrv_RecvReAssocRsp(vmacStaInfo_t *vStaInfo_p, 
									  dot11MgtFrame_t *MgmtMsg_p );
extern SINT32 assocSrv_RecvAssocRsp( vmacStaInfo_t *vStaInfo_p, 
									 dot11MgtFrame_t *MgmtMsg_p );
extern SINT32 assocSrv_AssocActTimeOut(UINT8 *data);
extern SINT32 assocSrv_DisAssocActTimeOut(vmacStaInfo_t *vStaInfo_p, UINT8 *peerAddr);
extern SINT32 assocSrv_ReAssocActTimeOut(vmacStaInfo_t *vStaInfo_p,IEEEtypes_MacAddr_t *peerAddr);
extern SINT32 assocSrv_Reset(vmacStaInfo_t *vStaInfo_p);
extern SINT32 assocSrv_DisAssocCmd(vmacStaInfo_t *vStaInfo_p,
                                   IEEEtypes_DisassocCmd_t *DisassocCmd_p );
extern SINT32 assocSrv_RecvDisAssocMsg(vmacStaInfo_t *vStaInfo_p, 
									   dot11MgtFrame_t *MgmtMsg_p);

/* Function for wlMlmeSrv */
extern int wl_MacMlme_AssocSrvStaTimeout( void *info_p, void *data_p );
extern int wl_MacMlme_AssocCmd( void *info_p, void *data_p );
extern int wl_MacMlme_ReAssocCmd( void *info_p, void *data_p );
extern int wl_MacMlme_AssocRsp( void *info_p, void *data_p );
extern int wl_MacMlme_ReAssocRsp( void *info_p, void *data_p );
extern int wl_MacMlme_AuthReqCmd( void *info_p, void *data_p );
extern int wl_MacMlme_AuthStaEven( void *info_p, void *data_p );
extern int wl_MacMlme_AuthSrvStaTimeout( void *info_p, void *data_p );
extern int wl_MacMlme_StartReq( void *info_p, void *data_p );
extern int wl_MacMlme_ScanReqSta( void *info_p, void *data_p );

extern void macMgtSyncSrvStaInit(vmacStaInfo_t *vStaInfo_p);

/* SmeStateMgr Functions */
extern void smeStateMgr_ResetCfrm( UINT8 *info_p,
                                   macmgmtQ_CmdRsp_t *MgmtMsg_p );
extern void smeStateMgr_StartCfrm( UINT8  *info_p,
                                   macmgmtQ_CmdRsp_t *MgmtMsg_p );
extern void smeStateMgr_AssociateCfrm( UINT8  *info_p,
                                       macmgmtQ_CmdRsp_t *MgmtMsg_p );
extern void smeStateMgr_AssociateInd( UINT8  *info_p,
                                      macmgmtQ_CmdRsp_t *MgmtMsg_p );
extern void smeStateMgr_DisassociateCfrm( UINT8 *info_p,
                                          macmgmtQ_CmdRsp_t *MgmtMsg_p );
extern void smeStateMgr_DisassociateInd( UINT8 *info_p,
                                         macmgmtQ_CmdRsp_t *MgmtMsg_p );
extern void smeStateMgr_ReassociateCfrm( UINT8 *info_p, macmgmtQ_CmdRsp_t *MgmtMsg_p );
extern void smeStateMgr_ReassociateInd( UINT8 *info_p,
                                        macmgmtQ_CmdRsp_t *MgmtMsg_p );
extern void smeStateMgr_AuthenticateCfrm( UINT8 *info_p,
                                          macmgmtQ_CmdRsp_t *MgmtMsg_p );
extern void smeStateMgr_AuthenticateInd( UINT8  *info_p,
                                         macmgmtQ_CmdRsp_t *MgmtMsg_p );
extern void smeStateMgr_DeauthenticateCfrm( UINT8 *info_p,
                                            macmgmtQ_CmdRsp_t *MgmtMsg_p );
extern void smeStateMgr_DeauthenticateInd( UINT8 *info_p,
                                           macmgmtQ_CmdRsp_t *MgmtMsg_p );
extern SINT32 smeStateMgr_SndAuthCmd(UINT8 *info_p,
                                   IEEEtypes_MacAddr_t *peerAddr);
extern void smeStateMgr_ScanCfrm( UINT8 *info_p,
								  macmgmtQ_CmdRsp_t *MgmtMsg_p );
extern SINT32 smeStateMgr_JoinCfrm( UINT8 *info_p,
									macmgmtQ_CmdRsp_t *MgmtMsg_p );
extern void smeStateMgr_SndStartCmd( UINT8 *info_p,
                                    IEEEtypes_BssDesc_t *bssDesc_p );
extern SINT32 smeStateMgr_SndJoinCmd( UINT8 *info_p,
                                    IEEEtypes_BssDesc_t *bssDesc_p);
extern SINT32 smeSendResetCmd_Sta(UINT8 macIndex, int quiet);

#ifdef WMON
extern UINT8 gScan;
#endif //WMON
#endif /* MAC_MLME_STA */





