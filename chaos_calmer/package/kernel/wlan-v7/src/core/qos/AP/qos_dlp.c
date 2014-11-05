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


#ifdef QOS_FEATURE

#include "ap8xLnxIntf.h"

#include "wltypes.h"
#include "IEEE_types.h"
#include "mib.h"
#include "wl_mib.h"
#include "wl_hal.h"
#include "qos.h"
#include "ds.h"
#include "osif.h"
#include "keyMgmtCommon.h"
#include "keyMgmt.h"
#include "tkip.h"
#include "StaDb.h"
#include "macmgmtap.h"
#include "wlmac.h"


#ifdef AP_MAC_LINUX
extern struct sk_buff *mlmeApiPrepMgtMsg(UINT32 Subtype, IEEEtypes_MacAddr_t *DestAddr, IEEEtypes_MacAddr_t *SrcAddr);
#else
extern tx80211_MgmtMsg_t *mlmeApiPrepMgtMsg(UINT32 Subtype, IEEEtypes_MacAddr_t *DestAddr, IEEEtypes_MacAddr_t *SrcAddr);
#endif


void ProcessDlpReq(vmacApInfo_t *vmacSta_p, macmgmtQ_MgmtMsg_t *pMgmtMsg)
{
	MIB_STA_CFG  *mib_StaCfg_p=vmacSta_p->Mib802dot11->StationConfig;
	IEEEtypes_DlpReq_t  *pDlpReq;
	extStaDb_StaInfo_t  *pStaInfo;
#ifdef AP_MAC_LINUX
	struct sk_buff *txSkb_p;
#endif

	pDlpReq = &pMgmtMsg->Body.DlpReq;

	/* Allocate space for response message */
#ifdef AP_MAC_LINUX
	if ((txSkb_p = mlmeApiPrepMgtMsg(IEEE_MSG_QOS_ACTION, (IEEEtypes_MacAddr_t *)pDlpReq->DstAddr, &vmacSta_p->macStaAddr)) == NULL)
		return;
	//TxMsg_p = (tx80211_MgmtMsg_t *) txSkb_p->data;
#else
	if ((TxMsg_p = mlmeApiPrepMgtMsg(IEEE_MSG_QOS_ACTION, (IEEEtypes_MacAddr_t *)pDlpReq->DstAddr, &vmacSta_p->macStaAddr)) == NULL)
	{
		return;
	}
#endif

	if (!mib_StaCfg_p->DirectOptImpl)
	{//AP doesnot support DLP
		//MakeDlpRespErrorFrm(pDlpReq, TxMsg_p, IEEEtypes_STATUS_QOS_DLP_NOT_ALLOW);
	}
	else if((pStaInfo = extStaDb_GetStaInfo(vmacSta_p,&pDlpReq->DstAddr, 0)) == NULL)
	{//Dst Sta is not associated to AP
		//MakeDlpRespErrorFrm(pDlpReq, TxMsg_p, IEEEtypes_STATUS_QOS_DLP_NOT_PRESENT);
	}
	else 
	{   
		if(pStaInfo->IsStaQSTA == FALSE)
		{//Dst Sta is not a QoS Sta
			//MakeDlpRespErrorFrm(pDlpReq, TxMsg_p, IEEEtypes_STATUS_QOS_NOT_QSTA);
		}
		else
		{//Forward the Msg to the Dst Addr
			//BodyLen = pMgmtMsg->Hdr.FrmBodyLen - 
			//	sizeof(IEEEtypes_MgmtHdr_t) - sizeof(RxSign_t);
			//memcpy(&TxMsg_p->MgmtFrame.Body, pDlpReq, BodyLen);
			//TxMsg_p->MgmtFrame.Hdr.FrmBodyLen = BodyLen; 
		}
	}
#ifdef AP_MAC_LINUX
	if (txMgmtMsg(vmacSta_p->dev,txSkb_p) != OS_SUCCESS )
		dev_kfree_skb_any(txSkb_p);
#else
	if (tx80211Q_MgmtWriteNoBlock(TxMsg_p) != OS_SUCCESS )
	{
		pool_FreeBuf((UINT8 *)TxMsg_p);
	}
#endif
}


