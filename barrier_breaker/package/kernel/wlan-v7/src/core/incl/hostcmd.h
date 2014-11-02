
/******************** (c) Marvell Semiconductor, Inc., 2003 ********************
*
*  $HEADER$
*
*      File name: HostCmd.h
*
*      Purpose:
*
*      This file contains the function prototypes, data structure and defines for
*      all the host/station commands. Please check the Eagle 802.11 GUI/Driver/Station 
*      Interface Specification for detailed command information
*
*      Notes:
*
*****************************************************************************/
//
// (C) Copyright 2003, 2004,2005 Marvell Semiconductor, Inc. -- http://www.Marvell.com 
// All rights reserved.
//
// This copyright notice must be maintained in all derivative works.
//
// License to use this software is granted under the terms outlined in 
// the Marvell Software License Agreement.
//
// $Id: //depot/MRVL/BBU_EC85_11n/Common/hostcmd.h#15 $
// $DateTime: 2005/12/05 16:57:03 $
// $Author: kaic $
// Component  :	Common Headers
// Description: Driver/Firmware interface decleration
// Started    : 02/11/2005
// Environment: Kernel Mode
//

#ifndef __HOSTCMD__H
#define __HOSTCMD__H

#include "IEEE_types.h"

#define MAX_SUPPORTED_RATES						12
#define MAX_SUPPORTED_MCS                       32
#ifndef BIT
#define BIT(n) (1 << (n))
#endif

//=============================================================================
//          Driver Compile Option
//=============================================================================
#ifdef FIRMWARE
#ifdef IGX_QOS
#define IGX_QOS
#endif
#endif

//=============================================================================
//          Firmware Compile Option
//=============================================================================
#ifdef FIRMWARE
#ifdef IEEE_QOS
#define IGX_QOS
#endif
#define LED_CONTROL
#define IEEE802_11h
#define DOT_11A_SUPPORT
#define SEND_PEER_RATES
#endif
//=============================================================================
//=============================================================================
#define SUPERFLY 1
/* Capabilities Field */
typedef struct tagCAPABILITY_FIELD
{
	UINT16       ESS:                1;
	UINT16       IBSS:               1;
	UINT16       CFPollable:         1;
	UINT16       CFPollRequest:      1;
	UINT16       Privacy:            1;
	UINT16       ShortPreamble:      1;
	UINT16       PBCC:               1;
	UINT16       ChannelAgility:     1;
#ifdef IEEE802_11dh
	UINT16       SpectrumMgmt:       1;
#else
	UINT16       Rsvd1:              1;
#endif //IEEE802_11dh
#ifdef IGX_QOS
	UINT16       QOS:                1;
#else
	UINT16       Rsvd2:              1;
#endif	//IGX_QOS
	UINT16       ShortSlotTime:      1;
	UINT16       Rsvd3:              2;
	UINT16       DSSS_OFDM:          1;
#ifdef IGX_QOS
	UINT16       BlockAck:           1;
	UINT16       ExtCaps:            1;
#endif
} CAPABILITY_FIELD, *PCAPABILITY_FIELD;

/**
* Define total number of TX queues in the shared memory.
* This count includes the EDCA queues, Block Ack queues, and HCCA queues
* In addition to this, there could be a management packet queue some 
* time in the future
*/
#define NUM_EDCA_QUEUES					4

#ifdef SUPERFLY
#define NUM_HCCA_QUEUES				0
#define NUM_BA_QUEUES				0
#define NUM_MGMT_QUEUES				0
#else
#define NUM_HCCA_QUEUES				0
#define NUM_BA_QUEUES				0
#define NUM_MGMT_QUEUES				0
#endif // #ifdef MRVL_SUPERFLY

#ifdef MCAST_PS_OFFLOAD_SUPPORT
#define TOTAL_TX_QUEUES					NUM_EDCA_QUEUES + NUM_HCCA_QUEUES + NUM_BA_QUEUES + NUM_MGMT_QUEUES + NUMOFAPS
#else
#define TOTAL_TX_QUEUES					NUM_EDCA_QUEUES + NUM_HCCA_QUEUES + NUM_BA_QUEUES + NUM_MGMT_QUEUES
#endif
#define MAX_RXWCB_QUEUES				1	/** < There is only one receive queue for now. */

#ifdef NDIS_MINIPORT_DRIVER
#pragma pack(1)
#ifdef USE_CHAINED_DMA

#define MAX_DMA_DESC	10
//
//          Definition of data structure for each command
//
//          Define general data structure
typedef  struct tagDMA_DESC
{
	UINT32	ByteCount;
	UINT32	SrcAddr;
	UINT32	DestAddr;
	UINT32	NextAddr;
}DMA_DESC;

// WCB descriptor
//
// Status:		Current Tx packet transmit status
// PktLen:		Tx packet length
// NextWCBPtr:	Address to next WCB (Used by the driver)
// DmaDesc:		DMA Descriptor array for S/G DMA.
//
typedef struct _WCB {
	UINT32		WCBStatus;
	UINT32		NextWCBPtr;
	UINT16		PktLen;
	DMA_DESC		DmaDesc[ MAX_DMA_DESC ];
} WCB, *PWCB, RxPD, *PRxPD;

#else
//          WCB descriptor
//
//          Status                : Current Tx packet transmit status
//          PktPtr                : Physical address of the Tx Packet on host PC memory
//          PktLen                : Tx packet length
//          UINT8 DestMACAdrHi[2] : First 2 byte of destination MAC address
//          DestMACAdrLow[4]      : Last 2 byte of destination MAC address
//          DataRate              : Driver uses this field to specify data rate for the Tx packet
//          NextWCBPtr            : Address to next WCB (Used by the driver)
//
typedef struct _WCB {
	UINT32   WCBStatus;
	UINT8    DataRate;
#ifdef IGX_QOS
	UINT8    TxPriority;	// 0..7 or 8 for periodic
	QOS_CTRL QosCtrl;		// QoS Control
#else
	UINT8    NotUsed[3];
#endif
	UINT32   PktPtr;
	UINT16   PktLen;
	UINT8    DestMACAdrHi[2];
	UINT8    DestMACAdrLow[4];
	FWR_POINTER NextWCBPtr;
	UINT32   BClient;
	UINT32   Reserved;
} WCB, *PWCB;

//          RxPD descirptor
//			
//          Control     : Ownership indication
//          RSSI        : FW uses this field to report last Rx packet RSSI
//          Status      : Rx packet type
//          PktLen      : FW uses this field to report last Rx packet length 
//			SQBody			: Signal quality for packet body
//          PktPtr      : Driver uses this field to report host memory buffer size
//          NextRxPDPtr : Physical address of the next RxPD
//
typedef struct _RxPD {
	UINT8    RxControl;
	UINT8    RSSI;
#ifdef DOT_11A_SUPPORT
	UINT8    Status;
	UINT8    Channel;
#else
	UINT16   Status;
#endif
	UINT16   PktLen; 
	UINT8    SQBody;
	UINT8    Rate; 
	UINT32   PktPtr;
	UINT32   NextRxPDPtr;
#ifdef IGX_QOS
	UINT16   QosCtrl;
	UINT16   FWReserved;
#endif //IGX_QOS
	ULONG_PTR ulDriverReserved;

} RxPD, *PRxPD;
#endif	// USE_CHAINED_DMA
#pragma pack()
#endif

//=============================================================================
//          PUBLIC DEFINITIONS
//=============================================================================

#define RATE_INDEX_MAX_ARRAY        14
#define WOW_MAX_STATION         32

//***************************************************************************
//***************************************************************************
//
//          Define OpMode for SoftAP/Station mode
//
//  The following mode signature has to be written to PCI scratch register#0
//  right after successfully downloading the last block of firmware and
//  before waiting for firmware ready signature

#define HostCmd_STA_MODE     0x5A
#define HostCmd_SOFTAP_MODE  0xA5

#define HostCmd_STA_FWRDY_SIGNATURE     0xF0F1F2F4
#define HostCmd_SOFTAP_FWRDY_SIGNATURE  0xF1F2F4A5

//***************************************************************************
//***************************************************************************

//***************************************************************************

//
//          Define Command Processing States and Options
//
#define HostCmd_STATE_IDLE                      0x0000
#define HostCmd_STATE_IN_USE_BY_HOST            0x0001
#define HostCmd_STATE_IN_USE_BY_MINIPORT        0x0002
#define HostCmd_STATE_FINISHED                  0x000f

#define HostCmd_Q_NONE                          0x0000
#define HostCmd_Q_INIT                          0x0001
#define HostCmd_Q_RESET                         0x0002
#define HostCmd_Q_STAT                          0x0003

//
//	        Command pending states
//
#define HostCmd_PENDING_ON_NONE                 0x0000
#define HostCmd_PENDING_ON_MISC_OP              0x0001
#define HostCmd_PENDING_ON_INIT                 0x0002
#define HostCmd_PENDING_ON_RESET                0x0003
#define HostCmd_PENDING_ON_SET_OID              0x0004
#define HostCmd_PENDING_ON_GET_OID              0x0005
#define HostCmd_PENDING_ON_CMD                  0x0006
#define HostCmd_PENDING_ON_STAT                 0x0007

#define HostCmd_OPTION_USE_INT                  0x0000
#define HostCmd_OPTION_NO_INT                   0x0001

#define HostCmd_DELAY_NORMAL                    0x00000200  //  512 micro sec
#define HostCmd_DELAY_MIN                       0x00000100  //  256 micro sec
#define HostCmd_DELAY_MAX                       0x00000400  // 1024 micro sec

//***************************************************************************
//
//          16 bit host command code - HHH updated on 110201
//
#define HostCmd_CMD_NONE                        0x0000
#define HostCmd_CMD_CODE_DNLD                   0x0001
#define HostCmd_CMD_GET_HW_SPEC                 0x0003
#define HostCmd_CMD_SET_HW_SPEC 				0x0004
#define HostCmd_CMD_MAC_MULTICAST_ADR           0x0010
#define HostCmd_CMD_802_11_GET_STAT             0x0014
#define HostCmd_CMD_MAC_REG_ACCESS              0x0019
#define HostCmd_CMD_BBP_REG_ACCESS              0x001a
#define HostCmd_CMD_RF_REG_ACCESS               0x001b
#define HostCmd_CMD_802_11_RADIO_CONTROL        0x001c
#define HostCmd_CMD_MEM_ADDR_ACCESS             0x001d
#define HostCmd_CMD_802_11_RF_TX_POWER          0x001e
#define HostCmd_CMD_802_11_TX_POWER             0x001f
#define HostCmd_CMD_802_11_RF_ANTENNA           0x0020

#define HostCmd_CMD_MFG_COMMAND					0x0040

#define HostCmd_CMD_SET_BEACON                  0x0100
#define HostCmd_CMD_SET_PRE_SCAN                0x0107
#define HostCmd_CMD_SET_POST_SCAN               0x0108
#define HostCmd_CMD_SET_RF_CHANNEL              0x010a
#define HostCmd_CMD_SET_SENDPSPOLL              0x010c
#define HostCmd_CMD_SET_AID                     0x010d
#define HostCmd_CMD_SET_INFRA_MODE              0x010e
#define HostCmd_CMD_SET_G_PROTECT_FLAG          0x010f
#define HostCmd_CMD_SET_RATE                    0x0110
#define HostCmd_CMD_SET_FINALIZE_JOIN           0x0111

#define HostCmd_CMD_802_11_RTS_THSD             0x0113	// formerly 0x002E

#define HostCmd_CMD_802_11_SET_SLOT             0x0114	// formerly 0x002F

#define HostCmd_CMD_SET_EDCA_PARAMS             0x0115
#define HostCmd_CMD_802_11_BOOST_MODE           0x0116

#define HostCmd_CMD_PARENT_TSF                  0x0118
#define HostCmd_CMD_RPI_DENSITY                 0x0119
#define HostCmd_CMD_CCA_BUSY_FRACTION           0x011A

#ifdef IEEE80211_DH
// Define DFS lab commands
#define HostCmd_CMD_STOP_BEACON                 0x011d
#define HostCmd_CMD_802_11H_DETECT_RADAR		0x0120
#define HostCmd_CMD_802_11H_QUERY_DETECT_INFO   0x0121
#define HostCmd_CMD_802_11_RF_TX_POWER_REAL		0x0122
#endif /* IEEE80211_DH */
#define HostCmd_CMD_SET_WMM_MODE                0x0123
#define HostCmd_CMD_HT_GUARD_INTERVAL			0x0124
#define HostCmd_CMD_MIMO_CONFIG					0x0125
#define HostCmd_CMD_SET_FIXED_RATE              0x0126 
#define HostCmd_CMD_SET_IES                     0x0127 
#define HostCmd_CMD_SET_REGION_POWER            0x0128
#define HostCmd_CMD_SET_LINKADAPT_CS_MODE		0x0129
#define HostCmd_CMD_HT_GF_MODE                  0x0140
#define HostCmd_CMD_HT_TX_STBC                  0x0141
// Define LED control commands
#ifdef LED_CONTROL
#define HostCmd_CMD_LED_SET_INFORMATION         0x0fff
#define HostCmd_CMD_LED_GET_STATE               0x011b
#define HostCmd_CMD_LED_SET_STATE               0x011c
#endif

#define HostCmd_CMD_SET_PASSTHRU                0x01ff
#define HostCmd_CMD_SET_EAPOL_START             0x0201
#define HostCmd_CMD_SET_MAC_ADDR                0x0202
#define HostCmd_CMD_SET_RATE_ADAPT_MODE			0x0203
#define HostCmd_CMD_GET_NOISE_LEVEL             0x0204

#define HostCmd_CMD_GET_WATCHDOG_BITMAP         0x0205  /** this is different from main branch, it uses 204, need to fix **/
#define HostCmd_CMD_DEL_MAC_ADDR                0x0206

//SoftAP command code
#define HostCmd_CMD_BROADCAST_SSID_ENABLE       0x0050
#define HostCmd_CMD_BSS_START                   0x1100	
#define HostCmd_CMD_AP_BEACON                   0x1101	
#define HostCmd_CMD_UPDATE_PROBE                0x1102 	//Set the Probe response buffer or Update this buffer 30/9/2003
#define HostCmd_CMD_UPDATE_TIM                  0x1103
#define HostCmd_CMD_WDS_ENABLE                  0x1110
#define  HostCmd_CMD_SET_NEW_STN                0x1111
#define HostCmd_CMD_SET_KEEP_ALIVE              0x1112
#define HostCmd_CMD_SET_BURST_MODE              0x1113
#ifdef	AP_SCAN
#define HostCmd_CMD_AP_SCAN_FINISH              0x1117	//Close the AP Scan 2003/12/29
#endif
#define HostCmd_CMD_SET_APMODE                  0x1114

#ifdef	BURST_MODE
#define HostCmd_CMD_AP_BURST_MODE               0x1118	//???Set the Burst Mode 2004/02/04
#endif
#ifdef WOW
#define HostCmd_CMD_AP_SET_STANDBY              0x1119
#endif

#define HostCmd_CMD_GET_HW_CAPABILITY           0x1120
#ifdef IEEE80211_DH
#define HostCmd_CMD_SET_SWITCH_CHANNEL          0x1121
#define HostCmd_CMD_SET_SPECTRUM_MGMT           0x1128
#define HostCmd_CMD_SET_POWER_CONSTRAINT        0x1129
#define HostCmd_CMD_SET_COUNTRY_CODE            0x1130
#endif //IEEE80211_DH

/*
@HWENCR@
Command to update firmware encryption keys.
*/
#define HostCmd_CMD_UPDATE_ENCRYPTION			0x1122
/**
* @STADB@
* Command to update firmware station information database
*/
#define HostCmd_CMD_UPDATE_STADB				0x1123

/**
* Command to enable loopback mode
*/
#define HostCmd_CMD_SET_LOOPBACK_MODE           0x1124
/*
@11E-BA@
Command to create/destroy block ACK
*/
#define HostCmd_CMD_BASTREAM					0x1125
#define HostCmd_CMD_SET_RIFS                    0x1126
#define HostCmd_CMD_SET_N_PROTECT_FLAG          0x1131
#define HostCmd_CMD_SET_N_PROTECT_OPMODE        0x1132
#define HostCmd_CMD_SET_OPTIMIZATION_LEVEL      0x1133
#define HostCmd_CMD_GET_CALTABLE                0x1134
#define HostCmd_CMD_SET_MIMOPSHT                0x1135
// WSC Simple Config IE Set command
#define HostCmd_CMD_SET_WSC_IE                  0x1136
#define HostCmd_CMD_GET_RATETABLE               0x1137
#define HostCmd_CMD_GET_BEACON                  0x1138
#define HostCmd_CMD_SET_REGION_CODE             0x1139

#ifdef POWERSAVE_OFFLOAD 
#define HostCmd_CMD_SET_POWERSAVESTATION        0x1140
#define HostCmd_CMD_SET_TIM                     0x1141
#define HostCmd_CMD_GET_TIM                     0x1142
#endif
#define HostCmd_CMD_GET_SEQNO                   0x1143

#ifdef RXPATHOPT  /* RXPATHOPT is used only for V4-PIKA firmware.*/
#define HostCmd_CMD_SET_RXPATHOPT               0x1144
#endif
#ifdef V6FW /* DWDS is used for V5FW and V6FW firmware. */
#define HostCmd_CMD_DWDS_ENABLE					0x1144
#endif
#define HostCmd_CMD_FW_FLUSH_TIMER				0x1148
#ifdef COEXIST_20_40_SUPPORT
#define HostCmd_CMD_SET_11N_20_40_CHANNEL_SWITCH     0x1149    
#endif

