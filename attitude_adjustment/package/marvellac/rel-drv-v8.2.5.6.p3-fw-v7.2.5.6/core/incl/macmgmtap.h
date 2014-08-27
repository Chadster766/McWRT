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

#ifndef _MACMGMTAP_H_
#define _MACMGMTAP_H_

/*============================================================================= */
/*                               INCLUDE FILES */
/*============================================================================= */
#include "wltypes.h"
#include "IEEE_types.h"
#include "StaDb.h"

/*============================================================================= */
/*                          PUBLIC TYPE DEFINITIONS */
/*============================================================================= */
#define macMgmtMain_802_11_MGMT_MSG_RCVD   (1<<0)
#define macMgmtMain_BCN_RCVD               (1<<1)
#define macMgmtMain_PWR_MODE_CHANGE_RCVD   (1<<2)
#define macMgmtMain_SME_MSG_RCVD           (1<<3)
#define macMgmtMain_CB_PROC_MSG_RCVD       (1<<4)
#define macMgmtMain_TIMER_EXPIRED          (1<<5) 
#define macMgmtMain_TIMER_CALLBACK         (1<<6)

typedef enum
{
	AUTHENTICATE_ACTION,
	AGING_ACTION
} macMgmtMain_Action_e;
typedef UINT8 macMgmtMain_Action_t;

typedef struct macMgmtMain_PendingData_t
{
	macMgmtMain_Action_t Action;
	IEEEtypes_MacAddr_t Addr;
	IEEEtypes_AuthType_t AuthType;
	extStaDb_State_t State;
}
macMgmtMain_PendingData_t;
//
// Structure that is used to record data associated with sent messages
// for which there is a response expected. Typically this is the data
// that is attached with a timer used to keep from waiting forever for
// a response.
//

/*---------------------------------*/
/* IEEE 802.11 Management Messages */
/*---------------------------------*/
typedef struct PACK_START macmgmtQ_MgmtMsg_t
{

	IEEEtypes_MgmtHdr_t Hdr;
	union
	{
		IEEEtypes_Bcn_t Bcn;
		IEEEtypes_DisAssoc_t DisAssoc;
		IEEEtypes_AssocRqst_t AssocRqst;
		IEEEtypes_AssocRsp_t AssocRsp;
		IEEEtypes_ReassocRqst_t ReassocRqst;
		IEEEtypes_ReassocRsp_t ReassocRsp;
		IEEEtypes_ProbeRqst_t ProbeRqst;
		IEEEtypes_ProbeRsp_t ProbeRsp;
		IEEEtypes_Auth_t Auth;
		IEEEtypes_Deauth_t Deauth;
		IEEEtypes_ActionField_t Action;   /* IEEE80211H */
		IEEEtypes_ManageActionField_t Act;
		/* QOS_FEATURE */
		//IEEEtypes_QoSActElem_t QoSAction;
		IEEEtypes_ADDTS_Req_t    AddTSReq;
		IEEEtypes_ADDTS_Rsp_t    AddTSRsp;
		IEEEtypes_DELTS_Req_t    DelTSReq;
		WSM_DELTS_Req_t          DelWSMTSReq;
		IEEEtypes_ADDBA_Req_t    AddBAReq;
		IEEEtypes_ADDBA_Rsp_t   AddBAResp;
		IEEEtypes_DELBA_t        DelBA;
		IEEEtypes_DlpReq_t       DlpReq;
		IEEEtypes_DlpResp_t      DlpResp;
		IEEEtypes_DlpTearDown_t  DlpTearDown;

	} Body;
	UINT32 FCS;
} PACK_END macmgmtQ_MgmtMsg_t;


typedef struct PACK_START macmgmtQ_MgmtMsg3_t
{                                         
	IEEEtypes_MgmtHdr3_t Hdr;
	union
	{
		IEEEtypes_Bcn_t Bcn;
		IEEEtypes_DisAssoc_t DisAssoc;
		IEEEtypes_AssocRqst_t AssocRqst;
		IEEEtypes_AssocRsp_t AssocRsp;
		IEEEtypes_ReassocRqst_t ReassocRqst;
		IEEEtypes_ReassocRsp_t ReassocRsp;
		IEEEtypes_ProbeRqst_t ProbeRqst;
		IEEEtypes_ProbeRsp_t ProbeRsp;
		IEEEtypes_Auth_t Auth;
		IEEEtypes_Deauth_t Deauth;
		IEEEtypes_ActionField_t Action;  /* IEEE80211H */
		IEEEtypes_ManageActionField_t Act;
		/* QOS_FEATURE */
		//IEEEtypes_QoSActElem_t QoSAction;
		IEEEtypes_ADDTS_Req_t    AddTSReq;
		IEEEtypes_ADDTS_Rsp_t    AddTSRsp;
		IEEEtypes_DELTS_Req_t    DelTSReq;
		WSM_DELTS_Req_t          DelWSMTSReq;
		IEEEtypes_ADDBA_Req_t    AddBAReq;
		IEEEtypes_ADDBA_Rsp_t   AddBAResp;
		IEEEtypes_DELBA_t        DelBA;
		IEEEtypes_DlpReq_t       DlpReq;
		IEEEtypes_DlpResp_t      DlpResp;
		IEEEtypes_DlpTearDown_t  DlpTearDown;
        IEEEtypes_CSIReport_t    CsiReport;

	} Body;
	UINT32 FCS;
} PACK_END macmgmtQ_MgmtMsg3_t;


