/*
*                Copyright 2002-2014, Marvell Semiconductor, Inc.
* This code contains confidential information of Marvell Semiconductor, Inc.
* No rights are granted herein under any patent, mask work right or copyright
* of Marvell or any third party.
* Marvell reserves the right at its sole discretion to request that this code
* be immediately returned to Marvell. This code is provided "as is".
* Marvell makes no warranties, express, implied or otherwise, regarding its
* accuracy, completeness or performance.
*/


#ifndef _DFS_SM_
#define _DFS_SM_

/* DFS_SUPPORT */
#include "mhsm.h"
#include "timer.h"
#include "wltypes.h"
#include "List.h"
#include "hostcmd.h"
#include "wl_hal.h"

#define MAX_VMAC_AP    NUMOFAPS
struct _wlprivate_data ;

#define DFS_DEFAULT_CAC_TIMEOUT  60000 // 60000 ms = 60 seconds 
#define DFS_DEFAULT_NOC_TIMEOUT  1800000 // 1800000 ms = 1800 seconds 
#define DFS_DEFAULT_CSA_TIMEOUT  10000 // 10000 ms = 10 seconds 

typedef enum _DFS_STATE
{
	DFS_STATE_UNKNOWN = 0,
	DFS_STATE_INIT,
	DFS_STATE_SCAN,
	DFS_STATE_OPERATIONAL,
	DFS_STATE_CSA
}DFS_STATE ;
typedef UINT8 DFS_STATE_t;

#define DFS_QUIET_MODE		0
#define DFS_NORMAL_MODE		1

typedef enum _DFS_STATUS
{
	DFS_FAILURE = 0 ,
	DFS_SUCCESS 
}DFS_STATUS;

typedef struct _DfsChanInfo
{
	UINT8		channel ;
	CHNL_FLAGS	chanflag ;
}DfsChanInfo ;

typedef struct _Dfs_RadarDetInd_t
{
	DfsChanInfo	chInfo ;
}Dfs_RadarDetInd_t;

typedef struct _Dfs_ChanSwitchCfrm_t
{
	UINT8		result ;
	DfsChanInfo	chInfo ;
}Dfs_ChanSwitchCfrm_t;


typedef struct _NOCListItem
{
	struct _NOCListItem	*nxt ;
	struct _NOCListItem *prv ;
	UINT8 channel ;
	UINT32	occurance ;
}NOCListItem ;

typedef struct _DfsApDesc
{
	DFS_STATE_t	currState ;
	DfsChanInfo	currChanInfo ;
	Timer		CACTimer ;
	Timer		NOCTimer ;
	Timer		CSATimer ;
	List		NOCList ;
	UINT32		CACTimeOut ;
	UINT32		CSATimeOut ;
	UINT32		NOCTimeOut ;
	UINT32      cac_complete;
	UINT8		vaplist[MAX_VMAC_AP];
	UINT8		vapcount ;
}DfsApDesc ;

struct _DfsAp
{
	Mhsm_t super;
	MhsmState_t sTop;
	MhsmState_t Dfs_Ap;
	MhsmState_t Dfs_Init, Dfs_Scan, Dfs_Operational, Dfs_Csa ;
	DfsApDesc		dfsApDesc ;
	UINT32			dropData ;
	void *wlpptr;
};

#define DfsAp struct _DfsAp

typedef struct _DfsApMsg
{
	UINT8       opMode;
	UINT8       *mgtMsg;
}DfsApMsg;


/* MLME State Machine Events */
enum DfsEvents
{
	CHANNEL_CHANGE_EVT,
	RADAR_EVT,
	CAC_EXPIRY_EVT,
	WL_RESET_EVT,
	CSA_EXPIRY_EVT
};

/* Function declarations */
void DfsDeInit(struct _wlprivate_data *wlpd_p);
DFS_STATUS DfsAddToNOL( DfsApDesc *dfsDesc_p, UINT8 channel );
DFS_STATUS DfsRemoveFromNOL( DfsApDesc *dfsDesc_p );
//static void NOCTimeoutHandler(void *data_p);
UINT8 DfsDecideNewTargetChannel( DfsApDesc *dfsDescr_p );
DFS_STATUS DfsFindInNOL( List *NOCList, UINT8 chan );
void DfsPrintNOLChannelDetails( DfsAp *me, char *NOLPrintStr_p, int maxLength);
DFS_STATE DfsGetCurrentState(DfsAp *me);
UINT16 DfsGetCACTimeOut( DfsAp *me );
UINT16 DfsGetNOCTimeOut( DfsAp *me );
DFS_STATUS DfsSetCACTimeOut( DfsAp *me , UINT16 timeout);
DFS_STATUS DfsSetNOCTimeOut( DfsAp *me , UINT16 timeout);
UINT8 DFSGetExtensionChannelOfDFSChan( UINT8 domain, UINT8 extChnlOffset , UINT8 channel);
UINT8 DFSGetCurrentRadarDetectionMode(DfsAp *me ,UINT8 channel, CHNL_FLAGS chanFlag );
void DFSApReset( DfsAp *me);

#endif //_DFS_SM_
