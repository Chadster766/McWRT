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

/*******************************************************************************************
*
* File: AuthSta_srv.c
*        Client Authentication Service Function Calls
* Description:  Implementation of the Client MLME Authentication Services
*
*******************************************************************************************/
#include "mlmeSta.h"
#include "wl_mib.h"

#include "mlmeApi.h"
#include "wlvmac.h"

//=============================================================================
//                         IMPORTED PUBLIC VARIABLES
//=============================================================================

/* Functions Declarations */
static SINT32 authSrv_DoOpenAuth( vmacStaInfo_t *vStaInfo_p, dot11MgtFrame_t *MgmtMsg_p );
static SINT32 authSrv_DoSharedKeyAuth( vmacStaInfo_t *vStaInfo_p, dot11MgtFrame_t *MgmtMsg_p );


/*************************************************************************
* Function: authSrv_SndAuthCnfm
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
static void authSrv_SndAuthCnfm(vmacStaInfo_t *vStaInfo_p,
                                UINT8 authType, 
                                UINT16 authResult, 
                                UINT8 *authPeerAddr)
{
    IEEEtypes_AuthCfrm_t AuthCfrm;

    AuthCfrm.AuthType = authType;
    AuthCfrm.Result   = authResult;
    memcpy(&AuthCfrm.PeerStaAddr, authPeerAddr, sizeof(IEEEtypes_MacAddr_t));
    mlmeApiSndNotification(vStaInfo_p, (UINT8 *)&AuthCfrm, MlmeAuth_Cnfm);
}

/*************************************************************************
* Function: authSrv_SndDeAuthCnfm
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
static void authSrv_SndDeAuthCnfm( vmacStaInfo_t *vStaInfo_p,
                                   UINT16 deAuthResult, 
                                   UINT8 *deAuthPeerAddr)
{
    IEEEtypes_DeauthCfrm_t DeAuthCfrm;

    DeAuthCfrm.Result   = deAuthResult;
    memcpy(&DeAuthCfrm.PeerStaAddr, deAuthPeerAddr, sizeof(IEEEtypes_MacAddr_t));
    mlmeApiSndNotification(vStaInfo_p, (UINT8 *)&DeAuthCfrm, MlmeDeAuth_Cnfm);
}

/*************************************************************************
* Function: authSrv_SndDeAuthInd
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
/*static*/extern void authSrv_SndDeAuthInd( vmacStaInfo_t *vStaInfo_p,
                                            UINT16 deAuthReason, 
                                            UINT8 *deAuthPeerAddr)
{
    IEEEtypes_DeauthInd_t DeAuthInd;

    DeAuthInd.Reason   = deAuthReason;
    memcpy(&DeAuthInd.PeerStaAddr, deAuthPeerAddr, sizeof(IEEEtypes_MacAddr_t));
    mlmeApiSndNotification(vStaInfo_p, (UINT8 *)&DeAuthInd, MlmeDeAuth_Ind);
}

