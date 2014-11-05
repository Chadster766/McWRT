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

#ifndef _WL_MIB_H_
#define _WL_MIB_H_


/*!
* \file    wl_mib.h
* \brief   This file contains the MIB structure definitions based on IEEE 802.11 specification.
*
*/


/*============================================================================= */
/*                    Management Information Base STRUCTURES (IEEE 802.11) */
/*============================================================================= */

#include "timer.h"
#include "hostcmd.h"
/*-----------------------------*/
/* Station Configuration Table */
/*-----------------------------*/
typedef struct MIB_StaCfg_s
{
	//   IEEEtypes_MacAddr_t StationId;
	//   UINT16 MedOccLimit;    /* 0 to 1000             */
	UINT8 CfPollable;      /* SNMP_Boolean_e values */
	UINT8 CfPeriod;        /* 0 to 255              */
	UINT16 CfpMax;         /* 0 to 65535            */
	//    UINT32 AuthRspTimeOut; /* 0 to 4294967295       */
	UINT8 PrivOption;      /* SNMP_Boolean_e values */
	//    UINT8 PwrMgtMode;      /* PwrMgmtMode_e values  */
	UINT8 DesiredSsId[33]; /* 32 byte string        */
	UINT8 DesiredBssType;  /* Bss_e values          */
	UINT8 OpRateSet[126];  /* 126 byte string       */
	UINT8 DesiredBSSId[6]; /* Desired BSSID        */

	UINT8 DtimPeriod;      /* 1 to 255              */
	//    UINT32 AssocRspTimeOut; /* 1 to 4292967295      */
	//    UINT16 DisassocReason;  /* 0 to 65535           */
	//    IEEEtypes_MacAddr_t DisassocSta;
	//    UINT16 DeauthReason;    /* 0 to 65535           */
	//    IEEEtypes_MacAddr_t DeauthSta;
	//    UINT16 AuthFailStatus;  /* 0 to 65535           */
	//    IEEEtypes_MacAddr_t AuthFailSta;
	//    UINT8 RSNOptionImplemented; /* SNMP_Boolean_e values */
	//    UINT32 TKIPNumberOfReplayCounters; // 1 to 4292967295
	//    UINT8 SelfCts;
	UINT8 OpRateSet2[126];  /* 126 byte string       */
	//    UINT8 DesiredSsId2[33]; /* 32 byte string        */
#ifdef QOS_FEATURE
	//UINT8 QoSOptImpl; //QoSOptionImplemented
	UINT8 BlckAckOptImpl; //BlockAckOptionImplemented
	UINT8 DirectOptImpl; //DirectOptionimplemented
	UINT8 APSDOptImpl;   //APSDOptionImplemented
	UINT8 QAckOptImpl;   //QAckOptionImplemented
	UINT8 QBSSLoadOptImpl; //QBSSLoadOptionImplemented
	UINT8 QReqOptImpl; //QueueRequestOptionimplemented
	UINT8 TXOPReqOptImpl;  //TXOPOptionImplemented
	UINT8 MoreDataAckOptImpl; //MoreDataAckOptionImplemented
	UINT8 AssocinNQBSS; //AssociateInNQBSS
	UINT8 NoAckMode;
#ifdef QOS_WSM_FEATURE
	UINT8 WSMQoSOptImpl; //If WSM is supported.
#endif 
#endif
#ifdef IEEE80211H
	UINT8 SpectrumManagementImplemented; /* truth value */
	UINT8 SpectrumManagementRequired; /* truth value */
#endif
	//    UINT8 mib_BoosterMode;
	UINT8 mib_preAmble;
#ifdef IEEE80211_DH
	UINT32 sta11hMode ;
#endif
#ifdef MRVL_WPS_CLIENT
	UINT32 wpawpa2Mode ;
#endif

}
MIB_STA_CFG;
#define BOOSTER_OFF  0
#define BOOSTER_ON   1
#define BOOSTER_AUTO 2



/*---------------------------------*/
/* Authentication Algorithms Table */
/*---------------------------------*/

typedef struct MIB_AuthAlg_s
{
	UINT32 Idx;
	UINT8 Type;    /* AuthType_e values */
	UINT8 Enable;  /* SNMP_Boolean_e values */
}
MIB_AUTH_ALG;



/*------------------------*/
/* WEP Default Keys Table */
/*------------------------*/

typedef struct MIB_WepDefaultKeys_s
{
	UINT8 WepDefaultKeyIdx;        /* 1 to 4 */
	UINT8 WepType;
	UINT8 WepDefaultKeyValue[13];   /* 5 byte string */
}
MIB_WEP_DEFAULT_KEYS;


/*------------------------*/
/* WEP Key Mappings Table */
/*------------------------*/

