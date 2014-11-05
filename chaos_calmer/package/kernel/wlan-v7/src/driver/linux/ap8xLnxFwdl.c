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


/** include files **/
#ifdef SOC_W8363
    #ifdef WLMEM_DISABLED
        #ifdef V4FW /* V4FW => WLMEM=0 && AMPDU_SUPPORT_SBA=1 */
            #include "88W8363-V4.h"
        #else
            #include "88W8363-S.h"
        #endif
    #else
        #include "88W8363.h"
    #endif
#elif defined(SOC_W8366)
  	#include "88W8366.h"
#elif defined(SOC_W8364)
	#include "88W8364.h"
#elif defined(SOC_W8764)
    #ifdef DEFAULT_MFG_MODE
        #include "88W8764-mfg.h"
    #else
	    #include "88W8764.h"
    #endif
#else
    #ifdef CB8361
        #include "88W836X.h"
    #else
        #include "cb8350.h"
    #endif
#endif

#include "ap8xLnxVer.h"
#include "ap8xLnxFwdl.h"
#include "ap8xLnxRegs.h"
#include "ap8xLnxFwcmd.h"
#include "ap8xLnxIntf.h"
#include "wldebug.h"

#ifdef SOC_W8764
   /* no helper image for SC2 */
#elif defined(SOC_W8366)
#include "hlpimage_W8366.h" 
#elif defined(SOC_W8364)
#include "hlpimage_W8364.h" 
#else
#include "hlpimage.h"
#endif


#if defined (MFG_SUPPORT)
#include "wl_mib.h"
#include "wl_hal.h"
#endif



/* default settings */

/** external functions **/

/** external data **/

/** public data **/
/** private data **/

/** local definitions **/

#define FW_DOWNLOAD_BLOCK_SIZE                 256  
#define FW_CHECK_MSECS                           1  

#ifdef SOC_W8764
    #define FW_MAX_NUM_CHECKS                      0xffff /* why is this needed? */
#else
    #define FW_MAX_NUM_CHECKS                      200  
#endif

#define FW_LOAD_STA_FWRDY_SIGNATURE     0xf0f1f2f4
#define FW_LOAD_SOFTAP_FWRDY_SIGNATURE  0xf1f2f4a5


#define HostCmd_STA_MODE     0x5A
#define HostCmd_SOFTAP_MODE  0xA5

#define WL_SEC_SLEEP(NumSecs)       mdelay(NumSecs * 1000); 
#define WL_MSEC_SLEEP(NumMilliSecs) mdelay(NumMilliSecs);

/** internal functions **/
static void wltriggerPciCmd(struct net_device *);
static void wltriggerPciCmd_bootcode(struct net_device *);


/** public functions **/
int wlFwDownload(struct net_device *netdev)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	unsigned char *pFwImage = wlpptr->FwPointer;//&fmimage[0];
	unsigned int currIteration = FW_MAX_NUM_CHECKS;
	//unsigned short firmwareBlockSize = FW_DOWNLOAD_BLOCK_SIZE;
	unsigned int FwReadySignature = FW_LOAD_STA_FWRDY_SIGNATURE;
	unsigned int OpMode = HostCmd_STA_MODE;
	unsigned int downloadSuccessful = 1;
	unsigned int sizeFwDownloaded = 0;
	//unsigned int remainingFwBytes = 0;
	unsigned int intCode;
	//unsigned int sizeSend = 0;
	//unsigned int sizeGood = 0;
	//unsigned int i,	sizeBlock;
	//unsigned char useHelp = 0;
	//unsigned long dummy;
	unsigned long len;
	//unsigned short expectfield=0;
	//unsigned long loopcnt=0;

	WLDBG_ENTER(DBG_LEVEL_3);
	
#ifdef NO_FW_DOWNLOAD
    printk("AP8X: This version does not support host fwdl!!!\n");
    return SUCCESS;
