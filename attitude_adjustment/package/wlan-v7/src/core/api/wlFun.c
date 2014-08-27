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


#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/net.h>
#include <linux/netdevice.h>
#include <linux/wireless.h>
#include <linux/pci.h>
#include <linux/delay.h>
#include <linux/wait.h>
#include <linux/proc_fs.h>

#include <net/iw_handler.h>
#include <asm/processor.h>
#include <asm/uaccess.h>

#include "wl.h"
#include "wltypes.h"
#include "wlApi.h"
#include "wlFun.h"


/* CONSTANTS AND MACROS
 */

#define WL_FUN_SETUP_APICMDBUF(info, pBuf, buf_size, cmd, body_size) \
{ \
	pBuf = kmalloc(buf_size, GFP_KERNEL); \
	if (pBuf == NULL) return FALSE; \
	pBuf->cmdCode = cpu_to_le16(cmd); \
	pBuf->seqNum = cpu_to_le16(1); \
	pBuf->cmdBodyLen = cpu_to_le16(body_size); \
	pBuf->cmdBody = (char *)info; \
}

#define WL_FUN_PROCESS_API(pBuf, result) \
{ \
	result = apctl( pBuf); \
	kfree (pBuf); \
	return result; \
}


/* PUBLIC FUNCTIONS DEFINITION
 */

BOOLEAN WL_FUN_BssStart(void *info)
{
	APICMDBUF *pBuf; 
	BOOLEAN result;

	WL_FUN_SETUP_APICMDBUF(info,pBuf, sizeof(APICMDBUF),
		APCMD_BSS_START, 0);
	
	WL_FUN_PROCESS_API(pBuf, result);
}

 
BOOLEAN WL_FUN_SetChannel(void *info, int channel)
{
	APICMDBUF *pBuf; 
	BOOLEAN result;

	WL_FUN_SETUP_APICMDBUF(info,pBuf, sizeof (APICMDBUF_SET_RF_CHANNEL), 
		APCMD_SET_RF_CHANNEL, sizeof(APICMDBUF_SET_RF_CHANNEL)-sizeof(APICMDBUF));
	
	((APICMDBUF_SET_RF_CHANNEL*)pBuf)->curChanNum = (UCHAR)channel;
	
	WL_FUN_PROCESS_API(pBuf, result);
}


BOOLEAN WL_FUN_SetESSID(void *info, char *essid)
{
	APICMDBUF *pBuf; 
	BOOLEAN result;

	WL_FUN_SETUP_APICMDBUF(info,pBuf, sizeof (APICMDBUF)+32, 
		APCMD_SET_SSID, 32);	
	
	memcpy(pBuf->cmdBody, essid, 32);
	
	WL_FUN_PROCESS_API(pBuf, result);
}


BOOLEAN WL_FUN_SetRTS(void *info, int rts)
{
	APICMDBUF *pBuf; 
	BOOLEAN result;

	WL_FUN_SETUP_APICMDBUF(info,pBuf, sizeof (APICMDBUF_SET_RTS_THRESHOLD), 
		APCMD_SET_RTS_THRESHOLD, sizeof(APICMDBUF_SET_RTS_THRESHOLD)-sizeof(APICMDBUF));
	
	((APICMDBUF_SET_RTS_THRESHOLD*)pBuf)->rtsThreshold = cpu_to_le16((USHORT)rts);

	WL_FUN_PROCESS_API(pBuf, result);
}

BOOLEAN WL_FUN_SetPrivacyOption(void *info, int privacy)
{
	APICMDBUF *pBuf; 
	BOOLEAN result;

	WL_FUN_SETUP_APICMDBUF(info,pBuf, sizeof (APICMDBUF_SET_PRIVACY_OPTION), 
		APCMD_SET_PRIVACY_OPTION, sizeof(APICMDBUF_SET_PRIVACY_OPTION)-sizeof(APICMDBUF));
	
	((APICMDBUF_SET_PRIVACY_OPTION*)pBuf)->dataEncrypt = cpu_to_le16((USHORT)privacy);

	WL_FUN_PROCESS_API(pBuf, result);
}

BOOLEAN WL_FUN_SetWEPKey(void *info, int keyindex, int keyType, char *keyValue)
{
	APICMDBUF *pBuf; 
	BOOLEAN result;

	WL_FUN_SETUP_APICMDBUF(info,pBuf, sizeof(APICMDBUF_SET_DEFAULT_WEP_KEYS), 
		APCMD_SET_DEFAULT_WEP_KEYS, sizeof(APICMDBUF_SET_DEFAULT_WEP_KEYS)-sizeof(APICMDBUF));	
	
	((APICMDBUF_SET_DEFAULT_WEP_KEYS*)pBuf)->keyindex = cpu_to_le16(keyindex);
	((APICMDBUF_SET_DEFAULT_WEP_KEYS*)pBuf)->keyType0 = cpu_to_le16(keyType);
	((APICMDBUF_SET_DEFAULT_WEP_KEYS*)pBuf)->keyType1 = cpu_to_le16(keyType);
	((APICMDBUF_SET_DEFAULT_WEP_KEYS*)pBuf)->keyType2 = cpu_to_le16(keyType);
	((APICMDBUF_SET_DEFAULT_WEP_KEYS*)pBuf)->keyType3 = cpu_to_le16(keyType);
	if(keyindex == 0)
	    memcpy(((APICMDBUF_SET_DEFAULT_WEP_KEYS*)pBuf)->keyValue0, keyValue, 16);
	if(keyindex == 1)
	memcpy(((APICMDBUF_SET_DEFAULT_WEP_KEYS*)pBuf)->keyValue1, keyValue, 16);
	if(keyindex == 2)
	memcpy(((APICMDBUF_SET_DEFAULT_WEP_KEYS*)pBuf)->keyValue2, keyValue, 16);
	if(keyindex == 3)
	memcpy(((APICMDBUF_SET_DEFAULT_WEP_KEYS*)pBuf)->keyValue3, keyValue, 16);
	
	WL_FUN_PROCESS_API(pBuf, result);
}

