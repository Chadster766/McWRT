/*
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 *
 *  Copyright (C) 2011 Thomas Langer <thomas.langer@lantiq.com>
 *  Copyright (C) 2011 John Crispin <blogic@openwrt.org>
 */

#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/slab.h>
#include <linux/export.h>
#include <linux/err.h>
#include <linux/platform_device.h>

#include <lantiq_soc.h>

/* Multiplexer Control Register */
#define LTQ_PADC_MUX(x)         (x * 0x4)
/* Pad Control Availability Register */
#define LTQ_PADC_AVAIL          0x000000F0

/* Data Output Register */
#define LTQ_GPIO_OUT            0x00000000
/* Data Input Register */
#define LTQ_GPIO_IN             0x00000004
/* Direction Register */
#define LTQ_GPIO_DIR            0x00000008
/* External Interrupt Control Register 0 */
#define LTQ_GPIO_EXINTCR0       0x00000018
/* External Interrupt Control Register 1 */
#define LTQ_GPIO_EXINTCR1       0x0000001C
/* IRN Capture Register */
#define LTQ_GPIO_IRNCR          0x00000020
/* IRN Interrupt Configuration Register */
#define LTQ_GPIO_IRNCFG		0x0000002C
/* IRN Interrupt Enable Set Register */
#define LTQ_GPIO_IRNRNSET       0x00000030
/* IRN Interrupt Enable Clear Register */
#define LTQ_GPIO_IRNENCLR       0x00000034
/* Output Set Register */
#define LTQ_GPIO_OUTSET         0x00000040
/* Output Cler Register */
#define LTQ_GPIO_OUTCLR         0x00000044
/* Direction Clear Register */
#define LTQ_GPIO_DIRSET         0x00000048
/* Direction Set Register */
#define LTQ_GPIO_DIRCLR         0x0000004C

/* turn a gpio_chip into a falcon_gpio_port */
#define ctop(c)		container_of(c, struct falcon_gpio_port, gpio_chip)
/* turn a irq_data into a falcon_gpio_port */
#define itop(i)		((struct falcon_gpio_port *) irq_get_chip_data(i->irq))

#define ltq_pad_r32(p, reg)		ltq_r32(p->pad + reg)
#define ltq_pad_w32(p, val, reg)	ltq_w32(val, p->pad + reg)
#define ltq_pad_w32_mask(c, clear, set, reg) \
		ltq_pad_w32(c, (ltq_pad_r32(c, reg) & ~(clear)) | (set), reg)

#define ltq_port_r32(p, reg)		ltq_r32(p->port + reg)
#define ltq_port_w32(p, val, reg)	ltq_w32(val, p->port + reg)
#define ltq_port_w32_mask(p, clear, set, reg) \
		ltq_port_w32(p, (ltq_port_r32(p, reg) & ~(clear)) | (set), reg)

#define MAX_PORTS		5
#define PINS_PER_PORT		32

struct falcon_gpio_port {
	struct gpio_chip gpio_chip;
	void __iomem *pad;
	void __iomem *port;
	unsigned int irq_base;
	unsigned int chained_irq;
	struct clk *clk;
};

static struct falcon_gpio_port ltq_gpio_port[MAX_PORTS];

int gpio_to_irq(unsigned int gpio)
{
	return __gpio_to_irq(gpio);
}
EXPORT_SYMBOL(gpio_to_irq);

int ltq_gpio_mux_set(unsigned int pin, unsigned int mux)
{
	int port = pin / 100;
	int offset = pin % 100;
	struct falcon_gpio_port *gpio_port;

	if ((offset >= PINS_PER_PORT) || (port >= MAX_PORTS))
		return -EINVAL;

	gpio_port = &ltq_gpio_port[port];
	ltq_pad_w32(gpio_port, mux & 0x3, LTQ_PADC_MUX(offset));

	return 0;
}
EXPORT_SYMBOL(ltq_gpio_mux_set);

