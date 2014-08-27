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

#ifndef AP8X_INTF_H_
#define AP8X_INTF_H_

#include <linux/version.h>
#include <linux/module.h>
#include <linux/pci.h>
#include <linux/interrupt.h>
#include <linux/spinlock.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/skbuff.h>
#include <linux/delay.h>
#include <linux/if_ether.h>   
#include <linux/if_arp.h>   
#include <linux/init.h>
#include <linux/net.h>
#include <linux/wireless.h>

#include <net/iw_handler.h>
#include "IEEE_types.h"
#include "wltypes.h"
#include "ap8xLnxDesc.h"
#include "ap8xLnxApi.h"
#include "wl_mib.h"
#include "wl_hal.h"
#include "dfs.h"
#include "buildModes.h"

#include <linux/spinlock.h>
#include <asm/atomic.h>
#include <linux/interrupt.h>
#include <linux/smp.h>
#include "osif.h"

extern int wlInit(WL_NETDEV *, u_int16_t);
extern int wlDeinit(WL_NETDEV *);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
extern irqreturn_t  wlISR(int irq, void *dev_id);
#else
extern irqreturn_t wlISR(int, void *, struct pt_regs *);
#endif
extern void wlInterruptEnable(WL_NETDEV *);
extern void wlInterruptDisable(WL_NETDEV *);
extern void wlFwReset(WL_PRIV *);
extern int wlChkAdapter(WL_PRIV *);
extern void wlSendEvent(WL_NETDEV *dev, int, IEEEtypes_MacAddr_t *,const char *);

#define WLSNDEVT(dev, cmd, Addr, info) wlSendEvent(dev,cmd, Addr, info)

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,29)
#define NETDEV_PRIV(pre, dev)  ((pre *)netdev_priv(dev))
#define NETDEV_PRIV_P(pre, dev)  ((pre *)dev->ml_priv)
#define NETDEV_PRIV_S(dev)  (dev->ml_priv)
#else
#define NETDEV_PRIV(pre, dev) ((pre *)dev->priv)
#define NETDEV_PRIV_P(pre, dev)  ((pre *)dev->priv)
#define NETDEV_PRIV_S(dev)	(dev->priv)
#endif

struct wldesc_data {
	dma_addr_t       pPhysTxRing;          /* ptr to first TX desc (phys.)    */
	wltxdesc_t    *pTxRing;              /* ptr to first TX desc (virt.)    */
	wltxdesc_t    *pNextTxDesc;          /* next TX desc that can be used   */
	wltxdesc_t    *pStaleTxDesc;         /* the staled TX descriptor        */
	dma_addr_t       pPhysRxRing;          /* ptr to first RX desc (phys.)    */
	wlrxdesc_t    *pRxRing;              /* ptr to first RX desc (virt.)    */
	wlrxdesc_t    *pNextRxDesc;          /* next RX desc that can be used   */
	unsigned int     wcbBase;              /* FW base offset for registers    */
	unsigned int     rxDescWrite;          /* FW descriptor write position    */
	unsigned int     rxDescRead;           /* FW descriptor read position     */
	unsigned int     rxBufSize;            /* length of the RX buffers        */
};

#define MAX_SUPPORT_AMPDU_TX_STREAM 4  /* AMPDU_SUPPORT_SBA */

#define MAX_AMPDU_REORDER_BUFFER MAX_AID
#define MAX_AC 4
#define MAX_UP 8
#define MAX_AC_SEQNO 4096

/* WME stream classes */
#define	WME_AC_BE	0		/* best effort */
#define	WME_AC_BK	1		/* background */
#define	WME_AC_VI	2		/* video */
#define	WME_AC_VO	3		/* voice */

struct wlprivate_data {
	struct _wlprivate_data appriv;

	int 			SDRAMSIZE_Addr;
	int 			CardDeviceInfo;
	int 			fwDescCnt[NUM_OF_DESCRIPTOR_DATA];/* number of descriptors owned by fw at any one time */
	int 			txDoneCnt;/* number of tx packet to call wlTXDONE() */
	vmacApInfo_t 		*vmacampdurxap_p;
	UINT8 			ampdurxmacaddrset;
	struct wlprivate *masterwlp;
	struct timer_list	Timer; /* timer tick for Timer.c	 */
	Bool_t				isTxTimeout;	  /* timeout may collide with scan*/
	Bool_t				inReset;		  /* is chip currently resetting  */
	Bool_t				inResetQ;		  /* is chip currently resetting  */
	struct wlpriv_stats   privStats;		/* wireless statistic data		*/
	struct iw_statistics  wStats;		  /* wireless statistic data	  */
	DECLARE_QUEUE(aggreQ);
	DECLARE_QUEUE(txQ[NUM_OF_DESCRIPTOR_DATA]);
	struct wldesc_data	descData[NUM_OF_DESCRIPTOR_DATA];		/* various descriptor data		*/
	UINT8 isTxTaskScheduled;			/*To keep scheduling status of a tx task*/
	struct tasklet_struct txtask;
	struct tasklet_struct rxtask;
	struct work_struct 	resettask;
	/* DFS_SUPPORT */
	struct work_struct dfstask;
	struct work_struct csatask;
	/* DFS_SUPPORT */
	struct work_struct 		kickstatask;		
};
extern int wlinitcnt;
extern UINT8   tmpScanResults[NUM_OF_WLMACS][MAX_SCAN_BUF_SIZE];
extern UINT8   tmpNumScanDesc[NUM_OF_WLMACS];
extern int wlResetTask(WL_NETDEV *dev);
extern void wlLinkMgt(WL_NETDEV *netdev, UINT8 phyIndex);
#endif /* AP8X_INTF_H_ */