/* iwpriv command */
BOOLEAN WL_FUN_SetAuthType(void *info, int authtype)
{
	APICMDBUF *pBuf;
	BOOLEAN result;

	WL_FUN_SETUP_APICMDBUF(info,pBuf, sizeof (APICMDBUF_SET_AUTH_MODE), 
		APCMD_SET_AUTH_MODE, sizeof(APICMDBUF_SET_AUTH_MODE)-sizeof(APICMDBUF));	
	/* Firmware API will decrease 1 */
	((APICMDBUF_SET_AUTH_MODE*)pBuf)->authMode = (UCHAR)(authtype+1);
	
	WL_FUN_PROCESS_API(pBuf, result);
}

BOOLEAN WL_FUN_SetBand(void *info, int band)
{
	APICMDBUF *pBuf;
	BOOLEAN result;

	WL_FUN_SETUP_APICMDBUF(info,pBuf, sizeof (APICMDBUF_SET_AP_MODE), 
		APCMD_SET_AP_MODE, sizeof(APICMDBUF_SET_AP_MODE)-sizeof(APICMDBUF));	
	
	((APICMDBUF_SET_AP_MODE*)pBuf)->apMode = cpu_to_le32((UINT32)band);
	
	WL_FUN_PROCESS_API(pBuf, result);
}

BOOLEAN WL_FUN_SetRegionCode(void *info, int region)
{
	APICMDBUF *pBuf;
	BOOLEAN result;

#ifdef IEEE80211H
	WL_FUN_SETUP_APICMDBUF(info,pBuf, sizeof (APICMDBUF_SET_REGULATORY_DOMAIN), 
		APCMD_SET_REGULATORY_DOMAIN, sizeof(APICMDBUF_SET_REGULATORY_DOMAIN)-sizeof(APICMDBUF));	
	
	((APICMDBUF_SET_REGULATORY_DOMAIN*)pBuf)->domain = (UCHAR)region;
	
	WL_FUN_PROCESS_API(pBuf, result);
#else
    return FALSE;
#endif
}

BOOLEAN WL_FUN_SetHideSSID(void *info, int hidessid)
{
	APICMDBUF *pBuf;
	BOOLEAN result;

	WL_FUN_SETUP_APICMDBUF(info,pBuf, sizeof (APICMDBUF_SET_OPEN_SSID_EN_STATE), 
		APCMD_SET_OPEN_SSID_EN_STATE, sizeof(APICMDBUF_SET_OPEN_SSID_EN_STATE)-sizeof(APICMDBUF));	
	if(hidessid)
	    ((APICMDBUF_SET_OPEN_SSID_EN_STATE*)pBuf)->openSSIDEnabled = 0;
	else
	    ((APICMDBUF_SET_OPEN_SSID_EN_STATE*)pBuf)->openSSIDEnabled = 1;
	
	WL_FUN_PROCESS_API(pBuf, result);
}

BOOLEAN WL_FUN_SetPreamble(void *info, int preamble)
{
	APICMDBUF *pBuf;
	BOOLEAN result;

	WL_FUN_SETUP_APICMDBUF(info,pBuf, sizeof (APICMBUF_SET_RF_PREAMBLE_OPTION), 
		APCMD_SET_RF_PREAMBLE_OPTION, sizeof(APICMBUF_SET_RF_PREAMBLE_OPTION)-sizeof(APICMDBUF));	

	((APICMBUF_SET_RF_PREAMBLE_OPTION*)pBuf)->rfPreambleOption = (UCHAR)preamble;
	
	WL_FUN_PROCESS_API(pBuf, result);
}

BOOLEAN WL_FUN_SetGProtect(void *info, int Gprotect)
{
	APICMDBUF *pBuf;
	BOOLEAN result;

#ifdef WPA /* ap8xLnxApi.h define it that compatible Firmware*/
	WL_FUN_SETUP_APICMDBUF(info,pBuf, sizeof (APICMDBUF_SET_FORCE_PROTECTION_MODE), 
		APCMD_SET_FORCE_PROTECTION_MODE, sizeof(APICMDBUF_SET_FORCE_PROTECTION_MODE)-sizeof(APICMDBUF));	

    if(Gprotect)
	    ((APICMDBUF_SET_FORCE_PROTECTION_MODE*)pBuf)->forceProtectMode = cpu_to_le16(0);
	else
	    ((APICMDBUF_SET_FORCE_PROTECTION_MODE*)pBuf)->forceProtectMode = cpu_to_le16(1);
	
	WL_FUN_PROCESS_API(pBuf, result);
#else
    return FALSE;
#endif
}