/*************************************************************************
* Function: authSrv_AuthActTimeOut
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern SINT32 authSrv_AuthActTimeOut(UINT8 *data)
{
    vmacStaInfo_t *vStaInfo_p = (vmacStaInfo_t *)data;
    
    #ifdef ETH_DEBUG
    eprintf("*** Auth Process timeout ****\n");
    #endif /* ETH_DEBUG */
    evtMgtSrvTimeOut(vStaInfo_p, auth_req_srv);
    authSrv_SndAuthCnfm(vStaInfo_p, vStaInfo_p->staSecurityMibs.mib_AuthAlg_p->Type, AUTH_RESULT_TIMEOUT, 
						&vStaInfo_p->macMgmtMlme_ThisStaData.BssId[0]);
    /* L2 Event Notification */
    mlmeApiEventNotification(vStaInfo_p,
                              MlmeAuth_Cnfm,
                              (UINT8 *)&vStaInfo_p->macMgmtMlme_ThisStaData.BssId, 
                              ETH_EVT_JOIN_TIMEOUT);
    return 0;
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
extern SINT32 authSrv_SndAuthError( vmacStaInfo_t *vStaInfo_p,
                                    IEEEtypes_StatusCode_t statusCode,
                                    IEEEtypes_AuthTransSeq_t seqNum,
                                    UINT16 arAlg_in, 
                                    IEEEtypes_MacAddr_t *destMac)
{
    dot11MgtFrame_t *mgtAuthRsp_p;
    vmacEntry_t * vmacEntry_p = (vmacEntry_t *)vStaInfo_p->vMacEntry_p;
    
    /* Send response message due to error in Auth Process*/
    if((mgtAuthRsp_p = mlmeApiAllocMgtMsg(vmacEntry_p->phyHwMacIndx)) == NULL)
    {
        return MLME_FAILURE;
    }
    /* Fill out default info */
    mlmePrepDefaultMgtMsg_Sta(vStaInfo_p, 
                              mgtAuthRsp_p, destMac, 
                              IEEE_MSG_AUTHENTICATE,
                              &(vStaInfo_p->macMgmtMlme_ThisStaData.BssId));
    mgtAuthRsp_p->Hdr.FrmBodyLen = 0;
    /* Add Auth Alg */
    mgtAuthRsp_p->Body.Auth.AuthAlg = arAlg_in;
    mgtAuthRsp_p->Hdr.FrmBodyLen += sizeof(IEEEtypes_AuthAlg_t);
    /* Add Auth Seq Number */
    mgtAuthRsp_p->Body.Auth.AuthTransSeq = seqNum + 1;
    mgtAuthRsp_p->Hdr.FrmBodyLen += sizeof(IEEEtypes_AuthTransSeq_t);
    /* Add Status Code */
    mgtAuthRsp_p->Body.Auth.StatusCode = statusCode;
    mgtAuthRsp_p->Hdr.FrmBodyLen += sizeof(IEEEtypes_StatusCode_t);
    /* Send for tx */
    if (mlmeApiSendMgtMsg_Sta(vStaInfo_p, mgtAuthRsp_p, NULL) == MLME_FAILURE)
    {
        return MLME_FAILURE;
    }
    return MLME_SUCCESS;
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
extern SINT32 authSrv_SndDeAuthMsg( vmacStaInfo_t *vStaInfo_p, 
                                    IEEEtypes_MacAddr_t *destMac_p,
                                    IEEEtypes_MacAddr_t *bssId_p,
                                    UINT16 reasonCode)
{
    dot11MgtFrame_t *mgtDeAuth_p;
    vmacEntry_t * vmacEntry_p = (vmacEntry_t *)vStaInfo_p->vMacEntry_p;

    /* Send response message due to error in Auth Process*/
    if((mgtDeAuth_p = mlmeApiAllocMgtMsg(vmacEntry_p->phyHwMacIndx)) == NULL)
    {
        return MLME_FAILURE;
    }
    /* Build mgt frame */
    mlmePrepDefaultMgtMsg_Sta(vStaInfo_p,
                              mgtDeAuth_p, 
                              destMac_p, 
                              IEEE_MSG_DEAUTHENTICATE,
                              bssId_p);
    mgtDeAuth_p->Hdr.FrmBodyLen = sizeof(IEEEtypes_Deauth_t);
    mgtDeAuth_p->Body.Deauth.ReasonCode = reasonCode;
    /* Send mgt frame */
    if (mlmeApiSendMgtMsg_Sta(vStaInfo_p, mgtDeAuth_p, NULL) == MLME_FAILURE)
    {
        return MLME_FAILURE;
    }
    /* L2 Event Notification */
    mlmeApiEventNotification(vStaInfo_p,
                              MlmeDeAuth_Req,
                              (UINT8 *)destMac_p, 
                              reasonCode);
    return MLME_SUCCESS;
}

/*************************************************************************
* Function: authSrv_DoOpenAuth
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
static SINT32 authSrv_DoOpenAuth( vmacStaInfo_t *vStaInfo_p,
                                  dot11MgtFrame_t *MgmtMsg_p )
{
    #ifdef ETH_DEBUG
    eprintf("authSrv_DoOpenAuth:: Entered\n");
    eprintf("authSrv_DoOpenAuth:: Seq-> %d\n", MgmtMsg_p->Body.Auth.AuthTransSeq);
    #endif /* ETH_DEBUG */

    if(MgmtMsg_p->Body.Auth.AuthTransSeq != 2)
    {
        return MLME_FAILURE;
    }    
    if(MgmtMsg_p->Body.Auth.StatusCode == IEEEtypes_STATUS_SUCCESS)
    {
        vStaInfo_p->macMgmtMain_State = STATE_AUTHENTICATED_WITH_AP;
        #ifdef ETH_DEBUG
        eprintf("authSrv_DoOpenAuth: case 2 ->success: send confrm\n");
        #endif /* ETH_DEBUG */
        authSrv_SndAuthCnfm(vStaInfo_p, AUTH_OPEN_SYSTEM, 
                            AUTH_RESULT_SUCCESS, &MgmtMsg_p->Hdr.SrcAddr[0]);
    }
    else
    {
        #ifdef ETH_DEBUG  
        eprintf("authSrv_DoOpenAuth: case default\n");
        #endif /* ETH_DEBUG */
        authSrv_SndAuthCnfm(vStaInfo_p, AUTH_OPEN_SYSTEM, 
                            AUTH_RESULT_REFUSED, &MgmtMsg_p->Hdr.SrcAddr[0]);
        /* L2 Event Notification */
        mlmeApiEventNotification(vStaInfo_p,
                                  MlmeAuth_Cnfm,
                                  (UINT8 *)&MgmtMsg_p->Hdr.SrcAddr,
                                  MgmtMsg_p->Body.Auth.StatusCode);
    }
    return MLME_SUCCESS;
}
         
