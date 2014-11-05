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


//=============================================================================
//                               INCLUDE FILES
//=============================================================================


#ifdef WDS_FEATURE
#include "wds.h"

#include "ap8xLnxFwcmd.h"
#include "bcngen.h"
#define MAX_WDS_PORT   6


void SetWdsMode(UINT8 Mode);
void SetWdsBcastMode(UINT8 Mode);
void setWdsPeerInfo(PeerInfo_t *pWdsPeerInfo, UINT8 Mode);
BOOLEAN wlCreateWdsPort(struct wlprivate *wlpptr, UINT8 *macAddr, UINT8 wdsMode);
void setStaPeerInfoApMode(struct wlprivate *wlpptr, extStaDb_StaInfo_t *pStaInfo, PeerInfo_t *pWdsPeerInfo, UINT8 ApMode, struct wds_port *pWdsPort);
BOOLEAN setWdsStaPeerInfoWdsPortMode(struct wlprivate *wlpptr, extStaDb_StaInfo_t *pStaInfo, PeerInfo_t *pWdsPeerInfo, UINT8 wdsPortMode);
BOOLEAN wlSetWdsPort(struct wlprivate *wlpptr, UINT8 *macAddr, UINT8 wdsIndex, UINT8 wdsPortMode);

extern extStaDb_StaInfo_t *macMgtStaDbInit(vmacApInfo_t *vmacSta_p,IEEEtypes_MacAddr_t *staMacAddr, 
										   IEEEtypes_MacAddr_t *apMacAddr);
extern int wlFwSetSecurity(struct net_device *netdev,	u_int8_t *staaddr);
extern void rtnl_lock(void);
extern void rtnl_unlock(void);
extern void FixedRateCtl(extStaDb_StaInfo_t *pStaInfo, PeerInfo_t	*PeerInfo, MIB_802DOT11 *mib);

extern UINT32 txbfcap;
extern UINT32 vht_cap;

//=============================================================================
//                         CODED PUBLIC PROCEDURES
//=============================================================================

#define IS_GROUP(macaddr)  ((*(UINT8*)macaddr & 0x01) == 0x01) 

#define MACADDR_CPY(macaddr1,macaddr2) { *(UINT16*)macaddr1 = *(UINT16*)macaddr2; \
	*(UINT16 *)((UINT16*)macaddr1+1) = *(UINT16 *)((UINT16*)macaddr2+1); \
	*(UINT16 *)((UINT16*)macaddr1+2) = *(UINT16 *)((UINT16*)macaddr2+2);}

BOOLEAN validWdsIndex(UINT8 wdsIndex)
{
	if (/*(wdsIndex >= 0) && */(wdsIndex < MAX_WDS_PORT))
	{
		return TRUE;
	}
	return FALSE;
}

void getWdsModeStr(char *wdsModeStr, UINT8 wdsPortMode)
{
	switch (wdsPortMode)
	{
	case NONLY_MODE:
		sprintf(wdsModeStr, "%s",  "11n mode");
		break;
	case AONLY_MODE:
		sprintf(wdsModeStr, "%s",  "11a mode");
		break;

	case BONLY_MODE:
		sprintf(wdsModeStr, "%s",  "11b mode");
		break;

	case GONLY_MODE:           
		sprintf(wdsModeStr, "%s",  "11g mode");
		break;

	default:
		sprintf(wdsModeStr, "%s",  "11x invalid mode");
		break;

	}

}

void AP_InitWds(struct wlprivate *wlpptr)
{
	UINT8 i;
	if (*(wlpptr->vmacSta_p->Mib802dot11->mib_wdsEnable))
	{
		/* Simple Initialization for G mode WDS ports. */
		for (i = 0 ; i < MAX_WDS_PORT ; i++)
		{
			wlpptr->vmacSta_p->wdsPort[i].pWdsDevInfo = (void *) &wlpptr->vmacSta_p->wdsPeerInfo[i];
			setWdsPeerInfo(&wlpptr->vmacSta_p->wdsPeerInfo[i], *(wlpptr->vmacSta_p->Mib802dot11->mib_ApMode));
			if (wlpptr->vmacSta_p->wdsPort[i].active)
			{
				updateWds(wlpptr->vmacSta_p->wdsPort[i].netDevWds);
			}
		}
	}
}

