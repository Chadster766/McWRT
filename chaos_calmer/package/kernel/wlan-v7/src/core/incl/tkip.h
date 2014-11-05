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


#ifndef _TKIP_H_
#define _TKIP_H_

#include "wl_mib.h"
#include "wl_hal.h"
#include "ds.h"
#include "keyMgmtCommon.h"

#if !defined(ECL_WPA)
#define WPA_NO_WAIT
#endif

// fixed algorithm "parameters"
#define PHASE1_LOOP_CNT   8    // this needs to be "big enough"
#define TA_SIZE           6    //  48-bit transmitter address
#define TK_SIZE          16    // 128-bit temporal key
#define P1K_SIZE         10    // 80-bit Phase1 key
#define RC4_KEY_SIZE     16    // 128-bit RC4KEY (104 bits unknown)

#define IV_SIZE          4
#define EIV_SIZE         4
#define MIC_SIZE         8
#define ICV_SIZE         4
#define ExtIV                 0x20

#define MICFAILTIMEOUTTIME  5000//5000 x 12  = 60000 msec = 60 sec

// macros for extraction/creation of UINT8/UINT16 values
#define   Lo8(v16)   ((UINT8)( (v16)       & 0x00FF))
#define   Hi8(v16)   ((UINT8)(((v16) >> 8) & 0x00FF))
#define  Lo16(v32)   ((UINT16)( (v32)       & 0xFFFF))
#define  Hi16(v32)   ((UINT16)(((v32) >>16) & 0xFFFF))

#define RotR1(v16)   ((((v16) >> 1) & 0x7FFF) ^ (((v16) & 1) << 15))
#define  Mk16(hi,lo) ((lo) ^ (((UINT16)(hi)) << 8))

// select the Nth 16-bit word of the temporal key UINT8 array TK[]
#define  TK16(N)     Mk16(TK[2*(N)+1],TK[2*(N)])

// S-box lookup: 16 bits --> 16 bits
#define _S_(v16)     (Sbox[0][Lo8(v16)] ^ Sbox[1][Hi8(v16)])

/*
#define ROL32(A,n) (((A) << (n)) | ((A) >> (32-(n))))
#define ROR32(A,n) ROL32((A),32-(n))
*/
#if !defined(ECL_WPA)
#define ROR32(_out,_in,len) __asm("mov	%0, %1, ror %2\n" : "=r" (_out) : "r" (_in), "g" (len))
#define ROL32(out,in,len) ROR32(out,in,(32-len))
#else
#define ROL32(B,A,n) B = (((A) << (n)) | ((A) >> (32-(n))))
#define ROR32(A,B,n) ROL32((A),(B),(32-(n)))
#endif

#define XSWAP(A) (((A & 0xff00ff00) >> 8) | ((A & 0x00ff00ff) << 8))

/*
#define CheckMIC(calc_MIC, rx_MIC) \
((*calc_MIC == *rx_MIC) && \
(*(calc_MIC + 1) == *(rx_MIC + 1)))
*/
#define CheckMIC(calc_MIC, rx_MIC) \
	((*calc_MIC == *(UINT32*)rx_MIC) && \
	(*(calc_MIC + 1) == *(UINT32*)(rx_MIC + 4)))

typedef enum
{
	Rx,
	Tx
}PacketType_e;
typedef UINT8 PacketType_t;

typedef enum
{
	NO_MIC_FAILURE,
	FIRST_MIC_FAIL_IN_60_SEC,
	SECOND_MIC_FAIL_IN_60_SEC
}MIC_Fail_State_e;
typedef UINT8 MIC_Fail_State_t;

/* Macro to put things in ITCM memory region */

//Function Prototypes
void generateRand(UINT8 *Data, UINT32 length);
void EncryptGrpKey(UINT8 *Encr_Key, UINT8 *IV, UINT8 *Data, UINT16 data_length);
#if !defined(ECL_WPA)
void Insert8021xHdr(Hdr_8021x_t *pHdr, UINT16 data_length);
#endif
void apppendEAPOL_MIC(UINT8 *data, UINT8 *MIC_Data);
Status_e CompareRSN_IE(UINT8 *pRSN_IE_data, UINT8 *PrbReqRSNIE);
Status_e checkEAPOL_MIC(UINT8 *MIC1, UINT8 *MIC2, UINT8 length);
#ifdef AP_WPA2
inline void appendMIC(UINT8 *data_ptr,UINT32 *MIC);
void ComputeEAPOL_MIC(vmacApInfo_t *vmacSta_p,UINT8 *data, UINT16 data_length,
					  UINT8 *MIC_Key, UINT8 MIC_Key_length,
					  UINT8 *computed_MIC, UINT8* RsnIEBuf);
