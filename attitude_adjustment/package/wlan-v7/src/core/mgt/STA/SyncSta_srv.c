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
* File: SyncSta_srv.c
*        Client Synchronization Service Function Calls
                     
* Description:  Implementation of the Client MLME Synchronization Services
*
*******************************************************************************************/
#include "mlmeSta.h"
#include "wl_mib.h"
#include "wl_hal.h"

#include "mlmeApi.h"
#include "wlvmac.h"
#include "mlmeParent.h"
#include "buildModes.h"
#ifdef STA_QOS
#include "qos.h"
#endif
#include "wldebug.h"

#include "StaDb.h"
#include "ap8xLnxIntf.h"
#include "ap8xLnxFwcmd.h"
#include "linkmgt.h"

static SINT32 syncSrv_SndProbeReq( vmacStaInfo_t *vStaInfo_p, BOOLEAN isScan );
#ifdef IEEE80211_DH
void station11hTimerCB(void *data_p);
#endif //IEEE80211_DH

#if 1 //bt_port
UINT32 ClientModeDataCount[NUM_OF_WLMACS] = {0,0};		//To keep track active tx/ rx data traffic within g_CheckStatusTime interval

#endif
UINT32 NoiseSampleBeaconCount = 0;
#define NOISE_SAMPLE_BEACON_COUNT  0x05

#define MAX_BEACON_CHANNEL_MISMATCH 0x10  /* Maximum nuber of beacons with mismatched 
                                             channel before Link Status lost. */

extern void AccumulateScanResults(vmacStaInfo_t *vStaInfo_p, void *BssData_p, UINT8 *rfHdr_p );
#ifdef AMPDU_SUPPORT_TX_CLIENT
extern void cleanupAmpduTx(vmacApInfo_t *vmacSta_p,UINT8 *macaddr);
#endif

extern void macMgmtMlme_StopDataTraffic( struct net_device *dev );
extern void macMgmtMlme_RestartDataTraffic( struct net_device *dev );

extern int wlFwGetNoiseLevel (struct net_device *netdev, UINT16 action, UINT8 *pNoise);
extern IEEEtypes_InfoElementHdr_t *smeParseIeType(UINT8 ieType, UINT8 *ieBuf_p, UINT16 ieBufLen);

extern UINT8 mib_BeaconRate[NUM_OF_WLMACS];
extern UINT8 mib_BeaconPreamble[NUM_OF_WLMACS];

/* For Child */
extern UINT8 mib_childMode[NUM_OF_WLMACS];

/* Imported customer specific function. To be customised by customer at aproot.c 
* In heavy traffic, Probe Resp could be delayed and reported as missed. 
* Small interval will cause g_PrbeReqCheckTheshold to be reached faster due to missed Probe Resp
* and trigger false disconnection. 
* g_CheckStatusTime reduce from 60 to 30, g_PrbeReqCheckTheshold from 10 to 4 for Belkin. 03142014
*/
UINT32 g_CheckStatusTime[NUM_OF_WLMACS] = {30, 30};		

UINT32 g_PrbeReqCheckTheshold[NUM_OF_WLMACS] = {4, 4};

UINT32 g_KeepAliveTime = 10;
UINT32 g_KeepAliveEnable = 1;
UINT32 urBeaconChannelMismatchCnt = 0;
#ifdef WMON
extern char *g_wmon_DFSLog ;
#endif //WMON

#define ADHOC_TIMEOUT_NO_BCN_THRESHOLD      10


/*************************************************************************
* Function: syncSrv_SndScanCnfm
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
static void syncSrv_SndScanCnfm(vmacStaInfo_t *vStaInfo_p,
								UINT16 scanResult,
                                UINT8 numSet,
                                UINT16 bufSize,
                                IEEEtypes_BssDesc_t  *BssDescSet_p)
{
    mlmeApiSndScanNotificationOnly(vStaInfo_p,
                                   scanResult,
                                   numSet,
                                   bufSize,
                                   (UINT8 *)BssDescSet_p);    
}

/*************************************************************************
* Function: syncSrv_SndJoinCnfm
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
static void syncSrv_SndJoinCnfm(vmacStaInfo_t *vStaInfo_p,
								UINT16 joinResult)
{
   IEEEtypes_JoinCfrm_t JoinCfrm;

   JoinCfrm.Result   = joinResult;
   mlmeApiSndNotification(vStaInfo_p, (UINT8 *)&JoinCfrm, MlmeJoin_Cnfm);
}

/*************************************************************************
* Function: syncSrv_SndStartCnfm
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
static void syncSrv_SndStartCnfm(vmacStaInfo_t *vStaInfo_p,
								 UINT16 startResult)
{
   IEEEtypes_StartCfrm_t StartCfrm;

   StartCfrm.Result   = startResult;
   mlmeApiSndNotification(vStaInfo_p, (UINT8 *)&StartCfrm, MlmeStart_Cnfm);
}

/*************************************************************************
* Function: syncSrv_SndResetCnfm
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
static void syncSrv_SndResetCnfm(vmacStaInfo_t *vStaInfo_p,
								 UINT16 resetResult)
{
   IEEEtypes_ResetCfrm_t ResetCfrm;

   ResetCfrm.Result   = resetResult;
   mlmeApiSndNotification(vStaInfo_p, (UINT8 *)&ResetCfrm, MlmeReset_Cnfm);
}

/*************************************************************************
* Function: syncSrv_UpdateJoinStatus
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern void syncSrv_UpdateJoinStatus(vmacStaInfo_t *vStaInfo_p,
								UINT16 status)
{
    UINT16 joinResult;

    switch(status)
    {
    case JOIN_SUCCESS:
        joinResult = JOIN_RESULT_SUCCESS;
        break;

    case JOIN_FAIL_AUTH_TIMEOUT:
    case JOIN_FAIL_ASSOC_TIMEOUT:
        joinResult = JOIN_RESULT_TIMEOUT;
        break;

    case JOIN_FAIL_AUTH_REJECTED:
    case JOIN_FAIL_ASSOC_REJECTED:
    case JOIN_FAIL_INTERNAL_ERROR:
    default:
        joinResult = JOIN_RESULT_INVALID_PARAMETERS;
        break;

    }
    syncSrv_SndJoinCnfm(vStaInfo_p, joinResult);
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
extern void syncSrv_SndLinkLostInd(vmacStaInfo_t *vStaInfo_p)
{
	vmacStaInfo_t *vStaInfo_tmp_p;
	vmacId_t clientVMacId;

    #ifdef ETH_DEBUG
    eprintf("==> syncSrv_SndLinkLostInd\n");
    #endif /* ETH_DEBUG */
	if(vStaInfo_p == NULL)
	{
		/* This should not happen, but Default to MAC_0 */
		clientVMacId=parentGetVMacId(MAC_0);
		if((vStaInfo_tmp_p = (vmacStaInfo_t *)vmacGetVMacStaInfo(clientVMacId)) == NULL)
		{
			return;
		}
	}
	else
	{
		vStaInfo_tmp_p = vStaInfo_p;
	}
    mlmeApiSndNotification(vStaInfo_tmp_p, NULL, Tbcn);
}

/*************************************************************************
* Function: syncSrvSta_ScanActTimeOut
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern SINT32 syncSrvSta_ScanDwellTimeOut(UINT8 *data)
{ 
#ifdef DFS_PASSIVE_SCAN
    UINT8 currentChnl = 0;
#endif
    vmacStaInfo_t *vStaInfo_p = (vmacStaInfo_t *)data;
    
    #ifdef MLME_POLL_BEACON_Q
    mlmeApiQueryBeaconQueue(vStaInfo_p);
    #endif /* MLME_POLL_BEACON_Q */
#ifdef SOC_W8764
    printk("syncSrvSta_ScanDwellTimeOut \n");
#endif
    if (vStaInfo_p->ScanParams.ScanType == SCAN_ACTIVE)
    {
#ifdef DFS_PASSIVE_SCAN
    currentChnl = mlmeApiGetRfChannel(vStaInfo_p);
     if (!(((currentChnl >= 52) && (currentChnl <= 64)) ||
        ((currentChnl >= 100) && (currentChnl <= 140))))
#endif
        syncSrv_SndProbeReq(vStaInfo_p, TRUE);   
    }

    /* Get and start a scan timer with duration of the maximum channel time */
    #ifdef MLME_SEPERATE_SCAN_TIMER
    mlmeApiStartScanTimer(vStaInfo_p, vStaInfo_p->scanTime_tick, &syncSrvSta_ScanActTimeOut);
    #else
    mlmeApiStartTimer(vStaInfo_p, 
                      (UINT8 *)&vStaInfo_p->scaningTimer,
                      &syncSrvSta_ScanActTimeOut,
                      vStaInfo_p->scanTime_tick);
    #endif /* MLME_SEPERATE_SCAN_TIMER */
    return 0;
}

/*************************************************************************
* Function: syncSrvSta_ScanActTimeOut
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern SINT32 syncSrvSta_ScanActTimeOut(UINT8 *data)
{ 
    vmacStaInfo_t *vStaInfo_p = (vmacStaInfo_t *)data;

    #ifdef MLME_POLL_BEACON_Q
    mlmeApiQueryBeaconQueue(vStaInfo_p);
    #endif /* MLME_POLL_BEACON_Q */
    syncSrvSta_SetNextChannel((vmacStaInfo_t *)data);
    return 0;
}

/*************************************************************************
* Function: syncSrv_JoinActTimeOut
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern SINT32 syncSrv_JoinActTimeOut(UINT8 *data)
{
    vmacStaInfo_t *vStaInfo_p = (vmacStaInfo_t *)data;

    if (vStaInfo_p->macMgmtMain_State == STATE_JOINING)
    {
        /* Notify SME of time out */
        syncSrv_SndJoinCnfm(vStaInfo_p, JOIN_RESULT_TIMEOUT);
        vStaInfo_p->macMgmtMain_State   = STATE_IDLE;
        return 0;
    }
   return -1;
}

/*************************************************************************
* Function: syncSrv_ReJoinActTimeOut
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern SINT32 syncSrv_ReJoinActTimeOut(UINT8 *data)
{
    vmacStaInfo_t *vStaInfo_p = (vmacStaInfo_t *)data;

    if (vStaInfo_p->macMgmtMlme_ThisStaData.CapInfo.Ibss)
    {
        /* bt_todo:: just do a scan all over again */
        return 0;
    }
    return -1;
}

/*************************************************************************
* Function: syncSrv_ReJoinActTimeOut
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern void syncSrv_SetAdhocWatchTimer( vmacStaInfo_t *vStaInfo_p)
{
    /* I moved the adhoc status check to the general status check */
    /* So no need to start an adhoc timer */
    //StartTimer(ADHOC_ACTION, NULL, 0, 60 * 30);
}

/*************************************************************************
* Function: syncSrv_IsLinkConnected
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern BOOLEAN syncSrv_IsLinkConnected( UINT8 *data_p)
{
    vmacStaInfo_t *vStaInfo_p = (vmacStaInfo_t *)data_p;
    if(vStaInfo_p->AssociatedFlag)
    {
        return TRUE;
    }
    return FALSE;
}

/*************************************************************************
* Function: syncSrv_IsIbssMode
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern BOOLEAN syncSrv_IsIbssMode( UINT8 *data_p)
{
    vmacStaInfo_t *vStaInfo_p = (vmacStaInfo_t *)data_p;
    if(vStaInfo_p->macMgmtMlme_ThisStaData.BssType == BSS_INDEPENDENT)
    {
        return TRUE;
    }
    return FALSE;
}
    
/*************************************************************************
* Function: syncSrv_AdhocActTimeOut
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern SINT32 syncSrv_AdhocActTimeOut(UINT8 *data)
{
    vmacStaInfo_t *vStaInfo_p = (vmacStaInfo_t *)data;
    static SINT8 backOffCnt = -1;
    UINT32 seed;

    seed = mlmeApiGetTime(vStaInfo_p);
    seed += *(UINT32 *)&(vStaInfo_p->macMgmtMlme_ThisStaData.BssId);
    if(backOffCnt == -1)
    {
        /*eprintf("syncSrv_AdhocActTimeOut:: Set random backoff\n");*/    
        //backOffCnt = (mlmeApiGenRandomUINT32() % 4) + 1;
        backOffCnt = (mlmeApiGenRandomUINT32(vStaInfo_p, seed) % 31) + 15;
    }

    if (vStaInfo_p->macMgmtMain_State == STATE_JOINED || vStaInfo_p->macMgmtMain_State == STATE_IBSS_STARTED )
    {  
        if(vStaInfo_p->Adhoc_Active)
        {
            vStaInfo_p->Adhoc_Active = 0;
            //backOffCnt = (mlmeApiGenRandomUINT32(vStaInfo_p, seed) % 31) + 15;
            backOffCnt = (mlmeApiGenRandomUINT32(vStaInfo_p, seed) % 11) + 11;
            if(vStaInfo_p->Station_p->NetworkCtrl.isB_Network)
            {
                vStaInfo_p->Station_p->NetworkCtrl.isB_Network = 0;
            }
            else
            {
                /* There is no present of b-client for at least adhoc time period */
                /* Safe to assume that this is just a G-clients network */
                vStaInfo_p->Station_p->NetworkCtrl.RateOptions = STA_MIXED_MODE;
            }
            return 0;
        }
        else
        {
            backOffCnt--;
            syncSrv_SndProbeReq(vStaInfo_p, FALSE);
            vStaInfo_p->g_rcvdProbeRsp = 0;

            if(backOffCnt == 0)
            {
                /* no one is in our network so do a scan again */
                #ifdef ETH_DEBUG
                eprintf("syncSrv_AdhocActTimeOut:: Scan again\n");
                #endif /* ETH_DEBUG */
                syncSrv_SndLinkLostInd(vStaInfo_p);
                backOffCnt = -1;
                return -1;
            }
            return 0;
        }
    }
    return 0;
}

/*************************************************************************
* Function: syncSrv_StatusCheckTimeOut
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern void syncSrv_StatusCheckTimeOut(UINT8 *data)
{
    vmacStaInfo_t *vStaInfo_p = (vmacStaInfo_t *)data;
	UINT16 macId;
    UINT8  macIndex;
#ifdef AMPDU_SUPPORT_TX_CLIENT
	vmacEntry_t  *vmacEntry_p;
    struct net_device *staDev = NULL;
    struct wlprivate *stapriv = NULL;
    vmacApInfo_t *vmacSta_p = NULL;
    
    
    vmacEntry_p = (vmacEntry_t *)vStaInfo_p->vMacEntry_p;
    staDev = (struct net_device *)vmacEntry_p->privInfo_p;
    stapriv = NETDEV_PRIV_P(struct wlprivate, staDev);
    vmacSta_p = (vmacApInfo_t *)stapriv->vmacSta_p;
#endif

	macId = mlmeApiGetMacId(vStaInfo_p);
    macIndex = mlmeApiGetMacIndex(vStaInfo_p);

    if(vStaInfo_p->AssociatedFlag)
    {
        /* For checking status when Associated */
        if(vStaInfo_p->mib_WB_p->opMode)
        {
            /* Adhoc Mode */
            vStaInfo_p->linkQuality = 100;
            //syncSrv_AdhocActTimeOut((UINT8 *)vStaInfo_p);
        }
        else 
        {
			if(ClientModeDataCount[macIndex] == 0)	
            {
                /* Infrastructure Mose */
                /* Check for link lost */
                //if(checkLink && !vStaInfo_p->rxBcnCnt)
#ifndef SC_PALLADIUM
                if (vStaInfo_p->urProbeRspMissedCnt >= g_PrbeReqCheckTheshold[macIndex])      
                {
                    if(!vStaInfo_p->g_rcvdProbeRsp)/* added probe req link check */
                    {
                        /* Link Lost due to not receiving AP beacons */
                        #ifdef ETH_DEBUG
                        eprintf("==> syncSrv_StatusCheckTimeOut: send syncSrv_SndLinkLostInd \n");
                        #endif /* ETH_DEBUG */
#ifdef AMPDU_SUPPORT_TX_CLIENT
	                    cleanupAmpduTx(vmacSta_p, (UINT8 *)&vStaInfo_p->macMgmtMlme_ThisStaData.BssId[0]);
#endif                        
                        mlmeApiEventNotification(vStaInfo_p,
                                                    Tbcn,
                                                    &vStaInfo_p->macMgmtMlme_ThisStaData.BssId[0], 
                                                    0);
                        mlmeApiUpdateLinkStatus(vStaInfo_p, LINK_DOWN);
                        vStaInfo_p->AssociatedFlag=0;
						vStaInfo_p->staSecurityMibs.mib_PrivacyTable_p->RSNLinkStatus = 0;  	
                        syncSrv_SndLinkLostInd(vStaInfo_p);
						ClientModeTxMonitor = 0;		
                    }
                    vStaInfo_p->urProbeRspMissedCnt = 0;
                }
                else //if(!vStaInfo_p->rxBcnCnt)
                {
                    #ifdef MLME_SW_LINK_LOST 
                    /* added probe req link check */
                    vStaInfo_p->urProbeRspMissedCnt++;
                    syncSrv_SndProbeReq(vStaInfo_p, FALSE);
                    vStaInfo_p->g_rcvdProbeRsp = 0;
                    #endif /* MLME_SW_LINK_LOST */
                }
#endif
            }
            else
            {		
                ClientModeDataCount[macIndex] = 0; 
                vStaInfo_p->urProbeRspMissedCnt = 0;
            }
            /* Clip link quality at 100 */
            if((vStaInfo_p->linkQuality= (vStaInfo_p->rxBcnPeriod/g_CheckStatusTime[macIndex]) * vStaInfo_p->rxBcnCnt) > 100)
            {
                vStaInfo_p->linkQuality = 100;
            }
        }

        #ifdef ETH_DEBUG
        eprintf("rxBcn Count = %d: Period = %d: Quality = %d \n", vStaInfo_p->rxBcnCnt,vStaInfo_p->rxBcnPeriod, vStaInfo_p->linkQuality);
        #endif /* ETH_DEBUG */
        vStaInfo_p->rxBcnCnt = 0;
    }
    else if((vStaInfo_p->macMgmtMain_State == STATE_IDLE) && !vStaInfo_p->mib_WB_p->opMode)
    {
        syncSrv_SndProbeReq(vStaInfo_p, FALSE);
        if (!vStaInfo_p->keyMgmtInfoSta_p->sta_MIC_Error.disableStaAsso)
        {
            if (vStaInfo_p->staProbeReqCntIdle++ > IDLE_PROBEREQ_CNT)
            {
                linkMgtReStart(0, vmacEntry_p);
                vStaInfo_p->staProbeReqCntIdle = 0;
            }
        }
        else
        {
            vStaInfo_p->staProbeReqCntIdle = 0;
        }
    }

    TimerRearm(&vStaInfo_p->statusTimer, g_CheckStatusTime[macIndex]);

}