#define HostCmd_CMD_SET_CDD                     0x1150

#ifdef EXPLICIT_BF
#define HostCmd_CMD_SET_BF                        0x1151
#define HostCmd_CMD_SET_NOACK	          0x1152
#define HostCmd_CMD_SET_NOSTEER	          0x1153
#define HostCmd_CMD_SET_TXHOP	   	0x1154
#define HostCmd_CMD_SET_BFTYPE              0x1155
#endif

#ifdef SSU_SUPPORT
#define HostCmd_CMD_SET_SPECTRAL_ANALYSIS   0x1156
#endif
#define HostCmd_CMD_CAU_REG_ACCESS 0x1157

#define HostCmd_CMD_RC_CAL         0x1158
#define HostCmd_CMD_GET_TEMP       0x1159
#ifdef QUEUE_STATS
#define HostCmd_CMD_GET_QUEUE_STATS         0x1160
#define HostCmd_CMD_RESET_QUEUE_STATS       0x1161
#define HostCmd_CMD_QSTATS_SET_SA           0x1162
#endif

#define HostCmd_CMD_SET_BW_SIGNALLING 		0x1163
#define HostCmd_CMD_GET_CONSEC_TXFAIL_ADDR	0x1164			
#define HostCmd_CMD_SET_TXFAILLIMIT			0x1165		
#define HostCmd_CMD_GET_TXFAILLIMIT			0x1166	
#define HostCmd_CMD_SET_WAPI_IE             		0x1167
#define HostCmd_CMD_SET_VHT_OP_MODE		0x1168			
#ifdef WNC_LED_CTRL
#define HostCmd_CMD_LED_CTRL					0x1169
#endif

// The test command.
#define HostCmd_CMD_TEST_SET_RATE_TABLE         0x2000
//***************************************************************************
//
//          16 bit RET code, MSB is set to 1
//
#define HostCmd_RET_NONE                        0x8000
#define HostCmd_RET_HW_SPEC_INFO                0x8003
#define HostCmd_RET_MAC_MULTICAST_ADR           0x8010
#define HostCmd_RET_802_11_STAT                 0x8014
#define HostCmd_RET_MAC_REG_ACCESS              0x8019
#define HostCmd_RET_BBP_REG_ACCESS              0x801a
#define HostCmd_RET_RF_REG_ACCESS               0x801b
#define HostCmd_RET_802_11_RADIO_CONTROL        0x801c
#define HostCmd_RET_802_11_RF_CHANNEL           0x801d
#define HostCmd_RET_802_11_RF_TX_POWER          0x801e
#define HostCmd_RET_802_11_RSSI                 0x801f
#define HostCmd_RET_802_11_RF_ANTENNA           0x8020
#define HostCmd_RET_802_11_PS_MODE              0x8021
#define HostCmd_RET_802_11_DATA_RATE            0x8022

#define HostCmd_RET_MFG_COMMAND					0x8040

#define HostCmd_RET_802_11_RTS_THSD             0x8113	// formerly 0x802E

#define HostCmd_RET_802_11_SET_SLOT             0x8114	// formerly 0x802F

#define HostCmd_RET_802_11_SET_CWMIN_MAX        0x8115
#define HostCmd_RET_802_11_BOOST_MODE           0x8116

#define HostCmd_RET_PARENT_TSF                  0x8118
#define HostCmd_RET_RPI_DENSITY                 0x8119
#define HostCmd_RET_CCA_BUSY_FRACTION           0x811A

#define HostCmd_RET_SET_EAPOL_START             0x8201
#define HostCmd_RET_SET_MAC_ADDR                0x8202 

#define HostCmd_RET_SET_BEACON                  0x8100
#define HostCmd_RET_SET_PRE_SCAN                0x8107
#define HostCmd_RET_SET_POST_SCAN               0x8108
#define HostCmd_RET_SET_RF_CHANNEL              0x810a
#define HostCmd_RET_SET_SENDPSPOLL              0x810c
#define HostCmd_RET_SET_AID                     0x810d
#define HostCmd_RET_SET_INFRA_MODE              0x810e
#define HostCmd_RET_SET_G_PROTECT_FLAG          0x810f
#define HostCmd_RET_SET_RATE                    0x8110
#define HostCmd_RET_SET_FINALIZE_JOIN           0x8111

#define HostCmd_RET_SET_PASSTHRU                0x81ff

// Define LED control RET code
#ifdef LED_CONTROL
#define HostCmd_RET_LED_SET_INFORMATION         0x8fff
#define HostCmd_RET_LED_GET_STATE               0x811b
#define HostCmd_RET_LED_SET_STATE               0x811c
#endif

#ifdef IEEE802_11dh
// Define DFS lab commands
#define HostCmd_RET_STOP_BEACON                 0x811d
#endif

#define HostCmd_RET_SET_WMM_MODE                0x8123

#define HostCmd_RET_HT_GUARD_INTERVAL			0x8124
#define HostCmd_RET_MIMO_CONFIG					0x8125

//SoftAP command code
#define HostCmd_RET_GET_HW_CAPABILITY           0x9120
#ifdef IEEE802_11dh
#define HostCmd_RET_SET_SWITCH_CHANNEL          0x9121
#endif
/*
@HWENCR@
ID for encryption response from firmware
*/
#define HostCmd_RET_UPDATE_ENCRYPTION			0x9122
/**
* @STADB@
* Command to update firmware station information database
*/
#define HostCmd_RET_UPDATE_STADB				0x9123

/**
* Command to enable loopback mode
*/
#define HostCmd_RET_SET_LOOPBACK_MODE           0x9124
/*
@11E-BA@
Command to create/destroy block ACK
*/
#define HostCmd_RET_BASTREAM					0x9125

//***************************************************************************
// 
//          Define general result code for each command
//
#define HostCmd_RESULT_OK                       0x0000 // OK
#define HostCmd_RESULT_ERROR                    0x0001 // Genenral error
#define HostCmd_RESULT_NOT_SUPPORT              0x0002 // Command is not valid
#define HostCmd_RESULT_PENDING                  0x0003 // Command is pending (will be processed)
#define HostCmd_RESULT_BUSY                     0x0004 // System is busy (command ignored)
#define HostCmd_RESULT_PARTIAL_DATA             0x0005 // Data buffer is not big enough


//***************************************************************************
// 
//          Definition of action or option for each command
//
//          Define general purpose action
//
#define HostCmd_ACT_GEN_READ                    0x0000
#define HostCmd_ACT_GEN_WRITE                   0x0001
#define HostCmd_ACT_GEN_GET                     0x0000
#define HostCmd_ACT_GEN_SET                     0x0001
#define HostCmd_ACT_GEN_OFF                     0x0000
#define HostCmd_ACT_GEN_ON                      0x0001

#define HostCmd_ACT_DIFF_CHANNEL                0x0002
#define HostCmd_ACT_GEN_SET_LIST                0x0002
#define HostCmd_ACT_GEN_GET_LIST                0x0003
//          Define action or option for HostCmd_FW_USE_FIXED_RATE
#define HostCmd_ACT_USE_FIXED_RATE              0x0001
#define HostCmd_ACT_NOT_USE_FIXED_RATE          0x0002
//          Define action or option for HostCmd_CMD_802_11_AUTHENTICATE
#define HostCmd_ACT_AUTHENTICATE                0x0001
#define HostCmd_ACT_DEAUTHENTICATE              0x0002

//          Define action or option for HostCmd_CMD_802_11_ASSOCIATE
#define HostCmd_ACT_ASSOCIATE                   0x0001
#define HostCmd_ACT_DISASSOCIATE                0x0002
#define HostCmd_ACT_REASSOCIATE                 0x0003

#define HostCmd_CAPINFO_ESS                     0x0001
#define HostCmd_CAPINFO_IBSS                    0x0002
#define HostCmd_CAPINFO_CF_POLLABLE             0x0004
#define HostCmd_CAPINFO_CF_REQUEST              0x0008
#define HostCmd_CAPINFO_PRIVACY                 0x0010

//          Define action or option for HostCmd_CMD_802_11_SET_WEP
//#define HostCmd_ACT_ENABLE                    0x0001 // Use MAC control for WEP on/off
//#define HostCmd_ACT_DISABLE                   0x0000
#define HostCmd_ACT_ADD                         0x0002
#define HostCmd_ACT_REMOVE                      0x0004
#define HostCmd_ACT_USE_DEFAULT                 0x0008

#define HostCmd_TYPE_WEP_40_BIT                 0x0001 // 40 bit
#define HostCmd_TYPE_WEP_104_BIT                0x0002 // 104 bit
#define HostCmd_TYPE_WEP_128_BIT                0x0003 // 128 bit
#define HostCmd_TYPE_WEP_TX_KEY                 0x0004 // TX WEP

#define HostCmd_NUM_OF_WEP_KEYS                 4

#define HostCmd_WEP_KEY_INDEX_MASK              0x3fffffff


//          Define action or option for HostCmd_CMD_802_11_RESET
#define HostCmd_ACT_HALT                        0x0001
#define HostCmd_ACT_RESTART                     0x0002
//#define HostCmd_ACT_NOT_REVERT_MIB              0x0001
//#define HostCmd_ACT_REVERT_MIB                  0x0002
//#define HostCmd_ACT_HALT                        0x0003

//          Define action or option for HostCmd_CMD_802_11_SCAN
#define HostCmd_BSS_TYPE_BSS                    0x0001
#define HostCmd_BSS_TYPE_IBSS                   0x0002
#define HostCmd_BSS_TYPE_ANY                    0x0003

//          Define action or option for HostCmd_CMD_802_11_SCAN
#define HostCmd_SCAN_TYPE_ACTIVE                0x0000
#define HostCmd_SCAN_TYPE_PASSIVE               0x0001

#define HostCmd_SCAN_802_11_B_CHANNELS          11

#define HostCmd_SCAN_MIN_CH_TIME                6
#define HostCmd_SCAN_MAX_CH_TIME                12

#define HostCmd_SCAN_PROBE_DELAY_TIME           0

//          Define action or option for HostCmd_CMD_802_11_QUERY_STATUS
#define HostCmd_STATUS_FW_INIT                  0x0000
#define HostCmd_STATUS_FW_IDLE                  0x0001
#define HostCmd_STATUS_FW_WORKING               0x0002
#define HostCmd_STATUS_FW_ERROR                 0x0003
#define HostCmd_STATUS_FW_POWER_SAVE            0x0004

#define HostCmd_STATUS_MAC_RX_ON                0x0001
#define HostCmd_STATUS_MAC_TX_ON                0x0002
#define HostCmd_STATUS_MAC_LOOP_BACK_ON         0x0004
#define HostCmd_STATUS_MAC_WEP_ENABLE           0x0008
#define HostCmd_STATUS_MAC_INT_ENABLE           0x0010

//          Define action or option for HostCmd_CMD_MAC_CONTROL 
#define HostCmd_ACT_MAC_RX_ON                   0x0001
#define HostCmd_ACT_MAC_TX_ON                   0x0002
#define HostCmd_ACT_MAC_LOOPBACK_ON             0x0004
#define HostCmd_ACT_MAC_WEP_ENABLE              0x0008
#define HostCmd_ACT_MAC_INT_ENABLE              0x0010
#define HostCmd_ACT_MAC_MULTICAST_ENABLE        0x0020
#define HostCmd_ACT_MAC_BROADCAST_ENABLE        0x0040
#define HostCmd_ACT_MAC_PROMISCUOUS_ENABLE      0x0080
#define HostCmd_ACT_MAC_ALL_MULTICAST_ENABLE    0x0100

//          Define action or option or constant for HostCmd_CMD_MAC_MULTICAST_ADR
#define HostCmd_SIZE_MAC_ADR                    6
#define HostCmd_MAX_MCAST_ADRS                  32

#define NDIS_PACKET_TYPE_DIRECTED               0x00000001
#define NDIS_PACKET_TYPE_MULTICAST              0x00000002
#define NDIS_PACKET_TYPE_ALL_MULTICAST          0x00000004
#define NDIS_PACKET_TYPE_BROADCAST              0x00000008
#define NDIS_PACKET_TYPE_PROMISCUOUS            0x00000020

//          Define action or option for HostCmd_CMD_802_11_SNMP_MIB 
#define HostCmd_TYPE_MIB_FLD_BOOLEAN            0x0001 // Boolean
#define HostCmd_TYPE_MIB_FLD_INTEGER            0x0002 // 32 UINT8 unsigned integer
#define HostCmd_TYPE_MIB_FLD_COUNTER            0x0003 // Counter
#define HostCmd_TYPE_MIB_FLD_OCT_STR            0x0004 // Octet string
#define HostCmd_TYPE_MIB_FLD_DISPLAY_STR        0x0005 // String
#define HostCmd_TYPE_MIB_FLD_MAC_ADR            0x0006 // MAC address
#define HostCmd_TYPE_MIB_FLD_IP_ADR             0x0007 // IP address
#define HostCmd_TYPE_MIB_FLD_WEP                0x0008 // WEP


//          Define action or option for HostCmd_CMD_802_11_RADIO_CONTROL 
#define HostCmd_TYPE_AUTO_PREAMBLE              0x0001
#define HostCmd_TYPE_SHORT_PREAMBLE             0x0002
#define HostCmd_TYPE_LONG_PREAMBLE              0x0003

#define SET_AUTO_PREAMBLE						0x05
#define SET_SHORT_PREAMBLE						0x03
#define SET_LONG_PREAMBLE						0x01
//          Define action or option for CMD_802_11_RF_CHANNEL
#define HostCmd_TYPE_802_11A                    0x0001
#define HostCmd_TYPE_802_11B                    0x0002

//          Define action or option for HostCmd_CMD_802_11_RF_TX_POWER
#define HostCmd_ACT_TX_POWER_OPT_SET_HIGH       0x0003
#define HostCmd_ACT_TX_POWER_OPT_SET_MID        0x0002
#define HostCmd_ACT_TX_POWER_OPT_SET_LOW        0x0001
#define HostCmd_ACT_TX_POWER_OPT_SET_AUTO        0x0000

/*
#define HostCmd_ACT_TX_POWER_OPT_GET            0x0000
#define HostCmd_ACT_TX_POWER_OPT_SET_HIGH       0x8007
#define HostCmd_ACT_TX_POWER_OPT_SET_MID        0x8004
#define HostCmd_ACT_TX_POWER_OPT_SET_LOW        0x8000

#define HostCmd_ACT_TX_POWER_INDEX_HIGH         0x0007
#define HostCmd_ACT_TX_POWER_INDEX_MID          0x0004
#define HostCmd_ACT_TX_POWER_INDEX_LOW          0x0000
*/

#define HostCmd_ACT_TX_POWER_LEVEL_MIN          0x000e // in dbm
#define HostCmd_ACT_TX_POWER_LEVEL_GAP          0x0001 // in dbm
//          Define action or option for HostCmd_CMD_802_11_DATA_RATE 
#define HostCmd_ACT_SET_TX_AUTO					0x0000
#define HostCmd_ACT_SET_TX_FIX_RATE				0x0001
#define HostCmd_ACT_GET_TX_RATE					0x0002

#define HostCmd_ACT_SET_RX                      0x0001
#define HostCmd_ACT_SET_TX                      0x0002
#define HostCmd_ACT_SET_BOTH                    0x0003
#define HostCmd_ACT_GET_RX                      0x0004
#define HostCmd_ACT_GET_TX                      0x0008
#define HostCmd_ACT_GET_BOTH                    0x000c

#define TYPE_ANTENNA_DIVERSITY                  0xffff

//          Define action or option for HostCmd_CMD_802_11_PS_MODE 
#define HostCmd_TYPE_CAM                        0x0000
#define HostCmd_TYPE_MAX_PSP                    0x0001
#define HostCmd_TYPE_FAST_PSP                   0x0002

//          Define LED control command state
#ifdef LED_CONTROL
#define HostCmd_STATE_LED_HALTED                0x00
#define HostCmd_STATE_LED_IDLE                  0x01
#define HostCmd_STATE_LED_SCAN                  0x02
#define HostCmd_STATE_LED_AUTHENTICATED         0x03
#define HostCmd_STATE_LED_BSS_ASSO_IN_PROGRESS  0x04
#define HostCmd_STATE_LED_BSS_ASSOCIATED        0x05
#define HostCmd_STATE_LED_IBSS_JOINED           0x06
#define HostCmd_STATE_LED_IBSS_STARTED          0x07
#define HostCmd_STATE_LED_TX_TRAFFIC            0x08
#define HostCmd_STATE_LED_RX_TRAFFIC            0x09
#define HostCmd_STATE_LED_TX_TRAFFIC_LOW_RATE   0x0a
#define HostCmd_STATE_LED_RX_TRAFFIC_LOW_RATE   0x0b
#define HostCmd_STATE_LED_TX_TRAFFIC_HIGH_RATE  0x0c
#define HostCmd_STATE_LED_RX_TRAFFIC_HIGH_RATE  0x0d

#define HostCmd_STATE_LED_ENCYP_ON              0x10
#define HostCmd_STATE_LED_AP_MODE               0x11
#define HostCmd_STATE_LED_IN_PS_MODE            0x12
#define HostCmd_STATE_LED_IN_MAX_PSP_PS_MODE    0x13
#define HostCmd_STATE_LED_IN_FAST_PSP_PS_MODE   0x14
#define HostCmd_STATE_LED_IN_CMS_PS_MODE        0x15