typedef struct MIB_WepKeyMappings_s
{
	UINT32 WepKeyMappingIdx;
	IEEEtypes_MacAddr_t WepKeyMappingAddr;
	UINT8 WepKeyMappingWepOn;   /* SNMP_Boolean_e values */
	UINT8 WepKeyMappingVal[13];  /* 5 byte string */
	UINT8 WepKeyMappingStatus;  /* SNMP_Rowstatus_e values */
}
MIB_WEP_KEY_MAPPINGS;


/*---------------*/
/* Privacy Table */
/*---------------*/

typedef struct MIB_PrivacyTable_s
{
	UINT8 PrivInvoked;       /* SNMP_Boolean_e values */
	UINT8 WepDefaultKeyId;   /* 0 to 3 */
	UINT32 WepKeyMappingLen;  /* 10 to 4294967295 */
	UINT8 ExcludeUnencrypt;  /* SNMP_Boolean_e values */
	UINT32 WepIcvErrCnt;
	UINT32 WepExcludedCnt;
	UINT8    RSNEnabled;      /* SNMP_Boolean_e values */
	UINT8 ApiUpdateWpa;
	UINT8 PrivInvoked_G;
#ifdef MRVL_WAPI
	UINT8 WAPIEnabled;
#endif
	UINT8 RSNLinkStatus;			//After key exchange, 0: Fail, 1: Pass
}
MIB_PRIVACY_TABLE;


/*---------------------------------*/
/* Disassociate Notificaton Object */
/*---------------------------------*/

typedef struct MIB_DisassocNot_s
{
	UINT16 DisassocReason;  /* 0 to 65535 */
	IEEEtypes_MacAddr_t DisassocSta;
}
MIB_DISASSOC_NOT;


/*-----------------------------------*/
/* Deauthenticate Notificaton Object */
/*-----------------------------------*/

typedef struct MIB_DeauthNot_s
{
	UINT16 DeauthReason;  /* 0 to 65535 */
	IEEEtypes_MacAddr_t DeauthSta;
}
MIB_DEAUTH_NOT;



/*--------------------------------------*/
/* Authenticate Fail Notificaton Object */
/*--------------------------------------*/

typedef struct MIB_AuthFailNot_s
{
	UINT16 AuthFailStatus;  /* 0 to 65535 */
	IEEEtypes_MacAddr_t AuthFailSta;
}
MIB_AUTH_FAIL_NOT;


/*============================================================================= */
/*                             MAC ATTRIBUTES */
/*============================================================================= */


/*---------------------*/
/* MAC Operation Table */
/*---------------------*/

typedef struct MIB_OpData_s
{
	IEEEtypes_MacAddr_t StaMacAddr;
	UINT8 ShortRetryLim;  /* 1 to 255 */
	UINT8 LongRetryLim;   /* 1 to 255 */
	UINT16 FragThresh;     /* 256 to 2346 */
	UINT32 MaxTxMsduLife;  /* 1 to 4294967295 */
	UINT32 MaxRxLife;      /* 1 to 4294967295 */
	UINT8 ManufId[128];   /* 128 byte string */
	UINT8 ProdId[128];    /* 128 byte string */
}
MIB_OP_DATA;


/*----------------*/
/* Counters Table */
/*----------------*/

typedef struct MIB_Counters_s
{
	UINT32 RxFrmCnt;
	UINT32 MulticastTxFrmCnt;
	UINT32 FailedCnt;
	UINT32 RetryCnt;
	UINT32 MultRetryCnt;
	UINT32 FrmDupCnt;
	UINT32 RtsSuccessCnt;
	UINT32 RtsFailCnt;
	UINT32 AckFailCnt;
	UINT32 RxFragCnt;
	UINT32 MulticastRxFrmCnt;
	UINT32 FcsErrCnt;
	UINT32 TxFrmCnt;
	UINT32 WepUndecryptCnt;
}
MIB_COUNTERS;


/*-----------------------*/
/* Group Addresses Table */
/*-----------------------*/

typedef struct MIB_GroupAddr_s
{
	UINT32 GroupAddrIdx;
	IEEEtypes_MacAddr_t Addr;
	UINT8 GroupAddrStatus;  /* SNMP_Rowstatus_e values */
}
MIB_GROUP_ADDR;


/*----------------------------*/
/* Resource Information Table */
/*----------------------------*/

typedef struct MIB_RsrcInfo_s
{
	UINT8 ManufOui[3];         /* 3 byte string */
	UINT8 ManufName[128];      /* 128 byte string */
	UINT8 ManufProdName[128];  /* 128 byte string */
	UINT8 ManufProdVer[128];   /* 128 byte string */
}
MIB_RESOURCE_INFO;

