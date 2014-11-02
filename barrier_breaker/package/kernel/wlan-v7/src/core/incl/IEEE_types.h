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


/******************** (c) Marvell Semiconductor, Inc., 2001 *******************
*
* Purpose:
*    This file contains definitions relating to messages specified in the
*    IEEE 802.11 spec.
*
*****************************************************************************/

#ifndef _IEEE_TYPES_H_
#define _IEEE_TYPES_H_

/*============================================================================= */
/*                               INCLUDE FILES */
/*============================================================================= */
#include "wltypes.h"

/*============================================================================= */
/*                            PUBLIC DEFINITIONS */
/*============================================================================= */

/*--------------------------------------------------------------*/
/* Reason Codes - these codes are used in management message    */
/* frame bodies to indicate why an action is taking place (such */
/* as a disassociation or deauthentication).                    */
/*--------------------------------------------------------------*/
#define MAX_NR_IE 20 
#define IEEE_80211_MAX_NUMBER_OF_CHANNELS 64
#define IEEEtypes_REASON_RSVD                    0
#define IEEEtypes_REASON_UNSPEC                  1
#define IEEEtypes_REASON_PRIOR_AUTH_INVALID      2
#define IEEEtypes_REASON_DEAUTH_LEAVING          3
#define IEEEtypes_REASON_DISASSOC_INACTIVE       4
#define IEEEtypes_REASON_DISASSOC_AP_BUSY        5
#define IEEEtypes_REASON_CLASS2_NONAUTH          6
#define IEEEtypes_REASON_CLASS3_NONASSOC         7
#define IEEEtypes_REASON_DISASSOC_STA_HASLEFT    8
#define IEEEtypes_REASON_CANT_ASSOC_NONAUTH      9
#ifdef IEEE80211H
/***************IEEE802dot11h*****************/
#define IEEEtypes_REASON_DISASSOC_PWR_CAP_UNACCEPT 10
#define IEEEtypes_REASON_DISASSOC_SUP_CHA_UNACCEPT 11
#endif /* IEEE80211H */
/***************WPA Reasons*******************/
#define IEEEtypes_REASON_INVALID_IE              13
#define IEEEtypes_REASON_MIC_FAILURE             14
#define IEEEtypes_REASON_4WAY_HANDSHK_TIMEOUT    15
#define IEEEtypes_REASON_GRP_KEY_UPD_TIMEOUT     16
#define IEEEtypes_REASON_IE_4WAY_DIFF            17
#define IEEEtypes_REASON_INVALID_MCAST_CIPHER    18
#define IEEEtypes_REASON_INVALID_UNICAST_CIPHER  19
#define IEEEtypes_REASON_INVALID_AKMP            20
#define IEEEtypes_REASON_UNSUPT_RSNE_VER         21
#define IEEEtypes_REASON_INVALID_RSNE_CAP        22
#define IEEEtypes_REASON_8021X_AUTH_FAIL         23
/*********************************************/

/*------------------------------------------------------------*/
/* Status Codes - these codes are used in management message  */
/* frame bodies to indicate the results of an operation (such */
/* as association, reassociation, and authentication).        */
/*------------------------------------------------------------*/
#define IEEEtypes_STATUS_SUCCESS                 0
#define IEEEtypes_STATUS_UNSPEC_FAILURE          1
#define IEEEtypes_STATUS_CAPS_UNSUPPORTED        10
#define IEEEtypes_STATUS_REASSOC_NO_ASSOC        11
#define IEEEtypes_STATUS_ASSOC_DENIED_UNSPEC     12
#define IEEEtypes_STATUS_UNSUPPORTED_AUTHALG     13
#define IEEEtypes_STATUS_RX_AUTH_NOSEQ           14
#define IEEEtypes_STATUS_CHALLENGE_FAIL          15
#define IEEEtypes_STATUS_AUTH_TIMEOUT            16
#define IEEEtypes_STATUS_ASSOC_DENIED_BUSY       17
#define IEEEtypes_STATUS_ASSOC_DENIED_RATES      18

/* */
/* 802.11b additions */
/* */
#define IEEEtypes_STATUS_ASSOC_DENIED_NOSHORT    19
#define IEEEtypes_STATUS_ASSOC_DENIED_NOPBCC     20
#define IEEEtypes_STATUS_ASSOC_DENIED_NOAGILITY  21

#ifdef IEEE80211H
/* */
/* 802.11h additions */
/* */
#define IEEEtypes_STATUS_ASSOC_SPEC_MGMT_REQUIRED 22
#define IEEEtypes_STATUS_ASSOC_PWE_CAP_REQUIRED   23
#define IEEEtypes_STATUS_ASSOC_SUP_CHA_REQUIRED   24
#endif /* IEEE80211H */

/* */
/* 802.11g additions */
/* */
#define IEEEtypes_STATUS_ASSOC_DENIED_NOSHORTSLOTTIME 25
#define IEEEtypes_STATUS_ASSOC_DENIED_NODSSSOFDM      26

/* */
/* 802.11i additions */
/* */   
#define IEEEtypes_STATUS_ASSOC_DENIED_INVALID_IE                        40
#define IEEEtypes_STATUS_ASSOC_DENIED_INVALID_GRP_CIPHER                41
#define IEEEtypes_STATUS_ASSOC_DENIED_INVALID_PAIRWISE_CIPHER           42
#define IEEEtypes_STATUS_ASSOC_DENIED_INVALID_AKMP                      43
#define IEEEtypes_STATUS_ASSOC_DENIED_INVALID_RSN_IE                    44
#define IEEEtypes_STATUS_ASSOC_DENIED_INVALID_RSN_IE_CAP                45
#define IEEEtypes_STATUS_ASSOC_DENIED_CIPHER_SUITE_REJECTED             46

#ifdef QOS_FEATURE
#define IEEEtypes_STATUS_QOS_UNSPECIFIED_FAIL    32
#define IEEEtypes_STATUS_QOS_INSUFFICIENT_BW     33
#define IEEEtypes_STATUS_QOS_POOR_CONDITIONS     34
#define IEEEtypes_STATUS_QOS_NOT_QOSAP           35
#define IEEEtypes_STATUS_QOS_REFUSED             37
#define IEEEtypes_STATUS_QOS_INVALID_PARAMS      38
#define IEEEtypes_STATUS_QOS_TIMEOUT 39
#define IEEEtypes_STATUS_QOS_DLP_NOT_ALLOW       41
#define IEEEtypes_STATUS_QOS_DLP_NOT_PRESENT     42
#define IEEEtypes_STATUS_QOS_NOT_QSTA            43

#endif
/*--------------------------------------------*/
/* Various sizes used in IEEE 802.11 messages */
/*--------------------------------------------*/
#define IEEEtypes_ADDRESS_SIZE         6
#define IEEEtypes_BITMAP_SIZE          251
#define IEEEtypes_CHALLENGE_TEXT_SIZE  253
#define IEEEtypes_CHALLENGE_TEXT_LEN   128
#define IEEEtypes_MAX_DATA_RATES       8
#define IEEEtypes_MAX_DATA_BODY_LEN    2312
#define IEEEtypes_MAX_MGMT_BODY_LEN    2312
#define IEEEtypes_SSID_SIZE            32
#define IEEEtypes_TIME_STAMP_SIZE      8
#define IEEEtypes_MAX_CHANNELS         14
#define IEEEtypes_MAX_CHANNELS_A       IEEE_80211_MAX_NUMBER_OF_CHANNELS-IEEEtypes_MAX_CHANNELS//19
#define IEEEtypes_MAX_BSS_DESCRIPTS    128
#define W81_80211_HEADER_SIZE          32
#define IEEEtypes_MAX_DATA_RATES_G     14
#define IEEEtypes_MAX_DATA_RATES_A     9


#define HAL_MAX_SUPPORTED_MCS			24
#ifdef SOC_W8864
#define RATE_SUPPORTED_11AC_RATES   	20		//Value must be in sync with fw 11nrateadapt.h too
#else
#define RATE_SUPPORTED_11AC_RATES  		0
#endif

#define RATE_ADAPT_MAX_SUPPORTED_RATES \
        (IEEEtypes_MAX_DATA_RATES_G + HAL_MAX_SUPPORTED_MCS + RATE_SUPPORTED_11AC_RATES)


/*---------------------------------------------------------------------*/
/* Define masks used to extract fields from the capability information */
/* structure in a beacon message.                                      */
/*---------------------------------------------------------------------*/
#define IEEEtypes_CAP_INFO_ESS             1
#define IEEEtypes_CAP_INFO_IBSS            2
#define IEEEtypes_CAP_INFO_CF_POLLABLE     4
#define IEEEtypes_CAP_INFO_CF_POLL_RQST    8
#define IEEEtypes_CAP_INFO_PRIVACY         16
#define IEEEtypes_CAP_INFO_SHORT_PREAMB    32
#define IEEEtypes_CAP_INFO_PBCC            64
#define IEEEtypes_CAP_INFO_CHANGE_AGILITY  128
#ifdef IEEE80211H
#define IEEEtypes_CAP_INFO_SPECTRUM_MGMT    0x0100
#endif /* IEEE80211H */
#define IEEEtypes_CAP_INFO_SHORT_SLOT_TIME  0x0400
#define IEEEtypes_CAP_INFO_DSSS_OFDM        0x2000

/*---------------------------*/
/* Miscellaneous definitions */
/*---------------------------*/
#define IEEEtypes_PROTOCOL_VERSION     0

#define IEEEtypes_BASIC_RATE_FLAG 0x80
/* */
/* Used to determine which rates in a list are designated as basic rates */
/* */

#define IEEEtypes_BASIC_RATE_MASK 0x7F
/* */
/* Used to mask off the basic rate flag, if one exists, for given */
/* data rates */
/* */

#define IEEEtypes_RATE_MIN 2
/* */
/* The minimum allowable data rate in units of kb/s */
/* */

#define IEEEtypes_RATE_MAX 127
/* */
/* The maximum allowable data rate in units of kb/s */
/* */

#define IEEEtypes_TIME_UNIT 1024

#define IS_GROUP(macaddr)  ((*(UINT8*)macaddr & 0x01) == 0x01) 

/* */
/* The number of microseconds in 1 time unit, as specified in the */
/* 802.11 spec */
/* */
#ifdef MRVL_WSC
#define WSC_BEACON_IE_MAX_LENGTH        68 /* 21Dec06 changed from 28 to 68 per customer request */
#define WSC_PROBERESP_IE_MAX_LENGTH     251
#define WSC_OUI_LENGTH                  4

enum wsc_attribute {
WSC_VERSION_ATTRB =    0x104A,
WSC_RESP_TYPE_ATTRB =    0x103B,
WSC_VENDOR_EXTN_ATTRB =     0x1049,
};

#define WSC_VENDOR_ID                   0x00372A;
#endif //MRVL_WSC

/*============================================================================= */
/*                          PUBLIC TYPE DEFINITIONS */
/*============================================================================= */

/*---------------------------------------------------------------------------*/
/*                 Enumerated types used in 802.11 messages                  */
/*---------------------------------------------------------------------------*/
typedef enum
{
	IEEE_TYPE_MANAGEMENT = 0,
	IEEE_TYPE_CONTROL,
	IEEE_TYPE_DATA
} IEEEtypes_MsgType_e;
typedef UINT8 IEEEtypes_MsgType_t;
/* */
/* The 3 possible types of 802.11 messages */
/* */

typedef enum
{
	IEEE_MSG_ASSOCIATE_RQST = 0,
	IEEE_MSG_ASSOCIATE_RSP,
	IEEE_MSG_REASSOCIATE_RQST,
	IEEE_MSG_REASSOCIATE_RSP,
	IEEE_MSG_PROBE_RQST,
	IEEE_MSG_PROBE_RSP,
	IEEE_MSG_BEACON = 8,
	IEEE_MSG_ATIM,
	IEEE_MSG_DISASSOCIATE,
	IEEE_MSG_AUTHENTICATE,
	IEEE_MSG_DEAUTHENTICATE,
	IEEE_MSG_QOS_ACTION,
	IEEE_MSG_ACTION = 0x0d,
	IEEE_MSG_ACTION_NO_ACK = 0x0e
} IEEEtypes_MgmtSubType_e;
typedef UINT8 IEEEtypes_MgmtSubType_t;
/* */
/* The possible types of management messages */
/* */

typedef enum
{
#ifdef QOS_FEATURE
	BLK_ACK_REQ = 8,
	BLK_ACK,
#endif
	PS_POLL = 10,
	RTS,
	CTS,
	ACK,
	CF_END,
	CF_END_CF_ACK
} IEEEtypes_CtlSubType_e;
typedef UINT8 IEEEtypes_CtlSubType_t;
/* */
/* The possible types of control messages */
/* */

typedef enum
{
	DATA = 0,
	DATA_CF_ACK,
	DATA_CF_POLL,
	DATA_CF_ACK_CF_POLL,
	NULL_DATA,
	CF_ACK,
	CF_POLL,
	CF_ACK_CF_POLL,
#ifdef QOS_FEATURE
	QoS_DATA = 8,
	QoS_DATA_CF_ACK,
	QoS_DATA_CF_POLL,
	QoS_DATA_CF_ACK_CF_POLL,
	QoS_NULL_DATA,
	QoS_CF_ACK,
	QoS_CF_POLL,
	QoS_CF_ACK_CF_POLL
#endif
} IEEEtypes_DataSubType_e;
typedef UINT8 IEEEtypes_DataSubType_t;
/* */
/* The possible types of data messages */
/* */

typedef enum
{
	SME_CMD_ASSOCIATE,
	SME_CMD_AUTHENTICATE,
	SME_CMD_DEAUTHENTICATE,
	SME_CMD_DISASSOCIATE,
	SME_CMD_JOIN,
	SME_CMD_REASSOCIATE,
	SME_CMD_RESET,
	SME_CMD_SCAN,
	SME_CMD_START
#ifdef IEEE80211H
	,
	SME_CMD_MREQUEST,
	SME_CMD_MEASURE,
	SME_CMD_MREPORT,
	SME_CMD_TPCADPT,
	SMC_CMD_CHANNELSWITCH_REQ,
	SMC_CMD_CHANNELSWITCH_RSP
#endif
} IEEEtypes_SmeCmd_e;
typedef UINT8 IEEEtypes_SmeCmd_t;
/* */
/* The possible types of commands sent from the SME */
/* */

typedef enum
{
	SME_NOTIFY_PWR_MGMT_CFRM,
	SME_NOTIFY_SCAN_CFRM,
	SME_NOTIFY_JOIN_CFRM,
	SME_NOTIFY_AUTH_CFRM,
	SME_NOTIFY_AUTH_IND,
	SME_NOTIFY_DEAUTH_CFRM,
	SME_NOTIFY_DEAUTH_IND,
	SME_NOTIFY_ASSOC_CFRM,
	SME_NOTIFY_ASSOC_IND,
	SME_NOTIFY_REASSOC_CFRM,
	SME_NOTIFY_REASSOC_IND,
	SME_NOTIFY_DISASSOC_CFRM,
	SME_NOTIFY_DISASSOC_IND,
	SME_NOTIFY_RESET_CFRM,
#ifdef MRVL_DFS
	SME_NOTIFY_CHANNELSWITCH_CFRM,
	SME_NOTIFY_RADAR_DETECTION_IND,
#endif
	SME_NOTIFY_START_CFRM
#ifdef IEEE80211H
	,
	SME_NOTIFY_MREQUEST_IND,
	SME_NOTIFY_MREQUEST_CFRM,
	SME_NOTIFY_MEASURE_CFRM,
	SME_NOTIFY_MREPORT_IND,
	SME_NOTIFY_MREPORT_CFRM,
	SME_NOTIFY_TPCADPT_CFRM,
	SMC_NOTIFY_CHANNELSWITCH_IND,
	SMC_NOTIFY_CHANNELSWITCH_CFRM
#endif    
} IEEEtypes_SmeNotify_e;
typedef UINT8 IEEEtypes_SmeNotify_t;
/* */
/* The possible types of notifications sent from the SME */
/* */

/* */
/* The possible types of commands sent from the CB Processor */
/* */

