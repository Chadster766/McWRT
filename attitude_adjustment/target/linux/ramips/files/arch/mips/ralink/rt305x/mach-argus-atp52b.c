/*
 *  Argus ATP-52B router support
 *  http://www.argus-co.com/english/productsview.php?id=70&cid=81
 *
 *  Copyright (C) 2011 Roman Yeryomin <roman@advem.lv>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/init.h>
#include <linux/platform_device.h>

#include <asm/mach-ralink/machine.h>
#include <asm/mach-ralink/dev-gpio-buttons.h>
#include <asm/mach-ralink/dev-gpio-leds.h>
#include <asm/mach-ralink/rt305x.h>
#include <asm/mach-ralink/rt305x_regs.h>

#include "devices.h"

#define ARGUS_ATP52B_GPIO_LED_RUN		9
#define ARGUS_ATP52B_GPIO_LED_NET		13
#define ARGUS_ATP52B_GPIO_BUTTON_WPS		0
#define ARGUS_ATP52B_GPIO_BUTTON_RESET		10
#define ARGUS_ATP52B_KEYS_POLL_INTERVAL		20
#define ARGUS_ATP52B_KEYS_DEBOUNCE_INTERVAL	(3 * ARGUS_ATP52B_KEYS_POLL_INTERVAL)

static struct gpio_led argus_atp52b_leds_gpio[] __initdata = {
	{
		.name       = "argus-atp52b:green:run",
		.gpio       = ARGUS_ATP52B_GPIO_LED_RUN,
		.active_low = 1,
	},
	{
		.name       = "argus-atp52b:amber:net",
		.gpio       = ARGUS_ATP52B_GPIO_LED_NET,
		.active_low = 1,
	}
};

static struct gpio_keys_button argus_atp52b_gpio_buttons[] __initdata = {
	{
		.desc       = "wps",
		.type       = EV_KEY,
		.code       = KEY_WPS_BUTTON,
		.debounce_interval = ARGUS_ATP52B_KEYS_DEBOUNCE_INTERVAL,
		.gpio       = ARGUS_ATP52B_GPIO_BUTTON_WPS,
		.active_low = 1,
	},
	{
		.desc       = "reset",
		.type       = EV_KEY,
		.code       = KEY_RESTART,
		.debounce_interval = ARGUS_ATP52B_KEYS_DEBOUNCE_INTERVAL,
		.gpio       = ARGUS_ATP52B_GPIO_BUTTON_RESET,
		.active_low = 1,
	}
};

static void __init argus_atp52b_init(void)
{
	rt305x_gpio_init(RT305X_GPIO_MODE_GPIO << RT305X_GPIO_MODE_UART0_SHIFT);

	rt305x_register_flash(0);

	ramips_register_gpio_leds(-1, ARRAY_SIZE(argus_atp52b_leds_gpio),
					argus_atp52b_leds_gpio);
	ramips_register_gpio_buttons(-1, ARGUS_ATP52B_KEYS_POLL_INTERVAL,
					ARRAY_SIZE(argus_atp52b_gpio_buttons),
					argus_atp52b_gpio_buttons);
	rt305x_esw_data.vlan_config = RT305X_ESW_VLAN_CONFIG_WLLLL;
	rt305x_register_ethernet();
	rt305x_register_wifi();
	rt305x_register_wdt();
	rt305x_register_usb();
}

MIPS_MACHINE(RAMIPS_MACH_ARGUS_ATP52B, "ARGUS_ATP52B", "Argus ATP-52B",
					argus_atp52b_init);
