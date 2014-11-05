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
#include	<linux/module.h>
#include <linux/ethtool.h>
#include "wldebug.h"
#include "ap8xLnxFwdl.h"
#include "ap8xLnxRegs.h"
#include "ap8xLnxVer.h"
#include "ap8xLnxDesc.h"
#include "ap8xLnxFwcmd.h"
#include "ap8xLnxXmit.h"
#include "ap8xLnxIntf.h"
#include "ap8xLnxRecv.h"
#ifdef IEEE80211_DH
#include "ap8xLnxWlLog.h"
#endif //IEEE80211_DH
#include "mib.h"
#include "wlvmac.h"
#ifdef WL_KERNEL_26
#include <linux/workqueue.h>
#endif
#if defined (MFG_SUPPORT)
#include "wl_mib.h"
#endif
#include "wl_hal.h"
#include "wlApi.h"
#include "smeMain.h" // MRVL_DFS
#ifdef CLIENT_SUPPORT
#include "linkmgt.h"
#include "ap8xLnxWlLog.h"
#include "mlmeApi.h"
#include "mlmeParent.h"
#endif /* CLIENT_SUPPORT */
#include "StaDb.h"
#ifdef EWB
#include "ewb_hash.h"
#endif
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

#ifdef RELEASE_11N
#ifndef WL_KERNEL_26     
EXPORT_NO_SYMBOLS;
#endif
#endif

#if LINUX_VERSION_CODE >=KERNEL_VERSION(2,6,25)
#define SET_MODULE_OWNER(x)
#endif
#define CMD_BUF_SIZE        0x4000
#ifdef SSU_SUPPORT
#define SSU_BUF_SIZE        0x2000
#endif
#define MAX_ISR_ITERATION   1 // 10


#ifndef __MOD_INC_USE_COUNT
#define WL_MOD_INC_USE(_m, _err)                                     \
	if (1/*isIfUsed == WL_FALSE*/) {                               \
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
	if (1/*isIfUsed == WL_FALSE*/) {							   \
	isIfUsed++; 								  \
	MOD_INC_USE_COUNT;                                            \
	}
#define WL_MOD_DEC_USE(_m)                                           \
	if (isIfUsed ) {								\
	--isIfUsed; 								 \
	MOD_DEC_USE_COUNT;                                            \
	}
#endif

#ifdef CLIENT_SUPPORT
/* This is for MLME use, please don't change without MLME owner input */
struct net_device *mainNetdev_p[NUM_OF_WLMACS];
/* end MLME use */

#endif /* CLIENT_SUPPORT */
/* default settings */

/** external functions **/
extern int ap8x_stat_proc_register(struct net_device *dev);
extern int ap8x_dump_proc_register(struct net_device *dev);
extern int ap8x_stat_proc_unregister(struct net_device *dev);
extern int ap8x_dump_proc_unregister(struct net_device *dev);
extern int ap8x_remove_folder(void );
extern vmacApInfo_t * Mac_Init(struct wlprivate *wlp, struct net_device *dev,char *addr, UINT32 mode, int phyMacId);

extern void MrvlMICErrorHdl(vmacApInfo_t *vmacSta_p,COUNTER_MEASURE_EVENT event);
extern void MrvlICVErrorHdl(vmacApInfo_t *vmacSta_p);
extern extStaDb_Status_e extStaDb_RemoveStaNSendDeauthMsg(vmacApInfo_t *vmac_p,IEEEtypes_MacAddr_t *Addr_p);			


/** external data **/

/** internal functions **/

static int	wlInit_wds(struct wlprivate *wlpptr);
static int wlreset_wds(struct net_device *netdev);
#ifdef CLIENT_SUPPORT
static int wlInit_client(struct wlprivate *wlp, unsigned char * macAddr_p, unsigned char *ApRootmacAddr_p);
static int wlstop_client(struct net_device *netdev);
int wlreset_client(struct net_device *netdev);

extern void rtnl_lock(void);
extern void rtnl_unlock(void);

UINT8   tmpScanResults[NUM_OF_WLMACS][MAX_SCAN_BUF_SIZE];
UINT8   tmpNumScanDesc[NUM_OF_WLMACS];
#endif
#ifdef MBSS
static int wlstop_mbss(struct net_device *netdev);
static int wlopen_mbss(struct net_device *netdev);
#endif
#ifdef MRVL_DFS
int wlRadarDetection(struct net_device *netdev);
int wlApplyCSAChannel(struct net_device *netdev);
#endif

int wlConsecTxFail(struct net_device *netdev); 

/** public data **/
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
#ifdef WL_KERNEL_26 
static int wlsuspend(struct pci_dev *, pm_message_t);
#else
static int wlsuspend(struct pci_dev *, u32);
#endif
static int wlresume(struct pci_dev *);
#ifdef WL_DEBUG
static const char * wlgetAdapterDescription(u_int32_t, u_int32_t);
#endif
static int wlInit_mbss(struct wlprivate *wlp, unsigned char * macAddr);

static struct pci_driver wldriver = {
	.name     = DRV_NAME,
	.id_table = wlid_tbl,
	.probe    = wlprobe,
	.remove   = wlremove,
	.suspend  = wlsuspend,
	.resume   = wlresume,
};

static int wlopen(struct net_device *);
static int wlstop(struct net_device *);
static void wlsetMcList(struct net_device *);
static struct net_device_stats *wlgetStats(struct net_device *);
static int wlsetMacAddr(struct net_device *, void *);
static int wlchangeMtu(struct net_device *, int);
int wlreset(struct net_device *);
int wlreset_mbss(struct net_device *netdev);
int wlreset_client(struct net_device *netdev);

#ifndef SOC_W8764
static int wlreloadFirmwareAndInitDescriptors(struct net_device *);
#endif

static void wltxTimeout(struct net_device *);
module_init(wlmodule_init);
module_exit(wlmodule_exit);
#ifdef NAPI
void wlInterruptMask(struct net_device *netdev, int mask);
#endif
#ifdef WDS_FEATURE
static int wlopen_wds(struct net_device *);
int wlstop_wds(struct net_device *);
int	wlStop_wdsDevs(struct wlprivate *wlpptr);
static int wlsetMacAddr_wds(struct net_device *, void *);
static int wlchangeMtu_wds(struct net_device *, int);
static void wltxTimeout_wds(struct net_device *);
#endif
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
	wlpptr->wlpd_p = wlpdptr;
	wlpdptr->rootdev = wlpptr->netDev;

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

#ifdef SOC_W8764
	printk("wlprobe  wlpptr->ioBase0 = %x \n", (unsigned int) wlpptr->ioBase0);
#endif
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

#ifdef SOC_W8764
	printk("wlprobe  wlpptr->ioBase1 = %x \n", (unsigned int) wlpptr->ioBase1);
#endif
	if (!wlpptr->ioBase1)
	{
		printk(KERN_ERR "%s: cannot remap PCI memory region 1\n", DRV_NAME);
		goto err_release_mem_region_bar1;
	}

#if LINUX_VERSION_CODE >=KERNEL_VERSION(2,6,0)
	sprintf(wlpptr->netDev->name,  DRV_NAME, wlinitcnt);
#else
	memcpy(wlpptr->netDev->name, DRV_NAME, sizeof(DRV_NAME));
#endif
	wlpptr->netDev->irq       = pdev->irq;
	wlpptr->netDev->mem_start = pci_resource_start(pdev, 0);
	wlpptr->netDev->mem_end   = physAddr + pci_resource_len(pdev, 1);
	NETDEV_PRIV_S(wlpptr->netDev)      = wlpptr;
	wlpptr->pPciDev          = pdev;
#ifdef SINGLE_DEV_INTERFACE
#ifdef WDS_FEATURE
	wlprobeInitWds(wlpptr);
#endif
#endif
#ifdef WL_KERNEL_26
	SET_MODULE_OWNER(*(wlpptr->netDev));
#endif

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
	wlpptr->wlpd_p->CardDeviceInfo = pdev->device & 0xff ;
	if (wlpptr->wlpd_p->CardDeviceInfo == 4)
		wlpptr->wlpd_p->SDRAMSIZE_Addr = 0x40fc70b7; /* 16M SDRAM */
	else
		wlpptr->wlpd_p->SDRAMSIZE_Addr = 0x40fe70b7; /* 8M SDRAM */
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
	//kfree(wlpdptr);
err_kfree:
#if LINUX_VERSION_CODE >=KERNEL_VERSION(2,6,25)
	free_netdev(dev);
#else
	kfree(wlpptr);
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
#ifdef WL_KERNEL_26 
static int wlsuspend(struct pci_dev *pdev, pm_message_t state)
#else
static int wlsuspend(struct pci_dev *pdev, u32 state)
#endif
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
	wlpptr->wlpd_p->Timer.function = timer_routine;
	wlpptr->wlpd_p->Timer.data = (unsigned long) netdev;
	wlpptr->wlpd_p->Timer.expires=jiffies + HZ/100;
#ifndef AMSDUOVERAMPDU
	extStaDb_AggrFrameCk(wlpptr->vmacSta_p, 0);
#endif

#ifdef WL_KERNEL_26
	/*We constantly check all txq to see if any txq is not empty. Once any txq is not empty, we schedule a task again
	* to enable all txq are flushed out when no new incoming pkt from host. Sometimes pkts can sit inside txq forever when txq depth
	* is too deep.
	*/
	if(!wlpptr->wlpd_p->isTxTaskScheduled)
	{
		while (num--)
		{	
			if(wlpptr->wlpd_p->txQ[num].qlen!=0)
			{
#ifdef USE_TASKLET			
				tasklet_schedule(&wlpptr->wlpd_p->txtask);
#else
				schedule_work(&wlpptr->wlpd_p->txtask);
#endif
				wlpptr->wlpd_p->isTxTaskScheduled=1;
				break;
			}
		}
	}
#endif
	add_timer(&wlpptr->wlpd_p->Timer);

}

