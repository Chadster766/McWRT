/*
 *  Atheros AR934X SoCs built-in NAND flash controller support
 *
 *  Copyright (C) 2011-2012 Gabor Juhos <juhosg@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/init.h>
#include <linux/irq.h>
#include <linux/dma-mapping.h>
#include <linux/etherdevice.h>
#include <linux/platform_device.h>
#include <linux/platform/ar934x_nfc.h>

#include <asm/mach-ath79/ath79.h>
#include <asm/mach-ath79/ar71xx_regs.h>

#include "dev-nfc.h"

static struct resource ath79_nfc_resources[2];
static u64 ar934x_nfc_dmamask = DMA_BIT_MASK(32);
static struct ar934x_nfc_platform_data ath79_nfc_data;

static struct platform_device ath79_nfc_device = {
	.name		= AR934X_NFC_DRIVER_NAME,
	.id		= -1,
	.resource	= ath79_nfc_resources,
	.num_resources	= ARRAY_SIZE(ath79_nfc_resources),
	.dev = {
		.dma_mask = &ar934x_nfc_dmamask,
		.coherent_dma_mask = DMA_BIT_MASK(32),
		.platform_data = &ath79_nfc_data,
	},
};

static void ar934x_nfc_hw_reset(bool active)
{
	if (active) {
		ath79_device_reset_set(AR934X_RESET_NANDF);
		udelay(100);

		ath79_device_reset_set(AR934X_RESET_ETH_SWITCH_ANALOG);
		udelay(250);
	} else {
		ath79_device_reset_clear(AR934X_RESET_ETH_SWITCH_ANALOG);
		udelay(250);

		ath79_device_reset_clear(AR934X_RESET_NANDF);
		udelay(100);
	}
}

static void ar934x_nfc_setup(void)
{
	ath79_nfc_resources[0].start = AR934X_NFC_BASE;
	ath79_nfc_resources[0].end = AR934X_NFC_BASE + AR934X_NFC_SIZE - 1;
	ath79_nfc_resources[0].flags = IORESOURCE_MEM;

	ath79_nfc_resources[1].start = ATH79_MISC_IRQ(21);
	ath79_nfc_resources[1].end = ATH79_MISC_IRQ(21);
	ath79_nfc_resources[1].flags = IORESOURCE_IRQ;

	ath79_nfc_data.hw_reset = ar934x_nfc_hw_reset;

	platform_device_register(&ath79_nfc_device);
}

void __init ath79_nfc_set_select_chip(void (*f)(int chip_no))
{
	ath79_nfc_data.select_chip = f;
}

void __init ath79_nfc_set_scan_fixup(int (*f)(struct mtd_info *mtd))
{
	ath79_nfc_data.scan_fixup = f;
}

void __init ath79_nfc_set_parts(struct mtd_partition *parts, int nr_parts)
{
	ath79_nfc_data.parts = parts;
	ath79_nfc_data.nr_parts = nr_parts;
}

void __init ath79_register_nfc(void)
{
	if (soc_is_ar934x())
		ar934x_nfc_setup();
	else
		BUG();
}
