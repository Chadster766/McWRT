/*
 * Lantiq SoC SPI controller
 *
 * Copyright (C) 2011 Daniel Schwierzeck <daniel.schwierzeck@googlemail.com>
 *
 * This program is free software; you can distribute it and/or modify it
 * under the terms of the GNU General Public License (Version 2) as
 * published by the Free Software Foundation.
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/workqueue.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/completion.h>
#include <linux/spinlock.h>
#include <linux/err.h>
#include <linux/clk.h>
#include <linux/gpio.h>
#include <linux/spi/spi.h>
#include <linux/spi/spi_bitbang.h>

#include <lantiq_soc.h>
#include <lantiq_platform.h>

#define LTQ_SPI_CLC		0x00	/* Clock control */
#define LTQ_SPI_PISEL		0x04	/* Port input select */
#define LTQ_SPI_ID		0x08	/* Identification */
#define LTQ_SPI_CON		0x10	/* Control */
#define LTQ_SPI_STAT		0x14	/* Status */
#define LTQ_SPI_WHBSTATE	0x18	/* Write HW modified state */
#define LTQ_SPI_TB		0x20	/* Transmit buffer */
#define LTQ_SPI_RB		0x24	/* Receive buffer */
#define LTQ_SPI_RXFCON		0x30	/* Receive FIFO control */
#define LTQ_SPI_TXFCON		0x34	/* Transmit FIFO control */
#define LTQ_SPI_FSTAT		0x38	/* FIFO status */
#define LTQ_SPI_BRT		0x40	/* Baudrate timer */
#define LTQ_SPI_BRSTAT		0x44	/* Baudrate timer status */
#define LTQ_SPI_SFCON		0x60	/* Serial frame control */
#define LTQ_SPI_SFSTAT		0x64	/* Serial frame status */
#define LTQ_SPI_GPOCON		0x70	/* General purpose output control */
#define LTQ_SPI_GPOSTAT		0x74	/* General purpose output status */
#define LTQ_SPI_FGPO		0x78	/* Forced general purpose output */
#define LTQ_SPI_RXREQ		0x80	/* Receive request */
#define LTQ_SPI_RXCNT		0x84	/* Receive count */
#define LTQ_SPI_DMACON		0xEC	/* DMA control */
#define LTQ_SPI_IRNEN		0xF4	/* Interrupt node enable */
#define LTQ_SPI_IRNICR		0xF8	/* Interrupt node interrupt capture */
#define LTQ_SPI_IRNCR		0xFC	/* Interrupt node control */

#define LTQ_SPI_CLC_SMC_SHIFT	16	/* Clock divider for sleep mode */
#define LTQ_SPI_CLC_SMC_MASK	0xFF
#define LTQ_SPI_CLC_RMC_SHIFT	8	/* Clock divider for normal run mode */
#define LTQ_SPI_CLC_RMC_MASK	0xFF
#define LTQ_SPI_CLC_DISS	BIT(1)	/* Disable status bit */
#define LTQ_SPI_CLC_DISR	BIT(0)	/* Disable request bit */

#define LTQ_SPI_ID_TXFS_SHIFT	24	/* Implemented TX FIFO size */
#define LTQ_SPI_ID_TXFS_MASK	0x3F
#define LTQ_SPI_ID_RXFS_SHIFT	16	/* Implemented RX FIFO size */
#define LTQ_SPI_ID_RXFS_MASK	0x3F
#define LTQ_SPI_ID_REV_MASK	0x1F	/* Hardware revision number */
#define LTQ_SPI_ID_CFG		BIT(5)	/* DMA interface support */

#define LTQ_SPI_CON_BM_SHIFT	16	/* Data width selection */
#define LTQ_SPI_CON_BM_MASK	0x1F
#define LTQ_SPI_CON_EM		BIT(24)	/* Echo mode */
#define LTQ_SPI_CON_IDLE	BIT(23)	/* Idle bit value */
#define LTQ_SPI_CON_ENBV	BIT(22)	/* Enable byte valid control */
#define LTQ_SPI_CON_RUEN	BIT(12)	/* Receive underflow error enable */
#define LTQ_SPI_CON_TUEN	BIT(11)	/* Transmit underflow error enable */
#define LTQ_SPI_CON_AEN		BIT(10)	/* Abort error enable */
#define LTQ_SPI_CON_REN		BIT(9)	/* Receive overflow error enable */
#define LTQ_SPI_CON_TEN		BIT(8)	/* Transmit overflow error enable */
#define LTQ_SPI_CON_LB		BIT(7)	/* Loopback control */
#define LTQ_SPI_CON_PO		BIT(6)	/* Clock polarity control */
#define LTQ_SPI_CON_PH		BIT(5)	/* Clock phase control */
#define LTQ_SPI_CON_HB		BIT(4)	/* Heading control */
#define LTQ_SPI_CON_RXOFF	BIT(1)	/* Switch receiver off */
#define LTQ_SPI_CON_TXOFF	BIT(0)	/* Switch transmitter off */

