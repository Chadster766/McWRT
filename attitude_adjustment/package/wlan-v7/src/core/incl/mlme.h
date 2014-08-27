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
*        Header file for AP MLME State Machines
*
*/

#ifndef MAC_MLME_
#define MAC_MLME_

#define EVENT_NUM_MAX   64
#define ASSOC_TIMEOUT  5 /*500ms */
#define AUTH_TIMEOUT  5 /*500ms */

#include "mhsm.h"
#include "timer.h"
#include "wltypes.h"
#include "IEEE_types.h"

#define buildModes_RETRIES  1
//#define MLME_SEPERATE_SCAN_TIMER            1
//#define MLME_FORCE_STA_TO_PRIMARY_ADDR      1
#define MLME_START_11B_IBSS_ONLY            1
#define MLME_SYNC_AP                        1
#define MLME_POLL_BEACON_Q                  1

#define LINK_DOWN				0
#define LINK_UP					1
#define TIMER_TICK  		100   /* in unit of ms */
#ifdef MLME_SEPERATE_SCAN_TIMER
#define TIMER_TICK_SCAN 	10    /* in unit of ms */
#else
#define TIMER_TICK_SCAN 	TIMER_TICK    /* in unit of ms */
#endif /* MLME_SEPERATE_SCAN_TIMER */
#define MAX_SCAN_BUF_SIZE   40960
#define BUF_PAD_NUM         10
#define FCS_SIZE              4

/* Filter Bit Map */
#define MLME_SSID_FILTER                    (1<<0)
#define MLME_BSSID_FILTER                   (1<<1)
#define MLME_BSS_TYPE_FILTER                (1<<2)
#define MLME_SIGNAL_STRENGTH_FILTER         (1<<3)

#define MLME_CMD_CLIENT_INFRA_JOIN          0
#define MLME_CMD_CLIENT_IBSS_START          1
#define MLME_CMD_CLIENT_IBSS_JOIN           2

/* Command Bit Map */
#define cmdScan		(1)
#define cmdAssoc	(1 << 1)
#define cmdDisAssoc (1 << 2)
#define cmdAuth		(1 << 3)
#define cmdDeAuth	(1 << 4)
#define cmdJoin		(1 << 5)
#define cmdStart	(1 << 6)
#define cmdReset	(1 << 7)
#define cmdReAssoc  (1 << 8)

#define MLME_CAPINFO_VALIDATION_MASK        0x0013
#define MAX_CNT_PARAM_MISMATCH_DETECTED     5

/* Join Status */
#define JOIN_SUCCESS                        0
#define JOIN_FAIL_AUTH_TIMEOUT              1
#define JOIN_FAIL_AUTH_REJECTED             2
#define JOIN_FAIL_ASSOC_TIMEOUT             3
#define JOIN_FAIL_ASSOC_REJECTED            4
#define JOIN_FAIL_INTERNAL_ERROR            5

/* New Timer Timeout values */
#ifdef SC_PALLADIUM  /* Increase Scan time for Palladium testing */
#define SCAN_TIME  		200   /* in unit of 100ms */
#else
#define SCAN_TIME  		5   /* in unit of 100ms */
#endif

#ifndef AP_URPTR
#define STATUS_TIME     20 	/* in unit of 100ms */
#else
#define STATUS_TIME     300
#endif

#ifdef SC_PALLADIUM  /* Increase assoc and auth timeouts for Palladium */
#define ASSOC_TIME      6000 // 20 //10
#define AUTH_TIME       6000 //20 //10
#else
#define ASSOC_TIME      20 //10
#define AUTH_TIME       20 //10
#endif

#define MAX_SHARED_KEY_AUTHENTICATIONS  1    /* 5 -> 2 -> 1 */
#define WEP_ENCRYPT_OVER_HDR_LEN        8
#define EVENT_NUM_MAX   64
//#define ASSOC_TIMEOUT  5 //500ms
//#define AUTH_TIMEOUT  5 //500ms

#define MAX_B_DATA_RATES    4
#define MAX_G_DATA_RATES    8


#define MLME_SUCCESS	0
#define MLME_INPROCESS 1
#define MLME_FAILURE	-1

#define IEEEtypes_MAX_DATA_RATES_G     14
#define INTERVAL_LOOK_AHEAD 2