/*============================================================================= */
/*                             PHY ATTRIBUTES */
/*============================================================================= */


/*---------------------*/
/* PHY Operation Table */
/*---------------------*/
typedef struct MIB_PhyOpTable_s
{
	UINT8 PhyType;        /* SNMP_PhyType_e values */
	UINT32 CurrRegDomain;
	UINT8 TempType;       /* SNMP_TempType_e values */
}
MIB_PHY_OP_TABLE;


/*-------------------*/
/* PHY Antenna Table */
/*-------------------*/

typedef struct MIB_PhyAntTable_s
{
	UINT8 CurrTxAnt;   /* 1 to 255 */
	UINT8 DivSupport;  /* SNMP_DivSupp_e values */
	UINT8 CurrRxAnt;   /* 1 to 255 */
}
MIB_PHY_ANT_TABLE;


/*--------------------------*/
/* PHY Transmit Power Table */
/*--------------------------*/
typedef struct MIB_PhyTxPwrTable_s
{
	UINT8 NumSuppPwrLevels;  /* 1 to 8 */
	UINT16 TxPwrLevel1;       /* 0 to 10000 */
	UINT16 TxPwrLevel2;       /* 0 to 10000 */
	UINT16 TxPwrLevel3;       /* 0 to 10000 */
	UINT16 TxPwrLevel4;       /* 0 to 10000 */
	UINT16 TxPwrLevel5;       /* 0 to 10000 */
	UINT16 TxPwrLevel6;       /* 0 to 10000 */
	UINT16 TxPwrLevel7;       /* 0 to 10000 */
	UINT16 TxPwrLevel8;       /* 0 to 10000 */
	UINT8 CurrTxPwrLevel;    /* 1 to 8 */
}
MIB_PHY_TX_POWER_TABLE;


/*---------------------------------------------*/
/* PHY Frequency Hopping Spread Spectrum Table */
/*---------------------------------------------*/
typedef struct MIB_PhyFHSSTable_s
{
	UINT8 HopTime;        /* 224? */
	UINT8 CurrChanNum;    /* 0 to 99 */
	UINT16 MaxDwellTime;   /* 0 to 65535 */
	UINT16 CurrDwellTime;  /* 0 to 65535 */
	UINT16 CurrSet;        /* 0 to 255 */
	UINT16 CurrPattern;    /* 0 to 255 */
	UINT16 CurrIdx;        /* 0 to 255 */
}
MIB_PHY_FHSS_TABLE;


/*-------------------------------------------*/
/* PHY Direct Sequence Spread Spectrum Table */
/*-------------------------------------------*/
typedef enum MIB_CCAMode_s{ ENERGY_DETECT_ONLY = 1, CARRIER_SENSE_ONLY = 2, CARRIER_SENSE_AND_ENERGY_DETECT = 4} MIB_CCA_MODE;

typedef struct MIB_PhyDSSSTable_s
{
	CHNL_FLAGS Chanflag;
	UINT8 CurrChan;     /*  */
	UINT8 CurrCcaMode; /* MIB_CCA_MODE values only */
	UINT8 CurrChan2;
	UINT8 powinited;
	UINT16 maxTxPow[TX_POWER_LEVEL_TOTAL];	/* max tx power (dBm) */
	UINT16 targetPowers[TX_POWER_LEVEL_TOTAL];/* target powers (dBm) */
}
MIB_PHY_DSSS_TABLE;

typedef struct MIB_TxPowerTable_t 
{
	UINT8 Channel;
	UINT8 setcap;
	UINT16 TxPower[TX_POWER_LEVEL_TOTAL];
	UINT8 CDD;        /* 0: off, 1: on */
	UINT16 txantenna2;
}MIB_TX_POWER_TABLE;

/*--------------*/
/* PHY IR Table */
/*--------------*/
typedef struct MIB_PhyIRTable_s
{
	UINT32 CcaWatchDogTmrMax;
	UINT32 CcaWatchDogCntMax;
	UINT32 CcaWatchDogTmrMin;
	UINT32 CcaWatchDogCntMin;
}
MIB_PHY_IR_TABLE;


/*----------------------------------------*/
/* PHY Regulatory Domains Supported Table */
/*----------------------------------------*/
typedef struct MIB_PhyRegDomainsSupp_s
{
	UINT32 RegDomainsSuppIdx;
	UINT8 RegDomainsSuppVal;  /*SNMP_RegDomainsSuppVal_e values */
}
MIB_PHY_REG_DOMAINS_SUPPPORTED;


/*-------------------------*/
/* PHY Antennas List Table */
/*-------------------------*/
typedef struct MIB_PhyAntList_s
{
	UINT8 AntListIdx;
	UINT8 SuppTxAnt;   /*SNMP_Boolean_e values */
	UINT8 SuppRxAnt;   /*SNMP_Boolean_e values */
	UINT8 RxDiv;       /*SNMP_Boolean_e values */
}
MIB_PHY_ANT_LIST;


