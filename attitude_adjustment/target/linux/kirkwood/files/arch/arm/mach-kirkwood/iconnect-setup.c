/*
 * arch/arm/mach-kirkwood/iconnect-setup.c
 *
 * Iomega iConnect Wireless
 *
 * This file is licensed under the terms of the GNU General Public
 * License version 2.  This program is licensed "as is" without any
 * warranty of any kind, whether express or implied.
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/irq.h>
#include <linux/mtd/partitions.h>
#include <linux/mv643xx_eth.h>
#include <linux/ethtool.h>
#include <linux/gpio.h>
#include <linux/gpio_keys.h>
#include <linux/input.h>
#include <linux/leds.h>
#include <asm/mach-types.h>
#include <asm/mach/arch.h>
#include <mach/kirkwood.h>
#include "common.h"
#include "mpp.h"

static struct mtd_partition iconnect_nand_parts[] = {
	{
		.name = "u-boot",
		.offset = 0,
		.size = SZ_1M
	}, {
		.name = "uImage",
		.offset = MTDPART_OFS_NXTBLK,
		.size = SZ_1M + SZ_2M
	}, {
		.name = "rootfs",
		.offset = MTDPART_OFS_NXTBLK,
		.size = SZ_32M,
	}, {
		.name = "data",
		.offset = MTDPART_OFS_NXTBLK,
		.size = MTDPART_SIZ_FULL
	},
};

static struct mv643xx_eth_platform_data iconnect_ge00_data = {
	.phy_addr	= MV643XX_ETH_PHY_ADDR(11),
};

static struct gpio_led iconnect_led_pins[] = {
	{
		.name			= "iconnect:blue:power",
		.default_trigger	= "default-on",
		.gpio			= 42,
	},
	{
		.name			= "iconnect:red:power",
		.gpio			= 43,
	},
	{
		.name			= "iconnect:blue:usb1",
		.gpio			= 44,
	},
	{
		.name			= "iconnect:blue:usb2",
		.gpio			= 45,
	},
	{
		.name			= "iconnect:blue:usb3",
		.gpio			= 46,
	},
	{
		.name			= "iconnect:blue:usb4",
		.gpio			= 47,
	},
	{
		.name			= "iconnect:blue:otb",
		.gpio			= 48,
	},
};

static struct gpio_led_platform_data iconnect_led_data = {
	.leds		= iconnect_led_pins,
	.num_leds	= ARRAY_SIZE(iconnect_led_pins),
};

static struct platform_device iconnect_leds = {
	.name	= "leds-gpio",
	.id	= -1,
	.dev	= {
		.platform_data	= &iconnect_led_data,
	}
};

#define ICONNECT_GPIO_KEY_RESET		12
#define ICONNECT_GPIO_KEY_OTB		35

#define ICONNECT_SW_RESET		0x00
#define ICONNECT_SW_OTB			0x01

static struct gpio_keys_button iconnect_buttons[] = {
	{
		.type		= EV_SW,
		.code		= ICONNECT_SW_RESET,
		.gpio		= ICONNECT_GPIO_KEY_RESET,
		.desc		= "Reset Button",
		.active_low	= 1,
		.debounce_interval = 100,
	},
	{
		.type		= EV_SW,
		.code		= ICONNECT_SW_OTB,
		.gpio		= ICONNECT_GPIO_KEY_OTB,
		.desc		= "OTB Button",
		.active_low	= 1,
		.debounce_interval = 100,
	},
};

static struct gpio_keys_platform_data iconnect_button_data = {
	.buttons	= iconnect_buttons,
	.nbuttons	= ARRAY_SIZE(iconnect_buttons),
};

static struct platform_device iconnect_button_device = {
	.name	   	= "gpio-keys",
	.id		= -1,
	.num_resources	= 0,
	.dev = {
		.platform_data = &iconnect_button_data,
	},
};

static unsigned int iconnect_mpp_config[] __initdata = {
	MPP12_GPIO, /*Input for reset button*/
	MPP35_GPIO, /*Input for OTB button*/
	MPP42_GPIO,
	MPP43_GPIO,
	MPP44_GPIO,
	MPP45_GPIO,
	MPP46_GPIO,
	MPP47_GPIO,
	MPP48_GPIO,
	0
};

static void __init iconnect_init(void)
{
	u32 dev, rev;

	/*
	 * Basic setup. Needs to be called early.
	 */
	kirkwood_init();
	kirkwood_mpp_conf(iconnect_mpp_config);

	kirkwood_nand_init(ARRAY_AND_SIZE(iconnect_nand_parts), 25);
	kirkwood_ehci_init();

	kirkwood_ge00_init(&iconnect_ge00_data);
	kirkwood_pcie_id(&dev, &rev);

	kirkwood_uart0_init();
	kirkwood_i2c_init();

	platform_device_register(&iconnect_leds);
	platform_device_register(&iconnect_button_device);
}

static int __init iconnect_pci_init(void)
{
	if (machine_is_iconnect())
		kirkwood_pcie_init(KW_PCIE0);

	return 0;
}
subsys_initcall(iconnect_pci_init);


MACHINE_START(ICONNECT, "Iomega iConnect Wireless")
	.atag_offset	= 0x100,
	.init_machine	= iconnect_init,
	.map_io		= kirkwood_map_io,
	.init_early	= kirkwood_init_early,
	.init_irq	= kirkwood_init_irq,
	.timer		= &kirkwood_timer,
	.restart	= kirkwood_restart,
MACHINE_END