void setWdsPeerInfo(PeerInfo_t *pWdsPeerInfo, UINT8 Mode)
{
	memset((void *)pWdsPeerInfo, 0, sizeof(PeerInfo_t));

	switch (Mode)
	{
	case AP_MODE_N_ONLY:
	case AP_MODE_BandN:
	case AP_MODE_GandN:
	case AP_MODE_BandGandN:
	case AP_MODE_AandN:
    case AP_MODE_5GHZ_N_ONLY:
#ifdef SOC_W8864	
    case AP_MODE_5GHZ_11AC_ONLY:
    case AP_MODE_2_4GHZ_11AC_MIXED:
    case AP_MODE_5GHZ_Nand11AC:
#endif    
		break;

	case AP_MODE_B_ONLY:
		pWdsPeerInfo->CapInfo.APSD        = 0;
		pWdsPeerInfo->CapInfo.BlckAck     = 0;
		pWdsPeerInfo->CapInfo.CfPollable  = 0;
		pWdsPeerInfo->CapInfo.CfPollRqst  = 0;
		pWdsPeerInfo->CapInfo.ChanAgility = 0;
		pWdsPeerInfo->CapInfo.DsssOfdm    = 0;
		pWdsPeerInfo->CapInfo.Ess         = 1;
		pWdsPeerInfo->CapInfo.Ibss        = 0;
		pWdsPeerInfo->CapInfo.Pbcc        = 0;
		pWdsPeerInfo->CapInfo.Privacy     = 0;
		pWdsPeerInfo->CapInfo.ShortPreamble = 1;
		pWdsPeerInfo->CapInfo.ShortSlotTime = 1;
		pWdsPeerInfo->LegacyRateBitMap    = 0x0000000F;  // Set for b rates.
		pWdsPeerInfo->MrvlSta             = 0;
		break;

	default:    // case AP_MODE_G_ONLY:
		// case AP_MODE_MIXED:
		pWdsPeerInfo->CapInfo.APSD        = 0;
		pWdsPeerInfo->CapInfo.BlckAck     = 0;
		pWdsPeerInfo->CapInfo.CfPollable  = 0;
		pWdsPeerInfo->CapInfo.CfPollRqst  = 0;
		pWdsPeerInfo->CapInfo.ChanAgility = 0;
		pWdsPeerInfo->CapInfo.DsssOfdm    = 0;
		pWdsPeerInfo->CapInfo.Ess         = 1;
		pWdsPeerInfo->CapInfo.Ibss        = 0;
		pWdsPeerInfo->CapInfo.Pbcc        = 0;
		pWdsPeerInfo->CapInfo.Privacy     = 0;
		pWdsPeerInfo->CapInfo.ShortPreamble = 1;
		pWdsPeerInfo->CapInfo.ShortSlotTime = 1;
		pWdsPeerInfo->LegacyRateBitMap    = 0x00001FFF;  // Set for g rates.
		pWdsPeerInfo->MrvlSta             = 0;
		break;

	}


}

struct wds_port *getWdsPortFromMacAddr(struct wlprivate *wlpptr, UINT8 *pMacAddr)
{
	UINT8 i;

	for (i = 0 ; i < MAX_WDS_PORT ; i++)
	{
		if (!memcmp(&wlpptr->vmacSta_p->wdsPort[i].netDevWds->dev_addr, pMacAddr, 6))
		{
			if (wlpptr->vmacSta_p->wdsPort[i].active)
			{
				return (&wlpptr->vmacSta_p->wdsPort[i]);
			}
		}
	}
	return NULL;
}

struct wds_port *getWdsPortFromNetDev(struct wlprivate *wlpptr, struct net_device *netdev)
{
	UINT8 i;

	for (i = 0 ; i < MAX_WDS_PORT ; i++)
	{
		if (netdev == wlpptr->vmacSta_p->wdsPort[i].netDevWds)
		{
			return (&wlpptr->vmacSta_p->wdsPort[i]);
		}
	}    
	return NULL;
}


UINT8 defaultWdsMacAddr[6] = {0x00, 0xF8, 0x43, 0x20, 0x00, 0x80};
void wlprobeInitWds(struct wlprivate *wlpptr)
{
	UINT32 i;
	for (i = 0 ; i < MAX_WDS_PORT ; i++)
	{
		wlpptr->vmacSta_p->wdsPort[i].netDevWds->irq       = wlpptr->netDev->irq;
		wlpptr->vmacSta_p->wdsPort[i].netDevWds->mem_start = wlpptr->netDev->mem_start;
		wlpptr->vmacSta_p->wdsPort[i].netDevWds->mem_end   = wlpptr->netDev->mem_end;
		NETDEV_PRIV_S(wlpptr->vmacSta_p->wdsPort[i].netDevWds)= (void *) wlpptr;
		wlpptr->vmacSta_p->wdsPort[i].pWdsDevInfo         = (void *) &wlpptr->vmacSta_p->wdsPeerInfo[i];
		sprintf(wlpptr->vmacSta_p->wdsPort[i].netDevWds->name, DRV_NAME_WDS, wlpptr->netDev->name, (int )i);
		setWdsPeerInfo(&wlpptr->vmacSta_p->wdsPeerInfo[i], AP_MODE_G_ONLY); // Set to default G.
		memcpy(&wlpptr->vmacSta_p->wdsPort[i].netDevWds->dev_addr, defaultWdsMacAddr, 6);
		//defaultWdsMacAddr[5]+=1;
	}
}


void wds_Init(struct net_device *netdev)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	UINT32 i;
	for (i = 0 ; i < MAX_WDS_PORT ; i++)
	{
		wlpptr->vmacSta_p->wdsPort[i].netDevWds->irq       = wlpptr->netDev->irq;
		wlpptr->vmacSta_p->wdsPort[i].netDevWds->mem_start = wlpptr->netDev->mem_start;
		wlpptr->vmacSta_p->wdsPort[i].netDevWds->mem_end   = wlpptr->netDev->mem_end;
		NETDEV_PRIV_S(wlpptr->vmacSta_p->wdsPort[i].netDevWds)      = (void *)  wlpptr;
		wlpptr->vmacSta_p->wdsPort[i].pWdsDevInfo         = (void *)  &wlpptr->vmacSta_p->wdsPeerInfo[i];
		setWdsPeerInfo(&wlpptr->vmacSta_p->wdsPeerInfo[i], AP_MODE_G_ONLY); // Set to default G.
		wlpptr->vmacSta_p->wdsActive[i] = FALSE;
		wlpptr->vmacSta_p->wdsPort[i].netDevWds->flags     = 0;
		memcpy(&wlpptr->vmacSta_p->wdsPort[i].netDevWds->dev_addr, defaultWdsMacAddr, 6);
		//defaultWdsMacAddr[5]+=1;
	}
}


void createWdsPort(struct net_device *netdev, UINT8 *pMacAddr)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	wlCreateWdsPort(wlpptr, pMacAddr, AP_MODE_G_ONLY);
	return;
}

