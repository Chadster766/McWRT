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

#ifndef _EVENT_H_
#define _EVENT_H_ 

/*!
 * \file    event.h
 * \brief   This file contains the function prototypes and definitions for the
 *          generating Event messages
 *
 */

#include "wltypes.h"

#define EVT_PHY_IDX_NOT_APPLICABLE     0xFF
#define EVT_PHY_IDX_NOT_AVAILABLE      0xFE
#define EVT_PHY_IDX_ALL                0xFD

#define EVT_BSS_IDX_NOT_APPLICABLE     0xFF
#define EVT_BSS_IDX_NOT_AVAILABLE      0xFE
#define EVT_BSS_IDX_ALL                0xFD

// Message types for WPA/WPA2 handshake related events
typedef enum 
{
    RSN_PWK_MSG_1 = 1,
    RSN_PWK_MSG_2,
    RSN_PWK_MSG_3,
    RSN_PWK_MSG_4,
    RSN_GRP_MSG_1,
    RSN_GRP_MSG_2,

} RSN_MSG_TYPE;
typedef UINT8 RSN_MSG_TYPE_t;

// Event Types

#define APEVT_RST_BTN_PRESSED           1
#define APEVT_UTIL_BTN_PRESSED         (1 << 1 )
#define APEVT_ETH_PORT_STATUS_CHANGE   (1 << 2 )
#define APEVT_STA_ASSOC                (1 << 3 )
#define APEVT_STA_AUTH                 (1 << 4 )
#define STAEVT_ASSOC_WITH_AP           (1 << 5 )
#define STAEVT_AUTH_WITH_AP            (1 << 6 )
#define EVT_CLIENT                     (1 << 7 )
#define EVT_AP                         (1 << 8 )
#define EVT_SWITCH                     (1 << 9 )
#define EVT_SYSTEM                     (1 << 10)
#define EVT_DEBUG                      (1 << 11)

//------------------------------
// Sub-types for APEVT_STA_ASSOC

#define APEVT_ASSOC_SUBTYPE_REFUSE           1
#define APEVT_ASSOC_SUBTYPE_REASSOC_REFUSE  (1 << 1 )
#define APEVT_ASSOC_SUBTYPE_DISASSOC_RECV   (1 << 2 )
#define APEVT_ASSOC_SUBTYPE_DISASSOC_SEND   (1 << 3 )
#define APEVT_ASSOC_SUBTYPE_ASSOCIATED      (1 << 4 )

// Buffers for APEVT_STA_ASSOC sub-types

typedef PACK_START struct _ap_assoc_refuse
{
    UINT8   phyIndex;
    UINT8   bssIndex;
    UINT16  reserved;
    UINT32  reasonCode;
    UINT8   staMacAddr[6];
}
PACK_END EVTBUF_APEVT_ASSOC_SUBTYPE_REFUSE;

typedef PACK_START struct _ap_reassoc_refuse
{
    UINT8   phyIndex;
    UINT8   bssIndex;
    UINT16  reserved;
    UINT32  reasonCode;
    UINT8   staMacAddr[6];
}
PACK_END EVTBUF_APEVT_ASSOC_SUBTYPE_REASSOC_REFUSE;

typedef PACK_START struct _ap_disassoc_recv
{
    UINT8   phyIndex;
    UINT8   bssIndex;
    UINT16  reserved;
    UINT32  reasonCode;
    UINT8   staMacAddr[6];
}
PACK_END EVTBUF_APEVT_ASSOC_SUBTYPE_DISASSOC_RECV;

typedef PACK_START struct _ap_disassoc_send
{
    UINT8   phyIndex;
    UINT8   bssIndex;
    UINT16  reserved;
    UINT32  reasonCode;
    UINT8   staMacAddr[6];
}
PACK_END EVTBUF_APEVT_ASSOC_SUBTYPE_DISASSOC_SEND;

typedef PACK_START struct _ap_associated
{
    UINT8   phyIndex;
    UINT8   bssIndex;
    UINT8   staMacAddr[6];
}
PACK_END EVTBUF_APEVT_ASSOC_SUBTYPE_ASSOCIATED;


