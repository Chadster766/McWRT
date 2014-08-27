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


#ifndef _QOS_H_
#define _QOS_H_

#ifdef QOS_FEATURE
#include "wl_mib.h"
#include "wl_hal.h"

/*Please donot include any header files in this file */

//EDCA Default Constants
//As recommeded by 802.11 Standards D7.0
#define EDCA_UPDATE_PERIOD  2 //will send EDCA Param for 2 DTIM Periods

#define AIFSN_BK 7
#define AIFSN_BE 3
#define AIFSN_VI 1
#define AIFSN_VO 1

#define MAX_AC 4

#define TXOP_LIMIT_BK   0
#define TXOP_LIMIT_BE   0
#define TXOP_LIMIT_VI   94 //assuming an OFDM g Rate
#define TXOP_LIMIT_VO   47 //assuming an OFDM Rate.

#define TXOP_LIMIT_BK_BAP   0
#define TXOP_LIMIT_BE_BAP   0
#define TXOP_LIMIT_VI_BAP   188 //assuming an OFDM g Rate
#define TXOP_LIMIT_VO_BAP   102//94 //assuming an OFDM Rate.


#define MSDU_LIFETIME   500

#define ADMISSION_CONTROL  FALSE

#define BE_CWMIN    15
#define BE_CWMAX    1023

//Access Categories
#define AC_BE_PRIO 0
#define AC_BK_PRIO 1
#define AC_VI_PRIO 2
#define AC_VO_PRIO 3

//Access Category Index
#define AC_BE_ACI 0
#define AC_BK_ACI 1
#define AC_VI_ACI 2
#define AC_VO_ACI 3


#define AC_BK_Q 0
#define AC_BE_Q 1
#define AC_VI_Q 2
#define AC_VO_Q 3
#define CAP_BK_Q 4
#define CAP_BE_Q 5
#define CAP_VI_Q 6
#define CAP_VO_Q 7

//QOS Info Default Params
#define PROCESS_TXOP_REQ    1 ////We can process TxOp Request.
#define PROCESS_QUEUE_REQ   1 //We can process non-zero Queue Size.

#define MAX_QOS_STN 15 //Maximum number of Qos Stations.
#define MAX_QOS_STRMS 64 //Maximum # of QoS streams that will be supported
#define MAX_ALLOW_MSDU_SIZE 2304
#define MAX_PRI   7
#define MAX_DLP_STN 5
//TCLAS Info
#define MAX_TCLAS_PER_STN 6
#define ETHERNET_PARAM 0
#define IP_PARAM    1
#define IEEE802_1Q_PARAM 2

#define QOS_SUBTYPE   0x8

#define DEFAULT_SPEC_INTERVAL 100 //100ms. In TU units (1024us = 1TU)

#define DELTS_TIMEOUT 45// <= 10secs on a timer of 200ms

#define MAX_QOS_BW 24000000 //Maximum QoS BW allowed for admission Control


#define LOWEST_TSID 8
#define HIGHEST_TSID 15
#define FLAG_QOS_STA    0x00000001
#define FLAG_EDCA_PKT   0x00000002
extern UINT8 WiFiOUI[3];
extern UINT8 B_COMP_OUI[3];
extern UINT8 I_COMP_OUI[3];

/* Qos sw info flag definitions */
#define QOS_STA_MSK    0x00000001
#define EDCA_PKT_MSK   0x00000002

//Lengths of different elements.
#define WME_INFO_LEN 7
#define WME_PARAM_LEN 24
#define WSM_QOS_CAP  7
#define QOS_CAP 1
#define WSM_DELAY_LEN 4
#define WSM_SCHEDULE_LEN 18



typedef struct QosSwInfo_
{
	UINT32 Pri;
	UINT32 gTid;
	UINT32 flags;
}QosSwInfo_t;

//=============================================================================
//                         PUBLIC VARIABLES
//============================================================================
extern UINT32 EDCA_param_set_update_cnt; //Keeps track of any update in the EDCA Param
extern UINT32 EDCA_Beacon_Counter; //Keeps track of how many beacon sent with EDCA Param
#ifdef QOS_WSM_FEATURE
#ifdef STA_QOS
extern WSM_QoS_Cap_Elem_t gThisStaWSMQoSCapElem;
extern WSM_QoS_Cap_Elem_t gThisStaWMEQoSCapElem;
#endif
#endif
//UINT32 DELTSTimeoutCounter; // to count till DeltsTimeout
extern UINT8 *BcnWMEInfoElemLocation_p;
extern UINT8 *BcnWMEParamElemLocation_p;
extern UINT8 *PrbWMEParamElemLocation_p;
extern UINT8 *PrbWMEInfoElemLocation_p;
#ifndef STA_QOS
typedef enum
{
	NORMAL_ACK,
	NO_ACK,
	NO_EXPLICIT_ACK,
	BLCK_ACK
}IEEEtypes_AckPolicy_e;