#define LTQ_SPI_STAT_RXBV_MASK	0x7
#define LTQ_SPI_STAT_RXBV_SHIFT	28
#define LTQ_SPI_STAT_BSY	BIT(13)	/* Busy flag */
#define LTQ_SPI_STAT_RUE	BIT(12)	/* Receive underflow error flag */
#define LTQ_SPI_STAT_TUE	BIT(11)	/* Transmit underflow error flag */
#define LTQ_SPI_STAT_AE		BIT(10)	/* Abort error flag */
#define LTQ_SPI_STAT_RE		BIT(9)	/* Receive error flag */
#define LTQ_SPI_STAT_TE		BIT(8)	/* Transmit error flag */
#define LTQ_SPI_STAT_MS		BIT(1)	/* Master/slave select bit */
#define LTQ_SPI_STAT_EN		BIT(0)	/* Enable bit */

#define LTQ_SPI_WHBSTATE_SETTUE	BIT(15)	/* Set transmit underflow error flag */
#define LTQ_SPI_WHBSTATE_SETAE	BIT(14)	/* Set abort error flag */
#define LTQ_SPI_WHBSTATE_SETRE	BIT(13)	/* Set receive error flag */
#define LTQ_SPI_WHBSTATE_SETTE	BIT(12)	/* Set transmit error flag */
#define LTQ_SPI_WHBSTATE_CLRTUE	BIT(11)	/* Clear transmit underflow error flag */
#define LTQ_SPI_WHBSTATE_CLRAE	BIT(10)	/* Clear abort error flag */
#define LTQ_SPI_WHBSTATE_CLRRE	BIT(9)	/* Clear receive error flag */
#define LTQ_SPI_WHBSTATE_CLRTE	BIT(8)	/* Clear transmit error flag */
#define LTQ_SPI_WHBSTATE_SETME	BIT(7)	/* Set mode error flag */
#define LTQ_SPI_WHBSTATE_CLRME	BIT(6)	/* Clear mode error flag */
#define LTQ_SPI_WHBSTATE_SETRUE	BIT(5)	/* Set receive underflow error flag */
#define LTQ_SPI_WHBSTATE_CLRRUE	BIT(4)	/* Clear receive underflow error flag */
#define LTQ_SPI_WHBSTATE_SETMS	BIT(3)	/* Set master select bit */
#define LTQ_SPI_WHBSTATE_CLRMS	BIT(2)	/* Clear master select bit */
#define LTQ_SPI_WHBSTATE_SETEN	BIT(1)	/* Set enable bit (operational mode) */
#define LTQ_SPI_WHBSTATE_CLREN	BIT(0)	/* Clear enable bit (config mode */
#define LTQ_SPI_WHBSTATE_CLR_ERRORS	0x0F50

#define LTQ_SPI_RXFCON_RXFITL_SHIFT	8	/* FIFO interrupt trigger level */
#define LTQ_SPI_RXFCON_RXFITL_MASK	0x3F
#define LTQ_SPI_RXFCON_RXFLU		BIT(1)	/* FIFO flush */
#define LTQ_SPI_RXFCON_RXFEN		BIT(0)	/* FIFO enable */

#define LTQ_SPI_TXFCON_TXFITL_SHIFT	8	/* FIFO interrupt trigger level */
#define LTQ_SPI_TXFCON_TXFITL_MASK	0x3F
#define LTQ_SPI_TXFCON_TXFLU		BIT(1)	/* FIFO flush */
#define LTQ_SPI_TXFCON_TXFEN		BIT(0)	/* FIFO enable */

#define LTQ_SPI_FSTAT_RXFFL_MASK	0x3f
#define LTQ_SPI_FSTAT_RXFFL_SHIFT	0
#define LTQ_SPI_FSTAT_TXFFL_MASK	0x3f
#define LTQ_SPI_FSTAT_TXFFL_SHIFT	8

#define LTQ_SPI_GPOCON_ISCSBN_SHIFT	8
#define LTQ_SPI_GPOCON_INVOUTN_SHIFT	0

#define LTQ_SPI_FGPO_SETOUTN_SHIFT	8
#define LTQ_SPI_FGPO_CLROUTN_SHIFT	0

#define LTQ_SPI_RXREQ_RXCNT_MASK	0xFFFF	/* Receive count value */
#define LTQ_SPI_RXCNT_TODO_MASK		0xFFFF	/* Recevie to-do value */

#define LTQ_SPI_IRNEN_F		BIT(3)	/* Frame end interrupt request */
#define LTQ_SPI_IRNEN_E		BIT(2)	/* Error end interrupt request */
#define LTQ_SPI_IRNEN_T		BIT(1)	/* Transmit end interrupt request */
#define LTQ_SPI_IRNEN_R		BIT(0)	/* Receive end interrupt request */
#define LTQ_SPI_IRNEN_ALL	0xF

/* Hard-wired GPIOs used by SPI controller */
#define LTQ_SPI_GPIO_DI 	(ltq_is_ase()?  8 : 16)
#define LTQ_SPI_GPIO_DO 	(ltq_is_ase()?  9 : 17)
#define LTQ_SPI_GPIO_CLK	(ltq_is_ase()? 10 : 18)

struct ltq_spi {
	struct spi_bitbang	bitbang;
	struct completion	done;
	spinlock_t		lock;

	struct device		*dev;
	void __iomem		*base;
	struct clk		*fpiclk;
	struct clk		*spiclk;

