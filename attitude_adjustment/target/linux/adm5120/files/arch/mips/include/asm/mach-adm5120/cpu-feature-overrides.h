/*
 *  ADM5120 specific CPU feature overrides
 *
 *  Copyright (C) 2007-2008 Gabor Juhos <juhosg@openwrt.org>
 *
 *  This file was derived from: include/asm-mips/cpu-features.h
 *	Copyright (C) 2003, 2004 Ralf Baechle
 *	Copyright (C) 2004 Maciej W. Rozycki
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 *
 */
#ifndef __ASM_MACH_ADM5120_CPU_FEATURE_OVERRIDES_H
#define __ASM_MACH_ADM5120_CPU_FEATURE_OVERRIDES_H

/*
 * The ADM5120 SOC has a built-in MIPS 4Kc core.
 */
#define cpu_has_tlb			1
#define cpu_has_4kex			1
#define cpu_has_3k_cache		0
#define cpu_has_4k_cache		1
#define cpu_has_tx39_cache		0
#define cpu_has_sb1_cache		0
#define cpu_has_fpu			0
#define cpu_has_32fpr			0
#define cpu_has_counter			1
#define cpu_has_watch			1
#define cpu_has_divec			1
/* #define cpu_has_vce			? */
/* #define cpu_has_cache_cdex_p		? */
/* #define cpu_has_cache_cdex_s		? */
#define cpu_has_prefetch		1
/* #define cpu_has_mcheck		? */
#define cpu_has_ejtag			1
#define cpu_has_llsc			1

#define cpu_has_mips16			0
#define cpu_has_mdmx			0
#define cpu_has_mips3d			0
#define cpu_has_smartmips		0

/* #define cpu_has_vtag_icache		? */
/* #define cpu_has_dc_aliases		? */
/* #define cpu_has_ic_fills_f_dc	? */
/* #define cpu_has_pindexed_dcache	? */

/* #define cpu_icache_snoops_remote_store	? */

#define cpu_has_mips32r1		1
#define cpu_has_mips32r2		0
#define cpu_has_mips64r1		0
#define cpu_has_mips64r2		0

#define cpu_has_dsp			0
#define cpu_has_mipsmt			0

/* #define cpu_has_nofpuex		? */
#define cpu_has_64bits			0
#define cpu_has_64bit_zero_reg		0
#define cpu_has_64bit_gp_regs		0
#define cpu_has_64bit_addresses		0

/* #define cpu_has_inclusive_pcaches	? */

#define cpu_dcache_line_size()		16
#define cpu_icache_line_size()		16

#endif /* __ASM_MACH_ADM5120_CPU_FEATURE_OVERRIDES_H */
