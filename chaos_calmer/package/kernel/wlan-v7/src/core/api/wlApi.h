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

#ifndef _WL_API_H_
#define _WL_API_H_ 


/*!
* \file    api.h
* \brief   This file contains the function prototypes and definitions for the
*          AP commands API
*
*/

//=============================================================================
//                          API for Sending Commands
//=============================================================================
#if defined(UDP_API)
#define APICMDHEADER   unsigned short cmdCode; \
	unsigned short seqNum; \
	unsigned short cmdStatus; \
	unsigned short cmdBodyLen; \
	unsigned int *cmdBody
#define APICMDHEADERLEN  sizeof(unsigned short)*4 + sizeof(unsigned int *)
#else
#define APICMDHEADER   unsigned short cmdCode; \
	unsigned short seqNum; \
	unsigned short cmdStatus; \
	unsigned short cmdBodyLen
#define APICMDHEADERLEN  sizeof(unsigned short)*4
#endif
#define SIZEOF_APICMDHEADER 12


typedef enum
{
	APCTL_OK = 1,
	APCTL_BUF_TOO_SMALL,
	APCTL_BAD_CMD,
	APCTL_CMD_FAILED,
	APCTL_TIMEOUT,
	APCTL_ERROR,
	APCTL_MORE_DATA,
	APCTL_LAST_DATA,
}APCMD_STATUS;

typedef struct _APICMDBUF
{
	UINT16 cmdCode;
	UINT16 seqNum;
	UINT16 cmdStatus;
	UINT16 cmdBodyLen;
	UINT8 *cmdBody;
}
APICMDBUF;
BOOLEAN apctl(APICMDBUF * apicmdbuf);
#if defined(GATEWAY)
typedef struct _APICMDBUF_GET_ETH_LAN_IP_CONFIG
{
	APICMDHEADER;
	unsigned long ipAddress;
	unsigned long subnetMask;
}
APICMDBUF_GET_ETH_LAN_IP_CONFIG;

typedef struct _APICMDBUF_SET_ETH_LAN_IP_CONFIG
{
	APICMDHEADER;
	unsigned long ipAddress;
	unsigned long subnetMask;
}
APICMDBUF_SET_ETH_LAN_IP_CONFIG;

typedef struct _APICMDBUF_GET_ETH_WAN_IP_CONFIG
{
	APICMDHEADER;
	unsigned long dhcpEnabled;
	unsigned long ipAddress;
	unsigned long subnetMask;
	unsigned long gatewayAddress;
	unsigned long primaryDNSAddress;
	unsigned long secondaryDNSAddress;
}
APICMDBUF_GET_ETH_WAN_IP_CONFIG;

typedef struct _APICMDBUF_SET_ETH_WAN_IP_CONFIG
{
	APICMDHEADER;
	unsigned long dhcpEnabled;
	unsigned long ipAddress;
	unsigned long subnetMask;
	unsigned long gatewayAddress;
	unsigned long primaryDNSAddress;
	unsigned long secondaryDNSAddress;
}
APICMDBUF_SET_ETH_WAN_IP_CONFIG;
#endif

#if defined(GATEWAY) && defined(DHCPS)
#include "dhcps_api.h"
typedef struct _APICMDBUF_SET_ETH_DHCPS_CONFIG
{
	APICMDHEADER;
	unsigned long isEnable;
	unsigned long rangeStartAddr;
	unsigned long rangeEndAddr;
	unsigned long leaseTime;
	DHCPS_STATION_PROPERTIES station;
}

APICMDBUF_SET_ETH_DHCPS_CONFIG;

typedef struct _APICMDBUF_GET_ETH_DHCPS_CONFIG
{
	APICMDHEADER;
	unsigned long isEnable;
	unsigned long rangeStartAddr;
	unsigned long rangeEndAddr;
	unsigned long leaseTime;
	unsigned long numOfStations;
	DHCPS_STATION_PROPERTIES stationsTbl[DHCPS_MAX_NUM_OF_STATIONS];
}

APICMDBUF_GET_ETH_DHCPS_CONFIG;

#endif

typedef struct _APICMDBUF_GET_RF_CHANNEL
{
	APICMDHEADER;
	unsigned short rfModulationType;
	char	availChanList[14+19];
#ifdef AUTOCHANNEL
	unsigned short curChanNum : 15;
	unsigned short isAutoChan : 1;
#else
	unsigned short curChanNum;
#endif
}
APICMDBUF_GET_RF_CHANNEL;

typedef struct _APICMDBUF_GET_OPEN_SSID_EN_STATE
{
	APICMDHEADER;
	unsigned char openSSIDEnabled;
}
APICMDBUF_GET_OPEN_SSID_EN_STATE;

typedef struct _APICMDBUF_SET_OPEN_SSID_EN_STATE
{
	APICMDHEADER;
	unsigned char openSSIDEnabled;
}
APICMDBUF_SET_OPEN_SSID_EN_STATE;


typedef struct _APICMDBUF_SET_RF_CHANNEL
{
	APICMDHEADER;
	unsigned char curChanNum;

}
APICMDBUF_SET_RF_CHANNEL;

typedef struct _APICMDBUF_GET_RF_MACADDR
{
	APICMDHEADER;
	unsigned char physAddr[6];
}
APICMDBUF_GET_RF_MACADDR;

typedef struct _APICMDBUF_SET_RF_MACADDR
{
	APICMDHEADER;
	unsigned char physAddr[6];
}
APICMDBUF_SET_RF_MACADDR;

typedef struct _APICMDBUF_GET_BASIC_DATA_RATES
{
	APICMDHEADER;
	unsigned char basicDataRates[8];
}
APICMDBUF_GET_BASIC_DATA_RATES;

typedef struct _APICMDBUF_SET_BASIC_DATA_RATES
{
	APICMDHEADER;
	unsigned char basicDataRates[8];
}
APICMDBUF_SET_BASIC_DATA_RATES;

typedef struct _APICMDBUF_GET_SUP_DATA_RATE
{
	APICMDHEADER;
	unsigned char txDataRate[14];
}
APICMDBUF_GET_SUP_DATA_RATE;

typedef struct _APICMDBUF_SET_SUP_DATA_RATE
{
	APICMDHEADER;
	unsigned char txDataRate[14];
}
APICMDBUF_SET_SUP_DATA_RATE;

typedef struct _APICMDBUF_GET_TX_DATA_RATE
{
	APICMDHEADER;
	unsigned char txDataRate;
}
APICMDBUF_GET_TX_DATA_RATE;

typedef struct _APICMDBUF_SET_TX_DATA_RATE
{
	APICMDHEADER;
	unsigned char txDataRate;
}
APICMDBUF_SET_TX_DATA_RATE;

typedef struct _APICMDBUF_GET_TX_POWER_LEVEL
{
	APICMDHEADER;
	unsigned char txPowerLevel;
	unsigned char powerLevelList[8];
}
APICMDBUF_GET_TX_POWER_LEVEL;

