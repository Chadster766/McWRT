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

#include "osif.h"

unsigned char *wlBufPull(WL_BUFF *wlb, unsigned int l)
{
	unsigned char *data = skb_pull(WL_BUFF_SKB(wlb), l);
	WL_UPDATE_BUF_INFO(WL_BUFF_SKB(wlb));
	return data;
}

unsigned char *wlBufPut(WL_BUFF *wlb, unsigned int l)
{
	unsigned char *data = skb_put(WL_BUFF_SKB(wlb), l);
	WL_UPDATE_BUF_INFO(WL_BUFF_SKB(wlb));
	return data;
}

unsigned char *wlBufPush(WL_BUFF *wlb, unsigned int l)
{
	unsigned char *data = skb_push(WL_BUFF_SKB(wlb), l);
	WL_UPDATE_BUF_INFO(WL_BUFF_SKB(wlb));
	return data;
}

WL_BUFF *wlBufCopy(WL_BUFF *wlb)
{
	struct sk_buff *skb = skb_copy(WL_BUFF_SKB(wlb), GFP_ATOMIC);
	if(skb){
		WL_PREPARE_BUF_INFO(skb);
		return WL_BUFF_PTR(skb);
	}
	return NULL;
}

WL_BUFF *wlBufCopyExpand(WL_BUFF *wlb, int newheadroom, int newtailroom)
{
	struct sk_buff *skb = skb_copy_expand(WL_BUFF_SKB(wlb), newheadroom, newtailroom, GFP_ATOMIC);
	if(skb){
		WL_PREPARE_BUF_INFO(skb);
		return WL_BUFF_PTR(skb);
	}
	return NULL;
}

void wlBufReserve(WL_BUFF *wlb, unsigned int l)
{
	skb_reserve(WL_BUFF_SKB(wlb), l);
	WL_UPDATE_BUF_INFO(WL_BUFF_SKB(wlb));
}

WL_BUFF *wlBufUnshare(WL_BUFF *wlb)
{
	struct sk_buff *skb = skb_unshare(WL_BUFF_SKB(wlb), GFP_ATOMIC);
	if(skb){
		WL_PREPARE_BUF_INFO(skb);
		return WL_BUFF_PTR(skb);
	}
	return NULL;
}

WL_BUFF *wlBuffReallocHeadroom(WL_BUFF *wlb, unsigned int l)
{
	struct sk_buff *skb = skb_realloc_headroom(WL_BUFF_SKB(wlb), l);
	if(skb){
		WL_PREPARE_BUF_INFO(skb);
		return WL_BUFF_PTR(skb);
	}
	return NULL;
}

WL_BUFF *wlBuffAlloc(unsigned int len)
{
	struct sk_buff *skb = dev_alloc_skb(len);
	if(skb) {
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
		if (skb_linearize(skb))
#else
		if (skb_linearize(skb, GFP_ATOMIC))
#endif
		{
			dev_kfree_skb_any(skb);
			return NULL;
		}
		WL_PREPARE_BUF_INFO(skb);		
		return WL_BUFF_PTR(skb);
	}
	return NULL;
}


WL_BUFF *wlGetNewBufAndInit(unsigned int len, WL_BUFF *old)
{
	WL_BUFF *wlb = wlBuffAlloc(len);
	if(wlb)
	{
		struct sk_buff *skb = WL_BUFF_SKB(wlb);
		if(skb){
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
			if (skb_linearize(skb))
#else
			if (skb_linearize(skb, GFP_ATOMIC))
#endif
			{
				WL_BUFF_FREE(wlb);
				return NULL;
			}
			skb_reserve(skb, 64+sizeof(struct wlBufInfo)); /* 64 byte headroom */
			skb->dev = WL_BUFF_SKB(old)->dev;
			skb->protocol = WL_BUFF_SKB(old)->protocol;
			skb->priority = WL_BUFF_SKB(old)->priority;
			WL_PREPARE_BUF_INFO(skb);
			wlb = WL_BUFF_PTR(skb);
			return wlb;
		}
	}
	return NULL;
}