typedef enum
{
	BSS_INFRASTRUCTURE = 1,
	BSS_INDEPENDENT,
	BSS_ANY
} IEEEtypes_Bss_e;
typedef UINT8 IEEEtypes_Bss_t;
/* */
/* The possible types of Basic Service Sets */
/* */

typedef enum
{
	SSID = 0,
	SUPPORTED_RATES,
	FH_PARAM_SET,
	DS_PARAM_SET,
	CF_PARAM_SET,
	TIM,
	IBSS_PARAM_SET,
	COUNTRY,
	CHALLENGE_TEXT = 16,
	ERP_INFO = 42,
	EXT_SUPPORTED_RATES = 50,
#ifdef QOS_FEATURE
	QBSS_LOAD = 11,
	EDCA_PARAM_SET = 12,
	TSPEC = 13,
	TCLAS = 14,
	SCHEDULE = 15,
#endif
#ifdef IEEE80211H
	PWR_CONSTRAINT = 32,
	PWR_CAP = 33,
	TPC_REQ = 34,
	TPC_REP = 35,
	SUPPORTED_CHANNEL = 36,
	CSA = 37,
	MEASUREMENT_REQ = 38,
	MEASUREMENT_REP = 39,
	QUIET = 40,
	IBSS_DFS = 41,    
#endif /* IEEE80211H */
#ifdef QOS_FEATURE
	TS_DELAY = 43,
	TCLAS_PROCESSING = 44,
	QOS_ACTION = 45,
	QOS_CAPABILITY = 46,
#endif
#ifdef MRVL_WAPI
	WAPI_IE = 68,
#endif
	HT = 45 /*51*/,
	ADD_HT = 61 /* 52*/,
	/* PROPRIETARY tags for HT and ADD_HT.*/
	HT_PROP = 51,
	ADD_HT_PROP = 52, 
	CHAN_REPORT = 51,
	PROPRIETARY_IE = 221,
	RSN_IE = 221,
	RSN_IEWPA2 = 48,
	VHT_CAP = 191,				
	VHT_OPERATION = 192,
	EXT_BSS_LOAD = 193,
	WIDE_BW_CHAN_SWITCH = 194,			
	VHT_TRANSMIT_POW_ENV = 195,			
	CHAN_SWITCH_WRAPPER = 196,			
	AID = 197,
	QUIET_CHANNEL = 198,
	OP_MODE_NOTIFICATION = 199	
#ifdef COEXIST_20_40_SUPPORT
	,
	EXT_CAP_IE = 127,
	_20_40_BSSCOEXIST = 72,
	_20_40_BSS_INTOLERANT_CHANNEL_REPORT = 73,
	OVERLAPPING_BSS_SCAN_PARAMETERS = 74
#endif

} IEEEtypes_ElementId_e;
typedef UINT8 IEEEtypes_ElementId_t;
/* */
/* Variable length mandatory fields or optional frame body components */
/* within management messages are called information elements; these */
/* elements all have associated with them an Element ID. IDs 7 to 15 */
/* are reserved; IDs 17 to 31 are reserved for challenge text; IDs */
/* 32 to 255 are reserved. */
/* */

typedef enum
{
	PWR_MODE_ACTIVE = 1,
	PWR_MODE_PWR_SAVE
} IEEEtypes_PwrMgmtMode_e;
typedef UINT8 IEEEtypes_PwrMgmtMode_t;
/* */
/* The possible power management modes */
/* */

typedef enum
{
	SCAN_ACTIVE,
	SCAN_PASSIVE
} IEEEtypes_Scan_e;
typedef UINT8 IEEEtypes_Scan_t;
/* */
/* The possible methods to scan for APs */
/* */

typedef enum
{
	AUTH_OPEN_SYSTEM = 0,
	AUTH_SHARED_KEY,
	AUTH_OPEN_OR_SHARED_KEY,
	AUTH_NOT_SUPPORTED
} IEEEtypes_AuthType_e;
typedef UINT8 IEEEtypes_AuthType_t;
/* */
/* The possible types of authentication */
/* */

typedef enum
{
	SCAN_RESULT_SUCCESS,
	SCAN_RESULT_INVALID_PARAMETERS,
	SCAN_RESULT_UNEXPECTED_ERROR
} IEEEtypes_ScanResult_e;
typedef UINT8 IEEEtypes_ScanResult_t;
/* */
/* The possible responses to a request to scan */
/* */

typedef enum
{
	JOIN_RESULT_SUCCESS,
	JOIN_RESULT_INVALID_PARAMETERS,
	JOIN_RESULT_TIMEOUT
} IEEEtypes_JoinResult_e;
typedef UINT8 IEEEtypes_JoinResult_t;
/* */
/* The possible responses to a request to join a BSS */
/* */

typedef enum
{
	AUTH_RESULT_SUCCESS,
	AUTH_RESULT_INVALID_PARAMETERS,
	AUTH_RESULT_TIMEOUT,
	AUTH_RESULT_TOO_MANY_SIMULTANEOUS_RQSTS,
	AUTH_RESULT_REFUSED,
	AUTH_RESULT_RESOURCE_ERROR
} IEEEtypes_AuthResult_e;
typedef UINT8 IEEEtypes_AuthResult_t;
/* */
/* The possible results to a request to authenticate */
/* */

typedef enum
{
	DEAUTH_RESULT_SUCCESS,
	DEAUTH_RESULT_INVALID_PARAMETERS,
	DEAUTH_RESULT_TOO_MANY_SIMULTANEOUS_RQSTS
} IEEEtypes_DeauthResult_e;
typedef UINT8 IEEEtypes_DeauthResult_t;
/* */
/* The possible results to a request to deauthenticate */
/* */

typedef enum
{
	ASSOC_RESULT_SUCCESS,
	ASSOC_RESULT_INVALID_PARAMETERS,
	ASSOC_RESULT_TIMEOUT,
	ASSOC_RESULT_REFUSED,
	ADDTS_RESULT_REFUSED
} IEEEtypes_AssocResult_e;
typedef UINT8 IEEEtypes_AssocResult_t;
/* */
/* The possible results to a request to associate */
/* */

typedef enum
{
	REASSOC_RESULT_SUCCESS,
	REASSOC_RESULT_INVALID_PARAMETERS,
	REASSOC_RESULT_TIMEOUT,
	REASSOC_RESULT_REFUSED
} IEEEtypes_ReassocResult_e;
typedef UINT8 IEEEtypes_ReassocResult_t;
/* */
/* The possible results to a request to reassociate */
/* */

typedef enum
{
	DISASSOC_RESULT_SUCCESS,
	DISASSOC_RESULT_INVALID_PARAMETERS,
	DISASSOC_RESULT_TIMEOUT,
	DISASSOC_RESULT_REFUSED
} IEEEtypes_DisassocResult_e;
typedef UINT8 IEEEtypes_DisassocResult_t;
/* */
/* The possible results to a request to disassociate */
/* */

typedef enum
{
	PWR_MGMT_RESULT_SUCCESS,
	PWR_MGMT_RESULT_INVALID_PARAMETERS,
	PWR_MGMT_RESULT_NOT_SUPPORTED
} IEEEtypes_PwrMgmtResult_e;
typedef UINT8 IEEEtypes_PwrMgmtResult_t;
/* */
/* The possible results to a request to change the power management mode */
/* */

typedef enum
{
	RESET_RESULT_SUCCESS
} IEEEtypes_ResetResult_e;
typedef UINT8 IEEEtypes_ResetResult_t;
/* */
/* The possible results to a request to reset */
/* */

typedef enum
{
	START_RESULT_SUCCESS,
	START_RESULT_INVALID_PARAMETERS,
	START_RESULT_BSS_ALREADY_STARTED_OR_JOINED
} IEEEtypes_StartResult_e;
typedef UINT8 IEEEtypes_StartResult_t;
/* */
/* The possible results to a request to start */
/* */

#ifdef IEEE80211H
typedef enum
{
	MREQUEST_RESULT_SUCCESS,
	MREQUEST_RESULT_INVALID_PARAMETERS,
	MREQUEST_RESULT_UNSPECIFIED_FAILURE
} IEEEtypes_MRequestResult_e;
typedef UINT8 IEEEtypes_MRequestResult_t;

typedef enum
{
	MEASURE_RESULT_SUCCESS,
	MEASURE_RESULT_INVALID_PARAMETERS,
	MEASURE_RESULT_UNSPECIFIED_FAILURE
} IEEEtypes_MeasureResult_e;
typedef UINT8 IEEEtypes_MeasureResult_t;

typedef enum
{
	MREPORT_RESULT_SUCCESS,
	MREPORT_RESULT_INVALID_PARAMETERS,
	MREPORT_RESULT_UNSPECIFIED_FAILUR
} IEEEtypes_MReportResult_e;
typedef UINT8 IEEEtypes_MReportResult_t;

typedef enum
{
	CHANNELSWITCH_RESULT_SUCCESS,
	CHANNELSWITCH_INVALID_PARAMETERS,
	CHANNELSWITCH_UNSPECIFIED_FAILURE
} IEEEtypes_ChannelSwitchResult_e;
typedef UINT8 IEEEtypes_ChannelSwitchResult_t;

typedef enum
{
	TPCADAPT_RESULT_SUCCESS,
	TPCADAPT_INVALID_PARAMETERS,
	TPCADAPT_UNSPECIFID_FAILURE
} IEEEtypes_TPCAdaptResult_e;
typedef UINT8 IEEEtypes_TPCAdaptResult_t;
#endif /* IEEE80211H */

typedef enum
{
	STATE_IDLE,
	STATE_SCANNING,
	STATE_JOINING,
	STATE_JOINED,
	STATE_AUTHENTICATED_WITH_AP,
	STATE_ASSOCIATING,
	STATE_ASSOCIATED,
	STATE_REASSOCIATING,
	STATE_RESTORING_FROM_SCAN,
	STATE_IBSS_STARTED,
} IEEEtypes_MacMgmtStates_e;
typedef UINT8 IEEEtypes_MacMgmtStates_t;

/* */
/* The possible states the MAC Management Service Task can be in */
/* */

#ifdef IEEE80211H
typedef enum
{
	TYPE_REQ_BASIC = 0,
	TYPE_REQ_CCA,
	TYPE_REQ_RPI,    
	TYPE_REQ_BCN = 5,   
#ifdef WMON
	TYPE_REQ_APS = 10,
	TYPE_REQ_RSS,
	TYPE_REQ_NOI,
	TYPE_REQ_FCS,
	TYPE_REQ_DFS,
	TYPE_REQ_PSE,
	TYPE_REQ_VRX,
#endif
} IEEEtypes_MeasurementReqType_e;
typedef UINT8 IEEEtypes_MeasurementReqType_t;

typedef enum
{
	TYPE_REP_BASIC = 0,
	TYPE_REP_CCA,
	TYPE_REP_RPI,    
#ifdef WMON
	TYPE_REP_APS = 10,
	TYPE_REP_RSS,
	TYPE_REP_NOI,
	TYPE_REP_FCS,
	TYPE_REP_DFS,
	TYPE_REP_PSE,
	TYPE_REP_VRX,
#endif
} IEEEtypes_MeasurementRepType_e;
typedef UINT8 IEEEtypes_MeasurementRepType_t;

#endif /* IEEE80211H */

/*---------------------------------------------------------------------------*/
/*           Types Used In IEEE 802.11 MAC Message Data Structures           */
/*---------------------------------------------------------------------------*/
typedef UINT8 IEEEtypes_Len_t;
/* */
/* Length type */
/* */

typedef UINT8 IEEEtypes_Addr_t;
/* */
/* Address type */
/* */

typedef IEEEtypes_Addr_t IEEEtypes_MacAddr_t[IEEEtypes_ADDRESS_SIZE];
/* */
/* MAC address type */
/* */

typedef UINT8 IEEEtypes_DataRate_t;
/* */
/* Type used to specify the supported data rates */
/* */

typedef UINT8 IEEEtypes_SsId_t[IEEEtypes_SSID_SIZE];
/* */
/* SS ID type */
/* */

/*---------------------------------------------------------------------------*/
/*                 IEEE 802.11 MAC Message Data Structures                   */
/*                                                                           */
/* Each IEEE 802.11 MAC message includes a MAC header, a frame body (which   */
/* can be empty), and a frame check sequence field. This section gives the   */
/* structures that used for the MAC message headers and frame bodies that    */
/* can exist in the three types of MAC messages - 1) Control messages,       */
/* 2) Data messages, and 3) Management messages.                             */
/*---------------------------------------------------------------------------*/
typedef  struct IEEEtypes_FrameCtl_t
{
	UINT16 ProtocolVersion:2;
	UINT16 Type:2;
	UINT16 Subtype:4;
	UINT16 ToDs:1;
	UINT16 FromDs:1;
	UINT16 MoreFrag:1;
	UINT16 Retry:1;
	UINT16 PwrMgmt:1;
	UINT16 MoreData:1;
	UINT16 Wep:1;
	UINT16 Order:1;
} PACK_END IEEEtypes_FrameCtl_t;
/* */
/* The frame control field in the header of a MAC message */
/* */

typedef  struct IEEEtypes_GenHdr_t
{
	UINT16 FrmBodyLen;
	IEEEtypes_FrameCtl_t FrmCtl;
	UINT16 DurationId;
	IEEEtypes_MacAddr_t Addr1;
	IEEEtypes_MacAddr_t Addr2;
	IEEEtypes_MacAddr_t Addr3;
	UINT16 SeqCtl;
	IEEEtypes_MacAddr_t Addr4;
} PACK_END IEEEtypes_GenHdr_t;
/* */
/* The general header for MAC messages */
/* */

typedef  struct IEEEtypes_MgmtHdr_t
{
	UINT16 FrmBodyLen;
	IEEEtypes_FrameCtl_t FrmCtl;
	UINT16 Duration;
	IEEEtypes_MacAddr_t DestAddr;
	IEEEtypes_MacAddr_t SrcAddr;
	IEEEtypes_MacAddr_t BssId;
	UINT16 SeqCtl;
	IEEEtypes_MacAddr_t Rsrvd;
} PACK_END IEEEtypes_MgmtHdr_t;
/* */
/* The header for MAC management messages */
/* */

typedef  struct IEEEtypes_MgmtHdr2_t
{
	IEEEtypes_FrameCtl_t FrmCtl;
	UINT16 Duration;
	IEEEtypes_MacAddr_t DestAddr;
	IEEEtypes_MacAddr_t SrcAddr;
	IEEEtypes_MacAddr_t BssId;
	UINT16 SeqCtl;
	IEEEtypes_MacAddr_t Rsrvd;
} PACK_END IEEEtypes_MgmtHdr2_t;


typedef  struct IEEEtypes_MgmtHdr3_t
{
	UINT16 FrmBodyLen;
	IEEEtypes_FrameCtl_t FrmCtl;
	UINT16 Duration;
	IEEEtypes_MacAddr_t DestAddr;
	IEEEtypes_MacAddr_t SrcAddr;
	IEEEtypes_MacAddr_t BssId;
	UINT16 SeqCtl;
	IEEEtypes_MacAddr_t Rsrvd;
} PACK_END IEEEtypes_MgmtHdr3_t;


typedef  struct IEEEtypes_PsPollHdr_t
{
	UINT16 FrmBodyLen;
	IEEEtypes_FrameCtl_t FrmCtl;
	UINT16 Aid;
	IEEEtypes_MacAddr_t BssId;
	IEEEtypes_MacAddr_t TxAddr;
} PACK_END IEEEtypes_PsPollHdr_t;
/* */
/* The header for power-save poll messages (the only control message */
/* processed by the MAC software) */
/* */

typedef  struct IEEEtypes_CtlHdr_t
{
	UINT16 FrmBodyLen;
	IEEEtypes_FrameCtl_t FrmCtl;
	UINT16 Duration;
	IEEEtypes_MacAddr_t DestAddr;
	IEEEtypes_MacAddr_t SrcAddr;
} PACK_END IEEEtypes_CtlHdr_t;
/* */
/* The header for MAC Ctl messages */
/* */