int ltq_gpio_request(struct device *dev, unsigned int pin, unsigned int mux,
			unsigned int dir, const char *name)
{
	int port = pin / 100;
	int offset = pin % 100;

	if (offset >= PINS_PER_PORT || port >= MAX_PORTS)
		return -EINVAL;

	if (devm_gpio_request(dev, pin, name)) {
		pr_err("failed to setup lantiq gpio: %s\n", name);
		return -EBUSY;
	}

	if (dir)
		gpio_direction_output(pin, 1);
	else
		gpio_direction_input(pin);

	return ltq_gpio_mux_set(pin, mux);
}
EXPORT_SYMBOL(ltq_gpio_request);

static int
falcon_gpio_direction_input(struct gpio_chip *chip, unsigned int offset)
{
	ltq_port_w32(ctop(chip), 1 << offset, LTQ_GPIO_DIRCLR);

	return 0;
}

static void
falcon_gpio_set(struct gpio_chip *chip, unsigned int offset, int value)
{
	if (value)
		ltq_port_w32(ctop(chip), 1 << offset, LTQ_GPIO_OUTSET);
	else
		ltq_port_w32(ctop(chip), 1 << offset, LTQ_GPIO_OUTCLR);
}

static int
falcon_gpio_direction_output(struct gpio_chip *chip,
			unsigned int offset, int value)
{
	falcon_gpio_set(chip, offset, value);
	ltq_port_w32(ctop(chip), 1 << offset, LTQ_GPIO_DIRSET);

	return 0;
}

static int
falcon_gpio_get(struct gpio_chip *chip, unsigned int offset)
{
	if ((ltq_port_r32(ctop(chip), LTQ_GPIO_DIR) >> offset) & 1)
		return (ltq_port_r32(ctop(chip), LTQ_GPIO_OUT) >> offset) & 1;
	else
		return (ltq_port_r32(ctop(chip), LTQ_GPIO_IN) >> offset) & 1;
}

static int
falcon_gpio_request(struct gpio_chip *chip, unsigned offset)
{
	if ((ltq_pad_r32(ctop(chip), LTQ_PADC_AVAIL) >> offset) & 1) {
		if (ltq_pad_r32(ctop(chip), LTQ_PADC_MUX(offset)) > 1)
			return -EBUSY;
		/* switch on gpio function */
		ltq_pad_w32(ctop(chip), 1, LTQ_PADC_MUX(offset));
		return 0;
	}

	return -ENODEV;
}

static void
falcon_gpio_free(struct gpio_chip *chip, unsigned offset)
{
	if ((ltq_pad_r32(ctop(chip), LTQ_PADC_AVAIL) >> offset) & 1) {
		if (ltq_pad_r32(ctop(chip), LTQ_PADC_MUX(offset)) > 1)
			return;
		/* switch off gpio function */
		ltq_pad_w32(ctop(chip), 0, LTQ_PADC_MUX(offset));
	}
}

static int
falcon_gpio_to_irq(struct gpio_chip *chip, unsigned offset)
{
	return ctop(chip)->irq_base + offset;
}

static void
falcon_gpio_disable_irq(struct irq_data *d)
{
	unsigned int offset = d->irq - itop(d)->irq_base;

	ltq_port_w32(itop(d), 1 << offset, LTQ_GPIO_IRNENCLR);
}

static void
falcon_gpio_enable_irq(struct irq_data *d)
{
	unsigned int offset = d->irq - itop(d)->irq_base;

	if (!ltq_pad_r32(itop(d), LTQ_PADC_MUX(offset)) < 1)
		/* switch on gpio function */
		ltq_pad_w32(itop(d), 1, LTQ_PADC_MUX(offset));

	ltq_port_w32(itop(d), 1 << offset, LTQ_GPIO_IRNRNSET);
}

