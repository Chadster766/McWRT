/*
*                Copyright 2002-2014, Marvell Semiconductor, Inc.
* This code contains confidential information of Marvell Semiconductor, Inc.
* No rights are granted herein under any patent, mask work right or copyright
* of Marvell or any third party.
* Marvell reserves the right at its sole discretion to request that this code
* be immediately returned to Marvell. This code is provided "as is".
* Marvell makes no warranties, express, implied or otherwise, regarding its
* accuracy, completeness or performance.
*/

#ifndef FWCMD_H_
#define FWCMD_H_

#include "osif.h"

#include "wltypes.h"
#include "IEEE_types.h"
#include "mib.h"
#include "util.h"

#include "wl_mib.h"
#include "wl_hal.h"
#include "qos.h"
#include "wlmac.h"

#include "hostcmd.h"
#include "wl_macros.h"
#include "wldebug.h"
#include "StaDb.h"
#include "mhsm.h"
#include "dfsMgmt.h" // DFS_SUPPORT
#include "domain.h" // DFS_SUPPORT

typedef enum {
	WL_DISABLE = 0,
	WL_ENABLE = 1,
	WL_DISABLE_VMAC = 0x80,
} wlfacilitate_e;

typedef enum {
	WL_LONGSLOT = 0,
	WL_SHORTSLOT = 1,
} wlslot_e;

typedef enum {
	WL_RATE_AUTOSELECT = 0,
	WL_RATE_GAP = 0,
	WL_RATE_AP_EVALUATED = 1,
	WL_RATE_1_0MBPS = 2,
	WL_RATE_2_0MBPS = 4,
	WL_RATE_5_5MBPS = 11,
	WL_RATE_6_0MBPS = 12,
	WL_RATE_9_0MBPS = 18,
	WL_RATE_11_0MBPS = 22,
	WL_RATE_12_0MBPS = 24,
	WL_RATE_18_0MBPS = 36,
	WL_RATE_24_0MBPS = 48,
	WL_RATE_36_0MBPS = 72,
	WL_RATE_48_0MBPS = 96,
	WL_RATE_54_0MBPS = 108,
} wlrate_e;

typedef enum {
	WL_GET = 0,
	WL_SET = 1,
	WL_RESET = 2,
} wloperation_e;

typedef enum {
	WL_GET_RC4  = 0, /* WEP & WPA/TKIP algorithm       */
	WL_SET_RC4  = 1, /* WEP & WPA/TKIP algorithm       */
	WL_GET_AES  = 2, /* WPA/CCMP & WPA2/CCMP algorithm */
	WL_SET_AES  = 3, /* WPA/CCMP & WPA2/CCMP algorithm */
	WL_RESETKEY = 4, /* reset key value to default     */
} wlkeyaction_e;

typedef enum {
	WL_BOOST_MODE_REGULAR = 0,
	WL_BOOST_MODE_WIFI = 1,
	WL_BOOST_MODE_DOUBLE= 2,
} wlboostmode_e;

typedef enum {
	WL_UNKNOWN_CLIENT_MODE = 0,
	WL_SINGLE_CLIENT_MODE = 1,
	WL_MULTIPLE_CLIENT_MODE = 2,
} wlboostclientmode_e;

typedef enum {
	WL_LONG_PREAMBLE = 1,
	WL_SHORT_PREAMBLE = 3,
	WL_AUTO_PREAMBLE = 5,
} wlpreamble_e;

typedef enum {
	WL_TX_POWERLEVEL_LOW = 5,
	WL_TX_POWERLEVEL_MEDIUM = 10,
	WL_TX_POWERLEVEL_HIGH = 15,
} wltxpowerlevel_e;

typedef enum {
	WL_ANTENNATYPE_RX = 1,
	WL_ANTENNATYPE_TX = 2,
	WL_ANTENNATYPE_TX2=3,
} wlantennatype_e;

