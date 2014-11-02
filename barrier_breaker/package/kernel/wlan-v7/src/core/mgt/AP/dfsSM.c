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
* Description:  Implementation of the AP's DFS service 
*
*/
#ifdef MRVL_DFS

#include <linux/netdevice.h>
#include "mhsm.h"
#include "wltypes.h"
#include "wldebug.h"
#include "IEEE_types.h"
#include "macmgmtap.h"
#include "macMgmtMlme.h"
#include "hostcmd.h"
#include "domain.h"
#include "timer.h"
#include "List.h"
#include "dfs.h"
#include "ap8xLnxIntf.h"

#define DFS_MAX_CHANNELS 				15
#define DFS_DEFAULT_CSAMODE 			1
#define DFS_DEFAULT_COUNTDOWN_NUMBER	20
#define DFS_FALLBACK_CHANNEL			36 // Non DFS channel

extern UINT8 dfs_test_mode;

extern void ListPutItemFILO(List *me, ListItem *Item);

typedef  struct _DFS_CHANNEL_LIST
{
	UINT8 domainCode;
	UINT8 dfschannelEntry[DFS_MAX_CHANNELS];
}
PACK_END DFS_CHANNEL_LIST;

static DFS_CHANNEL_LIST dfsEnabledChannels[]=
{
	{DOMAIN_CODE_FCC, {52,56,60,64,100,104,108,112,0,0,0,0,0,136,140}},
	{DOMAIN_CODE_IC, {52,56,60,64,0,0,0,0,0,0,0,0,0,0,0}},
	{DOMAIN_CODE_ETSI, {52,56,60,64,100,104,108,112,116,120,124,128,132,136,140}},
	{DOMAIN_CODE_SPAIN, {52,56,60,64,100,104,108,112,116,120,124,128,132,136,140}},
	{DOMAIN_CODE_FRANCE, {52,56,60,64,100,104,108,112,116,120,124,128,132,136,140}},
	{DOMAIN_CODE_MKK, {52,56,60,64,100,104,108,112,116,120,124,128,132,136,140}},
	{DOMAIN_CODE_DGT, { 38,42,46,184,188,192,196,0,0,0,0,0,0,0,0}},
	{DOMAIN_CODE_AUS, { 38,42,46,184,188,192,196,0,0,0,0,0,0,0,0}},
};

