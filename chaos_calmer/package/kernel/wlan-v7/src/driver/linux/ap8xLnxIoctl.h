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

#ifndef	AP8X_IOCTL_H_
#define	AP8X_IOCTL_H_

#include <linux/version.h>
#include <linux/wireless.h>

#define	WL_IOCTL_WL_PARAM			(SIOCIWFIRSTPRIV + 0)
#define	WL_IOCTL_WL_GET_PARAM		(SIOCIWFIRSTPRIV + 1)
#define	WL_IOCTL_BSS_START			(SIOCIWFIRSTPRIV + 2)
#define	WL_IOCTL_GET_VERSION			(SIOCIWFIRSTPRIV + 3)
#define	WL_IOCTL_SET_TXRATE			(SIOCIWFIRSTPRIV + 4)
#define	WL_IOCTL_GET_TXRATE			(SIOCIWFIRSTPRIV + 5)
#define	WL_IOCTL_SET_CIPHERSUITE		(SIOCIWFIRSTPRIV + 6)
#define	WL_IOCTL_GET_CIPHERSUITE		(SIOCIWFIRSTPRIV + 7)
#define	WL_IOCTL_SET_PASSPHRASE		(SIOCIWFIRSTPRIV + 8)
#define	WL_IOCTL_GET_PASSPHRASE		(SIOCIWFIRSTPRIV + 9)
#define	WL_IOCTL_SET_FILTERMAC		(SIOCIWFIRSTPRIV + 10)
#define	WL_IOCTL_GET_FILTERMAC		(SIOCIWFIRSTPRIV + 11)
#define	WL_IOCTL_SET_BSSID				(SIOCIWFIRSTPRIV + 12)
#define	WL_IOCTL_GET_BSSID				(SIOCIWFIRSTPRIV + 13)
#define	WL_IOCTL_SET_TXPOWER			(SIOCIWFIRSTPRIV + 14)
#define	WL_IOCTL_GET_TXPOWER			(SIOCIWFIRSTPRIV + 15)
#define	WL_IOCTL_SET_WMMEDCAAP		(SIOCIWFIRSTPRIV + 16)
#define	WL_IOCTL_GET_WMMEDCAAP		(SIOCIWFIRSTPRIV + 17)
#define	WL_IOCTL_SET_WMMEDCASTA		(SIOCIWFIRSTPRIV + 18)
#define	WL_IOCTL_GET_WMMEDCASTA		(SIOCIWFIRSTPRIV + 19)
#define WL_IOCTL_SETCMD					(SIOCIWFIRSTPRIV + 20)
#define WL_IOCTL_GETCMD					(SIOCIWFIRSTPRIV + 25)
#define	WL_IOCTL_GET_STALISTEXT		(SIOCIWFIRSTPRIV + 21)
#define   WL_IOCTL_SET_APPIE			(SIOCIWFIRSTPRIV + 22)
#ifdef GENERIC_GETIE
#define   WL_IOCTL_GET_IE			(SIOCIWFIRSTPRIV + 23)
#define   WL_IOCTL_GET_SCAN_BSSPROFILE			(SIOCIWFIRSTPRIV + 31)
#else
#define   WL_IOCTL_GET_RSNIE			(SIOCIWFIRSTPRIV + 23)
#define   WL_IOCTL_GET_WSCIE			(SIOCIWFIRSTPRIV + 31)
#endif
#define   WL_IOCTL_SET_CLIENT			(SIOCIWFIRSTPRIV + 24)
#define WL_IOCTL_SET_WDS_PORT       		(SIOCIWFIRSTPRIV + 26)
#define WL_IOCTL_GET_WDS_PORT       (SIOCIWFIRSTPRIV + 27)
#define   WL_IOCTL_GET_STASCAN			(SIOCIWFIRSTPRIV + 29)
#define WL_IOCTL_SET_WAPI			(SIOCIWFIRSTPRIV + 30)


