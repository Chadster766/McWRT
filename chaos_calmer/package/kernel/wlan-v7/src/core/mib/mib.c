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


/*
*
* Purpose:
*    This file provides declarations and initialization of MIB variables.
*
*/

#include "wltypes.h"
#include "IEEE_types.h"
#include "mib.h"
#include "util.h"

#include "osif.h"
#include "wl_mib.h"
#include "wl_hal.h"
#include "qos.h"
#include "wlmac.h"

#include "wl_macros.h"
#include "wlvmac.h"

#ifdef ENABLE_WEP

#define WEP_ENABLE      TRUE
#define WEP_TYPE        AUTH_SHARED_KEY
#define MIB_WEP         TRUE
#define PRIV_OPTION     TRUE
#define PRIV_INVOKED    TRUE

#else

#define WEP_ENABLE      FALSE
#define WEP_TYPE        AUTH_OPEN_SYSTEM
#define MIB_WEP         FALSE
#define PRIV_OPTION     FALSE
#define PRIV_INVOKED    FALSE

#endif

mib_Counters_t mib_Counters;
unsigned char LocalMacAddress[6] = {0x00, 0x50, 0x43, 0x02, 0x00, 0xf0};;
mib_MrvlRSNDataTrafficEnabled_t mib_MrvlRSNDataTrafficEnabled;
mib_MrvlRSN_PWK_t mib_MrvlRSN_PWK;
UINT8 mib_MrvlRSN_GrpMasterKey[32];
#ifdef QOS_FEATURE
extern mib_QAPEDCATable_t mib_QAPEDCATable[4];
#endif


#ifdef CLIENT_SUPPORT
MIB_PRIVACY_TABLE                       staMib_PrivacyTable[NUM_OF_WLMACS];
MIB_PRIVACY_TABLE                       * staMib_PrivacyTable_p[NUM_OF_WLMACS] = {&staMib_PrivacyTable[0], &staMib_PrivacyTable[1]};

MIB_STA_CFG                         staMib_StaCfg[NUM_OF_WLMACS];
MIB_STA_CFG                         * staMib_StaCfg_p[NUM_OF_WLMACS] = {&staMib_StaCfg[0], &staMib_StaCfg[1]};

MIB_OP_DATA                             staMib_OpData[NUM_OF_WLMACS];
MIB_OP_DATA                             * staMib_OpData_p[NUM_OF_WLMACS] = {&staMib_OpData[0], &staMib_OpData[1]};

MIB_AUTH_ALG                        staMib_AuthAlg[NUM_OF_WLMACS];
MIB_AUTH_ALG                        * staMib_AuthAlg_p[NUM_OF_WLMACS] = {&staMib_AuthAlg[0], &staMib_AuthAlg[1]};

MIB_AUTH_ALG                            staMib_AuthAlgG[NUM_OF_WLMACS];
MIB_AUTH_ALG                            * staMib_AuthAlg_G[NUM_OF_WLMACS] = {&staMib_AuthAlgG[0], &staMib_AuthAlgG[1]};

MIB_PHY_DSSS_TABLE                      staMib_PhyDSSSTable;
MIB_PHY_DSSS_TABLE                      * staMib_PhyDSSSTable_p = {&staMib_PhyDSSSTable};

MIB_WEP_DEFAULT_KEYS                staMib_WepDefaultKeys[NUM_OF_WLMACS];
MIB_WEP_DEFAULT_KEYS                * staMib_WepDefaultKeys_p[NUM_OF_WLMACS] = {&staMib_WepDefaultKeys[0], &staMib_WepDefaultKeys[1]};

MIB_WEP_DEFAULT_KEYS                staMib_WepDefaultKeysG[NUM_OF_WLMACS];
MIB_WEP_DEFAULT_KEYS                * staMib_WepDefaultKeys_G[NUM_OF_WLMACS] = {&staMib_WepDefaultKeysG[0], &staMib_WepDefaultKeysG[1]};