typedef enum {
	WL_ANTENNAMODE_RX = 0xffff,
	WL_ANTENNAMODE_TX = 2,
} wlantennamode_e;

typedef enum {
	WL_MAC_TYPE_PRIMARY_CLIENT = 0,
	WL_MAC_TYPE_SECONDARY_CLIENT,
	WL_MAC_TYPE_PRIMARY_AP,
	WL_MAC_TYPE_SECONDARY_AP,
} wlmactype_e;

extern void wlFwCmdComplete(WL_PRIV *);
extern int wlFwSetRadio(WL_PRIV *, UINT16, wlpreamble_e);
extern int wlFwSetAntenna(WL_PRIV *, wlantennatype_e);
extern int wlFwSetRTSThreshold(WL_PRIV *, int);
extern int wlFwSetInfraMode(WL_PRIV *);
extern int wlFwSetRate(WL_PRIV *, wlrate_e);
extern int wlFwSetSlotTime(WL_PRIV *, wlslot_e);
extern int wlFwSetTxPower(WL_PRIV *, UINT8 flag, UINT32 powerLevel);
int wlFwGettxpower(WL_PRIV *,  UINT16 *powlist, UINT16 ch, 
        UINT16 band, UINT16 width, UINT16 sub_ch);
extern int wlsetFwPrescan(WL_PRIV *);
extern int wlsetFwPostscan(WL_PRIV *, UINT8 *, UINT8);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,35)
extern int wlFwSetMcast(WL_PRIV *, struct netdev_hw_addr *);
#else
extern int wlFwSetMcast(WL_PRIV *, struct dev_mc_list *);
#endif
extern int wlFwSetMacAddr(WL_PRIV *);
extern int wlFwSetAid(WL_PRIV *, UINT8 *, UINT16);
extern int wlFwSetChannel(WL_PRIV *, UINT8, CHNL_FLAGS,UINT8);
extern int wlgetFwStatistics(WL_PRIV *);
extern int wlFwSetApBeacon(WL_PRIV *);
extern int wlFwSetAPBss(WL_PRIV *, wlfacilitate_e);
extern int wlFwSetAPUpdateTim(WL_PRIV *, UINT16, Bool_e);
extern int wlFwSetAPBcastSSID(WL_PRIV *, wlfacilitate_e);
extern int wlsetFwApWds(WL_PRIV *, wlfacilitate_e);
extern int wlsetFwApBurstMode(WL_PRIV *, wlfacilitate_e);
extern int wlFwSetGProt(WL_PRIV *, wlfacilitate_e);
int wlFwSetHTGF( WL_PRIV *, UINT32 mode);
int wlFwSetRadarDetection(WL_PRIV *, UINT32 action);
int wlFwGetAddrValue(WL_PRIV *,  UINT32 addr, UINT32 len, UINT32 *val, UINT16 set);
int wlFwGetAddrtable(WL_PRIV *);
int wlFwGetHwStats(WL_PRIV *, char *page);
int wlFwGetEncrInfo(WL_PRIV *, unsigned char *addr);
extern int wlFwSetWdsMode(WL_PRIV *); /* WDS_SUPPORT */
int wlFwSetRegionCode(WL_PRIV *, UINT16 regionCode);
extern int wlFwSetWscIE(WL_PRIV *, UINT16 ieType, WSC_COMB_IE_t *pWscIE ); /* WPS_SUPPORT */
int wlFwApplyChannelSettings(WL_PRIV *); /* IEEE80211_DH */
int wlchannelSet(WL_PRIV *, int channel, CHNL_FLAGS chanflag, UINT8 initRateTable);
int wlFwSetSecurity(WL_PRIV *,	UINT8 *staaddr);
int wlFwGetNoiseLevel (WL_PRIV *, UINT16 action, UINT8 *pNoise);
BOOLEAN wlFwGetHwStatsForWlStats(WL_PRIV *, struct iw_statistics *pStats);
int wlFwSetDwdsStaMode(WL_PRIV *, UINT32 enable);
int wlFwSetFwFlushTimer(WL_PRIV *, UINT32 usecs);
#define HostCmd_CMD_SET_WEP                    0x0013
#define HostCmd_CMD_802_11_PTK                 0x0034
#define HostCmd_CMD_802_11_GTK                 0x0035


