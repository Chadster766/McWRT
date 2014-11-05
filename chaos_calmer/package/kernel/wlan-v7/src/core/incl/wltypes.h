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

#ifndef _WLTYPES_H_
#define _WLTYPES_H_ 

/*!
* \file    wltypes.h
* \brief   Basic types common to all the modules must be defined in this file.
*
*/

#define OS_LINUX

#if defined(OS_VxW)
#include "vxWorks.h"
#include <stdbool.h>
#endif




#ifdef OS_LINUX
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

#define free   kfree
#define malloc(x) kmalloc(x,GFP_KERNEL)
#endif

#define PACK   __attribute__ ((packed))
#define UNUSED __attribute__ ((unused)) 

/*! Create type names for native C types for portability reasons */
#ifndef UINT64
typedef unsigned long long  UINT64;
#endif
#ifndef SINT64
typedef signed long long    SINT64;
#endif
#ifndef UINT32
#ifndef OS_VxW
typedef unsigned long       UINT32;
#endif
#endif
#ifndef SINT32
typedef signed long         SINT32;
#endif
#ifndef UINT16
typedef unsigned short      UINT16;
#endif
#ifndef SINT16
typedef signed short        SINT16;
#endif
#ifndef UINT8
typedef unsigned char       UINT8;
#endif
#ifndef SINT8
typedef signed char         SINT8;
#endif
#ifndef BOOLEAN
typedef int                 BOOLEAN;
#endif
typedef signed int          BIT_FIELD;

//typedef int bool;

/*! Type name for error code returned by most functions in the API.
*
* In general, API functions return either BOOLEAN or WL_STATUS
* BOOLEAN API functions return TRUE when the function was successful and return
* FALSE when the function failed.
*
* WL_STATUS API functions return WL_STATUS_OK when the function was successful
* and return a error code that is less than 0 when the function failed.
*/
typedef int WL_STATUS;

/*! Generic status code */
#define WL_STATUS_OK         0
#define WL_STATUS_ERR       -1
#define WL_STATUS_BAD_PARAM -2

/*! BOOLEAN values */
#define FALSE 0
#define TRUE  1
typedef enum
{
	WL_FALSE,
	WL_TRUE,
} Bool_e;
typedef UINT8 Bool_t;

typedef enum
{
	SUCCESS,
	FAIL,
	TIMEOUT
} Status_e;

/*! Value for NULL pointer */
#if defined(NULL)
#undef NULL
#endif
#define NULL ((void *)0)
#define  PACK_START
#define PACK_END   __attribute__((__packed__))

#ifdef __GNUC__
#define ALLIGNED_START(x)
#define ALLIGNED_END(x) __attribute__ ((aligned(x)))
#else
#define ALLIGNED_START(x) __align(x)
#define ALLIGNED_END(x)
#endif

#ifdef __GNUC__
#define INLINE inline
#else
#define INLINE
#endif

#ifdef ECOS
#define SECTION_ITCM __attribute__ ((section (".itcmbuf")))
#else
#define SECTION_ITCM
#endif

#ifdef BE /* Big Endian */
#define SHORT_SWAP(X) (X)
#define WORD_SWAP(X) (X)
#define LONG_SWAP(X) ((l64)(X))

#else    /* Little Endian */

#define SHORT_SWAP(X) ((X <<8 ) | (X >> 8))

#define WORD_SWAP(X) (((X)&0xff)<<24)+      \
	(((X)&0xff00)<<8)+      \
	(((X)&0xff0000)>>8)+    \
	(((X)&0xff000000)>>24)

#define LONG_SWAP(X) ( (l64) (((X)&0xffULL)<<56)+               \
	(((X)&0xff00ULL)<<40)+              \
	(((X)&0xff0000ULL)<<24)+            \
	(((X)&0xff000000ULL)<<8)+           \
	(((X)&0xff00000000ULL)>>8)+         \
	(((X)&0xff0000000000ULL)>>24)+      \
	(((X)&0xff000000000000ULL)>>40)+    \
	(((X)&0xff00000000000000ULL)>>56))
#endif
typedef unsigned char WRAPUINT64[8];

#define WL_WLAN_TYPE_AMPDU (1<<0)
#define WL_WLAN_TYPE_AMSDU (1<<1)
#define WL_WLAN_TYPE_AP (1<<2)
#define WL_WLAN_TYPE_WDS (1<<3)
#define WL_WLAN_TYPE_STA (1<<4)

//the following bits are defined for pMib_11nAggrMode
#define WL_MODE_AMSDU_TX 0x01
#define WL_MODE_AMSDU_TX_4K 0x01
#define WL_MODE_AMSDU_TX_8K 0x02
#define WL_MODE_AMSDU_TX_MASK 0x3
#define WL_MODE_AMPDU_TX 0x04


typedef enum
{
	PREAMBLE_LONG =1,
	PREAMBLE_SHORT=3,
	PREAMBLE_AUTO_SELECT=5,
} PREAMBLE_TYPE;

/*
    bit0: B mode
    bit1: G mode
    bit2: N mode
    bit3: A mode
    bit4: 11ac mode
*/
#define AP_MODE_BAND_MASK   0xF
typedef enum
{
	AP_MODE_B_ONLY = 1,
	AP_MODE_G_ONLY,
	AP_MODE_MIXED,
	AP_MODE_N_ONLY,
	AP_MODE_BandN,
	AP_MODE_GandN,
	AP_MODE_BandGandN,
	AP_MODE_A_ONLY,
	AP_MODE_AandG = 10,
	AP_MODE_AandN = 12,
	AP_MODE_5GHZ_N_ONLY = 13,
	AP_MODE_11AC = 0x10,                  //generic 11ac indication mode	
    AP_MODE_2_4GHZ_11AC_MIXED = 0x17,
    AP_MODE_5GHZ_11AC_ONLY = 0x18,
    AP_MODE_5GHZ_Nand11AC = 0x1C,
}AP_RATE_MODE;

typedef enum {
	COUNTER_MEASURE_START,
	COUNTER_MEASURE_STOP,
} COUNTER_MEASURE_EVENT;

typedef enum
{
	CLIENT_MODE_DISABLE,
	CLIENT_MODE_B,
	CLIENT_MODE_G,
	CLIENT_MODE_BG,
	CLIENT_MODE_A,
	CLIENT_MODE_N,
	CLIENT_MODE_AUTO,
	CLIENT_MODE_N_24,
	CLIENT_MODE_N_5,
}CLIENT_MODE;


