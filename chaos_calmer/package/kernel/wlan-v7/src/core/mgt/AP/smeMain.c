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


/******************** (c) Marvell Semiconductor, Inc., 2004 *******************
*
* Purpose:
*    This file contains the implementations of the function prototypes given
*    in the associated header file for the Station Managment Entity (SME)
*    Task.
*
* Public Procedures:
*    smeMain_Init       Initialzies the SME Task and related components
*    smeMain_Start      Starts running the SME Task
*    smeMain_SendEvent  Mechanism used to trigger an event indicating a
*                       message has been received on one of the message
*                       queues
*
* Private Procedures:
*    SmeTask            The actual SME Task
*
* Notes:
*    None.
*
*****************************************************************************/

/*============================================================================= */
/*                               INCLUDE FILES */
/*============================================================================= */
#include "ap8xLnxIntf.h"

#include "wltypes.h"
#include "IEEE_types.h"
#include "osif.h"

#include "mib.h"
#include "qos.h"
#include "wlmac.h"
#include "ds.h"
#include "keyMgmtCommon.h"
#include "keyMgmt.h"
#include "tkip.h"
#include "StaDb.h"
#include "macmgmtap.h"


#include "macMgmtMlme.h"
#include "Fragment.h"

#include "wl_macros.h"
#include "wl_mib.h"
#include "wpa.h"
#include "keyMgmtSta.h"
#include "wldebug.h"
#include "dfs.h" //MRVL_DFS
#include "dfsMgmt.h" //MRVL_DFS
#include "domain.h"
#ifdef IEEE80211H
extern UINT8 bcn_reg_domain;
#endif /* IEEE80211H */

#ifdef AP_URPTR
extern UINT8 g_urAutoCfg;
#endif
/*============================================================================= */
/*                                DEFINITIONS */
/*============================================================================= */
#define SME_MAIN_EVENT_TRIGGERS smeMain_MGMT_MSG_RCVD      | \
	smeMain_CMD_RCVD   | \
	smeMain_ERROR_RCVD


typedef enum
{
	SME_STATE_IDLE,
	SME_STATE_RESET_IN_PROGRESS,
	SME_STATE_WAIT_FOR_RESET_CONFIRM,
	SME_STATE_RESTART_IN_PROGRESS
} Smetate_e;

/*============================================================================= */
/*                   PRIVATE PROCEDURES (ANSI Prototypes) */
/*============================================================================= */
UINT32 CopySsId(UINT8 * SsId1, UINT8 * SsId2);
#ifdef IEEE80211H
typedef struct SME_STA_INFOsss_t
{
	IEEEtypes_MacAddr_t staMac;    
#ifndef STA_INFO_DB 
	UINT8 pad0;
#endif /* STA_INFO_DB */
	UINT8 isAsso;
#ifdef STA_INFO_DB
	UINT8 ClientMode;
	UINT8 Rate;
	UINT8 Sq2;
	UINT8 Sq1;
	UINT8 RSSI;
	UINT8 isPwrsave;
#endif /* STA_INFO_DB */    
} PACK_END SME_STA_INFO_t; 


typedef struct SME_CHANNEL_INFO_t
{
	UINT8 channel;
	IEEEtypes_MeasurementRepMap_t mMap;
} PACK_END SME_CHANNEL_INFO_t; 

#define MREQEST_PERIODS 50

#ifdef IEEE80211H_NOTWIFI
static void StartSendMREQUESTCmd(void);
#endif
static WL_STATUS SendMREQUESTCmd(vmacApInfo_t *vmacSta_p,IEEEtypes_MacAddr_t *MacAddr_p, UINT8 MeasuredChannel);
static void semChannelUtilizationInit(UINT8 *ChannelList);
#ifdef AP_BUFFER
static UINT8 getDiaglogToken(void);
static UINT8 getMeasurementToken(void);
#endif
static void smeMrequestIndProcess(vmacApInfo_t *vmacSta_p,IEEEtypes_MRequestInd_t *MrequestInd_p);
static void smeMrequestCfrmProcess(vmacApInfo_t *vmacSta_p,IEEEtypes_MRequestCfrm_t *MrequestCfrm_p);
static void smeMeasureCfrmProcess(vmacApInfo_t *vmacSta_p,IEEEtypes_MeasureCfrm_t *MeasureCfrm_p);
static void smeMreportIndProcess(vmacApInfo_t *vmacSta_p,IEEEtypes_MacAddr_t *MacAddr_p,IEEEtypes_MReportInd_t *MreportInd_p);
static void smeMreportCfrmProcess(vmacApInfo_t *vmacSta_p,IEEEtypes_MReportCfrm_t *MreportCfrm_p);
static void smeMTpcAdptCfrmdProcess(vmacApInfo_t *vmacSta_p,IEEEtypes_TPCAdaptCfrm_t *TpcAdptCfrm_p);
static void smeChannelSwitchIndProcess(vmacApInfo_t *vmacSta_p,IEEEtypes_ChannelSwitchInd_t *ChannelSwitchInd_p);
static void smeChannelSwitchCfrmProcess(vmacApInfo_t *vmacSta_p,IEEEtypes_ChannelSwitchCfrm_t *ChannelSwitchCfrm_p);
extern int GetDomainCode(void);
extern SINT8 evtSmeCmdMsg(vmacApInfo_t *vmacSta_p,UINT8 *);
extern void syncSrv_ScanCmd(vmacApInfo_t *vmacSta_p, IEEEtypes_ScanCmd_t *ScanCmd_p );


#ifdef AP_BUFFER
static UINT8 D_Token;
static UINT8 M_Token;
#endif
static SME_CHANNEL_INFO_t semChannelUtilizationInfo[IEEE_80211_MAX_NUMBER_OF_CHANNELS];
static UINT8 nbr_of_channel;
#endif /* IEEE802111H */
/*============================================================================= */
/*                         CODED PUBLIC PROCEDURES */
/*============================================================================= */

extern IEEEtypes_MacAddr_t bcast;

/* Temporary holders for parameters, committed to the Mib when a Parameter Set
* Done message is received */
IEEEtypes_SsId_t smeSsId;

IEEEtypes_DataRate_t smeOpRateSet[IEEEtypes_MAX_DATA_RATES];
UINT8 RateIndex1Mbit = 0; //13;
UINT8 RateIndex2Mbit = 0x1;
UINT8 RateIndex11Mbit = 0x8;
extern UINT8 RfSwitchChanA, RfSwitchChanG;

extern UINT8 Radio_off;

