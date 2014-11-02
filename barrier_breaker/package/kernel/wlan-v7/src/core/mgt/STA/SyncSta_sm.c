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


#include "mhsm.h"
#include "mlmeSta.h"
#include "wltypes.h"

#include "mlmeApi.h"


MhsmEvent_t const *Synchronization_top_Sta(SyncSrvSta *me, MhsmEvent_t *msg)
{
    #ifdef ETH_DEBUG
	printk("%s[%d] event =%x %d\n", __FUNCTION__, __LINE__, msg->event,msg->event);
    #endif /* ETH_DEBUG */
    switch (msg->event)
    {
    case MHSM_ENTER:
        mhsm_transition(&me->super, &me->Sync_Srv_Sta);
        return 0;
    default:
        return msg;
    }
}

MhsmEvent_t const *Synchronization_Sta_Handle(SyncSrvSta *me, MhsmEvent_t *msg)
{
    SyncSrvStaMsg *syncMsg_p;

    syncMsg_p = (SyncSrvStaMsg *)msg->pBody;

    #ifdef ETH_DEBUG
	printk("%s[%d] event =%x %d\n", __FUNCTION__, __LINE__, msg->event,msg->event);
    #endif /* ETH_DEBUG */
    switch (msg->event)
    {
    case MHSM_ENTER:
        mhsm_transition(&me->super, &me->No_Bss);
        return 0;
    case MlmeReset_Req:
        mhsm_transition(&me->super, &me->No_Bss);
        return 0;
    case PsmDone:
        return 0;
    default:
        return msg;
    }
}

MhsmEvent_t const *No_Bss_Handle_Sta(SyncSrvSta *me, MhsmEvent_t *msg)
{  
    SyncSrvStaMsg *syncMsg_p;
 
    syncMsg_p = (SyncSrvStaMsg *)msg->pBody;
     
    #ifdef ETH_DEBUG
	printk("%s[%d] event =%x %d\n", __FUNCTION__, __LINE__, msg->event,msg->event);
    #endif /* ETH_DEBUG */
    switch (msg->event)
    {
    case MHSM_ENTER:
        return 0;

    case MlmeScan_Req:
        #ifdef ETH_DEBUG
	printk("%s[%d] event =%x %d\n", __FUNCTION__, __LINE__, msg->event,msg->event);
        #endif /* ETH_DEBUG */
        syncSrvSta_ScanCmd((vmacStaInfo_t *)msg->info, (macMgmtQ_ScanCmd_t *)syncMsg_p->cmdMsg_p);
        mhsm_transition(&me->super, &me->Act_Listen);
        return 0;

    case MlmeStart_Req:
        #ifdef ETH_DEBUG
	printk("%s[%d]\n", __FUNCTION__, __LINE__);
        #endif /* ETH_DEBUG */
        wl_MacMlme_StartReq(msg->info, syncMsg_p->cmdMsg_p);
        if(syncMsg_p->opMode == infrastructure)
        {
            mhsm_transition(&me->super, &me->No_Bss);
        }
        else
        {
            mhsm_transition(&me->super, &me->IBss_Active);
        }       
        return 0;

    case MlmeJoin_Req:
        #ifdef ETH_DEBUG
	printk("%s[%d] event =%x %d\n", __FUNCTION__, __LINE__, msg->event,msg->event);
        #endif /* ETH_DEBUG */
        syncSrv_JoinCmd((vmacStaInfo_t *)msg->info, (IEEEtypes_JoinCmd_t *)syncMsg_p->cmdMsg_p);
        if(syncMsg_p->opMode == infrastructure)
        {
            mhsm_transition(&me->super, &me->Join_Wait_Beacon);
        }
        else
        {
	   	 mhsm_transition(&me->super, &me->IBss_Active);
        }     
        return 0;
    case MlmeReset_Req:
        syncSrv_ResetCmd((vmacStaInfo_t *)msg->info, (IEEEtypes_ResetCmd_t *)syncMsg_p->cmdMsg_p);
        mhsm_transition(&me->super, &me->No_Bss);
        return 0;
    case MlmeScan_Cnfm:
        #ifdef ETH_DEBUG
	printk("%s[%d]\n", __FUNCTION__, __LINE__);
        #endif /* ETH_DEBUG */
#if 0
        smeStateMgr_ScanCfrm(msg->info, (macmgmtQ_CmdRsp_t *)syncMsg_p->statMsg_p);
        mhsm_transition(&me->super, &me->No_Bss);
#else
	printk("WARNING: MlmeScan_Cnfm event should not occur!\n");
#endif
        return 0;
    case Tbcn:
        syncSrv_LinkLostHandler((vmacStaInfo_t *)msg->info);
        mhsm_transition(&me->super, &me->No_Bss);
        return 0;
    case ProbeRsp:
        syncSrv_PrbeRspNoBssHandler( (vmacStaInfo_t *)msg->info,
	   				    (dot11MgtFrame_t *)syncMsg_p->mgtFrame_p, 
	   					syncMsg_p->rfHdr_p);
        return 0;
    case MlmeJoin_Cnfm:
        smeStateMgr_JoinCfrm(msg->info, (macmgmtQ_CmdRsp_t *)syncMsg_p->statMsg_p);
        return 0;
    }
    return msg;
}