/*-----------------------------------------*/
/* PHY Supported Transmit Data Rates Table */
/*-----------------------------------------*/
typedef struct MIB_PhySuppDataRatesTx_s
{
	UINT8 SuppDataRatesTxIdx;  /*1 to IEEEtypes_MAX_DATA_RATES_G */
	UINT8 SuppDataRatesTxVal;  /*2 to 127 */
}
MIB_PHY_SUPP_DATA_RATES_TX;


/*----------------------------------------*/
/* PHY Supported Receive Data Rates Table */
/*----------------------------------------*/
typedef struct MIB_PhySuppDataRatesRx_s
{
	UINT8 SuppDataRatesRxIdx;  /*1 to 8 */
	UINT8 SuppDataRatesRxVal;  /*2 to 127 */
}
MIB_PHY_SUPP_DATA_RATES_RX;


typedef struct MIB_DHCP_s
{
	UINT32 IPAddr;
	UINT32 SubnetMask;
	UINT32 GwyAddr;
}
MIB_DHCP;
#ifdef DHCPS
typedef struct MIB_DHCPS_s
{
	UINT32 DHCPSIsEnable;        /* DHCP Server Administrative Status */
	UINT32 DHCPSRangeStartAddr;  /* DHCP Server Start address in pool */
	UINT32 DHCPSRangeEndAddr;    /* DHCP Server End address in pool   */
	UINT32 DHCPSLeaseTime;       /* DHCP Server Lease time            */
}
MIB_DHCPS;
extern MIB_DHCPS mib_DHCPS;
#endif

/* Added for WB31 */
typedef struct _MIB_WB
{
	UINT8    devName[16];        // Must be a string: 15 Max characters  
	UINT8    cloneMacAddr[6];    // cloned MAC Address                   
	UINT8    opMode;             // 0 for infrastructure, 1 for ad-hoc
	UINT8    macCloneEnable;     // boolean
}MIB_WB;
/* Added for WB31 end */


/*---------------------*/
/* RSN Config Table */
/*---------------------*/
typedef struct MIB_RSNConfig_s
{
	UINT32  Index;
	UINT32  Version;
	UINT32  PairwiseKeysSupported;
	UINT8   MulticastCipher[4];
	UINT8   GroupRekeyMethod;
	UINT32  GroupRekeyTime;
	UINT32  GroupRekeyPackets;
	UINT8   GroupRekeyStrict;
	UINT8   PSKValue[40];
	UINT8   PSKPassPhrase[64];
	UINT8   TSNEnabled;
	UINT32  GroupMasterRekeyTime;
	UINT32  GroupUpdateTimeOut;
	UINT32  GroupUpdateCount;
	UINT32  PairwiseUpdateTimeOut;
	UINT32  PairwiseUpdateCount;
}
MIB_RSNCONFIG;

/*---------------------*/
/* RSN Unicast Cipher Suites Config Table */
/*---------------------*/
typedef struct MIB_RSNConfigUnicastCiphers_s
{
	UINT32  Index;
	UINT8   UnicastCipher[4];
	UINT8   Enabled;
}
MIB_RSNCONFIG_UNICAST_CIPHERS;

/*---------------------*/
/* RSN Authentication Suites Config Table */
/*---------------------*/
typedef struct MIB_RSNConfigAuthSuites_s
{
	UINT32  Index;
	UINT8   AuthSuites[4];
	UINT8   Enabled;
}
MIB_RSNCONFIG_AUTH_SUITES;

#ifdef AP_WPA2
typedef struct mib_RSNConfigWPA2_s
{
	UINT32 Index;
	UINT32 Version;
	UINT32 PairwiseKeysSupported;
	UINT8 MulticastCipher[4];
	UINT8 GroupRekeyMethod;
	UINT32 GroupRekeyTime;
	UINT32 GroupRekeyPackets;
	UINT8 GroupRekeyStrict;
	UINT8 PSKValue[40];
	UINT8 PSKPassPhrase[64];
	UINT8 TSNEnabled;
	UINT32 GroupMasterRekeyTime;
	UINT32 GroupUpdateTimeOut;
	UINT32 GroupUpdateCount;
	UINT32 PairwiseUpdateTimeOut;
	UINT32 PairwiseUpdateCount;
	UINT32 WPA2Enabled;
	UINT32 WPA2OnlyEnabled;
	UINT32 WPA2PreAuthEnabled;
}
MIB_RSNCONFIGWPA2;