enum
{
	WL_PARAM_AUTHTYPE = 1, 
	WL_PARAM_BAND = 2,
	WL_PARAM_REGIONCODE = 3,
	WL_PARAM_HIDESSID = 4,
	WL_PARAM_PREAMBLE = 5,
	WL_PARAM_GPROTECT = 6,
	WL_PARAM_BEACON = 7,
	WL_PARAM_DTIM = 8,
	WL_PARAM_FIXRATE = 9,
	WL_PARAM_ANTENNA = 10,
	WL_PARAM_WPAWPA2MODE = 11,
	WL_PARAM_AUTHSUITE = 12,
	WL_PARAM_GROUPREKEYTIME = 13,
	WL_PARAM_WMM = 14,
	WL_PARAM_WMMACKPOLICY = 15,
	WL_PARAM_FILTER = 16,
	WL_PARAM_INTRABSS = 17,
	WL_PARAM_AMSDU = 18,
	WL_PARAM_HTBANDWIDTH = 19,
	WL_PARAM_GUARDINTERVAL = 20,
	WL_PARAM_EXTSUBCH = 21,
	WL_PARAM_HTPROTECT = 22,
	WL_PARAM_GETFWSTAT=23,
	WL_PARAM_AGINGTIME=24,
	WL_PARAM_ANTENNATX2 = 25,
	WL_PARAM_AUTOCHANNEL = 26,
	WL_PARAM_AMPDUFACTOR = 27,
	WL_PARAM_AMPDUDENSITY = 28,
	WL_PARAM_CARDDEVINFO = 29,
	WL_PARAM_INTEROP = 30,
	WL_PARAM_OPTLEVEL = 31,
	WL_PARAM_REGIONPWR = 32,
	WL_PARAM_ADAPTMODE = 33,
	WL_PARAM_SETKEYS = 34,
	WL_PARAM_DELKEYS = 35,
	WL_PARAM_MLME_REQ = 36,
	WL_PARAM_COUNTERMEASURES = 37,
	WL_PARAM_CSADAPTMODE = 38,
	WL_PARAM_DELWEPKEY = 39,
	WL_PARAM_WDSMODE = 40,
	WL_PARAM_STRICTWEPSHARE = 41,
	WL_PARAM_11H_CSA_CHAN = 42,
	WL_PARAM_11H_CSA_COUNT = 43,
	WL_PARAM_11H_CSA_MODE = 44,
	WL_PARAM_11H_CSA_START = 45,
	WL_PARAM_SPECTRUM_MGMT = 46,
	WL_PARAM_POWER_CONSTRAINT = 47,
	WL_PARAM_11H_DFS_MODE = 48,
	WL_PARAM_11D_MODE = 49,
	WL_PARAM_TXPWRFRACTION = 50,
	WL_PARAM_DISABLEASSOC = 51,
	WL_PARAM_PSHT_MANAGEMENTACT = 52,
	/* CLIENT_SUPPORT*/
	WL_PARAM_STAMODE = 53,
	WL_PARAM_STASCAN = 54,
	WL_PARAM_AMPDU_TX = 55,
	WL_PARAM_11HCACTIMEOUT = 56,
	WL_PARAM_11hNOPTIMEOUT = 57,
	WL_PARAM_11hDFSMODE = 58,
	WL_PARAM_MCASTPRXY = 59,
	WL_PARAM_11H_STA_MODE = 60,
	WL_PARAM_RSSI = 61,
	WL_PARAM_INTOLERANT = 62,
	WL_PARAM_TXQLIMIT = 63,
	WL_PARAM_RXINTLIMIT = 64,
	WL_PARAM_LINKSTATUS = 65,
	WL_PARAM_ANTENNATX = 66,
	WL_PARAM_RXPATHOPT = 67,
	WL_PARAM_HTGF = 68,
	WL_PARAM_HTSTBC = 69,
	WL_PARAM_3X3RATE = 70,
	WL_PARAM_AMSDU_FLUSHTIME = 71,
	WL_PARAM_AMSDU_MAXSIZE = 72,
	WL_PARAM_AMSDU_ALLOWSIZE = 73,
	WL_PARAM_AMSDU_PKTCNT = 74,
    WL_PARAM_CDD = 75,
	WL_PARAM_WAPIMODE = 76,
	WL_PARAM_ACS_THRESHOLD = 80,
};