#ifdef WL_KERNEL_26
extern void wlRecv(struct net_device *netdev);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
static void _wlreset(struct work_struct *work)
{
	struct wlprivate_data *wlpd_p = container_of(work, struct wlprivate_data , resettask);
	struct wlprivate *wlpptr = wlpd_p->masterwlp ;
	wlreset(wlpptr->netDev);
}
#ifdef MRVL_DFS
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
#endif

static void _wlConsecTxFail(struct work_struct *work)
{
	struct wlprivate_data *wlpd_p = container_of(work, struct wlprivate_data , kickstatask);
	struct wlprivate *wlpptr = wlpd_p->masterwlp ;
	wlConsecTxFail(wlpptr->netDev);
}

#ifndef USE_TASKLET
static void _wlRecv(struct work_struct *work)
{
	struct wlprivate_data *wlpd_p = container_of(work, struct wlprivate_data , rxtask);
	struct wlprivate *wlpptr = wlpd_p->masterwlp ;
	wlRecv(wlpptr->netDev);
}
static void _wlDataTxHdl(struct work_struct *work)
{
	struct wlprivate_data *wlpd_p = container_of(work, struct wlprivate_data , txtask);
	struct wlprivate *wlpptr = wlpd_p->masterwlp ;
	wlDataTxHdl(wlpptr->netDev);
}
#endif
#endif
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
int wlinitcnt=0;
int	wlInit(struct net_device *netdev, u_int16_t devid)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	int retCode,index,i;
	int bssidmask =0;
	unsigned char macaddr[6]= {0x00, 0xde, 0xad,0xde, 0xad, 0xee};

	WLDBG_ENTER(DBG_LEVEL_2);
#ifdef WL_KERNEL_26
	wlpptr->wlpd_p->masterwlp = wlpptr;
	wlpptr->wlpd_p->isTxTaskScheduled=0;
#ifndef NAPI
#ifdef USE_TASKLET
	tasklet_init(&wlpptr->wlpd_p->rxtask, (void *)wlRecv, (unsigned long)netdev);
#else
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
	INIT_WORK(&wlpptr->wlpd_p->rxtask, (void (*)(void *))_wlRecv);
#else
	INIT_WORK(&wlpptr->wlpd_p->rxtask, (void (*)(void *))wlRecv, netdev);
#endif
#endif
#endif
#ifdef USE_TASKLET
	tasklet_init(&wlpptr->wlpd_p->txtask, (void *)wlDataTxHdl, (unsigned long)netdev);
#else
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
	INIT_WORK(&wlpptr->wlpd_p->txtask, (void (*)(void *))_wlDataTxHdl);
#else
	INIT_WORK(&wlpptr->wlpd_p->txtask, (void (*)(void *))wlDataTxHdl, netdev);
#endif
#endif
#ifdef MRVL_DFS
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
	INIT_WORK(&wlpptr->wlpd_p->dfstask, (void *)(void *)_wlRadarDetection);
	INIT_WORK(&wlpptr->wlpd_p->csatask, (void *)(void *)_wlApplyCSAChannel);    
#else
	INIT_WORK(&wlpptr->wlpd_p->dfstask, (void (*)(void *))wlRadarDetection, netdev);
	INIT_WORK(&wlpptr->wlpd_p->csatask, (void (*)(void *))wlApplyCSAChannel, netdev);
#endif
#endif
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
	INIT_WORK(&wlpptr->wlpd_p->resettask, (void *)(void *)_wlreset);		
	INIT_WORK(&wlpptr->wlpd_p->kickstatask, (void *)(void *)_wlConsecTxFail);		
#else
	INIT_WORK(&wlpptr->wlpd_p->resettask, (void (*)(void *))wlreset, netdev);
	INIT_WORK(&wlpptr->wlpd_p->kickstatask, (void (*)(void *))wlConsecTxFail, netdev);	
#endif
#endif

	SPIN_LOCK_INIT(&wlpptr->wlpd_p->locks.xmitLock);
	SPIN_LOCK_INIT(&wlpptr->wlpd_p->locks.fwLock);

	wlpptr->pCmdBuf = (unsigned short *) 
		pci_alloc_consistent(wlpptr->pPciDev, CMD_BUF_SIZE, &wlpptr->wlpd_p->pPhysCmdBuf);
#ifdef SSU_SUPPORT
	wlpptr->pSsuBuf = (unsigned short *) 
		pci_alloc_consistent(wlpptr->pPciDev, SSU_BUF_SIZE, &wlpptr->wlpd_p->pPhysSsuBuf);
    printk("wlInit SSU pSsuBuf = %x  pPhysSsuBuf = %x\n", wlpptr->pSsuBuf, (UINT32)wlpptr->wlpd_p->pPhysSsuBuf);
#endif
#ifdef SOC_W8764
	printk("wlInit wlpptr->pCmdBuf = %x  wlpptr->wlpd_p->pPhysCmdBuf = %x \n", 
		(unsigned int) wlpptr->pCmdBuf, (unsigned int) wlpptr->wlpd_p->pPhysCmdBuf);
#endif
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

	if (wlPrepareFwFile(netdev))
	{
		printk(KERN_ERR  "%s: prepare firmware downloading failed\n", netdev->name);
		goto err_init_rx;
	}
#ifndef SC_PALLADIUM
	if (wlFwDownload(netdev))
	{
		printk(KERN_ERR  "%s: firmware downloading failed\n", netdev->name);
		goto err_init_rx;
	}
#endif
	if (wlFwGetHwSpecs(netdev))
	{
		printk(KERN_ERR  "%s: failed to get HW specs\n", netdev->name);
		goto err_init_rx;
	}
	memcpy(netdev->dev_addr, &wlpptr->hwData.macAddr[0], 6);
#ifdef SOC_W8764
	printk("Mac address = %s \n", mac_display(&wlpptr->hwData.macAddr[0]));
	printk("Mac_Init \n");
#endif
	wlpptr->vmacSta_p = Mac_Init(NULL, netdev, &wlpptr->hwData.macAddr[0], WL_OP_MODE_AP, wlinitcnt);
	if(wlpptr->vmacSta_p == NULL)
	{
		printk(KERN_ERR  "%s: failed to init driver mac\n", netdev->name);
		goto err_init_rx;
	}
	writel((wlpptr->wlpd_p->descData[0].pPhysTxRing),
		wlpptr->ioBase0 + wlpptr->wlpd_p->descData[0].wcbBase);
#if NUM_OF_DESCRIPTOR_DATA >3
	for (i = 1; i < TOTAL_TX_QUEUES; i++)
	writel((wlpptr->wlpd_p->descData[i].pPhysTxRing),
		wlpptr->ioBase0 + wlpptr->wlpd_p->descData[i].wcbBase);
#endif
	writel((wlpptr->wlpd_p->descData[0].pPhysRxRing),
		wlpptr->ioBase0 + wlpptr->wlpd_p->descData[0].rxDescRead);
	writel((wlpptr->wlpd_p->descData[0].pPhysRxRing),
		wlpptr->ioBase0 + wlpptr->wlpd_p->descData[0].rxDescWrite);
	if (wlFwSetHwSpecs(netdev))
	{
		WLDBG_ERROR(DBG_LEVEL_2, "failed to set HW specs");
	}
#ifndef TIMER_TASK
	init_timer(&wlpptr->wlpd_p->Timer);
	wlpptr->wlpd_p->Timer.function = timer_routine;
	wlpptr->wlpd_p->Timer.data = (unsigned long) netdev;
	wlpptr->wlpd_p->Timer.expires=jiffies + HZ/10;
	add_timer(&wlpptr->wlpd_p->Timer);
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
#ifdef WLAN_INCLUDE_TSO
	netdev->features |= NETIF_F_TSO;
	netdev->features |= NETIF_F_IP_CSUM;
	netdev->features |= NETIF_F_SG;
#endif
	wlpptr->wlreset = wlreset;
	wlSetupWEHdlr(netdev);
#ifdef WL_KERNEL_26
	sprintf(netdev->name,  DRV_NAME, wlinitcnt);
#else
	memcpy(netdev->name, DRV_NAME, sizeof(DRV_NAME));
#endif
#if	defined(SINGLE_DEV_INTERFACE) && !defined(CLIENTONLY)
	wlpptr->vdev[wlpptr->wlpd_p->vmacIndex++] = wlpptr->netDev;
#endif
	if (register_netdev(netdev))
	{
		printk(KERN_ERR "%s: failed to register device\n", DRV_NAME);
		goto err_register_netdev;
	}
#ifdef NAPI
	netif_napi_add(netdev, &wlpptr->napi, wlRecvPoll, MAX_NUM_RX_DESC);
#endif

#ifdef AP8X_STATISTICS
	ap8x_stat_proc_register(netdev);
#endif
#ifdef AP8X_DUMP
	ap8x_dump_proc_register(netdev);
#endif

#ifdef SINGLE_DEV_INTERFACE
#ifdef WDS_FEATURE
	wlInit_wds(wlpptr);
#endif
#endif   
#ifdef CLIENTONLY
	if(wlInit_mbss(wlpptr, &wlpptr->hwData.macAddr[0]))
	{
		printk("*********** Fail to Init Client \n");
	}
#endif
#if defined(MBSS) && !defined(CLIENTONLY)
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
#endif

#ifdef CLIENT_SUPPORT
	{ 
		
		/*For client interface, we use different mac addr from master mac addr*/
		/*If client interface also takes master mac addr like ap0, then there will be conflict if ap0 is up too*/
		/*This procedure to generate client mac addr is also same in macclone api*/
		bssidmask = 0;
		memcpy(macaddr, wlpptr->hwData.macAddr, 6);
	#if defined(MBSS)
		for (index = 0; index< NUMOFAPS; index++)
	#else	
		for (index = 0; index< 1; index++)
	#endif			
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
			
#ifdef SINGLE_DEV_INTERFACE
		/* Set static for continue debugging purposes */
		UINT8 myAddr[6] = {0x00, 0x40, 0x05, 0x8F, 0x55, 0x17};

		/* Update the static with AP's wireless mac address*/
		memcpy(&myAddr[0],&wlpptr->hwData.macAddr[0],6);

		if(wlInit_client(wlpptr, &myAddr[0], &wlpptr->hwData.macAddr[0]))
#else
		if(wlInit_client(wlpptr, &macaddr[0], &macaddr[0]))	
#endif
		{
			printk("*********** Fail to Init Client \n");
		}
	}
