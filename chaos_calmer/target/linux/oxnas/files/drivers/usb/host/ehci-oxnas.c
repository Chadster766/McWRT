/*
 * drivers/usb/host/ehci-oxnas.c
 *
 * Tzachi Perelstein <tzachi@marvell.com>
 *
 * This file is licensed under  the terms of the GNU General Public
 * License version 2. This program is licensed "as is" without any
 * warranty of any kind, whether express or implied.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_irq.h>
#include <linux/usb.h>
#include <linux/usb/hcd.h>
#include <linux/dma-mapping.h>
#include <linux/clk.h>
#include <linux/reset.h>
#include <mach/hardware.h>
#include <mach/utils.h>

#include "ehci.h"

struct oxnas_hcd {
	struct clk *clk;
	struct clk *refsrc;
	struct clk *phyref;
	int use_pllb;
	int use_phya;
	struct reset_control *rst_host;
	struct reset_control *rst_phya;
	struct reset_control *rst_phyb;
};

#define DRIVER_DESC "Oxnas On-Chip EHCI Host Controller"

static struct hc_driver __read_mostly oxnas_hc_driver;

static void start_oxnas_usb_ehci(struct oxnas_hcd *oxnas)
{
	u32 reg;

	if (oxnas->use_pllb) {
		/* enable pllb */
		clk_prepare_enable(oxnas->refsrc);
		/* enable ref600 */
		clk_prepare_enable(oxnas->phyref);
		/* 600MHz pllb divider for 12MHz */
		writel(PLLB_DIV_INT(50) | PLLB_DIV_FRAC(0),
			SEC_CTRL_PLLB_DIV_CTRL);

	} else {
		/* ref 300 divider for 12MHz */
		writel(REF300_DIV_INT(25) | REF300_DIV_FRAC(0),
			SYS_CTRL_REF300_DIV);
	}

	/* Ensure the USB block is properly reset */
	reset_control_reset(oxnas->rst_host);
	reset_control_reset(oxnas->rst_phya);
	reset_control_reset(oxnas->rst_phyb);

	/* Force the high speed clock to be generated all the time, via serial
	 programming of the USB HS PHY */
	writel((2UL << USBHSPHY_TEST_ADD) |
		   (0xe0UL << USBHSPHY_TEST_DIN), SYS_CTRL_USBHSPHY_CTRL);

	writel((1UL << USBHSPHY_TEST_CLK) |
		   (2UL << USBHSPHY_TEST_ADD) |
		   (0xe0UL << USBHSPHY_TEST_DIN), SYS_CTRL_USBHSPHY_CTRL);

	writel((0xfUL << USBHSPHY_TEST_ADD) |
		   (0xaaUL << USBHSPHY_TEST_DIN), SYS_CTRL_USBHSPHY_CTRL);

	writel((1UL << USBHSPHY_TEST_CLK) |
		   (0xfUL << USBHSPHY_TEST_ADD) |
		   (0xaaUL << USBHSPHY_TEST_DIN), SYS_CTRL_USBHSPHY_CTRL);

	if (oxnas->use_pllb) /* use pllb clock */
		writel(USB_CLK_INTERNAL | USB_INT_CLK_PLLB, SYS_CTRL_USB_CTRL);
	else /* use ref300 derived clock */
		writel(USB_CLK_INTERNAL | USB_INT_CLK_REF300,
			SYS_CTRL_USB_CTRL);

	if (oxnas->use_phya) {
		/* Configure USB PHYA as a host */
		reg = readl(SYS_CTRL_USB_CTRL);
		reg &= ~USBAMUX_DEVICE;
		writel(reg, SYS_CTRL_USB_CTRL);
	}

	/* Enable the clock to the USB block */
	clk_prepare_enable(oxnas->clk);
}

static void stop_oxnas_usb_ehci(struct oxnas_hcd *oxnas)
{
	reset_control_assert(oxnas->rst_host);
	reset_control_assert(oxnas->rst_phya);
	reset_control_assert(oxnas->rst_phyb);

	if (oxnas->use_pllb) {
		clk_disable_unprepare(oxnas->phyref);
		clk_disable_unprepare(oxnas->refsrc);
	}
	clk_disable_unprepare(oxnas->clk);
}

static int ehci_oxnas_reset(struct usb_hcd *hcd)
{
	#define  txttfill_tuning	reserved2[0]

	struct ehci_hcd	*ehci;
	u32 tmp;
	int retval = ehci_setup(hcd);
	if (retval)
		return retval;

	ehci = hcd_to_ehci(hcd);
	tmp = ehci_readl(ehci, &ehci->regs->txfill_tuning);
	tmp &= ~0x00ff0000;
	tmp |= 0x003f0000; /* set burst pre load count to 0x40 (63 * 4 bytes)  */
	tmp |= 0x16; /* set sheduler overhead to 22 * 1.267us (HS) or 22 * 6.33us (FS/LS)*/
	ehci_writel(ehci, tmp,  &ehci->regs->txfill_tuning);

	tmp = ehci_readl(ehci, &ehci->regs->txttfill_tuning);
	tmp |= 0x2; /* set sheduler overhead to 2 * 6.333us */
	ehci_writel(ehci, tmp,  &ehci->regs->txttfill_tuning);

	return retval;
}