void genetate_PTK(vmacApInfo_t *vmacSta_p,UINT8 *PMK, IEEEtypes_MacAddr_t *pAddr1,
				  IEEEtypes_MacAddr_t *pAddr2,
				  UINT8 *pNonce1, UINT8 *pNonce2, UINT8 *pPTK);
#else
void ComputeEAPOL_MIC(vmacApInfo_t *vmacSta_p,UINT8 *data, UINT16 data_length,
					  UINT8 *MIC_Key, UINT8 MIC_Key_length,
					  UINT8 *computed_MIC);
void genetate_PTK(vmacApInfo_t *vmacSta_p,IEEEtypes_MacAddr_t *pAddr1, IEEEtypes_MacAddr_t *pAddr2,
				  UINT8 *pNonce1, UINT8 *pNonce2, UINT8 *);
#endif
void Phase1(UINT16 *P1K, const UINT8 *TK, const UINT8 *TA, UINT32 IV32);
inline void Phase2(UINT8 *RC4KEY, const UINT8 *TK, const UINT16 *P1K, UINT16 IV16) ;
extern void (*Phase2Fp)(UINT8 *RC4KEY, const UINT8 *TK, const UINT16 *P1K, UINT16 IV16);
inline void MichaelTx(const IEEEtypes_GenHdr_t *pHdr,
					  const UINT32 *Key, UINT32 *data, UINT32 *res, UINT32 prio) ;

inline void MichaelRx(const IEEEtypes_GenHdr_t *pHdr,
					  const UINT32 *Key, UINT32 *data, UINT32 *res, UINT32 prio) ;
#ifdef STA_QOS
inline void MichaelRxSta(const IEEEtypes_GenHdr_t *pHdr,
						 const UINT32 *Key, UINT32 *data, UINT32 *res, UINT32 prio) ;
inline void MichaelTxSta(const IEEEtypes_GenHdr_t *pHdr,
						 const UINT32 *Key, UINT32 *data, UINT32 *res, UINT32 prio) ;

#else
inline void MichaelRxSta(const IEEEtypes_GenHdr_t *pHdr,
						 const UINT32 *Key, UINT32 *data, UINT32 *res) ;
inline void MichaelTxSta(const IEEEtypes_GenHdr_t *pHdr,
						 const UINT32 *Key, UINT32 *data, UINT32 *res) ;
#endif
#ifdef STA_QOS
extern void (*MichaelRxStaFp)(const IEEEtypes_GenHdr_t *pHdr,
							  const UINT32 *Key, UINT32 *data, UINT32 *res,UINT32 prio);
extern void (*MichaelTxStaFp)(const IEEEtypes_GenHdr_t *pHdr,
							  const UINT32 *Key, UINT32 *data, UINT32 *res,UINT32 prio);

#else
extern void (*MichaelRxStaFp)(const IEEEtypes_GenHdr_t *pHdr,
							  const UINT32 *Key, UINT32 *data, UINT32 *res);
extern void (*MichaelTxStaFp)(const IEEEtypes_GenHdr_t *pHdr,
							  const UINT32 *Key, UINT32 *data, UINT32 *res);
#endif

void MicErrTimerExpCb(vmacApInfo_t *vmacSta_p);
#if !defined(PORTABLE_ARCH)
inline void DoTKIPEncrypt(UINT8 *des, UINT8 *src, UINT32 len, UINT8 *key);
inline UINT32 DoTKIPDecrypt(UINT8 *des, UINT8 *src, UINT32 len, UINT8 *key);
#else
INLINE void DoTKIPEncrypt(UINT8 *des, UINT8 *src, UINT32 len, UINT8 *key);
INLINE UINT32 DoTKIPDecrypt(UINT8 *des, UINT8 *src, UINT32 len, UINT8 *key);
#endif

