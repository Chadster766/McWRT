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


#ifndef AP8X_FWCMD_H_
#define AP8X_FWCMD_H_

#include <asm/atomic.h>
#include <linux/types.h>
#include <linux/version.h>
#include <linux/delay.h>
#include <linux/netdevice.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/pci.h>
#include <linux/interrupt.h>
#include <linux/spinlock.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/skbuff.h>
#include <linux/delay.h>
#include <linux/if_ether.h>
#include <linux/if_arp.h>
#include <linux/init.h> 

#include "ap8xLnxIntf.h"
#include "wltypes.h"
#include "IEEE_types.h"
#include "mib.h"
#include "util.h"

#include "osif.h"
#include "wl_mib.h"
#include "wl_hal.h"
#include "qos.h"
#include "wlmac.h"

#include "hostcmd.h"
#include "wl_macros.h"
#include "wldebug.h"
#include "StaDb.h"
#include "mhsm.h"
#include "dfsMgmt.h" // MRVL_DFS
#include "domain.h" // MRVL_DFS

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

extern void wlFwCmdComplete(struct net_device *);
extern int wlFwGetHwSpecs(struct net_device *);
extern int wlFwSetRadio(struct net_device *, u_int16_t, wlpreamble_e);
extern int wlFwSetAntenna(struct net_device *, wlantennatype_e);
extern int wlFwSetRTSThreshold(struct net_device *, int);
extern int wlFwSetInfraMode(struct net_device *);
extern int wlFwSetRate(struct net_device *, wlrate_e);
extern int wlFwSetSlotTime(struct net_device *, wlslot_e);
extern int wlFwSetTxPower(struct net_device *netdev, UINT8 flag, UINT32 powerLevel);
int wlFwGettxpower(struct net_device *netdev,  UINT16 *powlist, UINT16 ch, 
        UINT16 band, UINT16 width, UINT16 sub_ch);
extern int wlsetFwPrescan(struct net_device *);
extern int wlsetFwPostscan(struct net_device *, u_int8_t *, u_int8_t);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,35)
extern int wlFwSetMcast(struct net_device *, struct netdev_hw_addr *);
#else
extern int wlFwSetMcast(struct net_device *, struct dev_mc_list *);
#endif
extern int wlFwSetMacAddr(struct net_device *);
extern int wlFwSetAid(struct net_device *, u_int8_t *, u_int16_t);
extern int wlFwSetChannel(struct net_device *, u_int8_t, CHNL_FLAGS,u_int8_t);
extern int wlgetFwStatistics(struct net_device *);
extern int wlFwSetApBeacon(struct net_device *);
extern int wlFwSetAPBss(struct net_device *, wlfacilitate_e);
extern int wlFwSetAPUpdateTim(struct net_device *, u_int16_t, Bool_e);
extern int wlFwSetAPBcastSSID(struct net_device *, wlfacilitate_e);
extern int wlsetFwApWds(struct net_device *, wlfacilitate_e);
extern int wlsetFwApBurstMode(struct net_device *, wlfacilitate_e);
extern int wlFwSetGProt(struct net_device *, wlfacilitate_e);
int wlFwSetHTGF( struct net_device *netdev, UINT32 mode);
int wlFwSetRadarDetection(struct net_device *netdev, UINT32 action);
int wlFwGetAddrValue(struct net_device *netdev,  UINT32 addr, UINT32 len, UINT32 *val, UINT16 set);
int wlFwGetAddrtable(struct net_device *netdev);
int wlFwGetEncrInfo(struct net_device *netdev, unsigned char *addr);
#ifdef WDS_FEATURE
extern int wlFwSetWdsMode(struct net_device *netdev);
#endif
int wlFwSetRegionCode(struct net_device *netdev, UINT16 regionCode);
#ifdef MRVL_WSC
extern int wlFwSetWscIE(struct net_device *netdev, u_int16_t ieType, WSC_COMB_IE_t *pWscIE );
#endif
#ifdef MRVL_WAPI
extern int wlFwSetWapiIE(struct net_device *netdev, UINT16 ieType, WAPI_COMB_IE_t *pAPPIE );
#endif
#ifdef IEEE80211_DH
int wlFwApplyChannelSettings(struct net_device *netdev);
#endif
int wlchannelSet(struct net_device *netdev, int channel, CHNL_FLAGS chanflag, u_int8_t initRateTable);
int wlFwSetSecurity(struct net_device *netdev,	u_int8_t *staaddr);
int wlFwGetNoiseLevel (struct net_device *netdev, UINT16 action, UINT8 *pNoise);
BOOLEAN wlFwGetHwStatsForWlStats(struct net_device *netdev, struct iw_statistics *pStats);
#ifdef V6FW
int wlFwSetDwdsStaMode(struct net_device *netdev, UINT32 enable);
#endif
int wlFwSetFwFlushTimer(struct net_device *netdev, UINT32 usecs);
#ifdef SSU_SUPPORT
int wlFwSetSpectralAnalysis(struct net_device *netdev, UINT32 number_of_buffers, UINT32 buffer_size, UINT32 buffer_base_addr,
                            UINT8 fft_length, UINT8 fft_skip, UINT8 adc_dec, UINT32 time);