BOOLEAN WL_FUN_SetBeacon(void *info, int beacon)
{
	APICMDBUF *pBuf;
	BOOLEAN result;

	WL_FUN_SETUP_APICMDBUF(info,pBuf, sizeof (APICMDBUF_SET_BEACON_PERIOD), 
		APCMD_SET_BEACON_PERIOD, sizeof(APICMDBUF_SET_BEACON_PERIOD)-sizeof(APICMDBUF));	

    ((APICMDBUF_SET_BEACON_PERIOD*)pBuf)->beaconPeriod = cpu_to_le32(beacon);
	
	WL_FUN_PROCESS_API(pBuf, result);
}

BOOLEAN WL_FUN_SetDtim(void *info, int dtim)
{
	APICMDBUF *pBuf;
	BOOLEAN result;

	WL_FUN_SETUP_APICMDBUF(info,pBuf, sizeof (APICMDBUF_SET_DTIM_PERIOD), 
		APCMD_SET_DTIM_PERIOD, sizeof(APICMDBUF_SET_DTIM_PERIOD)-sizeof(APICMDBUF));	

    ((APICMDBUF_SET_DTIM_PERIOD*)pBuf)->dtimPeriod = cpu_to_le32(dtim);
	
	WL_FUN_PROCESS_API(pBuf, result);
}

BOOLEAN WL_FUN_SetFixedRate(void *info, int fixedrate)
{
	APICMDBUF *pBuf;
	BOOLEAN result;

	WL_FUN_SETUP_APICMDBUF(info,pBuf, sizeof (APICMDBUF_SET_TX_DATA_RATE), 
		APCMD_SET_TX_DATA_RATE, sizeof(APICMDBUF_SET_TX_DATA_RATE)-sizeof(APICMDBUF));	

	((APICMDBUF_SET_TX_DATA_RATE*)pBuf)->txDataRate = (UCHAR)fixedrate;
	
	WL_FUN_PROCESS_API(pBuf, result);
}

BOOLEAN WL_FUN_SetFixBRate(void *info, int rate)
{
	APICMDBUF *pBuf;
	BOOLEAN result;

	WL_FUN_SETUP_APICMDBUF(info,pBuf, sizeof (APICMDBUF_SET_TX_DATA_RATE), 
		APCMD_SET_TX_DATA_RATE, sizeof(APICMDBUF_SET_TX_DATA_RATE)-sizeof(APICMDBUF));	

	((APICMDBUF_SET_TX_DATA_RATE*)pBuf)->txDataRate = (UCHAR)rate;
	
	WL_FUN_PROCESS_API(pBuf, result);
}

BOOLEAN WL_FUN_SetFixGRate(void *info, int rate)
{
	APICMDBUF *pBuf;
	BOOLEAN result;

	WL_FUN_SETUP_APICMDBUF(info,pBuf, sizeof (APICMDBUF_SET_TX_DATA_RATE_G), 
		APCMD_SET_TX_DATA_RATE_G, sizeof(APICMDBUF_SET_TX_DATA_RATE_G)-sizeof(APICMDBUF));	

	((APICMDBUF_SET_TX_DATA_RATE_G*)pBuf)->txDataRate = (UCHAR)rate;
	
	WL_FUN_PROCESS_API(pBuf, result);
}

BOOLEAN WL_FUN_SetAntenna(void *info, int antenna)
{
	APICMDBUF *pBuf;
	BOOLEAN result;

	WL_FUN_SETUP_APICMDBUF(info,pBuf, sizeof (APICMDBUF_SET_ANTENNA_MODE), 
		APCMD_SET_ANTENNA_MODE, sizeof(APICMDBUF_SET_ANTENNA_MODE)-sizeof(APICMDBUF));	

    ((APICMDBUF_SET_ANTENNA_MODE*)pBuf)->whichAnt = 2;
    ((APICMDBUF_SET_ANTENNA_MODE*)pBuf)->antennaMode = (UCHAR)antenna;
	
	WL_FUN_PROCESS_API(pBuf, result);
}

BOOLEAN WL_FUN_SetWPAMode(void *info, int WPAWPA2mode)
{
	APICMDBUF *pBuf;
	BOOLEAN result;

    if(WPAWPA2mode){
	    WL_FUN_SETUP_APICMDBUF(info,pBuf, sizeof (APICMDBUF_SET_PRIVACY_TBL_RSN_ENABLED), 
		    APCMD_SET_PRIVACY_TBL_RSN_ENABLED, sizeof(APICMDBUF_SET_PRIVACY_TBL_RSN_ENABLED)-sizeof(APICMDBUF));	

        ((APICMDBUF_SET_PRIVACY_TBL_RSN_ENABLED*)pBuf)->Enabled = cpu_to_le32(1);
        
        WL_FUN_PROCESS_API(pBuf, result);
    }else{
        WL_FUN_SETUP_APICMDBUF(info,pBuf, sizeof (APICMDBUF_SET_PRIVACY_TBL_RSN_ENABLED), 
		    APCMD_SET_PRIVACY_TBL_RSN_ENABLED, sizeof(APICMDBUF_SET_PRIVACY_TBL_RSN_ENABLED)-sizeof(APICMDBUF));	

        ((APICMDBUF_SET_PRIVACY_TBL_RSN_ENABLED*)pBuf)->Enabled = cpu_to_le32(0);
        
        WL_FUN_PROCESS_API(pBuf, result);
    }
}

