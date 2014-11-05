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


/*!
* \file    wlMlmeSrv.c
* \brief   the implementation of state machine service routines
*/

#include "mlme.h"
#include "IEEE_types.h"
#include "mib.h"
#include "ds.h"
#include "osif.h"
#include "keyMgmtCommon.h"
#include "keyMgmt.h"

#include "wldebug.h"
#include "tkip.h"
#include "StaDb.h"
#include "macmgmtap.h"
#include "qos.h"
#include "macMgmtMlme.h"


extern SINT32 mlmeAuthDoOpenSys(vmacApInfo_t *vmacSta_p, AuthRspSrvApMsg *authRspMsg_p);
extern SINT32 mlmeAuthDoSharedKeySeq1(vmacApInfo_t *vmacSta_p, AuthRspSrvApMsg *authRspMsg_p);
extern SINT32 mlmeAuthDoSharedKeySeq3(vmacApInfo_t *vmacSta_p, AuthRspSrvApMsg *authRspMsg_p);
extern void macMgmtMlme_ChannelSwitchReq(vmacApInfo_t *vmacSta_p,IEEEtypes_ChannelSwitchCmd_t *ChannelSwitchtCmd_p);
extern void macMgmtMlme_MReportReq(vmacApInfo_t *vmacSta_p,IEEEtypes_MReportCmd_t *MreportCmd_p);
extern void macMgmtMlme_MRequestReq(vmacApInfo_t *vmacSta_p,IEEEtypes_MRequestCmd_t *MrequestCmd_p);
extern void syncSrv_ScanCmd(vmacApInfo_t *vmacSta_p, IEEEtypes_ScanCmd_t *ScanCmd_p );
extern void mlmeAuthError(vmacApInfo_t *vmacSta_p,IEEEtypes_StatusCode_t statusCode, UINT16 arAlg_in, UINT8 *Addr);
/*!
* association serveice timeout handler 
*  
* @param data_p Pointer to user defined data
* @return MLME_SUCCESS, MLME_INPROCESS, MLME_FAIL
*/
int wl_MacMlme_AssocSrvTimeout( void *data_p )
{
	/*    extStaDb_StaInfo_t *StaInfo_p = (extStaDb_StaInfo_t*)data_p; */
	/*    StaInfo_p->State = AUTHENTICATED; */
	return (MLME_SUCCESS);
}

/*!
* received association request service routine 
*  
* @param data_p Pointer to user defined data
* @return MLME_SUCCESS, MLME_INPROCESS, MLME_FAIL
*/
int wl_MacMlme_AssocReq( vmacApInfo_t *vmacSta_p, void *data_p, UINT32 msgSize )
{
	macmgmtQ_MgmtMsg_t *MgmtMsg_p = (macmgmtQ_MgmtMsg_t *)data_p;
	macMgmtMlme_AssociateReq(vmacSta_p, (macmgmtQ_MgmtMsg3_t *)MgmtMsg_p, msgSize);
	return (MLME_SUCCESS);
}

/*!
* received re-association request service routine 
*  
* @param data_p Pointer to user defined data
* @return MLME_SUCCESS, MLME_INPROCESS, MLME_FAIL
*/
int wl_MacMlme_ReAssocReq(vmacApInfo_t *vmacSta_p,  void *data_p, UINT32 msgSize )
{
	macmgmtQ_MgmtMsg_t *MgmtMsg_p = (macmgmtQ_MgmtMsg_t *)data_p;
	macMgmtMlme_ReassociateReq(vmacSta_p, (macmgmtQ_MgmtMsg3_t *)MgmtMsg_p, msgSize);
	return (MLME_SUCCESS);
}

/*!
* received dis-association request service routine 
*  
* @param data_p Pointer to user defined data
* @return MLME_SUCCESS, MLME_INPROCESS, MLME_FAIL
*/
int wl_MacMlme_DisAssocReq(vmacApInfo_t *vmacSta_p, void *data_p, UINT32 msgSize )
{
	macmgmtQ_MgmtMsg_t *MgmtMsg_p = (macmgmtQ_MgmtMsg_t *)data_p;
	macMgmtMlme_DisassociateMsg(vmacSta_p,(macmgmtQ_MgmtMsg3_t *)MgmtMsg_p, msgSize);
	return (MLME_SUCCESS);
}

/*!
* ds response 
*  
* @param data_p Pointer to user defined data
* @return MLME_SUCCESS, MLME_INPROCESS, MLME_FAIL
*/
int wl_MacMlme_DsResponse( void *data_p )
{
	/*    macmgmtQ_MgmtMsg_t *MgmtMsg_p = (macmgmtQ_MgmtMsg_t *)data_p; */
	return (MLME_SUCCESS);
}