typedef  struct IEEEtypes_CtlFrame_t
{
	IEEEtypes_PsPollHdr_t Hdr;
	UINT32 FCS;
} PACK_END IEEEtypes_CtlFrame_t;
/* */
/* The structure for control frames (of which only the power-save */
/* poll is processed by the MAC software) */
/* */

typedef  struct IEEEtypes_DataFrame_t
{
	IEEEtypes_GenHdr_t Hdr;
	UINT8 FrmBody[IEEEtypes_MAX_DATA_BODY_LEN];
	UINT32 FCS;
} PACK_END IEEEtypes_DataFrame_t;
/* */
/* The structure for data frames */
/* */


/*-------------------------------------------------*/
/* Management Frame Body Components - Fixed Fields */
/*-------------------------------------------------*/
typedef UINT16 IEEEtypes_AId_t;
/* */
/* Association ID assigned by an AP during the association process */
/* */

typedef UINT16 IEEEtypes_AuthAlg_t;
/* */
/* Number indicating the authentication algorithm used (it can take */
/* on the values given by IEEEtypes_AuthType_e): */
/*    0 = Open system */
/*    1 = Shared key */
/*    All other values reserved */
/* */

typedef UINT16 IEEEtypes_AuthTransSeq_t;
/* */
/* Authentication transaction sequence number that indicates the current */
/* state of progress through a multistep transaction */
/* */

typedef UINT16 IEEEtypes_BcnInterval_t;
/* */
/* Beacon interval that represents the number of time units between */
/* target beacon transmission times */
/* */

typedef UINT8 IEEEtypes_DtimPeriod_t;
/* */
/* Interval that represents the number of time units between DTIMs. */
/* */

typedef  struct IEEEtypes_CapInfo_t
{
#ifdef QOS_FEATURE
	UINT16 Ess:1;
	UINT16 Ibss:1;
	UINT16 CfPollable:1;
	UINT16 CfPollRqst:1;
	UINT16 Privacy:1;
	UINT16 ShortPreamble:1;
	UINT16 Pbcc:1;
	UINT16 ChanAgility:1 ;
	UINT16 SpectrumMgmt : 1;
	UINT16 QoS : 1;
	UINT16 ShortSlotTime:1;
	UINT16 APSD : 1;
	UINT16 Rsrvd1 : 1;
	UINT16 DsssOfdm:1;
	UINT16 BlckAck : 1;
	UINT16 Rsrvd2 : 1;
#else
	unsigned Ess:1;
	unsigned Ibss:1;
	unsigned CfPollable:1;
	unsigned CfPollRqst:1;
	unsigned Privacy:1;
	unsigned ShortPreamble:1;
	unsigned Pbcc:1;
	unsigned ChanAgility:1 ;
#ifndef ERP
	unsigned Rsrvd:8;
#else
#ifdef IEEE80211H
	unsigned SpectrumMgmt : 1;
	unsigned Rsrvd1:1;
#else
	unsigned Rsrvd1:2;
#endif /* IEEE80211H */
	unsigned ShortSlotTime:1;
	unsigned Rsrvd2:2;
	unsigned DsssOfdm:1;
	unsigned Rsrvd3:2;
#endif
#endif //QOS_FEATURE
} PACK_END IEEEtypes_CapInfo_t;
/* */
/* Capability information used to indicate requested or advertised */
/* capabilities */
/* */

typedef UINT16 IEEEtypes_ListenInterval_t;
/* */
/* Listen interval to indicate to an AP how often a STA wakes to listen */
/* to beacon management frames */
/* */

typedef UINT16 IEEEtypes_ReasonCode_t;
/* */
/* Reason code to indicate the reason that an unsolicited notification */
/* management frame of type Disassociation or Deauthentication was */
/* generated */
/* */

typedef UINT16 IEEEtypes_StatusCode_t;
/* */
/* Status code used in a response management frame to indicate the */
/* success or failure of a requested operation */
/* */

typedef UINT8 IEEEtypes_TimeStamp_t[IEEEtypes_TIME_STAMP_SIZE];
/* */
/* Time stamp */
/* */
typedef  struct IEEEtypes_QosCtl_t
{
	UINT16 QosControl;
} PACK_END IEEEtypes_QosCtl_t;
/*-------------------------------------------------------*/
/* Management Frame Body Components - Information Fields */
/*-------------------------------------------------------*/
typedef  struct IEEEtypes_InfoElementHdr_t
{
	IEEEtypes_ElementId_t ElementId;
	IEEEtypes_Len_t Len;
} PACK_END IEEEtypes_InfoElementHdr_t;

typedef  struct IEEEtypes_SsIdElement_t
{
	IEEEtypes_ElementId_t ElementId;
	IEEEtypes_Len_t Len;
	IEEEtypes_SsId_t SsId;
} PACK_END IEEEtypes_SsIdElement_t;
/* */
/* SSID element that idicates the identity of an ESS or IBSS */
/* */

typedef  struct IEEEtypes_SuppRatesElement_t
{
	IEEEtypes_ElementId_t ElementId;
	IEEEtypes_Len_t Len;
	IEEEtypes_DataRate_t Rates[IEEEtypes_MAX_DATA_RATES];
} PACK_END IEEEtypes_SuppRatesElement_t;
/* */
/* Supported rates element that specifies the rates in the operational */
/* rate set in the MLME join request and the MLME start request */
/* */

typedef  struct IEEEtypes_FhParamSet_t
{
	IEEEtypes_ElementId_t ElementId;
	IEEEtypes_Len_t Len;
	UINT16 DwellTime;
	UINT8 HopSet;
	UINT8 HopPattern;
	UINT8 HopIndex;
} PACK_END IEEEtypes_FhParamSet_t;
/* */
/* FH parameter set that conatins the set of parameters necessary to */
/* allow sychronization for stations using a frequency hopping PHY */
/* */

typedef  struct IEEEtypes_DsParamSet_t
{
	IEEEtypes_ElementId_t ElementId;
	IEEEtypes_Len_t Len;
	UINT8 CurrentChan;
	//  UINT8 CurrentChan2;
} PACK_END IEEEtypes_DsParamSet_t;
/* */
/* DS parameter set that contains information to allow channel number */
/* identification for stations using a direct sequence spread spectrum PHY */
/* */

typedef  struct IEEEtypes_CfParamSet_t
{
	IEEEtypes_ElementId_t ElementId;
	IEEEtypes_Len_t Len;
	UINT8 CfpCnt;
	UINT8 CfpPeriod;
	UINT16 CfpMaxDuration;
	UINT16 CfpDurationRemaining;
} PACK_END IEEEtypes_CfParamSet_t;
/* */
/* CF parameter set that contains the set of parameters necessary to */
/* support the PCF */
/* */

typedef  struct IEEEtypes_Tim_t
{
	IEEEtypes_ElementId_t ElementId;
	IEEEtypes_Len_t Len;
	UINT8 DtimCnt;
	UINT8 DtimPeriod;
	UINT8 BitmapCtl;
	UINT8 PartialVirtualBitmap[IEEEtypes_BITMAP_SIZE];
} PACK_END IEEEtypes_Tim_t;
/* */
/* TIM, which contains: */
/* 1) DTIM count - how many beacons (including the current beacon */
/*    frame) appear before the next DTIM; a count of 0 indicates the */
/*    current TIM is the DTIM */
/* */
/* 2) DTIM period - indicates the number of beacon intervals between */
/*    successive DTIMs */
/* */
/* 3) Bitmap control - contains the traffic indicator bit associated */
/*    with association ID 0 - this is set to 1 for TIM elements with a */
/*    a value of 0 in the DTIM count field when one or more broadcast */
/*    or multicast frames are buffered at the AP. The remaining bits */
/*    of the field form the bitmap offset */
/* */
/* 4) Partial virtual bitmap - indicates which stations have messages */
/*    buffered at the AP, for which the AP is prepared to deliver at */
/*    the time the beacon frame is transmitted */

typedef  struct IEEEtypes_IbssParamSet_t
{
	IEEEtypes_ElementId_t ElementId;
	IEEEtypes_Len_t Len;
	UINT16 AtimWindow;
} PACK_END IEEEtypes_IbssParamSet_t;
/* */
/* IBSS parameters necessary to support an IBSS */
/* */

typedef  struct IEEEtypes_ChallengeText_t
{
	IEEEtypes_ElementId_t ElementId;
	IEEEtypes_Len_t Len;
	UINT8 Text[IEEEtypes_CHALLENGE_TEXT_SIZE];
} PACK_END IEEEtypes_ChallengeText_t;
/* */
/* The challenge text used in authentication exchanges */
/* */

/*-------------------------*/
/* Management Frame Bodies */
/*-------------------------*/
typedef  union IEEEtypes_PhyParamSet_t
{
	IEEEtypes_FhParamSet_t FhParamSet;
	IEEEtypes_DsParamSet_t DsParamSet;
} PACK_END IEEEtypes_PhyParamSet_t;
/* */
/* The parameter set relevant to the PHY */
/* */

typedef  union IEEEtypes_SsParamSet_t
{
	IEEEtypes_CfParamSet_t CfParamSet;
	IEEEtypes_IbssParamSet_t IbssParamSet;
} PACK_END IEEEtypes_SsParamSet_t;
/* */
/* Service set parameters - for a BSS supporting, PCF, the */
/* CF parameter set is used; for an independent BSS, the IBSS */
/* parameter set is used. */
/* */
typedef  struct IEEEtypes_ExtSuppRatesElement_t
{
	IEEEtypes_ElementId_t ElementId;
	IEEEtypes_Len_t Len;
	IEEEtypes_DataRate_t Rates[IEEEtypes_MAX_DATA_RATES];
} PACK_END IEEEtypes_ExtSuppRatesElement_t;

typedef  struct IEEEtypes_ERPInfo_t
{
	UINT8 NonERPPresent:1;
	UINT8 UseProtection:1;
	UINT8 BarkerPreamble:1;
	UINT8 reserved:5;
} PACK_END IEEEtypes_ERPInfo_t;

typedef  struct IEEEtypes_ERPInfoElement_t
{
	IEEEtypes_ElementId_t ElementId;
	IEEEtypes_Len_t Len;
	IEEEtypes_ERPInfo_t ERPInfo;
} PACK_END IEEEtypes_ERPInfoElement_t;

#ifdef WPA

// Authentication Key Mgmt Suite Selector
#define IEEEtypes_RSN_AUTH_KEY_SUITE_RSVD      0
#define IEEEtypes_RSN_AUTH_KEY_SUITE_8021X     1
#define IEEEtypes_RSN_AUTH_KEY_SUITE_PSK       2

// Cipher Suite Selector
#define IEEEtypes_RSN_CIPHER_SUITE_NONE     0
#define IEEEtypes_RSN_CIPHER_SUITE_WEP40    1
#define IEEEtypes_RSN_CIPHER_SUITE_TKIP     2
#define IEEEtypes_RSN_CIPHER_SUITE_WRAP     3
#define IEEEtypes_RSN_CIPHER_SUITE_CCMP     4
#define IEEEtypes_RSN_CIPHER_SUITE_WEP104   5

//#define MAX_SIZE_RSN_IE_BUF 32  // number of bytes
#endif

typedef struct IEEEtypes_RSN_IE_t
{
	UINT8 ElemId;
	UINT8 Len;
	UINT8   OuiType[4];    /*00:50:f2:01 */
	UINT8   Ver[2];
	UINT8 GrpKeyCipher[4];
	UINT8   PwsKeyCnt[2];
	UINT8 PwsKeyCipherList[4];
	UINT8   AuthKeyCnt[2];
	UINT8 AuthKeyList[4];
	//UINT8   RsnCap[2];
} IEEEtypes_RSN_IE_t;

#ifdef AP_WPA2
typedef  struct IEEEtypes_RSN_IE_WPA2_t
{
	UINT8 ElemId;
	UINT8 Len;
	//UINT8   OuiType[4];    //00:50:f2:01
	UINT8   Ver[2];
	UINT8 GrpKeyCipher[4];
	UINT8   PwsKeyCnt[2];
	UINT8 PwsKeyCipherList[4];
	UINT8   AuthKeyCnt[2];
	UINT8 AuthKeyList[4];
	UINT8   RsnCap[2];
	UINT8   PMKIDCnt[2];
	UINT8 PMKIDList[16];
} PACK_END IEEEtypes_RSN_IE_WPA2_t;

typedef  struct IEEEtypes_RSN_IE_WPA2MixedMode_t
{
	UINT8 ElemId;
	UINT8 Len;
	//UINT8   OuiType[4];    //00:50:f2:01
	UINT8   Ver[2];
	UINT8 GrpKeyCipher[4];
	UINT8   PwsKeyCnt[2];
	UINT8 PwsKeyCipherList[4];
	UINT8 PwsKeyCipherList2[4];
	UINT8   AuthKeyCnt[2];
	UINT8 AuthKeyList[4];
	UINT8   RsnCap[2];
	UINT8   PMKIDCnt[2];
	UINT8 PMKIDList[16];
} PACK_END IEEEtypes_RSN_IE_WPA2MixedMode_t;
#else
typedef  struct IEEEtypes_RSN_IE_WPA2_t
{
	UINT8 ElemId;
	UINT8 Len;
	//UINT8   OuiType[4];    //00:50:f2:01
	UINT8   Ver[2];
	UINT8 GrpKeyCipher[4];
	UINT8   PwsKeyCnt[2];
	UINT8 PwsKeyCipherList[4];
	UINT8   AuthKeyCnt[2];
	UINT8 AuthKeyList[4];
	UINT8   RsnCap[2];
	UINT8   PMKIDCnt[2];
	UINT8 PMKIDList[16];
}PACK_END IEEEtypes_RSN_IE_WPA2_t;
#endif

#ifdef MRVL_WAPI
#define IEEEtypes_WAPI_IE_MAX_LEN 128
typedef  struct IEEEtypes_WAPI_IE_t
{
	IEEEtypes_ElementId_t ElementId;
	IEEEtypes_Len_t Len;
	UINT8 buf[IEEEtypes_WAPI_IE_MAX_LEN];
} PACK_END IEEEtypes_WAPI_IE_t;

struct nlmsghdr2
{
	u32 nlmsg_len;
	u16 nlmsg_type;
	u16 nlmsg_flags;
	u32 nlmsg_seq;
	u32 nlmsg_pid;
};

struct asso_mt_t
{
	struct nlmsghdr2 hdr; /**/
	u16		type;         /* Message Type */
	u16		data_len;     /* Message Length */
	u8 		ap_mac[6];
	u8 		pad1[2];
	u8		mac[6];       /* STA MAC address */
	u8 		pad[2];
	u8		gsn[16];      /* Mcast data index */
	u8		wie[256];     /* wapi IE */
};
typedef struct asso_mt_t asso_mt;
#endif

#ifdef COUNTRY_INFO_SUPPORT

typedef struct _DomainCapabilityEntry
{
	UINT8 FirstChannelNo;
	UINT8 NoofChannel;
	UINT8 MaxTransmitPw;   
}PACK_END DomainCapabilityEntry;



#endif

#ifdef IEEE80211H
typedef  struct IEEEtypes_PowerConstraintElement_t
{
	IEEEtypes_ElementId_t ElementId;
	IEEEtypes_Len_t Len;
	SINT8 value;
} PACK_END IEEEtypes_PowerConstraintElement_t;

typedef  struct IEEEtypes_COUNTRY_IE_t
{
	IEEEtypes_ElementId_t ElemId;//wyatth
	UINT8 Len;
	UINT8 CountryCode[3];    
	UINT8 DomainEntry[100]; /** give a big no for now **/
} PACK_END IEEEtypes_COUNTRY_IE_t;


typedef  struct IEEEtypes_PowerCapabilityElement_t
{
	IEEEtypes_ElementId_t ElementId;
	IEEEtypes_Len_t Len;
	SINT8 MaxTxPwr;
	SINT8 MinTxPwr;
} PACK_END IEEEtypes_PowerCapabilityElement_t;

typedef  struct IEEEtypes_TPCReqElement_t
{
	IEEEtypes_ElementId_t ElementId;
	IEEEtypes_Len_t Len;    
} PACK_END IEEEtypes_TPCReqElement_t;

