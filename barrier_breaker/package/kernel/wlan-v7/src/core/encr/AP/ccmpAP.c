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



#include "wltypes.h"
#include "IEEE_types.h"
#include "osif.h"

#include "mib.h"
#include "wl_mib.h"
#include "wl_hal.h"
#include "qos.h"
#include "wlmac.h"
#include "ds.h"
#include "keyMgmtCommon.h"
#include "keyMgmt.h"
#include "tkip.h"
#include "timer.h"
#include "StaDb.h"
#include "macmgmtap.h"


#include "macMgmtMlme.h"
#include "Fragment.h"
#include "ccmp.h"
#include "wl_macros.h"

#define SHORT_SWAP(X) ((X <<8 ) | (X >> 8))

#define WORD_SWAP(X) (((X)&0xff)<<24)+      \
	(((X)&0xff00)<<8)+      \
	(((X)&0xff0000)>>8)+    \
	(((X)&0xff000000)>>24)


UINT8  pow_tab[256];
UINT8  log_tab[256];
UINT32  rco_tab[ 10];
UINT8  sbx_tab[256];
UINT8  isb_tab[256];
UINT32  fl_tab[4][256];
UINT32  il_tab[4][256];
UINT32  ft_tab[4][256];
UINT32  it_tab[4][256];
UINT32  tab_gen = 0;

UINT32 reg_buf[4];

//NOte- Remove the rol and put ROT32L()
void gen_tabs(void)
{   UINT32  i, t;
UINT8  p, q;

/* log and power tables for GF(2**8) finite field with  */
/* 0x11b as modular polynomial - the simplest prmitive  */
/* root is 0x11, used here to generate the tables       */

for (i = 0,p = 1; i < 256; ++i)
{
	pow_tab[i] = (UINT8)p; log_tab[p] = (UINT8)i;

	p = p ^ (p << 1) ^ (p & 0x80 ? 0x01b : 0);
}

log_tab[1] = 0; p = 1;

for (i = 0; i < 10; ++i)
{
	rco_tab[i] = p; 

	p = (p << 1) ^ (p & 0x80 ? 0x1b : 0);
}

/* note that the affine byte transformation matrix in   */
/* rijndael specification is in big endian format with  */
/* bit 0 as the most significant bit. In the remainder  */
/* of the specification the bits are numbered from the  */
/* least significant end of a byte.                     */

for (i = 0; i < 256; ++i)
{
	p = (i ? pow_tab[255 - log_tab[i]] : 0); q = p; 
	q = (q >> 7) | (q << 1); p ^= q; 
	q = (q >> 7) | (q << 1); p ^= q; 
	q = (q >> 7) | (q << 1); p ^= q; 
	q = (q >> 7) | (q << 1); p ^= q ^ 0x63; 
	sbx_tab[i] = (UINT8)p; isb_tab[p] = (UINT8)i;
}

for (i = 0; i < 256; ++i)
{
	p = sbx_tab[i]; 

	t = p; fl_tab[0][i] = t;
	fl_tab[1][i] = rotl(t,  8);
	fl_tab[2][i] = rotl(t, 16);
	fl_tab[3][i] = rotl(t, 24);

	t = ((UINT32)ff_mult(2, p)) |
		((UINT32)p <<  8) |
		((UINT32)p << 16) |
		((UINT32)ff_mult(3, p) << 24);

	ft_tab[0][i] = t;
	ft_tab[1][i] = rotl(t,  8);
	ft_tab[2][i] = rotl(t, 16);
	ft_tab[3][i] = rotl(t, 24);

	p = isb_tab[i]; 

	t = p; il_tab[0][i] = t; 
	il_tab[1][i] = rotl(t,  8); 
	il_tab[2][i] = rotl(t, 16); 
	il_tab[3][i] = rotl(t, 24);

	t = ((UINT32)ff_mult(14, p)) |
		((UINT32)ff_mult( 9, p) <<  8) |
		((UINT32)ff_mult(13, p) << 16) |
		((UINT32)ff_mult(11, p) << 24);

	it_tab[0][i] = t; 
	it_tab[1][i] = rotl(t,  8); 
	it_tab[2][i] = rotl(t, 16); 
	it_tab[3][i] = rotl(t, 24); 
}
tab_gen = 1;
}