BOOLEAN setWdsPort(struct net_device *netdev, UINT8 *pMacAddr, UINT8 wdsIndex, UINT8 wdsPortMode)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	WLDBG_INFO(DBG_LEVEL_6, "setWdsPort index = %d Mode %d macAddr = %x:%x:%x:%x:%x:%x\n", wdsIndex, wdsPortMode,
		pMacAddr[0],
		pMacAddr[1],
		pMacAddr[2],
		pMacAddr[3],
		pMacAddr[4],
		pMacAddr[5]);
	return(wlSetWdsPort(wlpptr, pMacAddr, wdsIndex, wdsPortMode ));
}


extStaDb_StaInfo_t *updateWds(struct net_device *netdev)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	vmacApInfo_t *vmacSta_p = wlpptr->vmacSta_p;
	extStaDb_StaInfo_t *pStaInfo = NULL;
	struct wds_port *pWdsPort    = NULL;
	PeerInfo_t * pPeerInfo       = NULL;
	if ((pWdsPort = getWdsPortFromNetDev(wlpptr, netdev)) == NULL)
	{
		WLDBG_INFO(DBG_LEVEL_6, "Cannot add WDS Port %x to database \n", netdev->name);
		return NULL;
	}

	//if ((pStaInfo = extStaDb_GetStaInfo(vmacSta_p,(IEEEtypes_MacAddr_t *)&netdev->dev_addr, 1)) == NULL)
	if ((pStaInfo = extStaDb_GetStaInfo(vmacSta_p,(IEEEtypes_MacAddr_t *)&pWdsPort->wdsMacAddr, 1)) == NULL)
	{

		pPeerInfo = (PeerInfo_t *) pWdsPort->pWdsDevInfo;
		/* Station not in Stn table, add */
		//if ((pStaInfo=macMgtStaDbInit((IEEEtypes_MacAddr_t *) &netdev->dev_addr, (IEEEtypes_MacAddr_t *) wlpptr->netDev->dev_addr)))
		if ((pStaInfo=macMgtStaDbInit(vmacSta_p,(IEEEtypes_MacAddr_t *) &pWdsPort->wdsMacAddr, (IEEEtypes_MacAddr_t *) &vmacSta_p->macBssId)))
		{
			pStaInfo->State = UNAUTHENTICATED;
			pStaInfo->PwrMode = PWR_MODE_ACTIVE;
			pStaInfo->Aid = AssignAid();
			pStaInfo->AP = TRUE;
			pStaInfo->IsStaQSTA = *(wlpptr->vmacSta_p->Mib802dot11->QoSOptImpl);
			pStaInfo->wdsInfo = (void *) netdev;
			pStaInfo->wdsPortInfo = (void *) pWdsPort;
			AddHT_IE(wlpptr->vmacSta_p,&pStaInfo->HtElem);
		}
		else
		{
			return NULL;
		}
		// Set the Peer info, if wdsPortMode not set - use Ap Mode to set Peer info.
		if (!setWdsStaPeerInfoWdsPortMode(wlpptr, pStaInfo, pPeerInfo, pWdsPort->wdsPortMode))
		{
			setStaPeerInfoApMode(wlpptr, pStaInfo, pPeerInfo, *(wlpptr->vmacSta_p->Mib802dot11->mib_ApMode), pWdsPort);
		}
		// Set Qos values to 0.

#ifdef EXPLICIT_BF
#if 0
		pPeerInfo->TxBFCapabilities.ImplicitTxBFRxCapable=0;
		pPeerInfo->TxBFCapabilities.RxStaggeredSoundingCapable=0;
		pPeerInfo->TxBFCapabilities.TxStaggeredSoundingCapable=0;
		pPeerInfo->TxBFCapabilities.RxNDPCapable=1;
		pPeerInfo->TxBFCapabilities.TxNDPCapable=1;
		pPeerInfo->TxBFCapabilities.ImplicitTXBfTXCapable=0;
		pPeerInfo->TxBFCapabilities.Calibration=0;
		pPeerInfo->TxBFCapabilities.ExplicitCSITxBFCapable=1;
		pPeerInfo->TxBFCapabilities.ExplicitNonCompressedSteerCapable=1;
		pPeerInfo->TxBFCapabilities.ExplicitCompressedSteeringCapable=1;
		pPeerInfo->TxBFCapabilities.ExplicitTxBFCSIFB=0;
		pPeerInfo->TxBFCapabilities.ExplicitNonCompressedBFFBCapable=2; // 1 for delayed fb, 2 for immediate fb, 3 for delay and immediate fb
		pPeerInfo->TxBFCapabilities.ExplicitCompressedBFFBCapable=2;
		pPeerInfo->TxBFCapabilities.MinimalGrouping=1;
		pPeerInfo->TxBFCapabilities.CSINoBFAntSupport=0;
		pPeerInfo->TxBFCapabilities.NonCompressedSteerNoAntennaSupport=3; //0 for single tx ant, 1 for 2, 2 for 3 , 3 for 4 
		pPeerInfo->TxBFCapabilities.CompressedSteerNoAntennaSupport=3;
		pPeerInfo->TxBFCapabilities.CSIMaxNoRowBFSupport=0;
		pPeerInfo->TxBFCapabilities.ChannelEstimateCapable=0;
		pPeerInfo->TxBFCapabilities.Reserved=0;
#else
		pPeerInfo->TxBFCapabilities=txbfcap;
#endif
#endif
		pStaInfo->FwStaPtr = wlFwSetNewStn(netdev,(u_int8_t *)&pWdsPort->wdsMacAddr, pStaInfo->Aid, pStaInfo->StnId, 0, pPeerInfo, 0, *(wlpptr->vmacSta_p->Mib802dot11->QoSOptImpl));
		wlFwSetSecurity(netdev,( u_int8_t *)&pWdsPort->wdsMacAddr);
	}
	return pStaInfo;
}