#endif /* CLIENT_SUPPORT */
#ifdef QUEUE_STATS
    wldbgResetQueueStats();
#endif
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
#ifdef WL_KERNEL_26
#ifdef USE_TASKLET
#ifndef NAPI
	tasklet_kill(&wlpptr->wlpd_p->rxtask);
#endif
	tasklet_kill(&wlpptr->wlpd_p->txtask);
#endif
	flush_scheduled_work();
#else
	run_task_queue(&tq_immediate);
	run_task_queue(&tq_disk);
#endif
	pci_free_consistent(wlpptr->pPciDev, CMD_BUF_SIZE,
		wlpptr->pCmdBuf, wlpptr->wlpd_p->pPhysCmdBuf);
	WLDBG_EXIT_INFO(DBG_LEVEL_2, NULL);
	return FAIL;
}

int wlDeinit(struct net_device *netdev)
{
#ifdef MBSS
	void wlDeinit_mbss(struct net_device *netdev);
#endif
#ifdef CLIENT_SUPPORT
	extern void wlDeinit_client(struct net_device *netdev);
#endif /* CLIENT_SUPPORT */
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);

	WLDBG_ENTER(DBG_LEVEL_2);
#ifdef AP8X_STATISTICS
	ap8x_stat_proc_unregister(netdev);
#endif
#ifdef AP8X_DUMP
	ap8x_dump_proc_unregister(netdev);
#endif

	del_timer(&wlpptr->wlpd_p->Timer);
	{
		int stream;
		for (stream=0; stream<MAX_SUPPORT_AMPDU_TX_STREAM;stream++)	
			TimerRemove(&wlpptr->wlpd_p->Ampdu_tx[stream].timer);
	}
#ifdef MRVL_DFS
	DfsDeInit(wlpptr->wlpd_p);
#endif

	SendResetCmd(wlpptr->vmacSta_p, 1);
	if (netdev->flags & IFF_RUNNING)
	{
		if (wlstop(netdev))
		{
			printk(KERN_ERR "%s: failed to stop device\n", DRV_NAME);
		}
	}
#ifdef SINGLE_DEV_INTERFACE
#ifdef WDS_FEATURE
	wds_wlDeinit(netdev);
#endif
#endif
#ifdef MBSS
	wlDeinit_mbss(netdev);
#endif
#ifdef CLIENT_SUPPORT
	wlDeinit_client(netdev);
#endif /* CLIENT_SUPPORT */
#ifdef WL_KERNEL_26
#ifdef USE_TASKLET
#ifndef NAPI
	tasklet_kill(&wlpptr->wlpd_p->rxtask);
#endif
	tasklet_kill(&wlpptr->wlpd_p->txtask);
#endif
	flush_scheduled_work();
#else
	run_task_queue(&tq_immediate);
	run_task_queue(&tq_disk);
#endif
	wlInterruptDisable(netdev);
	wlFwReset(netdev);
	wlRxRingCleanup(netdev);
	wlRxRingFree(netdev);
	wlTxRingCleanup(netdev);
	wlTxRingFree(netdev);
	DisableMacMgmtTimers(wlpptr->vmacSta_p);
	MacMgmtMemCleanup(wlpptr->vmacSta_p);
	wlDestroySysCfg(wlpptr->vmacSta_p);
	unregister_netdev(netdev);
#ifdef AP8X_STATISTICS
	if(wlinitcnt == 1)//last one
		ap8x_remove_folder();
#endif
	wlinitcnt--;
	pci_free_consistent(wlpptr->pPciDev, CMD_BUF_SIZE,
		(caddr_t) wlpptr->pCmdBuf, wlpptr->wlpd_p->pPhysCmdBuf);
	//	free(wlpptr->wlpd_p);
	WLDBG_EXIT(DBG_LEVEL_2);
	return SUCCESS;
}
#ifndef NAPI
static void wlRecvHdlr(struct net_device *netdev)
{
#ifdef WL_KERNEL_26
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
#ifdef USE_TASKLET
	tasklet_schedule(&wlpptr->wlpd_p->rxtask);
#else
	schedule_work(&wlpptr->wlpd_p->rxtask);
#endif
#else
	static struct tq_struct rxtask;
	struct tq_struct *ptask=NULL;
	extern void wlRecv(struct net_device *netdev);

	rxtask.routine = (void *)wlRecv;
	rxtask.data = (void *) netdev;
	ptask=&rxtask;

	queue_task(ptask,&tq_immediate);

	if(in_interrupt())
	{
		mark_bh(IMMEDIATE_BH);
	}
#endif
	return;
}
#endif

#ifdef MRVL_DFS
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

	toSmeMsg->vmacSta_p = wlpptr->vmacSta_p ;

	toSmeMsg->MsgType = SME_NOTIFY_RADAR_DETECTION_IND;

	toSmeMsg->Msg.RadarDetectionInd.chInfo.channel = PhyDSSSTable->CurrChan ;
	memcpy(&toSmeMsg->Msg.RadarDetectionInd.chInfo.chanflag ,
		&PhyDSSSTable->Chanflag, sizeof(CHNL_FLAGS));

	smeQ_MgmtWriteNoBlock(toSmeMsg);
	kfree((UINT8 *)toSmeMsg);

	return 0 ;
}

static void radarDetectionHdlr(struct net_device *netdev)
{
#ifdef WL_KERNEL_26
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	schedule_work(&wlpptr->wlpd_p->dfstask);
#else
	static struct tq_struct dfstask;
	struct tq_struct *ptask=NULL;

	dfstask.routine = (void *)wlRadarDetection;
	dfstask.data = (void *) netdev;
	ptask=&dfstask;

	queue_task(ptask,&tq_immediate);

	if(in_interrupt())
	{
		mark_bh(IMMEDIATE_BH);
	}
#endif
	return;
}

int wlApplyCSAChannel(struct net_device *netdev)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	vmacApInfo_t *syscfg = (vmacApInfo_t *)wlpptr->vmacSta_p;
	MIB_802DOT11 *mib = syscfg->Mib802dot11;
	MIB_PHY_DSSS_TABLE *PhyDSSSTable=mib->PhyDSSSTable;

	ApplyCSAChannel( netdev, PhyDSSSTable->CurrChan );

	return 0 ;
}

static void dfsChanSwitchHdlr(struct net_device *netdev)
{
#ifdef WL_KERNEL_26
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	schedule_work(&wlpptr->wlpd_p->csatask);
#else
	static struct tq_struct csatask;
	struct tq_struct *ptask=NULL;

	csatask.routine = (void *)wlApplyCSAChannel;
	csatask.data = (void *) netdev;
	ptask=&csatask;

	queue_task(ptask,&tq_immediate);

	if(in_interrupt())
	{
		mark_bh(IMMEDIATE_BH);
	}
#endif
	return;
}
#endif //MRVL_DFS


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
		wlFwGetConsecTxFailAddr(netdev, (IEEEtypes_MacAddr_t *)addr);
		extStaDb_RemoveStaNSendDeauthMsg(syscfg,(IEEEtypes_MacAddr_t *)addr);
	}

	return 0 ;
}