/*Initialize the AES tables*/
void CCMP_Init(void)
{
	gen_tabs();
}

void AES_SetKey(const UINT32 in_key[], UINT32 out_key[])
{   
	UINT32  i, t;

	if (!tab_gen)
		gen_tabs();

	for (i=0;i<4;i++)
		out_key[i] = bswap(in_key[i]);
	t = out_key[4-1];

	for (i = 0; i < 10; ++i)
		loop4(i, out_key);

}        

void AES_SetKeyWrap(const UINT32 in_key[], UINT32 enc_key[], UINT32 dec_key[])
{   
	UINT32  i, t, u, v, w;

	if (!tab_gen)
		gen_tabs();

	// k_len = (128 + 31) / 32;

	for (i=0;i<4;i++)
		enc_key[i] = bswap(in_key[i]);
	t = enc_key[4-1];

	for (i = 0; i < 10; ++i)
		loop4(i, enc_key);

	dec_key[0] = enc_key[0]; dec_key[1] = enc_key[1];
	dec_key[2] = enc_key[2]; dec_key[3] = enc_key[3];

	for (i = 4; i < 4 * 4 + 24; ++i)
	{
		imix_col(dec_key[i], enc_key[i]);
	}

}

void AES_Encrypt(const UINT32 in_blk[4], UINT32 out_blk[4], UINT32 keys[])
{   
	UINT32  b0[4], b1[4], *kp;

	b0[0] = bswap(in_blk[0]) ^ keys[0];
	b0[1] = bswap(in_blk[1]) ^ keys[1];
	b0[2] = bswap(in_blk[2]) ^ keys[2];
	b0[3] = bswap(in_blk[3]) ^ keys[3];

	kp = keys + 4;

	f_nround(b1, b0, kp); f_nround(b0, b1, kp);
	f_nround(b1, b0, kp); f_nround(b0, b1, kp);
	f_nround(b1, b0, kp); f_nround(b0, b1, kp);
	f_nround(b1, b0, kp); f_nround(b0, b1, kp);
	f_nround(b1, b0, kp); f_lround(b0, b1, kp);

	out_blk[0] = bswap(b0[0]);
	out_blk[1] = bswap(b0[1]);
	out_blk[2] = bswap(b0[2]);
	out_blk[3] = bswap(b0[3]);
}

void AES_DecryptWrap(const UINT32 in_blk[4], UINT32 out_blk[4],
					 UINT32 enc_keys[], UINT32 dec_keys[])
{   
	UINT32  b0[4], b1[4], *kp, k_len;

	k_len = 4;

	b0[0] = bswap(in_blk[0]) ^ enc_keys[4 * k_len + 24];
	b0[1] = bswap(in_blk[1]) ^ enc_keys[4 * k_len + 25];
	b0[2] = bswap(in_blk[2]) ^ enc_keys[4 * k_len + 26];
	b0[3] = bswap(in_blk[3]) ^ enc_keys[4 * k_len + 27];

	kp = dec_keys + 4 * (k_len + 5);

	i_nround(b1, b0, kp); i_nround(b0, b1, kp);
	i_nround(b1, b0, kp); i_nround(b0, b1, kp);
	i_nround(b1, b0, kp); i_nround(b0, b1, kp);
	i_nround(b1, b0, kp); i_nround(b0, b1, kp);
	i_nround(b1, b0, kp);
	i_lround(b0, b1, kp);

	out_blk[0] = bswap(b0[0]);
	out_blk[1] = bswap(b0[1]);
	out_blk[2] = bswap(b0[2]);
	out_blk[3] = bswap(b0[3]);
}

inline void MakeMICHdr1(UINT8 *pMICHdr, IEEEtypes_GenHdr_t *pHdr,UINT8 Offset)
{
	pMICHdr[0] = 0;
	pMICHdr[1] = HLEN+Offset;

	*(UINT16 *)&pMICHdr[2] = *((UINT16*)&pHdr->FrmCtl) & 0xC78F;
	MACADDR_CPY(&pMICHdr[4], pHdr->Addr1);
	MACADDR_CPY(&pMICHdr[10], pHdr->Addr2);
}

