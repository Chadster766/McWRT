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

#ifndef _MACMGMTMLME_H_
#define _MACMGMTMLME_H_

#include "wltypes.h"
#include "IEEE_types.h"
#include "mib.h"
#include "wl_hal.h"

#include "StaDb.h"
#include "qos.h"
#include "wlmac.h"

#include "ds.h"
#include "keyMgmtCommon.h"
#include "keyMgmt.h"
#include "timer.h"
#include "tkip.h"

#include "smeMain.h"
#include "macmgmtap.h"
//=============================================================================
//                               INCLUDE FILES
//=============================================================================

//=============================================================================
//                            PUBLIC DEFINITIONS
//=============================================================================

#define ADD_TO_POLLING_LIST 1
#define DONOT_ADD_TO_POLLING_LIST 0
#define AID_PREFIX 0xC000
#ifdef ENABLE_RATE_ADAPTATION
#define RATE_ADAPT_STNCNT_THRESH   5
#endif
#define SLOT_TIME_MODE_SHORT 0
#define SLOT_TIME_MODE_LONG  1

#define NUM_MARGINS 6 
//=============================================================================
//                          PUBLIC TYPE DEFINITIONS
//=============================================================================
//
// Structure used to store data given at the time of successful
// association
//
typedef struct AssocReqData_t
{
	IEEEtypes_CapInfo_t CapInfo;
	IEEEtypes_ListenInterval_t ListenInterval;
	IEEEtypes_SuppRatesElement_t SuppRates;
	IEEEtypes_ExtSuppRatesElement_t ExtSuppRates;
	UINT32 SuppRateSetRegMap[IEEEtypes_MAX_DATA_RATES];
	UINT32 HighestRateIndex;
#ifdef ENABLE_RATE_ADAPTATION
	UINT32 RateToBeUsedForTx;
#ifdef ENABLE_RATE_ADAPTATION_BASEBAND
	UINT32                        TxFailures;
	UINT32                        InvAlpha; //to get running average.
	UINT32                        TxSuccess; //For Rate Adaptation
	UINT32                        TxRetry; //For Rate Adaptation.
#ifdef ENABLE_OSCILLATION_CODE
	UINT32 OSC_FLAG; //if 1, correction won't be decremented.
	SINT32 RSSI_Store;
	//Since recording the RSSI is not perfect, we will use this 
	UINT32 DisableOscFlagCounter;
#endif //ENABLE_OSCILLATION_CODE
	//For maintaining the statistical counter for logging failures.
	UINT32 PktTxSinceFail;
	UINT32 TxFailuresStats;
	UINT32 RateIncreaseIncrement;
	UINT32 AvgRSSI;
	UINT32 TxPkts;
	UINT32 RA_per_timer;  //Keeps track of the time
	UINT32 RAIdleCnt;
	UINT32 AvgSADRSSI;
	UINT32 Power_Level;
	UINT32 BailOutRate; //If 1 means taht we had to go down to BailOutRate
#endif //ENABLE_RATE_ADAPTATION_BASEBAND
#endif  //ENABLE_RATE_ADAPTATION

	/* QOS_FEATURE */
	Qos_Stn_Data_t Qos_Stn_Data;
	/* QOS_FEATURE */
} AssocReqData_t;

//=============================================================================
//                                PUBLIC DATA
//=============================================================================

extern AssocReqData_t AssocTable[MAX_AID + 1];
extern UINT32 HighestBasicRateIndex;
extern BOOLEAN macWepEnabled;
extern UINT32 AssocStationsCnt;
extern UINT32 macChangeSlotTimeModeTo;
extern UINT32 macCurSlotTimeMode;
extern UINT32 AssocStationsCnt;
extern UINT32 HighestBasicRateIndexB;
extern IEEEtypes_MacAddr_t bcast; // = {0xff,0xff,0xff,0xff,0xff,0xff};





//=============================================================================
//                    PUBLIC PROCEDURES (ANSI Prototypes)
//=============================================================================

/******************************************************************************
*
* Name: macMgmtMlme_AssociateReq
*
* Description:
*   Routine to handle a received associate reqeust.
*
* Conditions For Use:
*   All software components have been initialized and started.
*
* Arguments:
*   Arg1 (i  ): MgmtMsg_p - Pointer to an associate request
*
* Return Value:
*   None.
*
* Notes:
*   None.
*
*****************************************************************************/
extern void macMgmtMlme_AssociateReq( WL_PRIV *wlpptr,macmgmtQ_MgmtMsg3_t *MgmtMsg_p, UINT32 msgSize );

