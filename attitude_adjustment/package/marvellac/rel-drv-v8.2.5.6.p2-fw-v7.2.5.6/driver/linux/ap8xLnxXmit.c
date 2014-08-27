/*
*                Copyright 2002-2014, Marvell Semiconductor, Inc.
* This code contains confidential information of Marvell Semiconductor, Inc.
* No rights are granted herein under any patent, mask work right or copyright
* of Marvell or any third party.
* Marvell reserves the right at its sole discretion to request that this code
* be immediately returned to Marvell. This code is provided "as is".
* Marvell makes no warranties, express, implied or otherwise, regarding its
* accuracy, completeness or performance.
*/

/** include files **/
#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/skbuff.h>
#include <linux/pci.h>
#include <linux/udp.h>
#include <linux/ip.h>
#include <linux/in.h>
#include <linux/tcp.h>
#include <linux/igmp.h>

#include <net/checksum.h>

#include "wldebug.h"
#include "apintf.h"
#include "apRegs.h"
#include "ap8xLnxDesc.h"
#include "ap8xLnxIntf.h"
#include "ap8xLnxXmit.h"
#include "fwCmd.h"
#include "IEEE_types.h"
#include "wl_mib.h"
#include "wl_hal.h"
#include "StaDb.h"
#include "mlmeApi.h"
#include "wds.h"
#include "ccmp.h"

#include "ewb_packet.h"

/* MPRXY */
#include "apMPrxy.h"

#define	IS_IN_CLASSD(a)		((((UINT32) (a)) & 0xf0000000) == 0xe0000000)
#define	IS_IN_MULTICAST(a)		IS_IN_CLASSD(a)
#ifndef ETHERTYPE_IP
#define	ETHERTYPE_IP	    0x0800		/* IP protocol */
#define	ETHERTYPE_IP_NW	    0x0008		/* IP protocol network byte order*/
#endif
/* MPRXY */

/** local definitions **/
#define DROP_XMIT_FRAME(wlpptr, skb) {\
				wlpptr->netDevStats->tx_errors++; \
				WL_SKB_FREE(skb); \
}\


#define SET_QUEUE_NUMBER(skb, pri) 	{ \
										if((skb->priority)&0x7) \
											pri = AccCategoryQ[(skb->priority)&0x7]; \
										else \
											pri=AccCategoryQ[Qos_GetDSCPPriority(skb->data)&0x7];\
									}\
	
 
#define CURR_TXD(i) ((struct wlprivate_data *)(wlpptr->wlpd_p))->descData[i].pStaleTxDesc
#define	IEEE80211_ADDR_COPY(dst,src)	memcpy(dst,src,IEEEtypes_ADDRESS_SIZE)

/** external data **/
extern u_int32_t debug_tcpack;

static inline int tcp_ack_sequence_no(struct wlprivate *wlpptr, struct sk_buff *skb , struct net_device *netdev, u_int32_t *seq)
{
	struct iphdr *iph = (struct iphdr *)((UINT32)skb->data+14);
	struct tcphdr *th = (struct tcphdr*)((UINT32)skb->data +14 + (iph->ihl * 4));
	if((iph->protocol == IPPROTO_TCP) && (htons(ETH_P_IP) == skb->protocol)){
		if((th->ack == 1) && (htons(iph->tot_len) == (iph->ihl * 4 + th->doff * 4)))
		{

			if(th->syn || th->fin)
			{
				//do not mark syn and fin as drop cadidate
				skb->cb[1] = 0;
				return 0;
			}
			if(debug_tcpack)
			{
				*((u_int32_t *)&skb->cb[4]) = th->ack_seq;
				*((u_int32_t *)&skb->cb[8]) = th->source | (th->dest <<16);
				skb->cb[1] = 1;
				*seq = th->ack_seq;
				return 1;
			}
		}
	}
	skb->cb[1] = 0;
	return 0;
}

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

static struct sk_buff *wlan_skbhdr_adjust(struct sk_buff *skb)
{
	int need_headroom = sizeof(struct ieee80211_qosframe)
		+ sizeof(struct ether_header) + sizeof(struct wlBufInfo) +
		+ 14;
	skb = skb_unshare(skb, GFP_ATOMIC);
   
	if (skb == NULL)
	{
	    printk("SKB unshare operation failed!\n");
	} 
	else if (skb_headroom(skb) < need_headroom)
	{
		struct sk_buff *tmp;
  		tmp = skb_realloc_headroom(skb, need_headroom);
		WL_SKB_FREE(skb);

        if (tmp == NULL)
		{
	    	printk("SKB headroom not enough --- reallocate headroom!\n");
		}
        skb = tmp;        
	}
	return skb;
}

