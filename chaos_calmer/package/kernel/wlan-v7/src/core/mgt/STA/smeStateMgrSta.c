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


/*********************************************************************************
 *
 * Purpose:
 *    This file contains the implementations of the function prototypes given
 *    in the associated header file for the State Manager portion of the
 *    Station Managment Entity (SME) component.
 *
 * Public Procedures:
 *    smeStateMgr_ScanCmd             Process a command to perform a scan
 *    smeStateMgr_ScanCfrm            Process the results of a scan
 *
 *    smeStateMgr_StartCmd            Process a command to start an IBSS
 *    smeStateMgr_StartCfrm           Process the results of a start
 *
 *    smeStateMgr_JoinCmd             Process a command to join a BSS
 *    smeStateMgr_JoinCfrm            Process the results of joining a BSS
 *
 *    smeStateMgr_AuthenticateCmd     Process a command to authenticate
 *    smeStateMgr_AuthenticateCfrm    Process the results of authentication
 *    smeStateMgr_AuthenticateInd     Process notification of an authentication
 *
 *    smeStateMgr_AssociateCmd        Process a command to associate
 *    smeStateMgr_AssociateCfrm       Process the results of association
 *    smeStateMgr_AssociateInd        Process notification of an association
 *
 *    smeStateMgr_DeauthenticateCmd   Process a command to deauthenticate
 *    smeStateMgr_DeauthenticateCfrm  Process the results of deauthentication
 *    smeStateMgr_DeauthenticateInd   Process notification of a
 *                                    deauthentication
 *
 *    smeStateMgr_DisassociateCmd     Process a comand to disassociate
 *    smeStateMgr_DisassociateCfrm    Process the results of disassociation
 *    smeStateMgr_DisassociateInd     Process notification of a disassociation
 *
 *    smeStateMgr_ReassociateCmd      Process a command to reassociate
 *    smeStateMgr_ReassociateCfrm     Process the results of reassociation
 *    smeStateMgr_ReassociateInd      Process notification of a reassocation
 *
 *    smeStateMgr_ResetCmd            Process a command to reset
 *    smeStateMgr_ResetCfrm           Process the results of a reset
 *
 *    smeStateMgr_PwrMgmtCfrm         Process the results of a power mgmt
 *                                    change
 *
 * Private Procedures:
 *    BasicRatesSupported             Determines is the station supports the
 *                                    required data rates
 *    ConstructJoinCmd                Routine to construct a join command
 *    ConstructScanCmd                Routine to construct a scan command
 *    PerformJoin                     Routine to construct and send a join
 *                                    command
 *
 * Notes:
 *    None.
 *
 *****************************************************************************/

//=============================================================================
//                               INCLUDE FILES
//=============================================================================
#include "mlmeSta.h"
#include "wl_mib.h"

#include "mlmeApi.h"
#ifdef WPA_STA
#include "keyMgmt.h"
#include "keyMgmtSta.h"
#endif /* WPA_STA */
#include "wlvmac.h"
#include "wl_hal.h"
#include "mlmeParent.h"
#include "mlmeChild.h"
#include "StaDb.h"
#include "ap8xLnxIoctl.h"
#include "ap8xLnxIntf.h"

extern MIB_PHY_DSSS_TABLE         *PhyDSSSTable_p;


//=============================================================================
//                                DEFINITIONS
//=============================================================================
#define MAX_JOIN_RETRIES  0x3
   //
   // The number of retries to go through when trying to join a BSS fails
   //
   
#define MAX_AUTH_RETRIES  0x1
   //
   // The number of retries to go through when trying to authenticate with
   // an AP fails
   //
   
#define MAX_ASSOC_RETRIES 0x1
   //
   // The number of retries to go through when trying to associate with
   // an AP fails
   //
   
//=============================================================================
//                         IMPORTED PUBLIC VARIABLES
//=============================================================================
extern UINT32 mlmeApiSetStaMode(vmacStaInfo_t *vStaInfo_p, UINT8 Mode);
extern SINT32 mlmeApiUpdateBasicRatesSetting(vmacStaInfo_t *vStaInfo_p,
                                            UINT8 *rateBuf_p,
                                            UINT32 rateBufLen);


    
/* For Child */
extern UINT8 mib_childMode[NUM_OF_WLMACS];


//=============================================================================
//                          MODULE LEVEL VARIABLES
//=============================================================================


//extern void  *ScanResultsMap_p[IEEEtypes_MAX_BSS_DESCRIPTS];
   //
   // Structure recording pointers for received scan results; each
   // pointer indicates where the start of data is for a particular BSS
   //


//=============================================================================
//                   PRIVATE PROCEDURES (ANSI Prototypes)
//=============================================================================




//=============================================================================
//                         CODED PUBLIC PROCEDURES
//=============================================================================

void dummy_smeSta(void)
{
    /* Needed for multiIce problem. */
    return;
}

/*************************************************************************
* Function: smeStateMgr_SetCmd
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
static UINT32 smeStateMgr_SetCmd(UINT8 *info_p,
                                 UINT32 curCmd)
{
    vmacStaInfo_t *vStaInfo_p = (vmacStaInfo_t *)info_p;
	vStaInfo_p->cmdHistory |= curCmd;
    return vStaInfo_p->cmdHistory;
}

/*************************************************************************
* Function: smeStateMgr_UnSetCmd
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
static UINT32 smeStateMgr_UnSetCmd(UINT8 *info_p,
                                   UINT32 curCmd)
{
    vmacStaInfo_t *vStaInfo_p = (vmacStaInfo_t *)info_p;
	vStaInfo_p->cmdHistory &= ~curCmd;
    return vStaInfo_p->cmdHistory;
}

/*************************************************************************
* Function: smePendingCmd
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
static UINT32 smePendingCmd(UINT8 *info_p)
{
    vmacStaInfo_t *vStaInfo_p = (vmacStaInfo_t *)info_p;
    if((vStaInfo_p->cmdHistory &cmdJoin)&&!vStaInfo_p->AssociatedFlag&&(vStaInfo_p->smeMain_State ==STATE_ASSOCIATED))
    {
        vStaInfo_p->cmdHistory = 0;
    }
	return vStaInfo_p->cmdHistory;
}

/*************************************************************************
* Function: smeClearCmdHistory
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
static UINT32 smeClearCmdHistory(UINT8 *info_p)
{
    vmacStaInfo_t *vStaInfo_p = (vmacStaInfo_t *)info_p;
    vStaInfo_p->cmdHistory = 0;
	return vStaInfo_p->cmdHistory;
}

/*************************************************************************
* Function: smeStateMgr_SetCmd
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
static SINT32 smeNotifyApSrv(UINT8 *info_p)
{
    vmacStaInfo_t *vStaInfo_p = (vmacStaInfo_t *)info_p;
    UINT32 macIndex;
    UINT8  rateBuf[IEEEtypes_MAX_DATA_RATES_G];
    UINT32 rateBufLen = 0;
    UINT32 curLen;

    if(!vStaInfo_p->isParentSession || 
       (vStaInfo_p->macMgmtMlme_ThisStaData.CapInfo.Ibss))
    {
        return -1;
    }
    memset(&rateBuf[0], 0, IEEEtypes_MAX_DATA_RATES_G);
    macIndex = mlmeApiGetMacIndex(vStaInfo_p);
    curLen = util_ListLen(vStaInfo_p->bOpRateSet, MAX_B_DATA_RATES);
    if(curLen)
    {
        memcpy((void *)&rateBuf[rateBufLen], 
               (void const *)&vStaInfo_p->bOpRateSet[0],
               curLen);
        rateBufLen += curLen;
    }
    /* Copy over 11G rates */
    curLen = util_ListLen(vStaInfo_p->gOpRateSet, MAX_G_DATA_RATES);
    if(curLen)
    {
        memcpy((void *)&rateBuf[rateBufLen], 
               (void const *)&vStaInfo_p->gOpRateSet[0],
               curLen);
        rateBufLen += curLen;
    }
    if(mlmeApiUpdateBasicRatesSetting(vStaInfo_p,&rateBuf[0], rateBufLen) != 0)
    {
        return -1;
    }
    return 0;
}

UINT8   ScanResults_tmp[MAX_SCAN_BUF_SIZE + BUF_PAD_NUM];


/******************************************************************************
 *
 * Name: smeStateMgr_ScanCfrm
 *
 * Description:
 *    Routine to handle the results from a previous scan request.
 *
 * Conditions For Use:
 *    All software components have been initialized and started.
 *
 * Arguments:
 *    Arg1 (i  ): MgmtMsg_p - Pointer to the message containing scan results
 *
 * Return Value:
 *    None.
 *
 * Notes:
 *    None.
 *
 * PDL:
 *    xxx
 * END PDL
 *
 *****************************************************************************/
extern void smeStateMgr_ScanCfrm( UINT8 *info_p,
								  macmgmtQ_CmdRsp_t *MgmtMsg_p )
{
   vmacStaInfo_t *vStaInfo_p = (vmacStaInfo_t *)info_p;
   UINT32                     fixedSize;
   macMgmtMlme_SigQltySet_t   signalInfo;
   unsigned char             *scanCfrm_p;
   vmacEntry_t  *vmacEntry_p;
   UINT32 i, j, k;
   UINT32 strongest;
   UINT32 size;
   UINT8 *sorted_p;
   UINT32 SigQltyMark[IEEEtypes_MAX_BSS_DESCRIPTS];
   macMgmtMlme_SigQltySet_t SigQltyTmp[IEEEtypes_MAX_BSS_DESCRIPTS];
   
   if(info_p == NULL)
   {
       return;
   }
   vmacEntry_p = (vmacEntry_t *)vStaInfo_p->vMacEntry_p;
   vStaInfo_p->smeMain_State = vStaInfo_p->PostScanState;

   memset ((void *) ScanResults_tmp, 0, MAX_SCAN_BUF_SIZE + BUF_PAD_NUM);
   sorted_p = (UINT8 *)&ScanResults_tmp[0];
   SPIN_LOCK_IRQSAVE(&vStaInfo_p->ScanResultsLock, vStaInfo_p->ScanResultsFlags);
   for (i = 0; i < vStaInfo_p->NumDescripts; i++)
   {
       SigQltyMark[i] = 0;
       memcpy ((void *) &SigQltyTmp[i],
               (void *) &vStaInfo_p->scanTableResult_p->macMgmtMlme_SigQltyResults[i], 
               sizeof (macMgmtMlme_SigQltySet_t) );
   }
   
   for (i = 0; i < vStaInfo_p->NumDescripts; i++)
   {
       k = 0;
       while(SigQltyMark[k])
       {
           k++;
       }
       strongest = k;

       for (j = k + 1; j < vStaInfo_p->NumDescripts; j++)
       {
           if(SigQltyMark[j])
           {
               continue;
           }
           if( SigQltyTmp[strongest].RSSI > SigQltyTmp[j].RSSI)
           {
               strongest = j;
           }
       }
       SigQltyMark[strongest] = 1;
       if(strongest == vStaInfo_p->NumDescripts-1)
       {
           //size = vStaInfo_p->ScanResults_p - vStaInfo_p->scanTableResult_p->ScanResultsMap_p[strongest] +2;
           size = vStaInfo_p->ScanResults_p - vStaInfo_p->scanTableResult_p->ScanResultsMap_p[strongest];
       }
       else
       {
           size = vStaInfo_p->scanTableResult_p->ScanResultsMap_p[strongest+1] - vStaInfo_p->scanTableResult_p->ScanResultsMap_p[strongest];
       }
       memcpy (sorted_p,
               //vStaInfo_p->scanTableResult_p->ScanResultsMap_p[strongest] -2, 
               vStaInfo_p->scanTableResult_p->ScanResultsMap_p[strongest], 
               size);
       sorted_p += size;
       memcpy ((void *) &vStaInfo_p->scanTableResult_p->macMgmtMlme_SigQltyResults[i],
                (void *) (&SigQltyTmp[strongest]), 
                sizeof (macMgmtMlme_SigQltySet_t) );
    
   }
   memcpy ((void *) &vStaInfo_p->scanTableResult_p->ScanResults[0], 
            (void *) &ScanResults_tmp[0],
            (UINT8 *)vStaInfo_p->ScanResults_p - (UINT8 *)&vStaInfo_p->scanTableResult_p->ScanResults[0]);   

   smeStateMgr_UnSetCmd(info_p, cmdScan);

   memcpy((void *)vStaInfo_p->BssDescSet_p,
          &(MgmtMsg_p->Msg.ScanCfrm.Rsp.BssDescSet),
          MgmtMsg_p->Msg.ScanCfrm.BufSize);

   /*-----------------------------------------------------------------*/
   /* Now prepare for determining which of the found signals (if any) */
   /* is the best candidate to join.                                  */
   /*-----------------------------------------------------------------*/
   signalInfo.RSSI        = 0;
   signalInfo.SigQual1    = 0;
   signalInfo.SigQual2    = 0;
   signalInfo.NumReadings = 0;

   fixedSize = sizeof(IEEEtypes_MacAddr_t)     +
               sizeof(IEEEtypes_SsId_t)        +
               sizeof(IEEEtypes_Bss_t)         +
               sizeof(IEEEtypes_BcnInterval_t) +
               sizeof(IEEEtypes_DtimPeriod_t)  +
               2*sizeof(IEEEtypes_TimeStamp_t);

   scanCfrm_p = (unsigned char *)&(vStaInfo_p->BssDescSet_p->BssDesc[0]); 
 
   SPIN_UNLOCK_IRQRESTORE(&vStaInfo_p->ScanResultsLock, vStaInfo_p->ScanResultsFlags);
   
   mlmeApiEventNotification(vStaInfo_p,
                             MlmeScan_Cnfm,
                             (UINT8 *)&vStaInfo_p->macMgmtMlme_ThisStaData.BssId[0], 
                             0);
   return;

} // End smeStateMgr_ScanCfrm()

/******************************************************************************
 *
 * Name: smeStateMgr_StartCfrm
 *
 * Description:
 *    Routine to handle the results from a previous start request.
 *
 * Conditions For Use:
 *    All software components have been initialized and started.
 *
 * Arguments:
 *    Arg1 (i  ): MgmtMsg_p - Pointer to the message containing start
 *                            results
 *
 * Return Value:
 *    None.
 *
 * Notes:
 *    None.
 *
 * PDL:
 *    xxx
 * END PDL
 *
 *****************************************************************************/
