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
* Description:  Implementation of the AP's MLME Association Services
*
*/

#include "mhsm.h"
#include "mlme.h"
#include "wltypes.h"
#include "IEEE_types.h"
#include "timer.h"
#include "wl_mib.h"
#include "wl_hal.h"

extern void StateMachineTimeoutHandler(void *data_p);
extern int wl_MacMlme_AssocSrvTimeout( void *data_p );
extern int wl_MacMlme_AssocReq(vmacApInfo_t *vmacSta_p, void *data_p, UINT32 msgSize );
extern int wl_MacMlme_ReAssocReq(vmacApInfo_t *vmacSta_p, void *data_p, UINT32 msgSize );
extern int wl_MacMlme_DisAssocReq(vmacApInfo_t *vmacSta_p, void *data_p, UINT32 msgSize );
extern int wl_MacMlme_DisAssocCmd(vmacApInfo_t *vmacSta_p, void *data_p );
extern int wl_MacMlme_DsResponse(void *data_p );

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
MhsmEvent_t const *AssocSrvAp_top(AssocSrvAp *me, MhsmEvent_t *msg)
{
#ifdef DEBUG_PRINT
	printf("AssocSrvAp_top:: Enter\n");
#endif
	switch (msg->event)
	{
	case MHSM_ENTER:
		mhsm_transition(&me->super, &me->Assoc_Srv_Ap);
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
MhsmEvent_t const *Assoc_Srv_Ap_Handle(AssocSrvAp *me, MhsmEvent_t *msg)
{
#ifdef DEBUG_PRINT
	printf("Assoc_Srv_Ap_Handle:: Enter\n");
#endif
	switch (msg->event)
	{
	case MHSM_ENTER:
		mhsm_transition(&me->super, &me->Assoc_Idle);
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
MhsmEvent_t const *Assoc_Idle_Handle(AssocSrvAp *me, MhsmEvent_t *msg)
{
	struct sk_buff *skb = NULL;
	UINT32 msgSize      = 0;
#ifdef DEBUG_PRINT
	printf("Assoc_Idle_Handle:: Enter\n");
#endif
	switch (msg->event)
	{
	case MHSM_ENTER:
		return 0;

	case Timeout:
#ifdef DEBUG_PRINT
		printf("Assoc_Idle_Handle:: event-> Timeout\n");
#endif
		wl_MacMlme_AssocSrvTimeout(msg->pBody);
		/* House cleaning */
		return 0;

	case AssocReq:
#ifdef DEBUG_PRINT
		printf("Assoc_Idle_Handle:: event-> AssocReq\n");
#endif
		skb = (struct sk_buff *) msg->info;
		msgSize = skb->len;
		wl_MacMlme_AssocReq((vmacApInfo_t *)msg->devinfo, msg->pBody, msgSize);
		//currently, there is no confirmation for assoc rsp, no need to goto wait state, stay in this state
		//TimerFireIn(&me->timer, 1, &StateMachineTimeoutHandler, (unsigned char *)me, ASSOC_TIMEOUT);
		//mhsm_transition(&me->super, &me->Wait_Assoc_Rsp);
		return 0;

	case ReAssocReq:
#ifdef DEBUG_PRINT
		printf("Assoc_Idle_Handle:: event-> ReAssocReq\n");
#endif
		skb = (struct sk_buff *) msg->info;
		msgSize = skb->len;
		wl_MacMlme_ReAssocReq((vmacApInfo_t *)msg->devinfo,msg->pBody, msgSize);
		//TimerFireIn(&me->timer, 1, &StateMachineTimeoutHandler, (unsigned char *)me, ASSOC_TIMEOUT);
		//mhsm_transition(&me->super, &me->Wait_ReAssoc_Rsp);
		return 0;

	case DisAssoc:
#ifdef DEBUG_PRINT
		printf("Assoc_Idle_Handle:: event-> DisAssoc\n");
#endif
		skb = (struct sk_buff *) msg->info;
		msgSize = skb->len;
		wl_MacMlme_DisAssocReq((vmacApInfo_t *)msg->devinfo, msg->pBody, msgSize);
		/* Stay in this state */
		return 0;

	case MlmeDisAssoc_Req:
#ifdef DEBUG_PRINT
		printf("Assoc_Idle_Handle:: event-> DisAssoc\n");
#endif
		wl_MacMlme_DisAssocCmd((vmacApInfo_t *)msg->devinfo, msg->pBody);
		/* Stay in this state */
		return 0;
	default:
		break;
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
MhsmEvent_t const *Wait_Assoc_Rsp_Handle(AssocSrvAp *me, MhsmEvent_t *msg)
{
	struct sk_buff *skb = NULL;
	UINT32 msgSize      = 0;
#ifdef DEBUG_PRINT
	printf("Wait_Assoc_Rsp_Handle:: Enter\n");
#endif
	switch (msg->event)
	{
	case MHSM_ENTER:
		return 0;

	case Timeout:
#ifdef DEBUG_PRINT
		printf("Wait_Assoc_Rsp_Handle:: event-> Timeout\n");
#endif
		/* House cleaning */
		wl_MacMlme_AssocSrvTimeout(msg->pBody);
		mhsm_transition(&me->super, &me->Assoc_Idle);
		return 0;
	case AssocReq:
		skb = (struct sk_buff *) msg->info;
		msgSize = skb->len;
		wl_MacMlme_AssocReq((vmacApInfo_t *)msg->devinfo, msg->pBody, msgSize);
		mhsm_transition(&me->super, &me->Assoc_Idle);
		return 0;

	case ReAssocReq:
		skb = (struct sk_buff *) msg->info;
		msgSize = skb->len;
		wl_MacMlme_ReAssocReq((vmacApInfo_t *)msg->devinfo,msg->pBody, msgSize);
		mhsm_transition(&me->super, &me->Assoc_Idle);
		return 0;

	case DsResponse:
#ifdef DEBUG_PRINT
		printf("Wait_Assoc_Rsp_Handle:: event-> DsResponse\n");
#endif
		wl_MacMlme_DsResponse(msg->pBody);
		mhsm_transition(&me->super, &me->Assoc_Idle);
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
MhsmEvent_t const *Wait_ReAssoc_Rsp_Handle(AssocSrvAp *me, MhsmEvent_t *msg)
{
	struct sk_buff *skb = NULL;
	UINT32 msgSize      = 0;
#ifdef DEBUG_PRINT
	printf("Wait_ReAssoc_Rsp_Handle:: Enter\n");
#endif
	switch (msg->event)
	{
	case MHSM_ENTER:
		return 0;

	case Timeout:
#ifdef DEBUG_PRINT
		printf("Wait_ReAssoc_Rsp_Handle:: event-> Timeout\n");
#endif
		/* House cleaning */
		wl_MacMlme_AssocSrvTimeout(msg->pBody);
		mhsm_transition(&me->super, &me->Assoc_Idle);
		return 0;
	case AssocReq:
		skb = (struct sk_buff *) msg->info;
		msgSize = skb->len;
		wl_MacMlme_AssocReq((vmacApInfo_t *)msg->devinfo, msg->pBody, msgSize);
		mhsm_transition(&me->super, &me->Assoc_Idle);
		return 0;

	case ReAssocReq:
		skb = (struct sk_buff *) msg->info;
		msgSize = skb->len;
		wl_MacMlme_ReAssocReq((vmacApInfo_t *)msg->devinfo,msg->pBody, msgSize);
		mhsm_transition(&me->super, &me->Assoc_Idle);
		return 0;


	case DsResponse:
#ifdef DEBUG_PRINT
		printf("Wait_ReAssoc_Rsp_Handle:: event-> DsResponse\n");
#endif
		wl_MacMlme_DsResponse(msg->pBody);
		mhsm_transition(&me->super, &me->Assoc_Idle);
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
void AssocSrvApCtor(AssocSrvAp *me)
{
	mhsm_add(&me->sTop, NULL, (MhsmFcnPtr)AssocSrvAp_top);
	mhsm_add(&me->Assoc_Srv_Ap, 
		&me->sTop, (MhsmFcnPtr)Assoc_Srv_Ap_Handle);
	mhsm_add(&me->Assoc_Idle, &me->Assoc_Srv_Ap,
		(MhsmFcnPtr)Assoc_Idle_Handle);
	mhsm_add(&me->Wait_Assoc_Rsp,  &me->Assoc_Srv_Ap,
		(MhsmFcnPtr)Wait_Assoc_Rsp_Handle);
	mhsm_add(&me->Wait_ReAssoc_Rsp,  &me->Assoc_Srv_Ap,
		(MhsmFcnPtr)Wait_ReAssoc_Rsp_Handle);
}
