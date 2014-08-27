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



#include "wltypes.h"
#include "IEEE_types.h"
#include "osif.h"

#include "mib.h"
#include "wl_mib.h"
#include "wl_hal.h"
#include "qos.h"
#include "wlmac.h"
#include "ds.h"
#include "keyMgmtCommon.h"
#include "keyMgmt.h"
#include "tkip.h"
#include "StaDb.h"
#include "macmgmtap.h"
#include "wlvmac.h"

#include "bcngen.h"
#include "macMgmtMlme.h"
#include "Fragment.h"
#include "wl_macros.h"
#include "wpa.h"
#include "keyMgmtSta.h"

#include "mhsm.h"
#include "mlme.h"
#include "smeMain.h"
#include "wldebug.h"
#include "ap8xLnxIntf.h"
#include "ap8xLnxFwcmd.h"
#include "mlmeApi.h"
#include "idList.h"
#define	IEEE80211_ADDR_LEN	6
struct ieee80211_frame {
	u_int8_t	i_fc[2];
	u_int8_t	i_dur[2];
	u_int8_t	i_addr1[IEEE80211_ADDR_LEN];
	u_int8_t	i_addr2[IEEE80211_ADDR_LEN];
	u_int8_t	i_addr3[IEEE80211_ADDR_LEN];
	u_int8_t	i_seq[2];
	u_int8_t	i_addr4[IEEE80211_ADDR_LEN];
} PACK;



extern struct sk_buff *ieee80211_getmgtframe(UINT8 **frm, unsigned int pktlen);
extern void FixedRateCtl(extStaDb_StaInfo_t *pStaInfo, PeerInfo_t	*PeerInfo, MIB_802DOT11 *mib);


#ifdef AP_MAC_LINUX
struct sk_buff *mlmeApiPrepMgtMsg(UINT32 Subtype, IEEEtypes_MacAddr_t *DestAddr, IEEEtypes_MacAddr_t *SrcAddr)
#else
tx80211_MgmtMsg_t *mlmeApiPrepMgtMsg(UINT32 Subtype, IEEEtypes_MacAddr_t *DestAddr, IEEEtypes_MacAddr_t *SrcAddr)
#endif
{
	macmgmtQ_MgmtMsg2_t * MgmtMsg_p;
	//tx80211_MgmtMsg_t * TxMsg_p;
	//UINT32 size;
#ifdef AP_MAC_LINUX
	struct sk_buff *skb;
	UINT8 *frm;

	if ((skb = ieee80211_getmgtframe(&frm, sizeof(struct ieee80211_frame)+2)) != NULL)
	{
		//skb->len = 34;
		//skb->tail+= 34;
		WLDBG_INFO(DBG_LEVEL_8, "mlmeApiPrepMgtMsg length = %d \n", skb->len);
		MgmtMsg_p = (macmgmtQ_MgmtMsg2_t *) skb->data;
		MgmtMsg_p->Hdr.FrmCtl.Type = IEEE_TYPE_MANAGEMENT;
		MgmtMsg_p->Hdr.FrmCtl.Subtype = Subtype;
		MgmtMsg_p->Hdr.FrmCtl.Retry=0;
		MgmtMsg_p->Hdr.Duration = 300;
		memcpy(&MgmtMsg_p->Hdr.DestAddr, DestAddr, sizeof(IEEEtypes_MacAddr_t));
		memcpy(&MgmtMsg_p->Hdr.SrcAddr, SrcAddr, sizeof(IEEEtypes_MacAddr_t));
		memcpy(&MgmtMsg_p->Hdr.BssId, SrcAddr, sizeof(IEEEtypes_MacAddr_t));
	}

	return skb;
#else
#ifdef AP_BUFFER
	if ((TxMsg_p = (tx80211_MgmtMsg_t *)pool_GetBuf(txMgmtPoolId)) != NULL)
	{
		MgmtMsg_p = &TxMsg_p->MgmtFrame;
		MgmtMsg_p->Hdr.FrmCtl.Type = IEEE_TYPE_MANAGEMENT;
		MgmtMsg_p->Hdr.FrmCtl.Subtype = Subtype;
		MgmtMsg_p->Hdr.FrmCtl.Retry=0;
		MgmtMsg_p->Hdr.Duration = 300;
		memcpy(&MgmtMsg_p->Hdr.DestAddr, DestAddr, sizeof(IEEEtypes_MacAddr_t));
		memcpy(&MgmtMsg_p->Hdr.SrcAddr, SrcAddr, sizeof(IEEEtypes_MacAddr_t));
		memcpy(&MgmtMsg_p->Hdr.BssId, SrcAddr, sizeof(IEEEtypes_MacAddr_t));
	}
	return TxMsg_p;
#else
	return NULL;
#endif
#endif
}