/*************************************************************************
* Function: authSrv_AuthCmd
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern UINT32 authSrv_AuthCmd( vmacStaInfo_t *vStaInfo_p,
							   IEEEtypes_AuthCmd_t *AuthCmd_p )
{
    IEEEtypes_MacAddr_t      srcAddr;
    dot11MgtFrame_t *mgtFrame_p;
    vmacEntry_t * vmacEntry_p = (vmacEntry_t *)vStaInfo_p->vMacEntry_p;

#ifdef ETH_DEBUG
   eprintf("authSrv_AuthCmd:: Entered\n");
#endif

   //memcpy(&authCmd_last, AuthCmd_p, sizeof(IEEEtypes_AuthCmd_t));
   memcpy(&srcAddr, AuthCmd_p->PeerStaAddr, sizeof(IEEEtypes_MacAddr_t));

   /* Build mgt frame */
   if((mgtFrame_p = mlmeApiAllocMgtMsg(vmacEntry_p->phyHwMacIndx)) == NULL)
   {
	   /* Notify SME  of failure */
        authSrv_SndAuthCnfm(vStaInfo_p, AuthCmd_p->AuthType, 
                            AUTH_RESULT_RESOURCE_ERROR, &AuthCmd_p->PeerStaAddr[0]);
        return MLME_FAILURE;
    }
    mlmePrepDefaultMgtMsg_Sta(vStaInfo_p,
                              mgtFrame_p, 
                              &srcAddr, 
                              IEEE_MSG_AUTHENTICATE,
                              &(vStaInfo_p->macMgmtMlme_ThisStaData.BssId));
    mgtFrame_p->Hdr.FrmBodyLen = 0;
    /* Set Auth Alg Type */
    mgtFrame_p->Body.Auth.AuthAlg = AuthCmd_p->AuthType;
    mgtFrame_p->Hdr.FrmBodyLen += sizeof(IEEEtypes_AuthAlg_t);
    /* Set Auth Seq Number */
    mgtFrame_p->Body.Auth.AuthTransSeq = 1;
    mgtFrame_p->Hdr.FrmBodyLen += sizeof(IEEEtypes_AuthTransSeq_t);
    /* Set Status Code */
    mgtFrame_p->Body.Auth.StatusCode = IEEEtypes_STATUS_SUCCESS;
    mgtFrame_p->Hdr.FrmBodyLen += sizeof(IEEEtypes_StatusCode_t);
    /* Send mgt frame */
    if (mlmeApiSendMgtMsg_Sta(vStaInfo_p, mgtFrame_p, NULL) == MLME_FAILURE)
    {
        #ifdef ETH_DEBUG
        eprintf("authSrv_AuthCmd:: fail to tx msg\n");
        #endif /* ETH_DEBUG */
        /* Notify SME  of failure */
        authSrv_SndAuthCnfm(vStaInfo_p, AuthCmd_p->AuthType, 
                            AUTH_RESULT_RESOURCE_ERROR, &AuthCmd_p->PeerStaAddr[0]); 
		return MLME_FAILURE;
    }        
    /* Start a timer */
    /* Get and start a scan timer with duration of the maximum channel time */
    mlmeApiStartTimer(vStaInfo_p, 
                      (UINT8 *)&vStaInfo_p->authTimer,
                      &authSrv_AuthActTimeOut,
                      AUTH_TIME);
    return MLME_SUCCESS;
}