typedef struct mib_RSNConfigWPA2UnicastCiphers_s
{
	UINT32 Index;
	UINT8 UnicastCipher[4];
	UINT8 Enabled;
}
MIB_RSNCONFIGWPA2_UNICAST_CIPHERS;

typedef struct mib_RSNConfigWPA2AuthSuites_s
{
	UINT32 Index;
	UINT8 AuthSuites[4];
	UINT8 Enabled;
}
MIB_RSNCONFIGWPA2_AUTH_SUITES;

#endif

/*---------------------*/
/* RSN Statistics Table */
/*---------------------*/
typedef struct MIB_RSNStats_s
{
	UINT32  Index;
	IEEEtypes_MacAddr_t MacAddr; 
	UINT32  Version;
	UINT8   SelectedUnicastCipher[4];
	UINT32  TKIPICVErrors;
	UINT32  TKIPLocalMICFailures;
	UINT32  TKIPRemoteMICFailures;
	UINT32  TKIPCounterMeasuresInvoked;
	UINT32 WRAPFormatErrors;
	UINT32 WRAPReplays;
	UINT32 WRAPDecryptErrors;

	UINT32  CCMPFormatErrors;
	UINT32  CCMPReplays;
	UINT32  CCMPDecryptErrors;  
}
MIB_RSNSTATS;

typedef struct Mrvl_MIB_RSN_GrpKey_s
{
	UINT8  GrpMasterKey[32];
	UINT8  EncryptKey[16];
	UINT8  TxMICKey[8];
	UINT8  RxMICKey[8];
	UINT32 g_IV32;
	UINT16  g_IV16;
	UINT16 g_Phase1Key[5];
	UINT8 g_KeyIndex;
}
MRVL_MIB_RSN_GRP_KEY;

typedef struct MIB_BURST_MODE
{
	UINT8  mib_burstmode;
	UINT32 mib_burstrate;
}
MIB_BURST_MODE;


#ifdef IEEE80211H
typedef struct MIB_SPECTRUM_MGMT_S
{
#ifdef IEEE80211_DH
	UINT32  spectrumManagement; // 1: Enable, 0 : Disable
	UINT32  csaChannelNumber; // Channel in the channel switch announcement
	UINT32  csaCount; // initial count in the channel switch announcement
	UINT32  csaMode; // initial count in the channel switch announcement
	SINT32  powerConstraint; // local maximum power constraint
	UINT32  multiDomainCapability; // 1: Enable, 0 : Disable
	UINT32  countryCode;
#else
	UINT32  spectrumManagementIndex;
	SINT32  mitigationRequirement;
	UINT32  channelSwitchTime;
#endif
	SINT32  powerCapabilityMax;
	SINT32  powerCapabilityMin;
} MIB_SPECTRUM_MGMT;
#endif /* IEEE80211H*/

#define MAX_AC 4

#ifdef STA_QOS
typedef struct
{
	UINT32 QStaEDCATblIndx;
	UINT32 QStaEDCATblCWmin;
	UINT32 QStaEDCATblCWmax;
	UINT32 QStaEDCATblAIFSN;
	UINT32 QStaEDCATblTXOPLimitBSta;
	UINT32 QStaEDCATblTXOPLimit;
	UINT32 QStaEDCATblMandatory;
}MIB_QSTAEDCATABLE;
#endif
#ifdef BT_COEXISTENCE
/*---------------*/
/* BCA Config    */
/*---------------*/

typedef struct MIB_BCA_s {
	UINT8    mib_BcaEnabled;            /*  */
	UINT8    mib_Bca2W;                 /* TRUE - 2W; FALSE - 3/4W */
	UINT8    mib_BcaAntenna;            /* 0 - single; 1 - dual */
	UINT8    mib_BcaWlHighPri;          /* TRUE - WLAN has priority; FALSE - BT has priority */
	UINT32   mib_BcaWlTxPri[2];         /* */
	UINT32   mib_BcaWlRxPri[2];         /*  */
	UINT8    mib_BcaTimeshareEnabled;   /*  */
	UINT32   mib_BcaTimeshareInterval;  /*  */
	UINT32   mib_BcaTimeshareBtTime;    /*  */
	UINT32   mib_RegTsWlStateA5DC;    /*  */
	UINT32   mib_RegTsBtStateA5DC;    /*  */
	UINT32   mib_BcaWlTxOk;    /*  */
	UINT32   mib_BcaBtTxOk;    /*  */
	UINT32   mib_BcaBtSmCtrl;    /*  */
} MIB_BCA;

/*----------------*/
/* BCA Statistics */
/*----------------*/
typedef struct MIB_BCA_STATS_s {
	UINT32   mib_BcaWlTxFailCnt;            /* BCA WLAN Tx Request Fail Count */
	UINT32   mib_BcaWlTxStopCnt;            /* BCA WLAN Tx Early Stop Count */
	UINT32   mib_BcaBtTxFailCnt;            /* BCA BT Tx Request Fail Count */
} MIB_BCA_STATS;
#endif

