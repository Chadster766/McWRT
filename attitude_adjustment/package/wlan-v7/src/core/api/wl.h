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


#ifndef WL_H
#define WL_H

typedef	s8	CHAR;
typedef	s16	SHORT;
typedef	s32	LONG;
typedef u8	UCHAR;
typedef u8 BYTE;
typedef u16	USHORT;
typedef	u32	ULONG;
typedef	u32 DWORD;
//typedef long long	LARGE_INTEGER;
typedef	u8	OS_BOOL, *POS_BOOL;

typedef	s8	OS_INT8;
typedef s16	OS_INT16;
typedef s32	OS_INT32;

typedef	u8	OS_UINT8;
typedef	u16	OS_UINT16;
typedef	u32	OS_UINT32;

typedef u64	OS_UINT64, *POS_UINT64;	

#ifndef	FALSE
#define	FALSE		0
#endif

#ifndef	TRUE
#define	TRUE		1
#endif

#endif /* WL_H */