/*************************************************************************
* Function: authSrv_DeAuthCmd
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern SINT32 authSrv_DeAuthCmd(vmacStaInfo_t *vStaInfo_p,
								IEEEtypes_DeauthCmd_t *DeauthCmd_p )
{
    dot11MgtFrame_t *mgtFrame_p;
    vmacEntry_t  *vmacEntry_p;

    vmacEntry_p = (vmacEntry_t *)vStaInfo_p->vMacEntry_p;
    /* Check to see if this is our current AP */
    if (memcmp(vStaInfo_p->macMgmtMlme_ThisStaData.BssId,
              DeauthCmd_p->PeerStaAddr,
              sizeof(IEEEtypes_MacAddr_t)))
    {
        authSrv_SndDeAuthCnfm(vStaInfo_p, DEAUTH_RESULT_INVALID_PARAMETERS, 
                                DeauthCmd_p->PeerStaAddr);
        return MLME_FAILURE;
    }
    /* Build mgt frame */
    if((mgtFrame_p = mlmeApiAllocMgtMsg(vmacEntry_p->phyHwMacIndx)) == NULL)
    {
        return MLME_FAILURE;
    }
    mlmePrepDefaultMgtMsg_Sta(vStaInfo_p,
                              mgtFrame_p, 
                              &DeauthCmd_p->PeerStaAddr, 
                              IEEE_MSG_DEAUTHENTICATE,
                              &(vStaInfo_p->macMgmtMlme_ThisStaData.BssId));
    mgtFrame_p->Hdr.FrmBodyLen = sizeof(IEEEtypes_Deauth_t);
    mgtFrame_p->Body.Deauth.ReasonCode = DeauthCmd_p->Reason;
    /* Send mgt frame */
    if (authSrv_SndDeAuthMsg( vStaInfo_p, 
                                    &DeauthCmd_p->PeerStaAddr,
                                    &(vStaInfo_p->macMgmtMlme_ThisStaData.BssId),
                                    DeauthCmd_p->Reason) == MLME_FAILURE)
    {
        /* Notify SME of failure */
        authSrv_SndDeAuthCnfm(vStaInfo_p, DEAUTH_RESULT_INVALID_PARAMETERS, 
                                DeauthCmd_p->PeerStaAddr);
        return MLME_FAILURE;
    }
    /* DeAuth success so delete peer from DB */
    mlmeApiDelStaDbEntry(vStaInfo_p, (UINT8 *)&(DeauthCmd_p->PeerStaAddr));
    mlmeApiDisconnect(vStaInfo_p);
    /* set state to Idle */
    vStaInfo_p->macMgmtMain_State         = STATE_IDLE;
    vStaInfo_p->macMgmtMain_PostScanState = STATE_IDLE;
    /* Notify SME of success */
    authSrv_SndDeAuthCnfm(vStaInfo_p, DEAUTH_RESULT_SUCCESS,
                                DeauthCmd_p->PeerStaAddr);
    /*Milind. 09/29/05*/
    /*Free the AssocTable data structure that has been currently assigned to this*/
    /*peer station to which the WB was associated/joined*/
    mlmeApiFreePeerStationStaInfoAndAid(&(DeauthCmd_p->PeerStaAddr), vmacEntry_p);
    return MLME_SUCCESS;
}

