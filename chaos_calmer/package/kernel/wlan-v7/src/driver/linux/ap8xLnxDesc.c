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


/** include files **/
#include "ap8xLnxDesc.h"
#include "ap8xLnxIntf.h"
#include "wldebug.h"
/* default settings */

/** external functions **/

/** external data **/

/** internal functions **/

/** public data **/

/** private data **/

/** private functions **/

/** local definitions **/

#define MAX_NUM_TX_RING_BYTES  MAX_NUM_TX_DESC * sizeof(wltxdesc_t)
#define MAX_NUM_RX_RING_BYTES  MAX_NUM_RX_DESC * sizeof(wlrxdesc_t)

#define FIRST_TXD(i) wlpptr->wlpd_p->descData[i].pTxRing[0]
#define CURR_TXD(i)  wlpptr->wlpd_p->descData[i].pTxRing[currDescr]
#define NEXT_TXD(i)  wlpptr->wlpd_p->descData[i].pTxRing[currDescr+1]
#define LAST_TXD(i)  wlpptr->wlpd_p->descData[i].pTxRing[MAX_NUM_TX_DESC-1]

#define FIRST_RXD wlpptr->wlpd_p->descData[0].pRxRing[0]
#define CURR_RXD  wlpptr->wlpd_p->descData[0].pRxRing[currDescr]
#define NEXT_RXD  wlpptr->wlpd_p->descData[0].pRxRing[currDescr+1]
#define LAST_RXD  wlpptr->wlpd_p->descData[0].pRxRing[MAX_NUM_RX_DESC-1]

/** public functions **/

int wlTxRingAlloc(struct net_device *netdev)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	int num;
	UINT8 *mem = (UINT8 *) pci_alloc_consistent(wlpptr->pPciDev,
		MAX_NUM_TX_RING_BYTES *NUM_OF_DESCRIPTOR_DATA,
		&wlpptr->wlpd_p->descData[0].pPhysTxRing);
	for(num =0; num < NUM_OF_DESCRIPTOR_DATA; num++)
	{

		WLDBG_ENTER_INFO(DBG_LEVEL_12, "allocating %i (0x%x) bytes",MAX_NUM_TX_RING_BYTES, MAX_NUM_TX_RING_BYTES);
		wlpptr->wlpd_p->descData[num].pTxRing =(wltxdesc_t *) (mem +num*MAX_NUM_TX_RING_BYTES);
		wlpptr->wlpd_p->descData[num].pPhysTxRing = (dma_addr_t)((UINT32)wlpptr->wlpd_p->descData[0].pPhysTxRing+num*MAX_NUM_TX_RING_BYTES);
		if (wlpptr->wlpd_p->descData[num].pTxRing == NULL)
		{
			WLDBG_ERROR(DBG_LEVEL_12, "can not alloc mem");
			return FAIL;
		}
		memset(wlpptr->wlpd_p->descData[num].pTxRing, 0x00, MAX_NUM_TX_RING_BYTES);
		WLDBG_EXIT_INFO(DBG_LEVEL_12, "TX ring vaddr: 0x%x paddr: 0x%x", 
			wlpptr->wlpd_p->descData[num].pTxRing, wlpptr->wlpd_p->descData[num].pPhysTxRing);
	}
	return SUCCESS;
}

int wlTxRingInit(struct net_device *netdev)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	int currDescr;
#ifdef AMSDU_AGGREQ_FOR_8K
	struct sk_buff *newskb;
#endif
	int num;

	WLDBG_ENTER_INFO(DBG_LEVEL_12, "initializing %i descriptors", MAX_NUM_TX_DESC);
#ifdef AMSDU_AGGREQ_FOR_8K
	skb_queue_head_init(&wlpptr->wlpd_p->aggreQ);
	for (currDescr = 0; currDescr < MAX_NUM_AGGR_BUFF; currDescr++)
	{
		newskb = dev_alloc_skb(MAX_AGGR_SIZE);
		if(newskb)
		{
#ifdef WL_KERNEL_26
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
		if(skb_linearize(newskb))
#else
		if(skb_linearize(newskb, GFP_ATOMIC))
#endif
		{
			dev_kfree_skb_any(newskb);
			printk(KERN_ERR "%s: Need linearize memory\n", netdev->name);
			return FAIL;
		}

#endif
			skb_queue_tail(&wlpptr->wlpd_p->aggreQ, newskb);
		}
		else
		{
			printk(KERN_ERR "%s: Allocate TX buffer failed. Insufficient system memory\n", netdev->name);
			return FAIL;
		}
	}