typedef struct _APICMDBUF_SET_TX_POWER_LEVEL
{
	APICMDHEADER;
	unsigned char txPowerLevel;
}
APICMDBUF_SET_TX_POWER_LEVEL;

typedef struct _APICMBUF_GET_RF_PREAMBLE_OPTION
{
	APICMDHEADER;
	unsigned char rfPreambleOption;
}
APICMBUF_GET_RF_PREAMBLE_OPTION;

typedef struct _APICMBUF_SET_RF_PREAMBLE_OPTION
{
	APICMDHEADER;
	unsigned char rfPreambleOption;
}
APICMBUF_SET_RF_PREAMBLE_OPTION;
typedef struct _APICMDBUF_GET_ANTENNA_MODE
{
	APICMDHEADER;
	unsigned char rxAntennaMode;
	unsigned char txAntennaMode;
	unsigned char whichAnt;
}
APICMDBUF_GET_ANTENNA_MODE;

typedef struct _APICMDBUF_SET_ANTENNA_MODE
{
	APICMDHEADER;
	unsigned char whichAnt;
	unsigned char antennaMode;
}
APICMDBUF_SET_ANTENNA_MODE;
typedef struct _APICMDBUF_GET_FRAG_THRESHOLD
{
	APICMDHEADER;
	unsigned short fragThreshold;
}
APICMDBUF_GET_FRAG_THRESHOLD;

typedef struct _APICMDBUF_SET_FRAG_THRESHOLD
{
	APICMDHEADER;
	unsigned short fragThreshold;
}
APICMDBUF_SET_FRAG_THRESHOLD;

typedef struct _APICMDBUF_GET_RTS_THRESHOLD
{
	APICMDHEADER;
	unsigned short rtsThreshold;
}
APICMDBUF_GET_RTS_THRESHOLD;

typedef struct _APICMDBUF_SET_RTS_THRESHOLD
{
	APICMDHEADER;
	unsigned short rtsThreshold;
}
APICMDBUF_SET_RTS_THRESHOLD;


typedef struct _APICMDBUF_GET_RTS_RETRY_LIMIT
{
	APICMDHEADER;
	unsigned short rtsRetryLimit;
}
APICMDBUF_GET_RTS_RETRY_LIMIT;

typedef struct _APICMDBUF_SET_RTS_RETRY_LIMIT
{
	APICMDHEADER;
	unsigned short rtsRetryLimit;
}
APICMDBUF_SET_RTS_RETRY_LIMIT;

typedef struct _APICMDBUF_GET_DATA_RETRY_LIMIT
{
	APICMDHEADER;
	unsigned short dataRetryLimit;
}
APICMDBUF_GET_DATA_RETRY_LIMIT;

typedef struct _APICMDBUF_SET_DATA_RETRY_LIMIT
{
	APICMDHEADER;
	unsigned short dataRetryLimit;
}
APICMDBUF_SET_DATA_RETRY_LIMIT;

typedef struct _APICMDBUF_GET_BEACON_PERIOD
{
	APICMDHEADER;
	unsigned long beaconPeriod;
}
APICMDBUF_GET_BEACON_PERIOD;

typedef struct _APICMDBUF_SET_BEACON_PERIOD
{
	APICMDHEADER;
	unsigned long beaconPeriod;
}
APICMDBUF_SET_BEACON_PERIOD;

typedef struct _APICMDBUF_GET_DTIM_PERIOD
{
	APICMDHEADER;
	unsigned long dtimPeriod;
}
APICMDBUF_GET_DTIM_PERIOD;


typedef struct _APICMDBUF_SET_DTIM_PERIOD
{
	APICMDHEADER;
	unsigned long dtimPeriod;
}
APICMDBUF_SET_DTIM_PERIOD;

typedef struct _APICMDBUF_GET_802_11_STATS
{
	APICMDHEADER;
	unsigned long totaltxBytes;
	unsigned long cntMcastPackets;
	unsigned long cntUcastPackets;
	unsigned long cntTxBad;
	unsigned long cntRetry;
	unsigned long cntMultRetry;
	unsigned long rtsOKCnt;
	unsigned long rtsBadCnt;
	unsigned long ackBadCnt;
	unsigned long totalRxBytes;
	unsigned long rxMcastOkCnt;
	unsigned long rxUcastOkCnt;
	unsigned long fcsErrorCnt;
	unsigned long duplicateCnt;
	unsigned long wepDecryptBadCnt;
}
APICMDBUF_GET_802_11_STATS;

typedef struct _APICMDBUF_GET_ETH_MAC_ADDRESS
{
	APICMDHEADER;
	unsigned char phyAddress[6];
}
APICMDBUF_GET_ETH_MAC_ADDRESS;

typedef struct _APICMDBUF_SET_ETH_MAC_ADDRESS
{
	APICMDHEADER;
	unsigned char phyAddress[6];
}
APICMDBUF_SET_ETH_MAC_ADDRESS;

typedef struct _APICMDBUF_GET_WAN_MAC_ADDRESS
{
	APICMDHEADER;
	unsigned char phyAddress[6];
}
APICMDBUF_GET_WAN_MAC_ADDRESS;

typedef struct _APICMDBUF_SET_WAN_MAC_ADDRESS
{
	APICMDHEADER;
	unsigned char phyAddress[6];
}
APICMDBUF_SET_WAN_MAC_ADDRESS;

typedef struct _APICMDBUF_GET_ETH_IP_CONFIG
{
	APICMDHEADER;
	unsigned long dhcpEnabled;
	unsigned long ipAddress;
	unsigned long subnetMask;
	unsigned long gatewayAddress;
}
APICMDBUF_GET_ETH_IP_CONFIG;

typedef struct _APICMDBUF_SET_ETH_IP_CONFIG
{
	APICMDHEADER;
	unsigned long dhcpEnabled;
	unsigned long ipAddress;
	unsigned long subnetMask;
	unsigned long gatewayAddress;
}
APICMDBUF_SET_ETH_IP_CONFIG;

typedef struct _APICMDBUF_GET_ETH_PORT_STATUS
{
	APICMDHEADER;
	unsigned char portnumber;
	unsigned char ethOperStatus;
	unsigned char ethDuplex;
	unsigned char ethLinkSpeed;
}
APICMDBUF_GET_ETH_PORT_STATUS;

typedef enum
{
	ETH_OPER_STATUS_UP,
	ETH_OPER_STATUS_DOWN,
}ETHER_PORTSTATUS;


typedef struct _APICMDBUF_SET_ETH_PORT_PARAMS
{
	APICMDHEADER;
	unsigned short linkSpeed;
	unsigned short ethDuplex;
}
APICMDBUF_SET_ETH_PORT_PARAMS;

typedef struct _APICMDBUF_GET_ICMP_EN_STATE
{
	APICMDHEADER;
	unsigned char icmpEnabled;
}
APICMDBUF_GET_ICMP_EN_STATE;

typedef struct _APICMDBUF_SET_ICMP_EN_STATE
{
	APICMDHEADER;
	unsigned char icmpEnabled;
}
APICMDBUF_SET_ICMP_EN_STATE;