#define MAX_ALL_RATE_LEN    IEEEtypes_MAX_DATA_RATES_G

// This is defined to indicate the trunk type to the datapath
#define STA_TRUNK_TYPE 0x10

#define ETH_EVT_JOIN_TIMEOUT    0x10000

/* For Association Services */
/* For Station */
typedef struct AssocSrvSta
{
	Mhsm_t super;
	MhsmState_t sTop;
	MhsmState_t Assoc_Srv_Sta;
	MhsmState_t Assoc_Idle, Wait_Assoc_Rsp, Wait_ReAssoc_Rsp;
	Timer timer;
	unsigned char *userdata_p;
}AssocSrvSta;


/* For AP */
typedef struct AssocSrvAp
{
	Mhsm_t super;
	MhsmState_t sTop;
	MhsmState_t Assoc_Srv_Ap;
	MhsmState_t Assoc_Idle, Wait_Assoc_Rsp, Wait_ReAssoc_Rsp;
	Timer timer;
	unsigned char *userdata_p;
}AssocSrvAp;


/* For Authentication Services */
enum arAlg_t
{
    open_system,
    shared_key
};

/* For Station*/
typedef struct AuthRspSrvSta
{
	Mhsm_t super;
	MhsmState_t sTop;
	MhsmState_t Auth_Rsp_Srv_Sta;
	MhsmState_t Auth_Rsp_Idle, Wait_Chal_Rsp;
	Timer timer;
	unsigned char *userdata_p;
}AuthRspSrvSta;

typedef struct AuthReqSrvSta
{
	Mhsm_t super;
	MhsmState_t sTop;
	MhsmState_t Auth_Req_Srv_Sta;
	MhsmState_t Auth_Req_Idle, Wait_Auth_Seq2, Wait_Auth_Seq4;
	Timer timer;
	unsigned char *userdata_p;
}AuthReqSrvSta;

typedef struct RemoteCtrlSrv
{
	Mhsm_t super;
	MhsmState_t sTop;
	MhsmState_t remoteSrv_Ap;
	MhsmState_t remoteSrv_idle, remoteSrv_ctrl;
	Timer timer; 
    unsigned char *userdata_p;
	#define QUE_LEN 4
	UINT32 que[QUE_LEN];
	UINT8 que_idx_w;
	UINT8 que_idx_r;
	UINT8 maxtimeout;
}RemoteCtrlSrv;

typedef struct PowerSaveMonitor
{
    Mhsm_t super;
    MhsmState_t Power_Save_Monitor, Monitor_Idle;
}PowerSaveMonitor;

typedef struct AuthReqSrvStaMsg
{
    UINT8       rspMac[8];
    UINT16       arAlg;
    UINT16      arAlg_in;
    UINT8       arSeq;
    UINT8       arSeq2;
    UINT8       dot11Privacy;
    UINT8       *mgtMsg;
}AuthReqSrvStaMsg;

typedef struct SyncSrvStaMsg
{
    UINT8       opMode;
    UINT8       scanMode;
    UINT8       *rfHdr_p;
    UINT8       *mgtFrame_p;
    UINT8       *cmdMsg_p;
    UINT8       *statMsg_p;
}SyncSrvStaMsg;

/* For AP */
typedef struct AuthRspSrvAp
{
	Mhsm_t super;
	MhsmState_t sTop;
	MhsmState_t Auth_Rsp_Srv_Ap;
	MhsmState_t Auth_Rsp_Idle, Wait_Chal_Rsp;
	Timer timer;
	unsigned char *userdata_p;
}AuthRspSrvAp;

typedef struct AuthReqSrvAp
{
	Mhsm_t super;
	MhsmState_t sTop;
	MhsmState_t Auth_Req_Srv_Ap;
	MhsmState_t Auth_Req_Idle, Wait_Auth_Seq2, Wait_Auth_Seq4;
    Timer timer;
    unsigned char *userdata_p;
}AuthReqSrvAp;

typedef struct AuthRspSrvApMsg
{
    UINT8       rspMac[8];
    UINT16       arAlg;
	UINT16		arAlg_in;
    UINT8       arSeq;
    UINT8       arSeq2;
    UINT8       dot11Privacy;
    UINT8       *mgtMsg;
}AuthRspSrvApMsg;

/* For Synchronization Services */
enum apOpMode_t
{
    independent,
    infrastructure
};