/*Event handler to kick out client when consecutive tx failure count > limit*/
static void ConsecTxFailHdlr(struct net_device *netdev)
{
#ifdef WL_KERNEL_26
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	schedule_work(&wlpptr->wlpd_p->kickstatask);

#else
	static struct tq_struct kickstatask;
	struct tq_struct *ptask=NULL;

	kickstatask.routine = (void *)wlConsecTxFail;
	kickstatask.data = (void *) netdev;
	ptask=&kickstatask;

	queue_task(ptask,&tq_immediate);

	if(in_interrupt())
	{
		mark_bh(IMMEDIATE_BH);
	}
#endif
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
#ifdef WL_KERNEL_26
	irqreturn_t retVal = IRQ_NONE;
#else
	int retVal = IRQ_NONE;
#endif
#ifdef NAPI
	unsigned int mask;
#endif	
	do
	{
		intStatus = (readl(wlpptr->ioBase1 + MACREG_REG_A2H_INTERRUPT_CAUSE));
#ifdef NAPI
		mask = (readl(wlpptr->ioBase1 + MACREG_REG_A2H_INTERRUPT_MASK));
#endif
		if (intStatus != 0x00000000)
		{
			if (intStatus == 0xffffffff)
			{
				WLDBG_INFO(DBG_LEVEL_2, "card plugged out???");
				retVal = IRQ_HANDLED;
				break; /* card plugged out -> do not handle any IRQ */
			}
#ifdef NAPI
			intStatus &= mask;
#endif
			writel((MACREG_A2HRIC_BIT_MASK & ~intStatus),
				wlpptr->ioBase1 + MACREG_REG_A2H_INTERRUPT_CAUSE);
		}
		if ((intStatus & ISR_SRC_BITS) || (currIteration < MAX_ISR_ITERATION))
		{
			/* Eliminate txdone interrupt handling within ISR to reduce cpu util.
			 * MACREG_A2HRIC_BIT_MASK change, wlTxDone is now executed within transmit path 
 			 if (intStatus & MACREG_A2HRIC_BIT_TX_DONE)
			{
				intStatus &= ~MACREG_A2HRIC_BIT_TX_DONE;
				wlTxDone(netdev);
				retVal = IRQ_HANDLED;
			}*/

			if (intStatus & MACREG_A2HRIC_BIT_RX_RDY)
			{
				intStatus &= ~MACREG_A2HRIC_BIT_RX_RDY;
#ifdef NAPI
				if(netdev->flags & IFF_RUNNING)
				{
					wlInterruptMask(netdev, MACREG_A2HRIC_BIT_RX_RDY);
					napi_schedule(&wlpptr->napi);
				}
#else
				wlRecvHdlr(netdev);
#endif
				retVal = IRQ_HANDLED;
			}
			if (intStatus & MACREG_A2HRIC_BIT_OPC_DONE)
			{
				intStatus &= ~MACREG_A2HRIC_BIT_OPC_DONE;
				wlFwCmdComplete(netdev);
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
				MrvlICVErrorHdl(vmacSta_p);
				intStatus &= ~MACREG_A2HRIC_BIT_ICV_ERROR;
				retVal = IRQ_HANDLED;
			}
			if (intStatus & MACREG_A2HRIC_BIT_WEAKIV_ERROR)
			{
				MIB_802DOT11 *mib = wlpptr->vmacSta_p->ShadowMib802dot11;
				intStatus &= ~MACREG_A2HRIC_BIT_WEAKIV_ERROR;

				wlpptr->wlpd_p->privStats.weakiv_count++;
				wlpptr->wlpd_p->privStats.weakiv_threshold_count++;

				if ((wlpptr->wlpd_p->privStats.weakiv_threshold_count) >= *(mib->mib_weakiv_threshold)) {
					wlpptr->wlpd_p->privStats.weakiv_threshold_count = 0;
					WLSYSLOG(netdev, WLSYSLOG_CLASS_ALL, WLSYSLOG_MSG_WEP_WEAKIV_ERROR);
					WLSNDEVT(netdev,IWEVCUSTOM, (IEEEtypes_MacAddr_t *)&wlpptr->hwData.macAddr[0], WLSYSLOG_MSG_WEP_WEAKIV_ERROR);
				}
				retVal = IRQ_HANDLED;
			}
			if (intStatus & MACREG_A2HRIC_BIT_QUEUE_EMPTY)
			{
				intStatus &= ~MACREG_A2HRIC_BIT_QUEUE_EMPTY;
				if(extStaDb_AggrFrameCk(vmacSta_p, 1))
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
#ifdef IEEE80211_DH
			if (intStatus & MACREG_A2HRIC_BIT_RADAR_DETECT)
			{
				intStatus &= ~MACREG_A2HRIC_BIT_RADAR_DETECT;
				WLSYSLOG(netdev, WLSYSLOG_CLASS_ALL, WLSYSLOG_MSG_GEN_RADARDETECTION);
				WLSNDEVT(netdev,IWEVCUSTOM, (IEEEtypes_MacAddr_t *)&wlpptr->hwData.macAddr[0], WLSYSLOG_MSG_GEN_RADARDETECTION);
#ifdef MRVL_DFS
				radarDetectionHdlr(netdev);
#endif
				retVal = IRQ_HANDLED;
			}
			if (intStatus & MACREG_A2HRIC_BIT_CHAN_SWITCH)
			{
				intStatus &= ~MACREG_A2HRIC_BIT_CHAN_SWITCH;
#ifdef MRVL_DFS
				dfsChanSwitchHdlr(netdev);
#endif
				retVal = IRQ_HANDLED;
			}
#endif //IEEE80211_DH
			if(intStatus & MACREG_A2HRIC_BIT_TX_WATCHDOG)
			{
				intStatus &= ~MACREG_A2HRIC_BIT_TX_WATCHDOG;
				wlpptr->netDevStats.tx_heartbeat_errors++;
				wlResetTask(netdev);
				retVal = IRQ_HANDLED;
			}
#if defined(AMPDU_SUPPORT_SBA) || (BA_WATCHDOG)
			if (intStatus & MACREG_A2HRIC_BA_WATCHDOG)
			{
#ifdef SOC_W8864
#define BA_STREAM 4
#else
#define BA_STREAM 5
#endif
#define INVALID_WATCHDOG 0xAA
				u_int8_t bitmap=0xAA,stream=0;
				intStatus &= ~MACREG_A2HRIC_BA_WATCHDOG;
				wlFwGetWatchdogbitmap(netdev,&bitmap);
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
							disableAmpduTxstream(vmacSta_p,stream);
						}
					}
					else
						disableAmpduTxAll(vmacSta_p);

				}
				retVal = IRQ_HANDLED;
			}
#endif /* _AMPDU_SUPPORT_SBA */
#ifdef SSU_SUPPORT
			if(intStatus & MACREG_A2HRIC_BIT_SSU_DONE)
			{
                static UINT32 ssu_counter = 0;
				intStatus &= ~MACREG_A2HRIC_BIT_SSU_DONE;
				printk("SSU Done counter = %d\n", ssu_counter++);
				retVal = IRQ_HANDLED;
			}
#endif
			
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

#ifdef WL_KERNEL_26
	return retVal;
#else
	return;
#endif
}

void wlInterruptEnable(struct net_device *netdev)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);

	if (wlChkAdapter(netdev))
	{
		writel(0x00, wlpptr->ioBase1 + MACREG_REG_A2H_INTERRUPT_MASK);

		writel((MACREG_A2HRIC_BIT_MASK), 
			wlpptr->ioBase1 + MACREG_REG_A2H_INTERRUPT_MASK);
	}
}

void wlInterruptDisable(struct net_device *netdev)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);

	if (wlChkAdapter(netdev))
	{
		writel(0x00, wlpptr->ioBase1 + MACREG_REG_A2H_INTERRUPT_MASK);
	}
}
#ifdef NAPI
void wlInterruptUnMask(struct net_device *netdev, int mask)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);

	if (wlChkAdapter(netdev))
	{
		writel((readl(wlpptr->ioBase1 + MACREG_REG_A2H_INTERRUPT_MASK) | (mask)), 
			wlpptr->ioBase1 + MACREG_REG_A2H_INTERRUPT_MASK);
	}
}

void wlInterruptMask(struct net_device *netdev, int mask)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);

	if (wlChkAdapter(netdev))
	{
		writel((readl(wlpptr->ioBase1 + MACREG_REG_A2H_INTERRUPT_MASK)& (~mask)), 
			wlpptr->ioBase1 + MACREG_REG_A2H_INTERRUPT_MASK);
	}
}
#endif

void wlFwReset(struct net_device *netdev)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);

	if (wlChkAdapter(netdev))
	{
		writel(ISR_RESET, wlpptr->ioBase1 + MACREG_REG_H2A_INTERRUPT_EVENTS);
	}
}

int wlChkAdapter(struct net_device *netdev)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	u_int32_t regval;

	regval = readl(wlpptr->ioBase1 + MACREG_REG_INT_CODE);
	if (regval == 0xffffffff)
	{
#ifdef SOC_W8764
		printk(" wlChkAdapter FALSE  regval = %x \n", regval);
#endif
		return FALSE;
	}
	return TRUE;
}

#ifdef WFA_TKIP_NEGATIVE
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
#ifdef SOC_W8864		
		|| *(mib->mib_ApMode) == AP_MODE_2_4GHZ_11AC_MIXED
		|| *(mib->mib_ApMode) == AP_MODE_5GHZ_11AC_ONLY
		|| *(mib->mib_ApMode) == AP_MODE_5GHZ_Nand11AC
#endif		
		|| *(mib->mib_ApMode) == AP_MODE_AandN)) /*WPA-TKIP or WPA-AES mode */
	{
		printk("HT mode not supported when WPA is enabled\n");
		WLSYSLOG(netdev, WLSYSLOG_CLASS_ALL, "HT mode not supported when WPA is enabled\n");
		WLSNDEVT(netdev, IWEVCUSTOM,(IEEEtypes_MacAddr_t *)&wlpptr->hwData.macAddr[0], "HT mode not supported when WPA is enabled\n");

		WLDBG_EXIT_INFO(DBG_LEVEL_0, "settings not valid");
		retval = FAIL;
	}

	return retval;
}
#endif    

/** private functions **/

static int wlopen(struct net_device *netdev)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	vmacApInfo_t *vmacSta_p = wlpptr->vmacSta_p;		
	
	WLDBG_ENTER(DBG_LEVEL_2);
#ifdef CLIENTONLY
	//this will handle ifconfig down/up case
	wlpptr->wlreset(netdev);
	WL_MOD_INC_USE(THIS_MODULE, return -EIO);
	return 0;
#else
	memset(&wlpptr->netDevStats, 0x00, sizeof(struct net_device_stats));
	memset(&wlpptr->wlpd_p->privStats, 0x00, sizeof(struct wlpriv_stats));

	netdev->type = ARPHRD_ETHER;

	if (netdev->flags & IFF_RUNNING)
	{
		vmacSta_p->InfUpFlag = 0; 	
		netif_stop_queue(netdev);
		netdev->flags &= ~IFF_RUNNING;
		wlInterruptDisable(netdev);
	}
#ifdef WFA_TKIP_NEGATIVE
	if (wlValidateSettings(netdev))
		return -EIO;
#endif

	if(wlFwApplySettings(netdev))
		return -EIO;

	wlInterruptEnable(netdev);
	netif_wake_queue(netdev);  
	vmacSta_p->InfUpFlag = 1; 	
	netdev->flags |= IFF_RUNNING;

#ifdef AUTOCHANNEL
	scanControl(wlpptr->vmacSta_p);
#endif

	/* Initialize the STADB timers */    
	if(wlpptr->vmacSta_p->master== NULL)
	{
		extStaDb_AgingTimerInit(wlpptr->vmacSta_p);
		extStaDb_ProcessKeepAliveTimerInit(wlpptr->vmacSta_p);
	}

	WL_MOD_INC_USE(THIS_MODULE, return -EIO);
	WLDBG_EXIT(DBG_LEVEL_2);
#ifdef NAPI
	napi_enable(&wlpptr->napi);
#endif
	return 0;