int wlDataTx(struct sk_buff *skb, struct net_device *netdev)
{
	UINT8 Priority;
	DfsAp * pdfsApMain = NULL ; /* DFS_SUPPORT */
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	struct wlprivate *wlpptrvmac = NETDEV_PRIV_P(struct wlprivate, netdev);
	struct ether_header *pEth;

	BOOLEAN delskb = FALSE;			//Used in mcast proxy to mark original skb to be dropped 
	BOOLEAN skbCopyError = FALSE; 	
	BOOLEAN schedulepkt = FALSE;	//Schedule tx only when queue to txq. Dropped pkt won't be scheduled

	/* MPRXY */
	vmacApInfo_t *vmacSta_p = wlpptrvmac->vmacSta_p;		
	MIB_802DOT11 *mib = vmacSta_p->Mib802dot11; 		
	UINT32 i;
	UINT8 IPMcastGrpIndex=0xFF;
	struct sk_buff * skbCopy = NULL;
	struct ether_header *pEthHdr=NULL;
	struct ether_header *pEthHdrMcast=NULL; 
	IEEEtypes_IPv4_Hdr_t *IPv4_p = NULL;
	IEEEtypes_IPv6_Hdr_t *IPv6_p = NULL;
	UINT32 dIPAddr = 0;
	struct iphdr *ipheader = NULL;
	struct igmphdr *igmpheader = NULL;	
	/* MPRXY */


	if(wlpptr->vmacSta_p->rootvmac)			
		wlpptr = wlpptr->rootpriv;

	skb = wlan_skbhdr_adjust(skb);
    /* QUEUE_STATS: time stamp the start time of the packet*/
    WLDBG_SET_PKT_TIMESTAMP(skb);
	WL_PREPARE_BUF_INFO(skb);
	pEth = (struct ether_header *) skb->data;

	/* DFS_SUPPORT */
	pdfsApMain = wlpptr->wlpd_p->pdfsApMain;
	if( pdfsApMain && pdfsApMain->dropData )
	{
		((NETDEV_PRIV_P(struct wlprivate, netdev)))->netDevStats->tx_dropped++;
		((NETDEV_PRIV_P(struct wlprivate, netdev)))->netDevStats->tx_carrier_errors++;

        /* QUEUE_STATS:  count packets drop due to DFS */
		WLDBG_INC_DFS_DROP_CNT(AccCategoryQ[(skb->priority)&0x7]);
		
		WL_SKB_FREE(skb);
		WLDBG_EXIT_INFO(DBG_LEVEL_13, "%s: DFS", netdev->name);
		return 0;
	} /* DFS_SUPPORT */
	
	if ((netdev->flags & IFF_RUNNING) == 0)
	{
		((NETDEV_PRIV_P(struct wlprivate, netdev)))->netDevStats->tx_dropped++;
		((NETDEV_PRIV_P(struct wlprivate, netdev)))->netDevStats->tx_carrier_errors++;
		
        /* QUEUE_STATS:  count packets drop due to interface is not running */
		WLDBG_INC_IFF_DROP_CNT(AccCategoryQ[(skb->priority)&0x7]);
		
		WL_SKB_FREE(skb);
		WLDBG_EXIT_INFO(DBG_LEVEL_13, "%s: itf not running", netdev->name);
		return 0;
	}
	SET_QUEUE_NUMBER(skb, Priority);

	if (QUEUE_LEN(&((struct wlprivate_data *)(wlpptr->wlpd_p))->txQ[Priority]) > wlpptr->vmacSta_p->txQLimit)
	{	
        /* QUEUE_STATS:  count packets drop due to queue full */
		WLDBG_INC_TXQ_DROP_CNT(Priority);
	
		WL_SKB_FREE(skb);
		((NETDEV_PRIV_P(struct wlprivate, netdev)))->netDevStats->tx_dropped++;

		WLDBG_EXIT_INFO(DBG_LEVEL_13, "%s: qlen > limit", netdev->name);		
		return 0;		//Can return since we don't need new incoming pkt flush out pkts stuck in txq. timer_routine will flush out stuck pkts in txq
	}
	else
	{
	/* MPRXY */
	/*Move mcast proxy from _wlDataTx to wlDataTx so we only queue pkts to be sent into txq, not all incoming pkts.
	* This eliminates unnecessary mcast pkt going into txq and will save txq buffer from being occupied by 
	* these unnecesary pkts which are eventually dropped later.
	*/
		if (*(mib->mib_MCastPrxy) && (vmacSta_p->VMacEntry.modeOfService != VMAC_MODE_CLNT_INFRA))
		{
			dIPAddr = 0;
			ipheader = (struct iphdr *)((UINT8 *)pEth + sizeof(ether_hdr_t));
		
			/* Get the pointer to the IGMP header and its data */
			if (ipheader->protocol == IPPROTO_IGMP)
				igmpheader = (struct igmphdr*)((UINT32)ipheader + (UINT32)(ipheader->ihl * 4));
		
			/* check if IP packet, locate IP header check if IP address is multicast */
			if ((pEth->ether_type == (UINT16)ETHERTYPE_IP_NW && IS_GROUP ((UINT8 *)&(pEth->ether_dhost))
				&& (ipheader->protocol != IPPROTO_IGMP)) ||
				(pEth->ether_type == (UINT16)ETHERTYPE_IP_NW && IS_GROUP ((UINT8 *)&(pEth->ether_dhost))
				&& (ipheader->protocol == IPPROTO_IGMP) && igmpheader->type == IGMP_HOST_MEMBERSHIP_QUERY)) 
			{
				IPv4_p = (IEEEtypes_IPv4_Hdr_t *)((UINT8 *)pEth + sizeof(ether_hdr_t));
		
				dIPAddr = WORD_SWAP(*((UINT32 *)IPv4_p->dst_IP_addr));
		
				//check if the pkt is IPv4 or IPV6
				if (IPv4_p->ver == IPV6_VERSION)
				{
					IPv6_p = (IEEEtypes_IPv6_Hdr_t *)IPv4_p;
					dIPAddr = WORD_SWAP(*((UINT32 *)IPv6_p->dst_IP_addr));
				}
			}
			else if (ipheader->protocol == IPPROTO_IGMP &&
					((igmpheader->type == IGMP_HOST_MEMBERSHIP_REPORT) ||
					(igmpheader->type == IGMPV2_HOST_MEMBERSHIP_REPORT) ||
					(igmpheader->type == IGMPV3_HOST_MEMBERSHIP_REPORT)))
			{
				delskb = TRUE; 
					
				/* IGMP reports are not forwarded to all wireless clients */
				goto schedule;
			}
		}
		/*for mDNS etc, 224.0.0.x local group shall be sent out as is since the group */
		/* would not be created at the first place */
		if (*(mib->mib_MCastPrxy) && IS_IN_MULTICAST(dIPAddr) && !LOCAL_MCAST(WORD_SWAP(dIPAddr)))
		{
			/* Check if IPM address exists in IPM filter address list */
			/* if the address is present in the list then do not proxy */
			for (i=0; i < *(mib->mib_IPMFilteredAddressIndex); i++)
			{
				/* Do not proxy, just schedule for tx */
				if (dIPAddr == *(mib->mib_IPMFilteredAddress[i]))
				{
					/* QUEUE_STATS:  count packets successfully enqueue to TxQ */
	   				WLDBG_INC_TX_OK_CNT(skb, Priority);
					WLDBG_REC_TX_Q_DEPTH(QUEUE_LEN(&((struct wlprivate_data *)(wlpptr->wlpd_p))->txQ[Priority]), Priority);
					skb->dev = netdev;
					QUEUE_ENQUEUE_TAIL(&((struct wlprivate_data *)(wlpptr->wlpd_p))->txQ[Priority], skb);
					schedulepkt = TRUE;
					goto schedule;
				}
			}
		
			/* check if IP packet, locate IP header, check if IP address is multicast */
			/* determine if IP multicast group address is in IP multicast group tables*/
			for (i=0; i < *(mib->mib_IPMcastGrpCount) ; i ++)
			{
				if (dIPAddr == mib->mib_IPMcastGrpTbl[i]->mib_McastIPAddr)
				{
					/* store the index of this multicast group */
					IPMcastGrpIndex = i;	  
					break;
				}
			}
		
			pEthHdrMcast = (struct ether_header *)skb->data;
			if ((IPMcastGrpIndex != 0xFF) && IPMcastGrpIndex < MAX_IP_MCAST_GRPS)
			{
				if (skb != NULL)
				{
					for (i=0; i <mib->mib_IPMcastGrpTbl[IPMcastGrpIndex]->mib_MAddrCount; i ++)
					{
						/* First look up the the unicast address in the station database */
						/*Compare eth source addr with UCastAddr in list to prevent received multicast pkt from client from being converted to unicast*/
						/*Received multicast pkt from client should be sent out in wlan and eth as it is*/
						if (((extStaDb_GetStaInfo(wlpptr, &(mib->mib_IPMcastGrpTbl[IPMcastGrpIndex]->mib_UCastAddr[i]), 1)) != NULL)
							&& !(memcmp((char *)&mib->mib_IPMcastGrpTbl[IPMcastGrpIndex]->mib_UCastAddr[i],(char *)pEthHdrMcast->ether_shost, 6) == 0))
						{
							/* make a copy of the original skb */
							skbCopy = skb_copy(skb, GFP_ATOMIC);
		
							if (skbCopy == NULL){
								delskb = TRUE; 		
								skbCopyError = TRUE;
								goto schedule;
							}
		
							/* update the destination address from multicast to unicast */
							pEthHdr = (struct ether_header *)skbCopy->data;
							IEEE80211_ADDR_COPY(&(pEthHdr->ether_dhost), mib->mib_IPMcastGrpTbl[IPMcastGrpIndex]->mib_UCastAddr[i]);
		
							
							/* QUEUE_STATS:  count packets successfully enqueue to TxQ */
	   						WLDBG_INC_TX_OK_CNT(skbCopy, Priority);
							WLDBG_REC_TX_Q_DEPTH(QUEUE_LEN(&((struct wlprivate_data *)(wlpptr->wlpd_p))->txQ[Priority]), Priority);
							skbCopy->dev = netdev;
							QUEUE_ENQUEUE_TAIL(&((struct wlprivate_data *)(wlpptr->wlpd_p))->txQ[Priority], skbCopy);
							schedulepkt = TRUE;
						}
					}
					delskb = TRUE;		
					goto schedule;
				}
				delskb = TRUE;		
				goto schedule;
			}
			else {
				delskb = TRUE;		
				goto schedule;
			}
		}
		else 
		/* MPRXY */
		{	
        	/* QUEUE_STATS:  count packets successfully enqueue to TxQ */
	    	WLDBG_INC_TX_OK_CNT(skb, Priority);
			WLDBG_REC_TX_Q_DEPTH(QUEUE_LEN(&((struct wlprivate_data *)(wlpptr->wlpd_p))->txQ[Priority]), Priority);
			skb->dev = netdev;
			QUEUE_ENQUEUE_TAIL(&((struct wlprivate_data *)(wlpptr->wlpd_p))->txQ[Priority], skb);
			schedulepkt = TRUE;		
		}
	}

schedule: 

	if(delskb && skb)
	{
		((NETDEV_PRIV_P(struct wlprivate, netdev)))->netDevStats->tx_dropped++;
		/* QUEUE_STATS:  count packets drop due to error 
		* Mcat pkt dropped due to not inside mcast proxy list is not counted because it is not an error.
		*/
		if (skbCopyError)
			WLDBG_INC_TX_ERROR_CNT(AccCategoryQ[(skb->priority)&0x7]);
		
		WL_SKB_FREE(skb);
	}

	/*We don't schedule task for every pkt. Only schedule task when it is not scheduled yet. 
	* timer_routine helps flush out all txq when no new incoming pkt by scheduling task. This also prevent pkts sitting inside txq forever 
	* (wlDataTxHdl may not be able to send all pkts in one interupt)
	*/
	if (schedulepkt && !((struct wlprivate_data *)(wlpptr->wlpd_p))->isTxTaskScheduled)
	{
		tasklet_schedule(&((struct wlprivate_data *)(wlpptr->wlpd_p))->txtask);
		((struct wlprivate_data *)(wlpptr->wlpd_p))->isTxTaskScheduled=1;
	}

	return 0;
}