/*************************************************************************
* Function: syncSrv_KeepAliveTimeOut
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern void syncSrv_KeepAliveTimeOut(UINT8 *data)
{
    vmacStaInfo_t *vStaInfo_p = (vmacStaInfo_t *)data;
    UINT16 macId;
    UINT8  macIndex;
    UINT16 ii = 0;
    vmacEntry_t  *vmacEntry_p;

    vmacEntry_p = (vmacEntry_t *)vStaInfo_p->vMacEntry_p;

    macId = mlmeApiGetMacId(vStaInfo_p);
    macIndex = mlmeApiGetMacIndex(vStaInfo_p);
    
    if(!g_KeepAliveEnable)
	return;
    
    if(vStaInfo_p->AssociatedFlag)
    {
	if(vStaInfo_p->isParentSession)    
	if(mlmeApiIsTrunkIdActive(vStaInfo_p, vmacEntry_p->trunkId))
    	/* For checking status when Associated */
        if(!vStaInfo_p->mib_WB_p->opMode)
        {
           for(ii = 0; ii < 3; ii++){
              if(SUCCESS == mlmeApiSendNullDataPktUr(vStaInfo_p, &vStaInfo_p->macMgmtMlme_ThisStaData.BssId, &vmacEntry_p->vmacAddr))
              {
                    break;
              }
           }
        }

    }

    TimerRearm(&vStaInfo_p->keepaliveTimer, g_KeepAliveTime);

}

/*************************************************************************
* Function: syncSrvSta_ParseAttrib
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern void *syncSrvSta_ParseAttrib( dot11MgtFrame_t *mgtFrame_p, UINT8 attrib)
{
    void *data_p;
    UINT32 lenPacket;
    UINT32 lenOffset;

    lenPacket = mgtFrame_p->Hdr.FrmBodyLen;
    
    data_p = &mgtFrame_p->Body.Bcn.CapInfo;
    data_p += sizeof(IEEEtypes_CapInfo_t);

    lenOffset = sizeof(IEEEtypes_MgmtHdr_t)
              + sizeof(IEEEtypes_TimeStamp_t)
              + sizeof(IEEEtypes_BcnInterval_t) 
              + sizeof(IEEEtypes_CapInfo_t);

    while(lenOffset < lenPacket)
    {
        if(*(IEEEtypes_ElementId_t *)data_p == attrib)
        {
            return data_p;
        }

        lenOffset += (2 + *((UINT8 *)(data_p + 1)));
        data_p += (2 + *((UINT8 *)(data_p + 1)));
    }
    return NULL;
}

/*************************************************************************
* Function: syncSrv_ValidatePeerSetting
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern BOOLEAN syncSrv_ValidatePeerSetting(vmacStaInfo_t *vStaInfo_p,
                                             dot11MgtFrame_t *MgmtMsg_p)
{
    void *attrib_p;
    UINT16 profileCapInfo;
    vmacEntry_t  *vmacEntry_p;
    UINT8 ssidLen;
    UINT8  peerBasicRatesBuf[IEEEtypes_MAX_DATA_RATES_G];
    UINT32 peerBasicRatesLen = 0;
    UINT8  profileBasicRatesBuf[IEEEtypes_MAX_DATA_RATES_G];
    UINT32 profileBasicRatesLen = 0;
    UINT32 i, j;
    BOOLEAN matchFound;

    vmacEntry_p = (vmacEntry_t *)vStaInfo_p->vMacEntry_p;
    if (memcmp(&(MgmtMsg_p->Hdr.BssId),
                    &(vStaInfo_p->macMgmtMlme_ThisStaData.BssId),
                    sizeof(IEEEtypes_MacAddr_t)))
    {
       return FALSE;
    }
    /* Validate the CapInfo */
    memcpy((void *)&profileCapInfo, (const void *)&vStaInfo_p->bssDescProfile_p->Cap, sizeof(UINT16));
    if((*(UINT16 *)&MgmtMsg_p->Body.ProbeRsp.CapInfo&MLME_CAPINFO_VALIDATION_MASK) != 
        (profileCapInfo&MLME_CAPINFO_VALIDATION_MASK))
    {
        return FALSE;
    }
    /* Validate SSID */
    if((attrib_p = syncSrvSta_ParseAttrib(MgmtMsg_p, SSID)) != NULL)
    {  
        ssidLen = util_ListLen(&vStaInfo_p->bssDescProfile_p->SsId[0], IEEEtypes_SSID_SIZE);
        if((ssidLen != ((IEEEtypes_SsIdElement_t *)attrib_p)->Len)
           || memcmp(&((IEEEtypes_SsIdElement_t *)attrib_p)->SsId[0], 
                     &vStaInfo_p->bssDescProfile_p->SsId[0], 
                     ssidLen))
    
        {
            return FALSE;
        }
    }
    /* Validate RF Channel */
    if((attrib_p = syncSrvSta_ParseAttrib(MgmtMsg_p, DS_PARAM_SET)) != NULL)
    {  
        if (((IEEEtypes_DsParamSet_t *)attrib_p)->CurrentChan 
            != vStaInfo_p->bssDescProfile_p->PhyParamSet.DsParamSet.CurrentChan)
        {
            return FALSE;
        }
    }
    /* Validate Basic Rates */
    memset(&peerBasicRatesBuf[0], 0, IEEEtypes_MAX_DATA_RATES_G);
    memset(&profileBasicRatesBuf[0], 0, IEEEtypes_MAX_DATA_RATES_G);
    if((attrib_p = syncSrvSta_ParseAttrib(MgmtMsg_p, SUPPORTED_RATES)) != NULL)
    {  
        for(i= 0; i < ((IEEEtypes_SuppRatesElement_t *)attrib_p)->Len; i++)
        {
            if(((IEEEtypes_SuppRatesElement_t *)attrib_p)->Rates[i] > IEEEtypes_BASIC_RATE_FLAG)
            {
                if(peerBasicRatesLen < IEEEtypes_MAX_DATA_RATES_G)
                {
                    peerBasicRatesBuf[peerBasicRatesLen] = ((IEEEtypes_SuppRatesElement_t *)attrib_p)->Rates[i];
                    peerBasicRatesLen++;
                }
                else
                {
                    return FALSE;
                }
            }
        }
    }
    if((attrib_p = syncSrvSta_ParseAttrib(MgmtMsg_p, EXT_SUPPORTED_RATES)) != NULL)
    {  
        for(i= 0; i < ((IEEEtypes_SuppRatesElement_t *)attrib_p)->Len; i++)
        {
            if(((IEEEtypes_SuppRatesElement_t *)attrib_p)->Rates[i] > IEEEtypes_BASIC_RATE_FLAG)
            {
                if(peerBasicRatesLen < IEEEtypes_MAX_DATA_RATES_G)
                {
                    peerBasicRatesBuf[peerBasicRatesLen] = ((IEEEtypes_SuppRatesElement_t *)attrib_p)->Rates[i];
                    peerBasicRatesLen++;
                }
                else
                {
                    return FALSE;
                }
            }
        }
    }
    for(i= 0; i < IEEEtypes_MAX_DATA_RATES_G; i++)
    {
        if(vStaInfo_p->bssDescProfile_p->DataRates[i] > IEEEtypes_BASIC_RATE_FLAG)
        {
            profileBasicRatesBuf[profileBasicRatesLen] = vStaInfo_p->bssDescProfile_p->DataRates[i];
            profileBasicRatesLen++;
        }
    }
    if(peerBasicRatesLen != profileBasicRatesLen)
    {
        return FALSE;
    }
    for(i=0; i < profileBasicRatesLen; i++)
    {
        matchFound = FALSE;
        for(j=0; (j < peerBasicRatesLen) && !matchFound; j++)
        {
            if(profileBasicRatesBuf[i] == peerBasicRatesBuf[j])
            {
                matchFound = TRUE;
            }
        }
        if(!matchFound)
        {
            return FALSE;
        }
    }
    return TRUE;
}

/*************************************************************************
* Function: syncSrv_ProbeRspRcvd
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern void syncSrv_ProbeRspRcvd( vmacStaInfo_t *vStaInfo_p,
                                  dot11MgtFrame_t *MgmtMsg_p, 
                                  UINT8 *rfHdr_p )
{
    UINT8 ssidLen;
    UINT8 tmpBufSSID[IEEEtypes_SSID_SIZE];
//    extStaDb_StaInfo_t *wbStaInfo_p;
//    UINT32 local_urAid;
//    vmacEntry_t  *vmacEntry_p;
    void *attrib_p;
//    IEEEtypes_SuppRatesElement_t *PeerSupportedRates_p;
//    IEEEtypes_ExtSuppRatesElement_t *PeerExtSupportedRates_p;
//    UINT32                 capInfo;

    /* Reset check probe response */
    if( vStaInfo_p->g_rcvdProbeRsp == 0 )
    {
        ssidLen = util_CopyList(tmpBufSSID, vStaInfo_p->staSystemMibs.mib_StaCfg_p->DesiredSsId, IEEEtypes_SSID_SIZE);
        if(ssidLen > 0)
        {
            //if((ssidLen != MgmtMsg_p->Body.ProbeRsp.SsId.Len)
            //      || memcmp(MgmtMsg_p->Body.ProbeRsp.SsId.SsId, vStaInfo_p->mib_StaCfg_p->DesiredSsId, ssidLen))
            //{
            //    eprintf(" ProbeResp SSID error ssidLen = %d mgmtLen = %d SSID = %s.\n",
            //             ssidLen, MgmtMsg_p->Body.ProbeRsp.SsId.Len, MgmtMsg_p->Body.ProbeRsp.SsId.SsId);
            //    return;
            //}
            /* Verify that probe response is from associated AP/UR. */
            if (memcmp(&MgmtMsg_p->Hdr.BssId[0], &vStaInfo_p->macMgmtMlme_ThisStaData.BssId[0], sizeof(IEEEtypes_MacAddr_t)))
            {
                return;
            }
            if(syncSrv_ValidatePeerSetting(vStaInfo_p,MgmtMsg_p) == FALSE)
            {
                return;
            }
            /* This informs the status timer that link is active */
            vStaInfo_p->g_rcvdProbeRsp = 1;
            vStaInfo_p->urProbeRspMissedCnt = 0;
			ClientModeTxMonitor = 1;				
            if(vStaInfo_p->IBssStartFlag)
            {
                vStaInfo_p->Adhoc_Active = 1;
            }
        }

        if(vStaInfo_p->AssociatedFlag)
        {
            /* Check ERP */
            if((attrib_p = syncSrvSta_ParseAttrib(MgmtMsg_p, ERP_INFO)) != NULL)
            {
                vStaInfo_p->Station_p->ChipCtrl.Protection4g = ((IEEEtypes_ERPInfoElement_t *)attrib_p)->ERPInfo.UseProtection;
            }
            else
            {
                vStaInfo_p->Station_p->ChipCtrl.Protection4g = 0;
            }
        }
    }


}

/*************************************************************************
* Function: syncSrv_SndProbeReq
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
static SINT32 syncSrv_SndProbeReq( vmacStaInfo_t *vStaInfo_p, BOOLEAN isScan )
{
    UINT32 lenSsId;
    IEEEtypes_MacAddr_t     destAddr;
    IEEEtypes_MacAddr_t     bssIdAddr;
    dot11MgtFrame_t         *probeMsg_p;
    UINT8                   rateSupport[MAX_B_DATA_RATES];
    UINT8                   rateExt[MAX_G_DATA_RATES];
    vmacEntry_t           * vmacEntry_p = (vmacEntry_t *)vStaInfo_p->vMacEntry_p;
    UINT8 supportedRateLen = 0;
    UINT8 extendedRateLen = 0;
	struct net_device *staDev = (struct net_device *)vmacEntry_p->privInfo_p;
	struct wlprivate *stapriv = NULL;
	vmacApInfo_t *vmacSta_p; 
	IEEEtypes_InfoElementHdr_t *ieHdr_p;
	stapriv = NETDEV_PRIV_P(struct wlprivate, staDev);
	vmacSta_p = (vmacApInfo_t *)stapriv->vmacSta_p;
   
    if((probeMsg_p = mlmeApiAllocMgtMsg(vmacEntry_p->phyHwMacIndx)) == NULL)
    {
        return MLME_FAILURE;
    }
    /* Fill out the MAC header */
    if (!isScan)
    {
        memcpy(&destAddr,
			   &vStaInfo_p->macMgmtMlme_ThisStaData.BssId[0],
			   sizeof(IEEEtypes_MacAddr_t));
        memcpy(&bssIdAddr,
			   &vStaInfo_p->macMgmtMlme_ThisStaData.BssId[0],
			   sizeof(IEEEtypes_MacAddr_t));
    }
    else
    {
        memset(&destAddr, 0xFF, sizeof(IEEEtypes_MacAddr_t));
        memcpy(&bssIdAddr,&vStaInfo_p->ScanParams.BssId,sizeof(IEEEtypes_MacAddr_t));
    }
#ifdef SOC_W8764
    printk("syncSrv_SndProbeReq for SSID = %s \n", vStaInfo_p->ScanParams.SsId);