#define HostCmd_STATE_LED_B_BAND_RF_ON          0x20
#define HostCmd_STATE_LED_B_BAND_RF_OFF         0x21
#define HostCmd_STATE_LED_A_BAND_RF_ON          0x22
#define HostCmd_STATE_LED_A_BAND_RF_OFF         0x23
#define HostCmd_STATE_LED_B_MODE_ON             0x24
#define HostCmd_STATE_LED_B_MODE_OFF            0x25
#define HostCmd_STATE_LED_G_MODE_ON             0x26
#define HostCmd_STATE_LED_G_MODE_OFF            0x27
#define HostCmd_STATE_LED_A_MODE_ON             0x28
#define HostCmd_STATE_LED_A_MODE_OFF            0x29

#define HostCmd_STATE_LED_1_MBPS                0x30
#define HostCmd_STATE_LED_2_MBPS                0x31
#define HostCmd_STATE_LED_5_AND_HALF_MBPS       0x32
#define HostCmd_STATE_LED_11_MBPS               0x33
#define HostCmd_STATE_LED_22_MBPS               0x34
#define HostCmd_STATE_LED_6_MBPS                0x35
#define HostCmd_STATE_LED_9_MBPS                0x36
#define HostCmd_STATE_LED_12_MBPS               0x37
#define HostCmd_STATE_LED_18_MBPS               0x38
#define HostCmd_STATE_LED_24_MBPS               0x39
#define HostCmd_STATE_LED_36_MBPS               0x3a
#define HostCmd_STATE_LED_48_MBPS               0x3b
#define HostCmd_STATE_LED_54_MBPS               0x3c
#define HostCmd_STATE_LED_72_MBPS               0x3d

#define HostCmd_STATE_LED_DIAG_MODE             0xfd
#define HostCmd_STATE_LED_FW_UPDATE             0xfe
#define HostCmd_STATE_LED_HW_ERROR              0xff

//          Define LED control MASK
#define HostCmd_MASK_TYPE_PIN_MASK              0x00
#define HostCmd_MASK_TYPE_POWER_ON              0x01
#define HostCmd_MASK_TYPE_POWER_OFF             0x02

//          Define LED control command pattern
#define HostCmd_PATTERN_LED_37_MS_BLINK         0x00
#define HostCmd_PATTERN_LED_74_MS_BLINK         0x01
#define HostCmd_PATTERN_LED_149_MS_BLINK        0x02
#define HostCmd_PATTERN_LED_298_MS_BLINK        0x03
#define HostCmd_PATTERN_LED_596_MS_BLINK        0x04
#define HostCmd_PATTERN_LED_1192_MS_BLINK       0x05

#define HostCmd_PATTERN_LED_250_MS_STRETCH      0x12
#define HostCmd_PATTERN_LED_250_MS_OFF          0x22

#define HostCmd_PATTERN_LED_AUTO                0xfc
#define HostCmd_PATTERN_LED_STAY_CURRENT        0xfd
#define HostCmd_PATTERN_LED_STAY_OFF            0xfe
#define HostCmd_PATTERN_LED_STAY_ON             0xff
#endif

#ifdef IGX_QOS
#define HostCmd_CMD_SET_EDCA_PARAMS             0x0115
#define HostCmd_CMD_SET_BA_PARAMS               0x0031
#define HostCmd_CMD_SET_CFP                     0x0032
#define HostCmd_CMD_SET_HCCA                    0x0033
#define HostCmd_CMD_SET_MEDIUM_TIME             0x0034
#define HostCmd_RET_SET_EDCA_PARAMS             0x8115
#endif // end of IGX_QOS

//=============================================================================
//			HOST COMMAND DEFINITIONS
//=============================================================================

#ifdef NDIS_MINIPORT_DRIVER
#pragma pack(1)
#endif

//
//          Definition of data structure for each command
//
//          Define general data structure
typedef PACK_START struct tagFWCmdHdr
{
	UINT16     Cmd;
	UINT16     Length;
	UINT8      SeqNum;
	UINT8      macid;
	UINT16     Result; 
} PACK_END FWCmdHdr, *PFWCmdHdr;  


#define SIZE_FJ_BEACON_BUFFER 128
typedef PACK_START struct _HostCmd_FW_SET_FINALIZE_JOIN {
	FWCmdHdr    CmdHdr;
	UINT32      ulSleepPeriod;	// Number of beacon periods to sleep
	UINT8       BeaconBuffer[SIZE_FJ_BEACON_BUFFER];
} PACK_END HostCmd_FW_SET_FINALIZE_JOIN, *PHostCmd_FW_SET_FINALIZE_JOIN;

#define HW_SPEC_WCBBASE_OFFSET   0
#define HW_SPEC_WCBBASE1_OFFSET  4
#ifdef MFG
typedef PACK_START struct _HostCmd_MFG_CMD{ 

	ULONG    	MfgCmd; 
	ULONG 		Action; 
	ULONG 		Error;
} PACK_END HostCmd_MFG_CMD, *PHostCmd_MFG_CMD;
#endif
//          Define data structure for HostCmd_CMD_MAC_MULTICAST_ADR
typedef PACK_START struct _HostCmd_DS_MAC_MULTICAST_ADR {
	FWCmdHdr    CmdHdr;
	UINT16      Action;
	UINT16      NumOfAdrs;
	UINT8       MACList[HostCmd_SIZE_MAC_ADR*HostCmd_MAX_MCAST_ADRS];
} PACK_END HostCmd_DS_MAC_MULTICAST_ADR, *PHostCmd_DS_MAC_MULTICAST_ADR;



// called before a mlme bss scan in lounched
// to configure the hardware for the scan
// also used as read in init and disconnect
// This cmd is tied to settin packet filtering
// for scanning in both init and scan calls
typedef PACK_START struct tagHostCmd_FW_SET_PRESCAN
{
	FWCmdHdr    CmdHdr;
	UINT32      TsfTime;
} PACK_END HostCmd_FW_SET_PRESCAN, *PHostCmd_FW_SET_PRESCAN,
HostCmd_FW_SET_DISCONNECT, *PHostCmd_FW_SET_DISCONNECT,
HostCmd_FW_GET_TSF, *PHostCmd_FW_GET_TSF;


typedef PACK_START struct tagHostCmd_FW_SET_EAPOL
{
	FWCmdHdr    CmdHdr;
	UINT16      bAction;
} PACK_END HostCmd_FW_SET_EAPOL, *PHostCmd_FW_SET_EAPOL;

// called to set the hardware back to its pre Scan state
typedef PACK_START struct tagHostCmd_FW_SET_POSTSCAN
{
	FWCmdHdr    CmdHdr;
	UINT32      IsIbss;
	UINT8       BssId[6];
} PACK_END HostCmd_FW_SET_POSTSCAN, *PHostCmd_FW_SET_POSTSCAN;

// Indicate to FW the current state of AP ERP info
typedef PACK_START struct tagHostCmd_FW_SET_G_PROTECT_FLAG
{
	FWCmdHdr    CmdHdr;
	UINT32      GProtectFlag;
} PACK_END HostCmd_FW_SET_G_PROTECT_FLAG, *PHostCmd_FW_SET_G_PROTECT_FLAG;

typedef PACK_START struct tagHostCmd_FW_SET_INFRA_MODE
{
	FWCmdHdr    CmdHdr;
} PACK_END HostCmd_FW_SET_INFRA_MODE, *PHostCmd_FW_SET_INFRA_MODE;


typedef PACK_START struct _HostCmd_FW_GET_WATCHDOG_BITMAP 
{
	FWCmdHdr    CmdHdr;
	UINT8     Watchdogbitmap;                 // for SW/BA
} PACK_END HostCmd_FW_GET_WATCHDOG_BITMAP, *PHostCmd_FW_GET_WATCHDOG_BITMAP;


#ifndef SUPERFLY
//          Define data structure for HostCmd_CMD_802_11_RF_CHANNEL
typedef PACK_START struct tagHostCmd_FW_RF_CHANNEL {
	FWCmdHdr    CmdHdr;
	UINT16      Action;
	UINT8       CurrentChannel;
#ifdef DOT_11A_SUPPORT
	UINT8		   RFType;		//bit0: 0 = 2.4 GHz,
	//      1 = 5.0 GHz
	//bit1: 0 = 20 MHz Channel spacing
	//      1 = 10 MHz Channel spacing
#endif
} PACK_END HostCmd_FW_SET_RF_CHANNEL, *PHostCmd_FW_SET_RF_CHANNEL;

#define FREQ_5GHZ            0x1
#define _10_MHz_CH_SPACING   0x2

#else

#define FREQ_BAND_2DOT4GHZ	0x1 
#define FREQ_BAND_4DOT9GHZ	0x2
#define FREQ_BAND_5GHZ      0x4
#define FREQ_BAND_5DOT2GHZ	0x8 
#define CH_AUTO_WIDTH  	0
#define CH_10_MHz_WIDTH  	0x1
#define CH_20_MHz_WIDTH  	0x2
#define CH_40_MHz_WIDTH  	0x4
#if defined(SOC_W8897) || defined(SOC_W8864)
#define CH_80_MHz_WIDTH     0x5
#endif
#define EXT_CH_ABOVE_CTRL_CH 0x1
#define EXT_CH_AUTO			0x2
#define EXT_CH_BELOW_CTRL_CH 0x3
#define NO_EXT_CHANNEL		 0x0

#ifdef SOC_W8864
#define ACT_PRIMARY_CHAN_0  0 /* active primary 1st 20MHz channel */
#define ACT_PRIMARY_CHAN_1  1 /* active primary 2nd 20MHz channel */
#define ACT_PRIMARY_CHAN_2  2 /* active primary 3rd 20MHz channel */
#define ACT_PRIMARY_CHAN_3  3 /* active primary 4th 20MHz channel */
#define ACT_PRIMARY_CHAN_4  4 /* active primary 5th 20MHz channel */
#define ACT_PRIMARY_CHAN_5  5 /* active primary 6th 20MHz channel */
#define ACT_PRIMARY_CHAN_6  6 /* active primary 7th 20MHz channel */
#define ACT_PRIMARY_CHAN_7  7 /* active primary 8th 20MHz channel */

typedef PACK_START struct tagChnFlags11ac
{
	UINT32	FreqBand: 6;//bit0=1: 2.4GHz,bit1=1: 4.9GHz,bit2=1: 5GHz,bit3=1: 5.2GHz,
	UINT32	ChnlWidth: 5;//bit6=1:10MHz, bit7=1:20MHz, bit8=1:40MHz
	UINT32	ActPrimary: 3;//000: 1st 20MHz chan, 001:2nd 20MHz chan, 011:3rd 20MHz chan, 100:4th 20MHz chan 
	UINT32	Reserved: 18;
} CHNL_FLAGS_11AC, *PCHNL_FLAGS_11AC;
#endif

typedef PACK_START struct tagChnFlags
{
	UINT32	FreqBand: 6;//bit0=1: 2.4GHz,bit1=1: 4.9GHz,bit2=1: 5GHz,bit3=1: 5.2GHz,
	UINT32	ChnlWidth: 5;//bit6=1:10MHz, bit7=1:20MHz, bit8=1:40MHz
	UINT32	ExtChnlOffset: 2;//00: no extension, 01:above, 11:below 
	UINT32	Reserved: 19;
} CHNL_FLAGS, *PCHNL_FLAGS;

//          Define data structure for HostCmd_CMD_802_11_RF_CHANNEL
typedef PACK_START struct tagHostCmd_FW_RF_CHANNEL {
	FWCmdHdr    CmdHdr;
	UINT16      Action;
	UINT8       CurrentChannel;
#ifdef SOC_W8864
	CHNL_FLAGS_11AC ChannelFlags;
#else
	CHNL_FLAGS  	ChannelFlags;
#endif
	/*UINT8        initRateTable;  *//* un-used field in current FW */
} PACK_END HostCmd_FW_SET_RF_CHANNEL, *PHostCmd_FW_SET_RF_CHANNEL;

#ifdef SUPERFLY
typedef PACK_START struct tagHostCmd_FW_SET_RATE
{
	//
	//If HT mode is enabled, then HTMCSCodeSet will also contain one MCS code to be used as fixed rate (if applicable).	
	//
	FWCmdHdr    CmdHdr;
	//	UINT8       DataRateType;   // 0=Auto, Rate Adaption ON, 1=Legacy Fixed,2=HT fixed. No rate adaption
	//UINT8       RateIndex;     // Used for fixed rate - if fixed, then fill the index with the following condition
	// for LegacyRates, filled with index(0-9)
	//for HT, set RateIndex=0xff
	UINT8       LegacyRates[ RATE_INDEX_MAX_ARRAY];
	UINT8 		HTMCSCodeSet[16]; // Bit map for supported MCS codes.  
	//not used as of 11/30/05
	UINT8 		HTBasicMCSCodeSet[16]; // Bit map for supported basic MCS codes.
} PACK_END HostCmd_FW_SET_RATE, *PHostCmd_FW_SET_RATE;
#endif
#define FIXED_RATE_WITH_AUTO_RATE_DROP           0
#define FIXED_RATE_WITHOUT_AUTORATE_DROP        1

#define LEGACY_RATE_TYPE   0
#define HT_RATE_TYPE  	1

#define RETRY_COUNT_VALID   0 
#define RETRY_COUNT_INVALID     1

typedef  struct tagFIX_RATE_FLAG
{
	// lower rate after the retry count
	UINT32   FixRateType;	//0: legacy, 1: HT
	UINT32   RetryCountValid; //0: retry count is not valid, 1: use retry count specified
} PACK_END FIX_RATE_FLAG, *PFixRateFlag;

typedef  struct FixRateEntry
{
	FIX_RATE_FLAG 	FixRateTypeFlags;
	UINT32 		FixedRate; // depending on the flags above, this can be either a legacy rate(not index) or an MCS code.
	UINT32		RetryCount;
} PACK_END FIXED_RATE_ENTRY;

typedef  struct tagHostCmd_FW_USE_FIXED_RATE
{
	FWCmdHdr 			CmdHdr;
	UINT32      Action;	//HostCmd_ACT_GEN_GET						0x0000
	//HostCmd_ACT_GEN_SET 0x0001
	//HostCmd_ACT_NOT_USE_FIXED_RATE          	0x0002
	UINT32   	AllowRateDrop;  // use fixed rate specified but firmware can drop to 
	UINT32				EntryCount;
	FIXED_RATE_ENTRY	FixedRateTable[4];
	UINT8				MulticastRate;
	UINT8				MultiRateTxType;
	UINT8				ManagementRate;
} PACK_END HostCmd_FW_USE_FIXED_RATE, *PHostCmd_FW_USE_FIXED_RATE;

typedef  struct tagUseFixedRateInfo
{
	UINT32   			AllowRateDrop;   
	UINT32				EntryCount;
	FIXED_RATE_ENTRY	FixedRateTable[4];
} PACK_END USE_FIXED_RATE_INFO, *PUseFixedRateInfo;

typedef PACK_START struct tagGI_TYPE
{
	UINT32   LongGI  :1;
	UINT32   ShortGI :1; 
	UINT32   RESV:   30;
} PACK_END GI_TYPE, *PGIType;

typedef PACK_START struct tagHostCmd_FW_HT_GUARD_INTERVAL {
	FWCmdHdr    CmdHdr;
	UINT32      Action;
	GI_TYPE     GIType;  
} PACK_END HostCmd_FW_HT_GUARD_INTERVAL, *PHostCmd_FW_HT_GUARD_INTERVAL;

typedef PACK_START struct tagHostCmd_FW_HT_MIMO_CONFIG {
	FWCmdHdr		CmdHdr;
	UINT32      	Action; 
	UINT8		RxAntennaMap;
	UINT8		TxAntennaMap;
} PACK_END HostCmd_FW_HT_MIMO_CONFIG, *PHostCmd_FW_HT_MIMO_CONFIG;
#endif

typedef PACK_START struct tagHostCmd_FW_SET_SLOT {
	FWCmdHdr    CmdHdr;
	UINT16      Action;
	UINT8       Slot  ;   // Slot=0 if regular, Slot=1 if short.
} PACK_END HostCmd_FW_SET_SLOT, *PHostCmd_FW_SET_SLOT;

//          Define data structure used in HostCmd_CMD_802_11_GET_STAT
#ifdef QUEUE_STATS
#define QS_GET_TX_COUNTER   1
#define QS_GET_TX_LATENCY   2
#define QS_GET_RX_LATENCY   3
#define QS_GET_RETRY_HIST   4
#define QS_GET_TX_RATE_HIST 5
#define QS_GET_RX_RATE_HIST 6

#define NUM_OF_TCQ 8
#define NUM_OF_RETRY_BIN 64
#define NUM_OF_HW_BA 2
#define QS_MAX_DATA_RATES_G 14
#define QS_MAX_SUPPORTED_MCS 24
#define QS_NUM_SUPPORTED_RATES_G 12
#define QS_NUM_SUPPORTED_MCS    17
#define QS_NUM_SUPPORTED_11N_BW 2
#define QS_NUM_SUPPORTED_GI 2
#if defined(SOC_W8897) || defined(SOC_W8864)
#define QS_NUM_SUPPORTED_11AC_MCS 10
#define QS_NUM_SUPPORTED_11AC_BW 3
#endif
#if defined(SOC_W8897)
#define QS_NUM_SUPPORTED_11AC_NSS 2
#elif defined(SOC_W8864)
#define QS_NUM_SUPPORTED_11AC_NSS 3
#endif
#define QS_NUM_STA_SUPPORTED    4

