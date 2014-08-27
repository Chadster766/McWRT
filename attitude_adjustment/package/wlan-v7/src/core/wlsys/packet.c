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
* \file    packet.c
* \brief   Sample packets processing routines
*/
#include <linux/version.h>
#include <linux/ip.h>
#include <linux/igmp.h>
#include "Fragment.h"

#include "keyMgmt_if.h"
#include "wldebug.h"
#include "ap8xLnxRegs.h"
#include "ap8xLnxDesc.h"
#include "ap8xLnxIntf.h"
#include "ap8xLnxXmit.h"
#include "ap8xLnxFwcmd.h"
#include "ap8xLnxWlLog.h"
#include "wltypes.h"
#include "wl_macros.h"
#include "IEEE_types.h"
#include "wl_mib.h"
#include "wl_hal.h"
#include "StaDb.h"
#include "ds.h"
#include "ap8xLnxDma.h"
#include "buildModes.h"
#ifdef ZERO_COPY
#undef AGG_QUE
#endif
#include "macmgmtap.h"
#include "macMgmtMlme.h"
#ifdef EWB
#include "ewb_packet.h"
#endif
#ifdef DYNAMIC_BA_SUPPORT
#include "wltypes.h"
#include "osif.h"
#include "timer.h"
#endif
#include "wds.h"
#include "mlmeApi.h"
#include "keyMgmtSta.h"
#include "linkmgt.h"
#ifdef MPRXY
#include "ap8xLnxMPrxy.h"
#endif

#define INTERNAL_FLUSH_TIMER

#ifndef AMSDUOVERAMPDU
struct mcsratemap_t
{
	UINT16 rate2timesMbps[16];
}mcsratemap_t;

static struct mcsratemap_t macratemap[2] = {
	{{13, 26, 39, 52, 78, 104, 117, 130, 26, 52, 78, 104, 156, 208, 234, 260}},
	{{27, 54, 81, 108, 162, 216, 243, 270, 54, 108, 162, 216, 324, 432, 486, 540}}
};
#endif

#define	ETHER_TYPE_LEN		2	/* length of the Ethernet type field */
#define	ETHER_CRC_LEN		4	/* length of the Ethernet CRC */
#define	ETHER_HDR_LEN		(IEEEtypes_ADDRESS_SIZE*2+ETHER_TYPE_LEN)
#define	ETHER_MAX_LEN		1518

#define	ETHERMTU	(ETHER_MAX_LEN-ETHER_HDR_LEN-ETHER_CRC_LEN)
#define	IEEE80211_SEQ_SEQ_SHIFT			4

#define MAX_REORDERING_HOLD_TIME 	(HZ/20) /* 50ms */

/*Once receive Probe Resp or JoinCmd in client mode, we set to 1 to monitor active tx traffic.
* In client mode, we monitor active tx/ rx traffic so we don't send probe req during traffic.
* But when AP suddenly goes away in UDP client->AP case, consecutive tx failure will reach limit and fw will inform host.
* When host receives ISR for consecutive tx failure, we don't monitor active tx anymore so client mode send probe req out
*/
UINT8 ClientModeTxMonitor = 0;		
UINT8 ProbeReqOnTx = 0;			// Client mode sends Probe Req during tx. 0: No, 1: Yes

struct ieee80211_frame_min
{
	IEEEtypes_FrameCtl_t FrmCtl;		
	UINT8	dur[2];
	UINT8	addr1[IEEEtypes_ADDRESS_SIZE];
	UINT8	addr2[IEEEtypes_ADDRESS_SIZE];
	/* FCS */
} PACK;

/*
* Structure of a 10Mb/s Ethernet header.
*/
struct	ether_header
{
	UINT8	ether_dhost[IEEEtypes_ADDRESS_SIZE];
	UINT8	ether_shost[IEEEtypes_ADDRESS_SIZE];
	UINT16	ether_type;
};
struct ieee80211_qosframe
{
	IEEEtypes_FrameCtl_t FrmCtl;
	UINT8	dur[2];
	UINT8	addr1[IEEEtypes_ADDRESS_SIZE];
	UINT8	addr2[IEEEtypes_ADDRESS_SIZE];
	UINT8	addr3[IEEEtypes_ADDRESS_SIZE];
	UINT8	seq[2];
	UINT8	qos[2];
	UINT8   addr4[IEEEtypes_ADDRESS_SIZE];
} PACK;

#ifdef SOC_W8764
struct ieee80211_qosHtctlframe
{
	IEEEtypes_FrameCtl_t FrmCtl;
	UINT8	dur[2];
	UINT8	addr1[IEEEtypes_ADDRESS_SIZE];
	UINT8	addr2[IEEEtypes_ADDRESS_SIZE];
	UINT8	addr3[IEEEtypes_ADDRESS_SIZE];
	UINT8	seq[2];	
	UINT8   addr4[IEEEtypes_ADDRESS_SIZE];
    UINT8	qos[2];
    UINT8   htctl[4];
} PACK;
#endif

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


#define	IEEE80211_ADDR_COPY(dst,src)	memcpy(dst,src,IEEEtypes_ADDRESS_SIZE)
struct llc
{
	UINT8 llc_dsap;
	UINT8 llc_ssap;
	union
	{
		struct
		{
			UINT8 control;
			UINT8 format_id;
			UINT8 class;
			UINT8 window_x2;
		} PACK type_u;
		struct
		{
			UINT8 num_snd_x2;
			UINT8 num_rcv_x2;
		} PACK type_i;
		struct
		{
			UINT8 control;
			UINT8 num_rcv_x2;
		} PACK type_s;
		struct
		{
			UINT8 control;
			/*
			* We cannot put the following fields in a structure because
			* the structure rounding might cause padding.
			*/
			UINT8 frmr_rej_pdu0;
			UINT8 frmr_rej_pdu1;
			UINT8 frmr_control;
			UINT8 frmr_control_ext;
			UINT8 frmr_cause;
		} PACK type_frmr;
		struct
		{
			UINT8  control;
			UINT8  org_code[3];
			UINT16 ether_type;
		} PACK type_snap;
		struct
		{
			UINT8 control;
			UINT8 control_ext;
		} PACK type_raw;
	} llc_un /* XXX PACK ??? */;
} PACK;
#define LLC_SNAP_LSAP	0xaa
#define LLC_UI		0x03
#define	llc_control		llc_un.type_u.control
#define	llc_control_ext		llc_un.type_raw.control_ext
#define	llc_fid			llc_un.type_u.format_id
#define	llc_class		llc_un.type_u.class
#define	llc_window		llc_un.type_u.window_x2
#define	llc_frmrinfo 		llc_un.type_frmr.frmr_rej_pdu0
#define	llc_frmr_pdu0		llc_un.type_frmr.frmr_rej_pdu0
#define	llc_frmr_pdu1		llc_un.type_frmr.frmr_rej_pdu1
#define	llc_frmr_control	llc_un.type_frmr.frmr_control
#define	llc_frmr_control_ext	llc_un.type_frmr.frmr_control_ext
#define	llc_frmr_cause		llc_un.type_frmr.frmr_cause
#define	llc_snap		llc_un.type_snap

struct llc_rptr {
	struct llc llc;
	struct ether_header eh;
} PACK;

#define RPTR_ETHERTYPE	0x0003

#ifdef MPRXY
#define	IS_IN_CLASSD(a)		((((UINT32) (a)) & 0xf0000000) == 0xe0000000)
#define	IS_IN_MULTICAST(a)		IS_IN_CLASSD(a)
#endif

#define IPQUAD(addr) \
        ((unsigned char *)&addr)[3], \
        ((unsigned char *)&addr)[2], \
        ((unsigned char *)&addr)[1], \
        ((unsigned char *)&addr)[0]

#define DECRYPT_ERR_MASK        0x80
#define GENERAL_DECRYPT_ERR     0xFF
#define TKIP_DECRYPT_MIC_ERR    0x02
#define WEP_DECRYPT_ICV_ERR     0x04
#define TKIP_DECRYPT_ICV_ERR    0x08


static UINT16 EdcaSeqNum[MAX_STNS][MAX_PRI+1];
#define CurrentFrag(q) (pStaInfo->aggr11n.Frag[q])
#define NextFrag(q) (CurrentFrag(q).status =0);\
	(CurrentFrag(q).curPosition_p =0);
#define CurrentRdFrag(q) (pStaInfo->aggr11n.Frag[q])
#ifdef AMPDU_SUPPORT
#define REORDERING
#endif

extern void MrvlMICErrorHdl(vmacApInfo_t *vmacSta_p, COUNTER_MEASURE_EVENT event);
extern void macMgmtMlme_UpdatePwrMode(vmacApInfo_t *vmacSta_p,struct ieee80211_frame *Hdr_p, extStaDb_StaInfo_t *pStaInfo);

static int ForwardFrame(struct net_device *dev,struct sk_buff *skb);
static int DeAmsduPck(struct net_device *netdev,struct sk_buff *skb);
static void McastProxyCheck(struct sk_buff *skb);
void log_cnt(unsigned char *name, int location);
void start_log(void);

int send_11n_aggregation_skb(struct net_device *netdev, extStaDb_StaInfo_t *pStaInfo, int force)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	unsigned long flags;
	struct sk_buff *skb = NULL;
	int i;
	int retval=0;
	static int previous_aggregation_index;
	SPIN_LOCK_IRQSAVE(&wlpptr->wlpd_p->locks.xmitLock, flags);
#ifdef AGG_QUE
	while ((skb = skb_dequeue(&pStaInfo->aggr11n.txQ)) != NULL)
	{
		if(wlxmit(netdev, skb, IEEE_TYPE_DATA, pStaInfo, 0, FALSE))
		{
			//wlpptr->netDevStats.tx_errors++;
#ifdef AMSDU_AGGREQ_FOR_8K
			if(skb->truesize > MAX_AGGR_SIZE)
			{
				wlpptr->netDevStats.tx_errors += skb->cb[0];
				skb_queue_tail(&wlpptr->wlpd_p->aggreQ, skb);
			}
			else
#endif
			{
				wlpptr->netDevStats.tx_errors++;
				dev_kfree_skb_any(skb);
			}
			SPIN_UNLOCK_IRQRESTORE(&wlpptr->wlpd_p->locks.xmitLock, flags);
			return retval;
		}
		else
		{
			retval++;
			//wlpptr->netDevStats.tx_bytes += skb->len;
		}
	}
	pStaInfo->aggr11n.queon =0;
#endif
#ifndef AMSDUOVERAMPDU
	if(force && (CurrentRdFrag(previous_aggregation_index).status==0))
		pStaInfo->aggr11n.nextpktnoaggr=1;
#endif
	for (i = 0; i<MAX_AGG_QUE; i++)
	{
		if(CurrentRdFrag(i).status &&(force ||(jiffies >=(CurrentRdFrag(i).jiffies+HZ/100) /*CurrentRdFrag(i).status == CurrentRdFrag(i).status_pre*/)))
		{
			previous_aggregation_index = i;

#ifndef ZERO_COPY
			skb = CurrentRdFrag(i).skb;
			if(wlxmit(netdev, skb, IEEE_TYPE_DATA, pStaInfo, 0, FALSE))
#else
			skb = CurrentRdFrag(i).skb[0];
			if(wlxmitmfs(netdev, CurrentFrag(i).skb, pStaInfo, CurrentFrag(i).status))
#endif
			{
			}
			else
			{
				retval++;
				wlpptr->netDevStats.tx_bytes += skb->len;
				NextFrag(i);
				if( pStaInfo->aggr11n.start==0)
					pStaInfo->aggr11n.on = 0;
			}

		}
		CurrentRdFrag(i).status_pre= CurrentRdFrag(i).status;
	}
	SPIN_UNLOCK_IRQRESTORE(&wlpptr->wlpd_p->locks.xmitLock, flags);
	return retval;
}

#define WL_MAX_AMSDU_SIZE_8K 7935
#define WL_MAX_AMSDU_SIZE_4K 3839

static struct sk_buff *do_11n_aggregation(struct net_device *netdev, struct sk_buff *skb, extStaDb_StaInfo_t *pStaInfo, struct ether_header *pEh, int q, struct ieee80211_qosframe *wh)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	vmacApInfo_t *vmacSta_p = wlpptr->vmacSta_p;
	MIB_802DOT11 *mib = vmacSta_p->Mib802dot11;
	struct sk_buff *newskb = NULL;
	UINT16 length, pad, cap;
	unsigned long flags, xmitflags;
#ifdef ZERO_COPY
	struct ieee80211_qosframe wh;
#endif
	SPIN_LOCK_IRQSAVE(&pStaInfo->aggr11n.Lock, flags);
	//potential amsdu size, need add amsdu header 14 bytes+ maximum padding 3.
	length = skb->len-sizeof(struct ieee80211_qosframe)+17;
	if((skb->len > *(mib->mib_amsdu_allowsize))||(CurrentFrag(q).status && (CurrentFrag(q).skb->len + length) > *(mib->mib_amsdu_maxsize)))
	{
 		if(CurrentFrag(q).status)
		{
			SPIN_LOCK_IRQSAVE(&wlpptr->wlpd_p->locks.xmitLock, xmitflags);
			if (wlxmit(netdev, CurrentFrag(q).skb, IEEE_TYPE_DATA, pStaInfo, 0, FALSE))
			{
				WLDBG_INFO(DBG_LEVEL_13, "could not xmit");
				wlpptr->netDevStats.tx_errors++;
				dev_kfree_skb_any(CurrentFrag(q).skb);
			}
			SPIN_UNLOCK_IRQRESTORE(&wlpptr->wlpd_p->locks.xmitLock, xmitflags);
			NextFrag(q);
		} 
		SPIN_UNLOCK_IRQRESTORE(&pStaInfo->aggr11n.Lock, flags);
		return skb;
	}
	*(UINT16 *)&wh->qos[0] |= IEEE_QOS_CTL_AMSDU;
	CurrentFrag(q).jiffies = jiffies;
	switch (CurrentFrag(q).status)
	{
	case 0:
		CurrentFrag(q).length = 6*(netdev->mtu)+128;
#ifndef ZERO_COPY
#ifdef AMSDU_AGGREQ_FOR_8K
		newskb = skb_dequeue(&wlpptr->wlpd_p->aggreQ);//, GFP_ATOMIC);
#else
        /* Add more headroom to prevent skb_over_panic on A370.  
           TODO: need to find out why A370 need allocate more room? */
		newskb = dev_alloc_skb( *(mib->mib_amsdu_maxsize)+MIN_BYTES_HEADROOM);
		if(newskb)
		{
#ifdef WL_KERNEL_26
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
			if(skb_linearize(newskb))
#else
			if(skb_linearize(newskb, GFP_ATOMIC))
#endif //kernel 2.6.20
			{
				wlpptr->netDevStats.tx_errors++;
				dev_kfree_skb_any(newskb);
#ifndef ZERO_COPY
				dev_kfree_skb_any(skb);
#endif	//zero copy
				SPIN_UNLOCK_IRQRESTORE(&pStaInfo->aggr11n.Lock, flags);
				return NULL;
			}
#endif  //WL_KERNEL_26
		}
#endif	//AMSDU_AGGREQ_FOR_8K

#else
		newskb = skb;
#endif	//ndef zero copy
		/* Load the data pointers. */

		if (newskb != NULL)
		{
			/* Statistatices aggregated packets */
			newskb->cb[0] = 0;
#ifndef ZERO_COPY           
#ifdef AMSDU_AGGREQ_FOR_8K   
			/* The following three lines will cause nr_frags in skb_shared_info
			* becomes non-zero and result page fault crash at dev_kfree_skb_any()
			* in AXP platform
			* But for A370 platform, we need these 3 lines. Otherwise, mediaroom vid streaming
			* will cause kernel panic. TODO: find out why AXP must comment out these lines
			*/
			newskb->data = newskb->head;
			newskb->tail = newskb->head;
			newskb->end = newskb->head + newskb->truesize - sizeof(struct sk_buff);
			/* Set up other state */
			newskb->len = 0;
			newskb->cloned = 0;
			newskb->data_len = 0;
#endif
			skb_reserve(newskb, MIN_BYTES_HEADROOM); /* 64 byte headroom */
#endif
			length = skb->len-sizeof(struct ieee80211_qosframe);
#ifndef ZERO_COPY
			memcpy( newskb->data, skb->data, sizeof(struct ieee80211_qosframe));
			memcpy( newskb->data+sizeof(struct ieee80211_qosframe), pEh, ETHER_HDR_LEN-2);
#ifndef AMSDU_BYTE_REORDER
			*(UINT8 *)(newskb->data+sizeof(struct ieee80211_qosframe)+ETHER_HDR_LEN-1) = (length>>8)&0xff;
			*(UINT8 *)(newskb->data+sizeof(struct ieee80211_qosframe)+ETHER_HDR_LEN-2) = (length)&0xff;
#else
			*(UINT8 *)(newskb->data+sizeof(struct ieee80211_qosframe)+ETHER_HDR_LEN-1) = (length)&0xff; //hack for bcom
			*(UINT8 *)(newskb->data+sizeof(struct ieee80211_qosframe)+ETHER_HDR_LEN-2) = (length>>8)&0xff; //hack for bcom
#endif
			MEMCPY newskb->data+sizeof(struct ieee80211_qosframe)+ETHER_HDR_LEN, skb->data+sizeof(struct ieee80211_qosframe), length);
#else
			memcpy( &wh, skb->data, sizeof(struct ieee80211_qosframe));
			skb_push(newskb, ETHER_HDR_LEN);
			memcpy( newskb->data, &wh, sizeof(struct ieee80211_qosframe));
			memcpy( newskb->data+sizeof(struct ieee80211_qosframe), pEh, ETHER_HDR_LEN-2);
#ifndef AMSDU_BYTE_REORDER
			*(UINT8 *)(newskb->data+sizeof(struct ieee80211_qosframe)+ETHER_HDR_LEN-1) = (length>>8)&0xff;
			*(UINT8 *)(newskb->data+sizeof(struct ieee80211_qosframe)+ETHER_HDR_LEN-2) = (length)&0xff;
#else
			*(UINT8 *)(newskb->data+sizeof(struct ieee80211_qosframe)+ETHER_HDR_LEN-1) = (length)&0xff;
			*(UINT8 *)(newskb->data+sizeof(struct ieee80211_qosframe)+ETHER_HDR_LEN-2) = (length>>8)&0xff;
#endif
#endif
			CurrentFrag(q).pad = ((length+ETHER_HDR_LEN)%4)?(4-(length+ETHER_HDR_LEN)%4):0;
#ifndef ZERO_COPY
			skb_put(newskb, (skb->len+ETHER_HDR_LEN));
			newskb->dev = skb->dev;
			newskb->protocol = skb->protocol;
	        newskb->priority = skb->priority;
			CurrentFrag(q).skb = newskb;
			CurrentFrag(q).curPosition_p = CurrentFrag(q).skb->data + CurrentFrag(q).skb->len;
			CurrentFrag(q).skb->cb[0]++;
#else
			CurrentFrag(q).skb[0] = newskb;
#endif
			CurrentFrag(q).status++;
			//wlpptr->netDevStats.tx_packets++;
		}
		else
		{
			wlpptr->netDevStats.tx_errors++;
		}
#ifndef ZERO_COPY
		dev_kfree_skb_any(skb);
#endif
		SPIN_UNLOCK_IRQRESTORE(&pStaInfo->aggr11n.Lock, flags);
		return NULL;
	default:
		//wlpptr->netDevStats.tx_packets++;
		length = skb->len-sizeof(struct ieee80211_qosframe);
		pad = CurrentFrag(q).pad;
		CurrentFrag(q).curPosition_p +=pad;
#ifndef ZERO_COPY
		memcpy(CurrentFrag(q).curPosition_p, pEh, ETHER_HDR_LEN-2);
#ifndef AMSDU_BYTE_REORDER
		*(UINT8 *)(CurrentFrag(q).curPosition_p+ETHER_HDR_LEN-1) = (length>>8)&0xff;
		*(UINT8 *)(CurrentFrag(q).curPosition_p+ETHER_HDR_LEN-2) = (length)&0xff;
#else
		*(UINT8 *)(CurrentFrag(q).curPosition_p+ETHER_HDR_LEN-1) = (length)&0xff;
		*(UINT8 *)(CurrentFrag(q).curPosition_p+ETHER_HDR_LEN-2) = (length>>8)&0xff;


#endif
		MEMCPY CurrentFrag(q).curPosition_p+ETHER_HDR_LEN, skb->data+sizeof(struct ieee80211_qosframe), length);
		skb_put(CurrentFrag(q).skb, (length+ETHER_HDR_LEN)+pad);
#else
		skb_put(CurrentFrag(q).skb[CurrentFrag(q).status-1], pad);
		skb_pull(skb, sizeof(struct ieee80211_qosframe) -ETHER_HDR_LEN); 
		memcpy(skb->data, pEh, ETHER_HDR_LEN-2);
#ifndef AMSDU_BYTE_REORDER
		*(UINT8 *)(skb->data+ETHER_HDR_LEN-1) = (length>>8)&0xff;
		*(UINT8 *)(skb->data+ETHER_HDR_LEN-2) = (length)&0xff;
#else
		*(UINT8 *)(skb->data+ETHER_HDR_LEN-1) = (length)&0xff;
		*(UINT8 *)(skb->data+ETHER_HDR_LEN-2) = (length>>8)&0xff;
#endif
#endif
		CurrentFrag(q).pad  = ((length+ETHER_HDR_LEN)%4)?(4-(length+ETHER_HDR_LEN)%4):0;
#ifndef ZERO_COPY
		CurrentFrag(q).curPosition_p = CurrentFrag(q).skb->data + CurrentFrag(q).skb->len;
		CurrentFrag(q).status++;
		CurrentFrag(q).skb->cb[0]++;
		dev_kfree_skb_any(skb);
#else
		CurrentFrag(q).skb[CurrentFrag(q).status] = skb;
		CurrentFrag(q).status++;
#endif
		cap =pStaInfo->aggr11n.cap;
#ifndef AMSDUOVERAMPDU
		if(pStaInfo->RateInfo.Format == 0)
		{
			if(pStaInfo->RateInfo.RateIDMCS < 8)
			{
				cap = 0;
				pStaInfo->aggr11n.start = 0;
			}else if(pStaInfo->RateInfo.RateIDMCS < 10)
			{
				cap =1;
			}
			else
				pStaInfo->aggr11n.start = AGGKEEPNUM;
		}
		else if (pStaInfo->RateInfo.Format == 1) 
		{
			if(macratemap[pStaInfo->RateInfo.Bandwidth].rate2timesMbps[pStaInfo->RateInfo.RateIDMCS] < 18*2)
			{
				cap = 0;
				pStaInfo->aggr11n.start = 0;
			}else if (macratemap[pStaInfo->RateInfo.Bandwidth].rate2timesMbps[pStaInfo->RateInfo.RateIDMCS] < 36*2)
			{
				cap =1;
			}
			else
				pStaInfo->aggr11n.start = AGGKEEPNUM;
		}