MhsmEvent_t const *Bss_Handle_Sta(SyncSrvSta *me, MhsmEvent_t *msg)
{
    SyncSrvStaMsg *syncMsg_p;

    syncMsg_p = (SyncSrvStaMsg *)msg->pBody;
    #ifdef ETH_DEBUG
	printk("%s[%d]\n", __FUNCTION__, __LINE__);
    #endif /* ETH_DEBUG */
    switch (msg->event)
    {
    case MHSM_ENTER:
        return 0;

    case MlmeScan_Req:
        #ifdef ETH_DEBUG
	printk("%s[%d]\n", __FUNCTION__, __LINE__);
        #endif /* ETH_DEBUG */
        syncSrvSta_ScanCmd((vmacStaInfo_t *)msg->info, (macMgmtQ_ScanCmd_t *)syncMsg_p->cmdMsg_p);
        mhsm_transition(&me->super, &me->Act_Listen);
        return 0;

    case MlmePwMgt_Req:
        /* to previous state */
        return 0;

    case Tbcn:
        #ifdef ETH_DEBUG
	printk("%s[%d]\n", __FUNCTION__, __LINE__);
        #endif /* ETH_DEBUG */
        syncSrv_LinkLostHandler((vmacStaInfo_t *)msg->info);
        mhsm_transition(&me->super, &me->No_Bss);
        return 0;

    case Cfend:
        /* to previous state */
        return 0;

    case Beacon:
        syncSrv_BncRecvAssociatedHandler((vmacStaInfo_t *)msg->info,
										 (dot11MgtFrame_t *)syncMsg_p->mgtFrame_p, 
                                            syncMsg_p->rfHdr_p);
        return 0;

    case Tmocp:
      mhsm_transition(&me->super, &me->Wait_Hop_Bss);
        return 0;

    case MlmeReset_Req:
        syncSrv_ResetCmd((vmacStaInfo_t *)msg->info, (IEEEtypes_ResetCmd_t *)syncMsg_p->cmdMsg_p);
        mhsm_transition(&me->super, &me->No_Bss);
        return 0;
    case ProbeRsp:
       syncSrv_ProbeRspRcvd((vmacStaInfo_t *)msg->info,
							(dot11MgtFrame_t *)syncMsg_p->mgtFrame_p, 
							syncMsg_p->rfHdr_p);
        return 0;

    case MlmeJoin_Req:
        #ifdef ETH_DEBUG
	printk("%s[%d]\n", __FUNCTION__, __LINE__);
        #endif /* ETH_DEBUG */
        syncSrv_JoinCmd((vmacStaInfo_t *)msg->info, (IEEEtypes_JoinCmd_t *)syncMsg_p->cmdMsg_p);
        if(syncMsg_p->opMode == infrastructure)
        {
            mhsm_transition(&me->super, &me->Join_Wait_Beacon);
        }
        else
        {
			mhsm_transition(&me->super, &me->IBss_Active);
        }     
        return 0;

    default:
        /* Stay in this state */
        return 0;
    }

    return msg;
}