void setWdsPortMacAddr(struct net_device *netdev, UINT8 *pMacAddr)
{
	//	extStaDb_StaInfo_t *pStaInfo = NULL;
	WLDBG_INFO(DBG_LEVEL_6, "setWdsPortMacAddr %x:%x:%x:%x:%x:%x\n",
		pMacAddr[0],
		pMacAddr[2],
		pMacAddr[3],
		pMacAddr[4],
		pMacAddr[5]);

	memcpy(netdev->dev_addr, pMacAddr, 6);
	return;
}


BOOLEAN wlCreateWdsPort(struct wlprivate *wlpptr, UINT8 *macAddr, UINT8 wdsMode)
{
	// Check if maximum number of ports exceeded.
	if (wlpptr->vmacSta_p->CurrFreeWdsPort >= MAX_WDS_PORT)
	{
		printk(KERN_ERR "Exceeded max number of WDS ports = %d.\n", MAX_WDS_PORT);
		return FALSE;
	}

	if (wlpptr->vmacSta_p->wdsPort[wlpptr->vmacSta_p->CurrFreeWdsPort].active)
	{
		//error should not be active.
		printk(KERN_ERR "%s: WDS port already active.\n", wlpptr->vmacSta_p->wdsPort[wlpptr->vmacSta_p->CurrFreeWdsPort].netDevWds->name);
		return FALSE;
	}
	rtnl_unlock();
	memcpy(&wlpptr->vmacSta_p->wdsPort[wlpptr->vmacSta_p->CurrFreeWdsPort].netDevWds->dev_addr, macAddr, 6);
	setWdsPeerInfo((PeerInfo_t *)&wlpptr->vmacSta_p->wdsPort[wlpptr->vmacSta_p->CurrFreeWdsPort].pWdsDevInfo, *(wlpptr->vmacSta_p->Mib802dot11->mib_ApMode));
	ether_setup(wlpptr->vmacSta_p->wdsPort[wlpptr->vmacSta_p->CurrFreeWdsPort].netDevWds);
	sprintf(wlpptr->vmacSta_p->wdsPort[wlpptr->vmacSta_p->CurrFreeWdsPort].netDevWds->name, DRV_NAME_WDS, wlpptr->netDev->name,(int)wlpptr->vmacSta_p->CurrFreeWdsPort);
	if (register_netdev(wlpptr->vmacSta_p->wdsPort[wlpptr->vmacSta_p->CurrFreeWdsPort].netDevWds))
	{
		printk(KERN_ERR "%s: failed to register WDS device\n", wlpptr->vmacSta_p->wdsPort[wlpptr->vmacSta_p->CurrFreeWdsPort].netDevWds->name);
		return FALSE;
	}
	wlpptr->vmacSta_p->wdsPort[wlpptr->vmacSta_p->CurrFreeWdsPort].active = TRUE;
	WLDBG_INFO(DBG_LEVEL_6, "Create WDS port %s \n", wlpptr->vmacSta_p->wdsPort[wlpptr->vmacSta_p->CurrFreeWdsPort].netDevWds->name);
	rtnl_lock();
	// Update to the next free wds port.
	wlpptr->vmacSta_p->CurrFreeWdsPort++;
	return TRUE;

}

// Delete WDS port - netdev should be WDS port.
BOOLEAN wlDeleteWdsPort(struct net_device *netdev)
{
	netif_stop_queue(netdev);
	unregister_netdev(netdev);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,25)
	free_netdev(netdev);
#endif		
	return TRUE;
}

