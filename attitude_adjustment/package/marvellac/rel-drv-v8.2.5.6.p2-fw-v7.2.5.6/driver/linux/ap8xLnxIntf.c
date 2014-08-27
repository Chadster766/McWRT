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
#include <linux/module.h>
#include <linux/ethtool.h>
#include "apintf.h"
#include "wldebug.h"
#include "fwDl.h"
#include "apRegs.h"
#include "ap8xLnxVer.h"
#include "ap8xLnxDesc.h"
#include "fwCmd.h"
#include "ap8xLnxXmit.h"
#include "ap8xLnxIntf.h"
#include "ap8xLnxRecv.h"
#include "ap8xLnxWlLog.h" /* IEEE80211_DH */
#include "mib.h"
#include "wlvmac.h"
#include <linux/workqueue.h>
#include "wl_mib.h"
#include "wl_hal.h"
#include "smeMain.h" // DFS_SUPPORT
#include "linkmgt.h"
#include "ap8xLnxWlLog.h"
#include "mlmeApi.h"
#include "mlmeParent.h"
#include "StaDb.h"
#include "ewb_hash.h"
#include "dfsMgmt.h"
#include "macMgmtMlme.h"
#include "wds.h"

/** local definitions **/
#if LINUX_VERSION_CODE >=KERNEL_VERSION(3,8,0)
static struct pci_device_id wlid_tbl[MAX_CARDS_SUPPORT+1] = {
	{ 0x11ab,0x2a02, PCI_ANY_ID, PCI_ANY_ID, 0, 0,
	(unsigned long) "Marvell AP-8x 802.11n adapter"},
	{ 0x11ab,PCI_ANY_ID, PCI_ANY_ID, PCI_ANY_ID, 0, 0,
	(unsigned long) "Marvell AP-8x 802.11n adapter 1"},
	{ 0,0, 0, 0, 0, 0,
	0}
};
#else
static struct pci_device_id wlid_tbl[MAX_CARDS_SUPPORT+1] __devinitdata = {
	{ 0x11ab,0x2a02, PCI_ANY_ID, PCI_ANY_ID, 0, 0,
	(unsigned long) "Marvell AP-8x 802.11n adapter"},
	{ 0x11ab,PCI_ANY_ID, PCI_ANY_ID, PCI_ANY_ID, 0, 0,
	(unsigned long) "Marvell AP-8x 802.11n adapter 1"},
	{ 0,0, 0, 0, 0, 0,
	0}
};
#endif
MODULE_AUTHOR("Marvell Semiconductor, Inc.");
MODULE_LICENSE("Proprietary");
MODULE_SUPPORTED_DEVICE("Marvell AP-8x 802.11n adapter");
MODULE_DEVICE_TABLE(pci, wlid_tbl);

#if LINUX_VERSION_CODE >=KERNEL_VERSION(2,6,25)
#define SET_MODULE_OWNER(x)
#endif
#define CMD_BUF_SIZE        0x4000
#define MAX_ISR_ITERATION   1 // 10


#ifndef __MOD_INC_USE_COUNT
#define WL_MOD_INC_USE(_m, _err)                                     \
	{                               \
	isIfUsed++;                                   \
	if (!try_module_get(_m)) {                                    \
	printk( "%s: try_module_get?!?\n", __func__); \
	_err;                                                     \
	}                                                             \
	}
#define WL_MOD_DEC_USE(_m)                                           \
	if (isIfUsed ) {                                \
	--isIfUsed;                                  \
	module_put(_m);                                               \
	}
#else
#define WL_MOD_INC_USE(_m, _err)                                     \
	{							   \
	isIfUsed++; 								  \
	MOD_INC_USE_COUNT;                                            \
	}
#define WL_MOD_DEC_USE(_m)                                           \
	if (isIfUsed ) {								\
	--isIfUsed; 								 \
	MOD_DEC_USE_COUNT;                                            \
	}
#endif

/** external functions **/
SINT8 evtDFSMsg(WL_PRIV *wlpptr, UINT8 *message);

extern int ap8x_stat_proc_register(WL_NETDEV *dev);
extern int ap8x_dump_proc_register(WL_NETDEV *dev);
extern int ap8x_stat_proc_unregister(WL_NETDEV *dev);
extern int ap8x_dump_proc_unregister(WL_NETDEV *dev);
extern int ap8x_remove_folder(void );
extern vmacApInfo_t * wlvmacInit(WL_PRIV *wlp, WL_PRIV *wlpptr,char *addr, UINT32 mode, int phyMacId);

extern void MrvlMICErrorHdl(WL_PRIV *wlpptr,COUNTER_MEASURE_EVENT event);
extern void MrvlICVErrorHdl(WL_PRIV *wlpptr);
extern extStaDb_Status_e extStaDb_RemoveStaNSendDeauthMsg(WL_PRIV *wlpptr,IEEEtypes_MacAddr_t *Addr_p);				

/** internal functions **/

static int	wlInit_wds(struct wlprivate *wlpptr);
static int wlreset_wds(WL_NETDEV *netdev);

static int wlInit_client(struct wlprivate *wlp, unsigned char * macAddr_p, unsigned char *ApRootmacAddr_p);
static int wlstop_client(WL_NETDEV *netdev);
int wlreset_client(WL_NETDEV *netdev);

extern void rtnl_lock(void);
extern void rtnl_unlock(void);

UINT8   tmpScanResults[NUM_OF_WLMACS][MAX_SCAN_BUF_SIZE];
UINT8   tmpNumScanDesc[NUM_OF_WLMACS];

static int wlstop_mbss(WL_NETDEV *netdev);
static int wlopen_mbss(WL_NETDEV *netdev);
int wlRadarDetection(WL_NETDEV *netdev); /* DFS_SUPPORT */
int wlApplyCSAChannel(WL_NETDEV *netdev); /* DFS_SUPPORT */

int wlConsecTxFail(WL_NETDEV *netdev); 	

/** private data **/
static int isIfUsed = 0;

/** private functions **/
static int __init wlmodule_init(void);
static void __exit wlmodule_exit(void);
#if LINUX_VERSION_CODE >=KERNEL_VERSION(3,8,0)
static int wlprobe(struct pci_dev *, const struct pci_device_id *);
static void wlremove(struct pci_dev *);
#else
static int __devinit wlprobe(struct pci_dev *, const struct pci_device_id *);
static void __devexit wlremove(struct pci_dev *);
#endif
static int wlsuspend(struct pci_dev *, pm_message_t);
static int wlresume(struct pci_dev *);
#ifdef WL_DEBUG
static const char * wlgetAdapterDescription(u_int32_t, u_int32_t);
#endif
static int wlInit_mbss(struct wlprivate *wlp, unsigned char * macAddr);

void wlshowinfo(WL_NETDEV *netdev)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	vmacApInfo_t *vmacSta_p = wlpptr->vmacSta_p;
	int i;
	printk("netdev: %x %s\n", (unsigned int)netdev, netdev->name);
	printk("        addr %s\n", mac_display(netdev->dev_addr));
	printk("        netdev->flags & IFF_RUNNING=%x\n", (unsigned int)(netdev->flags & IFF_RUNNING));
	printk("\n");
	
	printk("wlpptr: %x\n", (unsigned int)wlpptr);
	printk("        rootpriv=%x\n", (unsigned int)(wlpptr->rootpriv));
	printk("        vmacSta_p=%x\n", (unsigned int)(wlpptr->vmacSta_p));
	printk("        netDev=%x\n", (unsigned int)(wlpptr->netDev));
	printk("        wlpd_p=%x\n", (unsigned int)(wlpptr->wlpd_p));

	printk("\n");
	
	printk("vmacSta_p: %x\n", (unsigned int)vmacSta_p);
	printk("        dev=%x\n", (unsigned int)(vmacSta_p->dev));
	printk("        rootvmac=%x\n", (unsigned int)(vmacSta_p->rootvmac));
	printk("        addr=%s\n", mac_display(vmacSta_p->macStaAddr));
	printk("        InfUpFlag=%x\n", (unsigned int)(vmacSta_p->InfUpFlag));
	printk("        mtu=%x\n", (unsigned int)(vmacSta_p->mtu));
	printk("\n");	
	printk("wdsPort:\n");
	for (i = 0; i < 6; i++)
	{
		if(vmacSta_p->wdsPort[i].netDevWds){
			printk("      %s\n", vmacSta_p->wdsPort[i].netDevWds->name);
			printk("      dev_addr: %s\n", mac_display(vmacSta_p->wdsPort[i].netDevWds->dev_addr));
			printk("      addr: %s\n", mac_display(vmacSta_p->wdsPort[i].wdsMacAddr));
			printk("      netDevWds: %x\n", (unsigned int)(vmacSta_p->wdsPort[i].netDevWds));
			printk("\n");
		}		
	}
}

static inline void ap_spin_lock_init(spinlock_t *l)
{
	spin_lock_init(l);
}

static inline void ap_spin_lock_irqsave(spinlock_t *l, unsigned long *flags)
{
	spin_lock_irqsave(l, *flags);
}

static inline void ap_spin_unlock_irqrestore(spinlock_t *l, unsigned long flags)
{
	spin_unlock_irqrestore(l, flags);
}

static inline u32 ap_readl(const volatile void __iomem *addr)
{
	return (readl(addr));
}

static inline void ap_writel(u32 val, void __iomem *addr)
{
	writel(val, addr);
}

static inline u32 ap_netrandom(void)
{
	return (net_random());
}

static inline void ap_mdelay(unsigned long ms)
{
	mdelay(ms);
}

struct ap_config ap8x_config = {
	sizeof(vmacApInfo_t),
	sizeof(WL_SYS_CFG_DATA),
	sizeof(struct sk_buff_head),
	sizeof(spinlock_t),
	sizeof(ExtStaInfoItem_t),
	.wlmalloc = kmalloc,
	.wlfree = kfree,
	.dev_kfree_skb_any = dev_kfree_skb_any,
	.print = printk,
	.netifstopqueue = netif_stop_queue,
	.netifwakequeue = netif_wake_queue,
	.spinlockinit = ap_spin_lock_init,
	.spinlockirqsave = ap_spin_lock_irqsave,
	.spinunlockirqrestore = ap_spin_unlock_irqrestore,
	.wlmemcmp = memcmp,
	.wlmemcpy = memcpy,
	.wlmemset = memset,
	.skbqueueheadinit = skb_queue_head_init,
	.skbqueuepurge = skb_queue_purge,
	.skbdequeue = skb_dequeue,
	.skbqueuetail = skb_queue_tail,
	.skbpeektail = skb_peek_tail,
	.skbqueuelen = skb_queue_len,
	.skbtailroom = skb_tailroom,
	.skbheadroom = skb_headroom,
	.skbtrim = skb_trim,
	.skbisnonlinear = skb_is_nonlinear,
	.netifrcv = netif_rx_ni,
	.unregnetdev = unregister_netdev,
	.freenetdev = free_netdev,
	.devgetbyname = dev_get_by_name,
	.ethtypetrans = eth_type_trans,
	.wlreadl = ap_readl,
	.wlwritel = ap_writel,
	.random = ap_netrandom,
	.wlmdelay = ap_mdelay
};
static struct ap_skb_ops ap8x_skb_ops = {
	.name = "skb ops",
	.wlGetNewBufAndInit = wlGetNewBufAndInit,
	.wlBufCopy = wlBufCopy,
	.wlBufCopyExpand = wlBufCopyExpand,
	.wlBufReserve = wlBufReserve,
	.wlBufUnshare = wlBufUnshare,
	.wlBuffReallocHeadroom = wlBuffReallocHeadroom,
	.wlBuffAlloc = wlBuffAlloc,
	.wlBufPull = wlBufPull,
	.wlBufPut = wlBufPut,
	.wlBufPush = wlBufPush
};
static struct ap_driver_ops ap8x_driver_ops = {
	.name = "ap8x",
	.desc = "lib_ap.a interface api",

	.wlLinkMgt = wlLinkMgt,
	
	.wlResetTask = wlResetTask,
	
	.wlTxDone = wlTxDone,
	
	.wlMgmtTx = wlMgmtTx,

	.wlDataTx = wlDataTx,

	.wlDataTxUnencr = wlDataTxUnencr,

	.wlxmit = wlxmit,

	.wlhandlepsxmit = wlhandlepsxmit,
	
	.wlprocesspsq = wlprocesspsq,
	
	.wlsyslog = wlsyslog,
	
	.wlSendEvent = wlSendEvent,
	
	.wlxmitmfs = wlxmitmfs,
	
	.wlshowinfo = wlshowinfo
};

static struct pci_driver wldriver = {
	.name     = DRV_NAME,
	.id_table = wlid_tbl,
	.probe    = wlprobe,
	.remove   = wlremove,
	.suspend  = wlsuspend,
	.resume   = wlresume,
};

static int wlopen(WL_NETDEV *);
static int wlstop(WL_NETDEV *);
static void wlsetMcList(WL_NETDEV *);
static struct net_device_stats *wlgetStats(WL_NETDEV *);
static int wlsetMacAddr(WL_NETDEV *, void *);
static int wlchangeMtu(WL_NETDEV *, int);
int wlreset(WL_NETDEV *);
int wlreset_mbss(WL_NETDEV *netdev);
int wlreset_client(WL_NETDEV *netdev);

static void wltxTimeout(struct net_device *);
module_init(wlmodule_init);
module_exit(wlmodule_exit);
/* WDS_SUPPORT */
static int wlopen_wds(struct net_device *);
static int wlstop_wds(struct net_device *);
static int	wlStop_wdsDevs(struct net_device *);
static int wlsetMacAddr_wds(struct net_device *, void *);
static int wlchangeMtu_wds(struct net_device *, int);
static void wltxTimeout_wds(struct net_device *);
/* WDS_SUPPORT */
static int __init wlmodule_init(void)
{

	struct pci_dev *dev=NULL, *dev1=NULL;
	unsigned long device_id;
	//int i=0;
	int j= 0, k=0;
#if LINUX_VERSION_CODE >=KERNEL_VERSION(2,6,27)
	while((dev = pci_get_device(0x11ab, PCI_ANY_ID, dev)) != NULL)
#else
	//pci_get_device will decrease pci_dev kobj reference counter, caused 
	//insmod/rmmod failure after 6 iterations, so it is better to have compiler warnnings.
	while((dev = pci_find_device(0x11ab, PCI_ANY_ID, dev)) != NULL)
#endif
	{
		if((dev->vendor != 0x11ab)||((dev->class >>16)& 0xff)!=PCI_BASE_CLASS_NETWORK)
		{
			continue;
		}

		dev1 = dev;
		while(dev1)
		{		
			device_id = dev->device;
			if(pci_dev_driver(dev))
			{
				j++;
				WLDBG_INFO(DBG_LEVEL_2,"device already inited\n");
				break;		
			}
			wlid_tbl[k].device = device_id;
			k++;
#if LINUX_VERSION_CODE >=KERNEL_VERSION(2,6,27)
			dev1 = pci_get_device(0x11ab, device_id, dev);
#else
			dev1 = pci_find_device(0x11ab, device_id, dev);
#endif
			if(dev1 == dev)
			{
				WLDBG_INFO(DBG_LEVEL_2,"same device id, same dev found\n");
				break;
			}
			else
			{
				if(dev1)
					WLDBG_INFO(DBG_LEVEL_2,"same device id, different dev found\n");
				else
					WLDBG_INFO(DBG_LEVEL_2,"no more device for id(%x)\n",device_id );
				break;
			}
		}
	}
	for (j = 0; j<k; j++)
		WLDBG_INFO(DBG_LEVEL_2,"found[%d] %x\n",j,  wlid_tbl[j].device);
	memcpy((void *)wldriver.name, DRV_NAME, sizeof(DRV_NAME));
	memset(&wlid_tbl[k], 0, sizeof(struct pci_device_id));		
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
	if (k== 0)
		//do not register if no card found
		return -ENODEV;
	return pci_register_driver(&wldriver);
#else
	return pci_module_init(&wldriver);
#endif
}