typedef	enum
{
	/* please do not add #ifdef inside of this enum */
	APCMD_BSS_START                  = 0x00000001       ,/* 0x00000001 */
	APCMD_BSS_START_REPLY                               ,/* 0x00000002 */
	APCMD_GET_SSID                                      ,/* 0x00000003 */
	APCMD_GET_SSID_REPLY                                ,/* 0x00000004 */
	APCMD_SET_SSID                                      ,/* 0x00000005 */
	APCMD_SET_SSID_REPLY                                ,/* 0x00000006 */
	APCMD_GET_OPEN_SSID_EN_STATE                        ,/* 0x00000007 */
	APCMD_GET_OPEN_SSID_EN_STATE_REPLY                  ,/* 0x00000008 */
	APCMD_SET_OPEN_SSID_EN_STATE                        ,/* 0x00000009 */
	APCMD_SET_OPEN_SSID_EN_STATE_REPLY                  ,/* 0x0000000a */
	APCMD_GET_RF_MAC_ADDRESS                            ,/* 0x0000000b */
	APCMD_GET_RF_MAC_ADDRESS_REPLY                      ,/* 0x0000000c */
	APCMD_SET_RF_MAC_ADDRESS                            ,/* 0x0000000d */
	APCMD_SET_RF_MAC_ADDRESS_REPLY                      ,/* 0x0000000e */
	APCMD_GET_RF_CHANNEL                                ,/* 0x0000000f */
	APCMD_GET_RF_CHANNEL_REPLY                          ,/* 0x00000010 */
	APCMD_SET_RF_CHANNEL                                ,/* 0x00000011 */
	APCMD_SET_RF_CHANNEL_REPLY                          ,/* 0x00000012 */
	APCMD_GET_BASIC_DATA_RATES                          ,/* 0x00000013 */
	APCMD_GET_BASIC_DATA_RATES_REPLY                    ,/* 0x00000014 */
	APCMD_SET_BASIC_DATA_RATES                          ,/* 0x00000015 */
	APCMD_SET_BASIC_DATA_RATES_REPLY                    ,/* 0x00000016 */
	APCMD_GET_SUP_DATA_RATES                            ,/* 0x00000017 */
	APCMD_GET_SUP_DATA_RATES_REPLY                      ,/* 0x00000018 */
	APCMD_SET_SUP_DATA_RATES                            ,/* 0x00000019 */
	APCMD_SET_SUP_DATA_RATES_REPLY                      ,/* 0x0000001a */
	APCMD_GET_TX_DATA_RATE                              ,/* 0x0000001b */
	APCMD_GET_TX_DATA_RATE_REPLY                        ,/* 0x0000001c */
	APCMD_SET_TX_DATA_RATE                              ,/* 0x0000001d */
	APCMD_SET_TX_DATA_RATE_REPLY                        ,/* 0x0000001e */
	APCMD_GET_TX_POWER_LEVEL                            ,/* 0x0000001f */
	APCMD_GET_TX_POWER_LEVEL_REPLY                      ,/* 0x00000020 */
	APCMD_SET_TX_POWER_LEVEL                            ,/* 0x00000021 */
	APCMD_SET_TX_POWER_LEVEL_REPLY                      ,/* 0x00000022 */
	APCMD_GET_RF_PREAMBLE_OPTION                        ,/* 0x00000023 */
	APCMD_GET_RF_PREAMBLE_OPTION_REPLY                  ,/* 0x00000024 */
	APCMD_SET_RF_PREAMBLE_OPTION                        ,/* 0x00000025 */
	APCMD_SET_RF_PREAMBLE_OPTION_REPLY                  ,/* 0x00000026 */
	APCMD_GET_ANTENNA_MODE                              ,/* 0x00000027 */
	APCMD_GET_ANTENNA_MODE_REPLY                        ,/* 0x00000028 */
	APCMD_SET_ANTENNA_MODE                              ,/* 0x00000029 */
	APCMD_SET_ANTENNA_MODE_REPLY                        ,/* 0x0000002a */
	APCMD_GET_FRAG_THRESHOLD                            ,/* 0x0000002b */
	APCMD_GET_FRAG_THRESHOLD_REPLY                      ,/* 0x0000002c */
	APCMD_SET_FRAG_THRESHOLD                            ,/* 0x0000002d */
	APCMD_SET_FRAG_THRESHOLD_REPLY                      ,/* 0x0000002e */
	APCMD_GET_RTS_THRESHOLD                             ,/* 0x0000002f */
	APCMD_GET_RTS_THRESHOLD_REPLY                       ,/* 0x00000030 */
	APCMD_SET_RTS_THRESHOLD                             ,/* 0x00000031 */
	APCMD_SET_RTS_THRESHOLD_REPLY                       ,/* 0x00000032 */
	APCMD_GET_RTS_RETRY_LIMIT                           ,/* 0x00000033 */
	APCMD_GET_RTS_RETRY_LIMIT_REPLY                     ,/* 0x00000034 */
	APCMD_SET_RTS_RETRY_LIMIT                           ,/* 0x00000035 */
	APCMD_SET_RTS_RETRY_LIMIT_REPLY                     ,/* 0x00000036 */
	APCMD_GET_DATA_RETRY_LIMIT                          ,/* 0x00000037 */
	APCMD_GET_DATA_RETRY_LIMIT_REPLY                    ,/* 0x00000038 */
	APCMD_SET_DATA_RETRY_LIMIT                          ,/* 0x00000039 */
	APCMD_SET_DATA_RETRY_LIMIT_REPLY                    ,/* 0x0000003a */
	APCMD_GET_BEACON_PERIOD                             ,/* 0x0000003b */
	APCMD_GET_BEACON_PERIOD_REPLY                       ,/* 0x0000003c */
	APCMD_SET_BEACON_PERIOD                             ,/* 0x0000003d */
	APCMD_SET_BEACON_PERIOD_REPLY                       ,/* 0x0000003e */
	APCMD_GET_DTIM_PERIOD                               ,/* 0x0000003f */
	APCMD_GET_DTIM_PERIOD_REPLY                         ,/* 0x00000040 */
	APCMD_SET_DTIM_PERIOD                               ,/* 0x00000041 */
	APCMD_SET_DTIM_PERIOD_REPLY                         ,/* 0x00000042 */
	APCMD_GET_802_11_STATS                              ,/* 0x00000043 */
	APCMD_GET_802_11_STATS_REPLY                        ,/* 0x00000044 */
	APCMD_ADD_WLAN_FILTER_ENTRY                         ,/* 0x00000045 */
	APCMD_ADD_WLAN_FILTER_ENTRY_REPLY                   ,/* 0x00000046 */
	APCMD_DEL_WLAN_FILTER_ENTRY                         ,/* 0x00000047 */
	APCMD_DEL_WLAN_FILTER_ENTRY_REPLY                   ,/* 0x00000048 */
	APCMD_GET_STA_LIST                                  ,/* 0x00000049 */
	APCMD_GET_STA_LIST_REPLY                            ,/* 0x0000004a */
	APCMD_GET_STA_INFO                                  ,/* 0x0000004b */
	APCMD_GET_STA_INFO_REPLY                            ,/* 0x0000004c */
	APCMD_DEAUTH_STA                                    ,/* 0x0000004d */
	APCMD_DEAUTH_STA_REPLY                              ,/* 0x0000004e */
	APCMD_DISASSOC_STA                                  ,/* 0x0000004f */
	APCMD_DISASSOC_STA_REPLY                            ,/* 0x00000050 */
	APCMD_ADD_ASSOC_FILTER_ENTRY                        ,/* 0x00000051 */
	APCMD_ADD_ASSOC_FILTER_ENTRY_REPLY                  ,/* 0x00000052 */
	APCMD_DEL_ASSOC_FILTER_ENTRY                        ,/* 0x00000053 */
	APCMD_DEL_ASSOC_FILTER_ENTRY_REPLY                  ,/* 0x00000054 */
	APCMD_GET_MAX_NUM_STA                               ,/* 0x00000055 */
	APCMD_GET_MAX_NUM_STA_REPLY                         ,/* 0x00000056 */
	APCMD_SET_MAX_NUM_STA                               ,/* 0x00000057 */
	APCMD_SET_MAX_NUM_STA_REPLY                         ,/* 0x00000058 */
	APCMD_GET_DEFAULT_WEP_KEYS                          ,/* 0x00000059 */
	APCMD_GET_DEFAULT_WEP_KEYS_REPLY                    ,/* 0x0000005a */
	APCMD_SET_DEFAULT_WEP_KEYS                          ,/* 0x0000005b */
	APCMD_SET_DEFAULT_WEP_KEYS_REPLY                    ,/* 0x0000005c */
	APCMD_SET_STA_WEP_KEY                               ,/* 0x0000005d */
	APCMD_SET_STA_WEP_KEY_REPLY                         ,/* 0x0000005e */
	APCMD_GET_PRIVACY_OPTION                            ,/* 0x0000005f */
	APCMD_GET_PRIVACY_OPTION_REPLY                      ,/* 0x00000060 */
	APCMD_SET_PRIVACY_OPTION                            ,/* 0x00000061 */
	APCMD_SET_PRIVACY_OPTION_REPLY                      ,/* 0x00000062 */
	APCMD_GET_AUTH_MODE                                 ,/* 0x00000063 */
	APCMD_GET_AUTH_MODE_REPLY                           ,/* 0x00000064 */
	APCMD_SET_AUTH_MODE                                 ,/* 0x00000065 */
	APCMD_SET_AUTH_MODE_REPLY                           ,/* 0x00000066 */
	APCMD_GET_ETH_MAC_ADDRESS                           ,/* 0x00000067 */
	APCMD_GET_ETH_MAC_ADDRESS_REPLY                     ,/* 0x00000068 */
	APCMD_GET_ETH_IP_CONFIG                             ,/* 0x00000069 */
	APCMD_GET_ETH_IP_CONFIG_REPLY                       ,/* 0x0000006a */
	APCMD_SET_ETH_IP_CONFIG                             ,/* 0x0000006b */
	APCMD_SET_ETH_IP_CONFIG_REPLY                       ,/* 0x0000006c */
	/* Added wireless lan and WAN ip info for VxWorks. */
	APCMD_GET_WL_IP_CONFIG                              ,/* 0x0000006d */
	APCMD_GET_WL_IP_CONFIG_REPLY                        ,/* 0x0000006e */
	APCMD_SET_WL_IP_CONFIG                              ,/* 0x0000006f */
	APCMD_SET_WL_IP_CONFIG_REPLY                        ,/* 0x00000070 */

	APCMD_GET_WAN_IP_CONFIG                             ,/* 0x00000071 */
	APCMD_GET_WAN_IP_CONFIG_REPLY                       ,/* 0x00000072 */
	APCMD_SET_WAN_IP_CONFIG                             ,/* 0x00000073 */
	APCMD_SET_WAN_IP_CONFIG_REPLY                       ,/* 0x00000074 */
	/* Added wireless lan and WAN ip info for VxWorks. */

	APCMD_GET_ETH_PORT_STATUS                           ,/* 0x00000075 */
	APCMD_GET_ETH_PORT_STATUS_REPLY                     ,/* 0x00000076 */
	APCMD_SET_ETH_PORT_PARAMS                           ,/* 0x00000077 */
	APCMD_SET_ETH_PORT_PARAMS_REPLY                     ,/* 0x00000078 */
	APCMD_GET_ICMP_EN_STATE                             ,/* 0x00000079 */
	APCMD_GET_ICMP_EN_STATE_REPLY                       ,/* 0x0000007a */
	APCMD_SET_ICMP_EN_STATE                             ,/* 0x0000007b */
	APCMD_SET_ICMP_EN_STATE_REPLY                       ,/* 0x0000007c */
	APCMD_GET_802_3_STATES                              ,/* 0x0000007d */
	APCMD_GET_802_3_STATES_REPLY                        ,/* 0x0000007e */
	APCMD_READ_GPIO                                     ,/* 0x0000007f */
	APCMD_READ_GPIO_REPLY                               ,/* 0x00000080 */
	APCMD_WRITE_GPIO                                    ,/* 0x00000081 */
	APCMD_WRITE_GPIO_REPLY                              ,/* 0x00000082 */
	APCMD_SET_ETH_MAC_ADDRESS                           ,/* 0x00000083 */
	APCMD_SET_ETH_MAC_ADDRESS_REPLY                     ,/* 0x00000084 */
	APCMD_GET_WAN_MAC_ADDRESS                           ,/* 0x00000085 */
	APCMD_GET_WAN_MAC_ADDRESS_REPLY                     ,/* 0x00000086 */
	APCMD_SET_WAN_MAC_ADDRESS                           ,/* 0x00000087 */
	APCMD_SET_WAN_MAC_ADDRESS_REPLY                     ,/* 0x00000088 */
	APCMD_FW_SOFT_RESET                                 ,/* 0x00000089 */
	APCMD_FW_SOFT_RESET_REPLY                           ,/* 0x0000008a */
	APCMD_GET_WLAN_FILTER_LIST                          ,/* 0x0000008b */
	APCMD_GET_WLAN_FILTER_LIST_REPLY                    ,/* 0x0000008c */
	APCMD_SET_WLAN_FILTER_LIST                          ,/* 0x0000008d */
	APCMD_SET_WLAN_FILTER_LIST_REPLY                    ,/* 0x0000008e */
	APCMD_GET_WLAN_FILTER_MODE                          ,/* 0x0000008f */
	APCMD_GET_WLAN_FILTER_MODE_REPLY                    ,/* 0x00000090 */
	APCMD_SET_WLAN_FILTER_MODE                          ,/* 0x00000091 */
	APCMD_SET_WLAN_FILTER_MODE_REPLY                    ,/* 0x00000092 */
	APCMD_UPDATE_FIRMWARE                               ,/* 0x00000093 */
	APCMD_UPDATE_FIRMWARE_REPLY                         ,/* 0x00000094 */
	APCMD_SAVE_USER_CONFIG                              ,/* 0x00000095 */
	APCMD_SAVE_USER_CONFIG_REPLY                        ,/* 0x00000096 */
	APCMD_SET_STA_AGING_TIME_IN_MINUTES                 ,/* 0x00000097 */
	APCMD_SET_STA_AGING_TIME_IN_MINUTES_REPLY           ,/* 0x00000098 */
	APCMD_GET_FIRMWARE_VERSION                          ,/* 0x00000099 */
	APCMD_GET_FIRMWARE_VERSION_REPLY                    ,/* 0x0000009a */
	APCMD_SET_BEACON_TX_MODE                            ,/* 0x0000009b */
	APCMD_SET_BEACON_TX_MODE_REPLY                      ,/* 0x0000009c */
	APCMD_SET_RATE_ADAPTATION_METHOD                    ,/* 0x0000009d */
	APCMD_SET_RATE_ADAPTATION_METHOD_REPLY              ,/* 0x0000009e */
	APCMD_GET_RATE_ADAPTATION_PARAMS                    ,/* 0x0000009f */
	APCMD_GET_RATE_ADAPTATION_PARAMS_REPLY              ,/* 0x000000a0 */
	APCMD_SET_RATE_ADAPTATION_PARAMS                    ,/* 0x000000a1 */
	APCMD_SET_RATE_ADAPTATION_PARAMS_REPLY              ,/* 0x000000a2 */
	APCMD_SET_SDRAM_BURST_MODE                          ,/* 0x000000a3 */
	APCMD_SET_SDRAM_BURST_MODE_REPLY                    ,/* 0x000000a4 */
	APCMD_GET_DEVICE_NAME                               ,/* 0x000000a5 */
	APCMD_GET_DEVICE_NAME_REPLY                         ,/* 0x000000a6 */
	APCMD_SET_DEVICE_NAME                               ,/* 0x000000a7 */
	APCMD_SET_DEVICE_NAME_REPLY                         ,/* 0x000000a8 */

	/* GET/SET Baseband Reg */
	APCMD_GET_BB_REG                                    ,/* 0x000000a9 */
	APCMD_GET_BB_REG_REPLY                              ,/* 0x000000aa */
	APCMD_SET_BB_REG                                    ,/* 0x000000ab */
	APCMD_SET_BB_REG_REPLY                              ,/* 0x000000ac */
	/* GET/SET Baseband Reg end */

	APCMD_GET_CUR_RADIO_TX_RATE                         ,/* 0x000000ad */
	APCMD_GET_CUR_RADIO_TX_RATE_REPLY                   ,/* 0x000000ae */
	/* GET/SET 11g commands */
	APCMD_GET_BASIC_DATA_RATES_G                        ,/* 0x000000af */
	APCMD_GET_BASIC_DATA_RATES_G_REPLY                  ,/* 0x000000b0 */
	APCMD_SET_BASIC_DATA_RATES_G                        ,/* 0x000000b1 */
	APCMD_SET_BASIC_DATA_RATES_G_REPLY                  ,/* 0x000000b2 */
	APCMD_GET_TX_DATA_RATE_G                            ,/* 0x000000b3 */
	APCMD_GET_TX_DATA_RATE_G_REPLY                      ,/* 0x000000b4 */
	APCMD_SET_TX_DATA_RATE_G                            ,/* 0x000000b5 */
	APCMD_SET_TX_DATA_RATE_G_REPLY                      ,/* 0x000000b6 */
	APCMD_GET_AP_MODE                                   ,/* 0x000000b7 */
	APCMD_GET_AP_MODE_REPLY                             ,/* 0x000000b8 */
	APCMD_SET_AP_MODE                                   ,/* 0x000000b9 */
	APCMD_SET_AP_MODE_REPLY                             ,/* 0x000000ba */
	APCMD_GET_STA_LIST_G                                ,/* 0x000000bb */
	APCMD_GET_STA_LIST_G_REPLY                          ,/* 0x000000bc */
	APCMD_GET_STA_INFO_G                                ,/* 0x000000bd */
	APCMD_GET_STA_INFO_G_REPLY                          ,/* 0x000000be */
	/*GET/SET 11g commands end */
	APCMD_GET_ETH_CONFIG                                ,/* 0x000000bf */
	APCMD_GET_ETH_CONFIG_REPLY                          ,/* 0x000000c0 */
	APCMD_SET_ETH_CONFIG                                ,/* 0x000000c1 */
	APCMD_SET_ETH_CONFIG_REPLY                          ,/* 0x000000c2 */
	APCMD_GET_PRIVACY_TBL_RSN_ENABLED                   ,/* 0x000000c3 */
	APCMD_GET_PRIVACY_TBL_RSN_ENABLED_REPLY             ,/* 0x000000c4 */
	APCMD_SET_PRIVACY_TBL_RSN_ENABLED                   ,/* 0x000000c5 */
	APCMD_SET_PRIVACY_TBL_RSN_ENABLED_REPLY             ,/* 0x000000c6 */
	APCMD_GET_RSN_CFG_AUTH_SUITE                        ,/* 0x000000c7 */
	APCMD_GET_RSN_CFG_AUTH_SUITE_REPLY                  ,/* 0x000000c8 */
	APCMD_SET_RSN_CFG_AUTH_SUITE                        ,/* 0x000000c9 */
	APCMD_SET_RSN_CFG_AUTH_SUITE_REPLY                  ,/* 0x000000ca */
	APCMD_GET_RSN_CFG_UNICAST_CIPHERS                   ,/* 0x000000cb */
	APCMD_GET_RSN_CFG_UNICAST_CIPHERS_REPLY             ,/* 0x000000cc */
	APCMD_SET_RSN_CFG_UNICAST_CIPHERS                   ,/* 0x000000cd */
	APCMD_SET_RSN_CFG_UNICAST_CIPHERS_REPLY             ,/* 0x000000ce */
	APCMD_SET_RSN_CFG_MULTICAST_CIPHERS                 ,/* 0x000000cf */
	APCMD_SET_RSN_CFG_MULTICAST_REPLY                   ,/* 0x000000d0 */
	APCMD_GET_RSN_CFG                                   ,/* 0x000000d1 */
	APCMD_GET_RSN_CFG_REPLY                             ,/* 0x000000d2 */
	APCMD_SET_PSK_PASSPHRASE                            ,/* 0x000000d3 */
	APCMD_SET_PSK_PASSPHRASE_REPLY                      ,/* 0x000000d4 */
	APCMD_SET_GRP_REKEY_TIME                            ,/* 0x000000d5 */
	APCMD_SET_GRP_REKEY_TIME_REPLY                      ,/* 0x000000d6 */
	APCMD_GET_RSN_STATS                                 ,/* 0x000000d7 */
	APCMD_GET_RSN_STATS_REPLY                           ,/* 0x000000d8 */

	APCMD_GET_WDS_MODE                                  ,/* 0x000000d9 */
	APCMD_GET_WDS_MODE_REPLY                            ,/* 0x000000da */
	APCMD_SET_WDS_MODE                                  ,/* 0x000000db */
	APCMD_SET_WDS_MODE_REPLY                            ,/* 0x000000dc */
	APCMD_GET_WDS_LIST                                  ,/* 0x000000dd */
	APCMD_GET_WDS_LIST_REPLY                            ,/* 0x000000de */
	APCMD_SET_WDS_LIST                                  ,/* 0x000000df */
	APCMD_SET_WDS_LIST_REPLY                            ,/* 0x000000e0 */
	APCMD_GET_DISABLE_ASSOC                             ,/* 0x000000e1 */
	APCMD_GET_DISABLE_ASSOC_REPLY                       ,/* 0x000000e2 */
	APCMD_SET_DISABLE_ASSOC                             ,/* 0x000000e3 */
	APCMD_SET_DISABLE_ASSOC_REPLY                       ,/* 0x000000e4 */
	/* WDS interop option */
	APCMD_GET_WDS_OPTION                                ,/* 0x000000e5 */
	APCMD_GET_WDS_OPTION_REPLY                          ,/* 0x000000e6 */
	APCMD_SET_WDS_OPTION                                ,/* 0x000000e7 */
	APCMD_SET_WDS_OPTION_REPLY                          ,/* 0x000000e8 */

	APCMD_GET_FORCE_PROTECTION_MODE                     ,/* 0x000000e9 */
	APCMD_GET_FORCE_PROTECTION_MODE_REPLY               ,/* 0x000000ea */
	APCMD_SET_FORCE_PROTECTION_MODE                     ,/* 0x000000eb */
	APCMD_SET_FORCE_PROTECTION_MODE_REPLY               ,/* 0x000000ec */

	/* WiPod 5-in-1 */
	APCMD_GET_WIPOD_MODE                                ,/* 0x000000ed */
	APCMD_GET_WIPOD_MODE_REPLY                          ,/* 0x000000ee */
	APCMD_SET_WIPOD_MODE                                ,/* 0x000000ef */
	APCMD_SET_WIPOD_MODE_REPLY                          ,/* 0x000000f0 */

	APCMD_GET_BURST_MODE                                ,/* 0x000000f1 */
	APCMD_GET_BURST_MODE_REPLY                          ,/* 0x000000f2 */
	APCMD_SET_BURST_MODE                                ,/* 0x000000f3 */
	APCMD_SET_BURST_MODE_REPLY                          ,/* 0x000000f4 */

	APCMD_SET_SURVEY_SCAN                               ,/* 0x000000f5 */
	APCMD_SET_SURVEY_SCAN_REPLY                         ,/* 0x000000f6 */
	APCMD_GET_SURVEY_INFO                               ,/* 0x000000f7 */
	APCMD_GET_SURVEY_INFO_REPLY                         ,/* 0x000000f8 */
	APCMD_SET_SELF_CTS                                  ,/* 0x000000f9 */
	APCMD_SET_SELF_CTS_REPLY                            ,/* 0x000000fa */
	APCMD_GET_SELF_CTS                                  ,/* 0x000000fb */
	APCMD_GET_SELF_CTS_REPLY                            ,/* 0x000000fc */
	APCMD_SET_RF_POWER_MODE                             ,/* 0x000000fd */
	APCMD_SET_RF_POWER_MODE_REPLY                       ,/* 0x000000fe */
	APCMD_SET_AUTO_LINK_MODE                            ,/* 0x000000ff */
	APCMD_SET_AUTO_LINK_MODE_REPLY                      ,/* 0x00000100 */
	APCMD_GET_AUTO_LINK_MODE                            ,/* 0x00000101 */
	APCMD_GET_AUTO_LINK_MODE_REPLY                      ,/* 0x00000102 */
	APCMD_GET_STA_CFG_RSN_OPTION_IMP                    ,/* 0x00000103 */
	APCMD_GET_STA_CFG_RSN_OPTION_IMP_REPLY              ,/* 0x00000104 */
	APCMD_GET_STA_CFG_TKIP_NUM_RPLY_COUNTERS            ,/* 0x00000105 */
	APCMD_GET_STA_CFG_TKIP_NUM_RPLY_COUNTERS_REPLY      ,/* 0x00000106 */
	APCMD_GET_RSN_CFG_TBL                               ,/* 0x00000107 */
	APCMD_GET_RSN_CFG_TBL_REPLY                         ,/* 0x00000108 */
	APCMD_SET_RSN_CFG_TBL                               ,/* 0x00000109 */
	APCMD_SET_RSN_CFG_TBL_REPLY                         ,/* 0x0000010a */
	APCMD_SET_RSN_DATA_TRAFFIC_ENABLED                  ,/* 0x0000010b */
	APCMD_SET_RSN_DATA_TRAFFIC_ENABLED_REPLY            ,/* 0x0000010c */
	APCMD_SET_RSN_PWK                                   ,/* 0x0000010d */
	APCMD_SET_RSN_PWK_REPLY                             ,/* 0x0000010e */
	APCMD_GET_RSN_PWK                                   ,/* 0x0000010f */
	APCMD_GET_RSN_PWK_REPLY                             ,/* 0x00000110 */
	APCMD_SET_RSN_GRP_KEY                               ,/* 0x00000111 */
	APCMD_SET_RSN_GRP_KEY_REPLY                         ,/* 0x00000112 */
	APCMD_GET_RSN_GRP_KEY                               ,/* 0x00000113 */
	APCMD_GET_RSN_GRP_KEY_REPLY                         ,/* 0x00000114 */
	APCMD_GET_THIS_STA_RSN_IE                           ,/* 0x00000115 */
	APCMD_GET_THIS_STA_RSN_IE_REPLY                     ,/* 0x00000116 */
	APCMD_GET_EXT_STA_RSN_IE                            ,/* 0x00000117 */
	APCMD_GET_EXT_STA_RSN_IE_REPLY                      ,/* 0x00000118 */
	APCMD_GET_PAIRWISE_TSC                              ,/* 0x00000119 */
	APCMD_GET_PAIRWISE_TSC_REPLY                        ,/* 0x0000011a */
	APCMD_SET_PAIRWISE_TSC                              ,/* 0x0000011b */
	APCMD_SET_PAIRWISE_TSC_REPLY                        ,/* 0x0000011c */
	APCMD_GET_GRP_TSC                                   ,/* 0x0000011d */
	APCMD_GET_GRP_TSC_REPLY                             ,/* 0x0000011e */
	APCMD_SET_GRP_TSC                                   ,/* 0x0000011f */
	APCMD_SET_GRP_TSC_REPLY                             ,/* 0x00000120 */
	APCMD_SET_COUNTER_MEASURE_ENABLED                   ,/* 0x00000121 */
	APCMD_SET_COUNTER_MEASURE_ENABLED_REPLY             ,/* 0x00000122 */
	APCMD_SET_RSN_PMK                                   ,/* 0x00000123 */
	APCMD_SET_RSN_PMK_REPLY                             ,/* 0x00000124 */

	/* WPA2 */
	APCMD_GET_WPA2_ENABLED                              ,/* 0x00000125 */
	APCMD_GET_WPA2_ENABLED_REPLY                        ,/* 0x00000126 */
	APCMD_SET_WPA2_ENABLED                              ,/* 0x00000127 */
	APCMD_SET_WPA2_ENABLED_REPLY                        ,/* 0x00000128 */
	APCMD_GET_WPA2ONLY_ENABLED                          ,/* 0x00000129 */
	APCMD_GET_WPA2ONLY_ENABLED_REPLY                    ,/* 0x0000012a */
	APCMD_SET_WPA2ONLY_ENABLED                          ,/* 0x0000012b */
	APCMD_SET_WPA2ONLY_ENABLED_REPLY                    ,/* 0x0000012c */
	APCMD_GET_RSN_CFG_WPA2_TBL                          ,/* 0x0000012d */
	APCMD_GET_RSN_CFG_WPA2_TBL_REPLY                    ,/* 0x0000012e */
	APCMD_SET_RSN_CFG_WPA2_TBL                          ,/* 0x0000012f */
	APCMD_SET_RSN_CFG_WPA2_TBL_REPLY                    ,/* 0x00000130 */
	APCMD_GET_RSN_CFG_WPA2_UNICAST_CIPHER               ,/* 0x00000131 */
	APCMD_GET_RSN_CFG_WPA2_UNICAST_CIPHER_REPLY         ,/* 0x00000132 */
	APCMD_SET_RSN_CFG_WPA2_UNICAST_CIPHER               ,/* 0x00000133 */
	APCMD_SET_RSN_CFG_WPA2_UNICAST_CIPHER_REPLY         ,/* 0x00000134 */
	APCMD_GET_RSN_CFG_WPA2_AUTH_SUITE                   ,/* 0x00000135 */
	APCMD_GET_RSN_CFG_WPA2_AUTH_SUITE_REPLY             ,/* 0x00000136 */
	APCMD_SET_RSN_CFG_WPA2_AUTH_SUITE                   ,/* 0x00000137 */
	APCMD_SET_RSN_CFG_WPA2_AUTH_SUITE_REPLY             ,/* 0x00000138 */
	APCMD_GET_THIS_STA_WPA2_RSN_IE                      ,/* 0x00000139 */
	APCMD_GET_THIS_STA_WPA2_RSN_IE_REPLY                ,/* 0x0000013a */
	APCMD_GET_RSN_CFG_WPA2_UNICAST_CIPHER2              ,/* 0x0000013b */
	APCMD_GET_RSN_CFG_WPA2_UNICAST_CIPHER2_REPLY        ,/* 0x0000013c */
	APCMD_SET_RSN_CFG_WPA2_UNICAST_CIPHER2              ,/* 0x0000013d */
	APCMD_SET_RSN_CFG_WPA2_UNICAST_CIPHER2_REPLY        ,/* 0x0000013e */
	APCMD_SET_WPA2_PREAUTH_ENABLED                      ,/* 0x0000013f */
	APCMD_SET_WPA2_PREAUTH_ENABLED_REPLY                ,/* 0x00000140 */
	APCMD_SET_CLOCK                                     ,/* 0x00000141 */
	APCMD_SET_CLOCK_REPLY                               ,/* 0x00000142 */
	APCMD_GET_CLOCK                                     ,/* 0x00000143 */
	APCMD_GET_CLOCK_REPLY                               ,/* 0x00000144 */
	APCMD_GET_UREPEATER                                 ,/* 0x00000145 */
	APCMD_GET_UREPEATER_REPLY                           ,/* 0x00000146 */
	APCMD_SET_UREPEATER                                 ,/* 0x00000147 */
	APCMD_SET_UREPEATER_REPLY                           ,/* 0x00000148 */
	APCMD_SET_BOOSTER_MODE                              ,/* 0x00000149 */
	APCMD_SET_BOOSTER_MODE_REPLY                        ,/* 0x0000014a */
	APCMD_GET_BOOSTER_MODE                              ,/* 0x0000014b */
	APCMD_GET_BOOSTER_MODE_REPLY                        ,/* 0x0000014c */
	APCMD_SET_SWITCH_IP_PRIO_MAP_EN                     ,/* 0x0000014d */
	APCMD_SET_SWITCH_IP_PRIO_MAP_EN_REPLY               ,/* 0x0000014e */
	APCMD_GET_SWITCH_IP_PRIO_MAP_EN                     ,/* 0x0000014f */
	APCMD_GET_SWITCH_IP_PRIO_MAP_EN_REPLY               ,/* 0x00000150 */
	APCMD_SET_SWITCH_USER_PRIO_MAP_EN                   ,/* 0x00000151 */
	APCMD_SET_SWITCH_USER_PRIO_MAP_EN_REPLY             ,/* 0x00000152 */
	APCMD_GET_SWITCH_USER_PRIO_MAP_EN                   ,/* 0x00000153 */
	APCMD_GET_SWITCH_USER_PRIO_MAP_EN_REPLY             ,/* 0x00000154 */
	APCMD_SET_SWITCH_PRIO_MAP_RULE                      ,/* 0x00000155 */
	APCMD_SET_SWITCH_PRIO_MAP_RULE_REPLY                ,/* 0x00000156 */
	APCMD_GET_SWITCH_PRIO_MAP_RULE                      ,/* 0x00000157 */
	APCMD_GET_SWITCH_PRIO_MAP_RULE_REPLY                ,/* 0x00000158 */
	APCMD_SET_SWITCH_IP_PRIO_MAP                        ,/* 0x00000159 */
	APCMD_SET_SWITCH_IP_PRIO_MAP_REPLY                  ,/* 0x0000015a */
	APCMD_GET_SWITCH_IP_PRIO_MAP                        ,/* 0x0000015b */
	APCMD_GET_SWITCH_IP_PRIO_MAP_REPLY                  ,/* 0x0000015c */
	APCMD_SET_SWITCH_USER_PRIO_MAP                      ,/* 0x0000015d */
	APCMD_SET_SWITCH_USER_PRIO_MAP_REPLY                ,/* 0x0000015e */
	APCMD_GET_SWITCH_USER_PRIO_MAP                      ,/* 0x0000015f */
	APCMD_GET_SWITCH_USER_PRIO_MAP_REPLY                ,/* 0x00000160 */
	APCMD_GET_UREPEATER_LINK                            ,/* 0x00000161 */
	APCMD_GET_UREPEATER_LINK_REPLY                      ,/* 0x00000162 */
	APCMD_GET_STA_AGING_TIME_IN_MINUTES                 ,/* 0x00000163 */
	APCMD_GET_STA_AGING_TIME_IN_MINUTES_REPLY           ,/* 0x00000164 */
	APCMD_GET_ETH_LAN_IP_CONFIG,						 /* 0x00000165 */
	APCMD_GET_ETH_LAN_IP_CONFIG_REPLY,					 /* 0x00000166 */
	APCMD_SET_ETH_LAN_IP_CONFIG,						 /* 0x00000167 */
	APCMD_SET_ETH_LAN_IP_CONFIG_REPLY,					 /* 0x00000168 */
	APCMD_GET_ETH_WAN_IP_CONFIG,						 /* 0x00000169 */
	APCMD_GET_ETH_WAN_IP_CONFIG_REPLY,					 /* 0x0000016a */
	APCMD_SET_ETH_WAN_IP_CONFIG,						 /* 0x0000016b */
	APCMD_SET_ETH_WAN_IP_CONFIG_REPLY,					 /* 0x0000016c */
	APCMD_GET_ETH_DHCPS_CONFIG,							 /* 0x0000016d */
	APCMD_GET_ETH_DHCPS_CONFIG_REPLY,					 /* 0x0000016e */
	APCMD_SET_ETH_DHCPS_CONFIG,							 /* 0x0000016f */
	APCMD_SET_ETH_DHCPS_CONFIG_REPLY,					 /* 0x00000170 */
	APCMD_GET_RADIUSC_CONFIG,							 /* 0x00000171 */
	APCMD_GET_RADIUSC_CONFIG_REPLY,						 /* 0x00000172 */
	APCMD_SET_RADIUSC_CONFIG,							 /* 0x00000173 */
	APCMD_SET_RADIUSC_CONFIG_REPLY,						 /* 0x00000174 */
	APCMD_GET_SNMP_ENABLE,								 /* 0x00000175 */
	APCMD_GET_SNMP_ENABLE_REPLY,						 /* 0x00000176 */
	APCMD_SET_SNMP_ENABLE,								 /* 0x00000177 */
	APCMD_SET_SNMP_ENABLE_REPLY,						 /* 0x00000178 */
	APCMD_GET_SNMP_COMMUNITY,							 /* 0x00000179 */
	APCMD_GET_SNMP_COMMUNITY_REPLY,						 /* 0x0000017a */
	APCMD_SET_SNMP_COMMUNITY,							 /* 0x0000017b */
	APCMD_SET_SNMP_COMMUNITY_REPLY,						 /* 0x0000017c */
	APCMD_SET_WPA_PSK_VALUE,							 /* 0x0000017d */
	APCMD_SET_WPA_PSK_VALUE_REPLY,						 /* 0x0000017e */
	APCMD_SET_WPA2_PSK_VALUE,							 /* 0x0000017f */
	APCMD_SET_WPA2_PSK_VALUE_REPLY,						 /* 0x00000180 */
	APCMD_SET_RF_POWER,									 /* 0x00000181 */
	APCMD_SET_RF_POWER_REPLY,							 /* 0x00000182 */
	APCMD_GET_IP_MAC_TABLE,								 /* 0x00000183 */
	APCMD_GET_IP_MAC_TABLE_REPLY,						 /* 0x00000184 */
	/*  added api changes from AP31 codebase above this line*/

	APCMD_GET_AUTH_MODE_G                               ,/* 0x00000171 */
	APCMD_GET_AUTH_MODE_G_REPLY                         ,/* 0x00000172 */
	APCMD_SET_AUTH_MODE_G,
	APCMD_SET_AUTH_MODE_G_REPLY,
	APCMD_GET_PRIVACY_OPTION_G,
	APCMD_GET_PRIVACY_OPTION_G_REPLY,
	APCMD_SET_PRIVACY_OPTION_G,
	APCMD_SET_PRIVACY_OPTION_G_REPLY,
	APCMD_GET_DEFAULT_WEP_KEYS_G,
	APCMD_GET_DEFAULT_WEP_KEYS_G_REPLY,
	APCMD_SET_DEFAULT_WEP_KEYS_G,
	APCMD_SET_DEFAULT_WEP_KEYS_G_REPLY,

	/** Hack new for A&g **/
	APCMD_GET_RF_CHANNEL2,
	APCMD_GET_RF_CHANNEL2_REPLY,
	APCMD_SET_RF_CHANNEL2,
	APCMD_SET_RF_CHANNEL2_REPLY,
	APCMD_GET_SSID2 ,
	APCMD_GET_SSID2_REPLY,
	APCMD_SET_SSID2 ,
	APCMD_SET_SSID2_REPLY,

	APCMD_GET_TX_DATA_RATE_A,
	APCMD_GET_TX_DATA_RATE_A_REPLY,
	APCMD_SET_TX_DATA_RATE_A,
	APCMD_SET_TX_DATA_RATE_A_REPLY,
	APCMD_SET_QOS_MODE,
	APCMD_SET_QOS_MODE_REPLY,
	APCMD_GET_QOS_MODE,
	APCMD_GET_QOS_MODE_REPLY,
	APCMD_GET_QOS_TS,
	APCMD_GET_QOS_TS_REPLY,
	APCMD_SET_EDCA_PARAM,
	APCMD_SET_EDCA_PARAM_REPLY,
	APCMD_GET_EDCA_PARAM,
	APCMD_GET_EDCA_PARAM_REPLY,
	APCMD_SET_NOACK_MODE,
	APCMD_SET_NOACK_MODE_REPLY,
	APCMD_GET_NOACK_MODE,
	APCMD_GET_NOACK_MODE_REPLY,

	APCMD_GET_SPECTRUM_MGMT,
	APCMD_GET_SPECTRUM_MGMT_REPLY,
	APCMD_SET_SPECTRUM_MGMT,
	APCMD_SET_SPECTRUM_MGMT_REPLY,
	APCMD_GET_MITIGATION,
	APCMD_GET_MITIGATION_REPLY,
	APCMD_SET_MITIGATION,
	APCMD_SET_MITIGATION_REPLY,
	APCMD_SEND_SWITCH_CHANNEL_ANNOUNCEMENT,
	APCMD_SEND_SWITCH_CHANNEL_ANNOUNCEMENT_REPLY,
	APCMD_GET_REGULATORY_DOMAIN,
	APCMD_GET_REGULATORY_DOMAIN_REPLY,
	APCMD_SET_REGULATORY_DOMAIN,
	APCMD_SET_REGULATORY_DOMAIN_REPLY,
	APCMD_SET_WLAN_RADIO                                ,/* 0x000000a9 */
	APCMD_SET_WLAN_RADIO_REPLY                          ,/* 0x000000aa */
	APCMD_GET_STA_ASSOC_STATE                           ,/* 0x000000ab */
	APCMD_GET_STA_ASSOC_STATE_REPLY                     ,/* 0x000000ac */
	APCMD_GET_RSN_CFG_UNICAST_CIPHER                    ,/* 0x000000ad */
	APCMD_GET_RSN_CFG_UNICAST_CIPHER_REPLY              ,/* 0x000000ae */
	APCMD_SET_RSN_CFG_UNICAST_CIPHER                    ,/* 0x000000af */
	APCMD_SET_RSN_CFG_UNICAST_CIPHER_REPLY              ,/* 0x000000b0 */
	APCMD_GUI_SET_PRIVACY_TBL_RSN_ENABLED               ,/* 0x000000b1 */
	APCMD_GUI_SET_PRIVACY_TBL_RSN_ENABLED_REPLY         ,/* 0x000000b2 */
	APCMD_GET_WDS_BCAST_MODE                            ,/* 0x000000b3 */
	APCMD_GET_WDS_BCAST_MODE_REPLY                      ,/* 0x000000b4 */
	APCMD_SET_WDS_BCAST_MODE                            ,/* 0x000000b5 */
	APCMD_SET_WDS_BCAST_MODE_REPLY                      ,/* 0x000000b6 */
	APCMD_GET_CHANNEL_SPACING,
	APCMD_GET_CHANNEL_SPACING_REPLY,
	APCMD_SET_CHANNEL_SPACING,
	APCMD_SET_CHANNEL_SPACING_REPLY,

	APCMD_SET_RSN_CFG_WPA2_MULTICAST_CIPHERS,
	APCMD_SET_RSN_CFG_WPA2_MULTICAST_CIPHERS_REPLY,
	/* GET/SET RF Reg */
	APCMD_GET_RF_REG                                    ,
	APCMD_GET_RF_REG_REPLY                              ,
	APCMD_SET_RF_REG                                    ,
	APCMD_SET_RF_REG_REPLY                              ,
	/* GET/SET RF Reg end */

	/* BCA BT_COEXISTENCE */
	APCMD_GET_BCA_CONFIG                                ,
	APCMD_GET_BCA_CONFIG_REPLY                          ,
	APCMD_SET_BCA_CONFIG                                ,
	APCMD_SET_BCA_CONFIG_REPLY                          ,
	APCMD_GET_BCA_CONFIG_TIMESHARE                      ,
	APCMD_GET_BCA_CONFIG_TIMESHARE_REPLY                ,
	APCMD_SET_BCA_CONFIG_TIMESHARE                      ,
	APCMD_SET_BCA_CONFIG_TIMESHARE_REPLY                ,
	APCMD_GET_BCA_ENABLED                               ,
	APCMD_GET_BCA_ENABLED_REPLY                         ,
	APCMD_SET_BCA_ENABLED                               ,
	APCMD_SET_BCA_ENABLED_REPLY                         ,
	APCMD_GET_BCA_TIMESHARE_ENABLED                     ,
	APCMD_GET_BCA_TIMESHARE_ENABLED_REPLY               ,
	APCMD_SET_BCA_TIMESHARE_ENABLED                     ,
	APCMD_SET_BCA_TIMESHARE_ENABLED_REPLY               ,
	APCMD_GET_BCA_TIMESHARE_INTERVAL                    ,
	APCMD_GET_BCA_TIMESHARE_INTERVAL_REPLY              ,
	APCMD_SET_BCA_TIMESHARE_INTERVAL                    ,
	APCMD_SET_BCA_TIMESHARE_INTERVAL_REPLY              ,
	APCMD_GET_BCA_TIMESHARE_BT_TIME                     ,
	APCMD_GET_BCA_TIMESHARE_BT_TIME_REPLY               ,
	APCMD_SET_BCA_TIMESHARE_BT_TIME                     ,
	APCMD_SET_BCA_TIMESHARE_BT_TIME_REPLY               ,
	APCMD_GET_BCA_WLAN_RX_FRAME_PRIORITY                ,
	APCMD_GET_BCA_WLAN_RX_FRAME_PRIORITY_REPLY          ,
	APCMD_SET_BCA_WLAN_RX_FRAME_PRIORITY                ,
	APCMD_SET_BCA_WLAN_RX_FRAME_PRIORITY_REPLY          ,
	APCMD_GET_BCA_WLAN_TX_FRAME_PRIORITY                ,
	APCMD_GET_BCA_WLAN_TX_FRAME_PRIORITY_REPLY          ,
	APCMD_SET_BCA_WLAN_TX_FRAME_PRIORITY                ,
	APCMD_SET_BCA_WLAN_TX_FRAME_PRIORITY_REPLY          ,
	/* BCA end */
	/* Alway add items above this line */
	/* please do not add #ifdef inside of this enum */
	MAX_APCMD
} APCMD;
/** use for mac access list **/
typedef enum{
	DISABLE_MODE,
	ACCESS_MODE,
	FILTER_MODE,
}FILTER_TYPE;

#ifndef bzero
#define bzero(a, l) memset(a, 0, l)
#endif

#ifndef bcopy
#define bcopy(s, d, l) memcpy(d, s, l)
#endif

#ifndef SIZE_MAX
#define SIZE_MAX 1024
#endif

#ifndef MIN
#define	MIN(a,b) (((a)<(b))?(a):(b))
#endif

#endif /* _WLTYPES_H_ */
