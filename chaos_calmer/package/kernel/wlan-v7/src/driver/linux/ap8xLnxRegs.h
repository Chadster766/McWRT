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

#ifndef AP8X_REGS_H_
#define AP8X_REGS_H_

#define EAGLE_NDIS_MAJOR_VERSION 0x5

#ifdef MRVL_WINXP_NDIS51
#define EAGLE_NDIS_MINOR_VERSION 0x1
#else
#define EAGLE_NDIS_MINOR_VERSION 0x0
#endif

#define EAGLE_DRIVER_VERSION ((EAGLE_NDIS_MAJOR_VERSION*0x100) + EAGLE_NDIS_MINOR_VERSION)

#define	VENDORDESCRIPTOR "Marvell W8350 802.11 NIC"

#define MRVL_PCI_VENDOR_ID                  0x11AB // VID
#define MRVL_8100_PCI_DEVICE_ID             0x2A02 // DID

#define MRVL_8100_PCI_REV_0                 0x00
#define MRVL_8100_PCI_REV_1                 0x01
#define MRVL_8100_PCI_REV_2                 0x02
#define MRVL_8100_PCI_REV_3                 0x03
#define MRVL_8100_PCI_REV_4                 0x04
#define MRVL_8100_PCI_REV_5                 0x05
#define MRVL_8100_PCI_REV_6                 0x06
#define MRVL_8100_PCI_REV_7                 0x07
#define MRVL_8100_PCI_REV_8                 0x08
#define MRVL_8100_PCI_REV_9                 0x09
#define MRVL_8100_PCI_REV_a                 0x0a
#define MRVL_8100_PCI_REV_b                 0x0b
#define MRVL_8100_PCI_REV_c                 0x0c
#define MRVL_8100_PCI_REV_d                 0x0d
#define MRVL_8100_PCI_REV_e                 0x0e
#define MRVL_8100_PCI_REV_f                 0x0f

#define MRVL_8100_PCI_VER_ID               0x00
#define MRVL_8100_CARDBUS_VER_ID           0x01


//          Map to 0x80000000 (Bus control) on BAR0
#define MACREG_REG_H2A_INTERRUPT_EVENTS     	0x00000C18 // (From host to ARM)
#define MACREG_REG_H2A_INTERRUPT_CAUSE      	0x00000C1C // (From host to ARM)
#define MACREG_REG_H2A_INTERRUPT_MASK       	0x00000C20 // (From host to ARM)
#define MACREG_REG_H2A_INTERRUPT_CLEAR_SEL      0x00000C24 // (From host to ARM)
#define MACREG_REG_H2A_INTERRUPT_STATUS_MASK	0x00000C28 // (From host to ARM)

#define MACREG_REG_A2H_INTERRUPT_EVENTS     	0x00000C2C // (From ARM to host)
#define MACREG_REG_A2H_INTERRUPT_CAUSE      	0x00000C30 // (From ARM to host)
#define MACREG_REG_A2H_INTERRUPT_MASK       	0x00000C34 // (From ARM to host)
#define MACREG_REG_A2H_INTERRUPT_CLEAR_SEL      0x00000C38 // (From ARM to host)
#define MACREG_REG_A2H_INTERRUPT_STATUS_MASK    0x00000C3C // (From ARM to host)


//  Map to 0x80000000 on BAR1
#define MACREG_REG_GEN_PTR                  0x00000C10
#define MACREG_REG_INT_CODE                 0x00000C14
#define MACREG_REG_SCRATCH                  0x00000C40
#define MACREG_REG_FW_PRESENT				0x0000BFFC


//	Bit definitio for MACREG_REG_A2H_INTERRUPT_CAUSE (A2HRIC)
#define MACREG_A2HRIC_BIT_TX_DONE           0x00000001 // bit 0
#define MACREG_A2HRIC_BIT_RX_RDY            0x00000002 // bit 1
#define MACREG_A2HRIC_BIT_OPC_DONE          0x00000004 // bit 2
#define MACREG_A2HRIC_BIT_MAC_EVENT         0x00000008 // bit 3
#define MACREG_A2HRIC_BIT_RX_PROBLEM        0x00000010 // bit 4

#define MACREG_A2HRIC_BIT_RADIO_OFF        	0x00000020 // bit 5
#define MACREG_A2HRIC_BIT_RADIO_ON        	0x00000040 // bit 6

#ifdef IEEE80211_DH
#define MACREG_A2HRIC_BIT_RADAR_DETECT      0x00000080 // bit 7
#endif //IEEE80211_DH