#ifdef AP_WPA2
BOOLEAN WL_FUN_SetWPA2OnlyMode(void *info, int WPAWPA2mode)
{
    APICMDBUF *pBuf;
	BOOLEAN result;
	
    if(WPAWPA2mode == 2){
        WL_FUN_SETUP_APICMDBUF(info,pBuf, sizeof (APICMDBUF_SET_WPA2ONLY_ENABLED), 
	     APCMD_SET_WPA2ONLY_ENABLED, sizeof(APICMDBUF_SET_WPA2ONLY_ENABLED)-sizeof(APICMDBUF));	

            ((APICMDBUF_SET_WPA2ONLY_ENABLED*)pBuf)->Enabled = cpu_to_le32(1);
        
        WL_FUN_PROCESS_API(pBuf, result);
    }else{
         WL_FUN_SETUP_APICMDBUF(info,pBuf, sizeof (APICMDBUF_SET_WPA2ONLY_ENABLED), 
		    APCMD_SET_WPA2ONLY_ENABLED, sizeof(APICMDBUF_SET_WPA2ONLY_ENABLED)-sizeof(APICMDBUF));	

        ((APICMDBUF_SET_WPA2ONLY_ENABLED*)pBuf)->Enabled = cpu_to_le32(0);
        
        WL_FUN_PROCESS_API(pBuf, result);
    }
}

BOOLEAN WL_FUN_SetWPA2MixMode(void *info, int WPAWPA2mode)
{
    APICMDBUF *pBuf;
	BOOLEAN result;
	
    if(WPAWPA2mode == 3){
        WL_FUN_SETUP_APICMDBUF(info,pBuf, sizeof (APICMDBUF_SET_WPA2_ENABLED), 
	     APCMD_SET_WPA2_ENABLED, sizeof(APICMDBUF_SET_WPA2_ENABLED)-sizeof(APICMDBUF));	

        ((APICMDBUF_SET_WPA2_ENABLED*)pBuf)->Enabled = cpu_to_le32(1);
        
        WL_FUN_PROCESS_API(pBuf, result);
    }else{
        WL_FUN_SETUP_APICMDBUF(info,pBuf, sizeof (APICMDBUF_SET_WPA2_ENABLED), 
		    APCMD_SET_WPA2_ENABLED, sizeof(APICMDBUF_SET_WPA2_ENABLED)-sizeof(APICMDBUF));	

        ((APICMDBUF_SET_WPA2_ENABLED*)pBuf)->Enabled = cpu_to_le32(0);
        
        WL_FUN_PROCESS_API(pBuf, result);
    }
}
#endif

BOOLEAN WL_FUN_SetAuthSuite(void *info, int auth_suite)
{
	APICMDBUF *pBuf;
	BOOLEAN result;

	WL_FUN_SETUP_APICMDBUF(info,pBuf, sizeof (APICMDBUF_SET_RSN_CFG_AUTH_SUITE), 
		APCMD_SET_RSN_CFG_AUTH_SUITE, sizeof(APICMDBUF_SET_RSN_CFG_AUTH_SUITE)-sizeof(APICMDBUF));	

    ((APICMDBUF_SET_RSN_CFG_AUTH_SUITE*)pBuf)->AuthSuites[0] = 0x00;
    ((APICMDBUF_SET_RSN_CFG_AUTH_SUITE*)pBuf)->AuthSuites[1] = 0x50;
    ((APICMDBUF_SET_RSN_CFG_AUTH_SUITE*)pBuf)->AuthSuites[2] = 0xf2;
    ((APICMDBUF_SET_RSN_CFG_AUTH_SUITE*)pBuf)->AuthSuites[3] = (UCHAR)auth_suite;
    ((APICMDBUF_SET_RSN_CFG_AUTH_SUITE*)pBuf)->Enabled = 1;
	
	WL_FUN_PROCESS_API(pBuf, result);
}

BOOLEAN WL_FUN_SetUnicastCipherSuite(void *info, int cipher_suite)
{
    APICMDBUF *pBuf;
	BOOLEAN result;

	WL_FUN_SETUP_APICMDBUF(info,pBuf, sizeof (APICMDBUF_SET_RSN_CFG_UNICAST_CIPHER), 
		APCMD_SET_RSN_CFG_UNICAST_CIPHER, sizeof(APICMDBUF_SET_RSN_CFG_UNICAST_CIPHER)-sizeof(APICMDBUF));	

    ((APICMDBUF_SET_RSN_CFG_UNICAST_CIPHER*)pBuf)->UnicastCipher[0] = 0x00;
    ((APICMDBUF_SET_RSN_CFG_UNICAST_CIPHER*)pBuf)->UnicastCipher[1] = 0x50;
    ((APICMDBUF_SET_RSN_CFG_UNICAST_CIPHER*)pBuf)->UnicastCipher[2] = 0xf2;
    ((APICMDBUF_SET_RSN_CFG_UNICAST_CIPHER*)pBuf)->UnicastCipher[3] = (UCHAR)cipher_suite;
    ((APICMDBUF_SET_RSN_CFG_UNICAST_CIPHER*)pBuf)->Enabled = 1;
	
	WL_FUN_PROCESS_API(pBuf, result);
}