#ifdef IEEE80211H_NOTWIFI
extern int tick;
#endif /* IEEE80211H */
/******************************************************************************
*
* Name: smeMain_Init
*
* Description:
*   This routine is called to initialize the the SME Task and related
*   components.
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
* PDL:
*   Call smeQ_Init()
*   If the queue was successfully initialized Then
*      Create the SME Task by calling os_TaskCreate()
*      If creating the SME Task succeeded Then
*         Return OS_SUCCESS
*      End If
*   End If
*
*   Return OS_FAIL
* END PDL
*
*****************************************************************************/
extern WL_STATUS smeMain_Init( vmacApInfo_t *vmacSta_p )
{
	//	WL_STATUS status;

#ifdef IEEE80211H
	{
		UINT8 ChannelList[IEEE_80211_MAX_NUMBER_OF_CHANNELS]; /* the assumption of NULL-terminated is made */

		memset(ChannelList, 0, IEEE_80211_MAX_NUMBER_OF_CHANNELS);

		/* get regulatory maximum tx power */   
		domainGetInfo(ChannelList);

		/* init channel utilization information database */
		semChannelUtilizationInit(ChannelList);
	}

	TimerInit(&vmacSta_p->reqMeasurementTimer);
#endif /* IEEE80211H */
#ifdef AP_MAC_LINUX
#ifdef IEEE80211H_NOTWIFI
	/* Start MREQUEST periodic timer (10 mins interval) */
	TimerFireEvery(&vmacSta_p->reqMeasurementTimer, 1, &StartSendMREQUESTCmd, vmacSta_p, 50);
#endif /* IEEE80211H */
#endif
	//    SendStartCmd();
	return(OS_SUCCESS);
}


/*============================================================================= */
/*                         CODED PRIVATE PROCEDURES */
/*============================================================================= */

/******************************************************************************
*
* Name: SmeTask
*
* Description:
*   This routine is the task that runs continuously, waking to process
*   messages received on its queues. It commences when the smeMain_Start()
*   routine is invoked.
*
* Conditions For Use:
*   The task and related components have been initialized by calling
*   smeMain_Init() and then the task has been started by calling
*   smeMain_Start().
*
* Arguments:
*   Arg1 (i  ): Data - Data passed to the task; not actually used - just
*                      included to conform to the structure the OS
*                      expects when creating a task
*
* Return Value:
*   None.
*
* Notes:
*   None.
*
* PDL:
*   While forever
*      Wait for an event to occur by calling os_EventWait()
*      If an error occurred waiting for an event Then
*         Do some error handling
*      Else
*         If the event smeMain_MGMT_MSG_RCVD occurred Then
*         End If
*
*         If the event smeMain_CB_PROC_MSG_RCVD occurred Then
*         End If
*
*         If the event smeMain_CB_PROC_MSG_RCVD occurred Then
*         End If
*      End If
*   End While
* END PDL
*
*****************************************************************************/
#ifdef IEEE80211_DH
WL_STATUS SendChannelSwitchCmd(vmacApInfo_t *vmacSta_p,IEEEtypes_ChannelSwitchCmd_t *ChannelSwitchCmd_p)
#else
static WL_STATUS SendChannelSwitchCmd(vmacApInfo_t *vmacSta_p,IEEEtypes_ChannelSwitchCmd_t *ChannelSwitchCmd_p)
#endif //IEEE80211_DH
{
	macmgmtQ_SmeCmd_t SmeCmd;
	macmgmtQ_SmeCmd_t *SmeCmd_p = &SmeCmd;

	{
		memset(SmeCmd_p, 0, sizeof(macmgmtQ_SmeCmd_t));  
		SmeCmd_p->CmdType = SMC_CMD_CHANNELSWITCH_REQ;
		memcpy(&SmeCmd_p->Body.ChannelSwitchCmd, ChannelSwitchCmd_p, sizeof(IEEEtypes_ChannelSwitchCmd_t));

		macMgmtQ_SmeWriteNoBlock(vmacSta_p,SmeCmd_p);          
		return(OS_SUCCESS);
	}
}

