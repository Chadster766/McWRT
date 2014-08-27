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

#include "ap8xLnxDesc.h"
#include "apintf.h"
#include "ap8xLnxIntf.h"
#include "wldebug.h"

#define MAX_NUM_TX_RING_BYTES  MAX_NUM_TX_DESC * sizeof(wltxdesc_t)
#define MAX_NUM_RX_RING_BYTES  MAX_NUM_RX_DESC * sizeof(wlrxdesc_t)

#define FIRST_TXD(i) ((struct wlprivate_data *)(wlpptr->wlpd_p))->descData[i].pTxRing[0]
#define CURR_TXD(i)  ((struct wlprivate_data *)(wlpptr->wlpd_p))->descData[i].pTxRing[currDescr]
#define NEXT_TXD(i)  ((struct wlprivate_data *)(wlpptr->wlpd_p))->descData[i].pTxRing[currDescr+1]
#define LAST_TXD(i)  ((struct wlprivate_data *)(wlpptr->wlpd_p))->descData[i].pTxRing[MAX_NUM_TX_DESC-1]

#define FIRST_RXD ((struct wlprivate_data *)(wlpptr->wlpd_p))->descData[0].pRxRing[0]
#define CURR_RXD  ((struct wlprivate_data *)(wlpptr->wlpd_p))->descData[0].pRxRing[currDescr]
#define NEXT_RXD  ((struct wlprivate_data *)(wlpptr->wlpd_p))->descData[0].pRxRing[currDescr+1]
#define LAST_RXD  ((struct wlprivate_data *)(wlpptr->wlpd_p))->descData[0].pRxRing[MAX_NUM_RX_DESC-1]

int wlTxRingAlloc(struct net_device *netdev)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	int num;
	UINT8 *mem = (UINT8 *) pci_alloc_consistent(wlpptr->pPciDev,
		MAX_NUM_TX_RING_BYTES *NUM_OF_DESCRIPTOR_DATA,
		&((struct wlprivate_data *)(wlpptr->wlpd_p))->descData[0].pPhysTxRing);
	for(num =0; num < NUM_OF_DESCRIPTOR_DATA; num++)
	{

		WLDBG_ENTER_INFO(DBG_LEVEL_12, "allocating %i (0x%x) bytes",MAX_NUM_TX_RING_BYTES, MAX_NUM_TX_RING_BYTES);
		((struct wlprivate_data *)(wlpptr->wlpd_p))->descData[num].pTxRing =(wltxdesc_t *) (mem +num*MAX_NUM_TX_RING_BYTES);
		((struct wlprivate_data *)(wlpptr->wlpd_p))->descData[num].pPhysTxRing = (dma_addr_t)((UINT32)((struct wlprivate_data *)(wlpptr->wlpd_p))->descData[0].pPhysTxRing+num*MAX_NUM_TX_RING_BYTES);
		if (((struct wlprivate_data *)(wlpptr->wlpd_p))->descData[num].pTxRing == NULL)
		{
			WLDBG_ERROR(DBG_LEVEL_12, "can not alloc mem");
			return FAIL;
		}
		memset(((struct wlprivate_data *)(wlpptr->wlpd_p))->descData[num].pTxRing, 0x00, MAX_NUM_TX_RING_BYTES);
		WLDBG_EXIT_INFO(DBG_LEVEL_12, "TX ring vaddr: 0x%x paddr: 0x%x", 
			((struct wlprivate_data *)(wlpptr->wlpd_p))->descData[num].pTxRing, ((struct wlprivate_data *)(wlpptr->wlpd_p))->descData[num].pPhysTxRing);
	}
	return SUCCESS;
}