static void __exit wlmodule_exit(void)
{
	pci_unregister_driver(&wldriver);
	WLDBG_INFO(DBG_LEVEL_2,  "Unloaded %s driver\n", DRV_NAME);
}
void wlwlan_setup(struct net_device *dev)
{
	//dummy
}

struct wlprivate_data global_private_data[MAX_CARDS_SUPPORT];
static UINT8 cardindex=0;
#if LINUX_VERSION_CODE >=KERNEL_VERSION(3,8,0)
static int wlprobe(struct pci_dev *pdev, const struct pci_device_id *id)
#else
static int __devinit wlprobe(struct pci_dev *pdev, const struct pci_device_id *id)
#endif
{
	struct wlprivate *wlpptr = NULL;
	struct wlprivate_data*wlpdptr = NULL;
	unsigned long physAddr = 0;
	unsigned long resourceFlags;
	void *physAddr1[2];
	void *physAddr2[2];
#if LINUX_VERSION_CODE >=KERNEL_VERSION(2,6,25)
	struct net_device *dev;
#endif
	WLDBG_ENTER(DBG_LEVEL_2);

	if (pci_enable_device(pdev))
	{
		return -EIO;
	}

	if (pci_set_dma_mask(pdev, 0xffffffff))
	{
		printk(KERN_ERR "%s: 32-bit PCI DMA not supported", DRV_NAME);
		goto err_pci_disable_device;
	}

	pci_set_master(pdev);
#if LINUX_VERSION_CODE >=KERNEL_VERSION(2,6,25)
	dev = alloc_netdev(sizeof(struct wlprivate), DRV_NAME, wlwlan_setup);
	if (dev)
	{
		wlpptr = NETDEV_PRIV(struct wlprivate, dev);
		NETDEV_PRIV_S(dev) = wlpptr;
	}
#else
	wlpptr = kmalloc(sizeof(struct wlprivate), GFP_KERNEL);
#endif
	if (wlpptr == NULL)
	{
		printk(KERN_ERR "%s: no mem for private driver context\n", DRV_NAME);
		goto err_pci_disable_device;
	}
	memset(wlpptr, 0, sizeof(struct wlprivate));
	wlpptr->netDevStats = kmalloc(sizeof(struct net_device_stats), GFP_KERNEL);
	if(wlpptr->netDevStats == NULL)
	{
		printk(KERN_ERR "%s: no mem for private driver context\n", DRV_NAME);
		WL_FREE(wlpptr);
		goto err_pci_disable_device;		
	}
	memset(wlpptr->netDevStats, 0x00, sizeof(struct net_device_stats));
#if LINUX_VERSION_CODE >=KERNEL_VERSION(2,6,25)
	wlpptr->netDev = dev;
#else
	wlpptr->netDev = &wlpptr->netDevPriv;
#endif
	//wlpdptr = kmalloc(sizeof(struct wlprivate_data), GFP_KERNEL);
	wlpdptr = &global_private_data[cardindex%MAX_CARDS_SUPPORT];
	cardindex++;
	if (wlpdptr == NULL)
	{
		printk(KERN_ERR "%s: no mem for private driver data context\n", DRV_NAME);
		goto err_kfree;
	}
	memset(wlpdptr, 0, sizeof(struct wlprivate_data));
	wlpptr->wlpd_p = (struct _wlprivate_data *)wlpdptr;
	wlpdptr->appriv.rootwlpptr = wlpptr;

	physAddr = pci_resource_start(pdev, 0);

	resourceFlags = pci_resource_flags(pdev, 0);	

	wlpptr->nextBarNum = 1;	/* 32-bit */

	if (resourceFlags & 0x04)
		wlpptr->nextBarNum = 2;	 /* 64-bit */

	if (!request_mem_region(physAddr, pci_resource_len(pdev, 0), DRV_NAME))
	{
		printk(KERN_ERR "%s: cannot reserve PCI memory region 0\n", DRV_NAME);
		goto err_kfree1;
	}

	physAddr1[0] = ioremap(physAddr,pci_resource_len(pdev,0));
	physAddr1[1] = 0;
	wlpptr->ioBase0 = physAddr1[0];	  

	printk("wlprobe  wlpptr->ioBase0 = %x \n", (unsigned int) wlpptr->ioBase0);
	if (!wlpptr->ioBase0)
	{
		printk(KERN_ERR "%s: cannot remap PCI memory region 0\n", DRV_NAME);
		goto err_release_mem_region_bar0;
	}

	physAddr = pci_resource_start(pdev, wlpptr->nextBarNum);
	if (!request_mem_region(physAddr, pci_resource_len(pdev, wlpptr->nextBarNum), DRV_NAME))
	{
		printk(KERN_ERR "%s: cannot reserve PCI memory region 1\n", DRV_NAME);
		goto err_iounmap_ioBase0;
	}

	physAddr2[0] = ioremap(physAddr,pci_resource_len(pdev,wlpptr->nextBarNum));
	physAddr2[1] = 0;
	wlpptr->ioBase1 = physAddr2[0];

	printk("wlprobe  wlpptr->ioBase1 = %x \n", (unsigned int) wlpptr->ioBase1);
	if (!wlpptr->ioBase1)
	{
		printk(KERN_ERR "%s: cannot remap PCI memory region 1\n", DRV_NAME);
		goto err_release_mem_region_bar1;
	}

	sprintf(wlpptr->netDev->name,  DRV_NAME, wlinitcnt);
	wlpptr->netDev->irq       = pdev->irq;
	wlpptr->netDev->mem_start = pci_resource_start(pdev, 0);
	wlpptr->netDev->mem_end   = physAddr + pci_resource_len(pdev, 1);
	NETDEV_PRIV_S(wlpptr->netDev)      = wlpptr;
	wlpptr->pPciDev          = pdev;
	SET_MODULE_OWNER(*(wlpptr->netDev));

	pci_set_drvdata(pdev, (wlpptr->netDev));

#if LINUX_VERSION_CODE >=KERNEL_VERSION(2,6,22)
	if (request_irq(wlpptr->netDev->irq, wlISR, IRQF_SHARED, 
		wlpptr->netDev->name, (wlpptr->netDev)))
#else
	if (request_irq(wlpptr->netDev->irq, wlISR, SA_SHIRQ, 
		wlpptr->netDev->name, (wlpptr->netDev)))
#endif
	{
		printk(KERN_ERR "%s: request_irq failed\n", wlpptr->netDev->name);
		goto err_iounmap_ioBase1;
	}
	((struct wlprivate_data *)(wlpptr->wlpd_p))->CardDeviceInfo = pdev->device & 0xff ;
	if (((struct wlprivate_data *)(wlpptr->wlpd_p))->CardDeviceInfo == 4)
		((struct wlprivate_data *)(wlpptr->wlpd_p))->SDRAMSIZE_Addr = 0x40fc70b7; /* 16M SDRAM */
	else
		((struct wlprivate_data *)(wlpptr->wlpd_p))->SDRAMSIZE_Addr = 0x40fe70b7; /* 8M SDRAM */
	WLDBG_INFO(DBG_LEVEL_2,"%s: %s: mem=0x%lx, irq=%d, ioBase0=%x, ioBase1=%x\n",
		wlpptr->netDev->name,wlgetAdapterDescription(id->vendor,id->device), 
		wlpptr->netDev->mem_start, wlpptr->netDev->irq,wlpptr->ioBase0, wlpptr->ioBase1);
	if (wlInit((wlpptr->netDev), id->device))
	{
		goto err_free_irq;
	}


	WLDBG_EXIT(DBG_LEVEL_2);
	return 0;

err_free_irq:
	free_irq(wlpptr->netDev->irq, (wlpptr->netDev));
err_iounmap_ioBase1:
	iounmap(wlpptr->ioBase1);
err_release_mem_region_bar1:
	release_mem_region(pci_resource_start(pdev, 1), pci_resource_len(pdev, 1));
err_iounmap_ioBase0:
	iounmap(wlpptr->ioBase0);
err_release_mem_region_bar0:
	release_mem_region(pci_resource_start(pdev, 0), pci_resource_len(pdev, 0));
err_kfree1:
	//WL_FREE(wlpdptr);
err_kfree:
	WL_FREE(wlpptr->netDevStats);
#if LINUX_VERSION_CODE >=KERNEL_VERSION(2,6,25)
	free_netdev(dev);
#else
	WL_FREE(wlpptr);
#endif
err_pci_disable_device:
	pci_disable_device(pdev);
	WLDBG_EXIT_INFO(DBG_LEVEL_2, "init error");
	return -EIO;
}

#if LINUX_VERSION_CODE >=KERNEL_VERSION(3,8,0)
static void wlremove(struct pci_dev *pdev)
#else
static void __devexit wlremove(struct pci_dev *pdev)
#endif
{
	struct net_device *netdev = pci_get_drvdata(pdev);
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);

	WLDBG_ENTER(DBG_LEVEL_2);

	if (wlDeinit(netdev))
	{
		printk(KERN_ERR "%s: deinit of device failed\n", netdev->name);
	}
	if (netdev->irq)
	{
		free_irq(netdev->irq, netdev);
	}
	iounmap(wlpptr->ioBase1);
	iounmap(wlpptr->ioBase0);
	release_mem_region(pci_resource_start(pdev,wlpptr->nextBarNum),pci_resource_len(pdev,wlpptr->nextBarNum));	 
	release_mem_region(pci_resource_start(pdev,0),pci_resource_len(pdev,0));
	pci_disable_device(pdev);
	free_netdev(netdev);
	WLDBG_EXIT(DBG_LEVEL_2);
}

static int wlsuspend(struct pci_dev *pdev, pm_message_t state)
{
	WLDBG_INFO(DBG_LEVEL_2, "%s: suspended device\n", DRV_NAME);
	return 0;
}

static int wlresume(struct pci_dev *pdev)
{
	WLDBG_INFO(DBG_LEVEL_2, "%s: resumed device\n", DRV_NAME);
	return 0;
}
#ifdef WL_DEBUG
static const char *wlgetAdapterDescription(u_int32_t vendorid, u_int32_t devid)
{
	int numEntry=((sizeof(wlid_tbl)/sizeof(struct pci_device_id))-1);

	while (numEntry)
	{
		numEntry--;
		if ((wlid_tbl[numEntry].vendor == vendorid) &&
			(wlid_tbl[numEntry].device == devid))
		{
			if ((const char *) wlid_tbl[numEntry].driver_data != NULL)
			{
				return (const char *) wlid_tbl[numEntry].driver_data;
			}
			break;
		}
	}
	return "Marvell ???";
}
#endif
static void timer_routine(unsigned long arg)
{
	UINT8 num = NUM_OF_DESCRIPTOR_DATA;
	struct net_device *netdev = (struct net_device *) arg;
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	((struct wlprivate_data *)(wlpptr->wlpd_p))->Timer.function = timer_routine;
	((struct wlprivate_data *)(wlpptr->wlpd_p))->Timer.data = (unsigned long) netdev;
	((struct wlprivate_data *)(wlpptr->wlpd_p))->Timer.expires=jiffies + HZ/100;

	/*We constantly check all txq to see if any txq is not empty. Once any txq is not empty, we schedule a task again
	* to enable all txq are flushed out when no new incoming pkt from host. Sometimes pkts can sit inside txq forever when txq depth
	* is too deep.
	*/
	if(!((struct wlprivate_data *)(wlpptr->wlpd_p))->isTxTaskScheduled)
	{
		while (num--)
		{	
			if(QUEUE_LEN(&((struct wlprivate_data *)(wlpptr->wlpd_p))->txQ[num])!=0)
			{			
				tasklet_schedule(&((struct wlprivate_data *)(wlpptr->wlpd_p))->txtask);
				((struct wlprivate_data *)(wlpptr->wlpd_p))->isTxTaskScheduled=1;
				break;
			}
		}
	}

	add_timer(&((struct wlprivate_data *)(wlpptr->wlpd_p))->Timer);

}

extern void wlRecv(struct net_device *netdev);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
static void _wlreset(struct work_struct *work)
{
	struct wlprivate_data *wlpd_p = container_of(work, struct wlprivate_data , resettask);
	struct wlprivate *wlpptr = wlpd_p->masterwlp ;
	wlreset(wlpptr->netDev);
}
/* DFS_SUPPORT */
static void _wlRadarDetection(struct work_struct *work)
{
	struct wlprivate_data *wlpd_p = container_of(work, struct wlprivate_data , dfstask);
	struct wlprivate *wlpptr = wlpd_p->masterwlp ;
	wlRadarDetection(wlpptr->netDev);
}
static void _wlApplyCSAChannel(struct work_struct *work)
{
	struct wlprivate_data *wlpd_p = container_of(work, struct wlprivate_data , csatask);
	struct wlprivate *wlpptr = wlpd_p->masterwlp ;
	wlApplyCSAChannel(wlpptr->netDev);
}
/* DFS_SUPPORT */

static void _wlConsecTxFail(struct work_struct *work)
{
	struct wlprivate_data *wlpd_p = container_of(work, struct wlprivate_data , kickstatask);
	struct wlprivate *wlpptr = wlpd_p->masterwlp ;
	wlConsecTxFail(wlpptr->netDev);
}
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,28)
static const struct net_device_ops wl_netdev_ops = {
	.ndo_open 		= wlopen,
	.ndo_stop		= wlstop,
	.ndo_start_xmit		= wlDataTx,
	.ndo_do_ioctl       = wlIoctl,
	.ndo_set_mac_address	= wlsetMacAddr,
	.ndo_tx_timeout 	= wltxTimeout,
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,2,0)
	.ndo_set_rx_mode    = wlsetMcList,
#else
	.ndo_set_multicast_list = wlsetMcList,
#endif
	.ndo_change_mtu		= wlchangeMtu,
	.ndo_get_stats     = wlgetStats,
};

static const struct ethtool_ops wl_ethtool_ops = {
	.get_settings = NULL,
};
#endif

/** public functions **/

#define MWL_SPIN_LOCK(X) spin_lock_irqsave(*X, flags)
#define MWL_SPIN_UNLOCK(X)	spin_unlock_irqrestore(*X, flags)

static int wlFwGetHwSpecs(WL_NETDEV *netdev)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	HostCmd_DS_GET_HW_SPEC *pCmd = (HostCmd_DS_GET_HW_SPEC *)&wlpptr->pCmdBuf[0];
	unsigned long flags;
	int i;
	WLDBG_ENTER(DBG_LEVEL_0);

	MWL_SPIN_LOCK(&wlpptr->wlpd_p->locks.fwLock);
	printk("wlFwGetHwSpecs pCmd = %x \n", (unsigned int) pCmd);
	memset(pCmd, 0x00, sizeof(HostCmd_DS_GET_HW_SPEC));
	memset(&pCmd->PermanentAddr[0], 0xff, ETH_ALEN);
	pCmd->CmdHdr.Cmd      = ENDIAN_SWAP16(HostCmd_CMD_GET_HW_SPEC);
	pCmd->CmdHdr.Length   = ENDIAN_SWAP16(sizeof(HostCmd_DS_GET_HW_SPEC));
	pCmd->ulFwAwakeCookie = ENDIAN_SWAP32(wlpptr->pPhysCmdBuf+2048);

	WLDBG_DUMP_DATA(DBG_LEVEL_0, (void *) pCmd, sizeof(HostCmd_DS_GET_HW_SPEC));
	while (wlexecuteCommand(wlpptr, HostCmd_CMD_GET_HW_SPEC))
	{
		printk( "failed execution");
		mdelay(1000);
		printk(" Repeat wlFwGetHwSpecs = %x \n", (unsigned int) pCmd);
		//MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
		//return FAIL;
	}
	memcpy(&wlpptr->hwData.macAddr[0],pCmd->PermanentAddr,ETH_ALEN);
	((struct wlprivate_data *)(wlpptr->wlpd_p))->descData[0].wcbBase       = ENDIAN_SWAP32(pCmd->WcbBase0)   & 0x0000ffff;
