/*
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; version 2 of the License
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
 *
 *   based on Ralink SDK3.3
 *   Copyright (C) 2009 John Crispin <blogic@openwrt.org>
 */

#ifndef RAMIPS_ETH_H
#define RAMIPS_ETH_H

#include <linux/mii.h>
#include <linux/interrupt.h>
#include <linux/netdevice.h>
#include <linux/dma-mapping.h>

#define NUM_RX_DESC     256
#define NUM_TX_DESC     256

#define RAMIPS_DELAY_EN_INT		0x80
#define RAMIPS_DELAY_MAX_INT		0x04
#define RAMIPS_DELAY_MAX_TOUT		0x04
#define RAMIPS_DELAY_CHAN		(((RAMIPS_DELAY_EN_INT | RAMIPS_DELAY_MAX_INT) << 8) | RAMIPS_DELAY_MAX_TOUT)
#define RAMIPS_DELAY_INIT		((RAMIPS_DELAY_CHAN << 16) | RAMIPS_DELAY_CHAN)
#define RAMIPS_PSE_FQFC_CFG_INIT	0x80504000

/* interrupt bits */
#define RAMIPS_CNT_PPE_AF		BIT(31)
#define RAMIPS_CNT_GDM_AF		BIT(29)
#define RAMIPS_PSE_P2_FC		BIT(26)
#define RAMIPS_PSE_BUF_DROP		BIT(24)
#define RAMIPS_GDM_OTHER_DROP		BIT(23)
#define RAMIPS_PSE_P1_FC		BIT(22)
#define RAMIPS_PSE_P0_FC		BIT(21)
#define RAMIPS_PSE_FQ_EMPTY		BIT(20)
#define RAMIPS_GE1_STA_CHG		BIT(18)
#define RAMIPS_TX_COHERENT		BIT(17)
#define RAMIPS_RX_COHERENT		BIT(16)
#define RAMIPS_TX_DONE_INT3		BIT(11)
#define RAMIPS_TX_DONE_INT2		BIT(10)
#define RAMIPS_TX_DONE_INT1		BIT(9)
#define RAMIPS_TX_DONE_INT0		BIT(8)
#define RAMIPS_RX_DONE_INT0		BIT(2)
#define RAMIPS_TX_DLY_INT		BIT(1)
#define RAMIPS_RX_DLY_INT		BIT(0)

#define RT5350_RX_DLY_INT		BIT(30)
#define RT5350_TX_DLY_INT		BIT(28)

/* registers */
#define RAMIPS_FE_OFFSET		0x0000
#define RAMIPS_GDMA_OFFSET		0x0020
#define RAMIPS_PSE_OFFSET		0x0040
#define RAMIPS_GDMA2_OFFSET		0x0060
#define RAMIPS_CDMA_OFFSET		0x0080
#define RAMIPS_PDMA_OFFSET		0x0100
#define RAMIPS_PPE_OFFSET		0x0200
#define RAMIPS_CMTABLE_OFFSET		0x0400
#define RAMIPS_POLICYTABLE_OFFSET	0x1000

#define RT5350_PDMA_OFFSET		0x0800
#define RT5350_SDM_OFFSET		0x0c00

#define RAMIPS_MDIO_ACCESS		(RAMIPS_FE_OFFSET + 0x00)
#define RAMIPS_MDIO_CFG			(RAMIPS_FE_OFFSET + 0x04)
#define RAMIPS_FE_GLO_CFG		(RAMIPS_FE_OFFSET + 0x08)
#define RAMIPS_FE_RST_GL		(RAMIPS_FE_OFFSET + 0x0C)
#define RAMIPS_FE_INT_STATUS		(RAMIPS_FE_OFFSET + 0x10)
#define RAMIPS_FE_INT_ENABLE		(RAMIPS_FE_OFFSET + 0x14)
#define RAMIPS_MDIO_CFG2		(RAMIPS_FE_OFFSET + 0x18)
#define RAMIPS_FOC_TS_T			(RAMIPS_FE_OFFSET + 0x1C)

#define	RAMIPS_GDMA1_FWD_CFG		(RAMIPS_GDMA_OFFSET + 0x00)
#define RAMIPS_GDMA1_SCH_CFG		(RAMIPS_GDMA_OFFSET + 0x04)
#define RAMIPS_GDMA1_SHPR_CFG		(RAMIPS_GDMA_OFFSET + 0x08)
#define RAMIPS_GDMA1_MAC_ADRL		(RAMIPS_GDMA_OFFSET + 0x0C)
#define RAMIPS_GDMA1_MAC_ADRH		(RAMIPS_GDMA_OFFSET + 0x10)