void setStaPeerInfoApMode(struct wlprivate *wlpptr, extStaDb_StaInfo_t *pStaInfo, PeerInfo_t *pWdsPeerInfo, UINT8 ApMode, struct wds_port *pWdsPort)
{
	MIB_PHY_DSSS_TABLE *PhyDSSSTable=wlpptr->vmacSta_p->Mib802dot11->PhyDSSSTable;
	UINT8 *mib_guardInterval_p = wlpptr->vmacSta_p->Mib802dot11->mib_guardInterval;
	memset((void *)pWdsPeerInfo, 0, sizeof(PeerInfo_t));

	switch (ApMode)
	{
	case AP_MODE_N_ONLY:
	case AP_MODE_BandN:
	case AP_MODE_GandN:
	case AP_MODE_BandGandN:
	case AP_MODE_AandN:
    case AP_MODE_5GHZ_N_ONLY:
#ifdef SOC_W8864	
    case AP_MODE_11AC:
    case AP_MODE_2_4GHZ_11AC_MIXED:
    case AP_MODE_5GHZ_11AC_ONLY:
    case AP_MODE_5GHZ_Nand11AC:
#endif    
		WLDBG_INFO(DBG_LEVEL_6, "WDS Port N Mode \n");

		if(PhyDSSSTable->Chanflag.ChnlWidth != CH_20_MHz_WIDTH)
			pWdsPeerInfo->HTCapabilitiesInfo.SupChanWidth = 1;
		else
			pWdsPeerInfo->HTCapabilitiesInfo.SupChanWidth = 0;
		if(*mib_guardInterval_p != 0)
		{
			pWdsPeerInfo->HTCapabilitiesInfo.SGI20MHz = (*mib_guardInterval_p == 2)?0:1;
			pWdsPeerInfo->HTCapabilitiesInfo.SGI40MHz = (*mib_guardInterval_p == 2)?0:1;
		}
		else
		{
			pWdsPeerInfo->HTCapabilitiesInfo.SGI20MHz = 1;
			pWdsPeerInfo->HTCapabilitiesInfo.SGI40MHz = 1;
		}
		pWdsPeerInfo->HTCapabilitiesInfo.AdvCoding = 1;
		pWdsPeerInfo->HTCapabilitiesInfo.MIMOPwSave = 0x3;

		pWdsPeerInfo->MacHTParamInfo     = *(wlpptr->vmacSta_p->Mib802dot11->mib_ampdu_factor)
			|((*(wlpptr->vmacSta_p->Mib802dot11->mib_ampdu_density))<<2);

		pWdsPeerInfo->HTRateBitMap = (0xff | (0xff<< 8) |(0xff << 16) | (0xff<< 24));
		if(pWdsPort)
		{
			if ((*(wlpptr->vmacSta_p->Mib802dot11->pMib_11nAggrMode)))
				pStaInfo->aggr11n.threshold = AGGRTHRESHOLD;
			else
				pStaInfo->aggr11n.threshold = 0;

			pStaInfo->aggr11n.thresholdBackUp = pStaInfo->aggr11n.threshold;   
			pStaInfo->aggr11n.cap = (*(wlpptr->vmacSta_p->Mib802dot11->pMib_11nAggrMode) & WL_MODE_AMSDU_TX_MASK);

			pStaInfo->ClientMode = NONLY_MODE;
			pWdsPort->wdsPortMode = NONLY_MODE;
#ifdef SOC_W8864	
			if(ApMode&AP_MODE_11AC) {
			    MIB_802DOT11 *mib=wlpptr->vmacSta_p->Mib802dot11;
			    if (*(mib->mib_3x3Rate) == 1 && (( * (mib->mib_rxAntenna) ==  0) || (*(mib->mib_rxAntenna) ==  3))) {
    			    pWdsPort->wdsPortMode = AC_3SS_MODE;
                    pWdsPeerInfo->vht_MaxRxMcs = 0xffea;
			    } else if(!((*(mib->mib_rxAntenna) ==  0) || (*(mib->mib_rxAntenna) ==  3) ||(*(mib->mib_rxAntenna) ==  2))) {
                    /** 1x1 configuration **/
    			    pWdsPort->wdsPortMode = AC_1SS_MODE;
                    pWdsPeerInfo->vht_MaxRxMcs = 0xfffe;
                } else {
    			    pWdsPort->wdsPortMode = AC_2SS_MODE;
                    pWdsPeerInfo->vht_MaxRxMcs = 0xfffa;
                }
                pWdsPeerInfo->vht_cap =vht_cap;
				pWdsPeerInfo->vht_RxChannelWidth = 2;		
			}
#endif			
		}

		break;
	case AP_MODE_A_ONLY:
		WLDBG_INFO(DBG_LEVEL_6, "WDS Port A Mode \n");
		pWdsPeerInfo->CapInfo.APSD        = 0;
		pWdsPeerInfo->CapInfo.BlckAck     = 0;
		pWdsPeerInfo->CapInfo.CfPollable  = 0;
		pWdsPeerInfo->CapInfo.CfPollRqst  = 0;
		pWdsPeerInfo->CapInfo.ChanAgility = 0;
		pWdsPeerInfo->CapInfo.DsssOfdm    = 0;
		pWdsPeerInfo->CapInfo.Ess         = 1;
		pWdsPeerInfo->CapInfo.Ibss        = 0;
		pWdsPeerInfo->CapInfo.Pbcc        = 0;
		pWdsPeerInfo->CapInfo.Privacy     = 0;
		pWdsPeerInfo->CapInfo.ShortPreamble = 1;
		pWdsPeerInfo->CapInfo.ShortSlotTime = 1;
		pWdsPeerInfo->LegacyRateBitMap    = 0x00001FE0;  // Set for A rates.
		pWdsPeerInfo->MrvlSta             = 0;
		if(pWdsPort)
		{
			pStaInfo->IsSpectrumMgmt          = TRUE;
			pStaInfo->ClientMode              = AONLY_MODE;
			pWdsPort->wdsPortMode             = AONLY_MODE;
		}
		break;

	case AP_MODE_B_ONLY:
		WLDBG_INFO(DBG_LEVEL_6, "WDS Port B Mode \n");
		pWdsPeerInfo->CapInfo.APSD        = 0;
		pWdsPeerInfo->CapInfo.BlckAck     = 0;
		pWdsPeerInfo->CapInfo.CfPollable  = 0;
		pWdsPeerInfo->CapInfo.CfPollRqst  = 0;
		pWdsPeerInfo->CapInfo.ChanAgility = 0;
		pWdsPeerInfo->CapInfo.DsssOfdm    = 0;
		pWdsPeerInfo->CapInfo.Ess         = 1;
		pWdsPeerInfo->CapInfo.Ibss        = 0;
		pWdsPeerInfo->CapInfo.Pbcc        = 0;
		pWdsPeerInfo->CapInfo.Privacy     = 0;
		pWdsPeerInfo->CapInfo.ShortPreamble = 1;
		pWdsPeerInfo->CapInfo.ShortSlotTime = 1;
		pWdsPeerInfo->LegacyRateBitMap    = 0x0000000F;  // Set for b rates.
		pWdsPeerInfo->MrvlSta             = 0;
		if(pWdsPort)
		{
			pStaInfo->ClientMode              = BONLY_MODE;
			pWdsPort->wdsPortMode             = BONLY_MODE;
		}
		break;

	default:    // case AP_MODE_G_ONLY:
		// case AP_MODE_MIXED:
		WLDBG_INFO(DBG_LEVEL_6, "WDS Port G Mode \n");
		pWdsPeerInfo->CapInfo.APSD        = 0;
		pWdsPeerInfo->CapInfo.BlckAck     = 0;
		pWdsPeerInfo->CapInfo.CfPollable  = 0;
		pWdsPeerInfo->CapInfo.CfPollRqst  = 0;
		pWdsPeerInfo->CapInfo.ChanAgility = 0;
		pWdsPeerInfo->CapInfo.DsssOfdm    = 0;
		pWdsPeerInfo->CapInfo.Ess         = 1;
		pWdsPeerInfo->CapInfo.Ibss        = 0;
		pWdsPeerInfo->CapInfo.Pbcc        = 0;
		pWdsPeerInfo->CapInfo.Privacy     = 0;
		pWdsPeerInfo->CapInfo.ShortPreamble = 1;
		pWdsPeerInfo->CapInfo.ShortSlotTime = 1;
		pWdsPeerInfo->LegacyRateBitMap    = 0x00001FFF;  // Set for g rates.
		pWdsPeerInfo->MrvlSta             = 0;
		if(pWdsPort)
		{
			pStaInfo->ClientMode              = GONLY_MODE;
			pWdsPort->wdsPortMode             = GONLY_MODE;
		}
		break;

	}


}