#endif
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
	u_int8_t    ElemId;
	u_int8_t    Len;
	u_int8_t    OuiType[4]; /* 00:50:f2:01 */
	u_int8_t    Ver[2];
	u_int8_t    GrpKeyCipher[4];
	u_int8_t    PwsKeyCnt[2];
	u_int8_t    PwsKeyCipherList[4];
	u_int8_t    AuthKeyCnt[2];
	u_int8_t    AuthKeyList[4];
} __attribute__ ((packed)) RsnIE_t;

typedef struct Rsn48IE_t {
	u_int8_t    ElemId;
	u_int8_t    Len;
	u_int8_t    Ver[2];
	u_int8_t    GrpKeyCipher[4];
	u_int8_t    PwsKeyCnt[2];
	u_int8_t    PwsKeyCipherList[4];
	u_int8_t    AuthKeyCnt[2];
	u_int8_t    AuthKeyList[4];
	u_int8_t    RsnCap[2];
} __attribute__ ((packed)) Rsn48IE_t;

typedef struct CfParams_t {
	u_int8_t    ElementId;
	u_int8_t    Len;
	u_int8_t    CfpCnt;
	u_int8_t    CfpPeriod;
	u_int16_t   CfpMaxDuration;
	u_int16_t   CfpDurationRemaining;
} __attribute__ ((packed)) CfParams_t;

typedef struct IbssParams_t {
	u_int8_t    ElementId;
	u_int8_t    Len;
	u_int16_t   AtimWindow;
} __attribute__ ((packed)) IbssParams_t;

typedef union SsParams_t {
	CfParams_t   CfParamSet;
	IbssParams_t IbssParamSet;
} __attribute__ ((packed)) SsParams_t;

typedef struct FhParams_t {
	u_int8_t    ElementId;
	u_int8_t    Len;
	u_int16_t   DwellTime;
	u_int8_t    HopSet;
	u_int8_t    HopPattern;
	u_int8_t    HopIndex;
} __attribute__ ((packed)) FhParams_t;

typedef struct DsParams_t {
	u_int8_t    ElementId;
	u_int8_t    Len;
	u_int8_t    CurrentChan;
} __attribute__ ((packed)) DsParams_t;

typedef union PhyParams_t {
	FhParams_t  FhParamSet;
	DsParams_t  DsParamSet;
} __attribute__ ((packed)) PhyParams_t;

typedef struct ChannelInfo_t {
	u_int8_t    FirstChannelNum;
	u_int8_t    NumOfChannels;
	u_int8_t    MaxTxPwrLevel;
} __attribute__ ((packed)) ChannelInfo_t;

typedef struct Country_t {
	u_int8_t       ElementId;
	u_int8_t       Len;
	u_int8_t       CountryStr[3];
	ChannelInfo_t  ChannelInfo[40];
} __attribute__ ((packed)) Country_t;

typedef struct ACIAIFSN_field_t
{
	u_int8_t AIFSN : 4;
	u_int8_t ACM : 1;
	u_int8_t ACI : 2;
	u_int8_t rsvd : 1;

}__attribute__ ((packed)) ACIAIFSN_field_t;

typedef  struct ECWmin_max_field_t
{
	u_int8_t ECW_min : 4;
	u_int8_t ECW_max : 4;
}__attribute__ ((packed))  ECWmin_max_field_t;

typedef  struct ACparam_rcd_t
{
	ACIAIFSN_field_t ACI_AIFSN;
	ECWmin_max_field_t ECW_min_max;
	u_int16_t TXOP_lim;
}__attribute__ ((packed))  ACparam_rcd_t;