#define	RAMIPS_GDMA2_FWD_CFG		(RAMIPS_GDMA2_OFFSET + 0x00)
#define RAMIPS_GDMA2_SCH_CFG		(RAMIPS_GDMA2_OFFSET + 0x04)
#define RAMIPS_GDMA2_SHPR_CFG		(RAMIPS_GDMA2_OFFSET + 0x08)
#define RAMIPS_GDMA2_MAC_ADRL		(RAMIPS_GDMA2_OFFSET + 0x0C)
#define RAMIPS_GDMA2_MAC_ADRH		(RAMIPS_GDMA2_OFFSET + 0x10)

#define RAMIPS_PSE_FQ_CFG		(RAMIPS_PSE_OFFSET + 0x00)
#define RAMIPS_CDMA_FC_CFG		(RAMIPS_PSE_OFFSET + 0x04)
#define RAMIPS_GDMA1_FC_CFG		(RAMIPS_PSE_OFFSET + 0x08)
#define RAMIPS_GDMA2_FC_CFG		(RAMIPS_PSE_OFFSET + 0x0C)

#define RAMIPS_CDMA_CSG_CFG		(RAMIPS_CDMA_OFFSET + 0x00)
#define RAMIPS_CDMA_SCH_CFG		(RAMIPS_CDMA_OFFSET + 0x04)

#define RT5350_TX_BASE_PTR0		(RT5350_PDMA_OFFSET + 0x00)
#define RT5350_TX_MAX_CNT0		(RT5350_PDMA_OFFSET + 0x04)
#define RT5350_TX_CTX_IDX0		(RT5350_PDMA_OFFSET + 0x08)
#define RT5350_TX_DTX_IDX0		(RT5350_PDMA_OFFSET + 0x0C)
#define RT5350_TX_BASE_PTR1		(RT5350_PDMA_OFFSET + 0x10)
#define RT5350_TX_MAX_CNT1		(RT5350_PDMA_OFFSET + 0x14)
#define RT5350_TX_CTX_IDX1		(RT5350_PDMA_OFFSET + 0x18)
#define RT5350_TX_DTX_IDX1		(RT5350_PDMA_OFFSET + 0x1C)
#define RT5350_TX_BASE_PTR2		(RT5350_PDMA_OFFSET + 0x20)
#define RT5350_TX_MAX_CNT2		(RT5350_PDMA_OFFSET + 0x24)
#define RT5350_TX_CTX_IDX2		(RT5350_PDMA_OFFSET + 0x28)
#define RT5350_TX_DTX_IDX2		(RT5350_PDMA_OFFSET + 0x2C)
#define RT5350_TX_BASE_PTR3		(RT5350_PDMA_OFFSET + 0x30)
#define RT5350_TX_MAX_CNT3		(RT5350_PDMA_OFFSET + 0x34)
#define RT5350_TX_CTX_IDX3		(RT5350_PDMA_OFFSET + 0x38)
#define RT5350_TX_DTX_IDX3		(RT5350_PDMA_OFFSET + 0x3C)
#define RT5350_RX_BASE_PTR0		(RT5350_PDMA_OFFSET + 0x100)
#define RT5350_RX_MAX_CNT0		(RT5350_PDMA_OFFSET + 0x104)
#define RT5350_RX_CALC_IDX0		(RT5350_PDMA_OFFSET + 0x108)
#define RT5350_RX_DRX_IDX0		(RT5350_PDMA_OFFSET + 0x10C)
#define RT5350_RX_BASE_PTR1		(RT5350_PDMA_OFFSET + 0x110)
#define RT5350_RX_MAX_CNT1		(RT5350_PDMA_OFFSET + 0x114)
#define RT5350_RX_CALC_IDX1		(RT5350_PDMA_OFFSET + 0x118)
#define RT5350_RX_DRX_IDX1		(RT5350_PDMA_OFFSET + 0x11C)
#define RT5350_PDMA_GLO_CFG		(RT5350_PDMA_OFFSET + 0x204)
#define RT5350_PDMA_RST_CFG		(RT5350_PDMA_OFFSET + 0x208)
#define RT5350_DLY_INT_CFG		(RT5350_PDMA_OFFSET + 0x20c)
#define RT5350_FE_INT_STATUS		(RT5350_PDMA_OFFSET + 0x220)
#define RT5350_FE_INT_ENABLE		(RT5350_PDMA_OFFSET + 0x228)
#define RT5350_PDMA_SCH_CFG		(RT5350_PDMA_OFFSET + 0x280)