MhsmEvent_t const *IBss_Active_Handle_Sta(SyncSrvSta *me, MhsmEvent_t *msg)
{
    SyncSrvStaMsg *syncMsg_p;

    syncMsg_p = (SyncSrvStaMsg *)msg->pBody;

    #ifdef ETH_DEBUG
	printk("%s[%d]\n", __FUNCTION__, __LINE__);
    #endif /* ETH_DEBUG */
    switch (msg->event)
    {
    case MHSM_ENTER:
        return 0;
    case MlmeScan_Req:
        wl_MacMlme_ScanReqSta(msg->info, syncMsg_p->cmdMsg_p);
        /* to previous state */
        /* mhsm_transition(&me->super, &me->No_Bss); */
        mhsm_transition(&me->super, &me->Act_Listen);
        /* mhsm_transition(&me->super, &me->Wait_Csw_done); */
        return 0;
    case MlmePwMgt_Req:
        /* to previous state */
        return 0;
    case MlmeJoin_Cnfm:
        smeStateMgr_JoinCfrm(msg->info, (macmgmtQ_CmdRsp_t *)syncMsg_p->statMsg_p);
        return 0;
    case Tbcn:
        #ifdef ETH_DEBUG
	printk("%s[%d]\n", __FUNCTION__, __LINE__);
        #endif /* ETH_DEBUG */
        syncSrv_LinkLostHandler((vmacStaInfo_t *)msg->info);
        mhsm_transition(&me->super, &me->No_Bss);
        return 0;
    case ATim:
        /* to previous state */
        return 0;
    case Beacon:
        syncSrv_BncRecvAssociatedHandler((vmacStaInfo_t *)msg->info,
										 (dot11MgtFrame_t *)syncMsg_p->mgtFrame_p, 
                                            syncMsg_p->rfHdr_p);
        /* to previous */
        return 0;
    case Tatim:
        /* to previous */
        return 0;
    case Tmocp:
      mhsm_transition(&me->super, &me->Wait_Hop_Bss);
        return 0;
    case MlmeReset_Req:
        syncSrv_ResetCmd((vmacStaInfo_t *)msg->info, (IEEEtypes_ResetCmd_t *)syncMsg_p->cmdMsg_p);
        mhsm_transition(&me->super, &me->No_Bss);
        return 0;
    case ProbeRsp:
       syncSrv_ProbeRspRcvd((vmacStaInfo_t *)msg->info,
							(dot11MgtFrame_t *)syncMsg_p->mgtFrame_p, 
							syncMsg_p->rfHdr_p);
        return 0;
    }

    return msg;
}

MhsmEvent_t const *IBss_Idle_Handle_Sta(SyncSrvSta *me, MhsmEvent_t *msg)
{
    SyncSrvStaMsg *syncMsg_p;

    syncMsg_p = (SyncSrvStaMsg *)msg->pBody;

    #ifdef ETH_DEBUG
	printk("%s[%d]\n", __FUNCTION__, __LINE__);
    #endif /* ETH_DEBUG */
    switch (msg->event)
    {
    case MHSM_ENTER:
        return 0;
    case MlmeScan_Req:
        wl_MacMlme_ScanReqSta(msg->info, syncMsg_p->cmdMsg_p);
        /* to previous state */
        /* mhsm_transition(&me->super, &me->No_Bss); */
        mhsm_transition(&me->super, &me->Act_Listen);
        /*mhsm_transition(&me->super, &me->Wait_Csw_done); */
        return 0;
    case MlmePwMgt_Req:
        /* to previous */
        return 0;
    case Tbcn:
        /* to previous */
        return 0;
    case ATim:
        /* to previous */
        return 0;
    case Beacon:
        /* to previous */
        return 0;
    case Tatim:
        /* to previous */
        return 0;
    case Tmocp:
      mhsm_transition(&me->super, &me->Wait_Hop_Bss);
        return 0;
    case MlmeReset_Req:
        syncSrv_ResetCmd((vmacStaInfo_t *)msg->info, (IEEEtypes_ResetCmd_t *)syncMsg_p->cmdMsg_p);
        mhsm_transition(&me->super, &me->No_Bss);
        return 0;
    }

    return msg;
}