typedef struct _APICMDBUF_GET_802_3_STATS
{
	APICMDHEADER;
	unsigned long clearStats;
	unsigned long totalTxBytes;
	unsigned long cntMcastPackets;
	unsigned long cntUcastPackets;
	unsigned long cntTxBad;
	unsigned long totalRxBytes;
	unsigned long rxBadCnt;
	unsigned long rxMcastOKCnt;
	unsigned long rxUcastOKCnt;
	unsigned long crcErrorCnt;
}
APICMDBUF_GET_802_3_STATS;


typedef struct _APICMDBUF_GET_STA_INFO
{
	APICMDHEADER;
	unsigned char staMACAddress[6];
	unsigned char staIPAddress;
	unsigned char staPowerSaveMode;
	unsigned char staWEPStatus;
	UINT32 staRSSI;
	UINT32 staSQ;
}
APICMDBUF_GET_STA_INFO;

typedef struct _APICMDBUF_DEAUTH_STA
{
	APICMDHEADER;
	unsigned char staMACAddress[6];
}
APICMDBUF_DEAUTH_STA;

typedef struct _APICMDBUF_DISASSOC_STA
{
	APICMDHEADER;
	unsigned char staMACAddress[6];
}
APICMDBUF_DISASSOC_STA;

/* SECURITY COMMANDS */
typedef struct _APICMDBUF_GET_DEFAULT_WEP_KEYS
{
	APICMDHEADER;
	unsigned short keyindex; /*cntWEPKEYS; */
	unsigned short keyType0;
	unsigned char keyValue0[16];
	unsigned short keyType1;
	unsigned char keyValue1[16];
	unsigned short keyType2;
	unsigned char keyValue2[16];
	unsigned short keyType3;
	unsigned char keyValue3[16];
}
APICMDBUF_GET_DEFAULT_WEP_KEYS;

typedef struct _APICMDBUF_SET_DEFAULT_WEP_KEYS
{
	APICMDHEADER;
	unsigned short keyindex;
	unsigned short keyType0;
	unsigned char keyValue0[16];
	unsigned short keyType1;
	unsigned char keyValue1[16];
	unsigned short keyType2;
	unsigned char keyValue2[16];
	unsigned short keyType3;
	unsigned char keyValue3[16];
}
APICMDBUF_SET_DEFAULT_WEP_KEYS;

typedef struct _APICMDBUF_SET_STA_WEP_KEY
{
	APICMDHEADER;
	unsigned char staMACAddress[6];
	unsigned short keyType;
	unsigned short KeyValue[16];
}
APICMDBUF_SET_STA_WEP_KEY;

typedef enum
{
	WEP_NOT_SET,
	WEP_40_BIT,
	WEP_104_BIT,
} WEP_KEY_TYPE;

typedef struct _APICMDBUF_GET_PRIVACY_OPTION
{
	APICMDHEADER;
	unsigned char dataEncrypt;
}
APICMDBUF_GET_PRIVACY_OPTION;

typedef struct _APICMDBUF_SET_PRIVACY_OPTION
{
	APICMDHEADER;
	unsigned char dataEncrypt;
}
APICMDBUF_SET_PRIVACY_OPTION;

typedef enum
{
	DATA_ENCRYPT_NOT_REQUIRED,
	DATA_ENCRYPT_OPTIONA,
	FULL_DATA_ENCRYPT_REQUIRED,
} PRIVACY_TYPE;

typedef struct _APICMDBUF_GET_AUTH_MODE
{
	APICMDHEADER;
	unsigned char authMode;
}
APICMDBUF_GET_AUTH_MODE;

typedef struct _APICMDBUF_SET_AUTH_MODE
{
	APICMDHEADER;
	unsigned char authMode;
}
APICMDBUF_SET_AUTH_MODE;

/*AP SYSTEM COMMANDS */
typedef struct _APICMDBUF_GET_API_VER
{
	APICMDHEADER;
	unsigned char apAPIVer[32];
}
APICMDBUF_GET_API_VER;

typedef struct _APICMDBUF_GET_AP_STATUS
{
	APICMDHEADER;
	unsigned short macStatus;
	unsigned short rfPortStatus;
	unsigned short ethPortStatus;
	unsigned short curChannel;
	unsigned char apname[32];
	unsigned char macAddress[6];
	unsigned short cntAssocSta;
	unsigned long maxLinkSpeed;
	unsigned char apFwVer[32];
	unsigned long apFwChecksum;
}
APICMDBUF_GET_AP_STATUS;

typedef enum
{
	OPER_STATUS_UP,
	OPER_STATUS_DOWN,
} AP_STATUS_TYPE;

typedef struct _APICMDBUF_GET_EVENT_LOG
{
	APICMDHEADER;
	unsigned char eventlog;
}
APICMDBUF_GET_EVENT_LOG;

typedef struct _APICMDBUF_PURGE_EVENT_LOG
{
	APICMDHEADER;
}
APICMDBUF_PURGE_EVENT_LOG;

typedef struct _APICMDBUF_UPDATE_FIRMWARE
{
	APICMDHEADER;
	unsigned char *imgPtr;
	unsigned char apFwVer[32];
	unsigned long apChecksum;
}
APICMDBUF_UPDATE_FIRMWARE;

typedef enum
{
	FW_UPDATE_OK,
	FW_UPDATE_FAIL_FLASH,
	FW_UPDATE_FAIL_CHECKUSM,
	FW_UPDATE_SAME_VER,
} UPDATE_FIRMWARE_TYPE;

typedef struct _APICMDBUF_RUN_DIAG
{
	APICMDHEADER;
	unsigned short testNum;
	unsigned short testResult;
}
APICMDBUF_RUN_DIAG;

typedef enum
{
	TEST_PASS,
	TEST_FAIL,
	TEST_INCONCLUSIVE,
} DIAG_TYPE;

typedef struct _APICMDBUF_START_LINK_TEST
{
	APICMDHEADER;
	unsigned char staMACAdress[6];
	unsigned long cntTxPacketsOK;
	unsigned long totalErrors;
	unsigned long totalWEPErrors;
}
APICMDBUF_START_LINK_TEST;

typedef struct _APICMDBUF_READ_GPIO
{
	APICMDHEADER;
	unsigned char gpioNum;
	unsigned char gpioVal;
}
APICMDBUF_READ_GPIO;

typedef struct _APICMDBUF_WRITE_GPIO
{
	APICMDHEADER;
	unsigned char gpioNum;
	unsigned char gpioVal;
}
APICMDBUF_WRITE_GPIO;

typedef struct _APICMDBUF_SET_STA_AGING_TIME
{
	APICMDHEADER;
	unsigned short timeInMinutes;

}
APICMDBUF_SET_STA_AGING_TIME;


typedef struct _APICMDBUF_SET_WLAN_RADIO
{
	APICMDHEADER;
	unsigned char radio;
}
APICMDBUF_SET_WLAN_RADIO;