void ProcessDlpRsp(vmacApInfo_t *vmacSta_p, macmgmtQ_MgmtMsg_t *pMgmtMsg)
{
	IEEEtypes_DlpResp_t  *pDlpRsp;
#ifdef AP_MAC_LINUX
	struct sk_buff *txSkb_p;
#endif

	pDlpRsp = &pMgmtMsg->Body.DlpResp;
	/* Allocate space for response message */
#ifdef AP_MAC_LINUX
	if ((txSkb_p = mlmeApiPrepMgtMsg(IEEE_MSG_QOS_ACTION, (IEEEtypes_MacAddr_t *)pDlpRsp->DstAddr, &vmacSta_p->macStaAddr)) == NULL)
		return;
	//TxMsg_p = (tx80211_MgmtMsg_t *) txSkb_p->data;
#else
	if ((TxMsg_p = mlmeApiPrepMgtMsg(IEEE_MSG_QOS_ACTION, (IEEEtypes_MacAddr_t *)pDlpRsp->DstAddr, &vmacSta_p->macStaAddr)) == NULL)
	{
		return;
	}
#endif
	//BodyLen = pMgmtMsg->Hdr.FrmBodyLen - 
	//	sizeof(IEEEtypes_MgmtHdr_t) - sizeof(RxSign_t);
	//memcpy(&TxMsg_p->MgmtFrame.Body, pDlpRsp, BodyLen);
	//TxMsg_p->MgmtFrame.Hdr.FrmBodyLen = BodyLen; 
#ifdef AP_MAC_LINUX
	if (txMgmtMsg(vmacSta_p->dev,txSkb_p) != OS_SUCCESS )
		dev_kfree_skb_any(txSkb_p);
#else
	if (tx80211Q_MgmtWriteNoBlock(TxMsg_p) != OS_SUCCESS )
	{
		pool_FreeBuf((UINT8 *)TxMsg_p);
	}
#endif
}

void ProcessDlpTeardown(vmacApInfo_t *vmacSta_p, macmgmtQ_MgmtMsg_t *pMgmtMsg)
{
	IEEEtypes_DlpTearDown_t  *pDlpMsg;
#ifdef AP_MAC_LINUX
	struct sk_buff *txSkb_p;
#endif

	pDlpMsg = &pMgmtMsg->Body.DlpTearDown;
	/* Allocate space for response message */
#ifdef AP_MAC_LINUX
	if ((txSkb_p = mlmeApiPrepMgtMsg(IEEE_MSG_QOS_ACTION, (IEEEtypes_MacAddr_t *)pDlpMsg->DstAddr, &vmacSta_p->macStaAddr)) == NULL)
		return;
	//TxMsg_p = (tx80211_MgmtMsg_t *) txSkb_p->data;
#else
	if ((TxMsg_p = mlmeApiPrepMgtMsg(IEEE_MSG_QOS_ACTION, (IEEEtypes_MacAddr_t *)pDlpMsg->DstAddr, &vmacSta_p->macStaAddr)) == NULL)
	{
		return;
	}
#endif
	//BodyLen = pMgmtMsg->Hdr.FrmBodyLen - 
	//	sizeof(IEEEtypes_MgmtHdr_t) - sizeof(RxSign_t);
	//memcpy(&TxMsg_p->MgmtFrame.Body, pDlpMsg, BodyLen);
	//TxMsg_p->MgmtFrame.Hdr.FrmBodyLen = BodyLen; 
#ifdef AP_MAC_LINUX
	if (txMgmtMsg(vmacSta_p->dev,txSkb_p) != OS_SUCCESS )
		dev_kfree_skb_any(txSkb_p);
#else
	if (tx80211Q_MgmtWriteNoBlock(TxMsg_p) != OS_SUCCESS )
	{
		pool_FreeBuf((UINT8 *)TxMsg_p);
	}
#endif
}

#endif
