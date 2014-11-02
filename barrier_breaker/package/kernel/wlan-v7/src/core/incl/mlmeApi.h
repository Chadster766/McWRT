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
* Description:  Handle all the events coming in and out of the MLME State Machines
*/
#ifndef _MLMEAPI_H
#define _MLMEAPI_H
#include "wltypes.h"
#include "StaDb.h"
#include "IEEE_types.h"

#ifdef STA_QOS
#include "qos.h"
#endif
#include "mlmeSta.h"
#define PACK   __attribute__ ((packed))

#define MLME_SEND_MAC_ONLY  1

#if 1 //Need Cleanup:: Defined else where, but will cause compile error if included
typedef PACK_START struct SwInfo_t
{
   UINT8 Reserved;
   UINT8 Age;
   UINT16 MacID;
   UINT16 StnId;
   UINT16 MacQId;
}
PACK_END SwInfo_t;

typedef struct tx80211_Frame_t
{
    SwInfo_t SwInfo;
    IEEEtypes_Frame_t Frame;
}
PACK_END tx80211_Frame_t;

typedef struct tx80211_MgmtMsg_t
{
    UINT8 reserved;
    // Bits 0-6 contain transmit rate index of frame, Bit 7 = 1 (long preamble), 0 (Short preamble)
    // A value of 0xFF in this member indicates "do not use this value". 0xFF can not be valid since 
    // 0x7F is not valid rate index
    UINT8 RateAndPreamble;
    UINT16 macID;
    UINT16 stnId;
    UINT16 macQId;
    macmgmtQ_MgmtMsg_t MgmtFrame;
}
PACK_END tx80211_MgmtMsg_t;
#endif //end_cleanup

#ifdef CLIENT_SUPPORT
#define	SSID_MAX_WPA_IE_LEN 40
#define SSID_MAX_WPS_IE_LEN 256
struct wps_scan_result {
	u8 bssid[ETH_ALEN];
	u8 ssid[32];
	size_t ssid_len;
	u8 wpa_ie[SSID_MAX_WPA_IE_LEN];
	size_t wpa_ie_len;
	u8 rsn_ie[SSID_MAX_WPA_IE_LEN];
	size_t rsn_ie_len;
	u8 wmm_ie[SSID_MAX_WPA_IE_LEN];
	size_t wmm_ie_len;
	u8 wps_ie[SSID_MAX_WPS_IE_LEN];      /* WPS IE */
	size_t wps_ie_len;
	int freq;
	u16 caps;
	int qual;
	int noise;
	int level;
	int maxrate;
};

typedef struct _MRVL_SCAN_ENTRY
{
	unsigned int dirty;
	struct wps_scan_result result;
}MRVL_SCAN_ENTRY ;
#endif //MRVL_WPS_CLIENT

/* MLME API Functions */
extern void mlmeApiSetBSSIDFilter(vmacStaInfo_t *vStaInfo_p, UINT8 mode);
extern void mlmeApiHwSetShortSlotTime(vmacStaInfo_t *vStaInfo_p, UINT8 opSet);
extern void mlmeApiSetAIdToMac(vmacStaInfo_t *vStaInfo_p, UINT32 AId);
extern void mlmeApiSetBssidToMac(vmacStaInfo_t *vStaInfo_p, UINT8 * bssid_p);
extern void mlmeApiSetSsIdToMac(vmacStaInfo_t *vStaInfo_p, UINT8 * ssid_p, UINT8 ssid_len);
extern void mlmeApiPrepHwStartIBss(vmacStaInfo_t *vStaInfo_p, IEEEtypes_StartCmd_t *StartCmd_p);
extern void mlmeApiPrepHwJoin(vmacStaInfo_t *vStaInfo_p, IEEEtypes_JoinCmd_t *JoinCmd_p);
extern void mlmeApiHwSetSTAMode(vmacStaInfo_t *vStaInfo_p);
extern void mlmeApiHwSetIBssMode(vmacStaInfo_t *vStaInfo_p);
extern void mlmeApiHwSetTxTimerRun(vmacStaInfo_t *vStaInfo_p, UINT32 val);
extern void mlmeApiHwStartIBssMode(vmacStaInfo_t *vStaInfo_p);
extern void mlmeApiResetTimerSync(vmacStaInfo_t *vStaInfo_p);
extern void mlmeApiNotifyNextBcnTime(vmacStaInfo_t *vStaInfo_p, UINT64   nextBcnTime);
extern void mlmeApiSetTimerSync(vmacStaInfo_t *vStaInfo_p, UINT32 val_lo, UINT32 val_hi);
extern void mlmeApiDisconnect (vmacStaInfo_t *vStaInfo_p);
extern void mlmeApiPrepHwToScan(vmacStaInfo_t *vStaInfo_p);
extern UINT8 mlmeApiGetRfChannel(vmacStaInfo_t *vStaInfo_p);
extern void mlmeApiSetRfChannel(vmacStaInfo_t *vStaInfo_p, UINT8 channel, UINT8 initRateTable, BOOLEAN scanning);
extern void mlmeApiGetTimerTxTSF( vmacStaInfo_t *vStaInfo_p,
                                  UINT32 *valueHi_p,
                                  UINT32 *valueLo_p);