#endif
		if(cap == 2)
		{
			if(CurrentFrag(q).skb->len > WL_MAX_AMSDU_SIZE_8K-netdev->mtu||CurrentFrag(q).status>*(mib->mib_amsdu_pktcnt))
			{
#ifndef ZERO_COPY
				newskb = CurrentFrag(q).skb;
#else
				newskb =0;
				if(wlxmitmfs(netdev, CurrentFrag(q).skb, pStaInfo, CurrentFrag(q).status))
				{
					int i;
					wlpptr->netDevStats.tx_errors +=  CurrentFrag(q).status;
					for (i = 0; i <  CurrentFrag(q).status; i++)
						dev_kfree_skb_any(CurrentFrag(q).skb[i]);
				}
#endif
				NextFrag(q);
				if( pStaInfo->aggr11n.start==0)
					pStaInfo->aggr11n.on = 0;
#ifdef AGG_QUE
				if(pStaInfo->aggr11n.queon)
				{
					skb_queue_tail(&pStaInfo->aggr11n.txQ, newskb);
					SPIN_UNLOCK_IRQRESTORE(&pStaInfo->aggr11n.Lock, flags);
					return NULL;
				}
				else
				{
					SPIN_UNLOCK_IRQRESTORE(&pStaInfo->aggr11n.Lock, flags);
					return newskb;
				}
#else
				SPIN_UNLOCK_IRQRESTORE(&pStaInfo->aggr11n.Lock, flags);
				return newskb;
#endif
			} else
			{
				SPIN_UNLOCK_IRQRESTORE(&pStaInfo->aggr11n.Lock, flags);
				return NULL;
			}
		}else if(cap == 1)
		{
			if(CurrentFrag(q).skb->len > WL_MAX_AMSDU_SIZE_4K-netdev->mtu||CurrentFrag(q).status>*(mib->mib_amsdu_pktcnt))
			{
#ifndef ZERO_COPY
				newskb = CurrentFrag(q).skb;
#else
				newskb =0;
				if(wlxmitmfs(netdev, CurrentFrag(q).skb, pStaInfo, CurrentFrag(q).status))
				{
					int i;
					wlpptr->netDevStats.tx_errors +=  CurrentFrag(q).status;
					for (i = 0; i <  CurrentFrag(q).status; i++)
						dev_kfree_skb_any(CurrentFrag(q).skb[i]);

				}
#endif
				NextFrag(q);
				if( pStaInfo->aggr11n.start==0)
					pStaInfo->aggr11n.on = 0;
#ifdef AGG_QUE
				if(pStaInfo->aggr11n.queon)
				{
					skb_queue_tail(&pStaInfo->aggr11n.txQ, newskb);
					SPIN_UNLOCK_IRQRESTORE(&pStaInfo->aggr11n.Lock, flags);
					return NULL;
				}
				else
				{
					SPIN_UNLOCK_IRQRESTORE(&pStaInfo->aggr11n.Lock, flags);
					return newskb;
				}
#else

				SPIN_UNLOCK_IRQRESTORE(&pStaInfo->aggr11n.Lock, flags);
				return newskb;
#endif
			} else
			{
				SPIN_UNLOCK_IRQRESTORE(&pStaInfo->aggr11n.Lock, flags);
				return NULL;
			}
		}
		else
		{
#ifndef ZERO_COPY
			newskb = CurrentFrag(q).skb;
#else
			newskb =0;
			if(wlxmitmfs(netdev, CurrentFrag(q).skb, pStaInfo, CurrentFrag(q).status))
			{
				int i;
				wlpptr->netDevStats.tx_errors +=  CurrentFrag(q).status;
				for (i = 0; i <  CurrentFrag(q).status; i++)
					dev_kfree_skb_any(CurrentFrag(q).skb[i]);
			}
#endif
			NextFrag(q);
			if( pStaInfo->aggr11n.start==0)
				pStaInfo->aggr11n.on = 0;
			//SPIN_UNLOCK_IRQRESTORE(&pStaInfo->aggr11n.Lock, flags);
#ifdef AGG_QUE
			if(pStaInfo->aggr11n.queon)
			{
				skb_queue_tail(&pStaInfo->aggr11n.txQ, newskb);
				SPIN_UNLOCK_IRQRESTORE(&pStaInfo->aggr11n.Lock, flags);
				return NULL;
			}
			else
			{
				SPIN_UNLOCK_IRQRESTORE(&pStaInfo->aggr11n.Lock, flags);
				return newskb;
			}
#else
			SPIN_UNLOCK_IRQRESTORE(&pStaInfo->aggr11n.Lock, flags);
			return newskb;
#endif
		}
	}
}
static struct sk_buff *wlan_skbhdr_adjust(struct sk_buff *skb)
{
	int need_headroom = sizeof(struct ieee80211_qosframe)
		+ sizeof(struct ether_header)
		+ 14;// maximum (NBR_BYTES_ADD_TXFWINFO+wep_padding-qos_padding) is 14
	//int need_tailroom = 0;

	skb = skb_unshare(skb, GFP_ATOMIC);
    
	if (skb == NULL)
	{
	    WLDBG_ERROR(DBG_LEVEL_9,"SKB unshare operation failed!\n");
	} 
#if 0
    else if (skb_tailroom(skb) < need_tailroom)
	  {
	  newskb = alloc_skb(netdev->mtu + NUM_EXTRA_RX_BYTES, GFP_ATOMIC);
	  if (newskb != NULL)
	  {
	  skb_reserve(newskb, MIN_BYTES_HEADROOM);
	  MEMCPY newskb->data, skb->data, skb->len);
	  skb_put(newskb, skb->len);
	  newskb->dev = skb->dev;
	  newskb->protocol = skb->protocol;
	  }
	  dev_kfree_skb_any(skb);
	  skb = newskb;
    }
#endif
	else if (skb_headroom(skb) < need_headroom)
	{
		struct sk_buff *tmp;
        
		tmp = skb_realloc_headroom(skb, need_headroom);
		dev_kfree_skb_any(skb);

        if (tmp == NULL)
		{
	    	WLDBG_ERROR(DBG_LEVEL_9,"SKB headroom not enough --- reallocate headroom!\n");
		}
        skb = tmp;        
	}
	return skb;
}

#ifdef AMPDU_SUPPORT
#ifdef SC_PALLADIUM
#define ADDBA_PERIOD_1SEC 100   /* Increase timeout for Palladium */
#else
#define ADDBA_PERIOD_1SEC 10
#endif
static inline void enableAmpduTx(vmacApInfo_t *vmacSta_p, UINT8 *macaddr, UINT8 tid);
void AddbaTimerProcess(UINT8 *data)
{
	Ampdu_tx_t *Ampdu_p= (Ampdu_tx_t *)data;
	vmacApInfo_t *vmacSta_p = (vmacApInfo_t *)Ampdu_p->vmacSta_p;
	MIB_802DOT11 *mib = vmacSta_p->Mib802dot11;
	extStaDb_StaInfo_t *pStaInfo=NULL;
	pStaInfo = extStaDb_GetStaInfo(vmacSta_p, (IEEEtypes_MacAddr_t *)&(Ampdu_p->MacAddr), 1);
	if(pStaInfo)
	{
		pStaInfo->aggr11n.onbytid[Ampdu_p->AccessCat]=0;
		pStaInfo->aggr11n.startbytid[Ampdu_p->AccessCat]=0;
	}
	Ampdu_p->InUse = 0;
	Ampdu_p->TimeOut = 0;
    
	switch(*(mib->mib_AmpduTx))
	{
	case 2:
		enableAmpduTx(vmacSta_p, (UINT8 *)&(Ampdu_p->MacAddr), Ampdu_p->AccessCat);
		break;
	case 1:
		if(pStaInfo)
			pStaInfo->aggr11n.type &=~WL_WLAN_TYPE_AMPDU;
		break;
	default:
		break;
	}
}



struct reorder_t {
	struct net_device *dev;
	UINT16 Aid;
	UINT8 tid;
	UINT16 SeqNo;
};
extern void Ampdu_Flush_All_Pck_in_Reorder_queue(struct net_device *dev,u_int16_t  Aid, u_int8_t Priority);
void ReorderingTimerProcess(UINT8 *data)
{
	struct reorder_t *ro_p = (struct reorder_t *)data;
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, ro_p->dev);
	Ampdu_Flush_All_Pck_in_Reorder_queue(ro_p->dev,ro_p->Aid, ro_p->tid);
	wlpptr->wlpd_p->AmpduPckReorder[ro_p->Aid].ReOrdering[ro_p->tid]=FALSE;
	wlpptr->wlpd_p->AmpduPckReorder[ro_p->Aid].CurrentSeqNo[ro_p->tid]=(ro_p->SeqNo+1)%MAX_AC_SEQNO;  /** assuming next pck **/
#ifdef AMPDU_DEBUG
	printk("reordering timer timeout at %d\n", jiffies);
#endif
	free(data);
}

#ifdef CLIENT_SUPPORT
void AddbaTimerProcessSta(UINT8 *data)
{
	Ampdu_tx_t *Ampdu_p= (Ampdu_tx_t *)data;
	vmacApInfo_t *vmacSta_p = (vmacApInfo_t *)Ampdu_p->vmacSta_p;
	MIB_802DOT11 *mib = vmacSta_p->Mib802dot11;
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, vmacSta_p->dev);
	vmacEntry_t  *vmacEntry_p = (vmacEntry_t  *) wlpptr->clntParent_priv_p;
	extStaDb_StaInfo_t *pStaInfo=NULL;

	pStaInfo = extStaDb_GetStaInfo(vmacSta_p, (IEEEtypes_MacAddr_t *)GetParentStaBSSID(vmacEntry_p->phyHwMacIndx), 1);
	if(pStaInfo)
	{
		pStaInfo->aggr11n.onbytid[Ampdu_p->AccessCat]=0;
		pStaInfo->aggr11n.startbytid[Ampdu_p->AccessCat]=0;
	}
	Ampdu_p->InUse = 0;
	Ampdu_p->TimeOut = 0;
    
	switch(*(mib->mib_AmpduTx))
	{
	case 2:
		enableAmpduTx(vmacSta_p, (UINT8 *)&(Ampdu_p->MacAddr), Ampdu_p->AccessCat);
		break;
	case 1:
		if(pStaInfo)
			pStaInfo->aggr11n.type &=~WL_WLAN_TYPE_AMPDU;
		break;
	default:
		break;
	}
}
#endif
#ifdef AMPDU_SUPPORT_SBA
#define AMPDU_STREAM_NO_START 0
#ifdef SOC_W8864
#define AMPDU_STREAM_NO_END 4
#else
#define AMPDU_STREAM_NO_END 7
#endif
void delbaTimerProcess(UINT8 *data)
{
}
#else /* _AMPDU_SUPPORT_SBA */
#define AMPDU_STREAM_NO_START 0
#define AMPDU_STREAM_NO_END 2
#endif /* _AMPDU_SUPPORT_SBA */

#ifdef DYNAMIC_BA_SUPPORT
typedef struct mwlbainfo{
	UINT32 flag;
	UINT32 pps;
    Ampdu_tx_t *sp;
	UINT8 tid;					
}mwlbainfo;


#define MAX_SECS_TO_RESET_PPS 5
/*
 * Traffic estimator support.  We estimate packets/sec for
 * each AC that is setup for AMPDU or will potentially be
 * setup for AMPDU.  The traffic rate can be used to decide
 * when AMPDU should be setup (according to a threshold)
 * and is available for drivers to do things like cache
 * eviction when only a limited number of BA streams are
 * available and more streams are requested than available.
 */

static void __inline
ieee80211_txampdu_update_pps(Ampdu_tx_t *tap)
{
	/* NB: scale factor of 2 was picked heuristically */
	tap->txa_avgpps = (((tap->txa_avgpps << 2) -
	     tap->txa_avgpps + (tap->txa_pkts*10)) >> 2); //Multiply 10 to get avgpps per sec, in sync with ieee80211_txampdu_count_packet

}

/*
 * Count a packet towards the pps estimate.
 */
void __inline
ieee80211_txampdu_count_packet(Ampdu_tx_t *tap)
{
	if (jiffies - tap->txa_lastsample >= MAX_SECS_TO_RESET_PPS*HZ)
	{
		tap->txa_pkts = 1;
		tap->txa_avgpps = 0;
		tap->txa_lastsample = jiffies;
		return;
	}
	/* XXX bound loop/do more crude estimate? */
	while (jiffies - tap->txa_lastsample >= (HZ/10)) {		//Make avgpps updated every 0.1sec
		ieee80211_txampdu_update_pps(tap);
		/* reset to start new sample interval */
		tap->txa_pkts = 0;
		if (tap->txa_avgpps == 0) {
			tap->txa_lastsample = jiffies;
			break;
		} else
			tap->txa_lastsample += (HZ/10);
	}
	tap->txa_pkts++;
}

static void __inline
tx_update_pps(txACInfo *tap)
{
	/* NB: scale factor of 2 was picked heuristically */
	tap->txa_avgpps = (((tap->txa_avgpps << 2) -
	     tap->txa_avgpps + (tap->txa_pkts*10)) >> 2);		//Multiply 10 to get avgpps per sec, in sync with tx_count_packet
}

/*
 * Count a packet towards the pps estimate.
 */
void __inline
tx_count_packet(txACInfo *tap)
{
	if (jiffies - tap->txa_lastsample >= MAX_SECS_TO_RESET_PPS*HZ)
	{
		tap->txa_pkts = 1;
		tap->txa_avgpps = 0;
		tap->txa_lastsample = jiffies;
		return;
	}
	/* XXX bound loop/do more crude estimate? */
	while (jiffies - tap->txa_lastsample >= (HZ/10)) {	//Make avgpps updated every 0.1sec
		tx_update_pps(tap);
		/* reset to start new sample interval */
		tap->txa_pkts = 0;
		if (tap->txa_avgpps == 0) {
			tap->txa_lastsample = jiffies;
			break;
		} else
			tap->txa_lastsample += (HZ/10);
	}
	tap->txa_pkts++;
}

/*
 * Get the current pps estimate.  If the average is out of
 * date due to lack of traffic then we decay the estimate
 * to account for the idle time.
 */
static int __inline
ieee80211_txampdu_getpps(Ampdu_tx_t *tap)
{
	if (jiffies - tap->txa_lastsample >= MAX_SECS_TO_RESET_PPS*HZ)
	{
		tap->txa_avgpps = 0;
		tap->txa_lastsample = jiffies;
		return 0;
	}
	/* XXX bound loop/do more crude estimate? */
	while (jiffies - tap->txa_lastsample >= (HZ/10)) {	//Make avgpps updated every 0.1sec
		ieee80211_txampdu_update_pps(tap);
		tap->txa_pkts = 0;
		if (tap->txa_avgpps == 0) {
			tap->txa_lastsample = jiffies;
			break;
		} else
			tap->txa_lastsample += (HZ/10);
	}
	return tap->txa_avgpps;
}
#endif

/** Auto Addba function **/
static inline void enableAmpduTx(vmacApInfo_t *vmacSta_p, UINT8 *macaddr, UINT8 tid)
{
	struct wlprivate    *wlpptr   = NETDEV_PRIV_P(struct wlprivate, vmacSta_p->dev);
	extStaDb_StaInfo_t *pStaInfo=NULL;
	int stream = -1;
	UINT8 *ampduMacAddr = macaddr;
#ifdef CLIENT_SUPPORT
	UINT8 AssociatedFlag = 0;
	UINT8 bssId[6];
#endif
#ifdef DYNAMIC_BA_SUPPORT
	MIB_802DOT11 *mib = vmacSta_p->Mib802dot11;
    UINT32 ac, i;

	/*To convert TID value to priority in ascending order. To be used in finding victim stream*/
	/*Currently we treat BE TID0 and TID3 same. We can adjust this in future*/
	/*TID1(BK)==0, TID0(BE)==1, TID4(VI)==2 and etc respectively*/
	UINT8 tidpriority[MAX_TIDS] = {1,0,0,1,2,2,3,3};			

	UINT8 found_stream = 0;							
	UINT8 victim_stream;							
    Ampdu_tx_t *sp;

    mwlbainfo mwl_bainfo[MAX_TIDS];
#endif    
	pStaInfo = extStaDb_GetStaInfo(vmacSta_p,(IEEEtypes_MacAddr_t *)macaddr, 0);

#ifdef AMPDU_SUPPORT_TX_CLIENT
	if (vmacSta_p->VMacEntry.modeOfService == VMAC_MODE_CLNT_INFRA)
	{
		if (!smeGetStaLinkInfo(vmacSta_p->VMacEntry.id,
			&AssociatedFlag,
			&bssId[0]))
		{
			return;
		}
		if (AssociatedFlag)
		{
			ampduMacAddr = &bssId[0];
		}
		else
			return;
	}
#endif    


#ifndef AMPDU_SUPPORT_SBA
	// 1. check if any stream are available
	// 2. if available continue, continue to addstream as per normal
	// 3. else, check if current tid is higer than any of current stream in use,
	// 4. if true, delete the current lower tid stream, wait ~ 3 second, before adding stream 

	if(wlpptr->wlpd_p->Ampdu_tx[0].InUse && wlpptr->wlpd_p->Ampdu_tx[1].InUse )  /** both stream are in use **/

	{
		/** step 2 here **/
		if((AccCategoryQ[tid]> AccCategoryQ[wlpptr->wlpd_p->Ampdu_tx[0].AccessCat]) ||\
			(AccCategoryQ[tid]> AccCategoryQ[wlpptr->wlpd_p->Ampdu_tx[1].AccessCat])  )
		{

			//for stream 0
			if(AccCategoryQ[wlpptr->wlpd_p->Ampdu_tx[0].AccessCat] <= AccCategoryQ[wlpptr->wlpd_p->Ampdu_tx[1].AccessCat])
			{
				/** send delba for stream 0 **/
				if((wlpptr->wlpd_p->Ampdu_tx[0].TimeOut!=0) && (jiffies > wlpptr->wlpd_p->Ampdu_tx[0].TimeOut))
				{
					//printk("TimeOut Occur for stream 0!!!!!!!!!!\n");
					wlpptr->wlpd_p->Ampdu_tx[0].TimeOut=0;
					wlpptr->wlpd_p->Ampdu_tx[0].InUse = 0;
				}
				else
				{
					if(wlpptr->wlpd_p->Ampdu_tx[0].TimeOut==0  && wlpptr->wlpd_p->Ampdu_tx[1].TimeOut==0)
					{
						//	printk("Inside 2 value of 0 = %d va 1 = %d\n",AccCategoryQ[Ampdu_tx[0].AccessCat],AccCategoryQ[Ampdu_tx[1].AccessCat]);
						wlpptr->wlpd_p->Ampdu_tx[0].TimeOut = jiffies + 300;
#ifdef AMPDU_SUPPORT_TX_CLIENT
						if (vmacSta_p->VMacEntry.modeOfService == VMAC_MODE_CLNT_INFRA)
							SendDelBASta(vmacSta_p, (UINT8 *)&wlpptr->wlpd_p->Ampdu_tx[0].MacAddr[0], wlpptr->wlpd_p->Ampdu_tx[0].AccessCat);
						else
#endif
							SendDelBA(vmacSta_p, (UINT8 *)&wlpptr->wlpd_p->Ampdu_tx[0].MacAddr[0], wlpptr->wlpd_p->Ampdu_tx[0].AccessCat);
						wlFwUpdateDestroyBAStream(vmacSta_p->dev, 0,0,0);
						if(pStaInfo)
						{
							pStaInfo->aggr11n.onbytid[tid]=0;
							pStaInfo->aggr11n.startbytid[tid]=0;					
							pStaInfo->aggr11n.type &= ~WL_WLAN_TYPE_AMPDU;
						}
					}
				}
			}
			else
			{
				if((wlpptr->wlpd_p->Ampdu_tx[1].TimeOut!=0) && (jiffies > wlpptr->wlpd_p->Ampdu_tx[1].TimeOut))
				{

					//	printk("TimeOut Occur for stream 1!!!!!!!!!!\n");
					wlpptr->wlpd_p->Ampdu_tx[1].TimeOut=0;
					wlpptr->wlpd_p->Ampdu_tx[1].InUse = 0;
				}
				else
				{
					if(wlpptr->wlpd_p->Ampdu_tx[1].TimeOut==0 && wlpptr->wlpd_p->Ampdu_tx[1].TimeOut==0)
					{
						//	printk("Inside 2 value of 0 = %d va 1 = %d\n",AccCategoryQ[Ampdu_tx[0].AccessCat],AccCategoryQ[Ampdu_tx[1].AccessCat]);
						wlpptr->wlpd_p->Ampdu_tx[1].TimeOut = jiffies + 300;
#ifdef AMPDU_SUPPORT_TX_CLIENT
						if (vmacSta_p->VMacEntry.modeOfService == VMAC_MODE_CLNT_INFRA)
							SendDelBASta(vmacSta_p, (UINT8 *)&wlpptr->wlpd_p->Ampdu_tx[1].MacAddr[0], wlpptr->wlpd_p->Ampdu_tx[1].AccessCat);
						else
#endif
							SendDelBA(vmacSta_p, (UINT8 *)&wlpptr->wlpd_p->Ampdu_tx[1].MacAddr[0], wlpptr->wlpd_p->Ampdu_tx[1].AccessCat);
						wlFwUpdateDestroyBAStream(vmacSta_p->dev,0,0,1);
						if(pStaInfo)
						{
							pStaInfo->aggr11n.onbytid[tid]=0;
							pStaInfo->aggr11n.startbytid[tid]=0;					
							pStaInfo->aggr11n.type &= ~WL_WLAN_TYPE_AMPDU;
						}
					}
				}
			}

		}
	}
#endif /* _AMPDU_SUPPORT_SBA */ 
#ifdef DYNAMIC_BA_SUPPORT
    /* Find Available Stream;
       also Allocate S/W BA stream for AC_BK traffic first */
        if (TID_TO_WME_AC(tid) == WME_AC_BK) {
            for(i = (AMPDU_STREAM_NO_END - 1); i >= 0; i--)
            {
                if(wlpptr->wlpd_p->Ampdu_tx[i].InUse != 1 ) 
				{
                    stream = i;
                    break;
                }
            }
        }
        else   
		{
            for(i = 0; i < AMPDU_STREAM_NO_END; i++)
			{
                if(wlpptr->wlpd_p->Ampdu_tx[i].InUse != 1 ) 
				{
                    stream = i;
                    break;            
                }
            }
        }
		
		if (stream == -1) {
			/*
			 * No available stream, return 0 so no
			 * a-mpdu aggregation will be done.
			 */
			if (!(*(mib->mib_ampdu_bamgmt))) {
				return;
			} else {
				/* 
				 * Check stats of current AMPDU streams to see whether it is possible to tear
				 * down an existing stream with low activities
				 */
				memset(mwl_bainfo, 0, sizeof(mwl_bainfo));				 	
				
				for (i = 0; i < AMPDU_STREAM_NO_END; i++) {
                    Ampdu_tx_t  *t;
                    t = &wlpptr->wlpd_p->Ampdu_tx[i];
                    
                    if (t->InUse != 1) 
						continue;
                   
					if ((t->ReleaseTimestamp>0) && (jiffies - (t->ReleaseTimestamp))> 5*HZ) 
					{    
						mwl_bainfo[i].flag = 1;
						mwl_bainfo[i].sp = t;
						mwl_bainfo[i].pps = ieee80211_txampdu_getpps(t);
						mwl_bainfo[i].tid = t->AccessCat;
					}

					
					/*Find a victim stream to be evicted based on tid priority and pps in current AMPDU tcqs that are running > 5sec*/
					/*We only consider streams that are well established and not keep deleting stream, thus >5sec criteria*/
					/*As we go thru stream by stream, we make decision whether to use current or victim stream*/
				
					/*We compare current tid priority with victim tid priority. 3 cases: current tid priority lower, higher or same */
					if(!found_stream)	
					{	/*Assign first victim having flag==1. This victim is used later for comparison*/
						if(mwl_bainfo[i].flag)
						{
							victim_stream = i;
							found_stream = 1;
							continue;
						}
					} 
					else 
					{	
						/*One victim stream is found, compare curent with victim here*/
						if(mwl_bainfo[i].flag)
						{	
							if(tidpriority[mwl_bainfo[i].tid] < tidpriority[mwl_bainfo[victim_stream].tid])
							{	
								/*If victim stream has higher priority but pps==0, we use same victim stream*/
								/*Otherwise, we use current stream with lower tid priority*/
								if((mwl_bainfo[i].pps != 0) && (mwl_bainfo[victim_stream].pps == 0))							
									continue;
								else 	
									victim_stream = i;
							}
							else if (tidpriority[mwl_bainfo[i].tid] > tidpriority[mwl_bainfo[victim_stream].tid])
							{
								/*If current stream has higher priority but pps==0, we use same current stream*/
								/*Otherwise, we use victim stream with lower tid priority*/
								if ((mwl_bainfo[i].pps == 0) && (mwl_bainfo[victim_stream].pps != 0))
									victim_stream = i;
								else	
									continue;
							}
							else if (tidpriority[mwl_bainfo[i].tid] == tidpriority[mwl_bainfo[victim_stream].tid])
							{
								/*If current stream same priority but lower or same pps than victim stream, we use current stream*/
								/*Otherwise, we use victim stream*/
								if(mwl_bainfo[i].pps <= mwl_bainfo[victim_stream].pps)
									victim_stream = i;
								else 							
									continue;
							}
							
						}
					}	
				}
				
				sp = NULL;
				
				
				/*After found a victim stream, we have to compare incoming tid, then pps to make sure we don't simply evict*/
				/*All running streams should have highest tid that have non-zero pps*/
				
				if (!found_stream)
					return;
				else
				{	
					ac = TID_TO_WME_AC(mwl_bainfo[victim_stream].tid);
					
					/*Always evict victim when incoming stream has higher tid priority*/
					if((tidpriority[mwl_bainfo[victim_stream].tid]) < tidpriority[tid])
						sp = mwl_bainfo[victim_stream].sp;
					else if ((tidpriority[mwl_bainfo[victim_stream].tid]) > tidpriority[tid])
					{	
						/*If victim stream has higher tid priority than incoming but pps ==0, evict victim*/
						/*We don't keep higher tid priority with pps==0*/
						if (mwl_bainfo[victim_stream].pps < *(mib->mib_ampdu_low_AC_thres[ac]))
						{	
							if ((mwl_bainfo[victim_stream].pps == 0))
								sp = mwl_bainfo[victim_stream].sp;	
							else 
								return;
						}
						else
							return;
					}
					else if ((tidpriority[mwl_bainfo[victim_stream].tid]) == tidpriority[tid])
					{	
						/*If victim stream has same tid priority as incoming but pps==0, evict victim*/
						/*To minimize impact on running traffic, we don't del if victim pps < incoming pps*/
						if (mwl_bainfo[victim_stream].pps < *(mib->mib_ampdu_low_AC_thres[ac]))
						{
							if ((mwl_bainfo[victim_stream].pps ==0))
								sp = mwl_bainfo[victim_stream].sp;
							else 
								return;
						}
						else
							return;
					}
				}

							
				/* Cannot tear down existing stream. We simply run out of AMPDU streams! */
				if (sp == NULL) 
					return;
               		 			
				
				/* Evicts the unlucky one */
				sp->ReleaseTimestamp = jiffies;
                disableAmpduTx(vmacSta_p,&sp->MacAddr[0], sp->AccessCat);
				return;
				/*
				 * Cannot reclaim the just released stream 
				 * right here since FW needs some time to clean it up.
				 * So let  retries to takes care of it. 
				 */				
			}
		}
        
        /* Check if stream can be allocated in the firmware */
        if (pStaInfo)
        {
        	if(wlFwCheckBAStream(vmacSta_p->dev, 64, 63 , macaddr, wlpptr->wlpd_p->Global_DialogToken,
                tid, 1, stream,pStaInfo->HtElem.MacHTParamInfo) != SUCCESS) 
                     	return;
        
        }
        else 		
            return;
        	
        /* Stream Allocated prepare rest of the information on the stream */
#else
	for(stream = AMPDU_STREAM_NO_START; stream < AMPDU_STREAM_NO_END; stream++)
#endif
		if(wlpptr->wlpd_p->Ampdu_tx[stream].InUse != 1 ) 
        {
			wlpptr->wlpd_p->Ampdu_tx[stream].MacAddr[0]=ampduMacAddr[0];
			wlpptr->wlpd_p->Ampdu_tx[stream].MacAddr[1]=ampduMacAddr[1];
			wlpptr->wlpd_p->Ampdu_tx[stream].MacAddr[2]=ampduMacAddr[2];
			wlpptr->wlpd_p->Ampdu_tx[stream].MacAddr[3]=ampduMacAddr[3];
			wlpptr->wlpd_p->Ampdu_tx[stream].MacAddr[4]=ampduMacAddr[4];
			wlpptr->wlpd_p->Ampdu_tx[stream].MacAddr[5]=ampduMacAddr[5];
			wlpptr->wlpd_p->Ampdu_tx[stream].AccessCat = tid;
			wlpptr->wlpd_p->Ampdu_tx[stream].InUse = 1;
			wlpptr->wlpd_p->Ampdu_tx[stream].TimeOut = 0;
			wlpptr->wlpd_p->Ampdu_tx[stream].AddBaResponseReceive=0;
			wlpptr->wlpd_p->Ampdu_tx[stream].ReleaseTimestamp=jiffies; 
			wlpptr->wlpd_p->Ampdu_tx[stream].DialogToken=wlpptr->wlpd_p->Global_DialogToken;
			wlpptr->wlpd_p->Global_DialogToken=(wlpptr->wlpd_p->Global_DialogToken+1)%63;
#ifdef DYNAMIC_BA_SUPPORT
            wlpptr->wlpd_p->Ampdu_tx[stream].txa_ac = TID_TO_WME_AC(tid);
#endif
			if(wlpptr->wlpd_p->Ampdu_tx[stream].initTimer==0)
			{
				TimerInit(&wlpptr->wlpd_p->Ampdu_tx[stream].timer);
				wlpptr->wlpd_p->Ampdu_tx[stream].initTimer= 1;
			}
			TimerDisarm(&wlpptr->wlpd_p->Ampdu_tx[stream].timer);
			wlpptr->wlpd_p->Ampdu_tx[stream].vmacSta_p = vmacSta_p;
            wlFwGetSeqNoBAStream(vmacSta_p->dev, macaddr, tid, 
               (UINT16 *)&(wlpptr->wlpd_p->Ampdu_tx[stream].start_seqno));
            
#ifdef AMPDU_SUPPORT_TX_CLIENT            			            
			if (vmacSta_p->VMacEntry.modeOfService == VMAC_MODE_CLNT_INFRA)
			{
				TimerFireIn(&wlpptr->wlpd_p->Ampdu_tx[stream].timer, 1, &AddbaTimerProcessSta, (UINT8 *)&wlpptr->wlpd_p->Ampdu_tx[stream], ADDBA_PERIOD_1SEC);
				SendAddBAReqSta(vmacSta_p, macaddr, tid,1, wlpptr->wlpd_p->Ampdu_tx[stream].start_seqno,wlpptr->wlpd_p->Ampdu_tx[stream].DialogToken);
			}
			else
#endif
			{
				TimerFireIn(&wlpptr->wlpd_p->Ampdu_tx[stream].timer, 1, &AddbaTimerProcess, (UINT8 *)&wlpptr->wlpd_p->Ampdu_tx[stream], ADDBA_PERIOD_1SEC);
				SendAddBAReq(vmacSta_p, macaddr, tid,1, wlpptr->wlpd_p->Ampdu_tx[stream].start_seqno,wlpptr->wlpd_p->Ampdu_tx[stream].DialogToken);  /** Only support immediate ba **/
			}
			if(pStaInfo)
			{
				pStaInfo->aggr11n.type |= WL_WLAN_TYPE_AMPDU;
				pStaInfo->aggr11n.startbytid[tid]=1;	
			}
			
			return;
		}
}