#endif
	for(num =0; num < NUM_OF_DESCRIPTOR_DATA; num++)
	{
		skb_queue_head_init(&wlpptr->wlpd_p->txQ[num]);
		wlpptr->wlpd_p->fwDescCnt[num] =0;
		if (wlpptr->wlpd_p->descData[num].pTxRing != NULL)
		{
			for (currDescr = 0; currDescr < MAX_NUM_TX_DESC; currDescr++)
			{
				CURR_TXD(num).Status    = ENDIAN_SWAP32(EAGLE_TXD_STATUS_IDLE);
				CURR_TXD(num).pNext     = &NEXT_TXD(num);
				CURR_TXD(num).pPhysNext =
					ENDIAN_SWAP32((u_int32_t) wlpptr->wlpd_p->descData[num].pPhysTxRing +
					((currDescr+1)*sizeof(wltxdesc_t)));
				WLDBG_INFO(DBG_LEVEL_12, 
					"txdesc: %i status: 0x%x (%i) vnext: 0x%p pnext: 0x%x",
					currDescr, EAGLE_TXD_STATUS_IDLE, EAGLE_TXD_STATUS_IDLE,
					CURR_TXD(num).pNext, ENDIAN_SWAP32(CURR_TXD(num).pPhysNext));
			}
			LAST_TXD(num).pNext = &FIRST_TXD(num);
			LAST_TXD(num).pPhysNext =
				ENDIAN_SWAP32((u_int32_t) wlpptr->wlpd_p->descData[num].pPhysTxRing);
			wlpptr->wlpd_p->descData[num].pStaleTxDesc = &FIRST_TXD(num);
			wlpptr->wlpd_p->descData[num].pNextTxDesc  = &FIRST_TXD(num);

			WLDBG_EXIT_INFO(DBG_LEVEL_12, 
				"last txdesc vnext: 0x%p pnext: 0x%x pstale 0x%x vfirst 0x%x",
				LAST_TXD(num).pNext, ENDIAN_SWAP32(LAST_TXD(num).pPhysNext),
				wlpptr->wlpd_p->descData[num].pStaleTxDesc, wlpptr->wlpd_p->descData[num].pNextTxDesc);
		}
		else
		{
			WLDBG_ERROR(DBG_LEVEL_12, "no valid TX mem");
			return FAIL;
		}
	}
	return SUCCESS;
}

void wlTxRingCleanup(struct net_device *netdev)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	int cleanedTxDescr = 0;
	int currDescr;
	int num;

	WLDBG_ENTER(DBG_LEVEL_12);

	for(num =0; num < NUM_OF_DESCRIPTOR_DATA; num++)
	{
		skb_queue_purge(&wlpptr->wlpd_p->txQ[num]);
		wlpptr->wlpd_p->fwDescCnt[num] =0;
		if (wlpptr->wlpd_p->descData[num].pTxRing != NULL)
		{
			for (currDescr = 0; currDescr < MAX_NUM_TX_DESC; currDescr++)
			{
				if (CURR_TXD(num).pSkBuff != NULL)
				{
					WLDBG_INFO(DBG_LEVEL_12, 
						"unmapped and free'd txdesc %i vaddr: 0x%p paddr: 0x%x",
						currDescr, CURR_TXD(num).pSkBuff->data, 
						ENDIAN_SWAP32(CURR_TXD(num).PktPtr));
					pci_unmap_single(wlpptr->pPciDev,
						ENDIAN_SWAP32(CURR_TXD(num).PktPtr),
						CURR_TXD(num).pSkBuff->len,
						PCI_DMA_TODEVICE);
#ifdef AMSDU_AGGREQ_FOR_8K
					if(CURR_TXD(num).pSkBuff->truesize > MAX_AGGR_SIZE)
					{
						skb_queue_tail(&wlpptr->wlpd_p->aggreQ, CURR_TXD(num).pSkBuff);
					}
					else
#endif
					{
						dev_kfree_skb_any(CURR_TXD(num).pSkBuff);
					}
					CURR_TXD(num).Status    = ENDIAN_SWAP32(EAGLE_TXD_STATUS_IDLE);
					CURR_TXD(num).pSkBuff   = NULL;
					CURR_TXD(num).PktPtr    = 0;
					CURR_TXD(num).PktLen    = 0;
					cleanedTxDescr++;
				}
			}
		}
	}
#ifdef AMSDU_AGGREQ_FOR_8K
	skb_queue_purge(&wlpptr->wlpd_p->aggreQ);
#endif
	WLDBG_EXIT_INFO(DBG_LEVEL_12, "cleaned %i TX descr", cleanedTxDescr);
}