typedef PACK_START struct _basic_stats_t
{
    UINT32  Min;
    UINT32  Max;
    UINT32  Mean;
} PACK_END BASIC_STATS_t;

typedef PACK_START struct _BA_stats_t
{
    UINT32  BarCnt;
    UINT32  BaRetryCnt;
    UINT32  BaPktEnqueued;
    UINT32  BaPktAttempts;
    UINT32  BaPktSuccess;
    UINT32  BaPktFailures;
    UINT32  BaRetryRatio;
} PACK_END BA_STATS_t;

typedef PACK_START struct _SwBA_LfTm_stats_t
{
    UINT32  SBLT_ExpiredCnt;
    UINT32  SBLT_Retry[63];
} PACK_END SWBA_LFTM_STATS_t;

typedef PACK_START struct _SWBA_STATS_t
{
    UINT32  SwBaPktEnqueued;
    UINT32  SwBaPktDone;
    UINT32  SwBaRetryCnt;
    UINT32  SwBaQNotReadyDrop;
    UINT32  SwBaQFullDrop;
    UINT32  SwBaWrongQ;
    UINT32  SwBaDropNonBa;
    UINT32  SwBaWrongQid;
    UINT32  SwBaDropMc;
    UINT32  SwBaFailHwEnQ;
    SWBA_LFTM_STATS_t  *pSBLTS;
} PACK_END SWBA_STATS_t;

typedef PACK_START struct _rate_hist_t
{
    UINT8   addr[HostCmd_SIZE_MAC_ADR];
    UINT16  valid;
    UINT32  LegacyRates[QS_NUM_SUPPORTED_RATES_G];
    UINT32  HtRates[QS_NUM_SUPPORTED_11N_BW][QS_NUM_SUPPORTED_GI][QS_NUM_SUPPORTED_MCS];
    UINT32  VHtRates[QS_NUM_SUPPORTED_11AC_NSS][QS_NUM_SUPPORTED_11AC_BW][QS_NUM_SUPPORTED_GI][QS_NUM_SUPPORTED_11AC_MCS];
} PACK_END RATE_HIST_t;

typedef PACK_START struct _sta_counters_t
{
    UINT8   addr[HostCmd_SIZE_MAC_ADR];
    UINT16  valid;
    UINT32  TxAttempts;
    UINT32  TxSuccesses;
    UINT32  TxRetrySuccesses;
    UINT32  TxMultipleRetrySuccesses;
    UINT32  TxFailures;
} PACK_END STA_COUNTERS_T;

typedef PACK_START struct _rx_sta_counters_t
{
    UINT8   addr[HostCmd_SIZE_MAC_ADR];
    UINT16  valid;
    UINT32  rxPktCounts;
} PACK_END RX_STA_COUNTERS_T;

typedef PACK_START struct _qs_counters_t
{
    UINT32  TCQxAttempts[NUM_OF_TCQ];
    UINT32  TCQxSuccesses[NUM_OF_TCQ];
    UINT32  TCQxRetrySuccesses[NUM_OF_TCQ];
    UINT32  TCQxMultipleRetrySuccesses[NUM_OF_TCQ];
    UINT32  TCQxFailures[NUM_OF_TCQ];
    BASIC_STATS_t   TCQxPktRates[NUM_OF_TCQ];
    BA_STATS_t      BAxStreamStats[NUM_OF_HW_BA];
    STA_COUNTERS_T StaCounters[QS_NUM_STA_SUPPORTED];
    RX_STA_COUNTERS_T rxStaCounters[QS_NUM_STA_SUPPORTED];
    SWBA_STATS_t SwBAStats[QS_NUM_STA_SUPPORTED];
} PACK_END QS_COUNTERS_t;

typedef PACK_START struct _qs_latency_t
{
    BASIC_STATS_t  TCQxTotalLatency[NUM_OF_TCQ];
    BASIC_STATS_t  TCQxFwLatency[NUM_OF_TCQ];
    BASIC_STATS_t  TCQxMacLatency[NUM_OF_TCQ];
    BASIC_STATS_t  TCQxMacHwLatency[NUM_OF_TCQ];
    BASIC_STATS_t  TCQxQSize[NUM_OF_TCQ];
	BASIC_STATS_t  RxFWLatency;
} PACK_END QS_LATENCY_t;

typedef PACK_START struct _qs_retry_hist_t
{
    UINT32  TotalPkts[NUM_OF_TCQ];
    UINT32  *TxPktRetryHistogram[NUM_OF_TCQ];
} PACK_END QS_RETRY_HIST_t;

typedef PACK_START struct _qs_rate_hist_t
{
    UINT32      duration;
    RATE_HIST_t RateHistoram;
} PACK_END QS_RATE_HIST_t;

typedef PACK_START struct _qs_rx_rate_hist_t
{
    UINT32      duration;
    RATE_HIST_t RateHistoram;
} PACK_END QS_RX_RATE_HIST_t;

typedef PACK_START struct _queue_stats_t
{
	PACK_START union
	{
#ifdef QUEUE_STATS_LATENCY
        QS_LATENCY_t    Latency;
#endif        
#ifdef QUEUE_STATS_CNT_HIST
        QS_COUNTERS_t   Counters;
        QS_RETRY_HIST_t RetryHist;
        QS_RATE_HIST_t  RateHist;
        QS_RX_RATE_HIST_t RxRateHist;
#endif        
	} qs_u;
} PACK_END QUEUE_STATS_t;

typedef PACK_START struct _HostCmd_GET_QUEUE_STATS {
	FWCmdHdr        CmdHdr;
    QUEUE_STATS_t   QueueStats;
} PACK_END HostCmd_GET_QUEUE_STATS, *PHostCmd_GET_QUEUE_STATS;

typedef PACK_START struct _HostCmd_QSTATS_SET_SA {
	FWCmdHdr        CmdHdr;
	UINT16          NumOfAddrs;
    UINT8   		Addr[24];
} PACK_END HostCmd_QSTATS_SET_SA, *PHostCmd_QSTATS_SET_SA;

#endif

//          Define data structure for HostCmd_CMD_802_11_GET_STAT
typedef PACK_START struct _HostCmd_DS_802_11_GET_STAT {
	FWCmdHdr    CmdHdr;
	UINT32		TxRetrySuccesses;
	UINT32		TxMultipleRetrySuccesses;
	UINT32		TxFailures;
	UINT32		RTSSuccesses;
	UINT32		RTSFailures;
	UINT32		AckFailures;
	UINT32		RxDuplicateFrames;
	UINT32		RxFCSErrors; // FCSErrorCount; use same name as stats.h
	UINT32		TxWatchDogTimeouts;
	UINT32  		RxOverflows;				//used
	UINT32  		RxFragErrors;			//used
	UINT32  		RxMemErrors;				//used
	UINT32  		PointerErrors;			//used
	UINT32  		TxUnderflows;			//used
	UINT32  		TxDone;
	UINT32  		TxDoneBufTryPut;
	UINT32  		TxDoneBufPut;
	UINT32  		Wait4TxBuf;				// Put size of requested buffer in here
	UINT32  		TxAttempts;
	UINT32  		TxSuccesses;
	UINT32  		TxFragments;
	UINT32  		TxMulticasts;
	UINT32  		RxNonCtlPkts;
	UINT32  		RxMulticasts;
	UINT32  		RxUndecryptableFrames;
	UINT32  		RxICVErrors;
	UINT32  		RxExcludedFrames;
	UINT32		RxWeakIVCount;
	UINT32		RxUnicasts;
	UINT32		RxBytes;
	UINT32		RxErrors;
	UINT32		RxRTSCount;
	UINT32		TxCTSCount;
#ifdef MRVL_WAPI		
	UINT32		RxWAPIPNErrors;
	UINT32		RxWAPIMICErrors;
	UINT32		RxWAPINoKeyErrors;
	UINT32		TxWAPINoKeyErrors;
#endif

#ifdef ENABLE_DEMO_MODE
	//@sps@ Added for demo mode
	UINT32			RxTotalDataPkts;
#endif // #ifdef ENABLE_DEMO_MODE
} PACK_END HostCmd_DS_802_11_GET_STAT, *PHostCmd_DS_802_11_GET_STAT;


//          Define data structure for HostCmd_CMD_MAC_REG_ACCESS
typedef PACK_START struct _HostCmd_DS_MAC_REG_ACCESS {
	FWCmdHdr    CmdHdr;
	UINT16      Action;
	UINT16      Offset;
	UINT32      Value;
	UINT16      Reserved;
} PACK_END HostCmd_DS_MAC_REG_ACCESS, *PHostCmd_DS_MAC_REG_ACCESS;

//          Define data structure for HostCmd_DS_MEM_ADDR_ACCESS
typedef PACK_START struct _HostCmd_DS_MEM_ADDR_ACCESS {
	FWCmdHdr    CmdHdr;
	UINT32      Address;
	UINT16      Length;
	UINT16      Reserved;
	UINT32      Value[64];
} PACK_END HostCmd_DS_MEM_ADDR_ACCESS, *PHostCmd_DS_MEM_ADDR_ACCESS;

//          Define data structure for HostCmd_CMD_BBP_REG_ACCESS
typedef PACK_START struct _HostCmd_DS_BBP_REG_ACCESS {
	FWCmdHdr    CmdHdr;
	UINT16      Action;
	UINT16      Offset;
	UINT8       Value;
	UINT8       Reserverd[3];
} PACK_END HostCmd_DS_BBP_REG_ACCESS, *PHostCmd_DS_BBP_REG_ACCESS;

//          Define data structure for HostCmd_CMD_RF_REG_ACCESS
typedef PACK_START struct _HostCmd_DS_RF_REG_ACCESS {
	FWCmdHdr    CmdHdr;
	UINT16      Action;
	UINT16      Offset;
	UINT8       Value;
	UINT8       Reserverd[3];
} PACK_END HostCmd_DS_RF_REG_ACCESS, *PHostCmd_DS_RF_REG_ACCESS;


typedef PACK_START struct _HostCmd_DS_802_11_BOOST_MODE {
	FWCmdHdr    CmdHdr;
	UINT8       Action;     // 0->get, 1->set
	UINT8       flag;       // bit 0: 0->unset (Boost mode), 1->set (Non-Boost Mode)
	// bit 1: 0->unset, 1->set (double Boost mode) 
	UINT8       ClientMode; // 0 -> mode 1, 1 -> mode 2
} PACK_END HostCmd_DS_802_11_BOOST_MODE, *PHostCmd_DS_802_11_BOOST_MODE;
#define CLIENT_MODE1 1
#define CLIENT_MODE2 2

//          Define data structure for HostCmd_CMD_802_11_RADIO_CONTROL
typedef PACK_START struct _HostCmd_DS_802_11_RADIO_CONTROL {
	FWCmdHdr    CmdHdr;
	UINT16      Action;                   
	UINT16      Control;	// @bit0: 1/0,on/off, @bit1: 1/0, long/short @bit2: 1/0,auto/fix
	UINT16      RadioOn;
} PACK_END HostCmd_DS_802_11_RADIO_CONTROL, *PHostCmd_DS_802_11_RADIO_CONTROL;

#ifdef SOC_W8864
#define MWL_MAX_TXPOWER_ENTRIES  16
#else
#define MWL_MAX_TXPOWER_ENTRIES  12
#endif

#if defined(SOC_W8864)
#define TX_POWER_LEVEL_TOTAL  16
#else
#define TX_POWER_LEVEL_TOTAL  12
#endif
#define HAL_TXPWR_ID_CCK                       0
#define HAL_TXPWR_ID_OFDM_HI                   1
#define HAL_TXPWR_ID_OFDM_MED                  2
#define HAL_TXPWR_ID_OFDM_LO                   3
#define HAL_TXPWR_ID_2STREAM_HT20_HI           4
#define HAL_TXPWR_ID_2STREAM_HT20_MED          5
#define HAL_TXPWR_ID_2STREAM_HT20_LO           6
#define HAL_TXPWR_ID_3STREAM_HT20              7
#define HAL_TXPWR_ID_2STREAM_HT40_HI           8
#define HAL_TXPWR_ID_2STREAM_HT40_MED          9
#define HAL_TXPWR_ID_2STREAM_HT40_LO           10
#define HAL_TXPWR_ID_3STREAM_HT40              11
#if defined(SOC_W8864)
#define HAL_TRPC_ID_2STREAM_HT80_HI            12
#define HAL_TRPC_ID_2STREAM_HT80_MED           13
#define HAL_TRPC_ID_2STREAM_HT80_LO            14
#define HAL_TRPC_ID_3STREAM_HT80               15
#endif

//          Define data structure for HostCmd_CMD_802_11_RF_TX_POWER
typedef PACK_START struct _HostCmd_DS_802_11_RF_TX_POWER {
	FWCmdHdr    CmdHdr;
	UINT16      Action;
	UINT16      SupportTxPowerLevel;     
	UINT16      CurrentTxPowerLevel;     
	UINT16      Reserved;
	UINT16      PowerLevelList[TX_POWER_LEVEL_TOTAL];
} PACK_END HostCmd_DS_802_11_RF_TX_POWER, *PHostCmd_DS_802_11_RF_TX_POWER;
//          Define data structure for HostCmd_CMD_802_11_TX_POWER
typedef struct {
	FWCmdHdr    CmdHdr;
	UINT16      Action;
	UINT16      band;
	UINT16      ch;
	UINT16      bw;
	UINT16      sub_ch;
	UINT16      PowerLevelList[TX_POWER_LEVEL_TOTAL];
} PACK_END HostCmd_DS_802_11_TX_POWER;
//          Define data structure for HostCmd_CMD_802_11_RF_ANTENNA
typedef PACK_START struct _HostCmd_DS_802_11_RF_ANTENNA {
	FWCmdHdr    CmdHdr;
	UINT16      Action;
	UINT16      AntennaMode;             // Number of antennas or 0xffff(diversity)
} PACK_END HostCmd_DS_802_11_RF_ANTENNA, *PHostCmd_DS_802_11_RF_ANTENNA;

//          Define data structure for HostCmd_CMD_802_11_PS_MODE
typedef PACK_START struct _HostCmd_DS_802_11_PS_MODE {
	FWCmdHdr    CmdHdr;
	UINT16      Action;
	UINT16      PowerMode;               // CAM, Max.PSP or Fast PSP
} PACK_END HostCmd_DS_802_11_PS_MODE, *PHostCmd_DS_802_11_PS_MODE;



typedef PACK_START struct _HostCmd_DS_802_11_RTS_THSD {
	FWCmdHdr		CmdHdr;
	UINT16		Action;
	UINT16		Threshold;
} PACK_END HostCmd_DS_802_11_RTS_THSD, *PHostCmd_DS_802_11_RTS_THSD;

// used for stand alone bssid sets/clears
typedef PACK_START struct tagHostCmd_FW_SET_MAC
{
	FWCmdHdr    CmdHdr;
	UINT16      MacType;
	UINT8       MacAddr[6];
} PACK_END HostCmd_DS_SET_MAC, *PHostCmd_DS_SET_MAC,
HostCmd_FW_SET_BSSID, *PHostCmd_FW_SET_BSSID,
HostCmd_FW_SET_MAC, *PHostCmd_FW_SET_MAC;

// Indicate to FW to send out PS Poll
typedef struct tagHostCmd_FW_TX_POLL
{
	FWCmdHdr    CmdHdr;
	UINT32      PSPoll;
} HostCmd_FW_TX_POLL, *PHostCmd_FW_TX_POLL;

// this struct is sent to the firmware for both the start and join
// mlme functions. FW to use these elements to config
typedef PACK_START struct tagHostCmd_FW_SET_BCN_CMD
{
	FWCmdHdr    CmdHdr;
	UINT32      CfOffset;
	UINT32      TimOffset;
	CAPABILITY_FIELD   Caps;
	UINT32      ProbeRspLen;
	UINT16      BcnPeriod;
	UINT16      CF_CfpMaxDuration;
	UINT16      IBSS_AtimWindow;
	UINT32      StartIbss;      // TRUE=start ibss, FALSE=join ibss
	UINT8       BssId[6];
	UINT8       BcnTime[8];
	UINT8       SsIdLength;
	UINT8       SsId[32];
	UINT8       SupportedRates[32]; 
	UINT8       DtimPeriod;
	UINT8       ParamBitMap;    // indicate use of IBSS or CF parameters
	UINT8       CF_CfpCount;
	UINT8       CF_CfpPeriod;
	UINT8       RfChannel;
	UINT8       AccInterval[8];
	UINT8       TsfTime[8];
	UINT8       BeaconFrameLength;
	UINT8       BeaconBuffer[128];
	UINT32      GProtection;
#ifdef SEND_PEER_RATES
	UINT8       PeerRates[ RATE_INDEX_MAX_ARRAY];
#endif
#ifdef DOT_11A_SUPPORT
	UINT8	RFType;		//bit0: 0 = 2.4 GHz,
	//      1 = 5.0 GHz
	//bit1: 0 = 20 MHz Channel spacing
	//      1 = 10 MHz Channel spacing

#endif

} PACK_END HostCmd_FW_SET_BCN_CMD, *PHostCmd_FW_SET_BCN_CMD;