	int			status;
	int			irq[3];

	const u8		*tx;
	u8			*rx;
	u32			tx_cnt;
	u32			rx_cnt;
	u32			len;
	struct spi_transfer	*curr_transfer;

	u32 (*get_tx) (struct ltq_spi *);

	u16			txfs;
	u16			rxfs;
	unsigned		dma_support:1;
	unsigned		cfg_mode:1;

};

struct ltq_spi_controller_state {
	void (*cs_activate) (struct spi_device *);
	void (*cs_deactivate) (struct spi_device *);
};

struct ltq_spi_irq_map {
	char		*name;
	irq_handler_t	handler;
};

struct ltq_spi_cs_gpio_map {
	unsigned	gpio;
	unsigned	mux;
};

static inline struct ltq_spi *ltq_spi_to_hw(struct spi_device *spi)
{
	return spi_master_get_devdata(spi->master);
}

static inline u32 ltq_spi_reg_read(struct ltq_spi *hw, u32 reg)
{
	return ioread32be(hw->base + reg);
}

static inline void ltq_spi_reg_write(struct ltq_spi *hw, u32 val, u32 reg)
{
	iowrite32be(val, hw->base + reg);
}

static inline void ltq_spi_reg_setbit(struct ltq_spi *hw, u32 bits, u32 reg)
{
	u32 val;

	val = ltq_spi_reg_read(hw, reg);
	val |= bits;
	ltq_spi_reg_write(hw, val, reg);
}

static inline void ltq_spi_reg_clearbit(struct ltq_spi *hw, u32 bits, u32 reg)
{
	u32 val;

	val = ltq_spi_reg_read(hw, reg);
	val &= ~bits;
	ltq_spi_reg_write(hw, val, reg);
}

static void ltq_spi_hw_enable(struct ltq_spi *hw)
{
	u32 clc;

	/* Power-up mdule */
	clk_enable(hw->spiclk);

	/*
	 * Set clock divider for run mode to 1 to
	 * run at same frequency as FPI bus
	 */
	clc = (1 << LTQ_SPI_CLC_RMC_SHIFT);
	ltq_spi_reg_write(hw, clc, LTQ_SPI_CLC);
}

static void ltq_spi_hw_disable(struct ltq_spi *hw)
{
	/* Set clock divider to 0 and set module disable bit */
	ltq_spi_reg_write(hw, LTQ_SPI_CLC_DISS, LTQ_SPI_CLC);

	/* Power-down mdule */
	clk_disable(hw->spiclk);
}

static void ltq_spi_reset_fifos(struct ltq_spi *hw)
{
	u32 val;

	/*
	 * Enable and flush FIFOs. Set interrupt trigger level to
	 * half of FIFO count implemented in hardware.
	 */
	if (hw->txfs > 1) {
		val = hw->txfs << (LTQ_SPI_TXFCON_TXFITL_SHIFT - 1);
		val |= LTQ_SPI_TXFCON_TXFEN | LTQ_SPI_TXFCON_TXFLU;
		ltq_spi_reg_write(hw, val, LTQ_SPI_TXFCON);
	}

	if (hw->rxfs > 1) {
		val = hw->rxfs << (LTQ_SPI_RXFCON_RXFITL_SHIFT - 1);
		val |= LTQ_SPI_RXFCON_RXFEN | LTQ_SPI_RXFCON_RXFLU;
		ltq_spi_reg_write(hw, val, LTQ_SPI_RXFCON);
	}
}

static inline int ltq_spi_wait_ready(struct ltq_spi *hw)
{
	u32 stat;
	unsigned long timeout;

	timeout = jiffies + msecs_to_jiffies(200);

	do {
		stat = ltq_spi_reg_read(hw, LTQ_SPI_STAT);
		if (!(stat & LTQ_SPI_STAT_BSY))
			return 0;

		cond_resched();
	} while (!time_after_eq(jiffies, timeout));

	dev_err(hw->dev, "SPI wait ready timed out stat: %x\n", stat);

	return -ETIMEDOUT;
}

static void ltq_spi_config_mode_set(struct ltq_spi *hw)
{
	if (hw->cfg_mode)
		return;

	/*
	 * Putting the SPI module in config mode is only safe if no
	 * transfer is in progress as indicated by busy flag STATE.BSY.
	 */
	if (ltq_spi_wait_ready(hw)) {
		ltq_spi_reset_fifos(hw);
		hw->status = -ETIMEDOUT;
	}
	ltq_spi_reg_write(hw, LTQ_SPI_WHBSTATE_CLREN, LTQ_SPI_WHBSTATE);

	hw->cfg_mode = 1;
}

static void ltq_spi_run_mode_set(struct ltq_spi *hw)
{
	if (!hw->cfg_mode)
		return;

	ltq_spi_reg_write(hw, LTQ_SPI_WHBSTATE_SETEN, LTQ_SPI_WHBSTATE);

	hw->cfg_mode = 0;
}

static u32 ltq_spi_tx_word_u8(struct ltq_spi *hw)
{
	const u8 *tx = hw->tx;
	u32 data = *tx++;

	hw->tx_cnt++;
	hw->tx++;

	return data;
}