void disableAmpduTxMacAddr(vmacApInfo_t *vmacSta_p,UINT8 *macaddr)
{
	struct wlprivate    *wlpptr   = NETDEV_PRIV_P(struct wlprivate, vmacSta_p->dev);
	extStaDb_StaInfo_t *pStaInfo=NULL;
	int i,j, tid;
	UINT8 *ampduMacAddr = macaddr;
#ifdef CLIENT_SUPPORT
	UINT8 AssociatedFlag = 0;
	UINT8 bssId[6];
#endif
	pStaInfo = extStaDb_GetStaInfo(vmacSta_p,(IEEEtypes_MacAddr_t *)macaddr, 0);
#ifdef AMPDU_SUPPORT_TX_CLIENT
	if (vmacSta_p->VMacEntry.modeOfService == VMAC_MODE_CLNT_INFRA)
	{
		if (!smeGetStaLinkInfo(vmacSta_p->VMacEntry.id,
			&AssociatedFlag,
			&bssId[0]))
		{
			return;
		}
		if (AssociatedFlag)
		{
			ampduMacAddr = &bssId[0];
		}
		else
			return;
	}
#endif

	for(i=AMPDU_STREAM_NO_START;i<AMPDU_STREAM_NO_END;i++)
	{
		//if(wlpptr->wlpd_p->Ampdu_tx[i].InUse == 1)
		{
			if(!MACADDR_CMP(wlpptr->wlpd_p->Ampdu_tx[i].MacAddr, ampduMacAddr))
			{	
				tid = wlpptr->wlpd_p->Ampdu_tx[i].AccessCat;		
				wlpptr->wlpd_p->Ampdu_tx[i].InUse=0;
				wlpptr->wlpd_p->Ampdu_tx[i].TimeOut = 0;
#ifdef AMPDU_SUPPORT_TX_CLIENT
				if (vmacSta_p->VMacEntry.modeOfService == VMAC_MODE_CLNT_INFRA)
					SendDelBASta(vmacSta_p,ampduMacAddr, tid);
				else
#endif
					SendDelBA(vmacSta_p,ampduMacAddr, tid);
				wlFwUpdateDestroyBAStream(vmacSta_p->dev,0,0,i);
				if(pStaInfo)
				{
					pStaInfo->aggr11n.onbytid[tid]=0;
					pStaInfo->aggr11n.startbytid[tid]=0;
					pStaInfo->aggr11n.type &= ~WL_WLAN_TYPE_AMPDU;
				}
				for(j=0;j<6;j++)
				{
					wlpptr->wlpd_p->Ampdu_tx[i].MacAddr[j]=0;
				}
				return;	
			}
		}
	}
}



void disableAmpduTx(vmacApInfo_t *vmacSta_p,UINT8 *macaddr, UINT8 tid)
{
	struct wlprivate    *wlpptr   = NETDEV_PRIV_P(struct wlprivate, vmacSta_p->dev);
	extStaDb_StaInfo_t *pStaInfo=NULL;
	int i,j;
	UINT8 *ampduMacAddr = macaddr;
#ifdef CLIENT_SUPPORT
	UINT8 AssociatedFlag = 0;
	UINT8 bssId[6];
#endif
	pStaInfo = extStaDb_GetStaInfo(vmacSta_p,(IEEEtypes_MacAddr_t *)macaddr, 0);
#ifdef AMPDU_SUPPORT_TX_CLIENT
	if (vmacSta_p->VMacEntry.modeOfService == VMAC_MODE_CLNT_INFRA)
	{
		if (!smeGetStaLinkInfo(vmacSta_p->VMacEntry.id,
			&AssociatedFlag,
			&bssId[0]))
		{
			return;
		}
		if (AssociatedFlag)
		{
			ampduMacAddr = &bssId[0];
		}
		else
			return;
	}
#endif

	for(i=AMPDU_STREAM_NO_START;i<AMPDU_STREAM_NO_END;i++)
	{
		//if(wlpptr->wlpd_p->Ampdu_tx[i].InUse == 1)
		{
			if(!MACADDR_CMP(wlpptr->wlpd_p->Ampdu_tx[i].MacAddr, ampduMacAddr))
			{
				/** they are equal **/
				if(wlpptr->wlpd_p->Ampdu_tx[i].AccessCat == tid /*&& wlpptr->wlpd_p->Ampdu_tx[i].InUse==1*/)
				{
					wlpptr->wlpd_p->Ampdu_tx[i].InUse=0;
					wlpptr->wlpd_p->Ampdu_tx[i].TimeOut = 0;
#ifdef AMPDU_SUPPORT_TX_CLIENT
					if (vmacSta_p->VMacEntry.modeOfService == VMAC_MODE_CLNT_INFRA)
						SendDelBASta(vmacSta_p,ampduMacAddr, tid);
					else
#endif
						SendDelBA(vmacSta_p,ampduMacAddr, tid);
					wlFwUpdateDestroyBAStream(vmacSta_p->dev,0,0,i);
					if(pStaInfo)
					{
						pStaInfo->aggr11n.onbytid[tid]=0;
						pStaInfo->aggr11n.startbytid[tid]=0;
						pStaInfo->aggr11n.type &= ~WL_WLAN_TYPE_AMPDU;
					}
					for(j=0;j<6;j++)
					{
						wlpptr->wlpd_p->Ampdu_tx[i].MacAddr[j]=0;

					}
					return;
				}
			}
		}
	}
}
void cleanupAmpduTx(vmacApInfo_t *vmacSta_p,UINT8 *macaddr)
{
	int i;
	for (i = 0; i<=7; i++)
		disableAmpduTx(vmacSta_p,macaddr, i);
}
#endif


void disableAmpduTxAll(vmacApInfo_t *vmacSta_p)
{
	int i,j;
	struct wlprivate    *wlpptr   = NETDEV_PRIV_P(struct wlprivate, vmacSta_p->dev);
	extStaDb_StaInfo_t *pStaInfo=NULL;



	for(i=AMPDU_STREAM_NO_START;i<AMPDU_STREAM_NO_END;i++)
	{
		if(wlpptr->wlpd_p->Ampdu_tx[i].InUse == 1)
		{
			/** they are equal **/
			//	printk("They match in delba stream = %d!!!!tid==%d\n",i, tid);

			SendDelBA(wlpptr->wlpd_p->Ampdu_tx[i].vmacSta_p,wlpptr->wlpd_p->Ampdu_tx[i].MacAddr,wlpptr->wlpd_p->Ampdu_tx[i].AccessCat);
			wlFwUpdateDestroyBAStream(wlpptr->wlpd_p->Ampdu_tx[i].vmacSta_p->dev,0,0,i);
			wlpptr->wlpd_p->Ampdu_tx[i].InUse=0;
			wlpptr->wlpd_p->Ampdu_tx[i].TimeOut = 0;
			pStaInfo = extStaDb_GetStaInfo(wlpptr->wlpd_p->Ampdu_tx[i].vmacSta_p,(IEEEtypes_MacAddr_t *)&wlpptr->wlpd_p->Ampdu_tx[i].MacAddr[0], 0);
			if(pStaInfo)
			{
				pStaInfo->aggr11n.type &= ~WL_WLAN_TYPE_AMPDU;
				pStaInfo->aggr11n.startbytid[wlpptr->wlpd_p->Ampdu_tx[i].AccessCat]=0;	
                
			}


			for(j=0;j<6;j++)
			{
				wlpptr->wlpd_p->Ampdu_tx[i].MacAddr[j]=0;
			}
		}
	}
}

void disableAmpduTxstream(vmacApInfo_t *vmacSta_p,int stream)
{
	extStaDb_StaInfo_t *pStaInfo=NULL;
	struct wlprivate    *wlpptr   = NETDEV_PRIV_P(struct wlprivate, vmacSta_p->dev);

	if(wlpptr->wlpd_p->Ampdu_tx[stream].InUse == 1)
	{
		/** they are equal **/
		//	printk("They match in delba stream = %d!!!!tid==%d\n",i, tid);
		int j;
		SendDelBA(wlpptr->wlpd_p->Ampdu_tx[stream].vmacSta_p,wlpptr->wlpd_p->Ampdu_tx[stream].MacAddr,wlpptr->wlpd_p->Ampdu_tx[stream].AccessCat);
		wlFwUpdateDestroyBAStream(wlpptr->wlpd_p->Ampdu_tx[stream].vmacSta_p->dev,0,0,stream);
		wlpptr->wlpd_p->Ampdu_tx[stream].InUse=0;
		wlpptr->wlpd_p->Ampdu_tx[stream].TimeOut = 0;
		pStaInfo = extStaDb_GetStaInfo(wlpptr->wlpd_p->Ampdu_tx[stream].vmacSta_p,(IEEEtypes_MacAddr_t *)&wlpptr->wlpd_p->Ampdu_tx[stream].MacAddr[0], 0);
		if(pStaInfo)
		{
			pStaInfo->aggr11n.type &= ~WL_WLAN_TYPE_AMPDU;
			pStaInfo->aggr11n.startbytid[wlpptr->wlpd_p->Ampdu_tx[stream].AccessCat]=0;	
            
		}
		for(j=0;j<6;j++)
		{
			wlpptr->wlpd_p->Ampdu_tx[stream].MacAddr[j]=0;
		}
	}
}


struct sk_buff *ieee80211_encap(struct sk_buff *skb, struct net_device *netdev, BOOLEAN eap)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	vmacApInfo_t *vmacSta_p = wlpptr->vmacSta_p;
	MIB_802DOT11 *mib = vmacSta_p->Mib802dot11;
	struct ether_header eh;
	struct llc *llc;
	struct llc_rptr *llc_rptr = NULL;
	struct ieee80211_qosframe *wh;
	extStaDb_StaInfo_t *pStaInfo=NULL;
	UINT8 Priority=0,tid;
	IEEEtypes_QoS_Ctl_t QosControl;
#ifdef CLIENT_SUPPORT
	vmacEntry_t  *vmacEntry_p = (vmacEntry_t  *) wlpptr->clntParent_priv_p;
	UINT8 AssociatedFlag = 0;
	UINT8 bssId[6];
#endif
	UINT8 *ampduMacAddr = NULL;
#ifdef WDS_FEATURE
	BOOLEAN wds = FALSE;
	struct wds_port *pWdsPort = NULL;
#endif
	eth_StaInfo_t *ethStaInfo_p;
	UINT8 ampdu_ready = 1;
#ifdef RD2_BOARD
	skb_trim(skb, skb->len-2);
#endif

	tid=Qos_GetDSCPPriority(skb->data);

	memcpy(&eh, skb->data, sizeof(struct ether_header));
	skb_pull(skb, sizeof(struct ether_header));

	skb = wlan_skbhdr_adjust(skb);
	if (skb == NULL)
	{
		goto bad;
	}
	
#ifdef CLIENT_SUPPORT
	if (vmacSta_p->VMacEntry.modeOfService == VMAC_MODE_CLNT_INFRA)
	{	
		if (vmacEntry_p == NULL)
			goto bad;
		if (!vmacEntry_p->active)
		{   			
			if (*(mib->mib_STAMacCloneEnable) == 1)
			{   
				UINT8  mlmeAssociatedFlag;
				UINT8  mlmeBssid[6];
				struct net_device *dev2;
                
				/* don't clone if packet (source mac) comes from itself or lan bridge */
				if (memcmp(eh.ether_shost, netdev->dev_addr, 6) == 0) {
					// printk("### Debug %s, source MAC %s is same as %s\n", __func__, mac_display(eh.ether_shost), netdev->name);
					goto bad;
				}

				if ((dev2 = dev_get_by_name(&init_net, "br0")) != NULL) {
					if (memcmp(eh.ether_shost, dev2->dev_addr, 6) == 0) {
						// printk("### Debug %s, source MAC %s is same as %s\n", __func__, mac_display(eh.ether_shost), dev2->name);
						goto bad;
					}
				}

				smeGetStaLinkInfo(vmacEntry_p->id, &mlmeAssociatedFlag, &mlmeBssid[0]);

                /* if there was an entry in fw, from a previous association, remove it*/
				wlFwRemoveMacAddr(vmacSta_p->dev, &vmacEntry_p->vmacAddr[0]);			

                if (mlmeAssociatedFlag)
				{
#ifdef AMPDU_SUPPORT_TX_CLIENT
					cleanupAmpduTx(vmacSta_p, (UINT8 *)&mlmeBssid[0]);
#endif
				}

				// This is a quick fix for setting parent client address.			
				memcpy(&vmacEntry_p->vmacAddr[0], eh.ether_shost, 6); 

				wlFwSetMacAddr_Client(vmacSta_p->dev, &vmacEntry_p->vmacAddr[0]);

				printk("Cloned MAC address = %02x:%02x:%02x:%02x:%02x:%02x \n", vmacEntry_p->vmacAddr[0],
					vmacEntry_p->vmacAddr[1], vmacEntry_p->vmacAddr[2], vmacEntry_p->vmacAddr[3],
					vmacEntry_p->vmacAddr[4], vmacEntry_p->vmacAddr[5]);

				WLSYSLOG(netdev, WLSYSLOG_CLASS_ALL, WLSYSLOG_MSG_CLIENT_CLONED "%02x%02x%02x%02x%02x%02x\n",
					vmacEntry_p->vmacAddr[0], vmacEntry_p->vmacAddr[1], vmacEntry_p->vmacAddr[2], 
					vmacEntry_p->vmacAddr[3], vmacEntry_p->vmacAddr[4], vmacEntry_p->vmacAddr[5]);

#ifdef MRVL_WPS_CLIENT
				WLSNDEVT(netdev, IWEVCUSTOM, 
					(IEEEtypes_MacAddr_t *)&vmacEntry_p->vmacAddr[0],
					WLSYSLOG_MSG_CLIENT_CLONED);
#endif //MRVL_WPS_CLIENT

				if (mlmeAssociatedFlag)
				{
					linkMgtReStart(vmacEntry_p->phyHwMacIndx, vmacEntry_p);
				}
				else
					wlLinkMgt(netdev, vmacEntry_p->phyHwMacIndx);
			}
			vmacEntry_p->active = 1;				
		}
		else
		{
			smeGetStaLinkInfo(vmacEntry_p->id,
				&AssociatedFlag,
				&bssId[0]);
			if (!AssociatedFlag)
				goto bad;
#ifdef CLIENT_SUPPORT_MULTIPLECLIENT
			if ((pStaInfo = extStaDb_GetStaInfo(vmacSta_p,&(eh.ether_shost), 1)) == NULL)
			{
				printk("Client not in database %s start child session \n", mac_display(&(eh.ether_shost)));
				wlFwSetMacAddr_Client(sme_GetParentPrivInfo(vmacEntry_p->phyHwMacIndx), &(eh.ether_shost));
				child_VMacEntry_p =  smeStartChildSession(vmacEntry_p->phyHwMacIndx, 
					&(eh.ether_shost),
					0xf,
					&wlStatusUpdate_clientParent,
					1,
					sme_GetParentPrivInfo(vmacEntry_p->phyHwMacIndx));
				goto bad;
			}
#endif  
		}
		if(IS_GROUP((UINT8 *) &(eh.ether_dhost)) && (*(mib->mib_STAMacCloneEnable) == 2)) {
			if (ethStaDb_AddSta(vmacSta_p, &(eh.ether_shost), NULL) == TABLE_FULL_ERROR) 
				goto bad;
		}
	}
	else
