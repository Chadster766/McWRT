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
* File: child_srv.c
*        Client Child Service Function Calls
* Description:  Implementation of the Client MLME Child Services
*
*******************************************************************************************/
#include "mlmeSta.h"
#include "wl_mib.h"
#include "wl_hal.h"

#include "mlmeApi.h"
#include "mlmeSta.h"
#include "wlvmac.h"
#include "wl_macros.h"
#include "mlmeChild.h"
#include "mlmeParent.h"

#ifdef WPA_STA
KeyData_t           childGKeyData    [MAX_MLME_CHILD_SESSIONS];
keyMgmtInfoSta_t    childKeyMgmtInfo [MAX_MLME_CHILD_SESSIONS];
#endif /* WPA_STA */
vmacStaInfo_t       childStaInfo     [MAX_MLME_CHILD_SESSIONS];
vmacEntry_t         childVMacEntry   [MAX_MLME_CHILD_SESSIONS];

UINT8 childBlockAddr[NUM_OF_WLMACS][IEEEtypes_ADDRESS_SIZE];
UINT8 childBlockAddr_mask[2] = {0,0};
halMacId_t childBlockAddrMacId[2] = {-1, -1};

#ifdef STA_ADHOC_SUPPORTED
extern TxBcnBuf txbcnbuf[NUM_OF_BEACONS];
extern TxBcnBuf txprbrspbuf[NUM_OF_BEACONS];
#endif /* STA_ADHOC_SUPPORTED */


/*************************************************************************
* Function: childSrv_InitChildServices
*
* Description: Clear up all the Child Service related data structures
*              Only to be call at system init
*
* Input:
*
* Output:
*
**************************************************************************/
extern  void childSrv_InitChildServices(void)
{
	memset((UINT8 *)&childStaInfo[0],     0, sizeof(vmacStaInfo_t)    * MAX_MLME_CHILD_SESSIONS);
    memset((UINT8 *)&childVMacEntry[0],   0, sizeof(vmacEntry_t)      * MAX_MLME_CHILD_SESSIONS);
    #ifdef WPA_STA
    memset((UINT8 *)&childGKeyData[0],    0, sizeof(KeyData_t)        * MAX_MLME_CHILD_SESSIONS); 
    memset((UINT8 *)&childKeyMgmtInfo[0], 0, sizeof(keyMgmtInfoSta_t) * MAX_MLME_CHILD_SESSIONS); 
    #endif /* WPA_STA */
}

/*************************************************************************
* Function: childSrv_GetEntry
*
* Description: Get a vMac Entry and fill it out with some default values
*
* Input:
*
* Output:
*
**************************************************************************/
static vmacEntry_t *childSrv_GetEntry(UINT8 *vMacAddr)
{
	UINT32 i;

	for(i=0; i < MAX_MLME_CHILD_SESSIONS; i++)
	{
		if(!childVMacEntry[i].active)
		{
			childVMacEntry[i].active = 1;
			memcpy(&childVMacEntry[i].vmacAddr[0], 
				   vMacAddr, 
				   sizeof(IEEEtypes_MacAddr_t));
			childVMacEntry[i].info_p = (UINT8 *)&childStaInfo[i];
			childVMacEntry[i].modeOfService = VMAC_MODE_CLNT_INFRA;
			childVMacEntry[i].mlmeMsgEvt = &evtSme_StaCmdMsg;
			childVMacEntry[i].dot11MsgEvt = &evtDot11_StaMgtMsg;
			/* For debugging purpose:: set to some familiar values */
			childVMacEntry[i].phyHwMacIndx = 0xff;
			childVMacEntry[i].macId = 0xffffffff;
			childVMacEntry[i].id = 0xff;
			childVMacEntry[i].trunkId = 0xff;

            #ifdef WPA_STA
            childStaInfo[i].keyMgmtInfoSta_p                      = &childKeyMgmtInfo[i];
            childStaInfo[i].keyMgmtInfoSta_p->pKeyData            = &childGKeyData[i];
            childKeyMgmtInfo[i].keyMgmtStaHskHsm.keyMgmtInfoSta_p = &childKeyMgmtInfo[i];            
            childKeyMgmtInfo[i].vmacEntry_p                       = &childVMacEntry[i];
            #endif /* WPA_STA */

			return &childVMacEntry[i];
		}
	}
	return NULL;
}