#endif
    mlmePrepDefaultMgtMsg_Sta(vStaInfo_p,
                              probeMsg_p, 
                              &destAddr, 
                              IEEE_MSG_PROBE_RQST,
                              &bssIdAddr);
    probeMsg_p->Hdr.FrmBodyLen = 0;
    /* Add SSID Attrib */
	if(!isScan)
	{
		lenSsId = strlen(vStaInfo_p->staSystemMibs.mib_StaCfg_p->DesiredSsId);
    	syncSrv_AddAttrib(probeMsg_p, 
                               SSID,
                               vStaInfo_p->staSystemMibs.mib_StaCfg_p->DesiredSsId,
	                           lenSsId);
        memcpy(&rateSupport[0], &vStaInfo_p->bOpRateSet[0], MAX_B_DATA_RATES);
        supportedRateLen = util_ListLen(&rateSupport[0], MAX_B_DATA_RATES);
        memcpy(&rateExt[0], &vStaInfo_p->gOpRateSet[0], MAX_G_DATA_RATES);
        extendedRateLen = util_ListLen(&rateExt[0], MAX_G_DATA_RATES);
	}
	else
	{
        lenSsId = strlen(vStaInfo_p->ScanParams.SsId);
    	syncSrv_AddAttrib(probeMsg_p, 
                               SSID,
                               vStaInfo_p->ScanParams.SsId,
	                           lenSsId);
        rateSupport[0] = 0x2;
        rateSupport[1] = 0x4;
        rateSupport[2] = 0xb;
        rateSupport[3] = 0x16;
        supportedRateLen = 0x4;
        rateExt[0]     = 0xc;
        rateExt[1]     = 0x12;
        rateExt[2]     = 0x18;
        rateExt[3]     = 0x24;
        rateExt[4]     = 0x30;
        rateExt[5]     = 0x48;
        rateExt[6]     = 0x60;
        rateExt[7]     = 0x6c;
        extendedRateLen = 0x8;
	}
    /* Add Support Rates Attrib */
    if(supportedRateLen)
    {
        syncSrv_AddAttrib(probeMsg_p, 
                          SUPPORTED_RATES,
                          &rateSupport[0],
                          supportedRateLen);
    }
    /* Add Extended Support Rates Attrib */
    if(!supportedRateLen && extendedRateLen)
    {
        syncSrv_AddAttrib(probeMsg_p, 
                          SUPPORTED_RATES,
                          &rateExt[0],
                          extendedRateLen);
    }
    else if(extendedRateLen)
    {
        syncSrv_AddAttrib(probeMsg_p, 
                          EXT_SUPPORTED_RATES,
                          &rateExt[0],
                          extendedRateLen);
    }
	/* Adding MRVL Rptr IE */
	if (*(vmacSta_p->Mib802dot11->mib_STAMacCloneEnable) == 2)
	{
		UINT8 *p = (UINT8 *)&probeMsg_p->Body;
		/* Pack in Marvell IE. */ 
		ieHdr_p = (IEEEtypes_InfoElementHdr_t *)&(p[probeMsg_p->Hdr.FrmBodyLen]);
		ieHdr_p->ElementId = PROPRIETARY_IE;
		ieHdr_p->Len = 6;
		probeMsg_p->Hdr.FrmBodyLen += sizeof(IEEEtypes_InfoElementHdr_t);
		memcpy((void *)&(p[probeMsg_p->Hdr.FrmBodyLen]), "\x00\x50\x43\x03\xE0\0x00", ieHdr_p->Len);
		probeMsg_p->Hdr.FrmBodyLen += ieHdr_p->Len;
		memcpy((void *)&(p[probeMsg_p->Hdr.FrmBodyLen]), vmacSta_p->Mib802dot11->mib_RptrDeviceType, strlen(vmacSta_p->Mib802dot11->mib_RptrDeviceType)); 
		ieHdr_p->Len +=	MAXRPTRDEVTYPESTR; /* Append Rptr Device type: 32 bytes */
		probeMsg_p->Hdr.FrmBodyLen += ieHdr_p->Len;
	}

    if (mlmeApiSendMgtMsg_Sta(vStaInfo_p, probeMsg_p, NULL) == MLME_FAILURE)
    {
        return MLME_FAILURE;
    }
    return MLME_SUCCESS;
}

//#ifdef MULTI_CAST_SUPPORT
#if 1
/*************************************************************************
* Function: syncSrv_ProbeReqRcvd
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern void syncSrv_ProbeReqRcvd( vmacStaInfo_t *vStaInfo_p, 
                                  dot11MgtFrame_t *MgmtMsg_p, 
                                  UINT8 *rfHdr_p)
{
    UINT8 ssidLen = MgmtMsg_p->Body.ProbeRqst.SsId.Len;

	if(ssidLen && (*(UINT8 *)&MgmtMsg_p->Body.ProbeRqst.SsId.SsId[0] != 0x0))
	{
		if((ssidLen != vStaInfo_p->macMgmtMlme_ThisStaData.BssSsIdLen) 
		   || memcmp((void *)MgmtMsg_p->Body.ProbeRqst.SsId.SsId,
					 vStaInfo_p->macMgmtMlme_ThisStaData.BssSsId,
					 vStaInfo_p->macMgmtMlme_ThisStaData.BssSsIdLen))
		{
			return;
		}
	}
	/* Pass all check so let's send out the response */
	//syncSrv_SndProbeRsp(vStaInfo_p, MgmtMsg_p->Hdr.SrcAddr, (RxSign_t *)rfHdr_p);
}
#endif /* MULTI_CAST_SUPPORT */

#ifdef STA_ADHOC_SUPPORTED
/*************************************************************************
* Function: syncSrv_WriteBcn
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
static void syncSrv_UpdateAdhocTxInfo( vmacStaInfo_t *vStaInfo_p,
                                       IEEEtypes_StartCmd_t *StartCmd_p,
                                       IEEEtypes_MacAddr_t  *BssId_p,
                                       UINT8 packType,
                                       macmgmtQ_MgmtMsg_t *bcnBuf_p,
                                       txBcnInfo_t *MgtTxInfo)
{
   UINT8                         *nextElement_p;
   IEEEtypes_SsIdElement_t       *ssId_p;
   IEEEtypes_SuppRatesElement_t  *suppRates_p;
   IEEEtypes_PhyParamSet_t       *phyParamSet_p;
   IEEEtypes_SsParamSet_t        *ssParamSet_p;
   IEEEtypes_Tim_t               *tim_p;
   IEEEtypes_MacAddr_t            bcastAddr = {0xff,0xff,0xff,0xff,0xff,0xff};
   IEEEtypes_ExtSuppRatesElement_t  *ExtsuppRates_p;
   IEEEtypes_ERPInfoElement_t       *ERPInfo_p;
   BOOLEAN                        AddERP = FALSE;
    
   /*----------------------------------*/
   /* First fill out the beacon header */
   /*----------------------------------*/
   mlmePrepDefaultMgtMsg_Sta(vStaInfo_p,
                             (dot11MgtFrame_t *)&(bcnBuf_p->Hdr),
                             &bcastAddr, 
                             packType,
                             BssId_p );
   
   /*-------------------------------------------------------------*/
   /* Now fill out the beacon specific information; first fill in */
   /* information given by the start command.                     */
   /*-------------------------------------------------------------*/
   bcnBuf_p->Body.Bcn.BcnInterval = StartCmd_p->BcnPeriod;
   bcnBuf_p->Body.Bcn.CapInfo     = StartCmd_p->CapInfo;
   if(bcnBuf_p->Body.Bcn.CapInfo.ShortSlotTime)
   {
        mlmeApiHwSetShortSlotTime(vStaInfo_p, 1);
   }
   else
   {
       mlmeApiHwSetShortSlotTime(vStaInfo_p, 0);
   }
   
   /*-------------------------------------------------------------*/
   /* Next fill in the station's SS ID and then advance a pointer */
   /* to the where the supported rates will be placed             */
   /*-------------------------------------------------------------*/
   nextElement_p     = (UINT8 *)&(bcnBuf_p->Body.Bcn.SsId);
   ssId_p            = &(bcnBuf_p->Body.Bcn.SsId);
   ssId_p->ElementId = SSID;
   ssId_p->Len       = util_CopyList(ssId_p->SsId,
                                     StartCmd_p->SsId,
                                     IEEEtypes_SSID_SIZE);

   nextElement_p  += sizeof(IEEEtypes_ElementId_t) +
                     sizeof(IEEEtypes_Len_t)       +
                     ssId_p->Len;

   /*----------------------------------------------------------*/
   /* Next fill in the station's operational rate set, taking  */
   /* care to mark which rates are also in the basic rate set. */
   /*----------------------------------------------------------*/
   suppRates_p            = (IEEEtypes_SuppRatesElement_t *)nextElement_p;
   suppRates_p->ElementId = SUPPORTED_RATES;
   suppRates_p->Len       = util_CopyList(suppRates_p->Rates,
                                          StartCmd_p->OpRateSet,
                                          IEEEtypes_MAX_DATA_RATES_G);

   if(suppRates_p->Len > MAX_B_DATA_RATES)
   {
      suppRates_p->Len = MAX_B_DATA_RATES;
      AddERP = TRUE;
   }
   
   nextElement_p += sizeof(IEEEtypes_ElementId_t) +
                    sizeof(IEEEtypes_Len_t)       +
                    suppRates_p->Len;

   /*---------------------------------*/
   /* Next copy in the phy parameters */
   /*---------------------------------*/
   phyParamSet_p = (IEEEtypes_PhyParamSet_t *)nextElement_p;

   if(*(UINT8 *)&StartCmd_p->PhyParamSet ==  FH_PARAM_SET )
   {
       phyParamSet_p->FhParamSet = StartCmd_p->PhyParamSet.FhParamSet;
       nextElement_p             += sizeof(IEEEtypes_FhParamSet_t);
   }
   else if (*(UINT8 *)&StartCmd_p->PhyParamSet == DS_PARAM_SET)
   {
      phyParamSet_p->DsParamSet = StartCmd_p->PhyParamSet.DsParamSet;
      nextElement_p             += sizeof(IEEEtypes_DsParamSet_t); 
   }

   /*-----------------------------------------*/
   /* Next copy in the service set parameters */
   /*-----------------------------------------*/
   ssParamSet_p = (IEEEtypes_SsParamSet_t *)nextElement_p;

   if(StartCmd_p->BssType == BSS_INFRASTRUCTURE)
   {
      ssParamSet_p->CfParamSet = StartCmd_p->SsParamSet.CfParamSet;
      nextElement_p            += sizeof(IEEEtypes_CfParamSet_t);
   }
   else if(StartCmd_p->BssType == BSS_INDEPENDENT)
   {
      ssParamSet_p->IbssParamSet = StartCmd_p->SsParamSet.IbssParamSet;
      nextElement_p              += sizeof(IEEEtypes_IbssParamSet_t);
   }

   /*-----------------------------------------------------------*/
   /* Next copy in the ERP parameters */
   /*-----------------------------------------------------------*/
   if(vStaInfo_p->macMgt_StaMode == CLIENT_MODE_G)
   {
        ERPInfo_p        = (IEEEtypes_ERPInfoElement_t *)nextElement_p;
        ERPInfo_p->ElementId = ERP_INFO;
        ERPInfo_p->Len   = sizeof(IEEEtypes_ERPInfo_t);
        ERPInfo_p->ERPInfo.BarkerPreamble = 0;
        ERPInfo_p->ERPInfo.NonERPPresent  = 0;
        ERPInfo_p->ERPInfo.UseProtection  = 1;
        ERPInfo_p->ERPInfo.reserved       = 0;
        nextElement_p                    += sizeof(IEEEtypes_ERPInfoElement_t);

        ExtsuppRates_p   = (IEEEtypes_ExtSuppRatesElement_t *)nextElement_p;
        ExtsuppRates_p->ElementId = EXT_SUPPORTED_RATES;
       

      ExtsuppRates_p->Len = util_CopyList(ExtsuppRates_p->Rates,
                            &StartCmd_p->OpRateSet[MAX_B_DATA_RATES],
                            IEEEtypes_MAX_DATA_RATES_G-MAX_B_DATA_RATES);
      
      nextElement_p  += sizeof(IEEEtypes_ElementId_t) + 
                        sizeof(IEEEtypes_Len_t)       + 
                        ExtsuppRates_p->Len;
   }

   /*-----------------------------------------------------------*/
   /* There is no TIM for an IBSS beacon, but we will still set */
   /* a pointer to where the TIM would be in an infrastructure  */
   /* beacon because the transmit hardware MAC may still make   */
   /* use of it.                                                */
   /*-----------------------------------------------------------*/
   tim_p            = (IEEEtypes_Tim_t *)nextElement_p;
   tim_p->ElementId = TIM;
   tim_p->Len       = 0;

   /*-------------------------------------------------------------*/
   /* Fill out the transmit information structure, which preceeds */
   /* the beacon message in the overall structure given to the    */
   /* MAC transmit hardware. Not that the header address is set   */
   /* to point to an address in memory just beyond where the      */
   /* transmit information structure is to be stored; this is     */
   /* where the MAC transmit hardware can access it.              */
   /*-------------------------------------------------------------*/
   MgtTxInfo->RetryCnt      = vStaInfo_p->staSystemMibs.mib_OpData_p->ShortRetryLim;
   MgtTxInfo->Status        = 0;
   MgtTxInfo->TxParam       = 0x700;
   MgtTxInfo->Service       = 0;
   MgtTxInfo->Rate        = mib_BeaconRate[mlmeApiGetMacIndex(vStaInfo_p)];
   MgtTxInfo->Power         = 0;
   MgtTxInfo->Tsf0          = 0;
   MgtTxInfo->Tsf1          = 0;
   MgtTxInfo->FragBasicDurId0 = 0;
   MgtTxInfo->FragBasicDurId1 = 0;

   if( !mib_BeaconPreamble[mlmeApiGetMacIndex(vStaInfo_p)] )
   {
       // short preamble
       MgtTxInfo->TxParam |= 0x4;
   }
   
   MgtTxInfo->HdrAddr = (IEEEtypes_Frame_t *)bcnBuf_p;

   /*----------------------------------------------------------------*/
   /* The beacon body length is calculated as the difference between */
   /* the TIM pointer and the beacon buffer pointer, less the size   */
   /* of the header since that is obviously not part of the body.    */
   /*----------------------------------------------------------------*/
   bcnBuf_p->Hdr.FrmBodyLen = (UINT32)tim_p    -
                                  (UINT32)bcnBuf_p -
                                  sizeof(IEEEtypes_MgmtHdr_t);
  


   /*--------------------------------------------------------------*/
   /* The following offset to the CF Parameters in the beacon is   */
   /* calculated for the MAC transmit hardware as follows:         */
   /*                                                              */
   /* 1) First, the offset to be calculated is the number of bytes */
   /*    the transmitter will transmit, starting from the frame    */
   /*    control field in the IEEE header, up to the point of the  */
   /*    CF parameter set information in the beacon body.          */
   /*                                                              */
   /* 2) Since the beacon body byte length field is not to be      */
   /*    transmitted, we must subtract out the length of this      */
   /*    field.                                                    */
   /*                                                              */
   /* 3) Since the 4th address in the IEEE header is not to be     */
   /*    transmitted (since there is no 4th address for management */
   /*    messages), the transmitter skips over this field - hence, */
   /*    we must subtract out the length of this field as well.    */
   /*                                                              */
   /* So, taking the difference between pointers to the CF         */
   /* parameter set and the beacon buffer pointer, and subtracting */
   /* out the additional fields mentioned above, yields the        */
   /* number of bytes tranmitted up to the point of the CF         */
   /* paramters.                                                   */
   /*--------------------------------------------------------------*/
   MgtTxInfo->CfOffset = (UINT32)ssParamSet_p                 -
                        (UINT32)bcnBuf_p                     -
                        sizeof(bcnBuf_p->Hdr.FrmBodyLen) -
                        sizeof(bcnBuf_p->Hdr.Rsrvd);
  

   /*-----------------------------------------------------------*/
   /* The following offset to the TIM (which has zero length as */
   /* there is no TIM in an IBSS beacon), is calculated for the */
   /* MAC transmit hardware according to the same rules given   */
   /* above (and for the same reasons).                         */
   /*-----------------------------------------------------------*/
   MgtTxInfo->TimOffset = (UINT32)tim_p                        -
                         (UINT32)bcnBuf_p                     -
                         sizeof(bcnBuf_p->Hdr.FrmBodyLen) -
                         sizeof(bcnBuf_p->Hdr.Rsrvd);

   
   /*--------------------------------------------------------------*/
   /* The probe response length is merely the complete length of   */
   /* the beacon frame that is actually tranmitted - not just the  */
   /* beacon body. Hence, its length is the body length, plus what */
   /* is transmitted in the IEEE header, as described above, and   */
   /* also the FCS length.                                         */
   /*--------------------------------------------------------------*/
   MgtTxInfo->ProbeRespLen = bcnBuf_p->Hdr.FrmBodyLen         +
                           sizeof(IEEEtypes_MgmtHdr_t)          -
                           sizeof(bcnBuf_p->Hdr.FrmBodyLen) -
                           sizeof(bcnBuf_p->Hdr.Rsrvd)      +
                           sizeof(UINT32);

   /*--------------------------------------------------------------*/
   /* For hardware generated probe response frames, RTS needs to   */
   /* configured.                                                  */
   /*--------------------------------------------------------------*/
   if( IEEE_MSG_PROBE_RSP == packType )
   {
       if( vStaInfo_p->staSystemMibs.mib_OpData_p->RtsThresh < (MgtTxInfo->ProbeRespLen) )
           MgtTxInfo->TxParam |= 0x20;
   }

}  /* End syncSrv_UpdateAdhocTxInfo() */
#endif /* STA_ADHOC_SUPPORTED */