typedef enum
{
	UPLINK,
	DOWNLINK,
	DIRECTLINK,
	BIDIRLINK
}Direction_e;

typedef enum
{
	ADDBA_REQ,
	ADDBA_RESP,
	DELBA
}IEEEtypes_BA_Act_e;

typedef enum
{
	DLP_REQ,
	DLP_RESP,
	DLP_TEAR_DOWN
}IEEEtypes_DLP_Act_e;


typedef enum
{
	VHT_COMPRESSED_BF,
	GROUP_ID_MGMT,
	OPERATING_MODE_NOTIFICATION
}IEEEtypes_VHT_Act_e;


typedef enum
{
	None,
	QoS,//Traffic Stream Setup
	DLP,//Direct link Protocol
	BlkAck, //Block Ack
	VHT=21		
}IEEEtypes_QoS_Category_e;

typedef enum
{
	DELAYED,
	IMMEDIATE
}IEEEtypes_QoS_BA_Policy_e;

typedef enum
{
	RSVD,
	EDCA,
	HCCA,
	BOTH
}IEEEtypes_QoS_Access_Policy_e;

typedef enum
{
	WSM_CAPABILITY = 5,
	WSM_TCLAS,
	WSM_TCLAS_PROCESSING,
	WSM_TS_DELAY,
	WSM_SCHED,
	WSM_ACTN_HDR
}WSM_OUI_SubType_e;

/*
typedef PACK_START struct
{
UINT16 rsvd : 12;
UINT16 TID : 4;
}PACK_END BA_Cntrl_t;
*/
typedef PACK_START struct
{
	IEEEtypes_FrameCtl_t FrmCtl;
	UINT16 DurationId;
	IEEEtypes_MacAddr_t RA;
	IEEEtypes_MacAddr_t TA;
	BA_Cntrl_t BA_Cntrl;
	UINT16 Start_Seq_Cntrl;
	UINT8 bitmap[128];
}PACK_END Block_Ack_t;

typedef PACK_START struct
{
	IEEEtypes_FrameCtl_t FrmCtl;
	UINT16 DurationId;
	IEEEtypes_MacAddr_t RA;
	IEEEtypes_MacAddr_t TA;
	BA_Cntrl_t BAR_Cntrl;
	UINT16 Start_Seq_Cntrl;
}PACK_END Block_Ack_Req_t;


typedef PACK_START struct
{
	UINT8 ElementId;
	UINT8 Len;
	UINT16 sta_cnt;
	UINT8 channel_util; /*channel utilization*/
	UINT16 avail_admit_cap; /*available admission capacity*/
}PACK_END QBSS_load_t;

typedef PACK_START struct
{
	UINT8 AIFSN : 4;
	UINT8 ACM : 1;
	UINT8 ACI : 2;
	UINT8 rsvd : 1;

}PACK_END ACI_AIFSN_field_t;

typedef PACK_START struct
{
	UINT8 ECW_min : 4;
	UINT8 ECW_max : 4;
}PACK_END ECW_min_max_field_t;

typedef PACK_START struct
{
	ACI_AIFSN_field_t ACI_AIFSN;
	ECW_min_max_field_t ECW_min_max;
	UINT16 TXOP_lim;
}PACK_END AC_param_rcd_t;


typedef PACK_START struct
{
	UINT8 ElementId;
	UINT8 Len;
	QoS_Info_t QoS_info;
	UINT8 rsvd;
	AC_param_rcd_t AC_BE;
	AC_param_rcd_t AC_BK;
	AC_param_rcd_t AC_VI;
	AC_param_rcd_t AC_VO;

}PACK_END EDCA_param_set_t;
#endif
#ifdef QOS_WSM_FEATURE
#ifndef STA_QOS
#ifdef WMM_PS_SUPPORT
typedef PACK_START struct
{
	UINT8 Uapsd_ac_vo:1; /*EDCA parameter set update count*/
	UINT8 Uapsd_ac_vi : 1;
	UINT8 Uapsd_ac_bk : 1;
	UINT8 Uapsd_ac_be : 1;
	UINT8 Reserved: 1;
	UINT8 Max_Sp: 2;
	UINT8 Reserved2:1;
}PACK_END QoS_WmeInfo_Info_t;
#endif

