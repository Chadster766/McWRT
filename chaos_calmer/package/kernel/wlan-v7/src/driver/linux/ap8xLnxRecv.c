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
#include "wldebug.h"
#include "ap8xLnxRegs.h"
#include "ap8xLnxDesc.h"
#include "ap8xLnxIntf.h"
#include "ap8xLnxXmit.h"
#include "ap8xLnxFwcmd.h"
#include "IEEE_types.h"
#include "wl_mib.h"
#include "wl_hal.h"
#include "StaDb.h"
#include "ap8xLnxDma.h"
#include "ds.h"

/** local definitions **/
struct ieee80211_frame
{
	IEEEtypes_FrameCtl_t FrmCtl;
	UINT8	dur[2];
	UINT8	addr1[IEEEtypes_ADDRESS_SIZE];
	UINT8	addr2[IEEEtypes_ADDRESS_SIZE];
	UINT8	addr3[IEEEtypes_ADDRESS_SIZE];
	UINT8	seq[2];
	UINT8	addr4[IEEEtypes_ADDRESS_SIZE];
} PACK;

#define W836X_RSSI_OFFSET 8

/* default settings */

/** external functions **/
/** external data **/

/** internal functions **/

/** public data **/

/** private data **/

/** public functions **/
//#define roundup(x, y)   ((((x)+((y)-1))/(y))*(y))
#ifdef NAPI
void wlInterruptUnMask(struct net_device *netdev, int mask);
int wlRecvPoll(struct napi_struct *napi, int budget)
#else
void wlRecv(struct net_device *netdev)
#endif
{
#ifdef NAPI
	struct wlprivate *wlpptr = container_of(napi, struct wlprivate, napi);
	struct net_device *netdev = wlpptr->netDev;
	int work_to_do = budget;
#else
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
#endif
	int work_done = 0;
	wlrxdesc_t *pCurrent = wlpptr->wlpd_p->descData[0].pNextRxDesc;
	static Bool_e isFunctionBusy = WL_FALSE;
	int receivedHandled = 0;
	u_int32_t rxRdPtr;
	u_int32_t rxWrPtr;
	struct sk_buff *pRxSkBuff=NULL;
	void *pCurrentData;
	u_int8_t rxRate;
	int rxCount;
	int rssi;
	vmacApInfo_t *vmacSta_p = wlpptr->vmacSta_p;
	u_int32_t status;
#ifdef SOC_W8764
    u_int32_t rssi_paths;
#endif
#ifdef QUEUE_STATS_LATENCY
	UINT32 curr_tm; 	/*Used for Rx latency calculation*/
#endif
#ifndef ZERO_COPY_RX
	int allocLen;
#endif
	WLDBG_ENTER(DBG_LEVEL_14);

    /* In a corner case the descriptors may be uninitialized and not usable, accessing these may cause a crash */
	if (isFunctionBusy || (pCurrent == NULL))
	{
#ifdef NAPI
 		napi_complete(napi);		
		wlInterruptUnMask(netdev, MACREG_A2HRIC_BIT_RX_RDY);
#endif
		return
#ifdef NAPI
			1
#endif
			;
	}
	isFunctionBusy = WL_TRUE;

	rxRdPtr = readl(wlpptr->ioBase0 + wlpptr->wlpd_p->descData[0].rxDescRead);
	rxWrPtr = readl(wlpptr->ioBase0 + wlpptr->wlpd_p->descData[0].rxDescWrite);

#ifdef NAPI
	while ((work_done < work_to_do) && pCurrent->RxControl ==EAGLE_RXD_CTRL_DMA_OWN )
#else
	while ((pCurrent->RxControl ==EAGLE_RXD_CTRL_DMA_OWN)
#ifdef WL_KERNEL_26
		&& (work_done < vmacSta_p->work_to_do)
#endif
		)
#endif
	{
	
#ifdef QUEUE_STATS
#ifdef QUEUE_STATS_LATENCY
		{
		/* Calculate fw-to-drv DMA latency*/
		WLDBG_RX_REC_PKT_FWToDRV_TIME(pCurrent,readl(wlpptr->ioBase1 + 0xa600));
		}
#endif
#ifdef QUEUE_STATS_CNT_HIST
		/* Count Rx packets based on matching Tag */
        if((pCurrent->qsRxTag&0xf0) == 0xA0){
            WLDBG_INC_RX_RECV_POLL_CNT_STA((pCurrent->qsRxTag&0x0f));
            pCurrent->qsRxTag = 0;
        }
#endif
#endif
		
#ifdef AUTOCHANNEL
		{
			if(vmacSta_p->StopTraffic)
				goto out;
		}
#endif

		rxCount = ENDIAN_SWAP16(pCurrent->PktLen);
#ifdef ZERO_COPY_RX
		pRxSkBuff = pCurrent->pSkBuff;
		if (pRxSkBuff == NULL)
		{
			goto out;
		}
		pci_unmap_single(wlpptr->pPciDev, 
			ENDIAN_SWAP32(pCurrent->pPhysBuffData),
			wlpptr->wlpd_p->descData[0].rxBufSize,
			PCI_DMA_FROMDEVICE);
#else
		// at least mtu size, otherwise defragment will fail 
		if(	rxCount > netdev->mtu)
			allocLen = rxCount + NUM_EXTRA_RX_BYTES;
		else
			allocLen = netdev->mtu + NUM_EXTRA_RX_BYTES;
		pRxSkBuff = dev_alloc_skb(allocLen);
		if (pRxSkBuff == NULL)
		{
			WLDBG_INFO(DBG_LEVEL_14,"out of skb\n");
			goto out;
		}
#ifdef WL_KERNEL_26
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
		if(skb_linearize(pRxSkBuff))
#else
		if(skb_linearize(pRxSkBuff, GFP_ATOMIC))
#endif
		{
			dev_kfree_skb_any(pRxSkBuff);
			printk(KERN_ERR "%s: Need linearize memory\n", netdev->name);
			goto out;
		}

#endif

		//invalidate cashe for platiform with L2 cashe enabled.
		pCurrent->pPhysBuffData =
					ENDIAN_SWAP32(pci_map_single(wlpptr->pPciDev,
					pCurrent->pSkBuff->data,
					wlpptr->wlpd_p->descData[0].rxBufSize,
					PCI_DMA_FROMDEVICE));
		skb_reserve(pRxSkBuff, MIN_BYTES_HEADROOM);
#endif
		pCurrentData = pCurrent->pBuffData;
		rxRate = pCurrent->Rate;
		status = (u_int32_t)pCurrent->Status;
		pRxSkBuff->protocol = 0;
#ifdef WMON
		if( pCurrent->QosCtrl & 0x5)
		{
			g_wmon_videoTrafficRx ++ ;
		}
#endif //WMON
#ifndef ZERO_COPY_RX
		memcpy( pRxSkBuff->data,  pCurrentData + OFFS_RXFWBUFF_IEEE80211HEADER,  NBR_BYTES_COMPLETE_IEEE80211HEADER);
#endif
		if(pCurrent->QosCtrl & IEEE_QOS_CTL_AMSDU)
		{
			pRxSkBuff->protocol |= WL_WLAN_TYPE_AMSDU;
		}
		rssi = (int)pCurrent->RSSI + W836X_RSSI_OFFSET;
#ifdef SOC_W8764
        rssi_paths = *((u_int32_t *)&pCurrent->HwRssiInfo);
#endif
#ifdef WMON
        if( g_wmon_rssi_count >= WMON_MAX_RSSI_COUNT )
        {
        	g_wmon_rssi_count = 0;
        }
		g_wmon_rssi[g_wmon_rssi_count++] = rssi ;
#endif
#ifdef ZERO_COPY_RX
		if (skb_tailroom(pRxSkBuff) >= rxCount)
		{
			skb_put(pRxSkBuff, rxCount ); 
			skb_pull(pRxSkBuff, 2); 
		}
#else
		if (skb_tailroom(pRxSkBuff) >= (rxCount - 2))
		{
			memcpy( &pRxSkBuff->data[NBR_BYTES_COMPLETE_IEEE80211HEADER], 
			pCurrentData+OFFS_RXFWBUFF_IEEE80211PAYLOAD, 
			rxCount - OFFS_RXFWBUFF_IEEE80211PAYLOAD);
			skb_put(pRxSkBuff, rxCount - 2); // 2 byte len + 6 bytes address
		}
#endif
		else
		{
			WLDBG_INFO(DBG_LEVEL_14,"Not enough tail room =%x recvlen=%x, pCurrent=%x, pCurrentData=%x", skb_tailroom(pRxSkBuff), rxCount,pCurrent, pCurrentData);
			dev_kfree_skb_any(pRxSkBuff);
			goto out;
		}

		wlpptr->netDevStats.rx_packets++;
#ifdef AMPDU_SUPPORT
		if(pCurrent->HtSig2 & 0x8 )
		{
			u_int8_t ampdu_qos;
			/** use bit 3 for ampdu flag, and 0,1,2,3 for qos so as to save a register **/	
			ampdu_qos = 8|(pCurrent->QosCtrl&0x7);
#ifdef SOC_W8764
			work_done+=ieee80211_input(netdev, pRxSkBuff,rssi,rssi_paths,ampdu_qos,status);
#else
			work_done+=ieee80211_input(netdev, pRxSkBuff,rssi,ampdu_qos,status);
#endif
		}	
		else
		{
			u_int8_t ampdu_qos;
			/** use bit 3 for ampdu flag, and 0,1,2,3 for qos so as to save a register **/	
			ampdu_qos = 0|(pCurrent->QosCtrl&0x7);  	
#ifdef SOC_W8764
			work_done+=ieee80211_input(netdev, pRxSkBuff,rssi,rssi_paths,ampdu_qos,status);
#else
			work_done+=ieee80211_input(netdev, pRxSkBuff,rssi,ampdu_qos,status);
#endif
		}
#else
#ifdef SOC_W8764
		work_done+=ieee80211_input(netdev, pRxSkBuff,rssi,rssi_paths, status);
#else
		work_done+=ieee80211_input(netdev, pRxSkBuff,rssi, status);
#endif
#endif
		//wlpptr->netDevStats.rx_bytes += pRxSkBuff->len;
#ifdef ZERO_COPY_RX
		{
			pCurrent->pSkBuff   = dev_alloc_skb(wlpptr->wlpd_p->descData[0].rxBufSize);
			if (pCurrent->pSkBuff != NULL)
			{
#ifdef WL_KERNEL_26
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
				if(skb_linearize(pCurrent->pSkBuff))
#else
				if(skb_linearize(pCurrent->pSkBuff, GFP_ATOMIC))
#endif
				{
					dev_kfree_skb_any(pCurrent->pSkBuff);
					printk(KERN_ERR "%s: Need linearize memory\n", netdev->name);
					goto out;
				}
#endif
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
					wlpptr->wlpd_p->descData[0].rxBufSize/*+sizeof(struct skb_shared_info)*/,
					PCI_DMA_BIDIRECTIONAL));
			}
		}
