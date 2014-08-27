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

#ifndef _BUILD_MODES_H_
#define _BUILD_MODES_H_

#define OEMSSID "MarvellAP%1x%1x" /* default SSID */
#define OEM_UR_SSID "Marvell_AP" /* default SSID of AP to connect UR */
#define OEMCHANNEL 6/** default channel **/
#define MAX_PMAC 2
#define MAX_STNS  64 /** Maximum allow associated station **/
#define MAX_AID (MAX_STNS*MAX_PMAC)

#define ENABLE_ERP_PROTECTION 1
#define ERP 1
#define DISABLE_B_AP_CHECK
#define CONSECTXFAILLIMIT		500			//Default value. Consecutive tx fail cnt > limit to kick out client. Zero to disable.
#define _CONSECTXFAILLIMIT		500			//Use this when enabling mcastproxy with mib_consectxfaillimit value set as default

/* QOS_FEATURE */
//#define RX_QOSDATA_SUPPORT 
//#define ETHER_RX_QOSDATA_SUPPORT
#ifdef UAPSD
#define WMM_PS_SUPPORT
#endif

#define MAX_CARDS_SUPPORT 2
#define MAX_VMAC_INSTANCE_AP	NUMOFAPS
#define MAX_VMAC_MIB_INSTANCE	NUMOFAPS+1


#endif /* _BUILD_MODES_H_ */
