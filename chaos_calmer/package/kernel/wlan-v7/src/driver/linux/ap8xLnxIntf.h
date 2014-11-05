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

#define CONFIG_SPINLOCK_DEBUG
#ifdef CONFIG_SPINLOCK_DEBUG
struct spinlock_debug
{
	spinlock_t l;
	atomic_t locked_by;
};

struct rwlock_debug
{
	rwlock_t l;
	long read_locked_map;
	long write_locked_map;
};

#define SPIN_LOCK_INIT(x)	\
spin_lock_init(&(x)->l); \
atomic_set(&(x)->locked_by, -1) 


#define DECLARE_LOCK(l)                                                 \
struct spinlock_debug l 

#define MUST_BE_LOCKED(l)                                               \
do { if (atomic_read(&(l)->locked_by) != smp_processor_id())            \
        printk("ASSERT %s:%u %s unlocked\n", __FILE__, __LINE__, #l);   \
} while(0)
 
#define MUST_BE_UNLOCKED(l)                                             \
do { if (atomic_read(&(l)->locked_by) == smp_processor_id())            \
        printk("ASSERT %s:%u %s locked\n", __FILE__, __LINE__, #l);     \
} while(0)

#define SPIN_LOCK_IRQSAVE(lk, flags)                                             \
 do {                                                            \
       MUST_BE_UNLOCKED(lk);                                   \
	spin_lock_irqsave(&(lk)->l, flags);                                 \
       atomic_set(&(lk)->locked_by, smp_processor_id());       \
} while(0)


#define SPIN_UNLOCK_IRQRESTORE(lk, flags)                           \
do {                                            \
        MUST_BE_LOCKED(lk);                     \
        atomic_set(&(lk)->locked_by, -1);       \
	spin_unlock_irqrestore(&(lk)->l, flags);               \
} while(0)

#else
#define DECLARE_LOCK(l) spinlock_t l

#define MUST_BE_LOCKED(l)
#define MUST_BE_UNLOCKED(l)
#define SPIN_LOCK_INIT(l) spin_lock_init(l)
#define SPIN_LOCK_IRQSAVE(l, f) spin_lock_irqsave(l, f)
#define SPIN_UNLOCK_IRQRESTORE(l, f) spin_unlock_irqrestore(l, f)
 
#endif /*CONFIG_SPINLOCK_DEBUG*/


#define MAX_CARDS_SUPPORT 2
#define MAX_VMAC_INSTANCE_AP	NUMOFAPS
#define MAX_VMAC_MIB_INSTANCE	NUMOFAPS+1

#ifdef WL_KERNEL_24
#undef IRQ_NONE 
#define IRQ_NONE 0
#undef IRQ_HANDLED
#define IRQ_HANDLED 1
#ifndef irqreturn_t
#define irqreturn_t void
#endif
#endif 

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,29)
#define NETDEV_PRIV(pre, dev)  ((pre *)netdev_priv(dev))
#define NETDEV_PRIV_P(pre, dev)  ((pre *)dev->ml_priv)
#define NETDEV_PRIV_S(dev)  (dev->ml_priv)
#else
#define NETDEV_PRIV(pre, dev) ((pre *)dev->priv)
#define NETDEV_PRIV_P(pre, dev)  ((pre *)dev->priv)
#define NETDEV_PRIV_S(dev)	(dev->priv)
#endif

extern int wlInit(struct net_device *, u_int16_t);
extern int wlDeinit(struct net_device *);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
extern irqreturn_t  wlISR(int irq, void *dev_id);
#else
extern irqreturn_t wlISR(int, void *, struct pt_regs *);
#endif
extern void wlInterruptEnable(struct net_device *);
extern void wlInterruptDisable(struct net_device *);
extern void wlFwReset(struct net_device *);
extern int wlChkAdapter(struct net_device *);
extern void wlSendEvent(struct net_device *dev, int, IEEEtypes_MacAddr_t *,const char *);

#ifdef ENABLE_WLSNDEVT
#define WLSNDEVT(dev, cmd, Addr, info) wlSendEvent(dev,cmd, Addr, info)
#else
#define WLSNDEVT(dev, cmd, Addr, info)
#endif
#ifdef WDS_FEATURE
int wlstop_wds(struct net_device *netdev);
#endif

#ifdef MCAST_PS_OFFLOAD_SUPPORT
#define NUM_OF_DESCRIPTOR_DATA (4 + NUMOFAPS) 
#else
#define NUM_OF_DESCRIPTOR_DATA 4
#endif
#ifdef AP82S//WL_KERNEL_26  
#define MAX_NUM_AGGR_BUFF 128//256
#define MAX_NUM_RX_DESC        200//256
#else
#define MAX_NUM_AGGR_BUFF 256
#define MAX_NUM_RX_DESC        256
#endif
/*3839 ~ 4k*/
#define MAX_AGGR_SIZE		4096
#define MAX_NUM_TX_DESC        256
#define MIN_BYTES_HEADROOM      64
#define NUM_EXTRA_RX_BYTES     (2*MIN_BYTES_HEADROOM)

#define ENDIAN_SWAP32(_val)   (cpu_to_le32(_val))
#define ENDIAN_SWAP16(_val)   (cpu_to_le16(_val))


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

struct wllocks {   
	DECLARE_LOCK(xmitLock);             /* used to protect TX actions      */
	DECLARE_LOCK(fwLock);               /* used to protect FW commands     */
};

struct wlhw_data {
	u_int32_t        fwReleaseNumber;      /* MajNbr:MinNbr:SubMin:PatchLevel */
	u_int8_t         hwVersion;            /* plain number indicating version */
	u_int8_t         hostInterface;        /* plain number of interface       */
	u_int16_t        maxNumTXdesc;         /* max number of TX descriptors    */
	u_int16_t        maxNumMCaddr;         /* max number multicast addresses  */
	u_int16_t        numAntennas;          /* number antennas used            */
	u_int16_t        regionCode;           /* region (eg. 0x10 for USA FCC)   */
	unsigned char    macAddr[ETH_ALEN];    /* well known -> AA:BB:CC:DD:EE:FF */
};

struct wlpriv_stats {
	u_int32_t        skbheaderroomfailure;
	u_int32_t		tsoframecount;
	u_int32_t	weakiv_count;
	u_int32_t	weakiv_threshold_count;

};

#ifdef AMPDU_SUPPORT
#ifdef AMPDU_SUPPORT_SBA
#ifdef SOC_W8864
#define MAX_SUPPORT_AMPDU_TX_STREAM 4
#else
#define MAX_SUPPORT_AMPDU_TX_STREAM 7  /** for superfly3, only 2 stream of ampdu is supported, add one more for s/w ba **/
#endif
#else
#define MAX_SUPPORT_AMPDU_TX_STREAM 2 
#endif

#define MAX_AMPDU_REORDER_BUFFER MAX_AID
#define MAX_AC 4
#define MAX_UP 8
#define MAX_AC_SEQNO 4096

/* WME stream classes */
#define	WME_AC_BE	0		/* best effort */
#define	WME_AC_BK	1		/* background */
#define	WME_AC_VI	2		/* video */
#define	WME_AC_VO	3		/* voice */

typedef struct
{
	vmacApInfo_t *vmacSta_p;
	UINT8 MacAddr[6];
	UINT8 AccessCat;
	UINT8 InUse;
	UINT8 DialogToken;
	Timer timer;
	UINT8 initTimer;
	UINT8 AddBaResponseReceive;
	UINT32 TimeOut;
	UINT16 start_seqno;
#ifdef DYNAMIC_BA_SUPPORT
	UINT32 txa_avgpps;
	UINT32 txa_ac;
	UINT32 txa_pkts;
	UINT32 txa_lastsample;
#endif
	UINT32 ReleaseTimestamp;
}Ampdu_tx_t;

#define TID_TO_WME_AC(_tid) (      \
	((_tid) == 0 || (_tid) == 3) ? WME_AC_BE : \
	((_tid) < 3) ? WME_AC_BK : \
	((_tid) < 6) ? WME_AC_VI : \
	WME_AC_VO)