MhsmEvent_t const *Act_Listen_Handle_Sta(SyncSrvSta *me, MhsmEvent_t *msg)
{
    SyncSrvStaMsg *syncMsg_p;

    syncMsg_p = (SyncSrvStaMsg *)msg->pBody;

    #ifdef ETH_DEBUG
	printk("%s[%d]\n", __FUNCTION__, __LINE__);
    #endif /* ETH_DEBUG */
    switch (msg->event)
    {
    case MHSM_ENTER:
        return 0;
	case Beacon:
        #ifdef ETH_DEBUG
	printk("%s[%d]\n", __FUNCTION__, __LINE__);
        #endif /* ETH_DEBUG */
        syncSrv_ScanFilter( (vmacStaInfo_t *)msg->info,
                            (dot11MgtFrame_t *)syncMsg_p->mgtFrame_p, 
						    syncMsg_p->rfHdr_p);
        return 0;
    case ProbeRsp:
       syncSrv_ScanFilter( (vmacStaInfo_t *)msg->info,
						    (dot11MgtFrame_t *)syncMsg_p->mgtFrame_p, 
							syncMsg_p->rfHdr_p);
        return 0;
    case Tscan:
        syncSrvSta_SetNextChannel((vmacStaInfo_t *)msg->info);
        return 0;
    case MlmeScan_Cnfm:
        #ifdef ETH_DEBUG
	printk("%s[%d]\n", __FUNCTION__, __LINE__);
        #endif /* ETH_DEBUG */
        smeStateMgr_ScanCfrm(msg->info, (macmgmtQ_CmdRsp_t *)syncMsg_p->statMsg_p);
        if(syncSrv_IsLinkConnected(msg->info))
		{
			if(syncSrv_IsIbssMode(msg->info))
			{
				mhsm_transition(&me->super, &me->IBss_Active);
			}
			else
			{
				mhsm_transition(&me->super, &me->Bss);
			}

		}
		else
		{
			mhsm_transition(&me->super, &me->No_Bss);
		}
        return 0;
    case Tbcn:
        #ifdef ETH_DEBUG
	printk("%s[%d]\n", __FUNCTION__, __LINE__);
        #endif /* ETH_DEBUG */
        syncSrv_LinkLostHandler((vmacStaInfo_t *)msg->info);
        mhsm_transition(&me->super, &me->No_Bss);
        return 0;
    case MlmeReset_Req:
        syncSrv_ResetCmd((vmacStaInfo_t *)msg->info, (IEEEtypes_ResetCmd_t *)syncMsg_p->cmdMsg_p);
        mhsm_transition(&me->super, &me->No_Bss);
        return 0;
    }

    return msg;
}


MhsmEvent_t const *Act_Recv_Handle_Sta(SyncSrvSta *me, MhsmEvent_t *msg)
{
    SyncSrvStaMsg *syncMsg_p;

    syncMsg_p = (SyncSrvStaMsg *)msg->pBody;

    #ifdef ETH_DEBUG
	printk("%s[%d]\n", __FUNCTION__, __LINE__);
    #endif /* ETH_DEBUG */
    switch (msg->event)
    {
    case MHSM_ENTER:
        return 0;
    case Beacon:
        /* to previous */
        return 0;
   case ProbeRsp:
      syncSrv_ProbeRspRcvd((vmacStaInfo_t *)msg->info,
						   (dot11MgtFrame_t *)syncMsg_p->mgtFrame_p, 
						   syncMsg_p->rfHdr_p);
        /* to previous */
        return 0;
    case Tscan:
      mhsm_transition(&me->super, &me->No_Bss);
      /* mhsm_transition(&me->super, &me->Pas_Listen); */
      /* mhsm_transition(&me->super, &me->Wait_Csw_done); */
        return 0;
    case MlmeReset_Req:
        syncSrv_ResetCmd((vmacStaInfo_t *)msg->info, (IEEEtypes_ResetCmd_t *)syncMsg_p->cmdMsg_p);
        mhsm_transition(&me->super, &me->No_Bss);
        return 0;
    }
    return msg;
}

MhsmEvent_t const *Pas_Listen_Handle_Sta(SyncSrvSta *me, MhsmEvent_t *msg)
{
    SyncSrvStaMsg *syncMsg_p;

    syncMsg_p = (SyncSrvStaMsg *)msg->pBody;
    #ifdef ETH_DEBUG
	printk("%s[%d]\n", __FUNCTION__, __LINE__);
    #endif /* ETH_DEBUG */
    switch (msg->event)
    {
    case MHSM_ENTER:
        return 0;
    case Beacon:
        /* to previous */
        return 0;
   case ProbeRsp:
      syncSrv_ProbeRspRcvd((vmacStaInfo_t *)msg->info,
						   (dot11MgtFrame_t *)syncMsg_p->mgtFrame_p, 
						   syncMsg_p->rfHdr_p);
        /* to previous */
        return 0;
    case Tscan:
        syncSrvSta_SetNextChannel((vmacStaInfo_t *)msg->info);
        return 0;
    case MlmeScan_Cnfm:
        smeStateMgr_ScanCfrm(msg->info, (macmgmtQ_CmdRsp_t *)syncMsg_p->statMsg_p);
        mhsm_transition(&me->super, &me->No_Bss);
        return 0;
    case MlmeReset_Req:
        syncSrv_ResetCmd((vmacStaInfo_t *)msg->info, (IEEEtypes_ResetCmd_t *)syncMsg_p->cmdMsg_p);
        mhsm_transition(&me->super, &me->No_Bss);
        return 0;
    }
    return msg;
}