static u32 ltq_spi_tx_word_u16(struct ltq_spi *hw)
{
	const u16 *tx = (u16 *) hw->tx;
	u32 data = *tx++;

	hw->tx_cnt += 2;
	hw->tx += 2;

	return data;
}

static u32 ltq_spi_tx_word_u32(struct ltq_spi *hw)
{
	const u32 *tx = (u32 *) hw->tx;
	u32 data = *tx++;

	hw->tx_cnt += 4;
	hw->tx += 4;

	return data;
}

static void ltq_spi_bits_per_word_set(struct spi_device *spi)
{
	struct ltq_spi *hw = ltq_spi_to_hw(spi);
	u32 bm;
	u8 bits_per_word = spi->bits_per_word;

	/*
	 * Use either default value of SPI device or value
	 * from current transfer.
	 */
	if (hw->curr_transfer && hw->curr_transfer->bits_per_word)
		bits_per_word = hw->curr_transfer->bits_per_word;

	if (bits_per_word <= 8)
		hw->get_tx = ltq_spi_tx_word_u8;
	else if (bits_per_word <= 16)
		hw->get_tx = ltq_spi_tx_word_u16;
	else if (bits_per_word <= 32)
		hw->get_tx = ltq_spi_tx_word_u32;

	/* CON.BM value = bits_per_word - 1 */
	bm = (bits_per_word - 1) << LTQ_SPI_CON_BM_SHIFT;

	ltq_spi_reg_clearbit(hw, LTQ_SPI_CON_BM_MASK <<
			     LTQ_SPI_CON_BM_SHIFT, LTQ_SPI_CON);
	ltq_spi_reg_setbit(hw, bm, LTQ_SPI_CON);
}

static void ltq_spi_speed_set(struct spi_device *spi)
{
	struct ltq_spi *hw = ltq_spi_to_hw(spi);
	u32 br, max_speed_hz, spi_clk;
	u32 speed_hz = spi->max_speed_hz;

	/*
	 * Use either default value of SPI device or value
	 * from current transfer.
	 */
	if (hw->curr_transfer && hw->curr_transfer->speed_hz)
		speed_hz = hw->curr_transfer->speed_hz;

	/*
	 * SPI module clock is derived from FPI bus clock dependent on
	 * divider value in CLC.RMS which is always set to 1.
	 */
	spi_clk = clk_get_rate(hw->fpiclk);

	/*
	 * Maximum SPI clock frequency in master mode is half of
	 * SPI module clock frequency. Maximum reload value of
	 * baudrate generator BR is 2^16.
	 */
	max_speed_hz = spi_clk / 2;
	if (speed_hz >= max_speed_hz)
		br = 0;
	else
		br = (max_speed_hz / speed_hz) - 1;

	if (br > 0xFFFF)
		br = 0xFFFF;

	ltq_spi_reg_write(hw, br, LTQ_SPI_BRT);
}

static void ltq_spi_clockmode_set(struct spi_device *spi)
{
	struct ltq_spi *hw = ltq_spi_to_hw(spi);
	u32 con;

	con = ltq_spi_reg_read(hw, LTQ_SPI_CON);

	/*
	 * SPI mode mapping in CON register:
	 * Mode CPOL CPHA CON.PO CON.PH
	 *  0    0    0      0      1
	 *  1    0    1      0      0
	 *  2    1    0      1      1
	 *  3    1    1      1      0
	 */
	if (spi->mode & SPI_CPHA)
		con &= ~LTQ_SPI_CON_PH;
	else
		con |= LTQ_SPI_CON_PH;

	if (spi->mode & SPI_CPOL)
		con |= LTQ_SPI_CON_PO;
	else
		con &= ~LTQ_SPI_CON_PO;

	/* Set heading control */
	if (spi->mode & SPI_LSB_FIRST)
		con &= ~LTQ_SPI_CON_HB;
	else
		con |= LTQ_SPI_CON_HB;

	ltq_spi_reg_write(hw, con, LTQ_SPI_CON);
}

static void ltq_spi_xmit_set(struct ltq_spi *hw, struct spi_transfer *t)
{
	u32 con;

	con = ltq_spi_reg_read(hw, LTQ_SPI_CON);

	if (t) {
		if (t->tx_buf && t->rx_buf) {
			con &= ~(LTQ_SPI_CON_TXOFF | LTQ_SPI_CON_RXOFF);
		} else if (t->rx_buf) {
			con &= ~LTQ_SPI_CON_RXOFF;
			con |= LTQ_SPI_CON_TXOFF;
		} else if (t->tx_buf) {
			con &= ~LTQ_SPI_CON_TXOFF;
			con |= LTQ_SPI_CON_RXOFF;
		}
	} else
		con |= (LTQ_SPI_CON_TXOFF | LTQ_SPI_CON_RXOFF);

	ltq_spi_reg_write(hw, con, LTQ_SPI_CON);
}

static void ltq_spi_gpio_cs_activate(struct spi_device *spi)
{
	struct ltq_spi_controller_data *cdata = spi->controller_data;
	int val = spi->mode & SPI_CS_HIGH ? 1 : 0;

	gpio_set_value(cdata->gpio, val);
}