/******************************************************************************
*
* Name: macMgmtMlme_AssociateRsp
*
* Description:
*   This routine handles a response from an AP to a prior associate request.
*
* Conditions For Use:
*   All software components have been initialized and started.
*
* Arguments:
*   Arg1 (i  ): MgmtMsg_p - Pointer to an 802.11 management message
*                           containing an associate response
*
* Return Value:
*   None.
*
* Notes:
*   None.
*
*****************************************************************************/
extern void macMgmtMlme_AssociateRsp( macmgmtQ_MgmtMsg_t *MgmtMsg_p );

/******************************************************************************
*
* Name: macMgmtMlme_Atim
*
* Description:
*   This routine handles an ATIM sent from another station in an IBSS.
*
* Conditions For Use:
*   All software components have been initialized and started.
*
* Arguments:
*   Arg1 (i  ): MgmtHdr_p - Pointer to an 802.11 management message header
*                           that contains an ATIM message
*
* Return Value:
*   None.
*
* Notes:
*   Only the 802.11 message header is input since the message body for an
*   ATIM message is empty.
*
*****************************************************************************/
extern void macMgmtMlme_Atim( IEEEtypes_MgmtHdr_t *MgmtHdr_p );

/******************************************************************************
*
* Name: macMgmtMlme_AuthenticateCmd
*
* Description:
*   Routine to handle a command to carry out an authentication with another
*   station or an AP.
*
* Conditions For Use:
*   All software components have been initialized and started.
*
* Arguments:
*   Arg1 (i  ): AuthCmd_p - Pointer to an authenticate command
*
* Return Value:
*   None.
*
* Notes:
*   None.
*
*****************************************************************************/
extern void macMgmtMlme_AuthenticateCmd( IEEEtypes_AuthCmd_t *AuthCmd_p );

/******************************************************************************
*
* Name: macMgmtMlme_AuthenticateMsg
*
* Description:
*   This routine handles a message from either another station of from an
*   AP relating to authentication; the message can be a request for
*   authentication or a response to a prior authentication message.
*
* Conditions For Use:
*   All software components have been initialized and started.
*
* Arguments:
*   Arg1 (i  ): MgmtMsg_p - Pointer to an 802.11 management message
*                           containing an authentication message
*
* Return Value:
*   None.
*
* Notes:
*   None.
*
*****************************************************************************/
extern void macMgmtMlme_AuthenticateMsg( macmgmtQ_MgmtMsg_t *MgmtMsg_p );

/******************************************************************************
*
* Name: macMgmtMlme_BeaconMsg
*
* Description:
*   This routine handles a beacon message received from an AP or station
*
* Conditions For Use:
*   All software components have been initialized and started.
*
* Arguments:
*   Arg1 (i  ): MgmtMsg_p - Pointer to an 802.11 management message
*                           containing an beacon message
*
* Return Value:
*   None.
*
* Notes:
*   None.
*
*****************************************************************************/
extern void macMgmtMlme_BeaconMsg( macmgmtQ_MgmtMsg_t *MgmtMsg_p );

/******************************************************************************
*
* Name: macMgmtMlme_DeauthenticateMsg
*
* Description:
*   This routine handles a deauthentication notification from another
*   station or an AP.
*
* Conditions For Use:
*   All software components have been initialized and started.
*
* Arguments:
*   Arg1 (i  ): MgmtMsg_p - Pointer to an 802.11 management message
*                           containing a deauthentication message
*
* Return Value:
*   None.
*
* Notes:
*   None.
*
*****************************************************************************/
extern void macMgmtMlme_DeauthenticateMsg(WL_PRIV *wlpptr,
										  macmgmtQ_MgmtMsg3_t *MgmtMsg_p, UINT32 msgSize );

/******************************************************************************
*
* Name: macMgmtMlme_DisassociateCmd
*
* Description:
*   Routine to handle a command to carry out a disassociation with an AP.
*
* Conditions For Use:
*   All software components have been initialized and started.
*
* Arguments:
*   Arg1 (i  ): DisassocCmd_p - Pointer to a disassociate command
*
* Return Value:
*   None.
*
* Notes:
*   None.
*
*****************************************************************************/
extern void macMgmtMlme_DisassociateCmd(WL_PRIV *wlpptr,
										IEEEtypes_DisassocCmd_t *DisassocCmd_p );