#ifdef AP_MAC_LINUX
struct sk_buff *mlmeApiPrepMgtMsg2(UINT32 Subtype, IEEEtypes_MacAddr_t *DestAddr, IEEEtypes_MacAddr_t *SrcAddr, UINT16 size)
#else
tx80211_MgmtMsg_t *mlmeApiPrepMgtMsg2(UINT32 Subtype, IEEEtypes_MacAddr_t *DestAddr, IEEEtypes_MacAddr_t *SrcAddr, UINT16 size)
#endif
{
	macmgmtQ_MgmtMsg2_t * MgmtMsg_p;
	//tx80211_MgmtMsg_t * TxMsg_p;
	//UINT32 size;
#ifdef AP_MAC_LINUX
	struct sk_buff *skb;
	UINT8 *frm;

	if ((skb = ieee80211_getmgtframe(&frm, sizeof(struct ieee80211_frame)+size)) != NULL)
	{
		//skb->len = 34;
		//skb->tail+= 34;
		WLDBG_INFO(DBG_LEVEL_8, "mlmeApiPrepMgtMsg length = %d \n", skb->len);
		MgmtMsg_p = (macmgmtQ_MgmtMsg2_t *) skb->data;
		MgmtMsg_p->Hdr.FrmCtl.Type = IEEE_TYPE_MANAGEMENT;
		MgmtMsg_p->Hdr.FrmCtl.Subtype = Subtype;
		MgmtMsg_p->Hdr.FrmCtl.Retry=0;
		MgmtMsg_p->Hdr.Duration = 300;
		memcpy(&MgmtMsg_p->Hdr.DestAddr, DestAddr, sizeof(IEEEtypes_MacAddr_t));
		memcpy(&MgmtMsg_p->Hdr.SrcAddr, SrcAddr, sizeof(IEEEtypes_MacAddr_t));
		memcpy(&MgmtMsg_p->Hdr.BssId, SrcAddr, sizeof(IEEEtypes_MacAddr_t));
	}

	return skb;
#else
#ifdef AP_BUFFER
	if ((TxMsg_p = (tx80211_MgmtMsg_t *)pool_GetBuf(txMgmtPoolId)) != NULL)
	{
		MgmtMsg_p = &TxMsg_p->MgmtFrame;
		MgmtMsg_p->Hdr.FrmCtl.Type = IEEE_TYPE_MANAGEMENT;
		MgmtMsg_p->Hdr.FrmCtl.Subtype = Subtype;
		MgmtMsg_p->Hdr.FrmCtl.Retry=0;
		MgmtMsg_p->Hdr.Duration = 300;
		memcpy(&MgmtMsg_p->Hdr.DestAddr, DestAddr, sizeof(IEEEtypes_MacAddr_t));
		memcpy(&MgmtMsg_p->Hdr.SrcAddr, SrcAddr, sizeof(IEEEtypes_MacAddr_t));
		memcpy(&MgmtMsg_p->Hdr.BssId, SrcAddr, sizeof(IEEEtypes_MacAddr_t));
	}
	return TxMsg_p;
#else
	return NULL;
#endif
#endif
}

#ifdef CLIENT_SUPPORT
UINT8 setClientPeerInfo(vmacEntry_t  *vmacEntry_p, dot11MgtFrame_t *MgmtMsg_p, PeerInfo_t *pPeerInfo, extStaDb_StaInfo_t *StaInfo_p, UINT8 *QosInfo_p, MIB_802DOT11 *mib);
PeerInfo_t stationPeerInfo[NUM_OF_WLMACS];
extern int wlinitcnt;
void InitClientPeerInfo(struct net_device *netdev)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	vmacEntry_t *clientVMacEntry_p = (vmacEntry_t *) wlpptr->clntParent_priv_p; 

	// Set the Peer info pointer.
	sme_SetClientPeerInfo(clientVMacEntry_p, &stationPeerInfo[wlinitcnt]);
	return;
}