/*************************************************************************
* Function: authSrv_RecvMsgRsp
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern SINT32 authSrv_RecvMsgRsp( vmacStaInfo_t *vStaInfo_p,
								  dot11MgtFrame_t *MgmtMsg_p )
{
    SINT32 authResult;

    #ifdef ETH_DEBUG
    eprintf("macMgmtMlme_AuthenticateMsg:: Entered\n");
    #endif

    TimerRemove(&vStaInfo_p->authTimer);
    switch (MgmtMsg_p->Body.Auth.AuthAlg)
    {
      case AUTH_OPEN_SYSTEM:
            authResult = authSrv_DoOpenAuth(vStaInfo_p, MgmtMsg_p);
            break;

      case AUTH_SHARED_KEY:
            authResult = authSrv_DoSharedKeyAuth(vStaInfo_p, MgmtMsg_p);
            break;
           
      default:
            #ifdef ETH_DEBUG
            eprintf("macMgmtMlme_AuthenticateMsg:: case default\n");
            #endif /* ETH_DEBUG */
            /* Notify peer of failure */
            authSrv_SndAuthError( vStaInfo_p, 
                                IEEEtypes_STATUS_UNSUPPORTED_AUTHALG,
                                MgmtMsg_p->Body.Auth.AuthTransSeq,
                                MgmtMsg_p->Body.Auth.AuthAlg, 
                                &MgmtMsg_p->Hdr.SrcAddr);
            authSrv_SndAuthCnfm(vStaInfo_p, AUTH_NOT_SUPPORTED, 
                            AUTH_RESULT_REFUSED, &MgmtMsg_p->Hdr.SrcAddr[0]);
            /* L2 Event Notification */
            mlmeApiEventNotification(vStaInfo_p,
                                  MlmeAuth_Cnfm,
                                  (UINT8 *)&MgmtMsg_p->Hdr.SrcAddr,
                                  MgmtMsg_p->Body.Auth.StatusCode);
            authResult = MLME_FAILURE;
            break;
    }
    return authResult;
}

/*************************************************************************
* Function: authSrv_RecvMsgDeAuth
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern SINT32 authSrv_RecvMsgDeAuth(vmacStaInfo_t *vStaInfo_p,
									dot11MgtFrame_t *MgmtMsg_p )
{
    vmacEntry_t  *vmacEntry_p;

    vmacEntry_p = (vmacEntry_t *)vStaInfo_p->vMacEntry_p;
    /* Is this packet for us */
    if (memcmp(vStaInfo_p->macMgmtMlme_ThisStaData.BssId,
               MgmtMsg_p->Hdr.SrcAddr,
               sizeof(IEEEtypes_MacAddr_t)))
    {
        /* No, so silently discard */
        return MLME_FAILURE;
    }
    /* If we are currently scanning */
    if (vStaInfo_p->macMgmtMain_State == STATE_SCANNING 
           || vStaInfo_p->macMgmtMain_State == STATE_RESTORING_FROM_SCAN)
    {
        vStaInfo_p->macMgmtMain_PostScanState = STATE_JOINED;
    }
    else
    {
        vStaInfo_p->macMgmtMain_State = STATE_IDLE;
    }
    /* L2 Event Notification */
    mlmeApiEventNotification(vStaInfo_p,
                              MlmeDeAuth_Ind,
                              &MgmtMsg_p->Hdr.SrcAddr[0], 
                              MgmtMsg_p->Body.Deauth.ReasonCode);
    /* Notify SME of DeAuth */
    authSrv_SndDeAuthInd(vStaInfo_p, MgmtMsg_p->Body.Deauth.ReasonCode, 
                           MgmtMsg_p->Hdr.SrcAddr);

    /*Milind. 09/29/05*/
    /*Free the AssocTable data structure that has been currently assigned to this*/
    /*peer station to which the WB was associated/joined*/
    mlmeApiFreePeerStationStaInfoAndAid(&(MgmtMsg_p->Hdr.SrcAddr), vmacEntry_p);
    return MLME_SUCCESS;
}