extern void smeStateMgr_StartCfrm( UINT8 *info_p,
                                   macmgmtQ_CmdRsp_t *MgmtMsg_p )
{
   vmacStaInfo_t *vStaInfo_p = (vmacStaInfo_t *)info_p;

   smeStateMgr_UnSetCmd(info_p, cmdStart);
   //
   // Set to started state if the action was successful.
   //
   if (MgmtMsg_p->Msg.StartCfrm.Result == START_RESULT_SUCCESS)
   {
      vStaInfo_p->smeMain_State = STATE_IBSS_STARTED;
      memcpy(&(vStaInfo_p->JoinAddr),
	          vStaInfo_p->macMgmtMlme_ThisStaData.BssId,
		       sizeof(IEEEtypes_MacAddr_t));
      if(vStaInfo_p->isParentSession)
      {
          #ifdef MLME_SYNC_AP /* Notify AP SME that Client has Joined a BSS */
          smeNotifyApSrv(info_p);
          #endif /* MLME_SYNC_AP */
          mlmeApiSetControlRates(vStaInfo_p);
      }
  }

} // End smeStateMgr_StartCfrm()


/******************************************************************************
 *
 * Name: smeStateMgr_JoinCfrm
 *
 * Description:
 *    Routine to handle the results from a previous join request.
 *
 * Conditions For Use:
 *    All software components have been initialized and started.
 *
 * Arguments:
 *    Arg1 (i  ): MgmtMsg_p - Pointer to the message containing join results
 *
 * Return Value:
 *    None.
 *
 * Notes:
 *    None.
 *
 * PDL:
 *    xxx
 * END PDL
 *
 *****************************************************************************/
extern SINT32 smeStateMgr_JoinCfrm( UINT8 *info_p,
									macmgmtQ_CmdRsp_t *MgmtMsg_p )
{
   vmacStaInfo_t *vStaInfo_p = (vmacStaInfo_t *)info_p;
   macmgmtQ_CmdReq_t   *macMgmtMsg_p;
   vmacEntry_t  *vmacEntry_p;

   vmacEntry_p = (vmacEntry_t *)vStaInfo_p->vMacEntry_p;
   smeStateMgr_UnSetCmd(info_p, cmdJoin);
   switch (vStaInfo_p->smeMain_State)
   {
      case STATE_SCANNING:
      case STATE_JOINED:
      //case STATE_AUTHENTICATED_WITH_AP:
      //case STATE_ASSOCIATING:
      //case STATE_ASSOCIATED:
      //case STATE_REASSOCIATING:
      case STATE_RESTORING_FROM_SCAN:
         /*--------------------------------------------------------*/
         /* Assume that this confirmation message is at this point */
         /* meaningless under these states and should therefore be */
         /* ignored.                                               */
         /*--------------------------------------------------------*/
         return MLME_FAILURE;
         break;

      //case STATE_IDLE:
      //case STATE_JOINING:
      //case STATE_IBSS_STARTED:
   default:
         if (MgmtMsg_p->Msg.JoinCfrm.Result == JOIN_RESULT_SUCCESS)
         {
            /*===============================================================*/
            /*                         JOIN SUCCEEDED                        */
            /*===============================================================*/
            vStaInfo_p->smeMain_State = STATE_JOINED;
            if(vStaInfo_p->isParentSession)
            {
                #ifdef MLME_SYNC_AP /* Notify AP SME that Client has Joined a BSS */
                smeNotifyApSrv(info_p);
                #endif /* MLME_SYNC_AP */
                mlmeApiSetControlRates(vStaInfo_p);
                // Set the trunkid as ACTIVE so that data traffic is sent
                #ifdef WPA_STA
                if (!vStaInfo_p->staSecurityMibs.mib_PrivacyTable_p->RSNEnabled) {
                    mlmeApiSetTrunkIdActive(vStaInfo_p, vmacEntry_p->trunkId, TRUE, STA_TRUNK_TYPE);
                }
                #endif /* WPA_STA */
            }
            return MLME_SUCCESS;
         }
         else
         {
            /*===============================================================*/
            /*                          JOIN FAILED                          */
            /*===============================================================*/
			 if(!vStaInfo_p->isParentSession)
			 {
				 /* No need to retry to join for child session */
				 mlmeApiUpdateLinkStatus(vStaInfo_p, LINK_DOWN);
				 return MLME_FAILURE;
			 }
#ifdef buildModes_RETRIES
            vStaInfo_p->JoinRetryCount++;

            if (vStaInfo_p->JoinRetryCount < MAX_JOIN_RETRIES)
            {
               /*-----------------------------------------------------*/
               /* Resend a command to the MAC Management Task to join */
               /*-----------------------------------------------------*/
               if ((macMgmtMsg_p = (macmgmtQ_CmdReq_t *)mlmeApiAllocSmeMsg()) != NULL)
               {

                  memcpy(macMgmtMsg_p,
                         &vStaInfo_p->LastJoinMsg,
                         sizeof(macmgmtQ_CmdReq_t));

				   memcpy((void *)&macMgmtMsg_p->targetAddr[0], 
						(const void *)&vmacEntry_p->vmacAddr[0],
						sizeof(IEEEtypes_MacAddr_t));

                  if (!mlmeApiSendSmeMsg(macMgmtMsg_p))
                  {
                     //
                     // Writing to the MAC Management Task's SME queue failed,
                     // so we had better release the buffer, or it will forever
                     // be tied up
                     //
                     mlmeApiFreeSmeMsg((macmgmtQ_CmdBuf_t *)macMgmtMsg_p);
                     vStaInfo_p->smeMain_State = STATE_IDLE;
                  }
                  else
                  {
                      smeStateMgr_SetCmd(info_p, cmdJoin);
                  }
               }
            }
            else
            {
               vStaInfo_p->JoinRetryCount = 0;

#endif //buildModes_RETRIES
                  /*--------------------------------------------------*/
                  /* All join attempts failed and we're not going to  */
                  /* continue by rescanning, so set the state to idle */
                  /* and send status back to the CB processor.        */
                  /*--------------------------------------------------*/
                  vStaInfo_p->smeMain_State = STATE_IDLE;
                  vStaInfo_p->macMgmtMain_State = STATE_IDLE;
                  mlmeApiUpdateLinkStatus(vStaInfo_p, LINK_DOWN);

#ifdef buildModes_RETRIES
         }
#endif //buildModes_RETRIES

            return MLME_FAILURE;
         }
         break;
   }

} // End smeStateMgr_JoinCfrm()



/******************************************************************************
 *
 * Name: smeStateMgr_AuthenticateCfrm
 *
 * Description:
 *    Routine to handle the results from a previous authenticate request.
 *
 * Conditions For Use:
 *    All software components have been initialized and started.
 *
 * Arguments:
 *    Arg1 (i  ): MgmtMsg_p - Pointer to the message containing
 *                            authentication results
 *
 * Return Value:
 *    None.
 *
 * Notes:
 *    None.
 *
 * PDL:
 *    xxx
 * END PDL
 *
 *****************************************************************************/
extern void smeStateMgr_AuthenticateCfrm( UINT8 *info_p,
                                          macmgmtQ_CmdRsp_t *MgmtMsg_p )
{
   vmacStaInfo_t *vStaInfo_p = (vmacStaInfo_t *)info_p;
   BOOLEAN                   reportBack = FALSE;
   macmgmtQ_CmdReq_t        *macMgmtMsg_p;
   UINT8  macIndx;
   vmacEntry_t  *vmacEntry_p;
   UINT16 macId;
	
   vmacEntry_p = (vmacEntry_t *)vStaInfo_p->vMacEntry_p;
   macIndx = mlmeApiGetMacIndex(vStaInfo_p);
   macId = mlmeApiGetMacId(vStaInfo_p);



         if (!memcmp(MgmtMsg_p->Msg.AuthCfrm.PeerStaAddr,
                     vStaInfo_p->macMgmtMlme_ThisStaData.BssId,
                     sizeof(IEEEtypes_MacAddr_t)))
         {
            /*------------------------------------------------------------*/
            /* If authentication was successful and the entity to         */
            /* authenticate with was an AP, try to perform an association */
            /*------------------------------------------------------------*/
            if (MgmtMsg_p->Msg.AuthCfrm.Result == AUTH_RESULT_SUCCESS)
            {
               vStaInfo_p->smeMain_State = STATE_AUTHENTICATED_WITH_AP;
               reportBack    = TRUE;

               vStaInfo_p->AuthRetryCount = 0;

               //
               // Authentication was with an AP, so try to associate with
               // it now
               //
#ifdef buildModes_RETRIES
               vStaInfo_p->AssocRetryCount = 0;
#endif

               if ((macMgmtMsg_p = (macmgmtQ_CmdReq_t *)mlmeApiAllocSmeMsg()) != NULL)
               {
				  assocSrv_Reset(vStaInfo_p);
                  memset(macMgmtMsg_p, 0, sizeof(macmgmtQ_CmdReq_t));
				  memcpy((void *)&macMgmtMsg_p->targetAddr[0], 
						(const void *)&vmacEntry_p->vmacAddr[0],
						sizeof(IEEEtypes_MacAddr_t));
                  macMgmtMsg_p->CmdType = MlmeAssoc_Req;
                  macMgmtMsg_p->Body.AssocCmd.FailTimeout    = ASSOC_TIME;
                  macMgmtMsg_p->Body.AssocCmd.ListenInterval = 5;
                  macMgmtMsg_p->Body.AssocCmd.CapInfo        = vStaInfo_p->bssDescProfile_p->Cap;
                  memcpy(&macMgmtMsg_p->Body.AssocCmd.SsId, &vStaInfo_p->bssDescProfile_p->SsId, sizeof(IEEEtypes_SsId_t));

                  memcpy(&(macMgmtMsg_p->Body.AssocCmd.PeerStaAddr),
                         MgmtMsg_p->Msg.AuthCfrm.PeerStaAddr,
                         sizeof(IEEEtypes_MacAddr_t));

                  if (!mlmeApiSendSmeMsg(macMgmtMsg_p))
                  {
                     //
                     // Writing to the MAC Management Task's SME queue failed,
                     // so we had better release the buffer, or it will forever
                     // be tied up
                     //
                     mlmeApiFreeSmeMsg((macmgmtQ_CmdBuf_t *)macMgmtMsg_p);
                  }
                  else
                  {
                     vStaInfo_p->smeMain_State = STATE_ASSOCIATING;
                  }
               }
            }
            else
            {
               vStaInfo_p->AuthRetryCount++;
               if (vStaInfo_p->AuthRetryCount < MAX_AUTH_RETRIES)
               {
                  /*------------------------------------------------*/
                  /* Resend a command to the MAC Management Task to */
                  /* authenticate                                   */
                  /*------------------------------------------------*/
                  if ((macMgmtMsg_p = (macmgmtQ_CmdReq_t *)mlmeApiAllocSmeMsg()) != NULL)
                  {
					  memcpy(macMgmtMsg_p,
                            &vStaInfo_p->LastAuthMsg,
                            sizeof(macmgmtQ_CmdReq_t));

					 memcpy((void *)&macMgmtMsg_p->targetAddr[0], 
						(const void *)&vmacEntry_p->vmacAddr[0],
						sizeof(IEEEtypes_MacAddr_t));
                     

                     if (!mlmeApiSendSmeMsg(macMgmtMsg_p))
                     {
                        //
                        // Writing to the MAC Management Task's SME queue
                        // failed, so we had better release the buffer, or
                        // it will forever be tied up
                        //
                         syncSrv_UpdateJoinStatus(vStaInfo_p, JOIN_FAIL_INTERNAL_ERROR);
                         mlmeApiFreeSmeMsg((macmgmtQ_CmdBuf_t *)macMgmtMsg_p);
                     }
                  }
               }
               else
               {
                   if (MgmtMsg_p->Msg.AuthCfrm.Result == AUTH_RESULT_TIMEOUT)
                   {
                       syncSrv_UpdateJoinStatus(vStaInfo_p, JOIN_FAIL_AUTH_TIMEOUT);
                   }
                   else
                   {
                       syncSrv_UpdateJoinStatus(vStaInfo_p, JOIN_FAIL_AUTH_REJECTED);
                   }
                   vStaInfo_p->AuthRetryCount = 0;
                   reportBack = TRUE;
               }
            }
         }
         else
         {
            //
            // Confirmation was not from the joined AP - it could be with
            // some other station this station is trying to authenticate
            // with. Since we only keep track of retries for an
            // authentication with an AP, just accept the result and
            // report it.
            //
            reportBack = TRUE;
         }

         

} // End smeStateMgr_AuthenticateCfrm()


/******************************************************************************
 *
 * Name: smeStateMgr_AuthenticateInd
 *
 * Description:
 *    Routine to send an indication to the CB Processor Task that an
 *    unsolicited authentication has occurred.
 *
 * Conditions For Use:
 *    All software components have been initialized and started.
 *
 * Arguments:
 *    Arg1 (i  ): MgmtMsg_p - Pointer to the message containing
 *                            authentication results
 *
 * Return Value:
 *    None.
 *
 * Notes:
 *    None.
 *
 * PDL:
 *    xxx
 * END PDL
 *
 *****************************************************************************/
extern void smeStateMgr_AuthenticateInd( UINT8 *info_p,
                                         macmgmtQ_CmdRsp_t *MgmtMsg_p )
{

   vmacStaInfo_t *vStaInfo_p = (vmacStaInfo_t *)info_p;
   /*---------------------------------------------------------------------*/
   /* If somehow the authentication indication happened to be from the AP */
   /* of the BSS we know about (this is not expected), then change the    */
   /* state.                                                              */
   /*---------------------------------------------------------------------*/
   if (!memcmp(MgmtMsg_p->Msg.AuthInd.PeerStaAddr,
               vStaInfo_p->macMgmtMlme_ThisStaData.BssId,
               sizeof(IEEEtypes_MacAddr_t)))
   {
      vStaInfo_p->smeMain_State = STATE_AUTHENTICATED_WITH_AP;
   }

   
} // End smeStateMgr_AuthenticateInd()




/******************************************************************************
 *
 * Name: smeStateMgr_AssociateCfrm
 *
 * Description:
 *    Routine to handle the results from a previous associate request.
 *
 * Conditions For Use:
 *    All software components have been initialized and started.
 *
 * Arguments:
 *    Arg1 (i  ): MgmtMsg_p - Pointer to the message containing
 *                            association results
 *
 * Return Value:
 *    None.
 *
 * Notes:
 *    None.
 *
 * PDL:
 *    xxx
 * END PDL
 *
 *****************************************************************************/