extern void SmeMgmt(vmacApInfo_t *vmacSta_p,UINT32 eventsTriggered, UINT8 *msg)
{
	MIB_802DOT11 *mib=vmacSta_p->Mib802dot11;
	MIB_STA_CFG  *mib_StaCfg_p=vmacSta_p->Mib802dot11->StationConfig;
	smeQ_MgmtMsg_t *CfrmIndMsg_p;

	WLDBG_ENTER(DBG_LEVEL_11);
	{

		//eventsTriggered = os_EventWait(sysinfo_SME_MAIN_Q_EVENT,
		//                               SME_MAIN_EVENT_TRIGGERS,
		//                               OS_EVENT_WAITMODE_CLR_OR);

		/*------------------------------*/
		/* Check for an error condition */
		/*------------------------------*/
		if (eventsTriggered == 0)
		{
			return;
		}
		else
		{
			/*---------------------------------------------------*/
			/* Check for receipt of an 802.11 management message */
			/*---------------------------------------------------*/
			if (eventsTriggered & smeMain_MGMT_MSG_RCVD)
			{
				CfrmIndMsg_p = (smeQ_MgmtMsg_t *) msg;
				{
					switch (CfrmIndMsg_p->MsgType)
					{
					case SME_NOTIFY_START_CFRM:
						if (CfrmIndMsg_p->Msg.StartCfrm.Result ==
							START_RESULT_SUCCESS)
						{         
						}
						else if (CfrmIndMsg_p->Msg.StartCfrm.Result ==
							START_RESULT_BSS_ALREADY_STARTED_OR_JOINED)
						{
						}
						else if (CfrmIndMsg_p->Msg.StartCfrm.Result ==
							START_RESULT_INVALID_PARAMETERS)
						{
						}
						break;
					case SME_NOTIFY_RESET_CFRM:
						if (vmacSta_p->SmeState == SME_STATE_WAIT_FOR_RESET_CONFIRM)
						{
							SendStartCmd(vmacSta_p);
						}
						break;
#ifdef IEEE80211H
					case SME_NOTIFY_ASSOC_IND:
						{
							extStaDb_StaInfo_t *staInfo = NULL;

							/* 
							* AP is in A mode and doing spectrum management
							*/                                                            
							if (((*(mib->mib_ApMode) == AP_MODE_A_ONLY)||(*(mib->mib_ApMode)==AP_MODE_AandG)) &&
								(mib_StaCfg_p->SpectrumManagementRequired == TRUE))
							{      
#ifdef DEBUG_ENABLE
								printf("A new friend is coming\n\r");
#endif /* DEBUG_ENABLE */                                
								if((staInfo = extStaDb_GetStaInfo(vmacSta_p,&CfrmIndMsg_p->Msg.AssocInd.PeerStaAddr, 0)))
								{ 
									/* Station IS capable of doing spectrum management */
									if (staInfo->IsSpectrumMgmt == TRUE)
									{
#ifdef DEBUG_ENABLE
										printf("Ask him measure this channel on behalf me\n\r");
#endif /* DEBUG_ENABLE */                       
#ifdef IEEE80211H_NOTWIFI
										SendMREQUESTCmd(vmacSta_p,&staInfo->Addr, RfSwitchChanA);
#endif
									}
								}
								else
								{
#ifdef DEBUG_ENABLE
									printf("Oops! Dest station not found\n\r");
#endif /* DEBUG_ENABLE */
								} 
							}
						}

						break;

					case SME_NOTIFY_REASSOC_IND:
						{
							extStaDb_StaInfo_t *staInfo = NULL;

							/* 
							* AP is in A mode and doing spectrum management
							*/                                                            
							if (((*(mib->mib_ApMode) == AP_MODE_A_ONLY)||(*(mib->mib_ApMode)==AP_MODE_AandG)) &&
								(mib_StaCfg_p->SpectrumManagementRequired == TRUE))
							{      
#ifdef DEBUG_ENABLE
								printf("A new friend is coming\n\r");
#endif /* DEBUG_ENABLE */                                
								if((staInfo = extStaDb_GetStaInfo(vmacSta_p, &CfrmIndMsg_p->Msg.ReassocInd.PeerStaAddr, 0)))
								{ 
									/* Station IS capable of doing spectrum management */
									if (staInfo->IsSpectrumMgmt == TRUE)
									{
#ifdef DEBUG_ENABLE
										printf("Ask him measure this channel on behalf me\n\r");
#endif /* DEBUG_ENABLE */                                        
										SendMREQUESTCmd(vmacSta_p,&staInfo->Addr, RfSwitchChanA);
									}
								}
								else
								{
#ifdef DEBUG_ENABLE
									printf("Oops! Dest station not found\n\r");
#endif /* DEBUG_ENABLE */
								} 
							}
						}

						break;    
#ifdef MRVL_DFS
						/*Channel switch has been performed.*/
					case SME_NOTIFY_CHANNELSWITCH_CFRM:
						{
							DfsCmd_t    dfsCmd ;
							/* If channel change is successful, send the message to
							* the event dispatcher
							*/
							if(  CfrmIndMsg_p->Msg.ChanSwitchCfrm.result)
							{
								dfsCmd.CmdType = DFS_CMD_CHANNEL_CHANGE ;
								memcpy( &dfsCmd.Body.chInfo , 
									&CfrmIndMsg_p->Msg.ChanSwitchCfrm.chInfo, 
									sizeof (DfsChanInfo) );
								evtDFSMsg( vmacSta_p->dev, (UINT8 *)&dfsCmd );
							}
						}
						break;
						/*Radar has been detected.*/
					case SME_NOTIFY_RADAR_DETECTION_IND:
						{
							DfsCmd_t    dfsCmd ;
							/* Send the radar detection message to
							* the event dispatcher
							*/
							dfsCmd.CmdType = DFS_CMD_RADAR_DETECTION ;
							memcpy( &dfsCmd.Body.chInfo , 
								&CfrmIndMsg_p->Msg.RadarDetectionInd.chInfo, 
								sizeof (DfsChanInfo) );
							evtDFSMsg( vmacSta_p->dev, (UINT8 *)&dfsCmd );
						}
						break;
#endif //MRVL_DFS

					case SME_NOTIFY_MREQUEST_IND:
						smeMrequestIndProcess(vmacSta_p,&CfrmIndMsg_p->Msg.MrequestInd);
						break;

					case SME_NOTIFY_MREQUEST_CFRM:
						smeMrequestCfrmProcess(vmacSta_p,&CfrmIndMsg_p->Msg.MrequestCfrm);
						break;

					case SME_NOTIFY_MEASURE_CFRM:
						smeMeasureCfrmProcess(vmacSta_p,&CfrmIndMsg_p->Msg.MeasureCfrm);
						break;

					case SME_NOTIFY_MREPORT_IND:
						{
							extStaDb_StaInfo_t *staInfo = NULL;

							/* 
							* AP is in A mode and doing spectrum management
							*/                                                            
							if (((*(mib->mib_ApMode) == AP_MODE_A_ONLY)||(*(mib->mib_ApMode)==AP_MODE_AandG)) &&
								(mib_StaCfg_p->SpectrumManagementRequired == TRUE))
							{      
								staInfo = extStaDb_GetStaInfo(vmacSta_p,&CfrmIndMsg_p->Msg.MreportInd.PeerStaAddr, 0);
#ifdef DEBUG_ENABLE
								printf("SME_NOTIFY_MREPORT_IND\n\r");
#endif /* DEBUG_ENABLE */                                
								if(staInfo != NULL)
								{   
									/* Station IS capable of doing spectrum management */
									if (staInfo->IsSpectrumMgmt == TRUE)
									{ 
#ifdef DEBUG_ENABLE
										printf("Process incoming MLME-MREPORT.ind\n\r");
#endif /* DEBUG_ENABLE */                                        
										smeMreportIndProcess(vmacSta_p,(IEEEtypes_MacAddr_t *)&staInfo->Addr, &CfrmIndMsg_p->Msg.MreportInd);
									}
								}
								else
								{
#ifdef DEBUG_ENABLE
									printf("Oops! Dest station not found\n\r");
#endif /* DEBUG_ENABLE */
								} 
							}
						}

						break;

					case SME_NOTIFY_MREPORT_CFRM:
						smeMreportCfrmProcess(vmacSta_p,&CfrmIndMsg_p->Msg.MreportCfrm);
						break;

					case SME_NOTIFY_TPCADPT_CFRM:
						smeMTpcAdptCfrmdProcess(vmacSta_p,&CfrmIndMsg_p->Msg.TPCAdaptCfrm);
						break;

					case SMC_NOTIFY_CHANNELSWITCH_IND:
						smeChannelSwitchIndProcess(vmacSta_p,&CfrmIndMsg_p->Msg.ChannelSwitchInd);
						break;

					case SMC_NOTIFY_CHANNELSWITCH_CFRM:
						smeChannelSwitchCfrmProcess(vmacSta_p,&CfrmIndMsg_p->Msg.ChannelSwitchCfrm);
						break;
#endif /* IEEE80211H */

					default:
						break;
					}
					//pool_FreeBuf((char *)CfrmIndMsg_p);
				}
			}

			/*---------------------------------------*/
			/* Check for receipt of a error messages */
			/*---------------------------------------*/
		}
	}
	WLDBG_EXIT(DBG_LEVEL_11);
}



