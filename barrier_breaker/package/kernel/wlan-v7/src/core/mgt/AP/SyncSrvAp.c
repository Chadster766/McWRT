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
* Description:  Implementation of the AP's MLME Synchronization Services
*
*/

#include "mhsm.h"
#include "mlme.h"
#include "wltypes.h"
#include "IEEE_types.h"
#include "mib.h"
#include "wltypes.h"
#include "IEEE_types.h"
#include "wl_hal.h"

extern void StateMachineTimeoutHandler(void *data_p);
extern int wl_MacMlme_ChannelswitchReq(vmacApInfo_t *vmacSta_p,void *data_p);
extern int wl_MacMlme_MRequestReq(vmacApInfo_t *vmacSta_p, void *data_p );
extern int wl_MacMlme_ScanReq(vmacApInfo_t *vmacSta_p, void *data_p );
extern int wl_MacMlme_ProbeReq(vmacApInfo_t *vmacSta_p, void *data_p );
extern int wl_MacMlme_StartCmd(vmacApInfo_t *vmacSta_p, void *data_p );
extern int wl_MacMlme_StartCmd(vmacApInfo_t *vmacSta_p, void *data_p );
extern int wl_MacMlme_SyncSrvTimeout( void *data_p );
extern int wl_MacMlme_MReportReq(vmacApInfo_t *vmacSta_p, void *data_p );
extern int wl_MacMlme_ResetCmd(vmacApInfo_t *vmacSta_p, void *data_p );
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
MhsmEvent_t const *SyncSrvAp_top(SyncSrvAp *me, MhsmEvent_t *msg)
{
#ifdef DEBUG_PRINT
	printf("SyncSrvAp_top:: Enter\n");
#endif
	if((me == NULL) || (msg == NULL))
	{
#ifdef DEBUG_PRINT
		printf("SyncSrvAp_top:: error: NULL pointer\n");
#endif
		return 0;
	}

	switch (msg->event)
	{
	case MHSM_ENTER:
		mhsm_transition(&me->super, &me->Sync_Srv_Ap);
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
MhsmEvent_t const *Sync_Srv_Ap_Handle(SyncSrvAp *me, MhsmEvent_t *msg)
{
#ifdef DEBUG_PRINT
	printf("Sync_Srv_Ap_Handle:: Enter\n");
#endif
	if((me == NULL) || (msg == NULL))
	{
#ifdef DEBUG_PRINT
		printf("Sync_Srv_Ap_Handle:: error: NULL pointer\n");
#endif
		return 0;
	}

	switch (msg->event)
	{
	case MHSM_ENTER:
		mhsm_transition(&me->super, &me->No_Bss);
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
MhsmEvent_t const *No_Bss_Handle(SyncSrvAp *me, MhsmEvent_t *msg)
{
	static int startupdone = 0;
#ifdef DEBUG_PRINT
	printf("No_Bss_Handle:: Enter\n");
#endif
	if((me == NULL) || (msg == NULL))
	{
#ifdef DEBUG
		printf("No_Bss_Handle:: error: NULL pointer\n");
#endif
		return 0;
	}

	switch (msg->event)
	{
	case MHSM_ENTER:
		return 0;

	case Timeout:
#ifdef DEBUG_PRINT
		printf("No_Bss_Handle:: event-> Timeout\n");
#endif
		/* House cleaning */
		wl_MacMlme_SyncSrvTimeout(msg->pBody);
		return 0;

	case ResetMAC:
#ifdef DEBUG_PRINT
		printf("No_Bss_Handle:: event-> ResetMAC\n");
#endif
		if(!startupdone )
		{
			startupdone = 1;
			wl_MacMlme_ResetCmd((vmacApInfo_t *)msg->devinfo,msg->pBody);
		}
		/* Stay in this state */
		return 0;

	case MlmeStart_Req:
#ifdef DEBUG_PRINT
		printf("No_Bss_Handle:: event-> MlmeStart_Req\n");
#endif
		wl_MacMlme_StartCmd((vmacApInfo_t *)msg->devinfo, msg->pBody);
		mhsm_transition(&me->super, &me->Bss);
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
MhsmEvent_t const *Bss_Handle(SyncSrvAp *me, MhsmEvent_t *msg)
{
	/*    SyncSrvApMsg *syncMsg; */

#ifdef DEBUG_PRINT
	printf("Bss_Handle:: Enter\n");
#endif
	if((me == NULL) || (msg == NULL))
	{
#ifdef DEBUG_PRINT
		printf("Bss_Handle:: error: NULL pointer\n");
#endif
		return 0;
	}
	/*    syncMsg = (SyncSrvApMsg *)msg->pBody; */

	switch (msg->event)
	{
	case MHSM_ENTER:
		return 0;

	case ProbeReq:
#ifdef DEBUG_PRINT
		printf("Bss_Handle:: event-> ProbeReq\n");
#endif
		wl_MacMlme_ProbeReq((vmacApInfo_t *)msg->devinfo, msg->pBody);
		return 0;
	case ResetMAC:
#ifdef DEBUG_PRINT
		printf("Bss_Handle:: event-> ResetMAC\n");
#endif
		wl_MacMlme_ResetCmd((vmacApInfo_t *)msg->devinfo,msg->pBody);
		mhsm_transition(&me->super, &me->No_Bss);
		return 0;
#if defined(AP_SITE_SURVEY) || defined(AUTOCHANNEL)
	case MlmeScan_Req:
#ifdef DEBUG_PRINT
		printf("Bss_Handle:: event-> MlmeScan_Req\n");
#endif
		wl_MacMlme_ScanReq((vmacApInfo_t *)msg->devinfo, msg->pBody);
		return 0;
#endif /* AP_SITE_SURVEY */        
#ifdef AP_URPTR
	case MlmeStart_Req:
#ifdef DEBUG_PRINT        
		printf("No_Bss_Handle:: event-> MlmeStart_Req\n");
#endif
		wl_MacMlme_StartCmd((vmacApInfo_t *)msg->devinfo, msg->pBody);
		mhsm_transition(&me->super, &me->Bss);
		return 0;
#endif /* AP_URPTR */
#ifdef IEEE80211H
	case MlmeMrequest_Req:
#ifdef DEBUG_PRINT
		printf("Bss_Handle:: event-> MlmeMrequest_Req\n");
#endif /* DEBUG_PRINT */
		wl_MacMlme_MRequestReq((vmacApInfo_t *)msg->devinfo, msg->pBody);
		return 0;

	case MlmeMreport_Req:
#ifdef DEBUG_PRINT
		printf("Bss_Handle:: event-> MlmeMrequest_Req\n");
#endif /* DEBUG_PRINT */
		wl_MacMlme_MReportReq((vmacApInfo_t *)msg->devinfo, msg->pBody);
		return 0;

	case MlmeChannelSwitch_Req:
#ifdef DEBUG_PRINT
		printf("Bss_Handle:: event-> MlmeChannelSwitch_Req\n");
#endif /* DEBUG_PRINT */
		wl_MacMlme_ChannelswitchReq((vmacApInfo_t *)msg->devinfo, msg->pBody);
		return 0;        

#endif /* IEEE80211H */   

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
MhsmEvent_t const *Sta_Active_Handle(SyncSrvAp *me, MhsmEvent_t *msg)
{
	/*    SyncSrvApMsg *syncMsg; */
#ifdef DEBUG_PRINT
	printf("Sta_Active_Handle:: Enter\n");
#endif
	if((me == NULL) || (msg == NULL))
	{
#ifdef DEBUG_PRINT
		printf("Bss_Handle:: error: NULL pointer\n");
#endif
		return 0;
	}
	/*    syncMsg = (SyncSrvApMsg *)msg->pBody; */

	switch (msg->event)
	{
	case MHSM_ENTER:
		return 0;

	case Timeout:
#ifdef DEBUG_PRINT
		printf("Sta_Active_Handle:: event-> Timeout\n");
#endif
		/* House cleaning */
		wl_MacMlme_SyncSrvTimeout(msg->pBody);
		mhsm_transition(&me->super, &me->No_Bss);
		return 0;

	case ResetMAC:
#ifdef DEBUG_PRINT
		printf("Sta_Active_Handle:: event-> ResetMAC\n");
#endif
		wl_MacMlme_ResetCmd((vmacApInfo_t *)msg->devinfo,msg->pBody);
		mhsm_transition(&me->super, &me->No_Bss);
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
void SyncSrvCtor(SyncSrvAp *me)
{
	mhsm_add(&me->sTop, NULL, (MhsmFcnPtr)SyncSrvAp_top);
	mhsm_add(&me->Sync_Srv_Ap, 
		&me->sTop, (MhsmFcnPtr)Sync_Srv_Ap_Handle);
	mhsm_add(&me->No_Bss, &me->Sync_Srv_Ap,
		(MhsmFcnPtr)No_Bss_Handle);
	mhsm_add(&me->Bss,  &me->Sync_Srv_Ap,
		(MhsmFcnPtr)Bss_Handle);
	mhsm_add(&me->Sta_Active,  &me->Sync_Srv_Ap,
		(MhsmFcnPtr)Sta_Active_Handle);
}