/*************************************************************************
* Function: childSrv_PutEntry
*
* Description: Put back a vMac Entry for other to use
*
* Input:
*
* Output:
*
**************************************************************************/
static BOOLEAN childSrv_PutEntry(UINT8 *vMacAddr)
{
	UINT32 i;

	for(i=0; i < MAX_MLME_CHILD_SESSIONS; i++)
	{
		if(memcmp(&childVMacEntry[i].vmacAddr[0], 
				  vMacAddr, 
				  sizeof(IEEEtypes_MacAddr_t)) == 0)
		{
			childVMacEntry[i].active = 0;
			return TRUE;
		}
	}
	return FALSE;
}

/*************************************************************************
* Function: childSrv_VmacStaInfoInit
*
* Description: Inital Client MLME Information Structure.
*              
*
* Input:       
*              
* Output:
**************************************************************************/
extern SINT32 childSrv_VmacStaInfoInit(vmacStaInfo_t *vStaInfo_p)
{
	vStaInfo_p->staSecurityMibs.mib_PrivacyTable_p->RSNLinkStatus = 0;  	
	vStaInfo_p->AssociatedFlag = 0;
#ifdef IEEE80211_DH
	vStaInfo_p->station11hTimerFired = 0;
#endif //IEEE80211_DH
	vStaInfo_p->IBssStartFlag = 0;
	vStaInfo_p->Adhoc_Active = 0;
	vStaInfo_p->ScanForAnyBeacons = TRUE;
	vStaInfo_p->NumScanChannels = 0;
	vStaInfo_p->counterInt = 0;
	vStaInfo_p->misMatchBssidCount = 0;
	vStaInfo_p->bcnCount = 0;
	vStaInfo_p->linkQuality = 0;
	vStaInfo_p->rxBcnCnt	= 0;
	vStaInfo_p->rxBcnPeriod = 100;
	vStaInfo_p->smeMain_State = STATE_IDLE;
	vStaInfo_p->macMgmtMain_State   = STATE_IDLE;
    vStaInfo_p->macMgmtMain_PostScanState = STATE_IDLE;
	vStaInfo_p->PostScanState = STATE_IDLE;
    vStaInfo_p->g_rcvdProbeRsp = 1;
	vStaInfo_p->macMgt_StaMode = *vStaInfo_p->mib_StaMode_p;
#ifdef IEEE80211_DH
	TimerInit( &vStaInfo_p->station11hTimer);
#endif //IEEE80211_DH
#ifdef WMON
	TimerInit( &vStaInfo_p->stationWMONTimer);
#endif //WMON
    /* Remove all the timer */
    /* assocTimer */
    mlmeApiStopTimer(vStaInfo_p, (UINT8 *)&vStaInfo_p->assocTimer);
	/* authTimer; */
    mlmeApiStopTimer(vStaInfo_p, (UINT8 *)&vStaInfo_p->authTimer);
    /* scaningTimer; */
    mlmeApiStopTimer(vStaInfo_p, (UINT8 *)&vStaInfo_p->scaningTimer);
    /* statusTimer; */
    syncSrv_SetStatusTimer(vStaInfo_p, 0); 
    #ifdef WPA_STA
    /* initialize the key management state machine. */
    KeyMgmtStaHskCtor(&vStaInfo_p->keyMgmtInfoSta_p->keyMgmtStaHskHsm);
    mhsm_initialize(&vStaInfo_p->keyMgmtInfoSta_p->keyMgmtStaHskHsm.super,&vStaInfo_p->keyMgmtInfoSta_p->keyMgmtStaHskHsm.sTop);
    if(vStaInfo_p->staSecurityMibs.mib_PrivacyTable_p->RSNEnabled)
    {
        KeyMgmtResetCounter(vStaInfo_p->keyMgmtInfoSta_p);
        CounterMeasureInit_Sta(&vStaInfo_p->keyMgmtInfoSta_p->sta_MIC_Error, TRUE);
    }
    else
    {
        CounterMeasureInit_Sta(&vStaInfo_p->keyMgmtInfoSta_p->sta_MIC_Error, FALSE);
    }
    #endif /* WPA_STA */
	/* Init the STA State Machine Services */
	macMgtSyncSrvStaInit(vStaInfo_p);
    vStaInfo_p->cmdHistory = 0;
	vStaInfo_p->macMgmtMain_PwrMode = PWR_MODE_ACTIVE;
	vStaInfo_p->ContinueScanning =  TRUE;
	vStaInfo_p->JoinRetryCount = 0;
	vStaInfo_p->AssocRetryCount = 0;
	vStaInfo_p->AuthRetryCount = 0;
	SPIN_LOCK_INIT(&vStaInfo_p->ScanResultsLock);
	return 0;
}

