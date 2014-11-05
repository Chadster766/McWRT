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
#include "md5.h"

void Mrvl_hmac_md5(UINT8 *text_data, int text_len, UINT8 *key, int key_len,
             void * digest)
{
    Mrvl_MD5_CTX context;
    unsigned char k_ipad[65]; /* inner padding - key XORd with ipad */
    unsigned char k_opad[65]; /* outer padding - key XORd with opad */
    int i;
    unsigned char text[300];

    memcpy(text, text_data, text_len);
/* if key is longer than 64 bytes reset it to key=MD5(key) */

    if (key_len > 64)
    {
        Mrvl_MD5_CTX tctx;

        wpa_MD5Init(&tctx);
        wpa_MD5Update(&tctx, key, key_len);
        wpa_MD5Final(context.buffer,&tctx);

        key = context.buffer;
        key_len = 16;
    }


    /* the HMAC_MD5 transform looks like: */
    /* */
    /*  MD5(K XOR opad, MD5(K XOR ipad, text)) */
    /* */
    /* where K is an n byte key */
    /* ipad is the byte 0x36 repeated 64 times */
    /* opad is the byte 0x5c repeated 64 times */
    /* and text is the data being protected */


/* start out by storing key in pads */
    memset(k_ipad, 0, sizeof(k_ipad));
    memset(k_opad, 0, sizeof(k_opad));
    memcpy(k_ipad, key, key_len);
    memcpy(k_opad, key, key_len);

/* XOR key with ipad and opad values */
    for (i = 0; i < 64; i++)
    {
        k_ipad[i] ^= 0x36;
        k_opad[i] ^= 0x5c;
    }

/* perform inner MD5 */
    wpa_MD5Init(&context); /* init context for 1st pass */
    wpa_MD5Update(&context, k_ipad, 64);  /* start with inner pad */
    wpa_MD5Update(&context, text, text_len); /* then text of datagram */
    wpa_MD5Final(digest,&context); /* finish up 1st pass */

/* perform outer MD5 */
    wpa_MD5Init(&context); /* init context for 2nd pass */
    wpa_MD5Update(&context,k_opad, 64);     
/* start with outer pad */
    wpa_MD5Update(&context,digest, 16);     
/* then results of 1st hash */
    wpa_MD5Final(digest,&context); /* finish up 2nd pass */
}
