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

#ifndef __EWB_PACKET_H__
#define __EWB_PACKET_H__

#include "wltypes.h"
#include "osif.h"
#include "wl_hal.h"
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
    unsigned char       offset[12];
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
    int (*lanPackHandler)(WL_BUFF *);
    int (*wlanPackHandler)(WL_BUFF *);
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

extern int ewbLanRecv(WL_PRIV *wlpptr, WL_BUFF *,unsigned char *rfAddr);
extern int ewbWlanRecv(WL_PRIV *wlpptr, WL_BUFF *,unsigned char *rfAddr);
extern int ewbLanRecvCloned(WL_BUFF *);
extern int ewbWlanRecvCloned(WL_BUFF *);
extern int ewbInit(void);
#endif /* __EWB_PACKET_H__ */