static int ehci_oxnas_drv_probe(struct platform_device *ofdev)
{
	struct device_node *np = ofdev->dev.of_node;
	struct usb_hcd *hcd;
	struct ehci_hcd *ehci;
	struct resource res;
	struct oxnas_hcd *oxnas;
	int irq, err;
	struct reset_control *rstc;

	if (usb_disabled())
		return -ENODEV;

	if (!ofdev->dev.dma_mask)
		ofdev->dev.dma_mask = &ofdev->dev.coherent_dma_mask;
	if (!ofdev->dev.coherent_dma_mask)
		ofdev->dev.coherent_dma_mask = DMA_BIT_MASK(32);

	hcd = usb_create_hcd(&oxnas_hc_driver,	&ofdev->dev,
					dev_name(&ofdev->dev));
	if (!hcd)
		return -ENOMEM;

	err = of_address_to_resource(np, 0, &res);
	if (err)
		goto err_res;

	hcd->rsrc_start = res.start;
	hcd->rsrc_len = resource_size(&res);

	hcd->regs = devm_ioremap_resource(&ofdev->dev, &res);
	if (IS_ERR(hcd->regs)) {
		dev_err(&ofdev->dev, "devm_ioremap_resource failed\n");
		err = PTR_ERR(hcd->regs);
		goto err_ioremap;
	}

	oxnas = (struct oxnas_hcd *)hcd_to_ehci(hcd)->priv;

	oxnas->use_pllb = of_property_read_bool(np, "plxtech,ehci_use_pllb");
	oxnas->use_phya = of_property_read_bool(np, "plxtech,ehci_use_phya");

	oxnas->clk = of_clk_get_by_name(np, "usb");
	if (IS_ERR(oxnas->clk)) {
		err = PTR_ERR(oxnas->clk);
		goto err_clk;
	}

	if (oxnas->use_pllb) {
		oxnas->refsrc = of_clk_get_by_name(np, "refsrc");
		if (IS_ERR(oxnas->refsrc)) {
			err = PTR_ERR(oxnas->refsrc);
			goto err_refsrc;
		}
		oxnas->phyref = of_clk_get_by_name(np, "phyref");
		if (IS_ERR(oxnas->refsrc)) {
			err = PTR_ERR(oxnas->refsrc);
			goto err_phyref;
		}

	} else {
		oxnas->refsrc = NULL;
		oxnas->phyref = NULL;
	}

	rstc = devm_reset_control_get(&ofdev->dev, "host");
	if (IS_ERR(rstc)) {
		err = PTR_ERR(rstc);
		goto err_rst;
	}
	oxnas->rst_host = rstc;

	rstc = devm_reset_control_get(&ofdev->dev, "phya");
	if (IS_ERR(rstc)) {
		err = PTR_ERR(rstc);
		goto err_rst;
	}
	oxnas->rst_phya = rstc;

	rstc = devm_reset_control_get(&ofdev->dev, "phyb");
	if (IS_ERR(rstc)) {
		err = PTR_ERR(rstc);
		goto err_rst;
	}
	oxnas->rst_phyb = rstc;

	irq = irq_of_parse_and_map(np, 0);
	if (!irq) {
		dev_err(&ofdev->dev, "irq_of_parse_and_map failed\n");
		err = -EBUSY;
		goto err_irq;
	}

	hcd->has_tt = 1;
	ehci = hcd_to_ehci(hcd);
	ehci->caps = hcd->regs;

	start_oxnas_usb_ehci(oxnas);

	err = usb_add_hcd(hcd, irq, IRQF_SHARED | IRQF_DISABLED);
	if (err)
		goto err_hcd;

	return 0;

err_hcd:
	stop_oxnas_usb_ehci(oxnas);
err_irq:
err_rst:
	if (oxnas->phyref)
		clk_put(oxnas->phyref);
err_phyref:
	if (oxnas->refsrc)
		clk_put(oxnas->refsrc);
err_refsrc:
	clk_put(oxnas->clk);
err_clk:
err_ioremap:
err_res:
	usb_put_hcd(hcd);

	return err;
}

static int ehci_oxnas_drv_remove(struct platform_device *pdev)
{
	struct usb_hcd *hcd = platform_get_drvdata(pdev);
	struct oxnas_hcd *oxnas = (struct oxnas_hcd *)hcd_to_ehci(hcd)->priv;

	usb_remove_hcd(hcd);
	if (oxnas->use_pllb) {
		clk_disable_unprepare(oxnas->phyref);
		clk_put(oxnas->phyref);
		clk_disable_unprepare(oxnas->refsrc);
		clk_put(oxnas->refsrc);
	}
	clk_disable_unprepare(oxnas->clk);
	usb_put_hcd(hcd);

	return 0;
}

static const struct of_device_id oxnas_ehci_dt_ids[] = {
	{ .compatible = "plxtech,nas782x-ehci" },
	{ /* sentinel */ }
};

MODULE_DEVICE_TABLE(of, oxnas_ehci_dt_ids);

static struct platform_driver ehci_oxnas_driver = {
	.probe		= ehci_oxnas_drv_probe,
	.remove		= ehci_oxnas_drv_remove,
	.shutdown	= usb_hcd_platform_shutdown,
	.driver.name	= "oxnas-ehci",
	.driver.of_match_table	= oxnas_ehci_dt_ids,
};

static const struct ehci_driver_overrides oxnas_overrides __initconst = {
	.reset = ehci_oxnas_reset,
	.extra_priv_size = sizeof(struct oxnas_hcd),
};

static int __init ehci_oxnas_init(void)
{
	if (usb_disabled())
		return -ENODEV;

	ehci_init_driver(&oxnas_hc_driver, &oxnas_overrides);
	return platform_driver_register(&ehci_oxnas_driver);
}
module_init(ehci_oxnas_init);

static void __exit ehci_oxnas_cleanup(void)
{
	platform_driver_unregister(&ehci_oxnas_driver);
}
module_exit(ehci_oxnas_cleanup);

MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_ALIAS("platform:oxnas-ehci");
MODULE_LICENSE("GPL");