typedef struct _APICMDBUF_GET_BASIC_DATA_RATES_G
{
	APICMDHEADER;
	unsigned char basicDataRates[16];
}
APICMDBUF_GET_BASIC_DATA_RATES_G;

typedef struct _APICMDBUF_SET_BASIC_DATA_RATES_G
{
	APICMDHEADER;
	unsigned char basicDataRates[16];
}
APICMDBUF_SET_BASIC_DATA_RATES_G;

typedef struct _APICMDBUF_GET_AP_MODE
{
	APICMDHEADER;
	UINT32 apMode;
}
APICMDBUF_GET_AP_MODE;

typedef struct _APICMDBUF_SET_AP_MODE
{
	APICMDHEADER;
	UINT32 apMode;
}
APICMDBUF_SET_AP_MODE;


#define SDRAM_BURST_REG (W81_SDRAM_CFG_BASE+0x14)
#define SDRAM_BURST_1   0
#define SDRAM_BURST_2   1
#define SDRAM_BURST_4   2
#define SDRAM_BURST_8   3
typedef struct _APICMDBUF_SDRAM_BURST_MODE
{
	APICMDHEADER;
	UINT8 sdramBurstMode;

}
APICMDBUF_SDRAM_BURST_MODE;
typedef enum
{
	FILTER_MODE_DISABLE,
	FILTER_MODE_ALLOW,
	FILTER_MODE_BLOCK,
}FILTER_MODE_TYPE;

typedef struct _APICMDBUF_FILTER_ENTRY
{
	APICMDHEADER;
	UINT8  macAddr[6];
}
APICMDBUF_FILTER_ENTRY;
typedef struct _APICMDBUF_GET_FILTER_MODE
{
	APICMDHEADER;
	unsigned char  mode; /* use FILTER_MODE_TYPE enum */
}
APICMDBUF_GET_FILTER_MODE;

typedef struct _APICMDBUF_SET_FILTER_MODE
{
	APICMDHEADER;
	unsigned char  mode; /* use FILTER_MODE_TYPE enum */
}
APICMDBUF_SET_FILTER_MODE;

typedef struct _APICMDBUF_FILTER_MODE
{
	APICMDHEADER;
	unsigned char  mode; 
}
APICMDBUF_FILTER_MODE;

typedef struct _APICMDBUF_SET_BURST_MODE
{
	APICMDHEADER;
	UINT8 BurstMode;
	UINT8 BurstRate;
}
APICMDBUF_SET_BURST_MODE;

typedef struct _APICMDBUF_GET_BURST_MODE
{
	APICMDHEADER;
	UINT8 BurstMode;
	UINT8 BurstRate;
}
APICMDBUF_GET_BURST_MODE;

#ifdef WPA

/**************** 802.11i MIB supported *********************/
typedef struct _APICMDBUF_GET_STA_CFG_RSN_OPTION_IMP
{
	APICMDHEADER;
	unsigned char RSN_Capable;
}
APICMDBUF_GET_STA_CFG_RSN_OPTION_IMP;

typedef struct _APICMDBUF_GET_STA_CFG_TKIP_NUM_RPLY_COUNTERS
{
	APICMDHEADER;
	unsigned long NumOfCounters;
}
APICMDBUF_GET_STA_CFG_TKIP_NUM_RPLY_COUNTERS;

typedef struct _APICMDBUF_GET_PRIVACY_TBL_RSN_ENABLED
{
	APICMDHEADER;
	unsigned long Enabled;
}
APICMDBUF_GET_PRIVACY_TBL_RSN_ENABLED;

typedef struct _APICMDBUF_SET_PRIVACY_TBL_RSN_ENABLED
{
	APICMDHEADER;
	unsigned long Enabled;
}
APICMDBUF_SET_PRIVACY_TBL_RSN_ENABLED;

typedef struct _APICMDBUF_GET_RSN_CFG_TBL
{
	APICMDHEADER;
	unsigned long Version;
	unsigned long PairwiseKeysSupported;
	unsigned char MulticastCipher[4];
	unsigned char GroupRekeyMethod;
	unsigned long GroupRekeyTime;
	unsigned long GroupRekeyPackets;
	unsigned char GroupRekeyStrict;
	//unsigned char  PSKValue[32];  // write only
	//unsigned char  PSKPassPhrase[64]; // write only
	unsigned char TSNEnabled;
	unsigned long GroupMasterRekeyTime;
	unsigned long GroupUpdateTimeOut;
	unsigned long GroupUpdateCount;
	unsigned long PairwiseUpdateTimeOut;
	unsigned long PairwiseUpdateCount;
}
APICMDBUF_GET_RSN_CFG_TBL;
typedef struct _APICMDBUF_GET_CLOCK
{
	APICMDHEADER;
	unsigned short cpuFreq;  /*MHz*/
	unsigned short sysFreq;  /*MHz*/
}
APICMDBUF_GET_CLOCK;
typedef struct _APICMDBUF_SET_CLOCK
{
	APICMDHEADER;
	unsigned short cpuFreq;  /*MHz*/
	unsigned short sysFreq;  /*MHz*/
}
APICMDBUF_SET_CLOCK;

typedef struct _APICMDBUF_SET_RSN_CFG_TBL
{
	APICMDHEADER;
	//unsigned long  Version;   // read only
	unsigned long PairwiseKeysSupported;
	unsigned char MulticastCipher[4];
	unsigned char GroupRekeyMethod;
	unsigned long GroupRekeyTime;
	unsigned long GroupRekeyPackets;
	unsigned char GroupRekeyStrict;
	unsigned char PSKValue[32]; //write only
    unsigned char PSKPassPhrase[65]; // write only
	unsigned char TSNEnabled;
	unsigned long GroupMasterRekeyTime;
	unsigned long GroupUpdateTimeOut;
	unsigned long GroupUpdateCount;
	unsigned long PairwiseUpdateTimeOut;
	unsigned long PairwiseUpdateCount;
}
APICMDBUF_SET_RSN_CFG_TBL;

typedef struct _APICMDBUF_GET_RSN_CFG_UNICAST_CIPHER
{
	APICMDHEADER;
	unsigned char UnicastCipher[4];
	unsigned char Enabled;
}
APICMDBUF_GET_RSN_CFG_UNICAST_CIPHER;

typedef struct _APICMDBUF_SET_RSN_CFG_UNICAST_CIPHER
{
	APICMDHEADER;
	unsigned char UnicastCipher[4];
	unsigned char Enabled;
}
APICMDBUF_SET_RSN_CFG_UNICAST_CIPHER;

typedef struct _APICMDBUF_SET_RSN_CFG_MULTICAST_CIPHERS
{
	APICMDHEADER;
	unsigned char MulticastCipher[4];
}
APICMDBUF_SET_RSN_CFG_MULTICAST_CIPHERS;