#ifdef MPRXY
/* Multicast proxy support */

#define MAX_UNICAST_ADDRESS 16
#define MAX_MULTICAST_ADDRESS 16

typedef struct MIB_IPMCAST_GRP_TBL_s {
	UINT32 mib_McastIPAddr;
	UINT8 mib_MAddrCount;
	IEEEtypes_MacAddr_t mib_UCastAddr[MAX_UNICAST_ADDRESS];
	UINT32 mib_UcEntryTS[MAX_UNICAST_ADDRESS]; /* Unicast entry time stamp */
}MIB_IPMCAST_GRP_TBL;
#endif

#define FILERMACNUM 64
#define PWTAGETRATETABLE20M 14*4
#define PWTAGETRATETABLE40M 9*4
#define PWTAGETRATETABLE20M_5G_ENTRY	35
#define PWTAGETRATETABLE40M_5G_ENTRY	16
#define PWTAGETRATETABLE20M_5G	PWTAGETRATETABLE20M_5G_ENTRY*4
#define PWTAGETRATETABLE40M_5G	PWTAGETRATETABLE40M_5G_ENTRY*4
#define AMSDU_DISABLE 0
#define AMSDU_4K 1
#define AMSDU_8k 2
typedef struct MIB_802DOT11_s
{
	/*-----------------------------------------*/
	/* Station Management Attributes */
	/*-----------------------------------------*/
	MIB_STA_CFG *StationConfig;      /* station configuration table */
	MIB_AUTH_ALG *AuthAlg;            /* authentication algorithms table */
	MIB_WEP_DEFAULT_KEYS *WepDefaultKeys;  /* wep default keys table */
	MIB_PRIVACY_TABLE *Privacy;            /* privacy table */

	/*-----------------------------------------*/
	/* MAC Attributes */
	/*-----------------------------------------*/
	MIB_OP_DATA *OperationTable;
	MIB_PHY_DSSS_TABLE *PhyDSSSTable;
	MIB_TX_POWER_TABLE *PhyTXPowerTable[IEEE_80211_MAX_NUMBER_OF_CHANNELS];
#ifdef IEEE80211H
	MIB_SPECTRUM_MGMT	*SpectrumMagament;
#endif /* IEEE80211H*/
#ifdef BRS_SUPPORT
	UINT32 *BssBasicRateMask;
	UINT32 *NotBssBasicRateMask;
#endif
	UINT8 *QoSOptImpl;
	UINT8 *mib_rxAntenna;
	UINT8 *mib_txAntenna;
	UINT16 *mib_txAntenna2;
	UINT32 *mib_CDD;
	UINT32 *mib_acs_threshold;
	UINT8 *mib_wlanfilterno;
	UINT8 *mib_wlanfiltermac;
	UINT8 *mib_wlanfiltertype;
	UINT8 *mib_guardInterval;
	UINT8 *mib_extSubCh;
	UINT32 *mib_agingtime;
	UINT8 *mib_PadAndSnap;
	UINT8 *mib_autochannel;
	UINT8 *mib_MaxTxPwr;
	UINT8 *PowerTagetRateTable20M;
	UINT8 *PowerTagetRateTable40M;
	UINT8 *PowerTagetRateTable20M_5G;
	UINT8 *PowerTagetRateTable40M_5G;
	MIB_PHY_SUPP_DATA_RATES_TX *SuppDataRatesTx;
	MIB_RSNCONFIG *RSNConfig;
	MIB_RSNCONFIG_UNICAST_CIPHERS *UnicastCiphers;
	MIB_RSNCONFIG_AUTH_SUITES  *RSNConfigAuthSuites;
	MIB_RSNSTATS *RSNStats;
	IEEEtypes_RSN_IE_t *thisStaRsnIE;
#ifdef AP_WPA2
	MIB_RSNCONFIGWPA2 *RSNConfigWPA2;
	MIB_RSNCONFIGWPA2_UNICAST_CIPHERS *WPA2UnicastCiphers;
	MIB_RSNCONFIGWPA2_UNICAST_CIPHERS *WPA2UnicastCiphers2;
	MIB_RSNCONFIGWPA2_AUTH_SUITES *WPA2AuthSuites;
	IEEEtypes_RSN_IE_WPA2_t *thisStaRsnIEWPA2;
	IEEEtypes_RSN_IE_WPA2MixedMode_t *thisStaRsnIEWPA2MixedMode;
#endif
	UINT8 *mib_broadcastssid;
	UINT8 *mib_defaultkeyindex;
	UINT8 *mib_enableFixedRateTx;
	UINT8 *mib_FixedRateTxType;
	UINT8 *mib_txDataRate;
	UINT8 *mib_txDataRateG;
	UINT8 *mib_txDataRateA;
	UINT8 *mib_txDataRateN;
#ifdef SOC_W8864	
	UINT8 *mib_txDataRateVHT;
#endif	
	UINT8 *mib_MulticastRate;
	UINT8 *mib_MultiRateTxType;
	UINT8 *mib_ManagementRate;
	UINT16 *mib_BcnPeriod;
	UINT8 *mib_ApMode;
	UINT8 *mib_shortSlotTime;
	UINT8 *mib_forceProtectiondisable;
	UINT8 *mib_WPAPSKValueEnabled;
	UINT8 *mib_WPA2PSKValueEnabled;
	UINT8 *mib_cipherSuite;
	UINT8 *mib_wpaWpa2Mode;
	UINT8 *mib_intraBSS;
	UINT8 *pMib_11nAggrMode;
	UINT8 *mib_wmmAckPolicy;
	UINT8 *mib_htProtect;
	UINT8 *mib_ampdu_factor;
	UINT8 *mib_ampdu_density;
	UINT8 *mib_amsdutx;		
	UINT16 *mib_amsdu_maxsize;
	UINT16 *mib_amsdu_allowsize;
	UINT16 *mib_amsdu_flushtime;
	UINT8 *mib_amsdu_pktcnt;
#ifdef INTEROP
	UINT8 *mib_interop;
#endif
	UINT8 *mib_optlevel;
	UINT8 *mib_RateAdaptMode;
	UINT8 *mib_CSMode;
	UINT8 *mib_maxsta;		/*User config max sta limit for each virtual interface, default is MAX_STNS*/
	UINT32 *mib_consectxfaillimit; 	/*Limit to kick out client when consecutive tx failure cnt > limit*/
#define	LINKADAPT_CS_ADAPT_STATE_CONSERV 0
#define	LINKADAPT_CS_ADAPT_STATE_AGGR 1
#define	LINKADAPT_CS_ADAPT_STATE_AUTO_ENABLED 2
#define	LINKADAPT_CS_ADAPT_STATE_AUTO_DISABLED 3,
#ifdef WDS_FEATURE
#define MAX_WDS_PORT   6
	IEEEtypes_MacAddr_t *mib_WdsMacAddr[MAX_WDS_PORT];
#endif
	UINT8 *mib_strictWepShareKey;
	MRVL_MIB_RSN_GRP_KEY *mib_MrvlRSN_GrpKey;
	UINT32 *mib_ErpProtEnabled;
#ifdef WDS_FEATURE
	UINT8 *mib_wdsEnable;
	UINT8 *mib_wdsno;
	UINT8 *mib_wdsmac;
	UINT8 *mib_wdsBcastMode; /* indicates how broadcast pks are sent over
							 * wds, ( broadcast or unicast to other APs */
#endif // WDS_FEATURE
	UINT8 *mib_disableAssoc;
#ifdef PWRFRAC
	UINT8 *mib_TxPwrFraction;
#endif
	UINT8 *mib_psHtManagementAct;
#ifdef CLIENT_SUPPORT
	UINT8 *mib_STAMode;
	UINT8 *mib_STAMacCloneEnable;
#endif
#ifdef AMPDU_SUPPORT
	UINT8 *mib_AmpduTx;
	UINT8 *mib_rifsQNum;
#endif

#if defined ( INTOLERANT40) || defined ( COEXIST_20_40_SUPPORT)
	UINT8 *mib_FortyMIntolerant;
	UINT8 *USER_ChnlWidth;
	UINT8 *mib_HT40MIntoler;
#endif
	UINT8 *mib_regionCode ;
#ifdef MPRXY
	UINT8 *mib_MCastPrxy;
	UINT8 *mib_IPMcastGrpCount;
	MIB_IPMCAST_GRP_TBL *mib_IPMcastGrpTbl[MAX_MULTICAST_ADDRESS];
	UINT32 *mib_IPMFilteredAddress[MAX_MULTICAST_ADDRESS];
	UINT8 *mib_IPMFilteredAddressIndex;
#endif
	UINT8 *mib_Rssi; 
#ifdef RXPATHOPT
	UINT32 *mib_RxPathOpt;
#endif
#ifdef MRVL_DFS
	UINT8 *mib_CACTimeOut ;
	UINT32 *mib_NOPTimeOut ;
#endif
	UINT16 *mib_RtsThresh;	   /* 0 to 2347 */
	UINT8 *mib_HtGreenField;
	UINT8 *mib_HtStbc;
	UINT8 *mib_3x3Rate;
#ifdef DYNAMIC_BA_SUPPORT
	UINT32 *mib_ampdu_bamgmt;
	UINT32 *mib_ampdu_mintraffic[MAX_AC];
	UINT32 *mib_ampdu_low_AC_thres[MAX_AC];
#endif
#ifdef COEXIST_20_40_SUPPORT
	UINT16 *mib_Channel_Width_Trigger_Scan_Interval;
	UINT16 *mib_Channel_Transition_Delay_Factor;

#endif
	UINT8 *mib_RptrMode; /* 1:enable, 0:disable */
	UINT8 *mib_RptrDeviceType; 
	UINT32 *mib_agingtimeRptr;
	UINT32 *mib_bftype;
	UINT32 *mib_bwSignaltype;
	UINT32 *mib_weakiv_threshold;

}
MIB_802DOT11;