#endif
	{
#ifdef WDS_FEATURE
		if (*(mib->mib_wdsEnable))
		{
			pWdsPort = getWdsPortFromNetDev(wlpptr, netdev);
			if (pWdsPort != NULL)
				wds = TRUE;
			else if (*(mib->mib_disableAssoc) ) 	
				goto bad;			
		}
#endif
	}
#ifdef WDS_FEATURE
	if(!IS_GROUP((UINT8 *) &(eh.ether_dhost)) || wds || (vmacSta_p->VMacEntry.modeOfService == VMAC_MODE_CLNT_INFRA))
	{
		if (wds)
		{            
			pStaInfo = extStaDb_GetStaInfo(vmacSta_p,&(pWdsPort->wdsMacAddr), 1);
		}
		else
#else                
	if(!IS_GROUP((UINT8 *) &(eh.ether_dhost)))
	{
#endif
		{
#ifdef CLIENT_SUPPORT
			if (vmacSta_p->VMacEntry.modeOfService == VMAC_MODE_CLNT_INFRA)
				pStaInfo = extStaDb_GetStaInfo(vmacSta_p,(IEEEtypes_MacAddr_t *)&bssId[0], 1);
			else
#endif
				if ((pStaInfo = extStaDb_GetStaInfo(vmacSta_p, &(eh.ether_dhost), 1)) == NULL)
				{
					if (*(mib->mib_RptrMode))
					{
						if ((ethStaInfo_p = ethStaDb_GetStaInfo(vmacSta_p, &(eh.ether_dhost), 1)) != NULL )	{							
							pStaInfo = ethStaInfo_p->pStaInfo_t;
							if (pStaInfo && (pStaInfo->StaType & 0x02) != 0x02)
								goto bad;
						} else {
							goto bad;
						}
					}
				}

		}
		if(pStaInfo && pStaInfo->aggr11n.threshold)
		{
			pStaInfo->aggr11n.txcnt++;
			if(pStaInfo->aggr11n.on)
			{
			}else if(pStaInfo->aggr11n.start)
			{
				pStaInfo->aggr11n.on = 1;
			}
#ifdef AMPDU_SUPPORT
			if(!eap)
			{
				switch(*(mib->mib_AmpduTx))
				{
				case 3:
					pStaInfo->aggr11n.txcntbytid[tid & 0x7]++;
					if(pStaInfo->aggr11n.startbytid[tid &0x07]==0)
					{
						if(pStaInfo->aggr11n.onbytid[tid & 0x7])
						{
							enableAmpduTx(vmacSta_p,pStaInfo->Addr, tid);
							pStaInfo->aggr11n.startbytid[tid] = 1;
						}
					}
					else
					{
						if(!pStaInfo->aggr11n.onbytid[tid & 0x7])
						{
							disableAmpduTx(vmacSta_p,pStaInfo->Addr, tid);
							pStaInfo->aggr11n.startbytid[tid] = 0;
						}
					}
					break;
				case 1:
#ifdef DYNAMIC_BA_SUPPORT
                    if ((*(mib->mib_ampdu_bamgmt)))
                        tx_count_packet(&pStaInfo->aggr11n.tx_ac_info[(tid & 0x07)]);
#endif                    
					if(pStaInfo->aggr11n.threshold && pStaInfo->aggr11n.startbytid[tid&0x07] == 0)
					{
#ifdef DYNAMIC_BA_SUPPORT
					    if ((!(*(mib->mib_ampdu_bamgmt))) || 
                            ( pStaInfo->aggr11n.tx_ac_info[(tid & 0x07)].txa_avgpps > 
                           *(mib->mib_ampdu_mintraffic[TID_TO_WME_AC(tid & 0x07)])) ) {
#endif
                            enableAmpduTx(vmacSta_p, pStaInfo->Addr, tid);
 #ifdef DYNAMIC_BA_SUPPORT
                        }
#endif
					}
					break;
				default:
					break;
				}
			}
#endif
		}
	}
#ifdef CLIENT_SUPPORT
	if ((vmacSta_p->VMacEntry.modeOfService == VMAC_MODE_CLNT_INFRA) && !(*(mib->mib_STAMacCloneEnable) == 2)) 
	{
		if (memcmp(&vmacEntry_p->vmacAddr[0], eh.ether_shost, 6))
		{
			goto bad;
		}
	}
#endif

	if ((pStaInfo && (pStaInfo->StaType & 0x02) == 0x02) 
#ifdef CLIENT_SUPPORT	
		|| ((*(mib->mib_STAMacCloneEnable) == 2) && (vmacSta_p->VMacEntry.modeOfService == VMAC_MODE_CLNT_INFRA))
#endif
	)
	{
		llc_rptr = (struct llc_rptr *) skb_push(skb, sizeof(struct llc_rptr));
		llc_rptr->llc.llc_dsap = llc_rptr->llc.llc_ssap = LLC_SNAP_LSAP;
		llc_rptr->llc.llc_control = LLC_UI;
		llc_rptr->llc.llc_snap.org_code[0] = 0x00; /* Rptr OUI 0x004096*/
		llc_rptr->llc.llc_snap.org_code[1] = 0x40;
		llc_rptr->llc.llc_snap.org_code[2] = 0x96;
		llc_rptr->llc.llc_snap.ether_type = htons(RPTR_ETHERTYPE);
		IEEE80211_ADDR_COPY(llc_rptr->eh.ether_dhost, eh.ether_dhost);
		IEEE80211_ADDR_COPY(llc_rptr->eh.ether_shost, eh.ether_shost);
		llc_rptr->eh.ether_type = eh.ether_type;
	} else 	
	{
	llc = (struct llc *) skb_push(skb, sizeof(struct llc));
	llc->llc_dsap = llc->llc_ssap = LLC_SNAP_LSAP;
	llc->llc_control = LLC_UI;
	llc->llc_snap.org_code[0] = 0;
	llc->llc_snap.org_code[1] = 0;
	llc->llc_snap.org_code[2] = 0;
	llc->llc_snap.ether_type = eh.ether_type;
	}

	wh = (struct ieee80211_qosframe *)skb_push(skb, sizeof(struct ieee80211_qosframe));

	*(UINT16 *)&wh->FrmCtl = 0;
	wh->FrmCtl.ProtocolVersion = IEEEtypes_PROTOCOL_VERSION;
	wh->FrmCtl.Type = IEEE_TYPE_DATA;

	
#ifdef CLIENT_SUPPORT
	//moved counter to syncSrv_BncRecvAssociatedHandler
	//if ((!ProbeReqOnTx) && ClientModeTxMonitor && (vmacSta_p->VMacEntry.modeOfService == VMAC_MODE_CLNT_INFRA))
		//ClientModeDataCount[vmacSta_p->VMacEntry.phyHwMacIndx]++;		
#endif


	*(UINT16 *)&wh->dur[0] = 0;
	/** todo check qos pri here **/
#ifdef WDS_FEATURE
	if(!IS_GROUP((UINT8 *) &(eh.ether_dhost)) || wds || (vmacSta_p->VMacEntry.modeOfService == VMAC_MODE_CLNT_INFRA))
	{

		if (((pStaInfo == NULL) || 
			((vmacSta_p->VMacEntry.modeOfService != VMAC_MODE_CLNT_INFRA) && (pStaInfo->State != ASSOCIATED))) 
			&& !wds)
#else
	if(!IS_GROUP((UINT8 *) &(eh.ether_dhost)))
	{

		if ((pStaInfo == NULL) || 
			((vmacSta_p->VMacEntry.modeOfService != VMAC_MODE_CLNT_INFRA)) && 
			(pStaInfo->State != ASSOCIATED))
#endif
		{
			wlpptr->netDevStats.tx_dropped++;
			wlpptr->netDevStats.tx_aborted_errors++;
			dev_kfree_skb_any(skb);
			return NULL;
		}
		if(*(mib->QoSOptImpl))
		{
			/** Foo todo need to check if station is qos capable **/
			if(pStaInfo->IsStaQSTA)
			{
				UINT8 i;
				Priority=AccCategoryQ[tid];


				*(UINT16 *)&QosControl = 0;
				QosControl.tid = tid;
#ifdef AMPDU_SUPPORT// ; assume only one stream is supported now
				//	printk("Value of priority = %d tx[0]= %d tx[1]=%d\n",Priority ,AccCategoryQ[wlpptr->wlpd_p->Ampdu_tx[0].AccessCat],AccCategoryQ[wlpptr->wlpd_p->Ampdu_tx[1].AccessCat]);
#ifdef AMPDU_SUPPORT_TX_CLIENT
				if (vmacSta_p->VMacEntry.modeOfService == VMAC_MODE_CLNT_INFRA)
					ampduMacAddr = &bssId[0];
				else
#endif
					if (pStaInfo && (pStaInfo->StaType & 0x02) == 0x02)
						ampduMacAddr = pStaInfo->Addr;
					else
					ampduMacAddr = eh.ether_dhost;
#ifdef WDS_FEATURE
                if (wds)
                    ampduMacAddr = pWdsPort->wdsMacAddr;
#endif
#ifndef AMPDU_SUPPORT_SBA
				if(!MACADDR_CMP(wlpptr->wlpd_p->Ampdu_tx[0].MacAddr, ampduMacAddr) && wlpptr->wlpd_p->Ampdu_tx[0].InUse)
				{
					if(!wlpptr->wlpd_p->Ampdu_tx[0].AddBaResponseReceive)
						goto bad;

					if(tid == wlpptr->wlpd_p->Ampdu_tx[0].AccessCat)
					{
						QosControl.ack_policy = 3;
					}
				}
				if(!MACADDR_CMP(wlpptr->wlpd_p->Ampdu_tx[1].MacAddr, ampduMacAddr) && wlpptr->wlpd_p->Ampdu_tx[1].InUse)
				{
					if(!wlpptr->wlpd_p->Ampdu_tx[1].AddBaResponseReceive)
						goto bad;


					if(tid == wlpptr->wlpd_p->Ampdu_tx[1].AccessCat)
					{
						QosControl.ack_policy = 3;
					}
				}
#else /* _AMPDU_SUPPORT_SBA */
				for(i=AMPDU_STREAM_NO_START;i<AMPDU_STREAM_NO_END;i++)
				{
					if(!MACADDR_CMP(wlpptr->wlpd_p->Ampdu_tx[i].MacAddr, ampduMacAddr) && wlpptr->wlpd_p->Ampdu_tx[i].InUse  )
					{
						if(!wlpptr->wlpd_p->Ampdu_tx[i].AddBaResponseReceive)
						{
							ampdu_ready = 0;
							break;
						}
						if(tid == wlpptr->wlpd_p->Ampdu_tx[i].AccessCat)
						{
							QosControl.ack_policy = 3;
#ifdef DYNAMIC_BA_SUPPORT
                           if ((*(mib->mib_ampdu_bamgmt)))
                                ieee80211_txampdu_count_packet(&wlpptr->wlpd_p->Ampdu_tx[i]);
#endif
							break;
						}
					}
				}



#endif /* _AMPDU_SUPPORT_SBA */

#endif		

				wh->FrmCtl.Subtype = QoS_DATA;
				*(UINT16 *)&wh->seq[0] = EdcaSeqNum[pStaInfo->StnId][Priority] << 4;
				EdcaSeqNum[pStaInfo->StnId][Priority]++;
				//if(pStaInfo->aggr11n.on && pStaInfo->aggr11n.nextpktnoaggr== 0 && !pStaInfo->aggr11n.type)
				//{
				//	*(UINT16 *)&wh->qos[0]=(*(UINT16 *)&QosControl ) | 0x0080;
				//}else
				*(UINT16 *)&wh->qos[0]=*(UINT16 *)&QosControl ;
			}
			else
			{
				//non qos station
				wh->FrmCtl.Subtype = 0;
				*(UINT16 *)&wh->seq[0] =cpu_to_le16(0 << IEEE80211_SEQ_SEQ_SHIFT);
				*(UINT16 *)&wh->qos[0]=0x0;

			}
		} 
		else
		{
			wh->FrmCtl.Subtype = 0;
			*(UINT16 *)&wh->seq[0] =cpu_to_le16(0 << IEEE80211_SEQ_SEQ_SHIFT);
			*(UINT16 *)&wh->qos[0]=0x0;
		}
	} else
	{
		wh->FrmCtl.Subtype = 0;
		*(UINT16 *)&wh->seq[0] =cpu_to_le16(0 << IEEE80211_SEQ_SEQ_SHIFT);
		*(UINT16 *)&wh->qos[0]=0x0;
	}
	if (!eap)
	{
#ifdef MRVL_WAPI
		if (mib->Privacy->PrivInvoked || mib->Privacy->RSNEnabled || mib->Privacy->WAPIEnabled)
#else
		if (mib->Privacy->PrivInvoked || mib->Privacy->RSNEnabled)
#endif
			wh->FrmCtl.Wep = 1;
	}
#ifdef CLIENT_SUPPORT
	if (vmacSta_p->VMacEntry.modeOfService == VMAC_MODE_CLNT_INFRA)
	{
		wh->FrmCtl.ToDs = 1;
		IEEE80211_ADDR_COPY(wh->addr1, &bssId[0]);
		if (*(mib->mib_STAMacCloneEnable) == 1)
			IEEE80211_ADDR_COPY(wh->addr2, eh.ether_shost);
		else
			IEEE80211_ADDR_COPY(wh->addr2, &vmacEntry_p->vmacAddr[0]);
		IEEE80211_ADDR_COPY(wh->addr3, eh.ether_dhost);
	}
	else
#endif
	{
		wh->FrmCtl.FromDs = 1;
		if(llc_rptr) 
			IEEE80211_ADDR_COPY(wh->addr1, pStaInfo->Addr);
		else
		IEEE80211_ADDR_COPY(wh->addr1, eh.ether_dhost);
		IEEE80211_ADDR_COPY(wh->addr2, vmacSta_p->macStaAddr);
		IEEE80211_ADDR_COPY(wh->addr3, eh.ether_shost);
#ifdef WDS_FEATURE
		if (wds)
		{
			wh->FrmCtl.FromDs = 1;
			wh->FrmCtl.ToDs   = 1;
			IEEE80211_ADDR_COPY(wh->addr4, wh->addr3);
			IEEE80211_ADDR_COPY(wh->addr3, wh->addr1);
			IEEE80211_ADDR_COPY(wh->addr1, pWdsPort->wdsMacAddr);
		}
#endif
	}
#ifdef QUEUE_STATS_CNT_HIST
    /* track per sta tx count */
    wldbgRecPerStatxPktStats(wh->addr1, QS_TYPE_TX_OK_CNT_CNT);
#endif

	if((!eap) && (wh->FrmCtl.Subtype ==QoS_DATA) && pStaInfo && (pStaInfo->aggr11n.type&WL_WLAN_TYPE_AMSDU) && pStaInfo->aggr11n.threshold)
	{
		//		Priority =0;
#ifndef AMSDUOVERAMPDU
		if(!pStaInfo->aggr11n.on)
		{
			return skb;
		}
		if(pStaInfo->aggr11n.nextpktnoaggr)
		{
			pStaInfo->aggr11n.nextpktnoaggr--;
			return skb;		
		}
		else
#endif
		{
			if (ampdu_ready) 
			{
				*(UINT16 *)&wh->qos[0]=(*(UINT16 *)&QosControl);
				skb = do_11n_aggregation(netdev,skb, pStaInfo, &eh, Priority, wh);
			}
		}
	}
	return skb;
bad:
	if (skb != NULL)
	{
		wlpptr->netDevStats.tx_dropped++;
		dev_kfree_skb_any(skb);
	}
	return NULL;
}

#ifdef SOC_W8764
UINT32 dispRxPacket = 0;
#endif

#ifndef ALIGNED_POINTER
#define ALIGNED_POINTER(p,t)	1
#endif
static struct sk_buff *ieee80211_decap(struct net_device *dev, struct sk_buff *skb, extStaDb_StaInfo_t *pStaInfo)
{
	struct wlprivate *priv = NETDEV_PRIV_P(struct wlprivate, dev);
	vmacApInfo_t *vmacSta_p = priv->vmacSta_p;
	MIB_802DOT11 *mib = vmacSta_p->Mib802dot11;
	struct ether_header *eh;
	struct ieee80211_frame wh;
	struct llc *llc;
	struct llc_rptr *llc_rptr = NULL;
	UINT16 ether_type = 0;

#ifdef CLIENT_SUPPORT
	vmacEntry_t  *VMacEntry_p;
	UINT8 AssociatedFlag = 0;
	UINT8 bssId[6];    
#endif
	if(skb->len < sizeof(struct ieee80211_frame))
	{
		goto dropPacket;
	}
	memcpy(&wh, skb->data, sizeof(struct ieee80211_frame));

#ifdef SC_PALLADIUM   
    if (dispRxPacket)
    {
        struct ieee80211_qosHtctlframe wh2;
        UINT32 j = 0;
        memcpy(&wh2, skb->data, sizeof(struct ieee80211_qosHtctlframe));
        printk("------------------------------------------------------------\n");
        printk("ieee80211_decap len = %d \n", skb->len);
        for (j=0; j < skb->len; j++)
        {
            printk("%02x ", skb->data[j]);
            if ((j!=0) && !(j%16)) printk("\n");
        }
        printk("\n");
        printk("FrmCtl = %x \n", *((UINT16 *)&wh2.FrmCtl));
        printk("dur    = %x \n", *((UINT16 *)&wh2.dur));
        printk("addr1  = %s \n", mac_display(wh2.addr1));
        printk("addr2  = %s \n", mac_display(wh2.addr2));
        printk("addr3  = %s \n", mac_display(wh2.addr3));
        printk("seq    = %x \n", *((UINT16 *)&wh2.seq));        
        printk("addr4  = %s \n", mac_display(wh2.addr4));
        printk("qos    = %x \n", *((UINT16 *)&wh2.qos));
        printk("htctl  = %x \n", *((UINT32 *)&wh2.htctl));
    }
#endif

#ifdef CLIENT_SUPPORT

	if (skb->protocol & WL_WLAN_TYPE_STA)
	{
		if ((VMacEntry_p = sme_GetParentVMacEntry(vmacSta_p->VMacEntry.phyHwMacIndx)) == NULL)
			goto dropPacket;
		smeGetStaLinkInfo(VMacEntry_p->id,
			&AssociatedFlag,
			&bssId[0]);

		if(!AssociatedFlag || memcmp(bssId, wh.addr2, sizeof(IEEEtypes_MacAddr_t)))
			goto dropPacket;
		/* Check to see if broadcast packet from AP. */ 
		if (IS_GROUP((UINT8 *) &(wh.addr1)))
		{
			/* Verify that broadcast src address is not client */
			if ((VMacEntry_p = vmacGetVMacEntryByAddr(wh.addr3)) != NULL)
				goto dropPacket;
		}
		else
		{
			/* Unicast check if for client */
			if ((VMacEntry_p = vmacGetVMacEntryByAddr(wh.addr1)) == NULL)
			{
				printk("drop packet\n");
				goto dropPacket;
			}
		}
	}
	else
#endif
	{
		if(memcmp(wh.addr1,vmacSta_p->macStaAddr,6) )
		{
			dev_kfree_skb_any(skb);
			return NULL;
		}
#ifdef WDS_FEATURE
		if (pStaInfo == NULL)
			goto deauth;
		else if ((pStaInfo->State != ASSOCIATED) && !pStaInfo->AP)
			goto deauth;
#else
		if ((pStaInfo == NULL) || (pStaInfo->State != ASSOCIATED))
		{
			WLDBG_INFO(DBG_LEVEL_9, "class3 frame from %x %d\n", pStaInfo, pStaInfo?pStaInfo->State:0);
			macMgmtMlme_SendDeauthenticateMsg(vmacSta_p,&(wh.addr2),  0, IEEEtypes_REASON_CLASS3_NONASSOC);
			dev_kfree_skb_any(skb);
			return NULL;
		}
#endif
	}

	if((*(mib->mib_STAMacCloneEnable) == 2) && IS_GROUP((UINT8 *) &(wh.addr1)) && wh.FrmCtl.FromDs) {
		if (ethStaDb_GetStaInfo(vmacSta_p, &(wh.addr3), 1) != NULL ) {															
			dev_kfree_skb_any(skb);
			return NULL;
		}
	}

	llc = (struct llc *) skb_pull(skb, sizeof(struct ieee80211_frame));
    
	if (skb->len >= sizeof(struct llc) &&
		llc->llc_dsap == LLC_SNAP_LSAP && llc->llc_ssap == LLC_SNAP_LSAP &&
		llc->llc_control == LLC_UI && 
		((llc->llc_snap.org_code[0] == 0x00 && llc->llc_snap.org_code[1] == 0x40 && llc->llc_snap.org_code[2] == 0x96) ||
		(llc->llc_snap.org_code[0] == 0x00 && llc->llc_snap.org_code[1] == 0x00 && llc->llc_snap.org_code[2] == 0x00)))
	{
		
		if (ntohs(llc->llc_un.type_snap.ether_type) == RPTR_ETHERTYPE) {
			llc_rptr = (struct llc_rptr *)llc;
			ether_type = llc_rptr->eh.ether_type;
			skb_pull(skb, sizeof(struct llc_rptr));
			llc = NULL;
		} else {
		ether_type = llc->llc_un.type_snap.ether_type;
		skb_pull(skb, sizeof(struct llc));
		llc = NULL;
	}
	}

	eh = (struct ether_header *) skb_push(skb, sizeof(struct ether_header));
#ifdef CLIENT_SUPPORT
	if (skb->protocol & WL_WLAN_TYPE_STA)
	{
		if (llc_rptr) { 
			IEEE80211_ADDR_COPY(eh->ether_dhost, llc_rptr->eh.ether_dhost);
			IEEE80211_ADDR_COPY(eh->ether_shost, llc_rptr->eh.ether_shost);
			llc_rptr = NULL;
		} else {
		IEEE80211_ADDR_COPY(eh->ether_dhost, wh.addr1);
		IEEE80211_ADDR_COPY(eh->ether_shost, wh.addr3);
	} 
	} 
	else
#endif
	{
#ifdef WDS_FEATURE
		if (pStaInfo->AP)
		{
			IEEE80211_ADDR_COPY(eh->ether_dhost, wh.addr3);
			IEEE80211_ADDR_COPY(eh->ether_shost, wh.addr4);
		} else
#endif
			if(wh.FrmCtl.ToDs)
			{
				if (llc_rptr) {
					IEEE80211_ADDR_COPY(eh->ether_dhost, llc_rptr->eh.ether_dhost);
					IEEE80211_ADDR_COPY(eh->ether_shost, llc_rptr->eh.ether_shost);
					llc_rptr = NULL;
 					if ((pStaInfo->StaType & 0x02) == 0x02) {
						/* AP records ether peer to table */
						if (ethStaDb_AddSta(vmacSta_p, &(eh->ether_shost), pStaInfo) == TABLE_FULL_ERROR) 
						{
							dev_kfree_skb_any(skb);
							return NULL;
						}
					}
				} else {
				IEEE80211_ADDR_COPY(eh->ether_dhost, wh.addr3);
				IEEE80211_ADDR_COPY(eh->ether_shost, wh.addr2);
				}
			} else
			{
				WLDBG_ERROR(DBG_LEVEL_9,"FromDS = %i, ToDs = %i", wh.FrmCtl.FromDs, wh.FrmCtl.ToDs);
				dev_kfree_skb_any(skb);
				return NULL;
			}
	}

	if (!ALIGNED_POINTER(skb->data + sizeof(*eh), u_int32_t))
	{
		struct sk_buff *n;

		n = skb_copy(skb, GFP_ATOMIC);
		n->protocol = skb->protocol;
		n->dev = skb->dev;
		dev_kfree_skb_any(skb);
		if (n == NULL)
			return NULL;
		skb = n;
		eh = (struct ether_header *) skb->data;
	}
	if (llc != NULL)
		eh->ether_type = htons(skb->len - sizeof(*eh));
	else
		eh->ether_type = ether_type;

#ifdef CLIENT_SUPPORT
	//moved counter to syncSrv_BncRecvAssociatedHandler
	//if (vmacSta_p->VMacEntry.modeOfService == VMAC_MODE_CLNT_INFRA)
		//ClientModeDataCount[vmacSta_p->VMacEntry.phyHwMacIndx]++;		 
	
#endif
	return skb;
#ifdef CLIENT_SUPPORT
dropPacket:
	dev_kfree_skb_any(skb);
	return NULL;
#endif
#ifdef WDS_FEATURE
deauth:
#endif
	{
		WLDBG_INFO(DBG_LEVEL_9, "class3 frame from %x %d\n", pStaInfo, pStaInfo?pStaInfo->State:0);
		macMgmtMlme_SendDeauthenticateMsg(vmacSta_p,&(wh.addr2),  0, IEEEtypes_REASON_CLASS3_NONASSOC);
		dev_kfree_skb_any(skb);
		return NULL;
	}
}

#ifdef AMPDU_SUPPORT
void ampdu_Init(struct net_device *dev)
{
	struct wlprivate    *wlpptr   = NETDEV_PRIV_P(struct wlprivate, dev);
	int i,j,k;
#ifdef REORDERING
	for(i=0;i<MAX_AID;i++)
	{
		for(j=0;j<MAX_AC;j++)
		{
			for(k=0;k<MAX_AMPDU_REORDER_BUFFER;k++)
			{
				wlpptr->wlpd_p->AmpduPckReorder[i].pFrame[j][k]=NULL;
				wlpptr->wlpd_p->AmpduPckReorder[i].ExpectedSeqNo[j][k]=0;
			}
			wlpptr->wlpd_p->AmpduPckReorder[i].CurrentSeqNo[j]=0;
			wlpptr->wlpd_p->AmpduPckReorder[i].ReOrdering[j]=FALSE;
			wlpptr->wlpd_p->AmpduPckReorder[i].AddBaReceive[j]=FALSE;
		}
	}
#endif
	memset(&wlpptr->wlpd_p->Ampdu_tx[0], 0, sizeof(Ampdu_tx_t) *MAX_SUPPORT_AMPDU_TX_STREAM);
}

void ampdu_ReInit(struct net_device *dev, u_int16_t Aid)
{
	struct wlprivate    *wlpptr   = NETDEV_PRIV_P(struct wlprivate, dev);
	int i=0,j,k;
	struct sk_buff *skb;


	for(j=0;j<MAX_AC;j++)
	{
		for(k=0;k<MAX_AMPDU_REORDER_BUFFER;k++)
		{
			skb=wlpptr->wlpd_p->AmpduPckReorder[Aid].pFrame[j][i];
			if (skb != NULL)
			{
				wlpptr->wlpd_p->AmpduPckReorder[Aid].pFrame[j][i]=NULL;
				dev_kfree_skb_any(skb);
			}

			wlpptr->wlpd_p->AmpduPckReorder[Aid].pFrame[j][k]=NULL;
			wlpptr->wlpd_p->AmpduPckReorder[Aid].ExpectedSeqNo[j][k]=0;
		}
		wlpptr->wlpd_p->AmpduPckReorder[Aid].CurrentSeqNo[j]=0;
		wlpptr->wlpd_p->AmpduPckReorder[Aid].ReOrdering[j]=FALSE;
		wlpptr->wlpd_p->AmpduPckReorder[Aid].AddBaReceive[j]=FALSE;
	}
}

void blockack_reorder_pck(struct net_device *dev, int offset, u_int16_t Aid,u_int8_t Priority)
{
	struct wlprivate    *wlpptr   = NETDEV_PRIV_P(struct wlprivate, dev);
	int j;

#ifdef DEBUG_AMPDU_RECEIVE
	printk("In Blockack_reorder_pck\n");
#endif

	for(j=0;j<MAX_AMPDU_REORDER_BUFFER-offset;j++)
	{
		if(wlpptr->wlpd_p->AmpduPckReorder[Aid].pFrame[Priority][j+offset]!=NULL)  //** I don't think we need to do this **/
		{
			if(wlpptr->wlpd_p->AmpduPckReorder[Aid].pFrame[Priority][j] != NULL)
				dev_kfree_skb_any(wlpptr->wlpd_p->AmpduPckReorder[Aid].pFrame[Priority][j]);
			wlpptr->wlpd_p->AmpduPckReorder[Aid].pFrame[Priority][j]=wlpptr->wlpd_p->AmpduPckReorder[Aid].pFrame[Priority][j+offset];
			wlpptr->wlpd_p->AmpduPckReorder[Aid].ExpectedSeqNo[Priority][j]=wlpptr->wlpd_p->AmpduPckReorder[Aid].ExpectedSeqNo[Priority][j+offset];
			wlpptr->wlpd_p->AmpduPckReorder[Aid].pFrame[Priority][j+offset]=NULL;
			wlpptr->wlpd_p->AmpduPckReorder[Aid].ExpectedSeqNo[Priority][j+offset]=0;
		}

	}

}
/** Todo check this path again **/
int flush_blockack_pck(struct net_device *dev, int i, u_int16_t  Aid, u_int8_t Priority)
{
	struct sk_buff *skb;
	extStaDb_StaInfo_t *pStaInfo=NULL;
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, dev);
	int cnt=0;


	skb=wlpptr->wlpd_p->AmpduPckReorder[Aid].pFrame[Priority][i];
	wlpptr->wlpd_p->AmpduPckReorder[Aid].pFrame[Priority][i]=NULL;
	wlpptr->wlpd_p->AmpduPckReorder[Aid].ExpectedSeqNo[Priority][i] = 0;

	if (skb != NULL)
	{
		if(skb->protocol & WL_WLAN_TYPE_AMSDU)
		{
			DeAmsduPck(dev, skb);
			return 1;
		}
		skb= DeFragPck(dev, skb, &pStaInfo);
		if (skb == NULL)
		{
			return 1;
		}
		skb = ieee80211_decap(dev, skb, pStaInfo);
		if (skb == NULL)
		{
			return 1;
		}
		//skb = ProcessEAPoL(skb, vmacSta_p, vmacEntry_p);
		cnt = ForwardFrame(dev, skb);
	}
	else
	{
		return 0;
	}
	dev->last_rx = jiffies;
	return 1;
}