typedef struct _APICMDBUF_SET_RSN_CFG_WPA2_MULTICAST_CIPHERS
{
	APICMDHEADER;
	unsigned char MulticastCipher[4];
}
APICMDBUF_SET_RSN_CFG_WPA2_MULTICAST_CIPHERS;

typedef struct _APICMDBUF_SET_PSK_PASSPHRASE
{
	APICMDHEADER;
	unsigned char passphrase[64];
}
APICMDBUF_SET_PSK_PASSPHRASE;

typedef struct _APICMDBUF_SET_GRP_REKEY_TIME
{
	APICMDHEADER;
	unsigned long rekey_time;
}
APICMDBUF_SET_GRP_REKEY_TIME;

typedef struct _APICMDBUF_GET_RSN_CFG_AUTH_SUITE
{
	APICMDHEADER;
	unsigned char AuthSuites[4];
	unsigned char Enabled;
}
APICMDBUF_GET_RSN_CFG_AUTH_SUITE;

typedef struct _APICMDBUF_SET_RSN_CFG_AUTH_SUITE
{
	APICMDHEADER;
	unsigned char AuthSuites[4];
	unsigned char Enabled;
}
APICMDBUF_SET_RSN_CFG_AUTH_SUITE;

typedef struct _APICMDBUF_GET_RSN_STATS
{
	APICMDHEADER;
	unsigned char MacAddr[6];
	unsigned char Version;
	unsigned char SelectedUnicastCipher[4];
	unsigned long TKIPICVErrors;
	unsigned long TKIPLocalMICFailures;
	unsigned long TKIPRemoteMICFailures;
	unsigned long TKIPCounterMeasuresInvoked;
	unsigned long WRAPFormatErrors;
	unsigned long WRAPReplays;
	unsigned long WRAPDecryptErrors;
	unsigned long CCMPFormatErrors;
	unsigned long CCMPReplays;
	unsigned long CCMPDecryptErrors;
}
APICMDBUF_GET_RSN_STATS;

typedef struct _APICMDBUF_GET_UREPEATER
{
	APICMDHEADER;
	unsigned char urMode; /* 1=enable; else=disable */
	unsigned char urSsid[32];
	unsigned short urRsvd;
	unsigned char urBssid[6];
	unsigned char urPrefBssid;
	unsigned char wbMode;
}
APICMDBUF_GET_UREPEATER;

typedef struct _APICMDBUF_SET_UREPEATER
{
	APICMDHEADER;
	unsigned char urMode; /* 1=enable; else=disable */
	unsigned char urSsid[32];
	unsigned short urRsvd;
	unsigned char urBssid[6];
	unsigned char urPrefBssid;
	unsigned char wbMode;
}
APICMDBUF_SET_UREPEATER;

typedef struct _IPMAC_ENTRY
{
	unsigned long    IpAddr;
	unsigned char    Addr[6];
}
IPMAC_ENTRY;
typedef struct _APICMDBUF_IPMAC_TABLE
{
	APICMDHEADER;
	unsigned short NoOfEntries;
	IPMAC_ENTRY entries[1];
}
APICMDBUF_IPMAC_TABLE;

enum 
{
	UR_DISCONNECTED = 0,
	UR_CONNECTED = 1
};
typedef struct _APICMDBUF_GET_UREPEATER_LINK
{
	APICMDHEADER;
	unsigned char urLink; /* 1=connected; 0=disconnected */
	unsigned char urBssid[6];
#ifdef APCFGUR
	unsigned char RateToBeUsedForTx;
	char 		  AvgRSSI;
#endif
}
APICMDBUF_GET_UREPEATER_LINK;

/* end of 802.11i MIB supported */

/************ Marvell private MIB **************************/

// Enable data traffic for a station. This will be enabled if authentication and key installation
// for the station is done.
typedef struct _APICMDBUF_SET_RSN_DATA_TRAFFIC_ENABLED
{
	APICMDHEADER;
	unsigned char StaMacAddr[6];
	unsigned char Enabled;
}
APICMDBUF_SET_RSN_DATA_TRAFFIC_ENABLED;

// Set Pairwise Key (PWK) for a station
typedef struct _APICMDBUF_SET_RSN_PWK
{
	APICMDHEADER;
	UINT8 StaMacAddr[6];
	UINT8 EncryptKey[16];
	UINT32 TxMICKey[2];
	UINT32  RxMICKey[2];
	//unsigned long  TempKey1[4];
	//unsigned long  TempKey2[4];
}
APICMDBUF_SET_RSN_PWK;

// Get Pairwise Key (PWK) for a station
typedef struct _APICMDBUF_GET_RSN_PWK
{
	APICMDHEADER;
	UINT8 StaMacAddr[6];
	UINT8 EncryptKey[16];
	UINT32 TxMICKey[2];
	UINT32 RxMICKey[2];
	//unsigned long  TempKey1[4];
	//unsigned long  TempKey2[4];
}
APICMDBUF_GET_RSN_PWK;

// Set group key
typedef struct _APICMDBUF_SET_RSN_GRP_KEY
{
	APICMDHEADER;
	unsigned char EncryptKey[16];
	UINT32 TxMICKey[2];
	UINT32 RxMICKey[2];
	//unsigned long  TempKey1[4];
	//unsigned long  TempKey2[4];
}
APICMDBUF_SET_RSN_GRP_KEY;

// Get group key
typedef struct _APICMDBUF_GET_RSN_GRP_KEY
{
	APICMDHEADER;
	unsigned char EncryptKey[16];
	UINT32 TxMICKey[2];
	UINT32 RxMICKey[2];
	//unsigned long  TempKey1[4];
	//unsigned long  TempKey2[4];
}
APICMDBUF_GET_RSN_GRP_KEY;
/* End of Marvell private MIB */

// Get a station's association state. The states include APEVT_STA_ASSOCIATED and
// APEVT_STA_DISASSOCIATED defined in enum STA_ASSOC_TYPE. Whenever a station gets
// associated or deassociated, MacMgmt task will send the state to a message queue.
// This function can be called to check the state in the message queue.
typedef struct _APICMDBUF_GET_STA_ASSOC_STATE
{
	APICMDHEADER;
	unsigned char staMACAddr[6];
	unsigned short assocType;
}
APICMDBUF_GET_STA_ASSOC_STATE;

// Get this station's RSN IE
#define MAX_SIZE_RSN_IE_BUF 64
typedef struct _APICMDBUF_GET_THIS_STA_RSN_IE
{
	APICMDHEADER;
	//unsigned char  staMACAddr[6];
	unsigned char RsnIEBuf[MAX_SIZE_RSN_IE_BUF];
	unsigned char RsnIEWPA2Buf[MAX_SIZE_RSN_IE_BUF];
}
APICMDBUF_GET_THIS_STA_RSN_IE;

// Get an external station's RSN IE
typedef struct _APICMDBUF_GET_EXT_STA_RSN_IE
{
	APICMDHEADER;
	unsigned char staMACAddr[6];
	unsigned char RsnIEBuf[MAX_SIZE_RSN_IE_BUF];
}
APICMDBUF_GET_EXT_STA_RSN_IE;