/*************************************************************************
* Function: authSrv_SKeyProcessSeq2
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
static SINT32 authSrv_SKeyProcessSeq2( vmacStaInfo_t *vStaInfo_p,
                                       dot11MgtFrame_t *MgmtMsg_p )
{
    dot11MgtFrame_t *mgtFrame_p;
    vmacEntry_t * vmacEntry_p = (vmacEntry_t *)vStaInfo_p->vMacEntry_p;

    if(MgmtMsg_p->Body.Auth.StatusCode != IEEEtypes_STATUS_SUCCESS)
    {         
        /* Notify SME of Auth Failure */
        authSrv_SndAuthCnfm(vStaInfo_p, AUTH_SHARED_KEY, 
                            AUTH_RESULT_REFUSED, &MgmtMsg_p->Hdr.SrcAddr[0]);
        /* L2 Event Notification */
        mlmeApiEventNotification(vStaInfo_p,
                              MlmeAuth_Cnfm,
                              (UINT8 *)&MgmtMsg_p->Hdr.SrcAddr, 
                              MgmtMsg_p->Body.Auth.StatusCode);
        return MLME_FAILURE;
    }
    /* Build mgt frame */
    if((mgtFrame_p = mlmeApiAllocMgtMsg(vmacEntry_p->phyHwMacIndx)) == NULL)
    {
        return MLME_FAILURE;
    }
    mlmePrepDefaultMgtMsg_Sta(vStaInfo_p,
                              mgtFrame_p, 
                              &MgmtMsg_p->Hdr.SrcAddr, 
                              IEEE_MSG_AUTHENTICATE,
                              &(vStaInfo_p->macMgmtMlme_ThisStaData.BssId));
    mgtFrame_p->Hdr.FrmBodyLen = 0;
    mgtFrame_p->Hdr.FrmCtl.Wep = 1;
    /* Set Auth Auth Alg Type */
    mgtFrame_p->Body.Auth.AuthAlg = AUTH_SHARED_KEY;
    mgtFrame_p->Hdr.FrmBodyLen += sizeof(IEEEtypes_AuthAlg_t);
    /* Set Aut Seq Number */
    mgtFrame_p->Body.Auth.AuthTransSeq = 3;
    mgtFrame_p->Hdr.FrmBodyLen += sizeof(IEEEtypes_AuthTransSeq_t);
    /* Set Status Code */
    mgtFrame_p->Body.Auth.StatusCode = IEEEtypes_STATUS_SUCCESS;
    mgtFrame_p->Hdr.FrmBodyLen += sizeof(IEEEtypes_StatusCode_t);
    /* Add Challenge text attribute */
    mgtFrame_p->Body.Auth.ChallengeText.ElementId = CHALLENGE_TEXT;
    mgtFrame_p->Body.Auth.ChallengeText.Len = MgmtMsg_p->Body.Auth.ChallengeText.Len;
    mgtFrame_p->Hdr.FrmBodyLen += sizeof(IEEEtypes_ElementId_t)
                                 + sizeof(IEEEtypes_Len_t);
    /* Since this is Management Frame we can always do software encryption */
    /* Easier for porting to different hardware later on */
    RemoveClientFw((UINT8 *)&mgtFrame_p->Hdr.BssId, vmacEntry_p);
    mlmeApiWepEncrypt(vStaInfo_p, (UINT8 *)&mgtFrame_p->Body.Auth,
                      MgmtMsg_p->Body.Auth.ChallengeText.Text,
                        MgmtMsg_p->Body.Auth.ChallengeText.Len);
    mgtFrame_p->Hdr.FrmBodyLen += MgmtMsg_p->Body.Auth.ChallengeText.Len 
                                    + WEP_ENCRYPT_OVER_HDR_LEN;
    /* Send mgt frame */
    if (mlmeApiSendMgtMsg_Sta(vStaInfo_p, mgtFrame_p, NULL) == MLME_FAILURE)
    {
        return MLME_FAILURE;
    }
    /* Start an Auth timer for Timeout period */
    /* Get and start a scan timer with duration of the maximum channel time */
    mlmeApiStartTimer(vStaInfo_p, 
                      (UINT8 *)&vStaInfo_p->authTimer,
                      &authSrv_AuthActTimeOut,
                      AUTH_TIME);
    return MLME_SUCCESS;
}