int Ampdu_Check_Valid_Pck_in_Reorder_queue(struct net_device *dev, u_int16_t  Aid, u_int8_t Priority)
{
	struct wlprivate    *wlpptr   = NETDEV_PRIV_P(struct wlprivate, dev);
	int i;

	for(i=0;i<MAX_AMPDU_REORDER_BUFFER;i++)
	{
		if(wlpptr->wlpd_p->AmpduPckReorder[Aid].pFrame[Priority][i]!=NULL)
		{
			return TRUE;
		}
	}
	return FALSE;

}
void Ampdu_Flush_All_Pck_in_Reorder_queue(struct net_device *dev,u_int16_t  Aid, u_int8_t Priority)
{
	int i;
	for(i=0;i<MAX_AMPDU_REORDER_BUFFER;i++)
	{
		/** flush all subsequent pck until the next hole **/
		flush_blockack_pck(dev,i,Aid,Priority);
	}
}

/** Use during assoc, deauth situation etc where we need to clear any pending queue **/
void free_any_pending_ampdu_pck(struct net_device *dev, u_int16_t Aid)
{
	struct wlprivate    *wlpptr   = NETDEV_PRIV_P(struct wlprivate, dev);
	int i,j;
	struct sk_buff *skb;


#ifdef DEBUG_AMPDU_RECEIVE
	printk("Inside  free_any_pending_ampdu_pck %d\n",Aid);
#endif

	for(j=0;j<MAX_AC;j++)
	{

		for(i=0;i<MAX_AMPDU_REORDER_BUFFER;i++)
		{
			skb=wlpptr->wlpd_p->AmpduPckReorder[Aid].pFrame[j][i];
			if (skb != NULL)
			{
				wlpptr->wlpd_p->AmpduPckReorder[Aid].pFrame[j][i]=NULL;
				dev_kfree_skb_any(skb);
			}
		}

	}

}

#endif

#ifdef MBSS
extern vmacApInfo_t  *vmacGetMBssByAddr(vmacApInfo_t *vmacSta_p,UINT8 *macAddr_p)
{
	struct net_device *dev;
	struct wlprivate *wlpptr, *wlpptr1 ;
	vmacApInfo_t  *vmac_ap;
	UINT8 i=0;
	UINT8 nullAddr[6] = {0,0,0,0,0,0};
	if(memcmp(macAddr_p, nullAddr, 6) == 0)
	{
		return NULL;
	}
	if(vmacSta_p->master)
		vmac_ap = vmacSta_p->master;
	else
		vmac_ap = vmacSta_p;
	wlpptr = NETDEV_PRIV_P(struct wlprivate, vmac_ap->dev);	
	while(i <=MAX_VMAC_INSTANCE_AP )
	{
		if(wlpptr->vdev[i]){
			dev = wlpptr->vdev[i];
			wlpptr1 = NETDEV_PRIV_P(struct wlprivate, dev);
			vmac_ap = wlpptr1->vmacSta_p;
			if(memcmp(macAddr_p, &vmac_ap->macStaAddr, IEEEtypes_ADDRESS_SIZE) == 0)
			{
				return vmac_ap;
			}
		}
		i++;
	}
	return NULL;
}
#endif
static struct sk_buff *ProcessEAPoL(struct sk_buff *skb, 
									vmacApInfo_t *vmacSta_p, vmacEntry_t  *vmacEntry_p )
{
	struct ether_header *eh  = (struct ether_header *) skb->data;
	MIB_802DOT11 *mib = vmacSta_p->Mib802dot11;
#ifdef MRV_8021X
	if (*(mib->mib_wpaWpa2Mode) < 4) /* For PSK modes use internal WPA state machine */
#endif
	{
		/* Process EAP packets. */
		if (eh->ether_type == IEEE_ETHERTYPE_PAE)
		{
#ifdef WPA_STA
			if (skb->protocol & WL_WLAN_TYPE_STA)
			{
				ProcessEAPoLSta((IEEEtypes_8023_Frame_t *) eh, &eh->ether_dhost);
			}
			else
#endif
			{
				ProcessEAPoLAp(vmacSta_p,(IEEEtypes_8023_Frame_t *) eh, &eh->ether_shost);
			}
			dev_kfree_skb_any(skb);
			return NULL;
		}
	}
#ifdef MRVL_WPS_CLIENT
	else // we are bypassing the internal security module
	{
		if (eh->ether_type == IEEE_ETHERTYPE_PAE && (skb->protocol & WL_WLAN_TYPE_STA))
		{
			//Get the MAC address of the wdev0 interface
			MACADDR_CPY(eh->ether_dhost, ((struct net_device *)vmacEntry_p->privInfo_p)->dev_addr );
#ifdef MRVL_WPS_DEBUG
			printk("MAC: %02X:%02X:%02X:%02X:%02X:%02X\n",
				eh->ether_dhost[0], eh->ether_dhost[1], eh->ether_dhost[2],
				eh->ether_dhost[3], eh->ether_dhost[4], eh->ether_dhost[5]);
#endif
		}
	}
#endif //MRVL_WPS_CLIENT
	return skb;
}
#ifdef SOC_W8764
int ieee80211_input(struct net_device *dev, struct sk_buff *skb, u_int32_t rssi, u_int32_t rssiPaths, u_int8_t ampdu_qos, u_int32_t status)
#else
int ieee80211_input(struct net_device *dev, struct sk_buff *skb, u_int32_t rssi, u_int8_t ampdu_qos, u_int32_t status)
#endif
{
	struct ieee80211_frame *wh;
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, dev);
	vmacApInfo_t *vmacSta_p = wlpptr->vmacSta_p;
	MIB_802DOT11 *mib = vmacSta_p->Mib802dot11;
	int cnt=0;
	UINT16 SeqNo;
	int i;
	extStaDb_StaInfo_t *pStaInfo=NULL;
	u_int8_t tid;
#ifdef WDS_FEATURE
	BOOLEAN wds = FALSE;    
	struct net_device *wdsDev   = NULL;
	struct wds_port   *pWdsPort = NULL;
#endif
#ifdef CLIENT_SUPPORT
	vmacEntry_t  *vmacEntry_p = NULL;
	struct wlprivate	*stapriv;
#endif
	BOOLEAN stationPacket = FALSE;

#ifdef CLIENT_SUPPORT
	if((vmacEntry_p = sme_GetParentVMacEntry(vmacSta_p->VMacEntry.phyHwMacIndx)) == NULL)
	{
		goto out ;
	}