// Get pairwise TSC
typedef struct _APICMDBUF_GET_PAIRWISE_TSC
{
	APICMDHEADER;
	unsigned char staMACAddr[6];
	unsigned long TxIV32;
	unsigned short TxIV16;
}
APICMDBUF_GET_PAIRWISE_TSC;

// Set pairwise TSC
typedef struct _APICMDBUF_SET_PAIRWISE_TSC
{
	APICMDHEADER;
	unsigned char staMACAddr[6];
	unsigned long TxIV32;
	unsigned short TxIV16;
}
APICMDBUF_SET_PAIRWISE_TSC;

// Get Group TSC
typedef struct _APICMDBUF_GET_GRP_TSC
{
	APICMDHEADER;
	unsigned long g_IV32;
	unsigned short g_IV16;
}
APICMDBUF_GET_GRP_TSC;

// Set Group TSC
typedef struct _APICMDBUF_SET_GRP_TSC
{
	APICMDHEADER;
	unsigned long g_IV32;
	unsigned short g_IV16;
}
APICMDBUF_SET_GRP_TSC;

typedef struct _APICMDBUF_SET_COUNTER_MEASURE_ENABLED
{
	APICMDHEADER;
	unsigned char Enabled;
}
APICMDBUF_SET_COUNTER_MEASURE_ENABLED;

// Set Pairwise Master Key (PMK) for a station
typedef struct _APICMDBUF_SET_RSN_PMK
{
	APICMDHEADER;
	unsigned char StaMacAddr[6];
	unsigned char PMK[32];
}
APICMDBUF_SET_RSN_PMK;


// Set WPA PSK Value
typedef struct _APICMDBUF_SET_WPA_PSK_VALUE
{
	APICMDHEADER;
	unsigned char PSKValueEnabled;
	unsigned char PSKValue[32];
}
APICMDBUF_SET_WPA_PSK_VALUE;

#ifdef AP_WPA2
/* WPA2 */
// Set WPA PSK Value
typedef struct _APICMDBUF_SET_WPA2_PSK_VALUE
{
	APICMDHEADER;
	unsigned char PSKValueEnabled;
	unsigned char PSKValue[32];
}
APICMDBUF_SET_WPA2_PSK_VALUE;

typedef struct _APICMDBUF_GET_RSN_CFG_WPA2_TBL
{
	APICMDHEADER;
	unsigned long Version;
	unsigned long PairwiseKeysSupported;
	unsigned char MulticastCipher[4];
	unsigned char GroupRekeyMethod;
	unsigned long GroupRekeyTime;
	unsigned long GroupRekeyPackets;
	unsigned char GroupRekeyStrict;
	//unsigned char  PSKValue[40];  // write only
	//unsigned char  PSKPassPhrase[64]; // write only
	unsigned char TSNEnabled;
	unsigned long GroupMasterRekeyTime;
	unsigned long GroupUpdateTimeOut;
	unsigned long GroupUpdateCount;
	unsigned long PairwiseUpdateTimeOut;
	unsigned long PairwiseUpdateCount;
}
APICMDBUF_GET_RSN_CFG_WPA2_TBL;

typedef struct _APICMDBUF_SET_RSN_CFG_WPA2_TBL
{
	APICMDHEADER;
	//unsigned long  Version;   // read only
	unsigned long PairwiseKeysSupported;
	unsigned char MulticastCipher[4];
	unsigned char GroupRekeyMethod;
	unsigned long GroupRekeyTime;
	unsigned long GroupRekeyPackets;
	unsigned char GroupRekeyStrict;
	unsigned char PSKValue[40]; //write only
    unsigned char PSKPassPhrase[65]; // write only
	unsigned char TSNEnabled;
	unsigned long GroupMasterRekeyTime;
	unsigned long GroupUpdateTimeOut;
	unsigned long GroupUpdateCount;
	unsigned long PairwiseUpdateTimeOut;
	unsigned long PairwiseUpdateCount;
}
APICMDBUF_SET_RSN_CFG_WPA2_TBL;

// Get this station's RSN IE
typedef struct _APICMDBUF_GET_THIS_STA_WPA2_RSN_IE
{
	APICMDHEADER;
	//unsigned char  staMACAddr[6];
	unsigned char RsnIEBuf[MAX_SIZE_RSN_IE_BUF];
	unsigned char RsnIEWPA2Buf[MAX_SIZE_RSN_IE_BUF];
}
APICMDBUF_GET_THIS_STA_WPA2_RSN_IE;

typedef struct _APICMDBUF_GET_RSN_CFG_WPA2_UNICAST_CIPHER
{
	APICMDHEADER;
	unsigned char UnicastCipher[4];
	unsigned char Enabled;
}
APICMDBUF_GET_RSN_CFG_WPA2_UNICAST_CIPHER;

typedef struct _APICMDBUF_SET_RSN_CFG_WPA2_UNICAST_CIPHER
{
	APICMDHEADER;
	unsigned char UnicastCipher[4];
	unsigned char Enabled;
}
APICMDBUF_SET_RSN_CFG_WPA2_UNICAST_CIPHER;

typedef struct _APICMDBUF_GET_RSN_CFG_WPA2_UNICAST_CIPHER2
{
	APICMDHEADER;
	unsigned char UnicastCipher[4];
	unsigned char Enabled;
}
APICMDBUF_GET_RSN_CFG_WPA2_UNICAST_CIPHER2;

typedef struct _APICMDBUF_SET_RSN_CFG_WPA2_UNICAST_CIPHER2
{
	APICMDHEADER;
	unsigned char UnicastCipher[4];
	unsigned char Enabled;
}
APICMDBUF_SET_RSN_CFG_WPA2_UNICAST_CIPHER2;


typedef struct _APICMDBUF_GET_RSN_CFG_WPA2_AUTH_SUITE
{
	APICMDHEADER;
	unsigned char AuthSuites[4];
	unsigned char Enabled;
}
APICMDBUF_GET_RSN_CFG_WPA2_AUTH_SUITE;

typedef struct _APICMDBUF_SET_RSN_CFG_WPA2_AUTH_SUITE
{
	APICMDHEADER;
	unsigned char AuthSuites[4];
	unsigned char Enabled;
}
APICMDBUF_SET_RSN_CFG_WPA2_AUTH_SUITE;


typedef struct _APICMDBUF_GET_WPA2_ENABLED
{
	APICMDHEADER;
	unsigned long Enabled;
}
APICMDBUF_GET_WPA2_ENABLED;

typedef struct _APICMDBUF_SET_WPA2_ENABLED
{
	APICMDHEADER;
	unsigned long Enabled;
}
APICMDBUF_SET_WPA2_ENABLED;

typedef struct _APICMDBUF_SET_WPA2_PREAUTH_ENABLED
{
	APICMDHEADER;
	unsigned long Enabled;
}
APICMDBUF_SET_WPA2_PREAUTH_ENABLED;