/*!
* disassociation command service routine 
*  
* @param data_p Pointer to user defined data
* @return MLME_SUCCESS, MLME_INPROCESS, MLME_FAIL
*/
int wl_MacMlme_DisAssocCmd( vmacApInfo_t *vmacSta_p, void *data_p )
{
	IEEEtypes_DisassocCmd_t *DisassocCmd_p = (IEEEtypes_DisassocCmd_t *)data_p;
	macMgmtMlme_DisassociateCmd(vmacSta_p, DisassocCmd_p);
	return (MLME_SUCCESS);
}

/*!
* received authenticate request service routine 
*  
* @param data_p Pointer to user defined data
* @return MLME_SUCCESS, MLME_INPROCESS, MLME_FAIL
*/
int wl_MacMlme_AuthReq( void *data_p )
{
	return (MLME_SUCCESS);
}

/*!
* received authenticate sequence even service routine 
*  
* @param data_p Pointer to user defined data
* @return MLME_SUCCESS, MLME_INPROCESS, MLME_FAIL
*/
int wl_MacMlme_AuthEven(vmacApInfo_t *vmacSta_p, void *data_p )
{
	AuthRspSrvApMsg *authRspMsg_p = (AuthRspSrvApMsg *)data_p;
	int share_key = 1;
	if ( share_key )
	{
		/*mlmeAuthDoSharedKey(authRspMsg_p); */
		return (MLME_INPROCESS);
	} else
	{
		mlmeAuthDoOpenSys(vmacSta_p,authRspMsg_p);
	}
	return (MLME_SUCCESS);
}

/*!
* received authenticate sequence 1 service routine 
*  
* @param data_p Pointer to user defined data
* @return MLME_SUCCESS, MLME_INPROCESS, MLME_FAIL
*/
int wl_MacMlme_AuthOdd1(vmacApInfo_t *vmacSta_p, void *data_p )
{
	AuthRspSrvApMsg *authRspMsg = (AuthRspSrvApMsg *)data_p;

	if ( authRspMsg->arAlg_in == shared_key  && authRspMsg->arAlg == shared_key)
	{
		WLDBG_INFO(DBG_LEVEL_4, "wl_MacMlme_AuthOdd1:: mlmeAuthDoSharedKeySeq1 \n");
		if ( mlmeAuthDoSharedKeySeq1(vmacSta_p,authRspMsg)==MLME_SUCCESS )
		{
			return (MLME_INPROCESS);
		} else
		{
			return (MLME_FAILURE);
		}
	}
	else if (authRspMsg->arAlg_in == open_system  && authRspMsg->arAlg == open_system)
	{
		WLDBG_INFO(DBG_LEVEL_4, "wl_MacMlme_AuthOdd1:: mlmeAuthDoOpenSys \n");
		mlmeAuthDoOpenSys(vmacSta_p,authRspMsg);
	}
	else    
	{ 
		macmgmtQ_MgmtMsg3_t  *MgmtMsg_p;
		WLDBG_INFO(DBG_LEVEL_4, "wl_MacMlme_AuthOdd1:: unsupported authalg \n");
		MgmtMsg_p = (macmgmtQ_MgmtMsg3_t *) authRspMsg->mgtMsg;
		mlmeAuthError(vmacSta_p, IEEEtypes_STATUS_UNSUPPORTED_AUTHALG, 
			authRspMsg->arAlg,
			(UINT8 *)&MgmtMsg_p->Hdr.SrcAddr);
		return (MLME_FAILURE);
	}


	return (MLME_SUCCESS);
}

/*!
* received authenticate sequence 3 service routine 
*  
* @param data_p Pointer to user defined data
* @return MLME_SUCCESS, MLME_INPROCESS, MLME_FAIL
*/
int wl_MacMlme_AuthOdd3(vmacApInfo_t *vmacSta_p,void *data_p )
{
	AuthRspSrvApMsg *authRspMsg = (AuthRspSrvApMsg *)data_p;

	return (mlmeAuthDoSharedKeySeq3(vmacSta_p,authRspMsg));
}

/*!
* received deauthentication service routine 
*  
* @param data_p Pointer to user defined data
* @return MLME_SUCCESS, MLME_INPROCESS, MLME_FAIL
*/
int wl_MacMlme_DeAuth(vmacApInfo_t *vmacSta_p, void *data_p, UINT32 msgSize )
{
	macmgmtQ_MgmtMsg_t *MgmtMsg_p = (macmgmtQ_MgmtMsg_t *)data_p;
	macMgmtMlme_DeauthenticateMsg(vmacSta_p, (macmgmtQ_MgmtMsg3_t *)MgmtMsg_p , msgSize);
	return (MLME_SUCCESS);
}


/*!
* authenticate service timeout handler 
*  
* @param data_p Pointer to user defined data
* @return MLME_SUCCESS, MLME_INPROCESS, MLME_FAIL
*/
int wl_MacMlme_AuthSrvTimeout( void *data_p )
{
	extStaDb_StaInfo_t *StaInfo_p = (extStaDb_StaInfo_t*)data_p;
	StaInfo_p->State = UNAUTHENTICATED;
	return (MLME_SUCCESS);
}

