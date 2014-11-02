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
*        Header file for AP DFS State Machines
*
*/

#ifndef _DFS_SM_
#define _DFS_SM_

#ifdef MRVL_DFS
#include <linux/netdevice.h>
#include "mhsm.h"
#include "timer.h"
#include "wltypes.h"
#include "List.h"
#include "hostcmd.h"
#include "wl_hal.h"

#define MAX_VMAC_AP    NUMOFAPS
struct wlprivate_data ;

#define DFS_DEFAULT_CAC_TIMEOUT  600 // 600*100 ms = 60 seconds 
#define DFS_DEFAULT_NOC_TIMEOUT  18000 // 18000*100 ms = 1800 seconds 
#define DFS_DEFAULT_CSA_TIMEOUT  100 // 100*100 ms = 10 seconds 

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
	UINT16		CACTimeOut ;
	UINT16		CSATimeOut ;
	UINT16		NOCTimeOut ;
	UINT32      cac_complete;
	UINT8		vaplist[MAX_VMAC_AP];
	UINT8		vapcount ;
}DfsApDesc ;

typedef struct _DfsAp
{
	Mhsm_t super;
	MhsmState_t sTop;
	MhsmState_t Dfs_Ap;
	MhsmState_t Dfs_Init, Dfs_Scan, Dfs_Operational, Dfs_Csa ;
	DfsApDesc		dfsApDesc ;
	UINT32			dropData ;
	struct net_device *pNetDev;
}DfsAp;

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
void DfsInit(struct net_device  *pNetDev, struct wlprivate_data *wlpd_p);
void DfsDeInit(struct wlprivate_data *wlpd_p);
void DFSApCtor(struct net_device *pNetDev, DfsAp *me);
DFS_STATUS DfsEnabledChannel(struct net_device *pNetDev, DfsChanInfo *chanInfo_p );
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
DFS_STATUS DfsPresentInNOL( struct net_device  *dev, UINT8 channel );
void DFSApReset( DfsAp *me);

extern SINT8 evtDFSMsg(struct net_device *dev, UINT8 *message);
#endif //MRVL_DFS
#endif //_DFS_SM_
