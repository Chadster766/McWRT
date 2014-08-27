/*
 *  Copyright (C) 2011 John Crispin <blogic@openwrt.org>
 *  Copyright (C) 2011 Andrej Vlašić <andrej.vlasic0@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/ath5k_platform.h>
#include <linux/ath9k_platform.h>
#include <linux/pci.h>

#include "dev-wifi-athxk.h"

extern int (*ltqpci_plat_dev_init)(struct pci_dev *dev);
struct ath5k_platform_data ath5k_pdata;
struct ath9k_platform_data ath9k_pdata = {
	.led_pin = -1,
	.endian_check = true,
};

static int
ath5k_pci_plat_dev_init(struct pci_dev *dev)
{
	dev->dev.platform_data = &ath5k_pdata;
	return 0;
}

static int
ath9k_pci_plat_dev_init(struct pci_dev *dev)
{
	dev->dev.platform_data = &ath9k_pdata;
	return 0;
}

void __init
ltq_register_ath5k(u16 *eeprom_data, u8 *macaddr)
{
	ath5k_pdata.eeprom_data = eeprom_data;
	ath5k_pdata.macaddr = macaddr;
	ltqpci_plat_dev_init = ath5k_pci_plat_dev_init;
}

void __init
ltq_register_ath9k(u16 *eeprom_data, u8 *macaddr)
{
	memcpy(ath9k_pdata.eeprom_data, eeprom_data, sizeof(ath9k_pdata.eeprom_data));
	ath9k_pdata.macaddr = macaddr;
	ltqpci_plat_dev_init = ath9k_pci_plat_dev_init;
}