#define MACREG_A2HRIC_BIT_ICV_ERROR         0x00000100 // bit 8
#define MACREG_A2HRIC_BIT_WEAKIV_ERROR      0x00000200 // bit 9
#define MACREG_A2HRIC_BIT_QUEUE_EMPTY		(1<<10)
#define MACREG_A2HRIC_BIT_QUEUE_FULL		(1<<11)
#ifdef IEEE80211_DH
#define MACREG_A2HRIC_BIT_CHAN_SWITCH      (1<<12)
#endif //IEEE80211_DH
#define MACREG_A2HRIC_BIT_TX_WATCHDOG		(1<<13)
#define MACREG_A2HRIC_BA_WATCHDOG           (1<<14)
#define MACREG_A2HRIC_BIT_SSU_DONE          (1<<16)
#define MACREG_A2HRIC_CONSEC_TXFAIL     	(1<<17)		//15 taken by ISR_TXACK

#ifdef IEEE80211_DH
#define ISR_SRC_BITS        ((MACREG_A2HRIC_BIT_RX_RDY)   | \
                             (MACREG_A2HRIC_BIT_TX_DONE)  | \
                             (MACREG_A2HRIC_BIT_OPC_DONE) | \
                             (MACREG_A2HRIC_BIT_MAC_EVENT)| \
                             (MACREG_A2HRIC_BIT_WEAKIV_ERROR)| \
                             (MACREG_A2HRIC_BIT_ICV_ERROR)| \
                             (MACREG_A2HRIC_BIT_SSU_DONE) | \
                             (MACREG_A2HRIC_BIT_RADAR_DETECT)| \
                             (MACREG_A2HRIC_BIT_CHAN_SWITCH)| \
				 			 (MACREG_A2HRIC_BIT_TX_WATCHDOG)| \
                             (MACREG_A2HRIC_BIT_QUEUE_EMPTY)| \
                             (MACREG_A2HRIC_BA_WATCHDOG)	| \
                             (MACREG_A2HRIC_CONSEC_TXFAIL))  
                             
#else
#define ISR_SRC_BITS        ((MACREG_A2HRIC_BIT_RX_RDY)   | \
                             (MACREG_A2HRIC_BIT_TX_DONE)  | \
                             (MACREG_A2HRIC_BIT_OPC_DONE) | \
                             (MACREG_A2HRIC_BIT_MAC_EVENT)| \
                             (MACREG_A2HRIC_BIT_WEAKIV_ERROR)| \
                             (MACREG_A2HRIC_BIT_ICV_ERROR)| \
                             (MACREG_A2HRIC_BIT_SSU_DONE) | \
				 			 (MACREG_A2HRIC_BIT_TX_WATCHDOG)| \
                             (MACREG_A2HRIC_BIT_QUEUE_EMPTY)| \
                             (MACREG_A2HRIC_BA_WATCHDOG)	|\
                             (MACREG_A2HRIC_CONSEC_TXFAIL)) 

#endif //IEEE80211_DH

#define MACREG_A2HRIC_BIT_MASK      ISR_SRC_BITS & (~MACREG_A2HRIC_BIT_TX_DONE)     


//	Bit definitio for MACREG_REG_H2A_INTERRUPT_CAUSE (H2ARIC)
#define MACREG_H2ARIC_BIT_PPA_READY         0x00000001 // bit 0
#define MACREG_H2ARIC_BIT_DOOR_BELL         0x00000002 // bit 1
#define MACREG_H2ARIC_BIT_PS         		0x00000004 // bit 2
#define MACREG_H2ARIC_BIT_PSPOLL       		0x00000008 // bit 3
#define ISR_RESET           				(1<<15)
#define ISR_RESET_AP33                                  (1<<26)

// Power Save events
#define MACREG_PS_OFF						0x00000000
#define MACREG_PS_ON						0x00000001

//	INT code register event definition
#define MACREG_INT_CODE_TX_PPA_FREE         0x00000000 
#define MACREG_INT_CODE_TX_DMA_DONE         0x00000001
#define MACREG_INT_CODE_LINK_LOSE_W_SCAN    0x00000002
#define MACREG_INT_CODE_LINK_LOSE_NO_SCAN   0x00000003
#define MACREG_INT_CODE_LINK_SENSED         0x00000004
#define MACREG_INT_CODE_CMD_FINISHED        0x00000005
#define MACREG_INT_CODE_MIB_CHANGED         0x00000006 
#define MACREG_INT_CODE_INIT_DONE           0x00000007 
#define MACREG_INT_CODE_DEAUTHENTICATED     0x00000008 
#define MACREG_INT_CODE_DISASSOCIATED       0x00000009 

#endif /* AP8X_REGS_H_ */