static inline void _wlDataTx(struct sk_buff *skb, struct net_device *netdev)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	vmacApInfo_t *vmacSta_p = wlpptr->vmacSta_p;
	MIB_802DOT11 *mib = vmacSta_p->Mib802dot11;
	unsigned long flags;
	struct ether_header *pEth;
	extStaDb_StaInfo_t *pStaInfo=NULL;
	UINT32 bcast = 0;
	BOOLEAN eapolPkt = FALSE;
	WL_BUFF *wlb = NULL;

	WLDBG_ENTER(DBG_LEVEL_13);

	// Multiple devices using the same queue.
	netdev = skb->dev;
	wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	vmacSta_p = wlpptr->vmacSta_p;
	mib = vmacSta_p->Mib802dot11;
	pStaInfo = NULL;
	pEth = (struct ether_header *) skb->data;

	if (netdev == wlpptr->netDev) /* WDS_SUPPORT */
	{
		if (vmacSta_p->VMacEntry.modeOfService == VMAC_MODE_CLNT_INFRA )
		{
			if (!*(mib->mib_STAMacCloneEnable))
			{
				WL_PREPARE_BUF_INFO(skb);
				/* LAN recv of EWB */
				if(ewbLanRecv(wlpptr, WL_BUFF_PTR(skb),vmacSta_p->macStaAddr))
				{
					goto error1;
				}
			}
		}
		if(!IS_GROUP((UINT8 *) &(pEth->ether_dhost)))
		{
			pStaInfo = extStaDb_GetStaInfo(wlpptr,&(pEth->ether_dhost), 1);
			bcast = 0;
		}
		else
			bcast = 1;

		/* 8021X_SUPPORT */
		if (pStaInfo != NULL)
		{
			if (!pStaInfo->keyMgmtStateInfo.RSNDataTrafficEnabled)
				eapolPkt = (pEth->ether_type == IEEE_ETHERTYPE_PAE) ? TRUE : FALSE;

			if (vmacSta_p->VMacEntry.modeOfService == VMAC_MODE_AP )
			{
				// Added for fixing Centrino connectivity issue, check relevant only in AP mode
				if ((mib->Privacy->RSNEnabled) && (pStaInfo->keyMgmtStateInfo.RSNDataTrafficEnabled == 0)
					&& (eapolPkt == FALSE))    
					goto error1;
			}
		} /* 8021X_SUPPORT */
	}
	else
	{
		if (vmacSta_p->VMacEntry.modeOfService != VMAC_MODE_CLNT_INFRA)
		{
			// Check for WDS port
			if (!*(wlpptr->vmacSta_p->Mib802dot11->mib_wdsEnable) || ((pStaInfo = updateWds(wlpptr,skb->dev)) == NULL))
				goto error;
		}
	} /* WDS_SUPPORT */
	
	if (vmacSta_p->VMacEntry.modeOfService == VMAC_MODE_CLNT_INFRA)
	{
		pStaInfo = extStaDb_GetStaInfo(wlpptr, (IEEEtypes_MacAddr_t *)GetParentStaBSSID(vmacSta_p->VMacEntry.phyHwMacIndx), 1);
		
		/* WPS_CLIENT */
		eapolPkt = (pEth->ether_type == IEEE_ETHERTYPE_PAE) ? TRUE : FALSE;
		/* The 2-way WPA Group key exchange eapol packets must be encrypted */
		if( pStaInfo && 
			pStaInfo->keyMgmtStateInfo.RSNDataTrafficEnabled && 
			eapolPkt )
		{
			eapolPkt = FALSE ;
		}
		/* WPS_CLIENT */
	}
	WL_PREPARE_BUF_INFO(skb);
	if ((wlb = ieee80211_encap(WL_BUFF_PTR(skb), wlpptr, eapolPkt)) == NULL)
	{
		skb = NULL;
		goto error;
	}
	skb = WL_BUFF_SKB(wlb);
	if(skb)
	{
		struct wlxmit_param param;
		param.ptr = (void *)pStaInfo;
		param.type = IEEE_TYPE_DATA;
		param.psq = 0;
		param.flags = (bcast?WLXMIT_PARAM_FLAGS_BCAST:0)|(eapolPkt?WLXMIT_PARAM_FLAGS_EAPOL:0);
		param.eth = NULL;
		param.totallen = 0;
		SPIN_LOCK_IRQSAVE(&wlpptr->wlpd_p->locks.xmitLock, flags);

		if (wlxmit(netdev, skb, &param))
		{
			WLDBG_INFO(DBG_LEVEL_13, "could not xmit");
		    SPIN_UNLOCK_IRQRESTORE(&wlpptr->wlpd_p->locks.xmitLock, flags);
			wlTxDone(netdev);
			goto error1;
		}
	}
	SPIN_UNLOCK_IRQRESTORE(&wlpptr->wlpd_p->locks.xmitLock, flags);
	WLDBG_EXIT(DBG_LEVEL_13);