/******************************************************************************
*
* Name: smeQ_MgmtWriteNoBlock
*
* Description:
*   This routine is called to write a message to the queue where IEEE 802.11
*   management messages are placed for the SME Task. If writing to the
*   queue cannot immediately occur, then the routine returns with a failure
*   status (non-blocking).
*
* Conditions For Use:
*   The queue has been initialized by calling smeQ_Init().
*
* Arguments:
*   Arg1 (i  ): MgmtMst_p - a pointer to the message to be placed on
*                           the queue
*
* Return Value:
*   Status indicating success or failure
*
* Notes:
*   None.
*
* PDL:
*   Call os_QueueWriteNoBlock() to write MgmtMsg_p to the management
*      message queue
*   If the message was successfully placed on the queue Then
*      Call smeMain_SendEvent() with event smeMain_MGMT_MSG_RCVD
*      Return SUCCESS
*   Else
*      Return FAIL
*   End If
* END PDL
*
*****************************************************************************/
extern WL_STATUS smeQ_MgmtWriteNoBlock( smeQ_MgmtMsg_t *MgmtMsg_p )
{
	SmeMgmt(MgmtMsg_p->vmacSta_p, smeMain_MGMT_MSG_RCVD, (UINT8 *) MgmtMsg_p); 
	return (SUCCESS);
}

/******************************************************************************
*
* Name:  
*
* Description:
*
* Conditions For Use:
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
* PDL:
* END PDL
*
*****************************************************************************/
UINT32 CopySsId(UINT8 * SsId1, UINT8 * SsId2)
{
	UINT8 i;
	i = 0;
	while ( i < IEEEtypes_SSID_SIZE && SsId2[i] != '\0')
	{
		SsId1[i] = SsId2[i];
		i++;
	}
	if (i < IEEEtypes_SSID_SIZE - 1 )
		SsId1[i] = '\0';
	return (i);
}

