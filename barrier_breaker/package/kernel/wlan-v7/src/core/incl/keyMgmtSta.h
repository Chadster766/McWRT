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


#ifndef _KEY_MGMT_STA_H_
#define _KEY_MGMT_STA_H_

#include "wltypes.h"
#include "mhsm.h"
#include "timer.h"
#include "IEEE_types.h"
#include "osif.h"
#include "mib.h"
#include "ds.h"
#include "macmgmtap.h"

#include "tkip.h"
#include "keyMgmtCommon.h"

#define EAPOL_TX_BUF        720
#define RSN_PSK_VALUE_LEN_MAX       32

#define RSN_PSK_PASS_PHRASE_LEN_MAX 64
#define RSN_CIPHER_VALUE_LEN_MAX    4
#define RSN_SUITE_VALUE_LEN_MAX     4

#define RSN_WPA_ID                  0x0
#define RSN_WPA2_ID                 0x1

#define RSN_TKIP_ID                 0x2
#define RSN_AES_ID                  0x4

#define RSN_PSK_ID                  0x2

struct _KeyMgmtInfoSta_struct;

typedef struct{
  Mhsm_t super;
  MhsmState_t sTop;
  MhsmState_t sta_hsk_start;
  MhsmState_t recvd_pwk_msg_1;
  MhsmState_t recvd_pwk_msg_3;
  MhsmState_t recvd_grp_msg_1;
  MhsmState_t sta_hsk_end;
  Timer  rsnSecuredTimer;
  struct _KeyMgmtInfoSta_struct * keyMgmtInfoSta_p;
 }keyMgmtStahsk_hsm_t;

typedef struct
{
    UINT8 PairwiseTempKey[TK_SIZE];
    UINT32 RSNPwkTxMICKey[2];
    UINT32 RSNPwkRxMICKey[2];
    UINT32 RSNDataTrafficEnabled; // Enabled after 4way handshake
    UINT32 RSNSecured;            // Enabled after group key is established
    UINT32 TxIV32;
    UINT32 RxIV32;
    UINT16 TxIV16;
}KeyData_t;

typedef struct _KeyMgmtInfoSta_struct
{
    UINT8 ANonce[NONCE_SIZE];
    UINT8 SNonce[NONCE_SIZE];
    UINT8 EAPOL_MIC_Key[EAPOL_MIC_KEY_SIZE];
    UINT8 EAPOL_Encr_Key[EAPOL_ENCR_KEY_SIZE];
    UINT8 PairwiseTempKey_tmp[TK_SIZE];
    UINT32 RSNPwkTxMICKey_tmp[2];
    UINT32 RSNPwkRxMICKey_tmp[2];
    UINT32 apCounterLo;       // last valid replay counter from authenticator
    UINT32 apCounterHi; 
    UINT32 apCounterZeroDone; // have we processed replay == 0?
    UINT32 staCounterLo;      // counter used in request EAPOL frames
    UINT32 staCounterHi;
    UINT8 RsnIEBuf[MAX_SIZE_RSN_IE_BUF];
    UINT8 TimeoutCtr;
    UINT16 Phase1KeyTx[5];
    UINT16 Phase1KeyRx[5];
    keyMgmtStahsk_hsm_t keyMgmtStaHskHsm;
    KeyData_t *pKeyData;
	void * vmacEntry_p; // point to the corresponding vmacEntry which points to this struct.
    MIC_Error_t  sta_MIC_Error;
}keyMgmtInfoSta_t;


extern MRVL_MIB_RSN_GRP_KEY mib_MrvlRSN_GrpKeyUr1[NUM_OF_WLMACS];
extern UINT8   tmpClientSSID[][32];
void defaultKeyMgmtInit(UINT8 phymacIndex);
void * ProcessEAPoLSta(IEEEtypes_8023_Frame_t *pEAPoLPckt, IEEEtypes_MacAddr_t *staAddr_p);
extern void MICCounterMeasureInvoke_Sta(vmacEntry_t *vmacEntry_p, BOOLEAN isUnicast);
#endif