/*!
* reset command service routine 
*  
* @param data_p Pointer to user defined data
* @return MLME_SUCCESS, MLME_INPROCESS, MLME_FAIL
*/
int wl_MacMlme_ResetCmd(vmacApInfo_t *vmacSta_p, void *data_p )
{
	SyncSrvApMsg *syncMsg = (SyncSrvApMsg *)data_p;
	macMgmtMlme_ResetCmd(vmacSta_p, (IEEEtypes_ResetCmd_t *)syncMsg->mgtMsg);
	return (MLME_SUCCESS);
}

/*!
* synchronization service timeout handler 
*  
* @param data_p Pointer to user defined data
* @return MLME_SUCCESS, MLME_INPROCESS, MLME_FAIL
*/
int wl_MacMlme_SyncSrvTimeout( void *data_p )
{
	return (MLME_SUCCESS);
}

/*!
* start command service routine 
*  
* @param data_p Pointer to user defined data
* @return MLME_SUCCESS, MLME_INPROCESS, MLME_FAIL
*/
int wl_MacMlme_StartCmd(vmacApInfo_t *vmacSta_p, void *data_p )
{
	SyncSrvApMsg *syncMsg = (SyncSrvApMsg *)data_p;
	macMgmtMlme_StartCmd(vmacSta_p,(IEEEtypes_StartCmd_t *)syncMsg->mgtMsg );
	return (MLME_SUCCESS);
}

/*!
* received probe request service routine 
*  
* @param data_p Pointer to user defined data
* @return MLME_SUCCESS, MLME_INPROCESS, MLME_FAIL
*/
int wl_MacMlme_ProbeReq(vmacApInfo_t *vmacSta_p,void *data_p )
{
	SyncSrvApMsg *syncMsg = (SyncSrvApMsg *)data_p;
	macMgmtMlme_ProbeRqst(vmacSta_p, (macmgmtQ_MgmtMsg3_t *)syncMsg->mgtMsg );
	return (MLME_SUCCESS);
}
#if defined(AP_SITE_SURVEY) || defined(AUTOCHANNEL)
/********************* Added for Site Survey on AP *******************************/
/*!
* Scan Request for Site Survey 
*  
* @param data_p Pointer to user defined data
* @return MLME_SUCCESS, MLME_INPROCESS, MLME_FAIL
*/
int wl_MacMlme_ScanReq(vmacApInfo_t *vmacSta_p, void *data_p )
{
	SyncSrvApMsg *syncMsg = (SyncSrvApMsg *)data_p;
	syncSrv_ScanCmd(vmacSta_p, (IEEEtypes_ScanCmd_t *)syncMsg->mgtMsg );
	return (MLME_SUCCESS);
}
#endif /* AP_SITE_SURVEY */

#ifdef IEEE80211H
/********************* Support IEEE 802.11h *******************************/
/*!
* MREQUEST Request 
*  
* @param data_p Pointer to user defined data
* @return MLME_SUCCESS, MLME_INPROCESS, MLME_FAIL
*/
int wl_MacMlme_MRequestReq(vmacApInfo_t *vmacSta_p,void *data_p )
{   
	SyncSrvApMsg *syncMsg = (SyncSrvApMsg *)data_p;
	macMgmtMlme_MRequestReq(vmacSta_p,(IEEEtypes_MRequestCmd_t *)syncMsg->mgtMsg );
	return (MLME_SUCCESS);
}

/*!
* MREPORT Request 
*  
* @param data_p Pointer to user defined data
* @return MLME_SUCCESS, MLME_INPROCESS, MLME_FAIL
*/
int wl_MacMlme_MReportReq(vmacApInfo_t *vmacSta_p, void *data_p )
{   
	SyncSrvApMsg *syncMsg = (SyncSrvApMsg *)data_p;
	macMgmtMlme_MReportReq(vmacSta_p,(IEEEtypes_MReportCmd_t *)syncMsg->mgtMsg );
	return (MLME_SUCCESS);
}

/*!
* CHANNELSWITCH Request 
*  
* @param data_p Pointer to user defined data
* @return MLME_SUCCESS, MLME_INPROCESS, MLME_FAIL
*/
int wl_MacMlme_ChannelswitchReq(vmacApInfo_t *vmacSta_p,void *data_p)
{   
	SyncSrvApMsg *syncMsg = (SyncSrvApMsg *)data_p;
	macMgmtMlme_ChannelSwitchReq(vmacSta_p,(IEEEtypes_ChannelSwitchCmd_t *)syncMsg->mgtMsg );
	return (MLME_SUCCESS);
}
#endif /* IEEE80211H */
#ifdef APCFGUR
int RmSrv_Timeout( UINT8 *data_p, UINT32 ptr )
{
	UINT8 *data;
	extStaDb_StaInfo_t *StaInfo_p = (extStaDb_StaInfo_t*)data_p; 
	data =(UINT8 *)ptr; 
	SendApiDataTo(&StaInfo_p->Addr, data);
	return (MLME_SUCCESS);
}
#endif

