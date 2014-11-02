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
 *
 * Purpose:
 *    This file contains the function prototypes and definitions for the
 *    MAC Management Service Module.
 *
 * Public Procedures:
 *    macMgmtMain_Init       Initialzies the MAC Management Service Task and
 *                           related components
 *    macMgmtMain_Start      Starts running the MAC Management Service Task
 *    macMgmtMain_SendEvent  Mechanism used to trigger an event indicating a
 *                           message has been received on one of the message
 *                           queues
 *
 * Notes:
 *    None.
 *
 *****************************************************************************/

#ifndef _MACMGMTMAIN_H_
#define _MACMGMTMAIN_H_

//=============================================================================
//                               INCLUDE FILES
//=============================================================================
#include "wltypes.h"
#include "IEEE_types.h"
#include "smeMain.h"

//=============================================================================
//                          PUBLIC TYPE DEFINITIONS
//=============================================================================


//=============================================================================
//                    PUBLIC PROCEDURES (ANSI Prototypes)
//=============================================================================

/******************************************************************************
 *
 * Name: macMgmtMain_Init
 *
 * Description:
 *   This routine is called to initialize the the MAC Management Service Task
 *   and related components.
 *
 * Conditions For Use:
 *   The timer component must be initialized first.
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
extern WL_STATUS macMgmtMain_Init( void );


/******************************************************************************
 *
 * Name: macMgmtMain_Start
 *
 * Description:
 *   This routine is called to start the MAC Management Service Task running.
 *
 * Conditions For Use:
 *   The task and its associated queues have been initialized by calling
 *   macMgmtMain_Init().
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
extern void macMgmtMain_Start( void );


/******************************************************************************
 *
 * Name: macMgmtMain_SendEvent
 *
 * Description:
 *   This routine provides the mechanism to trigger an event indicating to
 *   the MAC Management Service Task that a message has been written to one
 *   of its queues.
 *
 * Conditions For Use:
 *   The task and its associated queues has been initialized by calling
 *   macMgmtMain_Init().
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
extern WL_STATUS macMgmtMain_SendEvent( UINT16 Event );


/******************************************************************************
 *
 * Name: macMgmtMain_TimeoutHdlr
 *
 * Description:
 *   Callback routine used to handle timeouts.
 *
 * Conditions For Use:
 *   The task and its associated queues has been initialized by calling
 *   macMgmtMain_Init().
 *
 * Arguments:
 *   Arg1 (i  ): Data - the data associated with a timeout
 *
 * Return Value:
 *   None.
 *
 * Notes:
 *   None.
 *
 *****************************************************************************/
extern void macMgmtMain_TimeoutHdlr(UINT32 xx, os_Addrword_t  Data );


typedef enum 
{
   SCAN_CMD,
   RESCAN
} macMgmtQ_ScanSrc_e;
typedef UINT8 macMgmtQ_ScanSrc_t;
   //
   // The two different types of scans - SCAN_CMD is a command to perform
   // a scan that can be directed by the GUI; RESCAN is an automatic re-scan
   // for APs when it has been determined that contact with an AP for which
   // there was an association has been lost.
   //

typedef struct macMgmtQ_ScanCmd_t
{
   macMgmtQ_ScanSrc_t   ScanSrc;
   IEEEtypes_ScanCmd_t  Cmd;
} macMgmtQ_ScanCmd_t;
   //
   // The information used to carry out a scan for APs; this structure is
   // used simply to include the scan source information in addition to
   // the scan parameters defined by the IEEE 802.11 standard
   //


/*---------------------------------*/
/* IEEE 802.11 Management Messages */
/*---------------------------------*/
typedef struct tx80211MacQ_MgmtMsg_t
{
   IEEEtypes_MgmtHdr_t  Hdr;
   union
   {
      IEEEtypes_Bcn_t          Bcn;
      IEEEtypes_DisAssoc_t     DisAssoc;
      IEEEtypes_AssocRqst_t    AssocRqst;
      IEEEtypes_AssocRsp_t     AssocRsp;
      IEEEtypes_ReassocRqst_t  ReassocRqst;
      IEEEtypes_ReassocRsp_t   RassocRsp;
      IEEEtypes_ProbeRqst_t    ProbeRqst;
      IEEEtypes_ProbeRsp_t     ProbeRsp;
      IEEEtypes_Auth_t         Auth;
      IEEEtypes_Deauth_t       Deauth;
   } Body;
   UINT32  FCS;
} tx80211MacQ_MgmtMsg_t;

   // Structure used for management messages placed on the 802.11 Transmit
   // MAC Data Service Task's management message queue. The fields in the
   // structure are:
   // 1)  Hdr         - The header for an 802.11 msg
   // 2a) Bcn         - Beacon message body
   //  b) DisAssoc    - Disassociate message body
   //  c) AssocRqst   - Associate request message body
   //  d) AssocRsp    - Associate response message body
   //  e) ReassocRqst - Reassociate request message body
   //  f) ReassocRsp  - Reassociate response message body
   //  g) ProbeRqst   - Probe request message body
   //  h) ProbeRsp    - Probe response message body
   //  i) Auth        - Authentication message body
   //  j) Deauth      - Deauthentication message body
   // 3)  FCS         - Frame check sequence


typedef struct smeQ_ScanCmd_t
{
   UINT16                 AutoAssoc;
   IEEEtypes_ScanCmd_t    Cmd;
} PACK_END smeQ_ScanCmd_t;
   //
   // The information used to carry out a scan for APs; this structure is
   // used simply to include the host information in addition to
   // the scan parameters defined by the IEEE 802.11 standard
   //

#endif /* _MACMGMTMAIN_H_ */