#define RAMIPS_PDMA_GLO_CFG		(RAMIPS_PDMA_OFFSET + 0x00)
#define RAMIPS_PDMA_RST_CFG		(RAMIPS_PDMA_OFFSET + 0x04)
#define RAMIPS_PDMA_SCH_CFG		(RAMIPS_PDMA_OFFSET + 0x08)
#define RAMIPS_DLY_INT_CFG		(RAMIPS_PDMA_OFFSET + 0x0C)
#define RAMIPS_TX_BASE_PTR0		(RAMIPS_PDMA_OFFSET + 0x10)
#define RAMIPS_TX_MAX_CNT0		(RAMIPS_PDMA_OFFSET + 0x14)
#define RAMIPS_TX_CTX_IDX0		(RAMIPS_PDMA_OFFSET + 0x18)
#define RAMIPS_TX_DTX_IDX0		(RAMIPS_PDMA_OFFSET + 0x1C)
#define RAMIPS_TX_BASE_PTR1		(RAMIPS_PDMA_OFFSET + 0x20)
#define RAMIPS_TX_MAX_CNT1		(RAMIPS_PDMA_OFFSET + 0x24)
#define RAMIPS_TX_CTX_IDX1		(RAMIPS_PDMA_OFFSET + 0x28)
#define RAMIPS_TX_DTX_IDX1		(RAMIPS_PDMA_OFFSET + 0x2C)
#define RAMIPS_RX_BASE_PTR0		(RAMIPS_PDMA_OFFSET + 0x30)
#define RAMIPS_RX_MAX_CNT0		(RAMIPS_PDMA_OFFSET + 0x34)
#define RAMIPS_RX_CALC_IDX0		(RAMIPS_PDMA_OFFSET + 0x38)
#define RAMIPS_RX_DRX_IDX0		(RAMIPS_PDMA_OFFSET + 0x3C)
#define RAMIPS_TX_BASE_PTR2		(RAMIPS_PDMA_OFFSET + 0x40)
#define RAMIPS_TX_MAX_CNT2		(RAMIPS_PDMA_OFFSET + 0x44)
#define RAMIPS_TX_CTX_IDX2		(RAMIPS_PDMA_OFFSET + 0x48)
#define RAMIPS_TX_DTX_IDX2		(RAMIPS_PDMA_OFFSET + 0x4C)
#define RAMIPS_TX_BASE_PTR3		(RAMIPS_PDMA_OFFSET + 0x50)
#define RAMIPS_TX_MAX_CNT3		(RAMIPS_PDMA_OFFSET + 0x54)
#define RAMIPS_TX_CTX_IDX3		(RAMIPS_PDMA_OFFSET + 0x58)
#define RAMIPS_TX_DTX_IDX3		(RAMIPS_PDMA_OFFSET + 0x5C)
#define RAMIPS_RX_BASE_PTR1		(RAMIPS_PDMA_OFFSET + 0x60)
#define RAMIPS_RX_MAX_CNT1		(RAMIPS_PDMA_OFFSET + 0x64)
#define RAMIPS_RX_CALC_IDX1		(RAMIPS_PDMA_OFFSET + 0x68)
#define RAMIPS_RX_DRX_IDX1		(RAMIPS_PDMA_OFFSET + 0x6C)

#define RT5350_SDM_CFG			(RT5350_SDM_OFFSET + 0x00)  //Switch DMA configuration
#define RT5350_SDM_RRING		(RT5350_SDM_OFFSET + 0x04)  //Switch DMA Rx Ring
#define RT5350_SDM_TRING		(RT5350_SDM_OFFSET + 0x08)  //Switch DMA Tx Ring
#define RT5350_SDM_MAC_ADRL		(RT5350_SDM_OFFSET + 0x0C)  //Switch MAC address LSB
#define RT5350_SDM_MAC_ADRH		(RT5350_SDM_OFFSET + 0x10)  //Switch MAC Address MSB
#define RT5350_SDM_TPCNT		(RT5350_SDM_OFFSET + 0x100) //Switch DMA Tx packet count
#define RT5350_SDM_TBCNT		(RT5350_SDM_OFFSET + 0x104) //Switch DMA Tx byte count
#define RT5350_SDM_RPCNT		(RT5350_SDM_OFFSET + 0x108) //Switch DMA rx packet count
#define RT5350_SDM_RBCNT		(RT5350_SDM_OFFSET + 0x10C) //Switch DMA rx byte count
#define RT5350_SDM_CS_ERR		(RT5350_SDM_OFFSET + 0x110) //Switch DMA rx checksum error count