MhsmEvent_t const *Wait_Csw_done_Handle_Sta(SyncSrvSta *me, MhsmEvent_t *msg)
{
    SyncSrvStaMsg *syncMsg_p;

    syncMsg_p = (SyncSrvStaMsg *)msg->pBody;
    #ifdef ETH_DEBUG
	printk("%s[%d]\n", __FUNCTION__, __LINE__);
    #endif /* ETH_DEBUG */
    switch (msg->event)
    {
    case MHSM_ENTER:
        return 0;
    case SwDone:
      mhsm_transition(&me->super, &me->Wait_Probe_Delay);
        return 0;
    case MlmeReset_Req:
        syncSrv_ResetCmd((vmacStaInfo_t *)msg->info, (IEEEtypes_ResetCmd_t *)syncMsg_p->cmdMsg_p);
        mhsm_transition(&me->super, &me->No_Bss);
        return 0;
    }
    return msg;
}

MhsmEvent_t const *Wait_Probe_Delay_Handle_Sta(SyncSrvSta *me, MhsmEvent_t *msg)
{
    SyncSrvStaMsg *syncMsg_p;

    syncMsg_p = (SyncSrvStaMsg *)msg->pBody;
    #ifdef ETH_DEBUG
	printk("%s[%d]\n", __FUNCTION__, __LINE__);
    #endif /* ETH_DEBUG */
    switch (msg->event)
    {
    case MHSM_ENTER:
        return 0;
    case Tscan:
      mhsm_transition(&me->super, &me->Act_Listen);
        return 0;
    case MlmeReset_Req:
        syncSrv_ResetCmd((vmacStaInfo_t *)msg->info, (IEEEtypes_ResetCmd_t *)syncMsg_p->cmdMsg_p);
        mhsm_transition(&me->super, &me->No_Bss);
        return 0;
    }
    return msg;
}

MhsmEvent_t const *Init_Wait_Probe_Delay_Handle_Sta(SyncSrvSta *me, MhsmEvent_t *msg)
{
    SyncSrvStaMsg *syncMsg_p;

    syncMsg_p = (SyncSrvStaMsg *)msg->pBody;
    #ifdef ETH_DEBUG
	printk("%s[%d]\n", __FUNCTION__, __LINE__);
    #endif /* ETH_DEBUG */
    switch (msg->event)
    {
    case MHSM_ENTER:
        return 0;
    case Tpdly:
      mhsm_transition(&me->super, &me->IBss_Active);
        return 0;
    case MlmeReset_Req:
        syncSrv_ResetCmd((vmacStaInfo_t *)msg->info, (IEEEtypes_ResetCmd_t *)syncMsg_p->cmdMsg_p);
        mhsm_transition(&me->super, &me->No_Bss);
        return 0;
    }
    return msg;
}

MhsmEvent_t const *Join_Wait_Beacon_Handle_Sta(SyncSrvSta *me, MhsmEvent_t *msg)
{
    SyncSrvStaMsg *syncMsg_p;

    syncMsg_p = (SyncSrvStaMsg *)msg->pBody;
    #ifdef ETH_DEBUG
	printk("%s[%d]\n", __FUNCTION__, __LINE__);
    #endif /* ETH_DEBUG */
    switch (msg->event)
    {
    case MHSM_ENTER:
        return 0;
    case Tbcn:
        syncSrv_LinkLostHandler((vmacStaInfo_t *)msg->info);
        mhsm_transition(&me->super, &me->No_Bss);
        return 0;

    case MlmeJoin_Cnfm:
        if(smeStateMgr_JoinCfrm(msg->info, (macmgmtQ_CmdRsp_t *)syncMsg_p->statMsg_p) == MLME_SUCCESS)
        {
            mhsm_transition(&me->super, &me->Bss);
        }
        else
        {
            mhsm_transition(&me->super, &me->No_Bss);
        }
        return 0;
    case MlmeReset_Req:
        syncSrv_ResetCmd((vmacStaInfo_t *)msg->info, (IEEEtypes_ResetCmd_t *)syncMsg_p->cmdMsg_p);
        mhsm_transition(&me->super, &me->No_Bss);
        return 0;
    }
    return msg;
}