/*************************************************************************
* Function: syncSrv_ScanPrep
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
static void syncSrv_ScanPrep(vmacStaInfo_t *vStaInfo_p)
{
    UINT32  i;

    #ifdef ETH_DEBUG
	eprintf("==> syncSrv_ScanPrep\n");
    #endif /* ETH_DEBUG */
    mlmeApiSetIbssDefaultFilter(vStaInfo_p, FALSE);
    syncSrv_SetStatusTimer(vStaInfo_p, 0);
    syncSrv_SetKeepAliveTimer(vStaInfo_p, 0);
    /* Set up different variables prior to starting the scan */
	if(vStaInfo_p->staSystemMibs.PhyDSSSTable_p->CurrChan == 0)   
   		vStaInfo_p->ChanIdx      = -1;
	else
		vStaInfo_p->ChanIdx      = 0;
   vStaInfo_p->NumDescripts      = 0;
   vStaInfo_p->NumScanChannels   = 0;
   //ScanTime          = ScanParams.MaxChanTime;
   vStaInfo_p->macMgmtMain_PostScanState = vStaInfo_p->macMgmtMain_State;
   vStaInfo_p->macMgmtMain_State = STATE_SCANNING;

   /* Clear Scan results Table */
   memset(&vStaInfo_p->scanTableResult_p->ScanResults,
          0,
          MAX_SCAN_BUF_SIZE);

   memset(&vStaInfo_p->scanTableResult_p->BssSourceAddr,
          0,
          sizeof(IEEEtypes_MacAddr_t) * IEEEtypes_MAX_BSS_DESCRIPTS);
    
   for (i=0; i<=IEEEtypes_MAX_BSS_DESCRIPTS-1; i++)
   {
      vStaInfo_p->scanTableResult_p->macMgmtMlme_SigQltyResults[i].NumReadings = 0;
      vStaInfo_p->scanTableResult_p->macMgmtMlme_SigQltyResults[i].RSSI        = 0;
      vStaInfo_p->scanTableResult_p->macMgmtMlme_SigQltyResults[i].SigQual1    = 0;
      vStaInfo_p->scanTableResult_p->macMgmtMlme_SigQltyResults[i].SigQual2    = 0;
      vStaInfo_p->scanTableResult_p->ChannelMap[i]                             = 0;
      vStaInfo_p->scanTableResult_p->ScanResultsMap_p[i] = NULL;
   }
   vStaInfo_p->ScanResults_p       = &vStaInfo_p->scanTableResult_p->ScanResults[0];
   vStaInfo_p->scanTableResult_p->ScanResultsMap_p[0] = vStaInfo_p->ScanResults_p;
   vStaInfo_p->ScanResultsLen      = 0;
   /* Set the RF channel frequency to the first channel in the */
   vStaInfo_p->PreScanRfChannel = mlmeApiGetRfChannel(vStaInfo_p);
   /* Prepare HW to do scan */
   mlmeApiPrepHwToScan(vStaInfo_p);
}

/*************************************************************************
* Function: syncSrvSta_ScanCmd
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
UINT8 scanInitStartup = 1;
extern void syncSrvSta_ScanCmd( vmacStaInfo_t *vStaInfo_p,
								macMgmtQ_ScanCmd_t *ScanCmd_p )
{
   UINT32                   i = 0;
   UINT32                   ssIdLen;
   BOOLEAN                  moreChannels = TRUE;
   UINT32                   time_scan_start = 1;
   ssIdLen = strlen((UINT8 *)&ScanCmd_p->Cmd.SsId);


   /*--------------------------------------------------------------------*/
   /* If this is a new scan (not a rescan) save off the scan parameters; */
   /* otherwise, a rescan will use the parameters that were previously   */
   /* given for a scan.                                                  */
   /*--------------------------------------------------------------------*/
   if (ScanCmd_p->ScanSrc == SCAN_CMD)
   {
      memcpy(&vStaInfo_p->ScanParams, &ScanCmd_p->Cmd, sizeof(IEEEtypes_ScanCmd_t));
      //memcpy(&NewScanParams, &ScanCmd_p->Cmd, sizeof(IEEEtypes_ScanCmd_t));
   }

   /*-----------------------------------------*/
   /* Make sure channels to scan are provided */
   /*-----------------------------------------*/
   if (vStaInfo_p->ScanParams.ChanList[0] == 0)
   {
       /* Notify SME of Scan failure */
       syncSrv_SndScanCnfm(vStaInfo_p, 
						   SCAN_RESULT_INVALID_PARAMETERS, 
						   0, 
						   0, 
						   NULL);
      return;
   }

   syncSrv_ScanPrep(vStaInfo_p);


       moreChannels = TRUE;

   /*--------------------------------------------------------*/
   /* Determine how many channels there are to scan from the */
   /* given list.                                            */
   /*--------------------------------------------------------*/
   while (i<(IEEEtypes_MAX_CHANNELS+IEEEtypes_MAX_CHANNELS_A) && moreChannels)
   {
      if (vStaInfo_p->ScanParams.ChanList[i] > 0)
      {
         vStaInfo_p->NumScanChannels++;
      }
      else
      {
         moreChannels = FALSE;
      }
      i++;
   }

   mlmeApiSetRfChannel(vStaInfo_p, vStaInfo_p->ScanParams.ChanList[0], 1, TRUE);

   /*-----------------------------------------------------------*/
   /* Determine if the given SsId indicates to scan for beacons */
   /* with a specific SsId, or if the given SsId is a broadcast */
   /* SsId, in which case any found beacons are recorded.       */
   /*-----------------------------------------------------------*/
   if (ssIdLen == 0)
   {
      vStaInfo_p->ScanForAnyBeacons = TRUE;
   }
   else
   {
      vStaInfo_p->ScanForAnyBeacons = FALSE;
   }

   if (scanInitStartup)
   {
       time_scan_start = 20;
       scanInitStartup = 0;
   }
   /* Get and start a scan timer with duration of the maximum channel time */
   #ifdef MLME_SEPERATE_SCAN_TIMER
   mlmeApiStartScanTimer(vStaInfo_p, time_scan_start, &syncSrvSta_ScanDwellTimeOut);
   #else
   mlmeApiStartTimer(vStaInfo_p, 
                      (UINT8 *)&vStaInfo_p->scaningTimer,
                      &syncSrvSta_ScanDwellTimeOut,
                      time_scan_start);
   #endif /* MLME_SEPERATE_SCAN_TIMER */
   
}

/*************************************************************************
* Function: syncSrv_JoinCmd
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern void syncSrv_JoinCmd( vmacStaInfo_t *vStaInfo_p,
							 IEEEtypes_JoinCmd_t *JoinCmd_p )
{
   vmacEntry_t  *vmacEntry_p;
   UINT32                 length;
   IEEEtypes_StartCmd_t   StartCmd;
   UINT16 rateLen = 0;
   UINT16 i;
   UINT8  parseRate = 0;
   UINT8  bRateCnt = 0;
   UINT8  gRateCnt = 0;

   vmacEntry_p = (vmacEntry_t *)vStaInfo_p->vMacEntry_p;
   vStaInfo_p->rxBcnPeriod = JoinCmd_p->BssDesc.BcnPeriod;

   ClientModeTxMonitor = 1;	
   #ifdef ETH_DEBUG
   eprintf("==> syncSrv_JoinCmd \n");
   #endif /* ETH_DEBUG */

   vStaInfo_p->macMgmtMain_State = STATE_JOINING;     
   /* Record CapInfo */
   vStaInfo_p->macMgmtMlme_ThisStaData.CapInfo = JoinCmd_p->BssDesc.Cap;
   /* Set RF Channel */
   vStaInfo_p->JoinChannel = JoinCmd_p->BssDesc.PhyParamSet.DsParamSet.CurrentChan;
   /* Follow the AP rates setting */
   /* Record a rate */
   memset(vStaInfo_p->bOpRateSet, 0, MAX_B_DATA_RATES);
   memset(vStaInfo_p->gOpRateSet, 0, MAX_G_DATA_RATES);
   rateLen = util_ListLen(&JoinCmd_p->BssDesc.DataRates[0], IEEEtypes_MAX_DATA_RATES_G);
   for(i=0; i < rateLen; i++)
   {
       parseRate = JoinCmd_p->BssDesc.DataRates[i] 
    	   & IEEEtypes_BASIC_RATE_MASK;
       /* Search for 11b rates and record it */
       if(parseRate <= 0xb || parseRate == 0x16)
       {
    	   if(bRateCnt < MAX_B_DATA_RATES)
    	   {

    		   vStaInfo_p->bOpRateSet[bRateCnt] = JoinCmd_p->BssDesc.DataRates[i];
    		   bRateCnt++;
    	   }
       }
       else
       {
    	   /* Assume it's 11g rates */
    	   if(gRateCnt < MAX_G_DATA_RATES)
    	   {
    		   vStaInfo_p->gOpRateSet[gRateCnt] = JoinCmd_p->BssDesc.DataRates[i];
    		   gRateCnt++;
    	   }
       }
   }
   /* Set Control Rates (ACK/CTS) */
   if(vStaInfo_p->isParentSession)
   {
        /********************************************
		*Milind. 10/05/05
		*We will store the Supported Rate and the Extenbded Supported Rate
		*for this mac in the client mode.
		*This will be used later when we want to get the intersection of our rates
		*and the rates of every station that wants to join the network
		*********************************************/
		mlmeApiCreateSupportedRateElement(
			(IEEEtypes_SuppRatesElement_t *)mlmeApiGetClientModeSupportedRates(vmacEntry_p->phyHwMacIndx), 
		    (IEEEtypes_ExtSuppRatesElement_t *)mlmeApiGetClientModeExtSupportedRates(vmacEntry_p->phyHwMacIndx),
			&(vStaInfo_p->bOpRateSet[0]),
			&(vStaInfo_p->gOpRateSet[0]), 
			vmacEntry_p->phyHwMacIndx);

   mlmeApiSetRfChannel(vStaInfo_p, vStaInfo_p->JoinChannel, 1, FALSE);

   /* Set the promiscuous mode such that not every message is */
   if (JoinCmd_p->BssDesc.Cap.Ess)
   {
       mlmeApiSetBSSIDFilter(vStaInfo_p, 1);
   }
   else
   {
       mlmeApiSetBSSIDFilter(vStaInfo_p, 0);
   }
   /* Set BSS Id to HW Mac */
   mlmeApiSetBssidToMac(vStaInfo_p, JoinCmd_p->BssDesc.BssId);
   }
   /* Record BSS Id */
   memcpy(vStaInfo_p->macMgmtMlme_ThisStaData.BssId, JoinCmd_p->BssDesc.BssId, 6);
   /* Record SS Id to HW Mac */
   length = util_ListLen(JoinCmd_p->BssDesc.SsId, IEEEtypes_SSID_SIZE);
   strncpy(vStaInfo_p->macMgmtMlme_ThisStaData.BssSsId, JoinCmd_p->BssDesc.SsId, length);
   util_CopyList(vStaInfo_p->staSystemMibs.mib_StaCfg_p->DesiredSsId, JoinCmd_p->BssDesc.SsId, IEEEtypes_SSID_SIZE);
   vStaInfo_p->macMgmtMlme_ThisStaData.BssSsIdLen = length;
   if(vStaInfo_p->isParentSession)
   {
	   /* Set SS Id to HW Mac */
   mlmeApiSetSsIdToMac(vStaInfo_p,
					   &JoinCmd_p->BssDesc.SsId[0], length);
   /* Set other HW parameters to prepare for join */
   mlmeApiPrepHwJoin(vStaInfo_p, JoinCmd_p);
   }
   vStaInfo_p->macMgmtMlme_ThisStaData.BssType = BSS_INFRASTRUCTURE;
   /* Handler for IBSS (Ad-Hoc) */
   if (JoinCmd_p->BssDesc.Cap.Ibss)
   {
       /* At this point, we are acting as a client (Adhoc) */
       vmacEntry_p->modeOfService = VMAC_MODE_CLNT_ADHOC;
       memset(&StartCmd,
             0,
             sizeof(IEEEtypes_StartCmd_t));

      StartCmd.BcnPeriod    = JoinCmd_p->BssDesc.BcnPeriod;
      StartCmd.BssType      = BSS_INDEPENDENT;
      StartCmd.CapInfo      = JoinCmd_p->BssDesc.Cap;
      StartCmd.DtimPeriod   = JoinCmd_p->BssDesc.DtimPeriod;
      StartCmd.ProbeDelay   = JoinCmd_p->ProbeDelay;
      
      rateLen = util_CopyList(&StartCmd.OpRateSet[0],
                                     &JoinCmd_p->BssDesc.DataRates[0],
                                     IEEEtypes_MAX_DATA_RATES_G);

      if (JoinCmd_p->BssDesc.PhyParamSet.DsParamSet.ElementId ==
          DS_PARAM_SET)
      {
         memcpy(&(StartCmd.PhyParamSet.DsParamSet),
                &(JoinCmd_p->BssDesc.PhyParamSet.DsParamSet),
                sizeof(IEEEtypes_DsParamSet_t));
      }
      else
      {
         memcpy(&(StartCmd.PhyParamSet.FhParamSet),
                &(JoinCmd_p->BssDesc.PhyParamSet.FhParamSet),
                sizeof(IEEEtypes_FhParamSet_t));
      }

      memcpy(&(StartCmd.SsId),
             JoinCmd_p->BssDesc.SsId,
             sizeof(IEEEtypes_SsId_t));

      if(JoinCmd_p->BssDesc.SsParamSet.CfParamSet.ElementId ==
         CF_PARAM_SET)
      {
         memcpy(&(StartCmd.SsParamSet.CfParamSet),
                &(JoinCmd_p->BssDesc.SsParamSet.CfParamSet),
                sizeof(IEEEtypes_CfParamSet_t));
      }
      else
      {
         memcpy(&(StartCmd.SsParamSet.IbssParamSet),
                &(JoinCmd_p->BssDesc.SsParamSet.IbssParamSet),
                sizeof(IEEEtypes_IbssParamSet_t));
      }

      #ifdef STA_ADHOC_SUPPORTED
      syncSrv_UpdateAdhocTxInfo(vStaInfo_p,
                                &StartCmd,
                                &(JoinCmd_p->BssDesc.BssId),
                                IEEE_MSG_BEACON,
                                vStaInfo_p->BcnBuffer_p,
                                vStaInfo_p->BcnTxInfo_p);

      syncSrv_UpdateAdhocTxInfo(vStaInfo_p,
                                &StartCmd,
                                &(JoinCmd_p->BssDesc.BssId),
                                IEEE_MSG_PROBE_RSP,
                                vStaInfo_p->PrbRspBuf_p,
                                vStaInfo_p->PrbRspTxInfo_p);
      #endif /* STA_ADHOC_SUPPORTED */

      syncSrv_SetAdhocWatchTimer(vStaInfo_p);

      vStaInfo_p->macMgmtMlme_ThisStaData.BssType = BSS_INDEPENDENT;
      /* Since this is Adhoc and no need for Authentication/Association */
      /* Set association here */
      vStaInfo_p->AssociatedFlag = 1;
      vStaInfo_p->macMgmtMain_State = STATE_JOINED;

      /* Set HW Mac to IBSS Mode */
      mlmeApiHwStartIBssMode(vStaInfo_p);      
      syncSrv_SndJoinCnfm(vStaInfo_p, JOIN_RESULT_SUCCESS);

      /* Send Host event of IBSS start */
      mlmeApiEventNotification(vStaInfo_p,
                             MlmeStart_Cnfm,
                             (UINT8 *)&vStaInfo_p->macMgmtMlme_ThisStaData.BssId, 
                             0);

      return;
   }
         
   /* At this point, we are acting as a client (Infrastructure) */
   vmacEntry_p->modeOfService = VMAC_MODE_CLNT_INFRA;

   /* Start a timer of duration Join period */
   /* JoinFailTimeout = JoinCmd_p->FailTimeout; */
   /* StartTimer(JOIN_ACTION, &JoiningBss, 0, JoinFailTimeout); */

   /* Start the Auth Process */
   if(smeStateMgr_SndAuthCmd((UINT8 *)vStaInfo_p, 
							 &JoinCmd_p->BssDesc.BssId) == MLME_FAILURE)
   {
	   syncSrv_SndJoinCnfm(vStaInfo_p, JOIN_RESULT_INVALID_PARAMETERS);
   }

}

