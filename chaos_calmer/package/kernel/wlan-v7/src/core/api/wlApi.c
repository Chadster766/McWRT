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


#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/net.h>
#include <linux/netdevice.h>
#include <linux/wireless.h>
#include <linux/pci.h>
#include <linux/delay.h>
#include <linux/wait.h>

#include <net/iw_handler.h>
#include <asm/processor.h>
#include <asm/uaccess.h>

#include "wltypes.h"
#include "wl.h"
#include "wlApi.h"
#include "ap8xLnxFwcmd.h"


#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,0,6)
DEFINE_SEMAPHORE(api_sem);
#else
DECLARE_MUTEX(api_sem);
#endif
DECLARE_WAIT_QUEUE_HEAD(api_wq);


/* PRIVATE FUNCTIONS DECLARATION
 */
 
//static void apctl_callback_fun(void *info);


/* PUBLIC FUNCTIONS DEFINITION
 */
#define MAX_WAIT_FW_COMPLETE_ITERATIONS 10000
BOOLEAN apctl(APICMDBUF *apicmdbuf)
{
	struct wlprivate *priv ;
	BOOLEAN result;
	UINT16 org_cmd;
	UINT16 cmdCode;
	priv = (struct wlprivate *)apicmdbuf->cmdBody;
//	if (! priv->fw_cmd_ready)
//		return FALSE;

	if (down_interruptible(&api_sem))
		return FALSE;

//	priv->cur_api = (void *) apicmdbuf;

	org_cmd = le16_to_cpu(apicmdbuf->cmdCode);
	switch (apicmdbuf->cmdCode)
		{
		case APCMD_BSS_START:
//			cmdCode= HostCmd_CMD_802_11_RADIO_CONTROL;
			//PrepareAndSendCommand(priv, cmdCode, HostCmd_ACT_GEN_SET, apctl_callback_fun);
			break;
		case APCMD_SET_RF_CHANNEL:
			cmdCode= HostCmd_CMD_SET_RF_CHANNEL;
//			priv->apcfg_p->mib_ap.CurrRFChan = ((APICMDBUF_SET_RF_CHANNEL*)apicmdbuf)->curChanNum;
			//PrepareAndSendCommand(priv, cmdCode, HostCmd_ACT_GEN_SET, apctl_callback_fun);
			break;
		case APCMD_GET_RF_CHANNEL:
			cmdCode= HostCmd_CMD_SET_RF_CHANNEL;
			//PrepareAndSendCommand(priv, cmdCode, HostCmd_ACT_GEN_GET, apctl_callback_fun);
//			((APICMDBUF_SET_RF_CHANNEL*)apicmdbuf)->curChanNum =priv->apcfg_p->mib_ap.CurrRFChan ;
		default:
			break;
		}
	//interruptible_sleep_on(&api_wq);
	result = TRUE;

	up(&api_sem);
//	PRINTK("command %s\n", result?"SUCCESS":"FAIL");
	return result;
}


/* PRIVATE FUNCTIONS DEFINITION
 */







