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


#ifndef _MD5_H_
#define _MD5_H_

typedef struct{
    unsigned long state[4];/* state (ABCD) */
    unsigned long count[2];/* number of bits, modulo 2^64 (lsb first) */
    unsigned char buffer[64];/* input buffer */
}Mrvl_MD5_CTX; 

void wpa_MD5Init (Mrvl_MD5_CTX *context);
void wpa_MD5Update (Mrvl_MD5_CTX *context, UINT8 *input, UINT32 inputLen);
void wpa_MD5Final (unsigned char digest[16], Mrvl_MD5_CTX *context);
void Mrvl_hmac_md5(UINT8 *text, int text_len, UINT8 *key, int key_len,
              void * digest);
//void wpa_MD5Transform(UINT32 *, unsigned int *);

#endif