#define WL_KEY_XMIT	        0x01	/* key used for xmit */
#define WL_KEY_RECV	        0x02	/* key used for recv */
#define WL_KEY_GROUP	    0x04	/* key used for WPA group operation */
#define WL_KEY_DEFAULT	    0x80	/* default xmit key */
#define WL_KEYIX_NONE       ((u_int16_t) - 1)

#define WL_CIPHER_NONE      0x00
#define WL_CIPHER_WEP40     0x01
#define WL_CIPHER_TKIP      0x02
#define WL_CIPHER_WRAP      0x03
#define WL_CIPHER_CCMP      0x04
#define WL_CIPHER_WEP104    0x05

#ifdef GENERIC_GETIE
struct wlreq_ie {
	u_int8_t	macAddr[6];
    u_int8_t    IEtype;
    u_int8_t    IELen;
	u_int8_t	IE[64];
};
#else
struct wlreq_rsnie {
	u_int8_t	macAddr[6];
	u_int8_t	rsnIE[64];
};
#endif

#ifdef MRVL_WSC
struct wlreq_wscie {
	u_int8_t	macAddr[6];
	u_int8_t	wscIE[280];
};
#endif

struct wlreq_key {
	u_int8_t ik_type;		/* key/cipher type */
	u_int8_t ik_pad;
	u_int16_t ik_keyix;		/* key index */
	u_int8_t ik_keylen;		/* key length in bytes */
	u_int8_t ik_flags;
	u_int8_t ik_macaddr[6];
	u_int64_t ik_keyrsc;		/* key receive sequence counter */
	u_int64_t ik_keytsc;		/* key transmit sequence counter */
	u_int8_t ik_keydata[16+8+8];
};

struct wlreq_del_key {
	u_int8_t idk_keyix;		/* key index */
	u_int8_t idk_macaddr[6];
};

#define	WL_MLME_ASSOC		1	/* associate station */
#define	WL_MLME_DISASSOC		2	/* disassociate station */
#define	WL_MLME_DEAUTH		3	/* deauthenticate station */
#define	WL_MLME_AUTHORIZE	4	/* authorize station */
#define	WL_MLME_UNAUTHORIZE	5	/* unauthorize station */
#define WL_MLME_CLEAR_STATS	6	/* clear station statistic */

struct wlreq_mlme {
	u_int8_t im_op;			/* operation to perform */
	u_int8_t im_ssid_len;		/* length of optional ssid */
	u_int16_t im_reason;		/* 802.11 reason code */
	u_int8_t im_macaddr[6];
	u_int8_t im_ssid[32];
};

#ifdef MRVL_WSC
#define WL_APPIE_FRAMETYPE_BEACON           1
#define WL_APPIE_FRAMETYPE_PROBE_RESP       2

struct wlreq_set_appie {
	u_int32_t    appFrmType;
	u_int32_t   appBufLen;
	u_int8_t    appBuf[504]; /*total size of 512 bytes */
}__attribute__ ((packed));

#endif //MRVL_WSC

#ifdef MRVL_WAPI
/* come from wapid, 1 and 2 is useed as beacon/probe-resp */
#define P80211_PACKET_WAPIFLAG          0
#define P80211_PACKET_SETKEY   			3

#define KEY_LEN 			16  	
/* from wapid */
struct wlreq_wapi_key {
	u_int8_t ik_macaddr[6];	       /* sta mac, all "ff" for mcastkey */
	u_int8_t ik_flags;		       /* always = 1 */
	u_int8_t ik_keyid;		       /* key index */
	u_int8_t ik_keydata[KEY_LEN*2];/* mcastkey: 32 byte key; ucastkey: uek (16 byte) + uck (16 byte) */
};
#endif

#endif /* AP8X_IOCTL_H_ */