typedef struct WMM_param_elem_t  {
	u_int8_t    ElementId;
	u_int8_t    Len;
	u_int8_t    OUI[3];
	u_int8_t    Type;
	u_int8_t    Subtype;
	u_int8_t    version;
	u_int8_t    rsvd;
	ACparam_rcd_t AC_BE;
	ACparam_rcd_t AC_BK;
	ACparam_rcd_t AC_VI;
	ACparam_rcd_t AC_VO;
} __attribute__ ((packed)) WMM_param_elem_t ;

#define IEEEtypes_MAX_DATA_RATES     8
#define IEEEtypes_MAX_DATA_RATES_G  14
#define IEEEtypes_SSID_SIZE	    32


typedef struct StartCmd_t {
#if 1//def MBSS
	IEEEtypes_MacAddr_t StaMacAddr;
#endif
	u_int8_t    SsId[IEEEtypes_SSID_SIZE];
	u_int8_t    BssType;
	u_int16_t   BcnPeriod;
	u_int8_t    DtimPeriod;
	SsParams_t  SsParamSet;
	PhyParams_t PhyParamSet;
	u_int16_t   ProbeDelay;
	u_int16_t   CapInfo;
	u_int8_t    BssBasicRateSet[IEEEtypes_MAX_DATA_RATES_G];
	u_int8_t    OpRateSet[IEEEtypes_MAX_DATA_RATES_G];
	RsnIE_t     RsnIE;
	Rsn48IE_t   Rsn48IE;
	WMM_param_elem_t  WMMParam;
	Country_t   Country;
	u_int32_t   ApRFType; /* 0->B, 1->G, 2->Mixed, 3->A, 4->11J */
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
	u_int8_t    Version;          /* version of the HW                    */
	u_int8_t    HostIf;           /* host interface                       */
	u_int16_t   NumOfWCB;         /* Max. number of WCB FW can handle     */
	u_int16_t   NumOfMCastAddr;   /* MaxNbr of MC addresses FW can handle */
	u_int8_t    PermanentAddr[6]; /* MAC address programmed in HW         */
	u_int16_t   RegionCode;         
	u_int16_t   NumberOfAntenna;  /* Number of antenna used      */
	u_int32_t   FWReleaseNumber;  /* 4 byte of FW release number */
	u_int32_t   WcbBase0;
	u_int32_t   RxPdWrPtr;
	u_int32_t   RxPdRdPtr;
	u_int32_t   ulFwAwakeCookie;
	u_int32_t   WcbBase[TOTAL_TX_QUEUES-1];
} __attribute__ ((packed)) HostCmd_DS_GET_HW_SPEC;
typedef struct tagHostCmd_BSS_START {
	FWCmdHdr    CmdHdr;
	u_int32_t   Enable;   /* FALSE: Disable or TRUE: Enable */
} __attribute__ ((packed)) HostCmd_DS_BSS_START;

typedef struct tagHostCmd_AP_BEACON {
	FWCmdHdr    CmdHdr;
	StartCmd_t  StartCmd;
} __attribute__ ((packed)) HostCmd_DS_AP_BEACON;
int wlFwSetMixedWpaWpa2Mode_STA(struct net_device *netdev,  u_int8_t *staaddr);
int wlFwSetWpaAesMode_STA(struct net_device *netdev,  u_int8_t *staaddr);
int wlFwSetWpaTkipMode_STA(struct net_device *netdev, u_int8_t *staaddr);
int wlFwSetWpaAesGroupK_STA(struct net_device *netdev, 
								   UINT8 *macStaAddr_p, 
								   UINT8 *key_p, 
								   UINT16 keyLength);
int wlFwSetWpaTkipGroupK_STA(struct net_device *netdev, 
							 UINT8 *macStaAddr_p, 
							 UINT8 *key_p, 
							 UINT16 keyLength,
							 UINT8 *rxMicKey_p,
							 UINT16 rxKeyLength,
							 UINT8 *txMicKey_p,
							 UINT16 txKeyLength,
							 ENCR_TKIPSEQCNT	TkipTsc,
							 UINT8 keyIndex);