typedef PACK_START struct
{
	UINT8 ElementId;
	UINT8 Len;
	OUI_t OUI;
	UINT8 version;
#ifdef WMM_PS_SUPPORT
	QoS_WmeInfo_Info_t QoS_info;
#else
	QoS_Info_t QoS_info;
#endif
}PACK_END WME_info_elem_t;



typedef PACK_START struct
{
	UINT8 ElementId;
	UINT8 Len;
	OUI_t OUI;
	UINT8 version;
	QoS_Info_t QoS_info;
	UINT8 rsvd;
	AC_param_rcd_t AC_BE;
	AC_param_rcd_t AC_BK;
	AC_param_rcd_t AC_VI;
	AC_param_rcd_t AC_VO;
}PACK_END WME_param_elem_t;
#endif

typedef struct
{
	IEEEtypes_MacAddr_t src_addr; //address of the Sta
	UINT32 gTID; //not sure if we need this. Will remove later--Rahul
	Mrvl_TSPEC_t TSpec;
	BOOLEAN not_free; /*indicates if this TSpec table is free to use*/
	UINT32 DelTS_PktCnt; //For deciding how many pkts tx during that Delts timeout interval.
	UINT32 TxOpIndex;
}TSPECEntry_t;

#endif//QOS_WSM_FEATURE

typedef /*PACK_START*/ struct
{
	UINT32 gTID; //this will be initialised to 0xffffffff.
	TCLAS_t TCLAS;
}/*PACK_END*/ TCLASEntry_t; 

typedef /*PACK_START*/ struct
{
	UINT32 gTID; //this will be initialised to 0xffffffff.
	WSM_TCLAS_Elem_t TCLAS;
}/*PACK_END*/ WSM_TCLASEntry_t;

#ifndef STA_QOS

typedef PACK_START struct
{
	UINT8 ElementId;
	UINT8 Len;
	UINT32 delay;
}PACK_END TS_delay_t;

typedef PACK_START struct
{
	UINT8 ElementId;
	UINT8 Len;
	UINT8 processing;

}PACK_END TCLAS_Processing_t;

typedef PACK_START struct
{
	UINT8 ElementId;
	UINT8 Len;
	QoS_Info_t QoS_info;

}PACK_END QoS_Cap_Elem_t;

typedef enum
{
	ADDTS_REQ,
	ADDTS_RSP,
	DELTS,
	QOS_SCHEDULE
}QoS_Act_Elem_e;
#endif
typedef PACK_START struct
{
	UINT32 gTID;
	UINT32 TXOP;
	UINT32 mySI;
	UINT8 mySI_slot;
	UINT8 npckts; //# of pckts to be tx
	UINT8 usr_priority; //priority of the stream
	UINT8 direction;
	BOOLEAN not_free; //indicates if element is free
	UINT16 Aid;
	UINT8 ClientMode;

}PACK_END TXOP_t;



typedef /*PACK_START*/ struct 
{
#ifdef WSM_QOS_FEATURE
	WSM_TCLASEntry_t TCLASEntry[MAX_TCLAS_PER_STN];
	TCLAS_Processing_t TCLAS_Processing;
#else
	TCLASEntry_t TCLASEntry[MAX_TCLAS_PER_STN];
	TCLAS_Processing_t TCLAS_Processing;
#endif
	QoS_Info_t QoS_Info;
	UINT8    IsStaWSMQSTA;
	UINT8    TSpecCnt;
	UINT8    TClassCnt;
	UINT32   DefaultgTID;
}/*PACK_END*/ Qos_Stn_Data_t;

typedef enum
{
	NON_QOS_PKT,
	EDCA_PKT,
	TSPEC_PKT
} PktAccPolicy_t;
#ifdef STA_QOS
extern Qos_Stn_Data_t Qos_Stn_Data[MAX_QOS_STN];

typedef struct
{
	IEEEtypes_MacAddr_t StaAddr;
	IEEEtypes_CapInfo_t macCapInfo; /* Save this from Start command */
	UINT16  Timeout_val;
	IEEEtypes_DataRate_t bOpRateSet[4];
	IEEEtypes_DataRate_t gOpRateSet[8];
	UINT8 notFree;
}DlpDb_t;


typedef struct
{
	WSM_TSPEC_t TSpec;
	UINT8 isTclasPresent;
	WSM_TCLAS_Elem_t WSM_TCLAS_Elem;
}Tspec_From_Web_t;
#endif