#define RT5350_SDM_ICS_EN		BIT(16)
#define RT5350_SDM_TCS_EN		BIT(17)
#define RT5350_SDM_UCS_EN		BIT(18)


/* MDIO_CFG register bits */
#define RAMIPS_MDIO_CFG_AUTO_POLL_EN	BIT(29)
#define RAMIPS_MDIO_CFG_GP1_BP_EN	BIT(16)
#define RAMIPS_MDIO_CFG_GP1_FRC_EN	BIT(15)
#define RAMIPS_MDIO_CFG_GP1_SPEED_10	(0 << 13)
#define RAMIPS_MDIO_CFG_GP1_SPEED_100	(1 << 13)
#define RAMIPS_MDIO_CFG_GP1_SPEED_1000	(2 << 13)
#define RAMIPS_MDIO_CFG_GP1_DUPLEX	BIT(12)
#define RAMIPS_MDIO_CFG_GP1_FC_TX	BIT(11)
#define RAMIPS_MDIO_CFG_GP1_FC_RX	BIT(10)
#define RAMIPS_MDIO_CFG_GP1_LNK_DWN	BIT(9)
#define RAMIPS_MDIO_CFG_GP1_AN_FAIL	BIT(8)
#define RAMIPS_MDIO_CFG_MDC_CLK_DIV_1	(0 << 6)
#define RAMIPS_MDIO_CFG_MDC_CLK_DIV_2	(1 << 6)
#define RAMIPS_MDIO_CFG_MDC_CLK_DIV_4	(2 << 6)
#define RAMIPS_MDIO_CFG_MDC_CLK_DIV_8	(3 << 6)
#define RAMIPS_MDIO_CFG_TURBO_MII_FREQ	BIT(5)
#define RAMIPS_MDIO_CFG_TURBO_MII_MODE	BIT(4)
#define RAMIPS_MDIO_CFG_RX_CLK_SKEW_0	(0 << 2)
#define RAMIPS_MDIO_CFG_RX_CLK_SKEW_200	(1 << 2)
#define RAMIPS_MDIO_CFG_RX_CLK_SKEW_400	(2 << 2)
#define RAMIPS_MDIO_CFG_RX_CLK_SKEW_INV	(3 << 2)
#define RAMIPS_MDIO_CFG_TX_CLK_SKEW_0	0
#define RAMIPS_MDIO_CFG_TX_CLK_SKEW_200	1
#define RAMIPS_MDIO_CFG_TX_CLK_SKEW_400	2
#define RAMIPS_MDIO_CFG_TX_CLK_SKEW_INV	3

/* uni-cast port */
#define RAMIPS_GDM1_ICS_EN		BIT(22)
#define RAMIPS_GDM1_TCS_EN		BIT(21)
#define RAMIPS_GDM1_UCS_EN		BIT(20)
#define RAMIPS_GDM1_JMB_EN		BIT(19)
#define RAMIPS_GDM1_STRPCRC		BIT(16)
#define RAMIPS_GDM1_UFRC_P_CPU		(0 << 12)
#define RAMIPS_GDM1_UFRC_P_GDMA1	(1 << 12)
#define RAMIPS_GDM1_UFRC_P_PPE		(6 << 12)

/* checksums */
#define RAMIPS_ICS_GEN_EN		BIT(2)
#define RAMIPS_UCS_GEN_EN		BIT(1)
#define RAMIPS_TCS_GEN_EN		BIT(0)

/* dma ring */
#define RAMIPS_PST_DRX_IDX0		BIT(16)
#define RAMIPS_PST_DTX_IDX3		BIT(3)
#define RAMIPS_PST_DTX_IDX2		BIT(2)
#define RAMIPS_PST_DTX_IDX1		BIT(1)
#define RAMIPS_PST_DTX_IDX0		BIT(0)

#define RAMIPS_TX_WB_DDONE		BIT(6)
#define RAMIPS_RX_DMA_BUSY		BIT(3)
#define RAMIPS_TX_DMA_BUSY		BIT(1)
#define RAMIPS_RX_DMA_EN		BIT(2)
#define RAMIPS_TX_DMA_EN		BIT(0)