static void ltq_spi_gpio_cs_deactivate(struct spi_device *spi)
{
	struct ltq_spi_controller_data *cdata = spi->controller_data;
	int val = spi->mode & SPI_CS_HIGH ? 0 : 1;

	gpio_set_value(cdata->gpio, val);
}

static void ltq_spi_internal_cs_activate(struct spi_device *spi)
{
	struct ltq_spi *hw = ltq_spi_to_hw(spi);
	u32 fgpo;

	fgpo = (1 << (spi->chip_select + LTQ_SPI_FGPO_CLROUTN_SHIFT));
	ltq_spi_reg_setbit(hw, fgpo, LTQ_SPI_FGPO);
}

static void ltq_spi_internal_cs_deactivate(struct spi_device *spi)
{
	struct ltq_spi *hw = ltq_spi_to_hw(spi);
	u32 fgpo;

	fgpo = (1 << (spi->chip_select + LTQ_SPI_FGPO_SETOUTN_SHIFT));
	ltq_spi_reg_setbit(hw, fgpo, LTQ_SPI_FGPO);
}

static void ltq_spi_chipselect(struct spi_device *spi, int cs)
{
	struct ltq_spi *hw = ltq_spi_to_hw(spi);
	struct ltq_spi_controller_state *cstate = spi->controller_state;

	switch (cs) {
	case BITBANG_CS_ACTIVE:
		ltq_spi_bits_per_word_set(spi);
		ltq_spi_speed_set(spi);
		ltq_spi_clockmode_set(spi);
		ltq_spi_run_mode_set(hw);

		cstate->cs_activate(spi);
		break;

	case BITBANG_CS_INACTIVE:
		cstate->cs_deactivate(spi);

		ltq_spi_config_mode_set(hw);

		break;
	}
}

static int ltq_spi_setup_transfer(struct spi_device *spi,
				  struct spi_transfer *t)
{
	struct ltq_spi *hw = ltq_spi_to_hw(spi);
	u8 bits_per_word = spi->bits_per_word;

	hw->curr_transfer = t;

	if (t && t->bits_per_word)
		bits_per_word = t->bits_per_word;

	if (bits_per_word > 32)
		return -EINVAL;

	ltq_spi_config_mode_set(hw);

	return 0;
}

static const struct ltq_spi_cs_gpio_map ltq_spi_cs[] = {
	{ 15, 2 },
	{ 22, 2 },
	{ 13, 1 },
	{ 10, 1 },
	{  9, 1 },
	{ 11, 3 },
};

static const struct ltq_spi_cs_gpio_map ltq_spi_cs_ase[] = {
	{  7, 2 },
	{ 15, 1 },
	{ 14, 1 },
};

static int ltq_spi_setup(struct spi_device *spi)
{
	struct ltq_spi *hw = ltq_spi_to_hw(spi);
	struct ltq_spi_controller_data *cdata = spi->controller_data;
	struct ltq_spi_controller_state *cstate;
	u32 gpocon, fgpo;
	int ret;

	/* Set default word length to 8 if not set */
	if (!spi->bits_per_word)
		spi->bits_per_word = 8;

	if (spi->bits_per_word > 32)
		return -EINVAL;

	if (!spi->controller_state) {
		cstate = kzalloc(sizeof(struct ltq_spi_controller_state),
				 GFP_KERNEL);
		if (!cstate)
			return -ENOMEM;

		spi->controller_state = cstate;
	} else
		return 0;

	/*
	 * Up to six GPIOs can be connected to the SPI module
	 * via GPIO alternate function to control the chip select lines.
	 * For more flexibility in board layout this driver can also control
	 * the CS lines via GPIO API. If GPIOs should be used, board setup code
	 * have to register the SPI device with struct ltq_spi_controller_data
	 * attached.
	 */
	if (cdata && cdata->gpio) {
		ret = gpio_request(cdata->gpio, "spi-cs");
		if (ret)
			return -EBUSY;

		ret = spi->mode & SPI_CS_HIGH ? 0 : 1;
		gpio_direction_output(cdata->gpio, ret);

		cstate->cs_activate = ltq_spi_gpio_cs_activate;
		cstate->cs_deactivate = ltq_spi_gpio_cs_deactivate;
	} else {
		struct ltq_spi_cs_gpio_map *cs_map =
				ltq_is_ase() ? ltq_spi_cs_ase : ltq_spi_cs;
		ret = ltq_gpio_request(&spi->dev, cs_map[spi->chip_select].gpio,
				cs_map[spi->chip_select].mux,
				1, "spi-cs");
		if (ret)
			return -EBUSY;

		gpocon = (1 << (spi->chip_select +
				LTQ_SPI_GPOCON_ISCSBN_SHIFT));

		if (spi->mode & SPI_CS_HIGH)
			gpocon |= (1 << spi->chip_select);

		fgpo = (1 << (spi->chip_select + LTQ_SPI_FGPO_SETOUTN_SHIFT));

		ltq_spi_reg_setbit(hw, gpocon, LTQ_SPI_GPOCON);
		ltq_spi_reg_setbit(hw, fgpo, LTQ_SPI_FGPO);

		cstate->cs_activate = ltq_spi_internal_cs_activate;
		cstate->cs_deactivate = ltq_spi_internal_cs_deactivate;
	}

	return 0;
}