static void
falcon_gpio_ack_irq(struct irq_data *d)
{
	unsigned int offset = d->irq - itop(d)->irq_base;

	ltq_port_w32(itop(d), 1 << offset, LTQ_GPIO_IRNCR);
}

static void
falcon_gpio_mask_and_ack_irq(struct irq_data *d)
{
	unsigned int offset = d->irq - itop(d)->irq_base;

	ltq_port_w32(itop(d), 1 << offset, LTQ_GPIO_IRNENCLR);
	ltq_port_w32(itop(d), 1 << offset, LTQ_GPIO_IRNCR);
}

static struct irq_chip falcon_gpio_irq_chip;
static int
falcon_gpio_irq_type(struct irq_data *d, unsigned int type)
{
	unsigned int offset = d->irq - itop(d)->irq_base;
	unsigned int mask = 1 << offset;

	if ((type & IRQ_TYPE_SENSE_MASK) == IRQ_TYPE_NONE)
		return 0;

	if ((type & (IRQ_TYPE_LEVEL_HIGH | IRQ_TYPE_LEVEL_LOW)) != 0) {
		/* level triggered */
		ltq_port_w32_mask(itop(d), 0, mask, LTQ_GPIO_IRNCFG);
		irq_set_chip_and_handler_name(d->irq,
				&falcon_gpio_irq_chip, handle_level_irq, "mux");
	} else {
		/* edge triggered */
		ltq_port_w32_mask(itop(d), mask, 0, LTQ_GPIO_IRNCFG);
		irq_set_chip_and_handler_name(d->irq,
			&falcon_gpio_irq_chip, handle_simple_irq, "mux");
	}

	if ((type & IRQ_TYPE_EDGE_BOTH) == IRQ_TYPE_EDGE_BOTH) {
		ltq_port_w32_mask(itop(d), mask, 0, LTQ_GPIO_EXINTCR0);
		ltq_port_w32_mask(itop(d), 0, mask, LTQ_GPIO_EXINTCR1);
	} else {
		if ((type & (IRQ_TYPE_EDGE_RISING | IRQ_TYPE_LEVEL_HIGH)) != 0)
			/* positive logic: rising edge, high level */
			ltq_port_w32_mask(itop(d), mask, 0, LTQ_GPIO_EXINTCR0);
		else
			/* negative logic: falling edge, low level */
			ltq_port_w32_mask(itop(d), 0, mask, LTQ_GPIO_EXINTCR0);
		ltq_port_w32_mask(itop(d), mask, 0, LTQ_GPIO_EXINTCR1);
	}

	return gpio_direction_input(itop(d)->gpio_chip.base + offset);
}

static void
falcon_gpio_irq_handler(unsigned int irq, struct irq_desc *desc)
{
	struct falcon_gpio_port *gpio_port = irq_desc_get_handler_data(desc);
	unsigned long irncr;
	int offset;

	/* acknowledge interrupt */
	irncr = ltq_port_r32(gpio_port, LTQ_GPIO_IRNCR);
	ltq_port_w32(gpio_port, irncr, LTQ_GPIO_IRNCR);

	desc->irq_data.chip->irq_ack(&desc->irq_data);

	for_each_set_bit(offset, &irncr, gpio_port->gpio_chip.ngpio)
		generic_handle_irq(gpio_port->irq_base + offset);
}

static struct irq_chip falcon_gpio_irq_chip = {
	.name = "gpio_irq_mux",
	.irq_mask = falcon_gpio_disable_irq,
	.irq_unmask = falcon_gpio_enable_irq,
	.irq_ack = falcon_gpio_ack_irq,
	.irq_mask_ack = falcon_gpio_mask_and_ack_irq,
	.irq_set_type = falcon_gpio_irq_type,
};

static struct irqaction gpio_cascade = {
	.handler = no_action,
	.flags = IRQF_DISABLED,
	.name = "gpio_cascade",
};

