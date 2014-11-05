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

extern int wlxmit(struct net_device *netdev, struct sk_buff *skb, UINT8 type, extStaDb_StaInfo_t *pStaInfo, UINT32 bcast, BOOLEAN eap);
void wlDataTxHdl(struct net_device *netdev);
#endif /* AP8X_XMIT_H_ */