#define RAMIPS_PDMA_SIZE_4DWORDS	(0 << 4)
#define RAMIPS_PDMA_SIZE_8DWORDS	(1 << 4)
#define RAMIPS_PDMA_SIZE_16DWORDS	(2 << 4)

#define RAMIPS_US_CYC_CNT_MASK		0xff
#define RAMIPS_US_CYC_CNT_SHIFT		0x8
#define RAMIPS_US_CYC_CNT_DIVISOR	1000000

#define RX_DMA_PLEN0(_x)		(((_x) >> 16) & 0x3fff)
#define RX_DMA_LSO			BIT(30)
#define RX_DMA_DONE			BIT(31)

struct ramips_rx_dma {
	unsigned int rxd1;
	unsigned int rxd2;
	unsigned int rxd3;
	unsigned int rxd4;
} __packed __aligned(4);

#define TX_DMA_PLEN0_MASK		((0x3fff) << 16)
#define TX_DMA_PLEN0(_x)		(((_x) & 0x3fff) << 16)
#define TX_DMA_LSO			BIT(30)
#define TX_DMA_DONE			BIT(31)
#define TX_DMA_QN(_x)			((_x) << 16)
#define TX_DMA_PN(_x)			((_x) << 24)
#define TX_DMA_QN_MASK			TX_DMA_QN(0x7)
#define TX_DMA_PN_MASK			TX_DMA_PN(0x7)

struct ramips_tx_dma {
	unsigned int txd1;
	unsigned int txd2;
	unsigned int txd3;
	unsigned int txd4;
} __packed __aligned(4);

struct raeth_tx_info {
	struct ramips_tx_dma	*tx_desc;
	struct sk_buff		*tx_skb;
};

struct raeth_rx_info {
	struct ramips_rx_dma	*rx_desc;
	struct sk_buff		*rx_skb;
	dma_addr_t		rx_dma;
	unsigned int		pad;
};

struct raeth_int_stats {
	unsigned long		rx_delayed;
	unsigned long		tx_delayed;
	unsigned long		rx_done0;
	unsigned long		tx_done0;
	unsigned long		tx_done1;
	unsigned long		tx_done2;
	unsigned long		tx_done3;
	unsigned long		rx_coherent;
	unsigned long		tx_coherent;

	unsigned long		pse_fq_empty;
	unsigned long		pse_p0_fc;
	unsigned long		pse_p1_fc;
	unsigned long		pse_p2_fc;
	unsigned long		pse_buf_drop;

	unsigned long		total;
};

struct raeth_debug {
	struct dentry		*debugfs_dir;

	struct raeth_int_stats	int_stats;
};

struct raeth_priv
{
	struct raeth_rx_info	*rx_info;
	dma_addr_t		rx_desc_dma;
	struct tasklet_struct	rx_tasklet;
	struct ramips_rx_dma	*rx;

	struct raeth_tx_info	*tx_info;
	dma_addr_t		tx_desc_dma;
	struct tasklet_struct	tx_housekeeping_tasklet;
	struct ramips_tx_dma	*tx;

	unsigned int		skb_free_idx;

	spinlock_t		page_lock;
	struct net_device	*netdev;
	struct device		*parent;
	struct ramips_eth_platform_data *plat;

	int			link;
	int			speed;
	int			duplex;
	int			tx_fc;
	int			rx_fc;

	struct mii_bus		*mii_bus;
	int			mii_irq[PHY_MAX_ADDR];
	struct phy_device	*phy_dev;
	spinlock_t		phy_lock;

#ifdef CONFIG_NET_RAMIPS_DEBUG_FS
	struct raeth_debug	debug;
#endif
};

#ifdef CONFIG_NET_RAMIPS_DEBUG_FS
int raeth_debugfs_root_init(void);
void raeth_debugfs_root_exit(void);
int raeth_debugfs_init(struct raeth_priv *re);
void raeth_debugfs_exit(struct raeth_priv *re);
void raeth_debugfs_update_int_stats(struct raeth_priv *re, u32 status);
#else
static inline int raeth_debugfs_root_init(void) { return 0; }
static inline void raeth_debugfs_root_exit(void) {}
static inline int raeth_debugfs_init(struct raeth_priv *re) { return 0; }
static inline void raeth_debugfs_exit(struct raeth_priv *re) {}
static inline void raeth_debugfs_update_int_stats(struct raeth_priv *re,
						  u32 status) {}
#endif /* CONFIG_NET_RAMIPS_DEBUG_FS */

#endif /* RAMIPS_ETH_H */
