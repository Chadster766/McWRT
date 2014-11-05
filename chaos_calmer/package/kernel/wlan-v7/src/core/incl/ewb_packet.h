/*
 * Copyright (c) 2000-2005 by Marvell International Ltd.
 * 
* If you received this File from Marvell, you may opt to use, redistribute 
* and/or modify this File in accordance with the terms and conditions of the 
* General Public License Version 2, June 1991 (the “GPL License”), a copy of 
* which is available along with the File in the license.txt file or by writing 
* to the Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, 
* MA 02111-1307 or on the worldwide web at http://www.gnu.org/licenses/gpl.txt. 
*
* THE FILE IS DISTRIBUTED AS-IS, WITHOUT WARRANTY OF ANY KIND, AND THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE ARE 
* EXPRESSLY DISCLAIMED. The GPL License provides additional details about 
* this warranty disclaimer.
*
*/

#ifndef __EWB_PACKET_H__
#define __EWB_PACKET_H__

#include "wltypes.h"

/* For interface */
#define INTF_WLAN              0
#define INTF_LAN               1

/* For Ethernet Header */
#define HW_ADDR_LEN				6
#define ETH_HDR_LEN             14
#define ETH_IP_TYPE             0x800
#define ETH_ARP_TYPE            0x806
#define EAPOL_TYPE              0x888E

typedef PACK_START struct eth_hdr_t {
	unsigned char        dest[HW_ADDR_LEN];
	unsigned char        src[HW_ADDR_LEN];
	unsigned short       type;
} PACK_END eth_hdr_t;

/* For IP Header */
#define IP_ADDR_LEN				4
#define IP_HDR_LEN              20

typedef PACK_START struct ewb_ip_hdr{
    #if 1
    unsigned char       offset[12];
    #else
	unsigned char       hrdInfo;
	unsigned char       srvType;
//	unsigned short      len;
//	unsigned short		id;
    unsigned char       len[2];
	unsigned char		id[2];
	unsigned char       fragInfo[2];
	unsigned char       ttl;
	unsigned char       protocol;
//	unsigned short		chkSum;
//	unsigned long		srcAddr;
//	unsigned long       destAddr;
    unsigned char		chkSum[2];
    #endif
    unsigned char		srcAddr[IP_ADDR_LEN];
    unsigned char       destAddr[IP_ADDR_LEN];
} PACK_END ewb_ip_hdr;


/* For ARP Header */
#define ARP_HDR_LEN             8

typedef PACK_START struct ewb_arp_hdr {
	unsigned short      hwType;
	unsigned short      protoType;
	unsigned char       hwAddrLen;
	unsigned char       protoAddrLen;
	unsigned short      op;
} PACK_END ewb_arp_hdr;

/* For ARP using ethernet and IP Addresses only */
typedef PACK_START struct arp_eth_ip_addr {
	unsigned char sndHwAddr[HW_ADDR_LEN];
	unsigned char sndIpAddr[IP_ADDR_LEN];
	unsigned char trgtHwAddr[HW_ADDR_LEN];
	unsigned char trgtIpAddr[IP_ADDR_LEN];
//	unsigned char *padding;
} PACK_END arp_eth_ip_addr;

/* Packet Function handlers */
typedef struct _packHandler {
    int (*lanPackHandler)(struct sk_buff *skb);
    int (*wlanPackHandler)(struct sk_buff *skb);
} packHandler;


#define IS_BROADCAST_HWADDR(x) ( (*(unsigned long *)(x + 2) == 0xffffffff)  \
                                         && (*(unsigned short *)x == 0xffff) )

#ifndef ECOS_TCP_IP_STACK
#define LITTLE_ENDIAN   1

#ifndef ntohs
#ifdef LITTLE_ENDIAN
#define ntohs(x) ((x>>8)|(x<<8))
#else
#define ntohs(x) x
#endif //LITTLE_ENDIAN
#endif //ntohs
#endif /* ECOS_TCP_IP_STACK */

extern int ewbLanRecv(struct sk_buff *skb,unsigned char *rfAddr);
extern int ewbWlanRecv(struct sk_buff *skb,unsigned char *rfAddr);
extern int ewbLanRecvCloned(struct sk_buff *skb);
extern int ewbWlanRecvCloned(struct sk_buff *skb);
extern int ewbInit(void);
#endif /* __EWB_PACKET_H__ */