UINT8 initParentAp = 0;
UINT16 parentApAid = 0, parentApStnId = 0;
#ifdef V6FW
UINT16 ClientAid = 0, ClientStnId = 0;
#endif
void InitClientInfo(UINT8 *macAddr_p, dot11MgtFrame_t *MgmtMsg_p, vmacEntry_t *clientVMacEntry_p, BOOLEAN isApMrvl)
{

	struct net_device *pStaDev   = (struct net_device *) clientVMacEntry_p->privInfo_p;
	struct wlprivate  *wlpptrSta = NETDEV_PRIV_P(struct wlprivate, pStaDev);
	vmacApInfo_t *vmacSta_p = wlpptrSta->vmacSta_p;
	MIB_802DOT11 *mib = vmacSta_p->Mib802dot11;
	extStaDb_StaInfo_t *StaInfo = NULL;			
	extStaDb_StaInfo_t *StaInfo_p = NULL;
	extStaDb_StaInfo_t *StaInfoRmAp_p = NULL;
#ifdef V6FW
	extStaDb_StaInfo_t *StaInfoClient_p = NULL;
#endif 
	PeerInfo_t *pPeerInfo = NULL;
	//UINT32 Aid, StnId;
	UINT8 QosInfo = 0;
	UINT8 i = 0;
	STA_SECURITY_MIBS * pStaSecurityMibs = NULL;


	/*Use dynamic memory to prevent frame size > 1024bytes warning during compilation
	* extStaDb_StaInfo_t takes 1488bytes
	*/
	if((StaInfo = kmalloc(sizeof(extStaDb_StaInfo_t) ,GFP_KERNEL)) == NULL){
		printk("%s: Fail to allocate memory\n", __FUNCTION__);	
		return;
	}

	if (!initParentAp)
	{
		parentApAid   = AssignAid();
		parentApStnId = AssignStnId();
		initParentAp  = 1;
#ifdef V6FW
		ClientAid = AssignAid();
		ClientStnId = AssignStnId(); 
#endif
	}

	if ((StaInfoRmAp_p = extStaDb_GetStaInfo(wlpptrSta->vmacSta_p,(IEEEtypes_MacAddr_t *)macAddr_p, 1)) == NULL)
	{   
		memset(StaInfo, 0, sizeof(extStaDb_StaInfo_t));
		memcpy(&StaInfo->Addr, macAddr_p, sizeof(IEEEtypes_MacAddr_t));
		StaInfo->StnId = parentApStnId;
		StaInfo->Aid = parentApAid;
		StaInfo->AP = FALSE;
		StaInfo->Client = TRUE;
		StaInfo->mib_p = wlpptrSta->vmacSta_p->Mib802dot11;
		StaInfo->dev = pStaDev;
		extStaDb_AddSta(wlpptrSta->vmacSta_p, StaInfo);
		
		if ((StaInfoRmAp_p = extStaDb_GetStaInfo(wlpptrSta->vmacSta_p,(IEEEtypes_MacAddr_t *)macAddr_p, 1)) == NULL)
		{
			printk("InitClientInfo: ERROR - cannot add host Client Remote AP to station database. \n");
			kfree(StaInfo);		
			return;
		}
	}
	StaInfo_p = StaInfoRmAp_p;

#ifdef V6FW
	if ((StaInfoClient_p = extStaDb_GetStaInfo(wlpptrSta->vmacSta_p,(IEEEtypes_MacAddr_t *)MgmtMsg_p->Hdr.DestAddr, 1)) == NULL)
	{       
		memset(StaInfo, 0, sizeof(extStaDb_StaInfo_t));	
		memcpy(&StaInfo->Addr, MgmtMsg_p->Hdr.DestAddr, sizeof(IEEEtypes_MacAddr_t));
		StaInfo->StnId = ClientStnId;
		StaInfo->Aid = ClientAid;
		StaInfo->AP = FALSE;
		StaInfo->Client = TRUE;
		StaInfo->mib_p = wlpptrSta->vmacSta_p->Mib802dot11;
		StaInfo->dev = pStaDev;
		extStaDb_AddSta(wlpptrSta->vmacSta_p, StaInfo);
		
		if ((StaInfoClient_p = extStaDb_GetStaInfo(wlpptrSta->vmacSta_p,(IEEEtypes_MacAddr_t *)MgmtMsg_p->Hdr.DestAddr, 1)) == NULL)
		{
			printk("InitClientInfo: ERROR - Cannot add Host Client MAC to station database. \n");
			kfree(StaInfo);		
			return;
		}
	}
#endif
	kfree(StaInfo); 	
	
	// Set the Peer info.
	pPeerInfo = sme_GetClientPeerInfo(clientVMacEntry_p);
	memset(pPeerInfo, 0, sizeof(PeerInfo_t));
	setClientPeerInfo(clientVMacEntry_p, MgmtMsg_p, pPeerInfo, StaInfo_p, &QosInfo, mib);
#ifdef V6FW
	setClientPeerInfo(clientVMacEntry_p, MgmtMsg_p, pPeerInfo, StaInfoClient_p, &QosInfo, mib);
#endif

#ifdef AMPDU_SUPPORT
	free_any_pending_ampdu_pck(pStaDev,StaInfo_p->Aid);
	for(i=0;i<3;i++)  /** Reset the ampdu reorder pck anyway **/
		wlpptrSta->wlpd_p->AmpduPckReorder[StaInfo_p->Aid].AddBaReceive[i]=FALSE;  /** clear Ba flag **/
#ifdef V6FW
	free_any_pending_ampdu_pck(pStaDev,StaInfoClient_p->Aid);
	for(i=0;i<3;i++)  /** Reset the ampdu reorder pck anyway **/
		wlpptrSta->wlpd_p->AmpduPckReorder[StaInfoClient_p->Aid].AddBaReceive[i]=FALSE;  /** clear Ba flag **/
#endif

	if (*wlpptrSta->vmacSta_p->Mib802dot11->mib_AmpduTx)
	{
		memset(&StaInfo_p->aggr11n.startbytid[0], 0, 8);
		memset(&StaInfo_p->aggr11n.onbytid[0], 0, 8);					
		StaInfo_p->aggr11n.type &= ~WL_WLAN_TYPE_AMPDU;
#ifdef V6FW
		memset(&StaInfoClient_p->aggr11n.startbytid[0], 0, 8);
		memset(&StaInfoClient_p->aggr11n.onbytid[0], 0, 8);					
		StaInfoClient_p->aggr11n.type &= ~WL_WLAN_TYPE_AMPDU;
#endif
		AddHT_IE(vmacSta_p,&StaInfo_p->HtElem);
	}
#endif

	if ((*(wlpptrSta->vmacSta_p->Mib802dot11->pMib_11nAggrMode)&WL_MODE_AMSDU_TX_MASK )== 0)
	{
		/* if AMSDU disabled locally then setup stainfo accordingly */
		StaInfo_p->aggr11n.threshold = 0;
		StaInfo_p->aggr11n.cap = 0;
#ifdef V6FW
		StaInfoClient_p->aggr11n.threshold = 0;
		StaInfoClient_p->aggr11n.cap = 0;
#endif
	}
	else if (*(wlpptrSta->vmacSta_p->Mib802dot11->pMib_11nAggrMode)& WL_MODE_AMSDU_TX_4K)
	{
		/* if AMSDU set to 4k locally then setup stainfo as per this*/
		StaInfo_p->aggr11n.cap = 1;
		if(!(*(wlpptrSta->vmacSta_p->Mib802dot11->pMib_11nAggrMode) & WL_MODE_AMPDU_TX))
		{
			StaInfo_p->aggr11n.type |= WL_WLAN_TYPE_AMSDU;
		}
#ifdef V6FW
		StaInfoClient_p->aggr11n.cap = 1;
		if(!(*(wlpptrSta->vmacSta_p->Mib802dot11->pMib_11nAggrMode) & WL_MODE_AMPDU_TX))
			StaInfoClient_p->aggr11n.type |= WL_WLAN_TYPE_AMSDU;
#endif
	}

	if (mib->Privacy->RSNEnabled && !mib->RSNConfigWPA2->WPA2OnlyEnabled)
	{            
		pStaSecurityMibs = sme_GetStaSecurityMibsPtr(clientVMacEntry_p);
		if ((pStaSecurityMibs->thisStaRsnIE_p->ElemId == 221 || pStaSecurityMibs->thisStaRsnIE_p->ElemId == 48)
			&&(pStaSecurityMibs->thisStaRsnIE_p->PwsKeyCipherList[3] == 2))
		{   
			// TKIP disable non -Marvell AP aggregation
			if(!isApMrvl)
			{
				StaInfo_p->aggr11n.threshold = 0;
				StaInfo_p->aggr11n.thresholdBackUp = StaInfo_p->aggr11n.threshold;
#ifdef V6FW
				StaInfoClient_p->aggr11n.threshold = 0;
				StaInfoClient_p->aggr11n.thresholdBackUp = StaInfo_p->aggr11n.threshold;
#endif
			}
		}
	}

	if(!isApMrvl && mib->Privacy->PrivInvoked)
	{
		StaInfo_p->aggr11n.threshold = 0;
		StaInfo_p->aggr11n.thresholdBackUp = StaInfo_p->aggr11n.threshold;
#ifdef V6FW
		StaInfoClient_p->aggr11n.threshold = 0;
		StaInfoClient_p->aggr11n.thresholdBackUp = StaInfo_p->aggr11n.threshold;
#endif
	}

	/* First remove station even if not added to station database. */
	/* Remove AP entry into FW STADB */        
	wlFwSetNewStn(pStaDev,(u_int8_t *)macAddr_p, parentApAid, parentApStnId, 2, pPeerInfo, QosInfo, StaInfo_p->IsStaQSTA);
#ifdef V6FW
	wlFwSetNewStn(pStaDev,(u_int8_t *)MgmtMsg_p->Hdr.DestAddr, ClientAid, ClientStnId, 2, pPeerInfo, QosInfo, StaInfoClient_p->IsStaQSTA);
#endif

	/* Add AP entry into FW STADB */     
	wlFwSetNewStn(pStaDev,(u_int8_t *)macAddr_p, parentApAid, parentApStnId, 0, pPeerInfo, QosInfo, StaInfo_p->IsStaQSTA);
#ifdef V6FW
	wlFwSetNewStn(pStaDev,(u_int8_t *)MgmtMsg_p->Hdr.DestAddr, ClientAid, ClientStnId, 0, pPeerInfo, QosInfo, StaInfoClient_p->IsStaQSTA);
#endif

	wlFwSetSecurity(pStaDev,macAddr_p);
#ifdef V6FW
	wlFwSetSecurity(pStaDev,(u_int8_t *)MgmtMsg_p->Hdr.DestAddr);
#endif

	return;
}	