#ifdef WPA_STA
MIB_RSNCONFIG                       staMib_RSNConfig[NUM_OF_WLMACS];
MIB_RSNCONFIG                       * staMib_RSNConfig_p[NUM_OF_WLMACS]= {&staMib_RSNConfig[0], &staMib_RSNConfig[1]};
MIB_RSNCONFIG_UNICAST_CIPHERS       staMib_RSNConfigUnicastCiphers[NUM_OF_WLMACS];
MIB_RSNCONFIG_UNICAST_CIPHERS       * staMib_RSNConfigUnicastCiphers_p[NUM_OF_WLMACS]= {&staMib_RSNConfigUnicastCiphers[0], &staMib_RSNConfigUnicastCiphers[1]};
MIB_RSNSTATS                        staMib_RSNStats[NUM_OF_WLMACS];
MIB_RSNSTATS                        * staMib_RSNStats_p[NUM_OF_WLMACS] = {&staMib_RSNStats[0], &staMib_RSNStats[1]};

IEEEtypes_RSN_IE_t staMib_thisStaRsnIE[NUM_OF_WLMACS];
IEEEtypes_RSN_IE_t *staMib_thisStaRsnIE_p[NUM_OF_WLMACS]= {&staMib_thisStaRsnIE[0], &staMib_thisStaRsnIE[1]};
IEEEtypes_RSN_IE_WPA2_t staMib_thisStaRsnIEWPA2[NUM_OF_WLMACS];
IEEEtypes_RSN_IE_WPA2_t *staMib_thisStaRsnIEWPA2_p[NUM_OF_WLMACS] = {&staMib_thisStaRsnIEWPA2[0], &staMib_thisStaRsnIEWPA2[1]};

MIB_RSNCONFIGWPA2                   staMib_RSNConfigWPA2[NUM_OF_WLMACS];
MIB_RSNCONFIGWPA2                   * staMib_RSNConfigWPA2_p[NUM_OF_WLMACS] = {&staMib_RSNConfigWPA2[0], &staMib_RSNConfigWPA2[1]};

MIB_RSNCONFIGWPA2_UNICAST_CIPHERS   staMib_RSNConfigWPA2UnicastCiphers[NUM_OF_WLMACS];
MIB_RSNCONFIGWPA2_UNICAST_CIPHERS   * staMib_RSNConfigWPA2UnicastCiphers_p[NUM_OF_WLMACS] = {&staMib_RSNConfigWPA2UnicastCiphers[0], &staMib_RSNConfigWPA2UnicastCiphers[1]};

MIB_RSNCONFIGWPA2_UNICAST_CIPHERS   staMib_RSNConfigWPA2UnicastCiphers2[NUM_OF_WLMACS];
MIB_RSNCONFIGWPA2_UNICAST_CIPHERS   * staMib_RSNConfigWPA2UnicastCiphers2_p[NUM_OF_WLMACS] = {&staMib_RSNConfigWPA2UnicastCiphers2[0],&staMib_RSNConfigWPA2UnicastCiphers2[1]};

MIB_RSNCONFIGWPA2_AUTH_SUITES       staMib_RSNConfigWPA2AuthSuites[NUM_OF_WLMACS];
MIB_RSNCONFIGWPA2_AUTH_SUITES       * staMib_RSNConfigWPA2AuthSuites_p[NUM_OF_WLMACS] = {&staMib_RSNConfigWPA2AuthSuites[0], &staMib_RSNConfigWPA2AuthSuites[1]};

#endif /* WPA_STA */

UINT8 mib_defaultkeyindex_parent[NUM_OF_WLMACS];
UINT8 WepType_parent[NUM_OF_WLMACS][4];
UINT8 mib_childMode[NUM_OF_WLMACS] = {0};

MIB_WB                     				mib_WB[NUM_OF_WLMACS];
UINT8 					   				mib_StaMode[NUM_OF_WLMACS];