/******************************************************************************
*
* Name: SendStartCmd 
*
* Description:This function is called to send the Start command to MLME. Called 
* after init, and also when ever the Config param belonging to the Start command
*
* Conditions For Use:
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
* PDL:
* END PDL
*
*****************************************************************************/
UINT32 SendStartCmd(vmacApInfo_t *vmacSta_p)
{
	MIB_802DOT11 *mib=vmacSta_p->Mib802dot11;
	MIB_STA_CFG  *mib_StaCfg_p=vmacSta_p->Mib802dot11->StationConfig;
	MIB_OP_DATA  *mib_OpData_p=vmacSta_p->Mib802dot11->OperationTable;
	MIB_PHY_DSSS_TABLE *PhyDSSSTable_p=vmacSta_p->Mib802dot11->PhyDSSSTable;
	MIB_PRIVACY_TABLE *mib_PrivacyTable_p=vmacSta_p->Mib802dot11->Privacy;
#ifdef AP_URPTR
	extern UINT8 mib_urMode;
#endif
#ifdef AP_MAC_LINUX
	macmgmtQ_SmeCmd_t SmeCmd;
#endif
	macmgmtQ_SmeCmd_t *SmeCmd_p;
	UINT32 i, j, k;
#ifdef BRS_SUPPORT
	UINT32 rateMask;
	UINT32 basicRateMask;
#endif
	/* code for sending Start Cmd */
	SmeCmd_p = &SmeCmd;
	memset(SmeCmd_p,
		0,
		sizeof(macmgmtQ_SmeCmd_t));
	SmeCmd_p->CmdType = SME_CMD_START;
	CopySsId(SmeCmd_p->Body.StartCmd.SsId, mib_StaCfg_p->DesiredSsId);
	{
		SmeCmd_p->Body.StartCmd.BssType = mib_StaCfg_p->DesiredBssType;
		SmeCmd_p->Body.StartCmd.BcnPeriod = *(mib->mib_BcnPeriod);
		SmeCmd_p->Body.StartCmd.DtimPeriod = mib_StaCfg_p->DtimPeriod;
		/* Number of beacon intervals between DTIMs */
		SmeCmd_p->Body.StartCmd.SsParamSet.CfParamSet.ElementId = CF_PARAM_SET;
		SmeCmd_p->Body.StartCmd.SsParamSet.CfParamSet.CfpCnt = 0; /* HW MAC fills later */
		SmeCmd_p->Body.StartCmd.SsParamSet.CfParamSet.CfpPeriod = mib_StaCfg_p->CfPeriod;
		SmeCmd_p->Body.StartCmd.SsParamSet.CfParamSet.CfpMaxDuration = mib_StaCfg_p->CfpMax;
		SmeCmd_p->Body.StartCmd.SsParamSet.CfParamSet.CfpDurationRemaining = 0; /* HW MAC */
		SmeCmd_p->Body.StartCmd.SsParamSet.CfParamSet.Len = sizeof(IEEEtypes_CfParamSet_t) -
			sizeof(IEEEtypes_ElementId_t) - sizeof(IEEEtypes_Len_t);

		SmeCmd_p->Body.StartCmd.PhyParamSet.DsParamSet.ElementId = DS_PARAM_SET;
		SmeCmd_p->Body.StartCmd.PhyParamSet.DsParamSet.Len = 1;
#ifdef AUTOCHANNEL
		if(PhyDSSSTable_p->CurrChan ==0 ||PhyDSSSTable_p->CurrChan2==0) // || g_urAutoCfg)
		{
			UINT8 chsel_a, chsel_g;
			chsel_a = channelSelected(vmacSta_p,1);
			chsel_g = channelSelected(vmacSta_p,0);
			if(PhyDSSSTable_p->CurrChan ==0)
			{
				if((*(mib->mib_ApMode)&AP_MODE_BAND_MASK) >= AP_MODE_A_ONLY)
				{
					SmeCmd_p->Body.StartCmd.PhyParamSet.DsParamSet.CurrentChan = chsel_a;
				}
				else
				{
					SmeCmd_p->Body.StartCmd.PhyParamSet.DsParamSet.CurrentChan = chsel_g;
				}
			}
			else
			{
				SmeCmd_p->Body.StartCmd.PhyParamSet.DsParamSet.CurrentChan = PhyDSSSTable_p->CurrChan;
			}
			RfSwitchChanA = PhyDSSSTable_p->CurrChan?PhyDSSSTable_p->CurrChan:chsel_a;
			RfSwitchChanG = PhyDSSSTable_p->CurrChan2?PhyDSSSTable_p->CurrChan2:chsel_g;
		}
		else
#endif
		{
			SmeCmd_p->Body.StartCmd.PhyParamSet.DsParamSet.CurrentChan = PhyDSSSTable_p->CurrChan;
			RfSwitchChanA=PhyDSSSTable_p->CurrChan;
			RfSwitchChanG= PhyDSSSTable_p->CurrChan2;
		}

		SmeCmd_p->Body.StartCmd.ProbeDelay = 10;
		SmeCmd_p->Body.StartCmd.CapInfo.Ess = 1;
		SmeCmd_p->Body.StartCmd.CapInfo.Ibss = 0;
		SmeCmd_p->Body.StartCmd.CapInfo.CfPollable = mib_StaCfg_p->CfPollable ? 1 : 0;
		SmeCmd_p->Body.StartCmd.CapInfo.CfPollRqst = 0;
		if (*(mib->mib_ApMode) != AP_MODE_B_ONLY &&  *(mib->mib_ApMode) !=AP_MODE_AandG)
			SmeCmd_p->Body.StartCmd.CapInfo.ShortSlotTime = *(mib->mib_shortSlotTime);
		else
			SmeCmd_p->Body.StartCmd.CapInfo.ShortSlotTime = 0;

		//	  if(mib_StaCfg_p->PrivOption == TRUE && mib_PrivacyTable_p->PrivInvoked == TRUE)
#ifdef MRVL_WAPI
		if (mib_PrivacyTable_p->PrivInvoked || mib_PrivacyTable_p->WAPIEnabled)
#else
		if (mib_PrivacyTable_p->PrivInvoked)
#endif
		{
			SmeCmd_p->Body.StartCmd.CapInfo.Privacy = 1;
		}
		else
		{
			if (mib_PrivacyTable_p->RSNEnabled)
				SmeCmd_p->Body.StartCmd.CapInfo.Privacy = 1;
			else
				SmeCmd_p->Body.StartCmd.CapInfo.Privacy = 0;
		}

		if (mib_StaCfg_p->mib_preAmble == PREAMBLE_LONG || mib_StaCfg_p->mib_preAmble == PREAMBLE_AUTO_SELECT || *(mib->mib_ApMode)==AP_MODE_A_ONLY)
		{
			SmeCmd_p->Body.StartCmd.CapInfo.ShortPreamble = 0;
		}
		else
		{
			SmeCmd_p->Body.StartCmd.CapInfo.ShortPreamble = 1;
		}


		SmeCmd_p->Body.StartCmd.CapInfo.Pbcc = 0;
		SmeCmd_p->Body.StartCmd.CapInfo.ChanAgility = 0;

#ifdef IEEE80211H
		SmeCmd_p->Body.StartCmd.CapInfo.SpectrumMgmt = mib_StaCfg_p->SpectrumManagementRequired ? 1:0;
#endif /* IEEE80211H */

		/* Form the Rate from the mib, separate the Basic and Op rate set */

#ifdef BRS_SUPPORT
		i = j = k = 0;
		rateMask = *(mib->BssBasicRateMask) | *(mib->NotBssBasicRateMask);
		basicRateMask = *(mib->BssBasicRateMask);

		while ( i < IEEEtypes_MAX_DATA_RATES_G && mib_StaCfg_p->OpRateSet[i] != '\0') {
			if (rateMask & 0x01) {

				if ( basicRateMask & 0x01) {
					SmeCmd_p->Body.StartCmd.BssBasicRateSet[j] = (mib_StaCfg_p->OpRateSet[i] & 0x7F);
					SmeCmd_p->Body.StartCmd.OpRateSet[k] = (mib_StaCfg_p->OpRateSet[i] & 0x7F);

					j++;k++;
				}
				else
				{
					SmeCmd_p->Body.StartCmd.OpRateSet[k] = (mib_StaCfg_p->OpRateSet[i] & 0x7F);
					k++;
				}
			}
			i++;
			rateMask >>= 1;
			basicRateMask >>= 1;

		}
#else
		i = j = k = 0;
		while ( i < IEEEtypes_MAX_DATA_RATES_G && mib_StaCfg_p->OpRateSet[i] != '\0') {
			if ( mib_StaCfg_p->OpRateSet[i] & 0x80) {
				SmeCmd_p->Body.StartCmd.BssBasicRateSet[j] = (mib_StaCfg_p->OpRateSet[i] & 0x7f);
				SmeCmd_p->Body.StartCmd.OpRateSet[k] = mib_StaCfg_p->OpRateSet[i] & 0x7f;
				i++;j++;k++;
			}
			else
			{
				SmeCmd_p->Body.StartCmd.OpRateSet[k] = mib_StaCfg_p->OpRateSet[i] & 0x7f;
				i++;k++;
			}


		}
#endif
		if (i < IEEEtypes_MAX_DATA_RATES_G )
		{
			SmeCmd_p->Body.StartCmd.BssBasicRateSet[j] = '\0';
			SmeCmd_p->Body.StartCmd.OpRateSet[k] = '\0';
		}

		i = j = k = 0;


		if(*(mib->mib_ApMode)==AP_MODE_AandG)
		{
			while ( i < IEEEtypes_MAX_DATA_RATES_G && mib_StaCfg_p->OpRateSet2[i] != '\0') 
			{
				if ( mib_StaCfg_p->OpRateSet2[i] & 0x80) 
				{
					SmeCmd_p->Body.StartCmd.BssBasicRateSet2[j] = (mib_StaCfg_p->OpRateSet2[i] & 0x7f);
					SmeCmd_p->Body.StartCmd.OpRateSet2[k] = mib_StaCfg_p->OpRateSet2[i] & 0x7f;
					i++;j++;k++;
				}
				else
				{
					SmeCmd_p->Body.StartCmd.OpRateSet2[k] = mib_StaCfg_p->OpRateSet2[i] & 0x7f;
					i++;k++;
				}

			}
			if (i < IEEEtypes_MAX_DATA_RATES_G )
			{
				SmeCmd_p->Body.StartCmd.BssBasicRateSet2[j] = '\0';
				SmeCmd_p->Body.StartCmd.OpRateSet2[k] = '\0';
			}
		}



		/*   printf("SmeTask: Sending Start Cmd to MacMgmtAp Task\n"); */
		memcpy(&vmacSta_p->macStaAddr, mib_OpData_p->StaMacAddr, sizeof(IEEEtypes_MacAddr_t));

		if(*(mib->mib_ApMode)==AP_MODE_AandG)
			memcpy(&vmacSta_p->macStaAddr2, &vmacSta_p->macBssId2, sizeof(IEEEtypes_MacAddr_t));

		memcpy(&vmacSta_p->macBssId, mib_OpData_p->StaMacAddr, sizeof(IEEEtypes_MacAddr_t));
		//printk(KERN_CRIT "Event Initialize send SME start command\n");
#ifdef IEEE80211_DH
		evtSmeCmdMsg(vmacSta_p,(UINT8 *)SmeCmd_p);
#else
		//evtSmeCmdMsg(wlpptr->vmacSta_p,SmeCmd_p);
#endif /*IEEE80211_DH*/
		macMgmtMlme_StartCmd(vmacSta_p,&SmeCmd_p->Body.StartCmd);
	}
	return(OS_SUCCESS);
}