// MIB pointers cached in vStaInfo_t
typedef struct _sta_system_mibs
{
	MIB_STA_CFG                 * mib_StaCfg_p;
	MIB_OP_DATA                 * mib_OpData_p;
	MIB_PHY_DSSS_TABLE          * PhyDSSSTable_p;
}
STA_SYSTEM_MIBS;

typedef struct _sta_security_mibs
{
	MIB_AUTH_ALG                            * mib_AuthAlg_p;
	MIB_PRIVACY_TABLE                       * mib_PrivacyTable_p;
	MIB_RSNCONFIG                           * mib_RSNConfig_p;
	MIB_RSNCONFIG_UNICAST_CIPHERS           * mib_RSNConfigUnicastCiphers_p;
	MIB_WEP_DEFAULT_KEYS                    * mib_WepDefaultKeys_p;
	MIB_WEP_DEFAULT_KEYS                    * mib_WepDefaultKeys_G;
	MIB_AUTH_ALG                            * mib_AuthAlg_G;
	MIB_RSNSTATS                            * mib_RSNStats_p;
	IEEEtypes_RSN_IE_t *thisStaRsnIE_p;
#ifdef AP_WPA2
	MIB_RSNCONFIGWPA2                   * mib_RSNConfigWPA2_p;
	MIB_RSNCONFIGWPA2_UNICAST_CIPHERS   * mib_RSNConfigWPA2UnicastCiphers_p;
	MIB_RSNCONFIGWPA2_UNICAST_CIPHERS   * mib_RSNConfigWPA2UnicastCiphers2_p;
	MIB_RSNCONFIGWPA2_AUTH_SUITES       * mib_RSNConfigWPA2AuthSuites_p;
	IEEEtypes_RSN_IE_WPA2_t *thisStaRsnIEWPA2_p;
#endif
}
STA_SECURITY_MIBS;
// END - MIB pointers cached in vStaInfo_t