#endif //MRVL_WPS_CLIENT

	if (skb->len < sizeof(struct ieee80211_frame_min))
	{
		goto out;
	}
	wh = (struct ieee80211_frame *)skb->data;
	switch (wh->FrmCtl.Type)
	{
	case IEEE_TYPE_DATA:
		/* For physical interfqace:  rx_packets is already counted before calling ieee80211_input() */
		// wlpptr->netDevStats.rx_packets++; 
		wlpptr->netDevStats.rx_bytes += skb->len;
#ifdef QUEUE_STATS_CNT_HIST
        /* Record the Rx pkts based on pre-set STA MAC address*/
        WLDBG_REC_RX_80211_INPUT_PKTS(wh);
#endif
#ifdef MBSS
		{
			vmacApInfo_t * vmactem_p; 
			vmactem_p = vmacGetMBssByAddr(vmacSta_p, (UINT8 *)&(wh->addr1));
			if(vmactem_p)
				vmacSta_p= vmactem_p;
			mib = vmacSta_p->Mib802dot11;
			dev = vmacSta_p->dev;
		}
#endif
#ifdef WDS_FEATURE
		if ( (wh->FrmCtl.ToDs==1)&&(wh->FrmCtl.FromDs==1))
		{
			if (!*(mib->mib_wdsEnable) || ((pStaInfo = extStaDb_GetStaInfo(vmacSta_p,&(wh->addr2), 1))==NULL))
			{
				// WDS AP Packet not in database. 
				goto out;
			}
			if (pStaInfo->AP)
			{
				wdsDev = (struct net_device *) pStaInfo->wdsInfo;
				pWdsPort = (struct wds_port *) pStaInfo->wdsPortInfo;
				if (!pWdsPort->active)
					goto out;
			}
			else
				goto out;
			skb->protocol |= WL_WLAN_TYPE_WDS;
			skb->dev = (struct net_device *) pStaInfo->wdsInfo;
			wds = TRUE;
		}
		else
#endif
			if (!(wh->FrmCtl.ToDs==1))
			{
#ifdef CLIENT_SUPPORT
				if (wh->FrmCtl.FromDs==1)
				{	                
					stationPacket = TRUE;
					skb->protocol |= WL_WLAN_TYPE_STA;
					dev = (struct net_device *)vmacEntry_p->privInfo_p;
					stapriv = NETDEV_PRIV_P(struct wlprivate, dev);
					vmacSta_p = stapriv->vmacSta_p;
					mib = vmacSta_p->Mib802dot11;
					/*Store RSSI*/
					*(vmacSta_p->ShadowMib802dot11->mib_Rssi) = *(mib->mib_Rssi) = rssi;
				}
				else
#endif
					goto out;
			}
			/* check if status has a specific error bit (bit 7)set or indicates a general decrypt error*/
			if ((status == (u_int32_t)GENERAL_DECRYPT_ERR) ||(status & (u_int32_t)DECRYPT_ERR_MASK)) 
			{
				wlpptr->netDevStats.rx_frame_errors++;

				/* check if status is not equal to 0xFF */
				/* the 0xFF check is for backward compatibility*/
				if (status != (u_int32_t)GENERAL_DECRYPT_ERR)
				{
					/* If the status indicates it is a MIC error call the appropriate handler */                    
                    /* also check that this is not an ICV error.                              */
					if (((status & (~DECRYPT_ERR_MASK)) & TKIP_DECRYPT_MIC_ERR) &&
						!((status & (WEP_DECRYPT_ICV_ERR | TKIP_DECRYPT_ICV_ERR))))
                    {
#ifdef CLIENT_SUPPORT
						if (wh->FrmCtl.FromDs==1)
						{
							MICCounterMeasureInvoke_Sta(vmacEntry_p,IS_GROUP((UINT8 *)&(wh->addr1)));        
						}
						else
#endif
						{
							MrvlMICErrorHdl(vmacSta_p, 0);
						}
					}
				}
				goto err;
			}
#ifdef AMPDU_SUPPORT

#ifdef REORDERING


			SeqNo =  le16_to_cpu(*(u_int16_t *)(wh->seq)) >> IEEE80211_SEQ_SEQ_SHIFT;
#ifdef CLIENT_SUPPORT
			if (!stationPacket)
			{
#endif
				pStaInfo = extStaDb_GetStaInfo(vmacSta_p,&(wh->addr2), 1);


				if(pStaInfo)
				{
					if((pStaInfo->State != ASSOCIATED) && !pStaInfo->AP)
					{
						goto deauth;
					}
				}
				else
				{
					goto deauth;
				}
#ifdef CLIENT_SUPPORT
			}
			else
			{
				/* station packet get StaInfo for remote Ap */                
				if ((pStaInfo = extStaDb_GetStaInfo(vmacSta_p,&(wh->addr2), 1)) == NULL)
					goto blkackoutcontinue;
			}
#endif
#ifdef MCAST_PS_OFFLOAD_SUPPORT
			macMgmtMlme_UpdatePwrMode(vmacSta_p, wh, pStaInfo);
#endif
			pStaInfo->RSSI = rssi;
#ifdef SOC_W8764
            pStaInfo->RSSI_path = *((RssiPathInfo_t *)&rssiPaths);
#endif
			//Priority = AccCategoryQ[ampdu_qos&0x7];
			tid = ampdu_qos&0x7;

#ifdef AMPDU_DEBUG
			printk("** blk ack pck received , SeqNo = %d ExpectedSqNo = %d ampdu_qos = %x\n",SeqNo,wlpptr->wlpd_p->AmpduPckReorder[pStaInfo->Aid].CurrentSeqNo[tid],ampdu_qos);
#endif

			/*****************************************************************************************************************
			******************************************************************************************************************                    
			START OF AMPDU RECEIVING!! 

			******************************************************************************************************************
			*****************************************************************************************************************/
			{
				//printk("input pStaInfo = %x Aid = %d IsStaQSTA = %x \n", pStaInfo, pStaInfo->Aid, pStaInfo->IsStaQSTA);
				if(pStaInfo->IsStaQSTA)
				{
					/* check if rx BA has been added */
					if(wlpptr->wlpd_p->AmpduPckReorder[pStaInfo->Aid].AddBaReceive[tid]==TRUE)
					{
					    /* check if it is a unicast */
						if(!(wh->addr1[0]&0x01)) 
						{
                            /* check if it is a QoS pkt */
    						if((wh->FrmCtl.Subtype == QoS_DATA)         ||   
                               (wh->FrmCtl.Subtype == QoS_DATA_CF_ACK)  ||
                               (wh->FrmCtl.Subtype == QoS_DATA_CF_POLL) ||
                               (wh->FrmCtl.Subtype == QoS_DATA_CF_ACK_CF_POLL))
    						{
#ifdef AMPDU_DEBUG
    							printk("** 11 blk ack pck received , SeqNo = %d ExpectedSqNo = %d ampdu_qos = %x\n",SeqNo,wlpptr->wlpd_p->AmpduPckReorder[pStaInfo->Aid].CurrentSeqNo[tid],ampdu_qos);
#endif

    							/** check for qos pck **/
    							if(wlpptr->wlpd_p->AmpduPckReorder[pStaInfo->Aid].ReOrdering[tid]==FALSE)  /** previous pck is in order **/
    							{
    								/*****************************************************************************************************************
    								******************************************************************************************************************                    
    								Previous ampdu pck is in order!!

    								******************************************************************************************************************
    								*****************************************************************************************************************/
    								if(SeqNo == wlpptr->wlpd_p->AmpduPckReorder[pStaInfo->Aid].CurrentSeqNo[tid])  /** found the right one **/
    								{
										wlpptr->wlpd_p->AmpduPckReorder[pStaInfo->Aid].Time[tid] = jiffies;
    									wlpptr->wlpd_p->AmpduPckReorder[pStaInfo->Aid].CurrentSeqNo[tid]=(wlpptr->wlpd_p->AmpduPckReorder[pStaInfo->Aid].CurrentSeqNo[tid]+1)%MAX_AC_SEQNO;
    									goto blkackoutcontinue;  // go to normal path
    								}
    								else  /** Out of Seq **/
    								{
#ifdef INTERNAL_FLUSH_TIMER
										//Added the following to handle packets delay problem when receiving out of order frames frequently
										if(jiffies - wlpptr->wlpd_p->AmpduPckReorder[pStaInfo->Aid].Time[tid] > MAX_REORDERING_HOLD_TIME)
										{
											wlpptr->wlpd_p->AmpduPckReorder[pStaInfo->Aid].Time[tid] = jiffies;
											wlpptr->wlpd_p->AmpduPckReorder[pStaInfo->Aid].CurrentSeqNo[tid]=(SeqNo+1)%MAX_AC_SEQNO;  /** assuming next pck **/
											goto blkackoutcontinue;
										}
#endif
    									//	printk("Out of Seq 11!!\n");
    									if(wlpptr->wlpd_p->AmpduPckReorder[pStaInfo->Aid].CurrentSeqNo[tid]<SeqNo) /** Valid case **/
    									{
    										if(SeqNo-wlpptr->wlpd_p->AmpduPckReorder[pStaInfo->Aid].CurrentSeqNo[tid]>(MAX_AMPDU_REORDER_BUFFER-1))
    										{
    											/** overrun condition !!!!!!!!!!!**/
    											/** too many pck is missing, time to reset the cnt **/
    											//	printk("Error condition 1\n");
    											//	printk("3.  SeqNo = %d Expected Seqno= %d \n",SeqNo, AmpduPckReorder[pStaInfo->Aid].CurrentSeqNo[Priority]);


    											//no need	Ampdu_Flush_All_Pck_in_Reorder_queue(dev,pStaInfo->Aid, tid);
    											wlpptr->wlpd_p->AmpduPckReorder[pStaInfo->Aid].ReOrdering[tid]=FALSE;
    											wlpptr->wlpd_p->AmpduPckReorder[pStaInfo->Aid].CurrentSeqNo[tid]=(SeqNo+1)%MAX_AC_SEQNO;  /**  assuming next pck **/
    											goto blkackoutcontinue;
    										}

    										/** valid condition **/
    										{
    											int tempvalue = SeqNo-wlpptr->wlpd_p->AmpduPckReorder[pStaInfo->Aid].CurrentSeqNo[tid];
    											if (wlpptr->wlpd_p->AmpduPckReorder[pStaInfo->Aid].pFrame[tid][tempvalue] != NULL)
    												dev_kfree_skb_any(skb);
    											else
    												wlpptr->wlpd_p->AmpduPckReorder[pStaInfo->Aid].pFrame[tid][tempvalue]= skb;    
    										}
    										//AmpduPckReorder[pStaInfo->Aid].pFrame[Priority][SeqNo-AmpduPckReorder[pStaInfo->Aid].CurrentSeqNo[Priority]]= skb;  
    										wlpptr->wlpd_p->AmpduPckReorder[pStaInfo->Aid].ExpectedSeqNo[tid][SeqNo-wlpptr->wlpd_p->AmpduPckReorder[pStaInfo->Aid].CurrentSeqNo[tid]]=SeqNo;
#ifdef INTERNAL_FLUSH_TIMER
											{
												struct reorder_t *ro_p = malloc(sizeof(struct reorder_t));
												ro_p->dev = dev;
												ro_p->Aid = pStaInfo->Aid;
												ro_p->tid = tid;
												ro_p->SeqNo = SeqNo;
												TimerFireInByJiffies(&wlpptr->wlpd_p->AmpduPckReorder[pStaInfo->Aid].timer[tid], 1, &ReorderingTimerProcess, (UINT8 *)ro_p, MAX_REORDERING_HOLD_TIME);
											}
#endif
    										skb=NULL;
    										wlpptr->wlpd_p->AmpduPckReorder[pStaInfo->Aid].ReOrdering[tid]=TRUE;
    										wlpptr->wlpd_p->AmpduPckReorder[pStaInfo->Aid].Time[tid] = jiffies;
    										goto err;
    									}
    									else 
    									{
    										int tempvalue;
    										/** possible rollover condition **/
    										tempvalue = MAX_AC_SEQNO  -wlpptr->wlpd_p->AmpduPckReorder[pStaInfo->Aid].CurrentSeqNo[tid] + SeqNo;
    										if(tempvalue <MAX_AMPDU_REORDER_BUFFER)
    										{
    											if (wlpptr->wlpd_p->AmpduPckReorder[pStaInfo->Aid].pFrame[tid][tempvalue] != NULL)
    												dev_kfree_skb_any(skb);
    											else

    												wlpptr->wlpd_p->AmpduPckReorder[pStaInfo->Aid].pFrame[tid][tempvalue] = skb;
    											wlpptr->wlpd_p->AmpduPckReorder[pStaInfo->Aid].ExpectedSeqNo[tid][tempvalue]=SeqNo;

    										}
    										else 
    										{
    											/** treat it as most likely overflow condition **/
    											/** too many pck is missing, time to reset the cnt **/
    											Ampdu_Flush_All_Pck_in_Reorder_queue(dev,pStaInfo->Aid, tid);

    											wlpptr->wlpd_p->AmpduPckReorder[pStaInfo->Aid].ReOrdering[tid]=FALSE;
    											wlpptr->wlpd_p->AmpduPckReorder[pStaInfo->Aid].CurrentSeqNo[tid]=(SeqNo+1)%MAX_AC_SEQNO;  /**  assuming next pck **/
												TimerDisarmByJiffies(&wlpptr->wlpd_p->AmpduPckReorder[pStaInfo->Aid].timer[tid], 1);
    											goto blkackoutcontinue;
    										}
#ifdef INTERNAL_FLUSH_TIMER
											{
												struct reorder_t *ro_p = malloc(sizeof(struct reorder_t));
												ro_p->dev = dev;
												ro_p->Aid = pStaInfo->Aid;
												ro_p->tid = tid;
												ro_p->SeqNo = SeqNo;
												TimerFireInByJiffies(&wlpptr->wlpd_p->AmpduPckReorder[pStaInfo->Aid].timer[tid], 1, &ReorderingTimerProcess, (UINT8 *)ro_p, MAX_REORDERING_HOLD_TIME);
											}
#endif
										skb=NULL;
										wlpptr->wlpd_p->AmpduPckReorder[pStaInfo->Aid].ReOrdering[tid]=TRUE;
										wlpptr->wlpd_p->AmpduPckReorder[pStaInfo->Aid].Time[tid]  = jiffies;
										goto err;
									}

    								}

    							}
    							else /** reordering has happen **/
    							{
    								//printk("Reordering has started\n");
    								/*****************************************************************************************************************
    								******************************************************************************************************************                    
    								Previous ampdu pck reorder has started !!

    								******************************************************************************************************************
    								*****************************************************************************************************************/
    								if(SeqNo == wlpptr->wlpd_p->AmpduPckReorder[pStaInfo->Aid].CurrentSeqNo[tid])
    								{
    									if (wlpptr->wlpd_p->AmpduPckReorder[pStaInfo->Aid].pFrame[tid][0] != NULL)
    										dev_kfree_skb_any(wlpptr->wlpd_p->AmpduPckReorder[pStaInfo->Aid].pFrame[tid][0]);

    									wlpptr->wlpd_p->AmpduPckReorder[pStaInfo->Aid].pFrame[tid][0]=skb;
    									skb=NULL;
    									wlpptr->wlpd_p->AmpduPckReorder[pStaInfo->Aid].ExpectedSeqNo[tid][0] = SeqNo;
    									/** time to handle flushing for this pck **/
    									for(i=0;i<MAX_AMPDU_REORDER_BUFFER;i++)
    									{
    										/** flush all subsequent pck until the next hole **/
    										if(flush_blockack_pck(dev,i,pStaInfo->Aid,tid)==0)
    										{
    											wlpptr->wlpd_p->AmpduPckReorder[pStaInfo->Aid].CurrentSeqNo[tid]=(wlpptr->wlpd_p->AmpduPckReorder[pStaInfo->Aid].CurrentSeqNo[tid]+i)%MAX_AC_SEQNO;
    											blockack_reorder_pck(dev, i,pStaInfo->Aid,tid);
    											break;
    										}
    									}

    									if(Ampdu_Check_Valid_Pck_in_Reorder_queue(dev,pStaInfo->Aid,tid)==FALSE)
    									{
    										wlpptr->wlpd_p->AmpduPckReorder[pStaInfo->Aid].ReOrdering[tid]=FALSE;
											TimerDisarmByJiffies(&wlpptr->wlpd_p->AmpduPckReorder[pStaInfo->Aid].timer[tid], 1);
    									}
    									goto err;

    								}
    								else  /** SeqNo not equal **/
    								{

    									if((jiffies - wlpptr->wlpd_p->AmpduPckReorder[pStaInfo->Aid].Time[tid]) > MAX_REORDERING_HOLD_TIME)
    									{
#ifdef AMPDU_DEBUG
    										printk("flushing pck due to timer expire\n");
#endif
    										Ampdu_Flush_All_Pck_in_Reorder_queue(dev,pStaInfo->Aid, tid);
    										wlpptr->wlpd_p->AmpduPckReorder[pStaInfo->Aid].ReOrdering[tid]=FALSE;
    										wlpptr->wlpd_p->AmpduPckReorder[pStaInfo->Aid].CurrentSeqNo[tid]=(SeqNo+1)%MAX_AC_SEQNO;  /** assuming next pck **/
											TimerDisarmByJiffies(&wlpptr->wlpd_p->AmpduPckReorder[pStaInfo->Aid].timer[tid], 1);
    										goto blkackoutcontinue;
    									}





    									if(wlpptr->wlpd_p->AmpduPckReorder[pStaInfo->Aid].CurrentSeqNo[tid]<SeqNo) /** Valid case **/
    									{
    										//	printk("2.  SeqNo = %d Expected Seqno= %d put at %d\n",SeqNo, AmpduPckReorder[pStaInfo->Aid].CurrentSeqNo[Priority],SeqNo-AmpduPckReorder[pStaInfo->Aid].CurrentSeqNo[Priority]);
    										// pFrame array index is from 0 -127 only. If SeqNo-CurrentSeqNo>=128, flush buffer so 128 is not used later as pFrame index
    										if(SeqNo-wlpptr->wlpd_p->AmpduPckReorder[pStaInfo->Aid].CurrentSeqNo[tid] >= MAX_AMPDU_REORDER_BUFFER)
    										{
    											Ampdu_Flush_All_Pck_in_Reorder_queue(dev,pStaInfo->Aid, tid);
    											wlpptr->wlpd_p->AmpduPckReorder[pStaInfo->Aid].ReOrdering[tid]=FALSE;
    											wlpptr->wlpd_p->AmpduPckReorder[pStaInfo->Aid].CurrentSeqNo[tid]=(SeqNo+1)%MAX_AC_SEQNO;  /** assuming next pck **/
												TimerDisarmByJiffies(&wlpptr->wlpd_p->AmpduPckReorder[pStaInfo->Aid].timer[tid], 1);
    											goto blkackoutcontinue;	



    										}
    										if (wlpptr->wlpd_p->AmpduPckReorder[pStaInfo->Aid].pFrame[tid][SeqNo-wlpptr->wlpd_p->AmpduPckReorder[pStaInfo->Aid].CurrentSeqNo[tid]]!= NULL)  
    											dev_kfree_skb_any(skb);
    										else

    											wlpptr->wlpd_p->AmpduPckReorder[pStaInfo->Aid].pFrame[tid][SeqNo-wlpptr->wlpd_p->AmpduPckReorder[pStaInfo->Aid].CurrentSeqNo[tid]]= skb;  
    										wlpptr->wlpd_p->AmpduPckReorder[pStaInfo->Aid].ExpectedSeqNo[tid][SeqNo-wlpptr->wlpd_p->AmpduPckReorder[pStaInfo->Aid].CurrentSeqNo[tid]]=SeqNo;
    										skb=NULL;
    										goto err;
    									}
    									else 
    									{
    										int tempvalue;
    										tempvalue = MAX_AC_SEQNO  -wlpptr->wlpd_p->AmpduPckReorder[pStaInfo->Aid].CurrentSeqNo[tid] + SeqNo;
    										if(tempvalue <MAX_AMPDU_REORDER_BUFFER)
    										{
    											if(wlpptr->wlpd_p->AmpduPckReorder[pStaInfo->Aid].pFrame[tid][tempvalue] != NULL)
    												dev_kfree_skb_any(skb);
    											else

    												wlpptr->wlpd_p->AmpduPckReorder[pStaInfo->Aid].pFrame[tid][tempvalue] = skb;
    											wlpptr->wlpd_p->AmpduPckReorder[pStaInfo->Aid].ExpectedSeqNo[tid][tempvalue]=SeqNo;
    											skb=NULL;
    											goto err;
    										}
    										else 
    										{
    											/** treat it as most likely overflow condition **/
    											/** too many pck is missing, time to reset the cnt **/
#ifdef AMPDU_DEBUG
    											printk("Error condition 4\n");
#endif
    											Ampdu_Flush_All_Pck_in_Reorder_queue(dev,pStaInfo->Aid, tid);
    											wlpptr->wlpd_p->AmpduPckReorder[pStaInfo->Aid].ReOrdering[tid]=FALSE;
    											wlpptr->wlpd_p->AmpduPckReorder[pStaInfo->Aid].CurrentSeqNo[tid]=(SeqNo+1)%MAX_AC_SEQNO;  /** assuming next pck **/
												TimerDisarmByJiffies(&wlpptr->wlpd_p->AmpduPckReorder[pStaInfo->Aid].timer[tid], 1);
    											goto blkackoutcontinue;

										    }
									    }
								    }
							    }
							}
						}
						//else
						{
							//	printk("Error condition 5\n");
						}
					}
					else
					{
						/** Receive Ampdu pck but not addba !!!!  ****/
					}
				}
				else
				{
#ifdef AMPDU_DEBUG
					printk("Not qos station\n");
#endif
				}

#endif		
blkackoutcontinue:
#endif 
				{
					extStaDb_StaInfo_t *pStaInfo=NULL;
					if(skb->protocol & WL_WLAN_TYPE_AMSDU)
					{
						cnt = DeAmsduPck(vmacSta_p->dev, skb);
						return cnt;
					}
					skb= DeFragPck(vmacSta_p->dev, skb, &pStaInfo);
					if (skb == NULL)
					{
						goto err;
					}
					skb = ieee80211_decap( vmacSta_p->dev,skb, pStaInfo);

				}
#ifdef REORDERING
			}
#endif
			if (skb == NULL)
			{
				goto err;
			}

			cnt = ForwardFrame(dev, skb);
			return cnt;
	case IEEE_TYPE_MANAGEMENT:
#ifdef MBSS
		if(!IS_GROUP((UINT8 *) &(wh->addr1)))
		{
			vmacApInfo_t * vmactem_p; 
			vmacApInfo_t * vmacwds_p;			
			vmactem_p = vmacGetMBssByAddr(vmacSta_p, (UINT8 *)&(wh->addr3));	//If sent by client, addr3 is AP's bssid
#ifdef WDS_FEATURE         		
            if (wh->FrmCtl.Subtype == IEEE_MSG_QOS_ACTION)
            {
            	/*Use vmac for extStaDb_GetStaInfo because vmac and parent can have different bssid*/
				if(vmactem_p){
					/*Action frame sent by client*/
					pStaInfo = extStaDb_GetStaInfo(vmactem_p,&(wh->addr2), 0);
				}
				else 
				{	
					/*In WDS, if AP2 sends Action frame to AP1, addr3 is AP2's bssid, not AP1's bssid*/
					/*So we use dest addr1 to find vmac in WDS case*/
					vmacwds_p = vmacGetMBssByAddr(vmacSta_p, (UINT8 *)&(wh->addr1));
					if(vmacwds_p)
						pStaInfo = extStaDb_GetStaInfo(vmacwds_p,&(wh->addr2), 0);
					else{	
						/*If vmac is not found, we just use parent*/
						pStaInfo = extStaDb_GetStaInfo(vmacSta_p,&(wh->addr2), 0);	
					}
				}

				
                if (pStaInfo && pStaInfo->wdsPortInfo) {
                    pWdsPort = (struct wds_port *) pStaInfo->wdsPortInfo;
                    if (pWdsPort->active)
                        vmactem_p = vmacGetMBssByAddr(vmacSta_p, (UINT8 *)&(wh->addr1));
                        
                }
            }
#endif            
			if(vmactem_p)
				vmacSta_p= vmactem_p;
			else
				stationPacket	=TRUE;			
			mib = vmacSta_p->Mib802dot11;
		}
		else 
		{	//mark broadcast frame as Station packet (probe request & beacon)
			//the receiveWlanMsg will handle the packet accordingly			
			stationPacket	=TRUE;	
		}		
#endif
#ifdef AP_MAC_LINUX
		receiveWlanMsg(vmacSta_p->dev, skb, rssi, stationPacket);
#endif
		return cnt;
#ifdef AMPDU_SUPPORT
	case IEEE_TYPE_CONTROL:
		{
#ifdef REORDERING		
			/** Assume this is a BAR now, since this is the only one we pass up from the firmware **/
			extStaDb_StaInfo_t *pStaInfo=NULL;
			IEEEtypes_BA_ReqFrame_t2 *BaReqFrm;
			u_int8_t Priority,equal=0;

			BaReqFrm=(IEEEtypes_BA_ReqFrame_t2 *)skb->data;


			if(BaReqFrm->FrmCtl.Subtype!=8) /** it is not a bar frame **/
			{
				goto err; /** free the pck **/
			}

			pStaInfo = extStaDb_GetStaInfo(vmacSta_p,&(BaReqFrm->SrcAddr), 1);
			if(pStaInfo)
			{
				if(pStaInfo->State != ASSOCIATED)
				{
					goto err;
				}
			}
			else
			{
				goto err;
			}

#ifdef MBSS
			{
				vmacApInfo_t * vmactem_p; 
				vmactem_p = vmacGetMBssByAddr(vmacSta_p, (UINT8 *)(&BaReqFrm->DestAddr));
				if(vmactem_p)
					vmacSta_p= vmactem_p;
				dev = vmacSta_p->dev;
			}
#endif
			Priority =(BaReqFrm->BA_Ctrl.TID&0x7);
#if 1//for now

			/** Need to flush everything here if any **/
			/** If a blockack req is received, all complete MSDUs with lower sequence numbers than the starting sequence number contained in the 
			blockack Req shall be indicated to the MAC client using the MA-UNIDATA.indication primitive.  Upon arrival of a blockackreq frame, the
			recipient shall indicate the MSDUs starting with the Starting Sequence number sequentially until there is an incomplete MSDU in the buffer **/

			if(wlpptr->wlpd_p->AmpduPckReorder[pStaInfo->Aid].CurrentSeqNo[Priority] <BaReqFrm->Seq_Ctrl.StartSeqNo)
			{
				/** Make sure it is not a rollover condition **/

				if(BaReqFrm->Seq_Ctrl.StartSeqNo - wlpptr->wlpd_p->AmpduPckReorder[pStaInfo->Aid].CurrentSeqNo[Priority] <MAX_AMPDU_REORDER_BUFFER)
				{
					/** Need to free everything up to that point **/
					/** Need to start flushing from here till the next hole **/

					for(i=0;i<BaReqFrm->Seq_Ctrl.StartSeqNo-wlpptr->wlpd_p->AmpduPckReorder[pStaInfo->Aid].CurrentSeqNo[Priority];i++)
					{
						/** Need to continue flushing until next pck **/
						flush_blockack_pck(dev,i,pStaInfo->Aid,Priority); /** there is a hole in the pck **/

					}


					blockack_reorder_pck(dev,i,pStaInfo->Aid,Priority);

					wlpptr->wlpd_p->AmpduPckReorder[pStaInfo->Aid].CurrentSeqNo[Priority]=BaReqFrm->Seq_Ctrl.StartSeqNo;
					wlpptr->wlpd_p->AmpduPckReorder[pStaInfo->Aid].ExpectedSeqNo[Priority][0] = BaReqFrm->Seq_Ctrl.StartSeqNo;

					for(i=0;i<MAX_AMPDU_REORDER_BUFFER;i++)
					{
						/** flush all subsequent pck until the next hole **/
						if(flush_blockack_pck(dev,i,pStaInfo->Aid,Priority)==0) 
						{
							wlpptr->wlpd_p->AmpduPckReorder[pStaInfo->Aid].CurrentSeqNo[Priority]=(wlpptr->wlpd_p->AmpduPckReorder[pStaInfo->Aid].CurrentSeqNo[Priority]+i)%MAX_AC_SEQNO;
							blockack_reorder_pck(dev,i,pStaInfo->Aid,Priority);
							break;
						}
					}

					if(Ampdu_Check_Valid_Pck_in_Reorder_queue(dev,pStaInfo->Aid, Priority)==FALSE)
					{
						wlpptr->wlpd_p->AmpduPckReorder[pStaInfo->Aid].ReOrdering[Priority]=FALSE;
					}


				}
				else  
				{
					int tempvalue;
					/** check for error condition here **/

					tempvalue=MAX_AC_SEQNO -  BaReqFrm->Seq_Ctrl.StartSeqNo+wlpptr->wlpd_p->AmpduPckReorder[pStaInfo->Aid].CurrentSeqNo[Priority];

					if(tempvalue > MAX_AMPDU_REORDER_BUFFER)  /** invalid condition has happen **/
					{
						//	printk("Invalid 1-1 current seq no %d, ba-seqno %d diff\n",AmpduPckReorder[pStaInfo->Aid].CurrentSeqNo[Priority] , BaReqFrm->Seq_Ctrl.StartSeqNo);

					}
					else  
					{
						equal=1;
					}


					/** rollover condition, current seq no is actually larger, eg, current seq 3 and ba seq is 4081 so nothing to do **/
					/** flush any pending pck **/
					//	printk("1-2\n");
					//	printk("1-2 current seq no %d, ba-seqno %d diff\n",wlpptr->wlpd_p->AmpduPckReorder[pStaInfo->Aid].CurrentSeqNo[Priority] , BaReqFrm->Seq_Ctrl.StartSeqNo);
					//	Ampdu_Flush_All_Pck_in_Reorder_queue(dev,pStaInfo->Aid, Priority);
					//	wlpptr->wlpd_p->AmpduPckReorder[pStaInfo->Aid].CurrentSeqNo[Priority]=BaReqFrm->Seq_Ctrl.StartSeqNo;
				}


			}

			//	 
			else if(wlpptr->wlpd_p->AmpduPckReorder[pStaInfo->Aid].CurrentSeqNo[Priority] > BaReqFrm->Seq_Ctrl.StartSeqNo)
			{
				if(wlpptr->wlpd_p->AmpduPckReorder[pStaInfo->Aid].CurrentSeqNo[Priority] - BaReqFrm->Seq_Ctrl.StartSeqNo < MAX_AMPDU_REORDER_BUFFER)
				{
					equal=1;
				}
				else
				{
					int tempvalue;

					tempvalue=MAX_AC_SEQNO - wlpptr->wlpd_p->AmpduPckReorder[pStaInfo->Aid].CurrentSeqNo[Priority] +BaReqFrm->Seq_Ctrl.StartSeqNo;

					if(tempvalue <MAX_AMPDU_REORDER_BUFFER)
					{
						//	printk("2-1\n");
						//	printk("2-1 current seq no %d, ba-seqno %d diff=%d\n",AmpduPckReorder[pStaInfo->Aid].CurrentSeqNo[Priority] , BaReqFrm->Seq_Ctrl.StartSeqNo,
						//			(AmpduPckReorder[pStaInfo->Aid].CurrentSeqNo[Priority] - BaReqFrm->Seq_Ctrl.StartSeqNo) );



						for(i=0;i<(MAX_AC_SEQNO  -wlpptr->wlpd_p->AmpduPckReorder[pStaInfo->Aid].CurrentSeqNo[Priority]+ BaReqFrm->Seq_Ctrl.StartSeqNo);i++)
						{
							/** Need to continue flushing until next pck **/
							flush_blockack_pck(dev,i,pStaInfo->Aid,Priority); /** there is a hole in the pck **/

						}


						blockack_reorder_pck(dev,i,pStaInfo->Aid,Priority);

						//	for(j=0;j<64;j++)
						//	printk("2.  Expected seq no = %d %d\n",AmpduPckReorder[pStaInfo->Aid].ExpectedSeqNo[Priority][j],j);	
						//     printk("2.  Expected seq no = %d, baseq = %d\n",AmpduPckReorder[pStaInfo->Aid].ExpectedSeqNo[Priority][0],BaReqFrm->Seq_Ctrl.StartSeqNo);
						wlpptr->wlpd_p->AmpduPckReorder[pStaInfo->Aid].CurrentSeqNo[Priority]=BaReqFrm->Seq_Ctrl.StartSeqNo;
						wlpptr->wlpd_p->AmpduPckReorder[pStaInfo->Aid].ExpectedSeqNo[Priority][0] = BaReqFrm->Seq_Ctrl.StartSeqNo;
						wlpptr->wlpd_p->AmpduPckReorder[pStaInfo->Aid].CurrentSeqNo[Priority]=BaReqFrm->Seq_Ctrl.StartSeqNo;
						wlpptr->wlpd_p->AmpduPckReorder[pStaInfo->Aid].ExpectedSeqNo[Priority][0] = BaReqFrm->Seq_Ctrl.StartSeqNo;

						for(i=0;i<MAX_AMPDU_REORDER_BUFFER;i++)
						{
							/** flush all subsequent pck until the next hole **/
							if(flush_blockack_pck(dev,i,pStaInfo->Aid,Priority)==0) 
							{
								wlpptr->wlpd_p->AmpduPckReorder[pStaInfo->Aid].CurrentSeqNo[Priority]=(wlpptr->wlpd_p->AmpduPckReorder[pStaInfo->Aid].CurrentSeqNo[Priority]+i)%MAX_AC_SEQNO;
								blockack_reorder_pck(dev,i,pStaInfo->Aid,Priority);
								break;
							}
						}


						if(Ampdu_Check_Valid_Pck_in_Reorder_queue(dev,pStaInfo->Aid, Priority)==FALSE)
						{
							wlpptr->wlpd_p->AmpduPckReorder[pStaInfo->Aid].ReOrdering[Priority]=FALSE;
						}


					}
					else
					{
						//printk("Invalid 2-1 current seq no %d, ba-seqno %d diff ignore, window already move\n",AmpduPckReorder[pStaInfo->Aid].CurrentSeqNo[Priority] , BaReqFrm->Seq_Ctrl.StartSeqNo);

						//ignore this invaild bar **/


					}


				}
			}
			else
			{
				equal=1;
				//	printk("3, they are equal AmpduPckReorder[pStaInfo->Aid].CurrentSeqNo[Priority] = %d\n",wlpptr->wlpd_p->AmpduPckReorder[pStaInfo->Aid].CurrentSeqNo[Priority]);
			}


			if(!equal)
			{
				if((Ampdu_Check_Valid_Pck_in_Reorder_queue(dev, pStaInfo->Aid,Priority)==FALSE))
				{
					wlpptr->wlpd_p->AmpduPckReorder[pStaInfo->Aid].ReOrdering[Priority]=FALSE;
				}
				else
				{
					wlpptr->wlpd_p->AmpduPckReorder[pStaInfo->Aid].ReOrdering[Priority]=TRUE;
				}
			}

#endif

			if (skb != NULL)
			{
				wlpptr->netDevStats.rx_dropped++;
				dev_kfree_skb_any(skb);
			}
			return cnt;
#endif
		}

#endif	
	default:
		goto out;
	}
