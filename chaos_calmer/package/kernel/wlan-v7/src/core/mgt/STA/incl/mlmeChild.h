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

/*******************************************************************************************
*
* File: mlmeChild.h
*        MLME Child Module Header
* Description:  Implementation of the MLME Child Module Services
*
*******************************************************************************************/

#ifndef MAC_MLME_CHILD
#define MAC_MLME_CHILD

#define MAX_MLME_CHILD_SESSIONS			26

/* Control Bits Map for Child Session */
#define MLME_CHILD_SET_ADDR_TO_HW		(1<<0)

extern vmacEntry_t *childSrv_StartSession(UINT8 macIndex, 
										  UINT8 *macAddr,
 										  void *callBack_fp,
                                          UINT32 controlParam);
extern SINT32 childSrv_TerminateSession(vmacId_t vMacId);
extern SINT32 childSrv_TerminateAllLinks(void);
extern halMacId_t childSrv_RegisterBlockAddress(vmacStaInfo_t * vStaInfo_p,
                                                UINT8 macIndex, 
												UINT8 *macAddr,
												UINT8 addrMask);

#endif /* MAC_MLME_CHILD */