/*************************************************************************
* Function: syncSrv_StartCmd
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern void syncSrv_StartCmd( vmacStaInfo_t *vStaInfo_p,
							  IEEEtypes_StartCmd_t *StartCmd_p )
{
   vmacEntry_t  *vmacEntry_p;
   UINT8                newSsIdLen;
   IEEEtypes_Len_t      prevNameLen;
   IEEEtypes_Len_t      i;
   UINT16 rateLen = 0;
   UINT8  parseRate = 0;
   UINT8  bRateCnt = 0;
   UINT8  gRateCnt = 0;

   vmacEntry_p = (vmacEntry_t *)vStaInfo_p->vMacEntry_p;
   /* We are operating as a client once this function is invoked */
   vmacEntry_p->modeOfService = VMAC_MODE_CLNT_ADHOC;
   /* Record the rates */
   memset(vStaInfo_p->bOpRateSet, 0, MAX_B_DATA_RATES);
   memset(vStaInfo_p->gOpRateSet, 0, MAX_G_DATA_RATES);
   rateLen = util_ListLen(&StartCmd_p->OpRateSet[0], IEEEtypes_MAX_DATA_RATES_G);
   for(i=0; i < rateLen; i++)
   {
       parseRate = StartCmd_p->OpRateSet[i] 
    	   & IEEEtypes_BASIC_RATE_MASK;
       /* Search for 11b rates and record it */
       if(parseRate <= 0xb || parseRate == 0x16)
       {
    	   if(bRateCnt < MAX_B_DATA_RATES)
    	   {

    		   vStaInfo_p->bOpRateSet[bRateCnt] = StartCmd_p->OpRateSet[i];
    		   bRateCnt++;
    	   }
       }
       else
       {
    	   /* Assume it's 11g rates */
    	   if(gRateCnt < MAX_G_DATA_RATES)
    	   {
    		   vStaInfo_p->gOpRateSet[gRateCnt] = StartCmd_p->OpRateSet[i];
    		   gRateCnt++;
    	   }
       }
   }

   /********************************************
   *Milind. 10/05/05
   *We will store the Supported Rate and the Extenbded Supported Rate
   *for this mac in the client mode.
   *This will be used later when we want to get the intersection of our rates
   *and the rates of every station that wants to join the network
   *********************************************/
   mlmeApiCreateSupportedRateElement(
			(IEEEtypes_SuppRatesElement_t *)mlmeApiGetClientModeSupportedRates(vmacEntry_p->phyHwMacIndx), 
		    (IEEEtypes_ExtSuppRatesElement_t *)mlmeApiGetClientModeExtSupportedRates(vmacEntry_p->phyHwMacIndx),
			&(vStaInfo_p->bOpRateSet[0]),
			&(vStaInfo_p->gOpRateSet[0]),
			vmacEntry_p->phyHwMacIndx);


   /*------------------------------------------------------------------*/
   /* Check for being in a state in which it does not make sense to */
   /* start an IBSS; if there is something wrong, mark the appropriate */
   /* status for the confirmation message sent back to the SME.        */
   /*------------------------------------------------------------------*/
   if (vStaInfo_p->macMgmtMain_State == STATE_SCANNING)
   {
       syncSrv_SndStartCnfm(vStaInfo_p, START_RESULT_INVALID_PARAMETERS);
       return;
   }
   else if ((vStaInfo_p->macMgmtMain_State == STATE_IBSS_STARTED ||
            vStaInfo_p->macMgmtMain_State == STATE_JOINED) &&
            vStaInfo_p->macMgmtMlme_ThisStaData.CapInfo.Ibss)
   {
      /* Check that a network with the same SS ID has not already been started. */
      newSsIdLen = util_ListLen((UINT8 *)&(StartCmd_p->SsId),
                                IEEEtypes_SSID_SIZE);

      if (newSsIdLen == vStaInfo_p->macMgmtMlme_ThisStaData.BssSsIdLen &&
          !memcmp(vStaInfo_p->macMgmtMlme_ThisStaData.BssSsId,
                  StartCmd_p->SsId,
                  vStaInfo_p->macMgmtMlme_ThisStaData.BssSsIdLen))
      {
         syncSrv_SndStartCnfm(vStaInfo_p, START_RESULT_BSS_ALREADY_STARTED_OR_JOINED);
         return;
      }
   }

   /*--------------------------------------------------------------------*/
   /* Determine if a new BSS ID needs to be generated - this is just a   */
   /* check that we have not already created one for the given SS ID.    */
   /* If either the lengths of the old SS ID and new SS ID are not equal */
   /* or if the names of the old SS ID and new SS ID differ, go ahead    */
   /* and generate a new BSS ID. Along the way, we store the new SS ID   */
   /* and its length.                                                    */
   /*--------------------------------------------------------------------*/
   prevNameLen = vStaInfo_p->macMgmtMlme_ThisStaData.BssSsIdLen;
   vStaInfo_p->macMgmtMlme_ThisStaData.BssSsIdLen = util_CopyList(vStaInfo_p->macMgmtMlme_ThisStaData.BssSsId,
                       StartCmd_p->SsId,
                       IEEEtypes_SSID_SIZE);

   mlmeApiPrepHwStartIBss(vStaInfo_p, StartCmd_p);

   #ifdef STA_ADHOC_SUPPORTED
   /*-------------------------------------------------*/
   /* Now update the beacon with the given parameters */
   /*-------------------------------------------------*/
   syncSrv_UpdateAdhocTxInfo(vStaInfo_p,
                             StartCmd_p,
                             &(vStaInfo_p->macMgmtMlme_ThisStaData.BssId),
                             IEEE_MSG_BEACON,
                             vStaInfo_p->BcnBuffer_p,
                             vStaInfo_p->BcnTxInfo_p);

   syncSrv_UpdateAdhocTxInfo(vStaInfo_p,
                             StartCmd_p,
                             &(vStaInfo_p->macMgmtMlme_ThisStaData.BssId),
                             IEEE_MSG_PROBE_RSP,
                             vStaInfo_p->PrbRspBuf_p,
                             vStaInfo_p->PrbRspTxInfo_p);
   #endif /* STA_ADHOC_SUPPORTED */

   mlmeApiUpdateSTAVendorIEs(vStaInfo_p, 1);

   vStaInfo_p->macMgmtMlme_ThisStaData.BssType = BSS_INDEPENDENT;
   vStaInfo_p->macMgmtMlme_ThisStaData.CapInfo = StartCmd_p->CapInfo;

   /*--------------------------------------------------------------------*/
   /* Record the start confirmation as a success and inform the SME Task */
   /*--------------------------------------------------------------------*/
   syncSrv_SndStartCnfm(vStaInfo_p, START_RESULT_SUCCESS);

   /*-------------------------------------------*/
   /* Set the specified channel to listen in on */
   /*-------------------------------------------*/
   if (StartCmd_p->PhyParamSet.DsParamSet.ElementId == DS_PARAM_SET)
   {
      mlmeApiSetRfChannel(vStaInfo_p,
						  StartCmd_p->PhyParamSet.DsParamSet.CurrentChan, 1, TRUE);
   }

   syncSrv_SetAdhocWatchTimer(vStaInfo_p);

   /*--------------------------------------------------------------*/
   /* Now start the sending out of beacons by changing the mode to */
   /* the IBSS mode and starting the timer function; also set the  */
   /* state to indicate this.                                      */
   /*--------------------------------------------------------------*/
   vStaInfo_p->macMgmtMain_State = STATE_IBSS_STARTED;
   mlmeApiHwStartIBssMode(vStaInfo_p);

   /* Set the trunkid as ACTIVE so that data traffic is allowed */  
   mlmeApiSetTrunkIdActive(vStaInfo_p, vmacEntry_p->trunkId, TRUE, STA_TRUNK_TYPE);

   /* Send Host event of IBSS start */
   mlmeApiEventNotification(vStaInfo_p,
                             MlmeStart_Cnfm,
                             (UINT8 *)&vStaInfo_p->macMgmtMlme_ThisStaData.BssId, 
                             0);

}

/*************************************************************************
* Function: syncSrv_ResetCmd
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern void syncSrv_ResetCmd( vmacStaInfo_t *vStaInfo_p, 
							  IEEEtypes_ResetCmd_t *ResetCmd_p )
{
    vmacEntry_t  *vmacEntry_p;

    vmacEntry_p = (vmacEntry_t *)vStaInfo_p->vMacEntry_p;
    vStaInfo_p->AssociatedFlag = 0;
	vStaInfo_p->staSecurityMibs.mib_PrivacyTable_p->RSNLinkStatus = 0;  	
    /* Reset the HW MAC */
    mlmeApiStaMacReset( vmacEntry_p->phyHwMacIndx, vStaInfo_p );
    mlmeApiDisconnect(vStaInfo_p);
    #ifdef ETH_DEBUG
    eprintf("......syncSrv_ResetCmd\n");
    #endif /* ETH_DEBUG */
    /* Set to STA Mode  by default */
    mlmeApiHwSetSTAMode(vStaInfo_p);
    /* Clear the Bssid */
    mlmeApiSetBssidToMac(vStaInfo_p, NULL);
    syncSrv_SetInitValues(vStaInfo_p);
    /* Disarm all Sync Service Timers */
    mlmeApiStopTimer(vStaInfo_p, 
                      (UINT8 *)&vStaInfo_p->scaningTimer);

    #ifdef MLME_SEPERATE_SCAN_TIMER
    if((vStaInfo_p->macMgmtMain_State == STATE_SCANNING) 
    	&& vStaInfo_p->isParentSession)
    {
        mlmeApiStopScanTimer(vStaInfo_p);
    }
    #endif /* MLME_SEPERATE_SCAN_TIMER */
    
    /* Init the Authentication Request state machines */
    authSrv_Reset(vStaInfo_p);
    /* Init the Association state machines */
    assocSrv_Reset(vStaInfo_p); 
    /* Notify SME of Reset */
    syncSrv_SndResetCnfm(vStaInfo_p, RESET_RESULT_SUCCESS);
    /* Handle additional request from quiet cmd */
    if(!ResetCmd_p->mode)
    {
        /* Kick Start to join/start a BSS */
	    if(vStaInfo_p->mib_WB_p->opMode == MLME_CMD_CLIENT_IBSS_START)
	    {
		    smeStateMgr_SndStartCmd((UINT8 *)vStaInfo_p,
                                     vStaInfo_p->bssDescProfile_p);
	    }
        else
        {
            smeStateMgr_SndJoinCmd( (UINT8 *)vStaInfo_p,
                                    vStaInfo_p->bssDescProfile_p); 
        }
    }
}

/*************************************************************************
* Function: syncSrv_RestorePreScanParameters
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
static void syncSrv_RestorePreScanParameters(vmacStaInfo_t *vStaInfo_p)
{  
   vmacEntry_t  *vmacEntry_p;

   vmacEntry_p = (vmacEntry_t *)vStaInfo_p->vMacEntry_p;
   /*  Restore the RF channel */
   mlmeApiSetRfChannel(vStaInfo_p, vStaInfo_p->PreScanRfChannel, 1, FALSE);			
   /* Restore the state */
   switch (vStaInfo_p->macMgmtMain_PostScanState)
   {
      case STATE_JOINED:
      case STATE_AUTHENTICATED_WITH_AP:
      case STATE_ASSOCIATED:
      case STATE_IBSS_STARTED:        
         mlmeApiSetBssidToMac(vStaInfo_p, vStaInfo_p->macMgmtMlme_ThisStaData.BssId);
         /* Restore settings that will allow for re-joining */
         if (vStaInfo_p->macMgmtMlme_ThisStaData.CapInfo.Ibss)
         {
            mlmeApiSetBSSIDFilter(vStaInfo_p, 0);/* bt_todo */
         }
         else
         {
            mlmeApiSetBSSIDFilter(vStaInfo_p, 1);/* bt_todo */
         }
         mlmeApiSetTimerSync(vStaInfo_p, 0, 0);
         if (vStaInfo_p->macMgmtMlme_ThisStaData.CapInfo.Ibss)
         {
             mlmeApiHwStartIBssMode(vStaInfo_p);
         }
         /* Restore Status Timer for Infra mode */
         if(vStaInfo_p->AssociatedFlag && (vStaInfo_p->macMgmtMlme_ThisStaData.BssType == BSS_INFRASTRUCTURE))
         {
             syncSrv_SetStatusTimer(vStaInfo_p, 1);
             if(mib_childMode[vmacEntry_p->phyHwMacIndx])
                 syncSrv_SetKeepAliveTimer(vStaInfo_p, 1);
         }
         break;

      default:
         break;
   }
   vStaInfo_p->macMgmtMain_State = vStaInfo_p->macMgmtMain_PostScanState;
}

/*************************************************************************
* Function: syncSrvSta_SetNextChannel
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern void syncSrvSta_SetNextChannel ( vmacStaInfo_t *vStaInfo_p )
{
    /* Increment the index keeping track of which channel scanning for */
#ifndef SC_PALLADIUM
    vStaInfo_p->ChanIdx++;
#endif

    #ifdef ETH_DEBUG
    eprintf("==> syncSrvSta_SetNextChannel %d \n", vStaInfo_p->ChanIdx); 
    #endif /* ETH_DEBUG */
#ifdef SC_PALLADIUM   
    if (vStaInfo_p->ChanIdx >= 1) //vStaInfo_p->NumScanChannels)
#else
    if (vStaInfo_p->ChanIdx >= vStaInfo_p->NumScanChannels)
#endif
    {
        syncSrv_RestorePreScanParameters(vStaInfo_p);
        /* All channels have been scanned, so send results to SME. */
        syncSrv_SndScanCnfm(vStaInfo_p,
                          SCAN_RESULT_SUCCESS,
                                vStaInfo_p->NumDescripts,
                                vStaInfo_p->ScanResultsLen,
                                (IEEEtypes_BssDesc_t *)&vStaInfo_p->scanTableResult_p->ScanResults);
        return;
    }
#ifdef SC_PALLADIUM
    vStaInfo_p->ChanIdx++;
#endif

    /* Set the rf parameters */
#ifndef SC_PALLADIUM
    mlmeApiSetRfChannel(vStaInfo_p,
					   vStaInfo_p->ScanParams.ChanList[vStaInfo_p->ChanIdx], 1, TRUE);
#endif
    /* For passive scans, we don't need to do anything more */   
    /* For Active Scan the probe req will be sent at MacTxNotify.c */
    if(vStaInfo_p->ScanParams.ScanType == SCAN_ACTIVE)
    {
#ifdef DFS_PASSIVE_SCAN
        if (!(((vStaInfo_p->ScanParams.ChanList[vStaInfo_p->ChanIdx] >= 52) && 
            (vStaInfo_p->ScanParams.ChanList[vStaInfo_p->ChanIdx] <= 64)) ||
            ((vStaInfo_p->ScanParams.ChanList[vStaInfo_p->ChanIdx] >= 100) && 
            (vStaInfo_p->ScanParams.ChanList[vStaInfo_p->ChanIdx] <= 140))))
#endif
#ifdef SC_PALLADIUM
        printk("syncSrvSta_SetNextChannel send probe request. \n");
#endif
        syncSrv_SndProbeReq(vStaInfo_p, TRUE);
    }
    /* Start the scan timer with duration of the maximum channel time */
    #ifdef MLME_SEPERATE_SCAN_TIMER
    mlmeApiStartScanTimer(vStaInfo_p, vStaInfo_p->scanTime_tick, &syncSrvSta_ScanActTimeOut);
    #else
    mlmeApiStartTimer(vStaInfo_p, 
                      (UINT8 *)&vStaInfo_p->scaningTimer,
                      &syncSrvSta_ScanActTimeOut,
                      vStaInfo_p->scanTime_tick);
    #endif /* MLME_SEPERATE_SCAN_TIMER */
}

/*************************************************************************
* Function: syncSrv_SetInitValues
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
void syncSrv_SetInitValues( vmacStaInfo_t *vStaInfo_p )
{
   /* Remove associated AP from UR station database, intially added for
       pseudo ether port support. */
   mlmeApiDelStaDbEntry(vStaInfo_p, (UINT8 *)&vStaInfo_p->macMgmtMlme_ThisStaData.BssId);

   memset(&vStaInfo_p->macMgmtMlme_ThisStaData.BssId,
          0,
          sizeof(IEEEtypes_MacAddr_t));
   memset(&(vStaInfo_p->macMgmtMlme_ThisStaData.UsedRateSet),
          0,
          sizeof(IEEEtypes_SuppRatesElement_t));
   memset(&(vStaInfo_p->macMgmtMlme_ThisStaData.OpRateSet),
          0,
          sizeof(IEEEtypes_SuppRatesElement_t));
   memset(&(vStaInfo_p->macMgmtMlme_ThisStaData.CapInfo),
          0,
          sizeof(IEEEtypes_CapInfo_t));
   vStaInfo_p->macMgmtMlme_ThisStaData.CapInfo.CfPollable    = vStaInfo_p->staSystemMibs.mib_StaCfg_p->CfPollable;
   vStaInfo_p->macMgmtMlme_ThisStaData.UsedRateSet.ElementId = SUPPORTED_RATES;
   vStaInfo_p->macMgmtMlme_ThisStaData.OpRateSet.ElementId   = SUPPORTED_RATES;
   vStaInfo_p->macMgmtMlme_ThisStaData.OpRateSet.Len         =
         util_CopyList(vStaInfo_p->macMgmtMlme_ThisStaData.OpRateSet.Rates,
                       vStaInfo_p->staSystemMibs.mib_StaCfg_p->OpRateSet,
                       IEEEtypes_MAX_DATA_RATES_G);
   vStaInfo_p->macMgmtMain_PostScanState = STATE_IDLE;
}