BOOLEAN WL_FUN_SetMulticastCipherSuite(void *info, int cipher_suite)
{
    APICMDBUF *pBuf;
	BOOLEAN result;

	WL_FUN_SETUP_APICMDBUF(info,pBuf, sizeof (APICMDBUF_SET_RSN_CFG_MULTICAST_CIPHERS), 
		APCMD_SET_RSN_CFG_MULTICAST_CIPHERS, sizeof(APICMDBUF_SET_RSN_CFG_MULTICAST_CIPHERS)-sizeof(APICMDBUF));	

    ((APICMDBUF_SET_RSN_CFG_MULTICAST_CIPHERS*)pBuf)->MulticastCipher[0] = 0x00;
    ((APICMDBUF_SET_RSN_CFG_MULTICAST_CIPHERS*)pBuf)->MulticastCipher[1] = 0x50;
    ((APICMDBUF_SET_RSN_CFG_MULTICAST_CIPHERS*)pBuf)->MulticastCipher[2] = 0xf2;
    ((APICMDBUF_SET_RSN_CFG_MULTICAST_CIPHERS*)pBuf)->MulticastCipher[3] = (UCHAR)cipher_suite;
	
	WL_FUN_PROCESS_API(pBuf, result);
}

BOOLEAN WL_FUN_SetWPA2UnicastCipherSuite(void *info, int cipher_suite)
{
    APICMDBUF *pBuf;
	BOOLEAN result;

	WL_FUN_SETUP_APICMDBUF(info,pBuf, sizeof (APICMDBUF_SET_RSN_CFG_WPA2_UNICAST_CIPHER), 
		APCMD_SET_RSN_CFG_WPA2_UNICAST_CIPHER, sizeof(APICMDBUF_SET_RSN_CFG_WPA2_UNICAST_CIPHER)-sizeof(APICMDBUF));	

    ((APICMDBUF_SET_RSN_CFG_WPA2_UNICAST_CIPHER*)pBuf)->UnicastCipher[0] = 0x00;
    ((APICMDBUF_SET_RSN_CFG_WPA2_UNICAST_CIPHER*)pBuf)->UnicastCipher[1] = 0x0f;
    ((APICMDBUF_SET_RSN_CFG_WPA2_UNICAST_CIPHER*)pBuf)->UnicastCipher[2] = 0xac;
    ((APICMDBUF_SET_RSN_CFG_WPA2_UNICAST_CIPHER*)pBuf)->UnicastCipher[3] = (UCHAR)cipher_suite;
    ((APICMDBUF_SET_RSN_CFG_WPA2_UNICAST_CIPHER*)pBuf)->Enabled = 1;
	
	WL_FUN_PROCESS_API(pBuf, result);
}

BOOLEAN WL_FUN_SetWPA2UnicastCipherSuite2(void *info, int cipher_suite)
{
    APICMDBUF *pBuf;
	BOOLEAN result;

	WL_FUN_SETUP_APICMDBUF(info,pBuf, sizeof (APICMDBUF_SET_RSN_CFG_WPA2_UNICAST_CIPHER2), 
		APCMD_SET_RSN_CFG_WPA2_UNICAST_CIPHER2, sizeof(APICMDBUF_SET_RSN_CFG_WPA2_UNICAST_CIPHER2)-sizeof(APICMDBUF));	

    ((APICMDBUF_SET_RSN_CFG_WPA2_UNICAST_CIPHER2*)pBuf)->UnicastCipher[0] = 0x00;
    ((APICMDBUF_SET_RSN_CFG_WPA2_UNICAST_CIPHER2*)pBuf)->UnicastCipher[1] = 0x0f;
    ((APICMDBUF_SET_RSN_CFG_WPA2_UNICAST_CIPHER2*)pBuf)->UnicastCipher[2] = 0xac;
    ((APICMDBUF_SET_RSN_CFG_WPA2_UNICAST_CIPHER2*)pBuf)->UnicastCipher[3] = (UCHAR)cipher_suite;
    ((APICMDBUF_SET_RSN_CFG_WPA2_UNICAST_CIPHER2*)pBuf)->Enabled = 1;
	
	WL_FUN_PROCESS_API(pBuf, result);
}