typedef struct PACK_START macmgmtQ_MgmtMsg2_t
{
	IEEEtypes_MgmtHdr2_t Hdr;
	union
	{
		IEEEtypes_Bcn_t Bcn;
		IEEEtypes_DisAssoc_t DisAssoc;
		IEEEtypes_AssocRqst_t AssocRqst;
		IEEEtypes_AssocRsp_t AssocRsp;
		IEEEtypes_ReassocRqst_t ReassocRqst;
		IEEEtypes_ReassocRsp_t ReassocRsp;
		IEEEtypes_ProbeRqst_t ProbeRqst;
		IEEEtypes_ProbeRsp_t ProbeRsp;
		IEEEtypes_Auth_t Auth;
		IEEEtypes_Deauth_t Deauth;
		IEEEtypes_ActionField_t Action;  /* IEEE80211H */
		IEEEtypes_ManageActionField_t Act;
		/* QOS_FEATURE */
		//IEEEtypes_QoSActElem_t QoSAction;
		IEEEtypes_ADDTS_Req_t    AddTSReq;
		IEEEtypes_ADDTS_Rsp_t    AddTSRsp;
		IEEEtypes_DELTS_Req_t    DelTSReq;
		WSM_DELTS_Req_t          DelWSMTSReq;
		IEEEtypes_ADDBA_Req_t    AddBAReq;
		IEEEtypes_ADDBA_Rsp_t   AddBAResp;
		IEEEtypes_DELBA_t        DelBA;
		IEEEtypes_DlpReq_t       DlpReq;
		IEEEtypes_DlpResp_t      DlpResp;
		IEEEtypes_DlpTearDown_t  DlpTearDown;

	} Body;
	UINT32 FCS;
} PACK_END macmgmtQ_MgmtMsg2_t;

/* Structure used for messages placed on the MAC Management Service */
/* Task's 802.11 message queue. The fields in the structure are: */
/* 1a) MrvlHdr     - The Marvell header portion for 802.11 mgmt msgs */
/*  b) IeeeHdr & Hdr    - The IEEE header portion for 802.11 mgmt msgs */
/* 2a) Bcn         - Beacon message body */
/*  b) DisAssoc    - Disassociate message body */
/*  c) AssocRqst   - Associate request message body */
/*  d) AssocRsp    - Associate response message body */
/*  e) ReassocRqst - Reassociate request message body */
/*  f) ReassocRsp  - Reassociate response message body */
/*  g) ProbeRqst   - Probe request message body */
/*  h) ProbeRsp    - Probe response message body */
/*  i) Auth        - Authentication message body */
/*  j) Deauth      - Deauthentication message body */
/* 3)  FCS         - Frame check sequence */


/*--------------*/
/* SME Commands */
/*--------------*/
typedef struct macmgmtQ_SmeCmd_t
{
	IEEEtypes_SmeCmd_t CmdType;
	union
	{
		IEEEtypes_ScanCmd_t ScanCmd;
		/* IEEE80211H */
		IEEEtypes_MRequestCmd_t MrequestCmd;
		IEEEtypes_MReportCmd_t MreportCmd;
		IEEEtypes_ChannelSwitchCmd_t ChannelSwitchCmd;

		IEEEtypes_JoinCmd_t JoinCmd;
		IEEEtypes_AuthCmd_t AuthCmd;
		IEEEtypes_DeauthCmd_t DeauthCmd;
		IEEEtypes_AssocCmd_t AssocCmd;
		IEEEtypes_ReassocCmd_t ReassocCmd;
		IEEEtypes_DisassocCmd_t DisassocCmd;
		IEEEtypes_ResetCmd_t ResetCmd;
		IEEEtypes_StartCmd_t StartCmd;
	} Body;
	UINT32 reserved[16];
} PACK_END macmgmtQ_SmeCmd_t;
/* */
/* Structure used for messages placed on the MAC Management Service */
/* Task's SME message queue. The fields in the structure are: */
/* 1)  CmdType     - The type of SME command this message contains */
/* 2a) ScanCmd     - Scan command message body */
/*  b) JoinCmd     - Join command message body */
/*  c) AuthCmd     - Authenticate command message body */
/*  d) DeauthCmd   - Deauthenticate command message body */
/*  e) AssocCmd    - Associate command message body */
/*  f) ReassocCmd  - Reassociate commmand message body */
/*  g) DisassocCmd - Disassociate command message body */
/*  h) ResetCmd    - Reset command message body */
/*  i) StartCmd    - Start command message body */
/*----------------*/
/* Timer Messages */
/*----------------*/
typedef struct macmgmtQ_TimerMsg_t
{
	macMgmtMain_PendingData_t *PendingData_p;
	UINT8 Id;
}
macmgmtQ_TimerMsg_t;