#if NUM_OF_DESCRIPTOR_DATA >3
	for (i = 1; i < TOTAL_TX_QUEUES; i++)
		((struct wlprivate_data *)(wlpptr->wlpd_p))->descData[i].wcbBase       = ENDIAN_SWAP32(pCmd->WcbBase[i-1])   & 0x0000ffff;
#endif
	((struct wlprivate_data *)(wlpptr->wlpd_p))->descData[0].rxDescRead    = ENDIAN_SWAP32(pCmd->RxPdRdPtr)  & 0x0000ffff;
	((struct wlprivate_data *)(wlpptr->wlpd_p))->descData[0].rxDescWrite   = ENDIAN_SWAP32(pCmd->RxPdWrPtr)  & 0x0000ffff;
	wlpptr->hwData.regionCode      = ENDIAN_SWAP16(pCmd->RegionCode) & 0x00ff;
	//	domainSetDomain(wlpptr->wlpd_p->hwData.regionCode);
	wlpptr->hwData.fwReleaseNumber = ENDIAN_SWAP32(pCmd->FWReleaseNumber);
	wlpptr->hwData.maxNumTXdesc    = ENDIAN_SWAP16(pCmd->NumOfWCB);
	wlpptr->hwData.maxNumMCaddr    = ENDIAN_SWAP16(pCmd->NumOfMCastAddr);
	wlpptr->hwData.numAntennas     = ENDIAN_SWAP16(pCmd->NumberOfAntenna);
	wlpptr->hwData.hwVersion       = pCmd->Version;
	wlpptr->hwData.hostInterface   = pCmd->HostIf;

	WLDBG_EXIT_INFO(DBG_LEVEL_0, 
		"region code is %i (0x%x), HW version is %i (0x%x)",
		wlpptr->hwData.regionCode, wlpptr->hwData.regionCode,
		wlpptr->hwData.hwVersion, wlpptr->hwData.hwVersion);

	MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
	return SUCCESS;
}

static int wlFwSetHwSpecs(WL_NETDEV *netdev)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	HostCmd_DS_SET_HW_SPEC *pCmd = (HostCmd_DS_SET_HW_SPEC *)&wlpptr->pCmdBuf[0];
	unsigned long flags;
	int i;
	WLDBG_ENTER(DBG_LEVEL_1);

    /* Info for SOC team's debugging */
    printk("wlFwSetHwSpecs ...\n");
    printk("  -->pPhysTxRing[0] = %x\n",((struct wlprivate_data *)(wlpptr->wlpd_p))->descData[0].pPhysTxRing);
    printk("  -->pPhysTxRing[1] = %x\n",((struct wlprivate_data *)(wlpptr->wlpd_p))->descData[1].pPhysTxRing);
    printk("  -->pPhysTxRing[2] = %x\n",((struct wlprivate_data *)(wlpptr->wlpd_p))->descData[2].pPhysTxRing);
    printk("  -->pPhysTxRing[3] = %x\n",((struct wlprivate_data *)(wlpptr->wlpd_p))->descData[3].pPhysTxRing);
    printk("  -->pPhysRxRing    = %x\n",((struct wlprivate_data *)(wlpptr->wlpd_p))->descData[0].pPhysRxRing);
    printk("  -->numtxq %d wcbperq %d totalrxwcb %d \n",NUM_OF_DESCRIPTOR_DATA,MAX_NUM_TX_DESC,MAX_NUM_RX_DESC);

	MWL_SPIN_LOCK(&wlpptr->wlpd_p->locks.fwLock);
	memset(pCmd, 0x00, sizeof(HostCmd_DS_SET_HW_SPEC));
	pCmd->CmdHdr.Cmd	  = ENDIAN_SWAP16(HostCmd_CMD_SET_HW_SPEC);
	pCmd->CmdHdr.Length   = ENDIAN_SWAP16(sizeof(HostCmd_DS_SET_HW_SPEC));
	pCmd->WcbBase[0]	 = ENDIAN_SWAP32(((struct wlprivate_data *)(wlpptr->wlpd_p))->descData[0].pPhysTxRing);//ENDIAN_SWAP32(wlpptr->descData[0].wcbBase)	& 0x0000ffff;
#if NUM_OF_DESCRIPTOR_DATA >3
	for (i = 1; i < TOTAL_TX_QUEUES; i++)
		pCmd->WcbBase[i]	 = ENDIAN_SWAP32(((struct wlprivate_data *)(wlpptr->wlpd_p))->descData[i].pPhysTxRing);//ENDIAN_SWAP32(	wlpptr->descData[1].wcbBase )	& 0x0000ffff;
#endif
	pCmd->TxWcbNumPerQueue = ENDIAN_SWAP32(MAX_NUM_TX_DESC);
	pCmd->NumTxQueues = ENDIAN_SWAP32(NUM_OF_DESCRIPTOR_DATA);
	pCmd->TotalRxWcb = ENDIAN_SWAP32(MAX_NUM_RX_DESC);
	pCmd->RxPdWrPtr = ENDIAN_SWAP32(((struct wlprivate_data *)(wlpptr->wlpd_p))->descData[0].pPhysRxRing);
	pCmd->disablembss = 0;

#if NUMOFAPS == 1
	pCmd->disablembss = 1;
#endif

	if (wlexecuteCommand(wlpptr, HostCmd_CMD_SET_HW_SPEC))
	{
		WLDBG_EXIT_INFO(DBG_LEVEL_1, "failed execution");
		MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
		return FAIL;
	}

	MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
	return SUCCESS;
}

int wlinitcnt=0;
int	wlInit(struct net_device *netdev, u_int16_t devid)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	int retCode,index,i;
	int bssidmask =0;
	unsigned char macaddr[6]= {0x00, 0xde, 0xad,0xde, 0xad, 0xee};

	WLDBG_ENTER(DBG_LEVEL_2);

	((struct wlprivate_data *)(wlpptr->wlpd_p))->masterwlp = wlpptr;
	((struct wlprivate_data *)(wlpptr->wlpd_p))->isTxTaskScheduled=0;
	tasklet_init(&((struct wlprivate_data *)(wlpptr->wlpd_p))->rxtask, (void *)wlRecv, (unsigned long)netdev);
	tasklet_init(&((struct wlprivate_data *)(wlpptr->wlpd_p))->txtask, (void *)wlDataTxHdl, (unsigned long)netdev);
/* DFS_SUPPORT */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
	INIT_WORK(&((struct wlprivate_data *)(wlpptr->wlpd_p))->dfstask, (void *)(void *)_wlRadarDetection);
	INIT_WORK(&((struct wlprivate_data *)(wlpptr->wlpd_p))->csatask, (void *)(void *)_wlApplyCSAChannel);    
#else
	INIT_WORK(&((struct wlprivate_data *)(wlpptr->wlpd_p))->dfstask, (void (*)(void *))wlRadarDetection, netdev);
	INIT_WORK(&((struct wlprivate_data *)(wlpptr->wlpd_p))->csatask, (void (*)(void *))wlApplyCSAChannel, netdev);
#endif /* DFS_SUPPORT */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
	INIT_WORK(&((struct wlprivate_data *)(wlpptr->wlpd_p))->resettask, (void *)(void *)_wlreset);		
	INIT_WORK(&((struct wlprivate_data *)(wlpptr->wlpd_p))->kickstatask, (void *)(void *)_wlConsecTxFail);		
#else
	INIT_WORK(&((struct wlprivate_data *)(wlpptr->wlpd_p))->resettask, (void (*)(void *))wlreset, netdev);
	INIT_WORK(&((struct wlprivate_data *)(wlpptr->wlpd_p))->kickstatask, (void (*)(void *))wlConsecTxFail, netdev);	
#endif

	SPIN_LOCK_INIT(&wlpptr->wlpd_p->locks.xmitLock);
	SPIN_LOCK_INIT(&wlpptr->wlpd_p->locks.fwLock);

	wlpptr->pCmdBuf = (unsigned short *) 
		pci_alloc_consistent(wlpptr->pPciDev, CMD_BUF_SIZE, &wlpptr->pPhysCmdBuf);
		printk("wlInit wlpptr->pCmdBuf = %x  wlpptr->pPhysCmdBuf = %x \n", 
		(unsigned int) wlpptr->pCmdBuf, (unsigned int) wlpptr->pPhysCmdBuf);
	if (wlpptr->pCmdBuf == NULL)
	{
		printk(KERN_ERR  "%s: can not alloc mem\n", netdev->name);
		goto err_init_cmd_buf;
	}

	memset(wlpptr->pCmdBuf, 0x00, CMD_BUF_SIZE);

	ether_setup(netdev); /* init eth data structures */

	if ((retCode = wlTxRingAlloc(netdev)) == 0)
	{
		if ((retCode = wlTxRingInit(netdev)) != 0)
		{
			printk(KERN_ERR  "%s: initializing TX ring failed\n", netdev->name);
			goto err_init_tx2;
		}
	} else
	{
		printk(KERN_ERR  "%s: allocating TX ring failed\n", netdev->name);
		goto err_init_tx1;
	}

	if ((retCode = wlRxRingAlloc(netdev)) == 0)
	{
		if ((retCode = wlRxRingInit(netdev)) != 0)
		{
			printk(KERN_ERR  "%s: initializing RX ring failed\n", netdev->name);
			goto err_init_rx;
		}
	} else
	{
		printk(KERN_ERR  "%s: allocating RX ring failed\n", netdev->name);
		goto err_init_rx;
	}

	if (wlPrepareFwFile(wlpptr))
	{
		printk(KERN_ERR  "%s: prepare firmware downloading failed\n", netdev->name);
		goto err_init_rx;
	}

	if (wlFwDownload(wlpptr))
	{
		printk(KERN_ERR  "%s: firmware downloading failed\n", netdev->name);
		goto err_init_rx;
	}

	if (wlFwGetHwSpecs(netdev))
	{
		printk(KERN_ERR  "%s: failed to get HW specs\n", netdev->name);
		goto err_init_rx;
	}
	
	memcpy(netdev->dev_addr, &wlpptr->hwData.macAddr[0], 6);
	printk("Mac address = %s \n", mac_display(&wlpptr->hwData.macAddr[0]));
	wlpptr->vmacSta_p = wlvmacInit(NULL, wlpptr, &wlpptr->hwData.macAddr[0], WL_OP_MODE_AP, wlinitcnt);
	if(wlpptr->vmacSta_p == NULL)
	{
		printk(KERN_ERR  "%s: failed to init driver mac\n", netdev->name);
		goto err_init_rx;
	}
	wlpptr->vmacSta_p->driver = &ap8x_driver_ops;
	wlpptr->vmacSta_p->skbops = &ap8x_skb_ops;
	wlpptr->vmacSta_p->mtu = netdev->mtu;
	wlpptr->rootpriv = wlpptr;
	writel((((struct wlprivate_data *)(wlpptr->wlpd_p))->descData[0].pPhysTxRing),
		wlpptr->ioBase0 + ((struct wlprivate_data *)(wlpptr->wlpd_p))->descData[0].wcbBase);
#if NUM_OF_DESCRIPTOR_DATA >3
	for (i = 1; i < TOTAL_TX_QUEUES; i++)
	writel((((struct wlprivate_data *)(wlpptr->wlpd_p))->descData[i].pPhysTxRing),
		wlpptr->ioBase0 + ((struct wlprivate_data *)(wlpptr->wlpd_p))->descData[i].wcbBase);
#endif
	writel((((struct wlprivate_data *)(wlpptr->wlpd_p))->descData[0].pPhysRxRing),
		wlpptr->ioBase0 + ((struct wlprivate_data *)(wlpptr->wlpd_p))->descData[0].rxDescRead);
	writel((((struct wlprivate_data *)(wlpptr->wlpd_p))->descData[0].pPhysRxRing),
		wlpptr->ioBase0 + ((struct wlprivate_data *)(wlpptr->wlpd_p))->descData[0].rxDescWrite);
	if (wlFwSetHwSpecs(netdev))
	{
		WLDBG_ERROR(DBG_LEVEL_2, "failed to set HW specs");
	}
#ifndef TIMER_TASK
	init_timer(&((struct wlprivate_data *)(wlpptr->wlpd_p))->Timer);
	((struct wlprivate_data *)(wlpptr->wlpd_p))->Timer.function = timer_routine;
	((struct wlprivate_data *)(wlpptr->wlpd_p))->Timer.data = (unsigned long) netdev;
	((struct wlprivate_data *)(wlpptr->wlpd_p))->Timer.expires=jiffies + HZ/10;
	add_timer(&((struct wlprivate_data *)(wlpptr->wlpd_p))->Timer);
#endif
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,28)
	netdev->open            = wlopen;
	netdev->stop            = wlstop;
	netdev->hard_start_xmit = wlDataTx;
	netdev->tx_timeout        = wltxTimeout;
	netdev->set_multicast_list = wlsetMcList;
	netdev->do_ioctl           = wlIoctl;
	netdev->get_stats          = wlgetStats;
	netdev->set_mac_address    = wlsetMacAddr;
	netdev->change_mtu         = wlchangeMtu;
#else
	netdev->netdev_ops         = &wl_netdev_ops;
	netdev->ethtool_ops         = &wl_ethtool_ops;
#endif
	netdev->watchdog_timeo     = 30 * HZ;
	wlpptr->wlreset = wlreset;
	wlSetupWEHdlr(netdev);
	sprintf(netdev->name,  DRV_NAME, wlinitcnt);
	if (register_netdev(netdev))
	{
		printk(KERN_ERR "%s: failed to register device\n", DRV_NAME);
		goto err_register_netdev;
	}
	ap8x_stat_proc_register(netdev);
	ap8x_dump_proc_register(netdev);

	memcpy(macaddr, wlpptr->hwData.macAddr, 6);
	for (index = 0; index< NUMOFAPS; index++)
	{
		int i;
		if(wlInit_mbss(wlpptr, &macaddr[0]))
		{
			printk(KERN_ERR "%s: failed to setup mbss No. %d\n", netdev->name, index);
			break;
		}
		//uses mac addr bit 41 & up as mbss addresses
		for (i = 1; i<32; i++)
		{
			if((bssidmask & (1<<i))==0)
			{
				break;
			}
		}
		if(i)
		{
			macaddr[0]=wlpptr->hwData.macAddr[0] |((i<<2)|0x2);
		}
		bssidmask |=1<<i;
	}
	{
		
		/*For client interface, we use different mac addr from master mac addr*/
		/*If client interface also takes master mac addr like ap0, then there will be conflict if ap0 is up too*/
		/*This procedure to generate client mac addr is also same in macclone api*/
		bssidmask = 0;
		memcpy(macaddr, wlpptr->hwData.macAddr, 6);
		for (index = 0; index< NUMOFAPS; index++)
		{
			int i;
			//uses mac addr bit 41 & up as mbss addresses
			for (i = 1; i<32; i++)
			{
				if((bssidmask & (1<<i))==0)
					break;	
			}
			if(i)
			{
				macaddr[0]=wlpptr->hwData.macAddr[0] |((i<<2)|0x2);
			}
			bssidmask |=1<<i;
		}
			
		if(wlInit_client(wlpptr, &macaddr[0], &macaddr[0]))	
		{
			printk("*********** Fail to Init Client \n");
		}
	}
	wlinitcnt++;
	WLDBG_EXIT(DBG_LEVEL_2);
	return SUCCESS;

err_register_netdev:
err_init_rx:	
	wlRxRingCleanup(netdev);								    
	wlRxRingFree(netdev);
err_init_tx2:	
	wlTxRingCleanup(netdev);
err_init_tx1:	
	wlTxRingFree(netdev);