extern SINT32 mlmeApiSetIbssDefaultFilter( vmacStaInfo_t *vStaInfo_p,
                                     BOOLEAN enable);
extern SINT32 mlmeApiSndScanNotificationOnly(vmacStaInfo_t *vStaInfo_p,
                                             UINT16 scanResult,
                                             UINT8 numSet,
                                             UINT16 bufSize,
                                             UINT8  *BssDescSet_p);
extern SINT32 mlmeApiQueryBeaconQueue( vmacStaInfo_t *vStaInfo_p);
extern UINT32 mlmeApiGetTime(vmacStaInfo_t *vStaInfo_p);
extern UINT32 mlmeApiGenRandomUINT32(vmacStaInfo_t *vStaInfo_p, 
                                     UINT32 seed);

extern UINT8 mlmeApiGetMacIndex( vmacStaInfo_t *vStaInfo_p);
extern UINT16 mlmeApiGetMacId( vmacStaInfo_t *vStaInfo_p);
extern void mlmeApiSetMacAddrByMacId(vmacStaInfo_t * vStaInfo_p);
extern SINT32 mlmeApiAddMacAddr(vmacStaInfo_t * vStaInfo_p,
                                UINT8 macIndex,
                                UINT8 *addr, 
                                UINT8 addr_mask);
extern SINT32 mlmeApiDelMacAddr(vmacStaInfo_t * vStaInfo_p,                              
                                UINT32 mac_id);
extern void mlmeApiStopTimer(vmacStaInfo_t *vStaInfo_p, 
                             UINT8 *timer_p);
extern void mlmeApiStartTimer(vmacStaInfo_t *vStaInfo_p, 
                              UINT8 *timer_p,
                              void *callback,
                              UINT32 ticks);
extern SINT32 mlmeApiWepEncrypt(vmacStaInfo_t *vStaInfo_p, 
                                UINT8 * outBuf, 
                                UINT8 *data, 
                                SINT32 dataLen);
extern void mlmeApiUpdateLinkStatus(vmacStaInfo_t *vStaInfo_p, 
                                    UINT8 linkId);
extern SINT32 mlmeApiSndNotification(vmacStaInfo_t *vStaInfo_p,
									 UINT8 *evtMsg_p, 
									 UINT8 mlmeEvt);
extern void mlmeApiEventNotification(vmacStaInfo_t *vStaInfo_p,                                        
                                     UINT32 eventInvoked,
                                     UINT8 *peerMacAddr, 
                                     UINT32 reasonCode);
extern void mlmeApiFreeSmeMsg(macmgmtQ_CmdBuf_t *cmdMsg);
extern macmgmtQ_CmdBuf_t* mlmeApiAllocSmeMsg( void );
extern BOOLEAN mlmeApiSendSmeMsg( macmgmtQ_CmdReq_t *smeCmd_p );
extern dot11MgtFrame_t *mlmeApiAllocMgtMsg(UINT8 phymac);
extern void mlmePrepDefaultMgtMsg_Sta( vmacStaInfo_t *vStaInfo_p,
                                       dot11MgtFrame_t *mgtFrame_p, 
                                       IEEEtypes_MacAddr_t *DestAddr, 
                                       UINT32 Subtype,
                                       IEEEtypes_MacAddr_t *BssId_p );
extern SINT32 mlmeApiSendMgtMsg_Sta(vmacStaInfo_t *vStaInfo_p,
									dot11MgtFrame_t *mgtFrame_p,
                                    UINT8 *pRxSign);
extern UINT32 mlmeApiSetControlRates(vmacStaInfo_t *vStaInfo_p);
extern void mlmeApiStaMacReset( UINT8 phyMacIndx, 
								vmacStaInfo_t *vStaInfo_p );
extern BOOLEAN mlmeApiFreePeerStationStaInfoAndAid(IEEEtypes_MacAddr_t *staMacAddr_p,
                                                   vmacEntry_t *clientVMacEntry_p);
extern BOOLEAN mlmeApiGetPeerStationStaInfoAndAid(IEEEtypes_MacAddr_t *staMacAddr_p,
                                   extStaDb_StaInfo_t **StaInfo_pp,
                                   UINT32 *Aid_p);
extern BOOLEAN mlmeApiCreatePeerStationInfoForWBMode(IEEEtypes_MacAddr_t *staMacAddr,
                                     IEEEtypes_SuppRatesElement_t *Rates_p,
                                     IEEEtypes_ExtSuppRatesElement_t *ExtRates_p,
                                     phyMacId_t			phyHwMacIndx);
