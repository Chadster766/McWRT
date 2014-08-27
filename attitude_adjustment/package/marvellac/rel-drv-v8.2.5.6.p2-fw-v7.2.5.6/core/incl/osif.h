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

#ifndef _OSIF_H_
#define _OSIF_H_

#include        <linux/module.h>
#include        <linux/init.h>
#include        <linux/kernel.h>
#include        <linux/version.h> /* added */
#include        <linux/delay.h>
#include        <linux/sched.h>
#include        <linux/proc_fs.h>
#include        <linux/pci.h>
#include        <linux/ioport.h>
#include        <linux/net.h>
#include        <linux/netdevice.h>
#include        <linux/wireless.h>
#include        <linux/etherdevice.h>
#include        <linux/timer.h>
#include        <linux/fs.h>
#include        <linux/random.h>
#include        <asm/uaccess.h>
#include 		<linux/bug.h>
#include 		<linux/ip.h>

#define WL_PRIV   struct wlprivate 

struct wlBufInfo{
	unsigned int cnt;
	void *iphdr;
	void *nhdr;
	void *machdr;
	struct sk_buff *skb;
	__be16 *protocol;
	unsigned int *len;
	unsigned char *data;
	void *dev;
	__u32 *priority;
	__wsum *csum;
};
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,22)
#ifdef NET_SKBUFF_DATA_USES_OFFSET
#define SKB_IPHDR(skb) ((struct iphdr*)(skb->head+skb->network_header))
#else
#define SKB_IPHDR(skb) ((struct iphdr*)skb->network_header)
#endif
#define SKB_NHDR(skb) skb->network_header
#define SKB_MACHDR(skb) skb->mac_header
#else
#define SKB_IPHDR(skb) skb->nh.iph
#define SKB_NHDR(skb) skb->nh.raw
#define SKB_MACHDR(skb) skb->mac.raw
#endif


#define WL_UPDATE_BUF_INFO(skb) if(skb){\
	struct wlBufInfo *info_p = (struct wlBufInfo *)skb->head;\
	WLBUG_ON(((int)skb->data - (int)skb->head)<sizeof(struct wlBufInfo));\
	info_p->data = skb->data;\
}\


#define WL_PREPARE_BUF_INFO(s) if(s){\
	struct wlBufInfo *info_p = (struct wlBufInfo *)s->head;\
	WLBUG_ON(((int)s->data - (int)s->head)<sizeof(struct wlBufInfo));\
	info_p->skb = s;\
	info_p->data = s->data;\
	info_p->csum = &s->csum;\
	info_p->dev = (void *)s->dev;\
	info_p->iphdr = (void *)SKB_IPHDR(s);\
	info_p->len = &s->len;\
	info_p->machdr = (void *)SKB_MACHDR(s);\
	info_p->nhdr = (void *)SKB_NHDR(s);\
	info_p->priority = &s->priority;\
	info_p->protocol = &s->protocol;\
}\


