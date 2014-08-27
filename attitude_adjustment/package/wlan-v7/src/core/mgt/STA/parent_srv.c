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

/*******************************************************************************************
*
* File: parent_srv.c
*        Client Parent Service Function Calls
* Description:  Implementation of the Client MLME Parent Services
*
*******************************************************************************************/
#include "mlmeSta.h"
#include "wl_mib.h"
#include "wl_hal.h"

#include "mlmeApi.h"
#include "mlmeSta.h"
#include "wlvmac.h"
#include "wl_macros.h"


extern MIB_WB                     				mib_WB[NUM_OF_WLMACS];
extern UINT8 					   				mib_StaMode[NUM_OF_WLMACS];

extern UINT8 mib_defaultkeyindex_parent[NUM_OF_WLMACS];
extern UINT8 WepType_parent[NUM_OF_WLMACS][4];
/* need to add/move end */

/* Extern from other Modules */
#ifdef WPA_STA
/* need to move this to WPA module */
//extern IEEEtypes_RSN_IE_t 						thisStaRsnIE[NUM_OF_WLMACS];
extern keyMgmtInfoSta_t                         gkeyMgmtInfoSta[NUM_OF_WLMACS];

#ifdef WPA2
//extern IEEEtypes_RSN_IE_WPA2_t 			thisStaRsnIEWPA2[NUM_OF_WLMACS];  
#endif 
#endif /* WPA_STA */

extern MIB_STA_CFG                         * staMib_StaCfg_p[NUM_OF_WLMACS];
extern MIB_OP_DATA                         * staMib_OpData_p[NUM_OF_WLMACS];
extern MIB_AUTH_ALG                        * staMib_AuthAlg_p[NUM_OF_WLMACS];
extern MIB_AUTH_ALG                        * staMib_AuthAlg_G[NUM_OF_WLMACS];
extern MIB_PHY_DSSS_TABLE                  * staMib_PhyDSSSTable_p;
extern MIB_WEP_DEFAULT_KEYS                * staMib_WepDefaultKeys_p[NUM_OF_WLMACS];
extern MIB_WEP_DEFAULT_KEYS                * staMib_WepDefaultKeys_G[NUM_OF_WLMACS];
extern MIB_PRIVACY_TABLE                   * staMib_PrivacyTable_p[NUM_OF_WLMACS];
#ifdef WPA_STA
#ifdef WPA2
extern MIB_RSNCONFIG                       * staMib_RSNConfig_p[NUM_OF_WLMACS];
extern MIB_RSNCONFIG_UNICAST_CIPHERS       * staMib_RSNConfigUnicastCiphers_p[NUM_OF_WLMACS];
extern MIB_RSNSTATS                        * staMib_RSNStats_p[NUM_OF_WLMACS];
extern IEEEtypes_RSN_IE_t *staMib_thisStaRsnIE_p[NUM_OF_WLMACS];
#ifdef AP_WPA2
extern IEEEtypes_RSN_IE_WPA2_t *staMib_thisStaRsnIEWPA2_p[NUM_OF_WLMACS];
extern MIB_RSNCONFIGWPA2                   * staMib_RSNConfigWPA2_p[NUM_OF_WLMACS];
extern MIB_RSNCONFIGWPA2_UNICAST_CIPHERS   * staMib_RSNConfigWPA2UnicastCiphers_p[NUM_OF_WLMACS];
extern MIB_RSNCONFIGWPA2_UNICAST_CIPHERS   * staMib_RSNConfigWPA2UnicastCiphers2_p[NUM_OF_WLMACS];
extern MIB_RSNCONFIGWPA2_AUTH_SUITES       * staMib_RSNConfigWPA2AuthSuites_p[NUM_OF_WLMACS];
#endif
#endif
#endif

#ifdef STA_ADHOC_SUPPORTED
extern TxBcnBuf txbcnbuf[NUM_OF_BEACONS];
extern TxBcnBuf txprbrspbuf[NUM_OF_BEACONS];
#endif /* STA_ADHOC_SUPPORTED */


/* Extern from MLME Module */
scanTableResult_t						scanTableResult_sta[NUM_OF_WLMACS];
siteSurveyResult_t  					siteSurveyTable_sta[NUM_OF_WLMACS];

/* If you want to run an instance of Station MLME */
/* It is your responsibility to allocate the Station */
/* MLME Information Structure and initial some of the values*/
IEEEtypes_BssDesc_t                     bssDescProfile[NUM_OF_WLMACS];
vmacEntry_t								vmacEntry_parent[2];
vmacStaInfo_t 							mlmeStaInfo_parent[2];
Station_t 								Station_parent[2];