// used for AID sets/clears
typedef PACK_START struct tagHostCmd_FW_SET_AID
{
	FWCmdHdr    CmdHdr;
	UINT16      AssocID;
	UINT8       MacAddr[6]; //AP's Mac Address(BSSID)
	UINT32      GProtection;
	UINT8       ApRates[ RATE_INDEX_MAX_ARRAY];
} PACK_END HostCmd_FW_SET_AID, *PHostCmd_FW_SET_AID;

typedef PACK_START struct tagHostCmd_FW_SET_NEW_STN
{
	FWCmdHdr    CmdHdr;
	UINT16      AID;
	UINT8       MacAddr[6]; 
	UINT16      StnId;
	UINT16      Action;
	UINT16      Reserved;
	PeerInfo_t  PeerInfo;
#ifdef UAPSD_SUPPORT
	UINT8  Qosinfo;
	UINT8  isQosSta;
#endif
	UINT32      FwStaPtr;
} PACK_END HostCmd_FW_SET_NEW_STN, *PHostCmd_FW_SET_NEW_STN;

typedef PACK_START struct tagHostCmd_FW_SET_KEEP_ALIVE_TICK
{
	FWCmdHdr    CmdHdr;
	UINT8           tick;

} PACK_END HostCmd_FW_SET_KEEP_ALIVE_TICK, *PHostCmd_FW_SET_KEEP_ALIVE_TICK;

typedef PACK_START struct tagHostCmd_FW_SET_RIFS
{
	FWCmdHdr    CmdHdr;
	UINT8           QNum;
} PACK_END HostCmd_FW_SET_RIFS, *PHostCmd_FW_SET_RIFS;


typedef PACK_START struct tagHostCmd_FW_SET_APMODE
{
	FWCmdHdr    CmdHdr;
	UINT8 ApMode;

} PACK_END HostCmd_FW_SET_APMODE, *PHostCmd_FW_SET_APMODE;


#ifndef SUPERFLY
typedef  struct tagHostCmd_FW_SET_RATE
{
	FWCmdHdr    CmdHdr;
	UINT8       DataRateType;   // 0=Auto, Rate Adaption ON, 1=Fixed, No rate adaption
	UINT8       RateIndex;     // Used for fixed rate - if fixed, then fill the index
	UINT8       ApRates[ RATE_INDEX_MAX_ARRAY];
} PACK_END HostCmd_FW_SET_RATE, *PHostCmd_FW_SET_RATE;
#endif

/*
typedef PACK_START struct tagHostCmd_FW_SET_CWMinMax { 
FWCmdHdr    CmdHdr;
UINT16      Action;   //0 = get all, 0x1 =set CWMin/Max,  0x2 = set TXOP , 0x4 =set AIFSN
UINT16      TxOP;     // in unit of 32 us
UINT8	      CWMax;    // 0~15
UINT8	      CWMin;	 // 0~15
UINT8	      AIFSN;  
UINT8       TxQNum;   // Tx Queue number.
} PACK_END HostCmd_FW_SET_CWMinMax, *PHostCmd_FW_SET_CWMinMax;
*/

typedef PACK_START struct tagHostCmd_FW_GET_Parent_TSF {
	FWCmdHdr    CmdHdr;
	UINT16      Action;	   // 0 = get only	
	UINT32      ParentTSF;  // lower 4 byte of serving AP's TSF value at time measuring
	//    STA recv beacon or probe response 
} PACK_END HostCmd_FW_GET_Parent_TSF, *PHostCmd_FW_GET_Parent_TSF;

typedef PACK_START struct tagHostCmd_CCA_Busy_Fract {
	FWCmdHdr    CmdHdr;
	UINT16      Action;	  	   // 0 = stop, 1 = start 
	UINT16      Reserved;
	UINT32      CCABusyFrac;   // fraction duration over which CCA indicated channel is busy 
} PACK_END HostCmd_FW_GET_CCA_Busy_Fract, *PHostCmd_FW_GET_CCA_Busy_Fract;

typedef PACK_START struct tagHostCmd_FW_GET_RPI_Density {
	FWCmdHdr    CmdHdr;
	UINT16      Action;	  	   // 0 = stop, 1 = start	
	UINT16      DiffChannel;   // 0 = same channel, 1 = diff channel
	UINT32      RPI0Density;   // power >= -87
	UINT32      RPI1Density;   // -82<= power < -87
	UINT32      RPI2Density;   // -77<= power < -82
	UINT32      RPI3Density;   // -72<= power < -77
	UINT32      RPI4Density;   // -67<= power < -72
	UINT32      RPI5Density;   // -62<= power < -67
	UINT32      RPI6Density;   // -57<= power < -62
	UINT32      RPI7Density;   // power < -57

} PACK_END HostCmd_FW_GET_RPI_Density, *PHostCmd_FW_GET_RPI_Density;

typedef PACK_START struct tagHostCmd_FW_GET_NOISE_Level {
	FWCmdHdr    CmdHdr;
	UINT16      Action;	  	   // 0 = get only
	UINT8       Noise;         //

} PACK_END HostCmd_FW_GET_NOISE_Level, *PHostCmd_FW_GET_NOISE_Level;

//          Define LED control command data structure
#ifdef LED_CONTROL

typedef PACK_START struct tagLEDPattern {
	UINT8       ucReserved;
	UINT8       ucPattern;
	UINT8       ucLEDIndex;
	UINT8       usState;
} PACK_END LEDPattern, *PLEDPattern;

typedef PACK_START struct tagHostCmd_DS_LED_SET_INFORMATION {
	FWCmdHdr    CmdHdr;
	// FW command header includes:
	//      UINT16 Command;
	//      UINT16 Size;
	//      UINT16 SeqNum;
	//      UINT16 Result;
	UINT32 LEDInfoBuf[62];
} PACK_END HostCmd_DS_LED_SET_INFORMATION, *PHostCmd_DS_LED_SET_INFORMATION;

typedef PACK_START struct _HostCmd_Led_Pattern {
	FWCmdHdr   CmdHdr;
	UINT8      Reserved;
	UINT8      LedPattern;
	UINT8      LedIndex;
	UINT8      LedState;
} PACK_END HostCmd_Led_Pattern, *PHostCmd_Led_Pattern;
#define HOSTCMD_LED_AUTO_DEFAULT 0
#define HOSTCMD_LED_CTRL_BY_HOST 1


typedef PACK_START struct tagHostCmd_DS_LED_GET_STATE {
	FWCmdHdr    CmdHdr;
	// FW command header includes:
	//      UINT16 Command;
	//      UINT16 Size;
	//      UINT16 SeqNum;
	//      UINT16 Result;
	UINT32 LEDState;
} PACK_END HostCmd_DS_LED_GET_STATE, *PHostCmd_DS_LED_GET_STATE;

typedef PACK_START struct tagHostCmd_DS_LED_SET_STATE {
	FWCmdHdr    CmdHdr;
	// FW command header includes:
	//      UINT16 Command;
	//      UINT16 Size;
	//      UINT16 SeqNum;
	//      UINT16 Result;
	UINT32 LEDState;
} PACK_END HostCmd_DS_LED_SET_STATE, *PHostCmd_DS_LED_SET_STATE;
#endif

#ifdef IEEE80211_DH
typedef PACK_START struct _HostCmd_802_11h_Detect_Radar {
	FWCmdHdr	CmdHdr;
	UINT16 Action;			// see following
	UINT16 RadarTypeCode;
    UINT16 MinChirpCount;
    UINT16 ChirpTimeIntvl;
    UINT16 PwFilter;
    UINT16 MinNumRadar;
    UINT16 PriMinNum;
} PACK_END HostCmd_802_11h_Detect_Radar, *PHostCmd_802_11h_Detect_Radar;

#define DR_DFS_DISABLE						0
#define DR_CHK_CHANNEL_AVAILABLE_START		1
#define DR_CHK_CHANNEL_AVAILABLE_STOP		2
#define DR_IN_SERVICE_MONITOR_START			3

#define HostCmd_80211H_RADAR_TYPE_CODE_ETSI_151     151

typedef PACK_START struct _HostCmd_STOP_Beacon {
	UINT16 Command;			// HostCmd_CMD_STOP_BEACON
	UINT16 Size;
	UINT16 SeqNum;
	UINT16 Result;
} PACK_END HostCmd_STOP_Beacon, *PHostCmd_STOP_Beacon;
#endif //IEEE80211_DH

//SoftAP command structure
#if defined(AP_BCN2) || defined(MRVL_SOFTAP) || defined(AP_MAC_LINUX)	//Defined for the AP CMD 8/9/2003 by Edden Tsai
typedef PACK_START struct tagBSS_START	//Defined for the BSS Start 15/9/2003
{
	FWCmdHdr    CmdHdr;
	UINT32      Enable;		//0 -- Disbale. or 1 -- Enable.
} PACK_END HostCmd_BSS_START, *PHostCmd_BSS_START;

//New Add this Structure 17/9/2003
typedef	PACK_START struct tagHostCmd_AP_Beacon
{
	FWCmdHdr    CmdHdr;
	IEEEtypes_StartCmd_t       StartCmd; 
} PACK_END HostCmd_AP_Beacon, *PHostCmd_AP_Beacon;

//New Structure for Update Tim 30/9/2003

typedef	PACK_START struct	tagHost_Cmd_Update_TIM
{
	UINT16	   Aid;
	UINT32      Set;
} PACK_END HostCmd_Update_TIM, *PHostCmd_Update_TIM;

typedef	PACK_START struct	tagHost_CMD_UpdateTIM
{
	FWCmdHdr    CmdHdr;
	HostCmd_Update_TIM 	UpdateTIM;
} PACK_END HostCmd_UpdateTIM, *PHostCmd_UpdateTIM;

typedef struct tagHostCmd_SSID_BROADCAST
{
	FWCmdHdr	CmdHdr;
	UINT32      SsidBroadcastEnable;
} HostCmd_SSID_BROADCAST, *PHostCmd_SSID_BROADCAST;

typedef struct tagHostCmd_WDS
{
	FWCmdHdr	CmdHdr;
	UINT32      WdsEnable;
} HostCmd_WDS, *PHostCmd_WDS;


#ifdef	BURST_MODE	//Add the Burst Mode 2004/02/04
typedef PACK_START struct tagBURST_MODE	//Defined for the BSS Start 15/9/2003
{
	FWCmdHdr    CmdHdr;
	UINT32      Enable;		//0 -- Disbale. or 1 -- Enable.
} PACK_END HostCmd_BURST_MODE, *PHostCmd_BURST_MODE;
#endif //#ifdef	BURST_MODE

#ifdef WOW
typedef PACK_START struct _HostCmd_AP_STANDBY {
	FWCmdHdr    CmdHdr;
	UINT16      Action;	// TRUE -- Enable WOW
	UINT16      NumOfAdrs;
	UINT8       MACList[HostCmd_SIZE_MAC_ADR*WOW_MAX_STATION];
}PACK_END HostCmd_AP_STANDBY, *PHostCmd_AP_STANDBY;
#endif

#endif //#if defined(AP_BCN2) || defined(MRVL_SOFTAP)


#ifdef IEEE80211_DH
typedef  struct _DomainChannelEntry
{
	UINT8 FirstChannelNo;
	UINT8 NoofChannel;
	UINT8 MaxTransmitPw;
}
PACK_END DomainChannelEntry;

typedef  PACK_START struct _DomainCountryInfo
{
	UINT8 CountryString[3];
	UINT8 GChannelLen;
	DomainChannelEntry DomainEntryG[1]; /** Assume only 1 G zone **/
	UINT8 AChannelLen;
	DomainChannelEntry DomainEntryA[31]; /** Assume max of 5 A zone **/
} PACK_END DomainCountryInfo ;

typedef PACK_START struct _HostCmd_SET_SWITCH_CHANNEL {
	FWCmdHdr    CmdHdr;
	UINT32    Next11hChannel;
	UINT32    Mode;
	UINT32    InitialCount;
	CHNL_FLAGS_11AC ChannelFlags;		
	UINT32	  NextHTExtChnlOffset;		/*HT Ext Channel offset	*/		
	UINT32    dfs_test_mode;  			/* DFS test bypasses channel switch on CSA countdown. */
} PACK_END HostCmd_SET_SWITCH_CHANNEL, *PHostCmd_SET_SWITCH_CHANNEL;

typedef PACK_START struct _HostCmd_SET_SPECTRUM_MGMT {
	FWCmdHdr    CmdHdr;
	UINT32    	SpectrumMgmt;
} PACK_END HostCmd_SET_SPECTRUM_MGMT, *PHostCmd_SET_SPECTRUM_MGMT;

typedef PACK_START struct _HostCmd_SET_POWER_CONSTRAINT {
	FWCmdHdr    CmdHdr;
	SINT32    	PowerConstraint;
} PACK_END HostCmd_SET_POWER_CONSTRAINT, *PHostCmd_SET_POWER_CONSTRAINT;

typedef PACK_START struct _HostCmd_SET_COUNTRY_INFO {
	FWCmdHdr    CmdHdr;
	UINT32		Action ; // 0 -> unset, 1 ->set
	DomainCountryInfo DomainInfo ;
} PACK_END HostCmd_SET_COUNTRY_INFO, *PHostCmd_SET_COUNTRY_INFO;
#endif //IEEE80211_DH

typedef PACK_START struct _HostCmd_SET_REGIONCODE_INFO {
	FWCmdHdr    CmdHdr;
	UINT16      regionCode ; 
} PACK_END HostCmd_SET_REGIONCODE_INFO, *PHostCmd_SET_REGIONCODE_INFO;

#ifdef MRVL_WSC
#define WSC_BEACON_IE           0
#define WSC_PROBE_RESP_IE       1
typedef PACK_START struct _HostCmd_SET_WSC_IE {
	FWCmdHdr    CmdHdr;
	UINT16      ieType ;
	WSC_COMB_IE_t wscIE ;
} PACK_END HostCmd_SET_WSC_IE, *PHostCmd_SET_WSC_IE;
#endif

#ifdef MRVL_WAPI
#define WAPI_BEACON_IE           0
#define WAPI_PROBE_RESP_IE       1
typedef PACK_START struct _HostCmd_SET_WAPI_IE {
	FWCmdHdr    CmdHdr;
	UINT16      ieType ;
	WAPI_COMB_IE_t WAPIIE ;
} PACK_END HostCmd_SET_WAPI_IE, *PHostCmd_SET_WAPI_IE;
#endif

// for HostCmd_CMD_SET_WMM_MODE
typedef struct tagHostCmd_FW_SetWMMMode {
	FWCmdHdr    CmdHdr;
	UINT16      Action;  // 0->unset, 1->set
} HostCmd_FW_SetWMMMode, *PHostCmd_FW_SetWMMMode;

typedef struct tagHostCmd_FW_SetIEs {
	FWCmdHdr    CmdHdr;
	UINT16      Action;  // 0->unset, 1->set
	UINT16      IeListLenHT;
	UINT16      IeListLenVHT;
	UINT16      IeListLenProprietary;
	/*Buffer size same as Generic_Beacon*/
	UINT8       IeListHT[148];			
	UINT8       IeListVHT[24];
	UINT8       IeListProprietary[112];
} HostCmd_FW_SetIEs, *PHostCmd_FW_SetIEs;


#if 1//def IGX_QOS
#define EDCA_PARAM_SIZE				18
#define BA_PARAM_SIZE				2

typedef  struct tagHostCmd_FW_SET_EDCA_PARAMS
{
	FWCmdHdr    CmdHdr;
	UINT16		Action;   //0 = get all, 0x1 =set CWMin/Max,  0x2 = set TXOP , 0x4 =set AIFSN
	UINT16		TxOP;     // in unit of 32 us
	UINT32		CWMax;    // 0~15
	UINT32		CWMin;    // 0~15
	UINT8		AIFSN;
	UINT8		TxQNum;   // Tx Queue number.
} HostCmd_FW_SET_EDCA_PARAMS, *PHostCmd_FW_SET_EDCA_PARAMS;

typedef PACK_START struct tagHostCmd_FW_SET_MEDIUM_TIME
{
	FWCmdHdr    CmdHdr;
	UINT16   UserPriority;   // User Priority to set
	UINT16   MediumTime;     // in unit of 32 us
}PACK_END HostCmd_FW_SET_MEDIUM_TIME, *PHostCmd_FW_SET_MEDIUM_TIME;

typedef PACK_START struct tagHostCmd_FW_SET_BA_PARAMS
{
	FWCmdHdr     CmdHdr;
	UINT8        BaAction;
	UINT8        Reserved;
	UINT8        BAparams[BA_PARAM_SIZE];
}PACK_END HostCmd_FW_SET_BA_PARAMS, *PHostCmd_FW_SET_BA_PARAMS;

typedef PACK_START struct tagHostCmd_FW_SET_HCCA
{
	FWCmdHdr  CmdHdr;
	UINT32    ulQoSMode;
	UINT8     CFPollable;
	UINT8     CFPollrequest;
	UINT8     APSD;
	UINT8     QueueRequest;
	UINT8     TxOpRequest;
} PACK_END HostCmd_FW_SET_HCCA, *PHostCmd_FW_SET_HCCA;

typedef PACK_START struct tagHostCmd_FW_SET_CFP
{
	FWCmdHdr    CmdHdr;
	UINT8    CFPCount;
	UINT8    CFPPeriod;
	UINT16   CFPMaxDuration;
	UINT16   CFPDurRemaining;
	UINT32   DTIMPeriod;
} PACK_END HostCmd_FW_SET_CFP, *PHostCmd_FW_SET_CFP;

