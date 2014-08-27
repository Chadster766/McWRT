/*
 * (C) Copyright 2003-2005
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

/*
 * This file contains the configuration parameters for the Danube reference board.
 */

#ifndef __CONFIG_H
#define __CONFIG_H

#define CONFIG_MIPS32		1	/* MIPS32 CPU compatible		*/
#define CONFIG_MIPS24KEC	1	/* MIPS 24KEc CPU core			*/
#define CONFIG_DANUBE		1	/* in a Danube/Twinpass Chip		*/

#define CONFIG_SYS_MIPS_MULTI_CPU	1	/* This is a multi cpu system */

#define CONFIG_USE_DDR_RAM

#define CONFIG_FLASH_CFI_DRIVER	1

#define CONFIG_SYS_INIT_RAM_LOCK_MIPS

#ifdef CONFIG_SYS_RAMBOOT
	//#warning CONFIG_SYS_RAMBOOT
	#define CONFIG_SKIP_LOWLEVEL_INIT
#else /* CONFIG_SYS_RAMBOOT */
	#define CONFIG_SYS_EBU_BOOT
	#define INFINEON_EBU_BOOTCFG	0x688C688C	/* CMULT = 8 for 150 MHz */
#endif /* CONFIG_SYS_RAMBOOT */

#if 1
#ifndef	CPU_CLOCK_RATE
#define CPU_CLOCK_RATE	(ifx_get_cpuclk())
#endif
#endif

#undef CONFIG_SYS_HUSH_PARSER		/* Use the HUSH parser		*/

/*
 * Include common defines/options for all Infineon boards
 */
#include "ifx-common.h"


#undef CONFIG_EXTRA_ENV_SETTINGS                                       
#define CONFIG_EXTRA_ENV_SETTINGS                                       \
        "ram_addr=0x80500000\0"                                         \
        "kernel_addr=0xb0020000\0"                                      \
        "flashargs=setenv bootargs rootfstype=squashfs,jffs2\0"         \
        "nfsargs=setenv bootargs root=/dev/nfs rw "                     \
                "nfsroot=${serverip}:${rootpath} \0"                    \
        "addip=setenv bootargs ${bootargs} "                            \
                "ip=${ipaddr}:${serverip}:${gatewayip}:${netmask}"      \
                ":${hostname}:${netdev}:off\0"                          \
        "addmisc=setenv bootargs ${bootargs} init=/etc/preinit "        \
                "console=ttyS1,115200 ethaddr=${ethaddr} "              \
                "${mtdparts}\0"                                         \
        "flash_flash=run flashargs addip addmisc;"                      \
                "bootm ${kernel_addr}\0"                                \
        "flash_nfs=run nfsargs addip addmisc;bootm ${kernel_addr}\0"    \
        "net_flash=run load_kernel flashargs addip addmisc;"            \
                "bootm ${ram_addr}\0"                                   \
        "net_nfs=run load_kernel nfsargs addip addmisc;"                \
                "bootm ${ram_addr}\0"                                   \
        "load_kernel=tftp ${ram_addr} "                                 \
                "${tftppath}openwrt-ifxmips-uImage\0"                   \
        "update_uboot=tftp 0x80500000 ${tftppath}u-boot-" CONFIG_ARCADYAN ".bin;era 0xb0000000 +${filesize};" \
                "cp.b 0x80500000 0xb0000000 ${filesize}\0" \
        "update_openwrt=tftp ${ram_addr} "                              \
                "${tftppath}" CONFIG_ARCADYAN "-squashfs.image;"            \
                "era ${kernel_addr} +${filesize};"                      \
                "cp.b ${ram_addr} ${kernel_addr} ${filesize}\0"

/*
 * Cache Configuration (cpu/chip specific, Danube)
 */
#define CONFIG_SYS_DCACHE_SIZE		16384
#define CONFIG_SYS_ICACHE_SIZE		16384
#define CONFIG_SYS_CACHELINE_SIZE	32
#define CONFIG_SYS_MIPS_CACHE_OPER_MODE	CONF_CM_CACHABLE_NO_WA

#define CONFIG_NET_MULTI

#define CONFIG_IFX_ETOP
//#define CLK_OUT2_25MHZ

#define CONFIG_MII
#undef CONFIG_CMD_MII

#define CONFIG_IFX_ASC

#ifdef CONFIG_USE_ASC0
#define CONFIG_SYS_IFX_ASC_BASE		0x1E100400
#else
#define CONFIG_SYS_IFX_ASC_BASE		0x1E100C00
#endif

#ifdef CONFIG_SYS_RAMBOOT
/* Configuration of EBU: */
/* starting address from 0xb0000000 */
/* make the flash available from RAM boot */
#	define CONFIG_EBU_ADDSEL0		0x10000031
#	define CONFIG_EBU_BUSCON0		0x0001D7FF
#	define CONFIG_EBU_ADDSEL1		0x14000001
#	define CONFIG_EBU_BUSCON1		0x4041D7FD
#endif

#define CONFIG_CMD_HTTPD		/* enable upgrade via HTTPD */

#define CONFIG_IPADDR		192.168.1.1
#define CONFIG_SERVERIP		192.168.1.101
#define CONFIG_GATEWAYIP	192.168.1.254
#define CONFIG_NETMASK		255.255.255.0
#define CONFIG_ROOTPATH		"/export"

#ifdef CONFIG_BOOTSTRAP
#define CONFIG_BOOTSTRAP_BASE			CONFIG_BOOTSTRAP_TEXT_BASE
#define CONFIG_BOOTSTRAP_BAUDRATE		CONFIG_BAUDRATE
#define CONFIG_SKIP_LOWLEVEL_INIT
#define CONFIG_BOOTSTRAP_LZMA
//#define CONFIG_BOOTSTRAP_SERIAL
#endif



#endif	/* __CONFIG_H */