#define HostCmd_CAPINFO_DEFAULT                0x0000
#define HostCmd_CAPINFO_ESS                    0x0001
#define HostCmd_CAPINFO_IBSS                   0x0002
#define HostCmd_CAPINFO_CF_POLLABLE            0x0004
#define HostCmd_CAPINFO_CF_REQUEST             0x0008
#define HostCmd_CAPINFO_PRIVACY                0x0010
#define HostCmd_CAPINFO_SHORT_PREAMBLE         0x0020
#define HostCmd_CAPINFO_PBCC                   0x0040
#define HostCmd_CAPINFO_CHANNEL_AGILITY        0x0080
#define HostCmd_CAPINFO_SHORT_SLOT             0x0400
#define HostCmd_CAPINFO_DSSS_OFDM              0x2000


typedef struct RsnIE_t {
	UINT8    ElemId;
	UINT8    Len;
	UINT8    OuiType[4]; /* 00:50:f2:01 */
	UINT8    Ver[2];
	UINT8    GrpKeyCipher[4];
	UINT8    PwsKeyCnt[2];
	UINT8    PwsKeyCipherList[4];
	UINT8    AuthKeyCnt[2];
	UINT8    AuthKeyList[4];
} __attribute__ ((packed)) RsnIE_t;

typedef struct Rsn48IE_t {
	UINT8    ElemId;
	UINT8    Len;
	UINT8    Ver[2];
	UINT8    GrpKeyCipher[4];
	UINT8    PwsKeyCnt[2];
	UINT8    PwsKeyCipherList[4];
	UINT8    AuthKeyCnt[2];
	UINT8    AuthKeyList[4];
	UINT8    RsnCap[2];
} __attribute__ ((packed)) Rsn48IE_t;

typedef struct CfParams_t {
	UINT8    ElementId;
	UINT8    Len;
	UINT8    CfpCnt;
	UINT8    CfpPeriod;
	UINT16   CfpMaxDuration;
	UINT16   CfpDurationRemaining;
} __attribute__ ((packed)) CfParams_t;

typedef struct IbssParams_t {
	UINT8    ElementId;
	UINT8    Len;
	UINT16   AtimWindow;
} __attribute__ ((packed)) IbssParams_t;

typedef union SsParams_t {
	CfParams_t   CfParamSet;
	IbssParams_t IbssParamSet;
} __attribute__ ((packed)) SsParams_t;

typedef struct FhParams_t {
	UINT8    ElementId;
	UINT8    Len;
	UINT16   DwellTime;
	UINT8    HopSet;
	UINT8    HopPattern;
	UINT8    HopIndex;
} __attribute__ ((packed)) FhParams_t;

typedef struct DsParams_t {
	UINT8    ElementId;
	UINT8    Len;
	UINT8    CurrentChan;
} __attribute__ ((packed)) DsParams_t;

typedef union PhyParams_t {
	FhParams_t  FhParamSet;
	DsParams_t  DsParamSet;
} __attribute__ ((packed)) PhyParams_t;

typedef struct ChannelInfo_t {
	UINT8    FirstChannelNum;
	UINT8    NumOfChannels;
	UINT8    MaxTxPwrLevel;
} __attribute__ ((packed)) ChannelInfo_t;

typedef struct Country_t {
	UINT8       ElementId;
	UINT8       Len;
	UINT8       CountryStr[3];
	ChannelInfo_t  ChannelInfo[40];
} __attribute__ ((packed)) Country_t;

typedef struct ACIAIFSN_field_t
{
	UINT8 AIFSN : 4;
	UINT8 ACM : 1;
	UINT8 ACI : 2;
	UINT8 rsvd : 1;

}__attribute__ ((packed)) ACIAIFSN_field_t;