#ifdef STA_ADHOC_SUPPORTED
/*************************************************************************
* Function: syncSrv_adhocCheckDoRescan
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
static void syncSrv_adhocCheckDoRescan(vmacStaInfo_t *vStaInfo_p,
                                       dot11MgtFrame_t *MgmtMsg_p)
{
    UINT32    seed;

    if(MgmtMsg_p == NULL)
    {
        return;
    }
    if (!memcmp(&(vStaInfo_p->macMgmtMlme_ThisStaData.BssSsId), MgmtMsg_p->Body.Bcn.SsId.SsId, MgmtMsg_p->Body.Bcn.SsId.Len)
                 && vStaInfo_p->AssociatedFlag)
    {
        if(!vStaInfo_p->counterInt)
        {
            vStaInfo_p->counterInt = 1;
            seed = mlmeApiGetTime(vStaInfo_p);
            vStaInfo_p->misMatchBssidCount = (mlmeApiGenRandomUINT32(vStaInfo_p, seed) % 20) + 10;
        }
#ifdef ADHOC_DEBUG
        eprintf("syncSrv_adhocCheckDoRescan:: MisMatch count = %d \n", vStaInfo_p->misMatchBssidCount);
#endif /* ADHOC_DEBUG */
    
        if(vStaInfo_p->misMatchBssidCount--)
        {
            return;
        }
#ifdef ADHOC_DEBUG
        eprintf("syncSrv_adhocCheckDoRescan:: Bingo Need to rescan here\n");
#endif /* ADHOC_DEBUG */
        vStaInfo_p->counterInt = 0;
        vStaInfo_p->AssociatedFlag = 0;
		vStaInfo_p->staSecurityMibs.mib_PrivacyTable_p->RSNLinkStatus = 0;  	
    }
    return;
}

/*************************************************************************
* Function: syncSrv_SetBeaconTime
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
static void syncSrv_SetBeaconTime( vmacStaInfo_t *vStaInfo_p,
								   IEEEtypes_Bcn_t  *Bcn_p )
{
   UINT64   intervals;
   UINT64  *bcnTime_p;
   UINT64   nextBcnTime;
   

   /*-----------------------------------------------------------------*/
   /* Convert the timestamp field into a 64-bit data type so that we  */
   /* can operate on it. The timestamp is taken to be ordered such    */
   /* that the lowest byte contains the least significant bits and    */
   /* and the highest byte contains the most significant bits. Hence, */
   /* the conversion can be made by defining a pointer to a 64-bit    */
   /* type and setting this pointer equal to the timestamp field.     */
   /*-----------------------------------------------------------------*/
   bcnTime_p = ((UINT64 *)(&(Bcn_p->TimeStamp)));

   /*---------------------------------------------------------------*/
   /* Now figure out how many beacon intervals have passed up until */
   /* the timestamp given in the beacon                             */
   /*---------------------------------------------------------------*/
   intervals = *bcnTime_p/(Bcn_p->BcnInterval * IEEEtypes_TIME_UNIT);

   /*-------------------------------------------------------------*/
   /* We now want to compute the time at which a beacon should be */
   /* received in the future; to account for processing time and  */
   /* delays, we will choose the time for a beacon that is        */
   /* INTERVAL_LOOK_AHEAD beacon intervals from the last received */
   /* beacon. So, we compute:                                     */
   /*      Time of next beacon = time of last beacon +            */
   /*                            time of INTERVAL_LOOK_AHEAD      */
   /*-------------------------------------------------------------*/
   nextBcnTime = (intervals * Bcn_p->BcnInterval * IEEEtypes_TIME_UNIT) +
                 (INTERVAL_LOOK_AHEAD * Bcn_p->BcnInterval *
                  IEEEtypes_TIME_UNIT);

   /*----------------------------------------------------------------*/
   /* Now set the appropriate registers with the time found so that  */
   /* the MAC hardware will now know what time to look for a beacon. */
   /* Since this time is spread across two 32-bit registers, get the */
   /* lower half first, then do bit shifting to get the upper half - */
   /* then we can write the values out to the registers and commit   */
   /* them.                                                          */
   /*----------------------------------------------------------------*/
   mlmeApiNotifyNextBcnTime(vStaInfo_p, nextBcnTime);
   
}
#endif /* STA_ADHOC_SUPPORTED */

 /*************************************************************************
* Function: syncSrv_ParseAttribWithinFrame
*
* Description: To start at any Element ID in the frame and start parsing from there.
*
* Input: The pointer from where to start parsing, the attrib to match and the length of the packet.
*
* Output: The pointer where there is a match to the attribute.
*
* Note: We should make sure we advance this pointer to the next element id if 
* we want to search for more such patterns. Else we will go into an endless loop.
*
**************************************************************************/

extern void *syncSrv_ParseAttribWithinFrame(dot11MgtFrame_t *mgtFrame_p, UINT8 *data_p, UINT8 attrib)
{
    UINT32 lenPacket, lenOffset=0;

    lenPacket= mgtFrame_p->Hdr.FrmBodyLen;

    lenOffset += (UINT32)data_p - (UINT32)(&(mgtFrame_p->Body))
                + sizeof(IEEEtypes_MgmtHdr_t); 

	/*lenOffset should be < lenPacket. Otherwise if <=, it will be accessing beyond to unknown 
	* memory location by data_p if no matching attribute is found.
	*/
    while(lenOffset < lenPacket)
    {
        if(*(IEEEtypes_ElementId_t *)data_p == attrib)
        {
            return data_p;
        }

        lenOffset += (2 + *((UINT8 *)(data_p + 1)));
        data_p += (2 + *((UINT8 *)(data_p + 1)));
    }
    return NULL;
}

/*************************************************************************
* Function: syncSrv_AddAttrib
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern UINT32 syncSrv_AddAttrib( dot11MgtFrame_t *mgtMsg_p, 
                                   UINT8 attribType,
                                   UINT8 *attribData,
                                   UINT8 attribLen)
{
    UINT32 curMsgLen;
    UINT8 *curData_p;
    UINT8 lenAdded = 0;

    curMsgLen = mgtMsg_p->Hdr.FrmBodyLen;
    curData_p = (UINT8 *)&mgtMsg_p->Body + curMsgLen;
    *curData_p = attribType;
    curData_p ++;
    *curData_p = attribLen;
    curData_p++;
    memcpy(curData_p, attribData, attribLen);
    lenAdded = attribLen + 2;
    mgtMsg_p->Hdr.FrmBodyLen += lenAdded;
    return lenAdded;
}

/*************************************************************************
* Function: AccumulateScanResults
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern void AccumulateScanResults(vmacStaInfo_t *vStaInfo_p,
                                  void *BssData_p, 
                                  UINT8 *rfHdr_p )
{
    UINT32 i =0;
    UINT32                  length;
    BOOLEAN                 notFound      = TRUE;
    IEEEtypes_MacAddr_t     emptyAddr;
    UINT8     *attrib_p = NULL;
    UINT16  IElength;
    BOOLEAN addChannelIE = FALSE;
	UINT8 local_RSSI;
    dot11MgtFrame_t *MgmtMsg_p;
    UINT32 weakest;
    UINT32 stepShift;
    UINT32 bufShift;
    UINT32 length_plus;
    UINT32 scanDesctLen;
    UINT32 j;

    if((vStaInfo_p->ScanResults_p == NULL) 
       || (vStaInfo_p->macMgmtMain_State != STATE_SCANNING))
    {
        return;
    }
    memset(&emptyAddr, 0, sizeof(IEEEtypes_MacAddr_t));
    /* Find any empty entry or update an existing one */
    while (notFound  &&  i < IEEEtypes_MAX_BSS_DESCRIPTS)
    {
        if (memcmp(((scanDescptHdr_t *)vStaInfo_p->scanTableResult_p->ScanResultsMap_p[i])->bssId,
                   emptyAddr,
                   sizeof(IEEEtypes_MacAddr_t)))
        {
            /* Not Empty so let's see if Bssid Matched */
            if (!memcmp((((scanDescptHdr_t *)vStaInfo_p->scanTableResult_p->ScanResultsMap_p[i])->bssId),
                        ((dot11MgtFrame_t *)BssData_p)->Hdr.BssId,
                        sizeof(IEEEtypes_MacAddr_t)))
            {
                /* Bssid Matched so just update the signal quality info */
                notFound = FALSE;
                vStaInfo_p->scanTableResult_p->macMgmtMlme_SigQltyResults[i].NumReadings++;

                if( ((WLAN_RX_INFO *)rfHdr_p)->RSSI < vStaInfo_p->scanTableResult_p->macMgmtMlme_SigQltyResults[i].RSSI )
                {
					vStaInfo_p->scanTableResult_p->macMgmtMlme_SigQltyResults[i].RSSI = 
						mlmeApiHalRssiDbmGet(vStaInfo_p, ((WLAN_RX_INFO *)rfHdr_p)->RSSI, ((WLAN_RX_INFO *)rfHdr_p)->Sq1);
                    
                    ((scanDescptHdr_t *)vStaInfo_p->scanTableResult_p->ScanResultsMap_p[i])->rssi = 
                        vStaInfo_p->scanTableResult_p->macMgmtMlme_SigQltyResults[i].RSSI;
                }
                vStaInfo_p->scanTableResult_p->macMgmtMlme_SigQltyResults[i].SigQual1 = ((WLAN_RX_INFO *)rfHdr_p)->Sq1;
            }
            else
            {
                /* Bssid doesn't matched so let's check at the next location */
                i++;

                /* 
                 * we need to look at all the results we've accumulated 
                 * before we start removing the weakest results
                 */
                if((i < IEEEtypes_MAX_BSS_DESCRIPTS) && (i < vStaInfo_p->NumDescripts))
                {
                    continue;
                }
                local_RSSI = mlmeApiHalRssiDbmGet(vStaInfo_p, ((WLAN_RX_INFO *)rfHdr_p)->RSSI, ((WLAN_RX_INFO *)rfHdr_p)->Sq1);
                scanDesctLen = ((dot11MgtFrame_t *)BssData_p)->Hdr.FrmBodyLen - (sizeof(IEEEtypes_MgmtHdr_t));
                if ((attrib_p = syncSrvSta_ParseAttrib(BssData_p, DS_PARAM_SET)) == NULL)
                {
                    scanDesctLen += sizeof(IEEEtypes_DsParamSet_t);
                }
                scanDesctLen += sizeof(IEEEtypes_MacAddr_t)+sizeof(UINT8)+sizeof(IElength);
                length_plus = scanDesctLen+vStaInfo_p->ScanResultsLen;
                if ((i >= IEEEtypes_MAX_BSS_DESCRIPTS) || (length_plus > MAX_SCAN_BUF_SIZE))
                {
                    /* Check to see if we can free up enough room to include the new scan descriptor */
                    length = 0;
                    for (j = 0; j < vStaInfo_p->NumDescripts; j++)
                    {
                        if(vStaInfo_p->scanTableResult_p->macMgmtMlme_SigQltyResults[j].RSSI > local_RSSI)
                        {
                            length += ((scanDescptHdr_t *)vStaInfo_p->scanTableResult_p->ScanResultsMap_p[j])->length;
                        }
                    }
                    if(length < scanDesctLen)
                    {
                        return;
                    }
                }

                /********************************************************** 
                 * We'll throw out beacons if we will exceed the max number
                 * of descriptors, or if we will exceed the max buffer space
                 * allowed for scan results.  We'll also check to make sure
                 * we have a result to throw out in case the beacon is
                 * somehow too large
                 **********************************************************/
                while ((i >= IEEEtypes_MAX_BSS_DESCRIPTS ||
                        (length_plus > MAX_SCAN_BUF_SIZE 
                         && vStaInfo_p->NumDescripts > 0)) &&
                        notFound)
                {
                    weakest = 0;
                    for (j = 1; j < vStaInfo_p->NumDescripts; j++)
                    {
                        if(vStaInfo_p->scanTableResult_p->macMgmtMlme_SigQltyResults[weakest].RSSI < 
                           vStaInfo_p->scanTableResult_p->macMgmtMlme_SigQltyResults[j].RSSI)
                        {
                            weakest = j;
                        }
                    }
                    if (local_RSSI < vStaInfo_p->scanTableResult_p->macMgmtMlme_SigQltyResults[weakest].RSSI)
                    {
                        //discard the weakest and add the new one
                        stepShift = vStaInfo_p->NumDescripts-(weakest+1);
                        if(weakest == vStaInfo_p->NumDescripts-1)
                        {
                            bufShift = vStaInfo_p->ScanResults_p -
                                vStaInfo_p->scanTableResult_p->ScanResultsMap_p[weakest];
                        }
                        else
                        {
                            bufShift = vStaInfo_p->scanTableResult_p->ScanResultsMap_p[weakest+1] -
                                vStaInfo_p->scanTableResult_p->ScanResultsMap_p[weakest];
                        }
                        if(stepShift)
                        {
                            memcpy (vStaInfo_p->scanTableResult_p->ScanResultsMap_p[weakest],
                                    vStaInfo_p->scanTableResult_p->ScanResultsMap_p[weakest+1], 
                                    vStaInfo_p->ScanResults_p - 
                                    vStaInfo_p->scanTableResult_p->ScanResultsMap_p[weakest+1]);
                            memcpy (
                                (void *)&vStaInfo_p->scanTableResult_p->macMgmtMlme_SigQltyResults[weakest],
                                (void *)&vStaInfo_p->scanTableResult_p->macMgmtMlme_SigQltyResults[weakest+1],
                                sizeof (macMgmtMlme_SigQltySet_t) * stepShift);

                            for (j = weakest+1; j < vStaInfo_p->NumDescripts-1; j++)
                            {
                                vStaInfo_p->scanTableResult_p->ScanResultsMap_p[j] = vStaInfo_p->scanTableResult_p->ScanResultsMap_p[j+1] -
                                    bufShift;
                            }
                        }
                        memset (vStaInfo_p->ScanResults_p - bufShift, 
                                0, 
                                bufShift);
                        memset (&vStaInfo_p->scanTableResult_p->macMgmtMlme_SigQltyResults[vStaInfo_p->NumDescripts-1], 
                                0, 
                                sizeof (macMgmtMlme_SigQltySet_t));
                        vStaInfo_p->ScanResultsLen -= bufShift;
                        vStaInfo_p->ScanResults_p -= bufShift;
                        vStaInfo_p->scanTableResult_p->ScanResultsMap_p[vStaInfo_p->NumDescripts-1] = vStaInfo_p->ScanResults_p;
                        i--;
                        vStaInfo_p->NumDescripts--;
                        length_plus = scanDesctLen+vStaInfo_p->ScanResultsLen;
                    }
                    else
                    {
                        //discard the new one
                        notFound = FALSE;
                    }
                }
            }
            continue;
        }

        /* This is first time we received beacon from this Bssid */
        notFound      = FALSE;
        memcpy(&vStaInfo_p->scanTableResult_p->BssSourceAddr[i],
             &(((dot11MgtFrame_t *)BssData_p)->Hdr.SrcAddr),
             sizeof(IEEEtypes_MacAddr_t));
        vStaInfo_p->NumDescripts++;
        length = ((dot11MgtFrame_t *)BssData_p)->Hdr.FrmBodyLen - (sizeof(IEEEtypes_MgmtHdr_t));   

        /* RSSI of the BSS is packed inside the BSS Description */ 
        IElength = length+sizeof(IEEEtypes_MacAddr_t)+sizeof(UINT8);

        /* Find out if we have channel IE */
        if ((attrib_p = syncSrvSta_ParseAttrib(BssData_p, DS_PARAM_SET)) == NULL)
        {
            addChannelIE = TRUE;
            IElength += sizeof(IEEEtypes_DsParamSet_t);
        }
        else if ( mlmeApiGetRfChannel(vStaInfo_p) != ((IEEEtypes_DsParamSet_t *)attrib_p)->CurrentChan)
        {
            vStaInfo_p->NumDescripts--;
            return;
        }

        /* Make sure there is enought memory */
        if ((vStaInfo_p->ScanResultsLen + IElength + sizeof(IElength)) > MAX_SCAN_BUF_SIZE)
        {
            vStaInfo_p->NumDescripts--;
            return;
        }
        /* Fill Scan Result */
        vStaInfo_p->scanTableResult_p->ScanResultsMap_p[i] = vStaInfo_p->ScanResults_p;
        memcpy(vStaInfo_p->ScanResults_p, (const void *)&IElength, sizeof(IElength));
        vStaInfo_p->ScanResultsLen += sizeof(IElength);
        vStaInfo_p->ScanResults_p = (void*)((UINT8 *)vStaInfo_p->ScanResults_p + sizeof(IElength));
        /*Pack in the BssId*/
        memcpy((void *)vStaInfo_p->ScanResults_p,
               (void *)((dot11MgtFrame_t *)BssData_p)->Hdr.BssId,
               sizeof(IEEEtypes_MacAddr_t));
        vStaInfo_p->ScanResultsLen += sizeof(IEEEtypes_MacAddr_t);
        vStaInfo_p->ScanResults_p = (void*)((UINT8 *)vStaInfo_p->ScanResults_p + sizeof(IEEEtypes_MacAddr_t));
		/* Update the RSSI*/
		local_RSSI = mlmeApiHalRssiDbmGet(vStaInfo_p, ((WLAN_RX_INFO *)rfHdr_p)->RSSI, 
									((WLAN_RX_INFO *)rfHdr_p)->Sq1);
		memcpy((void *)(vStaInfo_p->ScanResults_p), &local_RSSI, sizeof(UINT8)); 
        vStaInfo_p->ScanResultsLen+=sizeof(UINT8);
        vStaInfo_p->ScanResults_p = (void*)((UINT8 *)vStaInfo_p->ScanResults_p + sizeof(UINT8));
        /* Pack in the rest to Scan Result */
        MgmtMsg_p = (dot11MgtFrame_t *)BssData_p;
        memcpy(vStaInfo_p->ScanResults_p, (const void *)&MgmtMsg_p->Body.Bcn.TimeStamp, length);
        memcpy(vStaInfo_p->ScanResults_p, (const void *)&((dot11MgtFrame_t *)BssData_p)->Body.Bcn.TimeStamp, length);
        vStaInfo_p->ScanResultsLen +=  length;
        vStaInfo_p->ScanResults_p = (void*)((UINT8 *)vStaInfo_p->ScanResults_p + length);
        /* Find out if we have channel IE */
        if (addChannelIE)
        {
            IEEEtypes_DsParamSet_t channelIE;
            channelIE.ElementId = DS_PARAM_SET;
            channelIE.Len = sizeof(UINT8);
            channelIE.CurrentChan = mlmeApiGetRfChannel(vStaInfo_p);
            memcpy(vStaInfo_p->ScanResults_p, (const void *)&channelIE, sizeof(IEEEtypes_DsParamSet_t));
            vStaInfo_p->ScanResultsLen +=  sizeof(IEEEtypes_DsParamSet_t);
            vStaInfo_p->ScanResults_p = (void*)((UINT8 *)vStaInfo_p->ScanResults_p + sizeof(IEEEtypes_DsParamSet_t));
        }
        /* Update the scan results map and increment the total size of the scan results */
        if( i < (IEEEtypes_MAX_BSS_DESCRIPTS-1) )
            vStaInfo_p->scanTableResult_p->ScanResultsMap_p[i+1] = vStaInfo_p->ScanResults_p;

        vStaInfo_p->scanTableResult_p->macMgmtMlme_SigQltyResults[i].NumReadings++;
		vStaInfo_p->scanTableResult_p->macMgmtMlme_SigQltyResults[i].RSSI = 
			mlmeApiHalRssiDbmGet(vStaInfo_p, ((WLAN_RX_INFO *)rfHdr_p)->RSSI, ((WLAN_RX_INFO *)rfHdr_p)->Sq1);
        vStaInfo_p->scanTableResult_p->macMgmtMlme_SigQltyResults[i].SigQual1 = ((WLAN_RX_INFO *)rfHdr_p)->Sq1;
    }                                                      /* while */
}                                                          /* AccumulateScanResults */