extern void smeStateMgr_AssociateCfrm( UINT8  *info_p,
                                       macmgmtQ_CmdRsp_t *MgmtMsg_p )
{
   vmacStaInfo_t *vStaInfo_p = (vmacStaInfo_t *)info_p;
   UINT8  macIndx;

   BOOLEAN                   reportBack = FALSE;
#ifdef buildModes_RETRIES
   macmgmtQ_CmdReq_t        *macMgmtMsg_p;
#endif
   vmacEntry_t  *vmacEntry_p;
   
   vmacEntry_p = (vmacEntry_t *)vStaInfo_p->vMacEntry_p;
   macIndx = mlmeApiGetMacIndex(vStaInfo_p);
   if (MgmtMsg_p->Msg.AssocCfrm.Result == ASSOC_RESULT_SUCCESS)
   {
#ifdef buildModes_RETRIES
       vStaInfo_p->AssocRetryCount = 0;
#endif
            vStaInfo_p->smeMain_State = STATE_ASSOCIATED;
            syncSrv_UpdateJoinStatus(vStaInfo_p, JOIN_SUCCESS);
            reportBack    = TRUE;
            mlmeApiUpdateLinkStatus(vStaInfo_p, LINK_UP);
         }
         else
         {
#ifdef buildModes_RETRIES
       vStaInfo_p->AssocRetryCount++;
       if (vStaInfo_p->AssocRetryCount < MAX_ASSOC_RETRIES)
       {
          if ((macMgmtMsg_p = (macmgmtQ_CmdReq_t *)mlmeApiAllocSmeMsg()) != NULL)
          {
	   	  memcpy((void *)&macMgmtMsg_p->targetAddr[0], 
	   			(const void *)&vmacEntry_p->vmacAddr[0],
	   			sizeof(IEEEtypes_MacAddr_t));
             macMgmtMsg_p->CmdType = MlmeAssoc_Req;
             macMgmtMsg_p->Body.AssocCmd.FailTimeout    = ASSOC_TIME;
             macMgmtMsg_p->Body.AssocCmd.ListenInterval = 5;
             macMgmtMsg_p->Body.AssocCmd.CapInfo        = vStaInfo_p->bssDescProfile_p->Cap;
             memcpy(&macMgmtMsg_p->Body.AssocCmd.SsId,
                     &vStaInfo_p->bssDescProfile_p->SsId, 
                    sizeof(IEEEtypes_SsId_t));
             memcpy(&(macMgmtMsg_p->Body.AssocCmd.PeerStaAddr),
                    &vStaInfo_p->macMgmtMlme_ThisStaData.BssId,
                    sizeof(IEEEtypes_MacAddr_t));

             if (!mlmeApiSendSmeMsg(macMgmtMsg_p))
             {
                //
                // Writing to the MAC Management Task's SME queue failed,
                // so we had better release the buffer, or it will forever
                // be tied up
                //
                mlmeApiFreeSmeMsg((macmgmtQ_CmdBuf_t *)macMgmtMsg_p);
                syncSrv_UpdateJoinStatus(vStaInfo_p, JOIN_FAIL_INTERNAL_ERROR);
             }
             else
             {
                vStaInfo_p->smeMain_State = STATE_ASSOCIATING;
             }
          }
       }
       else
       {
          vStaInfo_p->AssocRetryCount = 0;
          if (MgmtMsg_p->Msg.AssocCfrm.Result == ASSOC_RESULT_TIMEOUT)
          {
              syncSrv_UpdateJoinStatus(vStaInfo_p, JOIN_FAIL_ASSOC_TIMEOUT);
          }
          else
          {
              syncSrv_UpdateJoinStatus(vStaInfo_p, JOIN_FAIL_ASSOC_REJECTED);
          }
          vStaInfo_p->smeMain_State = STATE_AUTHENTICATED_WITH_AP;
          reportBack    = TRUE;
       }
#else
       vStaInfo_p->smeMain_State = STATE_AUTHENTICATED_WITH_AP;
       reportBack    = TRUE;
#endif //buildModes_RETRIES
   }

#ifdef buildModes_RETRIES
   if (!vStaInfo_p->AssocRetryCount || reportBack)
#else
   if (reportBack)
#endif
   {
      /*-----------------------------------------------------------*/
      /* Send an association confirmation to the CB Processor Task */
      /*-----------------------------------------------------------*/

   }

} // End smeStateMgr_AssociateCfrm()


/******************************************************************************
 *
 * Name: smeStateMgr_AssociateInd
 *
 * Description:
 *    Routine to send an indication to the CB Processor Task that an
 *    unsolicited association has occurred.
 *
 * Conditions For Use:
 *    All software components have been initialized and started.
 *
 * Arguments:
 *    Arg1 (i  ): MgmtMsg_p - Pointer to the message containing
 *                            association results
 *
 * Return Value:
 *    None.
 *
 * Notes:
 *    None.
 *
 * PDL:
 *    xxx
 * END PDL
 *
 *****************************************************************************/
extern void smeStateMgr_AssociateInd( UINT8 *info_p,
                                      macmgmtQ_CmdRsp_t *MgmtMsg_p )
{

   vmacStaInfo_t *vStaInfo_p = (vmacStaInfo_t *)info_p;

   if (!memcmp(MgmtMsg_p->Msg.AssocInd.PeerStaAddr,
               vStaInfo_p->macMgmtMlme_ThisStaData.BssId,
               sizeof(IEEEtypes_MacAddr_t)))
   {
      vStaInfo_p->smeMain_State = STATE_ASSOCIATED;
	  mlmeApiUpdateLinkStatus(vStaInfo_p, LINK_UP);
   }
   
} // End smeStateMgr_AssociateInd()




/******************************************************************************
 *
 * Name: smeStateMgr_DeauthenticateCfrm
 *
 * Description:
 *    Routine to handle the results from a previous deauthenticate request.
 *
 * Conditions For Use:
 *    All software components have been initialized and started.
 *
 * Arguments:
 *    Arg1 (i  ): MgmtMsg_p - Pointer to the message containing
 *                            deauthentication results
 *
 * Return Value:
 *    None.
 *
 * Notes:
 *    None.
 *
 * PDL:
 *    xxx
 * END PDL
 *
 *****************************************************************************/
extern void smeStateMgr_DeauthenticateCfrm( UINT8 *info_p,
                                            macmgmtQ_CmdRsp_t *MgmtMsg_p )
{
   vmacStaInfo_t *vStaInfo_p = (vmacStaInfo_t *)info_p;
   vmacEntry_t  *vmacEntry_p;
#ifdef AMPDU_SUPPORT_TX_CLIENT
   struct net_device *staDev = NULL;
   struct wlprivate *stapriv = NULL;
   vmacApInfo_t *vmacSta_p = NULL;
#endif

   vmacEntry_p = (vmacEntry_t *)vStaInfo_p->vMacEntry_p;
#ifdef AMPDU_SUPPORT_TX_CLIENT
   staDev = (struct net_device *)vmacEntry_p->privInfo_p;
   stapriv = NETDEV_PRIV_P(struct wlprivate, staDev);
   vmacSta_p = (vmacApInfo_t *)stapriv->vmacSta_p;
#endif

   smeStateMgr_UnSetCmd(info_p, cmdDeAuth);
   /*----------------------------------------------------------------*/
   /* If the deauthentication confirmation is from the AP of the BSS */
   /* we know about, then change the state.                          */
   /*----------------------------------------------------------------*/
   if (!memcmp(MgmtMsg_p->Msg.DeauthCfrm.PeerStaAddr,
               vStaInfo_p->macMgmtMlme_ThisStaData.BssId,
               sizeof(IEEEtypes_MacAddr_t)))
   {
      if (MgmtMsg_p->Msg.DeauthCfrm.Result == DEAUTH_RESULT_SUCCESS)
      {
          if(vStaInfo_p->isParentSession){
          // Set the trunkid as INACTIVE so that no data traffic is sent      
              mlmeApiSetTrunkIdActive(vStaInfo_p, vmacEntry_p->trunkId, FALSE, STA_TRUNK_TYPE);
          }
          #ifdef WPA_STA
          if (vStaInfo_p->staSecurityMibs.mib_PrivacyTable_p->RSNEnabled) {
              ((KeyData_t *)vStaInfo_p->keyMgmtInfoSta_p->pKeyData)->RSNDataTrafficEnabled = 0;
          }
          #endif /* WPA_STA */
#ifdef AMPDU_SUPPORT_TX_CLIENT       
	      cleanupAmpduTx(vmacSta_p, (UINT8 *)&vStaInfo_p->macMgmtMlme_ThisStaData.BssId[0]);
#endif          
		  mlmeApiUpdateLinkStatus(vStaInfo_p, LINK_DOWN);
          vStaInfo_p->smeMain_State = STATE_IDLE;

         memset(&(vStaInfo_p->macMgmtMlme_ThisStaData.BssSourceAddr),
                0,
                sizeof(IEEEtypes_MacAddr_t));

         /* Remove associated AP from UR station database, intially added for
         pseudo ether port support. */
         mlmeApiDelStaDbEntry(vStaInfo_p, (UINT8 *)&vStaInfo_p->macMgmtMlme_ThisStaData.BssId);

         memset(&(vStaInfo_p->macMgmtMlme_ThisStaData.BssId),
                0,
                sizeof(IEEEtypes_MacAddr_t));

         memset(&(vStaInfo_p->macMgmtMlme_ThisStaData.BssSsId),
                0,
                sizeof(IEEEtypes_SsId_t));

         vStaInfo_p->macMgmtMlme_ThisStaData.BssSsIdLen = 0;
      }
   }

} // End smeStateMgr_DeauthenticateCfrm()


/******************************************************************************
 *
 * Name: smeStateMgr_DeauthenticateInd
 *
 * Description:
 *    Routine to send an indication to the CB Processor Task that an
 *    unsolicited deauthentication has occurred.
 *
 * Conditions For Use:
 *    All software components have been initialized and started.
 *
 * Arguments:
 *    Arg1 (i  ): MgmtMsg_p - Pointer to the message containing
 *                            deauthentication results
 *
 * Return Value:
 *    None.
 *
 * Notes:
 *    None.
 *
 * PDL:
 *    xxx
 * END PDL
 *
 *****************************************************************************/
extern void smeStateMgr_DeauthenticateInd( UINT8 *info_p,
                                           macmgmtQ_CmdRsp_t *MgmtMsg_p )
{
   vmacStaInfo_t *vStaInfo_p = (vmacStaInfo_t *)info_p;
   vmacEntry_t  *vmacEntry_p;
#ifdef AMPDU_SUPPORT_TX_CLIENT
   struct net_device *staDev = NULL;
   struct wlprivate *stapriv = NULL;
   vmacApInfo_t *vmacSta_p = NULL;
#endif

   vmacEntry_p = (vmacEntry_t *)vStaInfo_p->vMacEntry_p;
#ifdef AMPDU_SUPPORT_TX_CLIENT
   staDev = (struct net_device *)vmacEntry_p->privInfo_p;
   stapriv = NETDEV_PRIV_P(struct wlprivate, staDev);
   vmacSta_p = (vmacApInfo_t *)stapriv->vmacSta_p;
#endif
   /*--------------------------------------------------------------*/
   /* If the deauthentication indication is from the AP of the BSS */
   /* we know about, then change the state.                        */
   /*--------------------------------------------------------------*/
   if (!memcmp(MgmtMsg_p->Msg.DeauthInd.PeerStaAddr,
               vStaInfo_p->macMgmtMlme_ThisStaData.BssId,
               sizeof(IEEEtypes_MacAddr_t)))
   {
      if(smePendingCmd(info_p))
      {
          syncSrv_CmdExceptionHandler(vStaInfo_p);
      }
      vStaInfo_p->smeMain_State = STATE_IDLE;
      vStaInfo_p->PostScanState = STATE_IDLE;
      if(vStaInfo_p->isParentSession){
          // Set the trunkid as INACTIVE so that no data traffic is sent      
          mlmeApiSetTrunkIdActive(vStaInfo_p, vmacEntry_p->trunkId, FALSE, STA_TRUNK_TYPE);
      }
      #ifdef WPA_STA
      if (vStaInfo_p->staSecurityMibs.mib_PrivacyTable_p->RSNEnabled) {
          ((KeyData_t *)vStaInfo_p->keyMgmtInfoSta_p->pKeyData)->RSNDataTrafficEnabled = 0;
          /* Stop Key management timer. */
          TimerRemove(&vStaInfo_p->keyMgmtInfoSta_p->keyMgmtStaHskHsm.rsnSecuredTimer);         
      }
      #endif /* WPA_STA */
#ifdef AMPDU_SUPPORT_TX_CLIENT 
	  cleanupAmpduTx(vmacSta_p, (UINT8 *)&vStaInfo_p->macMgmtMlme_ThisStaData.BssId[0]);
#endif       
      if(vStaInfo_p->AssociatedFlag)
      {
          mlmeApiEventNotification(vStaInfo_p,
                                      Tbcn,
                                      &vStaInfo_p->macMgmtMlme_ThisStaData.BssId[0], 
                                      0);
          mlmeApiUpdateLinkStatus(vStaInfo_p, LINK_DOWN);
          if(vStaInfo_p->isParentSession)
          {
              if(!vStaInfo_p->IBssStartFlag)
              {
              	  vStaInfo_p->staSecurityMibs.mib_PrivacyTable_p->RSNLinkStatus = 0;  	
                  vStaInfo_p->AssociatedFlag=0;
                  syncSrv_SndLinkLostInd(vStaInfo_p);
              }
          }
      }
   }

} // End smeStateMgr_DeauthenticateInd()



/******************************************************************************
 *
 * Name: smeStateMgr_DisassociateCfrm
 *
 * Description:
 *    Routine to handle the results from a previous disassociate request.
 *
 * Conditions For Use:
 *    All software components have been initialized and started.
 *
 * Arguments:
 *    Arg1 (i  ): MgmtMsg_p - Pointer to the message containing
 *                            disassociation results
 *
 * Return Value:
 *    None.
 *
 * Notes:
 *    None.
 *
 * PDL:
 *    xxx
 * END PDL
 *
 *****************************************************************************/
extern void smeStateMgr_DisassociateCfrm( UINT8 *info_p,
                                          macmgmtQ_CmdRsp_t *MgmtMsg_p )
{
   vmacStaInfo_t *vStaInfo_p = (vmacStaInfo_t *)info_p;
   vmacEntry_t  *vmacEntry_p;

   vmacEntry_p = (vmacEntry_t *)vStaInfo_p->vMacEntry_p;
   smeStateMgr_UnSetCmd(info_p, cmdDisAssoc);
   /*----------------------------------------------------------------------*/
   /* If the disassociation confirmation was successful, change the state. */
   /*----------------------------------------------------------------------*/
   if (MgmtMsg_p->Msg.DisassocCfrm.Result == DISASSOC_RESULT_SUCCESS)
   {
      vStaInfo_p->smeMain_State = STATE_AUTHENTICATED_WITH_AP;
      if(vStaInfo_p->isParentSession){
          // Set the trunkid as INACTIVE so that no data traffic is sent      
          mlmeApiSetTrunkIdActive(vStaInfo_p, vmacEntry_p->trunkId, FALSE, STA_TRUNK_TYPE);
      }
      #ifdef WPA_STA
      if (vStaInfo_p->staSecurityMibs.mib_PrivacyTable_p->RSNEnabled) {
          ((KeyData_t *)vStaInfo_p->keyMgmtInfoSta_p->pKeyData)->RSNDataTrafficEnabled = 0;
      }
      #endif /* WPA_STA */
	  mlmeApiUpdateLinkStatus(vStaInfo_p, LINK_DOWN);
   }


} // End smeStateMgr_DisassociateCfrm()