BOOLEAN WL_FUN_SetWPA2MulticastCipherSuite(void *info, int cipher_suite)
{
    APICMDBUF *pBuf;
	BOOLEAN result;

	WL_FUN_SETUP_APICMDBUF(info,pBuf, sizeof (APICMDBUF_SET_RSN_CFG_WPA2_MULTICAST_CIPHERS), 
		APCMD_SET_RSN_CFG_WPA2_MULTICAST_CIPHERS, sizeof(APICMDBUF_SET_RSN_CFG_WPA2_MULTICAST_CIPHERS)-sizeof(APICMDBUF));	

    ((APICMDBUF_SET_RSN_CFG_WPA2_MULTICAST_CIPHERS*)pBuf)->MulticastCipher[0] = 0x00;
    ((APICMDBUF_SET_RSN_CFG_WPA2_MULTICAST_CIPHERS*)pBuf)->MulticastCipher[1] = 0x0f;
    ((APICMDBUF_SET_RSN_CFG_WPA2_MULTICAST_CIPHERS*)pBuf)->MulticastCipher[2] = 0xac;
    ((APICMDBUF_SET_RSN_CFG_WPA2_MULTICAST_CIPHERS*)pBuf)->MulticastCipher[3] = (UCHAR)cipher_suite;
	
	WL_FUN_PROCESS_API(pBuf, result);
}

BOOLEAN WL_FUN_SetWPAPassPhrase(void *info, char *passphrase)
{
	APICMDBUF *pBuf; 
	BOOLEAN result;

	WL_FUN_SETUP_APICMDBUF(info,pBuf, sizeof (APICMDBUF_SET_PSK_PASSPHRASE), 
		APCMD_SET_PSK_PASSPHRASE, sizeof(APICMDBUF_SET_PSK_PASSPHRASE)-sizeof(APICMDBUF));	
	
	memcpy(((APICMDBUF_SET_PSK_PASSPHRASE *)pBuf)->passphrase, passphrase, 64);
	
	WL_FUN_PROCESS_API(pBuf, result);
}

BOOLEAN WL_FUN_SetWPA2PassPhrase(void *info, char *passphrase)
{
	APICMDBUF *pBuf; 
	BOOLEAN result;

	WL_FUN_SETUP_APICMDBUF(info,pBuf, sizeof (APICMDBUF_SET_RSN_CFG_WPA2_TBL), 
		APCMD_SET_RSN_CFG_WPA2_TBL, sizeof(APICMDBUF_SET_RSN_CFG_WPA2_TBL)-sizeof(APICMDBUF));	
	
	memcpy(((APICMDBUF_SET_RSN_CFG_WPA2_TBL *)pBuf)->PSKPassPhrase, passphrase, 64);
	
	WL_FUN_PROCESS_API(pBuf, result);
}

BOOLEAN WL_FUN_SetGroupReKeyTime(void *info, int group_rekey_time)
{
	APICMDBUF *pBuf;
	BOOLEAN result;

	WL_FUN_SETUP_APICMDBUF(info,pBuf, sizeof (APICMDBUF_SET_GRP_REKEY_TIME), 
		APCMD_SET_GRP_REKEY_TIME, sizeof(APICMDBUF_SET_GRP_REKEY_TIME)-sizeof(APICMDBUF));	

    ((APICMDBUF_SET_GRP_REKEY_TIME*)pBuf)->rekey_time = cpu_to_le32(group_rekey_time);
	
	WL_FUN_PROCESS_API(pBuf, result);
}

BOOLEAN WL_FUN_SetBCAConfig(void *info, int mode, ULONG wlTxPri0, ULONG wlTxPri1, ULONG wlRxPri0, ULONG wlRxPri1)
{
    APICMDBUF *pBuf;
	BOOLEAN result;
	
	WL_FUN_SETUP_APICMDBUF(info,pBuf, sizeof (APICMDBUF_BCA_CONFIG), 
		APCMD_SET_BCA_CONFIG, sizeof(APICMDBUF_BCA_CONFIG)-sizeof(APICMDBUF));
		
	((APICMDBUF_BCA_CONFIG*)pBuf)->mode =  cpu_to_le16((USHORT)mode);
	((APICMDBUF_BCA_CONFIG*)pBuf)->wlTxPri[0] = cpu_to_le32(wlTxPri0);
	((APICMDBUF_BCA_CONFIG*)pBuf)->wlTxPri[1] = cpu_to_le32(wlTxPri1);
	((APICMDBUF_BCA_CONFIG*)pBuf)->wlRxPri[0] = cpu_to_le32(wlRxPri0);
	((APICMDBUF_BCA_CONFIG*)pBuf)->wlRxPri[1] = cpu_to_le32(wlRxPri1);
	
	WL_FUN_PROCESS_API(pBuf, result);
}

/*
 * Arguments:
 *      traffic_type:   used for enabling/disabling Time Share algorithm.
 */
BOOLEAN WL_FUN_SetBCAConfigTimeShare(void *info, int traffic_type, ULONG timeshareInterval, ULONG btTime)
{
    APICMDBUF *pBuf;
	BOOLEAN result;
	
	WL_FUN_SETUP_APICMDBUF(info,pBuf, sizeof (APICMDBUF_BCA_CONFIG_TIMESHARE), 
		APCMD_SET_BCA_CONFIG_TIMESHARE, sizeof(APICMDBUF_BCA_CONFIG_TIMESHARE)-sizeof(APICMDBUF));
		
	((APICMDBUF_BCA_CONFIG_TIMESHARE*)pBuf)->timeshareInterval = cpu_to_le32(timeshareInterval);
	((APICMDBUF_BCA_CONFIG_TIMESHARE*)pBuf)->btTime = cpu_to_le32(btTime);
	((APICMDBUF_BCA_CONFIG_TIMESHARE*)pBuf)->timeshareEnable = (UCHAR)traffic_type;
	
	WL_FUN_PROCESS_API(pBuf, result);
}