extern MIB_802DOT11 *mibSystem_p;

extern BOOLEAN mib_InitAp(MIB_802DOT11 * mib, char *addr, int phyMacId, int vMacId, int shadow);

extern void mib_Update(void);

#ifdef BRS_SUPPORT
/* Following is for Bss Basic and Not Bss Basic Rate setting */
#define MRVL_BSSBASICRATEMASK			0x02AF	/* For default setting (G mode): 1, 2, 5.5, 11, 6, 12, 24 Mbps */
#define MRVL_NOTBSSBASICRATEMASK		0x1D40	/* For default setting (G mode): 9, 18, 36, 48, 54 Mbps */ 
#define MRVL_BSSBASICRATEMASK_B			0x000F	/* Default for B mode: 1, 2, 5.5 11 Mbps */
#define MRVL_NOTBSSBASICRATEMASK_B		0x0000	/* Default for B mode: 0 */
#define MRVL_BSSBASICRATEMASK_G			0x02AF	/* Default for G mode: 1, 2, 5.5, 11, 6, 12, 24 Mbps */
#define MRVL_NOTBSSBASICRATEMASK_G		0x1D40	/* Default for G mode: 9, 18, 36, 48, 54 Mbps */
#define MRVL_BSSBASICRATEMASK_A			0x02A0	/* Default for A mode: 6, 12, 24 Mbps */
#define MRVL_NOTBSSBASICRATEMASK_A		0x1D40	/* Default for A mode: 9, 18, 36, 48, 54 Mbps */
#define MRVL_BSSBASICRATEMASK_BGN		0x000F	/* Default for BGN mode: 1, 2, 5.5 11 Mbps */
#define MRVL_NOTBSSBASICRATEMASK_BGN	0x1FE0	/* Default for BGN mode: 6, 9, 12, 18, 24, 36, 48, 54 Mbps */
#endif
#define MINTXPOWER 11 /* min power 11db */
#endif /* _WL_MIB_H_ */