err_init_cmd_buf:
	tasklet_kill(&((struct wlprivate_data *)(wlpptr->wlpd_p))->rxtask);
	tasklet_kill(&((struct wlprivate_data *)(wlpptr->wlpd_p))->txtask);
	flush_scheduled_work();
	pci_free_consistent(wlpptr->pPciDev, CMD_BUF_SIZE,
		wlpptr->pCmdBuf, wlpptr->pPhysCmdBuf);
	WLDBG_EXIT_INFO(DBG_LEVEL_2, NULL);
	return FAIL;
}

int wlDeinit(struct net_device *netdev)
{
	void wlDeinit_mbss(struct net_device *netdev);
	extern void wlDeinit_client(struct net_device *netdev);
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);

	WLDBG_ENTER(DBG_LEVEL_2);
	ap8x_stat_proc_unregister(netdev);
	ap8x_dump_proc_unregister(netdev);

	del_timer(&((struct wlprivate_data *)(wlpptr->wlpd_p))->Timer);
	SPIN_LOCK_DEINIT(&wlpptr->wlpd_p->locks.xmitLock);
	SPIN_LOCK_DEINIT(&wlpptr->wlpd_p->locks.fwLock);
	
	DfsDeInit(wlpptr->wlpd_p); /* DFS_SUPPORT */

	SendResetCmd(wlpptr, 1);
	if (netdev->flags & IFF_RUNNING)
	{
		if (wlstop(netdev))
		{
			printk(KERN_ERR "%s: failed to stop device\n", DRV_NAME);
		}
	}
	wlDeinit_mbss(netdev);
	wlDeinit_client(netdev);
	tasklet_kill(&((struct wlprivate_data *)(wlpptr->wlpd_p))->rxtask);
	tasklet_kill(&((struct wlprivate_data *)(wlpptr->wlpd_p))->txtask);
	flush_scheduled_work();
	wlInterruptDisable(netdev);
	wlFwReset(wlpptr);
	wlRxRingCleanup(netdev);
	wlRxRingFree(netdev);
	wlTxRingCleanup(netdev);
	wlTxRingFree(netdev);
	DisableMacMgmtTimers(wlpptr);
	MacMgmtMemCleanup(wlpptr);
	wlDestroySysCfg(wlpptr);
	unregister_netdev(netdev);
	if(wlinitcnt == 1)//last one
		ap8x_remove_folder();
	wlinitcnt--;
	pci_free_consistent(wlpptr->pPciDev, CMD_BUF_SIZE,
		(caddr_t) wlpptr->pCmdBuf, wlpptr->pPhysCmdBuf);
	//	WL_FREE(wlpptr->wlpd_p);
	WL_FREE(wlpptr->netDevStats);
	TimerDestroyAll();
	WLDBG_EXIT(DBG_LEVEL_2);
	return SUCCESS;
}

static void wlRecvHdlr(struct net_device *netdev)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	tasklet_schedule(&((struct wlprivate_data *)(wlpptr->wlpd_p))->rxtask);
	return;
}

/* DFS_SUPPORT */
int wlRadarDetection(struct net_device *netdev)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	smeQ_MgmtMsg_t *toSmeMsg = NULL;
	vmacApInfo_t *syscfg = (vmacApInfo_t *)wlpptr->vmacSta_p;
	MIB_802DOT11 *mib = syscfg->Mib802dot11;
	MIB_PHY_DSSS_TABLE *PhyDSSSTable=mib->PhyDSSSTable;

	WLSYSLOG(netdev, WLSYSLOG_CLASS_ALL, WLSYSLOG_MSG_GEN_RADARDETECTION);
	WLSNDEVT(netdev, IWEVCUSTOM, (IEEEtypes_MacAddr_t *)&wlpptr->hwData.macAddr[0], 
		WLSYSLOG_MSG_GEN_RADARDETECTION);
	/* Send Radar detection indication to SME layer */
	if ((toSmeMsg=(smeQ_MgmtMsg_t *)kmalloc(sizeof(smeQ_MgmtMsg_t), GFP_ATOMIC)) == NULL)
	{
		WLDBG_INFO(DBG_LEVEL_2, "wlChannelSet: failed to alloc msg buffer\n");
		return 1;
	}

	memset(toSmeMsg, 0, sizeof(smeQ_MgmtMsg_t));

	toSmeMsg->wlpptr = wlpptr;

	toSmeMsg->MsgType = SME_NOTIFY_RADAR_DETECTION_IND;

	toSmeMsg->Msg.RadarDetectionInd.chInfo.channel = PhyDSSSTable->CurrChan ;
	memcpy(&toSmeMsg->Msg.RadarDetectionInd.chInfo.chanflag ,
		&PhyDSSSTable->Chanflag, sizeof(CHNL_FLAGS));

	smeQ_MgmtWriteNoBlock(toSmeMsg);
	WL_FREE((UINT8 *)toSmeMsg);

	return 0 ;
}

static void radarDetectionHdlr(struct net_device *netdev)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	schedule_work(&((struct wlprivate_data *)(wlpptr->wlpd_p))->dfstask);
	return;
}

int wlApplyCSAChannel(struct net_device *netdev)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	vmacApInfo_t *syscfg = (vmacApInfo_t *)wlpptr->vmacSta_p;
	MIB_802DOT11 *mib = syscfg->Mib802dot11;
	MIB_PHY_DSSS_TABLE *PhyDSSSTable=mib->PhyDSSSTable;

	ApplyCSAChannel(wlpptr, PhyDSSSTable->CurrChan );

	return 0 ;
}

static void dfsChanSwitchHdlr(struct net_device *netdev)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	schedule_work(&((struct wlprivate_data *)(wlpptr->wlpd_p))->csatask);
	return;
}
/* DFS_SUPPORT */


/*Function to kick out client when consecutive tx failure count > limit*/
int wlConsecTxFail(struct net_device *netdev)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	vmacApInfo_t *syscfg = (vmacApInfo_t *)wlpptr->vmacSta_p;

	IEEEtypes_MacAddr_t addr = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};	

	if((syscfg->OpMode == WL_OP_MODE_VSTA) || (syscfg->OpMode == WL_OP_MODE_STA)){
		ClientModeTxMonitor = 0;
	}
	else{
		wlFwGetConsecTxFailAddr(wlpptr, (IEEEtypes_MacAddr_t *)addr);
		extStaDb_RemoveStaNSendDeauthMsg(wlpptr,(IEEEtypes_MacAddr_t *)addr);
	}

	return 0 ;
}


/*Event handler to kick out client when consecutive tx failure count > limit*/
static void ConsecTxFailHdlr(struct net_device *netdev)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	schedule_work(&((struct wlprivate_data *)(wlpptr->wlpd_p))->kickstatask);
	return;
}


#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
irqreturn_t  wlISR(int irq, void *dev_id)
#else
irqreturn_t wlISR(int irq, void *dev_id, struct pt_regs *regs)
#endif
{
	struct net_device *netdev = (struct net_device *) dev_id;
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	vmacApInfo_t *vmacSta_p = wlpptr->vmacSta_p;
	unsigned int currIteration = 0;
	unsigned int intStatus;
	irqreturn_t retVal = IRQ_NONE;
	do
	{
		intStatus = (readl(wlpptr->ioBase1 + MACREG_REG_A2H_INTERRUPT_CAUSE));
		if (intStatus != 0x00000000)
		{
			if (intStatus == 0xffffffff)
			{
				WLDBG_INFO(DBG_LEVEL_2, "card plugged out???");
				retVal = IRQ_HANDLED;
				break; /* card plugged out -> do not handle any IRQ */
			}
			writel((MACREG_A2HRIC_BIT_MASK & ~intStatus),
				wlpptr->ioBase1 + MACREG_REG_A2H_INTERRUPT_CAUSE);
		}
		if ((intStatus & ISR_SRC_BITS) || (currIteration < MAX_ISR_ITERATION))
		{
			if (intStatus & MACREG_A2HRIC_BIT_RX_RDY)
			{
				intStatus &= ~MACREG_A2HRIC_BIT_RX_RDY;
				wlRecvHdlr(netdev);
				retVal = IRQ_HANDLED;
			}
			if (intStatus & MACREG_A2HRIC_BIT_OPC_DONE)
			{
				intStatus &= ~MACREG_A2HRIC_BIT_OPC_DONE;
				wlFwCmdComplete(wlpptr);
				retVal = IRQ_HANDLED;
			}
			if (intStatus & MACREG_A2HRIC_BIT_MAC_EVENT)
			{
				intStatus &= ~MACREG_A2HRIC_BIT_MAC_EVENT;
				retVal = IRQ_HANDLED;
			}
			if (intStatus & MACREG_A2HRIC_BIT_ICV_ERROR)
			{
				WLDBG_INFO(DBG_LEVEL_2, "MACREG_A2HRIC_BIT_ICV_ERROR *************. \n");
				MrvlICVErrorHdl(wlpptr);
				intStatus &= ~MACREG_A2HRIC_BIT_ICV_ERROR;
				retVal = IRQ_HANDLED;
			}
			if (intStatus & MACREG_A2HRIC_BIT_WEAKIV_ERROR)
			{
				MIB_802DOT11 *mib = wlpptr->vmacSta_p->ShadowMib802dot11;
				intStatus &= ~MACREG_A2HRIC_BIT_WEAKIV_ERROR;

				((struct wlprivate_data *)(wlpptr->wlpd_p))->privStats.weakiv_count++;
				((struct wlprivate_data *)(wlpptr->wlpd_p))->privStats.weakiv_threshold_count++;

				if ((((struct wlprivate_data *)(wlpptr->wlpd_p))->privStats.weakiv_threshold_count) >= *(mib->mib_weakiv_threshold)) {
					((struct wlprivate_data *)(wlpptr->wlpd_p))->privStats.weakiv_threshold_count = 0;
					WLSYSLOG(netdev, WLSYSLOG_CLASS_ALL, WLSYSLOG_MSG_WEP_WEAKIV_ERROR);
					WLSNDEVT(netdev,IWEVCUSTOM, (IEEEtypes_MacAddr_t *)&wlpptr->hwData.macAddr[0], WLSYSLOG_MSG_WEP_WEAKIV_ERROR);
				}
				retVal = IRQ_HANDLED;
			}
			if (intStatus & MACREG_A2HRIC_BIT_QUEUE_EMPTY)
			{
				intStatus &= ~MACREG_A2HRIC_BIT_QUEUE_EMPTY;
				if(extStaDb_AggrFrameCk(wlpptr, 1))
				{
					//interrupt when there are amsdu frames to fw.
					writel(MACREG_H2ARIC_BIT_PPA_READY,
						wlpptr->ioBase1 + MACREG_REG_H2A_INTERRUPT_EVENTS);
				}
				retVal = IRQ_HANDLED;
			}
			if (intStatus & MACREG_A2HRIC_BIT_QUEUE_FULL)
			{
				intStatus &= ~MACREG_A2HRIC_BIT_QUEUE_FULL;
				retVal = IRQ_HANDLED;
			}
			/* IEEE80211_DH */
			if (intStatus & MACREG_A2HRIC_BIT_RADAR_DETECT)
			{
				intStatus &= ~MACREG_A2HRIC_BIT_RADAR_DETECT;
				WLSYSLOG(netdev, WLSYSLOG_CLASS_ALL, WLSYSLOG_MSG_GEN_RADARDETECTION);
				WLSNDEVT(netdev,IWEVCUSTOM, (IEEEtypes_MacAddr_t *)&wlpptr->hwData.macAddr[0], WLSYSLOG_MSG_GEN_RADARDETECTION);
				radarDetectionHdlr(netdev); /* DFS_SUPPORT */
				retVal = IRQ_HANDLED;
			}
			if (intStatus & MACREG_A2HRIC_BIT_CHAN_SWITCH)
			{
				intStatus &= ~MACREG_A2HRIC_BIT_CHAN_SWITCH;
				dfsChanSwitchHdlr(netdev); /* DFS_SUPPORT */
				retVal = IRQ_HANDLED;
			}
			/* IEEE80211_DH */
			if(intStatus & MACREG_A2HRIC_BIT_TX_WATCHDOG)
			{
				intStatus &= ~MACREG_A2HRIC_BIT_TX_WATCHDOG;
				wlpptr->netDevStats->tx_heartbeat_errors++;
				wlResetTask(netdev);
				retVal = IRQ_HANDLED;
			}
			/* AMPDU_SUPPORT_SBA */
			if (intStatus & MACREG_A2HRIC_BA_WATCHDOG)
			{
#define BA_STREAM 4

#define INVALID_WATCHDOG 0xAA
				u_int8_t bitmap=0xAA,stream=0;
				intStatus &= ~MACREG_A2HRIC_BA_WATCHDOG;
				wlFwGetWatchdogbitmap(wlpptr,&bitmap);
				printk("watchdog cause by queue %d\n",bitmap);
				if(bitmap!=INVALID_WATCHDOG)
				{
					if(bitmap== BA_STREAM)
						stream=0;
					else if(bitmap>BA_STREAM)
						stream=bitmap-BA_STREAM;
					else
						stream=bitmap+3; /** queue 0 is stream 3*/
					if (bitmap!=0xFF)
					{
						/* Check if the stream is in use before disabling it */
						if (wlpptr->wlpd_p->Ampdu_tx[stream].InUse)
						{
							disableAmpduTxstream(wlpptr,stream);
						}
					}
					else
						disableAmpduTxAll(wlpptr);

				}
				retVal = IRQ_HANDLED;
			}
			/* AMPDU_SUPPORT_SBA */
			
			if(intStatus & MACREG_A2HRIC_CONSEC_TXFAIL)
			{
				MIB_802DOT11 *mib = vmacSta_p->ShadowMib802dot11;
				intStatus &= ~MACREG_A2HRIC_CONSEC_TXFAIL;				
				printk("Consecutive tx fail cnt > %d\n", (u_int32_t)*(mib->mib_consectxfaillimit));		
				ConsecTxFailHdlr(netdev);				
				retVal = IRQ_HANDLED;
			}
		}
		currIteration++;
	} while (currIteration < MAX_ISR_ITERATION);

	return retVal;
}

void wlInterruptEnable(struct net_device *netdev)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);

	if (wlChkAdapter(wlpptr))
	{
		writel(0x00, wlpptr->ioBase1 + MACREG_REG_A2H_INTERRUPT_MASK);

		writel((MACREG_A2HRIC_BIT_MASK), 
			wlpptr->ioBase1 + MACREG_REG_A2H_INTERRUPT_MASK);
	}
}

void wlInterruptDisable(struct net_device *netdev)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);

	if (wlChkAdapter(wlpptr))
	{
		writel(0x00, wlpptr->ioBase1 + MACREG_REG_A2H_INTERRUPT_MASK);
	}
}

/* WFA_TKIP_NEGATIVE */
int wlValidateSettings(struct net_device *netdev)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	vmacApInfo_t *vmacSta_p = wlpptr->vmacSta_p;
	MIB_802DOT11 *mib = vmacSta_p->ShadowMib802dot11;
	int retval = SUCCESS;

	/* Perform checks on the validity of configuration combinations */
	/* Check the validity of the opmode and security mode combination */
	if ((*(mib->mib_wpaWpa2Mode) & 0x0F) == 1 &&
		(*(mib->mib_ApMode)== AP_MODE_N_ONLY
		|| *(mib->mib_ApMode) == AP_MODE_BandN
		|| *(mib->mib_ApMode) == AP_MODE_GandN
		|| *(mib->mib_ApMode) == AP_MODE_BandGandN
		|| *(mib->mib_ApMode) == AP_MODE_5GHZ_N_ONLY
		|| *(mib->mib_ApMode) == AP_MODE_2_4GHZ_11AC_MIXED
		|| *(mib->mib_ApMode) == AP_MODE_5GHZ_11AC_ONLY
		|| *(mib->mib_ApMode) == AP_MODE_5GHZ_Nand11AC
		|| *(mib->mib_ApMode) == AP_MODE_AandN)) /*WPA-TKIP or WPA-AES mode */
	{
		printk("HT mode not supported when WPA is enabled\n");
		WLSYSLOG(netdev, WLSYSLOG_CLASS_ALL, "HT mode not supported when WPA is enabled\n");
		WLSNDEVT(netdev, IWEVCUSTOM,(IEEEtypes_MacAddr_t *)&wlpptr->hwData.macAddr[0], "HT mode not supported when WPA is enabled\n");

		WLDBG_EXIT_INFO(DBG_LEVEL_0, "settings not valid");
		retval = FAIL;
	}

	return retval;
} /* WFA_TKIP_NEGATIVE */  