#define TXDONE_THRESHOLD 64			
	if (((struct wlprivate_data *)(wlpptr->wlpd_p))->txDoneCnt ++ > TXDONE_THRESHOLD)
	{
		wlTxDone(netdev);
		((struct wlprivate_data *)(wlpptr->wlpd_p))->txDoneCnt = 0;
	}

	return;

error:
	if (skb)
	{
        /* QUEUE_STATS:  count packets drop due to error */
		WLDBG_INC_TX_ERROR_CNT(AccCategoryQ[(skb->priority)&0x7]);
		DROP_XMIT_FRAME(wlpptr, skb);
		
	}
error1:

	{
		writel(MACREG_H2ARIC_BIT_PPA_READY,
			wlpptr->ioBase1 + MACREG_REG_H2A_INTERRUPT_EVENTS);
	}
	WLDBG_EXIT_INFO(DBG_LEVEL_13, NULL);
	return;

}

void wlDataTxHdl(struct net_device *netdev)
{
	UINT8 num = NUM_OF_DESCRIPTOR_DATA;
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	struct sk_buff *skb; 
	u_int32_t seq;

	WLDBG_ENTER(DBG_LEVEL_13);


	while(num--)
	{
		/**  since f/w is slower than host cpu, the while loop below might get stuck,
			one way to fix this is to interrupt f/w to fetch packet when fwowndescriptor is max **/ 

		if(((struct wlprivate_data *)(wlpptr->wlpd_p))->fwDescCnt[num] >= MAX_NUM_TX_DESC)
		{
			writel(MACREG_H2ARIC_BIT_PPA_READY,wlpptr->ioBase1 + MACREG_REG_H2A_INTERRUPT_EVENTS);
			wlTxDone(netdev);
			((struct wlprivate_data *)(wlpptr->wlpd_p))->txDoneCnt = 0;
		}

		while (((struct wlprivate_data *)(wlpptr->wlpd_p))->fwDescCnt[num] < MAX_NUM_TX_DESC && (skb= QUEUE_DEQUEUE(&((struct wlprivate_data *)(wlpptr->wlpd_p))->txQ[num])) != 0)
		{
			tcp_ack_sequence_no(wlpptr, skb, netdev, &seq);
			_wlDataTx(skb, netdev);
		}
	}

	// Reset 	after tx task is done
	((struct wlprivate_data *)(wlpptr->wlpd_p))->isTxTaskScheduled=0;
}

int wlMgmtTx(struct sk_buff *skb, struct net_device *netdev)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	unsigned long flags;

	WLDBG_ENTER(DBG_LEVEL_13);


	/* Bypass this check for interface running when a scan is in progress */
	if (!wlpptr->vmacSta_p->busyScanning)
	{
		if ((netdev->flags & IFF_RUNNING) == 0)
		{
			wlpptr->netDevStats->tx_dropped++;
			WLDBG_EXIT_INFO(DBG_LEVEL_13, "%s: itf not running", netdev->name);
			return -ENETDOWN;
		}
	}

	SPIN_LOCK_IRQSAVE(&wlpptr->wlpd_p->locks.xmitLock, flags);
	{
		struct wlxmit_param param;
		param.ptr = NULL;
		param.type = IEEE_TYPE_MANAGEMENT;
		param.psq = 0;
		param.flags = 0;
		param.eth = NULL;
		param.totallen = 0;
		if (wlxmit(netdev, skb, &param))
		{
			WLDBG_INFO(DBG_LEVEL_13, "could not xmit");
			SPIN_UNLOCK_IRQRESTORE(&wlpptr->wlpd_p->locks.xmitLock, flags);
			wlTxDone(netdev);
			goto error1;
		}
	}
	wlpptr->netDevStats->tx_packets++;
	wlpptr->netDevStats->tx_bytes += skb->len;

	/* update physical interface counter for IEEE_TYPE_MANAGEMENT */
	if(wlpptr->vmacSta_p->rootvmac)	{		
       	wlpptr->rootpriv->netDevStats->tx_packets++;
		wlpptr->rootpriv->netDevStats->tx_bytes += skb->len;
	}

	SPIN_UNLOCK_IRQRESTORE(&wlpptr->wlpd_p->locks.xmitLock, flags);
	WLDBG_EXIT(DBG_LEVEL_13);
	return 0;

	if (skb)
	{
		DROP_XMIT_FRAME(wlpptr, skb);
	}
