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

#ifndef _WAPI_H_
#define _WAPI_H_
#define WAPI_PN_DW_LEN  4   //PN DWord length

#define INSERT_WAPIHDR(pWapiHdr, pPn)   \
    *pWapiHdr = *pPn;                   \
    *(pWapiHdr+1) = *(pPn + 1);         \
    *(pWapiHdr+2) = *(pPn + 2);         \
    *(pWapiHdr+3) = *(pPn + 3)

#define INCREASE_WAPI_PN(PN, inc)       \
{                                       \
    int        i;                       \
    UINT32     inc1=inc;                \
    UINT32     temp;                    \
    for(i = 0; i < WAPI_PN_DW_LEN; i++) \
    {                                   \
        temp = *(PN+i);                 \
        *(PN+i) += inc;                 \
        if (*(PN+i) < temp)             \
        {                               \
            inc1 = 1;                   \
        }                               \
        else                            \
        {                               \
            break;                      \
        }                               \
    }                                   \
}
#endif  //_WAPI_H_