#endif //IGX_QOS

#ifdef POWERSAVE_OFFLOAD
/** This cmmand will be send by driver to f/w whenever each bss has powersave station cnt going from
0> 1 and also when PS number goes to 0, f/w (as per normal) before queuing MC/BC traffic will check this
count before deciding whether to queue to the respective depth in the MC/BC queue **/
typedef PACK_START struct tagHostCmd_SET_POWERSAVESTATION
{
	FWCmdHdr    CmdHdr;
	UINT8           NumberofPowersave; /** No of active powersave station **/
	UINT8           reserved;
} PACK_END HostCmd_SET_POWERSAVESTATION, *PHostCmd_SET_POWERSAVESTATION;
/** this command will be send by the driver to f/w whenever driver detect that a station is going to
powersave and there is packet pending for the station.  This command is also use with the GET TIM command
to reset the TIM when there is no packet for the station or the station has gone out of powersave.  F/W will update
the tim on the next tbtt **/
typedef PACK_START struct tagHostCmd_SET_TIM
{
	FWCmdHdr CmdHdr;
	UINT16 Aid;
	UINT32 Set;  
	UINT8 reserved;
} PACK_END HostCmd_SET_TIM,*PHostCmd_SET_TIM;
/** this command will return the TIM buffer of the respective BSS to the driver **/
typedef PACK_START struct taghost_CMD_GET_TIM
{
	FWCmdHdr CmdHdr;
	UINT8 TrafficMap[251];
	UINT8 reserved;
} PACK_END HostCmd_GET_TIM, *PHostCmd_GET_TIM;
#endif

/******************************************************************************
@HWENCR@
Hardware Encryption related data structures and constant definitions.
Note that all related changes are marked with the @HWENCR@ tag.
*******************************************************************************/

#define MAX_ENCR_KEY_LENGTH						16                  /* max 128 bits - depends on type */
#define MIC_KEY_LENGTH							8					/* size of Tx/Rx MIC key - 8 bytes*/

#define KEY_TYPE_ID_WEP							0x00				/* Key type is WEP		*/
#define KEY_TYPE_ID_TKIP						0x01				/* Key type is TKIP		*/
#define KEY_TYPE_ID_AES							0x02				/* Key type is AES-CCMP	*/
#ifdef MRVL_WAPI
#define KEY_TYPE_ID_WAPI 						0x03				/* Key type is WAPI	*/
#endif

/* flags used in structure - same as driver EKF_XXX flags */
#define ENCR_KEY_FLAG_INUSE						0x00000001          /* indicate key is in use */
#define ENCR_KEY_FLAG_RXGROUPKEY				0x00000002          /* Group key for RX only */
#define ENCR_KEY_FLAG_TXGROUPKEY				0x00000004          /* Group key for TX */
#define ENCR_KEY_FLAG_PAIRWISE					0x00000008          /* pairwise */
#define ENCR_KEY_FLAG_RXONLY					0x00000010          /* only used for RX */
// These flags are new additions - for hardware encryption commands only.
#define ENCR_KEY_FLAG_AUTHENTICATOR				0x00000020			/* Key is for Authenticator */
#define ENCR_KEY_FLAG_TSC_VALID					0x00000040			/* Sequence counters are valid */
#define ENCR_KEY_FLAG_WEP_TXKEY					0x01000000			/* Tx key for WEP */
#define ENCR_KEY_FLAG_MICKEY_VALID				0x02000000			/* Tx/Rx MIC keys are valid */

typedef enum tagENCR_TYPE
{
	EncrTypeWep = 0,
	EncrTypeDisable = 1,
	EncrTypeTkip = 4,
	EncrTypeAes = 6,
	EncrTypeMix = 7,
}ENCR_TYPE;

/*
UPDATE_ENCRYPTION command action type.
*/
typedef enum tagENCR_ACTION_TYPE
{
	// request to enable/disable HW encryption
	EncrActionEnableHWEncryption,
	// request to set encryption key
	EncrActionTypeSetKey,
	// request to remove one or more keys
	EncrActionTypeRemoveKey,
	EncrActionTypeSetGroupKey,
}ENCR_ACTION_TYPE;

/*
Key material definitions (for WEP, TKIP, & AES-CCMP)
*/

/* 
WEP Key material definition
----------------------------
WEPKey	--> An array of 'MAX_ENCR_KEY_LENGTH' bytes.
Note that we do not support 152bit WEP keys
*/
typedef PACK_START struct _WEP_TYPE_KEY
{
	// WEP key material (max 128bit)
	UINT8   KeyMaterial[ MAX_ENCR_KEY_LENGTH ];
} PACK_END WEP_TYPE_KEY, *PWEP_TYPE_KEY;

/*
TKIP Key material definition
----------------------------
This structure defines TKIP key material. Note that
the TxMicKey and RxMicKey may or may not be valid.
*/
/* TKIP Sequence counter - 24 bits */
/* Incremented on each fragment MPDU */
typedef PACK_START struct tagENCR_TKIPSEQCNT
{
	UINT16 low;
	UINT32 high;
} PACK_END ENCR_TKIPSEQCNT, *PENCR_TKIPSEQCNT;

typedef PACK_START struct _TKIP_TYPE_KEY
{
	// TKIP Key material. Key type (group or pairwise key) is determined by flags 
	// in KEY_PARAM_SET structure.
	UINT8			KeyMaterial[ MAX_ENCR_KEY_LENGTH ];
	// MIC keys
	UINT8			TkipTxMicKey[ MIC_KEY_LENGTH ];
	UINT8			TkipRxMicKey[ MIC_KEY_LENGTH ];
	ENCR_TKIPSEQCNT	TkipRsc;
	ENCR_TKIPSEQCNT	TkipTsc;
} PACK_END TKIP_TYPE_KEY, *PTKIP_TYPE_KEY;

/*
AES-CCMP Key material definition
--------------------------------
This structure defines AES-CCMP key material.
*/
typedef PACK_START struct _AES_TYPE_KEY
{
	// AES Key material
	UINT8   KeyMaterial[ MAX_ENCR_KEY_LENGTH ];
} PACK_END AES_TYPE_KEY, *PAES_TYPE_KEY;

#ifdef MRVL_WAPI 
typedef PACK_START struct _WAPI_TYPE_KEY
{
	UINT8   KeyMaterial[ MAX_ENCR_KEY_LENGTH ];
	UINT8   MicKeyMaterial[ MAX_ENCR_KEY_LENGTH ];
} PACK_END WAPI_TYPE_KEY, *PWAPI_TYPE_KEY;
#endif
/*
Encryption key definition.
--------------------------
This structure provides all required/essential
information about the key being set/removed.
*/
typedef PACK_START struct _KEY_PARAM_SET
{
	// Total length of this structure (Key is variable size array)
	UINT16  Length;
	// Key type - WEP, TKIP or AES-CCMP.
	// See definitions above
	UINT16  KeyTypeId;
	// key flags (ENCR_KEY_FLAG_XXX_
	UINT32  KeyInfo;
	// For WEP only - actual key index
	UINT32  KeyIndex;  
	// Size of the key
	UINT16  KeyLen;
	// Key material (variable size array)
#ifdef MRVL_WAPI
	UINT16  Reserved;
#endif
	PACK_START union{
		WEP_TYPE_KEY	WepKey;
		TKIP_TYPE_KEY	TkipKey;
		AES_TYPE_KEY	AesKey;
#ifdef MRVL_WAPI
		WAPI_TYPE_KEY	WapiKey;
#endif
	}PACK_END Key;
	UINT8		Macaddr[6];
} PACK_END KEY_PARAM_SET, *PKEY_PARAM_SET;

/*
HostCmd_FW_UPDATE_ENCRYPTION
----------------------------
Define data structure for updating firmware encryption keys.

*/
typedef PACK_START struct tagHostCmd_FW_ENCRYPTION
{
	// standard command header
	FWCmdHdr    CmdHdr;
	// Action type - see ENCR_ACTION_TYPE
	UINT32		ActionType;						// ENCR_ACTION_TYPE
	// size of the data buffer attached.
	UINT32		DataLength;
	// data buffer - maps to one KEY_PARAM_SET structure
	//KEY_PARAM_SET Key;
	UINT8		macaddr[6];
	UINT8		ActionData[1];
} PACK_END HostCmd_FW_UPDATE_ENCRYPTION, *PHostCmd_FW_UPDATE_ENCRYPTION;


typedef PACK_START struct tagHostCmd_FW_ENCRYPTION_SET_KEY
{
	// standard command header
	FWCmdHdr    CmdHdr;
	// Action type - see ENCR_ACTION_TYPE
	UINT32		ActionType;						// ENCR_ACTION_TYPE
	// size of the data buffer attached.
	UINT32		DataLength;
	// data buffer - maps to one KEY_PARAM_SET structure
	KEY_PARAM_SET KeyParam;
} PACK_END HostCmd_FW_UPDATE_ENCRYPTION_SET_KEY, *PHostCmd_FW_UPDATE_ENCRYPTION_SET_KEY;


#ifdef SUPERFLY
//Superfly rate Info structure
#define LEGACY_FORMAT        0
#define HT_FORMAT            1

#define SHORT_GI             1
#define STANDARD_GI          0

#define BW_20MHZ             0
#define BW_40MHZ             1

#define NO_ADV_CODING         0
#define LDPC_ADV_CODING       1
#define RS_ADV_CODING         2
#define RESV_ADV_CODING       3

#define ANT_SELECT_A	        1
#define ANT_SELECT_B	        2
#define ANT_SELECT_2BY2	    3
#define ANT_SELECT_2BY3	    0

#define LOWER_ACT_SUBCH       0x00
#define UPPER_ACT_SUBCH       0x01
#define BOTH_SUBCH     		0x02

#define HC_LONG_PREAMBLE        0
#define HC_SHORT_PREAMBLE       1

#define		ENABLE_TEST_RATE   	1
#define		DISABLE_TEST_RATE		0
#define  	AUTO_RATE_DROP_TABLE_SIZE	4

typedef	 struct RateInfo_t
{
	UINT16	Format:		1	;	//0 = Legacy format, 1 = Hi-throughput format
	UINT16	ShortGI:	1	;	//0 = Use standard guard interval,1 = Use short guard interval
	UINT16	Bandwidth:	1	;	//0 = Use 20 MHz channel,1 = Use 40 MHz channel
	UINT16	RateIDMCS:	6	;	//= RateID[3:0]; Legacy format,= MCS[5:0]; HT format
	UINT16	AdvCoding:	2	;	//AdvCoding 0 = No AdvCoding,1 = LDPC,2 = RS,3 = Reserved
	UINT16	AntSelect:	2	;	//Bitmap to select one of the transmit antennae
	UINT16	ActSubChan:	2;   //Active subchannel for 40 MHz mode 00:lower, 01= upper, 10= both on lower and upper
	UINT16  Preambletype:1;  //Preambletype 0= Long, 1= Short;

}PACK_END RateInfo_t1;

//Superfly rate change structure
typedef  struct Rate_Change_t
{
	RateInfo_t1	RateInfo;//Superfly rate info 

	UINT16      Reserved1:  8;

	UINT16      Count:      4;

	UINT16      Reserved2:  3;

	UINT16      DropFrame:  1;
} PACK_END Rate_Change_t1;

typedef  struct tagRATE_TABLE_INFO
{	 
	UINT32 		EnableTestRate;
	UINT32   	        AllowRateDrop;  // use fixed rate specified but firmware can drop to 
	UINT32				TotalRetryCount;
	Rate_Change_t1	FixedAutoRateDropTable[AUTO_RATE_DROP_TABLE_SIZE];

} PACK_END RATE_TABLE_INFO_t, *PRATE_TABLE_INFO_t;


typedef   struct  tagHostCmd_FW_SET_TEST_RATE_TABLE
{
	FWCmdHdr 			CmdHdr;
	UINT32              Action;	//HostCmd_ACT_GEN_GET						0x0000
	//HostCmd_ACT_GEN_SET 						0x0001 						    		
	RATE_TABLE_INFO_t   RateTableInfo;
}  PACK_END HostCmd_FW_SET_TEST_RATE_TABLE, *PHostCmd_FW_SET_TEST_RATE_TABLE;

#endif


/******************************************************************************
@STADB@
Station information database related data structures and constant definitions.
Note that all related changes are marked with the @STADB@ tag.
*******************************************************************************/

//
// Reason codes - defines the reason why an entry is added/deleted or updated.
//				  May be useful for debugging purposes?
#define STABD_REASON_NEW_ENTRY			0x00000001
#define STABD_REASON_CONNECTION_TORN	0x00000002
#define STABD_REASON_PEER_INACTIVE		0x00000003
#define STABD_REASON_PEER_ACTIVE		0x00000004
#define STABD_REASON_PEER_CONFIG_CHANGE	0x00000005

// flags for the legacy rates.
#define RATE_INFO_FLAG_BASIC_RATE		BIT(1)
#define RATE_INFO_FLAG_OFDM_RATE		BIT(2)
typedef PACK_START struct tagRateInfo
{
	// Rate flags - see above.
	UINT32		Flags;
	// Rate in 500Kbps units.
	UINT8		RateKbps;
	// 802.11 rate to conversion table index value.
	// This is the value required by the firmware/hardware.
	UINT16		RateCodeToIndex;
}PACK_END RATE_INFO, *PRATE_INFO;

/*
UPDATE_STADB command action type.
*/
typedef  enum tagSTADB_ACTION_TYPE
{
	// request to add entry to stainfo db
	StaInfoDbActionAddEntry,
	// request to modify peer entry
	StaInfoDbActionModifyEntry,
	// request to remove peer from stainfo db
	StaInfoDbActionRemoveEntry
}PACK_END STADB_ACTION_TYPE;

// Peer Entry flags - used to define the charasticts of the peer node.
#define PEER_TYPE_ACCESSPOINT			BIT(1)
#define PEER_TYPE_ADHOC_STATION			BIT(2)

#define PEER_CAPABILITY_HT_CAPABLE_EWC	BIT(1)

/**
* PEER_CAPABILITY_INFO - Datastructure to store peer capability information.
*/

typedef PACK_START struct tagHostCmd_FW_SET_LOOPBACK_MODE
{
	FWCmdHdr	CmdHdr;
	UINT8		Enable;     // 0 = Disable loopback mode
	// 1 = Enable loopback mode
} PACK_END HostCmd_FW_SET_LOOPBACK_MODE, *PHostCmd_FW_SET_LOOPBACK_MODE;

/*
@11E-BA@
802.11e/WMM Related command(s)/data structures
*/

typedef PACK_START struct tagBAStreamFlags
{
	UINT32		BaType:1;
	UINT32		BaDirection:3;
	UINT32		Reserved:24;
}PACK_END BASTREAM_FLAGS;

// Flag to indicate if the stream is an immediate block ack stream.
// if this bit is not set, the stream is delayed block ack stream.
#define BASTREAM_FLAG_DELAYED_TYPE			0
#define BASTREAM_FLAG_IMMEDIATE_TYPE		1

// Flag to indicate the direction of the stream (upstream/downstream).
// If this bit is not set, the direction is downstream.
#define BASTREAM_FLAG_DIRECTION_UPSTREAM	0
#define BASTREAM_FLAG_DIRECTION_DOWNSTREAM	1
#define BASTREAM_FLAG_DIRECTION_DLP			2
#define BASTREAM_FLAG_DIRECTION_BOTH		3

typedef enum tagBaActionType
{
	BaCreateStream,
	BaUpdateStream,
	BaDestroyStream,
	BaFlushStream,
	BaCheckCreateStream
}BASTREAM_ACTION_TYPE;

typedef PACK_START struct tagBaContext
{
	UINT32	Context;
}PACK_END BASTREAM_CONTEXT;

// parameters for block ack creation
typedef PACK_START struct tagCreateBaParams
{
	// BA Creation flags - see above
	BASTREAM_FLAGS	Flags;
	// idle threshold
	UINT32			IdleThrs;
	// block ack transmit threshold (after how many pkts should we send BAR?)
	UINT32			BarThrs;
	// receiver window size
	UINT32			WindowSize;
	// MAC Address of the BA partner
	UINT8			PeerMacAddr[6];
	// Dialog Token
	UINT8			DialogToken;
	//TID for the traffic stream in this BA
	UINT8			Tid;
	// shared memory queue ID (not sure if this is required)
	UINT8			QueueId;
	UINT8           ParamInfo;
	// returned by firmware - firmware context pointer.
	// this context pointer will be passed to firmware for all future commands.
	BASTREAM_CONTEXT FwBaContext;
	UINT8           ResetSeqNo;  /** 0 or 1**/
	UINT16          CurrentSeq;
#ifdef V6FW
	UINT8			StaSrcMacAddr[6]; /* This is for virtual station in Sta proxy mode for V6FW */
#endif
}PACK_END BASTREAM_CREATE_STREAM;

// new transmit sequence number information 
typedef PACK_START struct tagBaUpdateSeqNum
{
	// BA flags - see above
	BASTREAM_FLAGS	Flags;
	// returned by firmware in the create ba stream response
	BASTREAM_CONTEXT FwBaContext;
	// new sequence number for this block ack stream
	UINT16			 BaSeqNum;
}PACK_END BASTREAM_UPDATE_STREAM;

typedef PACK_START struct tagBaStreamContext
{
	// BA Stream flags
	BASTREAM_FLAGS	 Flags;
	// returned by firmware in the create ba stream response
	BASTREAM_CONTEXT FwBaContext;
}PACK_END BASTREAM_STREAM_INFO;