static void ltq_spi_cleanup(struct spi_device *spi)
{
	struct ltq_spi_controller_data *cdata = spi->controller_data;
	struct ltq_spi_controller_state *cstate = spi->controller_state;
	unsigned gpio;

	if (cdata && cdata->gpio)
		gpio = cdata->gpio;
	else
		gpio = ltq_is_ase() ? ltq_spi_cs_ase[spi->chip_select].gpio :
					 ltq_spi_cs[spi->chip_select].gpio;

	gpio_free(gpio);
	kfree(cstate);
}

static void ltq_spi_txfifo_write(struct ltq_spi *hw)
{
	u32 fstat, data;
	u16 fifo_space;

	/* Determine how much FIFOs are free for TX data */
	fstat = ltq_spi_reg_read(hw, LTQ_SPI_FSTAT);
	fifo_space = hw->txfs - ((fstat >> LTQ_SPI_FSTAT_TXFFL_SHIFT) &
					LTQ_SPI_FSTAT_TXFFL_MASK);

	if (!fifo_space)
		return;

	while (hw->tx_cnt < hw->len && fifo_space) {
		data = hw->get_tx(hw);
		ltq_spi_reg_write(hw, data, LTQ_SPI_TB);
		fifo_space--;
	}
}

static void ltq_spi_rxfifo_read(struct ltq_spi *hw)
{
	u32 fstat, data, *rx32;
	u16 fifo_fill;
	u8 rxbv, shift, *rx8;

	/* Determine how much FIFOs are filled with RX data */
	fstat = ltq_spi_reg_read(hw, LTQ_SPI_FSTAT);
	fifo_fill = ((fstat >> LTQ_SPI_FSTAT_RXFFL_SHIFT)
			& LTQ_SPI_FSTAT_RXFFL_MASK);

	if (!fifo_fill)
		return;

	/*
	 * The 32 bit FIFO is always used completely independent from the
	 * bits_per_word value. Thus four bytes have to be read at once
	 * per FIFO.
	 */
	rx32 = (u32 *) hw->rx;
	while (hw->len - hw->rx_cnt >= 4 && fifo_fill) {
		*rx32++ = ltq_spi_reg_read(hw, LTQ_SPI_RB);
		hw->rx_cnt += 4;
		hw->rx += 4;
		fifo_fill--;
	}

	/*
	 * If there are remaining bytes, read byte count from STAT.RXBV
	 * register and read the data byte-wise.
	 */
	while (fifo_fill && hw->rx_cnt < hw->len) {
		rxbv = (ltq_spi_reg_read(hw, LTQ_SPI_STAT) >>
			LTQ_SPI_STAT_RXBV_SHIFT) & LTQ_SPI_STAT_RXBV_MASK;
		data = ltq_spi_reg_read(hw, LTQ_SPI_RB);

		shift = (rxbv - 1) * 8;
		rx8 = hw->rx;

		while (rxbv) {
			*rx8++ = (data >> shift) & 0xFF;
			rxbv--;
			shift -= 8;
			hw->rx_cnt++;
			hw->rx++;
		}

		fifo_fill--;
	}
}

static void ltq_spi_rxreq_set(struct ltq_spi *hw)
{
	u32 rxreq, rxreq_max, rxtodo;

	rxtodo = ltq_spi_reg_read(hw, LTQ_SPI_RXCNT) & LTQ_SPI_RXCNT_TODO_MASK;

	/*
	 * In RX-only mode the serial clock is activated only after writing
	 * the expected amount of RX bytes into RXREQ register.
	 * To avoid receive overflows at high clocks it is better to request
	 * only the amount of bytes that fits into all FIFOs. This value
	 * depends on the FIFO size implemented in hardware.
	 */
	rxreq = hw->len - hw->rx_cnt;
	rxreq_max = hw->rxfs << 2;
	rxreq = min(rxreq_max, rxreq);

	if (!rxtodo && rxreq)
		ltq_spi_reg_write(hw, rxreq, LTQ_SPI_RXREQ);
}

static inline void ltq_spi_complete(struct ltq_spi *hw)
{
	complete(&hw->done);
}

irqreturn_t ltq_spi_tx_irq(int irq, void *data)
{
	struct ltq_spi *hw = data;
	unsigned long flags;
	int completed = 0;

	spin_lock_irqsave(&hw->lock, flags);

	if (hw->tx_cnt < hw->len)
		ltq_spi_txfifo_write(hw);

	if (hw->tx_cnt == hw->len)
		completed = 1;

	spin_unlock_irqrestore(&hw->lock, flags);

	if (completed)
		ltq_spi_complete(hw);

	return IRQ_HANDLED;
}

irqreturn_t ltq_spi_rx_irq(int irq, void *data)
{
	struct ltq_spi *hw = data;
	unsigned long flags;
	int completed = 0;

	spin_lock_irqsave(&hw->lock, flags);

	if (hw->rx_cnt < hw->len) {
		ltq_spi_rxfifo_read(hw);

		if (hw->tx && hw->tx_cnt < hw->len)
			ltq_spi_txfifo_write(hw);
	}

	if (hw->rx_cnt == hw->len)
		completed = 1;
	else if (!hw->tx)
		ltq_spi_rxreq_set(hw);

	spin_unlock_irqrestore(&hw->lock, flags);

	if (completed)
		ltq_spi_complete(hw);

	return IRQ_HANDLED;
}

