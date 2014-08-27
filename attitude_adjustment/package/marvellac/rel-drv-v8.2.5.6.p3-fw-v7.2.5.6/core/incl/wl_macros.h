/*
*                Copyright 2002-2014, Marvell Semiconductor, Inc.
* This code contains confidential information of Marvell Semiconductor, Inc.
* No rights are granted herein under any patent, mask work right or copyright
* of Marvell or any third party.
* Marvell reserves the right at its sole discretion to request that this code
* be immediately returned to Marvell. This code is provided "as is".
* Marvell makes no warranties, express, implied or otherwise, regarding its
* accuracy, completeness or performance.
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