struct ap_config {
	int sizeof_vmacApInfo_t;
	int sizeof_WL_SYS_CFG_DATA;
	int sizeof_sk_buff_head;
	int sizeof_spinlock_t;
	int sizeof_ExtStaInfoItem_t;
	void *(*wlmalloc)(int l, int f);
	void (*wlfree)(void *p);
	void (*dev_kfree_skb_any)(struct sk_buff *skb);
	int (*print)(const char *fmt, ...);
	void (*netifstopqueue)(struct net_device *dev);
	void (*netifwakequeue)(struct net_device *dev);
	void (*spinlockinit)(spinlock_t *lock);
	void (*spinlockirqsave)(spinlock_t *lock, unsigned long *flags);
	void (*spinunlockirqrestore)(spinlock_t *lock, unsigned long flags);
	int (*wlmemcmp)(const void *cs, const void *ct, size_t n);
	void *(*wlmemcpy)(void *v_dst, const void *v_src, __kernel_size_t c);
	void *(*wlmemset)(void *v_src, int c, __kernel_size_t n);
	void (*skbqueueheadinit)(struct sk_buff_head *list);
	void (*skbqueuepurge)(struct sk_buff_head *list);
	struct sk_buff *(*skbdequeue)(struct sk_buff_head *list);
	void (*skbqueuetail)(struct sk_buff_head *list, struct sk_buff *newsk);
	struct sk_buff *(*skbpeektail)(const struct sk_buff_head *list_);
	__u32 (*skbqueuelen)(const struct sk_buff_head *list_);
	int (*skbtailroom)(const struct sk_buff *skb);
	unsigned int (*skbheadroom)(const struct sk_buff *skb);
	void (*skbtrim)(struct sk_buff *skb, unsigned int len);
	bool (*skbisnonlinear)(const struct sk_buff *skb);
	int (*netifrcv)(struct sk_buff *skb);
	void (*unregnetdev)(struct net_device *dev);
	void (*freenetdev)(struct net_device *dev);
	struct net_device *(*devgetbyname)(struct net *net, const char *name);
	__be16 (*ethtypetrans)(struct sk_buff *skb, struct net_device *dev);
	u32 (*wlreadl)(const volatile void __iomem *addr);
	void (*wlwritel)(u32 val, void __iomem *addr);
	u32 (*random)(void);
	void (*wlmdelay)(unsigned long ms);
};

extern struct ap_config ap8x_config;

#define UNREGISTER_NETDEV ap8x_config.unregnetdev
#define FREE_NETDEV ap8x_config.freenetdev
#define DEV_GET_BY_NAME ap8x_config.devgetbyname
#define ETH_TYPE_TRANS ap8x_config.ethtypetrans
#define NETIF_STOP_QUEUE ap8x_config.netifstopqueue
#define NETIF_WAKE_QUEUE ap8x_config.netifwakequeue
#define WL_PRINT ap8x_config.print  
#define WL_MEMCMP ap8x_config.wlmemcmp
#define WL_MEMSET ap8x_config.wlmemset
#define WL_MEMCPY ap8x_config.wlmemcpy
#define WL_READL ap8x_config.wlreadl
#define WL_WRITEL ap8x_config.wlwritel
#define WLNET_RANDOM ap8x_config.random
#define WL_MDELAY(x) ap8x_config.wlmdelay(x)

#define WL_BUFF		struct wlBufInfo
#define WLBUG_ON(x) 
#define WL_FREE   ap8x_config.wlfree
#define WL_MALLOC(x) ap8x_config.wlmalloc(x,GFP_ATOMIC)

#define DECLARE_QUEUE(h) struct sk_buff_head *h
#define QUEUE_INIT(h)  {\
							*h=(struct sk_buff_head *)WL_MALLOC(ap8x_config.sizeof_sk_buff_head);\
							ap8x_config.skbqueueheadinit(*h);\
					   }\

#define QUEUE_DEINIT(h)	kfree(*h)
#define QUEUE_PURGE(h) \
ap8x_config.skbqueuepurge(*h);\
QUEUE_DEINIT(h)

#define QUEUE_DEQUEUE(h) ap8x_config.skbdequeue(*h)
#define QUEUE_ENQUEUE_TAIL(h, buf) ap8x_config.skbqueuetail(*h, buf)
#define QUEUE_LEN(h)	ap8x_config.skbqueuelen(*h)
#define QUEUE_PEEK_TAIL(h)	ap8x_config.skbpeektail(*h)


#define WL_SKB_FREE(skb) {\
							ap8x_config.dev_kfree_skb_any(skb);\
						}\
							
#define WL_BUFF_FREE(s)	{\
							ap8x_config.dev_kfree_skb_any(s->skb);\
						}\

#define WL_BUFF_TRIM(s, l) ap8x_config.skbtrim(s->skb, l)

#define WL_BUFF_PULL(s, l) vmacSta_p->skbops->wlBufPull(s, l)

#define WL_BUFF_PUT(s, l) vmacSta_p->skbops->wlBufPut(s, l)

#define WL_BUFF_PUSH(s, l) vmacSta_p->skbops->wlBufPush(s, l)