inline void MakeMICHdr2(UINT8 *pMICHdr, IEEEtypes_GenHdr_t *pHdr,UINT16 Priority)
{
	UINT32 i;

	MACADDR_CPY(&pMICHdr[0], pHdr->Addr3);
	pMICHdr[6] = (UINT8)pHdr->SeqCtl & 0x0F; // WPA2 PATCH 0x0F;
	pMICHdr[7] = 0;//(pHdr->SeqCtl >> 8) & 0x00;

	*(UINT32 *)(&pMICHdr[8]) = 0;
	*(UINT32 *)(&pMICHdr[12]) = 0;
	i=8;
	if (pHdr->FrmCtl.ToDs && pHdr->FrmCtl.FromDs)
	{
		MACADDR_CPY(&pMICHdr[i], pHdr->Addr4);
		i += 6;
	}

	if (Priority != 0)
	{
		*(UINT16 *)(&pMICHdr[i]) = Priority&0x000f;
		//i += 2;
	}
}

inline void MakeCCMCounterNonce(UINT8 *pCCMNonce, IEEEtypes_Frame_t *pRxPckt,UINT16 Priority)
{
	pCCMNonce[0] = 0x01;
	pCCMNonce[1] = (UINT8)Priority;
	MACADDR_CPY(&pCCMNonce[2], &pRxPckt->Hdr.Addr2);


	*(UINT32 *)(&pCCMNonce[8]) = WORD_SWAP(*((UINT32 *)&pRxPckt->Body[4]));
	*(UINT16 *)(&pCCMNonce[12]) = SHORT_SWAP(*((UINT16 *)&pRxPckt->Body[0]));
	pCCMNonce[14]=pCCMNonce[15]=0;

}

inline void MakeMICIV(UINT8 *pMICIV, IEEEtypes_Frame_t *pRxPkt,
					  UINT16 payload_size,UINT16 Priority)
{
	pMICIV[0] = 0x59;
	pMICIV[1] = (UINT8)(Priority&0x000f);
	MACADDR_CPY(&pMICIV[2], &pRxPkt->Hdr.Addr2);
	//*(UINT16 *)(&pMICIV[8]) = SHORT_SWAP((UINT16)pPcktNo[1]);
	//can use the H/W engine over here
	*(UINT32 *)(&pMICIV[8]) = WORD_SWAP(*((UINT32 *)&pRxPkt->Body[4]));
	*(UINT16 *)(&pMICIV[12]) = SHORT_SWAP(*((UINT16 *)&pRxPkt->Body[0]));

	//Can use the HW LONG_SWAP function over here.
	*(UINT16 *)(&pMICIV[14]) = SHORT_SWAP(payload_size);
}

void pad_data(UINT8 *pData2Pad, UINT32 required_padding)
{
	UINT32 i;

	for (i=0;i<required_padding;i++)
		pData2Pad[i] = 0;
}

void DoFinalMICStep(UINT8 *pMIC, UINT8 *pCCMCtrNonce, UINT32 *pKey)
{
	block_u b;
	UINT32 i;

	//Make the CCMP MIC
	*(UINT16 *)(&pCCMCtrNonce[14]) = 0;//Make ctr val 0
	AES_Encrypt((UINT32 *)pCCMCtrNonce, b.x, pKey);
	for (i=0;i<BLK_SIZE;i++)
		pMIC[i] ^= b.b[i];      
}

void GenerateEncrData(UINT8 *pCCMCtrNonce, UINT8 *pSrcTxt, UINT8 *pDstTxt,
					  UINT16 data_len, UINT32 *pKey)
{
	UINT16 counter;
	block_u b;
	UINT32 no_of_blcks, remainder, i, j, blck_no, xor_cnt;

	no_of_blcks = data_len/BLK_SIZE;
	remainder = data_len % BLK_SIZE;
	if (remainder)
	{
		no_of_blcks++;
	}
	counter = 1;
	j = 0;

	for (blck_no=0;blck_no<no_of_blcks;blck_no++)
	{
		*(UINT16 *)(&pCCMCtrNonce[14]) = SHORT_SWAP(counter);
		AES_Encrypt((UINT32 *)pCCMCtrNonce, b.x, pKey);
		if (j+BLK_SIZE < data_len)
		{
			xor_cnt = BLK_SIZE;
		}
		else
		{
			xor_cnt = remainder;
		}
		for (i=0;i<xor_cnt;i++)
		{
			pDstTxt[j] = b.b[i] ^ pSrcTxt[j];
			j++;
		}
		counter++;
	}
}