//Command to create/destroy block ACK
typedef PACK_START struct tagHostCmd_FW_BASTREAM
{
	FWCmdHdr	CmdHdr;
	UINT32		ActionType;
	PACK_START union
	{
		// information required to create BA Stream...
		BASTREAM_CREATE_STREAM	CreateParams;
		// update starting/new sequence number etc.
		BASTREAM_UPDATE_STREAM	UpdtSeqNum;
		// destroy an existing stream...
		BASTREAM_STREAM_INFO	DestroyParams;
		// destroy an existing stream...
		BASTREAM_STREAM_INFO	FlushParams;
	}PACK_END BaInfo;
}PACK_END HostCmd_FW_BASTREAM, *PHostCmd_FW_BASTREAM;


typedef PACK_START struct tagHostCmd_GET_SEQNO
{
	FWCmdHdr    CmdHdr;
	uint8_t       MacAddr[6]; 
	uint8_t       TID;
	uint16_t      SeqNo;
	uint8_t       reserved;
} PACK_END HostCmd_GET_SEQNO;

#ifdef NDIS_MINIPORT_DRIVER
#pragma pack()
#endif



//
//*************************************************************
//*************************************************************
//*************************************************************
//
//
//
// Driver only
//
//   For diagnostic test purposes
//
#ifdef NDIS_MINIPORT_DRIVER

#define HostCmd_CMD_DUTY_CYCLE_TEST				0x002A
#define HostCmd_RET_DUTY_CYCLE_TEST				0x802A
#pragma pack(1)

/*  Define data structure for HostCmd_CMD_DUTY_CYCLE_TEST */
typedef struct _HostCmd_DS_DUTY_CYCLE_TEST {
	USHORT Command;
	USHORT Size;
	USHORT SeqNum;
	USHORT Result;
	USHORT Action;
	ULONG  BeaconOffsetInSQ;
	ULONG  RFParam;  //Replace beaconFrame[2] with RFParam 
	USHORT Reserved;

} HostCmd_DS_DUTY_CYCLE_TEST, *PHostCmd_DS_DUTY_CYCLE_TEST;

typedef struct _BSS_DESCRIPTION_SET_ALL_FIELDS
{
	UCHAR                     BSSID[EAGLE_ETH_ADDR_LEN];
	UCHAR                     SSID[EAGLE_MAX_SSID_LENGTH];
	UCHAR                     BSSType;
	USHORT                    BeaconPeriod;
	UCHAR                     DTIMPeriod;
	UCHAR                     TimeStamp[8];
	UCHAR                     LocalTime[8];
	IEEEtypes_PhyParamSet_t   PhyParamSet;
	IEEEtypes_SsParamSet_t    SsParamSet;
	IEEEtypes_CapInfo_t       Cap;
	//JS    
	//	UCHAR                     DataRates[16];
	UCHAR                     DataRates[RATE_INDEX_MAX_ARRAY];

	UCHAR					  Pad[5];
	// JS
	//UCHAR                     DataRates[8];
} BSS_DESCRIPTION_SET_ALL_FIELDS, *PBSS_DESCRIPTION_SET_ALL_FIELDS;


typedef struct _BSS_DESCRIPTION_SET_FIXED_FIELDS {
	UCHAR BSSID[EAGLE_ETH_ADDR_LEN];
	UCHAR SSID[EAGLE_MAX_SSID_LENGTH];
	UCHAR BSSType;
	USHORT BeaconPeriod;
	UCHAR DTIMPeriod;
	UCHAR TimeStamp[8];
	UCHAR LocalTime[8];
} BSS_DESCRIPTION_SET_FIXED_FIELDS, *PBSS_DESCRIPTION_SET_FIXED_FIELDS;
#pragma pack()
#endif   //NDIS_MINIPORT_DRIVER


#ifdef OBSOLETE
//***************************************************************************
//***************************************************************************
//***************************************************************************
//***************************************************************************
// 
//          Obsolete command code
//
#define HostCmd_CMD_OP_PARAM_DNLD               0x0002
#define HostCmd_CMD_EEPROM_UPDATE               0x0004
#define HostCmd_CMD_802_11_RESET                0x0005
#define HostCmd_CMD_802_11_SCAN                 0x0006
#define HostCmd_CMD_802_11_QUERY_TRAFFIC        0x0009
#define HostCmd_CMD_802_11_QUERY_STATUS         0x000a
#define HostCmd_CMD_802_11_GET_LOG              0x000b
#define HostCmd_CMD_802_11_AUTHENTICATE         0x0011
#define HostCmd_CMD_802_11_ASSOCIATE            0x0012
#define HostCmd_CMD_802_11_SET_WEP              0x0013
#define HostCmd_CMD_802_3_GET_STAT              0x0015
#define HostCmd_CMD_802_11_SNMP_MIB             0x0016
#define HostCmd_CMD_MAC_REG_MAP                 0x0017
#define HostCmd_CMD_BBP_REG_MAP                 0x0018
#define HostCmd_CMD_802_11_RSSI                 0x001f
#define HostCmd_CMD_802_11_RF_CHANNEL           0x001d
#define HostCmd_CMD_802_11_PS_MODE              0x0021
#define HostCmd_CMD_802_11_DATA_RATE            0x0022
#define HostCmd_CMD_RF_REG_MAP                  0x0023
#define HostCmd_CMD_802_11_DEAUTHENTICATE       0x0024
#define HostCmd_CMD_802_11_REASSOCIATE          0x0025
#define HostCmd_CMD_802_11_DISASSOCIATE         0x0026
#define HostCmd_CMD_MAC_CONTROL                 0x0028
#define HostCmd_CMD_802_11_QUERY_SCAN_RESULT    0x0029
#define HostCmd_CMD_802_11_AD_HOC_START         0x002B
#define HostCmd_CMD_802_11_AD_HOC_JOIN          0x002C

#define HostCmd_CMD_SET_TSF_TIMER               0x0101
#define HostCmd_CMD_SET_RUN_TX_TIMER            0x0102
#define HostCmd_CMD_SET_TX_POLL                 0x0103
#define HostCmd_CMD_SET_MPP_PSM                 0x0104
#define HostCmd_CMD_SET_BSSID                   0x0105
#define HostCmd_CMD_SET_REJOIN_TIMEOUT          0x0106
#define HostCmd_CMD_SET_BEACON_TIME             0x0109
#define HostCmd_CMD_SET_TWEAK_BEACON            0x010b

#define HostCmd_CMD_SET_ACTIVE_SCAN_SSID		   0x0112	// formerly 0x002D

#define HostCmd_RET_EEPROM_UPDATE               0x8004
#define HostCmd_RET_802_11_RESET                0x8005
#define HostCmd_RET_802_11_SCAN                 0x8006
#define HostCmd_RET_802_11_QUERY_TRAFFIC        0x8009
#define HostCmd_RET_802_11_STATUS_INFO          0x800a
#define HostCmd_RET_802_11_GET_LOG              0x800b
#define HostCmd_RET_802_11_AUTHENTICATE         0x8011
#define HostCmd_RET_802_11_ASSOCIATE            0x8012
#define HostCmd_RET_802_11_SET_WEP              0x8013
#define HostCmd_RET_802_3_STAT                  0x8015
#define HostCmd_RET_802_11_SNMP_MIB             0x8016
#define HostCmd_RET_MAC_REG_MAP                 0x8017
#define HostCmd_RET_BBP_REG_MAP                 0x8018
#define HostCmd_RET_RF_REG_MAP                  0x8023
#define HostCmd_RET_802_11_DEAUTHENTICATE       0x8024
#define HostCmd_RET_802_11_REASSOCIATE          0x8025
#define HostCmd_RET_802_11_DISASSOCIATE         0x8026
#define HostCmd_RET_MAC_CONTROL                 0x8028
#define HostCmd_RET_802_11_QUERY_SCAN_RESULT    0x8029
#define HostCmd_RET_802_11_AD_HOC_START         0x802B
#define HostCmd_RET_802_11_AD_HOC_JOIN          0x802C

#ifdef IGX_QOS
#define HostCmd_RET_802_11_SET_CW_PARAM         0x8030
#endif // end of IGX_QOS
#define HostCmd_RET_SET_ACTIVE_SCAN_SSID        0x8112	// formerly 0x802D

#define HostCmd_RET_SET_TSF_TIMER               0x8101
#define HostCmd_RET_SET_RUN_TX_TIMER            0x8102
#define HostCmd_RET_SET_TX_POLL                 0x8103
#define HostCmd_RET_SET_MPP_PSM                 0x8104
#define HostCmd_RET_SET_BSSID                   0x8105
#define HostCmd_RET_SET_REJOIN_TIMEOUT          0x8106
#define HostCmd_RET_SET_BEACON_TIME             0x8109
#define HostCmd_RET_SET_TWEAK_BEACON            0x810b
// 
//***************************************************************************
//***************************************************************************
//***************************************************************************
//

//*************************************************************
//*************************************************************
//*************************************************************
//
//          Obsolete command structures
//

//          Define data structure for HostCmd_CMD_OP_PARAM_DNLD
typedef struct _HostCmd_DS_OP_PARAM_DNLD {
	UCHAR OpParam[4096];
} HostCmd_DS_OP_PARAM_DNLD, *PHostCmd_DS_OP_PARAM_DNLD;

//          Define data structure for HostCmd_CMD_EEPROM_UPDATE
typedef struct _HostCmd_DS_EEPROM_UPDATE {
	USHORT Command;
	USHORT Size;
	USHORT SeqNum;
	USHORT Result;
	USHORT Action;                  // Detailed action or option
	ULONG Value;
} HostCmd_DS_EEPROM_UPDATE, *PHostCmd_DS_EEPROM_UPDATE;

//          Define data structure for HostCmd_CMD_802_11_RESET
typedef struct _HostCmd_DS_802_11_RESET {
	USHORT Command;
	USHORT Size;
	USHORT SeqNum;
	USHORT Result;
	USHORT Action;                  // ACT_NOT_REVERT_MIB, ACT_REVERT_MIB  or ACT_HALT
	USHORT Reserved;
} HostCmd_DS_802_11_RESET, *PHostCmd_DS_802_11_RESET;

//          Define data structure for HostCmd_CMD_802_11_SCAN
typedef struct _HostCmd_DS_802_11_SCAN {
	USHORT Command;
	USHORT Size;
	USHORT SeqNum;
	USHORT Result;
	USHORT IsAutoAssociation;
	UCHAR BSSType;
	UCHAR BSSID[EAGLE_ETH_ADDR_LEN];
	UCHAR SSID[EAGLE_MAX_SSID_LENGTH];
	UCHAR ScanType;
	USHORT ProbeDelay;
	UCHAR CHList[EAGLE_MAX_CHANNEL_NUMBER];
	USHORT MinCHTime;
	USHORT MaxCHTime;
	USHORT Reserved;
} HostCmd_DS_802_11_SCAN, *PHostCmd_DS_802_11_SCAN;

//          Define data structure for HostCmd_CMD_802_11_SCAN
typedef struct _HostCmd_DS_802_11_SCAN_RSP {
	USHORT Command;
	USHORT Size;
	USHORT SeqNum;
	USHORT Result;
	UCHAR BSSID[EAGLE_ETH_ADDR_LEN];
	UCHAR RSSI[EAGLE_MAX_BSS_DESCRIPTS];
	USHORT BSSDescriptSize;
	UCHAR  NumberOfSets;
} HostCmd_DS_802_11_SCAN_RSP, *PHostCmd_DS_802_11_SCAN_RSP;

//          Define data structure for HostCmd_CMD_802_11_QUERY_STATUS
typedef PACK_START struct _HostCmd_DS_802_11_QUERY_STATUS {
	FWCmdHdr    CmdHdr;
	UINT16      FWStatus;
	UINT16      MACStatus;
	UINT16      RFStatus;
	UINT16      CurentChannel;           // 1..99
	UINT8       APMACAdr[6];              // Associated AP MAC address
	UINT16      Reserved;
	UINT32      MaxLinkSpeed;             // Allowable max.link speed in unit of 100bps
} PACK_END HostCmd_DS_802_11_QUERY_STATUS, *PHostCmd_DS_802_11_QUERY_STATUS;

//          Define data structure for HostCmd_CMD_802_11_QUERY_TRAFFIC
typedef struct _HostCmd_DS_802_11_QUERY_TRAFFIC {
	USHORT Command;
	USHORT Size;
	USHORT SeqNum;
	USHORT Result;
	ULONG Traffic;                  // Traffic in bps
} HostCmd_DS_802_11_QUERY_TRAFFIC, *PHostCmd_DS_802_11_QUERY_TRAFFIC;

//          Define data structure for HostCmd_CMD_802_11_AUTHENTICATE
typedef struct _HostCmd_DS_802_11_AUTHENTICATE {
	USHORT Command;
	USHORT Size;
	USHORT SeqNum;
	USHORT Result;
	UCHAR MacAddr[6];
	UCHAR AuthType;
	USHORT TimeOut;
	UCHAR  Reserved[3];
} HostCmd_DS_802_11_AUTHENTICATE, *PHostCmd_DS_802_11_AUTHENTICATE;

//          Define data structure for HostCmd_RET_802_11_AUTHENTICATE
typedef struct _HostCmd_DS_802_11_AUTHENTICATE_RESULT {
	USHORT Command;
	USHORT Size;
	USHORT SeqNum;
	USHORT Result;
	UCHAR MacAddr[6];
	UCHAR AuthType;
	UCHAR AuthStatus;
} HostCmd_DS_802_11_AUTHENTICATE_RESULT, *PHostCmd_DS_802_11_AUTHENTICATE_RESULT;

//          Define data structure for HostCmd_CMD_802_11_DEAUTHENTICATE
typedef struct _HostCmd_DS_802_11_DEAUTHENTICATE {
	USHORT Command;
	USHORT Size;
	USHORT SeqNum;
	USHORT Result;
	UCHAR MacAddr[6];
	ULONG ReasonCode;
} HostCmd_DS_802_11_DEAUTHENTICATE, *PHostCmd_DS_802_11_DEAUTHENTICATE;

//          Define data structure for HostCmd_RET_802_11_DEAUTHENTICATE
typedef struct _HostCmd_DS_802_11_DEAUTHENTICATE_RESULT {
	USHORT Command;
	USHORT Size;
	USHORT SeqNum;
	USHORT Result;
	UCHAR MacAddr[6];
	UCHAR AuthStatus;
	UCHAR Reserved;
} HostCmd_DS_802_11_DEAUTHENTICATE_RESULT, *PHostCmd_DS_802_11_DEAUTHENTICATE_RESULT;

//          Define data structure for HostCmd_CMD_802_11_ASSOCIATE and  
//          HostCmd_CMD_802_11_REASSOCIATE
typedef struct _HostCmd_DS_802_11_ASSOCIATE {
	USHORT Command;
	USHORT Size;
	USHORT SeqNum;
	USHORT Result;
	UCHAR DestMacAddr[6];
	USHORT TimeOut;                 // Association failure timeout
	USHORT CapInfo;                 // Capability information
	USHORT ListenInterval;          // Listen interval
} HostCmd_DS_802_11_ASSOCIATE, *PHostCmd_DS_802_11_ASSOCIATE;

//          Define data structure for HostCmd_CMD_802_11_DISASSOCIATE
typedef struct _HostCmd_DS_802_11_DISASSOCIATE {
	USHORT Command;
	USHORT Size;
	USHORT SeqNum;
	USHORT Result;
	UCHAR DestMacAddr[6];
	USHORT ReasonCode;              // Disassociation reason code
} HostCmd_DS_802_11_DISASSOCIATE, *PHostCmd_DS_802_11_DISASSOCIATE;

//          Define data structure for HostCmd_RET_802_11_ASSOCIATE
typedef struct _HostCmd_DS_802_11_ASSOCIATE_RESULT {
	USHORT Command;
	USHORT Size;
	USHORT SeqNum;
	USHORT Result;
	UCHAR ResultCode;
	UCHAR Reserved[3];
}HostCmd_DS_802_11_ASSOCIATE_RESULT, *PHostCmd_DS_802_11_ASSOCIATE_RESULT;

//          Define data structure for HostCmd_RET_802_11_AD_HOC_JOIN
typedef struct _HostCmd_DS_802_11_AD_HOC_RESULT {
	USHORT Command;
	USHORT Size;
	USHORT SeqNum;
	USHORT Result;
	UCHAR  ResultCode;
	UCHAR  Reserved[3];
	UCHAR  BSSID[EAGLE_ETH_ADDR_LEN];	
}HostCmd_DS_802_11_AD_HOC_RESULT, *PHostCmd_DS_802_11_AD_HOC_RESULT;

//          Define data structure for HostCmd_CMD_802_11_SET_WEP
typedef struct _HostCmd_DS_802_11_SET_WEP {
	USHORT Command;
	USHORT Size;
	USHORT SeqNum;
	USHORT Result;
	USHORT Action;						// ACT_ADD, ACT_REMOVE or ACT_ENABLE 
	USHORT KeyIndex;					// Key Index selected for Tx
	UCHAR WEPTypeForKey1;				// 40, 128bit or TXWEP 
	UCHAR WEPTypeForKey2;
	UCHAR WEPTypeForKey3;
	UCHAR WEPTypeForKey4;
	UCHAR WEP1[16];						// WEP Key itself
	UCHAR WEP2[16];
	UCHAR WEP3[16];
	UCHAR WEP4[16];
} HostCmd_DS_802_11_SET_WEP, *PHostCmd_DS_802_11_SET_WEP;