#endif
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
		if (wlFwSetAPBss(netdev, WL_DISABLE))
		{
			WLDBG_WARNING(DBG_LEVEL_2, "disabling AP bss failed");
		}
		if (wlFwSetRadio(netdev, WL_DISABLE, WL_AUTO_PREAMBLE))
		{
			WLDBG_WARNING(DBG_LEVEL_2, "disabling rf failed");
		}
		wlInterruptDisable(netdev);
	}

#ifdef NAPI
	napi_disable(&wlpptr->napi);
#endif
	WL_MOD_DEC_USE(THIS_MODULE);
	WLDBG_EXIT(DBG_LEVEL_2);
	return 0;
}

static void wlsetMcList(struct net_device *netdev)
{
	//	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);

	WLDBG_ENTER(DBG_LEVEL_2);
	WLDBG_EXIT(DBG_LEVEL_2);
}
static struct net_device_stats *	wlgetStats(struct net_device *netdev)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);

	WLDBG_ENTER(DBG_LEVEL_2);

	WLDBG_EXIT(DBG_LEVEL_2);
	return &(wlpptr->netDevStats);
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

#ifdef MRVL_DFS
	DfsCmd_t    dfsCmd ;
#endif
#if defined( WDS_FEATURE) || defined (MBSS)
	int i;
	char dev_running[MAX_VMAC_INSTANCE_AP+1];
#endif
#ifdef AUTOCHANNEL
	{
		Disable_ScanTimerProcess(vmacSta_p);
		vmacSta_p->busyScanning = 0;
		Disable_extStaDb_ProcessKeepAliveTimer(vmacSta_p);
		Disable_MonitorTimerProcess(vmacSta_p);
	}
#endif
	vmacSta_p->download = TRUE;
	WLDBG_ENTER(DBG_LEVEL_2);

	if (wlpptr->wlpd_p->inReset)
	{
		return 0;
	} else
	{
		wlpptr->wlpd_p->inReset = WL_TRUE;
	}
#if	defined(CLIENT_SUPPORT)
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
#endif
	if(netdev->flags & IFF_RUNNING)
	{
		vmacSta_p->InfUpFlag = 0; 	
		netif_stop_queue(netdev);
		netdev->flags &= ~IFF_RUNNING;
#ifdef SINGLE_DEV_INTERFACE
#ifdef WDS_FEATURE
		// Stop any wds port queues that are active.
		for (i = 0; i < 6; i++)
		{
			if (wdsPortActive(i))
			{
				vmacSta_p->InfUpFlag = 0; 	
				netif_stop_queue(wlpptr->wlpd_p->wdsPort[i].netDevWds);
				wlpptr->wlpd_p->wdsPort[i].netDevWds->flags &= ~IFF_RUNNING;
			}
		}
#endif
#endif
	}
#if defined(MBSS)
	i = 0;
	while( i <=MAX_VMAC_INSTANCE_AP )
	{
		//remember the interface up/down status, and bring down it.
		if(wlpptr->vdev[i]){
			dev_running[i] = 0;
			if ((NETDEV_PRIV_P(struct wlprivate, wlpptr->vdev[i]))->vmacSta_p->OpMode == WL_OP_MODE_AP 
				||((NETDEV_PRIV_P(struct wlprivate, wlpptr->vdev[i]))->vmacSta_p->OpMode == WL_OP_MODE_VAP))
			{
				if(wlpptr->vdev[i]->flags & IFF_RUNNING)
					dev_running[i] = 1;
				wlstop_mbss(wlpptr->vdev[i]);
			}
			if ((NETDEV_PRIV_P(struct wlprivate, wlpptr->vdev[i]))->vmacSta_p->OpMode == WL_OP_MODE_VSTA)
			{
				if (wlpptr->vdev[i]->flags & IFF_RUNNING)
				{
					dev_running[i] = 1;
					wlstop_client(wlpptr->vdev[i]);
				}
			}
		}
		i++;
	}
#endif
	if (wlFwSetAPBss(netdev, WL_DISABLE))
	{
		WLDBG_EXIT_INFO(DBG_LEVEL_2, "disable AP bss failed");
		//if fw stop responding, do not block fw download
		//goto err_fw_cmd;
	}
	if (wlFwSetRadio(netdev, WL_DISABLE, WL_AUTO_PREAMBLE))
	{
		WLDBG_EXIT_INFO(DBG_LEVEL_2, "disable rf failed");
		//if fw stop responding, do not block fw download
		//goto err_fw_cmd;
	}
	wlInterruptDisable(netdev);
#ifndef COMMIT_NO_RELOAD
#ifndef SOC_W8764
	if (wlreloadFirmwareAndInitDescriptors(netdev))
	{
		WLDBG_EXIT_INFO(DBG_LEVEL_2, "loading firmware failed");
		goto err_fw_cmd;
	}
#endif
#endif
#ifdef SINGLE_DEV_INTERFACE
#ifdef WDS_FEATURE
	// Restart any wds port queues that are active.
	for (i = 0; i < 6; i++)
	{
		if (wdsPortActive(i))
		{                
			vmacSta_p->InfUpFlag = 1; 	
			netif_wake_queue(wlpptr->wlpd_p->wdsPort[i].netDevWds);
			wlpptr->wlpd_p->wdsPort[i].netDevWds->flags |= IFF_RUNNING;
		}
	}
#endif
#endif
	netif_wake_queue(netdev);  /* restart Q if interface was running */
	vmacSta_p->InfUpFlag = 1;	
	netdev->flags |= IFF_RUNNING;

	if(wlFwApplySettings(netdev))
		return -EIO;

	wlInterruptEnable(netdev);

	WLDBG_EXIT(DBG_LEVEL_2);
	wlpptr->vmacSta_p->download = FALSE;
	wlpptr->wlpd_p->inReset = WL_FALSE;
#ifdef MFG_SUPPORT
	if (wlpptr->mfgEnable) {
		return 0;
	}
#endif
#ifdef AUTOCHANNEL
	scanControl(wlpptr->vmacSta_p);
#endif
#if defined(MBSS)
	i = 0;
	while( i <=MAX_VMAC_INSTANCE_AP )
	{
		//bring the vitual interface back if it brought down the routine
		if(wlpptr->vdev[i]){
			if ((NETDEV_PRIV_P(struct wlprivate, wlpptr->vdev[i]))->vmacSta_p->OpMode == WL_OP_MODE_AP ||
				((NETDEV_PRIV_P(struct wlprivate, wlpptr->vdev[i]))->vmacSta_p->OpMode == WL_OP_MODE_VAP))
			{
				if(dev_running[i])
				{
					wlreset_mbss(wlpptr->vdev[i]);
				}
			}
			if ((NETDEV_PRIV_P(struct wlprivate, wlpptr->vdev[i]))->vmacSta_p->OpMode == WL_OP_MODE_VSTA)
			{
				if (dev_running[i])
				{
					wlreset_client(wlpptr->vdev[i]);
				}
			}
		}
		i++;
	}
#endif
	wlpptr->wlpd_p->inResetQ = WL_FALSE;
#ifdef MRVL_DFS
	/* Send the reset message to
	* the DFS event dispatcher
	*/
	dfsCmd.CmdType = DFS_CMD_WL_RESET ;
	evtDFSMsg( netdev, (UINT8 *)&dfsCmd );

#endif
	wlpptr->wlpd_p->BcnAddHtOpMode = 0;
	wlpptr->wlpd_p->TxGf = 0;


#ifdef COEXIST_20_40_SUPPORT
	{
		MIB_802DOT11 *mib =  vmacSta_p->ShadowMib802dot11;
		MIB_PHY_DSSS_TABLE *PhyDSSSTable=mib->PhyDSSSTable;
		extern int wlFwSet11N_20_40_Switch(struct net_device *netdev, UINT8 mode);
		extern void  Check20_40_Channel_switch(int option, int * mode);
		extern void Disable_StartCoexisTimer(vmacApInfo_t *vmacSta_p);



		if((*(mib->USER_ChnlWidth )&0xf0) && ((*(vmacSta_p->Mib802dot11->mib_ApMode)&AP_MODE_BAND_MASK) < AP_MODE_A_ONLY))
		{
			wlFwSet11N_20_40_Switch(vmacSta_p->dev, 0);
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
				Disable_StartCoexisTimer(vmacSta_p);

			}




	}
#endif

	return 0;
/*
err_fw_cmd:
	wlpptr->vmacSta_p->download = FALSE;
	wlpptr->wlpd_p->inReset = WL_FALSE;
	wlpptr->wlpd_p->inResetQ = WL_FALSE;
	return -EFAULT;*/
}

static void wltxTimeout(struct net_device *netdev)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);

	WLDBG_ENTER(DBG_LEVEL_2);

	if (wlpptr->wlpd_p->inReset)
	{
		return;
	}
#ifdef MRVL_DFS
	if ((netdev->flags & IFF_RUNNING) == 0 )
	{
		return ;
	}
#endif

	wlpptr->wlpd_p->isTxTimeout = WL_TRUE;
	wlpptr->wlreset(netdev);
	wlpptr->wlpd_p->isTxTimeout = WL_FALSE;
	WLDBG_EXIT(DBG_LEVEL_2);
}

#ifndef SOC_W8764
static int wlreloadFirmwareAndInitDescriptors(struct net_device *netdev)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	int i;
	WLDBG_ENTER(DBG_LEVEL_2);

	wlFwReset(netdev);

	writel(0x00000000, wlpptr->ioBase0 + wlpptr->wlpd_p->descData[0].wcbBase);
#if NUM_OF_DESCRIPTOR_DATA >3
	for (i = 1; i < TOTAL_TX_QUEUES; i++)
		writel(0x00000000, wlpptr->ioBase0 + wlpptr->wlpd_p->descData[i].wcbBase);
