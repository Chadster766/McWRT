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


/******************** (c) Marvell Semiconductor, Inc., 2001 *******************
*
* Purpose:
*    This file .
*
* Public Procedures:
*    macMgmtMlme_AssociateCmd      Process a cmd to associate with an AP
*    macMgmtMlme_AssociateRsp      Process an associate rsp from an AP
*    macMgmtMlme_Atim              Process an ATIM msg from another STA
*    macMgmtMlme_AuthenticateCmd   Process a cmd to authenticate with
*                                  another station or an AP
*    macMgmtMlme_AutheticateMsg    Process an authentication msg from a
*                                  station or an AP
*    macMgmtMlme_BeaconMsg         Process a beacon msg from an AP or a
*                                  station
*    macMgmtMlme_DeauthenticateMsg Process a deauthentication msg from a
*                                  station or an AP
*    macMgmtMlme_DisassociateCmd   Process a cmd to disassociate with an AP
*    macMgmtMlme_DisassociateMsg   Process a disassociation msg from an AP
*    macMgmtMlme_JoinCmd           Process a cmd to join a BSS
*    macMgmtMlme_ProbeRqst         Process a probe request from another
*                                  station in an IBSS
*    macMgmtMlme_ProbeRsp          Process a probe response from a station
*                                  or an AP
*    macMgmtMlme_ReassociateCmd    Process a cmd to reassociate with a new AP
*    macMgmtMlme_ReassociateRsp    Process a reassociation rsp from an AP
*    macMgmtMlme_ResetCmd          Process a cmd to peform a reset
*    macMgmtMlme_ScanCmd           Process a cmd to perform a scan for BSSs
*    macMgmtMlme_StartCmd          Process a cmd to start an IBSS
*    macMgmtMlme_Timeout           Process timeouts from previously set
*                                  timers
*
* Notes:
*    None.
*
*****************************************************************************/

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
#include "osif.h"
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

#ifdef QOS_FEATURE
	Qos_Stn_Data_t Qos_Stn_Data;
#endif //QOS_FEATURE
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
extern void macMgmtMlme_AssociateReq( vmacApInfo_t *vmacSta_p,macmgmtQ_MgmtMsg3_t *MgmtMsg_p, UINT32 msgSize );

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
extern void macMgmtMlme_DeauthenticateMsg(vmacApInfo_t *vmacSta_p,
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
extern void macMgmtMlme_DisassociateCmd(vmacApInfo_t *vmacSta_p,
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
extern void macMgmtMlme_DisassociateMsg(vmacApInfo_t *vmacSta_p, macmgmtQ_MgmtMsg3_t *MgmtMsg_p, UINT32 msgSize );

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
extern void macMgmtMlme_ProbeRqst(vmacApInfo_t *vmacSta_p, macmgmtQ_MgmtMsg3_t *MgmtMsg_p );

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
extern void macMgmtMlme_ReassociateReq(vmacApInfo_t *vmacSta_p, macmgmtQ_MgmtMsg3_t *MgmtMsg_p, UINT32 msgSize );

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
extern void macMgmtMlme_ResetCmd(vmacApInfo_t *vmacSta_p, IEEEtypes_ResetCmd_t *ResetCmd_p );

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
extern void macMgmtMlme_StartCmd(vmacApInfo_t *vmacSta_p, IEEEtypes_StartCmd_t *StartCmd_p );

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

extern WL_STATUS macMgmtMlme_Init(vmacApInfo_t *vmacSta_p,UINT32 maxStns, IEEEtypes_MacAddr_t *stnMacAddr);
extern void macMgmtMlme_PsPollMsg(macmgmtQ_MgmtMsg3_t *MgmtMsg_p, UINT32 msgSize);
extern void macMgmtMlme_SendDeauthenticateMsg(vmacApInfo_t *vmacSta_p,
											  IEEEtypes_MacAddr_t *Addr, UINT16 StnId, UINT16 Reason );
extern void macMgmtMlme_SendDisassociateMsg(vmacApInfo_t *vmacSta_p,
											IEEEtypes_MacAddr_t *Addr, UINT16 StnId, UINT16 Reason );
SINT8 isMacAccessList(vmacApInfo_t *vmacSta_p, IEEEtypes_MacAddr_t *destAddr_p);
extern void macMgmtMlme_DecrBonlyStnCnt(vmacApInfo_t *vmacSta_p,UINT8);
extern void macMgmtMlme_IncrBonlyStnCnt(vmacApInfo_t *vmacSta_p,UINT8 option);
extern int channelSelected(vmacApInfo_t *vmacSta_p,int mode);
extern void macMgmtCleanUp(vmacApInfo_t *vmacSta_p,extStaDb_StaInfo_t *StaInfo_p);

#ifdef MRVL_DFS
void macMgmtMlme_StartRadarDetection(struct net_device *dev, UINT8 detectionMode);
void macMgmtMlme_StopRadarDetection(struct net_device *dev, UINT8 detectionMode);
void macMgmtMlme_SendChannelSwitchCmd(struct net_device *dev,
    IEEEtypes_ChannelSwitchCmd_t *pChannelSwitchCmd);
void macMgmtMlme_SwitchChannel(struct net_device *dev,  UINT8 channel, CHNL_FLAGS *chanFlag_p);
void macMgmtMlme_Reset(struct net_device *dev, UINT8 *vaplist, UINT8 *vapcount_p);
void macMgmtMlme_MBSS_Reset(struct net_device *netdev, UINT8 *vaplist, UINT8 vapcount);
void macMgmtMlme_StopDataTraffic(struct net_device *dev );
void macMgmtMlme_RestartDataTraffic(struct net_device *dev );
BOOLEAN UpdateCurrentChannelInMIB(vmacApInfo_t *vmacSta_p,  UINT32 channel);
BOOLEAN macMgmtMlme_DfsEnabled(struct net_device *dev );
void ApplyCSAChannel(struct net_device *netdev, UINT32 channel );
UINT8 macMgmtMlme_Get40MHzExtChannelOffset( UINT8 channel );
extern WL_STATUS SendChannelSwitchCmd(vmacApInfo_t *vmacSta_p,IEEEtypes_ChannelSwitchCmd_t *ChannelSwitchCmd_p);
#endif //MRVL_DFS


extern void StopAutoChannel(vmacApInfo_t *vmacSta_p);
void IEEEToMrvlRateBitMapConversion(UINT8 SupportedIEEERate, UINT32 *pMrvlLegacySupportedRateBitMap);
extern void DisableMacMgmtTimers(vmacApInfo_t *);
extern void MacMgmtMemCleanup(vmacApInfo_t *vmacSta_p);
extern void Disable_ScanTimerProcess(vmacApInfo_t *vmacSta_p);
extern void Disable_MonitorTimerProcess(vmacApInfo_t *vmacSta_p);
extern void scanControl(vmacApInfo_t *vmacSta_p);
extern void MonitorTimerInit(vmacApInfo_t *vmacSta_p);

#ifdef MRVL_WAPI
extern void macMgmtMlme_WAPI_event(struct net_device *dev, int event_type, u16 auth_type, IEEEtypes_MacAddr_t *sta_addr, IEEEtypes_MacAddr_t *ap_addr, char * info);
#endif
#ifdef SOC_W8864
UINT8 macMgmtMlme_Get80MHzPrimaryChannelOffset( UINT8 channel );
#endif
#endif /* _MACMGMTMLME_H_ */