typedef  struct IEEEtypes_TPCRepElement_t
{
	IEEEtypes_ElementId_t ElementId;
	IEEEtypes_Len_t Len;   
	SINT8 TxPwr;
	SINT8 LinkMargin;
} PACK_END IEEEtypes_TPCRepElement_t;

typedef  struct ChannelDesp_t
{
	UINT8 FisrtChannel;
	UINT8 NumberofChannel;
} PACK_END ChannelDesp_t;

typedef  struct IEEEtypes_SupportedChannelElement_t
{
	IEEEtypes_ElementId_t ElementId;
	IEEEtypes_Len_t Len;       
#define MAX_SUPPORTED_CHANNEL_TUPLE (IEEE_80211_MAX_NUMBER_OF_CHANNELS * 2)        
	ChannelDesp_t SupportedChannel[MAX_SUPPORTED_CHANNEL_TUPLE];    
} PACK_END IEEEtypes_SupportedChannelElement_t;

typedef  struct IEEEtypes_ChannelSwitchAnnouncementElement_t
{
	IEEEtypes_ElementId_t ElementId;
	IEEEtypes_Len_t Len;   
	UINT8 Mode;
	UINT8 Channel;
	UINT8 Count;
} PACK_END IEEEtypes_ChannelSwitchAnnouncementElement_t;

typedef  struct IEEEtypes_MeasurementReqMode_t
{    
	UINT8 Rsv0:1;
	UINT8 Enable:1;
	UINT8 Request:1;
	UINT8 Report:1;
	UINT8 Rsv1:4;    
} PACK_END IEEEtypes_MeasurementReqMode_t;

typedef  struct IEEEtypes_MeasurementReq_t
{    
	UINT8 Channel;
	UINT8 StartTime[8];
	UINT16 Duration;   
} PACK_END IEEEtypes_MeasurementReq_t;

typedef  struct IEEEtypes_MeasurementRequestElement_t
{
	IEEEtypes_ElementId_t ElementId;
	IEEEtypes_Len_t Len;   
	UINT8 Token;
	IEEEtypes_MeasurementReqMode_t Mode;
	IEEEtypes_MeasurementReqType_t Type;
	IEEEtypes_MeasurementReq_t Request;
} PACK_END IEEEtypes_MeasurementRequestElement_t;

typedef  struct IEEEtypes_MeasurementRepMode_t
{    
	UINT8 Late:1;
	UINT8 Incapable:1;
	UINT8 Refused:1;    
	UINT8 Rsv1:5;    
} PACK_END IEEEtypes_MeasurementRepMode_t;

typedef  struct IEEEtypes_MeasurementRepMap_t
{    
	UINT8 BSS:1;
	UINT8 OFDM:1;
	UINT8 UnidentifiedSignal:1;    
	UINT8 Radar:1;    
	UINT8 Unmeasured:1;
	UINT8 Rsv:3;
} PACK_END IEEEtypes_MeasurementRepMap_t;


#ifdef WMON
#define	MAX_WMON_APS_SIZE	250
#endif
typedef  struct IEEEtypes_MeasurementRep_t
{    
	UINT8 Channel;
	UINT8 StartTime[8];
	UINT8 Duration[2];
	union
	{
		IEEEtypes_MeasurementRepMap_t Map;
		UINT8 BusyFraction;
		UINT8 RPI[8];
#ifdef WMON
		char	APS[MAX_WMON_APS_SIZE];
		UINT8	RSSI ;
		UINT32	FCS ;
		char	DFS[MAX_WMON_APS_SIZE];
		char 	PSE[MAX_WMON_APS_SIZE];
		UINT32	VRX ;
#endif
	} data;
} PACK_END IEEEtypes_MeasurementRep_t;

typedef  struct IEEEtypes_MeasurementReportElement_t
{
	IEEEtypes_ElementId_t ElementId;
	IEEEtypes_Len_t Len;   
	UINT8 MeasurementToken;
	IEEEtypes_MeasurementRepMode_t Mode;
	IEEEtypes_MeasurementRepType_t Type;
	IEEEtypes_MeasurementRep_t Report;    
} PACK_END IEEEtypes_MeasurementReportElement_t;


typedef  struct IEEEtypes_QuietElement_t
{
	IEEEtypes_ElementId_t ElementId;
	IEEEtypes_Len_t Len;   
	UINT8 Count;
	UINT8 Period;
	UINT16 Duration;
	UINT16 Offset;
} PACK_END IEEEtypes_QuietElement_t;

typedef  struct IEEEtypes_IBSS_DFS_Eleement_t
{
	IEEEtypes_ElementId_t ElementId;
	IEEEtypes_Len_t Len;   
	/*
	* No implementation necessary when working as AP
	*/
} PACK_END IEEEtypes_IBSS_DFS_Eleement_t;


#endif /* IEEE80211H */
#if 0//def EXPLICIT_BF
typedef  struct IEEEtypes_TXBF_Cap_t
{    
	UINT32 ImplicitTxBFRxCapable:1;
	UINT32 RxStaggeredSoundingCapable:1;
	UINT32 TxStaggeredSoundingCapable:1;
	UINT32 RxNDPCapable:1;
	UINT32 TxNDPCapable:1;
	UINT32 ImplicitTXBfTXCapable:1;
	UINT32 Calibration:2;
	UINT32 ExplicitCSITxBFCapable:1;
	UINT32 ExplicitNonCompressedSteerCapable:1;
	UINT32 ExplicitCompressedSteeringCapable:1;
	UINT32 ExplicitTxBFCSIFB:2;
	UINT32 ExplicitNonCompressedBFFBCapable:2;
	UINT32 ExplicitCompressedBFFBCapable:2;
	UINT32 MinimalGrouping:2;
	UINT32 CSINoBFAntSupport:2;
	UINT32 NonCompressedSteerNoAntennaSupport:2;
	UINT32 CompressedSteerNoAntennaSupport:2;
	UINT32 CSIMaxNoRowBFSupport:2;
	UINT32 ChannelEstimateCapable:2;
	UINT32 Reserved:3;
} PACK_END IEEEtypes_TXBF_Cap_t;
#endif

typedef  struct IEEEtypes_HT_Cap_t
{    
	UINT16 AdvCoding:1;
	UINT16 SupChanWidth:1;
	UINT16 MIMOPwSave:2;
	UINT16 GreenField:1;
	UINT16 SGI20MHz:1;
	UINT16 SGI40MHz:1;
	UINT16 TxSTBC:1;
	UINT16 RxSTBC:2;
	UINT16 DelayedBA:1;
	UINT16 MaxAMSDUSize:1;
	UINT16 DssCck40MHz:1;
	UINT16 PSMP:1;
	UINT16 FortyMIntolerant:1;
	UINT16 LSIGTxopProc:1;
} PACK_END IEEEtypes_HT_Cap_t;

typedef  struct IEEEtypes_HT_Element_t
{
	UINT8 ElementId;
	IEEEtypes_Len_t Len;  
	IEEEtypes_HT_Cap_t HTCapabilitiesInfo;
	UINT8   MacHTParamInfo;
	UINT8 SupportedMCSset[16];
	UINT16 ExtHTCapabilitiesInfo;
#ifdef EXPLICIT_BF
#if 0
	IEEEtypes_TXBF_Cap_t TxBFCapabilities;
#else
	UINT32 TxBFCapabilities;
#endif
#endif
	UINT8 ASCapabilities;
} PACK_END IEEEtypes_HT_Element_t;


typedef  PACK_START struct IEEEtypes_vht_cap_info_t
{    
	UINT32 MaximumMPDULength:2;
	UINT32 SupportedChannelWidthSet:2;
	UINT32 RxLDPC:1;
	UINT32 ShortGI80MHz:1;
	UINT32 ShortGI16080and80MHz:1;
	UINT32 TxSTBC:1;
	UINT32 RxSTBC:3;
	UINT32 SUBeamformerCapable:1;
	UINT32 SUBeamformeeCapable:1;
	UINT32 CompressedSteeringNumberofBeamformerAntennaSupported:3;
	UINT32 NumberofSoundingDimensions:3;
	UINT32 MUBeamformerCapable:1;
	UINT32 MUBeamformeeCapable:1;
	UINT32 VhtTxhopPS:1;
	UINT32 HtcVhtCapable:1;
	UINT32 MaximumAmpduLengthExponent:3;
	UINT32 VhtLinkAdaptationCapable:2;
	UINT32 RxAntennaPatternConsistency:1;
	UINT32 TxAntennaPatternConsistency:1;
	UINT32 Reserved:2;
} PACK_END IEEEtypes_VHT_Cap_Info_t;


typedef struct IEEEtypes_vht_cap {
    UINT8 id;
    UINT8 len;
    IEEEtypes_VHT_Cap_Info_t cap;		
    UINT32 SupportedRxMcsSet;
    UINT32 SupportedTxMcsSet;
} PACK_END IEEEtypes_VhtCap_t;

typedef struct IEEEtypes_vht_opt {
    UINT8 id;
    UINT8 len;
    UINT8 ch_width;
    UINT8 center_freq0;
    UINT8 center_freq1;
    UINT16 basic_mcs;
} PACK_END IEEEtypes_VhOpt_t;

typedef struct IEEEtypes_vht_operating_mode {
    UINT8 ChannelWidth:2;
    UINT8 Reserved:2;
    UINT8 RxNss:3;
	UINT8 RxNssType:1;	
} PACK_END IEEEtypes_VHT_operating_mode;

typedef struct IEEEtypes_vht_op_mode_action {
    UINT8 Category;
    UINT8 Action;
    IEEEtypes_VHT_operating_mode OperatingMode;
} PACK_END IEEEtypes_VHT_op_mode_action_t;

typedef struct IEEEtypes_vht_op_mode {
    UINT8 id;
    UINT8 len;
    IEEEtypes_VHT_operating_mode OperatingMode;
} PACK_END IEEEtypes_VHT_op_mode_t;


typedef  struct IEEEtypes_M_Element_t
{
	UINT8 ElementId;
	IEEEtypes_Len_t Len;  
	UINT8 OUI[3];
	UINT8 OUIType;
	UINT8 OUISubType;
	UINT8 Version;
} PACK_END IEEEtypes_M_Element_t;
#define MAXRPTRDEVTYPESTR	32
typedef  struct IEEEtypes_M_Rptr_Element_t
{
	UINT8 ElementId;
	IEEEtypes_Len_t Len;  
	UINT8 OUI[3];
	UINT8 OUIType;
	UINT8 OUISubType;
	UINT8 Version;
	UINT8 RptrDeviceType[MAXRPTRDEVTYPESTR];
} PACK_END IEEEtypes_M_Rptr_Element_t;

#ifdef COEXIST_20_40_SUPPORT
typedef  struct IEEEtypes_Ext_Cap_t
{    
	UINT8 _20_40Coexistence_Support:1;	/*bit0*/
	UINT8 Reserved1:1;					/*bit1*/
	UINT8 ExtChanSwitching:1;			/*bit2*/
	UINT8 Reserved2:1;					/*bit3*/
	UINT8 PSMP_Cap:1;					/*bit4*/
	UINT8 Reserved3:1;					/*bit5*/
	UINT8 SPSMP_Support:1;				/*bit6*/
	UINT8 Event:1;						/*bit7*/
	
	UINT8 Diagnotics:1;					/*bit8*/
	UINT8 MulticastDiagnostics:1;		/*bit9*/
	UINT8 LocationTracking:1;			/*bit10*/
	UINT8 FMS:1;						/*bit11*/
	UINT8 ProxyARPService:1;			/*bit12*/
	UINT8 CollocatedIntfReporting:1;	/*bit13*/
	UINT8 CivicLocation:1;				/*bit14*/
	UINT8 GioSpatialLocation:1;			/*bit15*/

	UINT8 TFS:1;						/*bit16*/
	UINT8 WNMSleepMode:1;				/*bit17*/
	UINT8 TIMBroadcast:1;				/*bit18*/
	UINT8 BSSTransition:1;				/*bit19*/
	UINT8 QoSTrafficCap:1;				/*bit20*/
	UINT8 ACStationCount:1;				/*bit21*/
	UINT8 MultipleBSSID:1;				/*bit22*/
	UINT8 TimingMeasurement:1;			/*bit23*/

	UINT8 ChannelUsage:1;				/*bit24*/
	UINT8 SSIDList:1;					/*bit25*/
	UINT8 DMS:1;						/*bit26*/
	UINT8 UTCTSFOffset:1;				/*bit27*/
	UINT8 TDLSPeerUAPSDBufSTASup:1;		/*bit28*/
	UINT8 TDLSPeerPSMSupport:1;			/*bit29*/
	UINT8 TDLSChanSwitching:1;			/*bit30*/
	UINT8 Interworking:1;				/*bit31*/

	UINT8 QoSMap:1;						/*bit32*/
	UINT8 EBR:1;						/*bit33*/
	UINT8 SSPNInterface:1;				/*bit34*/
	UINT8 Reserved4:1;					/*bit35*/
	UINT8 MSGCFCapibility:1;			/*bit36*/
	UINT8 TDLSSupport:1;				/*bit37*/
	UINT8 TDLSProhibited:1;				/*bit38*/
	UINT8 TDLSChanSwitchProhibited:1;	/*bit39*/

	UINT8 RejectUnadmittedFrame:1;		/*bit40*/
	UINT8 ServIntervalGranularity:3;	/*bit41 - bit43*/
	UINT8 IdentifierLocation:1;			/*bit44*/
	UINT8 UAPSDCoexistence:1;			/*bit45*/
	UINT8 WNMNotification:1;			/*bit46*/
	UINT8 Reserved5:1;					/*bit47*/

	UINT8 UTF8SSID:1;					/*bit48*/
	UINT8 Reserved6:7;					/*bit49 - bit55*/

	UINT8 Reserved7:5;					/*bit56 - bit60*/
	UINT8 TDLSWiderBW:1;				/*bit61*/
	UINT8 OpModeNotification:1;			/*bit62*/
	UINT8 Reserved8:1;					/*bit63*/
} PACK_END IEEEtypes_Ext_Cap_t;

typedef struct IEEEtypes_Extended_Cap_Element
{
	UINT8 ElementId;
	IEEEtypes_Len_t Len;
	IEEEtypes_Ext_Cap_t ExtCap;
} PACK_END IEEEtypes_Extended_Cap_Element_t;

typedef struct IEEEtypes_20_40_Coexist
{
	UINT8 Inform_Request:1;
	UINT8 FortyMhz_Intorant:1;
	UINT8 TwentyMhz_BSS_Width_Request:1;
	UINT8 OBSS_Scanning_Exemption_Request:1;
	UINT8 OBSS_Scanning_Exemption_Grant:1;
	UINT8 Reserved:3;

} PACK_END IEEEtypes_20_40_Coexist_t;

typedef struct IEEEtypes_20_40_BSS_COEXIST_Element
{
	UINT8 ElementId;
	IEEEtypes_Len_t Len;
	IEEEtypes_20_40_Coexist_t Coexist;
} PACK_END IEEEtypes_20_40_BSS_COEXIST_Element_t;




typedef struct IEEEtypes_OVERLAP_BSS_SCAN_PARAMETERS_Element
{
	UINT8 ElementId;
	IEEEtypes_Len_t Len;
	UINT16 Scan_Passive;
	UINT16 Scan_Active;
	UINT16 Channel_Width_Trigger_Scan_Interval;
	UINT16 Scan_Passive_Total_Per_Channel;
	UINT16 Scan_Active_Total_Per_Channel;
	UINT16 Width_Channel_Transition_Delay_Factor;
	UINT16 Scan_Activity_Threshold;

} PACK_END IEEEtypes_OVERLAP_BSS_SCAN_PARAMETERS_Element_t;

typedef  struct IEEEtypes_20_40_INTOLERANT_CHANNEL_REPORT_Element
{
	IEEEtypes_ElementId_t ElementId;
	IEEEtypes_Len_t Len;   
	UINT8 RegClass;
	UINT8 ChanList[IEEEtypes_MAX_CHANNELS+IEEEtypes_MAX_CHANNELS_A];
} PACK_END IEEEtypes_20_40_INTOLERANT_CHANNEL_REPORT_Element_t;