error1:
	WLDBG_EXIT_INFO(DBG_LEVEL_13, NULL);
	return 0;

}

void wlTxDone(struct net_device *netdev)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
    unsigned long flags;
	int num;
	extStaDb_StaInfo_t *pStaInfo;
	WLDBG_ENTER(DBG_LEVEL_13);
    SPIN_LOCK_IRQSAVE(&wlpptr->wlpd_p->locks.xmitLock, flags);
	{
		for (num=0; num<NUM_OF_DESCRIPTOR_DATA; num++)
		{		        
		    /**
		      WAR: this is to workaround the tx Traffic stop problem where both 
		      "pNextTxDesc" and "pStaleTxDesc" point to the same idle descriptor
		      and the rest of MAX_NUM_TX_DESC-1 of descriptors has been 
		      processed by fw and need to be free up by driver.
		      This WAR should be removed when the issue is rootcaused.
		     */
		    if((CURR_TXD(num)) && (((struct wlprivate_data *)(wlpptr->wlpd_p))->fwDescCnt[num]>= (MAX_NUM_TX_DESC-1)) 
		        && (((struct wlprivate_data *)(wlpptr->wlpd_p))->descData[num].pNextTxDesc == CURR_TXD(num)))
	        {
		            if(CURR_TXD(num)->Status == 0)
		            {
						/*When fwDescCnt==256 and pStaleTxDesc->Status==0, fwDescCnt should be 255.
						* When all desc Status==1, fwDescCnt is 256. We minus fwDescCnt when this condition occurs.
						*/		       
						if(((struct wlprivate_data *)(wlpptr->wlpd_p))->fwDescCnt[num] >= MAX_NUM_TX_DESC)
							((struct wlprivate_data *)(wlpptr->wlpd_p))->fwDescCnt[num]--;
						
                        CURR_TXD(num) = CURR_TXD(num)->pNext;												
		            }
	        }
	        
            while ((CURR_TXD(num))
                    &&(CURR_TXD(num)->Status & ENDIAN_SWAP32(EAGLE_TXD_STATUS_OK))
                    && (!(CURR_TXD(num)->Status & ENDIAN_SWAP32(EAGLE_TXD_STATUS_FW_OWNED))))
			{
				if (CURR_TXD(num)->Status & ENDIAN_SWAP32(EAGLE_TXD_STATUS_OK))
				{
				}

				{
					pci_unmap_single(wlpptr->pPciDev, 
						ENDIAN_SWAP32(CURR_TXD(num)->PktPtr),
						CURR_TXD(num)->pSkBuff->len,
						PCI_DMA_TODEVICE);

					if(CURR_TXD(num)->staInfo && CURR_TXD(num)->type==IEEE_TYPE_DATA)
					{
						pStaInfo = (extStaDb_StaInfo_t *)CURR_TXD(num)->staInfo;
						pStaInfo->RateInfo = *(dbRateInfo_t *)&(CURR_TXD(num)->RateInfo);
						if (!pStaInfo->RateInfo.Format || (pStaInfo->RateInfo.Format && pStaInfo->RateInfo.RateIDMCS == 0))
							pStaInfo->aggr11n.threshold = 0;
						else
							pStaInfo->aggr11n.threshold = pStaInfo->aggr11n.thresholdBackUp;

					}
					
						WL_SKB_FREE(CURR_TXD(num)->pSkBuff);
				}
                CURR_TXD(num)->PktLen = 0;
				CURR_TXD(num)->pSkBuff = NULL;
				CURR_TXD(num)->Status = ENDIAN_SWAP32(EAGLE_TXD_STATUS_IDLE);
				((struct wlprivate_data *)(wlpptr->wlpd_p))->fwDescCnt[num]--;
				CURR_TXD(num) = CURR_TXD(num)->pNext;
                wmb();
			}
		}
	}
    SPIN_UNLOCK_IRQRESTORE(&wlpptr->wlpd_p->locks.xmitLock, flags);
	WLDBG_EXIT(DBG_LEVEL_13);
}

#define DES_ADD_OFFSET 6

int wlhandlepsxmit(struct net_device *netdev, struct sk_buff *skb, struct wlxmit_param *param)
{
	return 0;
}
INLINE int wlprocesspsq(struct net_device *netdev, extStaDb_StaInfo_t *pStaInfo, struct ieee80211_frame *hdr_p)
{
	return 0;
}