/** private functions **/

static int wlopen(struct net_device *netdev)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	vmacApInfo_t *vmacSta_p = wlpptr->vmacSta_p;		

	WLDBG_ENTER(DBG_LEVEL_2);

	memset(wlpptr->netDevStats, 0x00, sizeof(struct net_device_stats));
	memset(&((struct wlprivate_data *)(wlpptr->wlpd_p))->privStats, 0x00, sizeof(struct wlpriv_stats));

	netdev->type = ARPHRD_ETHER;

	if (netdev->flags & IFF_RUNNING)
	{
		vmacSta_p->InfUpFlag = 0; 	
		netif_stop_queue(netdev);
		netdev->flags &= ~IFF_RUNNING;
		wlInterruptDisable(netdev);
	}

	if (wlValidateSettings(netdev)) /* WFA_TKIP_NEGATIVE */
		return -EIO;

	if(wlFwApplySettings(wlpptr))
		return -EIO;

	wlInterruptEnable(netdev);
	netif_wake_queue(netdev);  
	vmacSta_p->InfUpFlag = 1; 	
	netdev->flags |= IFF_RUNNING;

	scanControl(wlpptr); /* AUTOCHANNEL */

	/* Initialize the STADB timers */    
	if(wlpptr->vmacSta_p->rootvmac== NULL)
	{
		extStaDb_AgingTimerInit(wlpptr);
		extStaDb_ProcessKeepAliveTimerInit(wlpptr);
	}

	WL_MOD_INC_USE(THIS_MODULE, return -EIO);
	WLDBG_EXIT(DBG_LEVEL_2);
	return 0;
}

static int wlstop(struct net_device *netdev)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	vmacApInfo_t *vmacSta_p = wlpptr->vmacSta_p;	

	WLDBG_ENTER(DBG_LEVEL_2);

	/*Set InfUpFlag in beginning of stop function to prevent sending Auth pkt during stop process.*/
	/*When down interface, some connected client sends Auth pkt right away and AP still process it*/
	/*till Assoc Resp during down process. This can cause GlobalStationCnt in fw to be +1, which is wrong.*/
	vmacSta_p->InfUpFlag = 0;	
	
	if (netdev->flags & IFF_RUNNING)
	{
		netif_stop_queue(netdev);
		netdev->flags &= ~IFF_RUNNING;
		if (wlFwSetAPBss(wlpptr, WL_DISABLE))
		{
			WLDBG_WARNING(DBG_LEVEL_2, "disabling AP bss failed");
		}
		if (wlFwSetRadio(wlpptr, WL_DISABLE, WL_AUTO_PREAMBLE))
		{
			WLDBG_WARNING(DBG_LEVEL_2, "disabling rf failed");
		}
		wlInterruptDisable(netdev);
	}

	WL_MOD_DEC_USE(THIS_MODULE);
	WLDBG_EXIT(DBG_LEVEL_2);
	return 0;
}

static void wlsetMcList(struct net_device *netdev)
{
	WLDBG_ENTER(DBG_LEVEL_2);
	WLDBG_EXIT(DBG_LEVEL_2);
}

static struct net_device_stats *	wlgetStats(struct net_device *netdev)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);

	WLDBG_ENTER(DBG_LEVEL_2);

	WLDBG_EXIT(DBG_LEVEL_2);
	return (wlpptr->netDevStats);
}

static int wlsetMacAddr(struct net_device *netdev, void *addr)
{
	struct sockaddr *macAddr = (struct sockaddr *) addr;

	WLDBG_ENTER(DBG_LEVEL_2);

	if (is_valid_ether_addr(macAddr->sa_data))
	{
		WLDBG_EXIT(DBG_LEVEL_2);
		return 0; /* for safety do not allow changes in MAC-ADDR! */
	}
	WLDBG_EXIT_INFO(DBG_LEVEL_2, "invalid addr");
	return -EADDRNOTAVAIL;
}

static int wlchangeMtu(struct net_device *netdev, int mtu)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	netdev->mtu = mtu;
	if (netdev->flags & IFF_RUNNING)
	{
		return (wlpptr->wlreset(netdev));
	} else
		return -EPERM;

	return 0;
}

int wlreset(struct net_device *netdev)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	vmacApInfo_t *vmacSta_p=wlpptr->vmacSta_p;
	DfsCmd_t    dfsCmd ; /* DFS_SUPPORT */
	int i; /* WDS_SUPPORT */
	char dev_running[MAX_VMAC_INSTANCE_AP+1]; /* WDS_SUPPORT */

	vmacSta_p->mtu = netdev->mtu;
	/* AUTOCHANNEL */
	{
		Disable_ScanTimerProcess(wlpptr);
		vmacSta_p->busyScanning = 0;
		Disable_extStaDb_ProcessKeepAliveTimer(wlpptr);
		Disable_MonitorTimerProcess(wlpptr);
	} /* AUTOCHANNEL */
	vmacSta_p->download = TRUE;
	WLDBG_ENTER(DBG_LEVEL_2);

	if (((struct wlprivate_data *)(wlpptr->wlpd_p))->inReset)
	{
		return 0;
	} else
	{
		((struct wlprivate_data *)(wlpptr->wlpd_p))->inReset = WL_TRUE;
	}
	{
		printk(KERN_INFO "Stop client netdev = %x \n", (int)wlpptr->txNetdev_p);
		if (wlpptr->txNetdev_p)
		{
			if (wlpptr->txNetdev_p->flags & IFF_RUNNING)
			{
				vmacSta_p->InfUpFlag = 0; 
				netif_stop_queue(wlpptr->txNetdev_p);
				wlpptr->txNetdev_p->flags &= ~IFF_RUNNING;
			}
		}
	}
	if(netdev->flags & IFF_RUNNING)
	{
		vmacSta_p->InfUpFlag = 0; 	
		netif_stop_queue(netdev);
		netdev->flags &= ~IFF_RUNNING;
	}
	i = 0;
	while( i <=MAX_VMAC_INSTANCE_AP )
	{
		//remember the interface up/down status, and bring down it.
		if(wlpptr->vdev[i]){
			dev_running[i] = 0;
			if ((wlpptr->vdev[i]->vmacSta_p->OpMode == WL_OP_MODE_AP)
			|| (wlpptr->vdev[i]->vmacSta_p->OpMode == WL_OP_MODE_VAP))
			{
				if(wlpptr->vdev[i]->vmacSta_p->InfUpFlag )
					dev_running[i] = 1;
				wlstop_mbss(wlpptr->vdev[i]->vmacSta_p->dev);
			}
			if (wlpptr->vdev[i]->vmacSta_p->OpMode == WL_OP_MODE_VSTA)
			{
				if(wlpptr->vdev[i]->vmacSta_p->InfUpFlag )
				{
					dev_running[i] = 1;
					wlstop_client(wlpptr->vdev[i]->vmacSta_p->dev);
				}
			}
		}
		i++;
	}
	if (wlFwSetAPBss(wlpptr, WL_DISABLE))
	{
		WLDBG_EXIT_INFO(DBG_LEVEL_2, "disable AP bss failed");
	}
	if (wlFwSetRadio(wlpptr, WL_DISABLE, WL_AUTO_PREAMBLE))
	{
		WLDBG_EXIT_INFO(DBG_LEVEL_2, "disable rf failed");
	}
	wlInterruptDisable(netdev);
	netif_wake_queue(netdev);  /* restart Q if interface was running */
	vmacSta_p->InfUpFlag = 1;	
	netdev->flags |= IFF_RUNNING;

	if(wlFwApplySettings(wlpptr))
		return -EIO;

	wlInterruptEnable(netdev);

	WLDBG_EXIT(DBG_LEVEL_2);
	wlpptr->vmacSta_p->download = FALSE;
	((struct wlprivate_data *)(wlpptr->wlpd_p))->inReset = WL_FALSE;
	if (wlpptr->mfgEnable) {
		return 0;
	}
	scanControl(wlpptr); /* AUTOCHANNEL */
	i = 0;
	while( i <=MAX_VMAC_INSTANCE_AP )
	{
		//bring the vitual interface back if it brought down the routine
		if(wlpptr->vdev[i]){
			if ((wlpptr->vdev[i]->vmacSta_p->OpMode == WL_OP_MODE_AP) ||
				(wlpptr->vdev[i]->vmacSta_p->OpMode == WL_OP_MODE_VAP))
			{
				if(dev_running[i])
				{
					wlreset_mbss(wlpptr->vdev[i]->vmacSta_p->dev);
				}
			}
			if (wlpptr->vdev[i]->vmacSta_p->OpMode == WL_OP_MODE_VSTA)
			{
				if (dev_running[i])
				{
					wlreset_client(wlpptr->vdev[i]->vmacSta_p->dev);
				}
			}
		}
		i++;
	}
	((struct wlprivate_data *)(wlpptr->wlpd_p))->inResetQ = WL_FALSE;
	/* Send the reset message to
	* the DFS event dispatcher
	*/
	dfsCmd.CmdType = DFS_CMD_WL_RESET ; /* DFS_SUPPORT */
	evtDFSMsg( wlpptr, (UINT8 *)&dfsCmd ); /* DFS_SUPPORT */
	wlpptr->wlpd_p->BcnAddHtOpMode = 0;
	wlpptr->wlpd_p->TxGf = 0;


	/* COEXIST_20_40_SUPPORT */
	{
		MIB_802DOT11 *mib =  vmacSta_p->ShadowMib802dot11;
		MIB_PHY_DSSS_TABLE *PhyDSSSTable=mib->PhyDSSSTable;
		extern int wlFwSet11N_20_40_Switch(WL_PRIV *, UINT8 mode);
		extern void  Check20_40_Channel_switch(int option, int * mode);
		extern void Disable_StartCoexisTimer(WL_PRIV *wlpptr);



		if((*(mib->USER_ChnlWidth )&0xf0) && ((*(vmacSta_p->Mib802dot11->mib_ApMode)&AP_MODE_BAND_MASK) < AP_MODE_A_ONLY))
		{
			wlFwSet11N_20_40_Switch(wlpptr, 0);
			*(mib->USER_ChnlWidth )=0;
		}
		else

			if ((PhyDSSSTable->Chanflag.ChnlWidth == CH_AUTO_WIDTH) || 
				(PhyDSSSTable->Chanflag.ChnlWidth == CH_40_MHz_WIDTH))
			{

				if (PhyDSSSTable->CurrChan == 14)
					*(mib->USER_ChnlWidth )= 0;
				else
					*(mib->USER_ChnlWidth )= 1;
				Disable_StartCoexisTimer(wlpptr);

			}




	}
	/* COEXIST_20_40_SUPPORT */

	return 0;
}

static void wltxTimeout(struct net_device *netdev)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);

	WLDBG_ENTER(DBG_LEVEL_2);

	if (((struct wlprivate_data *)(wlpptr->wlpd_p))->inReset)
	{
		return;
	}

	if ((netdev->flags & IFF_RUNNING) == 0 )  /* DFS_SUPPORT */
	{
		return ;
	}

	((struct wlprivate_data *)(wlpptr->wlpd_p))->isTxTimeout = WL_TRUE;
	wlpptr->wlreset(netdev);
	((struct wlprivate_data *)(wlpptr->wlpd_p))->isTxTimeout = WL_FALSE;
	WLDBG_EXIT(DBG_LEVEL_2);
}

void wlSendEvent(struct net_device *dev, int cmd, IEEEtypes_MacAddr_t *Addr, const char * info)
{
	union iwreq_data wrqu;
	char buf[128];

	memset(&wrqu, 0, sizeof(wrqu));

	if (cmd == IWEVCUSTOM)
	{
		if(Addr == NULL)
		{
			snprintf(buf, sizeof(buf), "%s", info);
		}
		else
		{
			snprintf(buf, sizeof(buf), "%02x:%02x:%02x:%02x:%02x:%02x:%s",
				*((unsigned char*)Addr), *((unsigned char*)Addr+1), *((unsigned char*)Addr + 2), 
				*((unsigned char*)Addr+3), *((unsigned char*)Addr+4), *((unsigned char*)Addr + 5),info);
		}
		wrqu.data.length = strlen(buf);
	}
	else    
	{
		wrqu.data.length = 0;
		memcpy(wrqu.ap_addr.sa_data, (unsigned char *)Addr, sizeof(IEEEtypes_MacAddr_t));
		wrqu.ap_addr.sa_family = ARPHRD_ETHER;
	}
	/* Send event to user space */
	wireless_send_event(dev, cmd, &wrqu, buf);
}

/* WDS_SUPPORT */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,28)
static const struct net_device_ops wlwds_netdev_ops = {
	.ndo_open 		= wlopen_wds,
	.ndo_stop		= wlstop_wds,
	.ndo_start_xmit		= wlDataTx,
	.ndo_do_ioctl       = wlIoctl,
	.ndo_set_mac_address	= wlsetMacAddr_wds,
	.ndo_tx_timeout 	= wltxTimeout_wds,
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,2,0)
	.ndo_set_rx_mode    = wlsetMcList,
#else
	.ndo_set_multicast_list = wlsetMcList,
#endif
	.ndo_change_mtu		= wlchangeMtu_wds,
	.ndo_get_stats     = wlgetStats,
};
#endif