void RemoveRemoteAPFw(UINT8 *apMacAddr_p, vmacEntry_t *clientVMacEntry_p)
{
	struct net_device *pStaDev   = (struct net_device *) clientVMacEntry_p->privInfo_p;
	struct wlprivate  *wlpptrSta = NETDEV_PRIV_P(struct wlprivate, pStaDev);
	extStaDb_StaInfo_t *StaInfo_p = NULL;

	if (!clientVMacEntry_p->active)
		wlFwRemoveMacAddr(pStaDev, &clientVMacEntry_p->vmacAddr[0]);			

	if ((StaInfo_p = extStaDb_GetStaInfo(wlpptrSta->vmacSta_p,(IEEEtypes_MacAddr_t *)apMacAddr_p, 1)) != NULL)
	{
		extStaDb_DelSta(wlpptrSta->vmacSta_p,(IEEEtypes_MacAddr_t *)apMacAddr_p, 0);
		wlFwSetNewStn(wlpptrSta->master,(u_int8_t *)apMacAddr_p, StaInfo_p->Aid, StaInfo_p->StnId, 2, NULL, 0, 0);
	}

	return;
}
#endif
UINT8 setClientPeerInfo(vmacEntry_t  *vmacEntry_p,
						dot11MgtFrame_t *MgmtMsg_p,
						PeerInfo_t *pPeerInfo,
						extStaDb_StaInfo_t *StaInfo_p,
						UINT8 *QosInfo_p,
						MIB_802DOT11 *mib)
{
	UINT8 *attrib_p;
	WME_param_elem_t *WME_param_elem = NULL;
	//PeerInfo_t PeerInfo, *pPeerInfo;
	IEEEtypes_SuppRatesElement_t    *PeerSupportedRates_p;
	IEEEtypes_ExtSuppRatesElement_t *PeerExtSupportedRates_p;
	IEEEtypes_HT_Element_t          *pHT;
	IEEEtypes_Add_HT_Element_t      *pHTAdd;
	IEEEtypes_Generic_HT_Element_t  *pHTGen;
	IEEEtypes_Generic_Add_HT_Element_t *pHTAddGen;
	MIB_802DOT11 *pMasterMIB;
#ifdef SOC_W8864
    IEEEtypes_VhtCap_t *pVhtCap;
	IEEEtypes_VhOpt_t *pVhtOp;		
	UINT32 vhtcap=0;	
#endif

	struct net_device *dev = (struct net_device *)vmacEntry_p->privInfo_p;
	struct wlprivate *priv = NETDEV_PRIV_P(struct wlprivate, dev);
	vmacApInfo_t *vmacSta_p = priv->vmacSta_p;
	pMasterMIB = vmacSta_p->master->Mib802dot11;

	StaInfo_p->ClientMode = BONLY_MODE;
	StaInfo_p->aggr11n.cap = 0;
	StaInfo_p->aggr11n.threshold = 0;
	StaInfo_p->IsStaQSTA = 0;

	memset((void *)pPeerInfo, 0, sizeof(PeerInfo_t));
	pPeerInfo->CapInfo = MgmtMsg_p->Body.AssocRsp.CapInfo;
	attrib_p = (UINT8 *) &MgmtMsg_p->Body.AssocRsp.AId;
	attrib_p += sizeof(IEEEtypes_AId_t);

	PeerSupportedRates_p = (IEEEtypes_SuppRatesElement_t*) syncSrv_ParseAttribWithinFrame( MgmtMsg_p, attrib_p, SUPPORTED_RATES);

	PeerExtSupportedRates_p = (IEEEtypes_ExtSuppRatesElement_t *) syncSrv_ParseAttribWithinFrame( MgmtMsg_p, attrib_p, EXT_SUPPORTED_RATES);

	if (PeerSupportedRates_p && PeerExtSupportedRates_p)
		StaInfo_p->ClientMode = GONLY_MODE;

	pPeerInfo->LegacyRateBitMap = GetAssocRespLegacyRateBitMap(PeerSupportedRates_p, PeerExtSupportedRates_p);
	if (!(pPeerInfo->LegacyRateBitMap & 0x0f))
		StaInfo_p->ClientMode = AONLY_MODE;

	if ((pHT = (IEEEtypes_HT_Element_t *) syncSrv_ParseAttribWithinFrame( MgmtMsg_p, attrib_p, HT)))
	{
		pPeerInfo->HTCapabilitiesInfo = pHT->HTCapabilitiesInfo;
		pPeerInfo->MacHTParamInfo     = pHT->MacHTParamInfo;
		pPeerInfo->HTRateBitMap = (pHT->SupportedMCSset[0] | (pHT->SupportedMCSset[1] << 8) |
			(pHT->SupportedMCSset[2] << 16) | (pHT->SupportedMCSset[3] << 24));
		pPeerInfo->TxBFCapabilities = pHT->TxBFCapabilities;
		StaInfo_p->ClientMode = NONLY_MODE;
		StaInfo_p->aggr11n.threshold = 4;
		StaInfo_p->aggr11n.thresholdBackUp = StaInfo_p->aggr11n.threshold;
		if((*(pMasterMIB->pMib_11nAggrMode)& WL_MODE_AMSDU_TX_MASK) && !(*(pMasterMIB->pMib_11nAggrMode)& WL_MODE_AMPDU_TX))
			StaInfo_p->aggr11n.type |= WL_WLAN_TYPE_AMSDU;
		if (pHT->HTCapabilitiesInfo.MaxAMSDUSize)
			StaInfo_p->aggr11n.cap = 2;
		else
			StaInfo_p->aggr11n.cap = 1;
		// Green Field not supported.
		pPeerInfo->HTCapabilitiesInfo.GreenField   = 0;
		pPeerInfo->HTCapabilitiesInfo.RxSTBC = 0;
		pPeerInfo->HTCapabilitiesInfo.TxSTBC = 0;
	}

	if ((pHTAdd = (IEEEtypes_Add_HT_Element_t *) syncSrv_ParseAttribWithinFrame( MgmtMsg_p, attrib_p, ADD_HT)))
	{
		pPeerInfo->AddHtInfo.ControlChan     = pHTAdd->ControlChan;
		pPeerInfo->AddHtInfo.AddChan         = pHTAdd->AddChan;
		pPeerInfo->AddHtInfo.OpMode          = pHTAdd->OpMode;
		pPeerInfo->AddHtInfo.stbc            = pHTAdd->stbc;
	}
#ifdef SOC_W8864
    if((pVhtCap = (IEEEtypes_VhtCap_t *)syncSrv_ParseAttribWithinFrame( MgmtMsg_p, attrib_p, VHT_CAP)))
    {
    	vhtcap = *((UINT32*)&pVhtCap->cap);		
        printk("VHT AP: vht_cap=%x, vht_RxMcs=%x, vht_TxMcs=%x\n", 
			(u_int32_t)vhtcap, (u_int32_t)pVhtCap->SupportedRxMcsSet, 
			(u_int32_t)pVhtCap->SupportedTxMcsSet);		
        
		memcpy((UINT8 *)&pPeerInfo->vht_cap, (UINT8 *)&pVhtCap->cap, sizeof(IEEEtypes_VHT_Cap_Info_t));		
        pPeerInfo->vht_MaxRxMcs = pVhtCap->SupportedRxMcsSet;
		
		if((pVhtOp = (IEEEtypes_VhOpt_t *)syncSrv_ParseAttribWithinFrame( MgmtMsg_p, attrib_p, VHT_OPERATION)))
		{
			switch(pVhtOp->ch_width)
			{
				case 0:
					if(pHTAdd && pHTAdd->AddChan.STAChannelWidth == 1)
						pPeerInfo->vht_RxChannelWidth = 1;
					else
						pPeerInfo->vht_RxChannelWidth = 0;
					break;
				case 1:
					pPeerInfo->vht_RxChannelWidth = 2;
					break;
				case 2:
				case 3:
					pPeerInfo->vht_RxChannelWidth = 3;
					break;
				default:
					pPeerInfo->vht_RxChannelWidth = 2;
					break;
			}
		}
		else
			pPeerInfo->vht_RxChannelWidth = 2;
		
    }
#endif
	// Looks up Qos element.  This will be updated for Client station database entry.
	while((attrib_p = (UINT8 *) syncSrv_ParseAttribWithinFrame( MgmtMsg_p, attrib_p, PROPRIETARY_IE))!=NULL)
	{			
		WME_param_elem = (WME_param_elem_t *) attrib_p;
		pHTGen         = (IEEEtypes_Generic_HT_Element_t *) attrib_p;
		pHTAddGen      = (IEEEtypes_Generic_Add_HT_Element_t *) attrib_p;
		//check if it is a WME/WSM Info Element.
		if(!memcmp(WME_param_elem->OUI.OUI, WiFiOUI, 3))
		{
			//Check if it is a WME element
			if(WME_param_elem->OUI.Type==2)
			{
				//check if it is a WME Param Element
				if(WME_param_elem->OUI.Subtype==1)
				{
					AC_param_rcd_t *ac_Param_p = &WME_param_elem->AC_BE;
					UINT32 i;
					StaInfo_p->IsStaQSTA = TRUE;
					memcpy(QosInfo_p,&WME_param_elem->QoS_info, 1);
					/* Program queues with Qos settings. */
					for (i = 0; i < 4 ; i++)
					{
						/* If high performance mode use VI values for CWmin, AIFSN, and TXOP. */
						if (*(pMasterMIB->mib_optlevel) && i == 0)
						{
							wlFwSetEdcaParam(mainNetdev_p[vmacEntry_p->phyHwMacIndx], 
								i, 
								((0x01 << WME_param_elem->AC_VI.ECW_min_max.ECW_min)-1), 
								((0x01 << ac_Param_p->ECW_min_max.ECW_max)-1), 
								WME_param_elem->AC_VI.ACI_AIFSN.AIFSN,  
								WME_param_elem->AC_VI.TXOP_lim);
						}
						else
						{
							wlFwSetEdcaParam(mainNetdev_p[vmacEntry_p->phyHwMacIndx], 
								i, 
								((0x01 << ac_Param_p->ECW_min_max.ECW_min)-1), 
								((0x01 << ac_Param_p->ECW_min_max.ECW_max)-1), 
								ac_Param_p->ACI_AIFSN.AIFSN,  
								ac_Param_p->TXOP_lim);
						}
						ac_Param_p++;
					}
				}
			}
		}
		else if ((pHT == NULL) && !memcmp(&pHTGen->OUI, B_COMP_OUI,3))
		{
			/* Look up high throughput elements using proprietary tag if not found with with HT tag. */
			if (pHTGen->OUIType == HT_PROP)
			{
				pPeerInfo->HTCapabilitiesInfo = pHTGen->HTCapabilitiesInfo;
				pPeerInfo->MacHTParamInfo     = pHTGen->MacHTParamInfo;
				pPeerInfo->HTRateBitMap = (pHTGen->SupportedMCSset[0] | (pHTGen->SupportedMCSset[1] << 8) |
					(pHTGen->SupportedMCSset[2] << 16) | (pHTGen->SupportedMCSset[3] << 24));

				StaInfo_p->ClientMode = NONLY_MODE;
				StaInfo_p->aggr11n.threshold = 4;
				StaInfo_p->aggr11n.thresholdBackUp = StaInfo_p->aggr11n.threshold;
				if((*(pMasterMIB->pMib_11nAggrMode)& WL_MODE_AMSDU_TX_MASK) && !(*(pMasterMIB->pMib_11nAggrMode)& WL_MODE_AMPDU_TX))
					StaInfo_p->aggr11n.type |= WL_WLAN_TYPE_AMSDU; 
				if (pHTGen->HTCapabilitiesInfo.MaxAMSDUSize)
					StaInfo_p->aggr11n.cap = 2;
				else
					StaInfo_p->aggr11n.cap = 1;

				// Green Field not supported.
				pPeerInfo->HTCapabilitiesInfo.GreenField   = 0;
				pPeerInfo->HTCapabilitiesInfo.RxSTBC = 0;
				pPeerInfo->HTCapabilitiesInfo.TxSTBC = 0;
			}
			else if (pHTAddGen->OUIType == ADD_HT_PROP)
			{
				pPeerInfo->AddHtInfo.ControlChan     = pHTAddGen->ControlChan;
				pPeerInfo->AddHtInfo.AddChan         = pHTAddGen->AddChan;
				pPeerInfo->AddHtInfo.OpMode          = pHTAddGen->OpMode;
				pPeerInfo->AddHtInfo.stbc            = pHTAddGen->stbc;
			}
		}
		//Now process to the next element pointer.
		attrib_p += (2 + *((UINT8 *)(attrib_p + 1)));
	}
	/* Check if QOS set, if not then set to best effort if N mode */
	if (!StaInfo_p->IsStaQSTA && (StaInfo_p->ClientMode == NONLY_MODE))
	{
		StaInfo_p->IsStaQSTA = TRUE;
		*QosInfo_p = 0;
		/* Leave queue Qos settings default. */
	}
	FixedRateCtl(StaInfo_p, pPeerInfo, mib);
	return TRUE;
}