MhsmEvent_t const *Wait_Hop_Bss_Handle_Sta(SyncSrvSta *me, MhsmEvent_t *msg)
{
    SyncSrvStaMsg *syncMsg_p;

    syncMsg_p = (SyncSrvStaMsg *)msg->pBody;
    #ifdef ETH_DEBUG
	printk("%s[%d]\n", __FUNCTION__, __LINE__);
    #endif /* ETH_DEBUG */
    switch (msg->event)
    {
    case MHSM_ENTER:
        return 0;
    case SwDone:
      mhsm_transition(&me->super, &me->Bss);
        return 0;
    case MlmeReset_Req:
        syncSrv_ResetCmd((vmacStaInfo_t *)msg->info, (IEEEtypes_ResetCmd_t *)syncMsg_p->cmdMsg_p);
        mhsm_transition(&me->super, &me->No_Bss);
        return 0;
    }
    return msg;
}

MhsmEvent_t const *STA_Active_Handle(SyncSrvSta *me, MhsmEvent_t *msg)
{
    SyncSrvStaMsg *syncMsg_p;

    syncMsg_p = (SyncSrvStaMsg *)msg->pBody;
    #ifdef ETH_DEBUG
	printk("%s[%d]\n", __FUNCTION__, __LINE__);
    #endif /* ETH_DEBUG */
    switch (msg->event)
    {
    case MHSM_ENTER:
        return 0;
    }
    return msg;
}

void SyncSrvCtorSta(SyncSrvSta *me)
{
    mhsm_add(&me->sTop, NULL, (MhsmFcnPtr)Synchronization_top_Sta);
    mhsm_add(&me->Sync_Srv_Sta,  
              &me->sTop, (MhsmFcnPtr)Synchronization_Sta_Handle);
    mhsm_add(&me->Act_Listen,&me->Sync_Srv_Sta,
              (MhsmFcnPtr)Act_Listen_Handle_Sta);
    mhsm_add(&me->Act_Recv, &me->Sync_Srv_Sta,
              (MhsmFcnPtr)Act_Recv_Handle_Sta);
    mhsm_add(&me->AP_Active,  &me->Sync_Srv_Sta,
              (MhsmFcnPtr)STA_Active_Handle);
    mhsm_add(&me->Bss,  &me->Sync_Srv_Sta,
              (MhsmFcnPtr)Bss_Handle_Sta);
    mhsm_add(&me->IBss_Active,  &me->Sync_Srv_Sta,
              (MhsmFcnPtr)IBss_Active_Handle_Sta);
    mhsm_add(&me->IBss_Idle,  &me->Sync_Srv_Sta,
              (MhsmFcnPtr)IBss_Idle_Handle_Sta);
    mhsm_add(&me->Init_Wait_Probe_Delay,  &me->Sync_Srv_Sta,
              (MhsmFcnPtr)Init_Wait_Probe_Delay_Handle_Sta);
    mhsm_add(&me->Join_Wait_Beacon, &me->Sync_Srv_Sta,
              (MhsmFcnPtr)Join_Wait_Beacon_Handle_Sta);
    mhsm_add(&me->No_Bss,  &me->Sync_Srv_Sta,
              (MhsmFcnPtr)No_Bss_Handle_Sta);
    mhsm_add(&me->Pas_Listen, &me->Sync_Srv_Sta,
              (MhsmFcnPtr)Pas_Listen_Handle_Sta);
    mhsm_add(&me->Wait_Csw_done, &me->Sync_Srv_Sta,
              (MhsmFcnPtr)Wait_Csw_done_Handle_Sta);
    mhsm_add(&me->Wait_Hop_Bss,  &me->Sync_Srv_Sta,
              (MhsmFcnPtr)Wait_Hop_Bss_Handle_Sta);
    mhsm_add(&me->Wait_Probe_Delay,  &me->Sync_Srv_Sta,
              (MhsmFcnPtr)Wait_Probe_Delay_Handle_Sta);
}