/******************************************************************************
*
* Name:  
*
* Description:
*
* Conditions For Use:
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
* PDL:
* END PDL
*
*****************************************************************************/
void SendResetCmd(vmacApInfo_t *vmacSta_p, int quiet)
{

	macmgmtQ_SmeCmd_t resetMsg;
	macmgmtQ_SmeCmd_t *resetMsg_p = &resetMsg;

	vmacSta_p->SmeState = SME_STATE_WAIT_FOR_RESET_CONFIRM;


	/*-----------------------------------------------------------*/
	/* Send a command to the MAC Management Task to do the reset */
	/*-----------------------------------------------------------*/
	{
		resetMsg_p->CmdType = SME_CMD_RESET;
		resetMsg_p->Body.ResetCmd.SetDefaultMIB = TRUE;
		resetMsg_p->Body.ResetCmd.quiet = quiet;
		macMgmtMlme_ResetCmd(vmacSta_p, (IEEEtypes_ResetCmd_t *)&resetMsg_p->Body);
		return ;
	}

}

#if defined(AP_SITE_SURVEY) || defined(AUTOCHANNEL)
/******************************************************************************
*
* Name:  
*
* Description:
*
* Conditions For Use:
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
* PDL:
* END PDL
*
*****************************************************************************/
void SendScanCmd(vmacApInfo_t *vmacSta_p,UINT8 *channels)
{
#ifdef AP_MAC_LINUX
	macmgmtQ_SmeCmd_t SmeCmd;
	int i;

	SmeCmd.CmdType = SME_CMD_SCAN;


	memset(&SmeCmd.Body.ScanCmd.ChanList[0], 0, IEEEtypes_MAX_CHANNELS+IEEEtypes_MAX_CHANNELS_A);
	if((*(vmacSta_p->Mib802dot11->mib_ApMode)&AP_MODE_BAND_MASK) <AP_MODE_A_ONLY)
	{
		for ( i=0; (i < IEEEtypes_MAX_CHANNELS); i++ ) {
			SmeCmd.Body.ScanCmd.ChanList[i] = channels[i];
		}
		SmeCmd.Body.ScanCmd.ChanList[i] = 0;
	}
	else
	{
		for ( i=0; (i < IEEEtypes_MAX_CHANNELS_A); i++ ) {
			SmeCmd.Body.ScanCmd.ChanList[i] = channels[i+IEEEtypes_MAX_CHANNELS];
		}
		SmeCmd.Body.ScanCmd.ChanList[i] = 0;
	}
	syncSrv_ScanCmd(vmacSta_p,(IEEEtypes_ScanCmd_t *)&SmeCmd.Body.ScanCmd);
#endif
}
#endif /* AP_SITE_SURVEY */

#ifdef IEEE80211H
/* API open as user interface */
#ifdef IEEE80211H_NOTWIFI
void StartSendMREQUESTCmd(void)
{
}
#endif
#define	MAX(a,b) (((a)>(b))?(a):(b))
WL_STATUS StartSendChannelSwitchCmd(UINT8 Mode, UINT8 channel)
{
	//api_Cmd_t *apiCmdQueue;
	WL_STATUS err = WL_STATUS_ERR;
	//    unsigned short ticks;


#ifdef DEBUG_ENABLE    
	printf("TIMER:=notify smeMain_CHANNELSWITCH_START event\n\r");
#endif /* DEBUG_ENABLE */

	return err;
}



/* API used only for SMETASK */
#define IS_ACCEPT_AUTONOMOUS_REPORT 1  /* 0: no autonomous report allowed 1: autonomous report allowed */
static WL_STATUS SendMREQUESTCmd(vmacApInfo_t *vmacSta_p,IEEEtypes_MacAddr_t *MacAddr_p, UINT8 MeasuredChannel)
{    
#ifdef AP_BUFFER
	MIB_STA_CFG  *mib_StaCfg_p=vmacSta_p->Mib802dot11->StationConfig;
	macmgmtQ_SmeCmd_t *SmeCmd_p = NULL;
	extStaDb_measurement_info_t measureInfo;
#endif
#ifdef DEBUG_ENABLE
	printf("send out a MLME command to macMgmt task\n\r");
#endif /* DEBUG_ENABLE */
#ifdef AP_BUFFER
	if ((SmeCmd_p = (macmgmtQ_SmeCmd_t *)pool_GetBuf(smeCmdPoolId)) != NULL)
	{
		IEEEtypes_MRequestCmd_t *MrequestCmd_p = &SmeCmd_p->Body.MrequestCmd;
		memset(SmeCmd_p, 0, sizeof(macmgmtQ_SmeCmd_t));
		SmeCmd_p->CmdType = SME_CMD_MREQUEST;
		memcpy(SmeCmd_p->Body.MrequestCmd.PeerStaAddr, MacAddr_p, sizeof(IEEEtypes_MacAddr_t));

		MrequestCmd_p->DiaglogToken = getDiaglogToken(); /* should be unique */
		MrequestCmd_p->MeasureItems = 1;//for now just check for basic request ftang 3;

		/* the first measurement request IE */
		MrequestCmd_p->MeasureReqSet[0].MeasurementToken = getMeasurementToken(); 
		MrequestCmd_p->MeasureReqSet[0].Mode.Enable = 1;    /* only enable BASIC measurement */
		MrequestCmd_p->MeasureReqSet[0].Mode.Request = 0;   /* AP don't accept any measurement requests */
		MrequestCmd_p->MeasureReqSet[0].Mode.Report = IS_ACCEPT_AUTONOMOUS_REPORT;    /* if AP accept autonomous report */
		MrequestCmd_p->MeasureReqSet[0].Type = TYPE_REQ_BASIC;
		MrequestCmd_p->MeasureReqSet[0].Request.Channel = MeasuredChannel;  
		memset(MrequestCmd_p->MeasureReqSet[0].Request.StartTime, 0, 8); /* start to test immediately */
		MrequestCmd_p->MeasureReqSet[0].Request.Duration = 3 * mib_StaCfg_p->BcnPeriod; /* how long the measrement goes */        

		/* record it 
		* now we only support Basic measurement
		* so just record what we are interested in
		*/

		if (STATE_SUCCESS == extStaDb_GetMeasurementInfo(vmacSta_p,MacAddr_p, &measureInfo))
		{
			measureInfo.DiaglogToken = MrequestCmd_p->DiaglogToken;
			measureInfo.mBasic.mToken = MrequestCmd_p->MeasureReqSet[0].MeasurementToken;
			memcpy(measureInfo.mBasic.measureStartTime, MrequestCmd_p->MeasureReqSet[0].Request.StartTime, 8);
			measureInfo.mBasic.measureDuration = MrequestCmd_p->MeasureReqSet[0].Request.Duration;

			extStaDb_SetMeasurementInfo(vmacSta_p,MacAddr_p, &measureInfo);
		}    
		macMgmtQ_SmeWriteNoBlock(vmacSta_p,SmeCmd_p);          
		return(OS_SUCCESS);
	}
#endif
	return(OS_FAIL);
}

