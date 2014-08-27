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
#include "ds.h"


#define W836X_RSSI_OFFSET 8

void wlRecv(struct net_device *netdev)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	int work_done = 0;
	wlrxdesc_t *pCurrent = ((struct wlprivate_data *)(wlpptr->wlpd_p))->descData[0].pNextRxDesc;
	static Bool_e isFunctionBusy = WL_FALSE;
	int receivedHandled = 0;
	u_int32_t rxRdPtr;
	u_int32_t rxWrPtr;
	struct sk_buff *pRxSkBuff=NULL;
	WL_BUFF *wlb = NULL;
	void *pCurrentData;
	u_int8_t rxRate;
	int rxCount;
	int rssi;
	vmacApInfo_t *vmacSta_p = wlpptr->vmacSta_p;
	u_int32_t status;
    u_int32_t rssi_paths;
	WLDBG_ENTER(DBG_LEVEL_14);

    /* In a corner case the descriptors may be uninitialized and not usable, accessing these may cause a crash */
	if (isFunctionBusy || (pCurrent == NULL))
	{
		return;
	}
	isFunctionBusy = WL_TRUE;

	rxRdPtr = readl(wlpptr->ioBase0 + ((struct wlprivate_data *)(wlpptr->wlpd_p))->descData[0].rxDescRead);
	rxWrPtr = readl(wlpptr->ioBase0 + ((struct wlprivate_data *)(wlpptr->wlpd_p))->descData[0].rxDescWrite);

	while ((pCurrent->RxControl ==EAGLE_RXD_CTRL_DMA_OWN)
		&& (work_done < vmacSta_p->work_to_do)
		)
	{		
		/* AUTOCHANNEL */
		{
			if(vmacSta_p->StopTraffic)
				goto out;
		}

		rxCount = ENDIAN_SWAP16(pCurrent->PktLen);
		pRxSkBuff = pCurrent->pSkBuff;
		if (pRxSkBuff == NULL)
		{
			goto out;
		}
		pci_unmap_single(wlpptr->pPciDev, 
			ENDIAN_SWAP32(pCurrent->pPhysBuffData),
			((struct wlprivate_data *)(wlpptr->wlpd_p))->descData[0].rxBufSize,
			PCI_DMA_FROMDEVICE);
		pCurrentData = pCurrent->pBuffData;
		rxRate = pCurrent->Rate;
		status = (u_int32_t)pCurrent->Status;
		pRxSkBuff->protocol = 0;
		if(pCurrent->QosCtrl & IEEE_QOS_CTL_AMSDU)
		{
			pRxSkBuff->protocol |= WL_WLAN_TYPE_AMSDU;
		}
		rssi = (int)pCurrent->RSSI + W836X_RSSI_OFFSET;
        rssi_paths = *((u_int32_t *)&pCurrent->HwRssiInfo);
		if (skb_tailroom(pRxSkBuff) >= rxCount)
		{
			skb_put(pRxSkBuff, rxCount ); 
			skb_pull(pRxSkBuff, 2); 
		}
		else
		{
			WLDBG_INFO(DBG_LEVEL_14,"Not enough tail room =%x recvlen=%x, pCurrent=%x, pCurrentData=%x", WL_BUFF_TAILROOM(pRxSkBuff), rxCount,pCurrent, pCurrentData);
			WL_SKB_FREE(pRxSkBuff);
			goto out;
		}

		wlpptr->netDevStats->rx_packets++;
		wlb = WL_BUFF_PTR(pRxSkBuff);
		WL_PREPARE_BUF_INFO(pRxSkBuff);
		if(pCurrent->HtSig2 & 0x8 )
		{
			u_int8_t ampdu_qos;
			/** use bit 3 for ampdu flag, and 0,1,2,3 for qos so as to save a register **/	
			ampdu_qos = 8|(pCurrent->QosCtrl&0x7);
			work_done+=ieee80211_input(wlpptr, wlb,rssi,rssi_paths,ampdu_qos,status);
		}	
		else
		{
			u_int8_t ampdu_qos;
			/** use bit 3 for ampdu flag, and 0,1,2,3 for qos so as to save a register **/	
			ampdu_qos = 0|(pCurrent->QosCtrl&0x7); 
			work_done+=ieee80211_input(wlpptr, wlb,rssi,rssi_paths,ampdu_qos,status);
		}

		wlpptr->netDevStats->rx_bytes += pRxSkBuff->len;
		{
			pCurrent->pSkBuff   = dev_alloc_skb(((struct wlprivate_data *)(wlpptr->wlpd_p))->descData[0].rxBufSize);
			if (pCurrent->pSkBuff != NULL)
			{
				if(skb_linearize(pCurrent->pSkBuff))
				{
					WL_SKB_FREE(pCurrent->pSkBuff);
					printk(KERN_ERR "%s: Need linearize memory\n", netdev->name);
					goto out;
				}
				skb_reserve(pCurrent->pSkBuff , MIN_BYTES_HEADROOM);
				pCurrent->Status    = EAGLE_RXD_STATUS_OK;
				pCurrent->QosCtrl   = 0x0000;
				pCurrent->Channel   = 0x00;
				pCurrent->RSSI      = 0x00;
				pCurrent->SQ2       = 0x00;

				pCurrent->PktLen    = 6*netdev->mtu + NUM_EXTRA_RX_BYTES;
				pCurrent->pBuffData = pCurrent->pSkBuff->data;
				pCurrent->pPhysBuffData =
					ENDIAN_SWAP32(pci_map_single(wlpptr->pPciDev,
					pCurrent->pSkBuff->data,
					((struct wlprivate_data *)(wlpptr->wlpd_p))->descData[0].rxBufSize/*+sizeof(struct skb_shared_info)*/,
					PCI_DMA_BIDIRECTIONAL));
			}
		}
out:

		receivedHandled++;
		pCurrent->RxControl = EAGLE_RXD_CTRL_DRIVER_OWN;
		pCurrent->QosCtrl =0;
		rxRdPtr = ENDIAN_SWAP32(pCurrent->pPhysNext);
		pCurrent = pCurrent->pNext;
	}
	writel(rxRdPtr, wlpptr->ioBase0 + ((struct wlprivate_data *)(wlpptr->wlpd_p))->descData[0].rxDescRead);
	((struct wlprivate_data *)(wlpptr->wlpd_p))->descData[0].pNextRxDesc = pCurrent;
	isFunctionBusy = WL_FALSE;
	WLDBG_EXIT(DBG_LEVEL_14);
}