//          Define data structure for HostCmd_CMD_802_3_GET_STAT
typedef struct _HostCmd_DS_802_3_GET_STAT {
	USHORT Command;
	USHORT Size;
	USHORT SeqNum;
	USHORT Result;
	ULONG XmitOK;
	ULONG RcvOK;
	ULONG XmitError;
	ULONG RcvError;
	ULONG RcvNoBuffer;
	ULONG RcvCRCError;
} HostCmd_DS_802_3_GET_STAT, *PHostCmd_DS_802_3_GET_STAT;

//          Define data structure for HostCmd_CMD_802_11_SNMP_MIB
typedef struct _HostCmd_DS_802_11_SNMP_MIB {
	USHORT Command;
	USHORT Size;
	USHORT SeqNum;
	USHORT Result;
	USHORT QueryType;
	USHORT OID;
	USHORT BufSize;
	UCHAR Value[128];
} HostCmd_DS_802_11_SNMP_MIB, *PHostCmd_DS_802_11_SNMP_MIB;

//          Define data structure for HostCmd_CMD_MAC_REG_MAP
typedef struct _HostCmd_DS_MAC_REG_MAP {
	USHORT Command;
	USHORT Size;
	USHORT SeqNum;
	USHORT Result;
	USHORT BufferSize;              // 128 UCHARs
	UCHAR RegMap[128];
	USHORT Reserved;
} HostCmd_DS_MAC_REG_MAP, *PHostCmd_DS_MAC_REG_MAP;

//          Define data structure for HostCmd_CMD_BBP_REG_MAP
typedef struct _HostCmd_DS_BBP_REG_MAP {
	USHORT Command;
	USHORT Size;
	USHORT SeqNum;
	USHORT Result;
	USHORT BufferSize;              // 128 UCHARs
	UCHAR RegMap[128];
	USHORT Reserved;
} HostCmd_DS_BBP_REG_MAP, *PHostCmd_DS_BBP_REG_MAP;

//          Define data structure for HostCmd_CMD_RF_REG_MAP
typedef struct _HostCmd_DS_RF_REG_MAP {
	USHORT Command;
	USHORT Size;
	USHORT SeqNum;
	USHORT Result;
	USHORT BufferSize;              // 64 UCHARs
	UCHAR RegMap[64];
	USHORT Reserved;
} HostCmd_DS_RF_REG_MAP, *PHostCmd_DS_RF_REG_MAP;

//          Define data structure for HostCmd_CMD_802_11_RF_CHANNEL
typedef struct _HostCmd_DS_802_11_RF_CHANNEL {
	USHORT Command;
	USHORT Size;
	USHORT SeqNum;
	USHORT Result;
	USHORT Action;
	USHORT RFType;                  // HostCmd_TYPE_802_11A or HostCmd_TYPE_802_11A                                   
	UCHAR ChannelList[EAGLE_MAX_CHANNEL_NUMBER];
	UCHAR CurentChannel;
	UCHAR Reserved;
} HostCmd_DS_802_11_RF_CHANNEL, *PHostCmd_DS_802_11_RF_CHANNEL;

//          Define data structure for HostCmd_CMD_802_11_RSSI
typedef struct _HostCmd_DS_802_11_RSSI {
	USHORT Command;
	USHORT Size;
	USHORT SeqNum;
	USHORT Result;
	LONG RSSI;                      // RSSI in dBm
	LONG SQBody;						// Signal quality for Body.this info is from SQ2 of RxPD
} HostCmd_DS_802_11_RSSI, *PHostCmd_DS_802_11_RSSI;

typedef enum
{
	DesiredBssType_i = 0,
	OpRateSet_i,
	BcnPeriod_i,
	DtimPeriod_i,
	AssocRspTimeOut_i,
	RtsThresh_i,
	ShortRetryLim_i,
	LongRetryLim_i,
	FragThresh_i,
	MaxTxMsduLife_i,
	MaxRxLife_i,
	ManufId_i,
	ProdId_i,
	ManufOui_i,
	ManufName_i,
	ManufProdName_i,
	ManufProdVer_i      
} SNMP_MIB_INDEX_e;

//          Define data structure for start Command in Ad Hoc mode  
typedef struct _HostCmd_DS_802_11_AD_HOC_START {
	UINT16                    Command;
	UINT16                    Size;
	UINT16                    SeqNum;
	UINT16                    Result;
	UINT8                     SSID[EAGLE_MAX_SSID_LENGTH];
	UINT8                     BSSType;
	UINT16                    BeaconPeriod;
	UINT8                     DTIMPeriod;
	IEEEtypes_SsParamSet_t    SsParamSet;
	IEEEtypes_PhyParamSet_t   PhyParamSet;
	UINT16                    ProbeDelay;
	IEEEtypes_CapInfo_t       Cap;
	UINT8                     BasicDataRates[8];
	UINT8                     OpDataRates[8];
} HostCmd_DS_802_11_AD_HOC_START, *PHostCmd_DS_802_11_AD_HOC_START;

//          Define data structure for Join Command in Ad Hoc mode
typedef struct _HostCmd_DS_802_11_AD_HOC_JOIN {
	UINT16                          Command;
	UINT16                          Size;
	UINT16                          SeqNum;
	UINT16                          Result;
	BSS_DESCRIPTION_SET_ALL_FIELDS  BssDescriptor;
	UINT16                          FailTimeOut;
	UINT16                          ProbeDelay;
	UINT8                           DataRates[8];
} HostCmd_DS_802_11_AD_HOC_JOIN, *PHostCmd_DS_802_11_AD_HOC_JOIN;

/*  Define data structure for HostCmd_CMD_SET_ACTIVE_SCAN_SSID */
typedef PACK_START struct _HostCmd_DS_SET_ACTIVE_SCAN_SSID {
	FWCmdHdr    CmdHdr;
	UINT16      Action;
	NDIS_802_11_SSID ActiveScanSSID;
} PACK_END HostCmd_DS_SET_ACTIVE_SCAN_SSID, *PHostCmd_DS_SET_ACTIVE_SCAN_SSID;

//          Define data structure for HostCmd_CMD_802_11_DATA_RATE
typedef PACK_START struct _HostCmd_DS_802_11_DATA_RATE {
	FWCmdHdr    CmdHdr;
	UINT16      Action;
	UINT16      Reserverd;    
	UINT8       DataRate[8];               // Supported data reate list
}PACK_END HostCmd_DS_802_11_DATA_RATE, *PHostCmd_DS_802_11_DATA_RATE;
#endif //OBSOLETE
//          Define data structure for HostCmd_CMD_SET_REGION_POWER
typedef PACK_START struct _HostCmd_DS_SET_REGION_POWER {
	FWCmdHdr    CmdHdr;
	UINT16      MaxPowerLevel;     
	UINT16      Reserved;
} PACK_END HostCmd_DS_SET_REGION_POWER, *PHostCmd_DS_SET_REGION_POWER;
//          Define data structure for HostCmd_CMD_SET_RATE_ADAPT_MODE
typedef PACK_START struct _HostCmd_DS_SET_RATE_ADAPT_MODE {
	FWCmdHdr	CmdHdr;
	UINT16		Action;
	UINT16		RateAdaptMode;     
} PACK_END HostCmd_DS_SET_RATE_ADAPT_MODE, *PHostCmd_DS_SET_RATE_ADAPT_MODE;

//          Define data structure for HostCmd_CMD_SET_LINKADAPT_CS_MODE
typedef PACK_START struct _HostCmd_DS_SET_LINKADAPT_CS_MODE {
	FWCmdHdr	CmdHdr;
	UINT16		Action;
	UINT16		CSMode;     
} PACK_END HostCmd_DS_SET_LINKADAPT_CS_MODE, *PHostCmd_DS_SET_LINKADAPT_CS_MODE;

typedef PACK_START struct tagHostCmd_FW_SET_N_PROTECT_FLAG
{
	FWCmdHdr    CmdHdr;
	UINT32      NProtectFlag;
} PACK_END HostCmd_FW_SET_N_PROTECT_FLAG, *PHostCmd_FW_SET_N_PROTECT_FLAG;

typedef PACK_START struct tagHostCmd_FW_SET_N_PROTECT_OPMODE
{
	FWCmdHdr    CmdHdr;
	UINT8       NProtectOpMode;
} PACK_END HostCmd_FW_SET_N_PROTECT_OPMODE, *PHostCmd_FW_SET_N_PROTECT_OPMODE;

typedef PACK_START struct tagHostCmd_FW_SET_OPTIMIZATION_LEVEL
{
	FWCmdHdr    CmdHdr;
	UINT8       OptLevel;
} PACK_END HostCmd_FW_SET_OPTIMIZATION_LEVEL, *PHostCmd_FW_SET_OPTIMIZATION_LEVEL;
#define CAL_TBL_SIZE        160
typedef PACK_START struct tagHostCmd_FW_GET_CALTABLE
{
	FWCmdHdr    CmdHdr;
	UINT8       annex; 
	UINT8       index;
	UINT8       len;
	UINT8       Reserverd; 
	UINT8       calTbl[CAL_TBL_SIZE];
} PACK_END HostCmd_FW_GET_CALTABLE, *PHostCmd_FW_GET_CALTABLE;


typedef PACK_START struct tagHostCmd_FW_SET_MIMOPSHT
{
	FWCmdHdr    CmdHdr;
	UINT8       Addr[6]; 
	UINT8       Enable;
	UINT8       Mode;
} PACK_END HostCmd_FW_SET_MIMOPSHT, *PHostCmd_FW_SET_MIMOPSHT;

#define MAX_BEACON_SIZE        1024
typedef PACK_START struct tagHostCmd_FW_GET_BEACON
{
	FWCmdHdr    CmdHdr;
	UINT16      Bcnlen;
	UINT8       Reserverd[2]; 
	UINT8       Bcn[MAX_BEACON_SIZE];
} PACK_END HostCmd_FW_GET_BEACON, *PHostCmd_FW_GET_BEACON;

#ifdef RXPATHOPT
typedef PACK_START struct _HostCmd_SET_RXPATHOPT {
	FWCmdHdr    CmdHdr;
	UINT32    	RxPathOpt;
	UINT32 		RxPktThreshold;
} PACK_END HostCmd_SET_RXPATHOPT, *PHostCmd_SET_RXPATHOPT;
#endif

#ifdef V6FW
typedef PACK_START struct tagHostCmd_DWDS_ENABLE
{
	FWCmdHdr    CmdHdr;
	UINT32      Enable;		//0 -- Disable. or 1 -- Enable.
} PACK_END HostCmd_DWDS_ENABLE, *PHostCmd_DWDS_ENABLE;
#endif

typedef PACK_START struct tagHostCmd_FW_FLUSH_TIMER
{
	FWCmdHdr    CmdHdr;
	UINT32      value;		//0 -- Disable. > 0 -- holds time value in usecs.
} PACK_END HostCmd_FW_FLUSH_TIMER, *PHostCmd_FW_FLUSH_TIMER;

#define RATE_TBL_SIZE	100
typedef PACK_START struct tagHostCmd_FW_GET_RATETABLE
{
	FWCmdHdr    CmdHdr;
	UINT8       Addr[6]; 
	#ifdef SOC_W8864
	UINT32	   SortedRatesIndexMap[2*RATE_ADAPT_MAX_SUPPORTED_RATES];		//Multiply 2 because 2 DWORD in rate info
	#else
	UINT32	   SortedRatesIndexMap[RATE_ADAPT_MAX_SUPPORTED_RATES];
	#endif
} PACK_END HostCmd_FW_GET_RATETABLE, *PHostCmd_FW_GET_RATETABLE;

typedef PACK_START struct tagHostCmd_FW_HT_GF_MODE {
	FWCmdHdr    CmdHdr;
	UINT32      Action;
	UINT32	   Mode;  
} PACK_END HostCmd_FW_HT_GF_MODE, *PHostCmd_FW_HT_GF_MODE;

typedef PACK_START struct tagHostCmd_FW_HT_STBC_MODE {
	FWCmdHdr    CmdHdr;
	UINT32      Action;
	UINT32	   Mode;  
} PACK_END HostCmd_FW_HT_STBC_MODE, *PHostCmd_FW_HT_STBC_MODE;
#ifdef COEXIST_20_40_SUPPORT
typedef PACK_START struct tagHostCmd_FW_SET_11N_20_40_SWITCHING
{
	FWCmdHdr    CmdHdr;
	UINT8           AddChannel;
} PACK_END HostCmd_FW_SET_11N_20_40_SWITCHING, *PHostCmd_FW_SET_11N_20_40_SWITCHING;
#endif
#ifdef EXPLICIT_BF
typedef PACK_START struct tagHostCmd_FW_HT_BF_MODE {
	FWCmdHdr    CmdHdr;
	UINT8	  option;
	UINT8         csi_steering;
	UINT8	  mcsfeedback;
	UINT8         mode;  /**  NDPA or control wrapper **/
	UINT8         interval;
	UINT8         slp;
	UINT8         power;
} PACK_END HostCmd_FW_HT_BF_MODE, *PHostCmd_FW_HT_BF_MODE;
typedef PACK_START struct tagHostCmd_FW_SET_NOACK
{
	FWCmdHdr    CmdHdr;
	UINT8           Enable;
} PACK_END HostCmd_FW_SET_NOACK, *PHostCmd_FW_SET_NOACK;

typedef PACK_START struct tagHostCmd_FW_RC_CAL
{
	FWCmdHdr    CmdHdr;
    UINT32      rc_cal[4][2];
} PACK_END HostCmd_FW_RC_CAL, *PHostCmd_FW_RC_CAL;

typedef PACK_START struct tagHostCmd_FW_TEMP
{
	FWCmdHdr    CmdHdr;
    UINT32      temp;
} PACK_END HostCmd_FW_TEMP, *PHostCmd_FW_TEMP;

typedef PACK_START struct tagHostCmd_FW_SET_NOSTEER
{
	FWCmdHdr    CmdHdr;
	UINT8           Enable;
} PACK_END HostCmd_FW_SET_NOSTEER, *PHostCmd_FW_SET_NOSTEER;

typedef PACK_START struct tagHostCmd_FW_SET_CDD
{
	FWCmdHdr    CmdHdr;
	UINT32      Enable;
} PACK_END HostCmd_FW_SET_CDD, *PHostCmd_FW_SET_CDD;

typedef PACK_START struct tagHostCmd_FW_SET_TXHOP
{
	FWCmdHdr    CmdHdr;
	UINT8      Enable;
	UINT8      Txhopstatus;
} PACK_END HostCmd_FW_SET_TXHOP, *PHostCmd_FW_SET_TXHOP;
typedef PACK_START struct tagHostCmd_FW_HT_BF_TYPE {
	FWCmdHdr    CmdHdr;
	UINT32      Action;
	UINT32	   Mode;
} PACK_END HostCmd_FW_HT_BF_TYPE, *PHostCmd_FW_HT_BF_TYPE;



#endif

#ifdef SSU_SUPPORT
typedef PACK_START struct tagHostCmd_FW_SET_SPECTRAL_ANALYSIS {
	FWCmdHdr    CmdHdr;
	UINT32      Action;
	UINT32	    NumOfBuffers;
    UINT32      BufferSize;
    UINT32      BufferBaseAddress;
    UINT32      Time;
    UINT8       FFT_length;
    UINT8       FFT_skip;
    UINT8       ADC_dec;
    UINT8       Notify;
} PACK_END HostCmd_FW_SET_SPECTRAL_ANALYSIS_TYPE, *PHostCmd_FW_SET_SPECTRAL_ANALYSIS_TYPE;
#endif

typedef PACK_START struct tagHostCmd_FW_SET_BW_SIGNALLING
{
	FWCmdHdr    CmdHdr;
	UINT32      Action;
	UINT32	   Mode;
} PACK_END HostCmd_FW_SET_BW_SIGNALLING, *PHostCmd_FW_SET_BW_SIGNALLING;

typedef PACK_START struct tagHostCmd_FW_GET_CONSEC_TXFAIL_ADDR
{
	FWCmdHdr    CmdHdr;
	UINT8       Addr[6]; 
} PACK_END HostCmd_FW_GET_CONSEC_TXFAIL_ADDR, *PHostCmd_FW_GET_CONSEC_TXFAIL_ADDR;

typedef PACK_START struct tagHostCmd_FW_TXFAILLIMIT
{
	FWCmdHdr    CmdHdr; 
	UINT32		txfaillimit;
} PACK_END HostCmd_FW_TXFAILLIMIT, *PHostCmd_FW_TXFAILLIMIT;

typedef PACK_START struct tagHostCmd_FW_VHT_OP_MODE
{
	FWCmdHdr    CmdHdr; 
	UINT8		Addr[6];
	UINT8		vht_NewRxChannelWidth;
	UINT8 		vht_NewRxNss;
} PACK_END HostCmd_FW_VHT_OP_MODE, *PHostCmd_FW_VHT_OP_MODE;

#ifdef WNC_LED_CTRL
typedef PACK_START struct tagHostCmd_FW_LED_CTRL
{
	FWCmdHdr	CmdHdr;
	UINT8		led_on;
} PACK_END HostCmd_FW_LED_CTRL, *PHostCmd_FW_LED_CTRL;
#endif
#endif /* __HOSTCMD__H */
