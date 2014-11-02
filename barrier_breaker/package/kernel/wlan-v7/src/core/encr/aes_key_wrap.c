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
#include "ccmp.h"

static void uint64cpy(WRAPUINT64 *to, WRAPUINT64 *from)
{
    int sz;
    UINT8 *dst;
    UINT8 *src;
    
    src = (UINT8 *)from;
    dst = (UINT8 *)to;
    
    sz = 8;
    while (sz--)
    {
        *dst++ = *src++;
    }
}

static void uint64xor(WRAPUINT64 *res, WRAPUINT64 *n, UINT8 k)
{
    UINT8 *pn;
    UINT8 *pres;

    pn = (UINT8 *)n + 7;
    pres = (UINT8 *)res + 7;
    *pres = *pn ^ k;
}

static void uint64concat(UINT8 *res, WRAPUINT64 *n1, WRAPUINT64 *n2)
{
    int sz;
    UINT8 *src;
    UINT8 *dst;

    sz = 8;
    src = (UINT8 *)n1;
    dst = res;
    while (sz--)
    {
        *res++ = *src++;
    }

    sz = 8;
    src = (UINT8 *)n2;
    while (sz--)
    {
        *res++ = *src++;
    }
}

#define MAX_LEN_AES_DATA 128    //64
void AES_Wrap(UINT8 *pPlData, WRAPUINT64 *pCipTxt, WRAPUINT64 *pKEK, UINT32 len)
{
	int i, j, t;
	WRAPUINT64 R[1 + MAX_LEN_AES_DATA/8], A;
	UINT8 B[16], tmp[16];
	UINT32 EK[44], DK[44], n;
    WRAPUINT64 DEFAULT_IV = {0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6};

    if (len > MAX_LEN_AES_DATA)
        return;

	n = len/8;//no need to worry about non-integer multiple of 8

	uint64cpy(&A, &DEFAULT_IV);

    for (i=0; i<n; i++) 
    {
        uint64cpy(&R[i+1], (WRAPUINT64 *)&pPlData[i*8]);
    }

	AES_SetKeyWrap((UINT32 *)pKEK, EK, DK);  

    for (j = 0; j <= 5; j++)
    {
        for (i = 1; i <= n; i++)
        {
            uint64concat(tmp, &A, &R[i]);
			AES_Encrypt((UINT32 *)tmp, (UINT32 *)B, EK);
            uint64cpy(&A, (WRAPUINT64 *)&B[0]);
                        t = (n * j) + i;
            uint64xor(&A, &A, t);
            uint64cpy(&R[i], (WRAPUINT64 *)&B[8]);
            
        }
    }

    uint64cpy(&pCipTxt[0], &A);
    for (i = 1; i <= n; i++)
    {
        uint64cpy(&pCipTxt[i], &R[i]);
    }
}

void AES_UnWrap(WRAPUINT64 *pPlData, WRAPUINT64 *pCipTxt, WRAPUINT64 *pKEK, UINT32 len)
{
	int i, j, t;
	WRAPUINT64 R[1 + MAX_LEN_AES_DATA/8], A;
	UINT8 B[16], tmp[16];
	UINT32 EK[44], DK[44], n;

    if (len > MAX_LEN_AES_DATA)
        return;

	AES_SetKeyWrap((UINT32 *)pKEK, EK, DK);
	
    n = len/8 - 1;//no need to worry about non-integer multiple of 8
	uint64cpy(&A, &pCipTxt[0]);

    for (i=0; i<n; i++) 
    {
        uint64cpy(&R[i+1], (WRAPUINT64 *)&pCipTxt[i+1]);
    }

	for (j = 5; j >= 0; j--)
	{
        for (i = n; i >= 1; i--)
        {
			t = (n*j) + i;
			uint64xor(&A, &A, t);
			uint64concat(tmp, &A, &R[i]);
			AES_DecryptWrap((UINT32 *)tmp, (UINT32 *)B, EK, DK);
            
			uint64cpy(&A, (WRAPUINT64 *)&B[0]);//  A = MSB(64, B)
			uint64cpy(&R[i], (WRAPUINT64 *)&B[8]);//  R[i] = LSB(64, B)
			
		}
	}
	uint64cpy(&pPlData[0], &A);
	for (i = 1; i <= n; i++)
           uint64cpy(&pPlData[i], &R[i]);
}
			   