#endif
	writel(0x00000000, wlpptr->ioBase0 + wlpptr->wlpd_p->descData[0].rxDescRead);
	writel(0x00000000, wlpptr->ioBase0 + wlpptr->wlpd_p->descData[0].rxDescWrite);

	wlRxRingCleanup(netdev);
	wlRxRingFree(netdev);
	wlTxRingCleanup(netdev);
	wlTxRingFree(netdev);
#ifndef SOC_W8764
	if (wlFwDownload(netdev))
	{
		WLDBG_ERROR(DBG_LEVEL_2, "download firmware failed");
		return FAIL;
	}
#endif
	if (wlTxRingAlloc(netdev) == 0)
	{
		if (wlTxRingInit(netdev) != 0)
		{
			wlTxRingFree(netdev);
			WLDBG_ERROR(DBG_LEVEL_2, "init TX failed");
			return FAIL;
		}
	} else
	{
		WLDBG_ERROR(DBG_LEVEL_2, "allocate TX failed");
		return FAIL;
	}

	if (wlRxRingAlloc(netdev) == 0)
	{
		if (wlRxRingInit(netdev) != 0)
		{
			wlRxRingFree(netdev);
			wlTxRingFree(netdev);
			WLDBG_ERROR(DBG_LEVEL_2, "init RX failed");
			return FAIL;
		}
	} else
	{
		WLDBG_ERROR(DBG_LEVEL_2, "allocate RX failed");
		return FAIL;
	}
#ifdef MFG_SUPPORT
	if (!wlpptr->mfgEnable)
#endif
	{
		if (wlFwGetHwSpecs(netdev))
		{
			wlRxRingCleanup(netdev);
			wlRxRingFree(netdev);
			wlTxRingCleanup(netdev);
			wlTxRingFree(netdev);
			WLDBG_ERROR(DBG_LEVEL_2, "failed to get HW specs");
			return FAIL;
		}
	}

	writel((wlpptr->wlpd_p->descData[0].pPhysTxRing),
		wlpptr->ioBase0 + wlpptr->wlpd_p->descData[0].wcbBase);
#if NUM_OF_DESCRIPTOR_DATA >3
	for (i = 1; i < TOTAL_TX_QUEUES; i++)
	writel((wlpptr->wlpd_p->descData[i].pPhysTxRing),
		wlpptr->ioBase0 + wlpptr->wlpd_p->descData[i].wcbBase);
#endif
	writel((wlpptr->wlpd_p->descData[0].pPhysRxRing),
		wlpptr->ioBase0 + wlpptr->wlpd_p->descData[0].rxDescRead);
	writel((wlpptr->wlpd_p->descData[0].pPhysRxRing),
		wlpptr->ioBase0 + wlpptr->wlpd_p->descData[0].rxDescWrite);
	if (wlFwSetHwSpecs(netdev))
	{
		WLDBG_ERROR(DBG_LEVEL_2, "failed to set HW specs");
	}

	WLDBG_EXIT(DBG_LEVEL_2);
	return SUCCESS;
}
#endif

void wlSendEvent(struct net_device *dev, int cmd, IEEEtypes_MacAddr_t *Addr, const char * info)
{
	union iwreq_data wrqu;
	char buf[128];

	memset(&wrqu, 0, sizeof(wrqu));

	if (cmd == IWEVCUSTOM)
	{
		snprintf(buf, sizeof(buf), "%02x:%02x:%02x:%02x:%02x:%02x:%s",
			*((unsigned char*)Addr), *((unsigned char*)Addr+1), *((unsigned char*)Addr + 2), 
			*((unsigned char*)Addr+3), *((unsigned char*)Addr+4), *((unsigned char*)Addr + 5),info);
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
#ifdef WDS_FEATURE
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
		//setWdsPeerInfo(&wlpptr->vmacSta_p->wdsPeerInfo[i], AP_MODE_G_ONLY); // Set to default G.

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

int	wlStop_wdsDevs(struct wlprivate *wlpptr)
{
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

	//	wlfacilitate_e radioOnOff = WL_ENABLE ;

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

int wlstop_wds(struct net_device *netdev)
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

	if (wlpptr->wlpd_p->inReset)
	{
		return;
	}

	wlreset_wds(netdev);
	WLDBG_EXIT(DBG_LEVEL_2);
}


static int wlsetMacAddr_wds(struct net_device *netdev, void *addr)
{
	//	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	struct sockaddr *macAddr = (struct sockaddr *) addr;

	WLDBG_ENTER(DBG_LEVEL_2);
	if (is_valid_ether_addr(macAddr->sa_data))
	{ 
		//memcpy(netdev->dev_addr, addr, 6);
		setWdsPortMacAddr(netdev, (UINT8 *) addr);
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

	if (wlpptr->wlpd_p->inReset)
	{
		return 0;
	} 
    disableAmpduTxAll( wlpptr->vmacSta_p );

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

#endif

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

#ifdef WFA_TKIP_NEGATIVE
	if (wlValidateSettings(netdev))
		return -EIO;
#endif

	wlFwMultiBssApplySettings(netdev);
	if(wlpptr->master)
	{
		//set wdev0 OpMode to follow wdev0apX's opmode
		vmacSta_p->master->OpMode = vmacSta_p->OpMode;
	}

	netif_wake_queue(netdev);  /* Start/Restart Q if stopped. */
	vmacSta_p->InfUpFlag = 1;		
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
#ifdef WDS_FEATURE
	wlStop_wdsDevs(wlpptr);
#endif
	SendResetCmd(wlpptr->vmacSta_p, 0);
	if (netdev->flags & IFF_RUNNING)
	{
		if (wlFwSetAPBss(netdev, WL_DISABLE_VMAC))
		{
			WLDBG_EXIT_INFO(DBG_LEVEL_2, "disable AP bss failed");
		}
		netif_stop_queue(netdev);
		netdev->flags &= ~IFF_RUNNING;
		WL_MOD_DEC_USE(THIS_MODULE);
	}
	DisableMacMgmtTimers(wlpptr->vmacSta_p);
	WLDBG_INFO(DBG_LEVEL_2, "Stop mbss name = %s \n", netdev->name);

	WLDBG_EXIT(DBG_LEVEL_2);
	return 0;
}
int wlreset_mbss(struct net_device *netdev)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	vmacApInfo_t *vmacSta_p = wlpptr->vmacSta_p;		

	WLDBG_ENTER(DBG_LEVEL_2);

	if (wlpptr->wlpd_p->inReset)
	{
		return 0;
	} 
#ifdef WDS_FEATURE
	{
		int i;
		// Stop any wds port queues that are active.
		for (i = 0; i < 6; i++)
		{
			if (wdsPortActive(netdev, i))
			{
				vmacSta_p->InfUpFlag = 0;		
				netif_stop_queue(wlpptr->vmacSta_p->wdsPort[i].netDevWds);
				wlpptr->vmacSta_p->wdsPort[i].netDevWds->flags &= ~IFF_RUNNING;
			}
		}
	}
#endif

	if(netdev->flags & IFF_RUNNING)
	{
		vmacSta_p->InfUpFlag = 0;		
		if (wlFwSetAPBss(netdev, WL_DISABLE_VMAC))
		{
			WLDBG_EXIT_INFO(DBG_LEVEL_2, "disable AP bss failed");
		}
		netif_stop_queue(netdev);
		netdev->flags &= ~IFF_RUNNING;
	}
	wlFwMultiBssApplySettings(netdev);

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

	if (wlpptr->wlpd_p->inReset)
	{
		return;
	}

	wlreset_mbss(netdev);
	WLDBG_EXIT(DBG_LEVEL_2);
}


static int wlsetMacAddr_mbss(struct net_device *netdev, void *addr)
{
	//	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
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
	//	int retCode;
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
#if LINUX_VERSION_CODE >=KERNEL_VERSION(2,6,25)
	wlpptr->netDev = dev;
#else
	wlpptr->netDev = &wlpptr->netDevPriv;
#ifdef WL_KERNEL_26
	memset(wlpptr->netDev, 0, sizeof(struct net_device));
#endif
#endif

	//from probe
	wlpptr->ioBase0 = wlp->ioBase0;
	wlpptr->ioBase1 = wlp->ioBase1;

	//sprintf(wlpptr->netDev->name, "ap%1d", wlp->wlpd_p->vmacIndex);
	sprintf(wlpptr->netDev->name, DRV_NAME_VMAC, wlinitcnt,  wlp->wlpd_p->vmacIndex);
	wlpptr->netDev->irq	   = wlp->netDev->irq;
	wlpptr->netDev->mem_start = wlp->netDev->mem_start;
	wlpptr->netDev->mem_end	 = wlp->netDev->mem_end;
	NETDEV_PRIV_S( wlpptr->netDev) 	 = wlpptr;
	//	wlpptr->pPciDev 		 = wlp->pPciDev;
#ifdef WL_KERNEL_26
	SET_MODULE_OWNER(*(wlpptr->netDev));
#endif

	//	pci_set_drvdata(wlpptr->pPciDev, (wlpptr->netDev));

	//from init
	memcpy(wlpptr->netDev->dev_addr, &macAddr[0], 6);
	memcpy(&wlpptr->hwData.macAddr[0], &macAddr[0], 6);
	wlpptr->vmacSta_p = Mac_Init((void *)wlp, wlpptr->netDev, &macAddr[0], WL_OP_MODE_VAP, wlinitcnt);
	if(wlpptr->vmacSta_p == NULL)
	{
		printk(KERN_ERR  "%s: failed to init driver mac\n", wlpptr->netDev->name);
		goto err_out;
	}
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

#ifdef WLAN_INCLUDE_TSO
	wlpptr->netDev->features |= NETIF_F_TSO;
	wlpptr->netDev->features |= NETIF_F_IP_CSUM;
	wlpptr->netDev->features |= NETIF_F_SG;
#endif
	wlpptr->wlreset = wlreset_mbss;
#ifdef WL_KERNEL_26
	wlSetupWEHdlr(wlpptr->netDev);
#endif
	wlpptr->wlpd_p = wlp->wlpd_p;
	wlpptr->master= wlp->netDev;
	wlp->vdev[wlp->wlpd_p->vmacIndex++] = wlpptr->netDev;
#if LINUX_VERSION_CODE >=KERNEL_VERSION(3,0,6)
#else
	atomic_set(&wlpptr->netDev->refcnt, 0);
#endif
	ether_setup(wlpptr->netDev);
	if (register_netdev(wlpptr->netDev))
	{
		printk("%s: failed to register device\n", wlpptr->netDev->name);
		goto err_register_netdev;
	}
#ifdef AP8X_STATISTICS
	ap8x_stat_proc_register(wlpptr->netDev);
#endif
	wlpptr->netDev->mtu = wlp->netDev->mtu;
	memcpy(wlpptr->netDev->dev_addr, macAddr, 6);
#ifdef WDS_FEATURE
	wlInit_wds(wlpptr);
#endif
	WLDBG_EXIT(DBG_LEVEL_2);	
	return 0;
err_out:
err_register_netdev:
	free(wlpptr);
	WLDBG_EXIT(DBG_LEVEL_2);
	return -EIO;
}

void wlDeinit_mbss(struct net_device *netdev)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	struct wlprivate *wlp;
	int i;
	WLDBG_ENTER(DBG_LEVEL_2);
	SendResetCmd(wlpptr->vmacSta_p, 1);
#if defined(SINGLE_DEV_INTERFACE) && !defined(CLIENTONLY)
	for (i = 1; i < wlpptr->wlpd_p->vmacIndex; i++)
#else
	for (i = 0; i < wlpptr->wlpd_p->vmacIndex; i++)
#endif
	{
		wds_wlDeinit(wlpptr->vdev[i]);
		if (wlpptr->vdev[i]->flags & IFF_RUNNING)
		{
			if (wlstop_mbss(wlpptr->vdev[i]))
			{
				printk(KERN_ERR "%s: failed to stop device\n",wlpptr->vdev[i]->name );
			}
		}
		wlp = NETDEV_PRIV_P(struct wlprivate, wlpptr->vdev[i]);
		DisableMacMgmtTimers(wlp->vmacSta_p);
		MacMgmtMemCleanup(wlp->vmacSta_p);
		wlDestroySysCfg(wlp->vmacSta_p);
#ifdef AP8X_STATISTICS
		ap8x_stat_proc_unregister(wlpptr->vdev[i]);
#endif
		unregister_netdev(wlpptr->vdev[i]);		
#if LINUX_VERSION_CODE >=KERNEL_VERSION(2,6,25)
		free_netdev(wlpptr->vdev[i]);
#else
		free(wlp);
#endif
	}
	wlpptr->wlpd_p->vmacIndex =0;
	WLDBG_EXIT(DBG_LEVEL_2);
	return;
}

int wlResetTask(struct net_device *dev)
{
#ifdef WL_KERNEL_26
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, dev);
	if(wlpptr->wlpd_p->inResetQ)
		return 0;
	wlpptr->wlpd_p->inResetQ = TRUE;
	schedule_work(&wlpptr->wlpd_p->resettask);
#else
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, dev);
	static struct tq_struct resettask;
	struct tq_struct *ptask=NULL;
	if(wlpptr->wlpd_p->inResetQ)
		return 0;
	wlpptr->wlpd_p->inResetQ = TRUE;
	resettask.routine = (void *)wlpptr->wlreset;
	resettask.data = (void *) dev;
	ptask=&resettask;

	queue_task(ptask,&tq_disk);