/******************************************************************************
* 
* Name: GetLog 
* 
* Description: Will return the ceiling of the logarithm of the argument
*     
* 
* Conditions For Use: 
*    . 
* 
* Arguments: 
*    a 32 bit number. 
*                                 
* Return Value: 
*    a 32 bit Integer. If it returns zero, it is an error. 
*    If num=1; then  return 1.
* 
* Notes: 
*    None. 
* 
* PDL: 
*
* END PDL 
* 
*****************************************************************************/
UINT32 GetLog(UINT32 num);

/******************************************************************************
* 
* Name: GetChannelCapacity 
* 
* Description: Will return channel capacity used.
*     
* 
* Conditions For Use: 
*    . 
* 
* Arguments: 
*    . 
*                                 
* Return Value: 
*    A number in units of 32 micorseconds..
* 
* Notes: 
*    None. 
* 
* PDL: 
*
* END PDL 
* 
*****************************************************************************/
UINT16 GetChannelCapacity(void);

/******************************************************************************
* 
* Name: Qos_GetMatchingTid 
* 
* Description: Will return TID.
*     
* 
* Conditions For Use: 
*    When data pkt arrives at ethernet receive. 
* 
* Arguments: 
*    apio_bufdescr_t *pBufDescr
*    UINT32 Aid of associated station                            
* Return Value: 
*    TID for the pkt. If no TID, return 0xffffffff
*    Will return the priority for pkt if no TCLAS found but still a QoS Pkt.
* 
* Notes: 
*    None. 
* 
* PDL: 
*
* END PDL 
* 
*****************************************************************************/
Status_e DelTxOpEntry(UINT32 Tid);


UINT32 FindTid(IEEEtypes_MacAddr_t *pStaAddr, UINT32 TsId);
void ScheduleHccaPkts(void);
Mrvl_TSPEC_t * GetTspec(IEEEtypes_MacAddr_t *pAddr, UINT8 req_tsid);

/******************************************************************************
* 
* Name: QoS_UpdateEDCAParameters 
* 
* Description: 
*   This routine will update the EDCA_param_set_update_cnt
* 
* Conditions For Use: 
*   If the scheduler or user decides to change the EDCA parameters. 
* 
* Arguments: 
*   None for now.  
* 
* Return Value:
* 1 if success. 0 if fail 
*   
* 
* Notes: 
*    
* Owner: Milind
*
* PDL:      * 
*****************************************************************************/
//UINT32 QoS_UpdateEDCAParameters(vmacApInfo_t *vmacSta_p);
extern UINT16 Qos_UpdateWSMQosCapElem(vmacApInfo_t *vmacSta_p,UINT8 *pBcnBuf);
extern UINT16 QoS_AppendWMEParamElem(vmacApInfo_t *vmacSta_p,UINT8 *pBcnBuf);
extern void QoS_ReAppendWMEInfoElem(vmacApInfo_t *vmacSta_p);


extern BOOLEAN GetEther_QueueData(UINT8 **frame, int **pktFilterSel, int *len);

extern UINT16 ProcessADDTSRequest(vmacApInfo_t *vmacSta_p,IEEEtypes_ADDTS_Req_t *AddTSReq_p,
								  IEEEtypes_MacAddr_t *pStaAddr, UINT16 Aid,
								  UINT32 *TspecIndx, UINT8);

extern UINT32 ClearQoSDB(IEEEtypes_MacAddr_t *);
extern Status_e ProcessDELTSRequest(vmacApInfo_t *vmacSta_p,IEEEtypes_MacAddr_t *, UINT32 );
extern UINT8 ProcessADDTSRequestSchedule(IEEEtypes_ADDTS_Rsp_t *, UINT32);
void SendDelBA(vmacApInfo_t *vmacSta_p,IEEEtypes_MacAddr_t , UINT8);


extern TXOP_t * GetTxOpFrmTspecIndx(UINT32 Indx);


UINT8 Qos_GetDSCPPriority(UINT8 *Databufptr);

void SendAddBAReq(vmacApInfo_t *vmacSta_p,IEEEtypes_MacAddr_t StaAddrA, UINT8 tsid,
				  IEEEtypes_QoS_BA_Policy_e BaPolicy, UINT32 SeqNo, UINT8 DialogToken);
extern void SendDelBA2(vmacApInfo_t *vmacSta_p,IEEEtypes_MacAddr_t StaAddr, UINT8 tsid);
extern UINT16 AddWMEParam_IE(WME_param_elem_t * pNextElement);		   
extern void InitWMEParamElem(vmacApInfo_t *vmacSta_p);
extern UINT8 AccCategoryQ[8];
#endif //QOS_FEATURE
#endif