/******************************************************************************
 *
 * Name: smeStateMgr_DisassociateInd
 *
 * Description:
 *    Routine to send an indication to the CB Processor Task that an
 *    unsolicited disassociation has occurred.
 *
 * Conditions For Use:
 *    All software components have been initialized and started.
 *
 * Arguments:
 *    Arg1 (i  ): MgmtMsg_p - Pointer to the message containing
 *                            disassociation results
 *
 * Return Value:
 *    None.
 *
 * Notes:
 *    None.
 *
 * PDL:
 *    xxx
 * END PDL
 *
 *****************************************************************************/
extern void smeStateMgr_DisassociateInd( UINT8 *info_p,
                                         macmgmtQ_CmdRsp_t *MgmtMsg_p )
{
   vmacStaInfo_t *vStaInfo_p = (vmacStaInfo_t *)info_p;
   vmacEntry_t  *vmacEntry_p;
#ifdef AMPDU_SUPPORT_TX_CLIENT
   struct net_device *staDev = NULL;
   struct wlprivate *stapriv = NULL;
   vmacApInfo_t *vmacSta_p = NULL;
#endif

   vmacEntry_p = (vmacEntry_t *)vStaInfo_p->vMacEntry_p;
#ifdef AMPDU_SUPPORT_TX_CLIENT
   staDev = (struct net_device *)vmacEntry_p->privInfo_p;
   stapriv = NETDEV_PRIV_P(struct wlprivate, staDev);
   vmacSta_p = (vmacApInfo_t *)stapriv->vmacSta_p;
#endif
   /*------------------------------------------------------------*/
   /* If the disassociation indication is from the AP of the BSS */
   /* we know about, then change the state.                      */
   /*------------------------------------------------------------*/
   if (!memcmp(MgmtMsg_p->Msg.DisassocInd.PeerStaAddr,
               vStaInfo_p->macMgmtMlme_ThisStaData.BssId,
               sizeof(IEEEtypes_MacAddr_t)))
   {
      if(smePendingCmd(info_p))
      {
          syncSrv_CmdExceptionHandler(vStaInfo_p);
      }
      if (vStaInfo_p->smeMain_State == STATE_SCANNING ||
          vStaInfo_p->smeMain_State == STATE_RESTORING_FROM_SCAN)
      {
         if (vStaInfo_p->PostScanState == STATE_ASSOCIATED ||
             vStaInfo_p->PostScanState == STATE_REASSOCIATING)
         {
            vStaInfo_p->PostScanState = STATE_AUTHENTICATED_WITH_AP;
         }
      }
      else if (vStaInfo_p->smeMain_State == STATE_ASSOCIATED ||
               vStaInfo_p->smeMain_State == STATE_REASSOCIATING)
      {
         vStaInfo_p->smeMain_State = STATE_AUTHENTICATED_WITH_AP;
      }
      if(vStaInfo_p->isParentSession){
          // Set the trunkid as INACTIVE so that no data traffic is sent      
          mlmeApiSetTrunkIdActive(vStaInfo_p, vmacEntry_p->trunkId, FALSE, STA_TRUNK_TYPE);
      }
      #ifdef WPA_STA
      if (vStaInfo_p->staSecurityMibs.mib_PrivacyTable_p->RSNEnabled) {
          ((KeyData_t *)vStaInfo_p->keyMgmtInfoSta_p->pKeyData)->RSNDataTrafficEnabled = 0;
      }
      #endif /* WPA_STA */
#ifdef AMPDU_SUPPORT_TX_CLIENT 
	  cleanupAmpduTx(vmacSta_p, (UINT8 *)&vStaInfo_p->macMgmtMlme_ThisStaData.BssId[0]);
#endif      
      if(vStaInfo_p->AssociatedFlag)
      {
          mlmeApiEventNotification(vStaInfo_p,
                                      Tbcn,
                                      &vStaInfo_p->macMgmtMlme_ThisStaData.BssId[0], 
                                      0);
          mlmeApiUpdateLinkStatus(vStaInfo_p, LINK_DOWN);
          if(vStaInfo_p->isParentSession)
          {
              if(!vStaInfo_p->IBssStartFlag)
              {
              	  vStaInfo_p->staSecurityMibs.mib_PrivacyTable_p->RSNLinkStatus = 0;  	
                  vStaInfo_p->AssociatedFlag=0;
                  syncSrv_SndLinkLostInd(vStaInfo_p);
              }
          }
      }
   }


} // End smeStateMgr_DisassociateInd()



/******************************************************************************
 *
 * Name: smeStateMgr_ReassociateCfrm
 *
 * Description:
 *    Routine to handle the results from a previous reassociate request.
 *
 * Conditions For Use:
 *    All software components have been initialized and started.
 *
 * Arguments:
 *    Arg1 (i  ): MgmtMsg_p - Pointer to the message containing
 *                            reassociation results
 *
 * Return Value:
 *    None.
 *
 * Notes:
 *    None.
 *
 * PDL:
 *    xxx
 * END PDL
 *
 *****************************************************************************/
extern void smeStateMgr_ReassociateCfrm( UINT8 *info_p,
                                         macmgmtQ_CmdRsp_t *MgmtMsg_p )
{
   vmacStaInfo_t *vStaInfo_p = (vmacStaInfo_t *)info_p;

   smeStateMgr_UnSetCmd(info_p, cmdReAssoc);
   if (vStaInfo_p->smeMain_State == STATE_REASSOCIATING)
   {
      vStaInfo_p->smeMain_State = STATE_ASSOCIATED;
	  mlmeApiUpdateLinkStatus(vStaInfo_p, LINK_UP);
   }

   
} // End smeStateMgr_ReassociateCfrm()


/******************************************************************************
 *
 * Name: smeStateMgr_ReassociateInd
 *
 * Description:
 *    Routine to send an indication to the CB Processor Task that an
 *    unsolicited reassociation has occurred.
 *
 * Conditions For Use:
 *    All software components have been initialized and started.
 *
 * Arguments:
 *    Arg1 (i  ): MgmtMsg_p - Pointer to the message containing
 *                            reassociation results
 *
 * Return Value:
 *    None.
 *
 * Notes:
 *    None.
 *
 * PDL:
 *    xxx
 * END PDL
 *
 *****************************************************************************/
extern void smeStateMgr_ReassociateInd( UINT8 *info_p,
                                        macmgmtQ_CmdRsp_t *MgmtMsg_p )
{
   vmacStaInfo_t *vStaInfo_p = (vmacStaInfo_t *)info_p;

   if (vStaInfo_p->smeMain_State == STATE_REASSOCIATING)
   {
      vStaInfo_p->smeMain_State = STATE_ASSOCIATED;
	  mlmeApiUpdateLinkStatus(vStaInfo_p, LINK_UP);
   }

} // End smeStateMgr_ReassociateInd()

/******************************************************************************
 *
 * Name: smeStateMgr_ResetCfrm
 *
 * Description:
 *    Routine to handle the results from a previous reset request.
 *
 * Conditions For Use:
 *    All software components have been initialized and started.
 *
 * Arguments:
 *    Arg1 (i  ): MgmtMsg_p - Pointer to the message containing
 *                            reset results
 *
 * Return Value:
 *    None.
 *
 * Notes:
 *    None.
 *
 * PDL:
 *    xxx
 * END PDL
 *
 *****************************************************************************/
extern void smeStateMgr_ResetCfrm( UINT8 *info_p,
                                   macmgmtQ_CmdRsp_t *MgmtMsg_p )
{
   vmacStaInfo_t *vStaInfo_p = (vmacStaInfo_t *)info_p;

    mlmeApiEventNotification(vStaInfo_p,
                              MlmeReset_Cnfm,
                              &vStaInfo_p->macMgmtMlme_ThisStaData.BssId[0], 
                              0);
    return;
} // End smeStateMgr_ResetCfrm()


extern void smeStateMgr_PwrMgmtCfrm( macmgmtQ_CmdRsp_t *MgmtMsg_p )
{
   /* TBD */

} // End smeStateMgr_PwrMgmtCfrm()


////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

/*************************************************************************
* Function: smeStateMgr_SndAuthCmd
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern SINT32 smeStateMgr_SndAuthCmd( UINT8 *info_p,
                                    IEEEtypes_MacAddr_t   *peerAddr)
{
   vmacStaInfo_t *vStaInfo_p = (vmacStaInfo_t *)info_p;
   macmgmtQ_CmdReq_t   *macMgmtMsg_p;
   vmacEntry_t  *vmacEntry_p;

   vmacEntry_p = (vmacEntry_t *)vStaInfo_p->vMacEntry_p;

   //vStaInfo_p->smeMain_State = STATE_JOINED;

   if ((macMgmtMsg_p = (macmgmtQ_CmdReq_t *)mlmeApiAllocSmeMsg()) == NULL)
   {
	   return MLME_FAILURE;
   }
   memcpy((void *)&macMgmtMsg_p->targetAddr[0], 
						(const void *)&vmacEntry_p->vmacAddr[0],
						sizeof(IEEEtypes_MacAddr_t));
   macMgmtMsg_p->CmdType = MlmeAuth_Req;
   macMgmtMsg_p->Body.AuthCmd.AuthType    = vStaInfo_p->staSecurityMibs.mib_AuthAlg_p->Type;
   macMgmtMsg_p->Body.AuthCmd.FailTimeout = AUTH_TIME;
   memcpy(&(macMgmtMsg_p->Body.AuthCmd.PeerStaAddr),
                         peerAddr,
                         sizeof(IEEEtypes_MacAddr_t));

   vStaInfo_p->AuthRetryCount = 0;
   memcpy(&(vStaInfo_p->LastAuthMsg),
                         macMgmtMsg_p,
                         sizeof(macmgmtQ_CmdReq_t));
   if (!mlmeApiSendSmeMsg(macMgmtMsg_p))
   {
      mlmeApiFreeSmeMsg((macmgmtQ_CmdBuf_t *)macMgmtMsg_p);
      return MLME_FAILURE;
   }
   
   return MLME_SUCCESS;
}


/************  Child todo start :: need to rework these things for Child ********/


/**** These function are call from other module ******/