//Plain text and length of the data wil be sent as 1 unit
void GenerateMIC(UINT8 *pMICIV, UINT8 *pMICHdr1, UINT8 *pMICHdr2,
				 UINT8 *pCCMCtrNonce, UINT8 *pPlainTxt, UINT8 *pMIC,
				 UINT32 data_len, UINT32 *pKey)
{
	block_u b;
	UINT32 blck_no, no_of_blcks, remainder, i, j;

	//Calculate Padding
	no_of_blcks = data_len/16;
	remainder = data_len % BLK_SIZE;
	if (remainder)
	{//need to do padding
		pad_data(&pPlainTxt[data_len], BLK_SIZE - remainder);
		no_of_blcks++;
	}

	AES_Encrypt((UINT32 *)pMICIV, b.x, pKey);
	for (i=0;i<BLK_SIZE;i++)
		b.b[i] ^= pMICHdr1[i];
	AES_Encrypt(b.x, b.x, pKey);
	for (i=0;i<BLK_SIZE;i++)
		b.b[i] ^= pMICHdr2[i];
	AES_Encrypt(b.x, b.x, pKey);

	j= 0;
	for (blck_no=0;blck_no<no_of_blcks;blck_no++)
	{
		for (i=0;i<BLK_SIZE;i++)
			b.b[i] ^= pPlainTxt[j++];
		AES_Encrypt(b.x, b.x, pKey);
	}

	//Improvement- make b point to pMIC. So no need to do Memcpy later on
	if (0)
	{
		DoFinalMICStep(pMIC, pCCMCtrNonce, pKey);
	}
	else
	{
		DoFinalMICStep(b.b, pCCMCtrNonce, pKey);
		memcpy(pMIC, b.b, 16);//Copy 16 bytes of MIC
	}
}

void GenerateMIC2(UINT8 *pMICIV, UINT8 *pMICHdr1, UINT8 *pMICHdr2,
				  UINT8 *pCCMCtrNonce, UINT8 *pPlainTxt, UINT8 *pMIC,
				  UINT32 data_len, UINT32 *pKey)
{
	block_u *b;
	UINT32 blck_no, no_of_blcks, remainder, i, j;

	b = (block_u *)pMIC;
	//Calculate Padding
	no_of_blcks = data_len/16;
	remainder = data_len % BLK_SIZE;
	if (remainder)
	{//need to do padding
		pad_data(&pPlainTxt[data_len], BLK_SIZE - remainder);
		no_of_blcks++;
	}

	AES_Encrypt((UINT32 *)pMICIV, b->x, pKey);
	for (i=0;i<BLK_SIZE;i++)
		b->b[i] ^= pMICHdr1[i];
	AES_Encrypt(b->x, b->x, pKey);
	for (i=0;i<BLK_SIZE;i++)
		b->b[i] ^= pMICHdr2[i];
	AES_Encrypt(b->x, b->x, pKey);

	j= 0;
	for (blck_no=0;blck_no<no_of_blcks;blck_no++)
	{
		for (i=0;i<BLK_SIZE;i++)
			b->b[i] ^= pPlainTxt[j++];
		AES_Encrypt(b->x, b->x, pKey);
	}

	DoFinalMICStep(pMIC, pCCMCtrNonce, pKey);

}

inline void InsertCCMPHdr(UINT8 *pCCMPHdr,  UINT8 keyID, UINT16 IV16, UINT32 IV32)
{
	*((UINT16 *)pCCMPHdr) = IV16;
	pCCMPHdr[2] = N_RESERVED;
	pCCMPHdr[3] = ExtIV | (keyID << 6);
	*((UINT32 *)&pCCMPHdr[4]) = IV32;
}

inline void MakeCCMPCfgRegs(IEEEtypes_Frame_t *pFrm, UINT8 *pMICIV, UINT8 *pMICHdr1,
							UINT8 *pMICHdr2, UINT8 *pCCMCtrNonce, UINT16 length,UINT32 Priority)
{
	MakeMICIV(pMICIV, pFrm, length,Priority);
	MakeMICHdr1(pMICHdr1, &pFrm->Hdr,Priority);
	MakeMICHdr2(pMICHdr2, &pFrm->Hdr,Priority);
	MakeCCMCounterNonce(pCCMCtrNonce, pFrm,Priority);
}
