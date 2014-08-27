#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/leds.h>
#include <linux/gpio.h>
#include <linux/gpio_buttons.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/input.h>
#include <linux/interrupt.h>
#include <linux/spi/spi.h>
#include <linux/spi/flash.h>

#include <dev-gpio-leds.h>

#include "../machtypes.h"
#include "devices.h"

#define EASY98020_GPIO_LED_0 9
#define EASY98020_GPIO_LED_1 10
#define EASY98020_GPIO_LED_2 11
#define EASY98020_GPIO_LED_3 12
#define EASY98020_GPIO_LED_GE0_ACT 110
#define EASY98020_GPIO_LED_GE0_LINK 109
#define EASY98020_GPIO_LED_GE1_ACT 106
#define EASY98020_GPIO_LED_GE1_LINK 105

static struct mtd_partition easy98020_spi_partitions[] =
{
	{
		.name	= "uboot",
		.offset	= 0x0,
		.size	= 0x40000,
	},
	{
		.name	= "uboot_env",
		.offset	= 0x40000,
		.size	= 0x40000,	/* 2 sectors for redundant env. */
	},
	{
		.name	= "linux",
		.offset	= 0x80000,
		.size	= 0xF80000,	/* map only 16 MiB */
	},
};

static struct flash_platform_data easy98020_spi_flash_platform_data = {
	.name = "sflash",
	.parts = easy98020_spi_partitions,
	.nr_parts = ARRAY_SIZE(easy98020_spi_partitions)
};

static struct spi_board_info easy98020_spi_flash_data __initdata = {
	.modalias		= "m25p80",
	.bus_num		= 0,
	.chip_select		= 0,
	.max_speed_hz		= 10 * 1000 * 1000,
	.mode			= SPI_MODE_3,
	.platform_data		= &easy98020_spi_flash_platform_data
};

static struct gpio_led easy98020_gpio_leds[] __initdata = {
	{
		.name		= "easy98020:green:0",
		.gpio		= EASY98020_GPIO_LED_0,
		.active_low	= 0,
	}, {
		.name		= "easy98020:green:1",
		.gpio		= EASY98020_GPIO_LED_1,
		.active_low	= 0,
	}, {
		.name		= "easy98020:green:2",
		.gpio		= EASY98020_GPIO_LED_2,
		.active_low	= 0,
	}, {
		.name		= "easy98020:green:3",
		.gpio		= EASY98020_GPIO_LED_3,
		.active_low	= 0,
	}, {
		.name		= "easy98020:ge0_act",
		.gpio		= EASY98020_GPIO_LED_GE0_ACT,
		.active_low	= 0,
	}, {
		.name		= "easy98020:ge0_link",
		.gpio		= EASY98020_GPIO_LED_GE0_LINK,
		.active_low	= 0,
	}, {
		.name		= "easy98020:ge1_act",
		.gpio		= EASY98020_GPIO_LED_GE1_ACT,
		.active_low	= 0,
	}, {
		.name		= "easy98020:ge1_link",
		.gpio		= EASY98020_GPIO_LED_GE1_LINK,
		.active_low	= 0,
	}
};

static void __init easy98020_init(void)
{
	falcon_register_i2c();
	falcon_register_spi_flash(&easy98020_spi_flash_data);
	ltq_add_device_gpio_leds(-1, ARRAY_SIZE(easy98020_gpio_leds),
					easy98020_gpio_leds);
}

MIPS_MACHINE(LANTIQ_MACH_EASY98020,
			"EASY98020",
			"EASY98020 Eval Board",
			easy98020_init);

MIPS_MACHINE(LANTIQ_MACH_EASY98020_1LAN,
			"EASY98020_1LAN",
			"EASY98020 Eval Board (1 LAN port)",
			easy98020_init);

MIPS_MACHINE(LANTIQ_MACH_EASY98020_2LAN,
			"EASY98020_2LAN",
			"EASY98020 Eval Board (2 LAN ports)",
			easy98020_init);