UINT32 GetAssocRespLegacyRateBitMap(IEEEtypes_SuppRatesElement_t *SuppRates, IEEEtypes_ExtSuppRatesElement_t *ExtSuppRates)
{
	UINT16 i = 0, j = 0;
	UINT8  Rates[32] = {0};    
	UINT32 SupportedLegacyIEEERateBitMap = 0;

	/* Get legacy rates */    
	if (SuppRates)
	{
		for ( i = 0; i < SuppRates->Len; i++ )
		{
			Rates[i] = SuppRates->Rates[i];    
		}
	}
	if (ExtSuppRates)
	{
		if ( ExtSuppRates && ExtSuppRates->Len )
		{
			for ( j = 0; j < ExtSuppRates->Len; j++)
			{
				Rates[i+j] = ExtSuppRates->Rates[j];
			}
		}
	}

	/* Get legacy rate bit map */
	for( i = 0; i <IEEEtypes_MAX_DATA_RATES_G; i++)
	{
		IEEEToMrvlRateBitMapConversion(Rates[i], &SupportedLegacyIEEERateBitMap); 
	}
	return SupportedLegacyIEEERateBitMap; 
}

#if 1//def CLIENT_SUPPORT_MULTIPLECLIENT
void DeleteClientInfo(UINT8 *macAddr_p, vmacEntry_t *clientVMacEntry_p)
{
	struct net_device *pStaDev   = (struct net_device *) clientVMacEntry_p->privInfo_p;
	struct wlprivate  *wlpptrSta = NETDEV_PRIV_P(struct wlprivate, pStaDev);    
	extStaDb_StaInfo_t *StaInfo_p = NULL;


	if ((StaInfo_p = extStaDb_GetStaInfo(wlpptrSta->vmacSta_p,(IEEEtypes_MacAddr_t *)macAddr_p, 1)) != NULL)
	{
		FreeAid(StaInfo_p->Aid);
		ResetAid(StaInfo_p->StnId, StaInfo_p->Aid);
		FreeStnId(StaInfo_p->StnId);
		extStaDb_DelSta(wlpptrSta->vmacSta_p,(IEEEtypes_MacAddr_t *)macAddr_p, 0);
		wlFwSetNewStn(wlpptrSta->master,(u_int8_t *)macAddr_p, StaInfo_p->Aid, StaInfo_p->StnId, 2, NULL,0, 0);
		//only remove local address, when there is a BSSID entry.
		extStaDb_DelSta(wlpptrSta->vmacSta_p,(IEEEtypes_MacAddr_t *)clientVMacEntry_p->vmacAddr, 0);
	}
}
#endif /*CLIENT_SUPPORT_MULTIPLECLIENT*/