typedef struct
{
	struct sk_buff  *pFrame[MAX_UP][MAX_AMPDU_REORDER_BUFFER];
	UINT16 ExpectedSeqNo[MAX_UP][MAX_AMPDU_REORDER_BUFFER];
	UINT16 CurrentSeqNo[MAX_UP];
	UINT16 ReOrdering[MAX_UP];
	UINT8 AddBaReceive[MAX_UP];
	UINT32 Time[MAX_UP];
	Timer timer[MAX_UP];
	UINT8 timer_init[MAX_UP];
}Ampdu_Pck_Reorder_t;

#endif
struct wlprivate_data {
	dma_addr_t			pPhysCmdBuf;	  /* pointer to CmdBuf (physical) */
	struct timer_list		Timer; /* timer tick for Timer.c	 */
	Bool_t				isMtuChanged;	  /* change may interact with open*/
	Bool_t				isTxTimeout;	  /* timeout may collide with scan*/
	Bool_t				inReset;		  /* is chip currently resetting  */
	Bool_t				inResetQ;		  /* is chip currently resetting  */
	struct wllocks		   	locks;			/* various spinlocks			*/
	struct wlpriv_stats    	privStats;		/* wireless statistic data		*/
	struct iw_statistics	 	wStats;		  /* wireless statistic data	  */
	struct sk_buff_head   	aggreQ;
	struct sk_buff_head   	txQ[NUM_OF_DESCRIPTOR_DATA];
	struct wldesc_data	descData[NUM_OF_DESCRIPTOR_DATA];		/* various descriptor data		*/
#ifdef WL_KERNEL_26
	UINT8 isTxTaskScheduled;			/*To keep scheduling status of a tx task*/
#ifdef USE_TASKLET
	struct tasklet_struct txtask;
#else
	struct work_struct 		txtask;
#endif

#ifdef USE_TASKLET
	struct tasklet_struct rxtask;
#else
	struct work_struct 		rxtask;
#endif
	struct work_struct 		resettask;
#ifdef MRVL_DFS
	struct work_struct dfstask;
	struct work_struct csatask;
#endif

struct work_struct 		kickstatask;		


#endif
	int 			SDRAMSIZE_Addr;
	int 			CardDeviceInfo;
	int 			fwDescCnt[NUM_OF_DESCRIPTOR_DATA];/* number of descriptors owned by fw at any one time */
	int 			txDoneCnt;/* number of tx packet to call wlTXDONE() */
	int 			vmacIndex;
	Bool_t 			inSendCmd;
	vmacApInfo_t 		*vmacampdurxap_p;
	UINT8 			ampdurxmacaddrset;
#ifdef AMPDU_SUPPORT
	Ampdu_Pck_Reorder_t AmpduPckReorder[MAX_AID+1];
	Ampdu_tx_t Ampdu_tx[MAX_SUPPORT_AMPDU_TX_STREAM];
	UINT8 Global_DialogToken;
#endif
	struct wlprivate *masterwlp;
#ifdef MRVL_DFS
	DfsAp *pdfsApMain ;
#endif //MRVL_DFS
	UINT8 TxGf;
	UINT8 NonGFSta; 
	UINT8 BcnAddHtOpMode;
	UINT8 legClients;
	UINT8 n20MClients;					
	UINT8 nClients;	
	UINT8 legAPCount;
#ifdef COEXIST_20_40_SUPPORT
	UINT8 BcnAddHtAddChannel;
#endif
	struct net_device 	*rootdev;
#ifdef SSU_SUPPORT
    dma_addr_t			pPhysSsuBuf;
#endif    
};
struct wlprivate {
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,25)
	struct net_device        		netDevPriv;          /* the net_device struct        */