int wlFwSetWpaWpa2PWK_STA(struct net_device *netdev, extStaDb_StaInfo_t *StaInfo_p);
int wlFwSetNewStn(struct net_device *dev, u_int8_t *staaddr,u_int16_t assocId, u_int16_t stnId, u_int16_t action,	PeerInfo_t *pPeerInfo,UINT8 Qosinfo , UINT8 isQosSta);
int wlFwSetEdcaParam(struct net_device *netdev, u_int8_t Indx, u_int32_t CWmin, u_int32_t CWmax, u_int8_t AIFSN,  u_int16_t TXOPLimit);
extern int wlFwSetWep(struct net_device *netdev, u_int8_t *staaddr);
#ifdef AMPDU_SUPPORT
extern int wlFwUpdateDestroyBAStream(struct net_device *dev, u_int32_t ba_type, u_int32_t direction, u_int8_t stream);
extern int wlFwCreateBAStream(struct net_device *dev, 	u_int32_t BarThrs, u_int32_t WindowSize , u_int8_t *Macaddr,
							  u_int8_t DialogToken, u_int8_t Tid, u_int32_t ba_type, u_int32_t direction, u_int8_t ParamInfo,
                              u_int8_t *SrcMacaddr, UINT16 seqNo);
#endif
extern int wlFwSetMacAddr_Client(struct net_device *netdev, UINT8 *macAddr);
extern int wlFwRemoveMacAddr(struct net_device *netdev, UINT8 *macAddr);
extern int wlFwSetWpaWpa2PWK(struct net_device *netdev, extStaDb_StaInfo_t *StaInfo_p);
extern int wlFwSetWpaTkipGroupK(struct net_device *netdev, UINT8 index);
extern int wlFwSetWpaAesGroupK(struct net_device *netdev, UINT8 index);
extern int wlFwSetRadarDetection(struct net_device *netdev, UINT32 action);
extern int wlFwApplySettings(struct net_device *netdev);
extern int wlFwMultiBssApplySettings(struct net_device *netdev);

extern void PciWriteMacReg(struct net_device *netdev,UINT32 offset, UINT32 val);
extern UINT32 PciReadMacReg(struct net_device *netdev,UINT32 offset);
extern int wlFwGetHwStats(struct net_device *netdev, char *page);
extern int wlRegRF(struct net_device *netdev, UINT8 flag, UINT32 reg, UINT32 *val);
extern int wlRegBB(struct net_device *netdev, UINT8 flag, UINT32 reg, UINT32 *val);
extern int wlRegCAU(struct net_device *netdev, UINT8 flag, UINT32 reg, UINT32 *val);
extern int wlFwGetBeacon(struct net_device *netdev, UINT8 *pBcn, UINT16 *pLen);
extern int wlFwGetCalTable(struct net_device *netdev, UINT8 annex, UINT8 index);
extern int wlFwGetRateTable(struct net_device *netdev, UINT8 *addr, UINT8 *pRateInfo, UINT32 size);	
int wlFwApplyClientSettings(struct net_device *netdev);
extern int wlFwSetHwSpecs(struct net_device *netdev);
extern int wlFwGetWatchdogbitmap(struct net_device *dev, u_int8_t *bitmap);
extern int wlFwGetSeqNoBAStream(struct net_device *, u_int8_t *, uint8_t , uint16_t *);
extern int wlFwCheckBAStream(struct net_device *,	u_int32_t , u_int32_t  , u_int8_t *,
					   u_int8_t , u_int8_t , u_int32_t , int32_t , u_int8_t);
#ifdef RXPATHOPT
int wlFwSetRxPathOpt(struct net_device *netdev, UINT32 rxPathOpt);
#endif
#ifdef QUEUE_STATS
extern int wlFwGetQueueStats( struct net_device *netdev, int option);
extern int wlFwResetQueueStats( struct net_device *netdev);
extern int wlFwSetMacSa(struct net_device *netdev, int n, UINT8 *addr);
#endif

extern int wlFwGetConsecTxFailAddr(struct net_device *netdev, IEEEtypes_MacAddr_t *addr);
extern int wlFwSetConsecTxFailLimit(struct net_device *netdev, UINT32 value) ;
extern int wlFwGetConsecTxFailLimit(struct net_device *netdev, UINT32 *value)   ;
extern int wlFwSetVHTOpMode(struct net_device *netdev,IEEEtypes_MacAddr_t *staaddr, UINT8 vht_NewRxChannelWidth, UINT8 vht_NewRxNss);
#ifdef WNC_LED_CTRL
extern int wlFwLedOn(struct net_device *netdev, UINT8 led_on);
#endif
#endif /* AP8X_FWCMD_H_ */

