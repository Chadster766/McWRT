/*
 *  MoFi Network MOFI3500-3GN board support
 *
 *  Copyright (C) 2011 Layne Edwards <ledwards76@gmail.com>
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

#define MOFI3500_3GN_GPIO_LED_USB		8
#define MOFI3500_3GN_GPIO_LED_3G		11
#define MOFI3500_3GN_GPIO_LED_STATUS		12
#define MOFI3500_3GN_GPIO_LED_WPS		14

#define MOFI3500_3GN_GPIO_BUTTON_RESET		10
#define MOFI3500_3GN_GPIO_BUTTON_CONNECT	7
#define MOFI3500_3GN_GPIO_BUTTON_WPS		0

#define MOFI3500_3GN_KEYS_POLL_INTERVAL		20
#define MOFI3500_3GN_KEYS_DEBOUNCE_INTERVAL	(3 * MOFI3500_3GN_KEYS_POLL_INTERVAL)

static struct gpio_led mofi3500_3gn_leds_gpio[] __initdata = {
	{
		.name		= "mofi3500-3gn:green:usb",
		.gpio		= MOFI3500_3GN_GPIO_LED_USB,
		.active_low	= 1,
	}, {
		.name		= "mofi3500-3gn:green:3g",
		.gpio		= MOFI3500_3GN_GPIO_LED_3G,
		.active_low	= 1,
	}, {
		.name		= "mofi3500-3gn:green:status",
		.gpio		= MOFI3500_3GN_GPIO_LED_STATUS,
		.active_low	= 1,
	}, {
		.name		= "mofi3500-3gn:green:wps",
		.gpio		= MOFI3500_3GN_GPIO_LED_WPS,
		.active_low	= 1,
	}
};

static struct gpio_keys_button mofi3500_3gn_gpio_buttons[] __initdata = {
	{
		.desc		= "reset",
		.type		= EV_KEY,
		.code		= KEY_RESTART,
		.debounce_interval = MOFI3500_3GN_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= MOFI3500_3GN_GPIO_BUTTON_RESET,
		.active_low	= 1,
	}, {
		.desc		= "connect",
		.type		= EV_KEY,
		.code		= KEY_CONNECT,
		.debounce_interval = MOFI3500_3GN_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= MOFI3500_3GN_GPIO_BUTTON_CONNECT,
		.active_low	= 1,
	}, {
		.desc		= "wps",
		.type		= EV_KEY,
		.code		= KEY_WPS_BUTTON,
		.debounce_interval = MOFI3500_3GN_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= MOFI3500_3GN_GPIO_BUTTON_WPS,
		.active_low	= 1,
	}
};

#define MOFI3500_3GN_GPIO_MODE \
	((RT305X_GPIO_MODE_GPIO << RT305X_GPIO_MODE_UART0_SHIFT) | \
	 RT305X_GPIO_MODE_MDIO)

static void __init mofi3500_3gn_init(void)
{
	rt305x_gpio_init(MOFI3500_3GN_GPIO_MODE);

	rt305x_register_flash(0);

	rt305x_esw_data.vlan_config = RT305X_ESW_VLAN_CONFIG_LLLLW;
	rt305x_register_ethernet();
	ramips_register_gpio_leds(-1, ARRAY_SIZE(mofi3500_3gn_leds_gpio),
				  mofi3500_3gn_leds_gpio);
	ramips_register_gpio_buttons(-1, MOFI3500_3GN_KEYS_POLL_INTERVAL,
				     ARRAY_SIZE(mofi3500_3gn_gpio_buttons),
				     mofi3500_3gn_gpio_buttons);
	rt305x_register_wifi();
	rt305x_register_wdt();
	rt305x_register_usb();
}

MIPS_MACHINE(RAMIPS_MACH_MOFI3500_3GN, "MOFI3500-3GN", "MoFi Network MOFI3500-3GN",
	     mofi3500_3gn_init);
