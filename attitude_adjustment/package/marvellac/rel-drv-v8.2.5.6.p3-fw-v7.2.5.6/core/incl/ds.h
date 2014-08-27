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

/******************** (c) Marvell Semiconductor, Inc., 2001 *******************
* 
* Purpose: 
*    This file contains the function prototypes and definitions for the 
*    Distribution Service Module. 
* 
* Public Procedures: 
* 
* Notes: 
*    None. 
* 
*****************************************************************************/

#ifndef _DS_H_
#define _DS_H_
#include "wl_hal.h"
#include "StaDb.h"
//=============================================================================
//                          PUBLIC TYPE DEFINITIONS
//=============================================================================

#define ETHER_HDR_SIZE 14
#define MAX_ETHER_PKT_SIZE 1800 //should be 0x600, need to check.

#define IS_BROADCAST(macaddr) ((*(UINT32*)macaddr == 0xffff ) &&  \
                          *(UINT16 *)((UINT8*)macaddr+4) == 0xff)

#define IS_MULTICAST(macaddr)  ((*(UINT8*)macaddr & 0x01) == 0x01)

#define IS_GROUP(macaddr)  ((*(UINT8*)macaddr & 0x01) == 0x01)

#define MACADDR_CMP(macaddr1, macaddr2)         \
		((*(UINT32*)macaddr1 == *(UINT32*)macaddr2) && \
		(*(UINT16 *)((UINT8*)macaddr1+4) == (*(UINT16 *)((UINT8*)macaddr2+4))) ? \
		0 : 1)

#define MACADDR_CPY(macaddr1,macaddr2) { *(UINT16*)macaddr1 = *(UINT16*)macaddr2; \
                    *(UINT16 *)((UINT16*)macaddr1+1) = *(UINT16 *)((UINT16*)macaddr2+1); \
                    *(UINT16 *)((UINT16*)macaddr1+2) = *(UINT16 *)((UINT16*)macaddr2+2);}



typedef  struct 
{
   UINT8 ihl:4; //Note that the MS nibble is Version.
                //the LS nibble is Header Length.
   UINT8 ver:4;
   UINT8 tos;
   UINT16 total_length;
   UINT16 identification;
   UINT16 flag_fragoffset; //the MS three bits are for flag and rest is fragment offset.
   UINT8 ttl;
   UINT8 protocol;
   UINT16 header_chksum;
   UINT8 src_IP_addr[4];
   UINT8 dst_IP_addr[4];
}
PACK_END IEEEtypes_IPv4_Hdr_t;

typedef  struct
{
   UINT32 flow_label:16;
   UINT32 traffic_class:8;
   UINT32 ver:4;
   UINT16 payload_length;
   UINT8 next_header;
   UINT8 hop_limit;
   UINT8 src_IP_addr[4];
   UINT8 dst_IP_addr[4];
}
PACK_END IEEEtypes_IPv6_Hdr_t;

typedef  struct
{
  UINT16 src_port;
  UINT16 dst_port;
  UINT32 seq_num;
  UINT32 ack_num;
  UINT32 datoffset_control_window; //data offset is MS four bits
  UINT16 chksum;
  UINT16 urgent_ptr;
}
PACK_END IEEEtypes_TCP_Hdr_t;

typedef  struct
{
  UINT16 src_port;
  UINT16 dst_port;
  UINT16 len;
  UINT16 chksum;
}
PACK_END IEEEtypes_UDP_Hdr_t;


typedef  struct
{
    ether_hdr_t Hdr;
    UINT8 Body[1800 - ETHER_HDR_SIZE];
}
PACK_END IEEEtypes_8023_Frame_t;

typedef  struct
{
    IEEEtypes_8023_Frame_t DataFrame;
}
PACK_END rx8023_DataFrame_t;

typedef struct llc_snap_hdr
{
    UINT8 Dsap;
    UINT8 Ssap;
    UINT8 Control;
    UINT8 Org[3];
    UINT16 Type;
}
llc_snap_hdr_t;


typedef struct 
{
    UINT16 Type;
    UINT16 Control;
}
IEEE802_1QTag_t;

#define IEEE802_11Q_TYPE    0x8100
#define IPV6_VERSION 0x06

extern IEEEtypes_MacAddr_t bcast;

void free_any_pending_ampdu_pck(WL_PRIV *, UINT16 Aid);
extern void disableAmpduTx(WL_PRIV *wlpptr,UINT8 *macaddr, UINT8 tid);
extern void cleanupAmpduTx(WL_PRIV *wlpptr,UINT8 *macaddr);
extern void disableAmpduTxstream(WL_PRIV *wlpptr,int stream);
extern void disableAmpduTxMacAddr(WL_PRIV *wlpptr,UINT8 *macaddr);	
extern void disableAmpduTxAll(WL_PRIV *wlpptr);

extern int ieee80211_input(WL_PRIV *, WL_BUFF *, UINT32 rssi, UINT32 rssiPaths, UINT8 ampdu_qos, UINT32 status);
extern WL_BUFF *ieee80211_encap(WL_BUFF *, WL_PRIV *, BOOLEAN eap);
extern BOOLEAN McastProxyUCastAddrRemove(WL_PRIV *wlpptr, IEEEtypes_MacAddr_t *addr);	
#endif /* _DS_H_ */