BOOLEAN setWdsStaPeerInfoWdsPortMode(struct wlprivate *wlpptr, extStaDb_StaInfo_t *pStaInfo, PeerInfo_t *pWdsPeerInfo, UINT8 wdsPortMode)
{
	MIB_PHY_DSSS_TABLE *PhyDSSSTable=wlpptr->vmacSta_p->Mib802dot11->PhyDSSSTable;
	UINT8 *mib_guardInterval_p = wlpptr->vmacSta_p->Mib802dot11->mib_guardInterval;
	memset((void *)pWdsPeerInfo, 0, sizeof(PeerInfo_t));

	switch (wdsPortMode)
	{
	case NONLY_MODE:
#ifdef SOC_W8864
    case AC_1SS_MODE:
	case AC_2SS_MODE:
	case AC_3SS_MODE:
#endif	
		WLDBG_INFO(DBG_LEVEL_6, "WDS Port N Mode \n");

		if(PhyDSSSTable->Chanflag.ChnlWidth != CH_20_MHz_WIDTH)
			pWdsPeerInfo->HTCapabilitiesInfo.SupChanWidth = 1;
		else
			pWdsPeerInfo->HTCapabilitiesInfo.SupChanWidth = 0;
		if(*mib_guardInterval_p != 0)
		{
			pWdsPeerInfo->HTCapabilitiesInfo.SGI20MHz = (*mib_guardInterval_p == 2)?0:1;
			pWdsPeerInfo->HTCapabilitiesInfo.SGI40MHz = (*mib_guardInterval_p == 2)?0:1;
		}
		else
		{
			pWdsPeerInfo->HTCapabilitiesInfo.SGI20MHz = 1;
			pWdsPeerInfo->HTCapabilitiesInfo.SGI40MHz = 1;
		}
		pWdsPeerInfo->HTCapabilitiesInfo.AdvCoding = 1;
		pWdsPeerInfo->HTCapabilitiesInfo.MIMOPwSave = 0x3;

		pWdsPeerInfo->MacHTParamInfo     = *(wlpptr->vmacSta_p->Mib802dot11->mib_ampdu_factor)
			|((*(wlpptr->vmacSta_p->Mib802dot11->mib_ampdu_density))<<2);

		pWdsPeerInfo->HTRateBitMap = (0xff | (0xff<< 8) |(0xff << 16) | (0xff<< 24));
		if ((*(wlpptr->vmacSta_p->Mib802dot11->pMib_11nAggrMode)))
		{
			pStaInfo->aggr11n.threshold = AGGRTHRESHOLD;
		}
		else
			pStaInfo->aggr11n.threshold = 0;

		pStaInfo->aggr11n.thresholdBackUp = pStaInfo->aggr11n.threshold; 
		if ((*(wlpptr->vmacSta_p->Mib802dot11->pMib_11nAggrMode)& WL_MODE_AMSDU_TX_MASK)
			&&!(*(wlpptr->vmacSta_p->Mib802dot11->pMib_11nAggrMode)& WL_MODE_AMPDU_TX))
			pStaInfo->aggr11n.type |= WL_WLAN_TYPE_AMSDU;
		pStaInfo->aggr11n.cap = (*(wlpptr->vmacSta_p->Mib802dot11->pMib_11nAggrMode)& WL_MODE_AMSDU_TX_MASK);
#ifdef SOC_W8864
        if(wdsPortMode == AC_2SS_MODE) {        
            pWdsPeerInfo->vht_cap = vht_cap;
            pWdsPeerInfo->vht_MaxRxMcs = 0xfffa;
			pWdsPeerInfo->vht_RxChannelWidth = 2;		
        } else if(wdsPortMode == AC_3SS_MODE || wdsPortMode == NONLY_MODE) {
            pWdsPeerInfo->vht_cap = vht_cap;
            pWdsPeerInfo->vht_MaxRxMcs = 0xffea;
			pWdsPeerInfo->vht_RxChannelWidth = 2;		
        }
#endif        
		pStaInfo->ClientMode = NONLY_MODE;
		break;
	case AONLY_MODE:
		WLDBG_INFO(DBG_LEVEL_6, "WDS Port A Mode \n");
		pWdsPeerInfo->CapInfo.APSD        = 0;
		pWdsPeerInfo->CapInfo.BlckAck     = 0;
		pWdsPeerInfo->CapInfo.CfPollable  = 0;
		pWdsPeerInfo->CapInfo.CfPollRqst  = 0;
		pWdsPeerInfo->CapInfo.ChanAgility = 0;
		pWdsPeerInfo->CapInfo.DsssOfdm    = 0;
		pWdsPeerInfo->CapInfo.Ess         = 1;
		pWdsPeerInfo->CapInfo.Ibss        = 0;
		pWdsPeerInfo->CapInfo.Pbcc        = 0;
		pWdsPeerInfo->CapInfo.Privacy     = 0;
		pWdsPeerInfo->CapInfo.ShortPreamble = 1;
		pWdsPeerInfo->CapInfo.ShortSlotTime = 1;
		pWdsPeerInfo->LegacyRateBitMap    = 0x00001FE0;  // Set for A rates.
		pWdsPeerInfo->MrvlSta             = 0;
		pStaInfo->IsSpectrumMgmt          = TRUE;
		pStaInfo->ClientMode              = AONLY_MODE;
		break;

	case BONLY_MODE:
		WLDBG_INFO(DBG_LEVEL_6, "WDS Port B Mode \n");
		pWdsPeerInfo->CapInfo.APSD        = 0;
		pWdsPeerInfo->CapInfo.BlckAck     = 0;
		pWdsPeerInfo->CapInfo.CfPollable  = 0;
		pWdsPeerInfo->CapInfo.CfPollRqst  = 0;
		pWdsPeerInfo->CapInfo.ChanAgility = 0;
		pWdsPeerInfo->CapInfo.DsssOfdm    = 0;
		pWdsPeerInfo->CapInfo.Ess         = 1;
		pWdsPeerInfo->CapInfo.Ibss        = 0;
		pWdsPeerInfo->CapInfo.Pbcc        = 0;
		pWdsPeerInfo->CapInfo.Privacy     = 0;
		pWdsPeerInfo->CapInfo.ShortPreamble = 1;
		pWdsPeerInfo->CapInfo.ShortSlotTime = 1;
		pWdsPeerInfo->LegacyRateBitMap    = 0x0000000F;  // Set for b rates.
		pWdsPeerInfo->MrvlSta             = 0;
		pStaInfo->ClientMode              = BONLY_MODE;
		break;

	case GONLY_MODE:
		WLDBG_INFO(DBG_LEVEL_6, "WDS Port G Mode \n");
		pWdsPeerInfo->CapInfo.APSD        = 0;
		pWdsPeerInfo->CapInfo.BlckAck     = 0;
		pWdsPeerInfo->CapInfo.CfPollable  = 0;
		pWdsPeerInfo->CapInfo.CfPollRqst  = 0;
		pWdsPeerInfo->CapInfo.ChanAgility = 0;
		pWdsPeerInfo->CapInfo.DsssOfdm    = 0;
		pWdsPeerInfo->CapInfo.Ess         = 1;
		pWdsPeerInfo->CapInfo.Ibss        = 0;
		pWdsPeerInfo->CapInfo.Pbcc        = 0;
		pWdsPeerInfo->CapInfo.Privacy     = 0;
		pWdsPeerInfo->CapInfo.ShortPreamble = 1;
		pWdsPeerInfo->CapInfo.ShortSlotTime = 1;
		pWdsPeerInfo->LegacyRateBitMap    = 0x00001FFF;  // Set for g rates.
		pWdsPeerInfo->MrvlSta             = 0;
		pStaInfo->ClientMode              = GONLY_MODE;
		break;
	default:
		return FALSE;
		break;

	}
	return TRUE;
}


