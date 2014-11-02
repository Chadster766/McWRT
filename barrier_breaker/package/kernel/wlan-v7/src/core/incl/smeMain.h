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
*    This file contains the function prototypes and definitions for the
*    Station Management Entity (SME) Main module.
*
* Public Procedures:
*    smeMain_Init       Initialzies the SME Task and related components
*    smeMain_Start      Starts running the SME Task
*    smeMain_SendEvent  Mechanism used to trigger an event indicating a
*                           message has been received on one of the message
*                           queues
*
* Notes:
*    None.
*
*****************************************************************************/

#ifndef _SMEMAIN_H_
#define _SMEMAIN_H_

/*============================================================================= */
/*                               INCLUDE FILES */
/*============================================================================= */
#include "wltypes.h"
#include "IEEE_types.h"
#include "wl_mib.h"
#include "wl_hal.h"
#ifdef STA

#endif
#include "osif.h"
#include "dfs.h"

/*============================================================================= */
/*                          PUBLIC TYPE DEFINITIONS */
/*============================================================================= */
#define smeMain_MGMT_MSG_RCVD      (1<<0)
#define smeMain_CMD_RCVD           (1<<1)
#define smeMain_ERROR_RCVD         (1<<2)

/*------------------------------------------------------------*/
/* The types of problems that can be reported to the SME Task */
/*------------------------------------------------------------*/
typedef enum
{
	SMEQ_TX_ERROR,
	SMEQ_BCN_LOST
} smeQ_Error_e;
typedef UINT8 smeQ_Error_t;

typedef UINT16 smeQ_TxError_t;
typedef UINT16 smeQ_BcnError_t;

typedef struct smeQ_ScanRsp_t
{
	UINT16                 BufSize;
	IEEEtypes_ScanCfrm_t   Rsp;
} PACK_END smeQ_ScanRsp_t;

/*-----------------------------------------------------------------*/
/* Management messages coming from the MAC Management Service Task */
/*-----------------------------------------------------------------*/
typedef struct smeQ_MgmtMsg_t
{
	void *vmacSta_p;
	IEEEtypes_SmeNotify_t MsgType;
	union
	{
		IEEEtypes_PwrMgmtCfrm_t PwrMgmtCfrm;
		smeQ_ScanRsp_t            ScanCfrm;
		IEEEtypes_JoinCfrm_t JoinCfrm;
		IEEEtypes_AuthCfrm_t AuthCfrm;
		IEEEtypes_AuthInd_t AuthInd;
		IEEEtypes_DeauthCfrm_t DeauthCfrm;
		IEEEtypes_DeauthInd_t DeauthInd;
		IEEEtypes_AssocCfrm_t AssocCfrm;
		IEEEtypes_AssocInd_t AssocInd;
		IEEEtypes_ReassocCfrm_t ReassocCfrm;
		IEEEtypes_ReassocInd_t ReassocInd;
		IEEEtypes_DisassocCfrm_t DisassocCfrm;
		IEEEtypes_DisassocInd_t DisassocInd;
		IEEEtypes_ResetCfrm_t ResetCfrm;
		IEEEtypes_StartCfrm_t StartCfrm;
#ifdef IEEE80211H
		IEEEtypes_MRequestInd_t MrequestInd;
		IEEEtypes_MRequestCfrm_t MrequestCfrm;
		IEEEtypes_MeasureCfrm_t MeasureCfrm;
		IEEEtypes_MReportCfrm_t MreportCfrm;
		IEEEtypes_MReportInd_t MreportInd;
		IEEEtypes_ChannelSwitchCfrm_t ChannelSwitchCfrm;
		IEEEtypes_ChannelSwitchInd_t ChannelSwitchInd;
		IEEEtypes_TPCAdaptCfrm_t TPCAdaptCfrm;
#endif /* IEEEtypes_MRequestInd_t */
#ifdef MRVL_DFS
		Dfs_ChanSwitchCfrm_t      ChanSwitchCfrm;
		Dfs_RadarDetInd_t      	RadarDetectionInd;
#endif
	} Msg;
}
PACK_END smeQ_MgmtMsg_t ;
//smeQ_MgmtMsg_t ;

/*--------------------------------------------------------------------*/
/* Problems notifications - either transmit errors reported by the    */
/* 802.11 MAC Messages Transmitted Task or a beacon lost notification */
/* reported by the MAC Management Service Task.                       */
/*--------------------------------------------------------------------*/
typedef struct smeQ_ErrorMsg_t
{
	smeQ_Error_t ErrorType;
	union
	{
		smeQ_TxError_t TxErrors;
		smeQ_BcnError_t BcnError;
	} Msg;
}
smeQ_ErrorMsg_t;


/*============================================================================= */
/*                    PUBLIC PROCEDURES (ANSI Prototypes) */
/*============================================================================= */


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
*****************************************************************************/
extern WL_STATUS smeMain_Init( vmacApInfo_t *vmacSta_p );

/******************************************************************************
*
* Name: smeMain_SendEvent
*
* Description:
*   This routine provides the mechanism to trigger an event indicating to
*   the SME Task that a message has been written to one of its queues.
*
* Conditions For Use:
*   The task and its associated queues has been initialized by calling
*   smeMain_Init().
*
* Arguments:
*   Arg1 (i  ): Event - value indicating which queue has received a message
*
* Return Value:
*   Status indicating success or failure
*
* Notes:
*   None.
*
*****************************************************************************/

extern WL_STATUS smeQ_MgmtWriteNoBlock( smeQ_MgmtMsg_t *MgmtMsg_p );

/******************************************************************************
*
* Name: smeQ_CbProcWriteNoBlock
*
* Description:
*   This routine is called to write a message to the queue where messages
*   from the CB Processor Task are placed for the SME Task. If writing
*   to the queue cannot immediately occur, then the routine returns with
*   a failure status (non-blocking).
*
* Conditions For Use:
*   The queue has been initialized by calling smeQ_Init().
*
* Arguments:
*   Arg1 (i  ): CbCmd_p - a pointer to the message to be placed on the
*                         queue
*
* Return Value:
*   Status indicating success or failure
*
* Notes:
*   None.
*
*****************************************************************************/
/******************************************************************************
*
* Name: smeQ_ErrorWriteNoBlock
*
* Description:
*   This routine is called to write a message that indicates a system fault
*   has occured to the appropriate queue for the SME Task. If writing to
*   the queue cannot immediately occur, then the routine returns with a
*   failure status (non-blocking).
*
* Conditions For Use:
*   The queue has been initialized by calling smeQ_Init().
*
* Arguments:
*   Arg1 (i  ): ErrorMsg_p - a pointer to the message to be placed on
*                            the queue
*
* Return Value:
*   Status indicating success or failure
*
* Notes:
*   None.
*
*****************************************************************************/
UINT32 SendStartCmd(vmacApInfo_t *vmacSta_p);
void SendResetCmd(vmacApInfo_t *vmacSta_p, int);
#endif /* _SMEMAIN_H_ */