#endif

	wlFwReset(netdev);

	//FW before jumping to boot rom, it will enable PCIe transaction retry, wait for boot code to stop it.
	WL_MSEC_SLEEP(FW_CHECK_MSECS); 

	writel(MACREG_A2HRIC_BIT_MASK, wlpptr->ioBase1 + MACREG_REG_A2H_INTERRUPT_CLEAR_SEL);
	writel(0x00,wlpptr->ioBase1+MACREG_REG_A2H_INTERRUPT_CAUSE);
	writel(0x00,wlpptr->ioBase1+MACREG_REG_A2H_INTERRUPT_MASK);
	writel(MACREG_A2HRIC_BIT_MASK, wlpptr->ioBase1 + MACREG_REG_A2H_INTERRUPT_STATUS_MASK);


	/** SC3 MFG FW no longer use this signature
    if (wlpptr->mfgEnable)	 
	{
		FwReadySignature = FW_LOAD_STA_FWRDY_SIGNATURE;
		OpMode = HostCmd_STA_MODE;
		printk("client mode\n");
	}
	else */
	{
		FwReadySignature = FW_LOAD_SOFTAP_FWRDY_SIGNATURE;
		OpMode = HostCmd_SOFTAP_MODE;
	}

        /* this routine interacts with SC2 bootrom to download firmware binary 
        to the device. After DMA'd to SC2, the firmware could be deflated to reside 
        on its respective blocks such as ITCM, DTCM, SQRAM, 
        (or even DDR, AFTER DDR is init'd before fw download */
		sizeFwDownloaded = 0;
		printk("fw download start 88\n");

        /* Disable PFU before FWDL */
        writel(0x100,wlpptr->ioBase1+0xE0E4);

        /* make sure SCRATCH2 C40 is clear, in case we are too quick */
        while (readl(wlpptr->ioBase1 + 0xc40) == 0);

		while (sizeFwDownloaded < wlpptr->FwSize)
		{			
			len = readl(wlpptr->ioBase1 + 0xc40);

			if(!len)
				break;

            /* this copies the next chunk of fw binary to be delivered */
			memcpy((char *)&wlpptr->pCmdBuf[0],(pFwImage+sizeFwDownloaded),len);

            currIteration = FW_MAX_NUM_CHECKS; /* this is arbitrary per your platform; we use 0xffff */
            /* this function writes pdata to c10, then write 2 to c18 */
			wltriggerPciCmd_bootcode(netdev);

            /* NOTE: the following back to back checks on C1C is time sensitive, hence  
            may need to be tweaked dependent on host processor. Time for SC2 to go from 
            the write of event 2 to C1C == 2 is ~1300 nSec. Hence the checkings on host
            has to consider how efficient your code can be to meet this timing, or you
            can alternatively tweak this routines to fit your platform */
            do {
				intCode = readl(wlpptr->ioBase1 + 0xc1c);
				if(intCode!=0)
                    break;
				currIteration--;

			}while (currIteration);

            do {
    			intCode = readl(wlpptr->ioBase1 + 0xc1c);
	    		if((intCode & MACREG_H2ARIC_BIT_DOOR_BELL) !=  MACREG_H2ARIC_BIT_DOOR_BELL)
                    break;
                currIteration--;
            }while (currIteration);

			if (currIteration == 0)
			{
                /* This limited loop check allows you to exit gracefully without locking up
                your entire system just because fw download failed */
                printk("Exhausted currIteration during fw download\n");
				downloadSuccessful = 0;
				wlFwReset(netdev);
				return FAIL;
			}

			sizeFwDownloaded += len;
        }
        printk("FwSize = %d downloaded Size = %d currIteration %d\n", 
            (int) wlpptr->FwSize, sizeFwDownloaded, currIteration);

		if (downloadSuccessful) 
		{
            /* Now firware is downloaded successfully, so this part is to check 
            whether fw can properly execute to an extent that write back signature 
            to indicate its readiness to the host. NOTE: if your downloaded fw crashes,
            this signature checking will fail. This part is similar as SC1 */

			writew(0x00, &wlpptr->pCmdBuf[1]);
			wltriggerPciCmd(netdev);
			currIteration = FW_MAX_NUM_CHECKS;
			do
			{
				currIteration--;
				writel(OpMode, wlpptr->ioBase1 + MACREG_REG_GEN_PTR);
				WL_MSEC_SLEEP(FW_CHECK_MSECS);
				intCode = readl(wlpptr->ioBase1 + MACREG_REG_INT_CODE);
                if (!(currIteration%0xff)) 
                    printk("%x;", intCode);
			} while ((currIteration) &&  (intCode != FwReadySignature));

			if (currIteration == 0)
			{
                printk("Exhausted currIteration waiting for fw signature; firmware seems failed to operate\n");

				downloadSuccessful = 0;
				wlFwReset(netdev);
				return TIMEOUT;
			}
		}
		
    printk("wlFwDownload complete\n");
	writel(0x00, wlpptr->ioBase1 + MACREG_REG_INT_CODE);
	WLDBG_EXIT(DBG_LEVEL_3);
	return SUCCESS;
}

/** private functions **/

static void wltriggerPciCmd(struct net_device *netdev)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);

	writel(wlpptr->wlpd_p->pPhysCmdBuf, wlpptr->ioBase1 + MACREG_REG_GEN_PTR);

	writel(0x00, wlpptr->ioBase1 + MACREG_REG_INT_CODE);

	writel(MACREG_H2ARIC_BIT_DOOR_BELL, 
		wlpptr->ioBase1 + MACREG_REG_H2A_INTERRUPT_EVENTS);
}


static void wltriggerPciCmd_bootcode(struct net_device *netdev)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);

    /* write location of data to c10 */
	writel(wlpptr->wlpd_p->pPhysCmdBuf, wlpptr->ioBase1 + MACREG_REG_GEN_PTR);

    /* write 2 to c18 */
	writel(MACREG_H2ARIC_BIT_DOOR_BELL, 
		wlpptr->ioBase1 + MACREG_REG_H2A_INTERRUPT_EVENTS);
}

static unsigned int getIntFwSize(int index)
{
	int sizeFw = 0;

	if (index == 0)
		sizeFw = sizeof(fmimage);

	return (sizeFw);
}
static unsigned char *getIntFwPointer(int index)
{
	unsigned char *imageFw = NULL;

	if (index == 0)
		imageFw = &fmimage[0];

	return (imageFw);
}
extern int LoadExternalFw(struct wlprivate *priv, char *filename);
#define EXTERNAL_FILE_NAME "88W8764-mfg.bin"

int wlPrepareFwFile(struct net_device *netdev)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
#ifdef DEFAULT_MFG_MODE
	if(LoadExternalFw(wlpptr, EXTERNAL_FILE_NAME))
	{
		wlpptr->mfgEnable = 1;
		wlpptr->mfgLoaded = 1;
		return SUCCESS;
	}
	wlpptr->mfgEnable = 1;
	wlpptr->mfgLoaded = 1;
    return FAIL; /* Load external failed. */
#else
	wlpptr->mfgEnable = 0;
	wlpptr->mfgLoaded = 0;
#endif

	wlpptr->FwPointer = getIntFwPointer(0);
	wlpptr->FwSize = getIntFwSize(0);
    return SUCCESS;
}


















