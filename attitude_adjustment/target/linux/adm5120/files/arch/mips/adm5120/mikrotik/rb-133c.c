/*
 *  Mikrotik RouterBOARD 133C support
 *
 *  Copyright (C) 2007-2008 Gabor Juhos <juhosg@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 *
 */

#include "rb-1xx.h"

static struct gpio_led rb133c_gpio_leds[] __initdata = {
	GPIO_LED_STD(ADM5120_GPIO_PIN6, "power",	NULL),
	GPIO_LED_STD(ADM5120_GPIO_PIN5, "user",		NULL),
	GPIO_LED_INV(ADM5120_GPIO_P2L1,	"lan1_speed",	NULL),
	GPIO_LED_INV(ADM5120_GPIO_P2L0,	"lan1_lnkact",	NULL),
};

static u8 rb133c_vlans[6] __initdata = {
	0x7F, 0x00, 0x00, 0x00, 0x00, 0x00
};

static void __init rb133c_setup(void)
{
	rb1xx_generic_setup();
	rb1xx_add_device_nand();

	adm5120_add_device_switch(1, rb133c_vlans);
	adm5120_add_device_gpio(0);
	adm5120_add_device_gpio_leds(ARRAY_SIZE(rb133c_gpio_leds),
					rb133c_gpio_leds);
}

MIPS_MACHINE(MACH_ADM5120_RB_133C, "133C", "Mikrotik RouterBOARD 133C",
	     rb133c_setup);
