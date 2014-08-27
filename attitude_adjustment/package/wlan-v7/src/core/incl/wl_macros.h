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

#if !defined (_WL_MACROS_H_)
#define _WL_MACROS_H_

/*!
 * \file  wl_macros.h
 * \brief Common macros are defined here
 *
 *  Must include "wltypes.h" before this file 
 */

#define MACRO_START          do {
#define MACRO_END            } while (0)

#define WL_REGS8(x)     (*(volatile unsigned char *)(x))
#define WL_REGS16(x)    (*(volatile unsigned short *)(x))
#define WL_REGS32(x)    (*(volatile unsigned long *)(x))

#define WL_READ_REGS8(reg,val)      ((val) = WL_REGS8(reg))
#define WL_READ_REGS16(reg,val)     ((val) = WL_REGS16(reg))
#define WL_READ_REGS32(reg,val)     ((val) = WL_REGS32(reg))
#define WL_READ_BYTE(reg,val)       ((val) = WL_REGS8(reg))
#define WL_READ_HWORD(reg,val)      ((val) = WL_REGS16(reg)) /*half word; */
/*16bits */
#define WL_READ_WORD(reg,val)       ((val) = WL_REGS32(reg)) /*32 bits */
#define WL_WRITE_REGS8(reg,val)     (WL_REGS8(reg) = val)
#define WL_WRITE_REGS16(reg,val)    (WL_REGS16(reg) = val)
#define WL_WRITE_REGS32(reg,val)    (WL_REGS32(reg) = val)
#define WL_WRITE_BYTE(reg,val)      (WL_REGS8(reg) = val)
#define WL_WRITE_HWORD(reg,val)     (WL_REGS16(reg) = val)  /*half word; */
/*16bits */
#define WL_WRITE_WORD(reg,val)      (WL_REGS32(reg) = val)  /*32 bits */
#define WL_REGS8_SETBITS(reg, val)  (WL_REGS32(reg) |= (UINT8)val)
#define WL_REGS16_SETBITS(reg, val) (WL_REGS32(reg) |= (UINT16)val)
#define WL_REGS32_SETBITS(reg, val) (WL_REGS32(reg) |= val)

#define WL_REGS8_CLRBITS(reg, val)  (WL_REGS32(reg) = \
                                     (UINT8)(WL_REGS32(reg)&~val))

#define WL_REGS16_CLRBITS(reg, val) (WL_REGS32(reg) = \
                                     (UINT16)(WL_REGS32(reg)&~val))

#define WL_REGS32_CLRBITS(reg, val) (WL_REGS32(reg) = \
                                     (WL_REGS32(reg)&~val))

/*!
 * Alignment macros
 */
#define ALIGN4BYTE(x)   (x = (( x+4-1)/4) * 4)
 
#define PHYMAC(mid) 	(((UINT32)mid) >> 8)
#define TOMACID(pmac)	(0)

#define MULTICAST(x)	(((x) & htonl(0xf0000000)) == htonl(0xe0000000))
#define LOCAL_MCAST(x)	(((x) & htonl(0xFFFFFF00)) == htonl(0xE0000000))

#endif /* _WL_MACROS_H_ */
