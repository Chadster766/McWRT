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


#ifndef WLSYSLOG_H
#define WLSYSLOG_H

#include "wltypes.h"

/* Temp class*/
#define WLSYSLOG_CLASS_ALL 1
extern void wlsyslog(struct net_device *netdev,UINT32 classlevel, const char *format, ... );

#ifdef ENABLE_WLSYSLOG
#define WLSYSLOG(netdev, classlevel, ... ) wlsyslog(netdev, classlevel, __VA_ARGS__)
#else 
#define WLSYSLOG(classlevel, ... )
#endif	

#define WLSYSLOG_MSG_PSK_MSG2_FAILMIC   "WPA/WPA2 - PSK msg2 MIC failed: "
#define WLSYSLOG_MSG_PSK_MSG2_FAILRSNIE "WPA/WPA2 - PSK msg2 RSN IE failed: "
#define WLSYSLOG_MSG_PSK_MSG4_FAILMIC   "WPA/WPA2 - PSK mSg4 MIC failed: "
#define WLSYSLOG_MSG_PSK_SUCCESS        "WPA/WPA2 - PSK authentication successful: "
#define WLSYSLOG_MSG_PSK_GRPKEY_FAILMIC "WPA/WPA2 - Group key msg2 failed: "
#define WLSYSLOG_MSG_PSK_GRPKEY_SUCCESS "WPA/WPA2 - Group Key handshake successful: "
#define WLSYSLOG_MSG_PSK_GRPKEY_UPDATE  "WPA/WPA2 - Updating group key: "
#define WLSYSLOG_MSG_WEP_WEAKIV_ERROR   "WEP - Weak IV Received"

#define WLSYSLOG_MSG_MLME_ASSOC_SUCCESS     "MLME - Wireless client connected: "
#define WLSYSLOG_MSG_MLME_DEAUTH_TOSTA      "MLME - Disconnecting (deauth) wireless client: "
#define WLSYSLOG_MSG_MLME_DEAUTH_FROMSTA    "MLME - Wireless client disconnected (deauth): "
#define WLSYSLOG_MSG_MLME_DISASSOC_TOSTA    "MLME - Disconnecting (disassoc) wireless client: "
#define WLSYSLOG_MSG_MLME_DISASSOC_FROMSTA  "MLME - Wireless client disconnected (disassoc): "
#define WLSYSLOG_MSG_GEN_AUTOCHANNEL        "RF - Auto Channel selected: "
#ifdef IEEE80211_DH
#define WLSYSLOG_MSG_GEN_RADARDETECTION     "RF - Radar Detected on current channel: "
#endif //IEEE80211_DH

#define WLSYSLOG_MSG_MLME_AUTH_FAILURE      "MLME - Wireless client Authentication failure: "

#ifdef CLIENT_SUPPORT
#define WLSYSLOG_MSG_CLIENT_CONNECTED        "STA MLME - Client connected to BSSID: "
#define WLSYSLOG_MSG_CLIENT_DISCONNECTED        "STA MLME - Client disconnected "
#define WLSYSLOG_MSG_CLIENT_SCAN_DONE        "STA MLME - Client scan completed "
#define WLSYSLOG_MSG_CLIENT_CLONED        "STA MLME - MAC address cloned: "
#endif

#endif /* WLSYSLOG_H */