int wlxmit(struct net_device *netdev, struct sk_buff *skb, struct wlxmit_param *param)
{
	UINT8 type = param->type;
	extStaDb_StaInfo_t *pStaInfo = (extStaDb_StaInfo_t *)param->ptr;
	UINT32 bcast = param->flags & WLXMIT_PARAM_FLAGS_BCAST;
	BOOLEAN eap = (param->flags & WLXMIT_PARAM_FLAGS_EAPOL) ? TRUE: FALSE;
	BOOLEAN dofree = (param->flags & WLXMIT_PARAM_DONOT_FREE)? FALSE:TRUE;
	
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	vmacApInfo_t *vmacSta_p = wlpptr->vmacSta_p;
	MIB_802DOT11 *mib = vmacSta_p->Mib802dot11;
	MIB_RSNCONFIG	*mib_RSNConfig_p=vmacSta_p->Mib802dot11->RSNConfig ;
	u_int16_t *pFwLen;
	unsigned char buffer2[NBR_BYTES_IEEE80211HEADER];
	unsigned int wep_padding = 0, TxPriority=1, TxDesIndex=0, TxQueuePriority=0;
	unsigned int qos_padding = 0;
	unsigned int data_padding = 0;
	BOOLEAN ucastCipherCCMP = FALSE;
	BOOLEAN mcastCipherCCMP = FALSE;
	IEEEtypes_QoS_Ctl_t QosControl;
	UINT8 *ampduMacAddr = NULL;
	struct ieee80211_frame *ieee80211_hdr = (struct ieee80211_frame *)&skb->data[0];
	UINT8 AssociatedFlag = 0;
	UINT8 bssId[6];
	/* WDS_SUPPORT */
	struct ieee80211_frame *wh_wds;
	BOOLEAN wds = FALSE;
	struct wds_port *pWdsPort = NULL;
	UINT8 addr4[6];
	/* WDS_SUPPORT */
	unsigned int AmpduPck=FALSE;
	UINT8 xmitcontrol=0;
	UINT32 queueindex;
	WLDBG_ENTER(DBG_LEVEL_13);
	/* AUTOCHANNEL */
	if(vmacSta_p->StopTraffic)
	{
		if(dofree == TRUE)
		DROP_XMIT_FRAME(wlpptr, skb);
		return EAGAIN;
	} /* AUTOCHANNEL */

	memcpy(&buffer2[0],&skb->data[0],NBR_BYTES_IEEE80211HEADER);

	/* WDS_SUPPORT */
	if (vmacSta_p->VMacEntry.modeOfService != VMAC_MODE_CLNT_INFRA)
	{
		pWdsPort = getWdsPortFromNetDev(wlpptr, skb->dev);

		if (*(wlpptr->vmacSta_p->Mib802dot11->mib_wdsEnable) && (pWdsPort != NULL))
		{
			memcpy(&addr4[0],&skb->data[NBR_BYTES_IEEE80211HEADER + 2],6);
			wds = TRUE;
		}
	} /* WDS_SUPPORT */

	if(type == IEEE_TYPE_DATA)
	{
		/* For QoS data frames pick up the QoS information from the header*/
		if (ieee80211_hdr->FrmCtl.Subtype == QoS_DATA)
			*(UINT16 *)&QosControl = *(UINT16 *)&skb->data[NBR_BYTES_IEEE80211HEADER];
		else
			*(UINT16 *)&QosControl = 0;

		qos_padding = 2;
		if (!eap)
		{
			if(mib->Privacy->PrivInvoked)
				wep_padding = 4;

			if (mib->Privacy->RSNEnabled)
			{
				wep_padding = 8;
				if (bcast)
				{
					if ((!(mib->RSNConfigWPA2->WPA2Enabled || mib->RSNConfigWPA2->WPA2OnlyEnabled) && mib_RSNConfig_p->MulticastCipher[3] == 4) ||
						((mib->RSNConfigWPA2->WPA2Enabled || mib->RSNConfigWPA2->WPA2OnlyEnabled) && mib->RSNConfigWPA2->MulticastCipher[3] == 4))
					{
						mcastCipherCCMP = TRUE;
						data_padding = 0;
					}
					else
					{
						data_padding = 4;
					}
				}
				else
				{
					/* Unicast cipher CCMP */
					if (pStaInfo)
					{
						if ((pStaInfo->keyMgmtStateInfo.RsnIEBuf[0] == 48  && pStaInfo->keyMgmtStateInfo.RsnIEBuf[13] == 4)  ||
							(pStaInfo->keyMgmtStateInfo.RsnIEBuf[0] == 221 && pStaInfo->keyMgmtStateInfo.RsnIEBuf[17] == 4))
						{
							ucastCipherCCMP = TRUE;
							data_padding = 0;
						}
						else
						{
							data_padding = 4;
						}
					}
				}
			}
		}
        else
        {
            /* Use multicast data rate for eapol pkts */
            xmitcontrol |= EAGLE_TXD_XMITCTRL_USE_MC_RATE;
            TxPriority = 3;
        }
        
		if(!*(mib->QoSOptImpl))
		{
			//Non qos condition, goes to q 0 now
			if (!eap)
				TxPriority = 0;                
		}
		else
		{
#define SRC_ADD_OFFSET 16
			//the following check handles wmm w2w case 
			/* NBR_BYTES_CTRLSTATUS+NBR_BYTES_DURATION+NBR_BYTES_ADDR1+NBR_BYTES_ADDR2=2+2+6+6=16 */
			extStaDb_StaInfo_t *pStaSrc = NULL;
			if (vmacSta_p->VMacEntry.modeOfService != VMAC_MODE_CLNT_INFRA)
				pStaSrc = extStaDb_GetStaInfo(wlpptr,(IEEEtypes_MacAddr_t *)&(skb->data[SRC_ADD_OFFSET]), STADB_NO_BLOCK);
			if (pStaSrc)
			{
				TxPriority = 3;
			}
			else
			{
				if(!eap)
				{
					if((skb->priority)&0x7)
						TxPriority = AccCategoryQ[(skb->priority)&0x7];
					else
					TxPriority = AccCategoryQ[QosControl.tid ];
					if(pStaInfo && !pStaInfo->IsStaQSTA)
						TxPriority = 1;
				}
			}
		}
	}
	else
	{
		*(UINT16 *)&QosControl = 0;

		if(type == IEEE_TYPE_MANAGEMENT)
			TxPriority = 3;
		else
			TxPriority = 1;
	}

#if NUM_OF_DESCRIPTOR_DATA >3
#ifdef MCAST_PS_OFFLOAD_SUPPORT
	if(bcast && vmacSta_p->PwrSaveStnCnt)
	{
		TxDesIndex = NUM_EDCA_QUEUES + wlpptr->vmacSta_p->VMacEntry.macId;
	}else
#endif
	{
		TxDesIndex = TxPriority;
	} 
#endif

	if (((struct wlprivate_data *)(wlpptr->wlpd_p))->descData[TxDesIndex].pNextTxDesc == NULL)
	{
		if(dofree == TRUE)
		DROP_XMIT_FRAME(wlpptr, skb);
		return EAGAIN;
	}
	else
	{	
        /*Only queue to tx desc when Status is 0 (not when 0x1 or 0x80000000). If we queue even when Status==0x1
        	* (DMA'd to fw but txdone haven't change Status to 0), mismatch of fwDescCnt with actual number of desc with Status==0 
        	* could happen. E.g fwDescCnt 256 instead of 255 when there is one desc with Status==0. This can cause Tx to stall
		* when fwDescCnt==256 and pStaleTxDesc->Status==0.
		*/
		if (((struct wlprivate_data *)(wlpptr->wlpd_p))->descData[TxDesIndex].pNextTxDesc->Status != EAGLE_TXD_STATUS_IDLE)
		{	
			WLDBG_EXIT_INFO(DBG_LEVEL_13, "try again later");
			
			/*interrupt fw anyway*/
			if(((struct wlprivate_data *)(wlpptr->wlpd_p))->descData[TxDesIndex].pNextTxDesc->Status & ENDIAN_SWAP32(EAGLE_TXD_STATUS_FW_OWNED))
				writel(MACREG_H2ARIC_BIT_PPA_READY,wlpptr->ioBase1 + MACREG_REG_H2A_INTERRUPT_EVENTS);

			((struct wlprivate_data *)(wlpptr->wlpd_p))->txDoneCnt = 0;		
			if(dofree == TRUE)
			DROP_XMIT_FRAME(wlpptr, skb);
			return EAGAIN;
		}
	}

	// Addr4 is included so subtract 6 for headroom and push.
	if(skb_headroom(skb) < NBR_BYTES_ADD_TXFWINFO+wep_padding-qos_padding-6)
	{
		//the code should not come here, need to find out why this happens!!!!
		printk("skb_headroom error \n");
		if(dofree == TRUE)
		DROP_XMIT_FRAME(wlpptr, skb);
		return EAGAIN;
	}

    {
		pFwLen = (u_int16_t *) skb_push(skb, NBR_BYTES_ADD_TXFWINFO+wep_padding-qos_padding-6);
		*pFwLen = ENDIAN_SWAP16(skb->len - NBR_BYTES_COMPLETE_TXFWHEADER + wep_padding+data_padding);
	}
	memcpy(&skb->data[NBR_BYTES_FW_TX_PREPEND_LEN],&buffer2[0],NBR_BYTES_IEEE80211HEADER);
	memset(&skb->data[NBR_BYTES_IEEE80211HEADER+2],0,6+wep_padding);

	if(type == IEEE_TYPE_DATA)
	{
		if (!eap)
		{
			if (!bcast)
			{
				if (ucastCipherCCMP)
				{
					InsertCCMPHdr((UINT8 *) &skb->data[NBR_BYTES_IEEE80211HEADER+2+6],
						0, 
						pStaInfo->keyMgmtStateInfo.TxIV16, 
						pStaInfo->keyMgmtStateInfo.TxIV32);
					pStaInfo->keyMgmtStateInfo.TxIV16++;
					if (pStaInfo->keyMgmtStateInfo.TxIV16 == 0)
					{
						pStaInfo->keyMgmtStateInfo.TxIV32++;
					}

				}
			}
			else
			{
				if (mcastCipherCCMP)
				{
					if (pStaInfo && (vmacSta_p->VMacEntry.modeOfService == VMAC_MODE_CLNT_INFRA))
					{
						InsertCCMPHdr((UINT8 *) &skb->data[NBR_BYTES_IEEE80211HEADER+2+6],
							0, 
							pStaInfo->keyMgmtStateInfo.TxIV16, 
							pStaInfo->keyMgmtStateInfo.TxIV32);

						pStaInfo->keyMgmtStateInfo.TxIV16++;

						if (pStaInfo->keyMgmtStateInfo.TxIV16 == 0)
						{
							pStaInfo->keyMgmtStateInfo.TxIV32++;
						}
					}
					else
					{
						InsertCCMPHdr((UINT8 *) &skb->data[NBR_BYTES_IEEE80211HEADER+2+6]
						, mib->mib_MrvlRSN_GrpKey->g_KeyIndex
							, vmacSta_p->g_IV16
							, vmacSta_p->g_IV32);
						vmacSta_p->g_IV16++;
						if ( vmacSta_p->g_IV16 == 0 )
						{
							vmacSta_p->g_IV32++;
						}
					}
				}
			}
		}
		if(skb->truesize > MAX_AGGR_SIZE)
			wlpptr->netDevStats->tx_packets += skb->cb[0];
		else
			wlpptr->netDevStats->tx_packets++;
		wlpptr->netDevStats->tx_bytes += skb->len;

		/* add physical interface count for DATA */
		if(wlpptr->vmacSta_p->rootvmac){			
			if(skb->truesize > MAX_AGGR_SIZE)
	        	wlpptr->rootpriv->netDevStats->tx_packets += skb->cb[0];
			else
	        	wlpptr->rootpriv->netDevStats->tx_packets++;

			wlpptr->rootpriv->netDevStats->tx_bytes += skb->len;
		}
	}
	else
	{
		wlpptr->netDevStats->tx_packets++;
		wlpptr->netDevStats->tx_bytes += skb->len;		
	}
	/* AMPDU_SUPPORT_TX_CLIENT */
	if (vmacSta_p->VMacEntry.modeOfService == VMAC_MODE_CLNT_INFRA)
	{        
		smeGetStaLinkInfo(vmacSta_p->VMacEntry.id,
			&AssociatedFlag,
			&bssId[0]);
		ampduMacAddr = &bssId[0];
	}
	else /* AMPDU_SUPPORT_TX_CLIENT */
	{
		ampduMacAddr = (UINT8 *)&(skb->data[DES_ADD_OFFSET]);
	} 

	/* AMPDU_SUPPORT_SBA */
	if(type == IEEE_TYPE_DATA)
	{
#define MAX_TX_QUEUE 8
#define BA_QUEUE 4

		unsigned int i;

		for(i=0;i<MAX_SUPPORT_AMPDU_TX_STREAM;i++)
		{
			if((memcmp((UINT8 *)&wlpptr->wlpd_p->Ampdu_tx[i].MacAddr[0], ampduMacAddr,6)==0)
				&& wlpptr->wlpd_p->Ampdu_tx[i].InUse && (QosControl.tid == wlpptr->wlpd_p->Ampdu_tx[i].AccessCat)
				&& wlpptr->wlpd_p->Ampdu_tx[i].AddBaResponseReceive)
			{
				TxQueuePriority = (BA_QUEUE+i)%MAX_TX_QUEUE;
				AmpduPck=TRUE;
				break;
			}
		}

		if(AmpduPck==FALSE)
		{
			TxQueuePriority = TxPriority;
		}
	}
	/* AMPDU_SUPPORT_SBA */

	/* WDS_SUPPORT */
	if (wds)
	{
		wh_wds = (struct ieee80211_frame *) &skb->data[NBR_BYTES_FW_TX_PREPEND_LEN];
		IEEE80211_ADDR_COPY(wh_wds->addr4, addr4);
	} /* WDS_SUPPORT */

    /* for TCP ACK Enh */
    if(TxQueuePriority > 3 && TxQueuePriority < 8 )
	{
		if(skb->cb[1] == 1)
		{
			((struct wlprivate_data *)(wlpptr->wlpd_p))->descData[TxDesIndex].pNextTxDesc->tcpack_sn = *((u_int32_t *)&skb->cb[4]);
			((struct wlprivate_data *)(wlpptr->wlpd_p))->descData[TxDesIndex].pNextTxDesc->tcpack_src_dst = *((u_int32_t *)&skb->cb[8]);
			TxQueuePriority |=0x80;
		}
    } /* end TCP ACK Enh */

	((struct wlprivate_data *)(wlpptr->wlpd_p))->descData[TxDesIndex].pNextTxDesc->TxPriority = TxQueuePriority;				
	((struct wlprivate_data *)(wlpptr->wlpd_p))->descData[TxDesIndex].pNextTxDesc->QosCtrl =*(UINT16 *)&QosControl;
	((struct wlprivate_data *)(wlpptr->wlpd_p))->descData[TxDesIndex].pNextTxDesc->pSkBuff = skb;
	((struct wlprivate_data *)(wlpptr->wlpd_p))->descData[TxDesIndex].pNextTxDesc->PktLen = ENDIAN_SWAP16(skb->len);
	((struct wlprivate_data *)(wlpptr->wlpd_p))->descData[TxDesIndex].pNextTxDesc->ack_wcb_addr = 0; 
	((struct wlprivate_data *)(wlpptr->wlpd_p))->descData[TxDesIndex].pNextTxDesc->DataRate = 0;
	((struct wlprivate_data *)(wlpptr->wlpd_p))->descData[TxDesIndex].pNextTxDesc->staInfo = (UINT8*)pStaInfo;
	((struct wlprivate_data *)(wlpptr->wlpd_p))->descData[TxDesIndex].pNextTxDesc->type = type;
    wmb();
	if(AmpduPck==TRUE)
	{
		((struct wlprivate_data *)(wlpptr->wlpd_p))->descData[TxDesIndex].pNextTxDesc->xmitcontrol= xmitcontrol &0xfb;          //bit 0: use rateinfo, bit 1: disable ampdu
	}
	else
	{
		((struct wlprivate_data *)(wlpptr->wlpd_p))->descData[TxDesIndex].pNextTxDesc->xmitcontrol= xmitcontrol|0x4;
	}

	{
		((struct wlprivate_data *)(wlpptr->wlpd_p))->descData[TxDesIndex].pNextTxDesc->SapPktInfo = 0x00;
	}

    /* QUEUE_STATS: record drv latency and stamp fw start time */
    WLDBG_REC_PKT_DELTA_TIME(skb,readl(wlpptr->ioBase1 + 0xa600),
                ((struct wlprivate_data *)(wlpptr->wlpd_p))->descData[TxDesIndex].pNextTxDesc, TxPriority);

	((struct wlprivate_data *)(wlpptr->wlpd_p))->descData[TxDesIndex].pNextTxDesc->PktPtr =
		ENDIAN_SWAP32(pci_map_single(wlpptr->pPciDev, skb->data, 
		skb->len, PCI_DMA_TODEVICE));
	((struct wlprivate_data *)(wlpptr->wlpd_p))->descData[TxDesIndex].pNextTxDesc->Status =
		ENDIAN_SWAP32(/*EAGLE_TXD_STATUS_USED |*/ EAGLE_TXD_STATUS_FW_OWNED);
	((struct wlprivate_data *)(wlpptr->wlpd_p))->descData[TxDesIndex].pNextTxDesc = ((struct wlprivate_data *)(wlpptr->wlpd_p))->descData[TxDesIndex].pNextTxDesc->pNext;
	/* make sure all the memory transactions done by cpu were completed */
	wmb();	/*Data Memory Barrier*/
	SET_QUEUE_NUMBER(skb, queueindex);	

	if ((QUEUE_LEN(&((struct wlprivate_data *)(wlpptr->wlpd_p))->txQ[queueindex])==0))
	{
		writel(MACREG_H2ARIC_BIT_PPA_READY,
				wlpptr->ioBase1 + MACREG_REG_H2A_INTERRUPT_EVENTS);
	}

	netdev->trans_start = jiffies;
	((struct wlprivate_data *)(wlpptr->wlpd_p))->fwDescCnt[TxDesIndex]++;
	WLDBG_EXIT(DBG_LEVEL_13);
	return SUCCESS;
}

