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
* Description:  Implementation of the AP's MLME Authentication Services
*
*/

#include "mhsm.h"
#include "mlme.h"
#include "wltypes.h"
#include "timer.h"
#include "wldebug.h"
#include "wl_mib.h"
#include "wl_hal.h"

void StateMachineTimeoutHandler(void *data_p);

extern int wl_MacMlme_AuthReq(void *data_p );
extern int wl_MacMlme_AuthEven(vmacApInfo_t *vmacSta_p, void *data_p );
extern int wl_MacMlme_AuthSrvTimeout( void *data_p );
extern int wl_MacMlme_AuthOdd1(vmacApInfo_t *vmacSta_p,void *data_p );
extern int wl_MacMlme_DeAuth(vmacApInfo_t *vmacSta_p,void *data_p, UINT32 msgSize );
extern int wl_MacMlme_AuthOdd3(vmacApInfo_t *vmacSta_p,void *data_p );

/**************** Auth Request Service ***********************/

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
MhsmEvent_t const *AuthReqSrvAp_top(AuthReqSrvAp *me, MhsmEvent_t *msg)
{
#ifdef DEBUG_PRINT
	printf("AuthReqSrvAp_top:: Enter\n");
#endif
	switch ( msg->event )
	{
	case MHSM_ENTER:
		mhsm_transition(&me->super, &me->Auth_Req_Srv_Ap);
		return (0);

	}
	return (msg);
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
MhsmEvent_t const *Auth_Req_Srv_Ap_Handle(AuthReqSrvAp *me, MhsmEvent_t *msg)
{
#ifdef DEBUG_PRINT
	printf("Auth_Req_Srv_Ap_Handle:: Enter\n");
#endif
	switch ( msg->event )
	{
	case MHSM_ENTER:
		mhsm_transition(&me->super, &me->Auth_Req_Idle);
		return (0);

	}
	return (msg);
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
MhsmEvent_t const *Auth_Req_Idle_Handle(AuthReqSrvAp *me, MhsmEvent_t *msg)
{
#ifdef DEBUG_PRINT
	printf("Auth_Req_Idle_Handle:: Enter\n");
#endif
	switch ( msg->event )
	{
	case MHSM_ENTER:

		WLDBG_INFO(DBG_LEVEL_4, "Auth_Req_Idle_Handle:: Entry Event\n");

		return (0);

	case MlmeAuth_Req:

		WLDBG_INFO(DBG_LEVEL_4, "Auth_Req_Idle_Handle:: MlmeAuth_Req Event\n");

		wl_MacMlme_AuthReq(msg->pBody);
		mhsm_transition(&me->super, &me->Wait_Auth_Seq2);
		return (0);

	}
	return (msg);
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
MhsmEvent_t const *Wait_Auth_Seq2_Handle(AuthReqSrvAp *me, MhsmEvent_t *msg)
{
#ifdef DEBUG_PRINT
	printf("Wait_Auth_Seq2_Handle:: Enter\n");
#endif
	switch ( msg->event )
	{
	case MHSM_ENTER:
		return (0);

	case AuthEven:

		WLDBG_INFO(DBG_LEVEL_4, "Wait_Auth_Seq2_Handle AuthEven \n");

		TimerFireIn(&me->timer, 1, &StateMachineTimeoutHandler, (unsigned char *)me, AUTH_TIMEOUT);
		if ( wl_MacMlme_AuthEven((vmacApInfo_t *)msg->devinfo, msg->pBody) == MLME_INPROCESS )
		{
			mhsm_transition(&me->super, &me->Wait_Auth_Seq4);
		} else
		{
			TimerDisarm(&me->timer);
			mhsm_transition(&me->super, &me->Auth_Req_Idle);
		}
		return (0);
	}
	return (msg);
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
MhsmEvent_t const *Wait_Auth_Seq4_Handle(AuthReqSrvAp *me, MhsmEvent_t *msg)
{
#ifdef DEBUG_PRINT
	printf("Wait_Auth_Seq4_Handle:: Enter\n");
#endif
	switch ( msg->event )
	{
	case MHSM_ENTER:
		return (0);

	case AuthEven:
		wl_MacMlme_AuthEven((vmacApInfo_t *)msg->devinfo, msg->pBody);
		TimerDisarm(&me->timer);
		mhsm_transition(&me->super, &me->Auth_Req_Idle);
		return (0);
	}
	return (msg);
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
extern void AuthReqSrvApCtor(AuthReqSrvAp *me)
{
	mhsm_add(&me->sTop, NULL, (MhsmFcnPtr)AuthReqSrvAp_top);
	mhsm_add(&me->Auth_Req_Srv_Ap, 
		&me->sTop, (MhsmFcnPtr)Auth_Req_Srv_Ap_Handle);
	mhsm_add(&me->Auth_Req_Idle,  &me->Auth_Req_Srv_Ap,
		(MhsmFcnPtr)Auth_Req_Idle_Handle);
	mhsm_add(&me->Wait_Auth_Seq2,&me->Auth_Req_Srv_Ap,
		(MhsmFcnPtr)Wait_Auth_Seq2_Handle);
	mhsm_add(&me->Wait_Auth_Seq4, &me->Auth_Req_Srv_Ap,
		(MhsmFcnPtr)Wait_Auth_Seq4_Handle);
	TimerInit(&me->timer);
}

/**************** Auth Response Service ***********************/

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
MhsmEvent_t const *AuthRspSrvAp_top(AuthRspSrvAp *me, MhsmEvent_t *msg)
{
#ifdef DEBUG_PRINT
	printf("AuthRspSrvAp_top:: Enter\n");
#endif
	switch ( msg->event )
	{
	case MHSM_ENTER:
		mhsm_transition(&me->super, &me->Auth_Rsp_Srv_Ap);
		return (0);

	default:
		return (msg);
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
MhsmEvent_t const *Auth_Rsp_Srv_Ap_Handle(AuthRspSrvAp *me, MhsmEvent_t *msg)
{
#ifdef DEBUG_PRINT
	printf("Auth_Rsp_Srv_Ap_Handle:: Enter\n");
#endif
	switch ( msg->event )
	{
	case MHSM_ENTER:
		mhsm_transition(&me->super, &me->Auth_Rsp_Idle);
		return (0);

	default:
		return (msg);
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
MhsmEvent_t const *Auth_Rsp_Idle_Handle(AuthRspSrvAp *me, MhsmEvent_t *msg)
{
	struct sk_buff *skb = NULL;
	UINT32 msgSize      = 0;
#ifdef DEBUG_PRINT

	printf("Auth_Rsp_Idle_Handle:: Enter\n");
#endif

	if ( (me == NULL) || (msg == NULL) )
	{

		WLDBG_INFO(DBG_LEVEL_4, "Auth_Rsp_Idle_Handle:: error: NULL pointer\n");

		return (0);
	}

	switch ( msg->event )
	{
	case MHSM_ENTER:
		return (0);

	case Timeout:

		WLDBG_INFO(DBG_LEVEL_4, "Auth_Rsp_Idle_Handle:: event-> Timeout\n");

		wl_MacMlme_AuthSrvTimeout(me->userdata_p);
		return (0);

	case AuthOdd:

		WLDBG_INFO(DBG_LEVEL_4, "Auth_Rsp_Idle_Handle:: event-> AuthOdd aa\n");

		TimerFireIn(&me->timer, 1, &StateMachineTimeoutHandler, (unsigned char *)me, AUTH_TIMEOUT);
		if ( wl_MacMlme_AuthOdd1((vmacApInfo_t *)msg->devinfo, msg->pBody) == MLME_INPROCESS )
		{
			mhsm_transition(&me->super, &me->Wait_Chal_Rsp);
		} else
		{
			TimerDisarm(&me->timer);
		}
		return (0);

	case DeAuth:

		WLDBG_INFO(DBG_LEVEL_4, "Auth_Rsp_Idle_Handle:: event-> DeAuth\n");

		skb = (struct sk_buff *) msg->info;
		msgSize = skb->len;
		wl_MacMlme_DeAuth((vmacApInfo_t *)msg->devinfo, msg->pBody, msgSize);
		/* Stay in this state */
		return (0);

	}

	return (msg);
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
MhsmEvent_t const *Wait_Chal_Rsp_Handle(AuthRspSrvAp *me, MhsmEvent_t *msg)
{
	struct sk_buff *skb = NULL;
	UINT32 msgSize      = 0;
#ifdef DEBUG_PRINT
	printf("Wait_Chal_Rsp_Handle:: Enter\n");
#endif

	if ( (me == NULL) || (msg == NULL) )
	{

		WLDBG_INFO(DBG_LEVEL_4, "Wait_Chal_Rsp_Handle:: error: NULL pointer\n");

		return (0);
	}

	switch ( msg->event )
	{
	case MHSM_ENTER:
		return (0);

	case Timeout:

		WLDBG_INFO(DBG_LEVEL_4, "Wait_Chal_Rsp_Handle:: event-> Timeout\n");

		wl_MacMlme_AuthSrvTimeout(me->userdata_p);
		mhsm_transition(&me->super, &me->Auth_Rsp_Idle);
		return (0);

	case AuthOdd:

		WLDBG_INFO(DBG_LEVEL_4, "Wait_Chal_Rsp_Handle:: event-> AuthOdd\n");
		if ( wl_MacMlme_AuthOdd3((vmacApInfo_t *)msg->devinfo, msg->pBody)==MLME_SUCCESS )
		{
			TimerDisarm(&me->timer);
			mhsm_transition(&me->super, &me->Auth_Rsp_Idle);
			return (0);
		} else
		{
			TimerDisarm(&me->timer);
			mhsm_transition(&me->super, &me->Auth_Rsp_Idle);
			return (0);
		}
		break;

	case DeAuth:

		WLDBG_INFO(DBG_LEVEL_4, "Wait_Chal_Rsp_Handle:: event-> DeAuth\n");

		skb = (struct sk_buff *) msg->info;
		msgSize = skb->len;
		wl_MacMlme_DeAuth((vmacApInfo_t *)msg->devinfo, msg->pBody, msgSize);
		mhsm_transition(&me->super, &me->Auth_Rsp_Idle);
		return (0);

	}
	return (msg);
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
extern void AuthRspSrvApCtor(AuthRspSrvAp *me)
{
	mhsm_add(&me->sTop, NULL, (MhsmFcnPtr)AuthRspSrvAp_top);
	mhsm_add(&me->Auth_Rsp_Srv_Ap,  
		&me->sTop, (MhsmFcnPtr)Auth_Rsp_Srv_Ap_Handle);
	mhsm_add(&me->Auth_Rsp_Idle,  &me->Auth_Rsp_Srv_Ap,
		(MhsmFcnPtr)Auth_Rsp_Idle_Handle);
	mhsm_add(&me->Wait_Chal_Rsp, &me->Auth_Rsp_Srv_Ap,
		(MhsmFcnPtr)Wait_Chal_Rsp_Handle);
	TimerInit(&me->timer);
}

void StateMachineTimeoutHandler(void *data_p)
{
	MhsmEvent_t msg;
	msg.event = Timeout;
    WLDBG_INFO(DBG_LEVEL_4, "enter state machine timeout handler\n");

    mhsm_send_event((Mhsm_t *)data_p, &msg);
}