typedef  struct ECWmin_max_field_t
{
	UINT8 ECW_min : 4;
	UINT8 ECW_max : 4;
}__attribute__ ((packed))  ECWmin_max_field_t;

typedef  struct ACparam_rcd_t
{
	ACIAIFSN_field_t ACI_AIFSN;
	ECWmin_max_field_t ECW_min_max;
	UINT16 TXOP_lim;
}__attribute__ ((packed))  ACparam_rcd_t;

typedef struct WMM_param_elem_t  {
	UINT8    ElementId;
	UINT8    Len;
	UINT8    OUI[3];
	UINT8    Type;
	UINT8    Subtype;
	UINT8    version;
	UINT8    rsvd;
	ACparam_rcd_t AC_BE;
	ACparam_rcd_t AC_BK;
	ACparam_rcd_t AC_VI;
	ACparam_rcd_t AC_VO;
} __attribute__ ((packed)) WMM_param_elem_t ;

#define IEEEtypes_MAX_DATA_RATES     8
#define IEEEtypes_MAX_DATA_RATES_G  14
#define IEEEtypes_SSID_SIZE	    32


typedef struct StartCmd_t {
	IEEEtypes_MacAddr_t StaMacAddr;
	UINT8    SsId[IEEEtypes_SSID_SIZE];
	UINT8    BssType;
	UINT16   BcnPeriod;
	UINT8    DtimPeriod;
	SsParams_t  SsParamSet;
	PhyParams_t PhyParamSet;
	UINT16   ProbeDelay;
	UINT16   CapInfo;
	UINT8    BssBasicRateSet[IEEEtypes_MAX_DATA_RATES_G];
	UINT8    OpRateSet[IEEEtypes_MAX_DATA_RATES_G];
	RsnIE_t     RsnIE;
	Rsn48IE_t   Rsn48IE;
	WMM_param_elem_t  WMMParam;
	Country_t   Country;
	UINT32   ApRFType; /* 0->B, 1->G, 2->Mixed, 3->A, 4->11J */
} __attribute__ ((packed)) StartCmd_t;
typedef struct _HostCmd_DS_SET_HW_SPEC 
{
	FWCmdHdr				CmdHdr;
	UINT8					Version;					// HW revision
	UINT8					HostIf; 					// Host interface
	UINT16					NumOfMCastAdr;				// Max. number of Multicast address FW can handle
	UINT8					PermanentAddr[6];			// MAC address
	UINT16					RegionCode; 				// Region Code
	UINT32					FWReleaseNumber;			// 4 byte of FW release number, example 0x1234=1.2.3.4
	UINT32					ulFwAwakeCookie;			// Firmware awake cookie - used to ensure that the device is not in sleep mode
	UINT32		DeviceCaps; 				// Device capabilities (see above)
	//	SUPPORTED_MCS_BITMAP	DeviceMcsBitmap;				// Device supported MCS bitmap
	UINT32					RxPdWrPtr;					// Rx shared memory queue
	UINT32					NumTxQueues;				// Actual number of TX queues in WcbBase array
	UINT32					WcbBase[NUM_OF_DESCRIPTOR_DATA];	// TX WCB Rings
	UINT32					MaxAMsduSize: 2;			// Max AMSDU size (00 - AMSDU Disabled, 01 - 4K, 10 - 8K, 11 - not defined)
	UINT32					ImplicitAmpduBA: 1; 			// Indicates supported AMPDU type (1 = implicit, 0 = explicit (default))
	UINT32					disablembss:1;				// indicates mbss features disable in FW
	UINT32					hostFormBeacon:1;
	UINT32					hostFormProbeResponse:1;
	UINT32     				hostPowerSave:1;
	UINT32					hostEncrDecrMgt:1;
	UINT32					hostIntraBssOffload:1;
	UINT32					hostIVOffload:1;
	UINT32					hostEncrDecrFrame:1;
	UINT32					Reserved: 21;               // Reserved
	UINT32					TxWcbNumPerQueue;				
	UINT32					TotalRxWcb;
} __attribute__ ((packed)) HostCmd_DS_SET_HW_SPEC, *PHostCmd_DS_SET_HW_SPEC;
typedef struct _HostCmd_DS_GET_HW_SPEC {
	FWCmdHdr    CmdHdr;
	UINT8    Version;          /* version of the HW                    */
	UINT8    HostIf;           /* host interface                       */
	UINT16   NumOfWCB;         /* Max. number of WCB FW can handle     */
	UINT16   NumOfMCastAddr;   /* MaxNbr of MC addresses FW can handle */
	UINT8    PermanentAddr[6]; /* MAC address programmed in HW         */
	UINT16   RegionCode;         
	UINT16   NumberOfAntenna;  /* Number of antenna used      */
	UINT32   FWReleaseNumber;  /* 4 byte of FW release number */
	UINT32   WcbBase0;
	UINT32   RxPdWrPtr;
	UINT32   RxPdRdPtr;
	UINT32   ulFwAwakeCookie;
	UINT32   WcbBase[TOTAL_TX_QUEUES-1];
} __attribute__ ((packed)) HostCmd_DS_GET_HW_SPEC;
typedef struct tagHostCmd_BSS_START {
	FWCmdHdr    CmdHdr;
	UINT32   Enable;   /* FALSE: Disable or TRUE: Enable */
} __attribute__ ((packed)) HostCmd_DS_BSS_START;