typedef  struct IEEEtypes_20_40_Coexist_Act
{      
	UINT8  Category;
	UINT8  Action;
	IEEEtypes_20_40_BSS_COEXIST_Element_t  Coexist_Report;
	IEEEtypes_20_40_INTOLERANT_CHANNEL_REPORT_Element_t Intolerant_Report;

} PACK_END IEEEtypes_20_40_Coexist_Act_t;
#endif


#ifdef INTEROP
/** currently b company use this oui for High thruput element 51 **/
typedef  struct IEEEtypes_Generic_HT_Element_t 
{
	UINT8 ElementId;
	IEEEtypes_Len_t Len;
	UINT8 OUI[3];
	UINT8 OUIType;
	IEEEtypes_HT_Cap_t HTCapabilitiesInfo;
	UINT8   MacHTParamInfo;
	UINT8 SupportedMCSset[16];
	UINT16 ExtHTCapabilitiesInfo;
#ifdef EXPLICIT_BF
#if 0
	IEEEtypes_TXBF_Cap_t TxBFCapabilities;
#else
	UINT32 TxBFCapabilities;
#endif
#endif
	UINT8 ASCapabilities;
} PACK_END IEEEtypes_Generic_HT_Element_t;

/** Just for I_COMP **/
typedef  struct IEEEtypes_Generic_HT_Element_t2 
{
	UINT8 ElementId;
	IEEEtypes_Len_t Len;
	UINT8 OUI[3];
	UINT8 OUIType;
	IEEEtypes_Len_t Len2;
	IEEEtypes_HT_Cap_t HTCapabilitiesInfo;
	UINT8   MacHTParamInfo;
	UINT8 SupportedMCSset[16];
	UINT16 ExtHTCapabilitiesInfo;
#ifdef EXPLICIT_BF
#if 0
	IEEEtypes_TXBF_Cap_t TxBFCapabilities;
#else
	UINT32 TxBFCapabilities;
#endif
#endif
	UINT8 ASCapabilities;
} PACK_END IEEEtypes_Generic_HT_Element_t2;
#endif
typedef  struct IEEEtypes_Add_HT_Chan_t
{    
	UINT8 ExtChanOffset:2;
	UINT8 STAChannelWidth:1;
	UINT8 RIFSMode:1;    
	UINT8 PSMPStasOnly:1;    
	UINT8 SrvcIntvlGran:3;
} PACK_END IEEEtypes_Add_HT_Chan_t;
typedef  struct IEEEtypes_Add_HT_OpMode_t
{    
	UINT16 OpMode:2;
	UINT16 NonGFStaPresent:1;
	UINT16 TransBurstLimit:1;
	UINT16 NonHTStaPresent:1;
	UINT16 Rsrv:11;
} PACK_END IEEEtypes_Add_HT_OpMode_t;
typedef  struct IEEEtypes_Add_HT_STBC_t
{    
	UINT16 BscSTBC:7;
	UINT16 DualSTBCProc:1;
	UINT16 ScdBcn:1;
	UINT16 LSIGTxopProcFullSup:1;
	UINT16 PCOActive:1;
	UINT16 PCOPhase:1;
	UINT16 Rsrv:4;
} PACK_END IEEEtypes_Add_HT_STBC_t;
#ifdef INTEROP
typedef  struct IEEEtypes_Generic_Add_HT_Element_t
{
	UINT8 ElementId;
	IEEEtypes_Len_t Len;
	UINT8 OUI[3];
	UINT8 OUIType;
	UINT8 ControlChan;
	IEEEtypes_Add_HT_Chan_t AddChan; 
	IEEEtypes_Add_HT_OpMode_t OpMode;
	IEEEtypes_Add_HT_STBC_t stbc;
	UINT8 BscMCSSet[16];
} PACK_END IEEEtypes_Generic_Add_HT_Element_t;
typedef  struct IEEEtypes_Generic_Add_HT_Element_t2
{
	UINT8 ElementId;
	IEEEtypes_Len_t Len;
	UINT8 OUI[3];
	UINT8 OUIType;
	IEEEtypes_Len_t Len2;
	UINT8 ControlChan;
	IEEEtypes_Add_HT_Chan_t AddChan; 
	IEEEtypes_Add_HT_OpMode_t OpMode;
	IEEEtypes_Add_HT_STBC_t stbc;
	UINT8 BscMCSSet[16];
} PACK_END IEEEtypes_Generic_Add_HT_Element_t2;
#endif
typedef  struct IEEEtypes_Add_HT_Element_t
{
	UINT8 ElementId;
	IEEEtypes_Len_t Len; 
	UINT8 ControlChan;
	IEEEtypes_Add_HT_Chan_t AddChan; 
	IEEEtypes_Add_HT_OpMode_t OpMode;
	IEEEtypes_Add_HT_STBC_t stbc;
	UINT8 BscMCSSet[16];
} PACK_END IEEEtypes_Add_HT_Element_t;

typedef  struct IEEEtypes_Add_HT_INFO_t
{
	UINT8 ControlChan;
	IEEEtypes_Add_HT_Chan_t AddChan; 
	IEEEtypes_Add_HT_OpMode_t OpMode;
	IEEEtypes_Add_HT_STBC_t stbc;
} PACK_END IEEEtypes_Add_HT_INFO_t;

typedef struct IEEEtypes_Bcn_t
{
	IEEEtypes_TimeStamp_t TimeStamp;
	IEEEtypes_BcnInterval_t BcnInterval;
	IEEEtypes_CapInfo_t CapInfo;
	IEEEtypes_SsIdElement_t SsId;
	IEEEtypes_SuppRatesElement_t SuppRates;
	IEEEtypes_PhyParamSet_t PhyParamSet;
	IEEEtypes_SsParamSet_t SsParamSet;
	IEEEtypes_RSN_IE_t RsnIE;
	IEEEtypes_Tim_t Tim;
	IEEEtypes_ERPInfoElement_t ERPInfo;
	IEEEtypes_ExtSuppRatesElement_t ExtSuppRates;
#ifdef IEEE80211H    
	IEEEtypes_COUNTRY_IE_t  Country;
	IEEEtypes_PowerConstraintElement_t PwrCons;
	IEEEtypes_ChannelSwitchAnnouncementElement_t ChSwAnn;
	IEEEtypes_QuietElement_t Quiet;
	IEEEtypes_TPCRepElement_t TPCRep;

#endif /* IEEE80211H */
}PACK_END IEEEtypes_Bcn_t;
/* */
/* Beacon message body */
/* */

typedef  struct IEEEtypes_DisAssoc_t
{
	IEEEtypes_ReasonCode_t ReasonCode;
} PACK_END IEEEtypes_DisAssoc_t;
/* */
/* Disassociation message body */
/* */

typedef  struct IEEEtypes_AssocRqst_t
{
	IEEEtypes_CapInfo_t CapInfo;
	IEEEtypes_ListenInterval_t ListenInterval;
	IEEEtypes_SsIdElement_t SsId;
	IEEEtypes_SuppRatesElement_t SuppRates;
#ifdef IEEE80211H
	IEEEtypes_PowerCapabilityElement_t PwrCap;
	IEEEtypes_SupportedChannelElement_t Channels;
#endif /*IEEE80211H*/
	IEEEtypes_ExtSuppRatesElement_t ExtSuppRates;
	IEEEtypes_RSN_IE_t RsnIE;
}PACK_END IEEEtypes_AssocRqst_t;

#ifdef QOS_FEATURE
typedef  struct
{
	UINT16 traffic_type : 1;
	UINT16 tsid : 4;
	UINT16 direction : 2;
	UINT16 access_policy : 2;
	UINT16 aggregation : 1;
	UINT16 apsd : 1;
	UINT16 usr_priority : 3;
	UINT16 ts_info_ack_policy : 2;
	UINT8 sched : 1;
	UINT8 rsvd : 7;
}PACK_END IEEEtypes_TS_info_t;

typedef struct
{
	UINT16 traffic_type : 1;
	UINT16 tsid : 4;
	UINT16 direction : 2;
	UINT16 access_policy : 2;
	UINT16 aggregation : 1;
	UINT16 apsd : 1;
	UINT16 usr_priority : 3;
	UINT16 ts_info_ack_policy : 2;
	UINT8 sched : 1;
	UINT8 rsvd : 7;
}Mrvl_TS_info_t;

typedef  struct
{
	UINT8 ElementId;
	UINT8 Len;
	IEEEtypes_TS_info_t ts_info; 
	UINT16   nom_msdu_size; /*nominal msdu size*/
	UINT16 max_msdu_size;
	UINT32 min_SI; /*minimum service interval*/
	UINT32 max_SI; /*maximum service interval*/
	UINT32 inactive_intrvl; /*inactivity interval*/
	UINT32 suspen_intrvl; /*Suspension interval*/
	UINT32 serv_start_time; /*service start time*/
	UINT32 min_data_rate;
	UINT32 mean_data_rate;
	UINT32 peak_data_rate;
	UINT32 max_burst_size;
	UINT32 delay_bound;
	UINT32 min_phy_rate;
	UINT16 srpl_bw_allow; /*Surplus bandwidth allowance*/
	UINT16 med_time; /*medium time*/    
}PACK_END IEEEtypes_TSPEC_t;

typedef struct
{
	UINT8 ElementId;
	UINT8 Len;
	Mrvl_TS_info_t ts_info; 
	//UINT8 ts_info[3]; 
	UINT16   nom_msdu_size; /*nominal msdu size*/
	UINT16 max_msdu_size;
	UINT32 min_SI; /*minimum service interval*/
	UINT32 max_SI; /*maximum service interval*/
	UINT32 inactive_intrvl; /*inactivity interval*/
	UINT32 suspen_intrvl; /*Suspension interval*/
	UINT32 serv_start_time; /*service start time*/
	UINT32 min_data_rate;
	UINT32 mean_data_rate;
	UINT32 peak_data_rate;
	UINT32 max_burst_size;
	UINT32 delay_bound;
	UINT32 min_phy_rate;
	UINT16 srpl_bw_allow; /*Surplus bandwidth allowance*/
	UINT16 med_time; /*medium time*/    
}Mrvl_TSPEC_t;

typedef  struct
{
	IEEEtypes_MacAddr_t src_addr;
	IEEEtypes_MacAddr_t dst_addr;
	UINT16 type;
}PACK_END Classif_type_0;

typedef  struct
{
	UINT8 ver;
	UINT8 src_IP_addr[4];
	UINT8 dst_IP_addr[4];
	UINT16 src_port;
	UINT16 dst_port;
	UINT8 DSCP;
	UINT8 protocol;
	UINT8 rsvd;

}PACK_END Classif_type_1_IPv4;

typedef  struct
{
	UINT8 ver;
	UINT8 src_IP_addr[4];
	UINT8 dst_IP_addr[4];
	UINT16 src_port;
	UINT16 dst_port;
	UINT8 flow_label[3];

}PACK_END Classif_type_1_IPv6;

typedef  struct
{
	UINT16 eight02_dot_1_tag;
}PACK_END Classif_type_2;

typedef  union
{
	Classif_type_0      classif_0;
	Classif_type_1_IPv4 classif_1_IPv4;
	Classif_type_1_IPv6 classif_1_IPv6;
	Classif_type_2      classif_0_IPv2;

}PACK_END Frm_Classif_Params_t;

typedef  struct
{
	UINT8 classif_type;
	UINT8 classif_mask;
	Frm_Classif_Params_t classif_params;
}PACK_END Frm_classifier_t;

typedef  struct
{
	UINT8 ElementId;
	UINT8 Len;
	UINT8 usr_priority;
	Frm_classifier_t frm_classifier;
}PACK_END TCLAS_t; 

typedef  struct 
{
	UINT8 OUI[3];
	UINT8 Type;
	UINT8 Subtype;
} PACK_END OUI_t;

typedef  struct
{
	UINT8 EDCA_param_set_update_cnt : 4; /*EDCA parameter set update count*/
#ifndef WMM_PS_SUPPORT
	UINT8 Q_ack : 1;
	UINT8 Q_req : 1;
	UINT8 TXOP_req : 1;
	UINT8 more_data_ack : 1;
#else
	UINT8 Reserved: 3;
	UINT8 U_APSD: 1;
#endif
}PACK_END QoS_Info_t;

#ifdef WMM_PS_SUPPORT
typedef  struct
{
	UINT8 EDCA_param_set_update_cnt : 4; /*EDCA parameter set update count*/
	UINT8 Q_ack : 1;
	UINT8 Q_req : 1;
	UINT8 TXOP_req : 1;
	UINT8 more_data_ack : 1;
}PACK_END QoS_Wsm_Info_t;
#endif


#if 1//def QOS_WSM_FEATURE
typedef  struct
{
	UINT8 ElementId;
	UINT8 Len;
	OUI_t OUI;
	UINT8 version;
#ifdef WMM_PS_SUPPORT
	QoS_Wsm_Info_t QoS_info;
#else
	QoS_Info_t QoS_info;
#endif
}PACK_END WSM_QoS_Cap_Elem_t;
typedef  struct
{
	UINT8 ElementId;
	UINT8 Len;
	OUI_t OUI;
	UINT8 version;
	IEEEtypes_TS_info_t ts_info; 
	UINT16   nom_msdu_size; /*nominal msdu size*/
	UINT16 max_msdu_size;
	UINT32 min_SI; /*minimum service interval*/
	UINT32 max_SI; /*maximum service interval*/
	UINT32 inactive_intrvl; /*inactivity interval*/
	UINT32 suspen_intrvl; /*Suspension interval*/
	UINT32 serv_start_time; /*service start time*/
	UINT32 min_data_rate;
	UINT32 mean_data_rate;
	UINT32 peak_data_rate;
	UINT32 max_burst_size;
	UINT32 delay_bound;
	UINT32 min_phy_rate;
	UINT16 srpl_bw_allow; /*Surplus bandwidth allowance*/
	UINT16 med_time; /*medium time*/    
}PACK_END WSM_TSPEC_t;

typedef  struct
{
	UINT8 usr_priority;
	Frm_classifier_t frm_classifier;
} PACK_END WSM_Frm_classifier_t;

typedef  struct 
{          
	UINT8 ElementId;
	UINT8 Len;
	OUI_t OUI;
	UINT8 version;
#ifdef STA_QOS
	WSM_Frm_classifier_t WSM_Frm_classifier;
#else
	UINT8 usr_priority;
	Frm_classifier_t frm_classifier;
#endif
} PACK_END WSM_TCLAS_Elem_t;

typedef struct
{
	UINT8 ElementId;
	UINT8 Len;
	OUI_t OUI;
	UINT8 version;
	IEEEtypes_TS_info_t ts_info; 
}PACK WSM_TSInfo_t;

typedef struct 
{
	UINT8     Category;
	UINT8     Action;
	WSM_TSInfo_t TSInfo;
	UINT16  ReasonCode;
}PACK WSM_DELTS_Req_t;
#endif //QOS_WSM_FEATURE

typedef  struct 
{
	UINT8   Category;
	UINT8   Action;
	UINT8   DialogToken;
#ifdef QOS_WSM_FEATURE
	WSM_TSPEC_t TSpec;
#else
	IEEEtypes_TSPEC_t TSpec;
#endif
	union
	{
		TCLAS_t TCLAS;
#ifdef QOS_WSM_FEATURE
		WSM_TCLAS_Elem_t WSM_TCLAS_Elem;
#endif
	} TCLAS_u;
}PACK_END IEEEtypes_ADDTS_Req_t;



typedef  struct
{
	UINT16 aggr : 1; /*aggregation*/
	UINT16 TSID : 4;
	UINT16 dir : 2;
	UINT16 rsvd : 9;
}PACK_END Sched_Info_Field_t;

typedef  struct
{
	UINT8 ElementId;
	UINT8 Len;
	Sched_Info_Field_t sched_info;
	UINT32 serv_start_time;
	UINT32 serv_intrvl;
	UINT16 max_serv_duration;
	UINT16 spec_intrvl; /*specification inverval*/

}PACK_END IEEEtypes_Sched_Element_t;