WL_STATUS SendMREPORTCmd(vmacApInfo_t *vmacSta_p,IEEEtypes_MReportCmd_t *MReportCmd_p)
{
	//	MIB_802DOT11 *mib=vmacSta_p->Mib802dot11;
	macmgmtQ_SmeCmd_t SmeCmd;
	macmgmtQ_SmeCmd_t *SmeCmd_p = &SmeCmd;

	memset(SmeCmd_p, 0, sizeof(macmgmtQ_SmeCmd_t));  
	SmeCmd_p->CmdType = SME_CMD_MREPORT;
	memcpy(&SmeCmd_p->Body.MreportCmd, MReportCmd_p, sizeof(IEEEtypes_MReportCmd_t));
	macMgmtQ_SmeWriteNoBlock(vmacSta_p,SmeCmd_p);          
	return(OS_SUCCESS);
}




/*
* the api to process received mlme msg
*/
static void smeMrequestIndProcess(vmacApInfo_t *vmacSta_p,IEEEtypes_MRequestInd_t *MrequestInd_p)
{
	MIB_802DOT11 *mib=vmacSta_p->Mib802dot11;
	MIB_STA_CFG  *mib_StaCfg_p=vmacSta_p->Mib802dot11->StationConfig;
	/* reject all measurement requests send from other stations */
	IEEEtypes_MReportCmd_t MreportCmd;
	UINT32 loop = 0;

	if (((*(mib->mib_ApMode) != AP_MODE_A_ONLY)&&(*(mib->mib_ApMode)!=AP_MODE_AandG)) ||
		(mib_StaCfg_p->SpectrumManagementRequired != TRUE))
		return;

	memset(&MreportCmd, 0, sizeof(IEEEtypes_MReportCmd_t));

	memcpy(&MreportCmd.PeerStaAddr, &MrequestInd_p->PeerStaAddr, sizeof(IEEEtypes_MacAddr_t));

	/* identify the measurement transaction */
	MreportCmd.DiaglogToken = MrequestInd_p->DiaglogToken;

	MreportCmd.ReportItems = MrequestInd_p->RequestItems;

	//for now just go with basic report  for (loop = 0; loop<MreportCmd.ReportItems; loop++)
	loop=0;
	{
		MreportCmd.MeasureRepSet[loop].MeasurementToken = MrequestInd_p->MeasureReqSet[loop].MeasurementToken;
		MreportCmd.MeasureRepSet[loop].Mode.Refused = 1;
		MreportCmd.MeasureRepSet[loop].Type = MrequestInd_p->MeasureReqSet[loop].Type;
	}

	SendMREPORTCmd(vmacSta_p,&MreportCmd);

	return;
}

static void smeMrequestCfrmProcess(vmacApInfo_t *vmacSta_p,IEEEtypes_MRequestCfrm_t *MrequestCfrm_p)
{
	MIB_802DOT11 *mib=vmacSta_p->Mib802dot11;
	MIB_STA_CFG  *mib_StaCfg_p=vmacSta_p->Mib802dot11->StationConfig;
	if (((*(mib->mib_ApMode) != AP_MODE_A_ONLY)&&(*(mib->mib_ApMode)!=AP_MODE_AandG)) ||
		(mib_StaCfg_p->SpectrumManagementRequired != TRUE))
		return;



	return;
}                   

static void smeMeasureCfrmProcess(vmacApInfo_t *vmacSta_p,IEEEtypes_MeasureCfrm_t *MeasureCfrm_p)
{
	MIB_802DOT11 *mib=vmacSta_p->Mib802dot11;
	MIB_STA_CFG  *mib_StaCfg_p=vmacSta_p->Mib802dot11->StationConfig;
	if (((*(mib->mib_ApMode) != AP_MODE_A_ONLY)&&(*(mib->mib_ApMode)!=AP_MODE_AandG)) ||
		(mib_StaCfg_p->SpectrumManagementRequired != TRUE))
		return;


	return;
}

static void semUpdateChannelUtilizationStatus(basic_info *basic_measurement_p)
{    
	UINT32 loop = 0;

	do
	{
		if (semChannelUtilizationInfo[loop].channel == basic_measurement_p->measuredChannel)
		{
			semChannelUtilizationInfo[loop].mMap = basic_measurement_p->mMap;
			break;
		}

		loop++;
	} while (loop < IEEE_80211_MAX_NUMBER_OF_CHANNELS);

	return;
}

static void smeChannelSelect(UINT8 *targetChannel_p)
{    
	UINT32 loop = 0;

	do
	{
		/* start from A band */
		if (semChannelUtilizationInfo[loop].channel < 36)
		{
			loop++;
			continue;
		}

		/* no radar signal is detected in this channel */
		if (semChannelUtilizationInfo[loop].mMap.Radar == 0)
			break;        

		loop++;
	} while (loop < IEEE_80211_MAX_NUMBER_OF_CHANNELS);

	if (loop == IEEE_80211_MAX_NUMBER_OF_CHANNELS)
	{
		/* no clean channel in the record */

		/* return current channel */
		*targetChannel_p = RfSwitchChanA;        
	}
	else
		*targetChannel_p = semChannelUtilizationInfo[loop].channel;

	return;
}