/*************************************************************************
* Function: childSrv_CopyParentSessionParam
*
* Description: Inherit certain Parent's attributes
*
* Input:
*
* Output:
*
**************************************************************************/
static SINT32 childSrv_CopyParentSessionParam(vmacStaInfo_t * childStaInfo_p,
											 UINT8 parentMacIndx)
{
	vmacId_t 		parentVMacId;
	vmacStaInfo_t 	*parentStaInfo_p;

	parentVMacId=parentGetVMacId(parentMacIndx);
    if((parentStaInfo_p = (vmacStaInfo_t *)vmacGetVMacStaInfo(parentVMacId)) == NULL)
    {
        return MLME_FAILURE;
    }
	childStaInfo_p->scanTableResult_p = parentStaInfo_p->scanTableResult_p;
	childStaInfo_p->sSurveyTable_p = parentStaInfo_p->sSurveyTable_p;
    childStaInfo_p->BssDescSet_p = parentStaInfo_p->BssDescSet_p;
	childStaInfo_p->mib_StaMode_p         = parentStaInfo_p->mib_StaMode_p;
	childStaInfo_p->Station_p             = parentStaInfo_p->Station_p;
	childStaInfo_p->mib_WB_p              = parentStaInfo_p->mib_WB_p;
	childStaInfo_p->staSystemMibs.mib_StaCfg_p   = parentStaInfo_p->staSystemMibs.mib_StaCfg_p;
	childStaInfo_p->staSystemMibs.mib_OpData_p   = parentStaInfo_p->staSystemMibs.mib_OpData_p;
	childStaInfo_p->staSystemMibs.PhyDSSSTable_p = parentStaInfo_p->staSystemMibs.PhyDSSSTable_p;
	childStaInfo_p->staSecurityMibs.mib_AuthAlg_p = parentStaInfo_p->staSecurityMibs.mib_AuthAlg_p;
	childStaInfo_p->staSecurityMibs.mib_AuthAlg_G = parentStaInfo_p->staSecurityMibs.mib_AuthAlg_G;
    childStaInfo_p->staSecurityMibs.mib_WepDefaultKeys_p = parentStaInfo_p->staSecurityMibs.mib_WepDefaultKeys_p;
    childStaInfo_p->staSecurityMibs.mib_WepDefaultKeys_G = parentStaInfo_p->staSecurityMibs.mib_WepDefaultKeys_G;
    childStaInfo_p->WepType_p             = parentStaInfo_p->WepType_p;
    childStaInfo_p->mib_defaultkeyindex_p = parentStaInfo_p->mib_defaultkeyindex_p;
	childStaInfo_p->staSecurityMibs.mib_PrivacyTable_p            = parentStaInfo_p->staSecurityMibs.mib_PrivacyTable_p;
    childStaInfo_p->bssDescProfile_p = parentStaInfo_p->bssDescProfile_p;
    #ifdef WPA_STA
	childStaInfo_p->thisStaRsnIE_p      = parentStaInfo_p->thisStaRsnIE_p;
	childStaInfo_p->thisStaRsnIEWPA2_p  = parentStaInfo_p->thisStaRsnIEWPA2_p;
	childStaInfo_p->staSecurityMibs.mib_RSNStats_p                = parentStaInfo_p->staSecurityMibs.mib_RSNStats_p;
	childStaInfo_p->staSecurityMibs.mib_RSNConfig_p               = parentStaInfo_p->staSecurityMibs.mib_RSNConfig_p;
	childStaInfo_p->staSecurityMibs.mib_RSNConfigUnicastCiphers_p = parentStaInfo_p->staSecurityMibs.mib_RSNConfigUnicastCiphers_p;
	#ifdef WPA2                                        
	childStaInfo_p->staSecurityMibs.mib_RSNConfigWPA2_p                = parentStaInfo_p->staSecurityMibs.mib_RSNConfigWPA2_p;
	childStaInfo_p->staSecurityMibs.mib_RSNConfigWPA2UnicastCiphers_p  = parentStaInfo_p->staSecurityMibs.mib_RSNConfigWPA2UnicastCiphers_p;
    childStaInfo_p->staSecurityMibs.mib_RSNConfigWPA2UnicastCiphers2_p = parentStaInfo_p->staSecurityMibs.mib_RSNConfigWPA2UnicastCiphers2_p;
    childStaInfo_p->staSecurityMibs.mib_RSNConfigWPA2AuthSuites_p      = parentStaInfo_p->staSecurityMibs.mib_RSNConfigWPA2AuthSuites_p;
	#endif /* WPA2 */
	#endif /* WPA_STA */
    memcpy(&childStaInfo_p->ScanParams, 
		   &parentStaInfo_p->ScanParams, 
		   sizeof(IEEEtypes_ScanCmd_t));
	memcpy(&childStaInfo_p->LastJoinMsg, 
		   &parentStaInfo_p->LastJoinMsg, 
		   sizeof(macmgmtQ_CmdReq_t));
	return MLME_SUCCESS;
}