extern void syncSrv_StatusCheckTimeOut(UINT8 *data);



   /* Flag used to indicate if a scan should be performed at start up */

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
SINT32 smeStartBss(UINT8 phyMacIndx)
{
    vmacId_t clientVMacId;
    vmacStaInfo_t *vStaInfo_p;
	vmacEntry_t  *vmacEntry_p;
    UINT8        parentMacAddr[IEEEtypes_ADDRESS_SIZE];
    int clientType=0;
    UINT32           clientSrvId;
    UINT32           curVMacActiveSrv;

	clientVMacId=parentGetVMacId(phyMacIndx);
    if((vStaInfo_p = (vmacStaInfo_t *)vmacGetVMacStaInfo(clientVMacId)) == NULL)
    {
        return MLME_FAILURE;
    }
	vmacEntry_p = (vmacEntry_t *)vStaInfo_p->vMacEntry_p;
    if(smePendingCmd((UINT8 *)vStaInfo_p))
    {
        return MLME_FAILURE;
    }
    curVMacActiveSrv = vmacQueryActiveSrv(phyMacIndx, 0xffffffff);

    if(curVMacActiveSrv)
    {
        if((vStaInfo_p->mib_WB_p->opMode != MLME_CMD_CLIENT_IBSS_START) && 
            (vStaInfo_p->bssDescProfile_p->PhyParamSet.DsParamSet.CurrentChan != mlmeApiGetRfChannel(vStaInfo_p)))
        {
            return MLME_FAILURE;
        }
        vStaInfo_p->bssDescProfile_p->PhyParamSet.DsParamSet.CurrentChan = mlmeApiGetRfChannel(vStaInfo_p);
    }
    else
    {
        vStaInfo_p->staSystemMibs.PhyDSSSTable_p->CurrChan = vStaInfo_p->bssDescProfile_p->PhyParamSet.DsParamSet.CurrentChan;
    }
    
    if(mib_childMode[phyMacIndx])
    {
//        clientType = (vmacEntry_p->phyHwMacIndx == MAC_1)?LiberoClient:DaemonClient;
        mlmeApiMamGetMACAddress(vStaInfo_p, clientType, &parentMacAddr[0]);
    }
    else
    {
        if( !mlmeApiMamGetHostMACAddress(vStaInfo_p, &parentMacAddr[0]) )
		{
			return MLME_FAILURE;
		}
    }
    syncSrv_SetStatusTimer(vStaInfo_p, 0);
    syncSrv_SetKeepAliveTimer(vStaInfo_p, 0);
    memcpy((void *)&vmacEntry_p->vmacAddr[0], 
		  (const void *)&parentMacAddr[0],
		  IEEEtypes_ADDRESS_SIZE);
    
    if(vStaInfo_p->isParentSession){
        // Set the trunkid as INACTIVE so that no data traffic is sent
        mlmeApiSetTrunkIdActive(vStaInfo_p, vmacEntry_p->trunkId, FALSE, STA_TRUNK_TYPE);
    }
	smeSendResetCmd_Sta(phyMacIndx, 0);
    clientSrvId = (vmacEntry_p->phyHwMacIndx == MAC_1)?VMAC_SRV_CLIENT_M1:VMAC_SRV_CLIENT_M0;
    vmacActiveSrvId(phyMacIndx, clientSrvId);
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
SINT32 smeStopBss(UINT8 phyMacIndx)
{
    vmacId_t clientVMacId;
    vmacStaInfo_t *vStaInfo_p;
	vmacEntry_t  *vmacEntry_p;
    int clientType =0;
    UINT32           clientSrvId;
#ifdef AMPDU_SUPPORT_TX_CLIENT
    struct net_device *staDev = NULL;
    struct wlprivate *stapriv = NULL;
    vmacApInfo_t *vmacSta_p = NULL;
#endif    
    
	clientVMacId=parentGetVMacId(phyMacIndx);
    if((vStaInfo_p = (vmacStaInfo_t *)vmacGetVMacStaInfo(clientVMacId)) == NULL)
    {
        return MLME_FAILURE;
    }
	vmacEntry_p = (vmacEntry_t *)vStaInfo_p->vMacEntry_p;
#ifdef AMPDU_SUPPORT_TX_CLIENT
    staDev = (struct net_device *)vmacEntry_p->privInfo_p;
    stapriv = NETDEV_PRIV_P(struct wlprivate, staDev);
    vmacSta_p = (vmacApInfo_t *)stapriv->vmacSta_p;
#endif
    if(smePendingCmd((UINT8 *)vStaInfo_p))
    {
        smeClearCmdHistory((UINT8 *)vStaInfo_p);
    }
	TimerRemove(&vStaInfo_p->assocTimer);
	TimerRemove(&vStaInfo_p->authTimer);
	TimerRemove(&vStaInfo_p->scaningTimer);
#ifdef IEEE80211_DH
	TimerRemove(&vStaInfo_p->station11hTimer);
#endif
#ifdef WMON
	TimerRemove(&vStaInfo_p->stationWMONTimer);
#endif
#ifdef WPA_STA
	TimerRemove(&vStaInfo_p->keyMgmtInfoSta_p->keyMgmtStaHskHsm.rsnSecuredTimer);
#endif

    syncSrv_SetStatusTimer(vStaInfo_p, 0);
    syncSrv_SetKeepAliveTimer(vStaInfo_p, 0);
     /* Send out DeAuth if Associated to AP */
    if(vStaInfo_p->AssociatedFlag && !vStaInfo_p->mib_WB_p->opMode)
    {
#ifdef AMPDU_SUPPORT_TX_CLIENT
	    cleanupAmpduTx(vmacSta_p, (UINT8 *)&vStaInfo_p->macMgmtMlme_ThisStaData.BssId[0]);
#endif
		vStaInfo_p->staSecurityMibs.mib_PrivacyTable_p->RSNLinkStatus = 0;  	
        vStaInfo_p->AssociatedFlag = 0;
        if(authSrv_SndDeAuthMsg( vStaInfo_p, 
                                    &vStaInfo_p->macMgmtMlme_ThisStaData.BssId,
                                    &vStaInfo_p->macMgmtMlme_ThisStaData.BssId,
                                    IEEEtypes_REASON_DEAUTH_LEAVING) == MLME_SUCCESS)
        {
            /* delay so packet get send out before hw MAC get reset */
            os_TaskDelay(100);  /* delay so that the msg is tx by hw mac */
        }
    }
    //clientType = (vmacEntry_p->phyHwMacIndx == MAC_1)?LiberoClient:DaemonClient;
    mlmeApiMamGetMACAddress(vStaInfo_p, clientType, &vmacEntry_p->vmacAddr[0]);
    mlmeApiSetTrunkIdActive(vStaInfo_p, vmacEntry_p->trunkId, FALSE, STA_TRUNK_TYPE);
    smeSendResetCmd_Sta(phyMacIndx, 1);
    clientSrvId = (vmacEntry_p->phyHwMacIndx == MAC_1)?VMAC_SRV_CLIENT_M1:VMAC_SRV_CLIENT_M0;
    vmacDeActiveSrvId(phyMacIndx, clientSrvId);
    memset(&vStaInfo_p->linkInfo, 0, sizeof(iw_linkInfo_t));
    mlmeApiUpdateLinkStatus(vStaInfo_p, LINK_DOWN);
    return MLME_SUCCESS;
}

SINT32 smeApiValidateChildModeChange(UINT8 phyMacIndx)
{
    UINT32           clientSrvId;
    UINT32           curVMacActiveSrv;

    curVMacActiveSrv = vmacQueryActiveSrv(phyMacIndx, (UINT32)-1);
    clientSrvId = (phyMacIndx == MAC_1)
                  ? VMAC_SRV_CLIENT_M1
                  : VMAC_SRV_CLIENT_M0;

    // Client services should NOT be active on the
    // specified MAC during Child mode change
    if (curVMacActiveSrv & clientSrvId)
    {
        return MLME_FAILURE;
    }
    return MLME_SUCCESS;
}


/*********** Tmp End ********************************/


/******************************************************************************
 *
 * Name:  
 *
 * Description:
 *
 * Conditions For Use:
 *
 * Arguments:
 *   None.
 *
 * Return Value:
 *   None.
 *
 * Notes:
 *   None.
 *
 * PDL:
 * END PDL
 *
 *****************************************************************************/
SINT32 smeSendResetCmd_Sta(UINT8 macIndex, int quiet)
{
    macmgmtQ_CmdReq_t *resetMsg_p;
    vmacStaInfo_t *vStaInfo_p;
	vmacId_t vMacId_Inst;
	vmacEntry_t  *vmacEntry_p;
   
	vMacId_Inst = parentGetVMacId(macIndex);
    if((vStaInfo_p = (vmacStaInfo_t *)vmacGetVMacStaInfo(vMacId_Inst)) == NULL)
    {
        return MLME_FAILURE;
    }


	vmacEntry_p = (vmacEntry_t *)vStaInfo_p->vMacEntry_p;
    
    /* Send out DeAuth if Associated to AP */
    if(vStaInfo_p->AssociatedFlag && !vStaInfo_p->mib_WB_p->opMode)
    {
    	vStaInfo_p->staSecurityMibs.mib_PrivacyTable_p->RSNLinkStatus = 0;  	
        vStaInfo_p->AssociatedFlag = 0;
        if(authSrv_SndDeAuthMsg( vStaInfo_p, 
                                    &vStaInfo_p->macMgmtMlme_ThisStaData.BssId,
                                    &vStaInfo_p->macMgmtMlme_ThisStaData.BssId,
                                    IEEEtypes_REASON_DEAUTH_LEAVING) == MLME_SUCCESS)
        {
            /* delay so packet get send out before hw MAC get reset */
            os_TaskDelay(100);  /* delay so that the msg is tx by hw mac */
        }
    }

    /* Remove associated AP from UR station database, intially added for
       pseudo ether port support. */
    //mlmeApiDelStaDbEntry(vStaInfo_p, &vStaInfo_p->macMgmtMlme_ThisStaData.BssId[0]);

    /* Reinit MIB to handle runtime config changes by customers */
    if( quiet == 2 )
    {
        /* Inform current connected rootAP as our config changed. */
        //quiet = 0;
		quiet = 1;
    }
    /* Set UR station mode from AP mode. */
    mlmeApiSetStaMode(vStaInfo_p, *vStaInfo_p->mib_StaMode_p);

    /* end init MIB */

    if ( (resetMsg_p = (macmgmtQ_CmdReq_t *)mlmeApiAllocSmeMsg()) != NULL)
    {
		memcpy((void *)&resetMsg_p->targetAddr[0], 
		  (const void *)&vmacEntry_p->vmacAddr[0],
		  sizeof(IEEEtypes_MacAddr_t));
        resetMsg_p->CmdType = MlmeReset_Req;
        resetMsg_p->Body.ResetCmd.SetDefaultMIB = TRUE;
        resetMsg_p->Body.ResetCmd.mode = quiet;
        if (!mlmeApiSendSmeMsg(resetMsg_p))
        {
           
           mlmeApiFreeSmeMsg((macmgmtQ_CmdBuf_t *)resetMsg_p);
           return MLME_FAILURE;
        }
    }
    return MLME_SUCCESS;
}


#ifdef STA_QOS
int IsURQSta(UINT macIndex)
{
    vmacStaInfo_t *vStaInfo_p;
    vmacId_t vMacId_Inst;
   
	vMacId_Inst = parentGetVMacId(macIndex);
    if((vStaInfo_p = (vmacStaInfo_t *)vmacGetVMacStaInfo(vMacId_Inst)) == NULL)
    {
        return 1;
    }

    return( vStaInfo_p->macMgmtMlme_ThisStaData.IsStaQosSTA);
}
#endif

extern SINT32 keyMgmtAssocSrv_DisAssocCmd(UINT8 macIndex, IEEEtypes_DisassocCmd_t *DisassocCmd_p )
{
    vmacStaInfo_t *vStaInfo_p;
	vmacId_t vMacId_Inst;
   
	vMacId_Inst = parentGetVMacId(macIndex);
    if((vStaInfo_p = (vmacStaInfo_t *)vmacGetVMacStaInfo(vMacId_Inst)) == NULL)
    {
        return 1;
    }
    assocSrv_DisAssocCmd(vStaInfo_p, DisassocCmd_p);
    return 0;
}


extern UINT8 *GetParentStaBSSID(UINT8 macIndex)
{
    vmacStaInfo_t *vStaInfo_p;

    vStaInfo_p = (vmacStaInfo_t *)vmacGetVMacStaInfo(parentGetVMacId(macIndex));

    if( vStaInfo_p )
    {
        return( &(vStaInfo_p->macMgmtMlme_ThisStaData.BssId[0]) );
    }
    return NULL;
}


extern UINT8 *GetURMACAddr(UINT8 macIndex)
{
    vmacEntry_t     *vmacEntry_p;
    vmacStaInfo_t   *vStaInfo_p;
    UINT8           *pMACAddr = NULL;

    vStaInfo_p = (vmacStaInfo_t *)vmacGetVMacStaInfo(parentGetVMacId(macIndex));

    if( vStaInfo_p )
    {
        vmacEntry_p = (vmacEntry_t *)vStaInfo_p->vMacEntry_p;

        if( vmacEntry_p )
        {
            pMACAddr = vmacEntry_p->vmacAddr;
        }
    }

    return pMACAddr;
}



extern IEEEtypes_MacMgmtStates_e GetURStaMacMgmtState(UINT8 macIndex)
{
    vmacStaInfo_t *vStaInfo_p;

    vStaInfo_p = (vmacStaInfo_t *)vmacGetVMacStaInfo(parentGetVMacId(macIndex));

    if( vStaInfo_p )
    {
        return vStaInfo_p->macMgmtMain_State;
    }
    else
    {
        return STATE_IDLE;
    }
}


extern macmgmtQ_MgmtMsg_t *GetURStaBeaconBuffer(UINT8 macIndex)
{
    vmacStaInfo_t *vStaInfo_p;

    vStaInfo_p = (vmacStaInfo_t *)vmacGetVMacStaInfo(parentGetVMacId(macIndex));

    if( vStaInfo_p )
    {
        return vStaInfo_p->BcnBuffer_p;
    }
    else
    {
        return NULL;
    }
}


extern macmgmtQ_MgmtMsg_t *GetURStaProbeRspBuffer(UINT8 macIndex)
{
    vmacStaInfo_t *vStaInfo_p;

    vStaInfo_p = (vmacStaInfo_t *)vmacGetVMacStaInfo(parentGetVMacId(macIndex));

    if( vStaInfo_p )
    {
        return vStaInfo_p->PrbRspBuf_p;
    }
    else
    {
        return NULL;
    }
}


extern UINT8 IsURStaAdhocActive(UINT8 macIndex)
{
    UINT8           fRetVal = 0;
    vmacStaInfo_t   *vStaInfo_p;

    vStaInfo_p = (vmacStaInfo_t *)vmacGetVMacStaInfo(parentGetVMacId(macIndex));

    if( vStaInfo_p )
    {
        if( (vStaInfo_p->macMgmtMlme_ThisStaData.BssType == BSS_INDEPENDENT) && (vStaInfo_p->AssociatedFlag) )
            fRetVal = 1;
    }

    return fRetVal;
}


extern BOOLEAN smeGetStaLinkInfo(vmacId_t mlme_vMacId,
                                    UINT8 *AssociatedFlag_p,
                                    UINT8 *bssId_p)
{
    vmacEntry_t  *vmacEntry_p;
    vmacStaInfo_t *vStaInfo_p;

    if((vmacEntry_p = vmacGetVMacEntryById(mlme_vMacId)) == NULL)
    {
        return FALSE;
    }
    if((vmacEntry_p->modeOfService == VMAC_MODE_AP)
       || ((vStaInfo_p=(vmacStaInfo_t *)vmacEntry_p->info_p) == NULL))
    {
        return FALSE;
    }
    vStaInfo_p = (vmacStaInfo_t *)vmacEntry_p->info_p;
    *AssociatedFlag_p = vStaInfo_p->AssociatedFlag;
    memcpy(bssId_p, 
           &vStaInfo_p->macMgmtMlme_ThisStaData.BssId[0], 
           sizeof(IEEEtypes_MacAddr_t));

    return TRUE;
}


/************  Child todo end ********/

/*************************************************************************
* Function: smeStateMgr_SndJoinCmd
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern SINT32 smeStateMgr_SndJoinCmd( UINT8 *info_p,
                                    IEEEtypes_BssDesc_t *bssDesc_p)
{
    vmacStaInfo_t *vStaInfo_p = (vmacStaInfo_t *)info_p;
    macmgmtQ_CmdReq_t   *macMgmtMsg_p;
    vmacEntry_t  *vmacEntry_p;

    vmacEntry_p = (vmacEntry_t *)vStaInfo_p->vMacEntry_p;

    #ifdef WPA_STA
    if(vStaInfo_p->staSecurityMibs.mib_PrivacyTable_p->RSNEnabled
       && vStaInfo_p->keyMgmtInfoSta_p->sta_MIC_Error.MICCounterMeasureEnabled
       && vStaInfo_p->keyMgmtInfoSta_p->sta_MIC_Error.disableStaAsso)
    {
        return MLME_FAILURE;
    }
    #endif /* WPA_STA */

    if(smePendingCmd(info_p))
    {
        syncSrv_CmdExceptionHandler(vStaInfo_p);
    }
    if ((macMgmtMsg_p = (macmgmtQ_CmdReq_t *)mlmeApiAllocSmeMsg()) == NULL)
    {
        return MLME_FAILURE;
    }

    memcpy((void *)&macMgmtMsg_p->targetAddr[0], 
           (const void *)&vmacEntry_p->vmacAddr[0],
           sizeof(IEEEtypes_MacAddr_t));

    macMgmtMsg_p->CmdType = MlmeJoin_Req;
    memcpy((void *)&macMgmtMsg_p->Body.JoinCmd.BssDesc, 
           (const void *)bssDesc_p,
           sizeof(IEEEtypes_BssDesc_t));

    if (!mlmeApiSendSmeMsg(macMgmtMsg_p))
    {
        //
        // Writing to the MAC Management Task's SME queue failed,
        // so we had better release the buffer, or it will forever
        // be tied up
        //
        mlmeApiFreeSmeMsg((macmgmtQ_CmdBuf_t *)macMgmtMsg_p);
        return MLME_FAILURE;
    }
    else
    {
        smeStateMgr_SetCmd(info_p, cmdJoin);
    }
    vStaInfo_p->smeMain_State = STATE_JOINING;
    memcpy(&(vStaInfo_p->LastJoinMsg),
             macMgmtMsg_p,
             sizeof(macmgmtQ_CmdReq_t));
    /* Get and start a status timer to check on certain status periodically */
    syncSrv_SetStatusTimer(vStaInfo_p, 1);
    if(mib_childMode[vmacEntry_p->phyHwMacIndx])
        syncSrv_SetKeepAliveTimer(vStaInfo_p, 1);
    return MLME_SUCCESS;
}