//-----------------------------
// Sub-types for APEVT_STA_AUTH

#define APEVT_AUTH_SUBTYPE_REFUSE         1
#define APEVT_AUTH_SUBTYPE_DEAUTH_RECV   (1 << 1 )
#define APEVT_AUTH_SUBTYPE_DEAUTH_SEND   (1 << 2 )

// Buffers for APEVT_STA_AUTH sub-types

typedef PACK_START struct _ap_auth_refuse
{
    UINT8   phyIndex;
    UINT8   bssIndex;
    UINT16  reserved;
    UINT32  reasonCode;
    UINT8   staMacAddr[6];
}
PACK_END EVTBUF_APEVT_AUTH_SUBTYPE_REFUSE;

typedef PACK_START struct _ap_auth_deauth_recv
{
    UINT8   phyIndex;
    UINT8   bssIndex;
    UINT16  reserved;
    UINT32  reasonCode;
    UINT8   staMacAddr[6];
}
PACK_END EVTBUF_APEVT_AUTH_SUBTYPE_DEAUTH_RECV;

typedef PACK_START struct _ap_auth_deauth_send
{
    UINT8   phyIndex;
    UINT8   bssIndex;
    UINT16  reserved;
    UINT32  reasonCode;
    UINT8   staMacAddr[6];
}
PACK_END EVTBUF_APEVT_AUTH_SUBTYPE_DEAUTH_SEND;

//-----------------------------------
// Sub-types for STAEVT_ASSOC_WITH_AP

#define STAEVT_ASSOC_SUBTYPE_DISASSOC_RECV   1
#define STAEVT_ASSOC_SUBTYPE_DISASSOC_SEND  (1 << 1 )

// Buffers for STAEVT_ASSOC_WITH_AP sub-types

typedef PACK_START struct _sta_disassoc_recv
{
    UINT8   phyIndex;
    UINT8   bssIndex;
    UINT16  reserved;
    UINT32  reasonCode;
    UINT8   apMacAddr[6];
    UINT8   staMacAddr[6];
}
PACK_END EVTBUF_STAEVT_ASSOC_SUBTYPE_DISASSOC_RECV;

typedef PACK_START struct _sta_disassoc_send
{
    UINT8   phyIndex;
    UINT8   bssIndex;
    UINT16  reserved;
    UINT32  reasonCode;
    UINT8   apMacAddr[6];
    UINT8   staMacAddr[6];
}
PACK_END EVTBUF_STAEVT_ASSOC_SUBTYPE_DISASSOC_SEND;

//----------------------------------
// Sub-types for STAEVT_AUTH_WITH_AP

#define STAEVT_AUTH_SUBTYPE_DEAUTH_RECV   1
#define STAEVT_AUTH_SUBTYPE_DEAUTH_SEND  (1 << 1 )

// Buffers for STAEVT_AUTH_WITH_AP sub-types

typedef PACK_START struct _sta_deauth_recv
{
    UINT8   phyIndex;
    UINT8   bssIndex;
    UINT16  reserved;
    UINT32  reasonCode;
    UINT8   apMacAddr[6];
    UINT8   staMacAddr[6];
}
PACK_END EVTBUF_STAEVT_AUTH_SUBTYPE_DEAUTH_RECV;

typedef PACK_START struct _sta_deauth_send
{
    UINT8   phyIndex;
    UINT8   bssIndex;
    UINT16  reserved;
    UINT32  reasonCode;
    UINT8   apMacAddr[6];
    UINT8   staMacAddr[6];
}
PACK_END EVTBUF_STAEVT_AUTH_SUBTYPE_DEAUTH_SEND;

//--------------------
// Sub-type for EVT_AP

#define EVT_AP_SUBTYPE_AGE_OUT                  1
#define EVT_AP_SUBTYPE_PROTECTION_STATUS       (1 << 1 )
#define EVT_AP_SUBTYPE_SHORT_SLOT_TIME_STATUS  (1 << 2 )
#define EVT_AP_SUBTYPE_SHORT_CWMIN_STATUS      (1 << 3 )
#define EVT_AP_SUBTYPE_BSS_STARTED             (1 << 4 )
#define EVT_AP_SUBTYPE_RSN_SECURED             (1 << 5 )
#define EVT_AP_SUBTYPE_RSN_FAIL_MIC_DIFF       (1 << 6 )