/* Global to hold the assigned vMac Id of the Parent Session */
vmacId_t idAssigned_parent[2] = {-1, -1};

/* Scan descriptors */
IEEEtypes_BssDescSet_t 		BssDescSet[NUM_OF_WLMACS];

extern vmacId_t parentGetVMacId( UINT8 macIndex )
{
	if(macIndex >= NUM_OF_WLMACS)
	{
		return idAssigned_parent[MAC_0];
	}
	else
	{
		return idAssigned_parent[macIndex];
	}
}

extern vmacEntry_t * mlmeStaInit_Parent(UINT8   macIndex, 
										UINT8 * macAddr, 
										void  * callBackFunc_p)
{
	vmacId_t idAssigned;
	UINT16   macId;

	macId = TOMACID(macIndex);

	/* Init Station MLME Information Structure */
	//mib_StaMode[macIndex] = 1;
	//staMib_StaCfg_p[macIndex]->BcnPeriod = 100;
	staMib_StaCfg_p[macIndex]->DesiredBssType = BSS_INFRASTRUCTURE;

	mlmeStaInfo_parent[macIndex].scanTableResult_p = &scanTableResult_sta[macIndex];
	mlmeStaInfo_parent[macIndex].sSurveyTable_p = &siteSurveyTable_sta[macIndex];

	/* Set the scan buffer */
	mlmeStaInfo_parent[macIndex].BssDescSet_p = &BssDescSet[macIndex];

#ifdef STA_ADHOC_SUPPORTED
	/* Beacon Buffer Pointer */
	mlmeStaInfo_parent[macIndex].BcnTxInfo_p = (txBcnInfo_t *)&txbcnbuf[macid2index(macId)];
	mlmeStaInfo_parent[macIndex].BcnBuffer_p = (macmgmtQ_MgmtMsg_t *)&(txbcnbuf[macid2index(macId)].Bcn);

	/* Probe Response Pointer */
	mlmeStaInfo_parent[macIndex].PrbRspTxInfo_p = (txBcnInfo_t *)&txprbrspbuf[macid2index(macId)];
	mlmeStaInfo_parent[macIndex].PrbRspBuf_p = (macmgmtQ_MgmtMsg_t *)&(txprbrspbuf[macid2index(macId)].Bcn);
#endif /* STA_ADHOC_SUPPORTED */

	mlmeStaInfo_parent[macIndex].mib_StaMode_p = &mib_StaMode[macIndex];
	mlmeStaInfo_parent[macIndex].Station_p = &Station_parent[macIndex];

	mlmeStaInfo_parent[macIndex].mib_WB_p = &mib_WB[macIndex];

	// Initialize the system mib pointers
	mlmeStaInfo_parent[macIndex].staSystemMibs.mib_StaCfg_p   = staMib_StaCfg_p[macIndex];
	mlmeStaInfo_parent[macIndex].staSystemMibs.mib_OpData_p   = staMib_OpData_p[macIndex];
	mlmeStaInfo_parent[macIndex].staSystemMibs.PhyDSSSTable_p = staMib_PhyDSSSTable_p;

	// Initialize WEP mibs
	mlmeStaInfo_parent[macIndex].WepType_p             = &WepType_parent[macIndex][0];
	mlmeStaInfo_parent[macIndex].mib_defaultkeyindex_p = &mib_defaultkeyindex_parent[macIndex];

#ifdef WPA_STA
	/* For WPA */
	//mlmeStaInfo_parent[macIndex].thisStaRsnIE_p     = &thisStaRsnIE[macIndex];
	//mlmeStaInfo_parent[macIndex].thisStaRsnIEWPA2_p = &thisStaRsnIEWPA2[macIndex];
	mlmeStaInfo_parent[macIndex].keyMgmtInfoSta_p     = &gkeyMgmtInfoSta[macIndex];
#endif

	// Initialize the security mib pointers
	mlmeStaInfo_parent[macIndex].staSecurityMibs.mib_AuthAlg_p                 = staMib_AuthAlg_p[macIndex];
	mlmeStaInfo_parent[macIndex].staSecurityMibs.mib_AuthAlg_G                 = staMib_AuthAlg_G[macIndex];
	mlmeStaInfo_parent[macIndex].staSecurityMibs.mib_PrivacyTable_p            = staMib_PrivacyTable_p[macIndex];
	mlmeStaInfo_parent[macIndex].staSecurityMibs.mib_WepDefaultKeys_p          = staMib_WepDefaultKeys_p[macIndex];
	mlmeStaInfo_parent[macIndex].staSecurityMibs.mib_WepDefaultKeys_G          = staMib_WepDefaultKeys_G[macIndex];

#ifdef WPA_STA
	mlmeStaInfo_parent[macIndex].staSecurityMibs.mib_RSNConfig_p               = staMib_RSNConfig_p[macIndex];
	mlmeStaInfo_parent[macIndex].staSecurityMibs.mib_RSNConfigUnicastCiphers_p = staMib_RSNConfigUnicastCiphers_p[macIndex];
	mlmeStaInfo_parent[macIndex].staSecurityMibs.mib_RSNStats_p                = staMib_RSNStats_p[macIndex];
	mlmeStaInfo_parent[macIndex].staSecurityMibs.thisStaRsnIE_p = staMib_thisStaRsnIE_p[macIndex];
#ifdef WPA2                                        
	mlmeStaInfo_parent[macIndex].staSecurityMibs.mib_RSNConfigWPA2_p                = staMib_RSNConfigWPA2_p[macIndex];
	mlmeStaInfo_parent[macIndex].staSecurityMibs.mib_RSNConfigWPA2UnicastCiphers_p  = staMib_RSNConfigWPA2UnicastCiphers_p[macIndex];
	mlmeStaInfo_parent[macIndex].staSecurityMibs.mib_RSNConfigWPA2UnicastCiphers2_p = staMib_RSNConfigWPA2UnicastCiphers2_p[macIndex];
	mlmeStaInfo_parent[macIndex].staSecurityMibs.mib_RSNConfigWPA2AuthSuites_p      = staMib_RSNConfigWPA2AuthSuites_p[macIndex];
	mlmeStaInfo_parent[macIndex].staSecurityMibs.thisStaRsnIEWPA2_p= staMib_thisStaRsnIEWPA2_p[macIndex];
#endif /* WPA2 */
#endif /* WPA_STA */

	/* Set parent session BSS Profile pointer */
	mlmeStaInfo_parent[macIndex].bssDescProfile_p = &bssDescProfile[macIndex];
	memset((void *)mlmeStaInfo_parent[macIndex].bssDescProfile_p, 0, sizeof(IEEEtypes_BssDesc_t));

	/* Set it as a Parent session */
	mlmeStaInfo_parent[macIndex].isParentSession = TRUE;

	/* Record the callBack Function */
	mlmeStaInfo_parent[macIndex].mlmeCallBack_fp = callBackFunc_p;

	/* Record the parent vMacEntry address */
	mlmeStaInfo_parent[macIndex].vMacEntry_p = (UINT8 *)&vmacEntry_parent[macIndex];
	memcpy(&vmacEntry_parent[macIndex].vmacAddr[0],
		macAddr,
		sizeof(IEEEtypes_MacAddr_t));

	vmacEntry_parent[macIndex].phyHwMacIndx = macIndex;
#ifdef MLME_FORCE_STA_TO_PRIMARY_ADDR
	vmacEntry_parent[macIndex].macId = TOMACID(macIndex);
#else
	vmacEntry_parent[macIndex].macId = TOMACID(vmacEntry_parent[macIndex].phyHwMacIndx)|MAX_ALLOW_AP_PER_MAC;
	mlmeApiSetMacAddrByMacId((vmacStaInfo_t *)vmacEntry_parent[macIndex].info_p);
#endif

	vmacEntry_parent[macIndex].modeOfService = VMAC_MODE_CLNT_INFRA;
	vmacEntry_parent[macIndex].mlmeMsgEvt = &evtSme_StaCmdMsg;
	vmacEntry_parent[macIndex].dot11MsgEvt = &evtDot11_StaMgtMsg;
	vmacEntry_parent[macIndex].info_p = (UINT8 *)&mlmeStaInfo_parent[macIndex];

	/* */
	vmacStaInfoInit(vmacEntry_parent[macIndex].info_p);
	/* Lastly register the station id and information structure */
	if((idAssigned = vmacRegister(&vmacEntry_parent[macIndex])) == -1 )
	{
		return NULL;
	}
	idAssigned_parent[macIndex] = idAssigned;
	vmacEntry_parent[macIndex].id = idAssigned;

	mlmeApiHwSetSTAMode((vmacStaInfo_t *)vmacEntry_parent[macIndex].info_p);

	/* If all is good to go, Initialize entry for use */
	vmacEntry_parent[macIndex].active = 0;

	return &vmacEntry_parent[macIndex];
}
