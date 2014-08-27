/* ==========================================================================
 * $File: //dwh/usb_iip/dev/software/otg_ipmate/linux/drivers/dwc_otg_driver.h $
 * $Revision: 1.1.1.1 $
 * $Date: 2009-04-17 06:15:34 $
 * $Change: 510275 $
 *
 * Synopsys HS OTG Linux Software Driver and documentation (hereinafter,
 * "Software") is an Unsupported proprietary work of Synopsys, Inc. unless
 * otherwise expressly agreed to in writing between Synopsys and you.
 * 
 * The Software IS NOT an item of Licensed Software or Licensed Product under
 * any End User Software License Agreement or Agreement for Licensed Product
 * with Synopsys or any supplement thereto. You are permitted to use and
 * redistribute this Software in source and binary forms, with or without
 * modification, provided that redistributions of source code must retain this
 * notice. You may not view, use, disclose, copy or distribute this file or
 * any information contained herein except pursuant to this license grant from
 * Synopsys. If you do not agree with this notice, including the disclaimer
 * below, then you are not authorized to use the Software.
 * 
 * THIS SOFTWARE IS BEING DISTRIBUTED BY SYNOPSYS SOLELY ON AN "AS IS" BASIS
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE HEREBY DISCLAIMED. IN NO EVENT SHALL SYNOPSYS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 * ========================================================================== */

#if !defined(__DWC_OTG_DRIVER_H__)
#define __DWC_OTG_DRIVER_H__

/** @file
 * This file contains the interface to the Linux driver.
 */
#include "dwc_otg_cil.h"

/* Type declarations */
struct dwc_otg_pcd;
struct dwc_otg_hcd;

/**
 * This structure is a wrapper that encapsulates the driver components used to
 * manage a single DWC_otg controller.
 */
typedef struct dwc_otg_device
{
    /** Base address returned from ioremap() */
    void *base;
    
    /** Pointer to the core interface structure. */
    dwc_otg_core_if_t *core_if;

    /** Register offset for Diagnostic API.*/
    uint32_t reg_offset;

    /** Pointer to the PCD structure. */
    struct dwc_otg_pcd *pcd;

    /** Pointer to the HCD structure. */
    struct dwc_otg_hcd *hcd;

    /** Flag to indicate whether the common IRQ handler is installed. */
    uint8_t common_irq_installed;

    /** Interrupt request number. */
	unsigned int irq;

    /** Physical address of Control and Status registers, used by
     *  release_mem_region().
     */
	resource_size_t phys_addr;

    /** Length of memory region, used by release_mem_region(). */
	unsigned long base_len;
} dwc_otg_device_t;

//#define dev_dbg(fake, format, arg...) printk(KERN_CRIT __FILE__ ":%d: " format "\n" , __LINE__, ## arg)

#endif