void CACTimeoutHandler(void *data_p);
void CSATimeoutHandler(void *data_p);
static void NOCTimeoutHandler(void *data_p);

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
MhsmEvent_t const *DfsAp_top( DfsAp *me, MhsmEvent_t *msg)
{
#ifdef DEBUG_PRINT
	printk("DfsAp_top:: Enter\n");
#endif
	if((me == NULL) || (msg == NULL))
	{
#ifdef DEBUG_PRINT
		printk("DfsAp_top:: error: NULL pointer\n");
#endif
		return 0;
	}

	switch (msg->event)
	{
	case MHSM_ENTER:
		mhsm_transition(&me->super, &me->Dfs_Ap);
		return 0;

	default:
		return msg;
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
MhsmEvent_t const *Dfs_Ap_Handle(DfsAp *me, MhsmEvent_t *msg)
{
#ifdef DEBUG_PRINT
	printk("Dfs_Ap_Handle:: Enter\n");
#endif
	if((me == NULL) || (msg == NULL))
	{
#ifdef DEBUG_PRINT
		printk("Dfs_Ap_Handle:: error: NULL pointer\n");
#endif
		return 0;
	}

	switch (msg->event)
	{
	case MHSM_ENTER:
		mhsm_transition(&me->super, &me->Dfs_Init);
		return 0;

	default:
		return msg;
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
MhsmEvent_t const *Dfs_Init_Handle(DfsAp *me, MhsmEvent_t *msg)
{

	DfsApMsg		*dfsMsg_p = NULL ;
	DfsApDesc		*dfsDesc_p = NULL ;
	struct net_device *dev;

#ifdef DEBUG_PRINT
	printk("Dfs_Init_Handle:: Enter :%d\n", msg->event);
#endif
	if((me == NULL) || (msg == NULL))
	{
#ifdef DEBUG
		printk("Dfs_Init_Handle:: error: NULL pointer\n");
#endif
		return 0;
	}
	dfsMsg_p = (DfsApMsg *)msg->pBody ;
	dfsDesc_p = (DfsApDesc *)&me->dfsApDesc ;
	dev = me->pNetDev;

	switch (msg->event)
	{
	case MHSM_ENTER:
		dfsDesc_p->currState = DFS_STATE_INIT ;
		return 0;

	case CHANNEL_CHANGE_EVT:
		{
			DfsChanInfo		*chanInfo_p = (DfsChanInfo *)dfsMsg_p->mgtMsg ;
#ifdef DEBUG_PRINT
			printk("Dfs_Init_Handle:: event-> CHANNEL_CHANGE_EVT\n");
#endif

			if( chanInfo_p == NULL )
			{
				return 0 ;
			}
			memcpy( &dfsDesc_p->currChanInfo, chanInfo_p, sizeof( DfsChanInfo ) );
			/* First check if the given channel is within DFS range */
			if( DfsEnabledChannel(me->pNetDev,  chanInfo_p ) )
			{
				/* Start DFS radar SCAN */
				TimerFireIn(&dfsDesc_p->CACTimer, 1, 
					&CACTimeoutHandler, (unsigned char *)me, dfsDesc_p->CACTimeOut);

				/* Stop data traffic */
				macMgmtMlme_StopDataTraffic(dev);

				/*Initiate Quiet mode radar detection */
				macMgmtMlme_StartRadarDetection(dev, DFS_QUIET_MODE);

				mhsm_transition(&me->super, &me->Dfs_Scan);
			}
			else
			{
				WLDBG_INFO(DBG_LEVEL_1, "Dfs_Init_Handle  Become Operational \n");
				/* Become operational */
				mhsm_transition(&me->super, &me->Dfs_Operational);
			}
			return 0;

		}
	default:
		return 0;

	}

	return msg;
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
MhsmEvent_t const *Dfs_Scan_Handle(DfsAp *me, MhsmEvent_t *msg)
{
	UINT8 	channel ;
	DfsApMsg		*dfsMsg_p = NULL ;
	DfsApDesc	*dfsDesc_p = NULL ;
	struct net_device *dev;
	struct wlprivate *wlpptr = NULL ;
	vmacApInfo_t *vmacSta_p = NULL ;

#ifdef DEBUG_PRINT
	printk("Dfs_Scan_Handle:: Enter :%d\n", msg->event);
#endif
	if((me == NULL) || (msg == NULL))
	{
#ifdef DEBUG
		printk("Dfs_Scan_Handle:: error: NULL pointer\n");
#endif
		return 0;
	}
	dev = me->pNetDev;
    wlpptr = NETDEV_PRIV_P(struct wlprivate, dev);
    vmacSta_p=(vmacApInfo_t *)wlpptr->vmacSta_p;

	dfsMsg_p = (DfsApMsg *)msg->pBody ;
	dfsDesc_p = (DfsApDesc *)&me->dfsApDesc ;
	switch (msg->event)
	{
	case MHSM_ENTER:
		dfsDesc_p ->currState = DFS_STATE_SCAN ;
		dfsDesc_p->cac_complete = 0 ;
		dfsDesc_p->vapcount = 0 ;
		return 0;

	case RADAR_EVT:
//#ifdef DEBUG_PRINT
		printk("Dfs_Scan_Handle:: event-> RADAR_EVT\n");
//#endif
		dfsDesc_p->cac_complete = 0 ;
		dfsDesc_p->vapcount = 0 ;
		/* Stops CAC timer */
        if (!dfs_test_mode)
        {
            /* For W8764 DFS test mode do not enable CAC timer and do not add channel to NOL. */ 
		    TimerDisarm(&dfsDesc_p->CACTimer);

		    /* Updated NOL */
		    DfsAddToNOL( dfsDesc_p, dfsDesc_p->currChanInfo.channel );
        }

		/* decide a new target channel */
		channel = DfsDecideNewTargetChannel( dfsDesc_p );
		if( channel == 0 )
		{
//#ifdef DEBUG_PRINT
			printk("No target channel found to switch to\n");
//#endif
			return 0 ;
		}
		dfsDesc_p->currChanInfo.channel = channel ;

//#ifdef DEBUG_PRINT
		printk("New Target channel is :%d\n", channel );
//#endif

        if (!dfs_test_mode)
        {
            /* For W8764 DFS test mode do not switch channel, CAC timer not enabled. */
		    /* Switch to the target channel */
		    macMgmtMlme_SwitchChannel(dev,channel, &dfsDesc_p->currChanInfo.chanflag);

		    /* Stops CAC timer */
		    TimerDisarm(&dfsDesc_p->CACTimer);
        }

		if( DfsEnabledChannel(me->pNetDev, &dfsDesc_p->currChanInfo ) )
		{
			/* Restart the CAC timer */
			TimerFireIn(&dfsDesc_p->CACTimer, 1, 
				&CACTimeoutHandler, (unsigned char *)me, dfsDesc_p->CACTimeOut);

			/* Need to restart the radar detection after a channel change */
			macMgmtMlme_StartRadarDetection(dev, DFS_QUIET_MODE);
		}
		else
		{

			/* Restart data traffic */
			macMgmtMlme_RestartDataTraffic(dev);

			macMgmtMlme_Reset(dev, dfsDesc_p->vaplist, &dfsDesc_p->vapcount);

			dfsDesc_p->cac_complete = 2 ; // Reset on Non DFS Channel

		}

		return 0;

	case CAC_EXPIRY_EVT:
#ifdef DEBUG_PRINT
		printk("Dfs_Scan_Handle:: event-> CAC_EXPIRY_EVT\n");
#endif
		/* Restart data traffic */
		macMgmtMlme_RestartDataTraffic(dev);

        vmacSta_p->dfsCacExp = 1;
		macMgmtMlme_Reset(dev, dfsDesc_p->vaplist, &dfsDesc_p->vapcount);

		dfsDesc_p->cac_complete = 1 ; // Reset on DFS Channel

		return 0;

	case WL_RESET_EVT:
#ifdef DEBUG_PRINT
		printk("Dfs_Scan_Handle:: event-> WL_RESET_EVT\n");
#endif
		if( dfsDesc_p->cac_complete > 0 )
		{
			dfsDesc_p->currState = DFS_STATE_OPERATIONAL ;
			macMgmtMlme_MBSS_Reset(dev, dfsDesc_p->vaplist, dfsDesc_p->vapcount);

			if( dfsDesc_p->cac_complete == 1 )
			{
				/* Starts normal mode radar detection */
				macMgmtMlme_StartRadarDetection(dev, DFS_NORMAL_MODE);
			}

			dfsDesc_p->cac_complete = 0 ;
			dfsDesc_p->vapcount = 0 ;

			/* go to OPERATIONAL state */
			mhsm_transition(&me->super, &me->Dfs_Operational);
		}
		return 0 ;

	case CHANNEL_CHANGE_EVT:
		{
#ifdef DEBUG_PRINT
			printk("Dfs_Scan_Handle:: event-> CHANNEL_CHANGE_EVT\n");
#endif
			DfsChanInfo		*chanInfo_p = (DfsChanInfo *)dfsMsg_p->mgtMsg ;

			/* Stop data traffic */
			macMgmtMlme_StopDataTraffic(dev);

			/* Stops CAC timer */
			TimerDisarm(&dfsDesc_p->CACTimer);
			memcpy( &dfsDesc_p->currChanInfo, chanInfo_p, sizeof( DfsChanInfo ) );
			if( DfsEnabledChannel( me->pNetDev,&dfsDesc_p->currChanInfo ) )
			{
				/* Restart the CAC timer */
				TimerFireIn(&dfsDesc_p->CACTimer, 1, 
					&CACTimeoutHandler, (unsigned char *)me, dfsDesc_p->CACTimeOut);

			}
			else
			{
				/* Restart data traffic */
				macMgmtMlme_RestartDataTraffic(dev);

				macMgmtMlme_Reset(dev, dfsDesc_p->vaplist, &dfsDesc_p->vapcount);

				dfsDesc_p->cac_complete = 2 ; //Reset on Non-DFS channel
			}
		}
		return 0;
	default:
		return 0;

	}

	return msg;
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
MhsmEvent_t const *Dfs_Operational_Handle(DfsAp *me, MhsmEvent_t *msg)
{
	UINT8	channel ;
	IEEEtypes_ChannelSwitchCmd_t  ChannelSwitchCmd ;
	DfsApMsg	*dfsMsg = NULL ;
	DfsApDesc	*dfsDesc_p = NULL ;
	struct net_device *dev;

#ifdef DEBUG_PRINT
	printk("Dfs_Operational_Handle:: Enter :%d\n", msg->event );
#endif
	if((me == NULL) || (msg == NULL))
	{
#ifdef DEBUG
		printk("Dfs_Operational_Handle:: error: NULL pointer\n");
#endif
		return 0;
	}
	dfsMsg = (DfsApMsg *)msg->pBody ;
	dfsDesc_p = (DfsApDesc *)&me->dfsApDesc ;
#ifdef AP_MAC_LINUX
	dev = me->pNetDev;
#endif
	switch (msg->event)
	{
	case MHSM_ENTER:
		dfsDesc_p ->currState = DFS_STATE_OPERATIONAL ;
		return 0;

	case CHANNEL_CHANGE_EVT:
		{
			DfsChanInfo	*chanInfo_p = (DfsChanInfo *)dfsMsg->mgtMsg ;
#ifdef DEBUG_PRINT
			printk("Dfs_Operational_Handle:: event-> POST_CHANNEL_CHANGE_EVT\n");
#endif
			if( chanInfo_p == NULL )
			{
				printk("NULL ChanInfo received\n");
				return 0 ;
			}
			/* Decide if target channel is a DFS channel */

			if( DfsEnabledChannel(me->pNetDev, chanInfo_p ) )
			{

				/* Switch on quiet mode radar detection */
				macMgmtMlme_StartRadarDetection(dev, DFS_QUIET_MODE);

				/* Stop data traffic */
				macMgmtMlme_StopDataTraffic(dev);

				memcpy( &dfsDesc_p->currChanInfo, chanInfo_p, sizeof( DfsChanInfo ) );


				TimerFireIn(&dfsDesc_p->CACTimer, 1, 
					&CACTimeoutHandler, (unsigned char *)me, dfsDesc_p->CACTimeOut);

				/* go to SCAN state */
				mhsm_transition(&me->super, &me->Dfs_Scan);
			}
			else /* target channel is not dfs enabled */
			{
				memcpy( &dfsDesc_p->currChanInfo, chanInfo_p, sizeof( DfsChanInfo ) );
			}

		}
		return 0;

	case RADAR_EVT:
//#ifdef DEBUG_PRINT
		printk("Dfs_Operational_Handle:: event-> RADAR_EVT\n");
//#endif
		if( DfsEnabledChannel(me->pNetDev, &dfsDesc_p->currChanInfo ) )
		{

            if (!dfs_test_mode)
            {  /* Do not add channel to NOL in dfs test mode. */
			    /* Stop data traffic */
			    macMgmtMlme_StopDataTraffic(dev);

			    /* Switch off radar detection */
			    macMgmtMlme_StopRadarDetection(dev,DFS_NORMAL_MODE);

			    /* Updated NOL */
			    DfsAddToNOL( dfsDesc_p, dfsDesc_p->currChanInfo.channel );

            }
			/* decide a new target channel */
			channel = DfsDecideNewTargetChannel( dfsDesc_p );
			if( channel == 0 )
			{
//#ifdef DEBUG_PRINT
				printk("No target channel found to switch to\n");
//#endif
				return 0;
			}
//#ifdef DEBUG_PRINT
			printk("New Target channel is :%d\n", channel );
//#endif

			/* Insert Channel Switch Announcement IE in the beacon/probe-response
			* and initiate countdown process */
			ChannelSwitchCmd.Mode = DFS_DEFAULT_CSAMODE ;
			ChannelSwitchCmd.ChannelNumber = channel ;
			ChannelSwitchCmd.ChannelSwitchCount = DFS_DEFAULT_COUNTDOWN_NUMBER ;

			macMgmtMlme_SendChannelSwitchCmd(dev, &ChannelSwitchCmd);

            if (!dfs_test_mode)   /* DFS test mode, stay in same state. */
            {   
			    /* go to CSA state */
			    mhsm_transition(&me->super, &me->Dfs_Csa);
            }
		}

		return 0;

	default:
		return 0;

	}

	return msg;
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
MhsmEvent_t const *Dfs_Csa_Handle(DfsAp *me, MhsmEvent_t *msg)
{
	DfsApMsg	*dfsMsg = NULL ;
	DfsApDesc	*dfsDesc_p = NULL ;
	DfsChanInfo	*chanInfo_p = NULL ;
#ifdef AP_MAC_LINUX
	struct net_device *dev;
#endif

#ifdef DEBUG_PRINT
	printk("Dfs_Csa_Handle:: Enter :%d\n", msg->event);
#endif
	if((me == NULL) || (msg == NULL))
	{
#ifdef DEBUG
		printk("Dfs_Csa_Handle:: error: NULL pointer\n");
#endif
		return 0;
	}
#ifdef AP_MAC_LINUX
	dev = me->pNetDev;
#endif

	dfsMsg = (DfsApMsg *)msg->pBody ;
	dfsDesc_p = (DfsApDesc *)&me->dfsApDesc ;
	switch (msg->event)
	{
	case MHSM_ENTER:
#ifdef DEBUG_PRINT
		printk("Dfs_Csa_Handle:: event-> MHSM_ENTER\n");
#endif
		dfsDesc_p ->currState = DFS_STATE_CSA ;
		/* Start DFS CSA timer*/
		TimerFireIn(&dfsDesc_p->CSATimer, 1, 
					&CSATimeoutHandler, (unsigned char *)me, dfsDesc_p->CSATimeOut);
		return 0;

	case CHANNEL_CHANGE_EVT:
		{
			if( !dfsMsg )
			{
				printk("CSA::CHANNEL_CHANGE_EVT NULL dfsMsg\n");
				return 0 ;
			}
			chanInfo_p = (DfsChanInfo *)dfsMsg->mgtMsg ;
#ifdef DEBUG_PRINT
			printk("Dfs_Csa_Handle:: event-> CHANNEL_CHANGE_EVT\n");
#endif
			/* Stops CSA timer */
			TimerDisarm(&dfsDesc_p->CSATimer);

			if( chanInfo_p == NULL )
			{
				return 0 ;
			}

			memcpy( &dfsDesc_p->currChanInfo, chanInfo_p, sizeof( DfsChanInfo ) );
			/* Decide if target channel is a DFS channel */
			if( DfsEnabledChannel(me->pNetDev, chanInfo_p ) )
			{

				/* Start CAC timer from the beginning */
				TimerFireIn(&dfsDesc_p->CACTimer, 1, &CACTimeoutHandler, 
					(unsigned char *)me, dfsDesc_p->CACTimeOut);
				macMgmtMlme_StartRadarDetection(dev, DFS_QUIET_MODE);

				/* go to SCAN state */
				mhsm_transition(&me->super, &me->Dfs_Scan);
			}
			else
			{

				/* Restart data traffic */
				macMgmtMlme_RestartDataTraffic(dev);

				/* go to OPERATIONAL state */
				mhsm_transition(&me->super, &me->Dfs_Operational);
			}

			return 0;
		}
	case CSA_EXPIRY_EVT:
		{
#ifdef DEBUG_PRINT
			printk("Dfs_Csa_Handle:: event-> CSA_EXPIRY_EVT\n");
#endif
				/* Restart data traffic */
				macMgmtMlme_RestartDataTraffic(dev);

				/* go to OPERATIONAL state */
				mhsm_transition(&me->super, &me->Dfs_Operational);
			return 0;
		}

	default:
		return 0;

	}

	return msg;
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
void DFSApCtor(struct net_device *pNetDev, DfsAp *me)
{
	mhsm_add(&me->sTop, NULL, (MhsmFcnPtr)DfsAp_top);
	mhsm_add(&me->Dfs_Ap, 
		&me->sTop, (MhsmFcnPtr)Dfs_Ap_Handle);
	mhsm_add(&me->Dfs_Init, &me->Dfs_Ap,
		(MhsmFcnPtr)Dfs_Init_Handle);
	mhsm_add(&me->Dfs_Scan, &me->Dfs_Ap,
		(MhsmFcnPtr)Dfs_Scan_Handle);
	mhsm_add(&me->Dfs_Operational, &me->Dfs_Ap,
		(MhsmFcnPtr)Dfs_Operational_Handle);
	mhsm_add(&me->Dfs_Csa,  &me->Dfs_Ap,
		(MhsmFcnPtr)Dfs_Csa_Handle);
	me->dropData = 0 ;
	memset (&me->dfsApDesc, 0, sizeof(DfsApDesc) );
	TimerInit( &me->dfsApDesc.CACTimer );
	TimerInit( &me->dfsApDesc.NOCTimer );
	TimerInit( &me->dfsApDesc.CSATimer );
	ListInit(&me->dfsApDesc.NOCList);
	me->dfsApDesc.CACTimeOut = DFS_DEFAULT_CAC_TIMEOUT;
	me->dfsApDesc.CSATimeOut = DFS_DEFAULT_CSA_TIMEOUT;
	me->dfsApDesc.NOCTimeOut = DFS_DEFAULT_NOC_TIMEOUT;
	me->pNetDev = (struct net_device *)pNetDev;
}

void CACTimeoutHandler(void *data_p)
{
	MhsmEvent_t msg;
	msg.event = CAC_EXPIRY_EVT;
	WLDBG_INFO(DBG_LEVEL_1, "enter CAC timeout handler\n");

	mhsm_send_event((Mhsm_t *)data_p, &msg);
}

void CSATimeoutHandler(void *data_p)
{
	MhsmEvent_t msg;
	msg.event = CSA_EXPIRY_EVT;
	WLDBG_INFO(DBG_LEVEL_1, "enter CSA timeout handler\n");

	mhsm_send_event((Mhsm_t *)data_p, &msg);
}

DFS_STATUS DfsEnabledChannel(struct net_device *pNetDev, DfsChanInfo *chanInfo_p )
{
	int 	i = 0, j = 0 ;
	UINT8	domainCode = 0 ;
	UINT8	domain = 0 ;

	if ( !macMgmtMlme_DfsEnabled(pNetDev) )
	{
#ifdef DEBUG_PRINT
		printk("DFS is not enabled\n");
#endif
		return DFS_FAILURE ;
	}
	if( chanInfo_p == NULL )
	{
#ifdef DEBUG_PRINT
		printk("NULL ChanInfo passed to DfsEnabledChannel function\n");
#endif
		return DFS_FAILURE ;
	}
	if( chanInfo_p->chanflag.FreqBand != FREQ_BAND_5GHZ )
	{
#ifdef DEBUG_PRINT
		printk("This is not a 5GHz channel\n");
#endif
		return DFS_FAILURE ;
	}
	domainCode = domainGetDomain() ; // get current domain
	for( i = 0 ; i < sizeof(dfsEnabledChannels)/sizeof(DFS_CHANNEL_LIST) ; i ++ )
	{
		domain = dfsEnabledChannels[i].domainCode ;
		if( domain != domainCode )
			continue ;
		for( j = 0 ; j < DFS_MAX_CHANNELS; j ++ )
		{
			if( chanInfo_p->channel == dfsEnabledChannels[i].dfschannelEntry[j] )
			{
				return DFS_SUCCESS ; 
			}
		}
	}
	return DFS_FAILURE ;
}

DFS_STATUS DfsAddToNOL( DfsApDesc *dfsDesc_p, UINT8 channel )
{
	NOCListItem *nocListItem_p = NULL ;
	NOCListItem *tmp = NULL ;
	UINT8	firstItem = 0 ;

	if( dfsDesc_p == NULL )
	{
		return DFS_FAILURE ;
	}
	if( DfsFindInNOL(  &dfsDesc_p->NOCList, channel ) == DFS_SUCCESS )
	{
#ifdef DEBUG_PRINT
		printk("Channel %d already in NOL\n", channel );
#endif
		return DFS_FAILURE ;
	}

	if( (tmp = (NOCListItem *)ListGetItem( &dfsDesc_p->NOCList )) == NULL )
	{
		firstItem = 1 ;
	}
	else
	{
		/* Put it back */
		ListPutItemFILO( &dfsDesc_p->NOCList, (ListItem *)tmp );
	}

	if(( nocListItem_p = kmalloc( sizeof( NOCListItem ), GFP_ATOMIC ) ) == NULL )
	{
		printk("Cannot allocate memory for NOCList\n");
		return DFS_FAILURE ;
	}
	nocListItem_p->channel = channel ;
	nocListItem_p->occurance = jiffies ;
	ListPutItem( &dfsDesc_p->NOCList, (ListItem *)nocListItem_p );

	/* Now initiate/update the NOCTimer */
	if( firstItem )
	{
		TimerFireIn(&dfsDesc_p->NOCTimer, 1, &NOCTimeoutHandler, 
			(unsigned char *)dfsDesc_p, dfsDesc_p->NOCTimeOut);
	}
	return DFS_SUCCESS ;
}

DFS_STATUS DfsRemoveFromNOL( DfsApDesc *dfsDesc_p )
{
	NOCListItem *nocListItem_p ;
	UINT32	timeoutPeriod ;

	if( dfsDesc_p == NULL )
	{
		return DFS_FAILURE ;
	}
	nocListItem_p = (NOCListItem *)ListGetItem( &dfsDesc_p->NOCList );

	if( nocListItem_p == NULL )
	{
		return DFS_FAILURE ;
	}
	kfree( nocListItem_p );

	/* Get the next item */
	nocListItem_p = (NOCListItem *)ListGetItem( &dfsDesc_p->NOCList );
	if( nocListItem_p )
	{
		timeoutPeriod = dfsDesc_p->NOCTimeOut - ((jiffies - nocListItem_p->occurance )/10);
		TimerFireIn(&dfsDesc_p->NOCTimer, 1, &NOCTimeoutHandler, 
			(unsigned char *)dfsDesc_p, timeoutPeriod);
		/* Put it back */
		ListPutItemFILO( &dfsDesc_p->NOCList, (ListItem *)nocListItem_p );
	}
	return DFS_SUCCESS ;
}

static void NOCTimeoutHandler(void *data_p)
{
	WLDBG_INFO(DBG_LEVEL_1, "enter NOC timeout handler\n");

	DfsRemoveFromNOL( (DfsApDesc *)data_p );
}

UINT8 DfsDecideNewTargetChannel( DfsApDesc *dfsDesc_p )
{
	UINT8	domainCode ;
	UINT8 	randInd = 0 ;
	int i, j, count = 0;
	UINT8	testchannel ;
	UINT8	chanList[DFS_MAX_CHANNELS];
	UINT8	extChan = 0 ;
	UINT8	extTestChan = 0 ;
	UINT8	extChanOffset;
	UINT8	domainInd ;

	if( !dfsDesc_p )
	{
		return 0 ;
	}

	domainCode = domainGetDomain() ; // get current domain
	for( i = 0 ; i < sizeof( dfsEnabledChannels) ; i ++ )
	{
		if( domainCode == dfsEnabledChannels[i].domainCode)
			break ;
	}
	if( i == sizeof( dfsEnabledChannels) )
	{
#ifdef DEBUG_PRINT
		printk("Could not find the domain\n");
#endif
		return 0 ;
	}
	domainInd = i ;
	if( (dfsDesc_p->currChanInfo.chanflag.ChnlWidth == CH_40_MHz_WIDTH) ||
		(dfsDesc_p->currChanInfo.chanflag.ChnlWidth == CH_AUTO_WIDTH))
	{
		extChanOffset = dfsDesc_p->currChanInfo.chanflag.ExtChnlOffset ;
		extChan = DFSGetExtensionChannelOfDFSChan( domainInd, extChanOffset , 
			dfsDesc_p->currChanInfo.channel);
	}
	/* Now create a list of eligible channels */
	/* Current channel should be excluded. */
	/* If 40 MHz, extension channel of current channel should be excluded */
	/* If 40 MHz, extension channel of the NOL channels should be excluded */
	for( j = 0 ; j < DFS_MAX_CHANNELS ; j ++ )
	{
		if( dfsEnabledChannels[domainInd].dfschannelEntry[j] == 0 )
			continue ;
		if( dfsEnabledChannels[domainInd].dfschannelEntry[j] == dfsDesc_p->currChanInfo.channel )
			continue ;
		if( extChan && dfsEnabledChannels[domainInd].dfschannelEntry[j] == extChan )
			continue ;
		testchannel = dfsEnabledChannels[domainInd].dfschannelEntry[j] ;
		if( (dfsDesc_p->currChanInfo.chanflag.ChnlWidth == CH_40_MHz_WIDTH) ||
			(dfsDesc_p->currChanInfo.chanflag.ChnlWidth == CH_AUTO_WIDTH))
		{
			extChanOffset = macMgmtMlme_Get40MHzExtChannelOffset( testchannel );
			extTestChan = DFSGetExtensionChannelOfDFSChan( domainInd, extChanOffset , testchannel);
			/* 40 MHz mode, do not select channel 140 */
			if(  testchannel == 140 || extTestChan == 140 )
			{
				continue ;
			}
		}
		if( DfsFindInNOL(  &dfsDesc_p->NOCList, testchannel ) == DFS_FAILURE )
		{
			if( extTestChan == 0 )
				chanList[count ++] = testchannel ;
			else if( DfsFindInNOL(  &dfsDesc_p->NOCList, extTestChan ) == DFS_FAILURE )
				chanList[count ++] = testchannel ;
		}
	}
	if ( count == 0 )
	{
#ifdef DEBUG_PRINT
		printk("No eligible channel found. Going to FALLBACK channel\n");
#endif
		return DFS_FALLBACK_CHANNEL ;
	}
	// pick a random channel that does not exist in the NOC
	randInd = jiffies % count ;
	return chanList[randInd] ;
}

DFS_STATUS DfsFindInNOL( List *nocList_p, UINT8 channel )
{
	NOCListItem 	*ptr = NULL ;

	ptr = (NOCListItem *)nocList_p->head ;
	while ( ptr )
	{
		if( ptr->channel == channel )
			return DFS_SUCCESS ;
		ptr = ptr->nxt ;
	}
	return DFS_FAILURE ;
}

void DfsPrintNOLChannelDetails( DfsAp *me, char *NOLPrintStr_p, int maxLength)
{
	int len = 0 ;
	int copylen = 0 ;
	NOCListItem *nocListItem_p ;
	List		*list_p ;
	char	*buf = NULL ;

	if( !me || !NOLPrintStr_p || maxLength == 0 )
	{
		return ;
	}
	if (( buf = kmalloc( maxLength, GFP_KERNEL ) ) == NULL )
	{
		return ;
	}

	list_p = &me->dfsApDesc.NOCList;
	len += sprintf( buf+len, "***** NOC LIST *******\n");
	nocListItem_p = (NOCListItem *)list_p->head ;
	while( nocListItem_p )
	{
		len += sprintf( buf+len, "Channel:%d\t\tAge:%lu seconds\n", 
			nocListItem_p->channel, (jiffies - nocListItem_p->occurance)/100);
		nocListItem_p = nocListItem_p->nxt ;
	}
	copylen = len > maxLength ? maxLength : len ;
	strncpy( NOLPrintStr_p, buf, copylen );
	NOLPrintStr_p[copylen] = '\0';
	kfree(buf);

}

DFS_STATE DfsGetCurrentState(DfsAp *me)
{
#ifdef AP_MAC_LINUX
	struct net_device *dev;
#endif
	if( !me )
	{
		return DFS_STATE_UNKNOWN ;
	}
#ifdef AP_MAC_LINUX
	dev = me->pNetDev ;
#endif
	if ( !macMgmtMlme_DfsEnabled(dev) )
	{
		return DFS_STATE_UNKNOWN ;
	}
	return me->dfsApDesc.currState ;
}

DFS_STATUS DfsPresentInNOL( struct net_device *dev, UINT8 channel )
{
	struct wlprivate *priv = NETDEV_PRIV_P(struct wlprivate, dev);
	struct wlprivate_data *wlpd_p = priv->wlpd_p ;
	DfsAp *me = wlpd_p->pdfsApMain ;

	if( !me )
	{
		return DFS_FAILURE ;
	}
	if ( !macMgmtMlme_DfsEnabled(dev) )
	{
		return DFS_FAILURE ;
	}
	if((DfsGetCurrentState(me)) == DFS_STATE_CSA )
	{
		return DFS_SUCCESS ;
	}
	return (DfsFindInNOL( &me->dfsApDesc.NOCList, channel ) );
}

UINT16 DfsGetCACTimeOut( DfsAp *me )
{
	if( !me )
	{
		return 0 ;
	}
	return (me->dfsApDesc.CACTimeOut/10) ;
}

UINT16 DfsGetNOCTimeOut( DfsAp *me )
{
	if( !me )
	{
		return 0 ;
	}
	return (me->dfsApDesc.NOCTimeOut/10) ;
}

DFS_STATUS DfsSetCACTimeOut( DfsAp *me , UINT16 timeout)
{
	if( !me )
	{
		return DFS_FAILURE ;
	}
	me->dfsApDesc.CACTimeOut = timeout*10; //timeout in seconds
	return DFS_SUCCESS ;
}

DFS_STATUS DfsSetNOCTimeOut( DfsAp *me , UINT16 timeout)
{
	if( !me )
	{
		return DFS_FAILURE ;
	}
	me->dfsApDesc.NOCTimeOut = timeout*10; // timeout in seconds
	return DFS_SUCCESS ;
}

UINT8 DFSGetExtensionChannelOfDFSChan( UINT8 domain, UINT8 extChnlOffset , UINT8 channel)
{
	SINT8	extChanInd = 0 ;
	UINT8	extChan = 0 ;
	int		j ;

	if (extChnlOffset == EXT_CH_BELOW_CTRL_CH )
	{
		extChanInd = -1 ;
	}
	else
	{
		extChanInd = 1 ;
	}
	/* Find out the extension channel */
	for( j = 0 ; j < DFS_MAX_CHANNELS ; j ++ )
	{
		if( extChanInd && (j + extChanInd >= 0 ) && ((j+extChanInd) < DFS_MAX_CHANNELS))
		{
			if( dfsEnabledChannels[domain].dfschannelEntry[j] == channel )
			{
				extChan = dfsEnabledChannels[domain].dfschannelEntry[j+extChanInd]  ;
				return extChan ;
			}
		}
	}
	return 0 ;
}

UINT8 DFSGetCurrentRadarDetectionMode(DfsAp *me ,UINT8 channel, CHNL_FLAGS chanFlag )
{
	DfsChanInfo		chanInfo ;
	UINT8	action = 0 ;

	chanInfo.channel = channel ;
	chanInfo.chanflag = chanFlag ;

	if( !me )
	{
		return 0 ;
	}
	if( !DfsEnabledChannel(me->pNetDev, &chanInfo ) )
	{
		return 0 ;
	}
	if( (DfsGetCurrentState(me) == DFS_STATE_OPERATIONAL))
		action = 3 ; //Normal mode radar detection
	else if( (DfsGetCurrentState(me) == DFS_STATE_SCAN))
		action = 1 ; //CAC mode radar detection
	else
		action = 0 ;
	return action ;
}

void DFSApReset( DfsAp *me)
{
	NOCListItem *nocListItem_p = NULL ;
	DfsApDesc *dfsDesc_p = NULL ;

	if( !me )
	{
		return ;
	}
	dfsDesc_p = &me->dfsApDesc ;
	nocListItem_p = (NOCListItem *)ListGetItem( &dfsDesc_p->NOCList );

	if( (DfsGetCurrentState(me)) == DFS_STATE_SCAN)
	{
		/* Stops CAC timer */
		TimerRemove(&dfsDesc_p->CACTimer);
	}
	if( nocListItem_p == NULL )
	{
		memset (dfsDesc_p, 0, sizeof(DfsApDesc) );
		return ;
	}
	TimerRemove(&dfsDesc_p->NOCTimer);
	TimerRemove(&dfsDesc_p->CSATimer);
	kfree( nocListItem_p );
	while((nocListItem_p = (NOCListItem *)ListGetItem( &dfsDesc_p->NOCList ) ) != NULL )
	{
		kfree( nocListItem_p );
	}
	memset (dfsDesc_p, 0, sizeof(DfsApDesc) );
}
#endif