#endif
	return 0;
}

#ifdef CLIENT_SUPPORT
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
	if(wlpptr->master)
	{
		/* Get Primary info. */
		wlMPrvPtr = NETDEV_PRIV_P(struct wlprivate, wlpptr->master);        
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
	wlFwSetInfraMode(netdev);        
	Disable_extStaDb_ProcessKeepAliveTimer(primary_vmacSta_p);
	Disable_MonitorTimerProcess(primary_vmacSta_p);

	extStaDb_ProcessKeepAliveTimerInit(primary_vmacSta_p);
	MonitorTimerInit(primary_vmacSta_p);

	PhyDSSSTable=mib->PhyDSSSTable;

	/* Pass the channel list */
	/* if autochannel is enabled then pass in the channel list */
	/* else if autochannel is disabled only pass in a single ch */
	if (*(primary_mib->mib_autochannel))
	{
		/* Stop Autochannel on AP first */
		StopAutoChannel(primary_vmacSta_p);

		/* get range to scan */
		domainGetInfo(mainChnlList);

		if(*(vmacSta_p->Mib802dot11->mib_STAMode) == CLIENT_MODE_AUTO) // ||
			//(*(vmacSta_p->Mib802dot11->mib_STAMode) == CLIENT_MODE_N))
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
#ifdef RSN_RESOLVE
	strncpy((char *)&tmpClientSSID[phyIndex][0], (const char *)&(mib->StationConfig->DesiredSsId[0]), 32);
#endif /* RSN_RESOLVE */
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
#ifdef RSN_RESOLVE
	defaultKeyMgmtInit(phyIndex);
#endif

#ifdef MRVL_WPS_CLIENT
	if( memcmp( mib->StationConfig->DesiredBSSId, "\0x00\0x00\0x00\0x00\0x00\0x00", 6 ))
		memcpy( &tmpClientBSSID[phyIndex][0], &(mib->StationConfig->DesiredBSSId[0]), 6 );
#endif
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
	if(wlpptr->master)
	{
		vmacSta_p = wlpptr->vmacSta_p;
		mib = vmacSta_p->Mib802dot11;
		//set wdev0 OpMode to follow wdev0staX's opmode
		vmacSta_p->master->OpMode = vmacSta_p->OpMode;
	}
	else
	{
		//printk("wlInitClientLink: ERROR -cannot get master mib from netdev = %x \n", netdev); 
		return;
	}

	if (!(*(mib->mib_STAMacCloneEnable) == 1))
	{        
		vmacEntry_t  *vmacEntry_p;
		if((vmacEntry_p = sme_GetParentVMacEntry(vmacSta_p->VMacEntry.phyHwMacIndx)) != NULL)
		{ 
			wlFwSetMacAddr_Client(netdev, &vmacEntry_p->vmacAddr[0]);
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
	struct net_device *dev_p;
	struct wlprivate	*priv;
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
	dev_p = (struct net_device *)vmacEntry_p->privInfo_p;
	priv = NETDEV_PRIV_P(struct wlprivate, dev_p);
	mib = priv->vmacSta_p->Mib802dot11;
	vmacSta_p = priv->vmacSta_p;
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

			wlFwSetAid(dev_p, mlmeBssid, 0);

			printk("**** %s: LINK UP to %02x%02x%02x%02x%02x%02x\n", 
				dev_p->name, mlmeBssid[0],mlmeBssid[1],mlmeBssid[2],mlmeBssid[3],mlmeBssid[4],mlmeBssid[5]);

#ifdef MRVL_WPS_CLIENT
			/* Send event to user space */
			WLSNDEVT(dev_p, IWEVREGISTERED, &mlmeBssid, NULL);
#endif
			WLSYSLOG(dev_p, WLSYSLOG_CLASS_ALL, WLSYSLOG_MSG_CLIENT_CONNECTED "%02x%02x%02x%02x%02x%02x\n",
				mlmeBssid[0],mlmeBssid[1],mlmeBssid[2],mlmeBssid[3],mlmeBssid[4],mlmeBssid[5]);

#ifndef MRVL_WPS_CLIENT
			WLSNDEVT(dev_p, IWEVCUSTOM, &vmacEntry_p->vmacAddr, WLSYSLOG_MSG_CLIENT_CONNECTED);
#endif
			memcpy(priv->hwData.macAddr, mlmeBssid, 6);

			/* If Mac cloneing disabled, set vmacEntry to active here. */
			if (!(*(mib->mib_STAMacCloneEnable) == 1))
				vmacEntry_p->active = 1;

		}
		else
		{
			printk("**** %s: LINK NOT UP\n", dev_p->name);
#ifdef WPA_STA
			/* Verify that Key timer is disabled. */
			sme_DisableKeyMgmtTimer(vmacEntry_p);        
#endif /* WPA_STA */

#if 1 //enable if you want to use link mgt to connect
			/* do not restart linkmgt if user started a scan */
			/* scan complete will trigger a link Mgt restart */
			if (vmacSta_p->gUserInitScan != TRUE)
				linkMgtReStart(vmacEntry_p->phyHwMacIndx, vmacEntry_p);
#endif //end link mgt

			if(*(mib->mib_STAMacCloneEnable) == 2)
				ethStaDb_RemoveAllStns(vmacSta_p);

			/* Remove client and remote ap from Fw and driver databases. */
			RemoveRemoteAPFw((UINT8 *) &mlmeBssid[0],vmacEntry_p);
			WLSYSLOG(dev_p, WLSYSLOG_CLASS_ALL, WLSYSLOG_MSG_CLIENT_DISCONNECTED);

#ifndef MRVL_WPS_CLIENT
			WLSNDEVT(dev_p, IWEVCUSTOM, &vmacEntry_p->vmacAddr, WLSYSLOG_MSG_CLIENT_DISCONNECTED);
#endif
#ifdef MRVL_WPS_CLIENT
			/* Send event to user space */
			if ((mlmeBssid[0] && mlmeBssid[1] && mlmeBssid[2] &&
				mlmeBssid[3] && mlmeBssid[4] && mlmeBssid[5]))
			{
				WLSNDEVT(dev_p, IWEVEXPIRED, (IEEEtypes_MacAddr_t *)&mlmeBssid[0], NULL);
			}
#endif

			memset(priv->hwData.macAddr, 0, 6);
		}
		break;

	case MlmeScan_Cnfm:
		printk("***** %s SCAN completed\n", dev_p->name);

		WLSYSLOG(dev_p, WLSYSLOG_CLASS_ALL, WLSYSLOG_MSG_CLIENT_SCAN_DONE);

#ifdef WMON
		if(!gScan)
#endif
		{
			/* If user initiated a scan */
			if (vmacSta_p->gUserInitScan == TRUE)
			{
				vmacSta_p->gUserInitScan = FALSE;

#ifdef MRVL_WPS_CLIENT
				/* Send event to user space */
				WLSNDEVT(dev_p, IWEVCUSTOM, &vmacEntry_p->vmacAddr, WLSYSLOG_MSG_CLIENT_SCAN_DONE);
#endif

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
				priv->vmacSta_p->busyScanning = 0;

				/*Restart link management */
				if(dev_p->flags & IFF_RUNNING)
				{
					smeGetStaLinkInfo(vmacEntry_p->id,
						&mlmeAssociatedFlag,
						&mlmeBssid[0]);

					if (mlmeAssociatedFlag)
					{
#ifdef AMPDU_SUPPORT_TX_CLIENT
						cleanupAmpduTx(vmacSta_p, (UINT8 *)&mlmeBssid[0]);
#endif
						linkMgtReStart(vmacEntry_p->phyHwMacIndx, vmacEntry_p);
					}
					else
						wlLinkMgt(dev_p, vmacEntry_p->phyHwMacIndx);
				}
			}       
			else
			{
				// linkMgtParseScanResult() might need a MIBs to control funct call
				linkMgtParseScanResult(vmacEntry_p->phyHwMacIndx);
			}
		}
		break;
	case MlmeReset_Cnfm:
		{
			struct net_device *apdev_p= priv->master;
			struct wlprivate *appriv = NETDEV_PRIV_P(struct wlprivate, apdev_p), *appriv1;
			vmacApInfo_t *vap_p;
			int i;
			for (i = 0; i < appriv->wlpd_p->vmacIndex; i++)
			{
				appriv1 = NETDEV_PRIV_P(struct wlprivate, appriv->vdev[i]);
				vap_p = appriv1->vmacSta_p;
				if ((appriv->vdev[i]->flags & IFF_RUNNING) &&(vap_p->VMacEntry.modeOfService == VMAC_MODE_AP) )
					wlreset_mbss(appriv->vdev[i]);
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
	//	wlfacilitate_e radioOnOff = WL_ENABLE ;
	
	WLDBG_ENTER(DBG_LEVEL_2);

	netdev->type = ARPHRD_ETHER;

	if (netdev->flags & IFF_RUNNING)
	{
		vmacSta_p->InfUpFlag = 0;		
		netif_stop_queue(netdev);
		netdev->flags &= ~IFF_RUNNING;
	}
	wlFwApplyClientSettings(netdev);
	netif_wake_queue(netdev);  /* Start/Restart Q if stopped. */
	vmacSta_p->InfUpFlag = 1;		
	netdev->flags |= IFF_RUNNING;
	WL_MOD_INC_USE(THIS_MODULE, return -EIO);

	/* Wireless Client Specific */
	{
		// Moved to ieee80211_encapSta for Client auto connect.
		//wlLinkMgt(netdev, vmacEntry_p->phyHwMacIndx);
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

	WLDBG_ENTER(DBG_LEVEL_2);
	vmacSta_p->InfUpFlag = 0;		

	/* Wireless Client Specific */
	//printk("********  wlstop_client\n");
	// linkMgtStop() might need a MIBs to control funct call
	linkMgtStop(vmacParentEntry_p->phyHwMacIndx);

	if (vmacParentEntry_p->active)
	{
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


	if (wlpptr->wlpd_p->inReset)
	{
		return 0;
	} 

	/* Wireless Client Specific */
	//printk("********  wlreset_client\n");
	smeStopBss(vmacParentEntry_p->phyHwMacIndx);
	/* end Wireless Client Specific */

	if(netdev->flags & IFF_RUNNING)
	{
		vmacSta_p->InfUpFlag = 0; 	
		netif_stop_queue(netdev);
		netdev->flags &= ~IFF_RUNNING;
	}
	wlFwApplyClientSettings(netdev);

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

	if (wlpptr->wlpd_p->inReset)
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
#if LINUX_VERSION_CODE >=KERNEL_VERSION(2,6,25)
		wlpptr->netDev = dev;
#else
		wlpptr->netDev = &wlpptr->netDevPriv;
#ifdef WL_KERNEL_26
		memset(wlpptr->netDev, 0, sizeof(struct net_device));
#endif
#endif
		wlpptr->netDev->flags = 0;
#if LINUX_VERSION_CODE >=KERNEL_VERSION(3,0,6)
		wlpptr->netDev->priv_flags = 0;
#else
		wlpptr->netDev->br_port = NULL;
#endif
		//from probe
		wlpptr->ioBase0 = wlp->ioBase0;
		wlpptr->ioBase1 = wlp->ioBase1;
		//sprintf(wlpptr->netDev->name, wlp->netDev->name);
		sprintf(wlpptr->netDev->name, DRV_NAME_CLIENT, wlinitcnt, 0);
		wlpptr->netDev->irq	   = wlp->netDev->irq;
		wlpptr->netDev->mem_start = wlp->netDev->mem_start;
		wlpptr->netDev->mem_end	 = wlp->netDev->mem_end;
		NETDEV_PRIV_S(wlpptr->netDev) = wlpptr;
		//	wlpptr->pPciDev 		 = wlp->pPciDev;
#ifdef WL_KERNEL_26
		SET_MODULE_OWNER(*(wlpptr->netDev));
#endif

		/* Use the same address as root AP for stations. */
		memcpy(wlpptr->netDev->dev_addr, ApRootmacAddr_p, 6);
		memcpy(&wlpptr->hwData.macAddr[0], macAddr_p, 6);
		wlpptr->vmacSta_p = Mac_Init(wlp, wlpptr->netDev, macAddr_p, WL_OP_MODE_VSTA, wlinitcnt);
		if(wlpptr->vmacSta_p == NULL)
		{
			printk(KERN_ERR  "%s: failed to init driver mac\n", wlpptr->netDev->name);
			goto err_out;
		}
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
#ifdef WL_KERNEL_26
		wlSetupWEHdlr(wlpptr->netDev);
#endif
		wlpptr->wlpd_p = wlp->wlpd_p;
		wlpptr->master= wlp->netDev;
		wlp->vdev[wlp->wlpd_p->vmacIndex++] = wlpptr->netDev;
#if LINUX_VERSION_CODE >=KERNEL_VERSION(3,0,6)
#else
		atomic_set(&wlpptr->netDev->refcnt, 0);
#endif
		ether_setup(wlpptr->netDev);
		if (register_netdev(wlpptr->netDev))
		{
			printk("%s: failed to register device\n", wlpptr->netDev->name);
			goto err_register_netdev;
		}
#ifdef AP8X_STATISTICS
		ap8x_stat_proc_register(wlpptr->netDev);
#endif

		/* Wireless Client Specific */
		{        
			{
				if((clientVMacEntry_p = smeInitParentSession(wlinitcnt, 
					macAddr_p,
					0,
					&wlStatusUpdate_clientParent,
					(void *)wlpptr->netDev)) == NULL)
				{
					goto err_init;
				}
				mainNetdev_p[wlinitcnt] = wlp->netDev;
				wlpptr->txNetdev_p = wlpptr->netDev;
				wlpptr->clntParent_priv_p = (void *)clientVMacEntry_p;
				wlpptr->vmacSta_p->VMacEntry.id = clientVMacEntry_p->id;
			}
			// Initialize Client PeerInfo.
			InitClientPeerInfo(wlpptr->netDev);

		}
	}
	/* end Wireless Client Specific */

#ifdef EWB    
	wetHashInit();
#endif

	WLDBG_EXIT(DBG_LEVEL_2);	
	return 0;
err_init:
#ifdef AP8X_STATISTICS
	ap8x_stat_proc_unregister(wlpptr->netDev);
#endif
	//		wlRxRingCleanup(netdev);
err_out:
err_register_netdev:
	free(wlpptr);
	WLDBG_EXIT(DBG_LEVEL_2);
	return -EIO;
}

void wlDeinit_client(struct net_device *netdev)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	vmacEntry_t  *vmacParentEntry_p = (vmacEntry_t	*) wlpptr->clntParent_priv_p;
	struct wlprivate *wlp;
	int i;
	WLDBG_ENTER(DBG_LEVEL_2);
#if defined(SINGLE_DEV_INTERFACE) && !defined(CLIENTONLY)
	for (i = 1; i < wlpptr->wlpd_p->vmacIndex; i++)
#else
	for (i = 0; i < wlpptr->wlpd_p->vmacIndex; i++)
#endif
	{
		if (wlpptr->vdev[i]->flags & IFF_RUNNING)
		{
			if (wlstop_client(wlpptr->vdev[i]))
			{
				printk(KERN_ERR "%s: failed to stop device\n",wlpptr->vdev[i]->name );
			}
		}
		wlp = NETDEV_PRIV_P(struct wlprivate, wlpptr->vdev[i]);
		DisableMacMgmtTimers(wlp->vmacSta_p);

		/* Wireless Client Specific */
		smeStopBss(vmacParentEntry_p->phyHwMacIndx);
		/* end Wireless Client Specific */

#ifdef AP8X_STATISTICS
		ap8x_stat_proc_unregister(wlpptr->vdev[i]);
#endif

		free(wlp->vmacSta_p);
		unregister_netdev(wlpptr->vdev[i]);		
#if LINUX_VERSION_CODE >=KERNEL_VERSION(2,6,25)
		free_netdev(wlpptr->vdev[i]);
#else
		free(wlp);
#endif
	}
	wlpptr->wlpd_p->vmacIndex =0;
#ifdef EWB    
	wetHashDeInit();
#endif


	WLDBG_EXIT(DBG_LEVEL_2);
	return;
}

#endif /* CLIENT_SUPPORT */