typedef struct tagHostCmd_AP_BEACON {
	FWCmdHdr    CmdHdr;
	StartCmd_t  StartCmd;
} __attribute__ ((packed)) HostCmd_DS_AP_BEACON;
int wlFwSetMixedWpaWpa2Mode_STA(WL_PRIV *,  UINT8 *staaddr);
int wlFwSetWpaAesMode_STA(WL_PRIV *,  UINT8 *staaddr);
int wlFwSetWpaTkipMode_STA(WL_PRIV *, UINT8 *staaddr);
int wlFwSetWpaAesGroupK_STA(WL_PRIV *, 
								   UINT8 *macStaAddr_p, 
								   UINT8 *key_p, 
								   UINT16 keyLength);
int wlFwSetWpaTkipGroupK_STA(WL_PRIV *, 
							 UINT8 *macStaAddr_p, 
							 UINT8 *key_p, 
							 UINT16 keyLength,
							 UINT8 *rxMicKey_p,
							 UINT16 rxKeyLength,
							 UINT8 *txMicKey_p,
							 UINT16 txKeyLength,
							 ENCR_TKIPSEQCNT	TkipTsc,
							 UINT8 keyIndex);
int wlFwSetWpaWpa2PWK_STA(WL_PRIV *, extStaDb_StaInfo_t *StaInfo_p);
int wlFwSetNewStn(WL_PRIV *, UINT8 *staaddr,UINT16 assocId, UINT16 stnId, UINT16 action,	PeerInfo_t *pPeerInfo,UINT8 Qosinfo , UINT8 isQosSta);
int wlFwSetEdcaParam(WL_PRIV *, UINT8 Indx, UINT32 CWmin, UINT32 CWmax, UINT8 AIFSN,  UINT16 TXOPLimit);
extern int wlFwSetWep(WL_PRIV *, UINT8 *staaddr);
extern int wlFwUpdateDestroyBAStream(WL_PRIV *, UINT32 ba_type, UINT32 direction, UINT8 stream);
extern int wlFwCreateBAStream(WL_PRIV *, 	UINT32 BarThrs, UINT32 WindowSize , UINT8 *Macaddr,
							  UINT8 DialogToken, UINT8 Tid, UINT32 ba_type, UINT32 direction, UINT8 ParamInfo,
                              UINT8 *SrcMacaddr, UINT16 seqNo);
