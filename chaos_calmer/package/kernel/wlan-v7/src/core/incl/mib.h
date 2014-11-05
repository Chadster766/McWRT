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
*    This file contains the definitions for the Management Informaton Base
*    (MIB).
*
* Notes:
*    The structure of the MIB is given in the IEEE 802.11 standard.
*
*/

#ifndef _MIB_H_
#define _MIB_H_
#include "wl_mib.h"

//=============================================================================
//                               INCLUDE FILES
//=============================================================================

//=============================================================================
//                            PUBLIC DEFINITIONS
//=============================================================================
#define mib_MAXIMUM_DATA_RATES  8
#define mib_MAX_DEFAULT_KEYS    4


//=============================================================================
//                          PUBLIC TYPE DEFINITIONS
//=============================================================================

/*---------------------------------------------------------------------------*/
/*                     Enumerated types used for the MIB                     */
/*---------------------------------------------------------------------------*/
typedef enum
{
	RSSI_ONLY = 1,
	SIG1_AND_RSSI_A = 2,
	RSSI_AND_POS_SIG1 = 3,
	SIG1_AND_SIG2_AND_RSSI = 4,
	SIG1_AND_RSSI_B = 5
} snmp_CcaMode_e;

//=============================================================================
//                       STATION MANAGEMENT ATTRIBUTES
//=============================================================================



//=============================================================================
//                             MAC ATTRIBUTES
//=============================================================================

/*----------------*/
/* Counters Table */
/*----------------*/
typedef struct mib_Counters_t
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
mib_Counters_t;

/*-----------------------*/
/* Group Addresses Table */
/*-----------------------*/
typedef struct mib_GroupAddr_t
{
	UINT32 GroupAddrIdx;
	IEEEtypes_MacAddr_t Addr;
	UINT8 GroupAddrStatus;  // SNMP_Rowstatus_e values
}
mib_GroupAddr_t;

/*----------------------------*/
/* Resource Information Table */
/*----------------------------*/
typedef struct mib_RsrcInfo_t
{
	UINT8 ManufOui[3];         // 3 byte string
	UINT8 ManufName[128];      // 128 byte string
	UINT8 ManufProdName[128];  // 128 byte string
	UINT8 ManufProdVer[128];   // 128 byte string
}
mib_RsrcInfo_t;


#ifdef WDS_FEATURE
#ifndef MAX_WDS_PORT
#define MAX_WDS_PORT   6
#endif
#endif // WDS_FEATURE

#ifdef WIPOD
extern UINT8 mib_wipodMode;
#endif
//=============================================================================
//                             PHY ATTRIBUTES
//=============================================================================


/*---------------------------------------------*/
/* PHY Frequency Hopping Spread Spectrum Table */
/*---------------------------------------------*/
typedef struct mib_PhyFreqHopData_t
{
	UINT8 HopTime;        // 224?
	UINT8 CurrChanNum;    // 0 to 99
	UINT16 MaxDwellTime;   // 0 to 65535
	UINT16 CurrDwellTime;  // 0 to 65535
	UINT16 CurrSet;        // 0 to 255
	UINT16 CurrPattern;    // 0 to 255
	UINT16 CurrIdx;        // 0 to 255
}
mib_PhyFreqHopData_t;

/*--------------*/
/* PHY IR Table */
/*--------------*/
typedef struct mib_PhyIRData_t
{
	UINT32 CcaWatchDogTmrMax;
	UINT32 CcaWatchDogCntMax;
	UINT32 CcaWatchDogTmrMin;
	UINT32 CcaWatchDogCntMin;
}
mib_PhyIRData_t;

/*----------------------------------------*/
/* PHY Regulatory Domains Supported Table */
/*----------------------------------------*/
typedef struct mib_PhySuppRegDomains_t
{
	UINT32 RegDomainsSuppIdx;
	UINT8 RegDomainsSuppVal;  //SNMP_RegDomainsSuppVal_e values
}
mib_PhySuppRegDomains_t;