int wlset_mibChannel(vmacEntry_t *clientVMacEntry_p, UINT8 mib_STAMode)
{
	int rc = 0;
	//not sure the purpose of changing wdev0's mib, TODO
#ifndef MBSS
	mibMaster = vmacSta_p->master->ShadowMib802dot11;
	if ((mib_STAMode == CLIENT_MODE_N_5) || (mib_STAMode == CLIENT_MODE_A) ||(mib_STAMode == CLIENT_MODE_AUTO))
	{
		channel = 64;
#ifndef SOC_W8864		
		*mibMaster->mib_ApMode = AP_MODE_AandN;
#else		
		*mibMaster->mib_ApMode = AP_MODE_5GHZ_Nand11AC;
#endif		
	}
	else
	{
		channel = 6;
#ifndef SOC_W8864		
		*mibMaster->mib_ApMode = AP_MODE_BandGandN;
#else		
		*mibMaster->mib_ApMode = AP_MODE_2_4GHZ_11AC_MIXED;
#endif		
	}

	//#ifdef BRS_SUPPORT
	switch (*(mibMaster->mib_ApMode))
	{
	case AP_MODE_B_ONLY:
		*(mibMaster->BssBasicRateMask) = MRVL_BSSBASICRATEMASK_B;
		*(mibMaster->NotBssBasicRateMask) = MRVL_NOTBSSBASICRATEMASK_B;
		break;

	case AP_MODE_G_ONLY:
		*(mibMaster->BssBasicRateMask) = MRVL_BSSBASICRATEMASK_G;
		*(mibMaster->NotBssBasicRateMask)  = MRVL_NOTBSSBASICRATEMASK_G;
		break;

	case AP_MODE_A_ONLY:
	case AP_MODE_AandN:
	case AP_MODE_5GHZ_N_ONLY:
#ifdef SOC_W8864	
	case AP_MODE_5GHZ_11AC_ONLY:
	case AP_MODE_5GHZ_Nand11AC:
#endif	
		*(mibMaster->BssBasicRateMask) = MRVL_BSSBASICRATEMASK_A;
		*(mibMaster->NotBssBasicRateMask)  = MRVL_NOTBSSBASICRATEMASK_A;
		break;

	case AP_MODE_N_ONLY:
	case AP_MODE_MIXED:
#ifdef SOC_W8864	
	case AP_MODE_2_4GHZ_11AC_MIXED:
#endif	
	default:
		*(mibMaster->BssBasicRateMask) = MRVL_BSSBASICRATEMASK_BGN;
		*(mibMaster->NotBssBasicRateMask)  = MRVL_NOTBSSBASICRATEMASK_BGN;
		break;
	}
	//#endif

	if(domainChannelValid(channel, channel <= 14?FREQ_BAND_2DOT4GHZ:FREQ_BAND_5GHZ))
	{
		PhyDSSSTable->CurrChan = channel;
		/* Currentlly, 40M is not supported for channel 14 */
		if (PhyDSSSTable->CurrChan == 14)
		{
			if ((PhyDSSSTable->Chanflag.ChnlWidth == CH_AUTO_WIDTH) || 
				(PhyDSSSTable->Chanflag.ChnlWidth == CH_40_MHz_WIDTH))
				PhyDSSSTable->Chanflag.ChnlWidth = CH_20_MHz_WIDTH;
		}

		//PhyDSSSTable->Chanflag.ChnlWidth=CH_40_MHz_WIDTH;
		PhyDSSSTable->Chanflag.ExtChnlOffset=EXT_CH_ABOVE_CTRL_CH;
		if(((PhyDSSSTable->Chanflag.ChnlWidth==CH_40_MHz_WIDTH) || (PhyDSSSTable->Chanflag.ChnlWidth==CH_AUTO_WIDTH)))
		{
			switch(PhyDSSSTable->CurrChan)
			{
			case 1:
			case 2:
			case 3:
			case 4:
				PhyDSSSTable->Chanflag.ExtChnlOffset=EXT_CH_ABOVE_CTRL_CH;
				break;
			case 5:
				/* Now AutoBW use 5-1 instead of 5-9 for wifi cert convenience*/
				/*if(*mib_extSubCh_p==0)
					PhyDSSSTable->Chanflag.ExtChnlOffset=EXT_CH_ABOVE_CTRL_CH;
				else if(*mib_extSubCh_p==1)
					PhyDSSSTable->Chanflag.ExtChnlOffset=EXT_CH_BELOW_CTRL_CH;
				else if(*mib_extSubCh_p==2)
					PhyDSSSTable->Chanflag.ExtChnlOffset=EXT_CH_ABOVE_CTRL_CH;
				break;*/
			case 6:
			case 7:
			case 8:
			case 9:
			case 10:
				if(*mib_extSubCh_p==0)
					PhyDSSSTable->Chanflag.ExtChnlOffset=EXT_CH_BELOW_CTRL_CH;
				else if(*mib_extSubCh_p==1)
					PhyDSSSTable->Chanflag.ExtChnlOffset=EXT_CH_BELOW_CTRL_CH;
				else if(*mib_extSubCh_p==2)
					PhyDSSSTable->Chanflag.ExtChnlOffset=EXT_CH_ABOVE_CTRL_CH;
				break;
			case 11:
			case 12:
			case 13:
			case 14:
				PhyDSSSTable->Chanflag.ExtChnlOffset=EXT_CH_BELOW_CTRL_CH;
				break;
				/* for 5G */
			case 36:
				PhyDSSSTable->Chanflag.ExtChnlOffset=EXT_CH_ABOVE_CTRL_CH;
				break;
			case 40:
				PhyDSSSTable->Chanflag.ExtChnlOffset=EXT_CH_BELOW_CTRL_CH;
				break;
			case 44:
				PhyDSSSTable->Chanflag.ExtChnlOffset=EXT_CH_ABOVE_CTRL_CH;
				break;
			case 48:
				PhyDSSSTable->Chanflag.ExtChnlOffset=EXT_CH_BELOW_CTRL_CH;
				break;
			case 52:
				PhyDSSSTable->Chanflag.ExtChnlOffset=EXT_CH_ABOVE_CTRL_CH;
				break;
			case 56:
				PhyDSSSTable->Chanflag.ExtChnlOffset=EXT_CH_BELOW_CTRL_CH;
				break;
			case 60:
				PhyDSSSTable->Chanflag.ExtChnlOffset=EXT_CH_ABOVE_CTRL_CH;
				break;
			case 64:
				PhyDSSSTable->Chanflag.ExtChnlOffset=EXT_CH_BELOW_CTRL_CH;
				break;

			case 100:
				PhyDSSSTable->Chanflag.ExtChnlOffset=EXT_CH_ABOVE_CTRL_CH;
				break;
			case 104:
				PhyDSSSTable->Chanflag.ExtChnlOffset=EXT_CH_BELOW_CTRL_CH;
				break;
			case 108:
				PhyDSSSTable->Chanflag.ExtChnlOffset=EXT_CH_ABOVE_CTRL_CH;
				break;
			case 112:
				PhyDSSSTable->Chanflag.ExtChnlOffset=EXT_CH_BELOW_CTRL_CH;
				break;
			case 116:
				PhyDSSSTable->Chanflag.ExtChnlOffset=EXT_CH_ABOVE_CTRL_CH;
				break;
			case 120:
				PhyDSSSTable->Chanflag.ExtChnlOffset=EXT_CH_BELOW_CTRL_CH;
				break;
			case 124:
				PhyDSSSTable->Chanflag.ExtChnlOffset=EXT_CH_ABOVE_CTRL_CH;
				break;
			case 128:
				PhyDSSSTable->Chanflag.ExtChnlOffset=EXT_CH_BELOW_CTRL_CH;
				break;
			case 132:
				PhyDSSSTable->Chanflag.ExtChnlOffset=EXT_CH_ABOVE_CTRL_CH;
				break;
			case 136:
				PhyDSSSTable->Chanflag.ExtChnlOffset=EXT_CH_BELOW_CTRL_CH;
				break;
			case 140:
				PhyDSSSTable->Chanflag.ExtChnlOffset=EXT_CH_ABOVE_CTRL_CH;
				break;

			case 149:
				PhyDSSSTable->Chanflag.ExtChnlOffset=EXT_CH_ABOVE_CTRL_CH;
				break;
			case 153:
				PhyDSSSTable->Chanflag.ExtChnlOffset=EXT_CH_BELOW_CTRL_CH;
				break;
			case 157:
				PhyDSSSTable->Chanflag.ExtChnlOffset=EXT_CH_ABOVE_CTRL_CH;
				break;
			case 161:
				PhyDSSSTable->Chanflag.ExtChnlOffset=EXT_CH_BELOW_CTRL_CH;
				break;
			}
		}
		if(PhyDSSSTable->CurrChan<=14)
			PhyDSSSTable->Chanflag.FreqBand=FREQ_BAND_2DOT4GHZ;
		else
			PhyDSSSTable->Chanflag.FreqBand=FREQ_BAND_5GHZ;
	}else
		WLDBG_INFO(DBG_LEVEL_1,  "invalid channel %d\n", channel);
#endif
	return rc;
}