irqreturn_t ltq_spi_err_irq(int irq, void *data)
{
	struct ltq_spi *hw = data;
	unsigned long flags;

	spin_lock_irqsave(&hw->lock, flags);

	/* Disable all interrupts */
	ltq_spi_reg_clearbit(hw, LTQ_SPI_IRNEN_ALL, LTQ_SPI_IRNEN);

	/* Clear all error flags */
	ltq_spi_reg_write(hw, LTQ_SPI_WHBSTATE_CLR_ERRORS, LTQ_SPI_WHBSTATE);

	/* Flush FIFOs */
	ltq_spi_reg_setbit(hw, LTQ_SPI_RXFCON_RXFLU, LTQ_SPI_RXFCON);
	ltq_spi_reg_setbit(hw, LTQ_SPI_TXFCON_TXFLU, LTQ_SPI_TXFCON);

	hw->status = -EIO;
	spin_unlock_irqrestore(&hw->lock, flags);

	ltq_spi_complete(hw);

	return IRQ_HANDLED;
}

static int ltq_spi_txrx_bufs(struct spi_device *spi, struct spi_transfer *t)
{
	struct ltq_spi *hw = ltq_spi_to_hw(spi);
	u32 irq_flags = 0;

	hw->tx = t->tx_buf;
	hw->rx = t->rx_buf;
	hw->len = t->len;
	hw->tx_cnt = 0;
	hw->rx_cnt = 0;
	hw->status = 0;
	INIT_COMPLETION(hw->done);

	ltq_spi_xmit_set(hw, t);

	/* Enable error interrupts */
	ltq_spi_reg_setbit(hw, LTQ_SPI_IRNEN_E, LTQ_SPI_IRNEN);

	if (hw->tx) {
		/* Initially fill TX FIFO with as much data as possible */
		ltq_spi_txfifo_write(hw);
		irq_flags |= LTQ_SPI_IRNEN_T;

		/* Always enable RX interrupt in Full Duplex mode */
		if (hw->rx)
			irq_flags |= LTQ_SPI_IRNEN_R;
	} else if (hw->rx) {
		/* Start RX clock */
		ltq_spi_rxreq_set(hw);

		/* Enable RX interrupt to receive data from RX FIFOs */
		irq_flags |= LTQ_SPI_IRNEN_R;
	}

	/* Enable TX or RX interrupts */
	ltq_spi_reg_setbit(hw, irq_flags, LTQ_SPI_IRNEN);
	wait_for_completion_interruptible(&hw->done);

	/* Disable all interrupts */
	ltq_spi_reg_clearbit(hw, LTQ_SPI_IRNEN_ALL, LTQ_SPI_IRNEN);

	/*
	 * Return length of current transfer for bitbang utility code if
	 * no errors occured during transmission.
	 */
	if (!hw->status)
		hw->status = hw->len;

	return hw->status;
}

static const struct ltq_spi_irq_map ltq_spi_irqs[] = {
	{ "spi_tx", ltq_spi_tx_irq },
	{ "spi_rx", ltq_spi_rx_irq },
	{ "spi_err", ltq_spi_err_irq },
};