IEEEtypes_MacAddr_t allZeroMacAddr = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

BOOLEAN wlSetWdsPort(struct wlprivate *wlpptr, UINT8 *macAddr, UINT8 wdsIndex, UINT8 wdsPortMode)
{
	MIB_802DOT11 *mib = wlpptr->vmacSta_p->ShadowMib802dot11;
	// Check if maximum number of ports exceeded.
	if (wdsIndex >= MAX_WDS_PORT)
	{
		printk(KERN_ERR "WDS index out of range %d...%d.\n", 0, MAX_WDS_PORT-1);
		return FALSE;
	}
	WLDBG_INFO(DBG_LEVEL_6, "Set WDS Port dev_addr = %x:%x:%x:%x:%x:%x \n",
		macAddr[0],
		macAddr[1],
		macAddr[2],
		macAddr[3],
		macAddr[4],
		macAddr[5]);
	if(wlpptr->vmacSta_p->wdsPort[wdsIndex].netDevWds==NULL)
	{
		return FALSE;
	}
	if (!wlpptr->vmacSta_p->wdsPort[wdsIndex].active)
	{
		sprintf(wlpptr->vmacSta_p->wdsPort[wdsIndex].netDevWds->name, DRV_NAME_WDS,wlpptr->netDev->name, (int )wdsIndex);
	}
	wlpptr->vmacSta_p->wdsPort[wdsIndex].wdsPortMode = wdsPortMode;
	memcpy(&mib->mib_WdsMacAddr[wdsIndex][0], macAddr, 6);
	// Use the AP Mac Address for the device address.
	memcpy(wlpptr->vmacSta_p->wdsPort[wdsIndex].netDevWds->dev_addr, wlpptr->vmacSta_p->macStaAddr, 6);
	*(wlpptr->vmacSta_p->ShadowMib802dot11->mib_wdsEnable) = 1;
	return TRUE;

}


BOOLEAN wlMakeWdsPort(struct wds_port *pWdsPort) 
{

	ether_setup(pWdsPort->netDevWds);
	if (register_netdev(pWdsPort->netDevWds))
	{
		printk(KERN_ERR "%s: failed to register WDS device\n", pWdsPort->netDevWds->name);
		return FALSE;
	}

	WLDBG_INFO(DBG_LEVEL_6, "Register WDS port %s \n", &pWdsPort->netDevWds->name);
	return TRUE;

}


BOOLEAN wlCreateNewWdsPort(struct wds_port *pWdsPort) 
{
	//struct net_device *netdev = &pWdsPort->netDevWds;
	// Check if maximum number of ports exceeded.
	rtnl_unlock();
	ether_setup(pWdsPort->netDevWds);
	if (register_netdev(pWdsPort->netDevWds))
	{
		printk(KERN_ERR "%s: failed to register WDS device\n", pWdsPort->netDevWds->name);
		return FALSE;
	}
	netif_wake_queue(pWdsPort->netDevWds);
	pWdsPort->active = TRUE;
	WLDBG_INFO(DBG_LEVEL_6, "Register WDS port %s \n", &pWdsPort->netDevWds->name);
	rtnl_lock();
	return TRUE;

}


