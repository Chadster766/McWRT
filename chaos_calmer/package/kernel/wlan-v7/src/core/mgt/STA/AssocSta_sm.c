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

MhsmEvent_t const *AssocSrvSta_top(AssocSrvSta *me, MhsmEvent_t *msg)
{
    #ifdef ETH_DEBUG
    eprintf("AssocSrvSta_top:: Enter\n");
    #endif /* ETH_DEBUG */
    switch (msg->event)
    {
    case MHSM_ENTER:
        mhsm_transition(&me->super, &me->Assoc_Srv_Sta);
        return 0;
    default:
        return msg;
    }
}


MhsmEvent_t const *Assoc_Srv_Sta_Handle(AssocSrvSta *me, MhsmEvent_t *msg)
{
    #ifdef ETH_DEBUG
    eprintf("Assoc_Srv_Sta_Handle:: Enter\n"); 
    #endif /* ETH_DEBUG */
    switch (msg->event)
    {
    case MHSM_ENTER:
        mhsm_transition(&me->super, &me->Assoc_Idle);
        return 0;
    default:
        return msg;
    }
}

MhsmEvent_t const *Assoc_Sta_Idle_Handle(AssocSrvSta *me, MhsmEvent_t *msg)
{
    #ifdef ETH_DEBUG
    eprintf("Assoc_Sta_Idle_Handle:: Enter\n");
    #endif /* ETH_DEBUG */
    switch (msg->event)
    {
    case MHSM_ENTER:
        return 0;
    case Timeout:
        #ifdef ETH_DEBUG
        eprintf("Assoc_Sta_Idle_Handle:: event-> Timeout\n");
        #endif /* ETH_DEBUG */
        /* House cleaning */
        wl_MacMlme_AssocSrvStaTimeout(msg->info, msg->pBody);
        mhsm_transition(&me->super, &me->Assoc_Idle);
        return 0;
    case AssocReq:
        if (wl_MacMlme_AssocCmd(msg->info, msg->pBody) == MLME_SUCCESS)
        {
            mhsm_transition(&me->super, &me->Wait_Assoc_Rsp);
        }
        return 0;
    case ReAssocReq:
        wl_MacMlme_ReAssocCmd(msg->info, msg->pBody);
        mhsm_transition(&me->super, &me->Wait_ReAssoc_Rsp);
        return 0;

	case MlmeAssoc_Req:
		if (wl_MacMlme_AssocCmd(msg->info, msg->pBody) == MLME_SUCCESS)
        {
            mhsm_transition(&me->super, &me->Wait_Assoc_Rsp);
        }
        return 0;

	case MlmeReAssoc_Req:
        wl_MacMlme_ReAssocCmd(msg->info, msg->pBody);
        mhsm_transition(&me->super, &me->Wait_ReAssoc_Rsp);
        return 0;

    case DisAssoc:
        assocSrv_RecvDisAssocMsg((vmacStaInfo_t *)msg->info, 
								 (dot11MgtFrame_t *)msg->pBody);
        mhsm_transition(&me->super, &me->Assoc_Idle);
        return 0;
    }
    return msg;
}
MhsmEvent_t const *Wait_Assoc_Sta_Rsp_Handle(AssocSrvSta *me, MhsmEvent_t *msg)
{
    #ifdef ETH_DEBUG
    eprintf("Wait_Assoc_Sta_Rsp_Handle:: Enter\n");
    #endif /* ETH_DEBUG */
    switch (msg->event)
    {
    case MHSM_ENTER:
        return 0;
    case Timeout:
        #ifdef ETH_DEBUG
        eprintf("Wait_Assoc_Sta_Rsp_Handle:: event-> Timeout\n");
        #endif /* ETH_DEBUG */
        /* House cleaning */
        wl_MacMlme_AssocSrvStaTimeout(msg->info, msg->pBody);
        mhsm_transition(&me->super, &me->Assoc_Idle);
        return 0;
    case AssocRsp:
        if (wl_MacMlme_AssocRsp(msg->info, msg->pBody) == MLME_SUCCESS)
        {
            mhsm_transition(&me->super, &me->Assoc_Idle);
        }
        return 0;

    case DisAssoc:
        assocSrv_RecvDisAssocMsg((vmacStaInfo_t *)msg->info, 
								 (dot11MgtFrame_t *)msg->pBody);
        mhsm_transition(&me->super, &me->Assoc_Idle);
        return 0;
    }
    return msg;
}
MhsmEvent_t const *Wait_ReAssoc_Sta_Rsp_Handle(AssocSrvSta *me, MhsmEvent_t *msg)
{
    #ifdef ETH_DEBUG
    eprintf("Wait_ReAssoc_Sta_Rsp_Handle:: Enter\n");
    #endif /* ETH_DEBUG */
    switch (msg->event)
    {
    case MHSM_ENTER:
        return 0;
    case Timeout:
        #ifdef ETH_DEBUG
        eprintf("Wait_ReAssoc_Sta_Rsp_Handle:: event-> Timeout\n");
        #endif /* ETH_DEBUG */
        /* House cleaning */
        wl_MacMlme_AssocSrvStaTimeout(msg->info, msg->pBody);
        mhsm_transition(&me->super, &me->Assoc_Idle);
        return 0;
    case ReAssocRsp:
        wl_MacMlme_ReAssocRsp(msg->info, msg->pBody);
        mhsm_transition(&me->super, &me->Assoc_Idle);
        return 0;

    case DisAssoc:
        assocSrv_RecvDisAssocMsg((vmacStaInfo_t *)msg->info, 
								 (dot11MgtFrame_t *)msg->pBody);
        mhsm_transition(&me->super, &me->Assoc_Idle);
        return 0;
    }
    return msg;
}

void AssocSrvStaCtor(AssocSrvSta *me)
{
    mhsm_add(&me->sTop, NULL, (MhsmFcnPtr)AssocSrvSta_top);
    mhsm_add(&me->Assoc_Srv_Sta,  
              &me->sTop, (MhsmFcnPtr)Assoc_Srv_Sta_Handle);
    mhsm_add(&me->Assoc_Idle,  &me->Assoc_Srv_Sta,
              (MhsmFcnPtr)Assoc_Sta_Idle_Handle);
    mhsm_add(&me->Wait_Assoc_Rsp, &me->Assoc_Srv_Sta,
              (MhsmFcnPtr)Wait_Assoc_Sta_Rsp_Handle);
    mhsm_add(&me->Wait_ReAssoc_Rsp,  &me->Assoc_Srv_Sta,
              (MhsmFcnPtr)Wait_ReAssoc_Sta_Rsp_Handle);
}