typedef  struct
{
	UINT8 ElementId;
	UINT8 Len;
	OUI_t OUI;
	UINT8 version;
	Sched_Info_Field_t sched_info;
	UINT32 serv_start_time;
	UINT32 serv_intrvl;
	UINT16 spec_intrvl; /*specification inverval*/
}PACK_END WSM_Sched_Element_t;

typedef union
{
	IEEEtypes_Sched_Element_t   Schedule; //check size
	WSM_Sched_Element_t WSM_Schedule;
}Schedule_u;

typedef  struct 
{
	UINT8   Category;
	UINT8   Action;
	UINT8   DialogToken;
	UINT16   StatusCode;  //check size
	//  UINT8   TSDelay;     //check size
#ifdef QOS_WSM_FEATURE
	WSM_TSPEC_t TSpec;
#else
	IEEEtypes_TSPEC_t TSpec;
#endif
	union
	{
		Schedule_u   Schedule; //check size
		TCLAS_t TCLAS;
#ifdef QOS_WSM_FEATURE
		WSM_TCLAS_Elem_t WSM_TCLAS_Elem;
#endif
	} TCLAS_u;
	Schedule_u Schedule;
}PACK_END IEEEtypes_ADDTS_Rsp_t;

typedef  struct 
{
	UINT8     Category;
	UINT8     Action;
	IEEEtypes_TS_info_t TSInfo;
}PACK_END IEEEtypes_DELTS_Req_t;

typedef struct
{
	UINT16 amsdu:1;
	UINT16 BA_policy:1;
	UINT16 tid:4;
	UINT16 BufSize:10;

}PACK_END IEEEtypes_BA_ParamSet_t;

typedef struct 
{
	UINT16 FragNo:4;
	UINT16 Starting_Seq_No:12;
}PACK_END IEEEtypes_BA_Starting_Seq_Control_t;

typedef struct 
{
	UINT16 Resvd:11;
	UINT16 Initiator:1;
	UINT16 tid:4;

}PACK_END IEEEtypes_DELBA_ParamSet_t;

typedef  struct 
{
	UINT8     Category;
	UINT8     Action;
	UINT8   DialogToken;
	IEEEtypes_BA_ParamSet_t ParamSet;
	UINT16 Timeout_val;
	IEEEtypes_BA_Starting_Seq_Control_t SeqControl;
	// UINT16 SeqControl;
}PACK_END IEEEtypes_ADDBA_Req_t;

typedef  struct 
{
	UINT8     Category;
	UINT8     Action;
	UINT8   DialogToken;
	UINT16 ParamSet;
	UINT16 Timeout_val;
	// IEEEtypes_BA_Starting_Seq_Control_t SeqControl;
	UINT16 SeqControl;
}PACK_END IEEEtypes_ADDBA_Req_t2;

typedef  struct 
{
	UINT8     Category;
	UINT8     Action;
	UINT8   DialogToken;
	UINT16   StatusCode;  //check size
	IEEEtypes_BA_ParamSet_t ParamSet;
	UINT16  Timeout_val;
}PACK_END IEEEtypes_ADDBA_Rsp_t;

typedef  struct 
{
	UINT8     Category;
	UINT8     Action;
	IEEEtypes_DELBA_ParamSet_t ParamSet;
	UINT16    ReasonCode;
}PACK_END IEEEtypes_DELBA_t;

typedef  struct 
{
	UINT8     Category;
	UINT8     Action;
	IEEEtypes_MacAddr_t DstAddr;
	IEEEtypes_MacAddr_t SrcAddr;
	IEEEtypes_CapInfo_t macCapInfo; /* Save this from Start command */
	UINT16  Timeout_val;
	IEEEtypes_SuppRatesElement_t SuppRates;
}PACK_END IEEEtypes_DlpReq_t;

typedef  struct 
{
	UINT8     Category;
	UINT8     Action;
	UINT16   StatusCode;
	IEEEtypes_MacAddr_t DstAddr;
	IEEEtypes_MacAddr_t SrcAddr;
	IEEEtypes_CapInfo_t macCapInfo; /* Save this from Start command */
	UINT16  Timeout_val;
	IEEEtypes_SuppRatesElement_t SuppRates;
}PACK_END IEEEtypes_DlpResp_t;

typedef  struct 
{
	UINT8     Category;
	UINT8     Action;
	IEEEtypes_MacAddr_t DstAddr;
	IEEEtypes_MacAddr_t SrcAddr;
}PACK_END IEEEtypes_DlpTearDown_t;

typedef  struct
{
	UINT16 AckPolicy: 1;
	UINT16 MTID : 1;
	UINT16 CompressedBA : 1;
	UINT16 reserved: 9;
	UINT16 TID : 4;
}PACK_END BA_Cntrl_t;

typedef  struct 
{
	UINT16 FragNo : 4;
	UINT16 StartSeqNo  : 12;
}PACK_END Sequence_Cntrl_t;

typedef  struct 
{
	IEEEtypes_FrameCtl_t FrmCtl;
	UINT16 Duration;
	IEEEtypes_MacAddr_t DestAddr;
	IEEEtypes_MacAddr_t SrcAddr;
	UINT8 dummy[8+6];
	BA_Cntrl_t BA_Ctrl;
	Sequence_Cntrl_t Seq_Ctrl;
}PACK_END IEEEtypes_BA_ReqFrame_t2;

#ifdef IEEE80211_DH
typedef  struct
{
	UINT8     Category;
	UINT8     Action;
	IEEEtypes_ChannelSwitchAnnouncementElement_t   Csa ;
}PACK_END IEEEtypes_CSA_ACTION_t;
#endif //IEEE80211_DH

typedef  struct 
{
	BA_Cntrl_t BA_Ctrl;
	UINT16    SeqCtl;//starting seq control
}PACK_END IEEEtypes_BA_Req_Body_t;

typedef  struct 
{
	IEEEtypes_CtlHdr_t Hdr;
	IEEEtypes_BA_Req_Body_t Body;
	UINT32    FCS;
}PACK_END IEEEtypes_BA_ReqFrame_t;

typedef  struct 
{
	BA_Cntrl_t BA_Ctrl;
	UINT16    SeqCtl;//starting seq control
	UINT8     BitMap[128];
}PACK_END IEEEtypes_BA_Rsp_Body_t;

typedef  struct 
{
	IEEEtypes_CtlHdr_t Hdr;
	IEEEtypes_BA_Rsp_Body_t Body;
	UINT32    FCS;
}PACK_END IEEEtypes_BA_RspFrame_t;

typedef  struct
{
	UINT8 ElementId;
	UINT8 Len;
	UINT8 QoS_act;
	union
	{
		IEEEtypes_ADDTS_Req_t AddTSReq;
		IEEEtypes_ADDTS_Rsp_t AddTSRsp;
		IEEEtypes_DELTS_Req_t DelTSReq;

	}QoSAction_u;
}PACK_END IEEEtypes_QoSActElem_t; /*QoS action element*/

#endif
/* */
/* Association request message body */
/* */

typedef  struct IEEEtypes_AssocRsp_t
{
	IEEEtypes_CapInfo_t CapInfo;
	IEEEtypes_StatusCode_t StatusCode;
	IEEEtypes_AId_t AId;
	IEEEtypes_SuppRatesElement_t SuppRates;
	IEEEtypes_ExtSuppRatesElement_t ExtSuppRates;
#ifdef QOS_FEATURE_1
	IEEEtypes_QoSActElem_t QosActElem;
#endif
} PACK_END IEEEtypes_AssocRsp_t;
/* */
/* Association response message body */
/* */

typedef  struct IEEEtypes_ReassocRqst_t
{
	IEEEtypes_CapInfo_t CapInfo;
	IEEEtypes_ListenInterval_t ListenInterval;
	IEEEtypes_MacAddr_t CurrentApAddr;
	IEEEtypes_SsIdElement_t SsId;
	IEEEtypes_SuppRatesElement_t SuppRates;
#ifdef IEEE80211H
	IEEEtypes_PowerCapabilityElement_t PwrCap;
	IEEEtypes_SupportedChannelElement_t Channels;
#endif /* IEEE80211H */
	IEEEtypes_RSN_IE_t RsnIE;
	IEEEtypes_ExtSuppRatesElement_t ExtSuppRates;
} PACK_END IEEEtypes_ReassocRqst_t;
/* */
/* Reassociation request message body */
/* */

typedef  struct IEEEtypes_ReassocRsp_t
{
	IEEEtypes_CapInfo_t CapInfo;
	IEEEtypes_StatusCode_t StatusCode;
	IEEEtypes_AId_t AId;
	IEEEtypes_SuppRatesElement_t SuppRates;
	IEEEtypes_ExtSuppRatesElement_t ExtSuppRates;
} PACK_END IEEEtypes_ReassocRsp_t;
/* */
/* Reassociation response message body */
/* */

typedef  struct IEEEtypes_ProbeRqst_t
{
	IEEEtypes_SsIdElement_t SsId;
	IEEEtypes_SuppRatesElement_t SuppRates;
	IEEEtypes_ExtSuppRatesElement_t ExtSuppRates;
} PACK_END IEEEtypes_ProbeRqst_t;
/* */
/* Probe request message body */
/* */

typedef  struct IEEEtypes_ProbeRsp_t
{
	IEEEtypes_TimeStamp_t TimeStamp;
	IEEEtypes_BcnInterval_t BcnInterval;
	IEEEtypes_CapInfo_t CapInfo;
	IEEEtypes_SsIdElement_t SsId;
	IEEEtypes_SuppRatesElement_t SuppRates;
	IEEEtypes_PhyParamSet_t PhyParamSet;
	IEEEtypes_SsParamSet_t SsParamSet;
#ifdef AP_WPA2
	IEEEtypes_RSN_IE_t RsnIE;
	IEEEtypes_RSN_IE_WPA2_t RsnIEWPA2;
#endif
	IEEEtypes_Tim_t Tim;
	IEEEtypes_ERPInfoElement_t ERPInfo;
	IEEEtypes_ExtSuppRatesElement_t ExtSuppRates;

#ifdef IEEE80211H    
	IEEEtypes_COUNTRY_IE_t  Country;
	IEEEtypes_PowerConstraintElement_t PwrCons;
	IEEEtypes_ChannelSwitchAnnouncementElement_t ChSwAnn;
	IEEEtypes_QuietElement_t Quiet;
	IEEEtypes_TPCRepElement_t TPCRep;
#endif /* IEEE80211H */
} PACK_END IEEEtypes_ProbeRsp_t;
/* */
/* Probe response message body */
/* */

typedef  struct IEEEtypes_Auth_t
{
	IEEEtypes_AuthAlg_t AuthAlg;
	IEEEtypes_AuthTransSeq_t AuthTransSeq;
	IEEEtypes_StatusCode_t StatusCode;
	IEEEtypes_ChallengeText_t ChallengeText;
} PACK_END IEEEtypes_Auth_t;
/* */
/* Authentication message body */
/* */

typedef  struct IEEEtypes_Deauth_t
{
	IEEEtypes_ReasonCode_t ReasonCode;
} PACK_END IEEEtypes_Deauth_t;
/* */
/* Deauthentication message body */
/* */

#ifdef IEEE80211H
typedef enum
{
	SPECTRUM_MANAGEMENT = 0    
} IEEEtypes_Category_e;
typedef UINT8 IEEEtypes_Category_t;

typedef enum
{
	MEASUREMENT_REQUEST = 0,
	MEASUREMENT_REPORT,
	TPC_REQUEST,
	TPC_REPORT,
	CHANNEL_SWITCH_ANNOUNCEMENT
} IEEEtypes_ActionFieldType_e;
typedef UINT8 IEEEtypes_ActionFieldType_t;

typedef  struct IEEEtypes_ActionField_t
{      
	IEEEtypes_Category_t Category;
	IEEEtypes_ActionFieldType_t Action;
	UINT8 DialogToken; /* for coding issue, add extra byte for channel switch action frame */
	union
	{       
		IEEEtypes_MeasurementRequestElement_t MeasurementRequest[MAX_NR_IE];
		IEEEtypes_MeasurementReportElement_t MeasurementReport[MAX_NR_IE];
		IEEEtypes_TPCReqElement_t TPCRequest;
		IEEEtypes_TPCRepElement_t TPCReport;
		IEEEtypes_ChannelSwitchAnnouncementElement_t ChannelSwitchAnnouncement; 
	} Data;    
} PACK_END IEEEtypes_ActionField_t;

#ifdef SOC_W8764
typedef struct IEEEtypes_MimoControl_t
{
	UINT16 NcIndex      : 2;
	UINT16 NrIndex      : 2;
	UINT16 MimoBw       : 1;
	UINT16 GroupingNg   : 2;
	UINT16 CoeffSizeNb  : 2;
	UINT16 CodeBookInfo : 2;
	UINT16 RemMatrixSeg : 3;
	UINT16 Resvd        : 2;
	UINT32 SoundingTmStp;    
} PACK_END IEEEtypes_MimoControl_t;


typedef  struct IEEEtypes_CSIReport_t
{   
#if 0
	UINT32 resvd;
#endif
	IEEEtypes_Category_t Category;
	IEEEtypes_ActionFieldType_t Action;
	IEEEtypes_MimoControl_t Mimo;
	union
	{ 
		UINT8 Snr[64];
		UINT8 Data[64];
		UINT8 PhiLamda[64];
	} CSI;    
} PACK_END IEEEtypes_CSIReport_t;

typedef struct IEEEtypes_CompBeamReportCode0_t
{
	UINT8 psi : 1;
	UINT8 phi : 3;
} PACK_END IEEEtypes_CompBeamReportCode0_t;

typedef struct IEEEtypes_CompBeamReportCode1_t
{
	UINT8 psi : 2;
	UINT8 phi : 4;
} PACK_END IEEEtypes_CompBeamReportCode1_t;

typedef struct IEEEtypes_CompBeamReportCode2_t
{
	UINT8 psi : 3;
	UINT8 phi : 5;
} PACK_END IEEEtypes_CompBeamReportCode2_t;

typedef struct IEEEtypes_CompBeamReportCode3_t
{
	UINT32 psi : 4;
	UINT32 phi : 6;
} PACK_END IEEEtypes_CompBeamReportCode3_t;

#endif /* SOC_W8764 */
#endif /* IEEE80211H */

/*---------------------------------------------------------------------------*/
/*              IEEE 802.11 MLME SAP Interface Data Structures               */
/*                                                                           */
/* According to IEEE 802.11, services are provided by the MLME to the SME.   */
/* In the current architecture, the services are provided to the SME by the  */
/* MAC Management Service Task. This section describes data structures       */
/* needed for these services.                                                */
/*---------------------------------------------------------------------------*/

/*---------------------*/
/* BSS Description Set */
/*---------------------*/
#define STA_VENDOR_IE_BUF_MAX_LEN   384