#define W81_WEU_CFG_BASE 0x9000a400
#define WEU_REG_ADDR(offset) ((W81_WEU_CFG_BASE) | offset)
#define WEU_TK_0_REG WEU_REG_ADDR(0)
#define WEU_TK_1_REG WEU_REG_ADDR(4)
#define WEU_TK_2_REG WEU_REG_ADDR(8)
#define WEU_TK_3_REG WEU_REG_ADDR(0xc)
#define WEU_TA_Lo_REG WEU_REG_ADDR(0x10)
#define WEU_TA_Hi_REG WEU_REG_ADDR(0x14)
#define WEU_IV_Byte3_REG WEU_REG_ADDR(0x18)
#define WEU_TSC_Lo_REG WEU_REG_ADDR(0x1c)
#define WEU_TSC_Hi_REG WEU_REG_ADDR(0x20)
#define WEU_DA_Lo_REG WEU_REG_ADDR(0x24)
#define WEU_DA_Hi_REG WEU_REG_ADDR(0x28)
#define WEU_SA_Lo_REG WEU_REG_ADDR(0x2c)
#define WEU_SA_Hi_REG WEU_REG_ADDR(0x30)
#define WEU_PRIORITY_REG WEU_REG_ADDR(0x34)
#define WEU_DMA_SOURCE_ADDR_REG WEU_REG_ADDR(0x38)
#define WEU_DMA_SOURCE_LENGTH_REG WEU_REG_ADDR(0x3c)
#define WEU_DMA_DESTINATION_ADDR_REG WEU_REG_ADDR(0x40)
#define WEU_INTERRUPT_MASK_REG WEU_REG_ADDR(0x44)
#define WEU_INTERRUPT_RESET_SELECT_REG WEU_REG_ADDR(0x48)
#define WEU_DIN0_REG WEU_REG_ADDR(0x4c)
#define WEU_DIN1_REG WEU_REG_ADDR(0x50)
#define WEU_DIN2_REG WEU_REG_ADDR(0x54)
#define WEU_DIN3_REG WEU_REG_ADDR(0x58)
#define WEU_WEU_KEY0 WEU_REG_ADDR(0x5c)
#define WEU_WEU_KEY1 WEU_REG_ADDR(0x60)
#define WEU_WEU_KEY2 WEU_REG_ADDR(0x64)
#define WEU_WEU_KEY3 WEU_REG_ADDR(0x68)
#define WEU_WEU_KEY4 WEU_REG_ADDR(0x6c)
#define WEU_WEU_KEY5 WEU_REG_ADDR(0x70)
#define WEU_WEU_KEY6 WEU_REG_ADDR(0x74)
#define WEU_WEU_KEY7 WEU_REG_ADDR(0x78)
#define WEU_SW_WR_MASK_OFF WEU_REG_ADDR(0x7c)
#define WEU_SW_WR_MASK_ON WEU_REG_ADDR(0x80)
#define WEU_WEU_MODE WEU_REG_ADDR(0x84)
#define WEU_TKIP_CONTROL_REG WEU_REG_ADDR(0x100)
#define WEU_MIC_KEY_Lo WEU_REG_ADDR(0x104)
#define WEU_MIC_KEY_Hi WEU_REG_ADDR(0x108)  //  Need to check with Richard ??
#define WEU_PHASE1_RESULT_0_REG WEU_REG_ADDR(0x10c)
#define WEU_PHASE1_RESULT_1_REG WEU_REG_ADDR(0x110)
#define WEU_PHASE1_RESULT_2_REG WEU_REG_ADDR(0x114)
#define WEU_PHASE2_RESULT_0_REG WEU_REG_ADDR(0x118)
#define WEU_PHASE2_RESULT_1_REG WEU_REG_ADDR(0x11c)
#define WEU_PHASE2_RESULT_2_REG WEU_REG_ADDR(0x120)
#define WEU_PHASE2_RESULT_3_REG WEU_REG_ADDR(0x124)
#define WEU_DECAP_TKIP_ICV_ERR_CNT_REG WEU_REG_ADDR(0x128)
#define WEU_DECAP_ICV_ERR_CNT_REG WEU_REG_ADDR(0x12c)
#define WEU_DECAP_TKIP_MIC_ERR_CNT_REG WEU_REG_ADDR(0x130)
#define WEU_TKIP_STATUS_REG WEU_REG_ADDR(0x134)
#define WEU_INTERRUPT_STATUS_REG WEU_REG_ADDR(0x138)
#define WEU_AES_DOUT0 WEU_REG_ADDR(0x13c)
#define WEU_AES_DOUT1 WEU_REG_ADDR(0x140)
#define WEU_AES_DOUT2 WEU_REG_ADDR(0x144)
#define WEU_AES_DOUT3 WEU_REG_ADDR(0x148)
#define WEU_DBG_FSM WEU_REG_ADDR(0x14c)
#define WEU_SW_RST WEU_REG_ADDR(0x150)

#define WEU_ENC_DEC_BUSY 1
#define WEU_TKIP_MIC_ERR 2
#define WEU_ICV_ERR 4
#define WEU_TKIP_ICV_ERR 8
#define WEU_CURRENT_FRAME_OK 0x10 




#endif