enum scanMode_t
{
    scan_passive,
    scan_active
};

/* For AP */
typedef struct SyncSrvAp
{
	Mhsm_t super;
	MhsmState_t sTop;
	MhsmState_t Sync_Srv_Ap;
	MhsmState_t No_Bss, Bss, Sta_Active;
    Timer timer;
}SyncSrvAp;

typedef struct SyncSrvApMsg
{
    UINT8       opMode;
    UINT8       *mgtMsg;
}SyncSrvApMsg;

/* For STA */

typedef struct SyncSrvSta
{
	Mhsm_t super;
	MhsmState_t sTop;
	MhsmState_t Sync_Srv_Sta;
    MhsmState_t Act_Recv, No_Bss, AP_Active, IBss_Active, Pas_Listen, Bss,IBss_Idle;
    MhsmState_t Join_Wait_Beacon, Act_Listen, Wait_Csw_done, Wait_Hop_Bss;
    MhsmState_t Init_Wait_Probe_Delay, Wait_Probe_Delay;
}SyncSrvSta;

/* MLME Stat Machine Id */
enum macMgtSrvId_t
{
    sync_srv,
    auth_req_srv,
    auth_rsp_srv,
    assoc_srv
};

typedef enum
{
	BAND_B,
}RF_BAND;


/* MLME State Machine Events */
enum MacMgtEvents
{
    AssocReq, 
    AssocRsp, 
    ATim, 
    AuthEven, 
    AuthOdd, 
    Beacon, 
    Cfend, 
    Cls2err, 
    Cls3err, 
    DeAuth, 
    DisAssoc, //10
    DsResponse,
    Mlme_Confirms, 
    Mlme_Indications, 
    Mlme_Requests, 
    MlmeAssoc_Cnfm, 
    MlmeAssoc_Ind,
    MlmeAssoc_Req, 
    MlmeAuth_Cnfm, 
    MlmeAuth_Ind, 
    MlmeAuth_Req, //20
    MlmeDeAuth_Cnfm, 
    MlmeDeAuth_Ind, 
    MlmeDeAuth_Req, 
    MlmeDisAssoc_Cnfm, 
    MlmeDisAssoc_Ind, 
    MlmeDisAssoc_Req, 
    MlmeJoin_Cnfm, 
    MlmeJoin_Req, 
    MlmePwMgt_Cnfm, 
    MlmePwMgt_Req, //30
    MlmeReAssoc_Cnfm, 
    MlmeReAssoc_Ind,
    MlmeReAssoc_Req, 
    MlmeScan_Cnfm, 
    MlmeScan_Req, 
    MlmeStart_Cnfm, 
    MlmeStart_Req, 
    MlmeReset_Req,
    MlmeReset_Cnfm,
    MlmeReset_Ind, //40
    MmgtConfirmSignals, 
    MmgtIndicationSignals, 
    MmgtRequestSignals, 
    ProbeReq,
    ProbeRsp, 
    PsIndicate,
    PsmDone,
    ReAssocReq, 
    ReAssocRsp, 
    ResetMAC,
    Send, 
    Sent, 
    Sst, 
    StaState, 
    SwDone,
    Tatim,
    Tbcn,
    Tmocp,
    Tpdly,
    Tscan,
#ifdef QOS_FEATURE
    QoSAction,
#endif
#ifdef IEEE80211H
    MlmeMrequest_Cnfm, 
    MlmeMrequest_Req,
    MlmeMrequest_Ind,
    MlmeMeasure_Cnfm, 
    MlmeMeasure_Req,   
    MlmeMreport_Cnfm, 
    MlmeMreport_Req,
    MlmeMreport_Ind,
    MlmeChannelSwitch_Cnfm, 
    MlmeChannelSwitch_Req,
    MlmeChannelSwitch_Ind,
    MlmeChannelSwitch_Rsp, 
    MlmeTpcAdpt_Req,
    MlmeTpcAdpt_Cnfm,
#endif /* IEEE80211H */
    RmCtrl,
    RmCtrlAck, 
    Xport,
    PeerDetectInCompat,
    Timeout
};
extern void SyncSrvCtor(SyncSrvAp*me);
extern void AuthReqSrvApCtor(AuthReqSrvAp *me);



#endif /* MAC_MLME_ */