int	wlInit_wds(struct wlprivate *wlpptr)
{
	UINT16 i;
#if LINUX_VERSION_CODE >=KERNEL_VERSION(2,6,25)
	struct net_device *dev;
#endif
	//	char devName[16];
	for (i = 0 ; i < MAX_WDS_PORT ; i++ )
	{   
#if LINUX_VERSION_CODE >=KERNEL_VERSION(2,6,25)
		dev = alloc_netdev(0, DRV_NAME_WDS, wlwlan_setup);
		NETDEV_PRIV_S( dev) = NETDEV_PRIV(struct wlprivate, dev);
		wlpptr->vmacSta_p->wdsPort[i].netDevWds = dev;
#else
		wlpptr->vmacSta_p->wdsPort[i].netDevWds = &wlpptr->vmacSta_p->wdsPort[i].netDevWdsPriv;
		memset((UINT8 *) wlpptr->vmacSta_p->wdsPort[i].netDevWds, 0, sizeof(struct net_device));
#endif
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,28)
		wlpptr->vmacSta_p->wdsPort[i].netDevWds->open              = wlopen_wds;
		wlpptr->vmacSta_p->wdsPort[i].netDevWds->stop              = wlstop_wds;

		wlpptr->vmacSta_p->wdsPort[i].netDevWds->hard_start_xmit    = wlDataTx;
		wlpptr->vmacSta_p->wdsPort[i].netDevWds->tx_timeout         = wltxTimeout_wds;
		wlpptr->vmacSta_p->wdsPort[i].netDevWds->set_multicast_list = wlsetMcList;
		wlpptr->vmacSta_p->wdsPort[i].netDevWds->do_ioctl           = wlIoctl;
		wlpptr->vmacSta_p->wdsPort[i].netDevWds->get_stats          = wlgetStats;
		wlpptr->vmacSta_p->wdsPort[i].netDevWds->set_mac_address    = wlsetMacAddr_wds;
		wlpptr->vmacSta_p->wdsPort[i].netDevWds->change_mtu         = wlchangeMtu_wds;
#else
		wlpptr->vmacSta_p->wdsPort[i].netDevWds->netdev_ops         = &wlwds_netdev_ops;
		wlpptr->vmacSta_p->wdsPort[i].netDevWds->ethtool_ops         = &wl_ethtool_ops;
#endif
		wlpptr->vmacSta_p->wdsPort[i].netDevWds->watchdog_timeo     = 30 * HZ;

		wlpptr->vmacSta_p->wdsPort[i].netDevWds->irq       = wlpptr->netDev->irq;
		wlpptr->vmacSta_p->wdsPort[i].netDevWds->mem_start = wlpptr->netDev->mem_start;
		wlpptr->vmacSta_p->wdsPort[i].netDevWds->mem_end   = wlpptr->netDev->mem_end;
		NETDEV_PRIV_S( wlpptr->vmacSta_p->wdsPort[i].netDevWds)      = (void *) wlpptr;
		wlpptr->vmacSta_p->wdsPort[i].pWdsDevInfo         = (void *) &wlpptr->vmacSta_p->wdsPeerInfo[i];
		sprintf(wlpptr->vmacSta_p->wdsPort[i].netDevWds->name, DRV_NAME_WDS, wlpptr->netDev->name, (int )i);

		wlpptr->vmacSta_p->wdsActive[i] = FALSE;
		wlpptr->vmacSta_p->wdsPort[i].active = FALSE;
		memcpy(wlpptr->vmacSta_p->wdsPort[i].netDevWds->dev_addr, wlpptr->vmacSta_p->macStaAddr, 6);
		ether_setup(wlpptr->vmacSta_p->wdsPort[i].netDevWds);
		if (register_netdev(wlpptr->vmacSta_p->wdsPort[i].netDevWds))
		{
			printk(KERN_ERR "%s: failed to register WDS device\n", wlpptr->vmacSta_p->wdsPort[i].netDevWds->name);
			return FALSE;
		}
		wlpptr->vmacSta_p->wdsPort[i].wdsPortRegistered = TRUE;
	}

	return SUCCESS;
}

int	wlStop_wdsDevs(struct net_device *netdev)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	UINT16 i;
	for (i = 0 ; i < MAX_WDS_PORT ; i++ )
	{ 
		wlstop_wds(wlpptr->vmacSta_p->wdsPort[i].netDevWds);
	}

	return SUCCESS;
}

static int wlopen_wds(struct net_device *netdev)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	vmacApInfo_t *vmacSta_p = wlpptr->vmacSta_p;		

	WLDBG_ENTER(DBG_LEVEL_2);


	netdev->type = ARPHRD_ETHER;

	if (netdev->flags & IFF_RUNNING)
	{
		vmacSta_p->InfUpFlag = 0;		
		netif_stop_queue(netdev);
		netdev->flags &= ~IFF_RUNNING;
	}
	else
	{
		WL_MOD_INC_USE(THIS_MODULE, return -EIO);
	}


	netif_wake_queue(netdev);  /* Start/Restart Q if stopped. */
	vmacSta_p->InfUpFlag = 1;		
	netdev->flags |= IFF_RUNNING;
	WLDBG_EXIT(DBG_LEVEL_2);
	return 0;
}

static int wlstop_wds(struct net_device *netdev)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	vmacApInfo_t *vmacSta_p = wlpptr->vmacSta_p;		

	vmacSta_p->InfUpFlag = 0;		
	WLDBG_ENTER(DBG_LEVEL_2);

	if (netdev->flags & IFF_RUNNING)
	{
		netif_stop_queue(netdev);
		netdev->flags &= ~IFF_RUNNING;
		WL_MOD_DEC_USE(THIS_MODULE);
	}
	WLDBG_EXIT(DBG_LEVEL_2);
	return 0;
}

static void wltxTimeout_wds(struct net_device *netdev)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);

	WLDBG_ENTER(DBG_LEVEL_2);

	if (((struct wlprivate_data *)(wlpptr->wlpd_p))->inReset)
	{
		return;
	}

	wlreset_wds(netdev);
	WLDBG_EXIT(DBG_LEVEL_2);
}


static int wlsetMacAddr_wds(struct net_device *netdev, void *addr)
{
	struct sockaddr *macAddr = (struct sockaddr *) addr;

	WLDBG_ENTER(DBG_LEVEL_2);
	if (is_valid_ether_addr(macAddr->sa_data))
	{ 
		memcpy(netdev->dev_addr, addr, 6);
		WLDBG_EXIT(DBG_LEVEL_2);
		return 0;
	}
	WLDBG_EXIT_INFO(DBG_LEVEL_2, "invalid addr");
	return -EADDRNOTAVAIL;
}

static int wlchangeMtu_wds(struct net_device *netdev, int mtu)
{
	netdev->mtu = mtu;
	if (netdev->flags & IFF_RUNNING)
	{
		return (wlreset_wds(netdev));
	} else
		return -EPERM;

	return 0;
}


int wlreset_wds(struct net_device *netdev)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	vmacApInfo_t *vmacSta_p = wlpptr->vmacSta_p;		
	WLDBG_ENTER(DBG_LEVEL_2);

	if (((struct wlprivate_data *)(wlpptr->wlpd_p))->inReset)
	{
		return 0;
	} 
    disableAmpduTxAll( wlpptr );

	if(netdev->flags & IFF_RUNNING)
	{
		vmacSta_p->InfUpFlag = 0;		
		netif_stop_queue(netdev);
		netdev->flags &= ~IFF_RUNNING;
	}

	netif_wake_queue(netdev);  /* restart Q if interface was running */
	vmacSta_p->InfUpFlag = 1;		
	netdev->flags |= IFF_RUNNING;

	WLDBG_EXIT(DBG_LEVEL_2);
	return 0;

}
/* WDS_SUPPORT */

static int wlopen_mbss(struct net_device *netdev)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	vmacApInfo_t *vmacSta_p = wlpptr->vmacSta_p;

	WLDBG_ENTER(DBG_LEVEL_2);


	netdev->type = ARPHRD_ETHER;

	if (netdev->flags & IFF_RUNNING)
	{
		vmacSta_p->InfUpFlag = 0;		
		netif_stop_queue(netdev);
		netdev->flags &= ~IFF_RUNNING;
	}else
	{
		WL_MOD_INC_USE(THIS_MODULE, return -EIO);
	}

	if (wlValidateSettings(netdev)) /* WFA_TKIP_NEGATIVE */
		return -EIO;

	wlFwMultiBssApplySettings(wlpptr);
	if(wlpptr->vmacSta_p->rootvmac)
	{
		//set wdev0 OpMode to follow wdev0apX's opmode
		vmacSta_p->rootvmac->OpMode = vmacSta_p->OpMode;
	}

	netif_wake_queue(netdev);  /* Start/Restart Q if stopped. */
	vmacSta_p->InfUpFlag = 1;		
	vmacSta_p->mtu = netdev->mtu;
	netdev->flags |= IFF_RUNNING;
	WLDBG_EXIT(DBG_LEVEL_2);
	return 0;
}

static int wlstop_mbss(struct net_device *netdev)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	vmacApInfo_t *vmacSta_p = wlpptr->vmacSta_p;	

	/*Set InfUpFlag in beginning of stop function to prevent sending Auth pkt during stop process.*/
	/*When down interface, some connected client sends Auth pkt right away and AP still process it*/
	/*till Assoc Resp during down process. This can cause GlobalStationCnt in fw to be +1, which is wrong.*/	
	vmacSta_p->InfUpFlag = 0;	

	WLDBG_ENTER(DBG_LEVEL_2);
	wlStop_wdsDevs(netdev); /* WDS_SUPPORT */
	SendResetCmd(wlpptr, 0);
	if (netdev->flags & IFF_RUNNING)
	{
		if (wlFwSetAPBss(wlpptr, WL_DISABLE_VMAC))
		{
			WLDBG_EXIT_INFO(DBG_LEVEL_2, "disable AP bss failed");
		}
		netif_stop_queue(netdev);
		netdev->flags &= ~IFF_RUNNING;
		WL_MOD_DEC_USE(THIS_MODULE);
	}
	DisableMacMgmtTimers(wlpptr);
	WLDBG_INFO(DBG_LEVEL_2, "Stop mbss name = %s \n", netdev->name);

	WLDBG_EXIT(DBG_LEVEL_2);
	return 0;
}
int wlreset_mbss(struct net_device *netdev)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	vmacApInfo_t *vmacSta_p = wlpptr->vmacSta_p;		

	WLDBG_ENTER(DBG_LEVEL_2);
	vmacSta_p->mtu = netdev->mtu;

	if (((struct wlprivate_data *)(wlpptr->wlpd_p))->inReset)
	{
		return 0;
	}
	
	/* WDS_SUPPORT */
	{
		int i;
		// Stop any wds port queues that are active.
		for (i = 0; i < 6; i++)
		{
			if (wdsPortActive(wlpptr, i))
			{
				vmacSta_p->InfUpFlag = 0;		
				netif_stop_queue(wlpptr->vmacSta_p->wdsPort[i].netDevWds);
				wlpptr->vmacSta_p->wdsPort[i].netDevWds->flags &= ~IFF_RUNNING;
			}
		}
	} /* WDS_SUPPORT */

	if(netdev->flags & IFF_RUNNING)
	{
		vmacSta_p->InfUpFlag = 0;		
		if (wlFwSetAPBss(wlpptr, WL_DISABLE_VMAC))
		{
			WLDBG_EXIT_INFO(DBG_LEVEL_2, "disable AP bss failed");
		}
		netif_stop_queue(netdev);
		netdev->flags &= ~IFF_RUNNING;
	}
	wlFwMultiBssApplySettings(wlpptr);

	netif_wake_queue(netdev);  /* restart Q if interface was running */
	vmacSta_p->InfUpFlag = 1;		
	netdev->flags |= IFF_RUNNING;

	WLDBG_EXIT(DBG_LEVEL_2);
	return 0;

}

static void wltxTimeout_mbss(struct net_device *netdev)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);

	WLDBG_ENTER(DBG_LEVEL_2);

	if (((struct wlprivate_data *)(wlpptr->wlpd_p))->inReset)
	{
		return;
	}

	wlreset_mbss(netdev);
	WLDBG_EXIT(DBG_LEVEL_2);
}


static int wlsetMacAddr_mbss(struct net_device *netdev, void *addr)
{
	struct sockaddr *macAddr = (struct sockaddr *) addr;

	WLDBG_ENTER(DBG_LEVEL_2);
	if (is_valid_ether_addr(macAddr->sa_data))
	{ 
		memcpy(netdev->dev_addr, addr, 6);
		WLDBG_EXIT(DBG_LEVEL_2);
		return 0;
	}
	WLDBG_EXIT_INFO(DBG_LEVEL_2, "invalid addr");
	return -EADDRNOTAVAIL;
}

static int wlchangeMtu_mbss(struct net_device *netdev, int mtu)
{
	WLDBG_ENTER(DBG_LEVEL_2);
	netdev->mtu = mtu;
	if (netdev->flags & IFF_RUNNING)
	{
		WLDBG_EXIT(DBG_LEVEL_2);
		return (wlreset_mbss(netdev));
	} else
		WLDBG_EXIT(DBG_LEVEL_2);
	return -EPERM;
	WLDBG_EXIT(DBG_LEVEL_2);					
	return 0;
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,28)
static const struct net_device_ops wlmbss_netdev_ops = {
	.ndo_open 		= wlopen_mbss,
	.ndo_stop		= wlstop_mbss,
	.ndo_start_xmit		= wlDataTx,
	.ndo_do_ioctl       = wlIoctl,
	.ndo_set_mac_address	= wlsetMacAddr_mbss,
	.ndo_tx_timeout 	= wltxTimeout_mbss,
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,2,0)
	.ndo_set_rx_mode    = wlsetMcList,
#else
	.ndo_set_multicast_list = wlsetMcList,
#endif
	.ndo_change_mtu		= wlchangeMtu_mbss,
	.ndo_get_stats     = wlgetStats,
};
#endif

int wlInit_mbss(struct wlprivate *wlp, unsigned char * macAddr)
{
	struct wlprivate *wlpptr = NULL;
#if LINUX_VERSION_CODE >=KERNEL_VERSION(2,6,25)
	struct net_device *dev;
	dev = alloc_netdev(sizeof(struct wlprivate), DRV_NAME_VMAC, wlwlan_setup);
	if(dev)	
	{
		wlpptr = NETDEV_PRIV(struct wlprivate, dev);
		NETDEV_PRIV_S( dev) = wlpptr;
	}
#else
	WLDBG_ENTER(DBG_LEVEL_2);
	wlpptr = kmalloc(sizeof(struct wlprivate), GFP_KERNEL);
#endif
	if (wlpptr == NULL)
	{
		printk("%s: no mem for private driver context\n", DRV_NAME);
		goto err_out;
	}

	memset(wlpptr, 0, sizeof(struct wlprivate));
	memcpy(wlpptr, wlp, sizeof(struct wlprivate));
	wlpptr->netDevStats = kmalloc(sizeof(struct net_device_stats), GFP_KERNEL);
	if(wlpptr->netDevStats == NULL)
	{
		printk("%s: no mem for private driver context\n", DRV_NAME);
		goto err_out;		
	}
	memset(wlpptr->netDevStats, 0x00, sizeof(struct net_device_stats));
#if LINUX_VERSION_CODE >=KERNEL_VERSION(2,6,25)
	wlpptr->netDev = dev;
#else
	wlpptr->netDev = &wlpptr->netDevPriv;
	memset(wlpptr->netDev, 0, sizeof(struct net_device));
#endif

	//from probe
	wlpptr->ioBase0 = wlp->ioBase0;
	wlpptr->ioBase1 = wlp->ioBase1;

	sprintf(wlpptr->netDev->name, DRV_NAME_VMAC, wlinitcnt,  wlp->wlpd_p->vmacIndex);
	wlpptr->netDev->irq	   = wlp->netDev->irq;
	wlpptr->netDev->mem_start = wlp->netDev->mem_start;
	wlpptr->netDev->mem_end	 = wlp->netDev->mem_end;
	NETDEV_PRIV_S( wlpptr->netDev) 	 = wlpptr;
	SET_MODULE_OWNER(*(wlpptr->netDev));

	//from init
	memcpy(wlpptr->netDev->dev_addr, &macAddr[0], 6);
	memcpy(&wlpptr->hwData.macAddr[0], &macAddr[0], 6);
	wlpptr->vmacSta_p = wlvmacInit((void *)wlp, wlpptr, &macAddr[0], WL_OP_MODE_VAP, wlinitcnt);
	if(wlpptr->vmacSta_p == NULL)
	{
		printk(KERN_ERR  "%s: failed to init driver mac\n", wlpptr->netDev->name);
		goto err_out;
	}
	wlpptr->vmacSta_p->driver = &ap8x_driver_ops;
	wlpptr->vmacSta_p->skbops = &ap8x_skb_ops;
	wlpptr->vmacSta_p->mtu = wlpptr->netDev->mtu;
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,28)
	wlpptr->netDev->open 		   = wlopen_mbss;
	wlpptr->netDev->stop 		   = wlstop_mbss;

	wlpptr->netDev->hard_start_xmit	= wlDataTx;
	wlpptr->netDev->tx_timeout		= wltxTimeout_mbss;
	wlpptr->netDev->set_multicast_list = wlsetMcList;
	wlpptr->netDev->do_ioctl 		= wlIoctl;
	wlpptr->netDev->get_stats			= wlgetStats;
	wlpptr->netDev->set_mac_address	= wlsetMacAddr_mbss;
	wlpptr->netDev->change_mtu		= wlchangeMtu_mbss;