/*************************************************************************
* Function: childSrv_StartSession
*
* Description: Establish a Child session with Root AP
*
* Input:
*
* Output:
*
**************************************************************************/
extern vmacEntry_t *childSrv_StartSession(UINT8 macIndex, 
								          UINT8 *macAddr,
										  void *callBack_fp,
                                          UINT32 controlParam)
{
	vmacId_t idAssigned;
	halMacId_t macId;
	vmacEntry_t *childVMacEntry_p;
	vmacStaInfo_t *childStaInfo_p;

	if((childVMacEntry_p = vmacGetVMacEntryByAddr(macAddr)) != NULL)
	{
        #ifdef ETH_DEBUG
		eprintf("Child old Entry\n");
        #endif /* ETH_DEBUG */
		childStaInfo_p = (vmacStaInfo_t *)childVMacEntry_p->info_p;
		childStaInfo_p->staSecurityMibs.mib_PrivacyTable_p->RSNLinkStatus = 0;  	
		childStaInfo_p->AssociatedFlag = 0;
		childStaInfo_p->IBssStartFlag = 0;
		childStaInfo_p->Adhoc_Active = 0;
		childStaInfo_p->smeMain_State = STATE_IDLE;
		childStaInfo_p->macMgmtMain_State   = STATE_IDLE;
		childStaInfo_p->macMgmtMain_PostScanState = STATE_IDLE;
		childStaInfo_p->PostScanState = STATE_IDLE;
        childSrv_VmacStaInfoInit(childStaInfo_p);
		return childVMacEntry_p;
	}
    #ifdef ETH_DEBUG
	eprintf("Child new Entry\n");
    #endif /* ETH_DEBUG */
	if((childVMacEntry_p =childSrv_GetEntry(macAddr)) == NULL)
	{
		return NULL;
	}
	childStaInfo_p = (vmacStaInfo_t *)childVMacEntry_p->info_p;
	childStaInfo_p->childControlParam = controlParam;
	if(childStaInfo_p->childControlParam & MLME_CHILD_SET_ADDR_TO_HW)
	{
		if((macId = mlmeApiAddMacAddr(childStaInfo_p, macIndex, macAddr, 0)) == -1)
	    {
		    childSrv_PutEntry(macAddr);
		    return NULL;
	    }
	}
	else
	{
		if((childBlockAddr_mask[macIndex] == 0)
		   || (childBlockAddrMacId[macIndex] == -1))
	    {
		    return NULL;
	    }
		macId = childBlockAddrMacId[macIndex];
	}
	childVMacEntry_p->phyHwMacIndx = macIndex;
	childVMacEntry_p->macId = macId;
	/* Init Station MLME Information Structure */
	if(childSrv_CopyParentSessionParam(childStaInfo_p,
									  macIndex) == MLME_FAILURE) 
	{
		if(childStaInfo_p->childControlParam & MLME_CHILD_SET_ADDR_TO_HW)
		{
			mlmeApiDelMacAddr(childStaInfo_p, macId);
		}
		childSrv_PutEntry(macAddr);
		return NULL;
	}
    #ifdef STA_ADHOC_SUPPORTED
	/* Beacon Buffer Pointer */
	childStaInfo_p->BcnTxInfo_p = (txBcnInfo_t *)&txbcnbuf[macid2index(macId)];
	childStaInfo_p->BcnBuffer_p = (macmgmtQ_MgmtMsg_t *)&(txbcnbuf[macid2index(macId)].Bcn);  	
	/* Probe Response Pointer */
	childStaInfo_p->PrbRspTxInfo_p = (txBcnInfo_t *)&txprbrspbuf[macid2index(macId)];
	childStaInfo_p->PrbRspBuf_p = (macmgmtQ_MgmtMsg_t *)&(txprbrspbuf[macid2index(macId)].Bcn);
    #endif /* STA_ADHOC_SUPPORTED */
	/* Set it as a child session */
	childStaInfo_p->isParentSession = FALSE;
	/* Record the callBack Function */
	childStaInfo_p->mlmeCallBack_fp = callBack_fp;
    /* Record the parent vMacEntry address */
    childStaInfo_p->vMacEntry_p = (UINT8 *)childVMacEntry_p;
	/* Init default Parameters */
	childSrv_VmacStaInfoInit((vmacStaInfo_t *)childVMacEntry_p->info_p);
	/* Lastly register the station id and information structure */
	if((idAssigned = vmacRegister(childVMacEntry_p)) < 0 )
	{
		if(childStaInfo_p->childControlParam & MLME_CHILD_SET_ADDR_TO_HW)
		{
			mlmeApiDelMacAddr(childStaInfo_p, macId);
		}
		childSrv_PutEntry(macAddr);
		return NULL;
	}
	childVMacEntry_p->id = idAssigned;
	return childVMacEntry_p;
}