/*************************************************************************
* Function: smeParseIeType
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern IEEEtypes_InfoElementHdr_t *smeParseIeType(UINT8 ieType,
                               UINT8 *ieBuf_p,
                               UINT16 ieBufLen)
{
    UINT8 *ieBufEnd_p;
    UINT8 *ieCurrent_p;
    IEEEtypes_InfoElementHdr_t *IE_p;

    ieCurrent_p = ieBuf_p;
    ieBufEnd_p = ieBuf_p + ieBufLen;
        
    while(ieCurrent_p < ieBufEnd_p)
    {
        IE_p = ( IEEEtypes_InfoElementHdr_t *)ieCurrent_p;
        if(IE_p->ElementId == ieType)
        {
            return IE_p;
        }
        ieCurrent_p += (IE_p->Len + sizeof(IEEEtypes_InfoElementHdr_t));
    }
    return NULL;
}

/*************************************************************************
* Function: smeSetBssProfile
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern SINT32 smeSetBssProfile(UINT8 macIndex, 
                               UINT8 *bssid,
                               IEEEtypes_CapInfo_t capInfo,
                               UINT8 *ieBuf_p,
                               UINT16 ieBufLen,
                               BOOLEAN isApMrvl)
{
    vmacStaInfo_t   *vStaInfo_p;
    UINT8 ssidBuf[IEEEtypes_SSID_SIZE];
    UINT8 rateBuf[IEEEtypes_MAX_DATA_RATES_G];
    UINT8 channel;
    UINT8 rateOffset;
    IEEEtypes_InfoElementHdr_t *IE_p;
    UINT32 addedVendorLen;
    UINT8 *ieCurrent_p;
    vmacEntry_t  *vmacEntry_p = NULL;
	struct net_device *staDev = NULL;
	struct wlprivate *stapriv = NULL;
    vmacApInfo_t *vmacSta_p = NULL;

    vStaInfo_p = (vmacStaInfo_t *)vmacGetVMacStaInfo(parentGetVMacId(macIndex));

    if( vStaInfo_p )
    {
        memset(&ssidBuf[0], 0, IEEEtypes_SSID_SIZE);
        memset(&rateBuf[0], 0, 14);
        rateOffset=0;
        /* Parse here */
        if((IE_p = smeParseIeType(SSID,
                                  ieBuf_p,
                                  ieBufLen)) == NULL)
        {
            return MLME_FAILURE;
        }
        if(IE_p->Len > IEEEtypes_SSID_SIZE)
        {
            return MLME_FAILURE;
        }
        memcpy(&ssidBuf[0], (UINT8 *)IE_p + sizeof(IEEEtypes_InfoElementHdr_t), IE_p->Len);
        util_CopyList(vStaInfo_p->staSystemMibs.mib_StaCfg_p->DesiredSsId,
                      &ssidBuf[0],
                      IEEEtypes_SSID_SIZE);


        if((IE_p = smeParseIeType(DS_PARAM_SET,
                                  ieBuf_p,
                                  ieBufLen)) == NULL)
        {
            return MLME_FAILURE;
        }
        if(IE_p->Len > 1)
        {
            return MLME_FAILURE;
        }
        channel = *((UINT8 *)IE_p + sizeof(IEEEtypes_InfoElementHdr_t));

        if((IE_p = smeParseIeType(SUPPORTED_RATES,
                                  ieBuf_p,
                                  ieBufLen)) == NULL)
        {
            return MLME_FAILURE;
        }
        if(IE_p->Len > 14)
        {
            return MLME_FAILURE;
        }
        memcpy(&rateBuf[rateOffset], (UINT8 *)IE_p + sizeof(IEEEtypes_InfoElementHdr_t), IE_p->Len);
        rateOffset += IE_p->Len;

        if((IE_p = smeParseIeType(EXT_SUPPORTED_RATES,
                                  ieBuf_p,
                                  ieBufLen)) != NULL)
        {
            if((IE_p->Len + rateOffset) > 14)
            {
                memcpy(&rateBuf[rateOffset], (UINT8 *)IE_p + sizeof(IEEEtypes_InfoElementHdr_t),  14 - rateOffset);
            }
            else
            {
                memcpy(&rateBuf[rateOffset], (UINT8 *)IE_p + sizeof(IEEEtypes_InfoElementHdr_t), IE_p->Len);
            }
        }
        /* Populate the BSS Descriptor */
        memset((UINT8 *)vStaInfo_p->bssDescProfile_p, 0, sizeof(IEEEtypes_BssDesc_t));
        memcpy(&vStaInfo_p->bssDescProfile_p->BssId, bssid, 6);
        memcpy(&vStaInfo_p->bssDescProfile_p->SsId, &ssidBuf[0], 32);
        if(capInfo.Ess)
        {
            vStaInfo_p->bssDescProfile_p->BssType = BSS_INFRASTRUCTURE;
            vStaInfo_p->bssDescProfile_p->SsParamSet.CfParamSet.ElementId = CF_PARAM_SET;
            vStaInfo_p->bssDescProfile_p->SsParamSet.CfParamSet.Len = 6;
            vStaInfo_p->bssDescProfile_p->SsParamSet.CfParamSet.CfpCnt = 0;
            vStaInfo_p->bssDescProfile_p->SsParamSet.CfParamSet.CfpPeriod = 0;
            vStaInfo_p->bssDescProfile_p->SsParamSet.CfParamSet.CfpMaxDuration = 0;
            vStaInfo_p->bssDescProfile_p->SsParamSet.CfParamSet.CfpDurationRemaining = 0;
        }
        else
        {
            vStaInfo_p->bssDescProfile_p->BssType = BSS_INDEPENDENT;
            vStaInfo_p->bssDescProfile_p->SsParamSet.IbssParamSet.ElementId  = IBSS_PARAM_SET;
            vStaInfo_p->bssDescProfile_p->SsParamSet.IbssParamSet.Len        = 2;
            vStaInfo_p->bssDescProfile_p->SsParamSet.IbssParamSet.AtimWindow = 0;
        }
        vStaInfo_p->bssDescProfile_p->BcnPeriod = 100;
        vStaInfo_p->bssDescProfile_p->DtimPeriod = 4;
        memset(&vStaInfo_p->bssDescProfile_p->Tstamp[0], 0, sizeof(IEEEtypes_TimeStamp_t));
        memset(&vStaInfo_p->bssDescProfile_p->StartTs[0], 0, sizeof(IEEEtypes_TimeStamp_t));
        vStaInfo_p->bssDescProfile_p->PhyParamSet.DsParamSet.ElementId = DS_PARAM_SET;
        vStaInfo_p->bssDescProfile_p->PhyParamSet.DsParamSet.Len = 1;
        vStaInfo_p->bssDescProfile_p->PhyParamSet.DsParamSet.CurrentChan = channel;
        vStaInfo_p->bssDescProfile_p->Cap = capInfo;
        vStaInfo_p->isApMrvl = isApMrvl;
        memcpy(&vStaInfo_p->bssDescProfile_p->DataRates[0], &rateBuf[0], IEEEtypes_MAX_DATA_RATES_G);
        /* HT Element */
        if((IE_p = smeParseIeType(HT,
                                  ieBuf_p,
                                  ieBufLen)) != NULL)
        {
            if(!((IE_p->Len + sizeof(IEEEtypes_InfoElementHdr_t)) > sizeof(IEEEtypes_HT_Element_t)))
            {
                memcpy(&vStaInfo_p->bssDescProfile_p->HTElement, (UINT8 *)IE_p , IE_p->Len + sizeof(IEEEtypes_InfoElementHdr_t));

                /* Override MAX AMSDU size based on local MIB settings */
    			vmacEntry_p = sme_GetParentVMacEntry(macIndex);
    			staDev = (struct net_device *)vmacEntry_p->privInfo_p;
    			stapriv = NETDEV_PRIV_P(struct wlprivate, staDev);
    			vmacSta_p = (vmacApInfo_t *)stapriv->vmacSta_p;
                if(	*(vmacSta_p->Mib802dot11->mib_HtGreenField) == 0)
                	vStaInfo_p->bssDescProfile_p->HTElement.HTCapabilitiesInfo.GreenField = 0;
				if (*(vmacSta_p->Mib802dot11->mib_HtStbc) == 0)
				{
                	vStaInfo_p->bssDescProfile_p->HTElement.HTCapabilitiesInfo.RxSTBC = 0;
                	vStaInfo_p->bssDescProfile_p->HTElement.HTCapabilitiesInfo.TxSTBC = 0;
				}
                vStaInfo_p->bssDescProfile_p->HTElement.ExtHTCapabilitiesInfo = 0x00;

				if(*(vmacSta_p->Mib802dot11->pMib_11nAggrMode)& WL_MODE_AMSDU_TX_8K)
                    vStaInfo_p->bssDescProfile_p->HTElement.HTCapabilitiesInfo.MaxAMSDUSize = 1;
				else
                	vStaInfo_p->bssDescProfile_p->HTElement.HTCapabilitiesInfo.MaxAMSDUSize = 0;
            }
        }
     
        
        if((IE_p = smeParseIeType(ADD_HT,
                                  ieBuf_p,
                                  ieBufLen)) != NULL)
        {     				        		
            if(!((IE_p->Len + sizeof(IEEEtypes_InfoElementHdr_t)) > sizeof(IEEEtypes_Add_HT_Element_t)))
            {
                memcpy(&vStaInfo_p->bssDescProfile_p->ADDHTElement, (UINT8 *)IE_p , IE_p->Len + sizeof(IEEEtypes_InfoElementHdr_t));                	
            }
      
        }

		
		/*VHT Element*/
		 if((IE_p = smeParseIeType(VHT_CAP,
                                  ieBuf_p,
                                  ieBufLen)) != NULL)
		 {
			if(!((IE_p->Len + sizeof(IEEEtypes_InfoElementHdr_t)) > sizeof(IEEEtypes_VhtCap_t)))
			{
				memcpy(&vStaInfo_p->bssDescProfile_p->VHTCap, (UINT8 *)IE_p , IE_p->Len + sizeof(IEEEtypes_InfoElementHdr_t));
			}
		 }

		 if((IE_p = smeParseIeType(VHT_OPERATION,
                                  ieBuf_p,
                                  ieBufLen)) != NULL)
		 {
			if(!((IE_p->Len + sizeof(IEEEtypes_InfoElementHdr_t)) > sizeof(IEEEtypes_VhOpt_t)))
			{
				memcpy(&vStaInfo_p->bssDescProfile_p->VHTOp, (UINT8 *)IE_p , IE_p->Len + sizeof(IEEEtypes_InfoElementHdr_t));                	
			}
		 }

		
        /* RSN Element (WPA2) */
        if((IE_p = smeParseIeType(RSN_IEWPA2,
                                  ieBuf_p,
                                  ieBufLen)) != NULL)
        {
            if(!((IE_p->Len + sizeof(IEEEtypes_InfoElementHdr_t)) > sizeof(IEEEtypes_RSN_IE_WPA2_t)))
            {
                memcpy(&vStaInfo_p->bssDescProfile_p->Wpa2Element, (UINT8 *)IE_p , IE_p->Len + sizeof(IEEEtypes_InfoElementHdr_t));
            }
        }
        /* Lastly parse for vendor specific IEs */
        addedVendorLen = 0;
        ieCurrent_p = ieBuf_p;
        while(ieCurrent_p < (ieBuf_p + ieBufLen))
        {
            IE_p = (IEEEtypes_InfoElementHdr_t *)ieCurrent_p;
            if(IE_p->ElementId == PROPRIETARY_IE)
            {
                if(!(( addedVendorLen + IE_p->Len + sizeof(IEEEtypes_InfoElementHdr_t)) > STA_VENDOR_IE_BUF_MAX_LEN))
                {
                    memcpy(&vStaInfo_p->bssDescProfile_p->vendorBuf[addedVendorLen], (UINT8 *)IE_p , IE_p->Len + sizeof(IEEEtypes_InfoElementHdr_t));
                    addedVendorLen += IE_p->Len + sizeof(IEEEtypes_InfoElementHdr_t);
                    vStaInfo_p->bssDescProfile_p->vendorIENum++;
                    vStaInfo_p->bssDescProfile_p->vendorTotalLen = addedVendorLen;
                }
            }
            ieCurrent_p += (IE_p->Len + sizeof(IEEEtypes_InfoElementHdr_t));
        }
    }
    return MLME_SUCCESS;
}

#ifdef CLIENT_SUPPORT
extern SINT32 smeCopyBssProfile(UINT8 macIndex, MRVL_SCAN_ENTRY *target)
{
    vmacStaInfo_t   *vStaInfo_p;
	UINT8 *vendorIE = NULL ;
	UINT8 WPAOUI[] = {0x00, 0x50, 0xF2, 0x01};
	UINT8 WPSOUI[] = {0x00, 0x50, 0xF2, 0x04};
	int	vendorTotalLen = 0 ;
	UINT8 ielen = 0 ;

    if((vStaInfo_p = (vmacStaInfo_t *)vmacGetVMacStaInfo(parentGetVMacId(macIndex))) == NULL)
    {
        return MLME_FAILURE;
    }

	target->dirty = 1;
    memcpy(target->result.bssid, &vStaInfo_p->bssDescProfile_p->BssId[0], 6);
    memcpy(target->result.ssid, &vStaInfo_p->bssDescProfile_p->SsId[0], IEEEtypes_SSID_SIZE);
    memcpy(target->result.rsn_ie, &vStaInfo_p->bssDescProfile_p->Wpa2Element, vStaInfo_p->bssDescProfile_p->Wpa2Element.Len + 2);
	vendorIE = &vStaInfo_p->bssDescProfile_p->vendorBuf[0] ;
	vendorTotalLen = vStaInfo_p->bssDescProfile_p->vendorTotalLen ;
	while( vendorTotalLen > 0)
	{
		ielen = (UINT8)vendorIE[1] ;
		if( memcmp( &vendorIE[2], WPAOUI, 4 ) == 0 )
		{
    		memcpy(target->result.wpa_ie, vendorIE, ielen + 2);
			target->result.wpa_ie_len = ielen + 2 ;
		}
		else if( memcmp( &vendorIE[2], WPSOUI, 4 ) == 0 )
		{
    		memcpy(target->result.wps_ie, vendorIE, ielen + 2 );
			target->result.wps_ie_len = ielen + 2 ;
		}
		vendorTotalLen -= (ielen + 2) ;
		vendorIE += ielen + 2 ;
		
	}
	/* At the end reset the BSS Profile */
    memset((UINT8 *)vStaInfo_p->bssDescProfile_p, 0, sizeof(IEEEtypes_BssDesc_t));
    return MLME_SUCCESS;
}
#endif // MRVL_WPS_CLIENT