/*************************************************************************
* Function: authSrv_SKeyProcessSeq4
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
static SINT32 authSrv_SKeyProcessSeq4( vmacStaInfo_t *vStaInfo_p,
                                       dot11MgtFrame_t *MgmtMsg_p )
{
    if (MgmtMsg_p->Body.Auth.StatusCode != IEEEtypes_STATUS_SUCCESS)
    {
        /* Notify SME of Auth Failure */
        authSrv_SndAuthCnfm(vStaInfo_p, AUTH_SHARED_KEY, 
                            AUTH_RESULT_REFUSED, &MgmtMsg_p->Hdr.SrcAddr[0]);
        /* L2 Event Notification */
        mlmeApiEventNotification(vStaInfo_p,
                              MlmeAuth_Cnfm,
                              (UINT8 *)&MgmtMsg_p->Hdr.SrcAddr, 
                              MgmtMsg_p->Body.Auth.StatusCode);
        return MLME_FAILURE;
    }
    if (!memcmp(MgmtMsg_p->Hdr.SrcAddr,
                    MgmtMsg_p->Hdr.BssId,
                    sizeof(IEEEtypes_MacAddr_t)))
    {
       vStaInfo_p->macMgmtMain_State = STATE_AUTHENTICATED_WITH_AP;
    }
    /* Notify SME of Auth Success */
    authSrv_SndAuthCnfm(vStaInfo_p, AUTH_SHARED_KEY, 
                        AUTH_RESULT_SUCCESS, MgmtMsg_p->Hdr.SrcAddr);
    return MLME_SUCCESS;
}

/*************************************************************************
* Function: authSrv_DoSharedKeyAuth
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
static SINT32 authSrv_DoSharedKeyAuth( vmacStaInfo_t *vStaInfo_p,
                                       dot11MgtFrame_t *MgmtMsg_p )
{
    switch (MgmtMsg_p->Body.Auth.AuthTransSeq)
    {
        case 2:
            if(authSrv_SKeyProcessSeq2(vStaInfo_p, MgmtMsg_p) == MLME_SUCCESS)
            {
                return MLME_INPROCESS;
            }
            else
            {
                return MLME_FAILURE;
            }
            break;
        case 4:
            return authSrv_SKeyProcessSeq4(vStaInfo_p, MgmtMsg_p);
            break;
    }
    return MLME_FAILURE;
}

/*************************************************************************
* Function: authSrv_Reset
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern SINT32 authSrv_Reset(vmacStaInfo_t *vStaInfo_p)
{
    mlmeApiStopTimer(vStaInfo_p, (UINT8 *)&vStaInfo_p->authTimer);
    /* Init the Authentication Request state machines */
    AuthReqSrvStaCtor(&vStaInfo_p->mgtStaAuthReq);
    mhsm_initialize(&vStaInfo_p->mgtStaAuthReq.super,&vStaInfo_p->mgtStaAuthReq.sTop);
    return MLME_SUCCESS;
}