#endif /* CLIENT_SUPPORT */




#ifdef WIPOD
UINT8 mib_wipodMode;
#endif
#ifdef DHCPS
MIB_DHCPS	mib_DHCPS;
#endif

#ifdef AP_URPTR
UINT8                   mib_urMode;
UINT8                   mib_urModeConfig = 1;
UINT16                  mib_urRsvd;
UINT8                   mib_urSsid[33];
IEEEtypes_MacAddr_t     mib_urBssid;
UINT8                   mib_urPrefBssid;
extern  UINT8           mib_StaMode;
UINT8                   mib_wbMode = 0;
#endif

IEEEtypes_MacAddr_t      brdcastAddr = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
IEEEtypes_MacAddr_t      macAddrZero = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};


#ifdef JAPAN_CHANNEL_SPACING_10_SUPPORT
UINT8              mib_channelspacing;
#endif


BOOLEAN isStrictShareMode(vmacApInfo_t *vmacSta_p,BOOLEAN bType) 
{
	MIB_AUTH_ALG  *mib_AuthAlg_p=vmacSta_p->Mib802dot11->AuthAlg;
#ifdef AG_MIXED_SECURITY
	MIB_AUTH_ALG *mib_AuthAlg_G=vmacSta_p->Mib802dot11->AuthAlg_G;
	if (bType)
	{
		if (mib_AuthAlg_G->Type == AUTH_SHARED_KEY)
			return 1;
		else
			return 0;
	}
	else
#endif    
	{
		if (mib_AuthAlg_p->Type == AUTH_SHARED_KEY)
			return 1;
		else
			return 0;
	}
}

BOOLEAN isAuthAlgTypeMatch(vmacApInfo_t *vmacSta_p,UINT8 x, BOOLEAN bType) 
{  
	MIB_AUTH_ALG  *mib_AuthAlg_p=vmacSta_p->Mib802dot11->AuthAlg;
#ifdef AG_MIXED_SECURITY
	MIB_AUTH_ALG *mib_AuthAlg_G=vmacSta_p->Mib802dot11->AuthAlg_G;
	if (bType)
		return (x == mib_AuthAlg_G->Type);
	else 
#endif    
		return (x == mib_AuthAlg_p->Type);
}

BOOLEAN isWepRequired(vmacApInfo_t *vmacSta_p,BOOLEAN bType) 
{
	MIB_PRIVACY_TABLE *mib_PrivacyTable_p=vmacSta_p->Mib802dot11->Privacy;
#ifdef AG_MIXED_SECURITY
	if (bType)
		return (mib_PrivacyTable_p->PrivInvoked_G);
	else 
#endif    
		return (mib_PrivacyTable_p->PrivInvoked);
}

UINT8 isMacAccessList(vmacApInfo_t *vmacSta_p, IEEEtypes_MacAddr_t *destAddr_p)
{
	MIB_802DOT11 *mib=vmacSta_p->Mib802dot11;
	UINT8 i;
	UINT8 *mib_wlanfilterno_p = mib->mib_wlanfilterno;
	UINT8 *mib_wlanfiltertype_p = mib->mib_wlanfiltertype;

	if ( *(mib->mib_disableAssoc) )
		return FAIL;


	if ( *mib_wlanfiltertype_p  == DISABLE_MODE )
		return SUCCESS;

	for ( i = 0;i < *mib_wlanfilterno_p;i++ )
	{
		if ( memcmp(destAddr_p, mib->mib_wlanfiltermac + i*6, 6) == 0 )
		{
			/*found entry */
			if ( *mib_wlanfiltertype_p  == FILTER_MODE )
				return FAIL;
			return SUCCESS;
		}
	}
	/* not found */
	if ( *mib_wlanfiltertype_p  == FILTER_MODE )
		return SUCCESS;


	return FAIL;

}