/*************************************************************************
* Function: syncSrv_BncRecvAssociatedHandler
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern void syncSrv_BncRecvAssociatedHandler(vmacStaInfo_t *vStaInfo_p,
                                             dot11MgtFrame_t *MgmtMsg_p,
                                                UINT8 *rfHdr_p)
{
    void *attrib_p;
#ifdef STA_QOS
#ifdef QOS_WSM_FEATURE
    WME_param_elem_t *pWME_param_elem;
    WSM_QoS_Cap_Elem_t  *pWME_info_elem;
#endif //QOS_WSM_FEATURE
#endif
	UINT16 macId;
    UINT32 protection4g = 0;
	UINT8 macIndex;		
//  extStaDb_StaInfo_t *wbStaInfo_p;
//  UINT32 local_urAid;
    vmacEntry_t  *vmacEntry_p;
    vmacApInfo_t  *vmacSta_p;
    MIB_802DOT11 *mib;
//  IEEEtypes_SuppRatesElement_t *PeerSupportedRates_p;
//  IEEEtypes_ExtSuppRatesElement_t *PeerExtSupportedRates_p;
//  UINT32                 capInfo;
	struct net_device *staDev      = NULL;
    struct wlprivate  *stapriv     = NULL;   
    UINT8 noise = 0;

    vmacEntry_p = (vmacEntry_t *)vStaInfo_p->vMacEntry_p;
    staDev = (struct net_device *)vmacEntry_p->privInfo_p;
    stapriv = NETDEV_PRIV_P(struct wlprivate, staDev);
    vmacSta_p = (vmacApInfo_t *)stapriv->vmacSta_p;
	mib = (MIB_802DOT11 *)vmacSta_p->Mib802dot11; 
    
	macId = mlmeApiGetMacId(vStaInfo_p);
	macIndex = mlmeApiGetMacIndex(vStaInfo_p);		

#ifdef SC_PALLADIUM
    return;
#endif

    if (memcmp(&(MgmtMsg_p->Hdr.BssId),
                    &(vStaInfo_p->macMgmtMlme_ThisStaData.BssId),
                    sizeof(IEEEtypes_MacAddr_t)))
    {
       return;
    }

	if(vStaInfo_p->AssociatedFlag)
	{
		ClientModeDataCount[vmacSta_p->VMacEntry.phyHwMacIndx]++;
	}
	
	
    /* Validate RF Channel  */
    if (stapriv->vmacSta_p->Mib802dot11->PhyDSSSTable->Chanflag.FreqBand==FREQ_BAND_2DOT4GHZ)
    {
        if((attrib_p = syncSrvSta_ParseAttrib(MgmtMsg_p, DS_PARAM_SET)) != NULL)
        {  
            if (((IEEEtypes_DsParamSet_t *)attrib_p)->CurrentChan 
                != vStaInfo_p->bssDescProfile_p->PhyParamSet.DsParamSet.CurrentChan)
            {
                return;
            }
        }
    }
    /* Get current level, noise, and quality. */
    vStaInfo_p->linkInfo.wStats.qual.level = ((WLAN_RX_INFO *)rfHdr_p)->RSSI;
    
    if (NoiseSampleBeaconCount++ == NOISE_SAMPLE_BEACON_COUNT)
    {
        NoiseSampleBeaconCount = 0;
        wlFwGetNoiseLevel (staDev, 0, &noise);
        vStaInfo_p->linkInfo.wStats.qual.noise = (vStaInfo_p->linkInfo.wStats.qual.noise + noise)/2; 
    }

    vStaInfo_p->linkInfo.wStats.qual.qual = vStaInfo_p->linkInfo.wStats.qual.noise - vStaInfo_p->linkInfo.wStats.qual.level;

    /* Detecting b/g network */
    /* Calculate average level, noise and quality. */
    vStaInfo_p->linkInfo.avg_qual.level = (vStaInfo_p->linkInfo.avg_qual.level + vStaInfo_p->linkInfo.wStats.qual.level)/2;
    vStaInfo_p->linkInfo.avg_qual.noise = (vStaInfo_p->linkInfo.avg_qual.noise + vStaInfo_p->linkInfo.wStats.qual.noise)/2;
    vStaInfo_p->linkInfo.avg_qual.qual  = (vStaInfo_p->linkInfo.avg_qual.qual  + vStaInfo_p->linkInfo.wStats.qual.qual)/2;
    /* Keep track of maximum level, noise and quality. */
    /* Measurements in -dBm. */
    if ((vStaInfo_p->linkInfo.wStats.qual.level < vStaInfo_p->linkInfo.max_qual.level) ||
        (vStaInfo_p->linkInfo.max_qual.level == 0 ))
    {
        vStaInfo_p->linkInfo.max_qual.level = vStaInfo_p->linkInfo.wStats.qual.level;
    }
    if ((vStaInfo_p->linkInfo.wStats.qual.noise < vStaInfo_p->linkInfo.max_qual.noise) ||
        (vStaInfo_p->linkInfo.max_qual.noise == 0))
    {
        vStaInfo_p->linkInfo.max_qual.noise = vStaInfo_p->linkInfo.wStats.qual.noise;
    }
    if (vStaInfo_p->linkInfo.wStats.qual.qual > vStaInfo_p->linkInfo.max_qual.qual)
    {
        vStaInfo_p->linkInfo.max_qual.qual = vStaInfo_p->linkInfo.wStats.qual.qual;
    }
    vStaInfo_p->macMgmtMlme_ThisStaData.AP_RateLen = 0;
    vStaInfo_p->macMgmtMlme_ThisStaData.AP_gRateLen = 0;
    if((attrib_p = syncSrvSta_ParseAttrib(MgmtMsg_p, SUPPORTED_RATES)) != NULL)
    {  
        vStaInfo_p->macMgmtMlme_ThisStaData.AP_RateLen = ((IEEEtypes_SuppRatesElement_t *)attrib_p)->Len;   
    }
    if((attrib_p = syncSrvSta_ParseAttrib(MgmtMsg_p, EXT_SUPPORTED_RATES)) != NULL)
    {  
        vStaInfo_p->macMgmtMlme_ThisStaData.AP_gRateLen = ((IEEEtypes_SuppRatesElement_t *)attrib_p)->Len;   
    }
#ifdef STA_QOS
#ifdef QOS_WSM_FEATURE
    attrib_p = &MgmtMsg_p->Body.Bcn.CapInfo;
    attrib_p += sizeof(IEEEtypes_CapInfo_t);
    while((attrib_p = syncSrv_ParseAttribWithinFrame( MgmtMsg_p, attrib_p, PROPRIETARY_IE))!=NULL)
    {  
        //Check if it is a WME/WSM frame
        pWME_info_elem = attrib_p;
        if(!memcmp(pWME_info_elem->OUI.OUI, WiFiOUI, 3))
        {
            //Check if it is a WME element
            if(pWME_info_elem->OUI.Type==2)
            {
                //check if it is a Info Element
                if(pWME_info_elem->OUI.Subtype==0)
                {
                    //check if the EDCA parameters have been updated.
                    //If they have, send a probe request asking for an updated parameter element
                    if(pWME_info_elem->QoS_info.EDCA_param_set_update_cnt!=Qos_Stn_Data[0].QoS_Info.EDCA_param_set_update_cnt)
                    {
                        syncSrv_SndProbeReq(vStaInfo_p, FALSE);     /* put proper value later */
                    }
                }
                if(pWME_info_elem->OUI.Subtype==1)
                {
                    //It is a WME Parameter Element
                    //Update teh EDCA parameters.
                    pWME_param_elem = pWME_info_elem;
                    QoS_UpdateStnEDCAParameters(pWME_param_elem);
                    memcpy(&(Qos_Stn_Data[0].QoS_Info),&(pWME_param_elem->QoS_info),sizeof(QoS_Info_t));
                }
            }

        }
        attrib_p += (2 + *((UINT8 *)(attrib_p + 1)));
    }
#endif //QOS_WSM_FEATURE
#endif
    if(vStaInfo_p->macMgmtMlme_ThisStaData.BssType == BSS_INDEPENDENT)
    {
        vStaInfo_p->Adhoc_Active = 1;
        /*eprintf("AdHoc Active\n");*/

        /* In Adhod, possible to receive beacons from multiple source */
        /* So only act right away when there is a b-client, */
        /*else update when adhoc timer expire */
        if(!vStaInfo_p->macMgmtMlme_ThisStaData.AP_gRateLen 
           && vStaInfo_p->macMgmtMlme_ThisStaData.AP_RateLen <= MAX_B_DATA_RATES)
        {
            vStaInfo_p->Station_p->NetworkCtrl.isB_Network = 1;
            vStaInfo_p->Station_p->NetworkCtrl.RateOptions = STA_BONLY_MODE;
        }

#ifdef BCN_BALANCING
        vStaInfo_p->bcnCount++;
        if(!(vStaInfo_p->bcnCount % 3))
        {
            msi_wl_BeaconTxCount(macId);
            msi_wl_setBcnPeriod(macId,0x38, 0);
        }
#endif /* BCN_BALANCING */
    }
    else
    {
        /* In Infrastructure, only received from associated AP so update right away */
        vStaInfo_p->rxBcnCnt++;
        if(vStaInfo_p->macMgmtMlme_ThisStaData.AP_gRateLen 
           || vStaInfo_p->macMgmtMlme_ThisStaData.AP_RateLen > MAX_B_DATA_RATES)
        {
            vStaInfo_p->Station_p->NetworkCtrl.isB_Network = 0;
            vStaInfo_p->Station_p->NetworkCtrl.RateOptions = STA_MIXED_MODE; 
        }
        else
        {
            vStaInfo_p->Station_p->NetworkCtrl.isB_Network = 1;
            vStaInfo_p->Station_p->NetworkCtrl.RateOptions = STA_BONLY_MODE;
        }

        if((attrib_p = syncSrvSta_ParseAttrib(MgmtMsg_p, ERP_INFO)) != NULL)
        {
            vStaInfo_p->Station_p->ChipCtrl.Protection4g = ((IEEEtypes_ERPInfoElement_t *)attrib_p)->ERPInfo.UseProtection;
            if (!(*mib->mib_forceProtectiondisable))
            {                                   
                if (vStaInfo_p->Station_p->ChipCtrl.Protection4g)
                    protection4g = 0x02;
                else
                    protection4g = 0;
                if (stapriv->master) wlFwSetGProt(stapriv->master, protection4g);
            }
        }

        if((attrib_p = syncSrvSta_ParseAttrib(MgmtMsg_p, HT)) != NULL)
        {
            if ((*mib->mib_HtGreenField) && ((IEEEtypes_HT_Element_t *)attrib_p)->HTCapabilitiesInfo.GreenField)
            {                                   
        		if((attrib_p = syncSrvSta_ParseAttrib(MgmtMsg_p, ADD_HT)) != NULL)
            	{
                	if(	(vStaInfo_p->Station_p->ChipCtrl.GreenField == ((IEEEtypes_Add_HT_Element_t *)attrib_p)->OpMode.NonGFStaPresent) 
                	||(!vStaInfo_p->Station_p->ChipCtrl.GreenFieldSet) )
            		{
            			vStaInfo_p->Station_p->ChipCtrl.GreenField = !(((IEEEtypes_Add_HT_Element_t *)attrib_p)->OpMode.NonGFStaPresent);
                		if (stapriv->master) 
                		{
                			vStaInfo_p->Station_p->ChipCtrl.GreenFieldSet = 1; 
                			wlFwSetHTGF(stapriv->master, vStaInfo_p->Station_p->ChipCtrl.GreenField);
						}
					}
            	}
            }
            else if(vStaInfo_p->Station_p->ChipCtrl.GreenField)
			{
            	if (stapriv->master)
				{
					vStaInfo_p->Station_p->ChipCtrl.GreenField = 0;
                	wlFwSetHTGF(stapriv->master, vStaInfo_p->Station_p->ChipCtrl.GreenField);
				}
			}
        }


#ifdef IEEE80211_DH
		if( vStaInfo_p->staSystemMibs.mib_StaCfg_p->sta11hMode )
		{
        /* Retrieve the CSA attribute.
        * Perform 11h channel switch at the end of CSA countdown
        */
        	if((attrib_p = syncSrvSta_ParseAttrib(MgmtMsg_p, CSA)) != NULL)
            {
                UINT8 channel =
                ((IEEEtypes_ChannelSwitchAnnouncementElement_t *)attrib_p)->Channel ;
                UINT8 count =
                ((IEEEtypes_ChannelSwitchAnnouncementElement_t *)attrib_p)->Count ;
                UINT8 mode =
                ((IEEEtypes_ChannelSwitchAnnouncementElement_t *)attrib_p)->Mode ;
        		UINT16 beaconInterval = MgmtMsg_p->Body.Bcn.BcnInterval ;
				UINT16 tickCount = beaconInterval ? (beaconInterval/100 * count) : count ;
				WLDBG_INFO(DBG_LEVEL_7, "TICK COUNT IS :%d\n", tickCount );
				if( vStaInfo_p->station11hTimerFired == 0 )
				{
					vStaInfo_p->station11hTimerFired = 1 ;
					vStaInfo_p->station11hChannel = channel ;
					TimerFireIn( &vStaInfo_p->station11hTimer, 1,
							station11hTimerCB, (unsigned char *)vStaInfo_p, tickCount);
				}
				else
				{
					TimerRearm(&vStaInfo_p->station11hTimer, tickCount );
				}
                if( count == 1 )
                {
					vStaInfo_p->station11hTimerFired = 0 ;
                }
				/* If mode == 1, stop transmission, restart transmission after
				* channel switch.
				*/
				if( mode == 1 )
				{
					macMgmtMlme_StopDataTraffic(staDev);
				}
            }
		}
#endif //IEEE80211_DH
    }
}