typedef struct _APICMDBUF_GET_WPA2ONLY_ENABLED
{
	APICMDHEADER;
	unsigned long Enabled;
}
APICMDBUF_GET_WPA2ONLY_ENABLED;

typedef struct _APICMDBUF_SET_WPA2ONLY_ENABLED
{
	APICMDHEADER;
	unsigned long Enabled;
}
APICMDBUF_SET_WPA2ONLY_ENABLED;

#endif
/* GET/SET Baseband Reg */
typedef struct _APICMDBUF_GET_BB_REG{

	APICMDHEADER;
	unsigned short regNum;
	unsigned char regVal;

}
APICMDBUF_GET_BB_REG;

typedef struct _APICMDBUF_SET_BB_REG{

	APICMDHEADER;
	unsigned short regNum;
	unsigned char regVal;

}
APICMDBUF_SET_BB_REG;
typedef struct _APICMDBUF_GET_RF_REG{

	APICMDHEADER;
	unsigned short regNum;
	unsigned char regVal;
}
APICMDBUF_GET_RF_REG;

typedef struct _APICMDBUF_SET_RF_REG{

	APICMDHEADER;
	unsigned short regNum;
	unsigned char regVal; 
}
APICMDBUF_SET_RF_REG;

typedef struct _APICMDBUF_GET_FORCE_PROTECTION_MODE
{
	APICMDHEADER;
	unsigned short forceProtectMode;
}
APICMDBUF_GET_FORCE_PROTECTION_MODE;

typedef struct _APICMDBUF_SET_FORCE_PROTECTION_MODE
{
	APICMDHEADER;
	unsigned short forceProtectMode;
}
APICMDBUF_SET_FORCE_PROTECTION_MODE;

/* GET/SET Baseband Reg end*/
typedef struct _APICMDBUF_GET_SELF_CTS
{
	APICMDHEADER;
	unsigned char SelfCts;
}
APICMDBUF_GET_SELF_CTS;
typedef struct _APICMDBUF_SET_SELF_CTS
{
	APICMDHEADER;
	unsigned char SelfCts;
}

APICMDBUF_SET_SELF_CTS;

typedef struct _APICMDBUF_GET_BOOSTER_MODE
{
	APICMDHEADER;
	unsigned char BoosterMode;
}APICMDBUF_GET_BOOSTER_MODE;

typedef struct _APICMDBUF_SET_BOOSTER_MODE
{
	APICMDHEADER;
	unsigned char BoosterMode;
}APICMDBUF_SET_BOOSTER_MODE;


/* GET/SET Baseband Reg end*/

#ifdef WDS_FEATURE
typedef enum{
	WDS_MODE_DISABLE,
	WDS_MODE_STATIC,
	WDS_MODE_DYNAMIC,
}WDS_MODE_TYPE;
typedef enum {
	BCAST_AS_WDS_BCAST,
	BCAST_AS_WDS_UNICAST,
} WDS_BCAST_MODE_TYPE;
#endif // WDS_FEATURE
typedef struct _APICMDBUF_SET_TX_DATA_RATE_G
{
	APICMDHEADER;
	unsigned char txDataRate;
}APICMDBUF_SET_TX_DATA_RATE_G;

typedef struct _APICMDBUF_GET_RSN_CFG
{
	APICMDHEADER;
	unsigned long Version;
	unsigned long PairwiseKeysSupported;
	unsigned char MulticastCipher[4];
	unsigned char GroupRekeyMethod;
	unsigned long GroupRekeyTime;
	unsigned long GroupRekeyPackets;
	unsigned char GroupRekeyStrict;
	//unsigned char  PSKValue[32];  // write only
	//unsigned char  PSKPassPhrase[64]; // write only
	unsigned char TSNEnabled;
	unsigned long GroupMasterRekeyTime;
	unsigned long GroupUpdateTimeOut;
	unsigned long GroupUpdateCount;
	unsigned long PairwiseUpdateTimeOut;
	unsigned long PairwiseUpdateCount;
}
APICMDBUF_GET_RSN_CFG;

#ifdef AP_SITE_SURVEY
typedef struct _APICMDBUF_GET_SURVEY_INFO
{
	APICMDHEADER;
	unsigned char  busyScan;
	unsigned char  maxEntry;
	unsigned char  *infoAddr;

}APICMDBUF_GET_SURVEY_INFO;

typedef struct _API_SURVEY_ENTRY
{
	unsigned char dirty;
	unsigned char BssId[6];
	unsigned char SsId[32];
	unsigned char RSSI;
	unsigned char channel;
	unsigned char IBSS:1;
	unsigned char wepEnabled:1;
	unsigned char B_Support:1;
	unsigned char G_Support:1;
	unsigned char wpaEnabled:2;
	unsigned char A_Support:1;
	unsigned char reserve:1;

}API_SURVEY_ENTRY;
#endif /* AP_SITE_SURVEY */

#ifdef STA_INFO_DB
typedef struct _STA_INFO
{
	UINT8 macAdd[6];
	UINT8 assocState;
	UINT8 staMode;
	UINT8 rate;
	UINT8 sq2;
	UINT8 sq1;
	UINT8 RSSI;
	UINT8 powerSave;
}
PACK_END STA_INFO;
#else
typedef struct _STA_INFO
{
	UINT8 macAdd[6];
	UINT16 assocState;
}
PACK_END STA_INFO;
#endif
#if defined(WIPOD)
#define WIPOD_TYPE_AP 0x80
typedef enum
{
	WIPOD_MODE_CLIENT_BRIDGE = 0x01,
	WIPOD_MODE_AP = 0x82,
	WIPOD_MODE_REPEATER = 0x83,
	WIPOD_MODE_PT_2_PT = 0x84,
	WIPOD_MODE_PT_2_MULTIPT = 0x85
}
WIPOD_MODE;

typedef struct _APICMDBUF_GET_WIPOD_MODE
{
	APICMDHEADER;
	unsigned char  mode; /* use WIPOD_MODE enum */
	unsigned char  body[63];
}
APICMDBUF_GET_WIPOD_MODE;

typedef struct _APICMDBUF_SET_WIPOD_MODE
{
	APICMDHEADER;
	unsigned char mode; /* use WIPOD_MODE enum */
	unsigned char body[63];
}
APICMDBUF_SET_WIPOD_MODE;

typedef struct _APICMDBUF_WIPOD_REPEATER_MODE
{
	APICMDHEADER;
	unsigned char  mode; /* use WIPOD_MODE enum */
	unsigned char  macAddr1[6]; /* Set to all zero if not used */
	unsigned char  macAddr2[6]; /* Set to all zero if not used */
	unsigned char  enableApFunc; /* True, False */
}
APICMDBUF_WIPOD_REPEATER_MODE;

typedef struct _APICMDBUF_WIPOD_P2P_MODE
{
	APICMDHEADER;
	unsigned char  mode; /* use WIPOD_MODE enum */
	unsigned char  macAddr[6]; /* Set to all zero if not used */
}
APICMDBUF_WIPOD_P2P_MODE;

