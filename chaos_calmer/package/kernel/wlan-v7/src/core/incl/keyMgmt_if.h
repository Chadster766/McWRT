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


#ifndef _KEYMGMT_IF_H_
#define _KEYMGMT_IF_H_

#define NONCE_SIZE          32
#define EAPOL_MIC_KEY_SIZE  16
#include "wltypes.h"
#include "IEEE_types.h"
#include "wl_hal.h"
#include "ds.h"
typedef enum
{
	IDLE,
	START4WAYHSK,
	KEYTIMEOUT,
	MSGRECVDEVT,
	KEYMGMTTIMEOUTEVT,
	GRPKEYTIMEOUTEVT
}keyMgmtEvent_e;

#define KEY_INFO_KEYTYPE    0x0800
#define KEY_INFO_REQUEST    0x0008
#define KEY_INFO_ERROR      0x0004


extern UINT32 isMsgInQ;

extern void ProcessKeyMgmtData(vmacApInfo_t *vmacSta_p,void *rx_FrmPtr,
							   IEEEtypes_MacAddr_t *SourceAddr,
							   keyMgmtEvent_e evt);
void KeyMgmtInit(vmacApInfo_t *vmacSta_p);
extern void * ProcessEAPoLAp(vmacApInfo_t *vmacSta_p, IEEEtypes_8023_Frame_t *pEthFrame, IEEEtypes_MacAddr_t *pMacStaAddr);
#endif