extern UINT8 mlmeApiHalRssiDbmGet(vmacStaInfo_t *vStaInfo_p,
                            UINT8 rawRssi, 
                            UINT8 rawSq1);
extern BOOLEAN mlmeApiSetPeerStationStateForWB(extStaDb_StaInfo_t *StaInfo_p,
                            extStaDb_State_e state);
extern BOOLEAN mlmeApiCreateSupportedRateElement(IEEEtypes_SuppRatesElement_t *Rates_p,
                            IEEEtypes_ExtSuppRatesElement_t *ExtRates_p,
                            IEEEtypes_DataRate_t *bOpRateSet_p,
                            IEEEtypes_DataRate_t *gOpRateSet_p,
						    phyMacId_t phyHwMacIndx);
extern IEEEtypes_SuppRatesElement_t *mlmeApiGetClientModeSupportedRates(UINT32 phyHwMacIndx);
extern IEEEtypes_ExtSuppRatesElement_t *mlmeApiGetClientModeExtSupportedRates(UINT32 phyHwMacIndx);
extern void mlmeApiStartKeyMgmt(vmacStaInfo_t * vStaInfo_p);
extern SINT32 mlmeApiSetTrunkIdActive(vmacStaInfo_t *vStaInfo_p,
                                      SINT8 trunk_id, 
                                      BOOLEAN active, 
                                      UINT8 trunk_mode);
extern void mlmeApiUpdateSTAVendorIEs( vmacStaInfo_t *vStaInfo_p,
                                       UINT8 fForceUpdate);
extern UINT32 mlmeApiIsTrunkIdActive(vmacStaInfo_t *vStaInfo_p,
                                     SINT8 trunk_id);
extern WL_STATUS mlmeApiSendNullDataPktUr(vmacStaInfo_t *vStaInfo_p,
                                          IEEEtypes_MacAddr_t *DestAddr,
                                          IEEEtypes_MacAddr_t *SrcAddr);
extern UINT8 mlmeApiMamGetMACAddress(vmacStaInfo_t * vStaInfo_p,
                                     int clientType, 
                                     UINT8 *pMacAddress);
extern UINT8 mlmeApiMamGetHostMACAddress(vmacStaInfo_t *vStaInfo_p, 
                                          UINT8 *pAddr );
extern void mlmeApiDelStaDbEntry( vmacStaInfo_t *vStaInfo_p, UINT8 *peerAdr_p);

/* Mac Mgt Events Functions */
extern SINT32 evtMgtSrvTimeOut(vmacStaInfo_t *vStaInfo_p, UINT8 mgtSrvId);
extern SINT8 evtSme_StaCmdMsg(UINT8 *message, UINT8 *dummy, UINT8 *info_p);
extern SINT8 evtDot11_StaMgtMsg(UINT8 *message, UINT8 *rfHdr_p, UINT8 *info_p);
extern void mlmeApiAddStaDbEntry( vmacStaInfo_t *vStaInfo_p,
                                  dot11MgtFrame_t *MgmtMsg_p );
/* Misc Functions */
/* Need to resolve this if possible */
//extern UINT8 *strncpy(UINT8 *dst, UINT8 *src, SINT32 n);
//extern extStaDb_Status_e extStaDb_DelSta ( IEEEtypes_MacAddr_t *Addr_p, int option);
//extern extStaDb_StaInfo_t  *extStaDb_GetStaInfo( IEEEtypes_MacAddr_t *Addr_p, int option);
extern UINT32 macid2index(UINT32 macid);
extern int msi_wl_SetMacAddrByMacId(UINT32 phymac, UINT32 macId, UINT8 *addr);
extern UINT8 hal_RssiDbmGet(UINT8 rawRssi, UINT8 rawSq1);
extern void UpdateSTAVendorIEs( UINT8 fForceUpdate, UINT8 phymac);
extern SINT32 EurusSetTrunkIdActive(SINT8 trunk_id, UINT8 phymac, BOOLEAN active, UINT8 trunk_mode); 
extern UINT32 EurusIsTrunkIdActive(SINT8 trunk_id);
//extern BOOLEAN macMgtMlme_GetPeerStationAid(extStaDb_StaInfo_t *StaInfo_p, UINT32 *Aid_p);
extern WL_STATUS sendNullDataPktUr(IEEEtypes_MacAddr_t *DestAddr,IEEEtypes_MacAddr_t *SrcAddr, UINT16 phymac);
extern void KeyMgmtStaHskCtor(keyMgmtStahsk_hsm_t * me);
extern void KeyMgmtResetCounter(keyMgmtInfoSta_t *keyMgmtInfo_p);
extern void CounterMeasureInit_Sta(MIC_Error_t  *sta_MIC_Error_p, BOOLEAN optEnabled);

