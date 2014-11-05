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


/**************** Auth Request Service ***********************/

MhsmEvent_t const *AuthReqSrvSta_top(AuthReqSrvSta *me, MhsmEvent_t *msg) 
{
    #ifdef ETH_DEBUG
	eprintf("AuthReqSrvSta_top:: Enter\n");
    #endif /* ETH_DEBUG */
    switch (msg->event) {
	case MHSM_ENTER:
		mhsm_transition(&me->super, &me->Auth_Req_Srv_Sta);
		return 0;
	
	}
	return msg;
}

MhsmEvent_t const *Auth_Req_Srv_Sta_Handle(AuthReqSrvSta *me, MhsmEvent_t *msg)
{
    #ifdef ETH_DEBUG
	eprintf("Auth_Req_Srv_Sta_Handle:: Enter\n");
    #endif /* ETH_DEBUG */
    switch (msg->event) {
	case MHSM_ENTER:
		mhsm_transition(&me->super, &me->Auth_Req_Idle);
		return 0;
	
	}
	return msg;
}
MhsmEvent_t const *Auth_Sta_Req_Idle_Handle(AuthReqSrvSta *me, MhsmEvent_t *msg)
{
    #ifdef ETH_DEBUG
	eprintf("Auth_Sta_Req_Idle_Handle:: Enter\n");
    #endif /* ETH_DEBUG */
    switch (msg->event) {
	case MHSM_ENTER:
        #ifdef ETH_DEBUG
        eprintf("Auth_Sta_Req_Idle_Handle:: Entry Event\n");
        #endif /* ETH_DEBUG */
        return 0;
    case MlmeAuth_Req:
        if (wl_MacMlme_AuthReqCmd(msg->info, msg->pBody)==MLME_INPROCESS)
        {
            #ifdef ETH_DEBUG
            eprintf("Auth_Sta_Req_Idle_Handle:: MlmeAuth_Req Event\n");
            #endif /* ETH_DEBUG */
            mhsm_transition(&me->super, &me->Wait_Auth_Seq2);
        }
		return 0;

	case MlmeDeAuth_Req:
		/* handler */
		return 0;

	case Cls2err:
		/* handler */
		return 0;

	case DeAuth:/* added to handle deauth; need to check with bt */
		authSrv_RecvMsgDeAuth((vmacStaInfo_t *)msg->info,
							  (dot11MgtFrame_t *)msg->pBody);
		/* Stay in this idle state */
		return 0;

    //case AuthEven:
    //    wl_MacMlme_AuthStaEven(msg->info, msg->pBody);

	}
	return msg;
}


MhsmEvent_t const *Wait_Auth_Sta_Seq2_Handle(AuthReqSrvSta *me, MhsmEvent_t *msg)
{
    #ifdef ETH_DEBUG
	eprintf("Wait_Auth_Sta_Seq2_Handle:: Enter\n");
    #endif /* ETH_DEBUG */
    switch (msg->event) {
	case MHSM_ENTER:
		return 0;
    case AuthEven:
		if (wl_MacMlme_AuthStaEven(msg->info, msg->pBody) == MLME_INPROCESS)
		{
			mhsm_transition(&me->super, &me->Wait_Auth_Seq4);
		}
		else
		{
			mhsm_transition(&me->super, &me->Auth_Req_Idle);
		}
		return 0;

	case Timeout:
		wl_MacMlme_AuthSrvStaTimeout(msg->info, msg->pBody);
		mhsm_transition(&me->super, &me->Auth_Req_Idle);
		return 0;
	}
	return msg;
}
MhsmEvent_t const *Wait_Auth_Sta_Seq4_Handle(AuthReqSrvSta *me, MhsmEvent_t *msg)
{
    #ifdef ETH_DEBUG
	eprintf("Wait_Auth_Sta_Seq4_Handle:: Enter\n");
    #endif /* ETH_DEBUG */
    switch (msg->event) {
	case MHSM_ENTER:
		return 0;
    case AuthEven:
        wl_MacMlme_AuthStaEven(msg->info, msg->pBody);
		mhsm_transition(&me->super, &me->Auth_Req_Idle);
		return 0;
	case Timeout:
		wl_MacMlme_AuthSrvStaTimeout(msg->info, msg->pBody);
		mhsm_transition(&me->super, &me->Auth_Req_Idle);
		return 0;
	}
	return msg;
}

extern void AuthReqSrvStaCtor(AuthReqSrvSta *me)
{
    mhsm_add(&me->sTop, NULL, (MhsmFcnPtr)AuthReqSrvSta_top);
    mhsm_add(&me->Auth_Req_Srv_Sta,  
              &me->sTop, (MhsmFcnPtr)Auth_Req_Srv_Sta_Handle);
	mhsm_add(&me->Auth_Req_Idle,  &me->Auth_Req_Srv_Sta,
              (MhsmFcnPtr)Auth_Sta_Req_Idle_Handle);
	mhsm_add(&me->Wait_Auth_Seq2,  &me->Auth_Req_Srv_Sta,
              (MhsmFcnPtr)Wait_Auth_Sta_Seq2_Handle);
	mhsm_add(&me->Wait_Auth_Seq4,  &me->Auth_Req_Srv_Sta,
              (MhsmFcnPtr)Wait_Auth_Sta_Seq4_Handle);
}
 