deauth:
	{
		WLDBG_INFO(DBG_LEVEL_9, "class3 frame from %x %d\n", pStaInfo, pStaInfo?pStaInfo->State:0);
		macMgmtMlme_SendDeauthenticateMsg(vmacSta_p,&(wh->addr2),  0, IEEEtypes_REASON_CLASS3_NONASSOC);
	}
err:
out:
	//	WLDBG_ERROR(DBG_LEVEL_9,"Type= %i, ToDs = %i", wh->FrmCtl.Type, wh->FrmCtl.ToDs);
	if (skb != NULL)
	{
		wlpptr->netDevStats.rx_dropped++;
		dev_kfree_skb_any(skb);
	}
	return cnt;
}

//#define roundup(x, y)   ((((x)+((y)-1))/(y))*(y))
struct sk_buff *
	ieee80211_getmgtframe(UINT8 **frm, unsigned int pktlen)
{
	const unsigned int align = sizeof(u_int32_t);
	struct sk_buff *skb;
	unsigned int len;

	len = roundup_MRVL(sizeof(struct ieee80211_frame) + pktlen, 4);
	skb = dev_alloc_skb(len + align-1+NUM_EXTRA_RX_BYTES);
	if (skb != NULL)
	{
		unsigned int off = ((unsigned long) skb->data) % align;
		if (off != 0)
			skb_reserve(skb, align - off);

		skb_reserve(skb, MIN_BYTES_HEADROOM);
		*frm = skb_put(skb, pktlen);
		memset(skb->data, 0, sizeof(struct ieee80211_frame));
	}
	return skb;
}


struct sk_buff *
	ieee80211_getDataframe(UINT8 **frm, unsigned int pktlen)
{
	const unsigned int align = sizeof(u_int32_t);
	struct sk_buff *skb;
	unsigned int len;

	len = roundup_MRVL(pktlen, 4);
	skb = dev_alloc_skb(len + align-1+NUM_EXTRA_RX_BYTES);
	if (skb != NULL)
	{
		unsigned int off = ((unsigned long) skb->data) % align;
		if (off != 0)
			skb_reserve(skb, align - off);

		skb_reserve(skb, MIN_BYTES_HEADROOM);
		*frm = skb_put(skb, pktlen-sizeof(struct ieee80211_frame));
		skb_pull(skb, 6);
		memset(skb->data, 0, sizeof(struct ieee80211_frame));
	}
	return skb;
}
void sendLlcExchangeID(struct net_device *dev, IEEEtypes_MacAddr_t *src)
{
	struct sk_buff *skb;
	/* send a LLC exchange ID */
	unsigned char * bufptr;
	skb = dev_alloc_skb((dev->mtu) + NUM_EXTRA_RX_BYTES); 
	if(skb)
	{
		skb_reserve(skb, MIN_BYTES_HEADROOM);
		bufptr = skb->data;
		skb_put(skb, 60);
		memcpy(bufptr,(unsigned char *)&bcast, sizeof(bcast));
		bufptr += sizeof(bcast);
		memcpy(bufptr, (unsigned char*)src, sizeof(*src));
		bufptr += sizeof(IEEEtypes_MacAddr_t);             
		*(bufptr++) = 0x00;  /* Ieee802.3 length */
		*(bufptr++) = 0x06;  /* Ieee802.3 length */
		*(bufptr++) = 0x00;  /* Null DSAP */
		*(bufptr++) = 0x01;  /* Null SSAP */
		*(bufptr++) = 0xaf;  /* exchange ID */
		*(bufptr++) = 0x81;  /* LLC data */
		*(bufptr++) = 0x01;  /* LLC data */
		*(bufptr++) = 0x00;  /* LLC data */
		memset(bufptr, 0, 60 - (bufptr - skb->data)); /* pad */
		skb->dev = dev;
		skb->protocol = eth_type_trans(skb,dev);

        /*increment Rx packet counter per interface */
        ((NETDEV_PRIV_P(struct wlprivate, skb->dev)))->netDevStats.rx_packets++;
		((NETDEV_PRIV_P(struct wlprivate, skb->dev)))->netDevStats.rx_bytes += skb->len;

#ifdef NAPI
		netif_receive_skb(skb);
#else
//#ifdef CONFIG_SMP
        /* direct bridging part of Rx processing job to cpu1 for SMP platform */
//        smp_call_function_single(1, netif_rx_ni, (void *)skb, 0);
//#else
		netif_rx_ni(skb);
//#endif		
#endif
	}
}

#ifdef MPRXY
void McastProxyCheck(struct sk_buff *skb)
{
	struct ether_header *eh;
	IEEEtypes_IPv4_Hdr_t *IPv4_p = NULL;
	IEEEtypes_IPv6_Hdr_t *IPv6_p = NULL;
	UINT32 dIPAddr;
	/* check if IP packet, locate IP header check if IP address is multicast */
	eh = (struct ether_header *) skb->data;
	if (eh->ether_type == (UINT16)0x0008) /* type 0x0800 */
	{
		IPv4_p = (IEEEtypes_IPv4_Hdr_t *)((UINT8 *)eh + sizeof(ether_hdr_t));

		dIPAddr = WORD_SWAP(*((UINT32 *)IPv4_p->dst_IP_addr));

		//check if the pkt is IPv4 or IPV6
		if (IPv4_p->ver == IPV6_VERSION)
		{
			IPv6_p = (IEEEtypes_IPv6_Hdr_t *)IPv4_p;
			dIPAddr = WORD_SWAP(*((UINT32 *)IPv6_p->dst_IP_addr));
		}

		if (IS_IN_MULTICAST(dIPAddr))
		{
			/* recreate multicast mac from multicast IP */
			eh->ether_dhost[0] = 0x01;
			eh->ether_dhost[1] = 0x00;
			eh->ether_dhost[2] = 0x5e;
			eh->ether_dhost[3] = (UINT8)((dIPAddr & 0x007F0000) >> 16);
			eh->ether_dhost[4] = (UINT8)((dIPAddr & 0x0000FF00) >> 8);
			eh->ether_dhost[5] = (UINT8)(dIPAddr & 0x000000FF);                    
		}
	}

}
#endif

/*This function goes through multicast proxy list to find matching sta addr to be removed.
* This function is almost same as IGMP LEAVE section in ForwardFrame function
*/
BOOLEAN McastProxyUCastAddrRemove(vmacApInfo_t *vmacSta_p, IEEEtypes_MacAddr_t *addr)
{
	MIB_802DOT11 *mib = vmacSta_p->Mib802dot11;
    MIB_802DOT11 *mibShadow = vmacSta_p->ShadowMib802dot11;
	UINT8 i, j, IPMcastGrpCount, MAddrCount;		
	UINT8 retval= FALSE;
	UINT8 MCastGrpMovedUp=0;		//Once a group is moved up , set to 1
	UINT8 *pAddr = (UINT8 *)addr;
		

	/* Check if IP Multicast group entry already exists */
	IPMcastGrpCount = *(mib->mib_IPMcastGrpCount);		
	for(i=0; i < IPMcastGrpCount; i++)
    {
    	/*Once a group is moved up one slot, we have to inspect this group by going back one index behind after i++
		* Otherwise, the newly moved up group will not be inspected as the index moves forward
		*/
		if(MCastGrpMovedUp && (i > 0))
		{
			i--;
			MCastGrpMovedUp = 0;
		}
							
    	/*Find the unicast address entry in the IP multicast group.
    	   	* To save time, skip this group if there is no item
		*/
		if(mib->mib_IPMcastGrpTbl[i]->mib_MAddrCount)				
		{
			MAddrCount = mib->mib_IPMcastGrpTbl[i]->mib_MAddrCount; 		           	
			for (j=0; j < MAddrCount; j++)
           	{
           		if (memcmp ((char *)&(mib->mib_IPMcastGrpTbl[i]->mib_UCastAddr[j]),addr, 6) == 0)
               	{
               		printk("Mcast grp:%u.%u.%u.%u",IPQUAD(mib->mib_IPMcastGrpTbl[i]->mib_McastIPAddr));
					printk(" del: %02x%02x%02x%02x%02x%02x\n",
								pAddr[0],pAddr[1],pAddr[2],pAddr[3],pAddr[4],pAddr[5]);
								
				
               		/*decrement the count for unicast mac entries */
                    	mib->mib_IPMcastGrpTbl[i]->mib_MAddrCount --;
                    	/* update shadow MIB */
                    	mibShadow->mib_IPMcastGrpTbl[i]->mib_MAddrCount --;

                   	/*if this is the only entry, slot zero */
                   	if (mib->mib_IPMcastGrpTbl[i]->mib_MAddrCount == 0)
                   	{
                   		/* set the entry to zero */
                       	memset((char *)&mib->mib_IPMcastGrpTbl[i]->mib_UCastAddr[j],0,6);
                       	/* update shadow MIB*/
                       	memset((char *)&mibShadow->mib_IPMcastGrpTbl[i]->mib_UCastAddr[j],0,6);

                       	/* set the timestamp for the entry to zero */
                       	mib->mib_IPMcastGrpTbl[i]->mib_UcEntryTS[j] = 0;
						/* update shadow MIB */		
						mibShadow->mib_IPMcastGrpTbl[i]->mib_UcEntryTS[j] = 0;		

						/*IPM Grp table [0] is for 224.0.0.1 and we preserve this table by not deleting it. 
						*  It is created during initialization in wl_mib.c. Only upgrade from table[1] onwards
						*/
						if(i > 0)
						{
                       		/* Now that IPM Group table is empty remove this group */
                       		mib->mib_IPMcastGrpTbl[i]->mib_McastIPAddr = 0;
                       		mibShadow->mib_IPMcastGrpTbl[i]->mib_McastIPAddr = 0;

                       		/* Decrement the IP multicast group count */
                       		*(mib->mib_IPMcastGrpCount) = *(mib->mib_IPMcastGrpCount) -1;
                       		/* Update shadow MIB */
                       		*(mibShadow->mib_IPMcastGrpCount) = *(mibShadow->mib_IPMcastGrpCount) -1;

                       		/* If this is NOT 1st instance of IPM Group table */
							if (*(mib->mib_IPMcastGrpCount) > 1)  	
                       		{
                       			/* Move up entries to fill the empty spot */
                           		memcpy((char *)mib->mib_IPMcastGrpTbl[i],(char *)mib->mib_IPMcastGrpTbl[i+1],
                                   	sizeof(MIB_IPMCAST_GRP_TBL) * (*(mib->mib_IPMcastGrpCount) - i));
                           		/* Update shadow MIB */
                           		memcpy((char *)mibShadow->mib_IPMcastGrpTbl[i],(char *)mibShadow->mib_IPMcastGrpTbl[i+1],
                               		sizeof(MIB_IPMCAST_GRP_TBL) * (*(mibShadow->mib_IPMcastGrpCount) - i));
                                                    
                           		/* clear out the last instance of the IPM Grp table */
                           		memset((char *)mib->mib_IPMcastGrpTbl[*(mib->mib_IPMcastGrpCount)],
                           	   		0, sizeof(MIB_IPMCAST_GRP_TBL));
                           		/* update shadow MIB */
                           		memset((char *)mibShadow->mib_IPMcastGrpTbl[*(mibShadow->mib_IPMcastGrpCount)],
                               		0, sizeof(MIB_IPMCAST_GRP_TBL));

								MCastGrpMovedUp = 1;		
                   			}  
						}
						retval = TRUE;					
                       	break;
           			}
                   	else
                   	{
                   		/*if this is other than slot zero */
                       	/* set the entry to zero */
                       	memset((char *)&mib->mib_IPMcastGrpTbl[i]->mib_UCastAddr[j],0,6);
                       	/* Update the shadow MIB */
                       	memset((char *)&mibShadow->mib_IPMcastGrpTbl[i]->mib_UCastAddr[j],0,6);

                       	/* set the timestamp for the entry to zero */
                       	mib->mib_IPMcastGrpTbl[i]->mib_UcEntryTS[j] = 0;
						/* update shadow MIB */		
						mibShadow->mib_IPMcastGrpTbl[i]->mib_UcEntryTS[j] = 0;		

                       	/* move up entries to fill the vacant spot */
                       	memcpy((char *)&mib->mib_IPMcastGrpTbl[i]->mib_UCastAddr[j],
                           	(char *)&mib->mib_IPMcastGrpTbl[i]->mib_UCastAddr[j+1],
                           	(mib->mib_IPMcastGrpTbl[i]->mib_MAddrCount -j) * 6);
                       	/*Update shadow MIB */    
                       	memcpy((char *)&mibShadow->mib_IPMcastGrpTbl[i]->mib_UCastAddr[j],
                           	(char *)&mibShadow->mib_IPMcastGrpTbl[i]->mib_UCastAddr[j+1],
                           	(mibShadow->mib_IPMcastGrpTbl[i]->mib_MAddrCount -j) * 6);
                                                        
                       	/* clear the last unicast entry since all entries moved up by 1 */
                       	memset((char *)&mib->mib_IPMcastGrpTbl[i]->mib_UCastAddr[mib->mib_IPMcastGrpTbl[i]->mib_MAddrCount],
                               	0,6);
                       	/* Update shadow MIB */
                       	memset((char *)&mibShadow->mib_IPMcastGrpTbl[i]->mib_UCastAddr[mibShadow->mib_IPMcastGrpTbl[i]->mib_MAddrCount],
                               	0,6);

						retval = TRUE;
						MCastGrpMovedUp = 0;
                       	break;
           			}
           		}
        	}
    	}
	}
	return retval;
}


int ForwardFrame(struct net_device *dev,struct sk_buff *skb)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, dev);
	vmacApInfo_t *vmacSta_p = wlpptr->vmacSta_p;
	MIB_802DOT11 *mib = vmacSta_p->Mib802dot11;
    MIB_802DOT11 *mibShadow = vmacSta_p->ShadowMib802dot11;
	int cnt=0;
	struct sk_buff *newskb=NULL;
#ifdef CLIENT_SUPPORT
	struct net_device *staDev = NULL;
	vmacEntry_t  *vmacEntry_p = NULL;
#endif
	struct ether_header *eh;
	extStaDb_StaInfo_t *pStaInfo;
#ifdef MPRXY_SNOOP
    struct iphdr *ipheader = NULL;
    struct igmphdr *igmpheader = NULL;
    struct igmpv3_report *igmpv3_report = NULL;
    UINT32 igmp_addr, igmp_addr_host;
    UINT8 igmpheadertype = 0;
    UINT8 i,j, IPMcastGrpCount, MAddrCount;		
    BOOLEAN IPMcEntryExists = FALSE;
    BOOLEAN UcMACEntryExists = FALSE;
	UINT8 MCastGrpMovedUp=0;		//Once a group is moved up , set to 1
#ifdef MPRXY_IGMP_QUERY
    BOOLEAN IGMPQueryEntry = FALSE;
#endif
#endif
    eth_StaInfo_t *ethStaInfo_p;
    
	if (!skb)
		return cnt;
    
		eh = (struct ether_header *)skb->data;
#ifdef QUEUE_STATS_CNT_HIST
    /* Record the Rx pkts based on pre-set STA MAC address*/
    WLDBG_REC_RX_FWD_PKTS(eh);
#endif			


#ifdef CLIENT_SUPPORT
			if((vmacEntry_p = sme_GetParentVMacEntry(vmacSta_p->VMacEntry.phyHwMacIndx)) == NULL)
			{
				return cnt;
			}
			skb = ProcessEAPoL(skb, vmacSta_p, vmacEntry_p);
			
#else
			skb = ProcessEAPoL(skb, vmacSta_p, NULL);
#endif

			if(skb == NULL)
			{
				return cnt;
			}

#ifdef CLIENT_SUPPORT
			if (!(skb->protocol & WL_WLAN_TYPE_STA))
			{
#endif
				if(IS_GROUP((UINT8 *) &(eh->ether_dhost)))
				{
					wlpptr->netDevStats.multicast++;
					/* Intrabss packets get txed AP mode only*/
					if (*(mib->mib_intraBSS) && (vmacSta_p->VMacEntry.modeOfService != VMAC_MODE_CLNT_INFRA))
					{
						newskb = dev_alloc_skb(skb->len +NUM_EXTRA_RX_BYTES);
						if(newskb)
						{
							skb_reserve(newskb, MIN_BYTES_HEADROOM);
							MEMCPY newskb->data, skb->data, skb->len);
							skb_put(newskb, skb->len);
							wlDataTx(newskb, vmacSta_p->dev);
						}
					}

				}
#ifdef CLIENT_SUPPORT
			}
#endif

			pStaInfo = extStaDb_GetStaInfo(vmacSta_p,&(eh->ether_dhost), 0);

	if (*(mib->mib_RptrMode) && *(mib->mib_intraBSS) && (vmacSta_p->VMacEntry.modeOfService != VMAC_MODE_CLNT_INFRA))
	{
		if (!pStaInfo)
		{
			if ((ethStaInfo_p = ethStaDb_GetStaInfo(vmacSta_p, &(eh->ether_dhost), 1)) != NULL )	{							
				pStaInfo = ethStaInfo_p->pStaInfo_t;
				if (pStaInfo && (pStaInfo->StaType & 0x02) != 0x02)
					pStaInfo = NULL;
			}
		}
	}
#ifdef CLIENT_SUPPORT
#ifdef WDS_FEATURE
	if (pStaInfo && (!(skb->protocol & WL_WLAN_TYPE_STA)) && !pStaInfo->AP 
			/* In Rptr STA-AP case, DB entry hold a sta itself which is not ASSOCIATED. handling it here */
			&& (pStaInfo->Client == FALSE))
#else                   
			if (pStaInfo && (!(skb->protocol & WL_WLAN_TYPE_STA)))
#endif /* WDS_FEATURE */
#else
#ifdef WDS_FEATURE
			if (pStaInfo && !pStaInfo->AP)
#else                   
			if (pStaInfo)
#endif /* WDS_FEATURE */
#endif /*CLIENT_SUPPORT*/
			{
				if(pStaInfo->State == ASSOCIATED)
				{
					/* Intrabss packets get txed AP mode only*/
					if (*(mib->mib_intraBSS) && (vmacSta_p->VMacEntry.modeOfService != VMAC_MODE_CLNT_INFRA))
        			{
        			    /* set priority to 7(VO_Q) to match what wlxmit() will set for intrabss packets */
        			    skb->priority = (skb->priority&0xfffffff8)|0x7;
						wlDataTx(skb, vmacSta_p->dev);
        				dev->last_rx = jiffies;
        				return cnt;				
        			}
				} else
					goto err;
				}
	
				cnt++;