extern int wlFwSetMacAddr_Client(WL_PRIV *, UINT8 *macAddr);
extern int wlFwRemoveMacAddr(WL_PRIV *, UINT8 *macAddr);
extern int wlFwSetWpaWpa2PWK(WL_PRIV *, extStaDb_StaInfo_t *StaInfo_p);
extern int wlFwSetWpaTkipGroupK(WL_PRIV *, UINT8 index);
extern int wlFwSetWpaAesGroupK(WL_PRIV *, UINT8 index);
extern int wlFwSetRadarDetection(WL_PRIV *, UINT32 action);
extern int wlFwApplySettings(WL_PRIV *);
extern int wlFwMultiBssApplySettings(WL_PRIV *);

extern void PciWriteMacReg(WL_PRIV *,UINT32 offset, UINT32 val);
extern UINT32 PciReadMacReg(WL_PRIV *,UINT32 offset);
extern int wlRegRF(WL_PRIV *, UINT8 flag, UINT32 reg, UINT32 *val);
extern int wlRegBB(WL_PRIV *, UINT8 flag, UINT32 reg, UINT32 *val);
extern int wlFwGetBeacon(WL_PRIV *, UINT8 *pBcn, UINT16 *pLen);
extern int wlFwGetCalTable(WL_PRIV *, UINT8 annex, UINT8 index);
int wlFwGetRateTable(WL_PRIV *, UINT8 *addr, UINT8 *pRateInfo, UINT32 size);
int wlFwApplyClientSettings(WL_PRIV *);
extern int wlFwGetWatchdogbitmap(WL_PRIV *, UINT8 *bitmap);
extern int wlFwGetSeqNoBAStream(WL_PRIV *, UINT8 *, uint8_t , uint16_t *);
extern int wlFwCheckBAStream(WL_PRIV *,	UINT32 , UINT32  , UINT8 *,
					   UINT8 , UINT8 , UINT32 , int32_t , UINT8);
extern int wlFwGetConsecTxFailAddr(WL_PRIV *, IEEEtypes_MacAddr_t *addr);
extern int wlFwSetConsecTxFailLimit(WL_PRIV *, UINT32 value) ;
extern int wlFwGetConsecTxFailLimit(WL_PRIV *, UINT32 *value)   ;
int wlFwSetChannelSwitchIE(WL_PRIV *, UINT32 nextChannel, UINT32 mode, UINT32 count, CHNL_FLAGS Chanflag);
int wlFwSetWpaTkipMode(WL_PRIV *,  UINT8 *staaddr);
int wlFwSetMimoPsHt(WL_PRIV *, UINT8 *addr, UINT8 enable, UINT8 mode);
int wlFwSetNProtOpMode(WL_PRIV *, UINT8 mode);
int wlFwSetPowerSaveStation(WL_PRIV *, UINT8 StationPowerSave);
int wlFwSetWpaAesMode(WL_PRIV *,  UINT8 *staaddr);
int wlFwSetMixedWpaWpa2Mode(WL_PRIV *,  UINT8 *staaddr);
int wlFwSet11N_20_40_Switch(WL_PRIV *, UINT8 mode);
extern int wlFwSetVHTOpMode(WL_PRIV *,IEEEtypes_MacAddr_t *staaddr, UINT8 vht_NewRxChannelWidth, UINT8 vht_NewRxNss);
extern int wlFwLedOn(WL_PRIV *, UINT8 led_on);
int wlRegCAU(WL_PRIV *, UINT8 flag, UINT32 reg, UINT32 *val);
void wlFwReset(WL_PRIV *);
int wlChkAdapter(WL_PRIV *);
int wlexecuteCommand(WL_PRIV *, unsigned short cmd);
extern BOOLEAN wlSetRFChan(WL_PRIV *wlpptr,UINT32 chan);
extern BOOLEAN wlSetOpModeMCU(WL_PRIV *wlpptr,UINT32 mode);
void wlDestroySysCfg(WL_PRIV *wlpptr);

#endif /* FWCMD_H_ */