#else
	wlpptr->netDev->netdev_ops      = &wlmbss_netdev_ops;
	wlpptr->netDev->ethtool_ops         = &wl_ethtool_ops;
#endif
	wlpptr->netDev->watchdog_timeo	= 30 * HZ;
	wlpptr->wlreset = wlreset_mbss;
	wlSetupWEHdlr(wlpptr->netDev);
	wlpptr->wlpd_p = wlp->wlpd_p;
	wlpptr->rootpriv= wlp;
	wlp->vdev[wlp->wlpd_p->vmacIndex++] = wlpptr;
#if LINUX_VERSION_CODE >=KERNEL_VERSION(2,6,39)
#else
	atomic_set(&wlpptr->netDev->refcnt, 0);
#endif
	ether_setup(wlpptr->netDev);
	if (register_netdev(wlpptr->netDev))
	{
		printk("%s: failed to register device\n", wlpptr->netDev->name);
		goto err_register_netdev;
	}
	ap8x_stat_proc_register(wlpptr->netDev);
	wlpptr->netDev->mtu = wlp->netDev->mtu;
	memcpy(wlpptr->netDev->dev_addr, macAddr, 6);
	wlInit_wds(wlpptr); /* WDS_SUPPORT */
	WLDBG_EXIT(DBG_LEVEL_2);	
	return 0;
err_out:
err_register_netdev:
	WL_FREE(wlpptr->netDevStats);
	WL_FREE(wlpptr);
	WLDBG_EXIT(DBG_LEVEL_2);
	return -EIO;
}

void wlDeinit_mbss(struct net_device *netdev)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	struct wlprivate *wlp;
	int i;
	WLDBG_ENTER(DBG_LEVEL_2);
	SendResetCmd(wlpptr, 1);

	for (i = 0; i < (wlpptr->wlpd_p->vmacIndex - 1); i++)
	{
		wlp = wlpptr->vdev[i];
		
		wds_wlDeinit(wlp);
		
		if (wlp->vmacSta_p->InfUpFlag)
		{
			if (wlstop_mbss(wlp->vmacSta_p->dev))
			{
				printk(KERN_ERR "%s: failed to stop device\n",wlp->vmacSta_p->dev->name );
			}
		}
		
		DisableMacMgmtTimers(wlp);
		MacMgmtMemCleanup(wlp);
		ap8x_stat_proc_unregister(wlp->vmacSta_p->dev);
		unregister_netdev(wlp->vmacSta_p->dev);		
#if LINUX_VERSION_CODE >=KERNEL_VERSION(2,6,25)
		free_netdev(wlp->vmacSta_p->dev);
		wlDestroySysCfg(wlp);
#else
		wlDestroySysCfg(wlp);
		WL_FREE(wlp);
#endif
	}
	
	WL_FREE(wlpptr->netDevStats);
	WLDBG_EXIT(DBG_LEVEL_2);
	return;
}

int wlResetTask(struct net_device *dev)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, dev);
	if(((struct wlprivate_data *)(wlpptr->wlpd_p))->inResetQ)
		return 0;
	((struct wlprivate_data *)(wlpptr->wlpd_p))->inResetQ = TRUE;
	schedule_work(&((struct wlprivate_data *)(wlpptr->wlpd_p))->resettask);
	return 0;
}

/* Temporary declaration until suitable MIBs is made available */
UINT8   tmpClientBSSID[NUM_OF_WLMACS][6] = {{0xff, 0xff, 0xff, 0xff, 0xff, 0xff},{0xff, 0xff, 0xff, 0xff, 0xff, 0xff}} ;
void wlLinkMgt(struct net_device *netdev, UINT8 phyIndex)
{
	UINT8   ieBuf[256];
	UINT16  ieBufLen = 0;
	UINT8   ssidLen;
	UINT8   chnlListLen= 0;
	UINT8   chnlScanList[IEEEtypes_MAX_CHANNELS+IEEEtypes_MAX_CHANNELS_A];
	UINT8   currChnlIndex=0;
	UINT8   i;
	IEEEtypes_InfoElementHdr_t *IE_p;
	vmacApInfo_t *vmacSta_p, *primary_vmacSta_p;
	MIB_802DOT11 *mib, *primary_mib;
	struct wlprivate *wlMPrvPtr = NULL, *wlSPrvPtr = NULL;
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	MIB_PHY_DSSS_TABLE *PhyDSSSTable;
	UINT8   mainChnlList[IEEEtypes_MAX_CHANNELS+IEEEtypes_MAX_CHANNELS_A];

	vmacStaInfo_t *vStaInfo_p;

	vStaInfo_p = (vmacStaInfo_t *)vmacGetVMacStaInfo(parentGetVMacId(phyIndex));

	memset(&chnlScanList[0], 0, (IEEEtypes_MAX_CHANNELS+IEEEtypes_MAX_CHANNELS_A));
	memset(&vStaInfo_p->linkInfo, 0, sizeof(iw_linkInfo_t));

	/* Get VMAC structure of the master and host client. */
	if(wlpptr->vmacSta_p->rootvmac)
	{
		/* Get Primary info. */
		wlMPrvPtr = NETDEV_PRIV_P(struct wlprivate, wlpptr->rootpriv->vmacSta_p->dev);        
		primary_vmacSta_p = wlMPrvPtr->vmacSta_p;
		/* primary MIB used for channel settings, client only comes up after AP apply settings */
		primary_mib = primary_vmacSta_p->Mib802dot11;

		/* Get host Client info. */
		wlSPrvPtr =   NETDEV_PRIV_P(struct wlprivate, wlpptr->netDev);
		vmacSta_p   = wlSPrvPtr->vmacSta_p;
		mib = vmacSta_p->Mib802dot11;
	}
	else
		return;
	/* Setup the mode,filter registers appropriate for station operation */
	wlFwSetInfraMode(wlpptr);        
	Disable_extStaDb_ProcessKeepAliveTimer(wlpptr->rootpriv);
	Disable_MonitorTimerProcess(wlpptr->rootpriv);

	extStaDb_ProcessKeepAliveTimerInit(wlpptr->rootpriv);
	MonitorTimerInit(wlpptr->rootpriv);

	PhyDSSSTable=mib->PhyDSSSTable;

	/* Pass the channel list */
	/* if autochannel is enabled then pass in the channel list */
	/* else if autochannel is disabled only pass in a single ch */
	if (*(primary_mib->mib_autochannel))
	{
		/* Stop Autochannel on AP first */
		StopAutoChannel(wlpptr->rootpriv);

		/* get range to scan */
		domainGetInfo(mainChnlList);

		if(*(vmacSta_p->Mib802dot11->mib_STAMode) == CLIENT_MODE_AUTO)
		{
			for (i=0; i < IEEEtypes_MAX_CHANNELS; i++)
			{
				if (mainChnlList[i] > 0)
				{
					chnlScanList[currChnlIndex] = mainChnlList[i];
					currChnlIndex ++;
				}
			}

			for (i=0; i < IEEEtypes_MAX_CHANNELS_A; i++)
			{
				if (mainChnlList[i+IEEEtypes_MAX_CHANNELS] > 0)
				{
					chnlScanList[currChnlIndex] = mainChnlList[i+IEEEtypes_MAX_CHANNELS];
					currChnlIndex ++;
				}
			}
			chnlListLen = currChnlIndex;
		}
		else if((*(vmacSta_p->Mib802dot11->mib_STAMode) < CLIENT_MODE_A) ||
			(*(vmacSta_p->Mib802dot11->mib_STAMode) == CLIENT_MODE_N_24))
		{
			for ( i=0; i < IEEEtypes_MAX_CHANNELS; i++ ) {
				chnlScanList[i] = mainChnlList[i];
			}
			chnlScanList[i] = 0;
			chnlListLen = IEEEtypes_MAX_CHANNELS;
		}
		else
		{
			for ( i=0; i < IEEEtypes_MAX_CHANNELS_A; i++ ) {
				chnlScanList[i] = mainChnlList[i+IEEEtypes_MAX_CHANNELS];
			}
			chnlScanList[i] = 0;
			chnlListLen = IEEEtypes_MAX_CHANNELS_A;
		}
	}
	else
	{
		chnlScanList[0] = PhyDSSSTable->CurrChan;
		chnlListLen = 1;
	}

	/* Set the first channel */
	mlmeApiSetRfChannel(vStaInfo_p, chnlScanList[0], 1, TRUE);

	ieBufLen = 0;
	/* Build IE Buf */
	IE_p = (IEEEtypes_InfoElementHdr_t *)&ieBuf[ieBufLen];

	/* SSID element */
	/* Pick SSID from station net device */
	strncpy((char *)&tmpClientSSID[phyIndex][0], (const char *)&(mib->StationConfig->DesiredSsId[0]), 32);
	ssidLen = strlen((const char *)&(mib->StationConfig->DesiredSsId[0]));
	IE_p->ElementId = SSID;
	IE_p->Len = ssidLen;
	ieBufLen += sizeof(IEEEtypes_InfoElementHdr_t);
	strncpy((char *)&ieBuf[ieBufLen], (const char *)&(mib->StationConfig->DesiredSsId[0]), 32);
	ieBufLen += IE_p->Len;
	IE_p = (IEEEtypes_InfoElementHdr_t *)&ieBuf[ieBufLen];

	/* DS_PARAM_SET element */
	IE_p->ElementId = DS_PARAM_SET;
	IE_p->Len = chnlListLen;
	ieBufLen += sizeof(IEEEtypes_InfoElementHdr_t);
	memcpy((char *)&ieBuf[ieBufLen], &chnlScanList[0],chnlListLen);
	ieBufLen += IE_p->Len;
	IE_p = (IEEEtypes_InfoElementHdr_t *)&ieBuf[ieBufLen];

	// link Mgt might need a MIBs to control funct call
	defaultKeyMgmtInit(phyIndex);

	if( memcmp( mib->StationConfig->DesiredBSSId, "\0x00\0x00\0x00\0x00\0x00\0x00", 6 ))  /* WPS_CLIENT */
		memcpy( &tmpClientBSSID[phyIndex][0], &(mib->StationConfig->DesiredBSSId[0]), 6 );

	/* If user initiated a scan and it is in progress then do not start link mgt */
	if (((vmacApInfo_t *)(wlpptr->vmacSta_p))->gUserInitScan != TRUE)
		linkMgtStart(phyIndex, &tmpClientBSSID[phyIndex][0], &ieBuf[0], ieBufLen);
}

void wlInitClientLink(struct net_device *netdev)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	vmacApInfo_t *vmacSta_p;
	MIB_802DOT11 *mib;

	/* Get VMAC structure of the client.                       */
	/* If master pointer zero, then this is the master device. */
	if(wlpptr->vmacSta_p->rootvmac)
	{
		vmacSta_p = wlpptr->vmacSta_p;
		mib = vmacSta_p->Mib802dot11;
		//set wdev0 OpMode to follow wdev0staX's opmode
		vmacSta_p->rootvmac->OpMode = vmacSta_p->OpMode;
	}
	else
	{
		return;
	}

	if (!(*(mib->mib_STAMacCloneEnable) == 1))
	{        
		vmacEntry_t  *vmacEntry_p;
		if((vmacEntry_p = sme_GetParentVMacEntry(vmacSta_p->VMacEntry.phyHwMacIndx)) != NULL)
		{ 
			wlFwSetMacAddr_Client(wlpptr, &vmacEntry_p->vmacAddr[0]);
		}
		wlLinkMgt(netdev, vmacSta_p->VMacEntry.phyHwMacIndx);
	}


}

/* Client Parent Session Callback function */
void wlStatusUpdate_clientParent(UINT32 data1, UINT8 *info_p, UINT32 data2)
{
	UINT32 statusId = data1;
	UINT32 linkUp = data2;
	vmacEntry_t  *vmacEntry_p = (vmacEntry_t  *)info_p;
	WL_PRIV	*wlpptr;
	UINT8  mlmeAssociatedFlag;
	UINT8  mlmeBssid[6];
	UINT8 numDescpt = 0;
	UINT8 *buf_p;
	UINT16 bufSize = MAX_SCAN_BUF_SIZE;
	MIB_802DOT11 *mib = NULL;
	vmacApInfo_t *vmacSta_p = NULL;

	if(info_p == NULL)
	{
		return;
	}
	wlpptr = (WL_PRIV *)vmacEntry_p->privInfo_p;
	vmacSta_p = wlpptr->vmacSta_p;
	mib = vmacSta_p->Mib802dot11;
	switch(statusId)
	{
	case MmgtIndicationSignals:
		if(!smeGetStaLinkInfo(vmacEntry_p->id,
			&mlmeAssociatedFlag,
			&mlmeBssid[0]))
		{
			return;
		}

		if(linkUp && mlmeAssociatedFlag)
		{

			wlFwSetAid(wlpptr, mlmeBssid, 0);

			printk("**** %s: LINK UP to %02x%02x%02x%02x%02x%02x\n", 
				wlpptr->vmacSta_p->dev->name, mlmeBssid[0],mlmeBssid[1],mlmeBssid[2],mlmeBssid[3],mlmeBssid[4],mlmeBssid[5]);

			/* Send event to user space */
			WLSNDEVT(wlpptr->vmacSta_p->dev, IWEVREGISTERED, &mlmeBssid, NULL); /* WPS_CLIENT */
			WLSYSLOG(wlpptr->vmacSta_p->dev, WLSYSLOG_CLASS_ALL, WLSYSLOG_MSG_CLIENT_CONNECTED "%02x%02x%02x%02x%02x%02x\n",
				mlmeBssid[0],mlmeBssid[1],mlmeBssid[2],mlmeBssid[3],mlmeBssid[4],mlmeBssid[5]);

			memcpy(wlpptr->hwData.macAddr, mlmeBssid, 6);

			/* If Mac cloneing disabled, set vmacEntry to active here. */
			if (!(*(mib->mib_STAMacCloneEnable) == 1))
				vmacEntry_p->active = 1;

		}
		else
		{
			printk("**** %s: LINK NOT UP\n", wlpptr->vmacSta_p->dev->name);
			/* Verify that Key timer is disabled. */
			sme_DisableKeyMgmtTimer(vmacEntry_p);

			/* do not restart linkmgt if user started a scan */
			/* scan complete will trigger a link Mgt restart */
			if (vmacSta_p->gUserInitScan != TRUE)
				linkMgtReStart(vmacEntry_p->phyHwMacIndx, vmacEntry_p);

			if(*(mib->mib_STAMacCloneEnable) == 2)
				ethStaDb_RemoveAllStns(wlpptr);

			/* Remove client and remote ap from Fw and driver databases. */
			RemoveRemoteAPFw((UINT8 *) &mlmeBssid[0],vmacEntry_p);
			WLSYSLOG(wlpptr->vmacSta_p->dev, WLSYSLOG_CLASS_ALL, WLSYSLOG_MSG_CLIENT_DISCONNECTED);

			/* Send event to user space */
			if ((mlmeBssid[0] && mlmeBssid[1] && mlmeBssid[2] &&
				mlmeBssid[3] && mlmeBssid[4] && mlmeBssid[5]))  /* WPS_CLIENT */
			{
				WLSNDEVT(wlpptr->vmacSta_p->dev, IWEVEXPIRED, (IEEEtypes_MacAddr_t *)&mlmeBssid[0], NULL);
			}

			memset(wlpptr->hwData.macAddr, 0, 6);
		}
		break;

	case MlmeScan_Cnfm:
		printk("***** %s SCAN completed\n", wlpptr->vmacSta_p->dev->name);

		WLSYSLOG(wlpptr->vmacSta_p->dev, WLSYSLOG_CLASS_ALL, WLSYSLOG_MSG_CLIENT_SCAN_DONE);

		{
			/* If user initiated a scan */
			if (vmacSta_p->gUserInitScan == TRUE)
			{
				vmacSta_p->gUserInitScan = FALSE;

				/* Send event to user space */
				WLSNDEVT(wlpptr->vmacSta_p->dev, IWEVCUSTOM, &vmacEntry_p->vmacAddr, WLSYSLOG_MSG_CLIENT_SCAN_DONE); /* WPS_CLIENT */

				/* handle the case where a scan completed and link management restarted */
				if (smeGetScanResults(vmacEntry_p->phyHwMacIndx, &numDescpt, &bufSize, &buf_p) == MLME_SUCCESS )
				{
					tmpNumScanDesc[vmacEntry_p->phyHwMacIndx] = numDescpt;
					if (numDescpt > 0)
					{
						memset(tmpScanResults[vmacEntry_p->phyHwMacIndx], 0, MAX_SCAN_BUF_SIZE);
						memcpy(tmpScanResults[vmacEntry_p->phyHwMacIndx], buf_p, bufSize);
					}
				}

				/* reset the busy scanning flag */
				wlpptr->vmacSta_p->busyScanning = 0;

				/*Restart link management */
				if(wlpptr->vmacSta_p->dev->flags & IFF_RUNNING)
				{
					smeGetStaLinkInfo(vmacEntry_p->id,
						&mlmeAssociatedFlag,
						&mlmeBssid[0]);

					if (mlmeAssociatedFlag)
					{
						cleanupAmpduTx(wlpptr, (UINT8 *)&mlmeBssid[0]); /* AMPDU_SUPPORT_TX_CLIENT */
						linkMgtReStart(vmacEntry_p->phyHwMacIndx, vmacEntry_p);
					}
					else
						wlLinkMgt(wlpptr->vmacSta_p->dev, vmacEntry_p->phyHwMacIndx);
				}
			}       
			else
			{
				linkMgtParseScanResult(vmacEntry_p->phyHwMacIndx);
			}
		}
		break;
	case MlmeReset_Cnfm:
		{
			struct net_device *apdev_p= wlpptr->rootpriv->vmacSta_p->dev;
			struct wlprivate *appriv = NETDEV_PRIV_P(struct wlprivate, apdev_p), *appriv1;
			vmacApInfo_t *vap_p;
			int i;
			for (i = 0; i < appriv->wlpd_p->vmacIndex; i++)
			{
				appriv1 = appriv->vdev[i];
				vap_p = appriv1->vmacSta_p;
				if ((appriv->vdev[i]->vmacSta_p->InfUpFlag) &&(vap_p->VMacEntry.modeOfService == VMAC_MODE_AP) )
					wlreset_mbss(appriv->vdev[i]->vmacSta_p->dev);
			}
		}
		break;
	default:
		break;
	}
}

