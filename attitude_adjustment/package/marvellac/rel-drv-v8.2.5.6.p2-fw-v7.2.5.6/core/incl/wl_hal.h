#ifndef _WL_HAL_H_
#define _WL_HAL_H_
#include "osif.h"
#include "apintf.h"
#include "wl_mib.h"
#include "wlvmac.h"

#include "mlme.h"

#define SZ_PHY_ADDR 6   /*!< Number of bytes in ethernet MAC address */

/* mfg data struct*/
#define SZ_BOARD_NAME       8
#define SZ_BOOT_VERSION     12
#define SZ_PRODUCT_ID       58
#define SZ_INTERNAL_PA_CFG  14
#define SZ_EXTERNAL_PA_CFG  1
#define SZ_CCA_CFG          8
#define SZ_LED              4

typedef struct _MFG_CAL_DATA
{
	UINT8 BrdDscrpt[SZ_BOARD_NAME];      /* 8 byte ASCII to descript the type and version of the board */
	UINT8 Rev;
	UINT8 PAOptions;
	UINT8 ExtPA[SZ_EXTERNAL_PA_CFG];
	UINT8 Ant;
	UINT16 IntPA[SZ_INTERNAL_PA_CFG];
	UINT8 CCA[SZ_CCA_CFG];
	UINT16 Domain;            /* Wireless domain */
	UINT16 CstmrOpts;
	UINT8 LED[SZ_LED];
	UINT16 Xosc;
	UINT8 Reserved_1[2];
	UINT16 Magic;
	UINT16 ChkSum;
	UINT8 MfgMacAddr[SZ_PHY_ADDR];     /* Mfg mac address */
	UINT8 Reserved_2[4];
	UINT8 PID[SZ_PRODUCT_ID];          /* Production ID */
	UINT8 BootVersion[SZ_BOOT_VERSION];
} MFG_CAL_DATA;
typedef enum _WL_OP_MODE
{
	WL_OP_MODE_AP,
	WL_OP_MODE_VAP,
	WL_OP_MODE_STA,
	WL_OP_MODE_VSTA
}
WL_OP_MODE;

typedef struct _WL_SYS_CFG_DATA
{
	UINT8		 Spi16AddrLen;
	UINT8		 Rsrvd[3];
	MIB_802DOT11 Mib802dot11;
	MIB_802DOT11 ShadowMib802dot11;
}
WL_SYS_CFG_DATA;
/* WDS_SUPPORT */
struct wds_port  {
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,25)
	WL_NETDEV             netDevWdsPriv;
#endif
	WL_NETDEV             *netDevWds;
	UINT8                 wdsMacAddr[6];
	void                  *pWdsDevInfo;
	UINT8                 wdsPortMode;
	UINT8                 active;
	BOOLEAN               wdsPortRegistered;
};
/* WDS_SUPPORT */

struct ap_skb_ops {
	const char *name;
	WL_BUFF *(*wlGetNewBufAndInit)(unsigned int len, WL_BUFF *old);
	WL_BUFF *(*wlBufCopy)(WL_BUFF *s);
	WL_BUFF *(*wlBufCopyExpand)(WL_BUFF *s, int newheadroom, int newtailroom);
	void (*wlBufReserve)(WL_BUFF *s, unsigned int l);
	WL_BUFF *(*wlBufUnshare)(WL_BUFF *s);
	WL_BUFF *(*wlBuffReallocHeadroom)(WL_BUFF *s, unsigned int l);
	WL_BUFF *(*wlBuffAlloc)(unsigned int len);
	unsigned char *(*wlBufPull)(WL_BUFF *wlb, unsigned int l);
	unsigned char *(*wlBufPut)(WL_BUFF *wlb, unsigned int l);
	unsigned char *(*wlBufPush)(WL_BUFF *wlb, unsigned int l);
};


struct ap_driver_ops {
	const char *name;
	const char *desc;

	void (*wlLinkMgt)(struct net_device *, UINT8 phyIndex);
	
	int (*wlResetTask)(struct net_device *);
	
	void (*wlTxDone)(struct net_device *);
	
	int (*wlMgmtTx)(struct sk_buff *skb, struct net_device *);

	int (*wlDataTx)(struct sk_buff *skb, struct net_device *);

	int (*wlDataTxUnencr)(struct sk_buff *skb, struct net_device *, void *pStaInfo);

	int (*wlxmit)(struct net_device *, struct sk_buff *skb, struct wlxmit_param *param);

	int (*wlhandlepsxmit)(struct net_device *, struct sk_buff *skb, struct wlxmit_param *param);
	
	int (*wlprocesspsq)(struct net_device *, void *pStaInfo, struct ieee80211_frame *hdr_p);
	
	void (*wlsyslog)(struct net_device *,UINT32 classlevel, const char *format, ... );
	
	void (*wlSendEvent)(struct net_device *, int, IEEEtypes_MacAddr_t *,const char *);

	int (*wlxmitmfs)(struct net_device *, int qindex, void *pStaInfo );
	
