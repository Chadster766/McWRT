/*
*                Copyright 2002-2014, Marvell Semiconductor, Inc.
* This code contains confidential information of Marvell Semiconductor, Inc.
* No rights are granted herein under any patent, mask work right or copyright
* of Marvell or any third party.
* Marvell reserves the right at its sole discretion to request that this code
* be immediately returned to Marvell. This code is provided "as is".
* Marvell makes no warranties, express, implied or otherwise, regarding its
* accuracy, completeness or performance.
*/

#if !defined(_WLMAC_H_)
#define _WLMAC_H_ 

/*!
 * \file    wlmac.h
 * \brief   Definitions for 802.11 MAC unit.
 *
*/

/* Macro to compute MAC register addresses */
#define MAC_REG_ADDR(offset) (offset)
#define MAC_REG_ADDR_PCI(offset) ((wlpptr->ioBase1+0xA000) + offset)

#define RX_TRAFFIC_CNT     MAC_REG_ADDR(0x0850) /* Accumulated Radio Traffic in bytes */
#define RX_TRAFFIC_ERR_CNT MAC_REG_ADDR(0x0854) /* Accumulated Radio FCS error in bytes */
#define TX_MODE            MAC_REG_ADDR(0x0500)
#define   WL_STA_MODE           (0x0)                    /* 1 - AP */
#define   WL_AP_MODE            (0x1)                    /* 1 - AP */
#define   WL_IBSS_MODE          (0x2)                    /* 1 - IBSS */

#define RX_BBU_RXRDY_CNT  MAC_REG_ADDR(0x0860)
#endif /* _WLMAC_H_ */