typedef  struct IEEEtypes_BssDesc_t
{
	IEEEtypes_MacAddr_t BssId;
	IEEEtypes_SsId_t SsId;
	IEEEtypes_Bss_t BssType;
	IEEEtypes_BcnInterval_t BcnPeriod;
	IEEEtypes_DtimPeriod_t DtimPeriod;
	IEEEtypes_TimeStamp_t Tstamp;
	IEEEtypes_TimeStamp_t StartTs;
	IEEEtypes_PhyParamSet_t PhyParamSet;
	IEEEtypes_SsParamSet_t SsParamSet;
	IEEEtypes_CapInfo_t Cap;
	IEEEtypes_DataRate_t      DataRates[IEEEtypes_MAX_DATA_RATES_G];
	/* 11n related elements */
	IEEEtypes_HT_Element_t  HTElement;
	IEEEtypes_Add_HT_Element_t ADDHTElement;
	/*11ac related element*/
	IEEEtypes_VhtCap_t VHTCap;				
	IEEEtypes_VhOpt_t VHTOp;				
	/* RSN (WPA2) */
	IEEEtypes_RSN_IE_WPA2_t Wpa2Element;
	/* Vendor Specific IEs */
	UINT8                   vendorIENum;
	UINT8                   vendorTotalLen;
	UINT8                   vendorBuf[STA_VENDOR_IE_BUF_MAX_LEN];
	/* End Vendor Specific IEs */

} PACK_END IEEEtypes_BssDesc_t;
/* */
/* A description of a BSS, providing the following: */
/* BssId:        The ID of the BSS */
/* SsId:         The SSID of the BSS */
/* BssType:      The type of the BSS (INFRASTRUCTURE or INDEPENDENT) */
/* BcnPeriod:    The beacon period (in time units) */
/* DtimPeriod:   The DTIM period (in beacon periods) */
/* Tstamp:       Timestamp of a received frame from the BSS; this is an 8 */
/*                  byte string from a probe response or beacon */
/* StartTs:      The value of a station's timing synchronization function */
/*                  at the start of reception of the first octet of the */
/*                  timestamp field of a received frame (probe response or */
/*                  beacon) from a BSS; this is an 8 byte string */
/* PhyParamSet:  The parameter set relevant to the PHY (empty if not */
/*                  needed by the PHY) */
/* SsParamSet:   The service set parameters. These can consist of either */
/*                  the parameter set for CF periods or for an IBSS. */
/* Cap:          The advertised capabilities of the BSS */
/* DataRates:    The set of data rates that must be supported by all */
/*                  stations (the BSS basic rate set) */
/* */

typedef  struct IEEEtypes_BssDescSet_t
{
	UINT8 NumSets;
	IEEEtypes_BssDesc_t BssDesc[IEEEtypes_MAX_BSS_DESCRIPTS];
} PACK_END IEEEtypes_BssDescSet_t;
/* */
/* The set of BSS descriptions */
/* */

/*-------------------*/
/* MLME SAP Messages */
/*-------------------*/
#ifdef IEEE80211H
typedef struct IEEEtypes_RequestSet_t
{
	UINT8 MeasurementToken;
	IEEEtypes_MeasurementReqMode_t Mode;
	IEEEtypes_MeasurementReqType_t Type;
	IEEEtypes_MeasurementReq_t Request;    
} IEEEtypes_RequestSet_t;


typedef struct IEEEtypes_ReportSet_t
{
	UINT8 MeasurementToken;
	IEEEtypes_MeasurementRepMode_t Mode;
	IEEEtypes_MeasurementRepType_t Type;   
	IEEEtypes_MeasurementRep_t Report;    
} IEEEtypes_ReportSet_t;

/*
*  for request
*/

typedef  struct IEEEtypes_MRequestCmd_t
{
	IEEEtypes_MacAddr_t PeerStaAddr;
	UINT8 DiaglogToken;
	UINT8 MeasureItems;  /* number of IEs in MeasureReqSet */
	IEEEtypes_RequestSet_t MeasureReqSet[MAX_NR_IE];
} PACK_END IEEEtypes_MRequestCmd_t;

typedef  struct IEEEtypes_MRequestCfrm_t
{
	IEEEtypes_MRequestResult_t Result;    
} PACK_END IEEEtypes_MRequestCfrm_t;

typedef  struct IEEEtypes_MRequestInd_t
{
	IEEEtypes_MacAddr_t PeerStaAddr;
	UINT8 DiaglogToken;
	UINT8 RequestItems;  /* number of IEs in MeasureReqSet */
	IEEEtypes_RequestSet_t MeasureReqSet[MAX_NR_IE];
} PACK_END IEEEtypes_MRequestInd_t;

/*
*  for measure
*/

typedef  struct IEEEtypes_MeasureCmd_t
{
	IEEEtypes_MacAddr_t PeerStaAddr;
	UINT8 DiaglogToken;
	IEEEtypes_RequestSet_t MeasureReqSet[MAX_NR_IE];
} PACK_END IEEEtypes_MeasureCmd_t;

typedef  struct IEEEtypes_MeasureCfrm_t
{
	IEEEtypes_MeasureResult_t Result;
	UINT8 DiaglogToken;
	IEEEtypes_ReportSet_t MeasureReqSet[MAX_NR_IE];
} PACK_END IEEEtypes_MeasureCfrm_t;


/*
*  for report
*/

typedef  struct IEEEtypes_MReportCmd_t
{
	IEEEtypes_MacAddr_t PeerStaAddr;
	UINT8 DiaglogToken;
	UINT8 ReportItems;  /* number of IEs in MeasureReqSet */
	IEEEtypes_ReportSet_t MeasureRepSet[MAX_NR_IE];
} PACK_END IEEEtypes_MReportCmd_t;

typedef  struct IEEEtypes_MReportCfrm_t
{
	IEEEtypes_MReportResult_t Result;    
} PACK_END IEEEtypes_MReportCfrm_t;

typedef  struct IEEEtypes_MReportInd_t
{
	IEEEtypes_MacAddr_t PeerStaAddr;
	UINT8 DiaglogToken;
	UINT8 ReportItems;  /* number of IEs in MeasureReqSet */
	IEEEtypes_ReportSet_t MeasureRepSet[MAX_NR_IE];
} PACK_END IEEEtypes_MReportInd_t;


/*
*  for channel switch
*/
typedef  struct IEEEtypes_ChannelSwitchCmd_t
{
	UINT8 Mode;
	UINT8 ChannelNumber;
	UINT8 ChannelSwitchCount;
} PACK_END IEEEtypes_ChannelSwitchCmd_t;

typedef  struct IEEEtypes_ChannelSwitchCfrm_t
{
	IEEEtypes_ChannelSwitchResult_t Result;    
} PACK_END IEEEtypes_ChannelSwitchCfrm_t;

typedef  struct IEEEtypes_ChannelSwitchInd_t
{
	IEEEtypes_MacAddr_t PeerStaAddr;
	UINT8 Mode;
	UINT8 ChannelNumber;
	UINT8 ChannelSwitchCount;
} PACK_END IEEEtypes_ChannelSwitchInd_t;

typedef  struct IEEEtypes_ChannelSwitchResp_t
{
	UINT8 Mode;
	UINT8 ChannelNumber;
	UINT8 ChannelSwitchCount;
} PACK_END IEEEtypes_ChannelSwitchResp_t;

/*
*  for TPC adaptive
*/

typedef  struct IEEEtypes_TPCAdaptCmd_t
{
	IEEEtypes_MacAddr_t PeerStaAddr;
	UINT8 DiaglogToken;    
} PACK_END IEEEtypes_TPCAdaptCmd_t;

typedef  struct IEEEtypes_TPCAdaptCfrm_t
{
	IEEEtypes_TPCAdaptResult_t Result;    
} PACK_END IEEEtypes_TPCAdaptCfrm_t;

#endif /* IEEE80211H */


/*
Data structure to hold the Cipher Suites of AP's while scanning
*/
typedef struct
{
	UINT8 MulticastCipher[4];
	UINT8 UnicastCipher[4];
}WPA_AP_Ciphers_t;

typedef  struct IEEEtypes_PwrMgmtCmd_t
{
	IEEEtypes_PwrMgmtMode_t PwrMgmtMode;
	BOOLEAN                  WakeUp;
	BOOLEAN                  RcvDTIMs;
} PACK_END IEEEtypes_PwrMgmtCmd_t;
/* */
/* Power management request message from the SME */
/* */

typedef  struct IEEEtypes_PwrMgmtCfrm_t
{
	IEEEtypes_PwrMgmtResult_t Result;
} PACK_END IEEEtypes_PwrMgmtCfrm_t;
/* */
/* Power management confirm message sent from the MLME as a result */
/* of a power management request; it is sent after the change has */
/* taken place */
/* */

typedef  struct IEEEtypes_ScanCmd_t
{
	IEEEtypes_Bss_t BssType;
	IEEEtypes_MacAddr_t BssId;
	IEEEtypes_SsId_t SsId;
	IEEEtypes_Scan_t ScanType;
	UINT16 ProbeDelay;
	UINT8 ChanList[IEEEtypes_MAX_CHANNELS+IEEEtypes_MAX_CHANNELS_A];
	/*UINT8                Reserved; */
	UINT16 MinChanTime;
	UINT16 MaxChanTime;
} PACK_END IEEEtypes_ScanCmd_t;
/* */
/* Scan request message from the SME to determine if there are BSSs */
/* that can be joined */
/* */
/* Note: The "Reserved" field is inserted for alignment for */
/* commands coming from the host */
/* */

typedef  struct IEEEtypes_ScanCfrm_t
{
	IEEEtypes_BssDescSet_t BssDescSet;
	IEEEtypes_ScanResult_t Result;
#ifdef IEEE80211H
	IEEEtypes_COUNTRY_IE_t  Country;
#endif /* IEEE80211H */
} PACK_END IEEEtypes_ScanCfrm_t;
/* */
/* Scan confirm message sent from the MLME as a result of a scan request; */
/* it reports the results of the scan */
/* */

typedef  struct IEEEtypes_JoinCmd_t
{
	IEEEtypes_BssDesc_t BssDesc;
	UINT16 FailTimeout;
	UINT16 ProbeDelay;
#ifndef ERP
	IEEEtypes_DataRate_t OpRateSet[IEEEtypes_MAX_DATA_RATES];
#else   
	IEEEtypes_DataRate_t   OpRateSet[IEEEtypes_MAX_DATA_RATES_G];
#endif
} PACK_END IEEEtypes_JoinCmd_t;
/* */
/* Join request message from the SME to establish synchronization with */
/* a BSS */
/* */

typedef  struct IEEEtypes_JoinCfrm_t
{
	IEEEtypes_JoinResult_t Result;
} PACK_END IEEEtypes_JoinCfrm_t;
/* */
/* Join confirm message sent from the MLME as a result of a join request; */
/* it reports the result of the join */
/* */

typedef  struct IEEEtypes_AuthCmd_t
{
	IEEEtypes_MacAddr_t PeerStaAddr;
	UINT16 FailTimeout;
	IEEEtypes_AuthType_t AuthType;
} PACK_END IEEEtypes_AuthCmd_t;
/* */
/* Authenticate request message sent from the SME to establish */
/* authentication with a specified peer MAC entity */
/* */

typedef  struct IEEEtypes_AuthCfrm_t
{
	IEEEtypes_MacAddr_t PeerStaAddr;
	IEEEtypes_AuthType_t AuthType;
	IEEEtypes_AuthResult_t Result;
} PACK_END IEEEtypes_AuthCfrm_t;
/* */
/* Authenticate confirm message sent from the MLME as a result of an */
/* authenticate request; it reports the result of the authentication */
/* */

typedef  struct IEEEtypes_AuthInd_t
{
	IEEEtypes_MacAddr_t PeerStaAddr;
	IEEEtypes_AuthType_t AuthType;
} PACK_END IEEEtypes_AuthInd_t;
/* */
/* Authenticate indication message sent from the MLME to report */
/* authentication with a peer MAC entity that resulted from an */
/* authentication procedure that was initiated by that MAC entity */
/* */

typedef  struct IEEEtypes_DeauthCmd_t
{
	IEEEtypes_MacAddr_t PeerStaAddr;
	IEEEtypes_ReasonCode_t Reason;
} PACK_END IEEEtypes_DeauthCmd_t;
/* */
/* Deauthenticate request message sent from the SME to invalidate */
/* authentication with a specified peer MAC entity */
/* */

typedef  struct IEEEtypes_DeauthCfrm_t
{
	IEEEtypes_MacAddr_t PeerStaAddr;
	IEEEtypes_DeauthResult_t Result;
} PACK_END IEEEtypes_DeauthCfrm_t;
/* */
/* Deauthenticate confirm message sent from the MLME as a result of a */
/* deauthenticate request message; it reports the result of the */
/* deauthentication */
/* */

typedef  struct IEEEtypes_DeauthInd_t
{
	IEEEtypes_MacAddr_t PeerStaAddr;
	IEEEtypes_ReasonCode_t Reason;
} PACK_END IEEEtypes_DeauthInd_t;
/* */
/* Deauthentication indication message sent from the MLME to report */
/* invalidation of an authentication with a peer MAC entity; the message */
/* is generated as a result of an invalidation of the authentication */
/* */

typedef  struct IEEEtypes_AssocCmd_t
{
	IEEEtypes_MacAddr_t PeerStaAddr;
	IEEEtypes_SsId_t SsId;
	UINT16 FailTimeout;
	IEEEtypes_CapInfo_t CapInfo;
	IEEEtypes_ListenInterval_t ListenInterval;
#ifdef IEEE80211H
	IEEEtypes_SupportedChannelElement_t SupportedChannel;
#endif /* IEEE80211H */    
} PACK_END IEEEtypes_AssocCmd_t;
/* */
/* Association request message sent from the SME to establish an */
/* association with an AP */
/* */

typedef  struct IEEEtypes_AssocCfrm_t
{
	IEEEtypes_AssocResult_t Result;
} PACK_END IEEEtypes_AssocCfrm_t;
/* */
/* Association confirm message sent from the MLME as a result of an */
/* association request message; it reports the result of the assoication */
/* */

typedef  struct IEEEtypes_AssocInd_t
{
	IEEEtypes_MacAddr_t PeerStaAddr;
} PACK_END IEEEtypes_AssocInd_t;
/* */
/* Association indication message sent from the MLME to report an */
/* association with a specified peer MAC entity acting as an AP; the */
/* indication is the result of an association procedure that was */
/* initiated by the peer MAC entity */
/* */

typedef  struct IEEEtypes_ReassocCmd_t
{
	IEEEtypes_MacAddr_t NewApAddr;
	IEEEtypes_SsId_t SsId;
	UINT16 FailTimeout;
	IEEEtypes_CapInfo_t CapInfo;
	IEEEtypes_ListenInterval_t ListenInterval;
#ifdef IEEE80211H
	IEEEtypes_SupportedChannelElement_t SupportedChannel;
#endif /* IEEE80211H */    
} PACK_END IEEEtypes_ReassocCmd_t;
/* */
/* Reassociation request message sent from the SME to change association */
/* to a specified new peer MAC entity acting as an AP */
/* */

typedef  struct IEEEtypes_ReassocCfrm_t
{
	IEEEtypes_ReassocResult_t Result;
} PACK_END IEEEtypes_ReassocCfrm_t;
/* */
/* Reassociation confirm message sent from the MLME as the result of a */
/* reassociate request message; it reports the result of the reassociation */
/* */

typedef  struct IEEEtypes_ReassocInd_t
{
	IEEEtypes_MacAddr_t PeerStaAddr;
} PACK_END IEEEtypes_ReassocInd_t;
/* */
/* Reassociate indication message sent from the MLME to report a */
/* reassociation with a specified peer MAC entity; the */
/* indication is the result of a reassociation procedure that was */
/* initiated by the peer MAC entity */
/* */

typedef  struct IEEEtypes_DisassocCmd_t
{
	IEEEtypes_MacAddr_t PeerStaAddr;
	IEEEtypes_ReasonCode_t Reason;
} PACK_END IEEEtypes_DisassocCmd_t;
/* */
/* Disassociate request message sent from the SME to establish */
/* disassociation with an AP */
/* */

typedef  struct IEEEtypes_DisassocCfrm_t
{
	IEEEtypes_DisassocResult_t Result;
} PACK_END IEEEtypes_DisassocCfrm_t;
/* */
/* Disassociate confirm message sent from the MLME as a result of a */
/* disassociate request message; it reports the result of the */
/* disassociation */
/* */

typedef  struct IEEEtypes_DisassocInd_t
{
	IEEEtypes_MacAddr_t PeerStaAddr;
	IEEEtypes_ReasonCode_t Reason;
} PACK_END IEEEtypes_DisassocInd_t;
/* */
/* Disassociate indication message sent from the MLME to report the */
/* invalidation of an association relationship with a peer MAC entity; */
/* the message is generated as a result of an invalidation of an */
/* association relationship */
/* */

typedef  struct IEEEtypes_ResetCmd_t
{
	IEEEtypes_MacAddr_t StaAddr;
	BOOLEAN SetDefaultMIB;
	BOOLEAN              quiet;
	UINT32   mode;
} PACK_END IEEEtypes_ResetCmd_t;
/* */
/* Reset request message sent from the SME to reset the MAC to initial */
/* conditions; the reset must be used prior to a start command */
/* */