void AP_InitWdsPorts(struct wlprivate *wlpptr)
{
	UINT8 i;
	if (*(wlpptr->vmacSta_p->Mib802dot11->mib_wdsEnable))
	{
		// Create WDS ports
		for (i = 0 ; i < MAX_WDS_PORT ; i++)
		{
			WLDBG_INFO(DBG_LEVEL_6, "Mib wds Mac addr %x:%x:%x:%x:%x:%x\n",
				wlpptr->vmacSta_p->Mib802dot11->mib_WdsMacAddr[i][0][0],
				wlpptr->vmacSta_p->Mib802dot11->mib_WdsMacAddr[i][0][1],
				wlpptr->vmacSta_p->Mib802dot11->mib_WdsMacAddr[i][0][2],
				wlpptr->vmacSta_p->Mib802dot11->mib_WdsMacAddr[i][0][3],
				wlpptr->vmacSta_p->Mib802dot11->mib_WdsMacAddr[i][0][4],
				wlpptr->vmacSta_p->Mib802dot11->mib_WdsMacAddr[i][0][5]);

			if (!memcmp(&wlpptr->vmacSta_p->Mib802dot11->mib_WdsMacAddr[i][0], allZeroMacAddr, 6))
			{
				if (wlpptr->vmacSta_p->wdsActive[i])
				{
					if (wlpptr->vmacSta_p->wdsPort[i].netDevWds->flags & IFF_RUNNING)
					{
						wlstop_wds(wlpptr->vmacSta_p->wdsPort[i].netDevWds);
					}
					wlpptr->vmacSta_p->wdsActive[i] = FALSE;
					wlpptr->vmacSta_p->wdsPort[i].active = FALSE;
				}
			}
			else
			{
				{
					// enable wds port if wds mac address not all 0.
					wlpptr->vmacSta_p->wdsPort[i].pWdsDevInfo = (void *) &wlpptr->vmacSta_p->wdsPeerInfo[i];
					wlpptr->vmacSta_p->wdsPort[i].active = TRUE;
					wlpptr->vmacSta_p->wdsActive[i] = TRUE;
					NETDEV_PRIV_S(wlpptr->vmacSta_p->wdsPort[i].netDevWds) = wlpptr;
					MACADDR_CPY(&wlpptr->vmacSta_p->wdsPort[i].wdsMacAddr, &wlpptr->vmacSta_p->Mib802dot11->mib_WdsMacAddr[i][0]);
					WLDBG_INFO(DBG_LEVEL_6, "Add WDS port %s to database.\n", wlpptr->vmacSta_p->wdsPort[i].netDevWds->name);
					updateWds(wlpptr->vmacSta_p->wdsPort[i].netDevWds);
				}
			}
		}
	}
	else
	{
		MIB_802DOT11 *mib = wlpptr->vmacSta_p->ShadowMib802dot11;
		// If wds disabled disable any active ports.
		for (i = 0 ; i < MAX_WDS_PORT ; i++)
		{
			if (wlpptr->vmacSta_p->wdsActive[i])
			{
				if (wlpptr->vmacSta_p->wdsPort[i].netDevWds->flags & IFF_RUNNING)
				{
					wlstop_wds(wlpptr->vmacSta_p->wdsPort[i].netDevWds);
				}
				//wlDeleteWdsPortDynamic(&wlpptr->vmacSta_p->wdsPort[i].netDevWds);
				wlpptr->vmacSta_p->wdsActive[i] = FALSE;
				wlpptr->vmacSta_p->wdsPort[i].active = FALSE;
				memset(wlpptr->vmacSta_p->wdsPort[i].wdsMacAddr, 0, 6); // set WDS mac address to 0;
				memset(&mib->mib_WdsMacAddr[i][0], 0, 6);                
			}
			WLDBG_INFO(DBG_LEVEL_6, "Delete Mib wds Mac addr %x:%x:%x:%x:%x:%x\n",
				mib->mib_WdsMacAddr[i][0][0],
				mib->mib_WdsMacAddr[i][0][1],
				mib->mib_WdsMacAddr[i][0][2],
				mib->mib_WdsMacAddr[i][0][3],
				mib->mib_WdsMacAddr[i][0][4],
				mib->mib_WdsMacAddr[i][0][5]);


		}
	}
}

BOOLEAN wdsPortActive(struct net_device *netdev, UINT8 wdsIndex)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	return wlpptr->vmacSta_p->wdsActive[wdsIndex];
}

void wds_wlDeinit(struct net_device *netdev)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	UINT32 i;
	// Stop and remove any wds port devices.
	for (i = 0; i < MAX_WDS_PORT; i++)
	{         
		if (wlpptr->vmacSta_p->wdsPort[i].wdsPortRegistered)
		{
			if (wlpptr->vmacSta_p->wdsPort[i].netDevWds->flags & IFF_RUNNING)
			{
				wlstop_wds(wlpptr->vmacSta_p->wdsPort[i].netDevWds);
			}
			wlDeleteWdsPort(wlpptr->vmacSta_p->wdsPort[i].netDevWds);
			wlpptr->vmacSta_p->wdsActive[i] = FALSE;
			wlpptr->vmacSta_p->wdsPort[i].active = FALSE;
			wlpptr->vmacSta_p->wdsPort[i].wdsPortRegistered = FALSE;
		}
	}
}

#endif // WDS_FEATURE