extern void mlmeApiSetMacAddrByMacId(vmacStaInfo_t * vStaInfo_p);
extern iw_linkInfo_t *mlmeApiGetStaLinkInfo(struct net_device *dev);

extern IEEEtypes_InfoElementHdr_t *smeParseIeType(UINT8 ieType,
                               UINT8 *ieBuf_p,
                               UINT16 ieBufLen);

extern SINT32 smeGetScanResults(UINT8 macIndex, 
								UINT8 *numDescpt_p,
								UINT16 *scanResultLen_p,
								UINT8 **inBuf_p);

extern SINT32 smeStopBss(UINT8 phyMacIndx);

extern BOOLEAN smeGetStaLinkInfo(vmacId_t mlme_vMacId,UINT8 *AssociatedFlag_p,UINT8 *bssId_p);

extern UINT8 *GetParentStaBSSID(UINT8 macIndex);
extern void *sme_SetClientPeerInfo(vmacEntry_t	*vmacEntry_p, PeerInfo_t *peerInfo_p);
extern PeerInfo_t *sme_GetClientPeerInfo(vmacEntry_t  *vmacEntry_p);
extern STA_SECURITY_MIBS * sme_GetStaSecurityMibsPtr(vmacEntry_t * vmacEntry_p);
UINT32 GetAssocRespLegacyRateBitMap(IEEEtypes_SuppRatesElement_t *SuppRates, IEEEtypes_ExtSuppRatesElement_t *ExtSuppRates);
extern keyMgmtInfoSta_t * sme_GetKeyMgmtInfoStaPtr(vmacEntry_t * vmacEntry_p);
extern STA_SYSTEM_MIBS * sme_GetStaSystemMibsPtr(vmacEntry_t * vmacEntry_p);
int wlset_mibChannel(vmacEntry_t *clientVMacEntry_p, UINT8 mib_STAMode);
extern void SendDelBASta(vmacApInfo_t *vmacSta_p,IEEEtypes_MacAddr_t StaAddr, UINT8 tsid);
void InitClientPeerInfo(struct net_device *netdev);
extern SINT32 smeSendScanRequest(UINT8 macIndex,UINT8  scanType,UINT8  bssType,UINT16 scanTime,UINT8  *bssid, UINT8  *ieBuf_p,UINT16 ieBufLen);
extern vmacEntry_t *sme_GetParentVMacEntry(UINT8 phyMacIndx);
extern SINT32 smeSetBssProfile(UINT8 macIndex,  UINT8 *bssid,IEEEtypes_CapInfo_t capInfo,UINT8 *ieBuf_p,UINT16 ieBufLen, BOOLEAN isApMrvl);
#ifdef CLIENT_SUPPORT
extern SINT32 smeCopyBssProfile(UINT8 macIndex, MRVL_SCAN_ENTRY *target);
#endif
extern void *sme_GetParentPrivInfo(UINT8 phyMacIndx);
extern void RemoveClientFw(UINT8 *macAddr_p, vmacEntry_t *clientVMacEntry_p);
extern SINT32 smeStartBss(UINT8 phyMacIndx);
extern vmacEntry_t *smeInitParentSession(UINT8 phyMacIndx, 
										 UINT8 *macAddr,
										 trunkId_t trunkId,
										 void *callBack_fp,
										 void *privInfo_p);
extern void RemoveRemoteAPFw(UINT8 *apMacAddr_p, vmacEntry_t *clientVMacEntry_p);
extern void sme_DisableKeyMgmtTimer(vmacEntry_t  *vmacEntry_p);
#ifdef WMON
#define	WMON_MAX_RSSI_COUNT	 0xffff
extern UINT8 g_wmon_rssi[WMON_MAX_RSSI_COUNT] ;
extern UINT32 g_wmon_rssi_count ;
extern UINT32 g_wmon_videoTrafficRx ;
#endif //WMON
extern UINT32 ClientModeDataCount[NUM_OF_WLMACS];		
extern UINT8 ClientModeTxMonitor;
extern UINT8 ProbeReqOnTx;		

extern vmacEntry_t *smeStartChildSession(UINT8 phyMacIndx, 
										 UINT8 *macAddr,
										 trunkId_t trunkId,
										 void *callBack_fp,
										 UINT32 controlParam,
										 void *privInfo_p);
extern void wlStatusUpdate_clientParent(UINT32 data1, UINT8 *info_p, UINT32 data2);
extern void SendAddBAReqSta(vmacApInfo_t *vmacSta_p,IEEEtypes_MacAddr_t StaAddrA, UINT8 tsid,
							IEEEtypes_QoS_BA_Policy_e BaPolicy, UINT32 SeqNo, UINT8 DialogToken);
#endif