int wlopen_client(struct net_device *netdev)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);	
	vmacApInfo_t *vmacSta_p = wlpptr->vmacSta_p;

	WLDBG_ENTER(DBG_LEVEL_2);

	vmacSta_p->mtu = netdev->mtu;

	netdev->type = ARPHRD_ETHER;

	if (netdev->flags & IFF_RUNNING)
	{
		vmacSta_p->InfUpFlag = 0;		
		netif_stop_queue(netdev);
		netdev->flags &= ~IFF_RUNNING;
	}
	wlFwApplyClientSettings(wlpptr);
	netif_wake_queue(netdev);  /* Start/Restart Q if stopped. */
	vmacSta_p->InfUpFlag = 1;		
	netdev->flags |= IFF_RUNNING;
	WL_MOD_INC_USE(THIS_MODULE, return -EIO);

	/* Wireless Client Specific */
	{
		// Moved to ieee80211_encapSta for Client auto connect.
		wlInitClientLink(netdev);
	}
	/* end Wireless Client Specific */

	WLDBG_EXIT(DBG_LEVEL_2);
	return 0;
}

static int wlstop_client(struct net_device *netdev)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	vmacEntry_t  *vmacParentEntry_p = (vmacEntry_t  *) wlpptr->clntParent_priv_p;
	vmacApInfo_t *vmacSta_p = wlpptr->vmacSta_p;	
	vmacStaInfo_t *vStaInfo_p = (vmacStaInfo_t *)vmacParentEntry_p->info_p;

	WLDBG_ENTER(DBG_LEVEL_2);
	vmacSta_p->InfUpFlag = 0;		

	/* Wireless Client Specific */
	linkMgtStop(vmacParentEntry_p->phyHwMacIndx);

	if (vmacParentEntry_p->active)
	{
		if(vmacParentEntry_p->modeOfService == 1)//client
		{
			//when remove(action is 2), fw ONLY care mac addr				
			wlFwSetNewStn(wlpptr,(u_int8_t *)vmacParentEntry_p->vmacAddr, 0, 0, 2, vStaInfo_p->peerInfo_p, 0, 0);
			wlFwSetNewStn(wlpptr,(u_int8_t *)wlpptr->hwData.macAddr, 0, 0, 2, vStaInfo_p->peerInfo_p, 0, 0);
		}
		
		smeStopBss(vmacParentEntry_p->phyHwMacIndx);
	}
	vmacParentEntry_p->active = 0;
	/* end Wireless Client Specific */

	if (netdev->flags & IFF_RUNNING)
	{
		netif_stop_queue(netdev);
		netdev->flags &= ~IFF_RUNNING;
	}

	WL_MOD_DEC_USE(THIS_MODULE);
	WLDBG_EXIT(DBG_LEVEL_2);
	return 0;
}
int wlreset_client(struct net_device *netdev)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	vmacEntry_t  *vmacParentEntry_p = (vmacEntry_t	*) wlpptr->clntParent_priv_p;
	vmacApInfo_t *vmacSta_p = wlpptr->vmacSta_p;	
	
	WLDBG_ENTER(DBG_LEVEL_2);
	vmacSta_p->mtu = netdev->mtu;

	if (((struct wlprivate_data *)(wlpptr->wlpd_p))->inReset)
	{
		return 0;
	}
	
	smeStopBss(vmacParentEntry_p->phyHwMacIndx); /* Wireless Client Specific */

	if(netdev->flags & IFF_RUNNING)
	{
		vmacSta_p->InfUpFlag = 0; 	
		netif_stop_queue(netdev);
		netdev->flags &= ~IFF_RUNNING;
	}
	wlFwApplyClientSettings(wlpptr);

	netif_wake_queue(netdev);  /* restart Q if interface was running */
	vmacSta_p->InfUpFlag = 1; 	
	netdev->flags |= IFF_RUNNING;

	wlInitClientLink(netdev);

	WLDBG_EXIT(DBG_LEVEL_2);
	return 0;
}

static void wltxTimeout_client(struct net_device *netdev)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);

	WLDBG_ENTER(DBG_LEVEL_2);

	if (((struct wlprivate_data *)(wlpptr->wlpd_p))->inReset)
	{
		return;
	}

	wlreset_client(netdev);
	WLDBG_EXIT(DBG_LEVEL_2);
}


static int wlsetMacAddr_client(struct net_device *netdev, void *addr)
{
	struct sockaddr *macAddr = (struct sockaddr *) addr;

	WLDBG_ENTER(DBG_LEVEL_2);
	if (is_valid_ether_addr(macAddr->sa_data))
	{ 
		memcpy(netdev->dev_addr, addr, 6);
		WLDBG_EXIT(DBG_LEVEL_2);
		return 0;
	}
	WLDBG_EXIT_INFO(DBG_LEVEL_2, "invalid addr");
	return -EADDRNOTAVAIL;
}

static int wlchangeMtu_client(struct net_device *netdev, int mtu)
{
	WLDBG_ENTER(DBG_LEVEL_2);
	netdev->mtu = mtu;
	if (netdev->flags & IFF_RUNNING)
	{
		WLDBG_EXIT(DBG_LEVEL_2);
		return (wlreset_client(netdev));
	} else
		WLDBG_EXIT(DBG_LEVEL_2);
	return -EPERM;
	WLDBG_EXIT(DBG_LEVEL_2);					
	return 0;
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,28)
static const struct net_device_ops wlclient_netdev_ops = {
	.ndo_open 		= wlopen_client,
	.ndo_stop		= wlstop_client,
	.ndo_start_xmit		= wlDataTx,
	.ndo_do_ioctl       = wlIoctl,
	.ndo_set_mac_address	= wlsetMacAddr_client,
	.ndo_tx_timeout 	= wltxTimeout_client,
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,2,0)
	.ndo_set_rx_mode    = wlsetMcList,
#else
	.ndo_set_multicast_list = wlsetMcList,
#endif
	.ndo_change_mtu		= wlchangeMtu_client,
	.ndo_get_stats     = wlgetStats,
};
#endif

int wlInit_client(struct wlprivate *wlp, unsigned char * macAddr_p, unsigned char *ApRootmacAddr_p)
{
	struct wlprivate *wlpptr = NULL;
	vmacEntry_t *clientVMacEntry_p;
#if LINUX_VERSION_CODE >=KERNEL_VERSION(2,6,25)
	struct net_device *dev;
#endif
	/* end Wireless Client Specific */
	WLDBG_ENTER(DBG_LEVEL_2);
	{
#if LINUX_VERSION_CODE >=KERNEL_VERSION(2,6,25)
		dev = alloc_netdev(sizeof(struct wlprivate), DRV_NAME_CLIENT, wlwlan_setup);
		if(dev)
		{
			wlpptr = NETDEV_PRIV(struct wlprivate, dev);
			NETDEV_PRIV_S( dev) = wlpptr;
		}
#else
		wlpptr = kmalloc(sizeof(struct wlprivate), GFP_KERNEL);
#endif
		if (wlpptr == NULL)
		{
			printk("%s: no mem for private driver context\n", DRV_NAME);
			goto err_out;
		}
		memset(wlpptr, 0, sizeof(struct wlprivate));
		memcpy(wlpptr, wlp, sizeof(struct wlprivate));
		wlpptr->netDevStats = kmalloc(sizeof(struct net_device_stats), GFP_KERNEL);
		if(wlpptr->netDevStats == NULL)
		{
			printk("%s: no mem for private driver context\n", DRV_NAME);
			goto err_out;
		}
		memset(wlpptr->netDevStats, 0x00, sizeof(struct net_device_stats));
#if LINUX_VERSION_CODE >=KERNEL_VERSION(2,6,25)
		wlpptr->netDev = dev;
#else
		wlpptr->netDev = &wlpptr->netDevPriv;
		memset(wlpptr->netDev, 0, sizeof(struct net_device));
#endif
		wlpptr->netDev->flags = 0;
#if LINUX_VERSION_CODE >=KERNEL_VERSION(2,6,39)
		wlpptr->netDev->priv_flags = 0;
#else
		wlpptr->netDev->br_port = NULL;
#endif
		//from probe
		wlpptr->ioBase0 = wlp->ioBase0;
		wlpptr->ioBase1 = wlp->ioBase1;
		sprintf(wlpptr->netDev->name, DRV_NAME_CLIENT, wlinitcnt, 0);
		wlpptr->netDev->irq	   = wlp->netDev->irq;
		wlpptr->netDev->mem_start = wlp->netDev->mem_start;
		wlpptr->netDev->mem_end	 = wlp->netDev->mem_end;
		NETDEV_PRIV_S(wlpptr->netDev) = wlpptr;
		SET_MODULE_OWNER(*(wlpptr->netDev));

		/* Use the same address as root AP for stations. */
		memcpy(wlpptr->netDev->dev_addr, ApRootmacAddr_p, 6);
		memcpy(&wlpptr->hwData.macAddr[0], macAddr_p, 6);
		wlpptr->vmacSta_p = wlvmacInit(wlp, wlpptr, macAddr_p, WL_OP_MODE_VSTA, wlinitcnt);
		if(wlpptr->vmacSta_p == NULL)
		{
			printk(KERN_ERR  "%s: failed to init driver mac\n", wlpptr->netDev->name);
			goto err_out;
		}
		wlpptr->vmacSta_p->driver = &ap8x_driver_ops;
		wlpptr->vmacSta_p->skbops = &ap8x_skb_ops;
		wlpptr->vmacSta_p->mtu = wlpptr->netDev->mtu;
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,28)
		wlpptr->netDev->open 		   = wlopen_client;
		wlpptr->netDev->stop 		   = wlstop_client;

		wlpptr->netDev->hard_start_xmit	= wlDataTx;
		wlpptr->netDev->tx_timeout		= wltxTimeout_client;
		wlpptr->netDev->set_multicast_list = wlsetMcList;
		wlpptr->netDev->do_ioctl 		= wlIoctl;
		wlpptr->netDev->get_stats			= wlgetStats;
		wlpptr->netDev->set_mac_address	= wlsetMacAddr_client;
		wlpptr->netDev->change_mtu		= wlchangeMtu_client;
#else
		wlpptr->netDev->netdev_ops = &wlclient_netdev_ops; 
		wlpptr->netDev->ethtool_ops         = &wl_ethtool_ops;
#endif
		wlpptr->netDev->watchdog_timeo	= 30 * HZ;
		wlpptr->wlreset = wlreset_client;
		wlSetupWEHdlr(wlpptr->netDev);
		wlpptr->wlpd_p = wlp->wlpd_p;
		wlpptr->rootpriv= wlp;
		wlp->vdev[wlp->wlpd_p->vmacIndex++] = wlpptr;
#if LINUX_VERSION_CODE >=KERNEL_VERSION(2,6,39)
#else
		atomic_set(&wlpptr->netDev->refcnt, 0);
#endif
		ether_setup(wlpptr->netDev);
		if (register_netdev(wlpptr->netDev))
		{
			printk("%s: failed to register device\n", wlpptr->netDev->name);
			goto err_register_netdev;
		}
		ap8x_stat_proc_register(wlpptr->netDev);

		/* Wireless Client Specific */
		{        
			{
				if((clientVMacEntry_p = smeInitParentSession(wlinitcnt, 
					macAddr_p,
					0,
					&wlStatusUpdate_clientParent,
					(void *)wlpptr)) == NULL)
				{
					goto err_init;
				}
				wlpptr->txNetdev_p = wlpptr->netDev;
				wlpptr->clntParent_priv_p = (void *)clientVMacEntry_p;
				wlpptr->vmacSta_p->VMacEntry.id = clientVMacEntry_p->id;
			}
			// Initialize Client PeerInfo.
			InitClientPeerInfo(wlpptr);

		}
	}
	/* end Wireless Client Specific */

	wetHashInit();

	WLDBG_EXIT(DBG_LEVEL_2);	
	return 0;
err_init:
	ap8x_stat_proc_unregister(wlpptr->netDev);
err_out:
err_register_netdev:
	WL_FREE(wlpptr->netDevStats);
	WL_FREE(wlpptr);
	WLDBG_EXIT(DBG_LEVEL_2);
	return -EIO;
}

void wlDeinit_client(struct net_device *netdev)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	struct wlprivate *wlp;
	WLDBG_ENTER(DBG_LEVEL_2);

	wlp = wlpptr->vdev[wlpptr->wlpd_p->vmacIndex - 1]; 
		
	if (wlp->vmacSta_p->InfUpFlag)
	{
		if (wlstop_client(wlp->vmacSta_p->dev))
		{
			printk(KERN_ERR "%s: failed to stop device\n", wlp->vmacSta_p->dev->name );
		}
	}

	ap8x_stat_proc_unregister(wlp->vmacSta_p->dev);

	WL_FREE(wlp->vmacSta_p->StaCtl);
	
	unregister_netdev(wlp->vmacSta_p->dev);		
#if LINUX_VERSION_CODE >=KERNEL_VERSION(2,6,25)
	free_netdev(wlp->vmacSta_p->dev);
	WL_FREE(wlp->vmacSta_p);
#else
	WL_FREE(wlp->vmacSta_p);
	WL_FREE(wlp);
#endif

	wlpptr->wlpd_p->vmacIndex =0;
	wetHashDeInit();
	WL_FREE(wlpptr->netDevStats);

	WLDBG_EXIT(DBG_LEVEL_2);
	return;
}