/* */
/* Structure used for messages placed on the MAC Management Service */
/* Task's CB Processor message queue. The fields in the structure are: */
/* 1)  CmdType     - The type of CB Processor command this message contains */
/* 2a) PwrMgmtCmd  - Scan command message body */
/*  b) JoinCmd     - Join command message body */
/*  c) AuthCmd     - Authenticate command message body */
/*  d) DeauthCmd   - Deauthenticate command message body */
/*  e) AssocCmd    - Associate command message body */
/*  f) ReassocCmd  - Reassociate commmand message body */
/*  g) DisassocCmd - Disassociate command message body */
/*  h) ResetCmd    - Reset command message body */
/*  i) StartCmd    - Start command message body */

//extern pool_ID_t smeCmdPoolId;
/*============================================================================= */
/*                    PUBLIC PROCEDURES (ANSI Prototypes) */
/*============================================================================= */

/******************************************************************************
*
* Name: macMgmtQ_SmeWriteNoBlock
*
* Description:
*   This routine is called to write a message to the queue where messages
*   from the SME task are placed for the MAC Management Service Task. If
*   writing to the queue cannot immediately occur, then the routine returns
*   with a failure status (non-blocking).
*
* Conditions For Use:
*   The queue has been initialized by calling macMgmtQ_Init().
*
* Arguments:
*   Arg1 (i  ): SmeCmd_p - a pointer to the message to be placed on
*                          the queue
*
* Return Value:
*   Status indicating success or failure
*
* Notes:
*   None.
*
*****************************************************************************/
extern WL_STATUS macMgmtQ_SmeWriteNoBlock(WL_PRIV *wlpptr, macmgmtQ_SmeCmd_t *SmeCmd_p );

/******************************************************************************
*
* Name: macMgmtQ_TimerWriteNoBlock
*
* Description:
*   This routine is called to write a timer expired message on the timer
*   queue used by the MAC Management Service Task. If writing to the queue
*   cannot immediately occur, then the routine returns with a failure status
*   (non-blocking).
*
* Conditions For Use:
*   The queue has been initialized by calling macMgmtQ_Init().
*
* Arguments:
*   Arg1 (i  ): TimerMsg_p - Pointer to the message to be placed on
*                            the queue
*
* Return Value:
*   Status indicating success or failure
*
* Notes:
*   None.
*
*****************************************************************************/
extern WL_STATUS macMgmtQ_TimerWriteNoBlock( macmgmtQ_TimerMsg_t *TimerMsg_p );



/******************************************************************************
* 
* Name: macMgmAp_Init 
* 
* Description: 
*   This routine is called to initialize the the MAC Management Service Task 
*   and related components. 
* 
* Conditions For Use: 
*   None. 
* 
* Arguments: 
*   None. 
* 
* Return Value: 
*   Status indicating success or failure 
* 
* Notes: 
*   None. 
* 
*****************************************************************************/

extern WL_STATUS macMgmtAp_Init(WL_PRIV *wlpptr,UINT32 maxStns, IEEEtypes_MacAddr_t *stnMacAddr);

/******************************************************************************
* 
* Name: macMgmtAp_Start 
* 
* Description: 
*   This routine is called to start the MAC Management Service Task running. 
* 
* Conditions For Use: 
*   The task and its associated queues have been initialized by calling 
*   macMgmtAp_Init(). 
* 
* Arguments: 
*   None. 
* 
* Return Value: 
*   None. 
* 
* Notes: 
*   None. 
* 
*****************************************************************************/
extern void macMgmtAp_Start( void );


extern void macMgmtMain_TimeoutHdlr(UINT32 xx, UINT32 Data );

extern WL_STATUS txMgmtMsg(WL_PRIV *,WL_BUFF *);

extern void receiveWlanMsg(WL_PRIV *, WL_BUFF *, UINT32 rssi,BOOLEAN stationpacket);

#endif /* _MACMGMTAP_H_ */