/*************************************************************************
* Function: childSrv_TerminateSession
*
* Description: Terminate a Child session with Root AP
*
* Input:
*
* Output:
*
**************************************************************************/
extern SINT32 childSrv_TerminateSession(vmacId_t vMacId)
{
	vmacEntry_t *childVMacEntry_p;
	vmacStaInfo_t *childStaInfo_p;

	if((childVMacEntry_p = vmacGetVMacEntryById(vMacId)) != NULL)
	{
        #ifdef ETH_DEBUG
		eprintf("Child Terminate Entry confirmed\n");
        #endif /* ETH_DEBUG */
		childStaInfo_p = (vmacStaInfo_t *)childVMacEntry_p->info_p;
		if(!childStaInfo_p->isParentSession 
		   &&(childStaInfo_p->childControlParam & MLME_CHILD_SET_ADDR_TO_HW))
		{
			mlmeApiDelMacAddr(childStaInfo_p, childVMacEntry_p->macId);
		}
        /* Remove all the timer */
        /* assocTimer */
        mlmeApiStopTimer(childStaInfo_p, (UINT8 *)&childStaInfo_p->assocTimer);
	    /* authTimer; */
        mlmeApiStopTimer(childStaInfo_p, (UINT8 *)&childStaInfo_p->authTimer);
        /* scaningTimer; */
        mlmeApiStopTimer(childStaInfo_p, (UINT8 *)&childStaInfo_p->scaningTimer);
        /* statusTimer; */
        mlmeApiStopTimer(childStaInfo_p, (UINT8 *)&childStaInfo_p->statusTimer);
        childStaInfo_p->cmdHistory = 0;
        #ifdef WPA_STA
        /* CounterMeasure */
        CounterMeasureInit_Sta(&childStaInfo_p->keyMgmtInfoSta_p->sta_MIC_Error, FALSE);
        #endif /* WPA_STA */
		vmacUnRegister(childVMacEntry_p->id);
		childSrv_PutEntry(&childVMacEntry_p->vmacAddr[0]);
	}
	return 0;
}

/*************************************************************************
* Function: childSrv_TerminateAllLinks
*
* Description: Terminate a Child session with Root AP
*
* Input:
*
* Output:
*
**************************************************************************/
extern SINT32 childSrv_TerminateAllLinks(void)
{
	UINT32 i;

	for(i=0; i < MAX_MLME_CHILD_SESSIONS; i++)
	{
		if(childVMacEntry[i].active)
		{
			mlmeApiUpdateLinkStatus((vmacStaInfo_t *)childVMacEntry[i].info_p, 
									   LINK_DOWN);
		}
	}
	return MLME_SUCCESS;
}

/*************************************************************************
* Function: childSrv_RegisterBlockAddress
*
* Description: Register a block of Address to be used by child session
*
* Input:
*
* Output:
*
**************************************************************************/
extern halMacId_t childSrv_RegisterBlockAddress(vmacStaInfo_t * vStaInfo_p,
                                                UINT8 macIndex, 
												UINT8 *macAddr,
												UINT8 addrMask)
{
	halMacId_t macId;
	if((macId = mlmeApiAddMacAddr(vStaInfo_p, macIndex, macAddr, addrMask)) != -1)
	{
		childBlockAddrMacId[macIndex] = macId;
		childBlockAddr_mask[macIndex] = addrMask;
		memcpy(&childBlockAddr[macIndex][0], macAddr, IEEEtypes_ADDRESS_SIZE);
	}
	return macId;
}