BOOLEAN WL_FUN_SetBCAConfigTimeShareEnable(void *info, int Enabled)
{
    APICMDBUF *pBuf;
	BOOLEAN result;
	
	WL_FUN_SETUP_APICMDBUF(info,pBuf, sizeof (APICMDBUF_BCA_TIMESHARE_ENABLED), 
		APCMD_SET_BCA_TIMESHARE_ENABLED, sizeof(APICMDBUF_BCA_TIMESHARE_ENABLED)-sizeof(APICMDBUF));
		
	((APICMDBUF_BCA_TIMESHARE_ENABLED*)pBuf)->Enabled = (UCHAR)Enabled;
	
	WL_FUN_PROCESS_API(pBuf, result);
}

BOOLEAN WL_FUN_SetWMM(void *info, int wmm)
{
	APICMDBUF *pBuf;
	BOOLEAN result;
#ifdef QOS_FEATURE
	WL_FUN_SETUP_APICMDBUF(info,pBuf, sizeof (APICMDBUF_SET_QOS_MODE), 
		APCMD_SET_QOS_MODE, sizeof(APICMDBUF_SET_QOS_MODE)-sizeof(APICMDBUF));	

    ((APICMDBUF_SET_QOS_MODE*)pBuf)->qos_mode = (UCHAR)wmm;
	
	WL_FUN_PROCESS_API(pBuf, result);
#else
    return FALSE;
#endif
}

BOOLEAN WL_FUN_SetWMMAckPolicy(void *info, int wmm_ack_policy)
{
	APICMDBUF *pBuf;
	BOOLEAN result;
#ifdef QOS_FEATURE
	WL_FUN_SETUP_APICMDBUF(info,pBuf, sizeof (APICMDBUF_SET_NOACK_MODE), 
		APCMD_SET_NOACK_MODE, sizeof(APICMDBUF_SET_NOACK_MODE)-sizeof(APICMDBUF));	

    ((APICMDBUF_SET_NOACK_MODE*)pBuf)->noack_mode = (UCHAR)wmm_ack_policy;
	
	WL_FUN_PROCESS_API(pBuf, result);
#else
    return FALSE;
#endif
}

BOOLEAN WL_FUN_SetWMMParam(void *info, int Indx, int CWmin, int CWmax, int AIFSN, int TXOPLimitB, int TXOPLimit, int Mandatory)
{
    APICMDBUF *pBuf;
	BOOLEAN result;
#ifdef QOS_FEATURE
	WL_FUN_SETUP_APICMDBUF(info,pBuf, sizeof (APICMDBUF_SET_EDCA_PARAM), 
		APCMD_SET_EDCA_PARAM, sizeof(APICMDBUF_SET_EDCA_PARAM)-sizeof(APICMDBUF));	

    ((APICMDBUF_SET_EDCA_PARAM*)pBuf)->QAPEDCATable.QAPEDCATblIndx = cpu_to_le32(Indx);
    ((APICMDBUF_SET_EDCA_PARAM*)pBuf)->QAPEDCATable.QAPEDCATblCWmin = cpu_to_le32(CWmin);
    ((APICMDBUF_SET_EDCA_PARAM*)pBuf)->QAPEDCATable.QAPEDCATblCWmax = cpu_to_le32(CWmax);
    ((APICMDBUF_SET_EDCA_PARAM*)pBuf)->QAPEDCATable.QAPEDCATblAIFSN = cpu_to_le32(AIFSN);
    ((APICMDBUF_SET_EDCA_PARAM*)pBuf)->QAPEDCATable.QAPEDCATblTXOPLimitBAP = cpu_to_le32(TXOPLimitB);
    ((APICMDBUF_SET_EDCA_PARAM*)pBuf)->QAPEDCATable.QAPEDCATblTXOPLimit = cpu_to_le32(TXOPLimit);
    ((APICMDBUF_SET_EDCA_PARAM*)pBuf)->QAPEDCATable.QAPEDCATblMandatory = cpu_to_le32(Mandatory);
	
	WL_FUN_PROCESS_API(pBuf, result);
#else
    return FALSE;
#endif
}

BOOLEAN WL_FUN_SetFilter(void *info, int filter)
{
	APICMDBUF *pBuf;
	BOOLEAN result;

	WL_FUN_SETUP_APICMDBUF(info,pBuf, sizeof (APICMDBUF_SET_FILTER_MODE), 
		APCMD_SET_WLAN_FILTER_MODE, sizeof(APICMDBUF_SET_FILTER_MODE)-sizeof(APICMDBUF));	

    ((APICMDBUF_SET_FILTER_MODE*)pBuf)->mode = (UCHAR)filter;

	WL_FUN_PROCESS_API(pBuf, result);
}

