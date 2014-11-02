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


#ifndef _HANDHSK_H_
#define _HANDHSK_H_

#include "mhsm.h"

#include "tkip.h"
#include "wl_mib.h"
#include "wl_hal.h"

#include "timer.h"

#define S_SWAP(a,b) do { unsigned char  t = S[a]; S[a] = S[b]; S[b] = t; } while(0)
#define WS_SWAP(a,b) do { unsigned char  t = WS[a]; WS[a] = WS[b]; WS[b] = t; } while(0)
typedef struct{
	Mhsm_t super;
	MhsmState_t sTop;
	MhsmState_t hsk_start;
	MhsmState_t waiting_4_msg_2;
	MhsmState_t waiting_4_msg_4;
	MhsmState_t waiting_4_grpmsg_2;
	MhsmState_t hsk_end;
	Timer timer;
	struct timer_list keyTimer;
	UINT8 timeout_ctr;
	void *pData;
	vmacApInfo_t *vmacSta_p;
}keyMgmthsk_hsm_t;
#if !defined (ECL_WPA)
typedef struct
{
	UINT8 ANonce[NONCE_SIZE];
	UINT8 SNonce[NONCE_SIZE];
	UINT8 EAPOL_MIC_Key[EAPOL_MIC_KEY_SIZE];
	UINT8 EAPOL_Encr_Key[EAPOL_ENCR_KEY_SIZE];
	UINT8 PairwiseTempKey1[TK_SIZE];
	UINT8 RSNPwkTxMICKey[8];
	UINT8 RSNPwkRxMICKey[8];
	UINT8 PairwiseTempKey1_tmp[TK_SIZE];
	UINT8 RSNPwkTxMICKey_tmp[8];
	UINT8 RSNPwkRxMICKey_tmp[8];
	UINT32 counter; /*store only the lower counter */
	/*actually we should store both the lower and the upper counter. But */
	/*  in our implementation we only increment the lower counter */
	UINT8 RsnIEBuf[MAX_SIZE_RSN_IE_BUF];
	UINT32 TxIV32;
	UINT32 RxIV32;
	UINT16 TxIV16;
	UINT8 RSNDataTrafficEnabled;
	UINT8 TimeoutCtr;
	UINT16 Phase1KeyTx[5];
	UINT16 Phase1KeyRx[5];
	UINT8 PMK[32];
	/*keyMgmtState_e keyMgmtState; */
}keyMgmtInfo_t;
#endif
typedef enum
{
	STA_ASSO_EVT,
	MSGRECVD_EVT,
	KEYMGMTTIMEOUT_EVT,
	GRPKEYTIMEOUT_EVT,
	UPDATEKEYS_EVT
}keyMgmthsk_event_e;
typedef UINT8 keyMgmthsk_event_t;

typedef struct
{
	Timer timer;
	MIC_Fail_State_t status;
	BOOLEAN MICCounterMeasureEnabled;//indicates if counter Measures is enabled
	UINT32 disableStaAsso;//1= Sta Association is disabled
}MIC_Error_t;


void HskCtor(keyMgmthsk_hsm_t *me);
extern void SendKeyMgmtInitEvent(vmacApInfo_t *vmacSta_p);
extern void KeyMgmtReset(vmacApInfo_t *vmacSta_p);

#if !defined(PORTABLE_ARCH)
/*
extern inline UINT32 (*DoWPAAndSchedFrameFp)(WLAN_TX_FRAME *Frame_p, keyMgmtInfo_t *pKeyMgmtInfo,
UINT16 ethertype, BOOLEAN BcastFlag);
extern inline UINT32 DoTKIPAndSchedFrameAP(WLAN_TX_FRAME *Frame_p, keyMgmtInfo_t *pKeyMgmtInfo,
UINT16 ethertype, BOOLEAN BcastFlag);
extern inline USR_BUF_DESC *ProcessTKIPPcktAP(keyMgmtInfo_t *pkeyInfo, USR_BUF_DESC *usp,
WLAN_RX_FRAME **Data11Frame_pp);
extern inline USR_BUF_DESC *(*ProcessWPAPcktFp)(keyMgmtInfo_t *pkeyInfo, USR_BUF_DESC *usp,
WLAN_RX_FRAME **Data11Frame_pp);
#ifdef AP_WPA2
inline UINT32 DoCCMPAndSchedFrameAP(WLAN_TX_FRAME *Frame_p, keyMgmtInfo_t *pKeyMgmtInfo,
UINT16 ethertype, BOOLEAN BcastFlag);
inline USR_BUF_DESC *ProcessCCMPPcktAP(keyMgmtInfo_t *pkeyInfo,
USR_BUF_DESC *usp,
WLAN_RX_FRAME **Data11Frame_pp);
#endif
*/
#else
extern UINT32 (*DoWPAAndSchedFrameFp)(WLAN_TX_FRAME *Frame_p, keyMgmtInfo_t *pKeyMgmtInfo,
									  UINT16 ethertype, BOOLEAN BcastFlag);
extern INLINE UINT32 DoTKIPAndSchedFrameAP(WLAN_TX_FRAME *Frame_p, keyMgmtInfo_t *pKeyMgmtInfo,
										   UINT16 ethertype, BOOLEAN BcastFlag);
extern INLINE USR_BUF_DESC *ProcessTKIPPcktAP(keyMgmtInfo_t *pkeyInfo, USR_BUF_DESC *usp,
											  WLAN_RX_FRAME **Data11Frame_pp);
extern USR_BUF_DESC *(*ProcessWPAPcktFp)(keyMgmtInfo_t *pkeyInfo, USR_BUF_DESC *usp,
										 WLAN_RX_FRAME **Data11Frame_pp);
#ifdef AP_WPA2
inline UINT32 DoCCMPAndSchedFrameAP(WLAN_TX_FRAME *Frame_p, keyMgmtInfo_t *pKeyMgmtInfo,
									UINT16 ethertype, BOOLEAN BcastFlag);
inline USR_BUF_DESC *ProcessCCMPPcktAP(keyMgmtInfo_t *pkeyInfo,
									   USR_BUF_DESC *usp,
									   WLAN_RX_FRAME **Data11Frame_pp);
#endif
#endif
#endif