/*----------------------------------------*/
/* PHY Supported Receive Data Rates Table */
/*----------------------------------------*/
typedef struct mib_PhySuppRxDataRates_t
{
	UINT8 SuppDataRatesRxIdx;  //1 to 8
	UINT8 SuppDataRatesRxVal;  //2 to 127
}
mib_PhySuppRxDataRates_t;

#ifdef WPA 
//=============================================================================
//                             802.11i ATTRIBUTES
//=============================================================================


/* Marvell private MIB */
typedef struct mib_MrvlRSNDataTrafficEnabled_t
{
	UINT8 StaMacAddr[6];
	UINT8 Enabled;
}
mib_MrvlRSNDataTrafficEnabled_t;

typedef struct mib_MrvlRSN_PWK_t
{
	UINT8 StaMacAddr[6];
	UINT8 EncryptKey[16];
	UINT32 TxMICKey[2];
	UINT32 RxMICKey[2];
	//UINT32  TempKey1[4];
	//UINT32  TempKey2[4];
}
mib_MrvlRSN_PWK_t;
/*
typedef struct mib_MrvlRSN_GrpKey_t
{
UINT8 EncryptKey[16];
UINT32 TxMICKey[2];
UINT32 RxMICKey[2];
//UINT32  TempKey1[4];
//UINT32  TempKey2[4];
}
mib_MrvlRSN_GrpKey_t;
*/
typedef struct mib_DHCP_t
{
	UINT32  IPAddr;
	UINT32 SubnetMask;
	UINT32 GwyAddr;
#ifdef GATEWAY    
	UINT32 PrimaryDNS;
	UINT32 SecondaryDNS;
#endif    
}
mib_DHCP_t;

#ifdef QOS_FEATURE
typedef struct
{
	UINT32 EDCATblIndx;
	UINT32 EDCATblCWmin;
	UINT32 EDCATblCWmax;
	UINT32 EDCATblAIFSN;
	UINT32 EDCATblTXOPLimit;
	UINT32 EDCATblMSDULifeTime;
	UINT32 EDCATblMandatory;

}mib_EDCATable_t;

typedef struct
{
	UINT32 QAPEDCATblIndx;
	UINT32 QAPEDCATblCWmin;
	UINT32 QAPEDCATblCWmax;
	UINT32 QAPEDCATblAIFSN;
	UINT32 QAPEDCATblTXOPLimitBAP;
	UINT32 QAPEDCATblTXOPLimit;
	//     UINT32 QAPEDCATblMSDULifeTime;
	UINT32 QAPEDCATblMandatory;
}mib_QAPEDCATable_t;

typedef struct
{
	UINT32 QStaEDCATblIndx;
	UINT32 QStaEDCATblCWmin;
	UINT32 QStaEDCATblCWmax;
	UINT32 QStaEDCATblAIFSN;
	UINT32 QStaEDCATblTXOPLimitBSta;
	UINT32 QStaEDCATblTXOPLimit;
	UINT32 QStaEDCATblMandatory;
}mib_QStaEDCATable_t;

typedef struct
{
	UINT32 QoSCounterIndx;
	UINT32 QoSTxFracCnt;
	UINT32 QoSFailedCnt;
	UINT32 QoSRetryCnt;
	UINT32 QoSMultipleRetryCnt;
	UINT32 QoSFrmDupCnt; //FrameDuplicateCount
	UINT32 QoSRTSSuccessCnt;
	UINT32 QoSRTSFailureCnt;
	UINT32 QoSAckFailureCnt;
	UINT32 QoSRcvdFragCnt;
	UINT32 QoSTxFrmCnt;
	UINT32 QoSDiscardedFrmCnt;
	UINT32 QoSMPDURecvdCnt;
	UINT32 QoSRetryRecvdCnt;

}mib_QoSCounters_t;
#endif


/* End of Marvell private MIB */




#endif

#ifdef QOS_FEATURE
extern mib_QAPEDCATable_t mib_QAPEDCATable[4];
extern mib_QStaEDCATable_t mib_QStaEDCATable[4];
#endif



#endif /* _MIB_H_ */
