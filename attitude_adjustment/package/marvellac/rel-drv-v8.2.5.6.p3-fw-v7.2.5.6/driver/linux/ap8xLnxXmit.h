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

#ifndef AP8X_XMIT_H_
#define AP8X_XMIT_H_

#include <linux/version.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/pci.h>
#include <linux/spinlock.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/skbuff.h>
#include <linux/delay.h>
#include <linux/if_ether.h>   
#include <linux/if_arp.h>   
#include <linux/net.h>
#include <linux/wireless.h>

#include <net/iw_handler.h>

#include "ap8xLnxDesc.h"
#include "ap8xLnxApi.h"
#include "StaDb.h"

extern int wlDataTx(struct sk_buff *, struct net_device *);
extern void wlTxDone(struct net_device *);


#define NBR_BYTES_FW_RX_PREPEND_LEN   2
#define NBR_BYTES_FW_TX_PREPEND_LEN   2
#define NBR_BYTES_CTRLSTATUS          2
#define NBR_BYTES_DURATION            2
#define NBR_BYTES_ADDR1               6
#define NBR_BYTES_ADDR2               6
#define NBR_BYTES_ADDR3               6
#define NBR_BYTES_SEQFRAG             2
#define NBR_BYTES_ADDR4               6
#define NBR_BYTES_TIMESTAMP           8
#define NBR_BYTES_BEACON_INTERVAL     2
#define NBR_BYTES_CAP_INFO            2
#define NBR_BYTES_FCS                 4

#define NBR_BYTES_ADD_RXFWINFO        ((NBR_BYTES_ADDR4) + \
                                       (NBR_BYTES_FW_RX_PREPEND_LEN))

#define NBR_BYTES_ADD_TXFWINFO        ((NBR_BYTES_ADDR4) + \
                                       (NBR_BYTES_FW_TX_PREPEND_LEN))

#define NBR_BYTES_COMPLETE_TXFWHEADER ((NBR_BYTES_FW_TX_PREPEND_LEN) + \
                                       (NBR_BYTES_CTRLSTATUS)        + \
                                       (NBR_BYTES_DURATION)          + \
                                       (NBR_BYTES_ADDR1)             + \
                                       (NBR_BYTES_ADDR2)             + \
                                       (NBR_BYTES_ADDR3)             + \
                                       (NBR_BYTES_SEQFRAG)           + \
                                       (NBR_BYTES_ADDR4))

#define NBR_BYTES_IEEE80211HEADER     ((NBR_BYTES_CTRLSTATUS) + \
                                       (NBR_BYTES_DURATION)   + \
                                       (NBR_BYTES_ADDR1)      + \
                                       (NBR_BYTES_ADDR2)      + \
                                       (NBR_BYTES_ADDR3)      + \
                                       (NBR_BYTES_SEQFRAG))
                                       
#define NBR_BYTES_COMPLETE_IEEE80211HEADER     ((NBR_BYTES_CTRLSTATUS) + \
                                                (NBR_BYTES_DURATION)   + \
                                                (NBR_BYTES_ADDR1)      + \
                                                (NBR_BYTES_ADDR2)      + \
                                                (NBR_BYTES_ADDR3)      + \
                                                (NBR_BYTES_SEQFRAG)    + \
                                                (NBR_BYTES_ADDR4))                                                


#define NBR_BYTES_IEEE80211COPYLEN    ((NBR_BYTES_IEEE80211HEADER) - \
                                       (NBR_BYTES_ADDR4))

#define OFFS_IEEE80211HEADER           0
#define OFFS_IEEE80211PAYLOAD          (NBR_BYTES_IEEE80211HEADER)
#define OFFS_TXFWBUFF_IEEE80211HEADER  (NBR_BYTES_FW_TX_PREPEND_LEN)
#define OFFS_TXFWBUFF_IEEE80211PAYLOAD (NBR_BYTES_COMPLETE_TXFWHEADER)
#define OFFS_RXFWBUFF_IEEE80211HEADER  (NBR_BYTES_FW_TX_PREPEND_LEN)
#define OFFS_RXFWBUFF_IEEE80211PAYLOAD (NBR_BYTES_COMPLETE_TXFWHEADER)

int wlxmit(struct net_device *netdev, struct sk_buff *skb, struct wlxmit_param *param);

extern int wlhandlepsxmit(struct net_device *netdev, struct sk_buff *skb, struct wlxmit_param *param);
extern int wlprocesspsq(struct net_device *netdev, extStaDb_StaInfo_t *pStaInfo, struct ieee80211_frame *hdr_p);
extern void wlSetPsQueStatus(struct wlprivate *wlpptr, UINT32 psPciQid, int set);
void wlDataTxHdl(struct net_device *netdev);
int wlMgmtTx(struct sk_buff *skb, struct net_device *netdev);
int wlDataTxUnencr(struct sk_buff *skb, struct net_device *netdev, extStaDb_StaInfo_t *pStaInfo);
int wlxmitmfs(struct net_device *netdev, int qindex, extStaDb_StaInfo_t *pStaInfo );

#endif /* AP8X_XMIT_H_ */