static int __devinit
ltq_spi_probe(struct platform_device *pdev)
{
	struct spi_master *master;
	struct resource *r;
	struct ltq_spi *hw;
	struct ltq_spi_platform_data *pdata = pdev->dev.platform_data;
	int ret, i;
	u32 data, id;

	master = spi_alloc_master(&pdev->dev, sizeof(struct ltq_spi));
	if (!master) {
		dev_err(&pdev->dev, "spi_alloc_master\n");
		ret = -ENOMEM;
		goto err;
	}

	hw = spi_master_get_devdata(master);

	r = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (r == NULL) {
		dev_err(&pdev->dev, "platform_get_resource\n");
		ret = -ENOENT;
		goto err_master;
	}

	r = devm_request_mem_region(&pdev->dev, r->start, resource_size(r),
			pdev->name);
	if (!r) {
		dev_err(&pdev->dev, "devm_request_mem_region\n");
		ret = -ENXIO;
		goto err_master;
	}

	hw->base = devm_ioremap_nocache(&pdev->dev, r->start, resource_size(r));
	if (!hw->base) {
		dev_err(&pdev->dev, "devm_ioremap_nocache\n");
		ret = -ENXIO;
		goto err_master;
	}

	hw->fpiclk = clk_get_fpi();
	if (IS_ERR(hw->fpiclk)) {
		dev_err(&pdev->dev, "fpi clk\n");
		ret = PTR_ERR(hw->fpiclk);
		goto err_master;
	}

	hw->spiclk = clk_get(&pdev->dev, NULL);
	if (IS_ERR(hw->spiclk)) {
		dev_err(&pdev->dev, "spi clk\n");
		ret = PTR_ERR(hw->spiclk);
		goto err_master;
	}

	memset(hw->irq, 0, sizeof(hw->irq));
	for (i = 0; i < ARRAY_SIZE(ltq_spi_irqs); i++) {
		ret = platform_get_irq_byname(pdev, ltq_spi_irqs[i].name);
		if (0 > ret) {
			dev_err(&pdev->dev, "platform_get_irq_byname\n");
			goto err_irq;
		}

		hw->irq[i] = ret;
		ret = request_irq(hw->irq[i], ltq_spi_irqs[i].handler,
				  0, ltq_spi_irqs[i].name, hw);
		if (ret) {
			dev_err(&pdev->dev, "request_irq\n");
			goto err_irq;
		}
	}

	hw->bitbang.master = spi_master_get(master);
	hw->bitbang.chipselect = ltq_spi_chipselect;
	hw->bitbang.setup_transfer = ltq_spi_setup_transfer;
	hw->bitbang.txrx_bufs = ltq_spi_txrx_bufs;

	master->bus_num = pdev->id;
	master->num_chipselect = pdata->num_chipselect;
	master->setup = ltq_spi_setup;
	master->cleanup = ltq_spi_cleanup;

	hw->dev = &pdev->dev;
	init_completion(&hw->done);
	spin_lock_init(&hw->lock);

	/* Set GPIO alternate functions to SPI */
	ltq_gpio_request(&pdev->dev, LTQ_SPI_GPIO_DI, 2, 0, "spi-di");
	ltq_gpio_request(&pdev->dev, LTQ_SPI_GPIO_DO, 2, 1, "spi-do");
	ltq_gpio_request(&pdev->dev, LTQ_SPI_GPIO_CLK, 2, 1, "spi-clk");

	ltq_spi_hw_enable(hw);

	/* Read module capabilities */
	id = ltq_spi_reg_read(hw, LTQ_SPI_ID);
	hw->txfs = (id >> LTQ_SPI_ID_TXFS_SHIFT) & LTQ_SPI_ID_TXFS_MASK;
	hw->rxfs = (id >> LTQ_SPI_ID_TXFS_SHIFT) & LTQ_SPI_ID_TXFS_MASK;
	hw->dma_support = (id & LTQ_SPI_ID_CFG) ? 1 : 0;

	ltq_spi_config_mode_set(hw);

	/* Enable error checking, disable TX/RX, set idle value high */
	data = LTQ_SPI_CON_RUEN | LTQ_SPI_CON_AEN |
	    LTQ_SPI_CON_TEN | LTQ_SPI_CON_REN |
	    LTQ_SPI_CON_TXOFF | LTQ_SPI_CON_RXOFF | LTQ_SPI_CON_IDLE;
	ltq_spi_reg_write(hw, data, LTQ_SPI_CON);

	/* Enable master mode and clear error flags */
	ltq_spi_reg_write(hw, LTQ_SPI_WHBSTATE_SETMS |
			  LTQ_SPI_WHBSTATE_CLR_ERRORS, LTQ_SPI_WHBSTATE);

	/* Reset GPIO/CS registers */
	ltq_spi_reg_write(hw, 0x0, LTQ_SPI_GPOCON);
	ltq_spi_reg_write(hw, 0xFF00, LTQ_SPI_FGPO);

	/* Enable and flush FIFOs */
	ltq_spi_reset_fifos(hw);

	ret = spi_bitbang_start(&hw->bitbang);
	if (ret) {
		dev_err(&pdev->dev, "spi_bitbang_start\n");
		goto err_bitbang;
	}

	platform_set_drvdata(pdev, hw);

	pr_info("Lantiq SoC SPI controller rev %u (TXFS %u, RXFS %u, DMA %u)\n",
		id & LTQ_SPI_ID_REV_MASK, hw->txfs, hw->rxfs, hw->dma_support);

	return 0;

err_bitbang:
	ltq_spi_hw_disable(hw);

err_irq:
	clk_put(hw->fpiclk);

	for (; i > 0; i--)
		free_irq(hw->irq[i], hw);

err_master:
	spi_master_put(master);

err:
	return ret;
}

static int __devexit
ltq_spi_remove(struct platform_device *pdev)
{
	struct ltq_spi *hw = platform_get_drvdata(pdev);
	int ret, i;

	ret = spi_bitbang_stop(&hw->bitbang);
	if (ret)
		return ret;

	platform_set_drvdata(pdev, NULL);

	ltq_spi_config_mode_set(hw);
	ltq_spi_hw_disable(hw);

	for (i = 0; i < ARRAY_SIZE(hw->irq); i++)
		if (0 < hw->irq[i])
			free_irq(hw->irq[i], hw);

	gpio_free(LTQ_SPI_GPIO_DI);
	gpio_free(LTQ_SPI_GPIO_DO);
	gpio_free(LTQ_SPI_GPIO_CLK);

	clk_put(hw->fpiclk);
	spi_master_put(hw->bitbang.master);

	return 0;
}

static struct platform_driver ltq_spi_driver = {
	.probe = ltq_spi_probe,
	.remove = __devexit_p(ltq_spi_remove),
	.driver = {
		.name = "ltq_spi",
		.owner = THIS_MODULE,
		},
};

module_platform_driver(ltq_spi_driver);

MODULE_DESCRIPTION("Lantiq SoC SPI controller driver");
MODULE_AUTHOR("Daniel Schwierzeck <daniel.schwierzeck@googlemail.com>");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:ltq-spi");