// Buffers for EVT_AP sub-types

typedef PACK_START struct _ap_age_out
{
    UINT8   phyIndex;
    UINT8   bssIndex;
    UINT8   agedMacAddr[6];
}
PACK_END EVTBUF_EVT_AP_SUBTYPE_AGE_OUT;

typedef PACK_START struct _ap_protection_status
{
    UINT8   phyIndex;
    UINT8   bssIndex;
    UINT16  protectionStatus;
}
PACK_END EVTBUF_EVT_AP_SUBTYPE_PROTECTION_STATUS;

typedef PACK_START struct _ap_sst_status
{
    UINT8   phyIndex;
    UINT8   bssIndex;
    UINT16  sstStatus;
}
PACK_END EVTBUF_EVT_AP_SUBTYPE_SST_STATUS;

typedef PACK_START struct _ap_cwmin_status
{
    UINT8   phyIndex;
    UINT8   bssIndex;
    UINT16  cwminStatus;
}
PACK_END EVTBUF_EVT_AP_SUBTYPE_CWMIN_STATUS;

typedef PACK_START struct _ap_bss_started
{
    UINT8   phyIndex;
}
PACK_END EVTBUF_EVT_AP_SUBTYPE_BSS_STARTED;

typedef PACK_START struct _ap_rsn_secured
{
    UINT8 phyIndex;
    UINT8 bssIndex;
    UINT8 supplicantMACAddr[6];
}
PACK_END EVTBUF_EVT_AP_SUBTYPE_RSN_SECURED;

typedef PACK_START struct _ap_rsn_fail_mic_diff
{
    UINT8        phyIndex;
    UINT8        bssIndex;
    UINT8        supplicantMACAddr[6];
    RSN_MSG_TYPE_t messageType;
}
PACK_END EVTBUF_EVT_AP_SUBTYPE_RSN_FAIL_MIC_DIFF;


//-------------------------
// Sub-types for EVT_CLIENT

#define EVT_CLIENT_SUBTYPE_DISCONNECT                     1
#define EVT_CLIENT_SUBTYPE_JOIN                          (1 << 1)
#define EVT_CLIENT_SUBTYPE_SCAN_COMPLETE                 (1 << 2)
#define EVT_CLIENT_SUBTYPE_IBSS_STARTED                  (1 << 3)
#define EVT_CLIENT_SUBTYPE_INCOMPATIBLE_PEER_DETECTED    (1 << 4)
#define EVT_CLIENT_SUBTYPE_RSN_SECURED                   (1 << 5)
#define EVT_CLIENT_SUBTYPE_RSN_FAIL_MIC_DIFF             (1 << 6)
#define EVT_CLIENT_SUBTYPE_RSN_FAIL_ANONCE_DIFF          (1 << 7)

// Buffers for EVT_CLIENT sub-types

typedef PACK_START struct _client_disconnect
{
    UINT8   phyIndex;
}
PACK_END EVTBUF_EVT_CLIENT_SUBTYPE_DISCONNECT;

typedef PACK_START struct _client_join
{
    UINT8   phyIndex;
    UINT8   bssIndex;
    UINT8   clientMACAddr[6];
    UINT8   apMACAddr[6];
    UINT16  assocId;
    UINT32  joinStatus;
    UINT32  procType;   /* 0=Auth; 1 = Assoc */
}
PACK_END EVTBUF_EVT_CLIENT_SUBTYPE_JOIN;

typedef PACK_START struct _client_scan_complete
{
    UINT8   phyIndex;
}
PACK_END EVTBUF_EVT_CLIENT_SUBTYPE_SCAN_COMPLETE;

typedef PACK_START struct _client_ibss_started
{
    UINT8   phyIndex;
}
PACK_END EVTBUF_EVT_CLIENT_SUBTYPE_IBSS_STARTED;