int wlTxRingInit(struct net_device *netdev)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	int currDescr;
	int num;

	WLDBG_ENTER_INFO(DBG_LEVEL_12, "initializing %i descriptors", MAX_NUM_TX_DESC);
	for(num =0; num < NUM_OF_DESCRIPTOR_DATA; num++)
	{
		QUEUE_INIT(&((struct wlprivate_data *)(wlpptr->wlpd_p))->txQ[num]);
		((struct wlprivate_data *)(wlpptr->wlpd_p))->fwDescCnt[num] =0;
		if (((struct wlprivate_data *)(wlpptr->wlpd_p))->descData[num].pTxRing != NULL)
		{
			for (currDescr = 0; currDescr < MAX_NUM_TX_DESC; currDescr++)
			{
				CURR_TXD(num).Status    = ENDIAN_SWAP32(EAGLE_TXD_STATUS_IDLE);
				CURR_TXD(num).pNext     = &NEXT_TXD(num);
				CURR_TXD(num).pPhysNext =
					ENDIAN_SWAP32((u_int32_t) ((struct wlprivate_data *)(wlpptr->wlpd_p))->descData[num].pPhysTxRing +
					((currDescr+1)*sizeof(wltxdesc_t)));
				WLDBG_INFO(DBG_LEVEL_12, 
					"txdesc: %i status: 0x%x (%i) vnext: 0x%p pnext: 0x%x",
					currDescr, EAGLE_TXD_STATUS_IDLE, EAGLE_TXD_STATUS_IDLE,
					CURR_TXD(num).pNext, ENDIAN_SWAP32(CURR_TXD(num).pPhysNext));
			}
			LAST_TXD(num).pNext = &FIRST_TXD(num);
			LAST_TXD(num).pPhysNext =
				ENDIAN_SWAP32((u_int32_t) ((struct wlprivate_data *)(wlpptr->wlpd_p))->descData[num].pPhysTxRing);
			((struct wlprivate_data *)(wlpptr->wlpd_p))->descData[num].pStaleTxDesc = &FIRST_TXD(num);
			((struct wlprivate_data *)(wlpptr->wlpd_p))->descData[num].pNextTxDesc  = &FIRST_TXD(num);

			WLDBG_EXIT_INFO(DBG_LEVEL_12, 
				"last txdesc vnext: 0x%p pnext: 0x%x pstale 0x%x vfirst 0x%x",
				LAST_TXD(num).pNext, ENDIAN_SWAP32(LAST_TXD(num).pPhysNext),
				((struct wlprivate_data *)(wlpptr->wlpd_p))->descData[num].pStaleTxDesc, ((struct wlprivate_data *)(wlpptr->wlpd_p))->descData[num].pNextTxDesc);
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
		QUEUE_PURGE(&((struct wlprivate_data *)(wlpptr->wlpd_p))->txQ[num]);
		((struct wlprivate_data *)(wlpptr->wlpd_p))->fwDescCnt[num] =0;
		if (((struct wlprivate_data *)(wlpptr->wlpd_p))->descData[num].pTxRing != NULL)
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
					{
						WL_SKB_FREE(CURR_TXD(num).pSkBuff);
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
		if (((struct wlprivate_data *)(wlpptr->wlpd_p))->descData[num].pTxRing != NULL)
		{
			for (currDescr = 0; currDescr < MAX_NUM_TX_DESC; currDescr++)
			{
                p1=blank;
                p2=blank;
			    if((UINT32)&CURR_TXD(num) == (UINT32)((struct wlprivate_data *)(wlpptr->wlpd_p))->descData[num].pStaleTxDesc)
			    {
			        p1 = str1;
			    }
			    if((UINT32)&CURR_TXD(num) == (UINT32)((struct wlprivate_data *)(wlpptr->wlpd_p))->descData[num].pNextTxDesc)
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

	((struct wlprivate_data *)(wlpptr->wlpd_p))->descData[0].pRxRing =
		(wlrxdesc_t *) pci_alloc_consistent(wlpptr->pPciDev,
		MAX_NUM_RX_RING_BYTES,
		&((struct wlprivate_data *)(wlpptr->wlpd_p))->descData[0].pPhysRxRing);
		
	if (((struct wlprivate_data *)(wlpptr->wlpd_p))->descData[0].pRxRing == NULL)
	{
		WLDBG_ERROR(DBG_LEVEL_12, "can not alloc mem");
		return FAIL;
	}
	memset(((struct wlprivate_data *)(wlpptr->wlpd_p))->descData[0].pRxRing, 0x00, MAX_NUM_RX_RING_BYTES);
	WLDBG_EXIT_INFO(DBG_LEVEL_12, "RX ring vaddr: 0x%x paddr: 0x%x", 
		((struct wlprivate_data *)(wlpptr->wlpd_p))->descData[0].pRxRing, ((struct wlprivate_data *)(wlpptr->wlpd_p))->descData[0].pPhysRxRing);
	return SUCCESS;
}

int wlRxRingInit(struct net_device *netdev)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	int currDescr;

	WLDBG_ENTER_INFO(DBG_LEVEL_12,  "initializing %i descriptors", MAX_NUM_RX_DESC);

	if (((struct wlprivate_data *)(wlpptr->wlpd_p))->descData[0].pRxRing != NULL)
	{
		((struct wlprivate_data *)(wlpptr->wlpd_p))->descData[0].rxBufSize = MAX_AGGR_SIZE;
		for (currDescr = 0; currDescr < MAX_NUM_RX_DESC; currDescr++)
		{
			CURR_RXD.pSkBuff   = dev_alloc_skb(((struct wlprivate_data *)(wlpptr->wlpd_p))->descData[0].rxBufSize);
			if(skb_linearize(CURR_RXD.pSkBuff))
			{
				WL_SKB_FREE(CURR_RXD.pSkBuff);
				printk(KERN_ERR "%s: Need linearize memory\n", netdev->name);
				return FAIL;
			}
			skb_reserve(CURR_RXD.pSkBuff , MIN_BYTES_HEADROOM);
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
					((struct wlprivate_data *)(wlpptr->wlpd_p))->descData[0].rxBufSize,
					PCI_DMA_FROMDEVICE));
				CURR_RXD.pNext = &NEXT_RXD;
				CURR_RXD.pPhysNext =
					ENDIAN_SWAP32((u_int32_t) ((struct wlprivate_data *)(wlpptr->wlpd_p))->descData[0].pPhysRxRing +
					((currDescr+1)*sizeof(wlrxdesc_t)));
				WLDBG_INFO(DBG_LEVEL_12, 
					"rxdesc: %i status: 0x%x (%i) len: 0x%x (%i)",
					currDescr, EAGLE_TXD_STATUS_IDLE, EAGLE_TXD_STATUS_IDLE,
					((struct wlprivate_data *)(wlpptr->wlpd_p))->descData[0].rxBufSize, ((struct wlprivate_data *)(wlpptr->wlpd_p))->descData[0].rxBufSize);
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
			ENDIAN_SWAP32((u_int32_t) ((struct wlprivate_data *)(wlpptr->wlpd_p))->descData[0].pPhysRxRing);
		LAST_RXD.pNext             = &FIRST_RXD;
		((struct wlprivate_data *)(wlpptr->wlpd_p))->descData[0].pNextRxDesc = &FIRST_RXD;

		WLDBG_EXIT_INFO(DBG_LEVEL_12, 
			"last rxdesc vnext: 0x%p pnext: 0x%x vfirst 0x%x",
			LAST_RXD.pNext, ENDIAN_SWAP32(LAST_RXD.pPhysNext),
			((struct wlprivate_data *)(wlpptr->wlpd_p))->descData[0].pNextRxDesc);
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

	if (((struct wlprivate_data *)(wlpptr->wlpd_p))->descData[0].pRxRing != NULL)
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
					((struct wlprivate_data *)(wlpptr->wlpd_p))->descData[0].rxBufSize,
					PCI_DMA_FROMDEVICE);
				WL_SKB_FREE(CURR_RXD.pSkBuff);
				WLDBG_INFO(DBG_LEVEL_12, 
					"unmapped+free'd rxdesc %i vaddr: 0x%p paddr: 0x%x len: %i",
					currDescr, CURR_RXD.pBuffData, 
					ENDIAN_SWAP32(CURR_RXD.pPhysBuffData),
					((struct wlprivate_data *)(wlpptr->wlpd_p))->descData[0].rxBufSize);
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
	if (((struct wlprivate_data *)(wlpptr->wlpd_p))->descData[0].pTxRing != NULL)
		pci_free_consistent(wlpptr->pPciDev, 
		MAX_NUM_TX_RING_BYTES*NUM_OF_DESCRIPTOR_DATA,
		((struct wlprivate_data *)(wlpptr->wlpd_p))->descData[0].pTxRing, 
		((struct wlprivate_data *)(wlpptr->wlpd_p))->descData[0].pPhysTxRing);

	for(num =0; num < NUM_OF_DESCRIPTOR_DATA; num++)
	{
		if (((struct wlprivate_data *)(wlpptr->wlpd_p))->descData[num].pTxRing != NULL)
		{
			((struct wlprivate_data *)(wlpptr->wlpd_p))->descData[num].pTxRing = NULL;
		}
		((struct wlprivate_data *)(wlpptr->wlpd_p))->descData[num].pStaleTxDesc = NULL;
		((struct wlprivate_data *)(wlpptr->wlpd_p))->descData[num].pNextTxDesc  = NULL;
	}
	WLDBG_EXIT(DBG_LEVEL_12);
}

void wlRxRingFree(struct net_device *netdev)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);

	WLDBG_ENTER(DBG_LEVEL_12);

	if (((struct wlprivate_data *)(wlpptr->wlpd_p))->descData[0].pRxRing != NULL)
	{
		wlRxRingCleanup(netdev);
		pci_free_consistent(wlpptr->pPciDev, 
			MAX_NUM_RX_RING_BYTES,
			((struct wlprivate_data *)(wlpptr->wlpd_p))->descData[0].pRxRing, 
			((struct wlprivate_data *)(wlpptr->wlpd_p))->descData[0].pPhysRxRing);
		((struct wlprivate_data *)(wlpptr->wlpd_p))->descData[0].pRxRing = NULL;
	}
	((struct wlprivate_data *)(wlpptr->wlpd_p))->descData[0].pNextRxDesc = NULL;
	WLDBG_EXIT(DBG_LEVEL_12);
}