static void smeMreportIndProcess(vmacApInfo_t *vmacSta_p,IEEEtypes_MacAddr_t *MacAddr_p, IEEEtypes_MReportInd_t *MreportInd_p)
{
	MIB_802DOT11 *mib=vmacSta_p->Mib802dot11;
	MIB_STA_CFG  *mib_StaCfg_p=vmacSta_p->Mib802dot11->StationConfig;
	extStaDb_measurement_info_t measureInfo;
	UINT32 loop;

	if (((*(mib->mib_ApMode) != AP_MODE_A_ONLY)&&(*(mib->mib_ApMode)!=AP_MODE_AandG)) ||
		(mib_StaCfg_p->SpectrumManagementRequired != TRUE))
		return;

	if (STATE_SUCCESS != extStaDb_GetMeasurementInfo(vmacSta_p,MacAddr_p, &measureInfo))
		return;

	/* check the diaglog sequence */
	if ((MreportInd_p->DiaglogToken != 0) && (MreportInd_p->DiaglogToken != measureInfo.DiaglogToken))
		return; 

	for (loop = 0; loop < MreportInd_p->ReportItems; loop++)
	{
		switch(MreportInd_p->MeasureRepSet[loop].Type)
		{
		case TYPE_REP_BASIC:
			if ((MreportInd_p->DiaglogToken == 0) || (measureInfo.mBasic.mToken == MreportInd_p->MeasureRepSet[loop].MeasurementToken))
			{
				measureInfo.mBasic.capability = MreportInd_p->MeasureRepSet[loop].Mode.Incapable ? FALSE : TRUE;                    
				measureInfo.mBasic.mMap = MreportInd_p->MeasureRepSet[loop].Report.data.Map;                    
				measureInfo.mBasic.measuredChannel = MreportInd_p->MeasureRepSet[loop].Report.Channel;                    
			}
			break;

		case TYPE_REP_CCA:    
		case TYPE_REP_RPI:
		default:                

			break;
		}
	}

	extStaDb_SetMeasurementInfo(vmacSta_p,MacAddr_p, &measureInfo);
	semUpdateChannelUtilizationStatus(&measureInfo.mBasic);

	/*
	* 1. if station refuesed our measurement request
	*/

	/* 
	* 2. if radar activity is detected at current channel
	*/
	if ((measureInfo.mBasic.measuredChannel == RfSwitchChanA) && (measureInfo.mBasic.mMap.Radar == 1))
	{
		UINT8 targetChannel;
		/* some algorithm should be developed to do channel switch procedure 
		* we just change to another clean channel for currenly simple design
		*/

		/* 1. pick up a clean channel */
		smeChannelSelect(&targetChannel);

		/* 2. test target channel to see if it's really clean */

		/* 3. send channel switch command */
		StartSendChannelSwitchCmd(1 /* keep quiet before channel switch */, targetChannel);
	}
	return;
}

static void smeMreportCfrmProcess(vmacApInfo_t *vmacSta_p,IEEEtypes_MReportCfrm_t *MreportCfrm_p)
{
	MIB_802DOT11 *mib=vmacSta_p->Mib802dot11;
	MIB_STA_CFG  *mib_StaCfg_p=vmacSta_p->Mib802dot11->StationConfig;
	if (((*(mib->mib_ApMode) != AP_MODE_A_ONLY)&&(*(mib->mib_ApMode)!=AP_MODE_AandG)) ||
		(mib_StaCfg_p->SpectrumManagementRequired != TRUE))
		return;


	return;
}

static void smeMTpcAdptCfrmdProcess(vmacApInfo_t *vmacSta_p,IEEEtypes_TPCAdaptCfrm_t *TpcAdptCfrm_p)
{
	MIB_802DOT11 *mib=vmacSta_p->Mib802dot11;
	MIB_STA_CFG  *mib_StaCfg_p=vmacSta_p->Mib802dot11->StationConfig;
	if (((*(mib->mib_ApMode) != AP_MODE_A_ONLY)&&(*(mib->mib_ApMode)!=AP_MODE_AandG)) ||
		(mib_StaCfg_p->SpectrumManagementRequired != TRUE))
		return;


	return;
}

static void smeChannelSwitchIndProcess(vmacApInfo_t *vmacSta_p,IEEEtypes_ChannelSwitchInd_t *ChannelSwitchInd_p)
{
	MIB_802DOT11 *mib=vmacSta_p->Mib802dot11;
	MIB_STA_CFG  *mib_StaCfg_p=vmacSta_p->Mib802dot11->StationConfig;
	if (((*(mib->mib_ApMode) != AP_MODE_A_ONLY)&&(*(mib->mib_ApMode)!=AP_MODE_AandG)) ||
		(mib_StaCfg_p->SpectrumManagementRequired != TRUE))
		return;


	return;
}

static void smeChannelSwitchCfrmProcess(vmacApInfo_t *vmacSta_p, IEEEtypes_ChannelSwitchCfrm_t *ChannelSwitchCfrm_p)
{
	MIB_802DOT11 *mib=vmacSta_p->Mib802dot11;
	MIB_STA_CFG  *mib_StaCfg_p=vmacSta_p->Mib802dot11->StationConfig;
	if (((*(mib->mib_ApMode) != AP_MODE_A_ONLY)&&(*(mib->mib_ApMode)!=AP_MODE_AandG)) ||
		(mib_StaCfg_p->SpectrumManagementRequired != TRUE))
		return;


	return;
}
#ifdef AP_BUFFER
static UINT8 getDiaglogToken(void)
{
	return D_Token++;
}
static UINT8 getMeasurementToken(void)
{
	return M_Token++;
}
#endif
static void semChannelUtilizationInit(UINT8 *ChannelList)
{
	UINT32 loop;

	nbr_of_channel = strlen((const char *)ChannelList);

	for (loop = 0; loop < nbr_of_channel; loop++)
	{
		semChannelUtilizationInfo[loop].channel = ChannelList[loop];
		memset(&semChannelUtilizationInfo[loop].mMap, 0, sizeof(IEEEtypes_MeasurementRepMap_t));
	}

	return;    
}





void semShowChannelUtilizationStatus(void)
{
	//    UINT32 loop;
#ifdef DEBUG_ENABLE
	etherBugSend("the channel length is %d\n\r", nbr_of_channel);
	etherBugSend("Channel Measurement Report:\n\r");
	etherBugSend("Channel ID: status                            \n\r");
	etherBugSend("===========================================\n\r");
	for (loop = 0; loop < nbr_of_channel; loop++)
	{
		etherBugSend("Channel %d: BSS %d | OFDM %d | UnidentifiedSignal %d | Radar %d | Unmeasured %d\n\r", semChannelUtilizationInfo[loop].channel,
			semChannelUtilizationInfo[loop].mMap.BSS,
			semChannelUtilizationInfo[loop].mMap.OFDM,
			semChannelUtilizationInfo[loop].mMap.UnidentifiedSignal,
			semChannelUtilizationInfo[loop].mMap.Radar,
			semChannelUtilizationInfo[loop].mMap.Unmeasured);
	}
#endif
	return;
}
#endif /* IEEE80211H */