typedef struct _APICMDBUF_WIPOD_P2MP_MODE
{
	APICMDHEADER;
	unsigned char  mode; /* use WIPOD_MODE enum */
	unsigned char  macAddr1[6]; /* Set to all zero if not used */
	unsigned char  macAddr2[6]; /* Set to all zero if not used */
	unsigned char  macAddr3[6]; /* Set to all zero if not used */
	unsigned char  macAddr4[6]; /* Set to all zero if not used */
	unsigned char  macAddr5[6]; /* Set to all zero if not used */
	unsigned char  macAddr6[6]; /* Set to all zero if not used */
	unsigned char  enableDynamic; /* True or False */
}
APICMDBUF_WIPOD_P2MP_MODE;

#endif /* WIPOD */
enum
{
	POWER_ON,
	POWER_OFF,
};
typedef struct _APICMDBUF_SET_RF_POWER_MODE
{
	APICMDHEADER;
	unsigned char PowerMode;    /* POWER_ON, POWER_OFF */
}APICMDBUF_SET_RF_POWER_MODE;

#ifdef QOS_FEATURE
#include "mib.h"
typedef struct _APICMDBUF_SET_EDCA_PARAM
{
	APICMDHEADER;
	mib_QAPEDCATable_t QAPEDCATable;
}
APICMDBUF_SET_EDCA_PARAM;

typedef struct _APICMDBUF_GET_EDCA_PARAM
{
	APICMDHEADER;
	mib_QAPEDCATable_t QAPEDCATable;
}
APICMDBUF_GET_EDCA_PARAM;

typedef struct _APICMDBUF_GET_QOS_TS_INFO
{
	APICMDHEADER;
	unsigned char  maxEntry;
	unsigned char   *pInfo;

}APICMDBUF_GET_QOS_TS_INFO;

typedef struct _APICMDBUF_SET_QOS_MODE
{
	APICMDHEADER;
	unsigned char qos_mode;
}
APICMDBUF_SET_QOS_MODE;

typedef struct _APICMDBUF_GET_QOS_MODE
{
	APICMDHEADER;
	unsigned char qos_mode;
}
APICMDBUF_GET_QOS_MODE;

typedef struct _APICMDBUF_SET_NOACK_MODE

{
	APICMDHEADER;
	unsigned char noack_mode;

}

APICMDBUF_SET_NOACK_MODE;

typedef struct _APICMDBUF_GET_NOACK_MODE

{
	APICMDHEADER;
	unsigned char noack_mode;

}

APICMDBUF_GET_NOACK_MODE;

#endif

#endif

#ifdef IEEE80211H
typedef struct _APICMDBUF_GET_SPECTRUM_MGMT
{
	APICMDHEADER;
	UINT8 isEnable;
}
APICMDBUF_GET_SPECTRUM_MGMT;

typedef struct _APICMDBUF_SET_SPECTRUM_MGMT
{
	APICMDHEADER;
	UINT8 isEnable;
}
APICMDBUF_SET_SPECTRUM_MGMT;

typedef struct _APICMDBUF_GET_MITIGATION
{
	APICMDHEADER;
	UINT32 mitigationValue;
}
APICMDBUF_GET_MITIGATION;

typedef struct _APICMDBUF_SET_MITIGATION
{
	APICMDHEADER;
	UINT32 mitigationValue;
}
APICMDBUF_SET_MITIGATION;

typedef struct _APICMDBUF_SEND_SWITCH_CHANNEL_ANNOUNCEMENT
{
	APICMDHEADER;
	UINT8 channel; 
}
APICMDBUF_SEND_SWITCH_CHANNEL_ANNOUNCEMENT;

typedef struct _APICMDBUF_GET_REGULATORY_DOMAIN
{
	APICMDHEADER;
	UINT8 domain;
}
APICMDBUF_GET_REGULATORY_DOMAIN;

typedef struct _APICMDBUF_SET_REGULATORY_DOMAIN
{
	APICMDHEADER;
	UINT8 domain;
}
APICMDBUF_SET_REGULATORY_DOMAIN;
#endif /* IEEE80211H*/

#ifdef JAPAN_CHANNEL_SPACING_10_SUPPORT
typedef struct _APICMDBUF_GET_CHANNEL_SPACING
{
	APICMDHEADER;
	unsigned short curChanSpacing;
}
APICMDBUF_GET_CHANNEL_SPACING;

typedef struct _APICMDBUF_SET_CHANNEL_SPACING
{
	APICMDHEADER;
	unsigned char curChanSpacing;

}
APICMDBUF_SET_CHANNEL_SPACING;

#endif

typedef struct _APICMDBUF_SWITCH_PRIO_MAP_EN
{
	APICMDHEADER;
	unsigned long port;
	unsigned long mode;
}
APICMDBUF_SWITCH_PRIO_MAP_EN;

typedef struct _APICMDBUF_SWITCH_PRIO_MAP
{
	APICMDHEADER;
	unsigned long tag;
	unsigned long prio;
}
APICMDBUF_SWITCH_PRIO_MAP;

#ifdef BT_COEXISTENCE
typedef struct _APICMDBUF_BCA_CONFIG {
	APICMDHEADER;
	UINT16 mode;
	UINT16 antenna;
	UINT32 wlTxPri[2];      /* WLAN Tx Frame Priorities (64 bit mask) */
	UINT32 wlRxPri[2];      /* WLAN Rx Frame Priorities (64 bit mask) */
} APICMDBUF_BCA_CONFIG;
typedef struct _APICMDBUF_BCA_CONFIG_TIMESHARE {
	APICMDHEADER;
	UINT32 timeshareInterval;
	UINT32 btTime;
	UINT8  timeshareEnable;
} APICMDBUF_BCA_CONFIG_TIMESHARE;
typedef struct _APICMDBUF_BCA_ENABLED {
	APICMDHEADER;
	UINT8  Enabled;
} APICMDBUF_BCA_ENABLED;
typedef APICMDBUF_BCA_ENABLED APICMDBUF_BCA_TIMESHARE_ENABLED;
typedef struct _APICMDBUF_BCA_TIMESHARE_INTERVAL {
	APICMDHEADER;
	UINT32  timeshareInterval;
} APICMDBUF_BCA_TIMESHARE_INTERVAL;
typedef struct _APICMDBUF_BCA_TIMESHARE_BT_TIME {
	APICMDHEADER;
	UINT32  btTime;
} APICMDBUF_BCA_TIMESHARE_BT_TIME;
typedef struct _APICMDBUF_BCA_WLAN_RX_FRAME_PRIORITY {
	APICMDHEADER;
	UINT32  bcaWlRxPri[2];
} APICMDBUF_BCA_WLAN_RX_FRAME_PRIORITY;
typedef struct _APICMDBUF_BCA_WLAN_TX_FRAME_PRIORITY {
	APICMDHEADER;
	UINT32  bcaWlTxPri[2];
} APICMDBUF_BCA_WLAN_TX_FRAME_PRIORITY;
#endif

#endif
