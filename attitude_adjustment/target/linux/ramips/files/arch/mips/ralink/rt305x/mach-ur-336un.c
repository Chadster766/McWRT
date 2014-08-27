#include <linux/init.h>
#include <linux/platform_device.h>

#include <asm/mach-ralink/machine.h>
#include <asm/mach-ralink/dev-gpio-buttons.h>
#include <asm/mach-ralink/dev-gpio-leds.h>
#include <asm/mach-ralink/rt305x.h>
#include <asm/mach-ralink/rt305x_regs.h>

#include "devices.h"


#define UR_336UN_GPIO_LED_3G		9
#define UR_336UN_GPIO_LED_GATEWAY	11
#define UR_336UN_GPIO_LED_AP		12
#define UR_336UN_GPIO_LED_STATION	13
#define UR_336UN_GPIO_LED_WPS		14

#define UR_336UN_GPIO_BUTTON_RESET	10
#define UR_336UN_GPIO_BUTTON_CONNECT	7
#define UR_336UN_GPIO_BUTTON_WPS	0
#define UR_336UN_GPIO_BUTTON_WPS2	8

#define UR_336UN_KEYS_POLL_INTERVAL	20
#define UR_336UN_KEYS_DEBOUNCE_INTERVAL (3 * UR_336UN_KEYS_POLL_INTERVAL)

static struct gpio_led UR_336UN_leds_gpio[] __initdata = {
	{
		.name		= "ur336:green:3g",
		.gpio		= UR_336UN_GPIO_LED_3G,
		.active_low	= 1,
	}, {
		.name		= "ur336:green:gateway",
		.gpio		= UR_336UN_GPIO_LED_GATEWAY,
		.active_low	= 1,
	}, {
		.name		= "ur336:green:ap",
		.gpio		= UR_336UN_GPIO_LED_AP,
		.active_low	= 1,
	}, {
		.name		= "ur336:green:wps",
		.gpio		= UR_336UN_GPIO_LED_WPS,
		.active_low	= 1,
	}, {
		.name		= "ur336:green:station",
		.gpio		= UR_336UN_GPIO_LED_STATION,
		.active_low	= 1,
	}
};

static struct gpio_keys_button UR_336UN_gpio_buttons[] __initdata = {
	{
		.desc		= "reset_wps",
		.type		= EV_KEY,
		.code		= KEY_RESTART,
		.debounce_interval = UR_336UN_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= UR_336UN_GPIO_BUTTON_RESET,
		.active_low	= 1,
	}, {
		.desc		= "mode",
		.type		= EV_KEY,
		.code		= KEY_M,
		.debounce_interval = UR_336UN_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= UR_336UN_GPIO_BUTTON_CONNECT,
		.active_low	= 1,
	}
};

#define UR_336UN_GPIO_MODE \
	((RT305X_GPIO_MODE_GPIO << RT305X_GPIO_MODE_UART0_SHIFT) | \
	 RT305X_GPIO_MODE_MDIO)

static void __init UR_336UN_init(void)
{
	rt305x_gpio_init(UR_336UN_GPIO_MODE);

	rt305x_register_flash(0);

	rt305x_esw_data.vlan_config = RT305X_ESW_VLAN_CONFIG_LLLLW;
	rt305x_register_ethernet();
	ramips_register_gpio_leds(-1, ARRAY_SIZE(UR_336UN_leds_gpio),
				  UR_336UN_leds_gpio);
	ramips_register_gpio_buttons(-1, UR_336UN_KEYS_POLL_INTERVAL,
				     ARRAY_SIZE(UR_336UN_gpio_buttons),
				     UR_336UN_gpio_buttons);
	rt305x_register_wifi();
	rt305x_register_wdt();
	rt305x_register_usb();
}

MIPS_MACHINE(RAMIPS_MACH_UR_336UN, "UR-336UN", "UR-336UN Wireless N router",
	      UR_336UN_init);