typedef  struct IEEEtypes_ResetCfrm_t
{
	IEEEtypes_ResetResult_t Result;
} PACK_END IEEEtypes_ResetCfrm_t;
/* */
/* Reset confirm message sent from the MLME as a result of a reset */
/* request message; it reports the result of the reset */
/* */

typedef  struct IEEEtypes_StartCmd_t
{
	IEEEtypes_SsId_t SsId;
	IEEEtypes_Bss_t BssType;
	IEEEtypes_BcnInterval_t BcnPeriod;
	IEEEtypes_DtimPeriod_t DtimPeriod;
	IEEEtypes_SsParamSet_t SsParamSet;
	IEEEtypes_PhyParamSet_t PhyParamSet;
	UINT16 ProbeDelay;
	IEEEtypes_CapInfo_t CapInfo;
	IEEEtypes_DataRate_t BssBasicRateSet[IEEEtypes_MAX_DATA_RATES_G];
	IEEEtypes_DataRate_t OpRateSet[IEEEtypes_MAX_DATA_RATES_G];
	IEEEtypes_DataRate_t BssBasicRateSet2[IEEEtypes_MAX_DATA_RATES_G];
	IEEEtypes_DataRate_t OpRateSet2[IEEEtypes_MAX_DATA_RATES_G];
	IEEEtypes_SsId_t SsId2;
#ifdef IEEE80211H
	IEEEtypes_COUNTRY_IE_t  Country;
#endif /* IEEE80211H */
} PACK_END IEEEtypes_StartCmd_t;
/* */
/* Start request message sent from the SME to start a new BSS; the BSS */
/* may be either an infrastructure BSS (with the MAC entity acting as the */
/* AP) or an independent BSS (with the MAC entity acting as the first */
/* station in the IBSS) */
/* */

typedef  struct IEEEtypes_StartCfrm_t
{
	IEEEtypes_StartResult_t Result;
}PACK_END IEEEtypes_StartCfrm_t;
/* */
/* Start confirm message sent from the MLME as a result of a start request */
/* message; it reports the results of the BSS creation procedure */
/* */

typedef  struct IEEEtypes_Frame_t
{
	IEEEtypes_GenHdr_t Hdr;
	UINT8 Body[8];
} PACK_END IEEEtypes_Frame_t;

#ifdef QOS_FEATURE
typedef  struct
{
	UINT16 tid : 4;
	UINT16 eosp : 1;
	UINT16 ack_policy : 2;
	UINT16 amsdu :1;
	UINT16 var_data:8; /*signifies TXOP limit or TXOP duration request or Q size*/
}PACK_END IEEEtypes_QoS_Ctl_t;

typedef  struct
{
	UINT8 ElementId;
	UINT8 Len;
	UINT32 delay;
}PACK_END IEEEtypes_TS_Delay_t;

typedef  struct
{
	UINT8 ElementId;
	UINT8 Len;
	OUI_t OUI;
	UINT8 version;
	UINT32 delay;
}PACK_END WSM_TS_Delay_t;

typedef union
{
	IEEEtypes_TS_Delay_t TsDealy; //check size
	WSM_TS_Delay_t WSM_TsDealy;
}TS_Delay_u;
#ifdef STA_QOS
typedef enum
{
	NORMAL_ACK,
	NO_ACK,
	NO_EXPLICIT_ACK,
	BLCK_ACK
}IEEEtypes_AckPolicy_e;

typedef enum
{
	UPLINK,
	DOWNLINK,
	DIRECTLINK,
	BIDIRLINK
}Direction_e;

typedef enum
{
	TS_SETUP_EVT,
	TS_SETUP_THRU_ASSOCREQ_EVT,
	TS_SETUP_TIMEOUT_EVT,
	ADDTSRSP_EVT,
	DEL_TS_EVT
}TS_Setup_e;

typedef enum
{
	ADDBA_REQ,
	ADDBA_RESP,
	DELBA
}IEEEtypes_BA_Act_e;

typedef enum
{
	DLP_REQ,
	DLP_RESP,
	DLP_TEAR_DOWN
}IEEEtypes_DLP_Act_e;


typedef enum
{
	VHT_COMPRESSED_BF,
	GROUP_ID_MGMT,
	OPERATING_MODE_NOTIFICATION
}IEEEtypes_VHT_Act_e;


typedef enum
{
	None,
	QoS,//Traffic Stream Setup
	DLP,//Direct link Protocol
	BlkAck, //Block Ack
	VHT = 21					
}IEEEtypes_QoS_Category_e;

typedef enum
{
	DELAYED,
	IMMEDIATE
}IEEEtypes_QoS_BA_Policy_e;
typedef enum
{
	RSVD,
	EDCA,
	HCCA,
	BOTH
}IEEEtypes_QoS_Access_Policy_e;

typedef enum
{
	WSM_CAPABILITY = 5,
	WSM_TCLAS,
	WSM_TCLAS_PROCESSING,
	WSM_TS_DELAY,
	WSM_SCHED,
	WSM_ACTN_HDR
}WSM_OUI_SubType_e;

typedef  struct
{
	UINT16 rsvd : 12;
	UINT16 TID : 4;
}PACK_END IEEEtypes_BA_Cntrl_t;

typedef  struct
{
	IEEEtypes_FrameCtl_t FrmCtl;
	UINT16 DurationId;
	IEEEtypes_MacAddr_t RA;
	IEEEtypes_MacAddr_t TA;
	IEEEtypes_BA_Cntrl_t BA_Cntrl;
	UINT16 Start_Seq_Cntrl;
	UINT8 bitmap[128];
}PACK_END IEEEtypes_Block_Ack_t;

typedef  struct
{
	IEEEtypes_FrameCtl_t FrmCtl;
	UINT16 DurationId;
	IEEEtypes_MacAddr_t RA;
	IEEEtypes_MacAddr_t TA;
	IEEEtypes_BA_Cntrl_t BAR_Cntrl;
	UINT16 Start_Seq_Cntrl;
}PACK_END IEEEtypes_Block_Ack_Req_t;


typedef  struct
{
	UINT8 ElementId;
	UINT8 Len;
	UINT16 sta_cnt;
	UINT8 channel_util; /*channel utilization*/
	UINT16 avail_admit_cap; /*available admission capacity*/
}PACK_END QBSS_load_t;
typedef  struct
{
	UINT8 ElementId;
	UINT8 Len;
	UINT32 delay;
}PACK_END TS_delay_t;

typedef  struct
{
	UINT8 ElementId;
	UINT8 Len;
	UINT8 processing;

}PACK_END TCLAS_Processing_t;

typedef  struct
{
	UINT8 ElementId;
	UINT8 Len;
	QoS_Info_t QoS_info;

}PACK_END QoS_Cap_Elem_t;

typedef enum
{
	ADDTS_REQ,
	ADDTS_RSP,
	DELTS,
	QOS_SCHEDULE
}IEEEtypes_QoS_Act_e;

typedef enum
{
	ADDTS_REQ,
	ADDTS_RSP,
	DELTS,
	QOS_SCHEDULE
}IEEEtypes_QoS_Act_e;

#endif
#endif //QOS_FEATURE

typedef  struct 
{
	UINT32 LegacyRateBitMap;
	UINT32 HTRateBitMap;
	IEEEtypes_CapInfo_t CapInfo;
	IEEEtypes_HT_Cap_t HTCapabilitiesInfo;
	UINT8 MacHTParamInfo;
	UINT8 MrvlSta;
	IEEEtypes_Add_HT_INFO_t AddHtInfo;
#if 0 /* wlan-v5-sc2 merges: TODO LATER - FIRMWARE DEPENDENT */
	UINT8 StaMode;
	UINT8 IeeeRate;
	UINT8 Reserved[2];
#endif 
#ifdef EXPLICIT_BF
#if 0
	IEEEtypes_TXBF_Cap_t TxBFCapabilities;
#else
         	UINT32 TxBFCapabilities;
#endif
#endif
#if defined(SOC_W8864)
    UINT32 vht_MaxRxMcs;							
    UINT32 vht_cap;					
    UINT8 vht_RxChannelWidth;		//0:20Mhz, 1:40Mhz, 2:80Mhz, 3:160 or 80+80Mhz
#endif
}PACK_END PeerInfo_t;

#define SPECTRUM_MANAGE_CATEGOTY	0
#define ACTION_EXTCHANSWTANNO	4
#define RADIO_MEASURE_CATEGOTY 		5		
#define HT_CATEGORY 				7
#define ACTION_SMPS					1
#define ACTION_PSMP					2
#define ACTION_PCOPHA				3
#define ACTION_MIMO_CSI_REPORT		4
#define ACTION_MIMO_NONCOMP_REPORT	5
#define ACTION_MIMO_COMP_REPORT	    6
#define ACTION_INFOEXCH			8
#ifdef COEXIST_20_40_SUPPORT
#define ACTION_NOTIFYCHANNELWIDTH 0
#define ACTION_PUBLIC                      4 
#endif
typedef  struct
{
#ifdef IEEE80211N_MIMOPSD110
	UINT8 Enable : 1;
	UINT8 Mode : 1;
	UINT8 Rev : 6;
#else
	UINT8     Enable;
	UINT8     Mode;  
#endif
}PACK_END IEEEtypes_SM_PwrCtl_t;
#ifdef COEXIST_20_40_SUPPORT
typedef  struct
{
	UINT8 BandWidth;

}PACK_END IEEEtypes_BWCtl_t;
#endif

typedef  struct 
{
	UINT8 InfoReq : 1;
	UINT8 FortyMIntolerant : 1;
	UINT8 ChWd : 1;    
	UINT8 Rev : 5;
}PACK_END IEEEtypes_InfoExch_t;


#if 1//def INTOLERANT40
typedef  struct IEEEtypes_MeasurementReqBcn_t
{    
	UINT8 RegClass;
	UINT8 ChanNum;
	UINT16 RandInterval;
	UINT16 Duration;   
	UINT8 Mode;
	UINT8 BSSID[6];
	UINT8 ReportCondi;
	UINT8 Threshold_offset;
	UINT8 SSID[34];
} PACK_END IEEEtypes_MeasurementReqBcn_t;

typedef  struct IEEEtypes_MeasurementRequestEL_t
{
	IEEEtypes_ElementId_t ElementId;
	IEEEtypes_Len_t Len;   
	UINT8 Token;
	IEEEtypes_MeasurementReqMode_t Mode;
	IEEEtypes_MeasurementReqType_t Type;
	IEEEtypes_MeasurementReqBcn_t Request;
} PACK_END IEEEtypes_MeasurementRequestEl_t;

typedef  struct IEEEtypes_ExtendChanSwitchAnnounceEl_t
{
	IEEEtypes_ElementId_t ElementId;
	IEEEtypes_Len_t Len;   
	UINT8 Token;
	UINT8 ChanSwitchMode;
	UINT8 RegClass;
	UINT8 ChanNum;
	UINT8 ChanSwitchCount;
} PACK_END IEEEtypes_ExtendChanSwitchAnnounceEl_t;

/* This is for CateGory 5 struct */
typedef  struct IEEEtypes_ManageActionFieldC5_t
{      
	UINT8 DialogToken; /* for coding issue, add extra byte for channel switch action frame */
	UINT16 NumRepetition;
	union
	{       
		IEEEtypes_MeasurementRequestEl_t MeasurementRequestEl;
	} Data;    
} PACK_END IEEEtypes_ManageActionFieldC5_t;

typedef  struct IEEEtypes_ManageActionField_t
{      
	IEEEtypes_Category_t Category;
	IEEEtypes_ActionFieldType_t Action;
	union
	{       
		IEEEtypes_ManageActionFieldC5_t Field5;
		IEEEtypes_ExtendChanSwitchAnnounceEl_t ExtendChanSwitchAnnounceEl;
		IEEEtypes_SM_PwrCtl_t SmPwrCtl;
		IEEEtypes_InfoExch_t InfoExch;
#ifdef COEXIST_20_40_SUPPORT
		IEEEtypes_BWCtl_t BWCtl;
#endif

	} Field;    
} PACK_END IEEEtypes_ManageActionField_t;

typedef  struct IEEEtypes_ChannelReportEL_t
{
	IEEEtypes_ElementId_t ElementId;
	IEEEtypes_Len_t Len;   
	UINT8 RegClass;
	UINT8 ChanList[IEEEtypes_MAX_CHANNELS+IEEEtypes_MAX_CHANNELS_A];
} PACK_END IEEEtypes_ChannelReportEL_t;
#endif


#ifdef AP_WPA2
#define MAX_SIZE_RSN_IE_BUF 64  /* number of bytes */
#else
#define MAX_SIZE_RSN_IE_BUF 32  /* number of bytes */
#endif

#ifdef MRVL_WSC
typedef struct {
UINT16 ID;
UINT16 Len;
UINT8 Version;
}PACK WSC_Version_Attribute_t;

typedef struct {
UINT16 ID;
UINT16 Len;
UINT8 ResponseType;
}PACK WSC_ResponseType_Attribute_t;


typedef struct {
UINT16 ID;
UINT16 Len;
UINT8 VendorID[3];
}PACK WSC_VendorExtn_Attribute_t;

typedef struct {
UINT8 ID;
UINT8 Len;
UINT8 Version2;
}PACK WSC_Version2_VendorExtn_t;

typedef struct
{
	UINT8 ElementId;
	UINT8 Len;
	UINT8 OUI[WSC_OUI_LENGTH];
	UINT8 WSCData[WSC_BEACON_IE_MAX_LENGTH];
}PACK WSC_BeaconIE_t;

typedef struct
{
	UINT8 ElementId;
	UINT8 Len;
	UINT8 OUI[WSC_OUI_LENGTH];
	UINT8 WSCData[WSC_PROBERESP_IE_MAX_LENGTH];
}PACK WSC_ProbeRespIE_t;

typedef union
{
	WSC_BeaconIE_t beaconIE ;
	WSC_ProbeRespIE_t probeRespIE ;
}PACK WSC_COMB_IE_t;

typedef struct
{
	UINT16 ElementId;
	UINT16 Len;
}PACK_END WSC_HeaderIE_t;

typedef struct 
{
	UINT8 ElementId;
	UINT8 Len;
    UINT8 WSC_OUI[WSC_OUI_LENGTH];
    WSC_Version_Attribute_t Version;
    WSC_ResponseType_Attribute_t ResponseType;
    WSC_VendorExtn_Attribute_t VendorExtn;
    WSC_Version2_VendorExtn_t Version2;
}PACK_END AssocResp_WSCIE_t;
#endif //MRVL_WSC

#ifdef MRVL_WAPI
#define WAPI_BEACON_IE_MAX_LENGTH        68
#define WAPI_PROBERESP_IE_MAX_LENGTH     251
typedef struct
{
	UINT16 Len;
	UINT8 WAPIData[WAPI_BEACON_IE_MAX_LENGTH];
}PACK WAPI_BeaconIEs_t;

typedef struct
{
	UINT16 Len;
	UINT8 WAPIData[WAPI_PROBERESP_IE_MAX_LENGTH];
}PACK WAPI_ProbeRespIEs_t;

typedef union
{
	WAPI_BeaconIEs_t beaconIE ;
	WAPI_ProbeRespIEs_t probeRespIE ;
}PACK WAPI_COMB_IE_t;
#endif // WAPI

#ifdef MV_CPU_BE
#define	IEEE_ETHERTYPE_PAE	0x888e		/* EAPOL PAE/802.1x */
#define IEEE_QOS_CTL_AMSDU 0x8000
#define ETH_P_WAPI  0x88B4
#else
#define IEEE_QOS_CTL_AMSDU 0x80
#define	IEEE_ETHERTYPE_PAE	0x8e88		/* EAPOL PAE/802.1x */
#define ETH_P_WAPI  0xB488
#endif


typedef struct
{
	IEEEtypes_MacAddr_t da;
	IEEEtypes_MacAddr_t sa;
	UINT16 type;
}
PACK_END ether_hdr_t;
#endif /* _IEEE_TYPES_H_ */