/*************************************************************************
* Function: syncSrv_LinkLostHandler
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern SINT32 syncSrv_LinkLostHandler( vmacStaInfo_t *vStaInfo_p )
{
	vmacEntry_t  *vmacEntry_p;

    vmacEntry_p = (vmacEntry_t *)vStaInfo_p->vMacEntry_p;

    if(vStaInfo_p->macMgmtMain_State == STATE_SCANNING)
    {
        mlmeApiStopTimer(vStaInfo_p, 
                            (UINT8 *)&vStaInfo_p->scaningTimer);
        #ifdef MLME_SEPERATE_SCAN_TIMER
        if(vStaInfo_p->isParentSession)
        {
            mlmeApiStopScanTimer(vStaInfo_p);
        }
        #endif /* MLME_SEPERATE_SCAN_TIMER */
        
    }
    if(vStaInfo_p->AssociatedFlag)
    {
        authSrv_SndDeAuthMsg( vStaInfo_p, 
                                    &vStaInfo_p->macMgmtMlme_ThisStaData.BssId,
                                    &vStaInfo_p->macMgmtMlme_ThisStaData.BssId,
                                    IEEEtypes_REASON_DEAUTH_LEAVING);
        vStaInfo_p->AssociatedFlag = 0;
		vStaInfo_p->staSecurityMibs.mib_PrivacyTable_p->RSNLinkStatus = 0;  	
        if(vStaInfo_p->isParentSession)
        {
            /* Set the trunkid as INACTIVE so that data traffic is not allowed */  
            mlmeApiSetTrunkIdActive(vStaInfo_p, vmacEntry_p->trunkId, FALSE, STA_TRUNK_TYPE);
        }
    }
    vStaInfo_p->macMgmtMain_State = STATE_IDLE;
    /* Remove associated AP from UR station database, intially added for
       pseudo ether port support. */
    mlmeApiFreePeerStationStaInfoAndAid(&vStaInfo_p->macMgmtMlme_ThisStaData.BssId, vmacEntry_p);
    return MLME_SUCCESS;
}

/*************************************************************************
* Function: syncSrv_ScanFilter
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern void syncSrv_ScanFilter( vmacStaInfo_t *vStaInfo_p,
                                  dot11MgtFrame_t *MgmtMsg_p, 
                                  UINT8 *rfHdr_p )
{
    UINT8 ssidLen;
    UINT8 tmpBufSSID[IEEEtypes_SSID_SIZE];
    UINT16 parseLen = 0;
	IEEEtypes_RSN_IE_t      *wpaIE_p;
    IEEEtypes_RSN_IE_WPA2_t *wpa2IE_p;
    
    if(vStaInfo_p->scanFilterMap)
    {
        /* BSSID Filter */
        if(vStaInfo_p->scanFilterMap & MLME_BSSID_FILTER)
        {
            if(memcmp(&MgmtMsg_p->Hdr.BssId[0], 
                      &vStaInfo_p->ScanParams.BssId[0], 
                      sizeof(IEEEtypes_MacAddr_t)))
            {
                return;
            }
        }
        /* SSID Filter */
        if(vStaInfo_p->scanFilterMap & MLME_SSID_FILTER)
        {
            ssidLen = util_CopyList(tmpBufSSID, vStaInfo_p->ScanParams.SsId, IEEEtypes_SSID_SIZE);
            if((ssidLen != MgmtMsg_p->Body.ProbeRsp.SsId.Len)
               || memcmp(MgmtMsg_p->Body.ProbeRsp.SsId.SsId, 
                         vStaInfo_p->ScanParams.SsId, 
                         ssidLen))
            {
                return;
            }
        }
        /* BSS Type Filter */
        if(vStaInfo_p->scanFilterMap & MLME_BSS_TYPE_FILTER)
        {
            if((vStaInfo_p->ScanParams.BssType == BSS_INFRASTRUCTURE) &&
               !MgmtMsg_p->Body.ProbeRsp.CapInfo.Ess)
            {
                return;
            }
            else if((vStaInfo_p->ScanParams.BssType == BSS_INDEPENDENT) &&
               !MgmtMsg_p->Body.ProbeRsp.CapInfo.Ibss)
            {
                return;
            }
        }

		/* Check for encryption mode compatability. */
        parseLen = MgmtMsg_p->Hdr.FrmBodyLen - sizeof(IEEEtypes_MgmtHdr_t) - sizeof(IEEEtypes_TimeStamp_t) 
        - sizeof(IEEEtypes_BcnInterval_t) - sizeof(IEEEtypes_CapInfo_t) + 6 + 2;
		wpa2IE_p = (IEEEtypes_RSN_IE_WPA2_t *)smeParseIeType(RSN_IEWPA2, (UINT8 *)&MgmtMsg_p->Body.ProbeRsp.SsId, parseLen);
		wpaIE_p = linkMgtParseWpaIe((UINT8 *)&MgmtMsg_p->Body.ProbeRsp.SsId, parseLen);
		if (wpa2IE_p || wpaIE_p)
        {
			if (!vStaInfo_p->staSecurityMibs.mib_PrivacyTable_p->RSNEnabled)
            {
                if (vStaInfo_p->staSystemMibs.mib_StaCfg_p->wpawpa2Mode != 16)
                /* AP requires WPA/WPA2 - station WPA/WPA2 not enabled. */
				return;
            }
			else if (vStaInfo_p->staSecurityMibs.mib_RSNConfigWPA2_p->WPA2Enabled)
			{
				if (!wpa2IE_p)
				{
                    /* AP requires WPA - station WPA not enabled - station WPA2 enabled. */
    				return;
				}
			}
			else if (!vStaInfo_p->staSecurityMibs.mib_RSNConfigWPA2_p->WPA2Enabled)
			{
    			if (!wpaIE_p)
    			{
                    /* AP requires WPA2 - station WPA2 not enabled - station WPA enabled. */
    				return;
    			}
			}
        }
        else
        {
            /* Return failure if station WPA/WPA2 enabled but WPA/WPA2 elements not found. */
            if (vStaInfo_p->staSecurityMibs.mib_PrivacyTable_p->RSNEnabled)
            {
                return;
            }
            /* Return failure if station WEP enabled and capability Privacy bit mismatch.*/            
            if ((vStaInfo_p->staSecurityMibs.mib_PrivacyTable_p->PrivInvoked ^ MgmtMsg_p->Body.ProbeRsp.CapInfo.Privacy))
            {
                return;
            }
        }
    }
   	SPIN_LOCK_IRQSAVE(&vStaInfo_p->ScanResultsLock, vStaInfo_p->ScanResultsFlags);   
    AccumulateScanResults(vStaInfo_p, MgmtMsg_p, rfHdr_p);
    SPIN_UNLOCK_IRQRESTORE(&vStaInfo_p->ScanResultsLock, vStaInfo_p->ScanResultsFlags);	
}

/*************************************************************************
* Function: syncSrv_SetStatusTimer
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern SINT32 syncSrv_SetStatusTimer(vmacStaInfo_t *vStaInfo_p,
                                     UINT8 status) 
{
    UINT8  macIndex;
    
    macIndex = mlmeApiGetMacIndex(vStaInfo_p);
    if(status == 1)
    {
        mlmeApiStartTimer(vStaInfo_p, 
                          (UINT8 *)&vStaInfo_p->statusTimer,
                          &syncSrv_StatusCheckTimeOut,
                          g_CheckStatusTime[macIndex]);
    }
    else if (status == 0)
    {
        vStaInfo_p->urProbeRspMissedCnt = 0;
		ClientModeDataCount[macIndex] = 0;		
        mlmeApiStopTimer(vStaInfo_p, 
                          (UINT8 *)&vStaInfo_p->statusTimer);
    }
    return MLME_SUCCESS;
}

/*************************************************************************
* Function: syncSrv_SetKeepAliveTimer
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern SINT32 syncSrv_SetKeepAliveTimer(vmacStaInfo_t *vStaInfo_p,
                                     UINT8 status) 
{
    if(!g_KeepAliveEnable)
	return MLME_SUCCESS;
    
    if(status == 1)
    {
        mlmeApiStartTimer(vStaInfo_p, 
                          (UINT8 *)&vStaInfo_p->keepaliveTimer,
                          &syncSrv_KeepAliveTimeOut,
                          g_KeepAliveTime);
    }
    else if (status == 0)
    {
        mlmeApiStopTimer(vStaInfo_p, 
                          (UINT8 *)&vStaInfo_p->keepaliveTimer);
    }
    return MLME_SUCCESS;
}

/* Added for Site Survey */
/*************************************************************************
* Function: syncSrv_IsWpaEnabled
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
static UINT32 syncSrv_IsWpaMode( dot11MgtFrame_t *mgtFrame_p)
{
    void *data_p;
    UINT32 lenPacket;
    UINT32 lenOffset;
    UINT8 count = 0;

    lenPacket = mgtFrame_p->Hdr.FrmBodyLen;
    
    data_p = &mgtFrame_p->Body.Bcn.CapInfo;
    data_p += sizeof(IEEEtypes_CapInfo_t);
    lenOffset = sizeof(IEEEtypes_MgmtHdr_t)
              + sizeof(IEEEtypes_TimeStamp_t)
              + sizeof(IEEEtypes_BcnInterval_t) 
              + sizeof(IEEEtypes_CapInfo_t);
    while(lenOffset < lenPacket)
    {
        if(*(IEEEtypes_ElementId_t *)data_p == RSN_IE)
        {
            UINT32 wpaInfo;

            count++;
            memcpy((UINT8 *)&wpaInfo, (UINT8 *)(data_p + 2), sizeof(UINT32));
            if(wpaInfo == 0x01f25000)
            {
                return 1;
            }
        }
        else if(*(IEEEtypes_ElementId_t *)data_p == RSN_IEWPA2)
		{
            UINT8 *cipher = (UINT8 *)data_p + 4;
            if ((cipher[0] == 0x00) && (cipher[1] == 0x0f) && (cipher[2] == 0xac))
			return 2;
        }
        data_p += (2 + *((UINT8 *)(data_p + 1)));
        lenOffset += (2 + *((UINT8 *)(data_p + 1)));
    }
    return 0;
}

extern void SyncSrvSta_AccumulateSiteSurveyResults( vmacStaInfo_t *vStaInfo_p,
                                                    void *BssData_p, 
                                                    UINT8 *rfHdr_p)
{
   UINT32                  i;
   BOOLEAN  notFound = TRUE;
   API_SURVEY_ENTRY *bncEntry_p = NULL;
   void     *attrib_p = NULL;

   if((attrib_p = syncSrvSta_ParseAttrib((dot11MgtFrame_t *)BssData_p, DS_PARAM_SET)) != NULL)
   {  
       if(mlmeApiGetRfChannel(vStaInfo_p) != ((IEEEtypes_DsParamSet_t *)attrib_p)->CurrentChan)
       {
           return;
       }
   }
   for(i=0; (i < SITE_SURVEY_ENTRY_MAX) && notFound; i++)
   {
       if(!vStaInfo_p->sSurveyTable_p->siteSurveyInfo[i].dirty)
       {
           bncEntry_p = &vStaInfo_p->sSurveyTable_p->siteSurveyInfo[i];
           memset(bncEntry_p, 0, sizeof(API_SURVEY_ENTRY));
           notFound = FALSE;
       }
       else if(!memcmp(vStaInfo_p->sSurveyTable_p->siteSurveyInfo[i].BssId, 
                       ((dot11MgtFrame_t *)BssData_p)->Hdr.BssId, 6))
       {
           /* Record RSSI */
           vStaInfo_p->sSurveyTable_p->siteSurveyInfo[i].RSSI = ((WLAN_RX_INFO *)rfHdr_p)->RSSI;
           return;
       }
    }
    if(bncEntry_p == NULL)
    {
        return;
    }
    /* Record BSSID */
    memcpy(bncEntry_p->BssId, ((dot11MgtFrame_t *)BssData_p)->Hdr.BssId, 6);
    /* Record SSID */
    memcpy(bncEntry_p->SsId, ((dot11MgtFrame_t *)BssData_p)->Body.Bcn.SsId.SsId, ((dot11MgtFrame_t *)BssData_p)->Body.Bcn.SsId.Len);
    /* Record Channel */
    if((attrib_p = syncSrvSta_ParseAttrib((dot11MgtFrame_t *)BssData_p, DS_PARAM_SET)) != NULL)
    {  
       bncEntry_p->channel = ((IEEEtypes_DsParamSet_t *)attrib_p)->CurrentChan;
    }
    /* Assume B AP by default */
    bncEntry_p->B_Support = 1;
    /* Record if G AP */
    if((attrib_p = syncSrvSta_ParseAttrib((dot11MgtFrame_t *)BssData_p, EXT_SUPPORTED_RATES)) != NULL)
    {  
        if(((IEEEtypes_SuppRatesElement_t *)attrib_p)->Len > 0)
        {
            bncEntry_p->G_Support = 1;
            bncEntry_p->B_Support = 0;
        }
    }
    else if((attrib_p = syncSrvSta_ParseAttrib((dot11MgtFrame_t *)BssData_p, SUPPORTED_RATES)) != NULL)
    {  
        if(((IEEEtypes_SuppRatesElement_t *)attrib_p)->Len > MAX_B_DATA_RATES)
        {
            bncEntry_p->G_Support = 1;
            bncEntry_p->B_Support = 0;
        }
    }
    /* Record RSSI */
    bncEntry_p->RSSI = ((WLAN_RX_INFO *)rfHdr_p)->RSSI;
    /* Record IBSS bit */
    bncEntry_p->IBSS = ((dot11MgtFrame_t *)BssData_p)->Body.Bcn.CapInfo.Ibss;
    /* Record WEP enable option */
    bncEntry_p->wepEnabled = ((dot11MgtFrame_t *)BssData_p)->Body.Bcn.CapInfo.Privacy;
    /* Record WPA enable option */
    bncEntry_p->wpaEnabled = syncSrv_IsWpaMode((dot11MgtFrame_t *)BssData_p);
    /* Set dirty bit */
    bncEntry_p->dirty = 1;
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
extern SINT32 syncSrv_CmdExceptionHandler(vmacStaInfo_t *vStaInfo_p)
{
    if(vStaInfo_p->cmdHistory&cmdScan)
    {
        syncSrv_SndScanCnfm(vStaInfo_p, 
						   SCAN_RESULT_UNEXPECTED_ERROR, 
						   0, 
						   0, 
						   NULL);
    }
	vStaInfo_p->cmdHistory = 0;
    return MLME_SUCCESS;
}

/*************************************************************************
* Function: syncSrv_ScanFilter
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern void syncSrv_PrbeRspNoBssHandler( vmacStaInfo_t *vStaInfo_p,
                                  dot11MgtFrame_t *MgmtMsg_p, 
                                  UINT8 *rfHdr_p )
{
    /* Check BSSID */
    if(memcmp(&MgmtMsg_p->Hdr.BssId[0], 
                      &vStaInfo_p->bssDescProfile_p->BssId[0], 
                      sizeof(IEEEtypes_MacAddr_t)))
    {
        return;
    }
    if(syncSrv_ValidatePeerSetting(vStaInfo_p, MgmtMsg_p) == FALSE)
    {
        mlmeApiEventNotification(vStaInfo_p,
                                        PeerDetectInCompat,
                                        &vStaInfo_p->bssDescProfile_p->BssId[0], 
                                        0);
        return;
    }
    smeStateMgr_SndJoinCmd( (UINT8 *)vStaInfo_p,
	                                   vStaInfo_p->bssDescProfile_p);
}

#ifdef IEEE80211_DH
void station11hTimerCB(void *data_p)
{
	vmacStaInfo_t *vStaInfo_p = (vmacStaInfo_t *)data_p ;
    vmacEntry_t  *vmacEntry_p = (vmacEntry_t *)vStaInfo_p->vMacEntry_p;
	struct net_device *staDev = (struct net_device *)vmacEntry_p->privInfo_p;


    WLDBG_INFO(DBG_LEVEL_1, "enter station11hTimerCB timeout handler\n");
	vStaInfo_p->station11hTimerFired = 0 ;
    WLDBG_INFO(DBG_LEVEL_7, "NOW SWITCH CHANNEL to %d\n", vStaInfo_p->station11hChannel);
    mlmeApiSetRfChannel(vStaInfo_p, vStaInfo_p->station11hChannel, 1, FALSE );
	vStaInfo_p->JoinChannel = vStaInfo_p->station11hChannel ;
	macMgmtMlme_RestartDataTraffic(staDev);
#ifdef WMON
	sprintf(g_wmon_DFSLog + strlen(g_wmon_DFSLog), "DFS:: Switched Channel to %d\n", vStaInfo_p->JoinChannel );
#endif //WMON
}
#endif //IEEE80211_DH
