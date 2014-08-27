/*
 *  Sitecom WL341v3 board support
 *
 *  Copyright (C) 2012 Marco Antonio Mauro <marcus90@gmail.com>
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

#define WL341V3_GPIO_LED_FIRST_AMBER	9
#define WL341V3_GPIO_LED_FIRST_BLUE	13
#define WL341V3_GPIO_LED_THIRD_AMBER	11
#define WL341V3_GPIO_LED_THIRD_BLUE	14
#define WL341V3_GPIO_LED_FOURTH_BLUE	10
#define WL341V3_GPIO_LED_FIFTH_AMBER	12
#define WL341V3_GPIO_LED_FIFTH_BLUE	8

#define WL341V3_GPIO_BUTTON_WPS		5	/* active low */
#define WL341V3_GPIO_BUTTON_RESET	7	/* active low */

#define WL341V3_KEYS_POLL_INTERVAL	20
#define WL341V3_KEYS_DEBOUNCE_INTERVAL	(3 * WL341V3_KEYS_POLL_INTERVAL)

static struct gpio_led wl341v3_leds_gpio[] __initdata = {
	{
		.name		= "wl341v3:amber:first",
		.gpio		= WL341V3_GPIO_LED_FIRST_AMBER,
		.active_low	= 1,
	}, {
		.name		= "wl341v3:blue:first",
		.gpio		= WL341V3_GPIO_LED_FIRST_BLUE,
		.active_low	= 1,
	}, {
		.name		= "wl341v3:amber:third",
		.gpio		= WL341V3_GPIO_LED_THIRD_AMBER,
		.active_low	= 1,
	}, {
		.name		= "wl341v3:blue:third",
		.gpio		= WL341V3_GPIO_LED_THIRD_BLUE,
		.active_low	= 1,
	}, {
		.name		= "wl341v3:blue:fourth",
		.gpio		= WL341V3_GPIO_LED_FOURTH_BLUE,
		.active_low	= 1,
	}, {
		.name		= "wl341v3:amber:fifth",
		.gpio		= WL341V3_GPIO_LED_FIFTH_AMBER,
		.active_low	= 1,
	}, {
		.name		= "wl341v3:blue:fifth",
		.gpio		= WL341V3_GPIO_LED_FIFTH_BLUE,
		.active_low	= 1,
	}
};

static struct gpio_keys_button wl341v3_gpio_buttons[] __initdata = {
	{
		.desc		= "reset",
		.type		= EV_KEY,
		.code		= KEY_RESTART,
		.debounce_interval = WL341V3_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= WL341V3_GPIO_BUTTON_RESET,
		.active_low	= 1,
	}, {
		.desc		= "wps",
		.type		= EV_KEY,
		.code		= KEY_WPS_BUTTON,
		.debounce_interval = WL341V3_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= WL341V3_GPIO_BUTTON_WPS,
		.active_low	= 1,
	}
};

static void __init wl341v3_init(void)
{
	rt305x_gpio_init(RT305X_GPIO_MODE_GPIO << RT305X_GPIO_MODE_UART0_SHIFT);

	rt305x_register_flash(0);

	rt305x_esw_data.vlan_config = RT305X_ESW_VLAN_CONFIG_WLLLL;
	rt305x_register_ethernet();
	ramips_register_gpio_leds(-1, ARRAY_SIZE(wl341v3_leds_gpio),
				  wl341v3_leds_gpio);
	ramips_register_gpio_buttons(-1, WL341V3_KEYS_POLL_INTERVAL,
				     ARRAY_SIZE(wl341v3_gpio_buttons),
				     wl341v3_gpio_buttons);
	rt305x_register_wifi();
	rt305x_register_wdt();
	rt305x_register_usb();
}

MIPS_MACHINE(RAMIPS_MACH_WL341V3, "WL341V3", "Sitecom WL-341 v3",
	     wl341v3_init);