#ifdef CLIENT_SUPPORT
				if (!(skb->protocol & WL_WLAN_TYPE_STA))
				{
#endif
					if (skb->protocol & WL_WLAN_TYPE_WDS)
					{
						;//skb->dev = (struct net_device *) pStaInfo->wdsInfo;
					}
					else
					{
						skb->dev = vmacSta_p->dev;
					}
                    
					skb->protocol = eth_type_trans(skb, skb->dev);
#ifdef MPRXY_SNOOP
                    if (*(mib->mib_MCastPrxy))
                    {
           				ipheader = (struct iphdr *)((UINT8 *)eh + sizeof(ether_hdr_t));
                       
                        /* Filter out non-IGMP traffic */
                        if (ipheader->protocol != IPPROTO_IGMP)
                            goto mprxycontinue;

                        /* Get the pointer to the IGMP header and its data */
                        igmpheader = (struct igmphdr*)((UINT32)ipheader + (UINT32)(ipheader->ihl * 4));

                        /* Filter out unsupported IGMP messages */
                        if ((igmpheader->type != IGMP_HOST_MEMBERSHIP_REPORT) &&
                    	(igmpheader->type != IGMPV2_HOST_MEMBERSHIP_REPORT) &&
                    	(igmpheader->type != IGMPV3_HOST_MEMBERSHIP_REPORT) &&
                    	(igmpheader->type != IGMP_HOST_LEAVE_MESSAGE))    	
                    	    goto mprxycontinue;
                        
                        /* Determine the group address based on IGMP V1/V2 or IGMP V3*/                
                        if (igmpheader->type == IGMPV3_HOST_MEMBERSHIP_REPORT)
                        {
                            igmpv3_report = (struct igmpv3_report *)igmpheader;
                            /* Determine the IP multicast group address */
                            igmp_addr = igmpv3_report->grec[0].grec_mca;
                            igmp_addr_host = ntohl(igmpv3_report->grec[0].grec_mca);
                        }
                        else /* if IGMP V1 or V2 */
                        {
                            igmp_addr = igmpheader->group;
                            igmp_addr_host = ntohl(igmpheader->group);
                        }
                        
                        /* Filter out non-multicast messages */
                        if (!MULTICAST(igmp_addr))
                        {
                        	printk("\nIGMP snoop: Non-multicast group address in IGMP header\n");
                        	goto mprxycontinue;
                        }

                        /* According to "draft-ietf-magma-snoop-12.txt" local multicast messages (224.0.0.x) must be flooded to all ports */
                        /* So, don't do anything with such messages */
                        if (LOCAL_MCAST(igmp_addr))
                        {
                        	printk("\nIGMP snoop: Local IGMP messages (224.0.0.x) must be flooded \n");
                    	    goto mprxycontinue;
                        }
            
                        /* According to RFC 2236 IGMP LEAVE messages should be sent to ALL-ROUTERS address (224.0.0.2) */
                        if (igmpheader->type == IGMP_HOST_LEAVE_MESSAGE)
                        {
                        	if (ntohl(ipheader->daddr) != 0xE0000002)
        	                {
                        		printk("\nIGMP snoop: Ignore IGMP LEAVE message sent to non-ALL-ROUTERS address (224.0.0.2) \n");
        		                goto mprxycontinue;
        	                }	
                        }
                        else
                        {
#if 0                       /* this check needs to be reviewed, IGMPV3 messages are different*/
                        	/* According to RFC 2236 Membership Report (JOIN) IGMP messages should be sent to the IGMP group address */
                        	if (ipheader->daddr != igmp_addr)
            	            {
                        		printk("\nIGMP snoop: Ignore IGMP JOIN message with different destination IP(%u.%u.%u.%u) and IGMP group address(%u.%u.%u.%u) \n",
        		            	NIPQUAD(ipheader->daddr), NIPQUAD(igmp_addr));
        		                goto mprxycontinue;
           	                }
#endif
                        }

                        if (igmpheader->type == IGMPV3_HOST_MEMBERSHIP_REPORT)
                        {
                            /* Determine if IGMPV3 message is JOIN or LEAVE */
                            /* If LEAVE message then store the header type as LEAVE */
                            if ((igmpv3_report->grec[0].grec_type == IGMPV3_CHANGE_TO_INCLUDE) ||
                                (igmpv3_report->grec[0].grec_type == IGMPV3_BLOCK_OLD_SOURCES)) 
                            {
                                igmpheadertype = IGMP_HOST_LEAVE_MESSAGE;
                            }
                            else if ((igmpv3_report->grec[0].grec_type == IGMPV3_CHANGE_TO_EXCLUDE) ||
                                    (igmpv3_report->grec[0].grec_type == IGMPV3_ALLOW_NEW_SOURCES))
                            {
                                igmpheadertype = IGMPV3_HOST_MEMBERSHIP_REPORT;
                            }
                        }
                        else
                            igmpheadertype = igmpheader->type;

                        switch(igmpheadertype)
                        { 
                    	case IGMP_HOST_MEMBERSHIP_REPORT:
        	            case IGMPV2_HOST_MEMBERSHIP_REPORT:	
        	            case IGMPV3_HOST_MEMBERSHIP_REPORT:	
						
						printk("IGMP Report:%u.%u.%u.%u",IPQUAD(igmp_addr_host));
						printk("   %02x%02x%02x%02x%02x%02x\n",
											eh->ether_shost[0], eh->ether_shost[1], eh->ether_shost[2],
											eh->ether_shost[3],	eh->ether_shost[4],	eh->ether_shost[5]); 
						
							IPMcastGrpCount = *(mib->mib_IPMcastGrpCount);									
							for (i=0; i < IPMcastGrpCount; i++)
                			{
        	            		if(mib->mib_IPMcastGrpTbl[i]->mib_McastIPAddr == igmp_addr_host)
        			            {
                					IPMcEntryExists = TRUE;

                                    if (mib->mib_IPMcastGrpTbl[i]->mib_MAddrCount < MAX_UCAST_MAC_IN_GRP)
                                    {
                                        /*check if unicast adddress entry already exists in table*/
										MAddrCount = mib->mib_IPMcastGrpTbl[i]->mib_MAddrCount; 	
										for (j=0; j < MAddrCount; j++)
                                        {
                                            if (memcmp((char *)&mib->mib_IPMcastGrpTbl[i]->mib_UCastAddr[j],
                                                (char *)&eh->ether_shost, 6) == 0)
                                            {
                                                UcMACEntryExists = TRUE;
                                                /* update the timestamp for this entry */
                                                mib->mib_IPMcastGrpTbl[i]->mib_UcEntryTS[j] = jiffies;
                                                break;
                                            }
                                        }

                                        if (UcMACEntryExists == FALSE)
                                        {
                                            /* Add the MAC address into the table */
                                            memcpy ((char *)&mib->mib_IPMcastGrpTbl[i]->mib_UCastAddr[mib->mib_IPMcastGrpTbl[i]->mib_MAddrCount],
                                                    (char *)&eh->ether_shost, 6);

                                            /* Add the MAC address into shadow MIB table also */
                                            memcpy ((char *)&mibShadow->mib_IPMcastGrpTbl[i]->mib_UCastAddr[mibShadow->mib_IPMcastGrpTbl[i]->mib_MAddrCount],
                                                    (char *)&eh->ether_shost, 6);

                                            /* update the timestamp corresponding to the unicast entry */
                                            mib->mib_IPMcastGrpTbl[i]->mib_UcEntryTS[mib->mib_IPMcastGrpTbl[i]->mib_MAddrCount] = jiffies;
                                            
                                            /* Increment the number of MAC address in IPM table */
                                            mib->mib_IPMcastGrpTbl[i]->mib_MAddrCount ++;
                                            mibShadow->mib_IPMcastGrpTbl[i]->mib_MAddrCount ++;
#ifdef MPRXY_IGMP_QUERY
                                            /*check if unicast adddress entry already exists in table*/
											MAddrCount = mib->mib_IPMcastGrpTbl[0]->mib_MAddrCount;	
											for (j=0; j < MAddrCount; j++)
                                            {
                                                if (memcmp((char *)&(mib->mib_IPMcastGrpTbl[0]->mib_UCastAddr[j]),
                                                    (char *)&eh->ether_shost, 6) == 0)
                                                {
                                                    IGMPQueryEntry = TRUE;
                                                    break;
                                                }
                                            }
                                            if (!IGMPQueryEntry)
                                            {
                                                /* Add the unicast entry in the IPM Grp table [0] */
                                                memcpy ((char *)&(mib->mib_IPMcastGrpTbl[0]->mib_UCastAddr[mib->mib_IPMcastGrpTbl[0]->mib_MAddrCount]),
                                                    (char *)&eh->ether_shost, 6);
												/* Update shadow MIB */		
												memcpy ((char *)&(mibShadow->mib_IPMcastGrpTbl[0]->mib_UCastAddr[mibShadow->mib_IPMcastGrpTbl[0]->mib_MAddrCount]),
                                                    (char *)&eh->ether_shost, 6);
												
                                                /* increment unicast mac address count */
                                                mib->mib_IPMcastGrpTbl[0]->mib_MAddrCount ++;
												/* Update shadow MIB */		
												mibShadow->mib_IPMcastGrpTbl[0]->mib_MAddrCount ++;
                                            }
#endif																					
                                            break;
                                        }
                                    }
                                    else
                                    {
                                        break;
                                    }
                                 }
        		            }

                            /* if IP multicast group entry does not exist */
                            if (IPMcEntryExists == FALSE)
                            {
                                /*check if space available in table */
                                if (*(mib->mib_IPMcastGrpCount) < MAX_IP_MCAST_GRPS)
                                {
                                    /* Add the IPM entry into the table */
                                    mib->mib_IPMcastGrpTbl[*(mib->mib_IPMcastGrpCount)]->mib_McastIPAddr = igmp_addr_host;
                                    /* Update Shadow MIB */
                                    mibShadow->mib_IPMcastGrpTbl[*(mibShadow->mib_IPMcastGrpCount)]->mib_McastIPAddr = igmp_addr_host;

                                    /* Add the MAC address into the table */
                                    i = *(mib->mib_IPMcastGrpCount);

                                    /* Add the unicast entry in the IPM Grp table */
                                    memcpy ((char *)&(mib->mib_IPMcastGrpTbl[i]->mib_UCastAddr[mib->mib_IPMcastGrpTbl[i]->mib_MAddrCount]),
                                        (char *)&eh->ether_shost, 6);

                                    /* Update shadow MIB */
                                    memcpy ((char *)&(mibShadow->mib_IPMcastGrpTbl[i]->mib_UCastAddr[mibShadow->mib_IPMcastGrpTbl[i]->mib_MAddrCount]),
                                        (char *)&eh->ether_shost, 6);

                                    /* Update the timestamp for the unicast entry */
                                    mib->mib_IPMcastGrpTbl[i]->mib_UcEntryTS[mib->mib_IPMcastGrpTbl[i]->mib_MAddrCount] = jiffies;

                                    /* increment unicast mac address count */
                                    mib->mib_IPMcastGrpTbl[i]->mib_MAddrCount ++;
                                    /* Update shadow MIB */
                                    mibShadow->mib_IPMcastGrpTbl[i]->mib_MAddrCount ++;
                                    
                                    /*increment the IP multicast group slot by 1 */
                                    *(mib->mib_IPMcastGrpCount) = *(mib->mib_IPMcastGrpCount)+1;
                                    /* Update shadow MIB */
                                    *(mibShadow->mib_IPMcastGrpCount)= *(mibShadow->mib_IPMcastGrpCount) + 1;
#ifdef MPRXY_IGMP_QUERY
                                    /*check if unicast adddress entry already exists in table*/
									MAddrCount = mib->mib_IPMcastGrpTbl[0]->mib_MAddrCount;	                             
									for (j=0; j < MAddrCount; j++)	
                                    {
                                        if (memcmp((char *)&(mib->mib_IPMcastGrpTbl[0]->mib_UCastAddr[j]),
                                            (char *)&eh->ether_shost, 6) == 0)
                                        {
                                            IGMPQueryEntry = TRUE;
                                            break;
                                        }
                                    }
                                    if (!IGMPQueryEntry)
                                    {
                                        /* Add the unicast entry in the IPM Grp table [0] */
                                        memcpy ((char *)&(mib->mib_IPMcastGrpTbl[0]->mib_UCastAddr[mib->mib_IPMcastGrpTbl[0]->mib_MAddrCount]),
                                            (char *)&eh->ether_shost, 6);

										/* Update shadow MIB */		
                                        memcpy ((char *)&(mibShadow->mib_IPMcastGrpTbl[0]->mib_UCastAddr[mibShadow->mib_IPMcastGrpTbl[0]->mib_MAddrCount]),
                                            (char *)&eh->ether_shost, 6);

                                        /* increment unicast mac address count */
                                        mib->mib_IPMcastGrpTbl[0]->mib_MAddrCount ++;
										 /* Update shadow MIB */		
                                        mibShadow->mib_IPMcastGrpTbl[0]->mib_MAddrCount ++;

                                    }
#endif
                                    
                                }
                                else
                                {
                                    break;
                                }
                            }
                            break;

                        case IGMP_HOST_LEAVE_MESSAGE:
								printk("IGMP Leave:%u.%u.%u.%u",IPQUAD(igmp_addr_host));
								printk("   %02x%02x%02x%02x%02x%02x\n",
											eh->ether_shost[0], eh->ether_shost[1], eh->ether_shost[2],
											eh->ether_shost[3],	eh->ether_shost[4],	eh->ether_shost[5]); 
							
                                /* check if IP Multicast group entry already exists */
								IPMcastGrpCount = *(mib->mib_IPMcastGrpCount);		
								for(i=0; i < IPMcastGrpCount; i++)         
                                {

									/*Once a group is moved up one slot, we have to inspect this group by going back one index behind after i++
									* Otherwise, the newly moved up group will not be inspected as the index moves forward
									*/
									if(MCastGrpMovedUp && (i > 0))
									{
										i--;
										MCastGrpMovedUp = 0;
									}
																		
                                    /*match IP multicast grp address with entry */
                                    if(mib->mib_IPMcastGrpTbl[i]->mib_McastIPAddr == igmp_addr_host)
                                    {

										/*Find the unicast address entry in the IP multicast group.
    	   									* To save time, skip this group if there is no item
										*/
										if(mib->mib_IPMcastGrpTbl[i]->mib_MAddrCount)				
										{	
                                        	/*find the unicast address entry in the IP multicast group */
											MAddrCount = mib->mib_IPMcastGrpTbl[i]->mib_MAddrCount; 	
											for (j=0; j < MAddrCount; j++)						
                                        	{
                                            	if (memcmp ((char *)&(mib->mib_IPMcastGrpTbl[i]->mib_UCastAddr[j]),
                                                	(char *)&eh->ether_shost, 6) == 0)
                                            	{
                                                	/*decrement the count for unicast mac entries */
                                                	mib->mib_IPMcastGrpTbl[i]->mib_MAddrCount --;
                                                	/* update shadow MIB */
                                                	mibShadow->mib_IPMcastGrpTbl[i]->mib_MAddrCount --;
                                                
                                                	/*if this is the only entry, slot zero */
                                                	if (mib->mib_IPMcastGrpTbl[i]->mib_MAddrCount == 0)
                                                	{
                                                    	/* set the entry to zero */
                                                    	memset((char *)&mib->mib_IPMcastGrpTbl[i]->mib_UCastAddr[j],
                                                        	    0,
                                                            	6);
                                                    	/* update shadow MIB*/
                                                    	memset((char *)&mibShadow->mib_IPMcastGrpTbl[i]->mib_UCastAddr[j],
                                                        	    0,
                                                            	6);

                                                    	/* set the timestamp for the entry to zero */
                                                    	mib->mib_IPMcastGrpTbl[i]->mib_UcEntryTS[j] = 0;
														mibShadow->mib_IPMcastGrpTbl[i]->mib_UcEntryTS[j] = 0;	

														
														/*IPM Grp table [0] is for 224.0.0.1 and we preserve this table by not deleting it. 
														*  It is created during initialization in wl_mib.c. Only upgrade from table[1] onwards
														*/
														if(i > 0)
														{
															/* Now that IPM Group table is empty remove this group */
                                                    		mib->mib_IPMcastGrpTbl[i]->mib_McastIPAddr = 0;
                                                    		mibShadow->mib_IPMcastGrpTbl[i]->mib_McastIPAddr = 0;

                                                    		/* Decrement the IP multicast group count */
                                                    		*(mib->mib_IPMcastGrpCount) = *(mib->mib_IPMcastGrpCount) -1;
                                                    		/* Update shadow MIB */
                                                    		*(mibShadow->mib_IPMcastGrpCount) = *(mibShadow->mib_IPMcastGrpCount) -1;

                                                    		/* If this is NOT 1st instance of IPM Group table */
                                                    		if (*(mib->mib_IPMcastGrpCount) > 1)	
                                                    		{
                                                        		/* Move up entries to fill the empty spot */
                                                        		memcpy((char *)mib->mib_IPMcastGrpTbl[i],
                                                                	(char *)mib->mib_IPMcastGrpTbl[i+1],
                                                                	sizeof(MIB_IPMCAST_GRP_TBL) * (*(mib->mib_IPMcastGrpCount) - i));
                                                        		/* Update shadow MIB */
                                                        		memcpy((char *)mibShadow->mib_IPMcastGrpTbl[i],
                                                                	(char *)mibShadow->mib_IPMcastGrpTbl[i+1],
                                                                	sizeof(MIB_IPMCAST_GRP_TBL) * (*(mibShadow->mib_IPMcastGrpCount) - i));
                                                    
                                                        		/* clear out the last instance of the IPM Grp table */
                                                        		memset((char *)mib->mib_IPMcastGrpTbl[*(mib->mib_IPMcastGrpCount)],
                                                                   	0, sizeof(MIB_IPMCAST_GRP_TBL));
                                                        		/* update shadow MIB */
                                                        		memset((char *)mibShadow->mib_IPMcastGrpTbl[*(mibShadow->mib_IPMcastGrpCount)],
                                                                   	0, sizeof(MIB_IPMCAST_GRP_TBL));

																MCastGrpMovedUp = 1;		
                                                    		} 
                                                		}
                                                    	break;
                                                	}
                                                	else
                                                	{
                                                    	/*if this is other than slot zero */
                                                    	/* set the entry to zero */
                                                    	memset((char *)&mib->mib_IPMcastGrpTbl[i]->mib_UCastAddr[j],
                                                        	    0,
                                                            	6);
                                                    	/* Update the shadow MIB */
                                                    	memset((char *)&mibShadow->mib_IPMcastGrpTbl[i]->mib_UCastAddr[j],
                                                        	    0,
                                                            	6);

                                                    	/* set the timestamp for the entry to zero */
                                                    	mib->mib_IPMcastGrpTbl[i]->mib_UcEntryTS[j] = 0;
														mibShadow->mib_IPMcastGrpTbl[i]->mib_UcEntryTS[j] = 0;		

                                                    	/* move up entries to fill the vacant spot */
                                                    	memcpy((char *)&mib->mib_IPMcastGrpTbl[i]->mib_UCastAddr[j],
                                                        	    (char *)&mib->mib_IPMcastGrpTbl[i]->mib_UCastAddr[j+1],
                                                            	(mib->mib_IPMcastGrpTbl[i]->mib_MAddrCount -j) * 6);
                                                    	/*Update shadow MIB */    
                                                    	memcpy((char *)&mibShadow->mib_IPMcastGrpTbl[i]->mib_UCastAddr[j],
                                                            	(char *)&mibShadow->mib_IPMcastGrpTbl[i]->mib_UCastAddr[j+1],
                                                            	(mibShadow->mib_IPMcastGrpTbl[i]->mib_MAddrCount -j) * 6);
                                                        
                                                    	/* clear the last unicast entry since all entries moved up by 1 */
                                                    	memset((char *)&mib->mib_IPMcastGrpTbl[i]->mib_UCastAddr[mib->mib_IPMcastGrpTbl[i]->mib_MAddrCount],
                                                            	0,
                                                            	6);
                                                    	/* Update shadow MIB */
                                                    	memset((char *)&mibShadow->mib_IPMcastGrpTbl[i]->mib_UCastAddr[mibShadow->mib_IPMcastGrpTbl[i]->mib_MAddrCount],
                                                        	    0,
                                                            	6);

														MCastGrpMovedUp = 0;		
                                                    	break;
                                                	}
                                            	}
                                        	}
                                    	}
                                    }
                                }
                    	    break;
        	
        	            default:
        	                break;	
                        }
                    }
#endif /* MPRXY_SNOOP */
#ifdef CLIENT_SUPPORT
				}
				else
				{
#ifdef MPRXY
					if (*(mib->mib_MCastPrxy))
					{
						McastProxyCheck(skb);
					}
#endif
#ifdef EWB    
					if (!*(mib->mib_STAMacCloneEnable))
					{
						/* WLAN recv of EWB */
						ewbWlanRecv(skb,vmacSta_p->macStaAddr);
					}
#endif
					vmacEntry_p = sme_GetParentVMacEntry(vmacSta_p->VMacEntry.phyHwMacIndx);
					staDev = (struct net_device *)vmacEntry_p->privInfo_p;
					skb->dev = staDev;
					skb->protocol = eth_type_trans(skb, staDev);
				}
#endif /* CLIENT_SUPPORT */
#ifdef MPRXY_SNOOP
mprxycontinue:
#endif

				/*increment Rx packet counter per interface (data packet) */
				((NETDEV_PRIV_P(struct wlprivate, skb->dev)))->netDevStats.rx_packets++;
				((NETDEV_PRIV_P(struct wlprivate, skb->dev)))->netDevStats.rx_bytes += skb->len;

#ifdef NAPI
				netif_receive_skb(skb);
#else
//#ifdef CONFIG_SMP
                /* direct bridging part of Rx processing job to cpu1 for SMP platform */
//                smp_call_function_single(1, netif_rx_ni, (void *)skb, 0);
//#else
				netif_rx_ni(skb);
//#endif
#endif

		dev->last_rx = jiffies;
		return cnt;
	
err:
	//	WLDBG_ERROR(DBG_LEVEL_9,"Type= %i, ToDs = %i", wh->FrmCtl.Type, wh->FrmCtl.ToDs);
	if (skb != NULL)
	{
		wlpptr->netDevStats.rx_dropped++;
		dev_kfree_skb_any(skb);
	}
	return cnt;
}

int DeAmsduPck(struct net_device *netdev,struct sk_buff *skb)
{
	int rxCount, length, length1=0, length2, headerlength=sizeof(struct ieee80211_frame);
	struct sk_buff *pRxSkBuff;
	void *pCurrentData, *pLoc;
	int cnt=0;
	rxCount = skb->len;
	length1 = 0;
	pCurrentData = skb->data;
	while(rxCount -headerlength > length1)
	{
		pLoc = pCurrentData + headerlength +length1;
		length = (*(UINT16 *)((UINT32)pLoc+12));
#ifdef AMSDU_BYTE_REORDER
#ifdef MV_CPU_LE
		length2 = ((length&0xff00) >> 8) | ((length&0x00ff)<<8);
		length=length2;
#endif
#endif
		if(length>rxCount)
		{
			dev_kfree_skb_any(skb);
			return cnt;
		}
		pRxSkBuff= dev_alloc_skb((netdev->mtu) + NUM_EXTRA_RX_BYTES);
		if( pRxSkBuff )
		{
			skb_reserve(pRxSkBuff, MIN_BYTES_HEADROOM);
			memcpy( pRxSkBuff->data,  skb->data,  headerlength);
#ifdef CLIENT_SUPPORT
			if (!(skb->protocol & WL_WLAN_TYPE_STA))
#endif
			{
				//need copy dst address from aggr header to 802.11 addr3
				memcpy(&pRxSkBuff->data[headerlength-14], pLoc, 6);
				//need copy src address from aggr header to 802.11 addr4 for wds
				if (skb->protocol & WL_WLAN_TYPE_WDS)
				{
					memcpy(&pRxSkBuff->data[headerlength-6], pLoc+6, 6);
				}
			}
			pRxSkBuff->protocol = skb->protocol;
			pRxSkBuff->dev = skb->dev;

			//WLDBG_INFO(DBG_LEVEL_14,"aggregation skb_put len=%d", length + OFFS_RXFWBUFF_IEEE80211PAYLOAD - 8);
			if (skb_tailroom(pRxSkBuff) >= (length + headerlength) ) 
			{
				struct ether_header *eh;
				memcpy(&pRxSkBuff->data[headerlength], pLoc+14, length);
				skb_put(pRxSkBuff, length + headerlength);
				{
					extStaDb_StaInfo_t *pStaInfo=NULL;
					pRxSkBuff= DeFragPck(netdev, pRxSkBuff, &pStaInfo);
					if (pRxSkBuff == NULL)
					{
						dev_kfree_skb_any(skb);
						return cnt;
					}
					pRxSkBuff = ieee80211_decap( netdev,pRxSkBuff, pStaInfo);

				}

				eh = (struct ether_header *) pRxSkBuff->data;
				cnt = ForwardFrame(netdev, pRxSkBuff);
			}
			else
			{
				dev_kfree_skb_any(pRxSkBuff);
				dev_kfree_skb_any(skb);
				return cnt;
			}

			length1 += roundup_MRVL(length+14, 4);			   
		}
		else
			WLDBG_INFO(DBG_LEVEL_14,"out of skb\n");
	}
	dev_kfree_skb_any(skb);
	return cnt;
}