	void (*wlshowinfo)(struct net_device *);

};


typedef struct vmacApInfo_t
{
	vmacEntry_t VMacEntry;
	UINT32 OpMode; /*!< Mode of operation: client or access point */
	UINT32 CpuFreq; /* CPU frequency */
	MIB_802DOT11 *Mib802dot11; /* Initial 802.11 MIB settings */
	MIB_802DOT11 *ShadowMib802dot11; /* Initial 802.11 MIB settings */
	MFG_CAL_DATA *CalData; /*!< Calibration data */
#ifdef NEWCALDATA
	MFG_HW_INFO  *mfgHwData;
	UINT8	hwInfoRev;
#endif
	WL_NETDEV *dev;
	struct ap_driver_ops *driver;
	struct ap_skb_ops *skbops;
	WL_SYS_CFG_DATA *sysCfgData;
	Timer AgingTimer;
	Timer KeepAliveTimer;	
	Timer monTimer;
	Timer scaningTimer;
	Timer reqMeasurementTimer;
	Timer MicTimer;
	Timer GrpKeytimer;
	Timer CoexistTimer;  /* COEXIST_20_40_SUPPORT */
	SyncSrvAp mgtSync;
	BOOLEAN download;
	IEEEtypes_MacAddr_t macStaAddr;
	IEEEtypes_MacAddr_t macStaAddr2;
	IEEEtypes_SsIdElement_t macSsId;
	IEEEtypes_SsIdElement_t macSsId2;
	IEEEtypes_MacAddr_t macBssId,macBssId2;
	/* AUTOCHANNEL */
	UINT8 ChannelList[IEEE_80211_MAX_NUMBER_OF_CHANNELS];
	UINT32 autochannel[IEEEtypes_MAX_CHANNELS+IEEEtypes_MAX_CHANNELS_A];
	UINT8 autochannelstarted;
	int StopTraffic;
	/* AUTOCHANNEL */
	/* WDS_SUPPORT */
	struct wds_port 		 wdsPort[6];
	BOOLEAN wdsActive[6];
	PeerInfo_t wdsPeerInfo[6];
	int CurrFreeWdsPort;
	/* WDS_SUPPORT */
	BOOLEAN keyMgmtInitDone;
	UINT8  busyScanning;
	UINT8 gUserInitScan;
	UINT32	NumScanChannels;
	SINT32	ChanIdx;
	IEEEtypes_ScanCmd_t  ScanParams;
	UINT32 PwrSaveStnCnt;
	struct STADB_CTL *StaCtl;	
	UINT8 SmeState;
	IEEEtypes_CapInfo_t macCapInfo;
	UINT32 bOnlyStnCnt;
	UINT32 monitorcnt;
	UINT32 g_IV32; // = 0;
	UINT16 g_IV16; // = 0x0001;
	Timer MIC_Errortimer;
	UINT8 MIC_Errorstatus;
	BOOLEAN MICCounterMeasureEnabled;//indicates if counter Measures is enabled
	UINT32 MIC_ErrordisableStaAsso;//1
	UINT8 numClients;
	UINT8 nClients;
	UINT8 legClients;
	UINT8 n40MClients;
	UINT8 n20MClients;					
	UINT8 gaClients;
	UINT8 bClients;
	UINT8 txPwrTblLoaded;
	UINT8 regionCodeLoaded;
	UINT32 work_to_do;
	UINT32 txQLimit;
	UINT8 Ampdu_Rx_Disable_Flag;
	BOOLEAN InfUpFlag;		//Interface, 0: down, 1: up
	/* WPS_SUPPORT */
	WSC_BeaconIE_t thisbeaconIE ;
	WSC_ProbeRespIE_t thisprobeRespIE ;
	UINT8 WPSOn ;
	/* WPS_SUPPORT */
	struct vmacApInfo_t *rootvmac;
	UINT8 NonGFSta;
	struct ETHSTADB_CTL *EthStaCtl;
    UINT8 dfsCacExp;
	IEEEtypes_SuppRatesElement_t SuppRateSet;
	IEEEtypes_ExtSuppRatesElement_t ExtSuppRateSet;
	unsigned int mtu;
}
vmacApInfo_t;

typedef  struct _WLAN_RX_INFO
{
	UINT8    resvd0;           /* reserved */
	UINT8    Sq2;              /* Signal Quality 2 */
	UINT16   resvd1;           /* reserved */
	UINT8    resvd2;           /* reserved */
	UINT8    Sq1;              /* Signal Quality 1 */
	UINT8    Rate;             /* rate at which frame was received */
	UINT8    RSSI;             /* RF Signal Strength Indicator */
	UINT16   QosControl;       /* QoS Control field */
	UINT16   resvd3;           /* reserved */
}
PACK_END WLAN_RX_INFO;


typedef enum _MCU_OP_MODE
{
	MCU_MODE_AP,
	MCU_MODE_STA_INFRA,
	MCU_MODE_STA_ADHOC
}
MCU_OP_MODE;

#endif


