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
 * \brief   802.11 Mlme service implementations
 *
 */
#include "mlmeSta.h"


/*!
 * association serveice timeout handler 
 *  
 * @param data_p Pointer to user defined data
 * @return MLME_SUCCESS, MLME_INPROCESS, MLME_FAIL
 */
int wl_MacMlme_AssocSrvStaTimeout( void *info_p, void *data_p )
{
    return MLME_SUCCESS;
}

/*!
 * association command serveice routine 
 *  
 * @param data_p Pointer to user defined data
 * @return MLME_SUCCESS, MLME_INPROCESS, MLME_FAIL
 */
int wl_MacMlme_AssocCmd( void *info_p, void *data_p )
{
    IEEEtypes_AssocCmd_t *AssocCmd_p = (IEEEtypes_AssocCmd_t *)data_p;
    
    assocSrv_AssocCmd((vmacStaInfo_t *)info_p, AssocCmd_p);
    return MLME_SUCCESS;
}

/*!
 * re-association command serveice routine 
 *  
 * @param data_p Pointer to user defined data
 * @return MLME_SUCCESS, MLME_INPROCESS, MLME_FAIL
 */
int wl_MacMlme_ReAssocCmd( void *info_p, void *data_p )
{
    assocSrv_ReAssocCmd((vmacStaInfo_t *)info_p, (IEEEtypes_ReassocCmd_t *)data_p);
    return MLME_SUCCESS;
}

/*!
 * received association response serveice routine 
 *  
 * @param data_p Pointer to user defined data
 * @return MLME_SUCCESS, MLME_INPROCESS, MLME_FAIL
 */
int wl_MacMlme_AssocRsp( void *info_p, void *data_p )
{
    dot11MgtFrame_t *MgmtMsg_p = (dot11MgtFrame_t *)data_p;
    
    assocSrv_RecvAssocRsp((vmacStaInfo_t *)info_p, 
						   MgmtMsg_p);
    return MLME_SUCCESS;
}

/*!
 * received reassociation response serveice routine 
 *  
 * @param data_p Pointer to user defined data
 * @return MLME_SUCCESS, MLME_INPROCESS, MLME_FAIL
 */
int wl_MacMlme_ReAssocRsp( void *info_p, void *data_p )
{
    dot11MgtFrame_t *MgmtMsg_p = (dot11MgtFrame_t *)data_p;

    assocSrv_RecvReAssocRsp((vmacStaInfo_t *)info_p, MgmtMsg_p);
    return MLME_SUCCESS;
}

/*!
 * authentication command service routine 
 *  
 * @param data_p Pointer to user defined data
 * @return MLME_SUCCESS, MLME_INPROCESS, MLME_FAIL
 */
int wl_MacMlme_AuthReqCmd( void *info_p, void *data_p )
{
    IEEEtypes_AuthCmd_t *AuthCmd_p = (IEEEtypes_AuthCmd_t *)data_p;
    if(authSrv_AuthCmd((vmacStaInfo_t *)info_p, AuthCmd_p) == MLME_SUCCESS)
    {
        return MLME_INPROCESS;
    }
    return MLME_FAILURE;
}

/*!
 * received authentication sequence even serveice routine 
 *  
 * @param data_p Pointer to user defined data
 * @return MLME_SUCCESS, MLME_INPROCESS, MLME_FAIL
 */
int wl_MacMlme_AuthStaEven( void *info_p, void *data_p )
{
    dot11MgtFrame_t *MgmtMsg_p = (dot11MgtFrame_t *)data_p;
    
    return authSrv_RecvMsgRsp((vmacStaInfo_t *)info_p, MgmtMsg_p);
    //return MLME_SUCCESS;
}

/*!
 * received authentication sequence 1 serveice routine 
 *  
 * @param data_p Pointer to user defined data
 * @return MLME_SUCCESS, MLME_INPROCESS, MLME_FAIL
 */


/*!
 * received de-authentication serveice routine 
 *  
 * @param data_p Pointer to user defined data
 * @return MLME_SUCCESS, MLME_INPROCESS, MLME_FAIL
 */
int wl_MacMlme_DeAuthSta( void *info_p, void *data_p )
{
    dot11MgtFrame_t *MgmtMsg_p = (dot11MgtFrame_t *)data_p;
    
    authSrv_RecvMsgDeAuth((vmacStaInfo_t *)info_p, MgmtMsg_p );
    return MLME_SUCCESS;
}

/*!
 * de-authentication command serveice routine 
 *  
 * @param data_p Pointer to user defined data
 * @return MLME_SUCCESS, MLME_INPROCESS, MLME_FAIL
 */
int wl_MacMlme_DeAuthStaCmd( void *info_p, void *data_p )
{
    IEEEtypes_DeauthCmd_t *DeauthCmd_p = (IEEEtypes_DeauthCmd_t *)data_p;
    
    authSrv_DeAuthCmd((vmacStaInfo_t *)info_p, DeauthCmd_p );
    return MLME_SUCCESS;
}

/*!
 * authentication serveice timeout handler 
 *  
 * @param data_p Pointer to user defined data
 * @return MLME_SUCCESS, MLME_INPROCESS, MLME_FAIL
 */
int wl_MacMlme_AuthSrvStaTimeout( void *info_p, void *data_p )
{
    /* extStaDb_StaInfo_t *StaInfo_p = (extStaDb_StaInfo_t*)data_p; */
    /* StaInfo_p->State = UNAUTHENTICATED; */
    return MLME_SUCCESS;
}

/*!
 * scan request serveice routine 
 *  
 * @param data_p Pointer to user defined data
 * @return MLME_SUCCESS, MLME_INPROCESS, MLME_FAIL
 */
int wl_MacMlme_ScanReqSta( void *info_p, void *data_p )
{    
    syncSrvSta_ScanCmd((vmacStaInfo_t *)info_p, data_p);
    return MLME_SUCCESS;
}

/*!
 * start request serveice routine 
 *  
 * @param data_p Pointer to user defined data
 * @return MLME_SUCCESS, MLME_INPROCESS, MLME_FAIL
 */
int wl_MacMlme_StartReq( void *info_p, void *data_p )
{
    IEEEtypes_StartCmd_t *StartCmd_p = (IEEEtypes_StartCmd_t *)data_p;
    
    syncSrv_StartCmd((vmacStaInfo_t *)info_p, StartCmd_p);
    return MLME_SUCCESS;
}

/*!
 * join request serveice routine 
 *  
 * @param data_p Pointer to user defined data
 * @return MLME_SUCCESS, MLME_INPROCESS, MLME_FAIL
 */
int wl_MacMlme_JoinReq( void *info_p, void *data_p )
{
    IEEEtypes_JoinCmd_t *JoinCmd_p = (IEEEtypes_JoinCmd_t *)data_p;
    
    syncSrv_JoinCmd((vmacStaInfo_t *)info_p, JoinCmd_p);
    return MLME_SUCCESS;
}

