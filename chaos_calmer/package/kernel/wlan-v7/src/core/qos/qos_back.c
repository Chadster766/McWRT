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


/* This file contains function for Block Ack*/

#ifdef QOS_FEATURE


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

#include "wldebug.h"
#ifdef AP_MAC_LINUX
extern struct sk_buff *mlmeApiPrepMgtMsg2(UINT32 Subtype, IEEEtypes_MacAddr_t *DestAddr, IEEEtypes_MacAddr_t *SrcAddr, UINT16 size);
#else
extern tx80211_MgmtMsg_t *mlmeApiPrepMgtMsg2(UINT32 Subtype, IEEEtypes_MacAddr_t *DestAddr, IEEEtypes_MacAddr_t *SrcAddr, UINT16 size);
#endif

void SendAddBAReq(vmacApInfo_t *vmacSta_p,IEEEtypes_MacAddr_t StaAddrA, UINT8 tsid,
				  IEEEtypes_QoS_BA_Policy_e BaPolicy, UINT32 SeqNo, UINT8 DialogToken)
{
	//macmgmtQ_MgmtMsg_t * MgmtReq_p;
	//extStaDb_StaInfo_t StaInfo;
	//Mrvl_TSPEC_t *pTspec;
	//tx80211_MgmtMsg_t *TxMsg_p;
#ifdef AP_MAC_LINUX
	struct sk_buff *txSkb_p;
#endif
	macmgmtQ_MgmtMsg2_t * MgmtRsp_p;
	//extStaDb_StaInfo_t *pStaInfo;
	//WSM_DELTS_Req_t *pDelTSFrm;
	//IEEEtypes_ADDBA_Req_t2 *pAddBaReqFrm;
	IEEEtypes_ADDBA_Req_t *pAddBaReqFrm;


	if ((txSkb_p = mlmeApiPrepMgtMsg2(IEEE_MSG_QOS_ACTION,  (IEEEtypes_MacAddr_t *)StaAddrA, (IEEEtypes_MacAddr_t *)vmacSta_p->macBssId, sizeof(IEEEtypes_ADDBA_Req_t2))) == NULL)        
	{
		WLDBG_INFO(DBG_LEVEL_0,"No more buffer !!!!!!!!!!!!\n");
		return;
	}

	MgmtRsp_p = (macmgmtQ_MgmtMsg2_t *) txSkb_p->data;
	pAddBaReqFrm = (IEEEtypes_ADDBA_Req_t*)&MgmtRsp_p->Body;

	//HACK FOR NOW FOO

	//WLDBG_DUMP_DATA(1, txSkb_p->data, txSkb_p->len);

	pAddBaReqFrm->Category = BlkAck;
	pAddBaReqFrm->Action = ADDBA_REQ;
	pAddBaReqFrm->DialogToken = DialogToken;
	pAddBaReqFrm->ParamSet.amsdu = (*(vmacSta_p->Mib802dot11->pMib_11nAggrMode)& WL_MODE_AMSDU_TX_MASK)?1:0;
	pAddBaReqFrm->ParamSet.BA_policy = BaPolicy;
	pAddBaReqFrm->ParamSet.tid = tsid;
	pAddBaReqFrm->ParamSet.BufSize =64;
	pAddBaReqFrm->Timeout_val =0x0;//in sec
	pAddBaReqFrm->SeqControl.FragNo=0;
	pAddBaReqFrm->SeqControl.Starting_Seq_No = SeqNo;

	/* Send mgt frame */
#ifdef AP_MAC_LINUX
	if (txMgmtMsg(vmacSta_p->dev,txSkb_p) != OS_SUCCESS )
	{
		dev_kfree_skb_any(txSkb_p);
		return;
	}
#endif
	return;
}

void ProcessAddBAReq(macmgmtQ_MgmtMsg_t *pMgmtMsg)
{
	//The first condition finds if the dialog token matches...
	if (GetTspec((IEEEtypes_MacAddr_t *)pMgmtMsg->Hdr.SrcAddr, pMgmtMsg->Body.AddBAResp.DialogToken) ||
		(pMgmtMsg->Body.AddBAResp.StatusCode != 0))
	{
		return;
	}
	//stop timer
	//Record data
}

void SendDelBA(vmacApInfo_t *vmacSta_p,IEEEtypes_MacAddr_t StaAddr, UINT8 tsid)
{
	IEEEtypes_DELBA_t *pDelBA;
	//tx80211_MgmtMsg_t * TxMsg_p;
#ifdef AP_MAC_LINUX						   
	struct sk_buff *txSkb_p;
#endif
	macmgmtQ_MgmtMsg2_t * MgmtRsp_p;


	if ((txSkb_p = mlmeApiPrepMgtMsg2(IEEE_MSG_QOS_ACTION,  (IEEEtypes_MacAddr_t *)StaAddr,(IEEEtypes_MacAddr_t *)vmacSta_p->macBssId, sizeof(IEEEtypes_DELBA_t))) == NULL)        
	{
		WLDBG_INFO(DBG_LEVEL_0,"No more buffer !!!!!!!!!!!!\n");
		return;
	}

	MgmtRsp_p = (macmgmtQ_MgmtMsg2_t *) txSkb_p->data;
	pDelBA = (IEEEtypes_DELBA_t*)&MgmtRsp_p->Body;

	pDelBA->Category = BlkAck;
	pDelBA->Action = DELBA;
	pDelBA->ParamSet.Resvd = 0;
	pDelBA->ParamSet.Initiator = 1;
	pDelBA->ParamSet.tid = tsid;
	pDelBA->ReasonCode= 1;

	/* Send mgt frame */
	if (txMgmtMsg(vmacSta_p->dev,txSkb_p) != OS_SUCCESS )
	{
		dev_kfree_skb_any(txSkb_p);
		return;
	}
	//start timer
	//Do HouseKeeping
	return;
}
#if 1
void SendDelBA2(vmacApInfo_t *vmacSta_p,IEEEtypes_MacAddr_t StaAddr, UINT8 tsid)
{
	IEEEtypes_DELBA_t *pDelBA;
	//tx80211_MgmtMsg_t *TxMsg_p;
#ifdef AP_MAC_LINUX
	struct sk_buff *txSkb_p;
#endif
	macmgmtQ_MgmtMsg2_t * MgmtRsp_p;


	if ((txSkb_p = mlmeApiPrepMgtMsg2(IEEE_MSG_QOS_ACTION,  (IEEEtypes_MacAddr_t *)StaAddr,(IEEEtypes_MacAddr_t *)vmacSta_p->macBssId, sizeof(IEEEtypes_DELBA_t))) == NULL)        
	{
		WLDBG_INFO(DBG_LEVEL_0,"No more buffer !!!!!!!!!!!!\n");
		return;
	}

	MgmtRsp_p = (macmgmtQ_MgmtMsg2_t *) txSkb_p->data;
	pDelBA = (IEEEtypes_DELBA_t*)&MgmtRsp_p->Body;

	pDelBA->Category = BlkAck;
	pDelBA->Action = DELBA;
	pDelBA->ParamSet.Resvd = 0;
	pDelBA->ParamSet.Initiator = 0;
	pDelBA->ParamSet.tid = tsid;
	pDelBA->ReasonCode= 1;

	/* Send mgt frame */
	if (txMgmtMsg(vmacSta_p->dev,txSkb_p) != OS_SUCCESS )
	{
		dev_kfree_skb_any(txSkb_p);
		return;
	}
	//start timer
	//Do HouseKeeping
	return;

	//check to see if a BA has been established for this TS
	//return;
}
#endif
#endif