#endif
out:

#ifdef QUEUE_STATS
#ifdef QUEUE_STATS_LATENCY
		/* Calculate drv latency and total latency from fw start to drv end*/
		{
			curr_tm =readl(wlpptr->ioBase1 + 0xa600);
			WLDBG_RX_REC_PKT_DRV_TIME(pCurrent, curr_tm);
			WLDBG_RX_REC_PKT_TOTAL_TIME(pCurrent, curr_tm);
		}
#endif
#endif


		receivedHandled++;
		pCurrent->RxControl = EAGLE_RXD_CTRL_DRIVER_OWN;
		pCurrent->QosCtrl =0;
		rxRdPtr = ENDIAN_SWAP32(pCurrent->pPhysNext);
		pCurrent = pCurrent->pNext;
	}
	writel(rxRdPtr, wlpptr->ioBase0 + wlpptr->wlpd_p->descData[0].rxDescRead);
	wlpptr->wlpd_p->descData[0].pNextRxDesc = pCurrent;
	isFunctionBusy = WL_FALSE;
	WLDBG_EXIT(DBG_LEVEL_14);
#ifdef NAPI
	if(work_done < work_to_do || (!netif_running(netdev))) {
 		napi_complete(napi);		
		wlInterruptUnMask(netdev, MACREG_A2HRIC_BIT_RX_RDY);
	}
	/* notify upper layer about more work to do */
	return( work_done );
#endif
}