void wlTxDescriptorDump(struct net_device *netdev)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	int currDescr;
	int num;
	char *p1=NULL;
	char *p2=NULL;
	char str1[12]=" <- CURR_TXD";
	char str2[14]=" <- Next_TXD";
	char blank[2]=" ";

	for(num =0; num < NUM_OF_DESCRIPTOR_DATA; num++)
	{
		if (wlpptr->wlpd_p->descData[num].pTxRing != NULL)
		{
			for (currDescr = 0; currDescr < MAX_NUM_TX_DESC; currDescr++)
			{
                p1=blank;
                p2=blank;
			    if((UINT32)&CURR_TXD(num) == (UINT32)wlpptr->wlpd_p->descData[num].pStaleTxDesc)
			    {
			        p1 = str1;
			    }
			    if((UINT32)&CURR_TXD(num) == (UINT32)wlpptr->wlpd_p->descData[num].pNextTxDesc)
			    {
			        p2 = str2;
			    }
                printk("TxDescriptor(%d.%d) Status=0x%x %s %s\n", num, currDescr, CURR_TXD(num).Status, p1, p2);
			}
		}
	}
}

int wlRxRingAlloc(struct net_device *netdev)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);

	WLDBG_ENTER_INFO(DBG_LEVEL_12, "allocating %i (0x%x) bytes",
		MAX_NUM_RX_RING_BYTES, MAX_NUM_RX_RING_BYTES);

	wlpptr->wlpd_p->descData[0].pRxRing =
		(wlrxdesc_t *) pci_alloc_consistent(wlpptr->pPciDev,
		MAX_NUM_RX_RING_BYTES,
		&wlpptr->wlpd_p->descData[0].pPhysRxRing);
	if (wlpptr->wlpd_p->descData[0].pRxRing == NULL)
	{
		WLDBG_ERROR(DBG_LEVEL_12, "can not alloc mem");
		return FAIL;
	}
	memset(wlpptr->wlpd_p->descData[0].pRxRing, 0x00, MAX_NUM_RX_RING_BYTES);
	WLDBG_EXIT_INFO(DBG_LEVEL_12, "RX ring vaddr: 0x%x paddr: 0x%x", 
		wlpptr->wlpd_p->descData[0].pRxRing, wlpptr->wlpd_p->descData[0].pPhysRxRing);
	return SUCCESS;
}

int wlRxRingInit(struct net_device *netdev)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	int currDescr;

	WLDBG_ENTER_INFO(DBG_LEVEL_12,  "initializing %i descriptors", MAX_NUM_RX_DESC);

	if (wlpptr->wlpd_p->descData[0].pRxRing != NULL)
	{
		wlpptr->wlpd_p->descData[0].rxBufSize = MAX_AGGR_SIZE;
		for (currDescr = 0; currDescr < MAX_NUM_RX_DESC; currDescr++)
		{
			CURR_RXD.pSkBuff   = dev_alloc_skb(wlpptr->wlpd_p->descData[0].rxBufSize);
#ifdef WL_KERNEL_26
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
			if(skb_linearize(CURR_RXD.pSkBuff))
#else
			if(skb_linearize(CURR_RXD.pSkBuff, GFP_ATOMIC))
#endif
			{
				dev_kfree_skb_any(CURR_RXD.pSkBuff);
				printk(KERN_ERR "%s: Need linearize memory\n", netdev->name);
				return FAIL;
			}
#endif
#ifdef ZERO_COPY_RX
			skb_reserve(CURR_RXD.pSkBuff , MIN_BYTES_HEADROOM);
#endif
			CURR_RXD.RxControl = EAGLE_RXD_CTRL_DRIVER_OWN;
			CURR_RXD.Status    = EAGLE_RXD_STATUS_OK;
			CURR_RXD.QosCtrl   = 0x0000;
			CURR_RXD.Channel   = 0x00;
			CURR_RXD.RSSI      = 0x00;
			CURR_RXD.SQ2       = 0x00;

			if (CURR_RXD.pSkBuff != NULL)
			{
				CURR_RXD.PktLen    = 6*netdev->mtu + NUM_EXTRA_RX_BYTES;
				CURR_RXD.pBuffData = CURR_RXD.pSkBuff->data;
				CURR_RXD.pPhysBuffData =
					ENDIAN_SWAP32(pci_map_single(wlpptr->pPciDev,
					CURR_RXD.pSkBuff->data,
					wlpptr->wlpd_p->descData[0].rxBufSize,
					PCI_DMA_FROMDEVICE));
				CURR_RXD.pNext = &NEXT_RXD;
				CURR_RXD.pPhysNext =
					ENDIAN_SWAP32((u_int32_t) wlpptr->wlpd_p->descData[0].pPhysRxRing +
					((currDescr+1)*sizeof(wlrxdesc_t)));
				WLDBG_INFO(DBG_LEVEL_12, 
					"rxdesc: %i status: 0x%x (%i) len: 0x%x (%i)",
					currDescr, EAGLE_TXD_STATUS_IDLE, EAGLE_TXD_STATUS_IDLE,
					wlpptr->wlpd_p->descData[0].rxBufSize, wlpptr->wlpd_p->descData[0].rxBufSize);
				WLDBG_INFO(DBG_LEVEL_12, 
					"rxdesc: %i vnext: 0x%p pnext: 0x%x", currDescr,
					CURR_RXD.pNext, ENDIAN_SWAP32(CURR_RXD.pPhysNext));
			} else
			{
				WLDBG_ERROR(DBG_LEVEL_12, 
					"rxdesc %i: no skbuff available", currDescr);
				return FAIL;
			}
		}
		LAST_RXD.pPhysNext =
			ENDIAN_SWAP32((u_int32_t) wlpptr->wlpd_p->descData[0].pPhysRxRing);
		LAST_RXD.pNext             = &FIRST_RXD;
		wlpptr->wlpd_p->descData[0].pNextRxDesc = &FIRST_RXD;

		WLDBG_EXIT_INFO(DBG_LEVEL_12, 
			"last rxdesc vnext: 0x%p pnext: 0x%x vfirst 0x%x",
			LAST_RXD.pNext, ENDIAN_SWAP32(LAST_RXD.pPhysNext),
			wlpptr->wlpd_p->descData[0].pNextRxDesc);
		return SUCCESS;
	}
	WLDBG_ERROR(DBG_LEVEL_12, "no valid RX mem");
	return FAIL;
}

