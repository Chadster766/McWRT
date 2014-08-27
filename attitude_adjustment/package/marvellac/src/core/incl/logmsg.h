#ifndef _LOG_MSG_H_
#define _LOG_MSG_H_

#define WLSYSLOG_CLASS_ALL 1

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
#define WLSYSLOG_MSG_GEN_RADARDETECTION     "RF - Radar Detected on current channel: "   /* IEEE80211_DH */

#define WLSYSLOG_MSG_MLME_AUTH_FAILURE      "MLME - Wireless client Authentication failure: "

#define WLSYSLOG_MSG_CLIENT_CONNECTED        "STA MLME - Client connected to BSSID: "
#define WLSYSLOG_MSG_CLIENT_DISCONNECTED        "STA MLME - Client disconnected "
#define WLSYSLOG_MSG_CLIENT_SCAN_DONE        "STA MLME - Client scan completed "
#define WLSYSLOG_MSG_CLIENT_CLONED        "STA MLME - MAC address cloned: "

#endif