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


#ifndef _ENCRYPTAPI_H_
#define _ENCRYPTAPI_H_

#include "pbkdf2.h"
extern void hmac_sha1(const u_int8_t *text, size_t text_len, const u_int8_t *key,
    size_t key_len, u_int8_t *digest);
extern void Mrvl_PRF(unsigned char *key, int key_len, unsigned char *prefix,
                          int prefix_len, unsigned char *data, int data_len,
                          unsigned char *output, int len);

#define Mrvl_hmac_sha1 hmac_sha1
#define PKCS5_PBKDF2(pwd, ssid, slen, output) pkcs5_pbkdf2(pwd, strlen(pwd), ssid, slen, output, SHA1_DIGEST_LENGTH*2, 4096)

#endif