#endif
	struct net_device        		*netDev;          /* the net_device struct        */
#ifdef NAPI
	struct napi_struct napi;
#endif
	struct net_device_stats  	netDevStats;     /* net_device statistics        */
	struct pci_dev          		*pPciDev;         /* for access to pci cfg space  */
	void                    			*ioBase0;         /* MEM Base Address Register 0  */
	void                    			*ioBase1;         /* MEM Base Address Register 1  */
	unsigned short          		*pCmdBuf;         /* pointer to CmdBuf (virtual)  */
	struct wlhw_data		hwData; 		/* Adapter HW specific info 	*/
	vmacApInfo_t 			*vmacSta_p;
#ifdef CLIENT_SUPPORT
	void                    			*clntParent_priv_p;
#endif /* CLIENT_SUPPORT */
	int (*wlreset)(struct net_device *netdev);
	struct net_device 			*master;
	struct net_device 			*vdev[MAX_VMAC_INSTANCE_AP+1]; //+1 station
	struct wlprivate_data 		*wlpd_p;
	UINT8 calTbl[200];
	UINT8 *FwPointer;
	UINT32 FwSize;	
	UINT8 mfgEnable;
	UINT32 cmdFlags;  /* Command flags */
	struct net_device *txNetdev_p;
	UINT32 nextBarNum;
	UINT32 chipversion;
	UINT32 mfgLoaded;
#ifdef SSU_SUPPORT
    unsigned short                  *pSsuBuf;
#endif
};

extern struct net_device *mainNetdev_p[NUM_OF_WLMACS];
extern int wlinitcnt;
extern UINT8   tmpScanResults[NUM_OF_WLMACS][MAX_SCAN_BUF_SIZE];
extern UINT8   tmpNumScanDesc[NUM_OF_WLMACS];
extern int wlResetTask(struct net_device *dev);
extern void wlLinkMgt(struct net_device *netdev, UINT8 phyIndex);
#endif /* AP8X_INTF_H_ */