static int
falcon_gpio_probe(struct platform_device *pdev)
{
	struct falcon_gpio_port *gpio_port;
	int ret, i;
	struct resource *gpiores, *padres;
	int irq;

	if (pdev->id >= MAX_PORTS)
		return -ENODEV;

	gpiores = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	padres = platform_get_resource(pdev, IORESOURCE_MEM, 1);
	irq = platform_get_irq(pdev, 0);
	if (!gpiores || !padres)
		return -ENODEV;

	gpio_port = &ltq_gpio_port[pdev->id];
	gpio_port->gpio_chip.label = "falcon-gpio";
	gpio_port->gpio_chip.direction_input = falcon_gpio_direction_input;
	gpio_port->gpio_chip.direction_output = falcon_gpio_direction_output;
	gpio_port->gpio_chip.get = falcon_gpio_get;
	gpio_port->gpio_chip.set = falcon_gpio_set;
	gpio_port->gpio_chip.request = falcon_gpio_request;
	gpio_port->gpio_chip.free = falcon_gpio_free;
	gpio_port->gpio_chip.base = 100 * pdev->id;
	gpio_port->gpio_chip.ngpio = 32;
	gpio_port->gpio_chip.dev = &pdev->dev;

	gpio_port->port = ltq_remap_resource(gpiores);
	gpio_port->pad = ltq_remap_resource(padres);

	if (!gpio_port->port || !gpio_port->pad) {
		dev_err(&pdev->dev, "Could not map io ranges\n");
		ret = -ENOMEM;
		goto err;
	}

	gpio_port->clk = clk_get(&pdev->dev, NULL);
	if (IS_ERR(gpio_port->clk)) {
		dev_err(&pdev->dev, "Could not get clock\n");
		ret = PTR_ERR(gpio_port->clk);;
		goto err;
	}
	clk_enable(gpio_port->clk);

	if (irq > 0) {
		/* irq_chip support */
		gpio_port->gpio_chip.to_irq = falcon_gpio_to_irq;
		gpio_port->irq_base = INT_NUM_EXTRA_START + (32 * pdev->id);

		for (i = 0; i < 32; i++) {
			irq_set_chip_and_handler_name(gpio_port->irq_base + i,
				&falcon_gpio_irq_chip, handle_simple_irq,
				"mux");
			irq_set_chip_data(gpio_port->irq_base + i, gpio_port);
			/* set to negative logic (falling edge, low level) */
			ltq_port_w32_mask(gpio_port, 0, 1 << i,
				LTQ_GPIO_EXINTCR0);
		}

		gpio_port->chained_irq = irq;
		setup_irq(irq, &gpio_cascade);
		irq_set_handler_data(irq, gpio_port);
		irq_set_chained_handler(irq, falcon_gpio_irq_handler);
	}

	ret = gpiochip_add(&gpio_port->gpio_chip);
	if (ret < 0) {
		dev_err(&pdev->dev, "Could not register gpiochip %d, %d\n",
			pdev->id, ret);
		goto err;
	}
	platform_set_drvdata(pdev, gpio_port);
	return ret;

err:
	dev_err(&pdev->dev, "Error in gpio_probe %d, %d\n", pdev->id, ret);
	if (gpiores)
		release_resource(gpiores);
	if (padres)
		release_resource(padres);

	if (gpio_port->port)
		iounmap(gpio_port->port);
	if (gpio_port->pad)
		iounmap(gpio_port->pad);
	return ret;
}

static struct platform_driver falcon_gpio_driver = {
	.probe = falcon_gpio_probe,
	.driver = {
		.name = "falcon_gpio",
		.owner = THIS_MODULE,
	},
};

int __init
falcon_gpio_init(void)
{
	int ret;

	pr_info("FALC(tm) ON GPIO Driver, (C) 2011 Lantiq Deutschland Gmbh\n");
	ret = platform_driver_register(&falcon_gpio_driver);
	if (ret)
		pr_err("falcon_gpio: Error registering platform driver!");
	return ret;
}

postcore_initcall(falcon_gpio_init);