#define WL_BUFF_TAILROOM(s) ap8x_config.skbtailroom(s->skb)
#define WL_BUFF_HEADROOM(s) ap8x_config.skbheadroom(s->skb)
#define WL_BUFF_IS_NONLINEAR(s)	ap8x_config.skbisnonlinear(s->skb)

#define WL_BUFF_COPY(s)	vmacSta_p->skbops->wlBufCopy(s)

#define WL_BUFF_COPY_EXPAND(s, o, l) vmacSta_p->skbops->wlBufCopyExpand(s,o,l)

#define WL_BUFF_RESERVE(s, l) vmacSta_p->skbops->wlBufReserve(s, l)

#define WL_BUFF_UNSHARE(s)	vmacSta_p->skbops->wlBufUnshare(s)

#define WL_BUFF_REALLOC_HEADROOM(s, l) vmacSta_p->skbops->wlBuffReallocHeadroom(s, l)

#define WL_BUFF_ALLOC(l) vmacSta_p->skbops->wlBuffAlloc(l)
	

#define WL_BUFF_DATA_PTR(wlb) (wlb->data)
#define WL_BUFF_GET_LEN(wlb)  (*wlb->len)
#define WL_BUFF_UPDATE_LEN(wlb, len) *wlb->len=len 


#define WL_BUFF_HEAD_PTR(s) s->skb->head
#define WL_BUFF_TAIL_PTR(s) s->skb->tail
#define WL_BUFF_END_PTR(s) s->skb->end

#define WL_NETIF_RX_BUFF(s)		ap8x_config.netifrcv(s->skb)

#define WL_BUFF_LEN(s)	*s->len
#define WL_BUFF_DATA(s) s->data
#define WL_BUFF_SKB(s)	s->skb
#define WL_BUFF_CSUM(s) *s->csum
#define WL_BUFF_PROTOCOL(s) *s->protocol
#define WL_BUFF_PRIORITY(s) *s->priority
#define WL_BUFF_PTR(s)	(WL_BUFF *)s->head

#define WL_NETDEV   struct net_device 

#define JIFFIES			jiffies

//#define CONFIG_SPINLOCK_DEBUG
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

#define SPIN_LOCK_DEINIT(l)

#else
#define DECLARE_LOCK(l) spinlock_t *l

#define MUST_BE_LOCKED(l)
#define MUST_BE_UNLOCKED(l)

#define SPIN_LOCK_INIT(l) \
*l=(spinlock_t *)WL_MALLOC(ap8x_config.sizeof_spinlock_t); \
ap8x_config.spinlockinit(*l)

#define SPIN_LOCK_DEINIT(l) ap8x_config.wlfree(*l)
#define SPIN_LOCK_IRQSAVE(l, f) ap8x_config.spinlockirqsave(*l, (unsigned long *)&f)
#define SPIN_UNLOCK_IRQRESTORE(l, f) ap8x_config.spinunlockirqrestore(*l, f)
 
#endif /*CONFIG_SPINLOCK_DEBUG*/

#define ENDIAN_SWAP32(_val)   (cpu_to_le32(_val))
#define ENDIAN_SWAP16(_val)   (cpu_to_le16(_val))

WL_BUFF *wlGetNewBufAndInit(unsigned int len, WL_BUFF *old);
WL_BUFF *wlBufCopy(WL_BUFF *s);
WL_BUFF *wlBufCopyExpand(WL_BUFF *s, int newheadroom, int newtailroom);
void wlBufReserve(WL_BUFF *s, unsigned int l);
WL_BUFF *wlBufUnshare(WL_BUFF *s);
WL_BUFF *wlBuffReallocHeadroom(WL_BUFF *s, unsigned int l);
WL_BUFF *wlBuffAlloc(unsigned int len);
unsigned char *wlBufPull(WL_BUFF *wlb, unsigned int l);
unsigned char *wlBufPut(WL_BUFF *wlb, unsigned int l);
unsigned char *wlBufPush(WL_BUFF *wlb, unsigned int l);


#endif