void wlRxRingCleanup(struct net_device *netdev)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	int currDescr;

	WLDBG_ENTER(DBG_LEVEL_12);

	if (wlpptr->wlpd_p->descData[0].pRxRing != NULL)
	{
		for (currDescr = 0; currDescr < MAX_NUM_RX_DESC; currDescr++)
		{
			if (CURR_RXD.pSkBuff != NULL)
			{
				if (skb_shinfo(CURR_RXD.pSkBuff)->nr_frags)
				{
					skb_shinfo(CURR_RXD.pSkBuff)->nr_frags = 0;
				}
				if (skb_shinfo(CURR_RXD.pSkBuff)->frag_list)
				{
					skb_shinfo(CURR_RXD.pSkBuff)->frag_list = NULL;
				}
				pci_unmap_single(wlpptr->pPciDev, 
					ENDIAN_SWAP32(CURR_RXD.pPhysBuffData),
					wlpptr->wlpd_p->descData[0].rxBufSize,
					PCI_DMA_FROMDEVICE);
				dev_kfree_skb_any(CURR_RXD.pSkBuff);
				WLDBG_INFO(DBG_LEVEL_12, 
					"unmapped+free'd rxdesc %i vaddr: 0x%p paddr: 0x%x len: %i",
					currDescr, CURR_RXD.pBuffData, 
					ENDIAN_SWAP32(CURR_RXD.pPhysBuffData),
					wlpptr->wlpd_p->descData[0].rxBufSize);
				CURR_RXD.pBuffData = NULL;
				CURR_RXD.pSkBuff = NULL;
			}
		}
	}
	WLDBG_EXIT(DBG_LEVEL_12);
}

void wlTxRingFree(struct net_device *netdev)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	int num;
	WLDBG_ENTER(DBG_LEVEL_12);
	if (wlpptr->wlpd_p->descData[0].pTxRing != NULL)
		pci_free_consistent(wlpptr->pPciDev, 
		MAX_NUM_TX_RING_BYTES*NUM_OF_DESCRIPTOR_DATA,
		wlpptr->wlpd_p->descData[0].pTxRing, 
		wlpptr->wlpd_p->descData[0].pPhysTxRing);

	for(num =0; num < NUM_OF_DESCRIPTOR_DATA; num++)
	{
		if (wlpptr->wlpd_p->descData[num].pTxRing != NULL)
		{
			wlpptr->wlpd_p->descData[num].pTxRing = NULL;
		}
		wlpptr->wlpd_p->descData[num].pStaleTxDesc = NULL;
		wlpptr->wlpd_p->descData[num].pNextTxDesc  = NULL;
	}
	WLDBG_EXIT(DBG_LEVEL_12);
}

void wlRxRingFree(struct net_device *netdev)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);

	WLDBG_ENTER(DBG_LEVEL_12);

	if (wlpptr->wlpd_p->descData[0].pRxRing != NULL)
	{
		wlRxRingCleanup(netdev);
		pci_free_consistent(wlpptr->pPciDev, 
			MAX_NUM_RX_RING_BYTES,
			wlpptr->wlpd_p->descData[0].pRxRing, 
			wlpptr->wlpd_p->descData[0].pPhysRxRing);
		wlpptr->wlpd_p->descData[0].pRxRing = NULL;
	}
	wlpptr->wlpd_p->descData[0].pNextRxDesc = NULL;
	WLDBG_EXIT(DBG_LEVEL_12);
}