void RemoveClientFw(UINT8 *macAddr_p, vmacEntry_t *clientVMacEntry_p)
{
	struct net_device *pStaDev   = (struct net_device *) clientVMacEntry_p->privInfo_p;
	struct wlprivate  *wlpptrSta = NETDEV_PRIV_P(struct wlprivate, pStaDev);    
	extStaDb_StaInfo_t *StaInfo_p = NULL;


	if ((StaInfo_p = extStaDb_GetStaInfo(wlpptrSta->vmacSta_p,(IEEEtypes_MacAddr_t *)macAddr_p, 1)) != NULL)
	{
		FreeAid(StaInfo_p->Aid);
		ResetAid(StaInfo_p->StnId, StaInfo_p->Aid);
		FreeStnId(StaInfo_p->StnId);
		extStaDb_DelSta(wlpptrSta->vmacSta_p,(IEEEtypes_MacAddr_t *)macAddr_p, 0);
		wlFwSetNewStn(wlpptrSta->master,(u_int8_t *)macAddr_p, StaInfo_p->Aid, StaInfo_p->StnId, 2, NULL,0, 0);

	}
	else
	{
		wlFwSetNewStn(wlpptrSta->master,(u_int8_t *)macAddr_p, 0, 0, 2, NULL,0, 0);
	}
}