typedef PACK_START struct _client_incompatible_peer_detected
{
    UINT8   phyIndex;
    UINT8   bssId[6];
}
PACK_END EVTBUF_EVT_CLIENT_SUBTYPE_INCOMPATIBLE_PEER_DETECTED;

typedef PACK_START struct _client_rsn_secured
{
    UINT8 phyIndex;
    UINT8 bssIndex;
    UINT8 authenticatorMACAddr[6];
    UINT8 supplicantMACAddr[6];
}
PACK_END EVTBUF_EVT_CLIENT_SUBTYPE_RSN_SECURED;

typedef PACK_START struct _client_rsn_fail_mic_diff
{
    UINT8        phyIndex;
    UINT8        bssIndex;
    UINT8        authenticatorMACAddr[6];
    UINT8        supplicantMACAddr[6];
    RSN_MSG_TYPE_t messageType;
}
PACK_END EVTBUF_EVT_CLIENT_SUBTYPE_RSN_FAIL_MIC_DIFF;

typedef PACK_START struct _client_rsn_fail_anonce_diff
{
    UINT8         phyIndex;
    UINT8         bssIndex;
    UINT8         authenticatorMACAddr[6];
    UINT8         supplicantMACAddr[6];
    RSN_MSG_TYPE_t  messageType;
}
PACK_END EVTBUF_EVT_CLIENT_SUBTYPE_RSN_FAIL_ANONCE_DIFF;


//------------------------
// Sub-type for EVT_SWITCH

#define EVT_SWITCH_SUBTYPE_READ         1
#define EVT_SWITCH_SUBTYPE_WRITE        (1 << 1 )
#define EVT_SWITCH_SUBTYPE_PHY_CHANGE   (1 << 2 )
#define EVT_SWITCH_SUBTYPE_MAC_CHANGE   (1 << 3 )

// Buffers for EVT_SWITCH sub-types

typedef PACK_START struct _switch_read
{
    UINT32 smiDevAddr;
    UINT32 smiRegAddr;
    UINT32 readVal;
}
PACK_END EVTBUF_EVT_SWITCH_SUBTYPE_READ;

typedef PACK_START struct _switch_write
{
    UINT32 smiDevAddr;
    UINT32 smiRegAddr;
    UINT32 writeVal;
}
PACK_END EVTBUF_EVT_SWITCH_SUBTYPE_WRITE;

typedef PACK_START struct _switch_phy_change
{
    UINT16 port;
    UINT16 statusReg1; // Copper Specific Status Register 1
    UINT16 statusReg2; // Copper Specific Status Register 2
}
PACK_END EVTBUF_EVT_SWITCH_SUBTYPE_PHY_CHANGE;

typedef PACK_START struct _switch_mac_change
{
    UINT16 dummy;
    UINT16 status;
}
PACK_END EVTBUF_EVT_SWITCH_SUBTYPE_MAC_CHANGE;

//---------------------------
// Sub-types for EVT_SYSTEM

#define EVT_SYSTEM_SUBTYPE_FIRMWARE_READY           1
#define EVT_SYSTEM_SUBTYPE_EXT_ANT_CHANGE          (1 << 1 )
#define EVT_SYSTEM_SUBTYPE_CMD_DISPATCHER_READY    (1 << 31)

// Buffers for EVT_SYSTEM sub-types

typedef PACK_START struct _system_ext_ant_change
{
    UINT32 extAntStatus;  // 0 => Unplugged; 1 => Plugged
}
PACK_END EVTBUF_EVT_SYSTEM_SUBTYPE_EXT_ANT_CHANGE;


//---------------------------
// Sub-types for EVT_DEBUG

#define EVT_DEBUG_SUBTYPE_SAD_SAMPLE                1
#define EVT_DEBUG_SUBTYPE_SAD_LINK_SELECT          (1 << 1 )
#define EVT_DEBUG_SUBTYPE_SME_JOIN                 (1 << 2 )


// Prototypes

extern void eventInit(void);
extern void eventGenerate(UINT32 evtType, UINT32 evtSubType, UINT32 evtBodyLen, UINT8 * evtBody);


#endif  // _EVENT_H