/*************************************************************************
* Function: smeGetBssProfile
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern SINT32 smeGetBssProfile(UINT8 macIndex, 
                               UINT8 *bssid,
                               IEEEtypes_CapInfo_t *capInfo,
                               UINT8 *ieBuf_p,
                               UINT16 *ieBufLen)
{
    vmacStaInfo_t   *vStaInfo_p;
    IEEEtypes_InfoElementHdr_t *IE_p;
    UINT16  packedIeLen = 0;
    UINT8   i, dataRateLen = 0;
    BOOLEAN moreRate = TRUE;

    if((vStaInfo_p = (vmacStaInfo_t *)vmacGetVMacStaInfo(parentGetVMacId(macIndex))) == NULL)
    {
        return MLME_FAILURE;
    }

    memcpy(bssid, &vStaInfo_p->bssDescProfile_p->BssId[0], 6);
    *capInfo = vStaInfo_p->bssDescProfile_p->Cap;
    /* Pack in IEs */
    /* SSID */
    IE_p = (IEEEtypes_InfoElementHdr_t *)ieBuf_p;
    IE_p->ElementId = SSID;
    IE_p->Len = strlen(vStaInfo_p->bssDescProfile_p->SsId);
    packedIeLen += sizeof(IEEEtypes_InfoElementHdr_t);
    memcpy(ieBuf_p + packedIeLen, vStaInfo_p->bssDescProfile_p->SsId, IE_p->Len);
    packedIeLen += IE_p->Len;
    /* SUPPORTED_RATES */
    IE_p = (IEEEtypes_InfoElementHdr_t *)(ieBuf_p + packedIeLen);
    IE_p->ElementId = SUPPORTED_RATES;
    packedIeLen += sizeof(IEEEtypes_InfoElementHdr_t);
    for(i = 0; (i < 8) && moreRate; i++)
    {
        if(vStaInfo_p->bssDescProfile_p->DataRates[i] == 0)
        {
            moreRate = FALSE;
        }
        else
        {
            *(ieBuf_p + packedIeLen) = vStaInfo_p->bssDescProfile_p->DataRates[i];
            packedIeLen +=1;
            dataRateLen++;
        }
    }
    IE_p->Len = dataRateLen;
    /* DS_PARAM_SET */
    IE_p = (IEEEtypes_InfoElementHdr_t *)(ieBuf_p + packedIeLen);
    IE_p->ElementId = DS_PARAM_SET;
    IE_p->Len = sizeof(UINT8);
    packedIeLen += sizeof(IEEEtypes_InfoElementHdr_t);
    *(ieBuf_p + packedIeLen) = vStaInfo_p->bssDescProfile_p->PhyParamSet.DsParamSet.CurrentChan;
    packedIeLen += sizeof(UINT8);
    /* EXT_SUPPORTED_RATES */
    dataRateLen = 0;
    IE_p = (IEEEtypes_InfoElementHdr_t *)(ieBuf_p + packedIeLen);
    IE_p->ElementId = EXT_SUPPORTED_RATES;
    packedIeLen += sizeof(IEEEtypes_InfoElementHdr_t);
    for(i = 8; (i < 14) && moreRate; i++)
    {
        if(vStaInfo_p->bssDescProfile_p->DataRates[i] == 0)
        {
            moreRate = FALSE;
        }
        else
        {
            *(ieBuf_p + packedIeLen) = vStaInfo_p->bssDescProfile_p->DataRates[i];
            packedIeLen +=1;
            dataRateLen++;
        }
    }
    IE_p->Len = dataRateLen;

    *ieBufLen = packedIeLen;
    return MLME_SUCCESS;
} 

/*************************************************************************
* Function: smeGetScanResults
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern SINT32 smeGetScanResults(UINT8 macIndex, 
                               UINT8 *numDescpt_p,
                               UINT16 *scanResultLen_p,
                               UINT8 **inBuf_p)
{
    vmacStaInfo_t   *vStaInfo_p;
    vStaInfo_p = (vmacStaInfo_t *)vmacGetVMacStaInfo(parentGetVMacId(macIndex));

    if( vStaInfo_p )
    {
        if(vStaInfo_p->ScanResultsLen > *scanResultLen_p)
        {
            *scanResultLen_p = vStaInfo_p->ScanResultsLen;
            return MLME_FAILURE;
        }
        SPIN_LOCK_IRQSAVE(&vStaInfo_p->ScanResultsLock, vStaInfo_p->ScanResultsFlags);
        *numDescpt_p = vStaInfo_p->NumDescripts;
        *scanResultLen_p = vStaInfo_p->ScanResultsLen;
        #ifdef PORT_TO_LINUX_OS
        *inBuf_p = &vStaInfo_p->scanTableResult_p->ScanResults[0];
        #else
        memcpy((void *)inBuf_p, 
               (const void *)&vStaInfo_p->scanTableResult_p->ScanResults[0], 
               vStaInfo_p->ScanResultsLen);
        #endif /* PORT_TO_LINUX_OS */
   memset(vStaInfo_p->scanTableResult_p->ScanResultsMap_p,
          0,
          sizeof(void *) * IEEEtypes_MAX_BSS_DESCRIPTS);
   SPIN_UNLOCK_IRQRESTORE(&vStaInfo_p->ScanResultsLock, vStaInfo_p->ScanResultsFlags);        
        return MLME_SUCCESS;
    }
    return MLME_FAILURE;
}

/*************************************************************************
* Function: smeSendScanRequest
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern SINT32 smeSendScanRequest(UINT8 macIndex,
                               UINT8  scanType,
                               UINT8  bssType,
                               UINT16 scanTime,
                               UINT8  *bssid,
                               UINT8  *ieBuf_p,
                               UINT16 ieBufLen)
{
    vmacStaInfo_t   *vStaInfo_p;
    vmacEntry_t  *vmacEntry_p;
    macmgmtQ_CmdReq_t   *scanMsg_p;
    IEEEtypes_InfoElementHdr_t *IE_p;
    UINT8 bcAddr1[6]={0xff, 0xff, 0xff, 0xff, 0xff, 0xff};  /* BROADCAST BSSID */
    UINT8 bcAddr2[6]={0, 0, 0, 0, 0, 0};  /* BROADCAST BSSID */


#ifdef SC_PALLADIUM
    scanTime = 6000;
#endif
    vStaInfo_p = (vmacStaInfo_t *)vmacGetVMacStaInfo(parentGetVMacId(macIndex));
    if( vStaInfo_p == NULL )
    {
        return MLME_FAILURE;
    }
    vmacEntry_p = (vmacEntry_t *)vStaInfo_p->vMacEntry_p;
    if(smePendingCmd(vmacEntry_p->info_p))
    {
        return MLME_FAILURE;
    }
    if ((scanMsg_p = (macmgmtQ_CmdReq_t *)mlmeApiAllocSmeMsg()) == NULL)
    {
        return MLME_FAILURE;
    }
    memcpy((void *)&scanMsg_p->targetAddr[0], 
						(const void *)&vmacEntry_p->vmacAddr[0],
						sizeof(IEEEtypes_MacAddr_t));
    /* Populate Scan Command */
    scanMsg_p->CmdType              = MlmeScan_Req;
    scanMsg_p->Body.ScanCmd_sta.ScanSrc = SCAN_CMD;
    scanMsg_p->Body.ScanCmd_sta.Cmd.ScanType    = scanType;
    /* Scan Time Convert to timer tick */
    vStaInfo_p->scanTime_tick = scanTime/TIMER_TICK_SCAN;
    if (!vStaInfo_p->scanTime_tick || (scanTime%TIMER_TICK_SCAN))
    {
        /* Need to roundup one tick */
        vStaInfo_p->scanTime_tick++;
    }
    scanMsg_p->Body.ScanCmd_sta.Cmd.MaxChanTime = scanTime;
    scanMsg_p->Body.ScanCmd_sta.Cmd.MinChanTime = 0;

    /* Reset and Set Scan Filter */
    vStaInfo_p->scanFilterMap = 0;
    scanMsg_p->Body.ScanCmd_sta.Cmd.BssType     = bssType;
    if(scanMsg_p->Body.ScanCmd_sta.Cmd.BssType != BSS_ANY)
    {
        vStaInfo_p->scanFilterMap |= MLME_BSS_TYPE_FILTER;
    }
    memset(&scanMsg_p->Body.ScanCmd_sta.Cmd.BssId[0],
           0xff,
           sizeof(IEEEtypes_MacAddr_t));
    if(memcmp(bssid, bcAddr1, sizeof(IEEEtypes_MacAddr_t)) &&
       memcmp(bssid, bcAddr2, sizeof(IEEEtypes_MacAddr_t)))
    {
        vStaInfo_p->scanFilterMap |= MLME_BSSID_FILTER;
        memcpy(&scanMsg_p->Body.ScanCmd_sta.Cmd.BssId,
           bssid,
           sizeof(IEEEtypes_MacAddr_t));
    }
    memset(&scanMsg_p->Body.ScanCmd_sta.Cmd.SsId, 0x0, IEEEtypes_SSID_SIZE);
    if((IE_p = smeParseIeType(SSID,
                              ieBuf_p,
                              ieBufLen)) != NULL)
    {
        if(IE_p->Len && !(IE_p->Len > IEEEtypes_SSID_SIZE))
        {
            vStaInfo_p->scanFilterMap |= MLME_SSID_FILTER;
            memcpy(scanMsg_p->Body.ScanCmd_sta.Cmd.SsId, (UINT8 *)IE_p + sizeof(IEEEtypes_InfoElementHdr_t), IE_p->Len);
        }
    }
    
    /* Set Channel List to scan */
    memset(&scanMsg_p->Body.ScanCmd_sta.Cmd.ChanList[0], 0, (IEEEtypes_MAX_CHANNELS+IEEEtypes_MAX_CHANNELS_A));
    if((IE_p = smeParseIeType(DS_PARAM_SET,
                              ieBuf_p,
                              ieBufLen)) != NULL)
    {
       if(IE_p->Len > (IEEEtypes_MAX_CHANNELS+IEEEtypes_MAX_CHANNELS_A))
       {

           memcpy(&scanMsg_p->Body.ScanCmd_sta.Cmd.ChanList[0], (UINT8 *)IE_p + sizeof(IEEEtypes_InfoElementHdr_t), (IEEEtypes_MAX_CHANNELS+IEEEtypes_MAX_CHANNELS_A));
           
       }
       else
       {
           memcpy(&scanMsg_p->Body.ScanCmd_sta.Cmd.ChanList[0], (UINT8 *)IE_p + sizeof(IEEEtypes_InfoElementHdr_t), IE_p->Len);
       }
    }
    else
    {
        scanMsg_p->Body.ScanCmd_sta.Cmd.ChanList[0] = 1;
        scanMsg_p->Body.ScanCmd_sta.Cmd.ChanList[1] = 2;
        scanMsg_p->Body.ScanCmd_sta.Cmd.ChanList[2] = 3;
        scanMsg_p->Body.ScanCmd_sta.Cmd.ChanList[3] = 4;
        scanMsg_p->Body.ScanCmd_sta.Cmd.ChanList[4] = 5;
        scanMsg_p->Body.ScanCmd_sta.Cmd.ChanList[5] = 6;
        scanMsg_p->Body.ScanCmd_sta.Cmd.ChanList[6] = 7;
        scanMsg_p->Body.ScanCmd_sta.Cmd.ChanList[7] = 8;
        scanMsg_p->Body.ScanCmd_sta.Cmd.ChanList[8] = 9;
        scanMsg_p->Body.ScanCmd_sta.Cmd.ChanList[9] = 10;
        scanMsg_p->Body.ScanCmd_sta.Cmd.ChanList[10] = 11;
        scanMsg_p->Body.ScanCmd_sta.Cmd.ChanList[11] = 48;
        scanMsg_p->Body.ScanCmd_sta.Cmd.ChanList[12] = 56;
        scanMsg_p->Body.ScanCmd_sta.Cmd.ChanList[13] = 108;
        scanMsg_p->Body.ScanCmd_sta.Cmd.ChanList[14] = 145;
        scanMsg_p->Body.ScanCmd_sta.Cmd.ChanList[15] = 0;
    }
    
    memcpy(&vStaInfo_p->smeMain_LastScanMsg,
           scanMsg_p,
           sizeof(macmgmtQ_CmdReq_t));

    if (!mlmeApiSendSmeMsg(scanMsg_p))
    {
       /* Writing to the MAC Management Task's SME queue failed, so we */
       /* had better release the buffer, or it will forever be tied up */
       mlmeApiFreeSmeMsg((macmgmtQ_CmdBuf_t *)scanMsg_p);
       return MLME_FAILURE;
    }
    else
    {
       vStaInfo_p->smeMain_State = STATE_SCANNING;
       smeStateMgr_SetCmd(vmacEntry_p->info_p, cmdScan);
    }
    return MLME_SUCCESS;
}

