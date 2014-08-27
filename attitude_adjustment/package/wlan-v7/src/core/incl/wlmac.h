/** 
  * Copyright (C) 2008-2014, Marvell International Ltd. 
  * 
  * This software file (the "File") is distributed by Marvell International 
  * Ltd. under the terms of the GNU General Public License Version 2, June 1991 
  * (the "License").  You may use, redistribute and/or modify this File in 
  * accordance with the terms and conditions of the License, a copy of which 
  * is available by writing to the Free Software Foundation, Inc.,
  * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA or on the
  * worldwide web at http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt.
  *
  * THE FILE IS DISTRIBUTED AS-IS, WITHOUT WARRANTY OF ANY KIND, AND THE 
  * IMPLIED WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE 
  * ARE EXPRESSLY DISCLAIMED.  The License provides additional details about 
  * this warranty disclaimer.
  *
  */

#if !defined(_WLMAC_H_)
#define _WLMAC_H_ 

/*!
 * \file    wlmac.h
 * \brief   Definitions for 802.11 MAC unit.
 *
*/

/* Macro to compute MAC register addresses */
#ifdef AP_MAC_LINUX
#define MAC_REG_ADDR(offset) (offset)
#define MAC_REG_ADDR_PCI(offset) ((wlpptr->ioBase1+0xA000) + offset)

#define RX_TRAFFIC_CNT     MAC_REG_ADDR(0x0850) /* Accumulated Radio Traffic in bytes */
#define RX_TRAFFIC_ERR_CNT MAC_REG_ADDR(0x0854) /* Accumulated Radio FCS error in bytes */
#define TX_MODE            MAC_REG_ADDR(0x0500)
#define   WL_STA_MODE           (0x0)                    /* 1 - AP */
#define   WL_AP_MODE            (0x1)                    /* 1 - AP */
#define   WL_IBSS_MODE          (0x2)                    /* 1 - IBSS */

#define RX_BBU_RXRDY_CNT  MAC_REG_ADDR(0x0860)

#endif
#endif /* _WLMAC_H_ */
