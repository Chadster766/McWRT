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


#ifndef _CCMP_H_
#define _CCMP_H_

#define ff_mult(a,b)    (a && b ? pow_tab[(log_tab[a] + log_tab[b]) % 255] : 0)
#define bswap(x)    (x)
#define HLEN 22
#define N_RESERVED 0
#define BLK_SIZE 16

/* Extract byte from a 32 bit quantity (little endian notation)     */ 
#define byte(x,n)   ((UINT8)((x) >> (8 * n)))

#define f_rn(bo, bi, n, k)                          \
    bo[n] =  ft_tab[0][byte(bi[n],0)] ^             \
             ft_tab[1][byte(bi[(n + 1) & 3],1)] ^   \
             ft_tab[2][byte(bi[(n + 2) & 3],2)] ^   \
             ft_tab[3][byte(bi[(n + 3) & 3],3)] ^ *(k + n)

#define i_rn(bo, bi, n, k)                          \
    bo[n] =  it_tab[0][byte(bi[n],0)] ^             \
             it_tab[1][byte(bi[(n + 3) & 3],1)] ^   \
             it_tab[2][byte(bi[(n + 2) & 3],2)] ^   \
             it_tab[3][byte(bi[(n + 1) & 3],3)] ^ *(k + n)

#define f_rl(bo, bi, n, k)                          \
    bo[n] =  fl_tab[0][byte(bi[n],0)] ^             \
             fl_tab[1][byte(bi[(n + 1) & 3],1)] ^   \
             fl_tab[2][byte(bi[(n + 2) & 3],2)] ^   \
             fl_tab[3][byte(bi[(n + 3) & 3],3)] ^ *(k + n)

#define i_rl(bo, bi, n, k)                          \
    bo[n] =  il_tab[0][byte(bi[n],0)] ^             \
             il_tab[1][byte(bi[(n + 3) & 3],1)] ^   \
             il_tab[2][byte(bi[(n + 2) & 3],2)] ^   \
             il_tab[3][byte(bi[(n + 1) & 3],3)] ^ *(k + n)

/* encrypt a block of text  */
#define f_nround(bo, bi, k) \
    f_rn(bo, bi, 0, k);     \
    f_rn(bo, bi, 1, k);     \
    f_rn(bo, bi, 2, k);     \
    f_rn(bo, bi, 3, k);     \
    k += 4

#define f_lround(bo, bi, k) \
    f_rl(bo, bi, 0, k);     \
    f_rl(bo, bi, 1, k);     \
    f_rl(bo, bi, 2, k);     \
    f_rl(bo, bi, 3, k);

/* decrypt a block of text  */
#define i_nround(bo, bi, k) \
    i_rn(bo, bi, 0, k);     \
    i_rn(bo, bi, 1, k);     \
    i_rn(bo, bi, 2, k);     \
    i_rn(bo, bi, 3, k);     \
    k -= 4

#define i_lround(bo, bi, k) \
    i_rl(bo, bi, 0, k);     \
    i_rl(bo, bi, 1, k);     \
    i_rl(bo, bi, 2, k);     \
    i_rl(bo, bi, 3, k)
    
#define ls_box(x)                \
    ( fl_tab[0][byte(x, 0)] ^    \
      fl_tab[1][byte(x, 1)] ^    \
      fl_tab[2][byte(x, 2)] ^    \
      fl_tab[3][byte(x, 3)] )

#define loop4(i, key)                             \
{   t = ls_box(rotr(t,  8)) ^ rco_tab[i];           \
    t ^= key[4 * i];     key[4 * i + 4] = t;    \
    t ^= key[4 * i + 1]; key[4 * i + 5] = t;    \
    t ^= key[4 * i + 2]; key[4 * i + 6] = t;    \
    t ^= key[4 * i + 3]; key[4 * i + 7] = t;    \
}
    
#define rotr(x,n)   (((x) >> ((int)(n))) | ((x) << (32 - (int)(n))))
#define rotl(x,n)   (((x) << ((int)(n))) | ((x) >> (32 - (int)(n))))

#define star_x(x) (((x) & 0x7f7f7f7f) << 1) ^ ((((x) & 0x80808080) >> 7) * 0x1b)

#define imix_col(y,x)       \
    u   = star_x(x);        \
    v   = star_x(u);        \
    w   = star_x(v);        \
    t   = w ^ (x);          \
   (y)  = u ^ v ^ w;        \
   (y) ^= rotr(u ^ t,  8) ^ \
          rotr(v ^ t, 16) ^ \
          rotr(t,24)

typedef union
{             /* AES cipher block */
    UINT32 x[BLK_SIZE/4]; /* access as 8-bit octets or 32-bit words */
    UINT8 b[BLK_SIZE];
}block_u;
    

void AES_Encrypt(const UINT32 in_blk[4], UINT32 out_blk[4], UINT32 keys[]);
void AES_DecryptWrap(const UINT32 in_blk[4], UINT32 out_blk[4],UINT32 enc_keys[], UINT32 dec_keys[]);
void AES_SetKey(const UINT32 in_key[], UINT32 out_key[]);
void AES_SetKeyWrap(const UINT32 in_key[], UINT32 enc_key[], UINT32 dec_key[]);
inline void MakeCCMCounterNonce(UINT8 *pCCMNonce, IEEEtypes_Frame_t *pRxPckt,UINT16);
inline void MakeMICIV(UINT8 *pMICIV, IEEEtypes_Frame_t *pRxPkt,
               UINT16 payload_size,UINT16 );
inline void MakeMICHdr1(UINT8 *pMICHdr, IEEEtypes_GenHdr_t *pHdr,UINT8);
inline void MakeMICHdr2(UINT8 *pMICHdr, IEEEtypes_GenHdr_t *pHdr,UINT16);
void GenerateEncrData(UINT8 *pCCMCtrNonce, UINT8 *pSrcTxt, UINT8 *pDstTxt,
                      UINT16 data_len, UINT32 *pKey);
void GenerateMIC(UINT8 *, UINT8 *, UINT8 *, UINT8 *, UINT8 *, UINT8 *,
                 UINT32, UINT32 *);
inline UINT32 DoCCMPDecrypt(IEEEtypes_Frame_t *pRxPckt, UINT8 *pDest,
                            UINT16 data_length, UINT32 *pKey,UINT16 Priority);

extern void InsertCCMPHdr(UINT8 *pCCMPHdr,  UINT8 keyID, UINT16 IV16, UINT32 IV32);
#endif