/*************************************************************************
* Function: smeStateMgr_SndStartCmd
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern void smeStateMgr_SndStartCmd( UINT8 *info_p,
                                    IEEEtypes_BssDesc_t *bssDesc_p )
{
   macmgmtQ_CmdReq_t  *startMsg_p;
   vmacEntry_t  *vmacEntry_p;
   vmacStaInfo_t *vStaInfo_p;


   if((vStaInfo_p = (vmacStaInfo_t *)info_p) == NULL)
   {
       return;
   }
   vmacEntry_p = (vmacEntry_t *)vStaInfo_p->vMacEntry_p;

   if ((startMsg_p = (macmgmtQ_CmdReq_t *)mlmeApiAllocSmeMsg()) != NULL)
   {

      memset(startMsg_p, 0, sizeof(macmgmtQ_CmdReq_t));

	  memcpy((void *)&startMsg_p->targetAddr[0], 
		  (const void *)&vmacEntry_p->vmacAddr[0],
		  sizeof(IEEEtypes_MacAddr_t));
	  
      startMsg_p->CmdType                    = MlmeStart_Req;
      startMsg_p->Body.StartCmd.BcnPeriod    = bssDesc_p->BcnPeriod;
    
      startMsg_p->Body.StartCmd.BssType      = BSS_INDEPENDENT;
      startMsg_p->Body.StartCmd.CapInfo = bssDesc_p->Cap;
      startMsg_p->Body.StartCmd.CapInfo.Ibss = 1;
      startMsg_p->Body.StartCmd.DtimPeriod   = 0;
      startMsg_p->Body.StartCmd.ProbeDelay   = 0;
      startMsg_p->Body.StartCmd.CapInfo.Privacy = vStaInfo_p->staSecurityMibs.mib_AuthAlg_p->Enable;

      /* for operational rate */
      memset(&startMsg_p->Body.StartCmd.OpRateSet[0], 0, IEEEtypes_MAX_DATA_RATES_G);

      #ifdef MLME_START_11B_IBSS_ONLY
      mlmeApiSetStaMode(vStaInfo_p, CLIENT_MODE_B);
      memset(&bssDesc_p->DataRates[0], 0, IEEEtypes_MAX_DATA_RATES_G);
      bssDesc_p->DataRates[0] = 0x82;
      bssDesc_p->DataRates[1] = 0x84;
      bssDesc_p->DataRates[2] = 0x8b;
      bssDesc_p->DataRates[3] = 0x96;
      #endif /* MLME_START_11B_IBSS_ONLY */
      
      memcpy(&startMsg_p->Body.StartCmd.OpRateSet[0], &bssDesc_p->DataRates[0], IEEEtypes_MAX_DATA_RATES_G);
      startMsg_p->Body.StartCmd.PhyParamSet.DsParamSet.ElementId   = DS_PARAM_SET;
      startMsg_p->Body.StartCmd.PhyParamSet.DsParamSet.Len         = 1;
      startMsg_p->Body.StartCmd.PhyParamSet.DsParamSet.CurrentChan = bssDesc_p->PhyParamSet.DsParamSet.CurrentChan;
      startMsg_p->Body.StartCmd.SsParamSet.IbssParamSet.AtimWindow = 0;
      startMsg_p->Body.StartCmd.SsParamSet.IbssParamSet.ElementId  = IBSS_PARAM_SET;
      startMsg_p->Body.StartCmd.SsParamSet.IbssParamSet.Len        = 2;

      /* Get from BSS Descriptor */
      util_CopyList(startMsg_p->Body.StartCmd.SsId,
                    bssDesc_p->SsId,
                    IEEEtypes_SSID_SIZE);

      if (!mlmeApiSendSmeMsg(startMsg_p))
      {
         mlmeApiFreeSmeMsg((macmgmtQ_CmdBuf_t *)startMsg_p);
      }
      else
      {
         vStaInfo_p->macMgmtMain_PostScanState = STATE_IDLE;
         vStaInfo_p->macMgmtMain_State = STATE_IDLE;
         vStaInfo_p->AssociatedFlag = 2;
         vStaInfo_p->smeMain_State = STATE_IBSS_STARTED;
         smeStateMgr_SetCmd(vmacEntry_p->info_p, cmdStart);
      }
   }
}

#ifdef WPA_STA
/*************************************************************************
* Function:
*
* Description:
*  This function returns the pointer to the key management stucture inside vmacStaInfo_t
*  It assumes that info_p inside vmacEntry_t is a vmacStaInfo_t pointer.
*           
*
* Input:
*
* Output:
*
**************************************************************************/
extern keyMgmtInfoSta_t * sme_GetKeyMgmtInfoStaPtr(vmacEntry_t * vmacEntry_p)
{
    return ((vmacStaInfo_t *)(vmacEntry_p->info_p))->keyMgmtInfoSta_p;
}
#endif /* WPA_STA */

/*************************************************************************
* Function:
*
* Description:
*  This function returns the pointer to the system mib stucture inside vmacStaInfo_t
*  It assumes that info_p inside vmacEntry_t is a vmacStaInfo_t pointer.
*           
*
* Input:
*
* Output:
*
**************************************************************************/
extern STA_SYSTEM_MIBS * sme_GetStaSystemMibsPtr(vmacEntry_t * vmacEntry_p)
{
    return &(((vmacStaInfo_t *)(vmacEntry_p->info_p))->staSystemMibs);
}

/*************************************************************************
* Function:
*
* Description:
*  This function returns the pointer to the security mib stucture inside vmacStaInfo_t
*  It assumes that info_p inside vmacEntry_t is a vmacStaInfo_t pointer.
*           
*
* Input:
*
* Output:
*
**************************************************************************/
extern STA_SECURITY_MIBS * sme_GetStaSecurityMibsPtr(vmacEntry_t * vmacEntry_p)
{
    return &(((vmacStaInfo_t *)(vmacEntry_p->info_p))->staSecurityMibs);
}

/*************************************************************************
* Function:
*
* Description:
*  This function returns the value of the isParentSession BOOLEAN inside vmacStaInfo_t
*  It assumes that info_p inside vmacEntry_t is a vmacStaInfo_t pointer.
*           
*
* Input:
*
* Output:
*
**************************************************************************/
extern BOOLEAN sme_isParentSession(vmacEntry_t * vmacEntry_p)
{
    return (((vmacStaInfo_t *)(vmacEntry_p->info_p))->isParentSession);
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
vmacEntry_t *smeInitParentSession(UINT8 phyMacIndx, 
                                  UINT8 *macAddr,
                                  trunkId_t trunkId,
                                  void *callBack_fp,
                                  void *privInfo_p)
{
	vmacEntry_t  *vmacEntry_p;

	/* Initialize the Parent MLME session */
	if((vmacEntry_p = mlmeStaInit_Parent(phyMacIndx, 
                                         macAddr, 
                                         callBack_fp)) != NULL)
	{
		vmacEntry_p->trunkId = trunkId;
        vmacEntry_p->privInfo_p = privInfo_p;
	}
	return vmacEntry_p;
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
vmacEntry_t *smeStartChildSession(UINT8 phyMacIndx, 
						          UINT8 *macAddr,
						          trunkId_t trunkId,
						          void *callBack_fp,
                                  UINT32 controlParam,
                                  void *privInfo_p)
{
	vmacEntry_t  *vmacEntry_p;
	vmacStaInfo_t *vStaInfo_p;
	macmgmtQ_CmdReq_t   *macMgmtMsg_p;
    vmacStaInfo_t 	*parentStaInfo_p;
	vmacId_t 		parentVMacId;
	vmacId_t 		childVMacId;

    /* Check to see if Service is enabled */
    if(!mib_childMode[phyMacIndx])
    {
        return NULL;
    }

	/* Get Parent's information */
	parentVMacId=parentGetVMacId(phyMacIndx);
    if((parentStaInfo_p = (vmacStaInfo_t *)vmacGetVMacStaInfo(parentVMacId)) == NULL)
    {
        return NULL;
    }
	/* Check if Parent's Link with Root AP is up */
	if(!(parentStaInfo_p->isParentSession && parentStaInfo_p->AssociatedFlag))
	{
		return NULL;
	}
	/* Start a child session */
	if((vmacEntry_p = childSrv_StartSession(phyMacIndx,
										   macAddr,
										    callBack_fp,
											controlParam)) == NULL)
	{
		return NULL;
	}
	childVMacId = vmacEntry_p->id;
	/* Record Trunk Id */
	vmacEntry_p->trunkId = trunkId;
    /* Record Priv Info pointer */
    vmacEntry_p->privInfo_p = privInfo_p;

	/* Send Command to MLME to join Root AP */
	vStaInfo_p = (vmacStaInfo_t *)vmacEntry_p->info_p;
	if ((macMgmtMsg_p = (macmgmtQ_CmdReq_t *)mlmeApiAllocSmeMsg()) != NULL)
    {
       memcpy(macMgmtMsg_p,
              &vStaInfo_p->LastJoinMsg,
              sizeof(macmgmtQ_CmdReq_t));
	   memcpy((void *)&macMgmtMsg_p->targetAddr[0], 
	 		(const void *)&vmacEntry_p->vmacAddr[0],
	 		sizeof(IEEEtypes_MacAddr_t));
       if (!mlmeApiSendSmeMsg(macMgmtMsg_p))
       {
          mlmeApiFreeSmeMsg((macmgmtQ_CmdBuf_t *)macMgmtMsg_p);
		  childSrv_TerminateSession(childVMacId);
		  return NULL;
       }
       else
       {
          smeStateMgr_SetCmd(vmacEntry_p->info_p, cmdJoin);
       }
    }
    else
    {
		childSrv_TerminateSession(childVMacId);
		return NULL;
	}
    #ifdef ETH_DEBUG
	eprintf("Start Child Entry vMacId = %d hwMacId = %d\n", 
				 vmacEntry_p->id, vmacEntry_p->macId);
    #endif /* ETH_DEBUG */
	return vmacEntry_p;
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
void smeStopChildSession(vmacId_t childSessionId)
{
    #ifdef ETH_DEBUG
	eprintf("Stop Child Entry vMacId = %d \n", 
				 childSessionId);
    #endif /* ETH_DEBUG */
    childSrv_TerminateSession(childSessionId);
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
BOOLEAN smeSetChildBlockAddr(UINT8 phyMacIndx, 
								  UINT8 *macAddr,
								  UINT8 maskAddr)
{
    vmacStaInfo_t 	*parentStaInfo_p;
	vmacId_t 		parentVMacId;

	/* Get Parent's information */
	parentVMacId=parentGetVMacId(phyMacIndx);
    if((parentStaInfo_p = (vmacStaInfo_t *)vmacGetVMacStaInfo(parentVMacId)) == NULL)
    {
        return FALSE;
    }
	/* Register the block address */
	if(childSrv_RegisterBlockAddress(parentStaInfo_p,
                                     phyMacIndx, 
									 macAddr,
									 maskAddr) == -1)
	{
		return FALSE;
	}
	return TRUE;
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
void smeChildSendDeAuth(vmacId_t childSessionId)
{
    vmacEntry_t *vmacEntry_p;
	vmacStaInfo_t *vStaInfo_p;

	if((vmacEntry_p = vmacGetVMacEntryById(childSessionId)) != NULL)
	{
		vStaInfo_p = (vmacStaInfo_t *)vmacEntry_p->info_p;
		if(!vStaInfo_p->isParentSession 
           && vStaInfo_p->AssociatedFlag 
           && !vStaInfo_p->mib_WB_p->opMode)
        {
        	vStaInfo_p->staSecurityMibs.mib_PrivacyTable_p->RSNLinkStatus = 0;  	
            vStaInfo_p->AssociatedFlag = 0;
            authSrv_SndDeAuthMsg( vStaInfo_p, 
                                     &vStaInfo_p->macMgmtMlme_ThisStaData.BssId,
                                     &vStaInfo_p->macMgmtMlme_ThisStaData.BssId,
                                     IEEEtypes_REASON_DEAUTH_LEAVING);
        }
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
SINT32 smeForceAllChildLinksDown(UINT8 phyMacIndx)
{
    /* Notify data path that child sessions' links are down */
    childSrv_TerminateAllLinks();
    return MLME_SUCCESS;
}

/*************************************************************************
* Function: syncSrv_SndLinkLostInd
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern void smeSndLinkLostInd(vmacEntry_t *vmacEntry_p, UINT16 reason)
{
    vmacStaInfo_t *vStaInfo_p = (vmacStaInfo_t *)vmacEntry_p->info_p;

    #ifdef ETH_DEBUG
    eprintf("==> syncSrv_SndLinkLostInd\n");
    #endif /* ETH_DEBUG */
	if((vmacEntry_p == NULL)||(vStaInfo_p == NULL))
	{
		return;
	}
    if(vStaInfo_p->AssociatedFlag && !vStaInfo_p->mib_WB_p->opMode)
    {
    	vStaInfo_p->staSecurityMibs.mib_PrivacyTable_p->RSNLinkStatus = 0;  	
        vStaInfo_p->AssociatedFlag = 0;
        authSrv_SndDeAuthMsg( vStaInfo_p, 
                                &vStaInfo_p->macMgmtMlme_ThisStaData.BssId,
                                &vStaInfo_p->macMgmtMlme_ThisStaData.BssId,
                                reason);
    }
    mlmeApiSndNotification(vStaInfo_p, NULL, Tbcn);
}

/*************************************************************************
* Function: sme_GetParentVMacEntry
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern vmacEntry_t *sme_GetParentVMacEntry(UINT8 phyMacIndx)
{
	vmacEntry_t  *vmacEntry_p;

    if((vmacEntry_p = vmacGetVMacEntryById(parentGetVMacId(phyMacIndx))) == NULL)
    {
        #ifdef ETH_DEBUG
        eprintf("sme_GetParentVMacEntry:: fail \n");
        #endif /* ETH_DEBUG */
        return NULL;
    }
    return vmacEntry_p;
}

/*************************************************************************
* Function: sme_GetParentPrivInfo
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern void *sme_GetParentPrivInfo(UINT8 phyMacIndx)
{
    vmacId_t clientVMacId;
    vmacStaInfo_t *vStaInfo_p;
	vmacEntry_t  *vmacEntry_p;

	clientVMacId=parentGetVMacId(phyMacIndx);
    if((vStaInfo_p = (vmacStaInfo_t *)vmacGetVMacStaInfo(clientVMacId)) == NULL)
    {
        #ifdef ETH_DEBUG
        eprintf("sme_GetParentPrivInfo:: fail \n");
        #endif /* ETH_DEBUG */
        return NULL;
    }
	vmacEntry_p = (vmacEntry_t *)vStaInfo_p->vMacEntry_p;
    return vmacEntry_p->privInfo_p;
}


/*************************************************************************
* Function: sme_GetParentPrivInfo
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern void *sme_SetClientPeerInfo(vmacEntry_t  *vmacEntry_p, PeerInfo_t *peerInfo_p)
{
    vmacStaInfo_t *vStaInfo_p;
    if (vmacEntry_p)
    {
        vStaInfo_p = (vmacStaInfo_t *) vmacEntry_p->info_p;
        if (vStaInfo_p && peerInfo_p)
            vStaInfo_p->peerInfo_p = (void *) peerInfo_p;
        else
            return NULL;
    }
    else
        return NULL;

    return peerInfo_p;
    
}

/*************************************************************************
* Function: sme_GetParentPrivInfo
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern PeerInfo_t *sme_GetClientPeerInfo(vmacEntry_t  *vmacEntry_p)
{
    vmacStaInfo_t *vStaInfo_p;
    PeerInfo_t *peerInfo_p = NULL;
    if (vmacEntry_p)
    {
        vStaInfo_p = (vmacStaInfo_t *) vmacEntry_p->info_p;
        if (vStaInfo_p)
            peerInfo_p = (PeerInfo_t *) vStaInfo_p->peerInfo_p;
    }

    return peerInfo_p;
}

#ifdef WPA_STA
extern void sme_DisableKeyMgmtTimer(vmacEntry_t  *vmacEntry_p)
{
    vmacStaInfo_t *vStaInfo_p = (vmacStaInfo_t *) vmacEntry_p->info_p;
    if (vStaInfo_p)
    {
        if (vStaInfo_p->staSecurityMibs.mib_PrivacyTable_p->RSNEnabled) 
        {
            ((KeyData_t *)vStaInfo_p->keyMgmtInfoSta_p->pKeyData)->RSNDataTrafficEnabled = 0;
            /* Stop Key management timer. */
            TimerRemove(&vStaInfo_p->keyMgmtInfoSta_p->keyMgmtStaHskHsm.rsnSecuredTimer);
            /* Stop Mic timer. */
            TimerRemove(&vStaInfo_p->keyMgmtInfoSta_p->sta_MIC_Error.timer);
        }
    }
    return;
}
#endif
