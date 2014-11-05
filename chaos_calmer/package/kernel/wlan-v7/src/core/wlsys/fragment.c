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

#include "wldebug.h"
#include "ap8xLnxRegs.h"
#include "ap8xLnxDesc.h"
#include "ap8xLnxIntf.h"
#include "ap8xLnxXmit.h"
#include "ap8xLnxFwcmd.h"
#include "wltypes.h"
#include "wl_macros.h"
#include "IEEE_types.h"
#include "wl_mib.h"
#include "wl_hal.h"
#include "StaDb.h"
#include "ds.h"
#include "ap8xLnxDma.h"
#include "Fragment.h"


#define	IEEE80211_SEQ_SEQ_SHIFT			4
#define	IEEE80211_SEQ_FRAG_MASK			0x000f

struct ieee80211_frame
{
	IEEEtypes_FrameCtl_t FrmCtl;
	UINT8	dur[2];
	UINT8	addr1[IEEEtypes_ADDRESS_SIZE];
	UINT8	addr2[IEEEtypes_ADDRESS_SIZE];
	UINT8	addr3[IEEEtypes_ADDRESS_SIZE];
	UINT8	seq[2];
	UINT8   addr4[IEEEtypes_ADDRESS_SIZE];
} PACK;

inline void ResetDeFragBufInfo(DeFragBufInfo_t *pDeFragBufInfo)
{
	pDeFragBufInfo->SeqNo = 0;
	pDeFragBufInfo->FragNo = 0;
	pDeFragBufInfo->pFrame= NULL;
}
inline UINT32 GetDeFragBuf(extStaDb_StaInfo_t *pStaInfo, UINT16 SeqNo)
{
	if ((pStaInfo->DeFragBufInfo.SeqNo == SeqNo) )
	{
		return 1;
	}
	return -1;
}


inline UINT32 GetDeFragFreeBuf(extStaDb_StaInfo_t *pStaInfo)
{
	if(pStaInfo->DeFragBufInfo.pFrame)
	{
		dev_kfree_skb_any((struct sk_buff *) pStaInfo->DeFragBufInfo.pFrame);
		ResetDeFragBufInfo(&pStaInfo->DeFragBufInfo);
	}
	return 1;
}


struct sk_buff *DeFragPck(struct net_device *dev,struct sk_buff *skb, extStaDb_StaInfo_t **pStaInfo)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, dev);
	vmacApInfo_t *vmacSta_p = wlpptr->vmacSta_p;
	UINT32 BuffNo;
	struct sk_buff *Tmpskb;
	struct ieee80211_frame *wh = (struct ieee80211_frame *) skb->data;
	UINT8 FragNo;
	UINT16 SeqNo;
	UINT16 ieee80211HdrLen = 0;
    *pStaInfo =  extStaDb_GetStaInfo(vmacSta_p,&(wh->addr2), 1);
	ieee80211HdrLen = sizeof(struct ieee80211_frame);
	if (IS_GROUP((UINT8 *)&(wh->addr1)) || !(*pStaInfo))
	{
		return skb;
	}
	Tmpskb = skb;
	FragNo = le16_to_cpu(*(u_int16_t *)(wh->seq)) & IEEE80211_SEQ_FRAG_MASK;
	SeqNo =  le16_to_cpu(*(u_int16_t *)(wh->seq)) >> IEEE80211_SEQ_SEQ_SHIFT;
	if (!wh->FrmCtl.MoreFrag && FragNo == 0)
		return skb;
	if (FragNo == 0)
	{ //first fragment
		BuffNo = GetDeFragFreeBuf(*pStaInfo);
		if (BuffNo != 0xFFFFFFFF)
		{
			(*pStaInfo)->DeFragBufInfo.pFrame = skb;
			if (wh->FrmCtl.MoreFrag)
			{
				if (skb_is_nonlinear(skb))
				{
					(*pStaInfo)->DeFragBufInfo.pFrame= skb_copy(skb, GFP_ATOMIC);
					(*pStaInfo)->DeFragBufInfo.pFrame->protocol = skb->protocol;
					(*pStaInfo)->DeFragBufInfo.pFrame->dev = skb->dev;
					dev_kfree_skb_any(skb);
				} else if (skb->end - skb->head < dev->mtu +
					ieee80211HdrLen)
				{
					(*pStaInfo)->DeFragBufInfo.pFrame = skb_copy_expand(skb, 0,
						(dev->mtu  +
						ieee80211HdrLen)
						- (skb->end - skb->head), GFP_ATOMIC);
					(*pStaInfo)->DeFragBufInfo.pFrame->protocol = skb->protocol;
					(*pStaInfo)->DeFragBufInfo.pFrame->dev = skb->dev;
					dev_kfree_skb_any(skb);
				}
			}
		} else /** No buffer **/
		{
			dev_kfree_skb_any((struct sk_buff *)Tmpskb);
			return NULL;
		}
	} else
	{   //Get the DeFrag buff with the given sequence no

		BuffNo = GetDeFragBuf(*pStaInfo, SeqNo);
		if (BuffNo != 0xFFFFFFFF)
		{
			if ((*pStaInfo)->DeFragBufInfo.FragNo + 1 == FragNo)
			{
				struct ieee80211_frame *wh1 = (struct ieee80211_frame *)
					(*pStaInfo)->DeFragBufInfo.pFrame->data;
				/* Copy current fragment at end of previous one */
				memcpy((*pStaInfo)->DeFragBufInfo.pFrame->tail,
					skb->data + ieee80211HdrLen,
					skb->len - ieee80211HdrLen
					);
				/* Update tail and length */
				skb_put((*pStaInfo)->DeFragBufInfo.pFrame,
					skb->len - ieee80211HdrLen);
				/* Keep a copy of last sequence and fragno */
				*(u_int16_t *) wh1->seq = *(u_int16_t *) wh1->seq;
				(*pStaInfo)->DeFragBufInfo.pFrame->protocol = skb->protocol;
				(*pStaInfo)->DeFragBufInfo.pFrame->dev = skb->dev;
				dev_kfree_skb_any(skb);

			} else  /** fragment not in sequence **/
			{
				dev_kfree_skb_any((struct sk_buff *) (*pStaInfo)->DeFragBufInfo.pFrame);
				ResetDeFragBufInfo(&((*pStaInfo)->DeFragBufInfo));
				dev_kfree_skb_any((struct sk_buff *)Tmpskb);
				return NULL;
			}
		} else  /** Error condition **/
		{
			dev_kfree_skb_any((struct sk_buff *)Tmpskb);
			return NULL;
		}
	}

	/** Last Fragment handle here **/
	if (wh->FrmCtl.MoreFrag == 0)
	{
		skb= (*pStaInfo)->DeFragBufInfo.pFrame;
		ResetDeFragBufInfo(&((*pStaInfo)->DeFragBufInfo));
		return(skb);
	} else
	{
		(*pStaInfo)->DeFragBufInfo.SeqNo = SeqNo;
		(*pStaInfo)->DeFragBufInfo.FragNo = FragNo;
		return NULL;
	}
}