int wlxmitmfs(struct net_device *netdev, int qindex, extStaDb_StaInfo_t *pStaInfo )
{
	return FAIL;
}

int wlDataTxUnencr(struct sk_buff *skb, struct net_device *netdev, extStaDb_StaInfo_t *pStaInfo)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	unsigned long flags;
	UINT32 bcast = 0;
	WL_BUFF *wlb = NULL;

	WLDBG_ENTER(DBG_LEVEL_13);

	if ((netdev->flags & IFF_RUNNING) == 0)
	{
		DROP_XMIT_FRAME(wlpptr, skb);
		WLDBG_EXIT_INFO(DBG_LEVEL_13, "%s: itf not running", netdev->name);
		return -ENETDOWN;
	}
	WL_PREPARE_BUF_INFO(skb);
	if ((wlb = ieee80211_encap(WL_BUFF_PTR(skb), wlpptr, TRUE)) == NULL)
	{
		skb = NULL;
		goto error;
	}
	skb = WL_BUFF_SKB(wlb);
	if(skb == NULL)
		goto error;
	SPIN_LOCK_IRQSAVE(&wlpptr->wlpd_p->locks.xmitLock, flags);
	{
		struct wlxmit_param param;
		param.ptr = (void *)pStaInfo;
		param.type = IEEE_TYPE_DATA;
		param.psq = 0;
		param.flags = (bcast?WLXMIT_PARAM_FLAGS_BCAST:0)|WLXMIT_PARAM_FLAGS_EAPOL;
		param.eth = NULL;
		param.totallen = 0;
		if (wlxmit(netdev, skb, &param))
		{
			WLDBG_INFO(DBG_LEVEL_13, "could not xmit");
			SPIN_UNLOCK_IRQRESTORE(&wlpptr->wlpd_p->locks.xmitLock, flags);
			wlTxDone(netdev);
			goto error1;
		}
	}
	SPIN_UNLOCK_IRQRESTORE(&wlpptr->wlpd_p->locks.xmitLock, flags);
	WLDBG_EXIT(DBG_LEVEL_13);
	return 0;

error:
	if (skb)
	{
		DROP_XMIT_FRAME(wlpptr, skb);
	}
error1:
	WLDBG_EXIT_INFO(DBG_LEVEL_13, NULL);
	return 0;

}