/******************************************************************************
*
* Name: macMgmtMlme_DisassociateMsg
*
* Description:
*   This routine handles a disassociation notification from an AP.
*
* Conditions For Use:
*   All software components have been initialized and started.
*
* Arguments:
*   Arg1 (i  ): MgmtMsg_p - Pointer to an 802.11 management message
*                           containing a disassociation message
*
* Return Value:
*   None.
*
* Notes:
*   None.
*
*****************************************************************************/
extern void macMgmtMlme_DisassociateMsg(WL_PRIV *wlpptr, macmgmtQ_MgmtMsg3_t *MgmtMsg_p, UINT32 msgSize );

/******************************************************************************
*
* Name: macMgmtMlme_JoinCmd
*
* Description:
*   Routine to handle a command to join a BSS found during a scan.
*
* Conditions For Use:
*   All software components have been initialized and started.
*
* Arguments:
*   Arg1 (i  ): JoinCmd_p - Pointer to a join command
*
* Return Value:
*   None.
*
* Notes:
*   None.
*
*****************************************************************************/
extern void macMgmtMlme_JoinCmd( IEEEtypes_JoinCmd_t *JoinCmd_p );

/******************************************************************************
*
* Name: macMgmtMlme_ProbeRqst
*
* Description:
*   This routine handles a request from another station in an IBSS to
*   respond to a probe.
*
* Conditions For Use:
*   All software components have been initialized and started.
*
* Arguments:
*   Arg1 (i  ): MgmtMsg_p - Pointer to an 802.11 management message
*                           containing a probe request
* Return Value:
*   None.
*
* Notes:
*   None.
*
*****************************************************************************/
extern void macMgmtMlme_ProbeRqst(WL_PRIV *wlpptr, macmgmtQ_MgmtMsg3_t *MgmtMsg_p );

/******************************************************************************
*
* Name: macMgmtMlme_ProbeRsp
*
* Description:
*   This routine handles a response from another station in an IBSS or an
*   AP in a BSS to a prior probe request.
*
* Conditions For Use:
*   All software components have been initialized and started.
*
* Arguments:
*   Arg1 (i  ): MgmtMsg_p - Pointer to an 802.11 management message
*                           containing a probe response
*
* Return Value:
*   None.
*
* Notes:
*   None.
*
*****************************************************************************/
extern void macMgmtMlme_ProbeRsp( macmgmtQ_MgmtMsg_t *MgmtMsg_p );

/******************************************************************************
*
* Name: macMgmtMlme_ReassociateReq
*
* Description:
*   Routine to handle received reassociate request.
*
* Conditions For Use:
*   All software components have been initialized and started.
*
* Arguments:
*   Arg1 (i  ): MgmtMsg_p - Pointer to a reassociate reqeust
*
* Return Value:
*   None.
*
* Notes:
*   None.
*
*****************************************************************************/
extern void macMgmtMlme_ReassociateReq(WL_PRIV *wlpptr, macmgmtQ_MgmtMsg3_t *MgmtMsg_p, UINT32 msgSize );

/******************************************************************************
*
* Name: macMgmtMlme_ReassociateRsp
*
* Description:
*   This routine handles a response from an AP to a prior reassociate request.
*
* Conditions For Use:
*   All software components have been initialized and started.
*
* Arguments:
*   Arg1 (i  ): MgmtMsg_p - Pointer to an 802.11 management message
*                           containing a reassociate response
*
* Return Value:
*   None.
*
* Notes:
*   None.
*
*****************************************************************************/
extern void macMgmtMlme_ReassociateRsp( macmgmtQ_MgmtMsg_t *MgmtMsg_p );

/******************************************************************************
*
* Name: macMgmtMlme_ResetCmd
*
* Description:
*   Routine to handle a command to perform a reset, which resets the MAC
*   to initial conditions.
*
* Conditions For Use:
*   All software components have been initialized and started.
*
* Arguments:
*   Arg1 (i  ): ResetCmd_p - Pointer to a reset command
*
* Return Value:
*   None.
*
* Notes:
*   None.
*
*****************************************************************************/
extern void macMgmtMlme_ResetCmd(WL_PRIV *wlpptr, IEEEtypes_ResetCmd_t *ResetCmd_p );

/******************************************************************************
*
* Name: macMgmtMlme_ScanCmd
*
* Description:
*   Routine to handle a command to perform a scan of potential BSSs that
*   a station may later elect to join.
*
* Conditions For Use:
*   All software components have been initialized and started.
*
* Arguments:
*   Arg1 (i  ): ScanCmd_p - Pointer to a scan command
*
* Return Value:
*   None.
*
* Notes:
*   None.
*
*****************************************************************************/
extern void macMgmtMlme_ScanCmd( IEEEtypes_ScanCmd_t *ScanCmd_p );