BOOLEAN WL_FUN_SetMACFilter(void *info, int no, UCHAR * mac)
{
    APICMDBUF *pBuf;
	BOOLEAN result;
	UCHAR mac_buf[6*32+2];
	
	mac_buf[0] = 0;
	mac_buf[1] = no & 0xFF;
	memcpy(&(mac_buf[2]), mac , 6*32);
	WL_FUN_SETUP_APICMDBUF(info,pBuf, sizeof (APICMDBUF)+(32*6)+2, 
		APCMD_SET_WLAN_FILTER_LIST, (32*6)+2);	

    memcpy(pBuf->cmdBody, mac_buf, 32*6+2);

	WL_FUN_PROCESS_API(pBuf, result);
}

BOOLEAN WL_FUN_GetWMMParam(void *info, int Indx, int *CWmin, int *CWmax, int *AIFSN, int *TXOPLimitB, int *TXOPLimit, int *Mandatory)
{
    APICMDBUF *pBuf;
	BOOLEAN result = FALSE;
#ifdef QOS_FEATURE
	WL_FUN_SETUP_APICMDBUF(info,pBuf, sizeof (APICMDBUF_GET_EDCA_PARAM), 
		APCMD_GET_EDCA_PARAM, sizeof(APICMDBUF_GET_EDCA_PARAM)-sizeof(APICMDBUF));	

    ((APICMDBUF_GET_EDCA_PARAM*)pBuf)->QAPEDCATable.QAPEDCATblIndx = cpu_to_le32(Indx);
	
	if (apctl(pBuf)){
	    *CWmin = le32_to_cpu(((APICMDBUF_SET_EDCA_PARAM*)pBuf)->QAPEDCATable.QAPEDCATblCWmin);
        *CWmax = le32_to_cpu(((APICMDBUF_SET_EDCA_PARAM*)pBuf)->QAPEDCATable.QAPEDCATblCWmax);
        *AIFSN = le32_to_cpu(((APICMDBUF_SET_EDCA_PARAM*)pBuf)->QAPEDCATable.QAPEDCATblAIFSN);
        *TXOPLimitB = le32_to_cpu(((APICMDBUF_SET_EDCA_PARAM*)pBuf)->QAPEDCATable.QAPEDCATblTXOPLimitBAP);
        *TXOPLimit = le32_to_cpu(((APICMDBUF_SET_EDCA_PARAM*)pBuf)->QAPEDCATable.QAPEDCATblTXOPLimit);
        *Mandatory = le32_to_cpu(((APICMDBUF_SET_EDCA_PARAM*)pBuf)->QAPEDCATable.QAPEDCATblMandatory);
        result = TRUE;
        kfree (pBuf);
    }
    return result;
#else
    return FALSE;
#endif
}

BOOLEAN WL_FUN_GetSTAList(void *info, char *sta)
{
    APICMDBUF *pBuf;
	BOOLEAN result = FALSE;
	int len;
	
	WL_FUN_SETUP_APICMDBUF(info,pBuf, sizeof (APICMDBUF)+2, 
		APCMD_GET_STA_LIST, 2);
	apctl(pBuf);
	mdelay(1000);
	len = le16_to_cpu(pBuf->cmdBodyLen);
	kfree (pBuf);
    WL_FUN_SETUP_APICMDBUF(info,pBuf, sizeof (APICMDBUF)+len, 
		APCMD_GET_STA_LIST, len);
		
	result=apctl(pBuf);
	mdelay(1000);
	if (result)
	    memcpy(sta, pBuf->cmdBody, le16_to_cpu(pBuf->cmdBodyLen));
    kfree (pBuf);
    
    return result;
}

BOOLEAN WL_FUN_GetRfReg(void *info, USHORT reg, UCHAR *value)
{
	APICMDBUF *pBuf;
	BOOLEAN result;

	WL_FUN_SETUP_APICMDBUF(info,pBuf, sizeof (APICMDBUF_GET_RF_REG), 
		APCMD_GET_RF_REG, sizeof(APICMDBUF_GET_RF_REG)-sizeof(APICMDBUF));	

	((APICMDBUF_GET_RF_REG*)pBuf)->regNum = cpu_to_le16(reg);
	((APICMDBUF_GET_RF_REG*)pBuf)->regVal = 0;

	result = apctl(pBuf);

	if (result)
		*value = ((APICMDBUF_GET_RF_REG*)pBuf)->regVal;

	kfree(pBuf);

	return result;
}

BOOLEAN WL_FUN_SetRfReg(void *info, USHORT reg, UCHAR value)
{
	APICMDBUF *pBuf;
	BOOLEAN result;

	WL_FUN_SETUP_APICMDBUF(info,pBuf, sizeof (APICMDBUF_SET_RF_REG), 
		APCMD_SET_RF_REG, sizeof(APICMDBUF_SET_RF_REG)-sizeof(APICMDBUF));	

	((APICMDBUF_SET_RF_REG*)pBuf)->regNum = cpu_to_le16(reg);
	((APICMDBUF_SET_RF_REG*)pBuf)->regVal = value;

	WL_FUN_PROCESS_API(pBuf, result);
}
