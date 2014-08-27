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

#ifndef AP8X_DESC_H_
#define AP8X_DESC_H_

#include <linux/version.h>
#include <linux/if_ether.h>   
#include <linux/netdevice.h>
#include <linux/skbuff.h>
#include "osif.h"


#define SOFT_STAT_STALE                               0x80

#define EAGLE_RXD_CTRL_DRIVER_OWN                     0x00
#define EAGLE_RXD_CTRL_OS_OWN                         0x04
#define EAGLE_RXD_CTRL_DMA_OWN                        0x80

#define EAGLE_RXD_STATUS_IDLE                         0x00
#define EAGLE_RXD_STATUS_OK                           0x01
#define EAGLE_RXD_STATUS_MULTICAST_RX                 0x02
#define EAGLE_RXD_STATUS_BROADCAST_RX                 0x04
#define EAGLE_RXD_STATUS_FRAGMENT_RX                  0x08

#define EAGLE_TXD_STATUS_IDLE                   0x00000000
#define EAGLE_TXD_STATUS_USED                   0x00000001 
#define EAGLE_TXD_STATUS_OK                     0x00000001
#define EAGLE_TXD_STATUS_OK_RETRY               0x00000002
#define EAGLE_TXD_STATUS_OK_MORE_RETRY          0x00000004
#define EAGLE_TXD_STATUS_MULTICAST_TX           0x00000008
#define EAGLE_TXD_STATUS_BROADCAST_TX           0x00000010
#define EAGLE_TXD_STATUS_FAILED_LINK_ERROR      0x00000020
#define EAGLE_TXD_STATUS_FAILED_EXCEED_LIMIT    0x00000040
#define EAGLE_TXD_STATUS_FAILED_AGING           0x00000080
#define EAGLE_TXD_STATUS_FW_OWNED               0x80000000

#define EAGLE_TXD_XMITCTRL_USE_RATEINFO         0x1
#define EAGLE_TXD_XMITCTRL_DISABLE_AMPDU        0x2
#define EAGLE_TXD_XMITCTRL_ENABLE_AMPDU         0x4
#define EAGLE_TXD_XMITCTRL_USE_MC_RATE          0x8     // Use multicast data rate

#define MRVL_PCI_DMA_SIGNATURE			0xCAFEEFAC

typedef struct wlRateInfo_t
{
	u_int32_t	Format:		1;	//0 = Legacy format, 1 = Hi-throughput format
	u_int32_t	ShortGI:	1;	//0 = Use standard guard interval,1 = Use short guard interval
	u_int32_t	Bandwidth:	1;	//0 = Use 20 MHz channel,1 = Use 40 MHz channel
	u_int32_t	RateIDMCS:	7;	//= RateID[3:0]; Legacy format,= MCS[5:0]; HT format
	u_int32_t	AdvCoding:	1;	//AdvCoding 0 = No AdvCoding,1 = LDPC,2 = RS,3 = Reserved
	u_int32_t	AntSelect:	2;	//Bitmap to select one of the transmit antennae
	u_int32_t	ActSubChan:	2;   	//Active subchannel for 40 MHz mode 00:lower, 01= upper, 10= both on lower and upper
	u_int32_t  	Preambletype:1;  	//Preambletype 0= Long, 1= Short;
	u_int32_t 	pid:4; 				// Power ID
	u_int32_t 	ant2: 1; 			// bit 2 of antenna selection field 
	u_int32_t 	ant3: 1;
	u_int32_t 	bf: 1; 				// 0: beam forming off; 1: beam forming on
	u_int32_t 	gf:1; 				// 0: green field off; 1, green field on
	u_int32_t 	count:4;
	u_int32_t 	rsvd2:3;
	u_int32_t 	drop:1;
}__attribute__ ((packed)) wlRateInfo_t;

typedef struct _wltxdesc_t /*__attribute__ ((packed))*/ wltxdesc_t;
struct _wltxdesc_t {
	u_int8_t         DataRate;
	u_int8_t         TxPriority;
	u_int16_t        QosCtrl;
	u_int32_t        PktPtr;
	u_int16_t        PktLen;
	u_int8_t         DestAddr[6];	
	u_int32_t        pPhysNext;
	u_int32_t        SapPktInfo;
	wlRateInfo_t 	RateInfo;
	u_int8_t            type;
	u_int8_t    xmitcontrol;  //bit 0: use rateinfo, bit 1: disable ampdu
	u_int16_t	reserved;
	u_int32_t		tcpack_sn;
	u_int32_t		tcpack_src_dst;
	struct sk_buff  *pSkBuff;
	wltxdesc_t    *pNext;
	u_int32_t        SoftStat;
	u_int32_t ack_wcb_addr;	
	u_int8_t		*staInfo;
    u_int32_t        Status;
} __attribute__ ((packed));

typedef struct HwRssiInfo_t
{
	u_int32_t Rssi_a:   8;
	u_int32_t Rssi_b:   8;
	u_int32_t Rssi_c:   8;
	u_int32_t Rssi_d:   8;
}__attribute__ ((packed)) HwRssiInfo_t1;

typedef struct HwNoiseFloorInfo_t
{
	u_int32_t NoiseFloor_a: 8;
	u_int32_t NoiseFloor_b: 8;
	u_int32_t NoiseFloor_c: 8;
	u_int32_t NoiseFloor_d: 8;
}__attribute__ ((packed)) HwNoiseFloorInfo_t;

typedef struct _wlrxdesc_t /*__attribute__ ((packed))*/ wlrxdesc_t;
struct _wlrxdesc_t {
	u_int16_t        PktLen;         /* total length of received data      */
	u_int8_t         SQ2;            /* unused at the moment               */
	u_int8_t         Rate;           /* received data rate                 */
	u_int32_t        pPhysBuffData;  /* physical address of payload data   */
	u_int32_t        pPhysNext;      /* physical address of next RX desc   */ 
	u_int16_t        QosCtrl;        /* received QosCtrl field variable    */
	u_int16_t        HtSig2;         /* like name states                   */
	struct HwRssiInfo_t HwRssiInfo;
	struct HwNoiseFloorInfo_t HwNoiseFloorInfo ;
	u_int8_t  NoiseFloor;
	u_int8_t reserved[3];
	u_int8_t         RSSI;           /* received signal strengt indication */
	u_int8_t         Status;         /* status field containing USED bit   */
	u_int8_t         Channel;        /* channel this pkt was received on   */
	u_int8_t         RxControl;      /* the control element of the desc    */
	//above are 32bits aligned and is same as FW, RxControl put at end for sync	
	struct sk_buff  *pSkBuff;        /* associated sk_buff for Linux       */
	void            *pBuffData;      /* virtual address of payload data    */ 
	wlrxdesc_t    *pNext;            /* virtual address of next RX desc    */
	
} __attribute__ ((packed));

extern int wlTxRingAlloc(WL_NETDEV *netdev);
extern int wlRxRingAlloc(WL_NETDEV *netdev);
extern int wlTxRingInit(WL_NETDEV *netdev);
extern int wlRxRingInit(WL_NETDEV *netdev);
extern void wlTxRingFree(WL_NETDEV *netdev);
extern void wlRxRingFree(WL_NETDEV *netdev);
extern void wlTxRingCleanup(WL_NETDEV *netdev);
extern void wlRxRingCleanup(WL_NETDEV *netdev);

#endif /* AP8X_DESC_H_ */