/******************************************************************************
*
* Name: macMgmtMlme_StartCmd
*
* Description:
*   Routine to handle a command to start an IBSS.
*
* Conditions For Use:
*   All software components have been initialized and started.
*
* Arguments:
*   Arg1 (i  ): StartCmd_p - Pointer to a start command
*
* Return Value:
*   None.
*
* Notes:
*   None.
*
*****************************************************************************/
extern void macMgmtMlme_StartCmd(WL_PRIV *wlpptr, IEEEtypes_StartCmd_t *StartCmd_p );

/******************************************************************************
*
* Name: macMgmtMlme_Timeout
*
* Description:
*   Routine to handle timeouts that occur from previously set timers.
*
* Conditions For Use:
*   All software components have been initialized and started.
*
* Arguments:
*   Arg1 (i  ): TBD
*
* Return Value:
*   None.
*
* Notes:
*   None.
*
*****************************************************************************/
extern void macMgmtMlme_Timeout( macmgmtQ_TimerMsg_t *TimerMsg_p );

extern WL_STATUS macMgmtMlme_Init(WL_PRIV *wlpptr,UINT32 maxStns, IEEEtypes_MacAddr_t *stnMacAddr);
extern void macMgmtMlme_PsPollMsg(macmgmtQ_MgmtMsg3_t *MgmtMsg_p, UINT32 msgSize);
extern void macMgmtMlme_SendDeauthenticateMsg(WL_PRIV *wlpptr,
											  IEEEtypes_MacAddr_t *Addr, UINT16 StnId, UINT16 Reason );
extern void macMgmtMlme_SendDisassociateMsg(WL_PRIV *wlpptr,
											IEEEtypes_MacAddr_t *Addr, UINT16 StnId, UINT16 Reason );
SINT8 isMacAccessList(WL_PRIV *wlpptr, IEEEtypes_MacAddr_t *destAddr_p);
extern void macMgmtMlme_DecrBonlyStnCnt(WL_PRIV *wlpptr,UINT8);
extern void macMgmtMlme_IncrBonlyStnCnt(WL_PRIV *wlpptr,UINT8 option);
extern int channelSelected(WL_PRIV *wlpptr,int mode);
extern void macMgmtCleanUp(WL_PRIV *wlpptr,extStaDb_StaInfo_t *StaInfo_p);

/* DFS_SUPPORT */
void macMgmtMlme_StartRadarDetection(WL_PRIV *, UINT8 detectionMode);
void macMgmtMlme_StopRadarDetection(WL_PRIV *, UINT8 detectionMode);
void macMgmtMlme_SendChannelSwitchCmd(WL_PRIV *,
    IEEEtypes_ChannelSwitchCmd_t *pChannelSwitchCmd);
void macMgmtMlme_SwitchChannel(WL_PRIV *,  UINT8 channel, CHNL_FLAGS *chanFlag_p);
void macMgmtMlme_Reset(WL_PRIV *, UINT8 *vaplist, UINT8 *vapcount_p);
void macMgmtMlme_MBSS_Reset(WL_PRIV *, UINT8 *vaplist, UINT8 vapcount);
void macMgmtMlme_StopDataTraffic(WL_PRIV * );
void macMgmtMlme_RestartDataTraffic(WL_PRIV * );
BOOLEAN UpdateCurrentChannelInMIB(WL_PRIV *wlpptr,  UINT32 channel);
BOOLEAN macMgmtMlme_DfsEnabled(WL_PRIV * );
void ApplyCSAChannel(WL_PRIV *, UINT32 channel );
UINT8 macMgmtMlme_Get40MHzExtChannelOffset( UINT8 channel );
extern WL_STATUS SendChannelSwitchCmd(WL_PRIV *wlpptr,IEEEtypes_ChannelSwitchCmd_t *ChannelSwitchCmd_p);
/* DFS_SUPPORT */

extern void StopAutoChannel(WL_PRIV *wlpptr);
void IEEEToMrvlRateBitMapConversion(UINT8 SupportedIEEERate, UINT32 *pMrvlLegacySupportedRateBitMap);
extern void DisableMacMgmtTimers(WL_PRIV *wlpptr);
extern void MacMgmtMemCleanup(WL_PRIV *wlpptr);
extern void Disable_ScanTimerProcess(WL_PRIV *wlpptr);
extern void Disable_MonitorTimerProcess(WL_PRIV *wlpptr);
extern void scanControl(WL_PRIV *wlpptr);
extern void MonitorTimerInit(WL_PRIV *wlpptr);

UINT8 macMgmtMlme_Get80MHzPrimaryChannelOffset( UINT8 channel );
#endif /* _MACMGMTMLME_H_ */
