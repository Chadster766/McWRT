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
#include "ap8xLnxRegs.h"
#include "ap8xLnxFwcmd.h"
#include "ap8xLnxXmit.h"
#include "ap8xLnxIntf.h"
#include "ap8xLnxVer.h"
#include "bcngen.h"
#include "wds.h"
#include "keyMgmt_if.h"

#ifdef MRVL_WAPI 
#include "ap8xLnxIoctl.h"
#endif
#include "macMgmtMlme.h"

/** local definitions **/
#define MAX_WAIT_FW_COMPLETE_ITERATIONS 10000
#define WEP_KEY_40_BIT_LEN                 0x0005 // 40 bit
#define WEP_KEY_104_BIT_LEN                0x000D // 104 bit

#define MAX_NUM_STA_ENCR_KEY_ENTRIES 128

#define MWL_SPIN_LOCK(X) SPIN_LOCK_IRQSAVE(X, flags)
#define MWL_SPIN_UNLOCK(X)	SPIN_UNLOCK_IRQRESTORE(X, flags)

/* default settings */

/** external functions **/

/** external data **/
extern UINT8 dfs_test_mode;

/** internal functions **/
#ifdef MRVL_DFS
static int DecideDFSOperation(struct net_device *netdev, BOOLEAN bChannelChanged,UINT8 currDFSState,UINT8 newDFSState, MIB_802DOT11 *mib);
#endif
static int wlexecuteCommand(struct net_device *, unsigned short);
static void wlsendCommand(struct net_device *);
static int wlwaitForComplete(struct net_device *, u_int16_t);
#ifdef WL_DEBUG
static char *wlgetCmdString(u_int16_t cmd);
static char *wlgetCmdResultString(u_int16_t result);
static char *wlgetDrvName(struct net_device *netdev);
#endif
static int wlFwSetMaxTxPwr(struct net_device *netdev);
static int wlFwSetAdaptMode(struct net_device *netdev);
static int wlFwSetCSAdaptMode(struct net_device *netdev);
static int wlFwSetNProt(struct net_device *netdev, UINT32 mode);	
static int wlFwSetOptimizationLevel(struct net_device *netdev, UINT8 mode);
static int wlFwGetPwrCalTable(struct net_device *netdev);
static int wlFwGetRegionCode(struct net_device *netdev);
static int wlFwSetRifs(struct net_device *netdev, UINT8 QNum);
static int wlFwSetHTStbc(struct net_device *netdev, UINT32 mode);
extern int wlFwSetCDD(struct net_device *netdev, UINT32 cdd_mode);
static int wlFwSetBFType( struct net_device *netdev, UINT32 mode);
#ifdef QUEUE_STATS
int wlCheckBa(struct net_device *netdev, UINT8 *addr);
#endif
int wlFwSetBWSignalType( struct net_device *netdev, UINT32 mode);

/** public data **/

/** private data **/
//static u_int32_t numStaEncrKeyEntries = 0;
/** public functions **/

int wlRegBB(struct net_device *netdev, UINT8 flag, UINT32 reg, UINT32 *val);
int wlFwSetNProtOpMode(struct net_device *netdev, UINT8 mode);

void wlFwCmdComplete(struct net_device *netdev)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
#ifdef WL_DEBUG
	u_int16_t cmd = ENDIAN_SWAP16(((FWCmdHdr*)wlpptr->pCmdBuf)->Cmd) & 0x7fff;
#endif
	u_int16_t result =ENDIAN_SWAP16(((FWCmdHdr*)wlpptr->pCmdBuf)->Result);

	if (result != HostCmd_RESULT_OK)
	{
		WLDBG_INFO(DBG_LEVEL_0,  "%s: FW cmd 0x%04x=%s failed: 0x%04x=%s\n",
			wlgetDrvName(netdev), cmd, wlgetCmdString(cmd),
			result, wlgetCmdResultString(result));
	}
}

int wlFwGetHwSpecs(struct net_device *netdev)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	HostCmd_DS_GET_HW_SPEC *pCmd = (HostCmd_DS_GET_HW_SPEC *)&wlpptr->pCmdBuf[0];
	unsigned long flags;
	int i;
	WLDBG_ENTER(DBG_LEVEL_0);

	MWL_SPIN_LOCK(&wlpptr->wlpd_p->locks.fwLock);
#ifdef SOC_W8764
	printk("wlFwGetHwSpecs pCmd = %x \n", (unsigned int) pCmd);
#endif
	memset(pCmd, 0x00, sizeof(HostCmd_DS_GET_HW_SPEC));
	memset(&pCmd->PermanentAddr[0], 0xff, ETH_ALEN);
	pCmd->CmdHdr.Cmd      = ENDIAN_SWAP16(HostCmd_CMD_GET_HW_SPEC);
	pCmd->CmdHdr.Length   = ENDIAN_SWAP16(sizeof(HostCmd_DS_GET_HW_SPEC));
	pCmd->ulFwAwakeCookie = ENDIAN_SWAP32((unsigned int)wlpptr->wlpd_p->pPhysCmdBuf+2048);

	WLDBG_DUMP_DATA(DBG_LEVEL_0, (void *) pCmd, sizeof(HostCmd_DS_GET_HW_SPEC));
#ifdef SOC_W8764
	while (wlexecuteCommand(netdev, HostCmd_CMD_GET_HW_SPEC))
	{
		printk( "failed execution");
		mdelay(1000);
		printk(" Repeat wlFwGetHwSpecs = %x \n", (unsigned int) pCmd);
		//MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
		//return FAIL;
	}
#else
	if (wlexecuteCommand(netdev, HostCmd_CMD_GET_HW_SPEC))
	{
		WLDBG_EXIT_INFO(DBG_LEVEL_0, "failed execution");
		MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
		return FAIL;
	}
#endif

	memcpy(&wlpptr->hwData.macAddr[0],pCmd->PermanentAddr,ETH_ALEN);
	wlpptr->wlpd_p->descData[0].wcbBase       = ENDIAN_SWAP32(pCmd->WcbBase0)   & 0x0000ffff;
#if NUM_OF_DESCRIPTOR_DATA >3
	for (i = 1; i < TOTAL_TX_QUEUES; i++)
		wlpptr->wlpd_p->descData[i].wcbBase       = ENDIAN_SWAP32(pCmd->WcbBase[i-1])   & 0x0000ffff;
#endif
	wlpptr->wlpd_p->descData[0].rxDescRead    = ENDIAN_SWAP32(pCmd->RxPdRdPtr)  & 0x0000ffff;
	wlpptr->wlpd_p->descData[0].rxDescWrite   = ENDIAN_SWAP32(pCmd->RxPdWrPtr)  & 0x0000ffff;
	wlpptr->hwData.regionCode      = ENDIAN_SWAP16(pCmd->RegionCode) & 0x00ff;
	//	domainSetDomain(wlpptr->wlpd_p->hwData.regionCode);
	wlpptr->hwData.fwReleaseNumber = ENDIAN_SWAP32(pCmd->FWReleaseNumber);
	wlpptr->hwData.maxNumTXdesc    = ENDIAN_SWAP16(pCmd->NumOfWCB);
	wlpptr->hwData.maxNumMCaddr    = ENDIAN_SWAP16(pCmd->NumOfMCastAddr);
	wlpptr->hwData.numAntennas     = ENDIAN_SWAP16(pCmd->NumberOfAntenna);
	wlpptr->hwData.hwVersion       = pCmd->Version;
	wlpptr->hwData.hostInterface   = pCmd->HostIf;

	WLDBG_EXIT_INFO(DBG_LEVEL_0, 
		"region code is %i (0x%x), HW version is %i (0x%x)",
		wlpptr->hwData.regionCode, wlpptr->hwData.regionCode,
		wlpptr->hwData.hwVersion, wlpptr->hwData.hwVersion);

	MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
	return SUCCESS;
}
int wlFwSetHwSpecs(struct net_device *netdev)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	HostCmd_DS_SET_HW_SPEC *pCmd = (HostCmd_DS_SET_HW_SPEC *)&wlpptr->pCmdBuf[0];
	unsigned long flags;
	int i;
	WLDBG_ENTER(DBG_LEVEL_1);

    /* Info for SOC team's debugging */
    printk("wlFwSetHwSpecs ...\n");
    printk("  -->pPhysTxRing[0] = %x\n",wlpptr->wlpd_p->descData[0].pPhysTxRing);
    printk("  -->pPhysTxRing[1] = %x\n",wlpptr->wlpd_p->descData[1].pPhysTxRing);
    printk("  -->pPhysTxRing[2] = %x\n",wlpptr->wlpd_p->descData[2].pPhysTxRing);
    printk("  -->pPhysTxRing[3] = %x\n",wlpptr->wlpd_p->descData[3].pPhysTxRing);
    printk("  -->pPhysRxRing    = %x\n",wlpptr->wlpd_p->descData[0].pPhysRxRing);
    printk("  -->numtxq %d wcbperq %d totalrxwcb %d \n",NUM_OF_DESCRIPTOR_DATA,MAX_NUM_TX_DESC,MAX_NUM_RX_DESC);

	MWL_SPIN_LOCK(&wlpptr->wlpd_p->locks.fwLock);
	memset(pCmd, 0x00, sizeof(HostCmd_DS_SET_HW_SPEC));
	pCmd->CmdHdr.Cmd	  = ENDIAN_SWAP16(HostCmd_CMD_SET_HW_SPEC);
	pCmd->CmdHdr.Length   = ENDIAN_SWAP16(sizeof(HostCmd_DS_SET_HW_SPEC));

	pCmd->WcbBase[0]	 = ENDIAN_SWAP32(wlpptr->wlpd_p->descData[0].pPhysTxRing);//ENDIAN_SWAP32(wlpptr->descData[0].wcbBase)	& 0x0000ffff;
#if NUM_OF_DESCRIPTOR_DATA >3
	for (i = 1; i < TOTAL_TX_QUEUES; i++)
		pCmd->WcbBase[i]	 = ENDIAN_SWAP32(wlpptr->wlpd_p->descData[i].pPhysTxRing);//ENDIAN_SWAP32(	wlpptr->descData[1].wcbBase )	& 0x0000ffff;
#endif
	pCmd->TxWcbNumPerQueue = ENDIAN_SWAP32(MAX_NUM_TX_DESC);
	pCmd->NumTxQueues = ENDIAN_SWAP32(NUM_OF_DESCRIPTOR_DATA);
	pCmd->TotalRxWcb = ENDIAN_SWAP32(MAX_NUM_RX_DESC);
	pCmd->RxPdWrPtr = ENDIAN_SWAP32(wlpptr->wlpd_p->descData[0].pPhysRxRing);
#if	defined(CLIENTONLY) || !defined(MBSS)
	pCmd->disablembss = 1;
#else
	pCmd->disablembss = 0;
#endif

#if NUMOFAPS == 1
	pCmd->disablembss = 1;
#endif

#ifdef SOC_W8363
	// force to check hwVersion to decide to enable mbss on FW.
	if(pCmd->disablembss == 0 && wlpptr->hwData.hwVersion < 5 )
	{
		printk(KERN_WARNING "HW version %d prohibits enabling mbss feature!!\n", wlpptr->hwData.hwVersion );
		pCmd->disablembss = 1;
	}
#endif
	if (wlexecuteCommand(netdev, HostCmd_CMD_SET_HW_SPEC))
	{
		WLDBG_EXIT_INFO(DBG_LEVEL_1, "failed execution");
		MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
		return FAIL;
	}

	MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
	return SUCCESS;
}
#ifdef SOC_W8764
extern UINT32 dispRxPacket;
#endif
int wlFwGetHwStats(struct net_device *netdev, char *page)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	char *p = page;
	int len =0;
	HostCmd_DS_802_11_GET_STAT *pCmd = (HostCmd_DS_802_11_GET_STAT *)&wlpptr->pCmdBuf[0];

	unsigned long flags; 

	WLDBG_ENTER(DBG_LEVEL_0);

	MWL_SPIN_LOCK(&wlpptr->wlpd_p->locks.fwLock);
	memset(pCmd, 0x00, sizeof(HostCmd_DS_802_11_GET_STAT));
	pCmd->CmdHdr.Cmd      = ENDIAN_SWAP16(HostCmd_CMD_802_11_GET_STAT);
	pCmd->CmdHdr.Length   = ENDIAN_SWAP16(sizeof(HostCmd_DS_802_11_GET_STAT));

#ifdef SOC_W8764
	dispRxPacket = (dispRxPacket+1) & 0x01;
#endif

	WLDBG_DUMP_DATA(DBG_LEVEL_0, (void *) pCmd, sizeof(HostCmd_DS_802_11_GET_STAT));
	if (wlexecuteCommand(netdev, HostCmd_CMD_802_11_GET_STAT))
	{
		WLDBG_EXIT_INFO(DBG_LEVEL_0, "failed execution");
		MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
		return FAIL;
	}
	if(p)
	{
		p += sprintf(p,"TxRetrySuccesses.................%10u\n", ENDIAN_SWAP32((int)pCmd->TxRetrySuccesses));
		p += sprintf(p,"TxMultipleRetrySuccesses.........%10u\n", ENDIAN_SWAP32((int)pCmd->TxMultipleRetrySuccesses));
		p += sprintf(p,"TxFailures.......................%10u\n", ENDIAN_SWAP32((int)pCmd->TxFailures));
		p += sprintf(p,"RTSSuccesses.....................%10u\n", ENDIAN_SWAP32((int)pCmd->RTSSuccesses));
		p += sprintf(p,"RTSFailures......................%10u\n", ENDIAN_SWAP32((int)pCmd->RTSFailures));
		p += sprintf(p,"AckFailures......................%10u\n", ENDIAN_SWAP32((int)pCmd->AckFailures));
		p += sprintf(p,"RxDuplicateFrames................%10u\n", ENDIAN_SWAP32((int)pCmd->RxDuplicateFrames));
		p += sprintf(p,"RxFCSErrors......................%10u\n", ENDIAN_SWAP32((int)pCmd->RxFCSErrors));
		p += sprintf(p,"TxWatchDogTimeouts...............%10u\n", ENDIAN_SWAP32((int)pCmd->TxWatchDogTimeouts));
		p += sprintf(p,"RxOverflows......................%10u\n", ENDIAN_SWAP32((int)pCmd->RxOverflows));
		p += sprintf(p,"RxFragErrors.....................%10u\n", ENDIAN_SWAP32((int)pCmd->RxFragErrors));
		p += sprintf(p,"RxMemErrors......................%10u\n", ENDIAN_SWAP32((int)pCmd->RxMemErrors));
		p += sprintf(p,"PointerErrors....................%10u\n", ENDIAN_SWAP32((int)pCmd->PointerErrors));
		p += sprintf(p,"TxUnderflows.....................%10u\n", ENDIAN_SWAP32((int)pCmd->TxUnderflows));
		p += sprintf(p,"TxDone...........................%10u\n", ENDIAN_SWAP32((int)pCmd->TxDone));
		p += sprintf(p,"TxDoneBufTryPut..................%10u\n", ENDIAN_SWAP32((int)pCmd->TxDoneBufTryPut));
		p += sprintf(p,"TxDoneBufPut.....................%10u\n", ENDIAN_SWAP32((int)pCmd->TxDoneBufPut));
		p += sprintf(p,"Wait4TxBuf.......................%10u\n", ENDIAN_SWAP32((int)pCmd->Wait4TxBuf));
		p += sprintf(p,"TxAttempts.......................%10u\n", ENDIAN_SWAP32((int)pCmd->TxAttempts));
		p += sprintf(p,"TxSuccesses......................%10u\n", ENDIAN_SWAP32((int)pCmd->TxSuccesses));
		p += sprintf(p,"TxFragments......................%10u\n", ENDIAN_SWAP32((int)pCmd->TxFragments));
		p += sprintf(p,"TxMulticasts.....................%10u\n", ENDIAN_SWAP32((int)pCmd->TxMulticasts));
		p += sprintf(p,"RxNonCtlPkts.....................%10u\n", ENDIAN_SWAP32((int)pCmd->RxNonCtlPkts));
		p += sprintf(p,"RxMulticasts.....................%10u\n", ENDIAN_SWAP32((int)pCmd->RxMulticasts));
		p += sprintf(p,"RxUndecryptableFrames............%10u\n", ENDIAN_SWAP32((int)pCmd->RxUndecryptableFrames));
		p += sprintf(p,"RxICVErrors......................%10u\n", ENDIAN_SWAP32((int)pCmd->RxICVErrors));
		p += sprintf(p,"RxExcludedFrames.................%10u\n", ENDIAN_SWAP32((int)pCmd->RxExcludedFrames));
		/* new from Aug'2012 */	
		p += sprintf(p,"RxWeakIVCount....................%10u\n", ENDIAN_SWAP32((int)pCmd->RxWeakIVCount));
		p += sprintf(p,"RxUnicasts.......................%10u\n", ENDIAN_SWAP32((int)pCmd->RxUnicasts));
		p += sprintf(p,"RxBytes..........................%10u\n", ENDIAN_SWAP32((int)pCmd->RxBytes));
		p += sprintf(p,"RxErrors.........................%10u\n", ENDIAN_SWAP32((int)pCmd->RxErrors));
		p += sprintf(p,"RxRTSCount.......................%10u\n", ENDIAN_SWAP32((int)pCmd->RxRTSCount));
		p += sprintf(p,"TxCTSCount.......................%10u\n", ENDIAN_SWAP32((int)pCmd->TxCTSCount));
#ifdef MRVL_WAPI		
		p += sprintf(p,"RxWAPIPNErrors...................%10u\n", ENDIAN_SWAP32((int)pCmd->RxWAPIPNErrors));
		p += sprintf(p,"RxWAPIMICErrors..................%10u\n", ENDIAN_SWAP32((int)pCmd->RxWAPIMICErrors));
		p += sprintf(p,"RxWAPINoKeyErrors................%10u\n", ENDIAN_SWAP32((int)pCmd->RxWAPINoKeyErrors));
		p += sprintf(p,"TxWAPINoKeyErrors................%10u\n", ENDIAN_SWAP32((int)pCmd->TxWAPINoKeyErrors));
#endif
		len = (p - page);
	}else
	{
		printk("TxRetrySuccesses.................%10u\n", ENDIAN_SWAP32((int)pCmd->TxRetrySuccesses));
		printk("TxMultipleRetrySuccesses.........%10u\n", ENDIAN_SWAP32((int)pCmd->TxMultipleRetrySuccesses));
		printk("TxFailures.......................%10u\n", ENDIAN_SWAP32((int)pCmd->TxFailures));
		printk("RTSSuccesses.....................%10u\n", ENDIAN_SWAP32((int)pCmd->RTSSuccesses));
		printk("RTSFailures......................%10u\n", ENDIAN_SWAP32((int)pCmd->RTSFailures));
		printk("AckFailures......................%10u\n", ENDIAN_SWAP32((int)pCmd->AckFailures));
		printk("RxDuplicateFrames................%10u\n", ENDIAN_SWAP32((int)pCmd->RxDuplicateFrames));
		printk("RxFCSErrors......................%10u\n", ENDIAN_SWAP32((int)pCmd->RxFCSErrors));
		printk("TxWatchDogTimeouts...............%10u\n", ENDIAN_SWAP32((int)pCmd->TxWatchDogTimeouts));
		printk("RxOverflows......................%10u\n", ENDIAN_SWAP32((int)pCmd->RxOverflows));
		printk("RxFragErrors.....................%10u\n", ENDIAN_SWAP32((int)pCmd->RxFragErrors));
		printk("RxMemErrors......................%10u\n", ENDIAN_SWAP32((int)pCmd->RxMemErrors));
		printk("PointerErrors....................%10u\n", ENDIAN_SWAP32((int)pCmd->PointerErrors));
		printk("TxUnderflows.....................%10u\n", ENDIAN_SWAP32((int)pCmd->TxUnderflows));
		printk("TxDone...........................%10u\n", ENDIAN_SWAP32((int)pCmd->TxDone));
		printk("TxDoneBufTryPut..................%10u\n", ENDIAN_SWAP32((int)pCmd->TxDoneBufTryPut));
		printk("TxDoneBufPut.....................%10u\n", ENDIAN_SWAP32((int)pCmd->TxDoneBufPut));
		printk("Wait4TxBuf.......................%10u\n", ENDIAN_SWAP32((int)pCmd->Wait4TxBuf));
		printk("TxAttempts.......................%10u\n", ENDIAN_SWAP32((int)pCmd->TxAttempts));
		printk("TxSuccesses......................%10u\n", ENDIAN_SWAP32((int)pCmd->TxSuccesses));
		printk("TxFragments......................%10u\n", ENDIAN_SWAP32((int)pCmd->TxFragments));
		printk("TxMulticasts.....................%10u\n", ENDIAN_SWAP32((int)pCmd->TxMulticasts));
		printk("RxNonCtlPkts.....................%10u\n", ENDIAN_SWAP32((int)pCmd->RxNonCtlPkts));
		printk("RxMulticasts.....................%10u\n", ENDIAN_SWAP32((int)pCmd->RxMulticasts));
		printk("RxUndecryptableFrames............%10u\n", ENDIAN_SWAP32((int)pCmd->RxUndecryptableFrames));
		printk("RxICVErrors......................%10u\n", ENDIAN_SWAP32((int)pCmd->RxICVErrors));
		printk("RxExcludedFrames.................%10u\n", ENDIAN_SWAP32((int)pCmd->RxExcludedFrames));
		/* new from Aug'2012 */	
		printk("RxWeakIVCount....................%10u\n", ENDIAN_SWAP32((int)pCmd->RxWeakIVCount));
		printk("RxUnicasts.......................%10u\n", ENDIAN_SWAP32((int)pCmd->RxUnicasts));
		printk("RxBytes..........................%10u\n", ENDIAN_SWAP32((int)pCmd->RxBytes));
		printk("RxErrors.........................%10u\n", ENDIAN_SWAP32((int)pCmd->RxErrors));
		printk("RxRTSCount.......................%10u\n", ENDIAN_SWAP32((int)pCmd->RxRTSCount));
		printk("TxCTSCount.......................%10u\n", ENDIAN_SWAP32((int)pCmd->TxCTSCount));
#ifdef MRVL_WAPI		
		printk("RxWAPIPNErrors...................%10u\n", ENDIAN_SWAP32((int)pCmd->RxWAPIPNErrors));
		printk("RxWAPIMICErrors..................%10u\n", ENDIAN_SWAP32((int)pCmd->RxWAPIMICErrors));
		printk("RxWAPINoKeyErrors................%10u\n", ENDIAN_SWAP32((int)pCmd->RxWAPINoKeyErrors));
		printk("TxWAPINoKeyErrors................%10u\n", ENDIAN_SWAP32((int)pCmd->TxWAPINoKeyErrors));
#endif
	}
	MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
	return len;
}
static const char *
ntoa(const uint8_t *mac)
{
	static char addr[3*6+2];
	snprintf(addr, sizeof(addr), "%02x:%02x:%02x:%02x:%02x:%02x",
		mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
	return addr;
}
int wlFwGetAddrtable(struct net_device *netdev)
{
	const struct macaddr {
		unsigned char	MacAddressInDB[10][6];	//data base entries
		unsigned char LegacyMacAddr[6];	//MAC will ACK this address
		unsigned char MBSSMacAddr[21][6];	//MAC will ACK these addresses
	} *p;
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	int i =0;
	HostCmd_DS_802_11_GET_STAT *pCmd = (HostCmd_DS_802_11_GET_STAT *)&wlpptr->pCmdBuf[0];

	unsigned long flags; 

	WLDBG_ENTER(DBG_LEVEL_0);

	MWL_SPIN_LOCK(&wlpptr->wlpd_p->locks.fwLock);
	memset(pCmd, 0x00, sizeof(FWCmdHdr)+sizeof(const struct macaddr));
	pCmd->CmdHdr.Cmd	  = ENDIAN_SWAP16(HostCmd_CMD_802_11_GET_STAT);
	pCmd->CmdHdr.Length   = ENDIAN_SWAP16(sizeof(HostCmd_DS_802_11_GET_STAT));
	pCmd->CmdHdr.macid = 1;
	WLDBG_DUMP_DATA(DBG_LEVEL_0, (void *) pCmd, sizeof(HostCmd_DS_802_11_GET_STAT));
	if (wlexecuteCommand(netdev, HostCmd_CMD_802_11_GET_STAT))
	{
		WLDBG_EXIT_INFO(DBG_LEVEL_0, "failed execution");
		MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
		return FAIL;
	}
	p = (const struct macaddr *)&pCmd->TxRetrySuccesses;
	printk("LegacyMacAddr:\n");
	printk("  %s\n", ntoa(p->LegacyMacAddr));
	printk("MacAddressInDB:\n");
	for (i = 0; i<8; i += 4) {
		printk("  %s", ntoa(p->MacAddressInDB[i+0]));
		printk("  %s",  ntoa(p->MacAddressInDB[i+1]));
		printk("  %s", ntoa(p->MacAddressInDB[i+2]));
		printk("  %s\n", ntoa(p->MacAddressInDB[i+3]));
	}
	printk("  %s", ntoa(p->MacAddressInDB[i+0]));
	printk("  %s\n", ntoa(p->MacAddressInDB[i+1]));
	printk("MBSSMacAddr:\n");
	for (i = 0; i<20; i += 4) {
		printk("  %s", ntoa(p->MBSSMacAddr[i+0]));
		printk("  %s",  ntoa(p->MBSSMacAddr[i+1]));
		printk("  %s", ntoa(p->MBSSMacAddr[i+2]));
		printk("  %s\n", ntoa(p->MBSSMacAddr[i+3]));
	}
	printk("  %s\n", ntoa(p->MBSSMacAddr[i+0]));
	MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
	return 0;
}
typedef struct FW_WepDefaultKeys_s
{
	unsigned char WepDefaultKeyIdx;        // 1 to 4
	unsigned char WepDefaultKeyValue[13];   // 5 byte string
} FW_WEP_DEFAULT_KEYS;

typedef struct FWwepKeyMgmtInfo_t
{
	/* XXX byte order dependent */
	unsigned short LoopBackOn:1;
	unsigned short	WepEn:1;
	unsigned short	IntrEn:1;
	unsigned short	MultiCastEn:1;
	unsigned short	BroadCastEn:1;
	unsigned short	PromiscuousEn:1;
	unsigned short	AllMultiCastEn:1;
	unsigned short	KeyId:6;
	unsigned short	HWSpecCmdDone:1;
	unsigned short	WepType:1;	 
	unsigned short	EnforceProtection:1; 	 
	FW_WEP_DEFAULT_KEYS WepDefaultKeys[4];
} FWwepKeyMgmtInfo_t;

typedef struct
{
	unsigned char  UnicastKeyEnabled;
	unsigned char  MulticastKeyEnabled;
	unsigned char  UnicastKeyType;
	unsigned char  MulticastKeyType;    
	unsigned char  RSNPairwiseTempKey[16];
	unsigned int RSNPwkTxMICKey[2];
	unsigned int RSNPwkRxMICKey[2];
	unsigned char   RSNTempKey_group[16];
	unsigned int RSNTxMICKey_group[2];
	unsigned int RSNRxMICKey_group[2];
	unsigned int TxIV32;
	unsigned short TxIV16;
	unsigned int RxIV32;
	unsigned int groupTxIV32;
	unsigned short groupTxIV16;
	unsigned int groupRxIV32;    
	unsigned char  groupKeyIndex;
} FWkeyMgmtInfo_t;

int wlFwGetEncrInfo(struct net_device *netdev, unsigned char *addr)
{
	const struct encrdata {
		unsigned char StaEntryAddr[6];
		unsigned char EnHwEncr; /*EncrTypeWep = 0, EncrTypeDisable = 1,
								EncrTypeTkip = 4, EncrTypeAes = 6, EncrTypeMix = 7, */
		union{
			FWwepKeyMgmtInfo_t  gWepKeyData;
			FWkeyMgmtInfo_t         gKeyData;
		} PACK_END key;
	} PACK_END *p;
	static const char *encrnames[] =
	{ "WEP", "-", "#2", "#3", "TKIP", "#5", "AES", "MIX" };
	static const char *ciphernames[] =
	{ "WEP", "TKIP", "AES", "#3", "#4", "#5", "#6", "#7" };
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	int i =0, j;
	HostCmd_DS_802_11_GET_STAT *pCmd = (HostCmd_DS_802_11_GET_STAT *)&wlpptr->pCmdBuf[0];

	unsigned long flags; 

	WLDBG_ENTER(DBG_LEVEL_0);

	MWL_SPIN_LOCK(&wlpptr->wlpd_p->locks.fwLock);
	memset(pCmd, 0x00, sizeof(HostCmd_DS_802_11_GET_STAT));
	pCmd->CmdHdr.Cmd	  = ENDIAN_SWAP16(HostCmd_CMD_802_11_GET_STAT);
	pCmd->CmdHdr.Length   = ENDIAN_SWAP16(sizeof(HostCmd_DS_802_11_GET_STAT));
	pCmd->CmdHdr.macid = 2;
	memcpy(&pCmd->TxRetrySuccesses, addr, 6);

	WLDBG_DUMP_DATA(DBG_LEVEL_0, (void *) pCmd, sizeof(HostCmd_DS_802_11_GET_STAT));
	if (wlexecuteCommand(netdev, HostCmd_CMD_802_11_GET_STAT))
	{
		WLDBG_EXIT_INFO(DBG_LEVEL_0, "failed execution");
		MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
		return FAIL;
	}
	p = (const struct encrdata *)&pCmd->TxRetrySuccesses;
	printk("\n%s:\n", ntoa(p->StaEntryAddr));
	if (p->EnHwEncr == 1) {
		printk("Encryption disabled\n");
	} else if (p->EnHwEncr == 0) {
		printk("Mode 	Loop 	Wep 	Intr	Mcast	Bcast	Promisc	AllMcast\n");
		printk("%4s	%s	%s	%s	%s	%s	%s	%s\n"
			, encrnames[p->EnHwEncr & 7]
		, p->key.gWepKeyData.LoopBackOn ? "ena" : "-"
			, p->key.gWepKeyData.WepEn ? "ena" : "-"
			, p->key.gWepKeyData.IntrEn ? "ena" : "-"
			, p->key.gWepKeyData.MultiCastEn ? "ena" : "-"
			, p->key.gWepKeyData.BroadCastEn ? "ena" : "-"
			, p->key.gWepKeyData.PromiscuousEn ? "ena" : "-"
			, p->key.gWepKeyData.AllMultiCastEn ? "ena" : "-"
			);
		printk("KeyId 	HWSpec 	WepType	Protect\n");
		printk("%d	%s	%s	%s\n"
			, p->key.gWepKeyData.KeyId
			, p->key.gWepKeyData.HWSpecCmdDone ? "yes" : "no"
			, p->key.gWepKeyData.WepType ? "104" : "40"
			, p->key.gWepKeyData.EnforceProtection ? "yes" : "no"
			);
		for (i = 0; i < 4; i++) {
			printk("[%2d]", p->key.gWepKeyData.WepDefaultKeys[i].WepDefaultKeyIdx);
			for (j = 0; j < 13; j++)
				printk(" %02x", p->key.gWepKeyData.WepDefaultKeys[i].WepDefaultKeyValue[j]);
			printk("\n");
		}
	} else {
		printk("Mode 	PTK 	PTK type	GTK 	GTK type	GTK Index\n");
		printk("%4s	%s	%s		%s	%s		%d\n"
			, encrnames[p->EnHwEncr & 7]
		, p->key.gKeyData.UnicastKeyEnabled ? "ena" : "-"
			, p->key.gKeyData.UnicastKeyEnabled ?
			ciphernames[p->key.gKeyData.UnicastKeyType & 7] : "-"
			, p->key.gKeyData.MulticastKeyEnabled ? "ena" : "-"
			, p->key.gKeyData.MulticastKeyEnabled ?
			ciphernames[p->key.gKeyData.MulticastKeyType & 7] : "-"
			, p->key.gKeyData.groupKeyIndex
			);
		if (p->key.gKeyData.UnicastKeyEnabled) {
			printk("PTK:      ");
			for (i = 0; i < 16; i++)
				printk(" %02x",
				p->key.gKeyData.RSNPairwiseTempKey[i]);
			printk("\n");
			printk("PTK TxMIC: %08x %08x	TxIV: %08x %04x\n",
				p->key.gKeyData.RSNPwkTxMICKey[0],
				p->key.gKeyData.RSNPwkTxMICKey[1],
				/*le32toh*/(p->key.gKeyData.TxIV32),
				/*le16toh*/(p->key.gKeyData.TxIV16));
			printk("PTK RxMIC: %08x %08x	RxIV: %08x\n",
				p->key.gKeyData.RSNPwkRxMICKey[0],
				p->key.gKeyData.RSNPwkRxMICKey[1],
				/*le32toh*/(p->key.gKeyData.RxIV32));
		}
		if (p->key.gKeyData.MulticastKeyEnabled) {
			printk("GTK:      ");
			for (i = 0; i < 16; i++)
				printk(" %02x",
				p->key.gKeyData.RSNTempKey_group[i]);
			printk("\n");
			printk("GTK TxMIC: %08x %08x	TxIV: %08x %04x\n",
				p->key.gKeyData.RSNTxMICKey_group[0],
				p->key.gKeyData.RSNTxMICKey_group[1],
				/*le32toh*/(p->key.gKeyData.groupTxIV32),
				/*le16toh*/(p->key.gKeyData.groupTxIV16));
			printk("GTK RxMIC: %08x %08x	RxIV: %08x\n",
				p->key.gKeyData.RSNRxMICKey_group[0],
				p->key.gKeyData.RSNRxMICKey_group[1],
				/*le32toh*/(p->key.gKeyData.groupRxIV32));
		}
	}
	MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
	return 0;
}

BOOLEAN wlFwGetHwStatsForWlStats(struct net_device *netdev, struct iw_statistics *pStats)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);

	HostCmd_DS_802_11_GET_STAT *pCmd = (HostCmd_DS_802_11_GET_STAT *)&wlpptr->pCmdBuf[0];

	unsigned long flags; 

	WLDBG_ENTER(DBG_LEVEL_0);

	MWL_SPIN_LOCK(&wlpptr->wlpd_p->locks.fwLock);
	memset(pCmd, 0x00, sizeof(HostCmd_DS_802_11_GET_STAT));
	pCmd->CmdHdr.Cmd      = ENDIAN_SWAP16(HostCmd_CMD_802_11_GET_STAT);
	pCmd->CmdHdr.Length   = ENDIAN_SWAP16(sizeof(HostCmd_DS_802_11_GET_STAT));

	WLDBG_DUMP_DATA(DBG_LEVEL_0, (void *) pCmd, sizeof(HostCmd_DS_802_11_GET_STAT));
	if (wlexecuteCommand(netdev, HostCmd_CMD_802_11_GET_STAT))
	{
		WLDBG_EXIT_INFO(DBG_LEVEL_0, "failed execution");
		MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
		return FAIL;
	}
	MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
	pStats->discard.code     = ENDIAN_SWAP32(pCmd->RxUndecryptableFrames);
	pStats->discard.fragment = ENDIAN_SWAP32(pCmd->RxFragErrors);
	pStats->discard.misc     = 0;
	pStats->discard.nwid     = 0;
	pStats->discard.retries  = ENDIAN_SWAP32(pCmd->TxFailures);
	pStats->miss.beacon      = 0;

	return TRUE;
}


int wlFwHTGI(struct net_device *netdev, u_int32_t GIType)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	HostCmd_FW_HT_GUARD_INTERVAL *pCmd =
		(HostCmd_FW_HT_GUARD_INTERVAL *)&wlpptr->pCmdBuf[0];
	int retval = FAIL;
	unsigned long flags;

	MWL_SPIN_LOCK(&wlpptr->wlpd_p->locks.fwLock);
	memset(pCmd, 0x00, sizeof(HostCmd_FW_HT_GUARD_INTERVAL));
	pCmd->CmdHdr.Cmd   =ENDIAN_SWAP16(HostCmd_CMD_HT_GUARD_INTERVAL);
	pCmd->CmdHdr.Length=ENDIAN_SWAP16(sizeof(HostCmd_FW_HT_GUARD_INTERVAL));
	pCmd->Action       =ENDIAN_SWAP32(WL_SET);
	pCmd->CmdHdr.macid = wlpptr->vmacSta_p->VMacEntry.macId;

	if (GIType == 0 )
	{
		pCmd->GIType.LongGI  = 1;
		pCmd->GIType.ShortGI = 1;
	}
	else if (GIType == 1 )
	{
		pCmd->GIType.LongGI  = 0;
		pCmd->GIType.ShortGI = 1;
	}
	else
	{
		pCmd->GIType.LongGI  = 1;
		pCmd->GIType.ShortGI = 0;
	}
	pCmd->GIType.RESV = 0;

	WLDBG_DUMP_DATA(DBG_LEVEL_0, (void *) pCmd, 
		sizeof(HostCmd_FW_HT_GUARD_INTERVAL));
	retval = wlexecuteCommand(netdev, HostCmd_CMD_HT_GUARD_INTERVAL);
	MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
	return retval;
}


int wlFwSetRadio(struct net_device *netdev, u_int16_t mode, wlpreamble_e preamble)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	HostCmd_DS_802_11_RADIO_CONTROL *pCmd =
		(HostCmd_DS_802_11_RADIO_CONTROL *)&wlpptr->pCmdBuf[0];
	int retval = FAIL;
	unsigned long flags;

	WLDBG_ENTER_INFO(DBG_LEVEL_0,"mode: %s,preamble: %i",
		(mode == WL_DISABLE) ? "disable" : "enable" , preamble);

	MWL_SPIN_LOCK(&wlpptr->wlpd_p->locks.fwLock);
	memset(pCmd, 0x00, sizeof(HostCmd_DS_802_11_RADIO_CONTROL));
	pCmd->CmdHdr.Cmd   =ENDIAN_SWAP16(HostCmd_CMD_802_11_RADIO_CONTROL);
	pCmd->CmdHdr.Length=ENDIAN_SWAP16(sizeof(HostCmd_DS_802_11_RADIO_CONTROL));
	pCmd->Action       =ENDIAN_SWAP16(WL_SET);
	pCmd->Control      =ENDIAN_SWAP16(preamble);
	pCmd->RadioOn      =ENDIAN_SWAP16(mode);
	pCmd->CmdHdr.macid = wlpptr->vmacSta_p->VMacEntry.macId;

	if (mode == WL_DISABLE)
	{
		pCmd->Control = 0;
	}

	WLDBG_DUMP_DATA(DBG_LEVEL_0, (void *) pCmd, 
		sizeof(HostCmd_DS_802_11_RADIO_CONTROL));
	retval = wlexecuteCommand(netdev, HostCmd_CMD_802_11_RADIO_CONTROL);
	MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
	return retval;
}

int wlFwSetAntenna(struct net_device *netdev, wlantennatype_e dirSet)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	vmacApInfo_t *vmacSta_p = wlpptr->vmacSta_p;
	MIB_802DOT11 *mib = vmacSta_p->Mib802dot11;
	UINT8 *mib_rxAntenna_p = mib->mib_rxAntenna;
	UINT8 *mib_txAntenna_p = mib->mib_txAntenna;

	HostCmd_DS_802_11_RF_ANTENNA *pCmd =
		(HostCmd_DS_802_11_RF_ANTENNA *)&wlpptr->pCmdBuf[0];
	int retval = FAIL;
	unsigned long flags;

	WLDBG_ENTER_INFO(DBG_LEVEL_0,
		"will set %s antenna", (dirSet == WL_ANTENNATYPE_RX)?"RX":"TX");

	MWL_SPIN_LOCK(&wlpptr->wlpd_p->locks.fwLock);
	memset(pCmd, 0x00, sizeof(HostCmd_DS_802_11_RF_ANTENNA));
	pCmd->CmdHdr.Cmd    = ENDIAN_SWAP16(HostCmd_CMD_802_11_RF_ANTENNA);
	pCmd->CmdHdr.Length = ENDIAN_SWAP16(sizeof(HostCmd_DS_802_11_RF_ANTENNA));
	pCmd->Action        = ENDIAN_SWAP16(dirSet);
	pCmd->CmdHdr.macid = wlpptr->vmacSta_p->VMacEntry.macId;
	if (dirSet == WL_ANTENNATYPE_RX)
	{
#if defined(SOC_W8764)
		UINT8 rxAntenna = 4; /* if auto, set 4 rx antennas in SC2 */
#else
		UINT8 rxAntenna = 3;
#endif
		if ((*mib_rxAntenna_p != 0) )
			pCmd->AntennaMode = ENDIAN_SWAP16(*mib_rxAntenna_p);//(WL_ANTENNAMODE_RX);
		else
			pCmd->AntennaMode = ENDIAN_SWAP16(rxAntenna);
	} else
	{
#if defined(SOC_W8764)
		UINT8 txAntenna = 0xf; /* if auto, set 4 tx antennas in SC2 */
#elif defined(SOC_W8366)
		UINT8 txAntenna = 7; /* if auto, set 3 tx antennas in SJAY */
#else
		UINT8 txAntenna = 3;
#endif
		if(dirSet == WL_ANTENNATYPE_TX2)
			pCmd->AntennaMode = ENDIAN_SWAP16(*mib->mib_txAntenna2);
		else
		{
			printk("setting txantenna 0x%x, 0x%x\n", *mib_txAntenna_p, txAntenna);
			if (*mib_txAntenna_p != 0)

				pCmd->AntennaMode = ENDIAN_SWAP16(*mib_txAntenna_p);
			else
				pCmd->AntennaMode = ENDIAN_SWAP16(txAntenna); 
		}
	}

	WLDBG_DUMP_DATA(DBG_LEVEL_0, (void *) pCmd, 
		sizeof(HostCmd_DS_802_11_RF_ANTENNA));
	retval = wlexecuteCommand(netdev, HostCmd_CMD_802_11_RF_ANTENNA);
	MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
	return retval;
}

int wlFwSetRTSThreshold(struct net_device *netdev, int threshold)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	HostCmd_DS_802_11_RTS_THSD *pCmd =
		(HostCmd_DS_802_11_RTS_THSD *)&wlpptr->pCmdBuf[0];
	int retval = FAIL;
	unsigned long flags;

	WLDBG_ENTER_INFO(DBG_LEVEL_0,
		"RTS threshold: %i (0x%x)", threshold, threshold);

	MWL_SPIN_LOCK(&wlpptr->wlpd_p->locks.fwLock);
	memset(pCmd, 0x00, sizeof(HostCmd_DS_802_11_RTS_THSD));
	pCmd->CmdHdr.Cmd    = ENDIAN_SWAP16(HostCmd_CMD_802_11_RTS_THSD);
	pCmd->CmdHdr.Length = ENDIAN_SWAP16(sizeof(HostCmd_DS_802_11_RTS_THSD));
	pCmd->Action  = ENDIAN_SWAP16(WL_SET);
	pCmd->Threshold = ENDIAN_SWAP16(threshold);
	pCmd->CmdHdr.macid = wlpptr->vmacSta_p->VMacEntry.macId;

	WLDBG_DUMP_DATA(DBG_LEVEL_0, (void *) pCmd, 
		sizeof(HostCmd_DS_802_11_RTS_THSD));
	retval = wlexecuteCommand(netdev, HostCmd_CMD_802_11_RTS_THSD);
	MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
	return retval;
}
int wlFwSetInfraMode(struct net_device *netdev)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	HostCmd_FW_SET_INFRA_MODE *pCmd =
		(HostCmd_FW_SET_INFRA_MODE *)&wlpptr->pCmdBuf[0];
	int retval = FAIL;

	unsigned long flags; 

	WLDBG_ENTER(DBG_LEVEL_0);

	MWL_SPIN_LOCK(&wlpptr->wlpd_p->locks.fwLock);
	memset(pCmd, 0x00, sizeof(HostCmd_FW_SET_INFRA_MODE));
	pCmd->CmdHdr.Cmd    = ENDIAN_SWAP16(HostCmd_CMD_SET_INFRA_MODE);
	pCmd->CmdHdr.Length = ENDIAN_SWAP16(sizeof(HostCmd_FW_SET_INFRA_MODE));
	pCmd->CmdHdr.macid = wlpptr->vmacSta_p->VMacEntry.macId;

	WLDBG_DUMP_DATA(DBG_LEVEL_0, (void *) pCmd, 
		sizeof(HostCmd_FW_SET_INFRA_MODE));
	retval = wlexecuteCommand(netdev, HostCmd_CMD_SET_INFRA_MODE);
	MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
	return retval;
}

#ifdef IEEE80211_DH

extern UINT16 dfs_chirp_count_min;
extern UINT16 dfs_chirp_time_interval;
extern UINT16 dfs_pw_filter;
extern UINT16 dfs_min_pri_count;
extern UINT16 dfs_min_num_radar;

int wlFwSetRadarDetection(struct net_device *netdev, UINT32 action)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	vmacApInfo_t *vmacSta_p = wlpptr->vmacSta_p;
	MIB_802DOT11 *mib = vmacSta_p->Mib802dot11;
	UINT8 chan = mib->PhyDSSSTable->CurrChan;
	UINT16 radarTypeCode = 0 ;

	HostCmd_802_11h_Detect_Radar *pCmd =
		(HostCmd_802_11h_Detect_Radar *) &wlpptr->pCmdBuf[0];
	int retval = FAIL;
	unsigned long flags;

	WLDBG_ENTER_INFO(DBG_LEVEL_0,
		"AP radar detection enabled\n");
	/* First check if the region code is Japan and 5 GHz channel
	* If so, assign radarTypeCode = 56 for 104 < chan < 140
	* and assign radarTypeCode = 53 for 52 < chan < 64 
	*/
	if( *(mib->mib_regionCode) == DOMAIN_CODE_MKK && 
		domainChannelValid(chan, FREQ_BAND_5GHZ))
	{
		if( chan >= 52 && chan <= 64 )
		{
			radarTypeCode = 53 ;
		}
		else if( chan >= 100 && chan <= 140 )
		{
			radarTypeCode = 56 ;
		}
		else
			radarTypeCode = 0 ;
	}
	else if (*(mib->mib_regionCode) == DOMAIN_CODE_ETSI )
	{
		radarTypeCode = HostCmd_80211H_RADAR_TYPE_CODE_ETSI_151 ;
	}

	MWL_SPIN_LOCK(&wlpptr->wlpd_p->locks.fwLock);
	memset(pCmd, 0x00, sizeof(HostCmd_802_11h_Detect_Radar));
	pCmd->CmdHdr.Cmd = ENDIAN_SWAP16(HostCmd_CMD_802_11H_DETECT_RADAR);
	pCmd->CmdHdr.Length = ENDIAN_SWAP16(sizeof(HostCmd_802_11h_Detect_Radar));
	pCmd->CmdHdr.macid = wlpptr->vmacSta_p->VMacEntry.macId;
	pCmd->Action = ENDIAN_SWAP16(action);
	pCmd->RadarTypeCode = ENDIAN_SWAP16(radarTypeCode);

    pCmd->MinChirpCount  = ENDIAN_SWAP16(dfs_chirp_count_min);
    pCmd->ChirpTimeIntvl = ENDIAN_SWAP16(dfs_chirp_time_interval);
    pCmd->PwFilter = ENDIAN_SWAP16(dfs_pw_filter);
    pCmd->MinNumRadar = ENDIAN_SWAP16(dfs_min_num_radar);
    pCmd->PriMinNum = ENDIAN_SWAP16(dfs_min_pri_count);


	WLDBG_DUMP_DATA(DBG_LEVEL_0, (void *) pCmd,
		sizeof(HostCmd_802_11h_Detect_Radar));
	retval = wlexecuteCommand(netdev, HostCmd_CMD_802_11H_DETECT_RADAR);
	MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
	return retval;
} 

int wlFwSetChannelSwitchIE(struct net_device *netdev, UINT32 nextChannel, UINT32 mode, UINT32 count, CHNL_FLAGS Chanflag)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	vmacApInfo_t *vmacSta_p = wlpptr->vmacSta_p;			
	MIB_802DOT11 *mib = vmacSta_p->ShadowMib802dot11;		
	HostCmd_SET_SWITCH_CHANNEL *pCmd =
		(HostCmd_SET_SWITCH_CHANNEL *) &wlpptr->pCmdBuf[0];
	int retval = FAIL;
	unsigned long flags;

	WLDBG_ENTER_INFO(DBG_LEVEL_0,
		"AP Channel To Switch to %d Mode :%d and Count :%d", nextChannel, mode, count );

	MWL_SPIN_LOCK(&wlpptr->wlpd_p->locks.fwLock);
	memset(pCmd, 0x00, sizeof(HostCmd_SET_SWITCH_CHANNEL));
	pCmd->CmdHdr.Cmd = ENDIAN_SWAP16(HostCmd_CMD_SET_SWITCH_CHANNEL);
	pCmd->CmdHdr.Length = ENDIAN_SWAP16(sizeof(HostCmd_SET_SWITCH_CHANNEL));
	pCmd->Next11hChannel = ENDIAN_SWAP32(nextChannel);
	pCmd->Mode = ENDIAN_SWAP32(mode);
	pCmd->InitialCount = ENDIAN_SWAP32(count+1);
	pCmd->CmdHdr.macid = wlpptr->vmacSta_p->VMacEntry.macId;
	pCmd->dfs_test_mode = dfs_test_mode;


	/*Setting Chnlwidth &  ActPrimary same as in wlFwSetChannel*/	

	pCmd->ChannelFlags.FreqBand  = Chanflag.FreqBand;
	pCmd->ChannelFlags.ChnlWidth = Chanflag.ChnlWidth;
	if(Chanflag.ChnlWidth == CH_AUTO_WIDTH)
	{
		if (Chanflag.FreqBand == FREQ_BAND_2DOT4GHZ)
		{
			pCmd->ChannelFlags.ChnlWidth = CH_40_MHz_WIDTH;
		}
		else
		{	
			if((*(mib->mib_ApMode) >= AP_MODE_11AC) && (*(mib->mib_ApMode) <= AP_MODE_5GHZ_Nand11AC))
				pCmd->ChannelFlags.ChnlWidth = CH_80_MHz_WIDTH;
			else
				pCmd->ChannelFlags.ChnlWidth = CH_40_MHz_WIDTH;
		}
	}

	/*Get 11n HT ext channel offset of target channel*/
	if((pCmd->ChannelFlags.ChnlWidth == CH_40_MHz_WIDTH) ||(pCmd->ChannelFlags.ChnlWidth == CH_80_MHz_WIDTH))
		pCmd->NextHTExtChnlOffset = macMgmtMlme_Get40MHzExtChannelOffset(pCmd->Next11hChannel);
	else
		pCmd->NextHTExtChnlOffset = EXT_CH_ABOVE_CTRL_CH;

	/*Based on next channel HT ext offset, get ActPrimary*/
	if (pCmd->NextHTExtChnlOffset == EXT_CH_ABOVE_CTRL_CH)
		pCmd->ChannelFlags.ActPrimary = ACT_PRIMARY_CHAN_0;
	else if (pCmd->NextHTExtChnlOffset == EXT_CH_BELOW_CTRL_CH)
		pCmd->ChannelFlags.ActPrimary = ACT_PRIMARY_CHAN_1;
	else
		pCmd->ChannelFlags.ActPrimary = ACT_PRIMARY_CHAN_0;
	
	if (pCmd->ChannelFlags.ChnlWidth == CH_80_MHz_WIDTH)
		pCmd->ChannelFlags.ActPrimary = macMgmtMlme_Get80MHzPrimaryChannelOffset(pCmd->Next11hChannel);
	
	
	WLDBG_DUMP_DATA(DBG_LEVEL_0, (void *) pCmd,
		sizeof(HostCmd_SET_SWITCH_CHANNEL));
	retval = wlexecuteCommand(netdev, HostCmd_CMD_SET_SWITCH_CHANNEL);
	MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
	return retval;
}

int wlFwSetSpectrumMgmt(struct net_device *netdev, UINT32 spectrumMgmt)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	HostCmd_SET_SPECTRUM_MGMT *pCmd =
		(HostCmd_SET_SPECTRUM_MGMT *) &wlpptr->pCmdBuf[0];
	int retval = FAIL;
	unsigned long flags;

	WLDBG_ENTER_INFO(DBG_LEVEL_0,
		"Set Spectrum Management to %d", spectrumMgmt );

	MWL_SPIN_LOCK(&wlpptr->wlpd_p->locks.fwLock);
	memset(pCmd, 0x00, sizeof(HostCmd_SET_SPECTRUM_MGMT));
	pCmd->CmdHdr.Cmd = ENDIAN_SWAP16(HostCmd_CMD_SET_SPECTRUM_MGMT);
	pCmd->CmdHdr.Length = ENDIAN_SWAP16(sizeof(HostCmd_SET_SPECTRUM_MGMT));
	pCmd->SpectrumMgmt = ENDIAN_SWAP32(spectrumMgmt);
	pCmd->CmdHdr.macid = wlpptr->vmacSta_p->VMacEntry.macId;

	WLDBG_DUMP_DATA(DBG_LEVEL_0, (void *) pCmd,
		sizeof(HostCmd_SET_SPECTRUM_MGMT));
	retval = wlexecuteCommand(netdev, HostCmd_CMD_SET_SPECTRUM_MGMT);
	MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
	return retval;
}

int wlFwSetPowerConstraint(struct net_device *netdev, UINT32 powerConstraint)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	HostCmd_SET_POWER_CONSTRAINT *pCmd =
		(HostCmd_SET_POWER_CONSTRAINT *) &wlpptr->pCmdBuf[0];
	int retval = FAIL;
	unsigned long flags;

	WLDBG_ENTER_INFO(DBG_LEVEL_0,
		"Set power constraint to %d dB", powerConstraint );

	MWL_SPIN_LOCK(&wlpptr->wlpd_p->locks.fwLock);
	memset(pCmd, 0x00, sizeof(HostCmd_SET_POWER_CONSTRAINT));
	pCmd->CmdHdr.Cmd = ENDIAN_SWAP16(HostCmd_CMD_SET_POWER_CONSTRAINT);
	pCmd->CmdHdr.Length = ENDIAN_SWAP16(sizeof(HostCmd_SET_POWER_CONSTRAINT));
	pCmd->PowerConstraint = ENDIAN_SWAP32(powerConstraint);
	pCmd->CmdHdr.macid = wlpptr->vmacSta_p->VMacEntry.macId;

	WLDBG_DUMP_DATA(DBG_LEVEL_0, (void *) pCmd,
		sizeof(HostCmd_SET_POWER_CONSTRAINT));
	retval = wlexecuteCommand(netdev, HostCmd_CMD_SET_POWER_CONSTRAINT);
	MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
	return retval;
}

int wlFwSetCountryCode(struct net_device *netdev, UINT32 domainCode)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	MIB_802DOT11 *mib=wlpptr->vmacSta_p->Mib802dot11;
	MIB_SPECTRUM_MGMT	*mib_SpectrumMagament_p=wlpptr->vmacSta_p->Mib802dot11->SpectrumMagament;
	HostCmd_SET_COUNTRY_INFO *pCmd =
		(HostCmd_SET_COUNTRY_INFO *) &wlpptr->pCmdBuf[0];
	int retval = FAIL;
	unsigned long flags;
	DomainCountryInfo DomainInfo[1];
	MIB_PHY_DSSS_TABLE *PhyDSSSTable=mib->PhyDSSSTable;

	if(  !mib_SpectrumMagament_p->spectrumManagement ||
		!mib_SpectrumMagament_p->multiDomainCapability )
	{
		return retval ;
	}

	WLDBG_ENTER_INFO(DBG_LEVEL_0,
		"Set  the country infor for :%x\n", domainCode );

	memset(pCmd, 0x00, sizeof(HostCmd_SET_COUNTRY_INFO));
	pCmd->CmdHdr.Cmd = ENDIAN_SWAP16(HostCmd_CMD_SET_COUNTRY_CODE);
	pCmd->CmdHdr.Length = ENDIAN_SWAP16(sizeof(HostCmd_SET_COUNTRY_INFO));
	pCmd->CmdHdr.macid = wlpptr->vmacSta_p->VMacEntry.macId;
	if( domainCode == 0 ) 
	{
		pCmd->Action = 0 ;
		memset( &pCmd->DomainInfo, 0, sizeof( DomainCountryInfo ) );
	}
	else 
	{
		pCmd->Action = ENDIAN_SWAP32(1);
		bcn_reg_domain = domainCode ;
		domainGetPowerInfo((UINT8 *)DomainInfo);

		if (PhyDSSSTable->Chanflag.FreqBand == FREQ_BAND_5GHZ )
		{
			DomainInfo->GChannelLen = 0 ;
		}
		else 
		{
			DomainInfo->AChannelLen = 0;  
		}
		memcpy( &pCmd->DomainInfo, DomainInfo, sizeof( DomainCountryInfo ) );
	}

	WLDBG_DUMP_DATA(DBG_LEVEL_0, (void *) pCmd,
		sizeof(HostCmd_SET_COUNTRY_INFO));
	MWL_SPIN_LOCK(&wlpptr->wlpd_p->locks.fwLock);
	retval = wlexecuteCommand(netdev, HostCmd_CMD_SET_COUNTRY_CODE);
	MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
	return retval;
}

#endif // IEEE80211_DH

int wlFwSetRegionCode(struct net_device *netdev, UINT16 regionCode)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
#ifndef SOC_W8764
	vmacApInfo_t *vmacSta_p = wlpptr->vmacSta_p;
	MIB_802DOT11 *mib = vmacSta_p->Mib802dot11;
	UINT8 chan = mib->PhyDSSSTable->CurrChan;
#endif
	HostCmd_SET_REGIONCODE_INFO *pCmd =
		(HostCmd_SET_REGIONCODE_INFO *) &wlpptr->pCmdBuf[0];
	int retval = FAIL;
	unsigned long flags;
#ifndef SOC_W8764
	/* First check if the region code is Japan and 5 GHz channel
	* If so, re-assign region code as DOMAIN_CODE_MKK3(0x42) if the
	* configured channel is between 100 and 140
	*/
	if( regionCode == DOMAIN_CODE_MKK && 
		domainChannelValid(chan, FREQ_BAND_5GHZ) &&
		( chan >= 100 && chan <= 140 ))
	{
		regionCode = DOMAIN_CODE_MKK3 ;
	}
#endif

	MWL_SPIN_LOCK(&wlpptr->wlpd_p->locks.fwLock);
	memset(pCmd, 0x00, sizeof(HostCmd_SET_REGIONCODE_INFO));
	pCmd->CmdHdr.Cmd = ENDIAN_SWAP16(HostCmd_CMD_SET_REGION_CODE);
	pCmd->regionCode = ENDIAN_SWAP16(regionCode) ;
	pCmd->CmdHdr.Length = ENDIAN_SWAP16(sizeof(HostCmd_SET_REGIONCODE_INFO));
	pCmd->CmdHdr.macid = wlpptr->vmacSta_p->VMacEntry.macId;

	WLDBG_DUMP_DATA(DBG_LEVEL_1, (void *) pCmd, sizeof(HostCmd_SET_REGIONCODE_INFO));
	retval = wlexecuteCommand(netdev, HostCmd_CMD_SET_REGION_CODE);
	MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
	return retval;
}


int wlFwGetNoiseLevel (struct net_device *netdev, UINT16 action, UINT8 *pNoise)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	HostCmd_FW_GET_NOISE_Level *pCmd =
		(HostCmd_FW_GET_NOISE_Level *) &wlpptr->pCmdBuf[0];
	int retval = FAIL;
	unsigned long flags;

	MWL_SPIN_LOCK(&wlpptr->wlpd_p->locks.fwLock);
	memset(pCmd, 0x00, sizeof(HostCmd_FW_GET_NOISE_Level));
	pCmd->CmdHdr.Cmd = ENDIAN_SWAP16(HostCmd_CMD_GET_NOISE_LEVEL);
	pCmd->CmdHdr.Length = ENDIAN_SWAP16(sizeof(HostCmd_FW_GET_NOISE_Level));
	pCmd->Action = ENDIAN_SWAP16(action);

	WLDBG_DUMP_DATA(DBG_LEVEL_1, (void *) pCmd,
		sizeof(HostCmd_FW_GET_NOISE_Level));
	retval = wlexecuteCommand(netdev, HostCmd_CMD_GET_NOISE_LEVEL);
	MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
	*pNoise = pCmd->Noise;
	return retval;
}

#ifdef MRVL_WSC
int wlFwSetWscIE(struct net_device *netdev, UINT16 ieType, WSC_COMB_IE_t *pWscIE )
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	HostCmd_SET_WSC_IE *pCmd =
		(HostCmd_SET_WSC_IE *) &wlpptr->pCmdBuf[0];
	int retval = FAIL;
	unsigned long flags;

	if( pWscIE == NULL )
		return retval ;
	MWL_SPIN_LOCK(&wlpptr->wlpd_p->locks.fwLock);
	memset(pCmd, 0x00, sizeof(HostCmd_SET_WSC_IE));
	pCmd->CmdHdr.Cmd = ENDIAN_SWAP16(HostCmd_CMD_SET_WSC_IE);
	pCmd->CmdHdr.Length = ENDIAN_SWAP16(sizeof(HostCmd_SET_WSC_IE));
	pCmd->ieType = ENDIAN_SWAP16(ieType);
	pCmd->CmdHdr.macid = wlpptr->vmacSta_p->VMacEntry.macId;
	memcpy(&pCmd->wscIE, pWscIE, sizeof( WSC_COMB_IE_t));

	WLDBG_DUMP_DATA(DBG_LEVEL_1, (void *) pCmd,
		sizeof(HostCmd_SET_WSC_IE));
	retval = wlexecuteCommand(netdev, HostCmd_CMD_SET_WSC_IE);
	MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
	return retval;
}
#endif // MRVL_WSC

#ifdef MRVL_WAPI
int wlFwSetWapiIE(struct net_device *netdev, UINT16 ieType, WAPI_COMB_IE_t *pAPPIE )
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	HostCmd_SET_WAPI_IE *pCmd =
		(HostCmd_SET_WAPI_IE *) &wlpptr->pCmdBuf[0];
	int retval = FAIL;
	unsigned long flags;

	if( pAPPIE == NULL )
		return retval ;
	MWL_SPIN_LOCK(&wlpptr->wlpd_p->locks.fwLock);
	memset(pCmd, 0x00, sizeof(HostCmd_SET_WAPI_IE));
	pCmd->CmdHdr.Cmd = ENDIAN_SWAP16(HostCmd_CMD_SET_WAPI_IE);
	pCmd->CmdHdr.Length = ENDIAN_SWAP16(sizeof(HostCmd_SET_WAPI_IE));
	pCmd->ieType = ENDIAN_SWAP16(ieType);
	pCmd->CmdHdr.macid = wlpptr->vmacSta_p->VMacEntry.macId;
	memcpy(&pCmd->WAPIIE, pAPPIE, sizeof( WAPI_COMB_IE_t));

	WLDBG_DUMP_DATA(DBG_LEVEL_1, (void *) pCmd,
		sizeof(HostCmd_SET_WAPI_IE));
	retval = wlexecuteCommand(netdev, HostCmd_CMD_SET_WAPI_IE);
	MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
	return retval;
}
#endif // MRVL_WAPI

int wlFwSetRate(struct net_device *netdev, wlrate_e rate)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	vmacApInfo_t *vmacSta_p = wlpptr->vmacSta_p;
	MIB_802DOT11 *mib = vmacSta_p->Mib802dot11;
	HostCmd_FW_USE_FIXED_RATE *pCmd = (HostCmd_FW_USE_FIXED_RATE *) &wlpptr->pCmdBuf[0];
	int retval = FAIL;

	unsigned long flags; 

	WLDBG_ENTER(DBG_LEVEL_0);

	MWL_SPIN_LOCK(&wlpptr->wlpd_p->locks.fwLock);
	memset(pCmd, 0x00, sizeof(HostCmd_FW_USE_FIXED_RATE));
	pCmd->CmdHdr.Cmd    = ENDIAN_SWAP16(HostCmd_CMD_SET_FIXED_RATE);
	pCmd->CmdHdr.Length = ENDIAN_SWAP16(sizeof(HostCmd_FW_USE_FIXED_RATE));
	pCmd->CmdHdr.macid = wlpptr->vmacSta_p->VMacEntry.macId;

	if (rate == 0)
		pCmd->Action = ENDIAN_SWAP32(HostCmd_ACT_NOT_USE_FIXED_RATE);
	else 
		pCmd->Action = ENDIAN_SWAP32(HostCmd_ACT_GEN_SET);

	pCmd->MulticastRate = *(mib->mib_MulticastRate);
	pCmd->MultiRateTxType = *(mib->mib_MultiRateTxType);
	pCmd->ManagementRate = *(mib->mib_ManagementRate);
	if (*(mib->mib_enableFixedRateTx) == 2)
	{
		pCmd->AllowRateDrop = ENDIAN_SWAP32(FIXED_RATE_WITHOUT_AUTORATE_DROP);
#ifdef SOC_W8864		
		if(*(mib->mib_FixedRateTxType) == 2)
		{   //fixed 11ac rate
            pCmd->FixedRateTable[0].FixRateTypeFlags.FixRateType = ENDIAN_SWAP32(2);
            pCmd->FixedRateTable[0].FixedRate = ENDIAN_SWAP32(*(mib->mib_txDataRateVHT));
		}
		else
#endif		
		{
		pCmd->FixedRateTable[0].FixRateTypeFlags.FixRateType = ENDIAN_SWAP32(*(mib->mib_FixedRateTxType));
		pCmd->FixedRateTable[0].FixedRate = ENDIAN_SWAP32(*(mib->mib_FixedRateTxType)?*(mib->mib_txDataRateN):*(mib->mib_txDataRate));
		}
		pCmd->EntryCount = ENDIAN_SWAP32(1);
		WLDBG_INFO(DBG_LEVEL_0,"%s rate at %i w/o auto drop",*(mib->mib_FixedRateTxType)?"HT":"legacy", *(mib->mib_FixedRateTxType)?*(mib->mib_txDataRateN):*(mib->mib_txDataRate));
	} else if(*(mib->mib_enableFixedRateTx) == 1)
	{
		UINT8 i;
		UINT32 RetryCount = 3;//Allow Auto rate with drop totally to send 12 packets(11 retries);
		pCmd->AllowRateDrop = ENDIAN_SWAP32(FIXED_RATE_WITH_AUTO_RATE_DROP);
		pCmd->FixedRateTable[0].FixRateTypeFlags.FixRateType = ENDIAN_SWAP32(*(mib->mib_FixedRateTxType));
		pCmd->FixedRateTable[0].FixedRate = ENDIAN_SWAP32(*(mib->mib_FixedRateTxType)?*(mib->mib_txDataRateN):*(mib->mib_txDataRate));

		pCmd->EntryCount = ENDIAN_SWAP32(4);
		for (i=0; i<4; i++)
		{
#ifdef SOC_W8864		
            if(*(mib->mib_FixedRateTxType) == 2)
            {   //fixed 11ac rate with drop
                UINT32 vht_mcs = *(mib->mib_txDataRateVHT) - i;
                pCmd->FixedRateTable[i].FixRateTypeFlags.FixRateType = ENDIAN_SWAP32(2);
                if((vht_mcs&0xf) > 9)
                {
                    vht_mcs = (vht_mcs&0xf0)|0x9;
                }
                pCmd->FixedRateTable[i].FixedRate = ENDIAN_SWAP32(vht_mcs);
            }
#endif            
			pCmd->FixedRateTable[i].FixRateTypeFlags.RetryCountValid = ENDIAN_SWAP32(RETRY_COUNT_VALID);
			pCmd->FixedRateTable[i].RetryCount = ENDIAN_SWAP32(RetryCount);
		}

		WLDBG_INFO(DBG_LEVEL_0,"%s rate at %i w/ auto drop",*(mib->mib_FixedRateTxType)?"HT":"legacy", *(mib->mib_FixedRateTxType)?*(mib->mib_txDataRateN):*(mib->mib_txDataRate));
	} else
	{
		pCmd->AllowRateDrop = ENDIAN_SWAP32(FIXED_RATE_WITH_AUTO_RATE_DROP);
		WLDBG_INFO(DBG_LEVEL_0,"auto rate %i",*(mib->mib_enableFixedRateTx));
	}
	WLDBG_DUMP_DATA(DBG_LEVEL_0,(void *)pCmd,sizeof(HostCmd_FW_USE_FIXED_RATE));
	retval = wlexecuteCommand(netdev, HostCmd_CMD_SET_FIXED_RATE);
	MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
	return retval;
}
int wlFwSetSlotTime(struct net_device *netdev, wlslot_e slot)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	HostCmd_FW_SET_SLOT *pCmd = (HostCmd_FW_SET_SLOT *) &wlpptr->pCmdBuf[0];
	int retval = FAIL;
	unsigned long flags;

	WLDBG_ENTER_INFO(DBG_LEVEL_0,
		"set slot: %s", (slot == WL_SHORTSLOT) ? "short" : "long");

	MWL_SPIN_LOCK(&wlpptr->wlpd_p->locks.fwLock);
	memset(pCmd, 0x00, sizeof(HostCmd_FW_SET_SLOT));
	pCmd->CmdHdr.Cmd    = ENDIAN_SWAP16(HostCmd_CMD_802_11_SET_SLOT);
	pCmd->CmdHdr.Length = ENDIAN_SWAP16(sizeof(HostCmd_FW_SET_SLOT));
	pCmd->Action        = ENDIAN_SWAP16(WL_SET);
	pCmd->Slot          = slot;
	pCmd->CmdHdr.macid = wlpptr->vmacSta_p->VMacEntry.macId;

	WLDBG_DUMP_DATA(DBG_LEVEL_0,(void *) pCmd,sizeof(HostCmd_FW_SET_SLOT));
	retval = wlexecuteCommand(netdev, HostCmd_CMD_802_11_SET_SLOT);
	MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
	return retval;
}
static int wlFwSettxpowers(struct net_device *netdev,  UINT16 txpow[], UINT8 action, 
                           UINT16 ch, UINT16 band, UINT16 width, UINT16 sub_ch)
{
	int retval, i;
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	HostCmd_DS_802_11_TX_POWER *pCmd = (HostCmd_DS_802_11_TX_POWER*)&wlpptr->pCmdBuf[0];
	unsigned long flags;

	MWL_SPIN_LOCK(&wlpptr->wlpd_p->locks.fwLock);
	memset(pCmd, 0x00, sizeof(HostCmd_DS_802_11_TX_POWER));
	pCmd->CmdHdr.Cmd    = ENDIAN_SWAP16(HostCmd_CMD_802_11_TX_POWER);
	pCmd->CmdHdr.Length = ENDIAN_SWAP16(sizeof(HostCmd_DS_802_11_TX_POWER));
	pCmd->Action        = ENDIAN_SWAP16(action);
	pCmd->CmdHdr.macid = wlpptr->vmacSta_p->VMacEntry.macId;
	pCmd->ch = ENDIAN_SWAP16(ch);
	pCmd->bw = ENDIAN_SWAP16(width);
	pCmd->band = ENDIAN_SWAP16(band);
	pCmd->sub_ch = ENDIAN_SWAP16(sub_ch);

	for (i = 0; i < TX_POWER_LEVEL_TOTAL; i++)
		pCmd->PowerLevelList[i] = ENDIAN_SWAP16(txpow[i]);

	retval = wlexecuteCommand(netdev, HostCmd_CMD_802_11_TX_POWER);
	MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
	return retval;
}


int wlFwSetTxPower(struct net_device *netdev, UINT8 flag, UINT32 powerLevel)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	vmacApInfo_t *vmacSta_p = wlpptr->vmacSta_p;
#if defined(SOC_W8366)||defined(SOC_W8764)
	UINT16 txpow[TX_POWER_LEVEL_TOTAL];
	int reduceVal=0;
	int i,index, found=0;
	MIB_802DOT11 *mibS = vmacSta_p->ShadowMib802dot11;
	MIB_802DOT11 *mibA = vmacSta_p->Mib802dot11;
	UINT16 tmp;
#ifdef PWRFRAC
	switch (*(mibS->mib_TxPwrFraction))
	{
	case 0:
		reduceVal = 0; /* Max */
		break;	
	case 1:
		reduceVal = 2; /* 75% -1.25db */
		break;	
	case 2:
		reduceVal = 3;  /* 50% -3db */
		break;	
	case 3:
		reduceVal = 6; /* 25% -6db */
		break;	

	default:
		reduceVal = *(mibS->mib_MaxTxPwr);	 /* larger than case 3,  pCmd->MaxPowerLevel is min */
		break;	
	}
#endif

	/* search tx power table if exist */
	for (index=0; index<IEEE_80211_MAX_NUMBER_OF_CHANNELS; index++)
	{
		/* do nothing if table is not loaded */
		if (mibS->PhyTXPowerTable[index]->Channel == 0)
			break;

		if (mibS->PhyTXPowerTable[index]->Channel == mibS->PhyDSSSTable->CurrChan)
		{
			*(mibS->mib_CDD) = *(mibA->mib_CDD) = mibS->PhyTXPowerTable[index]->CDD;
			*(mibS->mib_txAntenna2) = *(mibA->mib_txAntenna2) = mibS->PhyTXPowerTable[index]->txantenna2;

			if (mibS->PhyTXPowerTable[index]->setcap)
				mibS->PhyDSSSTable->powinited = 0x01;
			else
				mibS->PhyDSSSTable->powinited = 0x02;

			for (i =0; i<TX_POWER_LEVEL_TOTAL; i++)
			{
				if (mibS->PhyTXPowerTable[index]->setcap)
					mibS->PhyDSSSTable->maxTxPow[i] = mibS->PhyTXPowerTable[index]->TxPower[i];
				else
					mibS->PhyDSSSTable->targetPowers[i] = mibS->PhyTXPowerTable[index]->TxPower[i];
			}

			found = 1;
			break;
		}
	}

	if((mibS->PhyDSSSTable->powinited&1) == 0)
	{	
		wlFwGettxpower(netdev, mibS->PhyDSSSTable->targetPowers, mibS->PhyDSSSTable->CurrChan,
			mibS->PhyDSSSTable->Chanflag.FreqBand, mibS->PhyDSSSTable->Chanflag.ChnlWidth,
			mibS->PhyDSSSTable->Chanflag.ExtChnlOffset);
		mibS->PhyDSSSTable->powinited |=1;		
	}
	if((mibS->PhyDSSSTable->powinited&2) == 0)
	{	
		wlFwGettxpower(netdev, mibS->PhyDSSSTable->maxTxPow, mibS->PhyDSSSTable->CurrChan,
			mibS->PhyDSSSTable->Chanflag.FreqBand, mibS->PhyDSSSTable->Chanflag.ChnlWidth,
			mibS->PhyDSSSTable->Chanflag.ExtChnlOffset);
		mibS->PhyDSSSTable->powinited |=2;		
	}
	for (i = 0; i < TX_POWER_LEVEL_TOTAL; i++)
	{
		if (found) {
			if ((mibS->PhyTXPowerTable[index]->setcap) 
				&& (mibS->PhyTXPowerTable[index]->TxPower[i] > mibS->PhyDSSSTable->maxTxPow[i]))
				tmp = mibS->PhyDSSSTable->maxTxPow[i];
			else
				tmp = mibS->PhyTXPowerTable[index]->TxPower[i];	
		}
		else {
			if(mibS->PhyDSSSTable->targetPowers[i] > mibS->PhyDSSSTable->maxTxPow[i])
				tmp = mibS->PhyDSSSTable->maxTxPow[i];
			else
				tmp = mibS->PhyDSSSTable->targetPowers[i];
		}
		txpow[i] = ((tmp-reduceVal)>0)?(tmp-reduceVal):0;
	}
	return wlFwSettxpowers(netdev, txpow, HostCmd_ACT_GEN_SET_LIST, mibS->PhyDSSSTable->CurrChan,
			mibS->PhyDSSSTable->Chanflag.FreqBand, mibS->PhyDSSSTable->Chanflag.ChnlWidth,
			mibS->PhyDSSSTable->Chanflag.ExtChnlOffset);
#else
	MIB_802DOT11 *mib = vmacSta_p->Mib802dot11;
	HostCmd_DS_802_11_RF_TX_POWER *pCmd =
		(HostCmd_DS_802_11_RF_TX_POWER *) &wlpptr->pCmdBuf[0];
	int retval = FAIL;
	MIB_PHY_DSSS_TABLE *PhyDSSSTable=mib->PhyDSSSTable;
	UINT8 *mib_MaxTxPwr_p = mib->mib_MaxTxPwr;
	unsigned long flags;
	WLDBG_ENTER_INFO(DBG_LEVEL_0, "powerlevel: %i", powerLevel);

	MWL_SPIN_LOCK(&wlpptr->wlpd_p->locks.fwLock);
	memset(pCmd, 0x00, sizeof(HostCmd_DS_802_11_RF_TX_POWER));
	pCmd->CmdHdr.Cmd    = ENDIAN_SWAP16(HostCmd_CMD_802_11_RF_TX_POWER);
	pCmd->CmdHdr.Length = ENDIAN_SWAP16(sizeof(HostCmd_DS_802_11_RF_TX_POWER));
	pCmd->Action        = ENDIAN_SWAP16(flag);
	pCmd->CmdHdr.macid = wlpptr->vmacSta_p->VMacEntry.macId;

	if (flag == HostCmd_ACT_GEN_SET) 
	{
		if ((powerLevel >= 0) && (powerLevel < 30))
		{
			pCmd->SupportTxPowerLevel = ENDIAN_SWAP16(WL_TX_POWERLEVEL_LOW);
		} else if ((powerLevel >= 30) && (powerLevel < 60))
		{
			pCmd->SupportTxPowerLevel = ENDIAN_SWAP16(WL_TX_POWERLEVEL_MEDIUM);
		} else
		{
			pCmd->SupportTxPowerLevel = ENDIAN_SWAP16(WL_TX_POWERLEVEL_HIGH);
		}
	}
	else if (flag == HostCmd_ACT_GEN_SET_LIST)
	{
		UINT8 i,j;
		UINT8 chan = PhyDSSSTable->CurrChan;
		UINT8 ChanBw = PhyDSSSTable->Chanflag.ChnlWidth;

		if (chan <= 14) /* BG case and Channel 14 has been forced to 20M in wlset_freq() */
		{
			if ((ChanBw == CH_40_MHz_WIDTH) || (ChanBw == CH_AUTO_WIDTH))
			{
				/* Only for 40M, also auto bw is set 40M currently in wlFwSetChannel()*/
				if (PhyDSSSTable->Chanflag.ExtChnlOffset == EXT_CH_BELOW_CTRL_CH)
				{	
					if (chan > 4)
						chan -= 4;				
				}
			}
			for (i=0; i<4; i++)
			{
				if ((ChanBw == CH_40_MHz_WIDTH) || (ChanBw == CH_AUTO_WIDTH))
				{	
					if (chan > 9)
						pCmd->PowerLevelList[i] = mib->PowerTagetRateTable40M[(chan - 5)*4 + i];
					else
						pCmd->PowerLevelList[i] = mib->PowerTagetRateTable40M[(chan-1)*4 + i];
				}
				else
					pCmd->PowerLevelList[i] = mib->PowerTagetRateTable20M[(chan-1)*4 + i];		
#ifdef PWRFRAC
				if (i == 0) /* The first is max */
				{
					if (pCmd->PowerLevelList[i] < *mib_MaxTxPwr_p)
						*mib_MaxTxPwr_p = pCmd->PowerLevelList[i];	
					//printk("mib_MaxTxPwr %d chan %d\n", *mib_MaxTxPwr_p, chan);
				}
#endif
				pCmd->PowerLevelList[i] = ENDIAN_SWAP16(pCmd->PowerLevelList[i]);

			}

		}
		else /* A case */
		{
			int numEntry;

			if ((ChanBw == CH_40_MHz_WIDTH) || (ChanBw == CH_AUTO_WIDTH))
			{	
				if (PhyDSSSTable->Chanflag.ExtChnlOffset == EXT_CH_BELOW_CTRL_CH)
				{	
					chan -= 4;				
				}
				numEntry = PWTAGETRATETABLE40M_5G_ENTRY;
				for (j=0; j<numEntry; j++)
				{
					if (mib->PowerTagetRateTable40M_5G[j*4] == chan)
					{
						break;
					}
				}
			}
			else
			{
				numEntry = PWTAGETRATETABLE20M_5G_ENTRY;
				for (j=0; j<numEntry; j++)
				{
					if (mib->PowerTagetRateTable20M_5G[j*4] == chan)
					{
						break;
					}
				}
			}

			if (j >= numEntry)
			{
				MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
				return retval;
			}
			for (i=0; i<4; i++)
			{
				if ((ChanBw == CH_40_MHz_WIDTH) || (ChanBw == CH_AUTO_WIDTH))
					pCmd->PowerLevelList[i] = mib->PowerTagetRateTable40M_5G[j*4 + i];
				else
					pCmd->PowerLevelList[i] = mib->PowerTagetRateTable20M_5G[j*4 + i];

#ifdef PWRFRAC
				if (i == 1) /* The first is max */
				{
					if (pCmd->PowerLevelList[i] < *mib_MaxTxPwr_p)
						*mib_MaxTxPwr_p = pCmd->PowerLevelList[i];	
					//printk("mib_MaxTxPwr %d chan %d\n", *mib_MaxTxPwr_p, chan);
				}
#endif
				pCmd->PowerLevelList[i] = ENDIAN_SWAP16(pCmd->PowerLevelList[i]);
			}
		}
	}else
	{
		MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
		return retval;
	}
	WLDBG_DUMP_DATA(DBG_LEVEL_1, (void *) pCmd,
		sizeof(HostCmd_DS_802_11_RF_TX_POWER));
	retval = wlexecuteCommand(netdev, HostCmd_CMD_802_11_RF_TX_POWER);
	MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
	return retval;
#endif
}

int wlFwGettxpower(struct net_device *netdev,  UINT16 *powlist, UINT16 ch, 
				   UINT16 band, UINT16 width, UINT16 sub_ch)
{
	int i, retval;
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	HostCmd_DS_802_11_TX_POWER *pCmd = (HostCmd_DS_802_11_TX_POWER*)&wlpptr->pCmdBuf[0];
	unsigned long flags;

	MWL_SPIN_LOCK(&wlpptr->wlpd_p->locks.fwLock);
	memset(pCmd, 0x00, sizeof(HostCmd_DS_802_11_TX_POWER));
	pCmd->CmdHdr.Cmd    = ENDIAN_SWAP16(HostCmd_CMD_802_11_TX_POWER);
	pCmd->CmdHdr.Length = ENDIAN_SWAP16(sizeof(HostCmd_DS_802_11_TX_POWER));
	pCmd->CmdHdr.macid = wlpptr->vmacSta_p->VMacEntry.macId;
	pCmd->Action = ENDIAN_SWAP16(HostCmd_ACT_GEN_GET_LIST);
	pCmd->ch = ENDIAN_SWAP16(ch);
	pCmd->bw = ENDIAN_SWAP16(width);
	pCmd->band = ENDIAN_SWAP16(band);
	pCmd->sub_ch = ENDIAN_SWAP16(sub_ch);
	retval = wlexecuteCommand(netdev, HostCmd_CMD_802_11_TX_POWER);
	if (retval == 0) {
		for (i = 0; i < MWL_MAX_TXPOWER_ENTRIES; i++) {
			powlist[i] = (UINT8)ENDIAN_SWAP16(pCmd->PowerLevelList[i]);
		}
	}
	MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
	return retval;
}


int wlFwSetTxPowerClientScan(struct net_device *netdev, UINT8 flag, UINT32 powerLevel, UINT16 channel)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	vmacApInfo_t *vmacSta_p = wlpptr->vmacSta_p;
	MIB_802DOT11 *mib = vmacSta_p->Mib802dot11;

	HostCmd_DS_802_11_RF_TX_POWER *pCmd =
		(HostCmd_DS_802_11_RF_TX_POWER *) &wlpptr->pCmdBuf[0];
	int retval = FAIL;
	MIB_PHY_DSSS_TABLE *PhyDSSSTable=mib->PhyDSSSTable;
	UINT8 *mib_MaxTxPwr_p = mib->mib_MaxTxPwr;
	unsigned long flags;
	WLDBG_ENTER_INFO(DBG_LEVEL_0, "powerlevel: %i", powerLevel);

	MWL_SPIN_LOCK(&wlpptr->wlpd_p->locks.fwLock);
	memset(pCmd, 0x00, sizeof(HostCmd_DS_802_11_RF_TX_POWER));
	pCmd->CmdHdr.Cmd    = ENDIAN_SWAP16(HostCmd_CMD_802_11_RF_TX_POWER);
	pCmd->CmdHdr.Length = ENDIAN_SWAP16(sizeof(HostCmd_DS_802_11_RF_TX_POWER));
	pCmd->Action        = ENDIAN_SWAP16(flag);
	pCmd->CmdHdr.macid = wlpptr->vmacSta_p->VMacEntry.macId;

	if (flag == HostCmd_ACT_GEN_SET) 
	{
		if ((powerLevel >= 0) && (powerLevel < 30))
		{
			pCmd->SupportTxPowerLevel = ENDIAN_SWAP16(WL_TX_POWERLEVEL_LOW);
		} else if ((powerLevel >= 30) && (powerLevel < 60))
		{
			pCmd->SupportTxPowerLevel = ENDIAN_SWAP16(WL_TX_POWERLEVEL_MEDIUM);
		} else
		{
			pCmd->SupportTxPowerLevel = ENDIAN_SWAP16(WL_TX_POWERLEVEL_HIGH);
		}
	}
	else if (flag == HostCmd_ACT_GEN_SET_LIST)
	{
		UINT8 i;
		UINT8 chan = channel;
		UINT8 ChanBw = PhyDSSSTable->Chanflag.ChnlWidth;

		if (chan <= 14) /* BG case and Channel 14 has been forced to 20M in wlset_freq() */
		{
			if ((ChanBw == CH_40_MHz_WIDTH) || (ChanBw == CH_AUTO_WIDTH))
			{
				/* Only for 40M, also auto bw is set 40M currently in wlFwSetChannel()*/
				if (PhyDSSSTable->Chanflag.ExtChnlOffset == EXT_CH_BELOW_CTRL_CH)
				{	
					if (chan > 4)
						chan -= 4;				
				}
			}
			for (i=0; i<4; i++)
			{
				if ((ChanBw == CH_40_MHz_WIDTH) || (ChanBw == CH_AUTO_WIDTH))
				{	
					if (chan > 9)
						pCmd->PowerLevelList[i] = mib->PowerTagetRateTable40M[(chan - 5)*4 + i];
					else
						pCmd->PowerLevelList[i] = mib->PowerTagetRateTable40M[(chan-1)*4 + i];
				}
				else
					pCmd->PowerLevelList[i] = mib->PowerTagetRateTable20M[(chan-1)*4 + i];		
#ifdef PWRFRAC
				if (i == 0) /* The first is max */
				{
					if (pCmd->PowerLevelList[i] < *mib_MaxTxPwr_p)
						*mib_MaxTxPwr_p = pCmd->PowerLevelList[i];	
					//printk("mib_MaxTxPwr %d chan %d\n", *mib_MaxTxPwr_p, chan);
				}
#endif
				pCmd->PowerLevelList[i] = ENDIAN_SWAP16(pCmd->PowerLevelList[i]);
			}

		}
		else /* A case */
		{
			// Todo
			MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
			return SUCCESS;
		}
	}else
	{
		MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
		return retval;
	}
	WLDBG_DUMP_DATA(DBG_LEVEL_0, (void *) pCmd,
		sizeof(HostCmd_DS_802_11_RF_TX_POWER));
	retval = wlexecuteCommand(netdev, HostCmd_CMD_802_11_RF_TX_POWER);
	MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
	return retval;
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,35)
int wlFwSetMcast(struct net_device *netdev, struct netdev_hw_addr *mcAddr)
#else
int wlFwSetMcast(struct net_device *netdev, struct dev_mc_list *mcAddr)
#endif
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	int retval = FAIL;
	unsigned int num = 0;
	HostCmd_DS_MAC_MULTICAST_ADR *pCmd =
		(HostCmd_DS_MAC_MULTICAST_ADR *) &wlpptr->pCmdBuf[0];

	unsigned long flags; 

	WLDBG_ENTER(DBG_LEVEL_0);

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,35)
	if (netdev_mc_count(netdev) == 0)
#else
	if (netdev->mc_count == 0)
#endif
	{
		WLDBG_WARNING(DBG_LEVEL_0, "set of 0 multicast addresses");
		return SUCCESS;
	}

	MWL_SPIN_LOCK(&wlpptr->wlpd_p->locks.fwLock);
	memset(pCmd, 0x00, sizeof(HostCmd_DS_MAC_MULTICAST_ADR));
	pCmd->CmdHdr.Cmd    = ENDIAN_SWAP16(HostCmd_CMD_MAC_MULTICAST_ADR);
	pCmd->CmdHdr.Length = ENDIAN_SWAP16(sizeof(HostCmd_DS_MAC_MULTICAST_ADR));
	pCmd->CmdHdr.macid = wlpptr->vmacSta_p->VMacEntry.macId;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,35)
	netdev_for_each_mc_addr(mcAddr, netdev) {
		memcpy(&pCmd->MACList[(num*ETH_ALEN)], mcAddr->addr, ETH_ALEN);
		num++;
		if (num > HostCmd_MAX_MCAST_ADRS)
		{
			break;
		}
	}
#else
	for (; num < netdev->mc_count ; mcAddr = mcAddr->next)
	{
		memcpy(&pCmd->MACList[(num*ETH_ALEN)],&mcAddr->dmi_addr[0],ETH_ALEN);
		num++;
		if (num > HostCmd_MAX_MCAST_ADRS)
		{
			break;
		}
	}
#endif

	pCmd->NumOfAdrs = ENDIAN_SWAP16(num);
	pCmd->Action    = ENDIAN_SWAP16(0xffff);
	WLDBG_DUMP_DATA(DBG_LEVEL_0, 
		(void *) pCmd, sizeof(HostCmd_DS_MAC_MULTICAST_ADR));
	retval = wlexecuteCommand(netdev, HostCmd_CMD_MAC_MULTICAST_ADR);
	MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
	return retval;
}

int wlFwSetWep(struct net_device *netdev, u_int8_t *staaddr)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	vmacApInfo_t *vmacSta_p = wlpptr->vmacSta_p;
	MIB_802DOT11 *mib = vmacSta_p->Mib802dot11;
	UINT32 keyIndex;
	UINT16 keyLen;
	HostCmd_FW_UPDATE_ENCRYPTION_SET_KEY *pCmd = (HostCmd_FW_UPDATE_ENCRYPTION_SET_KEY *) &wlpptr->pCmdBuf[0];
	HostCmd_FW_UPDATE_ENCRYPTION *pCmd2 = (HostCmd_FW_UPDATE_ENCRYPTION *) &wlpptr->pCmdBuf[0];
	int retval = FAIL;
	unsigned long flags; 

	WLDBG_ENTER(DBG_LEVEL_0);
	if (!mib->Privacy->PrivInvoked)
		return SUCCESS;
	MWL_SPIN_LOCK(&wlpptr->wlpd_p->locks.fwLock);

	if(mib->AuthAlg->Type == AUTH_SHARED_KEY)
	{
		/* RESTRICTED */
		if (!*(mib->mib_strictWepShareKey))
			mib->AuthAlg->Type = AUTH_OPEN_OR_SHARED_KEY;
	}

	if (mib->WepDefaultKeys[*(mib->mib_defaultkeyindex)].WepType == 2)
		keyLen  = ENDIAN_SWAP16(WEP_KEY_104_BIT_LEN);
	else
		keyLen  = ENDIAN_SWAP16(WEP_KEY_40_BIT_LEN);
	for (keyIndex = 0 ; keyIndex < 4; keyIndex++)
	{
		memset(pCmd, 0x00, sizeof(HostCmd_FW_UPDATE_ENCRYPTION_SET_KEY));
		pCmd->CmdHdr.Cmd    = ENDIAN_SWAP16(HostCmd_CMD_UPDATE_ENCRYPTION);
		pCmd->CmdHdr.Length = ENDIAN_SWAP16(sizeof(HostCmd_FW_UPDATE_ENCRYPTION_SET_KEY));
		pCmd->CmdHdr.macid = wlpptr->vmacSta_p->VMacEntry.macId;
		pCmd->ActionType    = ENDIAN_SWAP32(EncrActionTypeSetKey);
		pCmd->KeyParam.Length      = ENDIAN_SWAP16(sizeof(pCmd->KeyParam));
		pCmd->KeyParam.KeyTypeId   = ENDIAN_SWAP16(KEY_TYPE_ID_WEP);
		if (keyIndex == *(mib->mib_defaultkeyindex))
			pCmd->KeyParam.KeyInfo     = ENDIAN_SWAP32(ENCR_KEY_FLAG_WEP_TXKEY);
		pCmd->KeyParam.KeyIndex    = ENDIAN_SWAP32(keyIndex);
		pCmd->KeyParam.KeyLen      = keyLen;
		memcpy(pCmd->KeyParam.Key.WepKey.KeyMaterial, mib->WepDefaultKeys[keyIndex].WepDefaultKeyValue, pCmd->KeyParam.KeyLen);
		memcpy(&pCmd->KeyParam.Macaddr[0], staaddr, 6);
		WLDBG_INFO(DBG_LEVEL_0, "HostCmd_FW_UPDATE_ENCRYPTION_KEY_SET Len = %d pCmd->KeyParam = %d\n", 
			sizeof(HostCmd_FW_UPDATE_ENCRYPTION_SET_KEY), sizeof(pCmd->KeyParam));
		WLDBG_DUMP_DATA(DBG_LEVEL_0, (void *) pCmd, sizeof(HostCmd_FW_UPDATE_ENCRYPTION_SET_KEY));

		retval = wlexecuteCommand(netdev, HostCmd_CMD_UPDATE_ENCRYPTION);
	}
	memset(pCmd2, 0x00, sizeof(HostCmd_FW_UPDATE_ENCRYPTION));
	pCmd2->CmdHdr.Cmd    = ENDIAN_SWAP16(HostCmd_CMD_UPDATE_ENCRYPTION);
	pCmd2->CmdHdr.Length = ENDIAN_SWAP16(sizeof(HostCmd_FW_UPDATE_ENCRYPTION));
	pCmd2->CmdHdr.macid = wlpptr->vmacSta_p->VMacEntry.macId;
	pCmd2->ActionType    = ENDIAN_SWAP32(EncrActionEnableHWEncryption);
	pCmd2->ActionData[0] = 0;        
	retval = wlexecuteCommand(netdev, HostCmd_CMD_UPDATE_ENCRYPTION);
	MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
	return retval;
}
int wlFwSetWpaTkipMode(struct net_device *netdev,  u_int8_t *staaddr)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	vmacApInfo_t *vmacSta_p = wlpptr->vmacSta_p;
	MIB_802DOT11 *mib = vmacSta_p->Mib802dot11;
	HostCmd_FW_UPDATE_ENCRYPTION *pCmd = (HostCmd_FW_UPDATE_ENCRYPTION *) &wlpptr->pCmdBuf[0];
	int retval = FAIL;
	unsigned long flags; 

	WLDBG_ENTER(DBG_LEVEL_0);
	if (!mib->Privacy->RSNEnabled)
		return SUCCESS;
	MWL_SPIN_LOCK(&wlpptr->wlpd_p->locks.fwLock);
	memset(pCmd, 0x00, sizeof(HostCmd_FW_UPDATE_ENCRYPTION));
	pCmd->CmdHdr.Cmd    = ENDIAN_SWAP16(HostCmd_CMD_UPDATE_ENCRYPTION);
	pCmd->CmdHdr.Length = ENDIAN_SWAP16(sizeof(HostCmd_FW_UPDATE_ENCRYPTION));
	pCmd->CmdHdr.macid = wlpptr->vmacSta_p->VMacEntry.macId;
	pCmd->ActionType    = ENDIAN_SWAP32(EncrActionEnableHWEncryption);
	pCmd->ActionData[0] = EncrTypeTkip;
	memcpy(&pCmd->macaddr[0], staaddr, 6);
	retval = wlexecuteCommand(netdev, HostCmd_CMD_UPDATE_ENCRYPTION);

	MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
	return retval;
}


int wlFwSetWpaWpa2PWK(struct net_device *netdev, extStaDb_StaInfo_t *StaInfo_p)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	vmacApInfo_t *vmacSta_p = wlpptr->vmacSta_p;
	MIB_802DOT11 *mib = vmacSta_p->Mib802dot11;
	HostCmd_FW_UPDATE_ENCRYPTION_SET_KEY *pCmd = (HostCmd_FW_UPDATE_ENCRYPTION_SET_KEY *) &wlpptr->pCmdBuf[0];
	//HostCmd_FW_UPDATE_ENCRYPTION *pCmd2 = (HostCmd_FW_UPDATE_ENCRYPTION *) &wlpptr->pCmdBuf[0];
	int retval = FAIL;
	unsigned long flags; 

	WLDBG_ENTER(DBG_LEVEL_0);
	if (!mib->Privacy->RSNEnabled)
		return SUCCESS;
	MWL_SPIN_LOCK(&wlpptr->wlpd_p->locks.fwLock);
	memset(pCmd, 0x00, sizeof(HostCmd_FW_UPDATE_ENCRYPTION_SET_KEY));
	pCmd->CmdHdr.Cmd    = ENDIAN_SWAP16(HostCmd_CMD_UPDATE_ENCRYPTION);
	pCmd->CmdHdr.Length = ENDIAN_SWAP16(sizeof(HostCmd_FW_UPDATE_ENCRYPTION_SET_KEY));
	pCmd->CmdHdr.macid = wlpptr->vmacSta_p->VMacEntry.macId;
	pCmd->ActionType    = ENDIAN_SWAP32(EncrActionTypeSetKey);
	pCmd->KeyParam.Length      = ENDIAN_SWAP16(sizeof(pCmd->KeyParam));
	if ( (StaInfo_p->keyMgmtStateInfo.RsnIEBuf[0] == 221 && StaInfo_p->keyMgmtStateInfo.RsnIEBuf[17] == 2)
		|| (StaInfo_p->keyMgmtStateInfo.RsnIEBuf[0] == 48  && StaInfo_p->keyMgmtStateInfo.RsnIEBuf[13] == 2))
	{   
		// TKIP
		pCmd->KeyParam.KeyTypeId   = ENDIAN_SWAP16(KEY_TYPE_ID_TKIP);
		pCmd->KeyParam.KeyInfo     = ENDIAN_SWAP32(ENCR_KEY_FLAG_PAIRWISE  |
			ENCR_KEY_FLAG_TSC_VALID |
			ENCR_KEY_FLAG_MICKEY_VALID);
		pCmd->KeyParam.KeyIndex    = 0;
		pCmd->KeyParam.KeyLen  = ENDIAN_SWAP16(sizeof(TKIP_TYPE_KEY));
		memcpy(pCmd->KeyParam.Key.TkipKey.KeyMaterial,StaInfo_p->keyMgmtStateInfo.PairwiseTempKey1, MAX_ENCR_KEY_LENGTH);
		memcpy(pCmd->KeyParam.Key.TkipKey.TkipTxMicKey, StaInfo_p->keyMgmtStateInfo.RSNPwkTxMICKey, MIC_KEY_LENGTH);
		memcpy(pCmd->KeyParam.Key.TkipKey.TkipRxMicKey, StaInfo_p->keyMgmtStateInfo.RSNPwkRxMICKey, MIC_KEY_LENGTH);
		pCmd->KeyParam.Key.TkipKey.TkipRsc.low = 0;
		pCmd->KeyParam.Key.TkipKey.TkipRsc.high = 0;
		pCmd->KeyParam.Key.TkipKey.TkipTsc.low = ENDIAN_SWAP16(StaInfo_p->keyMgmtStateInfo.TxIV16); //= 0;
		pCmd->KeyParam.Key.TkipKey.TkipTsc.high = ENDIAN_SWAP32(StaInfo_p->keyMgmtStateInfo.TxIV32); //= = 0;
		memcpy(pCmd->KeyParam.Macaddr, StaInfo_p->Addr, 6);
		WLDBG_INFO(DBG_LEVEL_0, "HostCmd_FW_UPDATE_ENCRYPTION_KEY_SET WPA TKIP Len = %d pCmd->KeyParam = %d", 
			sizeof(HostCmd_FW_UPDATE_ENCRYPTION_SET_KEY), sizeof(pCmd->KeyParam));
	}
	else if ( (StaInfo_p->keyMgmtStateInfo.RsnIEBuf[0] == 221 && StaInfo_p->keyMgmtStateInfo.RsnIEBuf[17] == 4)
		|| (StaInfo_p->keyMgmtStateInfo.RsnIEBuf[0] == 48  && StaInfo_p->keyMgmtStateInfo.RsnIEBuf[13] == 4))
	{
		// AES
		pCmd->KeyParam.KeyTypeId   = ENDIAN_SWAP16(KEY_TYPE_ID_AES);
		pCmd->KeyParam.KeyInfo     = ENDIAN_SWAP32(ENCR_KEY_FLAG_PAIRWISE);
		pCmd->KeyParam.KeyIndex    = 0; // NA for wpa
		pCmd->KeyParam.KeyLen  = ENDIAN_SWAP16(sizeof(AES_TYPE_KEY));

		memcpy(pCmd->KeyParam.Key.AesKey.KeyMaterial,StaInfo_p->keyMgmtStateInfo.PairwiseTempKey1, TK_SIZE);
		memcpy(pCmd->KeyParam.Macaddr, StaInfo_p->Addr, 6);
		WLDBG_INFO(DBG_LEVEL_0, "HostCmd_FW_UPDATE_ENCRYPTION_KEY_SET WPA AES Len = %d pCmd->KeyParam = %d", 
			sizeof(HostCmd_FW_UPDATE_ENCRYPTION_SET_KEY), sizeof(pCmd->KeyParam));
	}
	else
		return retval;
	memcpy(pCmd->KeyParam.Macaddr, StaInfo_p->Addr, 6);
	WLDBG_DUMP_DATA(DBG_LEVEL_0, (void *) pCmd, sizeof(HostCmd_FW_UPDATE_ENCRYPTION_SET_KEY));
	retval = wlexecuteCommand(netdev, HostCmd_CMD_UPDATE_ENCRYPTION);

	MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
	return retval;
}

#ifdef MRVL_WAPI 
int wlFwSetWapiKey(struct net_device *netdev, struct wlreq_wapi_key *wapi_key, int groupkey)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	vmacApInfo_t *vmacSta_p = wlpptr->vmacSta_p;
	HostCmd_FW_UPDATE_ENCRYPTION_SET_KEY *pCmd = (HostCmd_FW_UPDATE_ENCRYPTION_SET_KEY *) &wlpptr->pCmdBuf[0];
	int retval = FAIL;
	unsigned long flags; 

	WLDBG_ENTER(DBG_LEVEL_0);
	MWL_SPIN_LOCK(&wlpptr->wlpd_p->locks.fwLock);
	memset(pCmd, 0x00, sizeof(HostCmd_FW_UPDATE_ENCRYPTION_SET_KEY));
	pCmd->CmdHdr.Cmd    = ENDIAN_SWAP16(HostCmd_CMD_UPDATE_ENCRYPTION);
	pCmd->CmdHdr.Length = ENDIAN_SWAP16(sizeof(HostCmd_FW_UPDATE_ENCRYPTION_SET_KEY));
	pCmd->CmdHdr.macid = wlpptr->vmacSta_p->VMacEntry.macId;

	if (groupkey) 
		pCmd->ActionType    = ENDIAN_SWAP32(EncrActionTypeSetGroupKey);
	else
		pCmd->ActionType    = ENDIAN_SWAP32(EncrActionTypeSetKey);

	pCmd->KeyParam.Length      = ENDIAN_SWAP16(sizeof(pCmd->KeyParam));

	pCmd->KeyParam.KeyTypeId   = ENDIAN_SWAP16(KEY_TYPE_ID_WAPI);
	pCmd->KeyParam.KeyInfo     = ENDIAN_SWAP32(ENCR_KEY_FLAG_PAIRWISE);
	pCmd->KeyParam.KeyIndex    = wapi_key->ik_keyid;
	pCmd->KeyParam.KeyLen  = ENDIAN_SWAP16(sizeof(WAPI_TYPE_KEY));

	memcpy(pCmd->KeyParam.Key.WapiKey.KeyMaterial, &(wapi_key->ik_keydata[0]), KEY_LEN);
	memcpy(pCmd->KeyParam.Key.WapiKey.MicKeyMaterial, &(wapi_key->ik_keydata[16]), KEY_LEN);

	WLDBG_INFO(DBG_LEVEL_0, "HostCmd_FW_UPDATE_ENCRYPTION_KEY_SET WAPI Len = %d pCmd->KeyParam = %d", 
		sizeof(HostCmd_FW_UPDATE_ENCRYPTION_SET_KEY), sizeof(pCmd->KeyParam));

	memcpy(pCmd->KeyParam.Macaddr, wapi_key->ik_macaddr, 6);
	WLDBG_DUMP_DATA(DBG_LEVEL_0, (void *) pCmd, sizeof(HostCmd_FW_UPDATE_ENCRYPTION_SET_KEY));
	retval = wlexecuteCommand(netdev, HostCmd_CMD_UPDATE_ENCRYPTION);

	MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);

	return retval;
}
#endif

int wlFwSetMixedWpaWpa2Mode(struct net_device *netdev,  u_int8_t *staaddr)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	vmacApInfo_t *vmacSta_p = wlpptr->vmacSta_p;
	MIB_802DOT11 *mib = vmacSta_p->Mib802dot11;
	HostCmd_FW_UPDATE_ENCRYPTION *pCmd = (HostCmd_FW_UPDATE_ENCRYPTION *) &wlpptr->pCmdBuf[0];
	int retval = FAIL;
	unsigned long flags; 

	WLDBG_ENTER(DBG_LEVEL_0);
	if (!mib->Privacy->RSNEnabled)
		return SUCCESS;
	MWL_SPIN_LOCK(&wlpptr->wlpd_p->locks.fwLock);

	memset(pCmd, 0x00, sizeof(HostCmd_FW_UPDATE_ENCRYPTION));
	pCmd->CmdHdr.Cmd    = ENDIAN_SWAP16(HostCmd_CMD_UPDATE_ENCRYPTION);
	pCmd->CmdHdr.Length = ENDIAN_SWAP16(sizeof(HostCmd_FW_UPDATE_ENCRYPTION));
	pCmd->CmdHdr.macid = wlpptr->vmacSta_p->VMacEntry.macId;
	pCmd->ActionType    = ENDIAN_SWAP32(EncrActionEnableHWEncryption);
	pCmd->ActionData[0] = EncrTypeMix;
	memcpy(&pCmd->macaddr[0], staaddr, 6);
	retval = wlexecuteCommand(netdev, HostCmd_CMD_UPDATE_ENCRYPTION);

	MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
	return retval;
}

int wlFwSetWpaTkipGroupK(struct net_device *netdev, UINT8 index)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	vmacApInfo_t *vmacSta_p = wlpptr->vmacSta_p;
	MIB_802DOT11 *mib = vmacSta_p->Mib802dot11;
	HostCmd_FW_UPDATE_ENCRYPTION_SET_KEY *pCmd = (HostCmd_FW_UPDATE_ENCRYPTION_SET_KEY *) &wlpptr->pCmdBuf[0];
	int retval = FAIL;

	unsigned long flags; 

	WLDBG_ENTER(DBG_LEVEL_0);
	if (!mib->Privacy->RSNEnabled)
		return SUCCESS;
	MWL_SPIN_LOCK(&wlpptr->wlpd_p->locks.fwLock);
	memset(pCmd, 0x00, sizeof(HostCmd_FW_UPDATE_ENCRYPTION_SET_KEY));
	pCmd->CmdHdr.Cmd    = ENDIAN_SWAP16(HostCmd_CMD_UPDATE_ENCRYPTION);
	pCmd->CmdHdr.Length = ENDIAN_SWAP16(sizeof(HostCmd_FW_UPDATE_ENCRYPTION_SET_KEY));
	pCmd->CmdHdr.macid = wlpptr->vmacSta_p->VMacEntry.macId;
	pCmd->ActionType    = ENDIAN_SWAP32(EncrActionTypeSetGroupKey);
	pCmd->KeyParam.Length      = ENDIAN_SWAP16(sizeof(pCmd->KeyParam));
	pCmd->KeyParam.KeyTypeId   = ENDIAN_SWAP16(KEY_TYPE_ID_TKIP);
	pCmd->KeyParam.KeyInfo     = ENDIAN_SWAP32(ENCR_KEY_FLAG_TXGROUPKEY   |
		ENCR_KEY_FLAG_MICKEY_VALID |
		ENCR_KEY_FLAG_TSC_VALID);
	pCmd->KeyParam.KeyIndex    = ENDIAN_SWAP32(index);
	pCmd->KeyParam.KeyLen  = ENDIAN_SWAP16(sizeof(TKIP_TYPE_KEY));
	memcpy(pCmd->KeyParam.Key.TkipKey.KeyMaterial,mib->mib_MrvlRSN_GrpKey->EncryptKey, MAX_ENCR_KEY_LENGTH);
	memcpy(pCmd->KeyParam.Key.TkipKey.TkipTxMicKey, mib->mib_MrvlRSN_GrpKey->TxMICKey, MIC_KEY_LENGTH);
	memcpy(pCmd->KeyParam.Key.TkipKey.TkipRxMicKey, mib->mib_MrvlRSN_GrpKey->RxMICKey, MIC_KEY_LENGTH);
	pCmd->KeyParam.Key.TkipKey.TkipRsc.low = 0;
	pCmd->KeyParam.Key.TkipKey.TkipRsc.high = 0;
	pCmd->KeyParam.Key.TkipKey.TkipTsc.low = ENDIAN_SWAP16(mib->mib_MrvlRSN_GrpKey->g_IV16); //= 0;
	pCmd->KeyParam.Key.TkipKey.TkipTsc.high = ENDIAN_SWAP32(mib->mib_MrvlRSN_GrpKey->g_IV32);// = 0;
	memcpy(pCmd->KeyParam.Macaddr, &vmacSta_p->macStaAddr[0], 6);

	WLDBG_INFO(DBG_LEVEL_0, "HostCmd_FW_UPDATE_ENCRYPTION_KEY_SET Len = %d pCmd->KeyParam = %d", 
		sizeof(HostCmd_FW_UPDATE_ENCRYPTION_SET_KEY), sizeof(pCmd->KeyParam));
	WLDBG_DUMP_DATA(DBG_LEVEL_0, (void *) pCmd, sizeof(HostCmd_FW_UPDATE_ENCRYPTION_SET_KEY));
	retval = wlexecuteCommand(netdev, HostCmd_CMD_UPDATE_ENCRYPTION);

	MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
	return retval;
}

int
wlFwSetWpaAesPWK(struct net_device *netdev, extStaDb_StaInfo_t *StaInfo_p)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	vmacApInfo_t *vmacSta_p = wlpptr->vmacSta_p;
	MIB_802DOT11 *mib = vmacSta_p->Mib802dot11;
	HostCmd_FW_UPDATE_ENCRYPTION_SET_KEY *pCmd = (HostCmd_FW_UPDATE_ENCRYPTION_SET_KEY *) &wlpptr->pCmdBuf[0];
	//HostCmd_FW_UPDATE_ENCRYPTION *pCmd2 = (HostCmd_FW_UPDATE_ENCRYPTION *) &wlpptr->pCmdBuf[0];
	int retval = FAIL;
	unsigned long flags; 

	WLDBG_ENTER(DBG_LEVEL_0);
	if (!mib->Privacy->RSNEnabled)
		return SUCCESS;
	MWL_SPIN_LOCK(&wlpptr->wlpd_p->locks.fwLock);
	memset(pCmd, 0x00, sizeof(HostCmd_FW_UPDATE_ENCRYPTION_SET_KEY));
	pCmd->CmdHdr.Cmd    = ENDIAN_SWAP16(HostCmd_CMD_UPDATE_ENCRYPTION);
	pCmd->CmdHdr.Length = ENDIAN_SWAP16(sizeof(HostCmd_FW_UPDATE_ENCRYPTION_SET_KEY));
	pCmd->CmdHdr.macid = wlpptr->vmacSta_p->VMacEntry.macId;
	pCmd->ActionType    = ENDIAN_SWAP32(EncrActionTypeSetKey);
	pCmd->KeyParam.Length      = ENDIAN_SWAP16(sizeof(pCmd->KeyParam));
	pCmd->KeyParam.KeyTypeId   = ENDIAN_SWAP16(KEY_TYPE_ID_AES);
	pCmd->KeyParam.KeyInfo     = ENDIAN_SWAP32(ENCR_KEY_FLAG_PAIRWISE);
	pCmd->KeyParam.KeyIndex    = 0; // NA for wpa
	pCmd->KeyParam.KeyLen  = ENDIAN_SWAP16(sizeof(AES_TYPE_KEY));

	memcpy(pCmd->KeyParam.Key.AesKey.KeyMaterial,StaInfo_p->keyMgmtStateInfo.PairwiseTempKey1, TK_SIZE);
	memcpy(pCmd->KeyParam.Macaddr, StaInfo_p->Addr, 6);
	WLDBG_INFO(DBG_LEVEL_0, "HostCmd_FW_UPDATE_ENCRYPTION_KEY_SET WPA AES Len = %d pCmd->KeyParam = %d", 
		sizeof(HostCmd_FW_UPDATE_ENCRYPTION_SET_KEY), sizeof(pCmd->KeyParam));
	WLDBG_DUMP_DATA(DBG_LEVEL_0, (void *) pCmd, sizeof(HostCmd_FW_UPDATE_ENCRYPTION_SET_KEY));
	retval = wlexecuteCommand(netdev, HostCmd_CMD_UPDATE_ENCRYPTION);

	MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
	return retval;
}

int wlFwSetWpaAesMode(struct net_device *netdev,  u_int8_t *staaddr)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	vmacApInfo_t *vmacSta_p = wlpptr->vmacSta_p;
	MIB_802DOT11 *mib = vmacSta_p->Mib802dot11;
	HostCmd_FW_UPDATE_ENCRYPTION *pCmd = (HostCmd_FW_UPDATE_ENCRYPTION *) &wlpptr->pCmdBuf[0];
	int retval = FAIL;
	unsigned long flags; 

	WLDBG_ENTER(DBG_LEVEL_0);
	if (!mib->Privacy->RSNEnabled)
		return SUCCESS;
	MWL_SPIN_LOCK(&wlpptr->wlpd_p->locks.fwLock);
	memset(pCmd, 0x00, sizeof(HostCmd_FW_UPDATE_ENCRYPTION));
	pCmd->CmdHdr.Cmd    = ENDIAN_SWAP16(HostCmd_CMD_UPDATE_ENCRYPTION);
	pCmd->CmdHdr.Length = ENDIAN_SWAP16(sizeof(HostCmd_FW_UPDATE_ENCRYPTION));
	pCmd->CmdHdr.macid = wlpptr->vmacSta_p->VMacEntry.macId;
	pCmd->ActionType    = ENDIAN_SWAP32(EncrActionEnableHWEncryption);
	pCmd->ActionData[0] = EncrTypeAes;
	memcpy(&pCmd->macaddr[0], staaddr, 6);
	retval = wlexecuteCommand(netdev, HostCmd_CMD_UPDATE_ENCRYPTION);

	MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
	return retval;
}


int
wlFwSetWpaAesGroupK(struct net_device *netdev, UINT8 index)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	vmacApInfo_t *vmacSta_p = wlpptr->vmacSta_p;
	MIB_802DOT11 *mib = vmacSta_p->Mib802dot11;
	HostCmd_FW_UPDATE_ENCRYPTION_SET_KEY *pCmd = (HostCmd_FW_UPDATE_ENCRYPTION_SET_KEY *) &wlpptr->pCmdBuf[0];
	int retval = FAIL;
	unsigned long flags; 

	WLDBG_ENTER(DBG_LEVEL_0);
	if (!mib->Privacy->RSNEnabled)
		return SUCCESS;
	MWL_SPIN_LOCK(&wlpptr->wlpd_p->locks.fwLock);
	memset(pCmd, 0x00, sizeof(HostCmd_FW_UPDATE_ENCRYPTION_SET_KEY));
	pCmd->CmdHdr.Cmd    = ENDIAN_SWAP16(HostCmd_CMD_UPDATE_ENCRYPTION);
	pCmd->CmdHdr.Length = ENDIAN_SWAP16(sizeof(HostCmd_FW_UPDATE_ENCRYPTION_SET_KEY));
	pCmd->CmdHdr.macid = wlpptr->vmacSta_p->VMacEntry.macId;
	pCmd->ActionType    = ENDIAN_SWAP32(EncrActionTypeSetGroupKey);
	pCmd->KeyParam.Length      = ENDIAN_SWAP16(sizeof(pCmd->KeyParam));
	pCmd->KeyParam.KeyTypeId   = ENDIAN_SWAP16(KEY_TYPE_ID_AES);
	pCmd->KeyParam.KeyInfo     = ENDIAN_SWAP32(ENCR_KEY_FLAG_TXGROUPKEY);
	pCmd->KeyParam.KeyIndex    = ENDIAN_SWAP32(index); // NA for wpa
	pCmd->KeyParam.KeyLen  = ENDIAN_SWAP16(sizeof(AES_TYPE_KEY));

	memcpy(pCmd->KeyParam.Key.AesKey.KeyMaterial,mib->mib_MrvlRSN_GrpKey->EncryptKey, TK_SIZE);
	memcpy(pCmd->KeyParam.Macaddr, &vmacSta_p->macStaAddr[0], 6);

	WLDBG_INFO(DBG_LEVEL_0, "HostCmd_FW_UPDATE_ENCRYPTION_KEY_SET WPA AES Group Key Len = %d pCmd->KeyParam = %d", 
		sizeof(HostCmd_FW_UPDATE_ENCRYPTION_SET_KEY), sizeof(pCmd->KeyParam));
	WLDBG_DUMP_DATA(DBG_LEVEL_0, (void *) pCmd, sizeof(HostCmd_FW_UPDATE_ENCRYPTION_SET_KEY));
	retval = wlexecuteCommand(netdev, HostCmd_CMD_UPDATE_ENCRYPTION);

	MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
	return retval;
}

#ifdef WPA_STA
int wlFwSetWpaWpa2PWK_STA(struct net_device *netdev, extStaDb_StaInfo_t *StaInfo_p)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	//vmacApInfo_t *vmacSta_p = wlpptr->vmacSta_p;
	//MIB_802DOT11 *mib = vmacSta_p->Mib802dot11;
	HostCmd_FW_UPDATE_ENCRYPTION_SET_KEY *pCmd = (HostCmd_FW_UPDATE_ENCRYPTION_SET_KEY *) &wlpptr->pCmdBuf[0];
	//HostCmd_FW_UPDATE_ENCRYPTION *pCmd2 = (HostCmd_FW_UPDATE_ENCRYPTION *) &wlpptr->pCmdBuf[0];
	int retval = FAIL;
	unsigned long flags; 

	WLDBG_ENTER(DBG_LEVEL_0);
	//if (!mib->Privacy->RSNEnabled)
	//	return SUCCESS;
	MWL_SPIN_LOCK(&wlpptr->wlpd_p->locks.fwLock);
	memset(pCmd, 0x00, sizeof(HostCmd_FW_UPDATE_ENCRYPTION_SET_KEY));
	pCmd->CmdHdr.Cmd    = ENDIAN_SWAP16(HostCmd_CMD_UPDATE_ENCRYPTION);
	pCmd->CmdHdr.Length = ENDIAN_SWAP16(sizeof(HostCmd_FW_UPDATE_ENCRYPTION_SET_KEY));
	pCmd->CmdHdr.macid = wlpptr->vmacSta_p->VMacEntry.macId;
	pCmd->ActionType    = ENDIAN_SWAP32(EncrActionTypeSetKey);
	pCmd->KeyParam.Length      = ENDIAN_SWAP16(sizeof(pCmd->KeyParam));
	if ( (StaInfo_p->keyMgmtStateInfo.RsnIEBuf[0] == 221 && StaInfo_p->keyMgmtStateInfo.RsnIEBuf[17] == 2)
		|| (StaInfo_p->keyMgmtStateInfo.RsnIEBuf[0] == 48  && StaInfo_p->keyMgmtStateInfo.RsnIEBuf[13] == 2))
	{   
		// TKIP
		pCmd->KeyParam.KeyTypeId   = ENDIAN_SWAP16(KEY_TYPE_ID_TKIP);
		pCmd->KeyParam.KeyInfo     = ENDIAN_SWAP32(ENCR_KEY_FLAG_PAIRWISE  |
			ENCR_KEY_FLAG_TSC_VALID |
			ENCR_KEY_FLAG_MICKEY_VALID);
		pCmd->KeyParam.KeyIndex    = 0;
		pCmd->KeyParam.KeyLen  = ENDIAN_SWAP16(sizeof(TKIP_TYPE_KEY));
		memcpy(pCmd->KeyParam.Key.TkipKey.KeyMaterial,StaInfo_p->keyMgmtStateInfo.PairwiseTempKey1, MAX_ENCR_KEY_LENGTH);
		memcpy(pCmd->KeyParam.Key.TkipKey.TkipTxMicKey, StaInfo_p->keyMgmtStateInfo.RSNPwkTxMICKey, MIC_KEY_LENGTH);
		memcpy(pCmd->KeyParam.Key.TkipKey.TkipRxMicKey, StaInfo_p->keyMgmtStateInfo.RSNPwkRxMICKey, MIC_KEY_LENGTH);
		pCmd->KeyParam.Key.TkipKey.TkipRsc.low = 0;
		pCmd->KeyParam.Key.TkipKey.TkipRsc.high = 0;
		pCmd->KeyParam.Key.TkipKey.TkipTsc.low = ENDIAN_SWAP16(StaInfo_p->keyMgmtStateInfo.TxIV16); //= 0;
		pCmd->KeyParam.Key.TkipKey.TkipTsc.high = ENDIAN_SWAP32(StaInfo_p->keyMgmtStateInfo.TxIV32); //= = 0;
		memcpy(pCmd->KeyParam.Macaddr, StaInfo_p->Addr, 6);
		WLDBG_INFO(DBG_LEVEL_0, "HostCmd_FW_UPDATE_ENCRYPTION_KEY_SET WPA TKIP Len = %d pCmd->KeyParam = %d", 
			sizeof(HostCmd_FW_UPDATE_ENCRYPTION_SET_KEY), sizeof(pCmd->KeyParam));
	}
	else if ( (StaInfo_p->keyMgmtStateInfo.RsnIEBuf[0] == 221 && StaInfo_p->keyMgmtStateInfo.RsnIEBuf[17] == 4)
		|| (StaInfo_p->keyMgmtStateInfo.RsnIEBuf[0] == 48  && StaInfo_p->keyMgmtStateInfo.RsnIEBuf[13] == 4))
	{
		// AES
		pCmd->KeyParam.KeyTypeId   = ENDIAN_SWAP16(KEY_TYPE_ID_AES);
		pCmd->KeyParam.KeyInfo     = ENDIAN_SWAP32(ENCR_KEY_FLAG_PAIRWISE);
		pCmd->KeyParam.KeyIndex    = 0; // NA for wpa
		pCmd->KeyParam.KeyLen  = ENDIAN_SWAP16(sizeof(AES_TYPE_KEY));

		memcpy(pCmd->KeyParam.Key.AesKey.KeyMaterial,StaInfo_p->keyMgmtStateInfo.PairwiseTempKey1, TK_SIZE);
		memcpy(pCmd->KeyParam.Macaddr, StaInfo_p->Addr, 6);
		WLDBG_INFO(DBG_LEVEL_0, "HostCmd_FW_UPDATE_ENCRYPTION_KEY_SET WPA AES Len = %d pCmd->KeyParam = %d", 
			sizeof(HostCmd_FW_UPDATE_ENCRYPTION_SET_KEY), sizeof(pCmd->KeyParam));
	}
	else
		return retval;
	memcpy(pCmd->KeyParam.Macaddr, StaInfo_p->Addr, 6);
	WLDBG_DUMP_DATA(DBG_LEVEL_0, (void *) pCmd, sizeof(HostCmd_FW_UPDATE_ENCRYPTION_SET_KEY));
	retval = wlexecuteCommand(netdev, HostCmd_CMD_UPDATE_ENCRYPTION);

	MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
	return retval;
}

int wlFwSetWpaAesMode_STA(struct net_device *netdev,  u_int8_t *staaddr)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	HostCmd_FW_UPDATE_ENCRYPTION *pCmd = (HostCmd_FW_UPDATE_ENCRYPTION *) &wlpptr->pCmdBuf[0];
	int retval = FAIL;
	unsigned long flags; 

	WLDBG_ENTER(DBG_LEVEL_0);
	MWL_SPIN_LOCK(&wlpptr->wlpd_p->locks.fwLock);
	memset(pCmd, 0x00, sizeof(HostCmd_FW_UPDATE_ENCRYPTION));
	pCmd->CmdHdr.Cmd    = ENDIAN_SWAP16(HostCmd_CMD_UPDATE_ENCRYPTION);
	pCmd->CmdHdr.Length = ENDIAN_SWAP16(sizeof(HostCmd_FW_UPDATE_ENCRYPTION));
	pCmd->CmdHdr.macid = wlpptr->vmacSta_p->VMacEntry.macId;
	pCmd->ActionType    = ENDIAN_SWAP32(EncrActionEnableHWEncryption);
	pCmd->ActionData[0] = 6/*EncrTypeAes*/;
	memcpy(&pCmd->macaddr[0], staaddr, 6);
	retval = wlexecuteCommand(netdev, HostCmd_CMD_UPDATE_ENCRYPTION);

	MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
	return retval;
}

int wlFwSetWpaAesGroupK_STA(struct net_device *netdev, 
							UINT8 *macStaAddr_p, 
							UINT8 *key_p, 
							UINT16 keyLength)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	//vmacApInfo_t *vmacSta_p = wlpptr->vmacSta_p;
	//MIB_802DOT11 *mib = vmacSta_p->Mib802dot11;
	HostCmd_FW_UPDATE_ENCRYPTION_SET_KEY *pCmd = (HostCmd_FW_UPDATE_ENCRYPTION_SET_KEY *) &wlpptr->pCmdBuf[0];
	int retval = FAIL;
	unsigned long flags; 

	WLDBG_ENTER(DBG_LEVEL_0);
	MWL_SPIN_LOCK(&wlpptr->wlpd_p->locks.fwLock);
	memset(pCmd, 0x00, sizeof(HostCmd_FW_UPDATE_ENCRYPTION_SET_KEY));
	pCmd->CmdHdr.Cmd    = ENDIAN_SWAP16(HostCmd_CMD_UPDATE_ENCRYPTION);
	pCmd->CmdHdr.Length = ENDIAN_SWAP16(sizeof(HostCmd_FW_UPDATE_ENCRYPTION_SET_KEY));
	pCmd->CmdHdr.macid = wlpptr->vmacSta_p->VMacEntry.macId;
	pCmd->ActionType    = ENDIAN_SWAP32(EncrActionTypeSetGroupKey);
	pCmd->KeyParam.Length      = ENDIAN_SWAP16(sizeof(pCmd->KeyParam));
	pCmd->KeyParam.KeyTypeId   = ENDIAN_SWAP16(KEY_TYPE_ID_AES);
	pCmd->KeyParam.KeyInfo     = ENDIAN_SWAP32(ENCR_KEY_FLAG_TXGROUPKEY);
	pCmd->KeyParam.KeyIndex    = 0; // NA for wpa
	pCmd->KeyParam.KeyLen  = ENDIAN_SWAP16(keyLength);

	memcpy(pCmd->KeyParam.Key.AesKey.KeyMaterial,key_p, keyLength);
	memcpy(pCmd->KeyParam.Macaddr, macStaAddr_p, 6);

	WLDBG_INFO(DBG_LEVEL_0, "HostCmd_FW_UPDATE_ENCRYPTION_KEY_SET WPA AES Group Key Len = %d pCmd->KeyParam = %d", 
		sizeof(HostCmd_FW_UPDATE_ENCRYPTION_SET_KEY), sizeof(pCmd->KeyParam));
	WLDBG_DUMP_DATA(DBG_LEVEL_0, (void *) pCmd, sizeof(HostCmd_FW_UPDATE_ENCRYPTION_SET_KEY));
	retval = wlexecuteCommand(netdev, HostCmd_CMD_UPDATE_ENCRYPTION);

	MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
	return retval;
}
int wlFwSetWpaTkipGroupK_STA(struct net_device *netdev, 
							 UINT8 *macStaAddr_p, 
							 UINT8 *key_p, 
							 UINT16 keyLength,
							 UINT8 *rxMicKey_p,
							 UINT16 rxKeyLength,
							 UINT8 *txMicKey_p,
							 UINT16 txKeyLength,
							 ENCR_TKIPSEQCNT	TkipTsc,
							 UINT8 keyIndex)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	HostCmd_FW_UPDATE_ENCRYPTION_SET_KEY *pCmd = (HostCmd_FW_UPDATE_ENCRYPTION_SET_KEY *) &wlpptr->pCmdBuf[0];
	int retval = FAIL;

	unsigned long flags; 

	WLDBG_ENTER(DBG_LEVEL_0);
	MWL_SPIN_LOCK(&wlpptr->wlpd_p->locks.fwLock);
	memset(pCmd, 0x00, sizeof(HostCmd_FW_UPDATE_ENCRYPTION_SET_KEY));
	pCmd->CmdHdr.Cmd    = ENDIAN_SWAP16(HostCmd_CMD_UPDATE_ENCRYPTION);
	pCmd->CmdHdr.Length = ENDIAN_SWAP16(sizeof(HostCmd_FW_UPDATE_ENCRYPTION_SET_KEY));
	pCmd->CmdHdr.macid = wlpptr->vmacSta_p->VMacEntry.macId;
	pCmd->ActionType    = ENDIAN_SWAP32(EncrActionTypeSetGroupKey);
	pCmd->KeyParam.Length      = ENDIAN_SWAP16(sizeof(pCmd->KeyParam));
	pCmd->KeyParam.KeyTypeId   = ENDIAN_SWAP16(KEY_TYPE_ID_TKIP);
	pCmd->KeyParam.KeyInfo     = ENDIAN_SWAP32(ENCR_KEY_FLAG_TXGROUPKEY   |
		ENCR_KEY_FLAG_MICKEY_VALID |
		ENCR_KEY_FLAG_TSC_VALID);
	pCmd->KeyParam.KeyIndex    = ENDIAN_SWAP32(keyIndex);//index;
	pCmd->KeyParam.KeyLen  = ENDIAN_SWAP16(keyLength+txKeyLength+rxKeyLength+2*sizeof(ENCR_TKIPSEQCNT));

	memcpy(pCmd->KeyParam.Key.TkipKey.KeyMaterial, key_p, keyLength);
	memcpy(pCmd->KeyParam.Key.TkipKey.TkipTxMicKey, txMicKey_p, txKeyLength);
	memcpy(pCmd->KeyParam.Key.TkipKey.TkipRxMicKey, rxMicKey_p, rxKeyLength);

	pCmd->KeyParam.Key.TkipKey.TkipRsc.low = 0;
	pCmd->KeyParam.Key.TkipKey.TkipRsc.high = 0;
	pCmd->KeyParam.Key.TkipKey.TkipTsc.low = ENDIAN_SWAP16(TkipTsc.low); //= 0;
	pCmd->KeyParam.Key.TkipKey.TkipTsc.high = ENDIAN_SWAP32(TkipTsc.high);// = 0;
	memcpy(pCmd->KeyParam.Macaddr, macStaAddr_p, 6);

	WLDBG_INFO(DBG_LEVEL_0, "HostCmd_FW_UPDATE_ENCRYPTION_KEY_SET Len = %d pCmd->KeyParam = %d", 
		sizeof(HostCmd_FW_UPDATE_ENCRYPTION_SET_KEY), sizeof(pCmd->KeyParam));
	WLDBG_DUMP_DATA(DBG_LEVEL_0, (void *) pCmd, sizeof(HostCmd_FW_UPDATE_ENCRYPTION_SET_KEY));
	retval = wlexecuteCommand(netdev, HostCmd_CMD_UPDATE_ENCRYPTION);

	MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
	return retval;
}

int wlFwSetWpaTkipMode_STA(struct net_device *netdev,  u_int8_t *staaddr)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	HostCmd_FW_UPDATE_ENCRYPTION *pCmd = (HostCmd_FW_UPDATE_ENCRYPTION *) &wlpptr->pCmdBuf[0];
	int retval = FAIL;
	unsigned long flags; 

	WLDBG_ENTER(DBG_LEVEL_0);
	MWL_SPIN_LOCK(&wlpptr->wlpd_p->locks.fwLock);
	memset(pCmd, 0x00, sizeof(HostCmd_FW_UPDATE_ENCRYPTION));
	pCmd->CmdHdr.Cmd    = ENDIAN_SWAP16(HostCmd_CMD_UPDATE_ENCRYPTION);
	pCmd->CmdHdr.Length = ENDIAN_SWAP16(sizeof(HostCmd_FW_UPDATE_ENCRYPTION));
	pCmd->CmdHdr.macid = wlpptr->vmacSta_p->VMacEntry.macId;
	pCmd->ActionType    = ENDIAN_SWAP32(EncrActionEnableHWEncryption);
	pCmd->ActionData[0] = EncrTypeTkip;
	memcpy(&pCmd->macaddr[0], staaddr, 6);
	retval = wlexecuteCommand(netdev, HostCmd_CMD_UPDATE_ENCRYPTION);

	MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
	return retval;
}

int wlFwSetMixedWpaWpa2Mode_STA(struct net_device *netdev,  u_int8_t *staaddr)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	HostCmd_FW_UPDATE_ENCRYPTION *pCmd = (HostCmd_FW_UPDATE_ENCRYPTION *) &wlpptr->pCmdBuf[0];
	int retval = FAIL;
	unsigned long flags; 

	WLDBG_ENTER(DBG_LEVEL_0);
	MWL_SPIN_LOCK(&wlpptr->wlpd_p->locks.fwLock);

	memset(pCmd, 0x00, sizeof(HostCmd_FW_UPDATE_ENCRYPTION));
	pCmd->CmdHdr.Cmd    = ENDIAN_SWAP16(HostCmd_CMD_UPDATE_ENCRYPTION);
	pCmd->CmdHdr.Length = ENDIAN_SWAP16(sizeof(HostCmd_FW_UPDATE_ENCRYPTION));
	pCmd->CmdHdr.macid = wlpptr->vmacSta_p->VMacEntry.macId;
	pCmd->ActionType    = ENDIAN_SWAP32(EncrActionEnableHWEncryption);
	pCmd->ActionData[0] = EncrTypeMix;
	memcpy(&pCmd->macaddr[0], staaddr, 6);
	retval = wlexecuteCommand(netdev, HostCmd_CMD_UPDATE_ENCRYPTION);

	MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
	return retval;
}

#endif /* WPA_STA */

#ifdef SINGLE_DEV_INTERFACE
int wlFwSetMacAddr(struct net_device *netdev)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	HostCmd_DS_SET_MAC *pCmd = (HostCmd_DS_SET_MAC *) &wlpptr->pCmdBuf[0];
	int retval = FAIL;

	unsigned long flags; 

	WLDBG_ENTER(DBG_LEVEL_0);

	MWL_SPIN_LOCK(&wlpptr->wlpd_p->locks.fwLock);
	memset(pCmd, 0x00, sizeof(HostCmd_DS_SET_MAC));
	pCmd->CmdHdr.Cmd    = ENDIAN_SWAP16(HostCmd_CMD_SET_MAC_ADDR);
	pCmd->CmdHdr.Length = ENDIAN_SWAP16(sizeof(HostCmd_DS_SET_MAC));
	pCmd->CmdHdr.macid = wlpptr->vmacSta_p->VMacEntry.macId;
#ifdef CLIENT_SUPPORT
	pCmd->MacType = ENDIAN_SWAP16(WL_MAC_TYPE_PRIMARY_CLIENT);
#endif
	memcpy(&pCmd->MacAddr[0],netdev->dev_addr,ETH_ALEN);

	WLDBG_DUMP_DATA(DBG_LEVEL_0,(void *) pCmd,sizeof(HostCmd_DS_SET_MAC));
	retval = wlexecuteCommand(netdev, HostCmd_CMD_SET_MAC_ADDR);
	MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
	return retval;
}
#endif
int wlFwRemoveMacAddr(struct net_device *netdev, UINT8 *macAddr)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	HostCmd_DS_SET_MAC *pCmd = (HostCmd_DS_SET_MAC *) &wlpptr->pCmdBuf[0];
	int retval = FAIL;

	unsigned long flags; 

	WLDBG_ENTER(DBG_LEVEL_0);

	MWL_SPIN_LOCK(&wlpptr->wlpd_p->locks.fwLock);
	memset(pCmd, 0x00, sizeof(HostCmd_DS_SET_MAC));
	pCmd->CmdHdr.Cmd    = ENDIAN_SWAP16(HostCmd_CMD_DEL_MAC_ADDR);
	pCmd->CmdHdr.Length = ENDIAN_SWAP16(sizeof(HostCmd_DS_SET_MAC));
	pCmd->CmdHdr.macid = wlpptr->vmacSta_p->VMacEntry.macId;
	memcpy(&pCmd->MacAddr[0],macAddr,ETH_ALEN);
	WLDBG_DUMP_DATA(DBG_LEVEL_0,(void *) pCmd,sizeof(HostCmd_DS_SET_MAC));
	retval = wlexecuteCommand(netdev, HostCmd_CMD_DEL_MAC_ADDR);
	MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
	return retval;
}


#ifdef CLIENT_SUPPORT
int wlFwSetMacAddr_Client(struct net_device *netdev, UINT8 *macAddr)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	HostCmd_DS_SET_MAC *pCmd = (HostCmd_DS_SET_MAC *) &wlpptr->pCmdBuf[0];
	int retval = FAIL;

	unsigned long flags; 

	WLDBG_ENTER(DBG_LEVEL_0);

	MWL_SPIN_LOCK(&wlpptr->wlpd_p->locks.fwLock);
	memset(pCmd, 0x00, sizeof(HostCmd_DS_SET_MAC));
	pCmd->CmdHdr.Cmd    = ENDIAN_SWAP16(HostCmd_CMD_SET_MAC_ADDR);
	pCmd->CmdHdr.Length = ENDIAN_SWAP16(sizeof(HostCmd_DS_SET_MAC));
	pCmd->CmdHdr.macid = wlpptr->vmacSta_p->VMacEntry.macId;
#ifdef CLIENT_SUPPORT
#ifdef CLIENTONLY
	pCmd->MacType = ENDIAN_SWAP16(WL_MAC_TYPE_PRIMARY_CLIENT);//WL_MAC_TYPE_SECONDARY_CLIENT;
#else
	pCmd->MacType = ENDIAN_SWAP16(WL_MAC_TYPE_SECONDARY_CLIENT);
#endif
#if NUMOFAPS == 1
	pCmd->MacType = ENDIAN_SWAP16(WL_MAC_TYPE_PRIMARY_CLIENT);//WL_MAC_TYPE_SECONDARY_CLIENT;
#endif
#endif
	memcpy(&pCmd->MacAddr[0],macAddr,ETH_ALEN);

	WLDBG_DUMP_DATA(DBG_LEVEL_0,(void *) pCmd,sizeof(HostCmd_DS_SET_MAC));
	retval = wlexecuteCommand(netdev, HostCmd_CMD_SET_MAC_ADDR);
	MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
	return retval;
}
#endif /* CLIENT_SUPPORT */
enum
{
	IEEE80211_ELEMID_SSID		= 0,
	IEEE80211_ELEMID_RATES		= 1,
	IEEE80211_ELEMID_FHPARMS	= 2,
	IEEE80211_ELEMID_DSPARMS	= 3,
	IEEE80211_ELEMID_CFPARMS	= 4,
	IEEE80211_ELEMID_TIM		= 5,
	IEEE80211_ELEMID_IBSSPARMS	= 6,
	IEEE80211_ELEMID_COUNTRY	= 7,
	IEEE80211_ELEMID_CHALLENGE	= 16,
	IEEE80211_ELEMID_ERP		= 42,
	IEEE80211_ELEMID_RSN		= 48,
	IEEE80211_ELEMID_XRATES		= 50,
	IEEE80211_ELEMID_TPC		= 150,
	IEEE80211_ELEMID_CCKM		= 156,
	IEEE80211_ELEMID_VENDOR		= 221,	
};

int wlFwSetApBeacon(struct net_device *netdev)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	vmacApInfo_t *vmacSta_p = wlpptr->vmacSta_p;
	MIB_802DOT11 *mib = vmacSta_p->ShadowMib802dot11;
	HostCmd_DS_AP_BEACON *pCmd = (HostCmd_DS_AP_BEACON *) &wlpptr->pCmdBuf[0];
	u_int8_t *basicRates = &pCmd->StartCmd.BssBasicRateSet[0];
	u_int8_t *opRates = &pCmd->StartCmd.OpRateSet[0];
	u_int16_t capInfo = HostCmd_CAPINFO_DEFAULT;
	int retval = FAIL;
	IbssParams_t *ibssParamSet;
	CfParams_t *cfParamSet;
	DsParams_t *phyDsParamSet;
	int currRate = 0;
	int rateMask;
	MIB_PHY_DSSS_TABLE *PhyDSSSTable=mib->PhyDSSSTable;

	unsigned long flags; 

	WLDBG_ENTER(DBG_LEVEL_0);
	MWL_SPIN_LOCK(&wlpptr->wlpd_p->locks.fwLock);
	memset(pCmd, 0x00, sizeof(HostCmd_DS_AP_BEACON));
	pCmd->CmdHdr.Cmd    = ENDIAN_SWAP16(HostCmd_CMD_AP_BEACON);
	pCmd->CmdHdr.Length = ENDIAN_SWAP16(sizeof(HostCmd_DS_AP_BEACON));
	pCmd->CmdHdr.macid = vmacSta_p->VMacEntry.macId;

	memcpy(pCmd->StartCmd.StaMacAddr, netdev->dev_addr, sizeof(IEEEtypes_MacAddr_t));
	memcpy(&pCmd->StartCmd.SsId[0], 
		&(mib->StationConfig->DesiredSsId[0]), 
		strlen(&(mib->StationConfig->DesiredSsId[0])));
	pCmd->StartCmd.BssType    = 1; // 0xffee; /* INFRA: 8bit */
	pCmd->StartCmd.BcnPeriod  = ENDIAN_SWAP16(*(mib->mib_BcnPeriod));
	pCmd->StartCmd.DtimPeriod = mib->StationConfig->DtimPeriod; /* 8bit */

	ibssParamSet = &pCmd->StartCmd.SsParamSet.IbssParamSet;
	ibssParamSet->ElementId  = IEEE80211_ELEMID_IBSSPARMS;
	ibssParamSet->Len        = sizeof(ibssParamSet->AtimWindow);
	ibssParamSet->AtimWindow = ENDIAN_SWAP16(0);

	cfParamSet = &pCmd->StartCmd.SsParamSet.CfParamSet;
	cfParamSet->ElementId            = IEEE80211_ELEMID_CFPARMS;
	cfParamSet->Len                  = sizeof(cfParamSet->CfpCnt) +
		sizeof(cfParamSet->CfpPeriod) +
		sizeof(cfParamSet->CfpMaxDuration) +
		sizeof(cfParamSet->CfpDurationRemaining);
	cfParamSet->CfpCnt               = 0; /* 8bit */
	cfParamSet->CfpPeriod            = 2; /* 8bit */
	cfParamSet->CfpMaxDuration       = ENDIAN_SWAP16(0);
	cfParamSet->CfpDurationRemaining = ENDIAN_SWAP16(0);

	phyDsParamSet = &pCmd->StartCmd.PhyParamSet.DsParamSet;
	phyDsParamSet->ElementId   = IEEE80211_ELEMID_DSPARMS;
	phyDsParamSet->Len         = sizeof(phyDsParamSet->CurrentChan);
	phyDsParamSet->CurrentChan =PhyDSSSTable->CurrChan;

	pCmd->StartCmd.ProbeDelay = ENDIAN_SWAP16(10);

	capInfo |= HostCmd_CAPINFO_ESS;
	if (mib->StationConfig->mib_preAmble== PREAMBLE_SHORT|| mib->StationConfig->mib_preAmble == PREAMBLE_AUTO_SELECT )
	{
		capInfo |= HostCmd_CAPINFO_SHORT_PREAMBLE;
	}

#ifdef MRVL_WAPI 
	if (mib->Privacy->PrivInvoked || mib->Privacy->WAPIEnabled)
#else
	if (mib->Privacy->PrivInvoked)
#endif
	{
		capInfo |= HostCmd_CAPINFO_PRIVACY;
	} else
	{
		if (mib->Privacy->RSNEnabled)
		{
			InitThisStaRsnIE(vmacSta_p);

			if (!mib->RSNConfigWPA2->WPA2OnlyEnabled)
			{
				AddRSN_IE(vmacSta_p, (IEEEtypes_RSN_IE_t*)&pCmd->StartCmd.RsnIE);
			}
			capInfo |= HostCmd_CAPINFO_PRIVACY;
		}
	}
	if(*(mib->QoSOptImpl))
	{
		InitWMEParamElem(vmacSta_p);
		AddWMEParam_IE((WME_param_elem_t * )&pCmd->StartCmd.WMMParam);
	}

#ifdef AP_WPA2
	if (mib->RSNConfigWPA2->WPA2Enabled || mib->RSNConfigWPA2->WPA2OnlyEnabled)
	{
		if (mib->RSNConfigWPA2->WPA2Enabled && !mib->RSNConfigWPA2->WPA2OnlyEnabled) 
			AddRSN_IEWPA2MixedMode( vmacSta_p, (IEEEtypes_RSN_IE_WPA2MixedMode_t*)&pCmd->StartCmd.Rsn48IE );
		else
			AddRSN_IEWPA2( vmacSta_p, (IEEEtypes_RSN_IE_WPA2_t*)&pCmd->StartCmd.Rsn48IE );
	}
#endif


	if (*(mib->mib_shortSlotTime))
	{
		capInfo |= HostCmd_CAPINFO_SHORT_SLOT;
	}
	pCmd->StartCmd.CapInfo = ENDIAN_SWAP16(capInfo);

#ifdef BRS_SUPPORT

	rateMask = *(mib->BssBasicRateMask);
	for (currRate = 0; currRate < 14; currRate++)
	{
		if (rateMask & 0x01)
		{
			*basicRates++ = mib->StationConfig->OpRateSet[currRate] & 0x7F;
			//printk("ap8xLnxFwcmd: basic rate %d \n", (mib->StationConfig->OpRateSet[currRate] & 0x7F));
		}		
		rateMask >>= 1;
	}

	rateMask = *(mib->BssBasicRateMask) | *(mib->NotBssBasicRateMask);
	for (currRate = 0; currRate < 14; currRate++)
	{
		if (rateMask & 0x01)
		{
			if (mib->StationConfig->OpRateSet[currRate] != 0)
				*opRates++ = mib->StationConfig->OpRateSet[currRate] & 0x7F;
			//printk("ap8xLnxFwcmd: rate %d \n", (mib->StationConfig->OpRateSet[currRate] & 0x7F));
		}
		rateMask >>= 1;
	}

#else
	switch(*(mib->mib_ApMode) )
	{
	case AP_MODE_B_ONLY:
		for (currRate = 0; currRate < 4; currRate++)
		{
			*basicRates++ = mib->StationConfig->OpRateSet[currRate] & 0x7f;
		}
		for (currRate = 0; currRate < 4; currRate++)
		{
			if (mib->StationConfig->OpRateSet[currRate] != 0)
			{
				*opRates++ = mib->StationConfig->OpRateSet[currRate] & 0x7f;
			}
		}
		break;

	case AP_MODE_G_ONLY:
		for (currRate = 0; currRate < 14; currRate++)
		{
			if (mib->StationConfig->OpRateSet[currRate] & 0x80)
				*basicRates++ = mib->StationConfig->OpRateSet[currRate] & 0x7f;
		}
		for (currRate = 0; currRate < 14; currRate++)
		{
			if (mib->StationConfig->OpRateSet[currRate] != 0)
			{
				*opRates++ = mib->StationConfig->OpRateSet[currRate] & 0x7f;
			}
		}
		break;

	case AP_MODE_A_ONLY:
		for (currRate = 0; currRate < 14; currRate++)
		{
			if (mib->StationConfig->OpRateSet[currRate] & 0x80)
				*basicRates++ = mib->StationConfig->OpRateSet[currRate] & 0x7f;
		}

		for (currRate = 0; currRate < 8; currRate++)
		{
			if (mib->StationConfig->OpRateSet[currRate+4] != 0)
			{
				*opRates++ = mib->StationConfig->OpRateSet[currRate+4] & 0x7f;
			}
		}
		break;

	case AP_MODE_N_ONLY:
	case AP_MODE_5GHZ_N_ONLY:
		if (PhyDSSSTable->CurrChan <= 14)
		{
			/* For 2.4G */
			for (currRate = 0; currRate < 4; currRate++)
			{
				*basicRates++ = mib->StationConfig->OpRateSet[currRate] & 0x7f;
			}

			for (currRate = 0; currRate < 14; currRate++)
			{
				if (mib->StationConfig->OpRateSet[currRate] != 0)
				{
					*opRates++ = mib->StationConfig->OpRateSet[currRate] & 0x7f;
				}
			}
		}
		else
		{
			/* For 5G */
			for (currRate = 0; currRate < 14; currRate++)
			{
				if (mib->StationConfig->OpRateSet[currRate] & 0x80)
					*basicRates++ = mib->StationConfig->OpRateSet[currRate] & 0x7f;
			}

			for (currRate = 0; currRate < 8; currRate++)
			{
				if (mib->StationConfig->OpRateSet[currRate+4] != 0)
				{
					*opRates++ = mib->StationConfig->OpRateSet[currRate+4] & 0x7f;
				}
			}
		}
		break;
	case AP_MODE_MIXED:
	default:
#ifdef SOC_W8864	
		if (*(mib->mib_ApMode) != AP_MODE_AandN && *(mib->mib_ApMode) != AP_MODE_5GHZ_Nand11AC)
#else
		if (*(mib->mib_ApMode) != AP_MODE_AandN)
#endif
		{
			/* For 2.4G */
			for (currRate = 0; currRate < 4; currRate++)
			{
				*basicRates++ = mib->StationConfig->OpRateSet[currRate] & 0x7f;
			}

			for (currRate = 0; currRate < 14; currRate++)
			{
				if (mib->StationConfig->OpRateSet[currRate] != 0)
				{
					*opRates++ = mib->StationConfig->OpRateSet[currRate] & 0x7f;
				}
			}
		}
		else
		{
			/* For 5G */
			for (currRate = 0; currRate < 14; currRate++)
			{
				if (mib->StationConfig->OpRateSet[currRate] & 0x80)
					*basicRates++ = mib->StationConfig->OpRateSet[currRate] & 0x7f;
			}

			for (currRate = 0; currRate < 8; currRate++)
			{
				if (mib->StationConfig->OpRateSet[currRate+4] != 0)
				{
					*opRates++ = mib->StationConfig->OpRateSet[currRate+4] & 0x7f;
				}
			}
		}
		break;
	}
#endif

	WLDBG_DUMP_DATA(DBG_LEVEL_0,(void *) pCmd,
		sizeof(HostCmd_DS_AP_BEACON));
	retval = wlexecuteCommand(netdev, HostCmd_CMD_AP_BEACON);
	MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
	return retval;
}
int wlFwSetAid(struct net_device *netdev, u_int8_t *bssId, u_int16_t assocId)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	HostCmd_FW_SET_AID *pCmd = (HostCmd_FW_SET_AID *) &wlpptr->pCmdBuf[0];
	int retval = FAIL;
	unsigned long flags;

	WLDBG_ENTER_INFO(DBG_LEVEL_0,
		"bssid: %s, association ID: %i", mac_display(bssId), assocId);

	MWL_SPIN_LOCK(&wlpptr->wlpd_p->locks.fwLock);
	memset(pCmd, 0x00, sizeof(HostCmd_FW_SET_AID));
	pCmd->CmdHdr.Cmd    = ENDIAN_SWAP16(HostCmd_CMD_SET_AID);
	pCmd->CmdHdr.Length = ENDIAN_SWAP16(sizeof(HostCmd_FW_SET_AID));
	pCmd->CmdHdr.macid = wlpptr->vmacSta_p->VMacEntry.macId;
	pCmd->AssocID       = ENDIAN_SWAP16(assocId);
	memcpy(&pCmd->MacAddr[0], bssId, 6);

	WLDBG_DUMP_DATA(DBG_LEVEL_0,(void *) pCmd,sizeof(HostCmd_FW_SET_AID));
	retval = wlexecuteCommand(netdev, HostCmd_CMD_SET_AID);
	MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
	return retval;
}
int wlFwSetChannel(struct net_device *netdev, u_int8_t channel, CHNL_FLAGS Chanflag, u_int8_t initRateTable)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	HostCmd_FW_SET_RF_CHANNEL *pCmd =
		(HostCmd_FW_SET_RF_CHANNEL *) &wlpptr->pCmdBuf[0];
	int retval = FAIL;
	unsigned long flags;

	WLDBG_ENTER_INFO(DBG_LEVEL_0, "channel: %i", channel);

	MWL_SPIN_LOCK(&wlpptr->wlpd_p->locks.fwLock);
	memset(pCmd, 0x00, sizeof(HostCmd_FW_SET_RF_CHANNEL));
	pCmd->CmdHdr.Cmd     = ENDIAN_SWAP16(HostCmd_CMD_SET_RF_CHANNEL);
	pCmd->CmdHdr.Length  = ENDIAN_SWAP16(sizeof(HostCmd_FW_SET_RF_CHANNEL));
	pCmd->CmdHdr.macid = wlpptr->vmacSta_p->VMacEntry.macId;
	pCmd->CurrentChannel = channel;
	pCmd->Action        = ENDIAN_SWAP16(WL_SET);
#ifdef SOC_W8864
	pCmd->ChannelFlags.FreqBand  = Chanflag.FreqBand;
	pCmd->ChannelFlags.ChnlWidth = Chanflag.ChnlWidth;
	if(Chanflag.ChnlWidth == CH_AUTO_WIDTH)
	{
        if (Chanflag.FreqBand == FREQ_BAND_2DOT4GHZ)
        {
            pCmd->ChannelFlags.ChnlWidth = CH_40_MHz_WIDTH;
        }
        else
        {
            pCmd->ChannelFlags.ChnlWidth = CH_80_MHz_WIDTH;
        }
    }
    if (Chanflag.ExtChnlOffset == EXT_CH_ABOVE_CTRL_CH)
        pCmd->ChannelFlags.ActPrimary = ACT_PRIMARY_CHAN_0;
    else if (Chanflag.ExtChnlOffset == EXT_CH_BELOW_CTRL_CH)
        pCmd->ChannelFlags.ActPrimary = ACT_PRIMARY_CHAN_1;
    else
        pCmd->ChannelFlags.ActPrimary = ACT_PRIMARY_CHAN_0;

    if (pCmd->ChannelFlags.ChnlWidth == CH_80_MHz_WIDTH)
        pCmd->ChannelFlags.ActPrimary = macMgmtMlme_Get80MHzPrimaryChannelOffset(channel);
#else
	pCmd->ChannelFlags= Chanflag;
#endif



	pCmd->Action = 1;
	/*pCmd->initRateTable = initRateTable;*//*un-used field in current FW */

	WLDBG_DUMP_DATA(DBG_LEVEL_0, (void *) pCmd,
		sizeof(HostCmd_FW_SET_RF_CHANNEL));
	retval = wlexecuteCommand(netdev, HostCmd_CMD_SET_RF_CHANNEL);
	MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
	return retval;
}

#ifdef AMPDU_SUPPORT
int wlFwCreateBAStream(struct net_device *dev,	u_int32_t BarThrs, u_int32_t WindowSize , u_int8_t *Macaddr,
					   u_int8_t DialogToken, u_int8_t Tid, u_int32_t ba_type, u_int32_t direction , u_int8_t ParamInfo, 
					   u_int8_t *SrcMacaddr, UINT16 seqNo)
{

	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, dev);
	HostCmd_FW_BASTREAM *pCmd = (HostCmd_FW_BASTREAM *) &wlpptr->pCmdBuf[0];
	int retval = FAIL;
	unsigned long flags;


	WLDBG_ENTER_INFO(DBG_LEVEL_0, "Create BA Stream: %i");
	MWL_SPIN_LOCK(&wlpptr->wlpd_p->locks.fwLock);
	memset(pCmd, 0x00, sizeof(HostCmd_FW_BASTREAM ));
	pCmd->CmdHdr.Cmd    = ENDIAN_SWAP16(HostCmd_CMD_BASTREAM );
	pCmd->CmdHdr.Length = ENDIAN_SWAP16(sizeof(HostCmd_FW_BASTREAM ));
	pCmd->CmdHdr.macid = wlpptr->vmacSta_p->VMacEntry.macId;
	pCmd->CmdHdr.Result = ENDIAN_SWAP16(0xffff);
	pCmd->ActionType = ENDIAN_SWAP32(BaCreateStream);
	pCmd->BaInfo.CreateParams.BarThrs = ENDIAN_SWAP32(BarThrs);
	pCmd->BaInfo.CreateParams.WindowSize = ENDIAN_SWAP32(WindowSize);
	pCmd->BaInfo.CreateParams.IdleThrs = ENDIAN_SWAP32(0x22000);
	memcpy(&pCmd->BaInfo.CreateParams.PeerMacAddr[0], Macaddr, 6);
	pCmd->BaInfo.CreateParams.DialogToken = DialogToken;
	pCmd->BaInfo.CreateParams.Tid = Tid;
	pCmd->BaInfo.CreateParams.Flags.BaType = ba_type;
	pCmd->BaInfo.CreateParams.Flags.BaDirection = 0;
	pCmd->BaInfo.CreateParams.QueueId = direction;
	pCmd->BaInfo.CreateParams.ParamInfo = ParamInfo;
	pCmd->BaInfo.CreateParams.ResetSeqNo = 0;
	pCmd->BaInfo.CreateParams.CurrentSeq = ENDIAN_SWAP16(seqNo);
#ifdef V6FW
	/* SrcMacaddr is used for client mode, not required for AP mode. */
	if(SrcMacaddr)
		memcpy(&pCmd->BaInfo.CreateParams.StaSrcMacAddr[0], SrcMacaddr, 6);
	else
		memset(&pCmd->BaInfo.CreateParams.StaSrcMacAddr[0], 0x00, 6);
#endif

	WLDBG_DUMP_DATA(DBG_LEVEL_0,(void *) pCmd,sizeof(HostCmd_FW_BASTREAM));
	retval = wlexecuteCommand(dev, HostCmd_CMD_BASTREAM);
	//	printk("Value of result = %x\n",pCmd->CmdHdr.Result);
	if(pCmd->CmdHdr.Result!=0)
	{
		//		printk("FW not ready to do addba!!!!!!!!! \n");
		retval = FAIL;
	}
	MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
	return retval;
}

int wlFwCheckBAStream(struct net_device *dev,	u_int32_t BarThrs, u_int32_t WindowSize , u_int8_t *Macaddr,
					  u_int8_t DialogToken, u_int8_t Tid, u_int32_t ba_type, int32_t qid , u_int8_t ParamInfo 
					  )
{

	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, dev);
	HostCmd_FW_BASTREAM *pCmd = (HostCmd_FW_BASTREAM *) &wlpptr->pCmdBuf[0];
	int retval = FAIL;
	unsigned long flags;


	WLDBG_ENTER_INFO(DBG_LEVEL_0, "Create BA Stream: %i");
	MWL_SPIN_LOCK(&wlpptr->wlpd_p->locks.fwLock);
	memset(pCmd, 0x00, sizeof(HostCmd_FW_BASTREAM ));
	pCmd->CmdHdr.Cmd    = ENDIAN_SWAP16(HostCmd_CMD_BASTREAM );
	pCmd->CmdHdr.Length = ENDIAN_SWAP16(sizeof(HostCmd_FW_BASTREAM ));
	pCmd->CmdHdr.macid = wlpptr->vmacSta_p->VMacEntry.macId;
	pCmd->CmdHdr.Result = ENDIAN_SWAP16(0xffff);
	pCmd->ActionType = ENDIAN_SWAP32(BaCheckCreateStream);
	pCmd->BaInfo.CreateParams.BarThrs = ENDIAN_SWAP32(63);//BarThrs;
	pCmd->BaInfo.CreateParams.WindowSize = ENDIAN_SWAP32(64); /*WindowSize;*/
	pCmd->BaInfo.CreateParams.IdleThrs = ENDIAN_SWAP32(0x22000);
	memcpy(&pCmd->BaInfo.CreateParams.PeerMacAddr[0], Macaddr, 6);
	pCmd->BaInfo.CreateParams.DialogToken = DialogToken;
	pCmd->BaInfo.CreateParams.Tid = Tid;
	pCmd->BaInfo.CreateParams.Flags.BaType = ba_type;
	pCmd->BaInfo.CreateParams.Flags.BaDirection = 0;
	pCmd->BaInfo.CreateParams.QueueId = qid;
	pCmd->BaInfo.CreateParams.ParamInfo = ParamInfo;
	pCmd->BaInfo.CreateParams.ResetSeqNo = 1;
	pCmd->BaInfo.CreateParams.CurrentSeq = ENDIAN_SWAP16(0);

	WLDBG_DUMP_DATA(DBG_LEVEL_0,(void *) pCmd,sizeof(HostCmd_FW_BASTREAM));
	retval = wlexecuteCommand(dev, HostCmd_CMD_BASTREAM);
	//	printk("Value of result = %x\n",pCmd->CmdHdr.Result);
	if(pCmd->CmdHdr.Result!=0)
	{
		//printk("FW not ready to do addba!!!!!!!!! \n");
		retval = FAIL;
	}
	MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
	return retval;


}

int
wlFwGetSeqNoBAStream(struct net_device *dev, u_int8_t *Macaddr, 
					 uint8_t Tid, uint16_t *pbaseq_no)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, dev);
	HostCmd_GET_SEQNO *pCmd = (HostCmd_GET_SEQNO *) &wlpptr->pCmdBuf[0];
	int retval;
	unsigned long flags;

	WLDBG_ENTER_INFO(DBG_LEVEL_0, "Get BA Stream Seqno %i");
	MWL_SPIN_LOCK(&wlpptr->wlpd_p->locks.fwLock);

	memset(pCmd, 0x00, sizeof(HostCmd_GET_SEQNO ));
	pCmd->CmdHdr.Cmd    = ENDIAN_SWAP16(HostCmd_CMD_GET_SEQNO );
	pCmd->CmdHdr.Length = ENDIAN_SWAP16(sizeof(HostCmd_GET_SEQNO ));

	memcpy(pCmd->MacAddr, Macaddr, 6);
	pCmd->TID = Tid;

	WLDBG_DUMP_DATA(DBG_LEVEL_0,(void *) pCmd,sizeof(HostCmd_GET_SEQNO));

	retval = wlexecuteCommand(dev, HostCmd_CMD_GET_SEQNO);
	if (retval == 0) {
		*pbaseq_no = ENDIAN_SWAP16(pCmd->SeqNo);
	}
	MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
	return retval;
}	

int wlFwUpdateDestroyBAStream(struct net_device *dev, u_int32_t ba_type, u_int32_t direction, u_int8_t stream)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, dev);
	HostCmd_FW_BASTREAM *pCmd = (HostCmd_FW_BASTREAM *) &wlpptr->pCmdBuf[0];
	int retval = FAIL;
	unsigned long flags;

	WLDBG_ENTER_INFO(DBG_LEVEL_0, "Destroy BA Stream: %i");
	MWL_SPIN_LOCK(&wlpptr->wlpd_p->locks.fwLock);
	memset(pCmd, 0x00, sizeof(HostCmd_FW_BASTREAM ));
	pCmd->CmdHdr.Cmd    = ENDIAN_SWAP16(HostCmd_CMD_BASTREAM );
	pCmd->CmdHdr.Length = ENDIAN_SWAP16(sizeof(HostCmd_FW_BASTREAM ));
	pCmd->CmdHdr.macid = wlpptr->vmacSta_p->VMacEntry.macId;
	pCmd->ActionType = ENDIAN_SWAP32(BaDestroyStream);
	pCmd->BaInfo.DestroyParams.Flags.BaType = ba_type;
	pCmd->BaInfo.DestroyParams.Flags.BaDirection = direction;
	pCmd->BaInfo.DestroyParams.FwBaContext.Context = ENDIAN_SWAP32(stream);

	WLDBG_DUMP_DATA(DBG_LEVEL_0,(void *) pCmd,sizeof(HostCmd_FW_BASTREAM));
	retval = wlexecuteCommand(dev, HostCmd_CMD_BASTREAM);
	MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
	return retval;

}



int wlFwUpdateUpdateBAStream(struct net_device *dev, u_int32_t ba_type, u_int32_t direction, u_int16_t seqNum)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, dev);
	HostCmd_FW_BASTREAM *pCmd = (HostCmd_FW_BASTREAM *) &wlpptr->pCmdBuf[0];
	int retval = FAIL;
	unsigned long flags;

	WLDBG_ENTER_INFO(DBG_LEVEL_0, "Update BA Stream: %i");
	MWL_SPIN_LOCK(&wlpptr->wlpd_p->locks.fwLock);
	memset(pCmd, 0x00, sizeof(HostCmd_FW_BASTREAM ));
	pCmd->CmdHdr.Cmd    = ENDIAN_SWAP16(HostCmd_CMD_BASTREAM );
	pCmd->CmdHdr.Length = ENDIAN_SWAP16(sizeof(HostCmd_FW_BASTREAM ));
	pCmd->CmdHdr.macid = wlpptr->vmacSta_p->VMacEntry.macId;
	pCmd->ActionType = ENDIAN_SWAP32(BaUpdateStream);
	pCmd->BaInfo.UpdtSeqNum.Flags.BaType = ba_type;
	pCmd->BaInfo.UpdtSeqNum.Flags.BaDirection = direction;
	pCmd->BaInfo.UpdtSeqNum.BaSeqNum = ENDIAN_SWAP16(seqNum);

	WLDBG_DUMP_DATA(DBG_LEVEL_0,(void *) pCmd,sizeof(HostCmd_FW_BASTREAM));
	retval = wlexecuteCommand(dev, HostCmd_CMD_BASTREAM);
	MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
	return retval;

}

int wlFwFlushBAStream(struct net_device *dev, u_int32_t ba_type, u_int32_t direction)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, dev);
	HostCmd_FW_BASTREAM *pCmd = (HostCmd_FW_BASTREAM *) &wlpptr->pCmdBuf[0];
	int retval = FAIL;
	unsigned long flags;

	WLDBG_ENTER_INFO(DBG_LEVEL_0, "Flush BA Stream: %i");
	MWL_SPIN_LOCK(&wlpptr->wlpd_p->locks.fwLock);
	memset(pCmd, 0x00, sizeof(HostCmd_FW_BASTREAM ));
	pCmd->CmdHdr.Cmd    = ENDIAN_SWAP16(HostCmd_CMD_BASTREAM );
	pCmd->CmdHdr.Length = ENDIAN_SWAP16(sizeof(HostCmd_FW_BASTREAM ));
	pCmd->CmdHdr.macid = wlpptr->vmacSta_p->VMacEntry.macId;
	pCmd->ActionType = ENDIAN_SWAP32(BaFlushStream);
	pCmd->BaInfo.FlushParams.Flags.BaType = ba_type;
	pCmd->BaInfo.FlushParams.Flags.BaDirection = direction;

	WLDBG_DUMP_DATA(DBG_LEVEL_0,(void *) pCmd,sizeof(HostCmd_FW_BASTREAM));
	retval = wlexecuteCommand(dev, HostCmd_CMD_BASTREAM);
	MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
	return retval;

}

#endif
int wlFwSetNewStn(struct net_device *dev,  u_int8_t *staaddr,u_int16_t assocId, u_int16_t stnId, u_int16_t action, 
				  PeerInfo_t *pPeerInfo,	UINT8 Qosinfo , UINT8 isQosSta)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, dev);
	HostCmd_FW_SET_NEW_STN *pCmd = (HostCmd_FW_SET_NEW_STN *) &wlpptr->pCmdBuf[0];
	int retval = FAIL;
	unsigned long flags;		

#ifdef UAPSD_SUPPORT
	WLDBG_ENTER_INFO(DBG_LEVEL_0,
		"staid: %s, association ID: %i stnId: %i action %i qosinfo %i qosSta %i\n", mac_display(staaddr), assocId,stnId,action,Qosinfo,isQosSta);
#else
	WLDBG_ENTER_INFO(DBG_LEVEL_0,
		"bssid: %s, association ID: %i stnId: %i action %i", mac_display(bssId), assocId,stnId,action);
#endif



	MWL_SPIN_LOCK(&wlpptr->wlpd_p->locks.fwLock);
	memset(pCmd, 0x00, sizeof(HostCmd_FW_SET_NEW_STN));
	pCmd->CmdHdr.Cmd    = ENDIAN_SWAP16(HostCmd_CMD_SET_NEW_STN);
	pCmd->CmdHdr.Length = ENDIAN_SWAP16(sizeof(HostCmd_FW_SET_NEW_STN));
	pCmd->CmdHdr.macid = wlpptr->vmacSta_p->VMacEntry.macId;
	pCmd->AID       = ENDIAN_SWAP16(assocId);
	pCmd->StnId  = ENDIAN_SWAP16(stnId);
	pCmd->Action  = ENDIAN_SWAP16(action);
	if (pPeerInfo)
	{
		pPeerInfo->TxBFCapabilities=WORD_SWAP(pPeerInfo->TxBFCapabilities);
		pPeerInfo->vht_cap = pPeerInfo->vht_cap;
		memcpy((void *)&(pCmd->PeerInfo), (void *)pPeerInfo, sizeof(PeerInfo_t));
	}
	memcpy(&pCmd->MacAddr[0], staaddr, 6);
#ifdef UAPSD_SUPPORT
	pCmd->Qosinfo = Qosinfo;
	pCmd->isQosSta = isQosSta;
#endif


	WLDBG_DUMP_DATA(DBG_LEVEL_0,(void *) pCmd,sizeof(HostCmd_FW_SET_NEW_STN));
	retval = wlexecuteCommand(dev, HostCmd_CMD_SET_NEW_STN);
	retval  = ENDIAN_SWAP32((int) (pCmd->FwStaPtr));
	MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
	return retval;
}

int wlFwSetKeepAliveTick(struct net_device *dev,  u_int8_t tick)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, dev);
	//	static int i;
	HostCmd_FW_SET_KEEP_ALIVE_TICK *pCmd = (HostCmd_FW_SET_KEEP_ALIVE_TICK *) &wlpptr->pCmdBuf[0];
	int retval = FAIL;
	unsigned long flags;
	//	WLDBG_ENTER_INFO(DBG_LEVEL_0,"FW keepalive %i",i );
#ifdef MFG_SUPPORT
	if (wlpptr->mfgEnable)
	{		
		return SUCCESS;
	}
#endif

	MWL_SPIN_LOCK(&wlpptr->wlpd_p->locks.fwLock);
	memset(pCmd, 0x00, sizeof(HostCmd_CMD_SET_KEEP_ALIVE));
	pCmd->CmdHdr.Cmd    = ENDIAN_SWAP16(HostCmd_CMD_SET_KEEP_ALIVE);
	pCmd->CmdHdr.Length = ENDIAN_SWAP16(sizeof(HostCmd_FW_SET_KEEP_ALIVE_TICK));
	pCmd->CmdHdr.macid = wlpptr->vmacSta_p->VMacEntry.macId;
	pCmd->tick = tick;

	//	WLDBG_DUMP_DATA(DBG_LEVEL_0,(void *) pCmd,sizeof(HostCmd_FW_SET_KEEP_ALIVE_TICK));
	retval = wlexecuteCommand(dev, HostCmd_CMD_SET_KEEP_ALIVE);
	if(retval == TIMEOUT)
	{
		if(wlpptr->netDevStats.tx_heartbeat_errors++ %2)
			wlResetTask(dev);
	}
	MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
	return retval;
}


int wlFwGetWatchdogbitmap( struct net_device *dev, u_int8_t *bitmap)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, dev);

	HostCmd_FW_GET_WATCHDOG_BITMAP *pCmd = (HostCmd_FW_GET_WATCHDOG_BITMAP *) &wlpptr->pCmdBuf[0];
	int retval = FAIL;
	unsigned long flags;
	//	WLDBG_ENTER_INFO(DBG_LEVEL_1,"FW keepalive %i",i );
#ifdef MFG_SUPPORT
	if (wlpptr->mfgEnable)
	{		
		return SUCCESS;
	}
#endif
	MWL_SPIN_LOCK(&wlpptr->wlpd_p->locks.fwLock);
	memset(pCmd, 0x00, sizeof(HostCmd_CMD_GET_WATCHDOG_BITMAP));
	pCmd->CmdHdr.Cmd    = ENDIAN_SWAP16(HostCmd_CMD_GET_WATCHDOG_BITMAP);
	pCmd->CmdHdr.Length = ENDIAN_SWAP16(sizeof(HostCmd_FW_GET_WATCHDOG_BITMAP));
	//	WLDBG_DUMP_DATA(DBG_LEVEL_1,(void *) pCmd,sizeof(HostCmd_FW_SET_KEEP_ALIVE_TICK));
	retval = wlexecuteCommand(dev, HostCmd_CMD_GET_WATCHDOG_BITMAP);
	if(retval == SUCCESS)
	{
		*bitmap=pCmd->Watchdogbitmap;
	}

	MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
	return retval;
}


int wlFwSetApMode( struct net_device *netdev,u_int8_t ApMode)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	HostCmd_FW_SET_APMODE *pCmd = (HostCmd_FW_SET_APMODE *) &wlpptr->pCmdBuf[0];
	int retval = FAIL;
	unsigned long flags;

	WLDBG_ENTER_INFO(DBG_LEVEL_0,
		"AP Mode = %d", ApMode);

	MWL_SPIN_LOCK(&wlpptr->wlpd_p->locks.fwLock);
	memset(pCmd, 0x00, sizeof(HostCmd_CMD_SET_APMODE));
	pCmd->CmdHdr.Cmd    = ENDIAN_SWAP16(HostCmd_CMD_SET_APMODE);
	pCmd->CmdHdr.Length = ENDIAN_SWAP16(sizeof(HostCmd_FW_SET_APMODE));
	pCmd->CmdHdr.macid = wlpptr->vmacSta_p->VMacEntry.macId;
	pCmd->ApMode = ApMode;

	WLDBG_DUMP_DATA(DBG_LEVEL_0, (void *) pCmd,
		sizeof(HostCmd_FW_SET_APMODE));

	retval = wlexecuteCommand(netdev, HostCmd_CMD_SET_APMODE);
	MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
	return retval;
}

int wlFwSetAPBss(struct net_device *netdev, wlfacilitate_e facility)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	vmacApInfo_t *vmacSta_p = wlpptr->vmacSta_p;
	HostCmd_DS_BSS_START *pCmd = (HostCmd_DS_BSS_START *) &wlpptr->pCmdBuf[0];
	int retval = FAIL;
	unsigned long flags;

	WLDBG_ENTER_INFO(DBG_LEVEL_0,
		"AP bss %s", (facility == WL_ENABLE) ? "enable": "disable");

	MWL_SPIN_LOCK(&wlpptr->wlpd_p->locks.fwLock);
	memset(pCmd, 0x00, sizeof(HostCmd_DS_BSS_START));
	pCmd->CmdHdr.Cmd    = ENDIAN_SWAP16(HostCmd_CMD_BSS_START);
	pCmd->CmdHdr.Length = ENDIAN_SWAP16(sizeof(HostCmd_DS_BSS_START));
	pCmd->Enable        = ENDIAN_SWAP32(facility);
	pCmd->CmdHdr.macid = vmacSta_p->VMacEntry.macId;

	WLDBG_DUMP_DATA(DBG_LEVEL_0, (void *) pCmd,
		sizeof(HostCmd_DS_BSS_START));
	retval = wlexecuteCommand(netdev, HostCmd_CMD_BSS_START);
	MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
	return retval;
}
int wlFwSetAPUpdateTim(struct net_device *netdev, u_int16_t assocId, Bool_e set)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	HostCmd_UpdateTIM *pCmd = (HostCmd_UpdateTIM *) &wlpptr->pCmdBuf[0];
	int retval = FAIL;
	unsigned long flags;

	WLDBG_ENTER_INFO(DBG_LEVEL_0,
		"association ID: %i %s", assocId,(set == WL_TRUE)? "update":"noact");

	MWL_SPIN_LOCK(&wlpptr->wlpd_p->locks.fwLock);
	memset(pCmd, 0x00, sizeof(HostCmd_UpdateTIM));
	pCmd->CmdHdr.Cmd    = ENDIAN_SWAP16(HostCmd_CMD_UPDATE_TIM);
	pCmd->CmdHdr.Length = ENDIAN_SWAP16(sizeof(HostCmd_UpdateTIM));
	pCmd->UpdateTIM.Aid           = ENDIAN_SWAP16(assocId);
	pCmd->UpdateTIM.Set           = ENDIAN_SWAP32(set);
	pCmd->CmdHdr.macid = wlpptr->vmacSta_p->VMacEntry.macId;

	WLDBG_DUMP_DATA(DBG_LEVEL_0, (void *) pCmd,
		sizeof(HostCmd_UpdateTIM));
	retval = wlexecuteCommand(netdev, HostCmd_CMD_UPDATE_TIM);
	MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
	return retval;
}

int wlFwSetAPBcastSSID(struct net_device *netdev, wlfacilitate_e facility)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	HostCmd_SSID_BROADCAST *pCmd =
		(HostCmd_SSID_BROADCAST *) &wlpptr->pCmdBuf[0];
	int retval = FAIL;
	unsigned long flags;

	WLDBG_ENTER_INFO(DBG_LEVEL_0,
		"AP SSID broadcast %s", (facility == WL_ENABLE) ? "enable":"disable");

	MWL_SPIN_LOCK(&wlpptr->wlpd_p->locks.fwLock);
	memset(pCmd, 0x00, sizeof(HostCmd_SSID_BROADCAST));
	pCmd->CmdHdr.Cmd = ENDIAN_SWAP16(HostCmd_CMD_BROADCAST_SSID_ENABLE);
	pCmd->CmdHdr.Length = ENDIAN_SWAP16(sizeof(HostCmd_SSID_BROADCAST));
	pCmd->SsidBroadcastEnable = ENDIAN_SWAP32(facility);
	pCmd->CmdHdr.macid = wlpptr->vmacSta_p->VMacEntry.macId;

	WLDBG_DUMP_DATA(DBG_LEVEL_0, (void *) pCmd,
		sizeof(HostCmd_SSID_BROADCAST));
	retval = wlexecuteCommand(netdev, HostCmd_CMD_BROADCAST_SSID_ENABLE);
	MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
	return retval;
}
int wlFwSetGProt(struct net_device *netdev, wlfacilitate_e facility)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	HostCmd_FW_SET_G_PROTECT_FLAG *pCmd =
		(HostCmd_FW_SET_G_PROTECT_FLAG *) &wlpptr->pCmdBuf[0];
	int retval = FAIL;
	unsigned long flags;

	WLDBG_ENTER_INFO(DBG_LEVEL_0, "G prot mode %d", facility);

	MWL_SPIN_LOCK(&wlpptr->wlpd_p->locks.fwLock);
	memset(pCmd, 0x00, sizeof(HostCmd_FW_SET_G_PROTECT_FLAG));
	pCmd->CmdHdr.Cmd    = ENDIAN_SWAP16(HostCmd_CMD_SET_G_PROTECT_FLAG);
	pCmd->CmdHdr.Length = ENDIAN_SWAP16(sizeof(HostCmd_FW_SET_G_PROTECT_FLAG));
	pCmd->CmdHdr.macid = wlpptr->vmacSta_p->VMacEntry.macId;
	pCmd->GProtectFlag  = ENDIAN_SWAP32(facility);

	WLDBG_DUMP_DATA(DBG_LEVEL_0, (void *) pCmd,
		sizeof(HostCmd_FW_SET_G_PROTECT_FLAG));
	retval = wlexecuteCommand(netdev, HostCmd_CMD_SET_G_PROTECT_FLAG);
	MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
	return retval;
}
int wlFwSetWmm(struct net_device *netdev,wlfacilitate_e facility)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	HostCmd_FW_SetWMMMode *pCmd =
		(HostCmd_FW_SetWMMMode *) &wlpptr->pCmdBuf[0];
	int retval = FAIL;

	unsigned long flags; 

	WLDBG_ENTER(DBG_LEVEL_0);

	MWL_SPIN_LOCK(&wlpptr->wlpd_p->locks.fwLock);
	memset(pCmd, 0x00, sizeof(HostCmd_FW_SetWMMMode));
	pCmd->CmdHdr.Cmd     = ENDIAN_SWAP16(HostCmd_CMD_SET_WMM_MODE);
	pCmd->CmdHdr.Length  = ENDIAN_SWAP16(sizeof(HostCmd_FW_SetWMMMode));
	pCmd->CmdHdr.macid = wlpptr->vmacSta_p->VMacEntry.macId;
	pCmd->Action = ENDIAN_SWAP16(facility);

	retval = wlexecuteCommand(netdev, HostCmd_CMD_SET_WMM_MODE);
	MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
	return retval;
}
int wlFwSetEdcaParam(struct net_device *netdev, u_int8_t Indx, u_int32_t CWmin, u_int32_t CWmax, u_int8_t AIFSN,  u_int16_t TXOPLimit)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	HostCmd_FW_SET_EDCA_PARAMS *pCmd =
		(HostCmd_FW_SET_EDCA_PARAMS *) &wlpptr->pCmdBuf[0];
	int retval = FAIL;
	unsigned long flags;

	WLDBG_ENTER_INFO(DBG_LEVEL_0," wlFwSetEdcaParam ");

	MWL_SPIN_LOCK(&wlpptr->wlpd_p->locks.fwLock);
	memset(pCmd, 0x00, sizeof(HostCmd_FW_SET_EDCA_PARAMS));
	pCmd->CmdHdr.Cmd     = ENDIAN_SWAP16(HostCmd_CMD_SET_EDCA_PARAMS);
	pCmd->CmdHdr.Length  = ENDIAN_SWAP16(sizeof(HostCmd_FW_SET_EDCA_PARAMS));
	pCmd->CmdHdr.macid = wlpptr->vmacSta_p->VMacEntry.macId;
	pCmd->Action = ENDIAN_SWAP16(0xffff);  //set everything
	pCmd->TxOP = ENDIAN_SWAP16(TXOPLimit);
	pCmd->CWMax = ENDIAN_SWAP32(CWmax);
	pCmd->CWMin = ENDIAN_SWAP32(CWmin);
	pCmd->AIFSN = AIFSN;
	pCmd->TxQNum = Indx;

#if 1 /* The array index defined in qos.h has a reversed bk and be. 
	The HW queue was not used this way; the qos code needs to be changed or 
	checked */
	if( Indx == 0 )
		pCmd->TxQNum = 1;
	else if( Indx == 1 )
		pCmd->TxQNum = 0;
#endif

	WLDBG_DUMP_DATA(DBG_LEVEL_0,(void *) pCmd,sizeof(HostCmd_FW_SET_EDCA_PARAMS));
	retval = wlexecuteCommand(netdev, HostCmd_CMD_SET_EDCA_PARAMS);
	MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
	return retval;
}

int wlFwSetIEs(struct net_device *netdev)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	vmacApInfo_t *vmacSta_p = wlpptr->vmacSta_p;
	MIB_802DOT11 *mib = vmacSta_p->ShadowMib802dot11;
	HostCmd_FW_SetIEs *pCmd =
		(HostCmd_FW_SetIEs *) &wlpptr->pCmdBuf[0];
	int retval = FAIL;
	int retvalVHT = 0, retvalProprietary=0;

	unsigned long flags; 

	WLDBG_ENTER(DBG_LEVEL_0);

	MWL_SPIN_LOCK(&wlpptr->wlpd_p->locks.fwLock);
	memset(pCmd, 0x00, sizeof(HostCmd_FW_SetIEs));
	pCmd->CmdHdr.Cmd     = ENDIAN_SWAP16(HostCmd_CMD_SET_IES);
	pCmd->CmdHdr.Length  = ENDIAN_SWAP16(sizeof(HostCmd_FW_SetIEs));
	pCmd->Action = ENDIAN_SWAP16(HostCmd_ACT_GEN_SET);
	pCmd->CmdHdr.macid = vmacSta_p->VMacEntry.macId;
	retval = 0;
	pCmd->IeListLenHT = 0;
	pCmd->IeListLenVHT = 0;				
	pCmd->IeListLenProprietary = 0;		

	/***************************************************************************************************/
	/*IMPORTANT: When adding new IE or IE's fields, make sure Hostcmd and Generic_Beacon buffer in fw has enough size*/
	/***************************************************************************************************/
	
	if (*(mib->QoSOptImpl))
	{
		//why do we need info element in beacon, take out?????
		//	pCmd->IeListLen  += QoS_AppendWMEInfoElem/*QoS_AppendWMEParamElem*/(&(pCmd->IeList[retval]));
		//	retval = pCmd->IeListLen;	

		pCmd->IeListLenHT  += AddHT_IE(wlpptr->vmacSta_p,(IEEEtypes_HT_Element_t*)&(pCmd->IeListHT[retval]));
		retval = pCmd->IeListLenHT;	

#ifdef INTOLERANT40
		{
			extern UINT16 AddChanReport_IE(vmacApInfo_t *vmacSta_p,IEEEtypes_ChannelReportEL_t* pNextElement);

			pCmd->IeListLenHT  += AddChanReport_IE(wlpptr->vmacSta_p,(IEEEtypes_ChannelReportEL_t*)&(pCmd->IeListHT[retval]));
			retval = pCmd->IeListLenHT;	
		}
#endif
		pCmd->IeListLenHT += AddAddHT_IE(wlpptr->vmacSta_p,(IEEEtypes_Add_HT_Element_t*)&(pCmd->IeListHT[retval]));
		retval = pCmd->IeListLenHT;	


#ifdef COEXIST_20_40_SUPPORT
		/** We are only going to use 20/40 coexist for 2.4G band **/
		if(*(vmacSta_p->ShadowMib802dot11->mib_HT40MIntoler)  &&  !((*(vmacSta_p->Mib802dot11->mib_ApMode)&AP_MODE_BAND_MASK) >= AP_MODE_A_ONLY 
			|| (*(vmacSta_p->Mib802dot11->mib_ApMode)&AP_MODE_BAND_MASK) <= AP_MODE_MIXED))
		{			
#if 0 //optional IE, not require at this time
			{
				extern UINT16 Add20_40_Coexist_IE(vmacApInfo_t *vmacSta_p,IEEEtypes_20_40_BSS_COEXIST_Element_t * pNextElement);

				pCmd->IeListLenHT  += Add20_40_Coexist_IE(wlpptr->vmacSta_p,(IEEEtypes_20_40_BSS_COEXIST_Element_t *)&(pCmd->IeListHT[retval]));
				retval = pCmd->IeListLenHT;	
			}
			{
				extern UINT16 Add20_40Interant_Channel_Report_IE(vmacApInfo_t *vmacSta_p,IEEEtypes_20_40_INTOLERANT_CHANNEL_REPORT_Element_t * pNextElement);

				pCmd->IeListLenHT  += Add20_40Interant_Channel_Report_IE(wlpptr->vmacSta_p,(IEEEtypes_20_40_INTOLERANT_CHANNEL_REPORT_Element_t *)&(pCmd->IeListHT[retval]));
				retval = pCmd->IeListLenHT;	
			}
#endif
			{
				extern UINT16 AddOverlap_BSS_Scan_Parameters_IE(vmacApInfo_t *vmacSta_p,IEEEtypes_OVERLAP_BSS_SCAN_PARAMETERS_Element_t * pNextElement);

				pCmd->IeListLenHT  += AddOverlap_BSS_Scan_Parameters_IE(wlpptr->vmacSta_p,(IEEEtypes_OVERLAP_BSS_SCAN_PARAMETERS_Element_t  *)&(pCmd->IeListHT[retval]));
				retval = pCmd->IeListLenHT;	

			}
			{
				extern UINT16 AddExtended_Cap_IE(vmacApInfo_t *vmacSta_p,IEEEtypes_Extended_Cap_Element_t * pNextElement);
				pCmd->IeListLenHT  += AddExtended_Cap_IE(wlpptr->vmacSta_p,(IEEEtypes_Extended_Cap_Element_t *)&(pCmd->IeListHT[retval]));
				retval = pCmd->IeListLenHT;	
			}
		}

		
#endif
		/*Always add Extended Cap if in 5Ghz and VHT mode to pass wifi operating mode IE199 test*/
		if(*(vmacSta_p->Mib802dot11->mib_ApMode) >= AP_MODE_5GHZ_11AC_ONLY)
		{	
			extern UINT16 AddExtended_Cap_IE(vmacApInfo_t *vmacSta_p,IEEEtypes_Extended_Cap_Element_t * pNextElement);
			pCmd->IeListLenHT  += AddExtended_Cap_IE(wlpptr->vmacSta_p,(IEEEtypes_Extended_Cap_Element_t *)&(pCmd->IeListHT[retval]));
			retval = pCmd->IeListLenHT;	
		}
		
		/*Add 11ac VHT supported IEs*/
		if(*(mib->mib_ApMode)&AP_MODE_11AC)
        {
            pCmd->IeListLenVHT  += Build_IE_191(wlpptr->vmacSta_p,(UINT8*)&(pCmd->IeListVHT[retvalVHT]));
            retvalVHT = pCmd->IeListLenVHT;
            
            pCmd->IeListLenVHT  += Build_IE_192(wlpptr->vmacSta_p,(UINT8*)&(pCmd->IeListVHT[retvalVHT]));
            retvalVHT = pCmd->IeListLenVHT;
        }
		

#ifdef INTEROP		
		pCmd->IeListLenProprietary  +=Add_Generic_HT_IE(wlpptr->vmacSta_p,(IEEEtypes_Generic_HT_Element_t* )&(pCmd->IeListProprietary[retvalProprietary]));
		retvalProprietary = pCmd->IeListLenProprietary;
		pCmd->IeListLenProprietary  +=Add_Generic_AddHT_IE(wlpptr->vmacSta_p,(IEEEtypes_Generic_Add_HT_Element_t*)&(pCmd->IeListProprietary[retvalProprietary]));
		/** For I_COMP only **/
		retvalProprietary = pCmd->IeListLenProprietary;
#endif

	}
	{
		//add M IE
		pCmd->IeListLenProprietary  += AddM_IE(wlpptr->vmacSta_p,(IEEEtypes_HT_Element_t*)&(pCmd->IeListProprietary[retvalProprietary]));
		retvalProprietary = pCmd->IeListLenProprietary;	
	}

	{
		//add M Rptr IE	
		if (*(mib->mib_RptrMode))	
		{
			pCmd->IeListLenProprietary  += AddM_Rptr_IE(wlpptr->vmacSta_p,(IEEEtypes_HT_Element_t*)&(pCmd->IeListProprietary[retvalProprietary]));
			retvalProprietary = pCmd->IeListLenProprietary;	
		}
	}
	retval = wlexecuteCommand(netdev, HostCmd_CMD_SET_IES);
	MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
	return retval;
}
#ifdef POWERSAVE_OFFLOAD
int wlFwSetPowerSaveStation(struct net_device *netdev, u_int8_t StationPowerSave)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	HostCmd_SET_POWERSAVESTATION *pCmd =
		(HostCmd_SET_POWERSAVESTATION *) &wlpptr->pCmdBuf[0];
	int retval = FAIL;
	unsigned long flags;

	WLDBG_ENTER_INFO(DBG_LEVEL_1, " wlFwSetPowerSaveStation %d", StationPowerSave);

	MWL_SPIN_LOCK(&wlpptr->wlpd_p->locks.fwLock);
	memset(pCmd, 0x00, sizeof(HostCmd_SET_POWERSAVESTATION));
	pCmd->CmdHdr.Cmd    = ENDIAN_SWAP16(HostCmd_CMD_SET_POWERSAVESTATION);
	pCmd->CmdHdr.Length = ENDIAN_SWAP16(sizeof(HostCmd_SET_POWERSAVESTATION));
	pCmd->CmdHdr.macid = wlpptr->vmacSta_p->VMacEntry.macId;
	pCmd->NumberofPowersave  = StationPowerSave;

	WLDBG_DUMP_DATA(DBG_LEVEL_1, (void *) pCmd,
		sizeof(HostCmd_SET_POWERSAVESTATION));
	retval = wlexecuteCommand(netdev,HostCmd_CMD_SET_POWERSAVESTATION);
	MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
	return retval;
}
int wlFwSetTIM(struct net_device *netdev, u_int16_t AID, u_int32_t Set )
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	HostCmd_SET_TIM *pCmd =
		(HostCmd_SET_TIM *) &wlpptr->pCmdBuf[0];
	int retval = FAIL;
	unsigned long flags;

	WLDBG_ENTER_INFO(DBG_LEVEL_1, " wlFwSetTim %d %d", AID, Set);

	MWL_SPIN_LOCK(&wlpptr->wlpd_p->locks.fwLock);
	memset(pCmd, 0x00, sizeof(HostCmd_SET_TIM));
	pCmd->CmdHdr.Cmd    = ENDIAN_SWAP16(HostCmd_CMD_SET_TIM);
	pCmd->CmdHdr.Length = ENDIAN_SWAP16(sizeof(HostCmd_SET_TIM));
	pCmd->CmdHdr.macid = wlpptr->vmacSta_p->VMacEntry.macId;
	pCmd->Aid  = ENDIAN_SWAP16(AID);
	pCmd->Set = ENDIAN_SWAP32(Set);

	WLDBG_DUMP_DATA(DBG_LEVEL_1, (void *) pCmd,
		sizeof(HostCmd_SET_TIM));
	retval = wlexecuteCommand(netdev,HostCmd_CMD_SET_TIM);
	MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
	return retval;
}
int wlFwGetTIM(struct net_device *netdev)	  
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	HostCmd_GET_TIM *pCmd =
		(HostCmd_GET_TIM *) &wlpptr->pCmdBuf[0];
	int retval = FAIL,i;
	unsigned long flags;

	//WLDBG_ENTER_INFO(DBG_LEVEL_1, "Optimization %d", mode);

	MWL_SPIN_LOCK(&wlpptr->wlpd_p->locks.fwLock);
	memset(pCmd, 0x00, sizeof(HostCmd_GET_TIM));
	pCmd->CmdHdr.Cmd    = ENDIAN_SWAP16(HostCmd_CMD_GET_TIM);
	pCmd->CmdHdr.Length = ENDIAN_SWAP16(sizeof(HostCmd_GET_TIM));
	pCmd->CmdHdr.macid = wlpptr->vmacSta_p->VMacEntry.macId;

	WLDBG_DUMP_DATA(DBG_LEVEL_1, (void *) pCmd,	sizeof(HostCmd_FW_GET_TIM));
	retval = wlexecuteCommand(netdev, HostCmd_CMD_GET_TIM);
	if(!retval)
	{
		for(i=0;i<10;i++)
			printk(" %x ",	pCmd->TrafficMap[i]);
		//memcpy(pBcn, &pCmd->Bcn, pCmd->Bcnlen);
		//*pLen = pCmd->Bcnlen;
	}
	MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
	return retval;
}	
#endif

int wlexecuteCommand(struct net_device *netdev, unsigned short cmd)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	WLDBG_ENTER_INFO(DBG_LEVEL_0, "%s send cmd 0x%04x to firmware", netdev->name,cmd);

#ifdef SC_PALLADIUM
    if (wlChkAdapter(netdev)) // && (!wlpptr->wlpd_p->inSendCmd))
#else
	if (wlChkAdapter(netdev)  && (!wlpptr->wlpd_p->inSendCmd))
#endif
	{
		wlpptr->wlpd_p->inSendCmd = TRUE;
		wlsendCommand(netdev);
		if (wlwaitForComplete(netdev, 0x8000 | cmd))
		{
			WLDBG_EXIT_INFO(DBG_LEVEL_0, "timeout");
			wlpptr->wlpd_p->inSendCmd = FALSE;
			return TIMEOUT;
		}
		WLDBG_EXIT(DBG_LEVEL_0);
		wlpptr->wlpd_p->inSendCmd = FALSE;
		return SUCCESS;
	}
	if(wlpptr->wlpd_p->inSendCmd == TRUE)
	{
		wlpptr->wlpd_p->inSendCmd = FALSE;
		return SUCCESS;
	}
	wlpptr->wlpd_p->inSendCmd = FALSE;
	WLDBG_EXIT_INFO(DBG_LEVEL_0, "no adapter plugged in");
	return FAIL;
}

static void wlsendCommand(struct net_device *netdev)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);

#if 0//def SOC_W8764
	printk("wlsendCommand pPhysCmdBuf = %x    ioBase1+MACREG_REG_GEN_PTR = %x \n",
		(UINT32) wlpptr->wlpd_p->pPhysCmdBuf, (UINT32) wlpptr->ioBase1+MACREG_REG_GEN_PTR);
#endif
	writel(wlpptr->wlpd_p->pPhysCmdBuf, wlpptr->ioBase1+MACREG_REG_GEN_PTR);
	writel(MACREG_H2ARIC_BIT_DOOR_BELL, 
		wlpptr->ioBase1+MACREG_REG_H2A_INTERRUPT_EVENTS);
}

static int wlwaitForComplete(struct net_device *netdev, u_int16_t cmdCode)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	unsigned int currIteration = MAX_WAIT_FW_COMPLETE_ITERATIONS;
	volatile unsigned short intCode = 0;

	do
	{
		intCode = ENDIAN_SWAP16(wlpptr->pCmdBuf[0]);
		mdelay(1);
#ifdef SC_PALLADIUM
	} while ((intCode != cmdCode)); // && (--currIteration));
#else
	} while ((intCode != cmdCode) && (--currIteration));
#endif

	if (currIteration == 0)
	{
		WLDBG_INFO(DBG_LEVEL_0,  "%s: cmd 0x%04x=%s timed out\n", 
			wlgetDrvName(netdev), cmdCode, wlgetCmdString(cmdCode));
		return TIMEOUT;
	}

	mdelay(3);
	return SUCCESS;
}
#ifdef WL_DEBUG
static char *wlgetCmdString(u_int16_t cmd)
{
	int maxNumCmdEntries = 0;
	int currCmd = 0;
	static const struct
	{
		u_int16_t   cmdCode;
		char       *cmdString;
	} cmds[] = {
		{ HostCmd_CMD_GET_HW_SPEC,           "GetHwSpecifications"    },
		{ HostCmd_CMD_802_11_RADIO_CONTROL,  "SetRadio"               },
		{ HostCmd_CMD_802_11_RF_ANTENNA,     "SetAntenna"             },
		{ HostCmd_CMD_802_11_RTS_THSD,       "SetStationRTSlevel"     },
		{ HostCmd_CMD_SET_INFRA_MODE,        "SetInfraMode"           },
		{ HostCmd_CMD_SET_RATE,              "SetRate"                },
		{ HostCmd_CMD_802_11_SET_SLOT,       "SetStationSlot"         },
		{ HostCmd_CMD_802_11_RF_TX_POWER,    "SetTxPower"             },
		{ HostCmd_CMD_SET_PRE_SCAN,          "SetPrescan"             },
		{ HostCmd_CMD_SET_POST_SCAN,         "SetPostscan"            },
		{ HostCmd_CMD_MAC_MULTICAST_ADR,     "SetMulticastAddr"       },
		{ HostCmd_CMD_SET_WEP,               "SetWepEncryptionKey"    },
		{ HostCmd_CMD_802_11_PTK,            "SetPairwiseTemporalKey" },
		{ HostCmd_CMD_802_11_GTK,            "SetGroupTemporalKey"    },
		{ HostCmd_CMD_SET_MAC_ADDR,          "SetMACaddress"          },
		{ HostCmd_CMD_SET_BEACON,            "SetStationBeacon"       },
		{ HostCmd_CMD_AP_BEACON,             "SetApBeacon"            },
		{ HostCmd_CMD_SET_FINALIZE_JOIN,     "SetFinalizeJoin"        },
		{ HostCmd_CMD_SET_AID,               "SetAid"                 },
		{ HostCmd_CMD_SET_RF_CHANNEL,        "SetChannel"             },
		{ HostCmd_CMD_802_11_GET_STAT,       "GetFwStatistics"        },
		{ HostCmd_CMD_BSS_START,             "SetBSSstart"            },
		{ HostCmd_CMD_UPDATE_TIM,            "SetTIM"                 },
		{ HostCmd_CMD_BROADCAST_SSID_ENABLE, "SetBroadcastSSID"       },
		{ HostCmd_CMD_WDS_ENABLE,            "SetWDS"                 },
		{ HostCmd_CMD_SET_BURST_MODE,        "SetBurstMode"           },
		{ HostCmd_CMD_SET_G_PROTECT_FLAG,    "SetGprotectionFlag"     },
		{ HostCmd_CMD_802_11_BOOST_MODE,     "SetBoostMode"           },
	};

	maxNumCmdEntries = sizeof(cmds) / sizeof(cmds[0]);
	for (currCmd = 0; currCmd < maxNumCmdEntries; currCmd++)
	{
		if ((cmd & 0x7fff) == cmds[currCmd].cmdCode)
		{
			return cmds[currCmd].cmdString;
		}
	}
	return "unknown";
}

static char *wlgetCmdResultString(u_int16_t result)
{
	int maxNumResultEntries = 0;
	int currResult = 0;
	static const struct
	{
		u_int16_t   resultCode;
		char       *resultString;
	} results[] = {
		{ HostCmd_RESULT_OK,           "ok"            },
		{ HostCmd_RESULT_ERROR,        "general error" },
		{ HostCmd_RESULT_NOT_SUPPORT,  "not supported" },
		{ HostCmd_RESULT_PENDING,      "pending"       },
		{ HostCmd_RESULT_BUSY,         "ignored"       },
		{ HostCmd_RESULT_PARTIAL_DATA, "incomplete"    },
	};

	maxNumResultEntries = sizeof(results) / sizeof(results[0]);
	for (currResult = 0; currResult < maxNumResultEntries; currResult++)
	{
		if (result == results[currResult].resultCode)
		{
			return results[currResult].resultString;
		}
	}
	return "unknown";
}

static char *wlgetDrvName(struct net_device *netdev)
{
	if (strchr(netdev->name, '%'))
	{
		return DRV_NAME;
	}
	return netdev->name;
}
#endif
int	wlchannelSet(struct net_device *netdev, int channel, CHNL_FLAGS chanflag, u_int8_t initRateTable)
{
	//	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	WLDBG_ENTER(DBG_LEVEL_0);

	if (wlFwSetChannel(netdev, channel, chanflag,initRateTable))
	{
		WLDBG_WARNING(DBG_LEVEL_0, "channel set failed");
	}

	WLDBG_EXIT(DBG_LEVEL_0);
	return SUCCESS;
}

#ifdef IEEE80211_DH
int wlFwApplyChannelSettings(struct net_device *netdev)
{
	int retval = SUCCESS;
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	vmacApInfo_t *vmacSta_p = wlpptr->vmacSta_p;
	//	MIB_802DOT11 *mib = vmacSta_p->Mib802dot11;
	MIB_802DOT11 *mib = vmacSta_p->ShadowMib802dot11;
	MIB_SPECTRUM_MGMT	*mib_SpectrumMagament_p=mib->SpectrumMagament;
	MIB_PHY_DSSS_TABLE *PhyDSSSTable=mib->PhyDSSSTable;


	if (wlchannelSet(netdev, PhyDSSSTable->CurrChan, PhyDSSSTable->Chanflag, 1))
	{
		WLDBG_EXIT_INFO(DBG_LEVEL_0, "setting channel");
		retval = FAIL;
	}
	if (wlFwSetApBeacon(netdev))
	{
		WLDBG_EXIT_INFO(DBG_LEVEL_0, "setting AP beacon");
		retval = FAIL;
	}
	if (wlFwSetAPBss(netdev, WL_ENABLE))
	{
		WLDBG_WARNING(DBG_LEVEL_0, "enabling AP bss");
		retval = FAIL;
	}
	if( wlFwSetSpectrumMgmt( netdev, mib_SpectrumMagament_p->spectrumManagement ))
	{
		WLDBG_WARNING(DBG_LEVEL_0, "enabling spectrum management");
		retval = FAIL;
	}
	if( wlFwSetCountryCode( netdev, mib_SpectrumMagament_p->countryCode ))
	{
		WLDBG_WARNING(DBG_LEVEL_0, "enabling country code info");
		retval = FAIL;
	}
	return retval ;
}

#endif // IEEE80211_DH
static BOOLEAN AnyDevAmsduEnabled(struct net_device *netdev)
{
#if defined(MBSS)
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	struct wlprivate *wlpptr1;
	vmacApInfo_t *vmacSta_p;
	MIB_802DOT11 *mib;
	int i=0;
	while(i <=MAX_VMAC_INSTANCE_AP )
	{
		if(wlpptr->vdev[i]){
			wlpptr1 = NETDEV_PRIV_P(struct wlprivate, wlpptr->vdev[i]);
			vmacSta_p = wlpptr1->vmacSta_p;
			mib = vmacSta_p->Mib802dot11;
			if(wlpptr->vdev[i]->flags & IFF_RUNNING)
			{
				if(*(mib->pMib_11nAggrMode) & WL_MODE_AMSDU_TX_MASK)
					return TRUE;
			}
		}
		i++;
	}
#endif
	return FALSE;
}
int wlFwApplySettings(struct net_device *netdev)
{
	int retval = SUCCESS;
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	vmacApInfo_t *vmacSta_p = wlpptr->vmacSta_p;
	MIB_802DOT11 *mib = vmacSta_p->Mib802dot11;
	MIB_802DOT11 *mib1 = vmacSta_p->ShadowMib802dot11;
	MIB_PHY_DSSS_TABLE *PhyDSSSTable=mib->PhyDSSSTable;
	MIB_PHY_DSSS_TABLE *PhyDSSSTable1=mib1->PhyDSSSTable;
	MIB_SPECTRUM_MGMT	*mib_SpectrumMagament_p=mib->SpectrumMagament;
	MIB_SPECTRUM_MGMT	*mib_SpectrumMagament1_p=mib1->SpectrumMagament;
	UINT8 *mib_guardInterval_p = mib->mib_guardInterval;

#ifdef MRVL_DFS
	BOOLEAN		bChannelChanged = FALSE ;
	UINT8		currDFSMode = 0, newDFSMode = 0 ;

	/* Check if channel is going to be modified. DFS will be kicked
	in only if a channel change occur here. 
	*/
	if( PhyDSSSTable->CurrChan != PhyDSSSTable1->CurrChan )
	{
		bChannelChanged = TRUE ;
	}
	currDFSMode = mib_SpectrumMagament_p->spectrumManagement ;
	newDFSMode = mib_SpectrumMagament1_p->spectrumManagement ;
    if (*mib->mib_autochannel && newDFSMode && !vmacSta_p->dfsCacExp)
        newDFSMode = vmacSta_p->autochannelstarted;    
#endif


	mib_Update();
#ifdef MFG_SUPPORT	
	if (wlpptr->mfgEnable)
	{		
		return retval;
	}
#endif
#ifdef SINGLE_DEV_INTERFACE
	if(wlFwSetMacAddr(netdev))
	{
		WLDBG_EXIT_INFO(DBG_LEVEL_0, "setting dev mac");
		retval = FAIL;
	}
#endif
	if (wlFwGetRegionCode(netdev))
	{
		WLDBG_EXIT_INFO(DBG_LEVEL_0, "getting Region code");
		retval = FAIL;
	}
	if (wlFwSetAntenna(netdev, WL_ANTENNATYPE_RX))
	{
		WLDBG_EXIT_INFO(DBG_LEVEL_0, "setting RX antenna");
		retval = FAIL;
	}
	if (wlFwSetAntenna(netdev, WL_ANTENNATYPE_TX))
	{
		WLDBG_EXIT_INFO(DBG_LEVEL_0, "setting TX antenna");
		retval = FAIL;
	}
	if( wlFwSetHTStbc(netdev, *mib->mib_HtStbc))
	{
		WLDBG_EXIT_INFO(DBG_LEVEL_0, "setting HT STBC");
		retval = FAIL;
	}
#if defined(SOC_W8764)
	if( wlFwSetBFType(netdev, *mib->mib_bftype))
	{
		WLDBG_EXIT_INFO(DBG_LEVEL_0, "setting BF TYPE");
		retval = FAIL;
	}
#endif
	if( wlFwSetBWSignalType(netdev, *mib->mib_bwSignaltype))
	{
		WLDBG_EXIT_INFO(DBG_LEVEL_0, "setting BW Signal TYPE");
		retval = FAIL;
	}

	if (wlFwSetRadio(netdev, WL_ENABLE, mib->StationConfig->mib_preAmble))
	{
		WLDBG_EXIT_INFO(DBG_LEVEL_0, "setting auto preamble");
		retval = FAIL;
	}
	if (wlchannelSet(netdev, PhyDSSSTable->CurrChan, PhyDSSSTable->Chanflag, 1))
	{
		WLDBG_EXIT_INFO(DBG_LEVEL_0, "setting channel");
		retval = FAIL;
	}
	if (wlFwSetRegionCode(netdev, *(mib->mib_regionCode)))
	{
		WLDBG_WARNING(DBG_LEVEL_1, "setting region code");
		retval = FAIL;
	}
#ifdef MRVL_DFS
	if(DecideDFSOperation(netdev, bChannelChanged, currDFSMode, newDFSMode , mib))
	{
		WLDBG_EXIT_INFO(DBG_LEVEL_1, "DFS Operation");
		retval = FAIL;
	}
#endif //MRVL_DFS
	if (wlFwGetPwrCalTable(netdev))
	{
		WLDBG_EXIT_INFO(DBG_LEVEL_1, "getting cal power table");
		retval = FAIL;
	}
	if( wlFwSetMaxTxPwr(netdev))
	{
		WLDBG_EXIT_INFO(DBG_LEVEL_0, "setting Max Tx Power");
		retval = FAIL;
	}
	if (wlFwSetTxPower(netdev, HostCmd_ACT_GEN_SET_LIST, 0))
	{
		WLDBG_EXIT_INFO(DBG_LEVEL_0, "setting tx power");
		retval = FAIL;
	}
#if defined(SOC_W8366)||defined(SOC_W8764)
	if (wlFwSetAntenna(netdev, WL_ANTENNATYPE_TX2))
	{
		WLDBG_EXIT_INFO(DBG_LEVEL_0, "setting TX antenna2");
		retval = FAIL;
	}
#endif
	if( wlFwSetCDD(netdev, *mib->mib_CDD))
	{
		WLDBG_EXIT_INFO(DBG_LEVEL_0, "setting CDD");
		retval = FAIL;
	}
	if (wlFwSetIEs(netdev))
	{
		WLDBG_WARNING(DBG_LEVEL_0, "setting IEs");
		retval = FAIL;
	}
	if (wlFwSetAPBcastSSID(netdev, *(mib->mib_broadcastssid)))
	{
		WLDBG_WARNING(DBG_LEVEL_0, "setting hidden ssid");
		retval = FAIL;
	}
	if( wlFwSetWmm(netdev,*(mib->QoSOptImpl)))
	{
		WLDBG_EXIT_INFO(DBG_LEVEL_0, "setting qos option");
		retval = FAIL;
	}
	if( wlFwSetApMode(netdev,*(mib->mib_ApMode)))
	{
		WLDBG_EXIT_INFO(DBG_LEVEL_0, "setting AP MODE");
		retval = FAIL;
	}
	if( wlFwHTGI(netdev,*mib_guardInterval_p))
	{
		WLDBG_EXIT_INFO(DBG_LEVEL_0, "setting HT GI");
		retval = FAIL;
	}
	if( wlFwSetAdaptMode(netdev))
	{
		WLDBG_EXIT_INFO(DBG_LEVEL_0, "setting Adpat mode");
		retval = FAIL;
	}
	if( wlFwSetCSAdaptMode(netdev))
	{
		WLDBG_EXIT_INFO(DBG_LEVEL_0, "setting CS Adpat mode");
		retval = FAIL;
	}
#ifdef RXPATHOPT
	if( wlFwSetRxPathOpt(netdev, *(mib->mib_RxPathOpt)))
	{
		WLDBG_EXIT_INFO(DBG_LEVEL_0, "setting RxPathOpt");
		retval = FAIL;
	}
#endif  
	
#ifdef V6FW
	/* For V5 and V6 firmware need to enable DwdsStaMode for AP operation. */
	/* This can be disabled if switched to station mode later. */
	if( wlFwSetDwdsStaMode(netdev, 1))
	{
		WLDBG_EXIT_INFO(DBG_LEVEL_0, "setting DwdsStaMode mode");
		retval = FAIL;
	}
#endif  
	//always turn flush timer off, and let VAP to turn it on.
	if( wlFwSetFwFlushTimer(netdev, 0))
	{
		WLDBG_EXIT_INFO(DBG_LEVEL_0, "setting Fw Flush timer");
		retval = FAIL;
	}
	if(*(mib->QoSOptImpl))
	{
		/** update qos param to fw here **/
		int i=0;
		for(i=0;i<4;i++)
		{

			/* NOTE: Sequence of hostcmd:
			wlFwSetEdcaParam
			wlFwSetOptimizationLevel if using HiPerfMode
			wlFwSetRate if using FixedRate
			*/
			if(wlFwSetEdcaParam(netdev, i, mib_QAPEDCATable[i].QAPEDCATblCWmin, mib_QAPEDCATable[i].QAPEDCATblCWmax, 
				mib_QAPEDCATable[i].QAPEDCATblAIFSN, mib_QAPEDCATable[i].QAPEDCATblTXOPLimit))
			{
				WLDBG_EXIT_INFO(DBG_LEVEL_0, "setting qos option");
				retval = FAIL;
			}
		}

	}
#ifdef SINGLE_DEV_INTERFACE
	if (wlFwSetApBeacon(netdev))
	{
		WLDBG_EXIT_INFO(DBG_LEVEL_0, "setting AP beacon");
		retval = FAIL;
	}
#endif
	/* NOTE: Sequence of hostcmd:
	wlFwSetEdcaParam
	wlFwSetOptimizationLevel if using HiPerfMode
	wlFwSetRate if using FixedRate
	*/
	if (wlFwSetOptimizationLevel(netdev, *(mib->mib_optlevel)))
	{
		WLDBG_WARNING(DBG_LEVEL_0, "set Optimization level");
		retval = FAIL;
	}
	/* NOTE: Sequence of hostcmd:
	wlFwSetEdcaParam
	wlFwSetOptimizationLevel if using HiPerfMode
	wlFwSetRate if using FixedRate
	*/
	if(wlFwSetRate(netdev, *(mib->mib_enableFixedRateTx)))
	{
		WLDBG_WARNING(DBG_LEVEL_0, "setting tx rate");
		retval = FAIL;
	}
	if(wlFwSetRTSThreshold(netdev, *(mib->mib_RtsThresh)))
	{
		WLDBG_WARNING(DBG_LEVEL_0, "setting rts threshold");
		retval = FAIL;
	}
#ifdef SINGLE_DEV_INTERFACE
	if (wlFwSetAPBss(netdev, WL_ENABLE))
	{
		WLDBG_WARNING(DBG_LEVEL_0, "enabling AP bss");
		retval = FAIL;
	}
#endif
	{
		extern int set_sta_aging_time(vmacApInfo_t *vmacSta_p,int minutes);
		set_sta_aging_time(vmacSta_p,*(mib->mib_agingtime)/60);
	}
	{
		extern int set_rptrSta_aging_time(vmacApInfo_t *vmacSta_p,int minutes);
		set_rptrSta_aging_time(vmacSta_p,*(mib->mib_agingtimeRptr)/60);
	}

#ifdef IEEE80211_DH
	if( wlFwSetSpectrumMgmt( netdev, mib_SpectrumMagament_p->spectrumManagement ))
	{
		WLDBG_WARNING(DBG_LEVEL_0, "enabling spectrum management");
		retval = FAIL;
	}
	/* power constraint ino IE should appear in beacon only on 5GHz */
	if(( mib_SpectrumMagament_p->spectrumManagement ) &&
		(PhyDSSSTable->Chanflag.FreqBand == FREQ_BAND_5GHZ ))
	{
		if(wlFwSetPowerConstraint( netdev, mib_SpectrumMagament_p->powerConstraint ))
		{
			WLDBG_WARNING(DBG_LEVEL_0, "setting power constraint info element");
			retval = FAIL;
		}
	}

	if( mib_SpectrumMagament_p->spectrumManagement && 
		mib_SpectrumMagament_p->multiDomainCapability )
	{
		if( wlFwSetCountryCode( netdev, mib_SpectrumMagament_p->countryCode ) )
		{
			WLDBG_WARNING(DBG_LEVEL_0, "setting country code info element");
			retval = FAIL;
		}
	}
	else 
	{
		/* remove the country code element from beacon */
		if( wlFwSetCountryCode( netdev, 0 ) )
		{
			WLDBG_WARNING(DBG_LEVEL_0, "setting country code info element");
			retval = FAIL;
		}
	}
#endif

	if (wlFwSetNProt(netdev, *(mib->mib_htProtect)))
	{
		WLDBG_WARNING(DBG_LEVEL_0, "set N protection");
		retval = FAIL;
	}
#ifdef AMPDU_SUPPORT
	if(*(mib->mib_rifsQNum)!=0)
		if (wlFwSetRifs(netdev, *(mib->mib_rifsQNum)))
		{
			WLDBG_WARNING(DBG_LEVEL_1, "set RIFS");
			retval = FAIL;
		}
#endif
#ifdef CLIENTONLY	
		wlFwSetNewStn(netdev,vmacSta_p->macStaAddr, 0, 0, 0, NULL,0,0);  //add new station
		wlFwSetSecurity(netdev,vmacSta_p->macStaAddr);
		if (vmacSta_p->Mib802dot11->Privacy->RSNEnabled) 
		{
			KeyMgmtInit(vmacSta_p);
		}
#endif
#ifdef SINGLE_DEV_INTERFACE
		SendResetCmd(wlpptr->vmacSta_p, 0);
#ifdef WDS_FEATURE
		wlFwSetWdsMode(netdev);
#endif
#endif
		
		if(wlFwSetConsecTxFailLimit(netdev, *(mib->mib_consectxfaillimit)))
		{
			WLDBG_EXIT_INFO(DBG_LEVEL_0, "setting consecutive txfail limit");
			retval = FAIL;
		}

		return SUCCESS;
}
int wlFwMultiBssApplySettings(struct net_device *netdev)
{
	int retval = SUCCESS;
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	vmacApInfo_t *vmacSta_p = wlpptr->vmacSta_p;
	MIB_802DOT11 *mib = vmacSta_p->Mib802dot11;
	//MIB_PHY_DSSS_TABLE *PhyDSSSTable=mib->PhyDSSSTable;
	MIB_SPECTRUM_MGMT	*mib_SpectrumMagament_p=mib->SpectrumMagament;
#ifdef MRVL_WSC
	WSC_COMB_IE_t *combIE = NULL ;
#endif

#ifdef MRVL_DFS
	/* If currently DFS scanning no need to apply MBSS settings */
	if(( DfsGetCurrentState( wlpptr->wlpd_p->pdfsApMain)) == DFS_STATE_SCAN )
	{
		/* Current State is DFS_STATE_SCAN. Beacons should not be sent out. 
		So do not proceed further.
		*/
		return SUCCESS ;
	}
#endif //MRVL_DFS
	mib_Update();
	/* in case virtual interface MAC addr changed (by cmd "bssid" etc.) */
	memcpy(wlpptr->hwData.macAddr, netdev->dev_addr, sizeof(IEEEtypes_MacAddr_t));
	if (wlFwSetIEs(netdev))
	{
		WLDBG_WARNING(DBG_LEVEL_0, "setting IEs");
		retval = FAIL;
	}
	if (wlFwSetAPBcastSSID(netdev, *(mib->mib_broadcastssid)))
	{
		WLDBG_WARNING(DBG_LEVEL_0, "setting hidden ssid");
		retval = FAIL;
	}
	if (wlFwSetApBeacon(netdev))
	{
		WLDBG_EXIT_INFO(DBG_LEVEL_0, "setting AP beacon");
		retval = FAIL;
	}
	disableAmpduTxAll(vmacSta_p);
	if (wlFwSetAPBss(netdev, WL_ENABLE))
	{
		WLDBG_WARNING(DBG_LEVEL_0, "enabling AP bss");
		retval = FAIL;
	}
#ifdef IEEE80211_DH
	if( wlFwSetSpectrumMgmt( netdev, mib_SpectrumMagament_p->spectrumManagement ))
	{
		WLDBG_WARNING(DBG_LEVEL_0, "enabling spectrum management");
		retval = FAIL;
	}
	/* power constraint ino IE should appear in beacon only on 5GHz */
	if(( mib_SpectrumMagament_p->spectrumManagement ) &&
		((*(mib->mib_ApMode) == AP_MODE_A_ONLY) 
		||(*(mib->mib_ApMode) == AP_MODE_AandN)
#ifdef SOC_W8864		
		||(*(mib->mib_ApMode) == AP_MODE_5GHZ_Nand11AC)
		||(*(mib->mib_ApMode) == AP_MODE_5GHZ_11AC_ONLY)
#endif		
		||(*(mib->mib_ApMode) == AP_MODE_N_ONLY)
		||(*(mib->mib_ApMode) == AP_MODE_5GHZ_N_ONLY)))
	{
		if(wlFwSetPowerConstraint( netdev, mib_SpectrumMagament_p->powerConstraint ))
		{
			WLDBG_WARNING(DBG_LEVEL_0, "setting power constraint info element");
			retval = FAIL;
		}
	}

	if( mib_SpectrumMagament_p->spectrumManagement && 
		mib_SpectrumMagament_p->multiDomainCapability )
	{
		if( wlFwSetCountryCode( netdev, mib_SpectrumMagament_p->countryCode ) )
		{
			WLDBG_WARNING(DBG_LEVEL_0, "setting country code info element");
			retval = FAIL;
		}
	}
	else 
	{
		/* remove the country code element from beacon */
		if( wlFwSetCountryCode( netdev, 0 ) )
		{
			WLDBG_WARNING(DBG_LEVEL_0, "setting country code info element");
			retval = FAIL;
		}
	}
#endif

	if((*(mib->pMib_11nAggrMode) & WL_MODE_AMSDU_TX_MASK) ||AnyDevAmsduEnabled(wlpptr->master))
	{
		if( wlFwSetFwFlushTimer(netdev, *(mib->mib_amsdu_flushtime)))
		{
			WLDBG_EXIT_INFO(DBG_LEVEL_0, "setting Fw Flush timer failed");
			retval = FAIL;
		}
	}
	else
	{
		if( wlFwSetFwFlushTimer(netdev, 0))
		{
			WLDBG_EXIT_INFO(DBG_LEVEL_0, "setting Fw Flush timer failed");
			retval = FAIL;
		}
	}
#ifdef MRVL_WSC
	combIE = kmalloc( sizeof(WSC_COMB_IE_t), GFP_ATOMIC );
	if( combIE == NULL )
	{
		printk("No memory left for WPS IE\n");
		retval = FAIL;
	}
	else
	{
		memset( combIE, 0, sizeof(WSC_COMB_IE_t));
		if( vmacSta_p->thisbeaconIE.Len != 0 )
		{
			memcpy( &combIE->beaconIE, &vmacSta_p->thisbeaconIE, sizeof( WSC_BeaconIE_t ) );
			if(wlFwSetWscIE(netdev, 0, combIE ))
			{
				WLDBG_WARNING(DBG_LEVEL_1, "Setting Beacon WSC IE");
				retval = FAIL;
			}
		}
		if( vmacSta_p->thisprobeRespIE.Len != 0 )
		{
			memcpy( &combIE->probeRespIE, &vmacSta_p->thisprobeRespIE, sizeof( WSC_ProbeRespIE_t ) );
			if(wlFwSetWscIE(netdev, 1, combIE ))
			{
				WLDBG_WARNING(DBG_LEVEL_1, "Setting Probe Response WSC IE");
				retval = FAIL;
			}
		}
		kfree(combIE);
	}
#endif

	SendResetCmd(wlpptr->vmacSta_p, 0);
#ifdef WDS_FEATURE
	// WDS ports must be initialized here since WDS ports are added to station
	// database after Mac reset and initialization.
	AP_InitWdsPorts(wlpptr);
	wlFwSetWdsMode(netdev);
#endif
#ifdef AMPDU_SUPPORT
	if(*(mib->mib_rifsQNum)!=0)
		if (wlFwSetRifs(netdev, *(mib->mib_rifsQNum)))
		{
			WLDBG_WARNING(DBG_LEVEL_1, "set RIFS");
			retval = FAIL;
		}
#endif
		wlFwSetNewStn(netdev,vmacSta_p->macStaAddr, 0, 0, 0, NULL,0,0);  //add new station
		wlFwSetSecurity(netdev,vmacSta_p->macStaAddr);
		if (vmacSta_p->Mib802dot11->Privacy->RSNEnabled) 
		{
			KeyMgmtInit(vmacSta_p);
		}
		#ifdef MRVL_WAPI
        {
            UINT32 *pDW = (UINT32 *)vmacSta_p->wapiPN;
            *pDW++ = 0x5c365c37;
            *pDW++ = 0x5c365c36;
            *pDW++ = 0x5c365c36;
            *pDW = 0x5c365c36;
            pDW = (UINT32 *)vmacSta_p->wapiPN_mc;
            *pDW++ = 0x5c365c36;
            *pDW++ = 0x5c365c36;
            *pDW++ = 0x5c365c36;
            *pDW = 0x5c365c36;
        }    
        #endif
		/* reset HtOpMode, inform both driver and f/w */
		wlpptr->wlpd_p->BcnAddHtOpMode = 0;
		wlFwSetNProtOpMode(netdev, 0);

		return SUCCESS;
}
#ifdef CLIENT_SUPPORT
int wlFwApplyClientSettings(struct net_device *netdev)
{
	int retval = SUCCESS;
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	vmacApInfo_t *vmacSta_p = wlpptr->vmacSta_p;
	MIB_802DOT11 *mib = vmacSta_p->Mib802dot11;

	mib_Update();
#ifdef MFG_SUPPORT	
	if (wlpptr->mfgEnable)
	{		
		return retval;
	}
#endif
	if((*(mib->pMib_11nAggrMode) & WL_MODE_AMSDU_TX_MASK) ||AnyDevAmsduEnabled(wlpptr->master))
	{
		if( wlFwSetFwFlushTimer(netdev, *(mib->mib_amsdu_flushtime)))
		{
			WLDBG_EXIT_INFO(DBG_LEVEL_0, "setting Fw Flush timer failed");
			retval = FAIL;
		}
	}
	else
	{
		if( wlFwSetFwFlushTimer(netdev, 0))
		{
			WLDBG_EXIT_INFO(DBG_LEVEL_0, "setting Fw Flush timer failed");
			retval = FAIL;
		}
	}
	if (wlFwSetAPBss(netdev, WL_ENABLE))
	{
		WLDBG_WARNING(DBG_LEVEL_0, "enabling AP bss");
		retval = FAIL;
	}
	if (wlFwSetInfraMode(netdev))
	{
		WLDBG_WARNING(DBG_LEVEL_0, "enabling Sta mode");
		retval = FAIL;
	}

	return SUCCESS;
}
#endif
int wlFwGetAddrValue(struct net_device *netdev,  UINT32 addr, UINT32 len, UINT32 *val, UINT16 set)
{
	int i ;
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	HostCmd_DS_MEM_ADDR_ACCESS *pCmd =
		(HostCmd_DS_MEM_ADDR_ACCESS *) &wlpptr->pCmdBuf[0];
	int retval = FAIL;
	unsigned long flags; 

	if( !val )
		return retval ;
	WLDBG_ENTER(DBG_LEVEL_1);

	MWL_SPIN_LOCK(&wlpptr->wlpd_p->locks.fwLock);
	memset(pCmd, 0x00, sizeof(HostCmd_DS_MEM_ADDR_ACCESS));
	pCmd->CmdHdr.Cmd     = ENDIAN_SWAP16(HostCmd_CMD_MEM_ADDR_ACCESS);
	pCmd->CmdHdr.Length  = ENDIAN_SWAP16(sizeof(HostCmd_DS_MEM_ADDR_ACCESS));
	pCmd->Address =  ENDIAN_SWAP32(addr);
	pCmd->Length = ENDIAN_SWAP16(len);
	pCmd->Value[0] = ENDIAN_SWAP32(*val);
	pCmd->Reserved = ENDIAN_SWAP16(set);
	WLDBG_DUMP_DATA(DBG_LEVEL_1, (void *) pCmd,
		sizeof(HostCmd_DS_MEM_ADDR_ACCESS));
	retval = wlexecuteCommand(netdev, HostCmd_CMD_MEM_ADDR_ACCESS);
	MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
	if(!retval)
	{
		for( i = 0 ; i < len ; i ++ )
			val[i] = ENDIAN_SWAP32(pCmd->Value[i]);
	}
	return retval;
}

int wlRegMac(struct net_device *netdev,  UINT8 flag, UINT32 reg, UINT32 *val)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	HostCmd_DS_MAC_REG_ACCESS *pCmd =
		(HostCmd_DS_MAC_REG_ACCESS *) &wlpptr->pCmdBuf[0];
	int retval = FAIL;

	unsigned long flags; 

	WLDBG_ENTER(DBG_LEVEL_0);

	MWL_SPIN_LOCK(&wlpptr->wlpd_p->locks.fwLock);
	memset(pCmd, 0x00, sizeof(HostCmd_DS_MAC_REG_ACCESS));
	pCmd->CmdHdr.Cmd     = ENDIAN_SWAP16(HostCmd_CMD_MAC_REG_ACCESS);
	pCmd->CmdHdr.Length  = ENDIAN_SWAP16(sizeof(HostCmd_DS_MAC_REG_ACCESS));
	pCmd->Offset =  ENDIAN_SWAP16(reg);
	pCmd->Action = ENDIAN_SWAP16(flag);
	pCmd->Value = ENDIAN_SWAP32(*val);
	WLDBG_DUMP_DATA(DBG_LEVEL_0, (void *) pCmd,
		sizeof(HostCmd_DS_MAC_REG_ACCESS));
	retval = wlexecuteCommand(netdev, HostCmd_CMD_MAC_REG_ACCESS);
	if(!retval)
		*val = ENDIAN_SWAP32(pCmd->Value);
	MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
	return retval;
}
int wlRegRF(struct net_device *netdev, UINT8 flag, UINT32 reg, UINT32 *val)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	HostCmd_DS_RF_REG_ACCESS *pCmd =
		(HostCmd_DS_RF_REG_ACCESS *) &wlpptr->pCmdBuf[0];
	int retval = FAIL;

	unsigned long flags; 

	WLDBG_ENTER(DBG_LEVEL_0);

	MWL_SPIN_LOCK(&wlpptr->wlpd_p->locks.fwLock);
	memset(pCmd, 0x00, sizeof(HostCmd_DS_MAC_REG_ACCESS));
	pCmd->CmdHdr.Cmd     = ENDIAN_SWAP16(HostCmd_CMD_RF_REG_ACCESS);
	pCmd->CmdHdr.Length  = ENDIAN_SWAP16(sizeof(HostCmd_DS_RF_REG_ACCESS));
	pCmd->Offset =  ENDIAN_SWAP16(reg);
	pCmd->Action = ENDIAN_SWAP16(flag);
	pCmd->Value = *val;
	WLDBG_DUMP_DATA(DBG_LEVEL_0, (void *) pCmd,
		sizeof(HostCmd_DS_RF_REG_ACCESS));
	retval = wlexecuteCommand(netdev, HostCmd_CMD_RF_REG_ACCESS);
	if(!retval)
		*val = pCmd->Value;
	MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
	return retval;
}
int wlRegBB(struct net_device *netdev, UINT8 flag, UINT32 reg, UINT32 *val)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	HostCmd_DS_BBP_REG_ACCESS *pCmd =
		(HostCmd_DS_BBP_REG_ACCESS *) &wlpptr->pCmdBuf[0];
	int retval = FAIL;

	unsigned long flags; 

	WLDBG_ENTER(DBG_LEVEL_0);

	MWL_SPIN_LOCK(&wlpptr->wlpd_p->locks.fwLock);
	memset(pCmd, 0x00, sizeof(HostCmd_DS_BBP_REG_ACCESS));
	pCmd->CmdHdr.Cmd     = ENDIAN_SWAP16(HostCmd_CMD_BBP_REG_ACCESS);
	pCmd->CmdHdr.Length  = ENDIAN_SWAP16(sizeof(HostCmd_DS_BBP_REG_ACCESS));
	pCmd->Offset =  ENDIAN_SWAP16(reg);
	pCmd->Action = ENDIAN_SWAP16(flag);
	pCmd->Value = *val;
	WLDBG_DUMP_DATA(DBG_LEVEL_0, (void *) pCmd,
		sizeof(HostCmd_DS_BBP_REG_ACCESS));
	retval = wlexecuteCommand(netdev, HostCmd_CMD_BBP_REG_ACCESS);
	if(!retval)
		*val = pCmd->Value;
	MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
	return retval;
}
int wlRegCAU(struct net_device *netdev, UINT8 flag, UINT32 reg, UINT32 *val)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	HostCmd_DS_BBP_REG_ACCESS *pCmd =
		(HostCmd_DS_BBP_REG_ACCESS *) &wlpptr->pCmdBuf[0];
	int retval = FAIL;

	unsigned long flags; 

	WLDBG_ENTER(DBG_LEVEL_0);

	MWL_SPIN_LOCK(&wlpptr->wlpd_p->locks.fwLock);
	memset(pCmd, 0x00, sizeof(HostCmd_DS_BBP_REG_ACCESS));
	pCmd->CmdHdr.Cmd     = ENDIAN_SWAP16(HostCmd_CMD_CAU_REG_ACCESS);
	pCmd->CmdHdr.Length  = ENDIAN_SWAP16(sizeof(HostCmd_DS_BBP_REG_ACCESS));
	pCmd->Offset =  ENDIAN_SWAP16(reg);
	pCmd->Action = ENDIAN_SWAP16(flag);
	pCmd->Value = *val;
	WLDBG_DUMP_DATA(DBG_LEVEL_0, (void *) pCmd,
		sizeof(HostCmd_DS_BBP_REG_ACCESS));
	retval = wlexecuteCommand(netdev, HostCmd_CMD_CAU_REG_ACCESS);
	if(!retval)
		*val = pCmd->Value;
	MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
	return retval;
}

UINT32 PciReadMacReg(struct net_device *netdev,UINT32 offset)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	return ENDIAN_SWAP32(WL_REGS32(MAC_REG_ADDR_PCI(offset)));
}
void PciWriteMacReg(struct net_device *netdev,UINT32 offset, UINT32 val)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	WL_WRITE_WORD(MAC_REG_ADDR_PCI(offset), ENDIAN_SWAP32(val));
}

static int wlFwSetMaxTxPwr(struct net_device *netdev)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	vmacApInfo_t *vmacSta_p = wlpptr->vmacSta_p;
#if defined(SOC_W8366)||defined(SOC_W8764)
	MIB_802DOT11 *mib = vmacSta_p->ShadowMib802dot11;
	int reduceVal=0;
	int i;
	UINT16 maxtxpow[TX_POWER_LEVEL_TOTAL];
	UINT16 tmp;
#ifdef SOC_W8864
    UINT16 chAutoWidth = ((mib->PhyDSSSTable->Chanflag.FreqBand == 
                           FREQ_BAND_2DOT4GHZ)?CH_40_MHz_WIDTH:CH_80_MHz_WIDTH);
#endif
#ifdef PWRFRAC
	switch (*(mib->mib_TxPwrFraction))
	{
	case 0:
		reduceVal = 0; /* Max */
		break;	
	case 1:
		reduceVal = 2; /* 75% -1.25db */
		break;	
	case 2:
		reduceVal = 3;  /* 50% -3db */
		break;	
	case 3:
		reduceVal = 6; /* 25% -6db */
		break;	

	default:
		reduceVal = *(mib->mib_MaxTxPwr);	 /* larger than case 3,  pCmd->MaxPowerLevel is min */
		break;	
	}
#endif
	if((mib->PhyDSSSTable->powinited&2) == 0)
	{	
		wlFwGettxpower(netdev, mib->PhyDSSSTable->maxTxPow, mib->PhyDSSSTable->CurrChan,
			mib->PhyDSSSTable->Chanflag.FreqBand, 
#ifdef SOC_W8864
			((mib->PhyDSSSTable->Chanflag.ChnlWidth == CH_AUTO_WIDTH)?chAutoWidth:mib->PhyDSSSTable->Chanflag.ChnlWidth),
#else
			((mib->PhyDSSSTable->Chanflag.ChnlWidth == CH_AUTO_WIDTH)?CH_40_MHz_WIDTH:mib->PhyDSSSTable->Chanflag.ChnlWidth),
#endif
			mib->PhyDSSSTable->Chanflag.ExtChnlOffset);
		mib->PhyDSSSTable->powinited |=2;		
	}
	if((mib->PhyDSSSTable->powinited&1) == 0)
	{
		wlFwGettxpower(netdev, mib->PhyDSSSTable->targetPowers, mib->PhyDSSSTable->CurrChan,
			mib->PhyDSSSTable->Chanflag.FreqBand, 
#ifdef SOC_W8864
			((mib->PhyDSSSTable->Chanflag.ChnlWidth == CH_AUTO_WIDTH)?chAutoWidth:mib->PhyDSSSTable->Chanflag.ChnlWidth),
#else
			((mib->PhyDSSSTable->Chanflag.ChnlWidth == CH_AUTO_WIDTH)?CH_40_MHz_WIDTH:mib->PhyDSSSTable->Chanflag.ChnlWidth),
#endif
			mib->PhyDSSSTable->Chanflag.ExtChnlOffset);
		mib->PhyDSSSTable->powinited |=1;		
	}
	for (i = 0; i < TX_POWER_LEVEL_TOTAL; i++) {
		if(mib->PhyDSSSTable->targetPowers[i] > mib->PhyDSSSTable->maxTxPow[i])
			tmp = mib->PhyDSSSTable->maxTxPow[i];
		else
			tmp = mib->PhyDSSSTable->targetPowers[i];
		maxtxpow[i] = ((tmp-reduceVal)>0)?(tmp-reduceVal):0;
	}
	return wlFwSettxpowers(netdev, maxtxpow,HostCmd_ACT_GEN_SET, mib->PhyDSSSTable->CurrChan,
			mib->PhyDSSSTable->Chanflag.FreqBand, mib->PhyDSSSTable->Chanflag.ChnlWidth,
			mib->PhyDSSSTable->Chanflag.ExtChnlOffset);
#else
	MIB_802DOT11 *mib = vmacSta_p->Mib802dot11;
	HostCmd_DS_SET_REGION_POWER *pCmd =
		(HostCmd_DS_SET_REGION_POWER *) &wlpptr->pCmdBuf[0];
	int retval = FAIL;
	int reduceVal;
	UINT8 *mib_MaxTxPwr_p = mib->mib_MaxTxPwr;

	unsigned long flags; 

	WLDBG_ENTER(DBG_LEVEL_0);

	MWL_SPIN_LOCK(&wlpptr->wlpd_p->locks.fwLock);
	memset(pCmd, 0x00, sizeof(HostCmd_DS_SET_REGION_POWER));
	pCmd->CmdHdr.Cmd     = ENDIAN_SWAP16(HostCmd_CMD_SET_REGION_POWER);
	pCmd->CmdHdr.Length  = ENDIAN_SWAP16(sizeof(HostCmd_DS_SET_REGION_POWER));
#ifdef PWRFRAC
	switch (*(mib->mib_TxPwrFraction))
	{
	case 0:
		reduceVal = 0; /* Max */
		break;	
	case 1:
		reduceVal = 2; /* 75% -1.25db */
		break;	
	case 2:
		reduceVal = 3;  /* 50% -3db */
		break;	
	case 3:
		reduceVal = 6; /* 25% -6db */
		break;	

	default:
		reduceVal = *mib_MaxTxPwr_p;	 /* larger than case 3,  pCmd->MaxPowerLevel is min */
		break;	
	}

	if ((*mib_MaxTxPwr_p-reduceVal) > MINTXPOWER)
		pCmd->MaxPowerLevel = ENDIAN_SWAP16(*mib_MaxTxPwr_p - reduceVal); 
	else
		pCmd->MaxPowerLevel = ENDIAN_SWAP16(MINTXPOWER);
#else	
	pCmd->MaxPowerLevel = ENDIAN_SWAP16(*mib_MaxTxPwr_p);
#endif
	retval = wlexecuteCommand(netdev, HostCmd_CMD_SET_REGION_POWER);
	MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
	return retval;
#endif
}

static int wlFwSetAdaptMode(struct net_device *netdev)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	vmacApInfo_t *vmacSta_p = wlpptr->vmacSta_p;
	MIB_802DOT11 *mib = vmacSta_p->ShadowMib802dot11;
	HostCmd_DS_SET_RATE_ADAPT_MODE *pCmd =
		(HostCmd_DS_SET_RATE_ADAPT_MODE *) &wlpptr->pCmdBuf[0];
	int retval = FAIL;

	unsigned long flags; 

	WLDBG_ENTER(DBG_LEVEL_0);

	MWL_SPIN_LOCK(&wlpptr->wlpd_p->locks.fwLock);
	memset(pCmd, 0x00, sizeof(HostCmd_DS_SET_RATE_ADAPT_MODE));
	pCmd->CmdHdr.Cmd     = ENDIAN_SWAP16(HostCmd_CMD_SET_RATE_ADAPT_MODE);
	pCmd->CmdHdr.Length  = ENDIAN_SWAP16(sizeof(HostCmd_DS_SET_RATE_ADAPT_MODE));
	pCmd->RateAdaptMode = ENDIAN_SWAP16(*(mib->mib_RateAdaptMode));
	pCmd->Action = ENDIAN_SWAP16(HostCmd_ACT_GEN_SET);
	retval = wlexecuteCommand(netdev, HostCmd_CMD_SET_RATE_ADAPT_MODE);
	MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
	return retval;
}
static int wlFwSetCSAdaptMode(struct net_device *netdev)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	vmacApInfo_t *vmacSta_p = wlpptr->vmacSta_p;
	MIB_802DOT11 *mib = vmacSta_p->ShadowMib802dot11;
	MIB_PHY_DSSS_TABLE *PhyDSSSTable=mib->PhyDSSSTable;
	HostCmd_DS_SET_LINKADAPT_CS_MODE *pCmd =
		(HostCmd_DS_SET_LINKADAPT_CS_MODE *) &wlpptr->pCmdBuf[0];
	int retval = FAIL;

	unsigned long flags; 

	WLDBG_ENTER(DBG_LEVEL_0);

	MWL_SPIN_LOCK(&wlpptr->wlpd_p->locks.fwLock);
	memset(pCmd, 0x00, sizeof(HostCmd_DS_SET_LINKADAPT_CS_MODE));
	pCmd->CmdHdr.Cmd     = ENDIAN_SWAP16(HostCmd_CMD_SET_LINKADAPT_CS_MODE);
	pCmd->CmdHdr.Length  = ENDIAN_SWAP16(sizeof(HostCmd_DS_SET_LINKADAPT_CS_MODE));
	if(PhyDSSSTable->Chanflag.FreqBand==FREQ_BAND_5GHZ)
	{
		*(mib->mib_CSMode) = LINKADAPT_CS_ADAPT_STATE_AUTO_ENABLED;
	}
	else
	{
		*(mib->mib_CSMode) = LINKADAPT_CS_ADAPT_STATE_CONSERV;
	}
	pCmd->CSMode = ENDIAN_SWAP16(*(mib->mib_CSMode));
	pCmd->Action = ENDIAN_SWAP16(HostCmd_ACT_GEN_SET);
	retval = wlexecuteCommand(netdev, HostCmd_CMD_SET_LINKADAPT_CS_MODE);
	MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
	return retval;
}
#ifdef WDS_FEATURE
int wlFwSetWdsMode(struct net_device *netdev)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	//	vmacApInfo_t *vmacSta_p = wlpptr->vmacSta_p;
	HostCmd_WDS *pCmd = (HostCmd_WDS *) &wlpptr->pCmdBuf[0];
	int retval = FAIL;

	unsigned long flags; 

	WLDBG_ENTER(DBG_LEVEL_0);

	MWL_SPIN_LOCK(&wlpptr->wlpd_p->locks.fwLock);
	memset(pCmd, 0x00, sizeof(HostCmd_WDS));
	pCmd->CmdHdr.Cmd     = ENDIAN_SWAP16(HostCmd_CMD_WDS_ENABLE);
	pCmd->CmdHdr.Length  = ENDIAN_SWAP16(sizeof(HostCmd_WDS));
	pCmd->WdsEnable = ENDIAN_SWAP32(*(wlpptr->vmacSta_p->Mib802dot11->mib_wdsEnable));
	retval = wlexecuteCommand(netdev, HostCmd_CMD_WDS_ENABLE);
	MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
	return retval;
}
#endif
static int wlFwSetNProt(struct net_device *netdev, UINT32 mode)	  
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	HostCmd_FW_SET_N_PROTECT_FLAG *pCmd =
		(HostCmd_FW_SET_N_PROTECT_FLAG *) &wlpptr->pCmdBuf[0];
	int retval = FAIL;
	unsigned long flags;

	WLDBG_ENTER_INFO(DBG_LEVEL_0, "N prot mode %d", mode);

	MWL_SPIN_LOCK(&wlpptr->wlpd_p->locks.fwLock);
	memset(pCmd, 0x00, sizeof(HostCmd_FW_SET_N_PROTECT_FLAG));
	pCmd->CmdHdr.Cmd    = ENDIAN_SWAP16(HostCmd_CMD_SET_N_PROTECT_FLAG);
	pCmd->CmdHdr.Length = ENDIAN_SWAP16(sizeof(HostCmd_FW_SET_N_PROTECT_FLAG));
	pCmd->NProtectFlag  = ENDIAN_SWAP32(mode);

	WLDBG_DUMP_DATA(DBG_LEVEL_0, (void *) pCmd,
		sizeof(HostCmd_FW_SET_N_PROTECT_FLAG));
	retval = wlexecuteCommand(netdev, HostCmd_CMD_SET_N_PROTECT_FLAG);
	MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
	return retval;
}

int wlFwSetNProtOpMode(struct net_device *netdev, UINT8 mode)	  
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	HostCmd_FW_SET_N_PROTECT_OPMODE *pCmd =
		(HostCmd_FW_SET_N_PROTECT_OPMODE *) &wlpptr->pCmdBuf[0];
	int retval = FAIL;
	unsigned long flags;

	WLDBG_ENTER_INFO(DBG_LEVEL_0, "N prot OP mode %d", mode);
	MWL_SPIN_LOCK(&wlpptr->wlpd_p->locks.fwLock);
	memset(pCmd, 0x00, sizeof(HostCmd_FW_SET_N_PROTECT_OPMODE));
	pCmd->CmdHdr.Cmd    = ENDIAN_SWAP16(HostCmd_CMD_SET_N_PROTECT_OPMODE);
	pCmd->CmdHdr.Length = ENDIAN_SWAP16(sizeof(HostCmd_FW_SET_N_PROTECT_OPMODE));
	pCmd->NProtectOpMode  = mode;

	WLDBG_DUMP_DATA(DBG_LEVEL_0, (void *) pCmd,
		sizeof(HostCmd_FW_SET_N_PROTECT_OPMODE));
	retval = wlexecuteCommand(netdev, HostCmd_CMD_SET_N_PROTECT_OPMODE);
	MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
	return retval;
}

static int wlFwSetOptimizationLevel(struct net_device *netdev, UINT8 mode)	  
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	HostCmd_FW_SET_OPTIMIZATION_LEVEL *pCmd =
		(HostCmd_FW_SET_OPTIMIZATION_LEVEL *) &wlpptr->pCmdBuf[0];
	int retval = FAIL;
	unsigned long flags;

	WLDBG_ENTER_INFO(DBG_LEVEL_0, "Optimization %d", mode);

	MWL_SPIN_LOCK(&wlpptr->wlpd_p->locks.fwLock);
	memset(pCmd, 0x00, sizeof(HostCmd_FW_SET_OPTIMIZATION_LEVEL));
	pCmd->CmdHdr.Cmd    = ENDIAN_SWAP16(HostCmd_CMD_SET_OPTIMIZATION_LEVEL);
	pCmd->CmdHdr.Length = ENDIAN_SWAP16(sizeof(HostCmd_FW_SET_OPTIMIZATION_LEVEL));
	pCmd->OptLevel  = mode;

	WLDBG_DUMP_DATA(DBG_LEVEL_0, (void *) pCmd,
		sizeof(HostCmd_FW_SET_OPTIMIZATION_LEVEL));
	retval = wlexecuteCommand(netdev, HostCmd_CMD_SET_OPTIMIZATION_LEVEL);
	MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
	return retval;
}
int wlFwGetCalTable(struct net_device *netdev, UINT8 annex, UINT8 index)	  
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);

	HostCmd_FW_GET_CALTABLE *pCmd =
		(HostCmd_FW_GET_CALTABLE *) &wlpptr->pCmdBuf[0];
	int retval = FAIL;
	unsigned long flags;

	//WLDBG_ENTER_INFO(DBG_LEVEL_1, "Optimization %d", mode);

	MWL_SPIN_LOCK(&wlpptr->wlpd_p->locks.fwLock);
	memset(pCmd, 0x00, sizeof(HostCmd_FW_GET_CALTABLE));
	pCmd->CmdHdr.Cmd    = ENDIAN_SWAP16(HostCmd_CMD_GET_CALTABLE);
	pCmd->CmdHdr.Length = ENDIAN_SWAP16(sizeof(HostCmd_FW_GET_CALTABLE));
	pCmd->annex = annex;
	pCmd->index = index;

	memset(&wlpptr->calTbl, 0x00, CAL_TBL_SIZE);

	WLDBG_DUMP_DATA(DBG_LEVEL_1, (void *) pCmd,
		sizeof(HostCmd_FW_GET_CALTABLE));
	retval = wlexecuteCommand(netdev, HostCmd_CMD_GET_CALTABLE);
	if(!retval)
		memcpy(&wlpptr->calTbl, &pCmd->calTbl, CAL_TBL_SIZE);

	if ((wlpptr->calTbl[0] != annex) && (annex != 0) && (annex != 255))
		retval = FAIL;

	MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
	return retval;
}							  

int wlFwSetMimoPsHt(struct net_device *netdev, UINT8 *addr, UINT8 enable, UINT8 mode)	  
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);

	HostCmd_FW_SET_MIMOPSHT *pCmd =
		(HostCmd_FW_SET_MIMOPSHT *) &wlpptr->pCmdBuf[0];
	int retval = FAIL;
	unsigned long flags;

	MWL_SPIN_LOCK(&wlpptr->wlpd_p->locks.fwLock);
	memset(pCmd, 0x00, sizeof(HostCmd_FW_SET_MIMOPSHT));
	pCmd->CmdHdr.Cmd    = ENDIAN_SWAP16(HostCmd_CMD_SET_MIMOPSHT);
	pCmd->CmdHdr.Length = ENDIAN_SWAP16(sizeof(HostCmd_FW_SET_MIMOPSHT));
	memcpy(pCmd->Addr, addr, 6);
	pCmd->Enable = enable;
	pCmd->Mode = mode;

	WLDBG_DUMP_DATA(DBG_LEVEL_0, (void *) pCmd,
		sizeof(HostCmd_FW_SET_MIMOPSHT));
	retval = wlexecuteCommand(netdev, HostCmd_CMD_SET_MIMOPSHT);

	MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
	return retval;
}
int wlFwSetSecurity(struct net_device *netdev,  u_int8_t *staaddr)
{	
	int retval = SUCCESS;
	if (wlFwSetWep(netdev,staaddr))
	{
		printk( "setting wep keyto sta fail\n");
		retval = FAIL;
	}
	if(wlFwSetWpaTkipMode(netdev,staaddr))
	{
		printk( "setting tkip sta fail\n");
		retval = FAIL;
	}
	if(wlFwSetWpaAesMode(netdev,staaddr))
	{
		printk( "setting aes fail\n");
		retval = FAIL;
	}	
	if (wlFwSetMixedWpaWpa2Mode(netdev,staaddr))
	{
		printk( "setting mixed mode sta fail\n");
		retval = FAIL;
	}
	return retval;
}
static int wlFwGetPwrCalTable(struct net_device *netdev)	  
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	vmacApInfo_t *syscfg = (vmacApInfo_t *)wlpptr->vmacSta_p;
	MIB_802DOT11 *mib = syscfg->Mib802dot11;
	MIB_802DOT11 *mib1 = syscfg->ShadowMib802dot11;
	UINT16 len;

	if (syscfg->txPwrTblLoaded)
	{
		return SUCCESS;
	}
	syscfg->txPwrTblLoaded = 1;

	if (wlFwGetCalTable(netdev, 33, 0) != FAIL)
	{
		len = wlpptr->calTbl[2] | (wlpptr->calTbl[3] << 8);
		len -= 12;
		if (len > PWTAGETRATETABLE20M)
			len = PWTAGETRATETABLE20M;
		memcpy(mib->PowerTagetRateTable20M, &wlpptr->calTbl[12], len);	
		memcpy(mib1->PowerTagetRateTable20M, &wlpptr->calTbl[12], len);		
	}

	if (wlFwGetCalTable(netdev, 34, 0) != FAIL)
	{
		len = wlpptr->calTbl[2] | (wlpptr->calTbl[3] << 8);
		len -= 12;
		if (len > PWTAGETRATETABLE40M)
			len = PWTAGETRATETABLE40M;
		memcpy(mib->PowerTagetRateTable40M, &wlpptr->calTbl[12], len);	
		memcpy(mib1->PowerTagetRateTable40M, &wlpptr->calTbl[12], len);	
	}

	if (wlFwGetCalTable(netdev, 35, 0) != FAIL)
	{
		len = wlpptr->calTbl[2] | (wlpptr->calTbl[3] << 8);
		len -= 20;
		if (len > PWTAGETRATETABLE20M_5G)
			len = PWTAGETRATETABLE20M_5G;
		memcpy(mib->PowerTagetRateTable20M_5G, &wlpptr->calTbl[20], len);
		memcpy(mib1->PowerTagetRateTable20M_5G, &wlpptr->calTbl[20], len);	
	}

	if (wlFwGetCalTable(netdev, 36, 0) != FAIL)
	{
		len = wlpptr->calTbl[2] | (wlpptr->calTbl[3] << 8);
		len -= 20;
		if (len > PWTAGETRATETABLE40M_5G)
			len = PWTAGETRATETABLE40M_5G;
		memcpy(mib->PowerTagetRateTable40M_5G, &wlpptr->calTbl[20], len);
		memcpy(mib1->PowerTagetRateTable40M_5G, &wlpptr->calTbl[20], len);	

	}
	return SUCCESS;
}

static int wlFwGetRegionCode(struct net_device *netdev)	  
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	vmacApInfo_t *syscfg = (vmacApInfo_t *)wlpptr->vmacSta_p;
	MIB_802DOT11 *mib = syscfg->Mib802dot11;
	MIB_802DOT11 *mib1 = syscfg->ShadowMib802dot11;
	MIB_SPECTRUM_MGMT	*mib_SpectrumMagament_p=mib->SpectrumMagament;
	MIB_SPECTRUM_MGMT	*mib_SpectrumMagament1_p=mib1->SpectrumMagament;

	if (syscfg->regionCodeLoaded)
	{
		return SUCCESS;
	}
	syscfg->regionCodeLoaded = 1;

	if (wlFwGetCalTable(netdev, 0, 0) != FAIL)
	{
		/* if this line is not added, the user configured region code will be overwritten by regioncode read from fw */
		if (!bcn_reg_domain)
		{
		mib_SpectrumMagament_p->countryCode = wlpptr->calTbl[16];
		mib_SpectrumMagament1_p->countryCode = wlpptr->calTbl[16];
		domainSetDomain(mib_SpectrumMagament1_p->countryCode);
		}
	}
	//printk("code 0x%x\n", mib_SpectrumMagament_p->countryCode ); 
	return SUCCESS;
}

int wlFwGetBeacon(struct net_device *netdev, UINT8 *pBcn, UINT16 *pLen)	  
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	HostCmd_FW_GET_BEACON *pCmd =
		(HostCmd_FW_GET_BEACON *) &wlpptr->pCmdBuf[0];
	int retval = FAIL;
	unsigned long flags;

	//WLDBG_ENTER_INFO(DBG_LEVEL_1, "Optimization %d", mode);

	MWL_SPIN_LOCK(&wlpptr->wlpd_p->locks.fwLock);
	memset(pCmd, 0x00, sizeof(HostCmd_FW_GET_BEACON));
	pCmd->CmdHdr.Cmd    = ENDIAN_SWAP16(HostCmd_CMD_GET_BEACON);
	pCmd->CmdHdr.Length = ENDIAN_SWAP16(sizeof(HostCmd_FW_GET_BEACON));
	pCmd->Bcnlen = 0;

	WLDBG_DUMP_DATA(DBG_LEVEL_1, (void *) pCmd,
		sizeof(HostCmd_FW_GET_BEACON));
	retval = wlexecuteCommand(netdev, HostCmd_CMD_GET_BEACON);
	if(!retval)
	{
		memcpy(pBcn, &pCmd->Bcn, pCmd->Bcnlen);
		*pLen = pCmd->Bcnlen;
	}
	MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
	return retval;
}
int wlFwSetRifs( struct net_device *netdev, UINT8 QNum)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	HostCmd_FW_SET_RIFS  *pCmd = (HostCmd_FW_SET_RIFS *) &wlpptr->pCmdBuf[0];
	int retval = FAIL;
	unsigned long flags;

#ifdef MFG_SUPPORT
	if (wlpptr->mfgEnable)
	{		
		return SUCCESS;
	}
#endif
	MWL_SPIN_LOCK(&wlpptr->wlpd_p->locks.fwLock);
	memset(pCmd, 0x00, sizeof(HostCmd_CMD_SET_RIFS));
	pCmd->CmdHdr.Cmd    = ENDIAN_SWAP16(HostCmd_CMD_SET_RIFS);
	pCmd->CmdHdr.Length = ENDIAN_SWAP16(sizeof(HostCmd_FW_SET_RIFS));
	pCmd->QNum = QNum;

	retval = wlexecuteCommand(netdev, HostCmd_CMD_SET_RIFS);
	MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
	return retval;
}

int wlFwSetHTGF( struct net_device *netdev, UINT32 mode)	    
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	HostCmd_FW_HT_GF_MODE  *pCmd = (HostCmd_FW_HT_GF_MODE *) &wlpptr->pCmdBuf[0];
	int retval = FAIL;
	unsigned long flags;

#ifdef MFG_SUPPORT
	if (wlpptr->mfgEnable)
	{		
		return SUCCESS;
	}
#endif
	MWL_SPIN_LOCK(&wlpptr->wlpd_p->locks.fwLock);
	memset(pCmd, 0x00, sizeof(HostCmd_FW_HT_GF_MODE));
	pCmd->CmdHdr.Cmd    = ENDIAN_SWAP16(HostCmd_CMD_HT_GF_MODE);
	pCmd->CmdHdr.Length = ENDIAN_SWAP16(sizeof(HostCmd_FW_HT_GF_MODE));
	pCmd->Action       =ENDIAN_SWAP32(WL_SET);
	pCmd->Mode = ENDIAN_SWAP32(mode);

	retval = wlexecuteCommand(netdev, HostCmd_CMD_HT_GF_MODE);
	MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
	return retval;
}

int wlFwSetHTStbc( struct net_device *netdev, UINT32 mode)
{
	int retval = FAIL;
#if defined(SOC_W8366) || defined(SOC_W8764)
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	HostCmd_FW_HT_STBC_MODE  *pCmd = (HostCmd_FW_HT_STBC_MODE *) &wlpptr->pCmdBuf[0];
	unsigned long flags;

#ifdef MFG_SUPPORT
	if (wlpptr->mfgEnable)
	{		
		return SUCCESS;
	}
#endif
	MWL_SPIN_LOCK(&wlpptr->wlpd_p->locks.fwLock);
	memset(pCmd, 0x00, sizeof(HostCmd_FW_HT_STBC_MODE));
	pCmd->CmdHdr.Cmd    = ENDIAN_SWAP16(HostCmd_CMD_HT_TX_STBC);
	pCmd->CmdHdr.Length = ENDIAN_SWAP16(sizeof(HostCmd_FW_HT_STBC_MODE));
	pCmd->Action       =ENDIAN_SWAP32(WL_SET);
	pCmd->Mode = ENDIAN_SWAP32(mode);
	retval = wlexecuteCommand(netdev, HostCmd_CMD_HT_TX_STBC);
	MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
#endif
	return retval;
}

	  
int wlFwGetRateTable(struct net_device *netdev, UINT8 *addr, UINT8 *pRateInfo, UINT32 size)	
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);

	HostCmd_FW_GET_RATETABLE *pCmd =
		(HostCmd_FW_GET_RATETABLE *) &wlpptr->pCmdBuf[0];
	int retval = FAIL;
	unsigned long flags;

	MWL_SPIN_LOCK(&wlpptr->wlpd_p->locks.fwLock);
	memset(pCmd, 0x00, sizeof(HostCmd_FW_GET_RATETABLE));
	pCmd->CmdHdr.Cmd    = ENDIAN_SWAP16(HostCmd_CMD_GET_RATETABLE);
	pCmd->CmdHdr.Length = ENDIAN_SWAP16(sizeof(HostCmd_FW_GET_RATETABLE));
	memcpy(pCmd->Addr, addr, 6);

	memset(pRateInfo, 0x00, size);					

	WLDBG_DUMP_DATA(DBG_LEVEL_1, (void *) pCmd,
		sizeof(HostCmd_FW_GET_RATETABLE));
	retval = wlexecuteCommand(netdev, HostCmd_CMD_GET_RATETABLE);

	if(!retval)
		memcpy(pRateInfo, &pCmd->SortedRatesIndexMap, size);
		

	MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
	return retval;
}
#ifdef MFG_SUPPORT
typedef PACK_START struct mfg_CmdRfReg_t
{
	UINT32 mfgCmd;
	UINT32 Action;
	UINT32 Error;
	UINT32 Address;
	UINT32 Data;
	UINT32 deviceId; 
} PACK_END mfg_CmdRfReg_t;

int wlFwMfgCmdIssue(struct net_device *netdev, char *pData,  char *pDataOut)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	FWCmdHdr *pCmd = (FWCmdHdr *) &wlpptr->pCmdBuf[0];
	int retval = FAIL;
	UINT8 *p = pData + 2;
	UINT16 CmdLen = *(UINT16 *)p; 

	unsigned long flags; 

	WLDBG_ENTER(DBG_LEVEL_1);

	MWL_SPIN_LOCK(&wlpptr->wlpd_p->locks.fwLock);
	memset(pCmd, 0x00, CmdLen);

	memcpy((void *)pCmd, (void *)pData, CmdLen);
	retval = wlexecuteCommand(netdev, HostCmd_CMD_MFG_COMMAND);

	if(!retval)
	{
		memcpy((void *)pDataOut, (void *)pCmd, CmdLen);
	}

	MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);

	return retval;
}
#endif

#ifdef MRVL_DFS
#define DFS_DISABLED 	0
#define DFS_ENABLED 	1
int DecideDFSOperation(struct net_device *netdev, BOOLEAN bChannelChanged, 
					   UINT8 currDFSState, 
					   UINT8 newDFSState ,
					   MIB_802DOT11 *mib)
{
	UINT8 noChannelChangeCheck = 0 ;
	smeQ_MgmtMsg_t *toSmeMsg = NULL;
	UINT32		action = 0 ;
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	vmacApInfo_t *vmacSta_p=(vmacApInfo_t *)wlpptr->vmacSta_p;
#if 1 //def COMMON_PHYDSSS
	MIB_PHY_DSSS_TABLE *PhyDSSSTable=mib->PhyDSSSTable;
#else
	MIB_PHY_DSSS_TABLE *PhyDSSSTable=&mib->PhyDSSSTable;
#endif
	/* First check if DFS is instantiated or not */
	if(  wlpptr->wlpd_p->pdfsApMain )
	{
		if( currDFSState == DFS_DISABLED )
		{
			/* This scenario cannot occur. */
			DfsDeInit(wlpptr->wlpd_p);
			return SUCCESS;
		}
		else
		{ /*currDFSState = 1 */
			if( newDFSState == DFS_DISABLED )
			{
				DfsDeInit(wlpptr->wlpd_p);
				return SUCCESS;
			}
		}
	}
	else
	{
		if( currDFSState == DFS_DISABLED )
		{
			if( newDFSState == DFS_ENABLED )
			{
				DfsInit(netdev, wlpptr->wlpd_p );
				noChannelChangeCheck = 1 ;
			}
			else
			{
				/* Do not enter into DFS SM */
				return SUCCESS;
			}
		}
		else
		{
			if( newDFSState == DFS_ENABLED )
			{
				/* This can happen when the AP boots up first time and
				driver deafult is DFS Enabled 
				*/
				DfsInit(netdev, wlpptr->wlpd_p);
				noChannelChangeCheck = 1 ;
			}
			else
			{
				/* Do not enter into DFS SM */
				return SUCCESS;
			}
		}
	}
	if( wlpptr->wlpd_p->pdfsApMain == NULL )
	{
		WLDBG_INFO(DBG_LEVEL_0, "DecideDFSOperation: failed to alloc DFS buffer\n");
		return FAIL ;
	}
	if( *(mib->mib_NOPTimeOut) != 0 )
	{
		DfsSetNOCTimeOut(wlpptr->wlpd_p->pdfsApMain, *(mib->mib_NOPTimeOut) );
	}
	if( *(mib->mib_CACTimeOut) != 0 )
	{
		DfsSetCACTimeOut(wlpptr->wlpd_p->pdfsApMain, *(mib->mib_CACTimeOut) );
	}
	if( bChannelChanged || noChannelChangeCheck)
	{
		/* Send Channel Change Event to DFS SM */
		if ((toSmeMsg=(smeQ_MgmtMsg_t *)kmalloc(sizeof(smeQ_MgmtMsg_t), GFP_ATOMIC)) == NULL)
		{
			WLDBG_INFO(DBG_LEVEL_0, "DecideDFSOperation: failed to alloc msg buffer\n");
			return FAIL;
		}

		memset(toSmeMsg, 0, sizeof(smeQ_MgmtMsg_t));

		toSmeMsg->MsgType = SME_NOTIFY_CHANNELSWITCH_CFRM;

		toSmeMsg->Msg.ChanSwitchCfrm.result = 1 ;
		toSmeMsg->Msg.ChanSwitchCfrm.chInfo.channel = PhyDSSSTable->CurrChan ;
		memcpy(&toSmeMsg->Msg.ChanSwitchCfrm.chInfo.chanflag,
			&PhyDSSSTable->Chanflag, sizeof(CHNL_FLAGS));
		toSmeMsg->vmacSta_p = vmacSta_p;

		smeQ_MgmtWriteNoBlock(toSmeMsg);
		kfree((UINT8 *)toSmeMsg);
	}
	else
	{
		action = DFSGetCurrentRadarDetectionMode(wlpptr->wlpd_p->pdfsApMain, PhyDSSSTable->CurrChan, 
			PhyDSSSTable->Chanflag);
		if( action && wlFwSetRadarDetection(netdev, action))
		{
			WLDBG_WARNING(DBG_LEVEL_1, "enabling AP radar detection");
			return FAIL ;
		}
	}
	return SUCCESS;
}
#endif
#ifdef WMON
UINT32 Rx_Traffic_FCS_Cnt(struct net_device *dev)
{
	return PciReadMacReg(dev,RX_TRAFFIC_ERR_CNT);
}
#endif
#ifdef RXPATHOPT
int wlFwSetRxPathOpt(struct net_device *netdev, UINT32 rxPathOpt)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	HostCmd_SET_RXPATHOPT *pCmd =	(HostCmd_SET_RXPATHOPT *) &wlpptr->pCmdBuf[0];
	int retval = FAIL;
	unsigned long flags;

	WLDBG_ENTER_INFO(DBG_LEVEL_0,	"Set RXPATHOPT to %d", rxPathOpt );

	MWL_SPIN_LOCK(&wlpptr->wlpd_p->locks.fwLock);
	memset(pCmd, 0x00, sizeof(HostCmd_SET_RXPATHOPT));
	pCmd->CmdHdr.Cmd = ENDIAN_SWAP16(HostCmd_CMD_SET_RXPATHOPT);
	pCmd->CmdHdr.Length = ENDIAN_SWAP16(sizeof(HostCmd_SET_RXPATHOPT));
	pCmd->RxPathOpt= ENDIAN_SWAP32(rxPathOpt);
	pCmd->RxPktThreshold = ENDIAN_SWAP32(0); //use fw default.
	pCmd->CmdHdr.macid = wlpptr->vmacSta_p->VMacEntry.macId;

	WLDBG_DUMP_DATA(DBG_LEVEL_0, (void *) pCmd,sizeof(HostCmd_SET_RXPATHOPT));

	retval = wlexecuteCommand(netdev, HostCmd_CMD_SET_RXPATHOPT);
	MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
	return retval;
}
#endif
#ifdef V6FW
int wlFwSetDwdsStaMode(struct net_device *netdev, UINT32 enable)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	HostCmd_DWDS_ENABLE *pCmd =	(HostCmd_DWDS_ENABLE *) &wlpptr->pCmdBuf[0];
	int retval = FAIL;
	unsigned long flags;

	WLDBG_ENTER_INFO(DBG_LEVEL_0,	"Set DwdsStaMode to %d", enable );

	MWL_SPIN_LOCK(&wlpptr->wlpd_p->locks.fwLock);
	memset(pCmd, 0x00, sizeof(HostCmd_DWDS_ENABLE));
	pCmd->CmdHdr.Cmd = ENDIAN_SWAP16(HostCmd_CMD_DWDS_ENABLE);
	pCmd->CmdHdr.Length = ENDIAN_SWAP16(sizeof(HostCmd_DWDS_ENABLE));
	pCmd->Enable= ENDIAN_SWAP32(enable);
	pCmd->CmdHdr.macid = wlpptr->vmacSta_p->VMacEntry.macId;

	WLDBG_DUMP_DATA(DBG_LEVEL_0, (void *) pCmd,sizeof(HostCmd_DWDS_ENABLE));

	retval = wlexecuteCommand(netdev, HostCmd_CMD_DWDS_ENABLE);
	MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
	return retval;
}
#endif

int wlFwSetFwFlushTimer(struct net_device *netdev, UINT32 usecs)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	HostCmd_FW_FLUSH_TIMER *pCmd =	(HostCmd_FW_FLUSH_TIMER *) &wlpptr->pCmdBuf[0];
	int retval = FAIL;
	unsigned long flags;

	WLDBG_ENTER_INFO(DBG_LEVEL_0,	"Set FwFlushTimer to %d usecs", usecs );

	MWL_SPIN_LOCK(&wlpptr->wlpd_p->locks.fwLock);
	memset(pCmd, 0x00, sizeof(HostCmd_FW_FLUSH_TIMER));
	pCmd->CmdHdr.Cmd = ENDIAN_SWAP16(HostCmd_CMD_FW_FLUSH_TIMER);
	pCmd->CmdHdr.Length = ENDIAN_SWAP16(sizeof(HostCmd_FW_FLUSH_TIMER));
	pCmd->value= ENDIAN_SWAP32(usecs);
	pCmd->CmdHdr.macid = wlpptr->vmacSta_p->VMacEntry.macId;

	WLDBG_DUMP_DATA(DBG_LEVEL_0, (void *) pCmd,sizeof(HostCmd_FW_FLUSH_TIMER));
	retval = wlexecuteCommand(netdev, HostCmd_CMD_FW_FLUSH_TIMER);
	MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
	return retval;
}
#ifdef COEXIST_20_40_SUPPORT
int wlFwSet11N_20_40_Switch(struct net_device *netdev, UINT8 mode)	  
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	HostCmd_FW_SET_11N_20_40_SWITCHING *pCmd =
		(HostCmd_FW_SET_11N_20_40_SWITCHING *) &wlpptr->pCmdBuf[0];
	int retval = FAIL;
	unsigned long flags;


	WLDBG_ENTER_INFO(DBG_LEVEL_0, "20/40 switching %d", mode);

	MWL_SPIN_LOCK(&wlpptr->wlpd_p->locks.fwLock);
	memset(pCmd, 0x00, sizeof(HostCmd_FW_SET_11N_20_40_SWITCHING));
	pCmd->CmdHdr.Cmd    = ENDIAN_SWAP16(HostCmd_CMD_SET_11N_20_40_CHANNEL_SWITCH);
	pCmd->CmdHdr.Length = ENDIAN_SWAP16(sizeof(HostCmd_FW_SET_11N_20_40_SWITCHING));
	pCmd->AddChannel  = mode;

	WLDBG_DUMP_DATA(DBG_LEVEL_0, (void *) pCmd,
		sizeof(HostCmd_FW_SET_11N_20_40_SWITCHING));
	retval = wlexecuteCommand(netdev, HostCmd_CMD_SET_11N_20_40_CHANNEL_SWITCH);
	MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
	return retval;
}



#endif

#ifdef EXPLICIT_BF

int wlFwSet11N_BF_Mode(struct net_device *netdev, UINT8 bf_option, UINT8 bf_csi_steering, UINT8 bf_mcsfeedback, UINT8 bf_mode, UINT8 bf_interval, 
					   UINT8 bf_slp, UINT8 bf_power)	  
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	HostCmd_FW_HT_BF_MODE *pCmd = (HostCmd_FW_HT_BF_MODE *)&wlpptr->pCmdBuf[0];
	int retval = FAIL;
	unsigned long flags;


	WLDBG_ENTER_INFO(DBG_LEVEL_0, "Set 11n BF mode %d %d %d %d",bf_option,bf_csi_steering,bf_mcsfeedback,bf_mode);

	MWL_SPIN_LOCK(&wlpptr->wlpd_p->locks.fwLock);
	memset(pCmd, 0x00, sizeof(HostCmd_FW_HT_BF_MODE));
	pCmd->CmdHdr.Cmd    = ENDIAN_SWAP16(HostCmd_CMD_SET_BF);
	pCmd->CmdHdr.Length = ENDIAN_SWAP16(sizeof(HostCmd_FW_HT_BF_MODE));
	pCmd->option = bf_option;
	pCmd->csi_steering=bf_csi_steering;
	pCmd->mcsfeedback=bf_mcsfeedback;
	pCmd->mode=bf_mode;
	pCmd->interval=bf_interval;
	pCmd->slp=bf_slp;
	pCmd->power=bf_power;

	pCmd->CmdHdr.macid = wlpptr->vmacSta_p->VMacEntry.macId;




	WLDBG_DUMP_DATA(DBG_LEVEL_0, (void *) pCmd,
		sizeof(HostCmd_FW_HT_BF_MODE));

	retval = wlexecuteCommand(netdev,HostCmd_CMD_SET_BF);
	MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
	return retval;
}

int wlFwSetNoAck( struct net_device *netdev, UINT8 Enable)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	HostCmd_FW_SET_NOACK  *pCmd = (HostCmd_FW_SET_NOACK *) &wlpptr->pCmdBuf[0];
	int retval = FAIL;
	unsigned long flags;

#ifdef MFG_SUPPORT
	if (wlpptr->mfgEnable)
	{		
		return SUCCESS;
	}
#endif
	MWL_SPIN_LOCK(&wlpptr->wlpd_p->locks.fwLock);
	memset(pCmd, 0x00, sizeof(HostCmd_FW_SET_NOACK));
	pCmd->CmdHdr.Cmd    = ENDIAN_SWAP16(HostCmd_CMD_SET_NOACK);
	pCmd->CmdHdr.Length = ENDIAN_SWAP16(sizeof(HostCmd_FW_SET_NOACK));
	pCmd->Enable = Enable;

	retval = wlexecuteCommand(netdev, HostCmd_CMD_SET_NOACK);
	MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
	return retval;
}

#ifdef SOC_W8864
int wlFwSetRCcal( struct net_device *netdev)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	HostCmd_FW_RC_CAL  *pCmd = (HostCmd_FW_RC_CAL *) &wlpptr->pCmdBuf[0];
	int retval = FAIL;
	unsigned long flags;

	MWL_SPIN_LOCK(&wlpptr->wlpd_p->locks.fwLock);
	memset(pCmd, 0x00, sizeof(HostCmd_FW_RC_CAL));
	pCmd->CmdHdr.Cmd    = ENDIAN_SWAP16(HostCmd_CMD_RC_CAL);
	pCmd->CmdHdr.Length = ENDIAN_SWAP16(sizeof(HostCmd_FW_RC_CAL));

	retval = wlexecuteCommand(netdev, HostCmd_CMD_RC_CAL);
	MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
    printk("RC Calibration \n");
    printk("    path A I = 0x%x Q = 0x%x \n", (unsigned int) pCmd->rc_cal[0][0], (unsigned int) pCmd->rc_cal[0][1]);
    printk("    path B I = 0x%x Q = 0x%x \n", (unsigned int) pCmd->rc_cal[1][0], (unsigned int) pCmd->rc_cal[1][1]);
    printk("    path C I = 0x%x Q = 0x%x \n", (unsigned int) pCmd->rc_cal[2][0], (unsigned int) pCmd->rc_cal[2][1]);
    printk("    path D I = 0x%x Q = 0x%x \n", (unsigned int) pCmd->rc_cal[3][0], (unsigned int) pCmd->rc_cal[3][1]);
	return retval;
}

int wlFwGetTemp( struct net_device *netdev)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	HostCmd_FW_TEMP  *pCmd = (HostCmd_FW_TEMP *) &wlpptr->pCmdBuf[0];
	int retval = FAIL;
	unsigned long flags;

	MWL_SPIN_LOCK(&wlpptr->wlpd_p->locks.fwLock);
	memset(pCmd, 0x00, sizeof(HostCmd_FW_TEMP));
	pCmd->CmdHdr.Cmd    = ENDIAN_SWAP16(HostCmd_CMD_GET_TEMP);
	pCmd->CmdHdr.Length = ENDIAN_SWAP16(sizeof(HostCmd_FW_TEMP));

	retval = wlexecuteCommand(netdev, HostCmd_CMD_GET_TEMP);
	MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
    printk("Temperature = 0x%x \n", (u_int32_t)pCmd->temp);
	return retval;
}
#endif

int wlFwSetNoSteer( struct net_device *netdev, UINT8 Enable)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	HostCmd_FW_SET_NOSTEER  *pCmd = (HostCmd_FW_SET_NOSTEER *) &wlpptr->pCmdBuf[0];
	int retval = FAIL;
	unsigned long flags;

#ifdef MFG_SUPPORT
	if (wlpptr->mfgEnable)
	{		
		return SUCCESS;
	}
#endif
	MWL_SPIN_LOCK(&wlpptr->wlpd_p->locks.fwLock);
	memset(pCmd, 0x00, sizeof(HostCmd_FW_SET_NOSTEER));
	pCmd->CmdHdr.Cmd    = ENDIAN_SWAP16(HostCmd_CMD_SET_NOSTEER);
	pCmd->CmdHdr.Length = ENDIAN_SWAP16(sizeof(HostCmd_FW_SET_NOSTEER));
	pCmd->Enable = Enable;

	retval = wlexecuteCommand(netdev, HostCmd_CMD_SET_NOSTEER);
	MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
	return retval;
}

int wlFwSetCDD( struct net_device *netdev, UINT32 cdd_mode)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	HostCmd_FW_SET_CDD  *pCmd = ( HostCmd_FW_SET_CDD *) &wlpptr->pCmdBuf[0];
	int retval = FAIL;
	unsigned long flags;

#ifdef MFG_SUPPORT
	if (wlpptr->mfgEnable)
	{		
		return SUCCESS;
	}
#endif
	MWL_SPIN_LOCK(&wlpptr->wlpd_p->locks.fwLock);
	memset(pCmd, 0x00, sizeof(HostCmd_FW_SET_CDD));
	pCmd->CmdHdr.Cmd    = ENDIAN_SWAP16( HostCmd_CMD_SET_CDD);
	pCmd->CmdHdr.Length = ENDIAN_SWAP16(sizeof(HostCmd_FW_SET_CDD));
	pCmd->Enable = cdd_mode;

	retval = wlexecuteCommand(netdev, HostCmd_CMD_SET_CDD);
	MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
	return retval;
}

int wlFwSetTxHOP( struct net_device *netdev, UINT8 Enable, UINT8 TxHopStatus)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	HostCmd_FW_SET_TXHOP  *pCmd = (HostCmd_FW_SET_TXHOP *) &wlpptr->pCmdBuf[0];
	int retval = FAIL;
	unsigned long flags;

#ifdef MFG_SUPPORT
	if (wlpptr->mfgEnable)
	{		
		return SUCCESS;
	}
#endif
	MWL_SPIN_LOCK(&wlpptr->wlpd_p->locks.fwLock);
	memset(pCmd, 0x00, sizeof(HostCmd_FW_SET_TXHOP));
	pCmd->CmdHdr.Cmd    = ENDIAN_SWAP16(HostCmd_CMD_SET_TXHOP);
	pCmd->CmdHdr.Length = ENDIAN_SWAP16(sizeof(HostCmd_FW_SET_TXHOP));
	pCmd->Enable = Enable;
	pCmd->Txhopstatus= TxHopStatus;

	retval = wlexecuteCommand(netdev, HostCmd_CMD_SET_TXHOP);
	MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
	return retval;
}
int wlFwSetBFType( struct net_device *netdev, UINT32 mode)
{
	int retval = FAIL;
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	HostCmd_FW_HT_BF_TYPE  *pCmd = (HostCmd_FW_HT_BF_TYPE *) &wlpptr->pCmdBuf[0];
	unsigned long flags;

#ifdef MFG_SUPPORT
	if (wlpptr->mfgEnable)
	{		
		return SUCCESS;
	}
#endif
	MWL_SPIN_LOCK(&wlpptr->wlpd_p->locks.fwLock);
	memset(pCmd, 0x00, sizeof(HostCmd_FW_HT_BF_TYPE));
	pCmd->CmdHdr.Cmd    = ENDIAN_SWAP16(HostCmd_CMD_SET_BFTYPE);
	pCmd->CmdHdr.Length = ENDIAN_SWAP16(sizeof(HostCmd_FW_HT_BF_TYPE));
	pCmd->Action       =ENDIAN_SWAP32(WL_SET);
	pCmd->Mode = ENDIAN_SWAP32(mode);
	retval = wlexecuteCommand(netdev, HostCmd_CMD_SET_BFTYPE);
	MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
	return retval;
}

#endif
#ifdef SSU_SUPPORT
int wlFwSetSpectralAnalysis( struct net_device *netdev, UINT32 number_of_buffers, UINT32 buffer_size, UINT32 buffer_base_addr,
                             UINT8 fft_length, UINT8 fft_skip, UINT8 adc_dec, UINT32 time)
{
	int retval = FAIL;
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	vmacApInfo_t *vmacSta_p = wlpptr->vmacSta_p;
	MIB_802DOT11 *mib = vmacSta_p->Mib802dot11;
    UINT16 fft_len, adc_len;
	HostCmd_FW_SET_SPECTRAL_ANALYSIS_TYPE  *pCmd = (HostCmd_FW_SET_SPECTRAL_ANALYSIS_TYPE *) &wlpptr->pCmdBuf[0];
    unsigned long flags;

	MWL_SPIN_LOCK(&wlpptr->wlpd_p->locks.fwLock);
	memset(pCmd, 0x00, sizeof(HostCmd_FW_SET_SPECTRAL_ANALYSIS_TYPE));
	pCmd->CmdHdr.Cmd    = ENDIAN_SWAP16(HostCmd_CMD_SET_SPECTRAL_ANALYSIS);
	pCmd->CmdHdr.Length = ENDIAN_SWAP16(sizeof(HostCmd_FW_SET_SPECTRAL_ANALYSIS_TYPE));
	pCmd->Action        = ENDIAN_SWAP32(WL_SET);
    pCmd->NumOfBuffers      = number_of_buffers;
    pCmd->BufferSize        = buffer_size;
    pCmd->BufferBaseAddress = buffer_base_addr;
    pCmd->FFT_length        = fft_length & 0x3;
    pCmd->FFT_skip          = fft_skip   & 0x3;
    pCmd->ADC_dec           = adc_dec    & 0x3;
    pCmd->Time              = time; 
    pCmd->Notify            = 0x1;
	retval = wlexecuteCommand(netdev, HostCmd_CMD_SET_SPECTRAL_ANALYSIS);
	MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
	return retval;
}
#endif
#ifdef QUEUE_STATS
#define IS_HW_BA    0
#define IS_SW_BA    1
#define NONE_BA     2

int wlFwGetQueueStats( struct net_device *netdev, int option)
{
    struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
    int i;
    HostCmd_GET_QUEUE_STATS *pCmd = (HostCmd_GET_QUEUE_STATS *)&wlpptr->pCmdBuf[0];
#ifdef QUEUE_STATS_CNT_HIST    
    QS_COUNTERS_t *pQS_Counters = &(pCmd->QueueStats.qs_u.Counters);
    QS_RETRY_HIST_t *pQS_RetryHist = &(pCmd->QueueStats.qs_u.RetryHist);
    QS_RATE_HIST_t *pQS_RateHist = &(pCmd->QueueStats.qs_u.RateHist);
    QS_RX_RATE_HIST_t *pQS_RxRateHist = &(pCmd->QueueStats.qs_u.RxRateHist);
    /* 22 & 72 MBPS not supported */
    UINT8 qsSupportedRatesG[QS_MAX_DATA_RATES_G] = 
            {0,1,2,3,0xff,4,5,6,7,8,9,10,11,0xff};
    /* support MCS 1,2,3,4,5,6,7,9,10,11,12,13,14,15,21,22,23 only */
    UINT8 qsSupportedMCS[QS_MAX_SUPPORTED_MCS] = 
            {0xff,0,1,2,3,4,5,6,0xff,7,8,9,10,11,12,13,0xff,0xff,0xff,0xff,0xff,14,15,16};
    int bw,sgi,nss;
    int SwBaQIndx[4] = {7,0,1,2};
#endif
#ifdef QUEUE_STATS_LATENCY
    QS_LATENCY_t *pQS_Latency = &(pCmd->QueueStats.qs_u.Latency);
#endif
    unsigned long flags;
    int retval=0;

    WLDBG_ENTER(DBG_LEVEL_0);

    MWL_SPIN_LOCK(&wlpptr->wlpd_p->locks.fwLock);
    memset(pCmd, 0x00, sizeof(HostCmd_GET_QUEUE_STATS));
    pCmd->CmdHdr.Cmd      = ENDIAN_SWAP16(HostCmd_CMD_GET_QUEUE_STATS);
    pCmd->CmdHdr.Length   = ENDIAN_SWAP16(sizeof(FWCmdHdr));
    pCmd->CmdHdr.macid    = (UINT8)option;

#ifdef SOC_W8764
    dispRxPacket = (dispRxPacket+1) & 0x01;
#endif

    WLDBG_DUMP_DATA(DBG_LEVEL_0, (void *) pCmd, sizeof(HostCmd_GET_QUEUE_STATS));
    if (wlexecuteCommand(netdev, HostCmd_CMD_GET_QUEUE_STATS))
    {
        WLDBG_EXIT_INFO(DBG_LEVEL_0, "failed execution");
        printk("failed execution\n");
        MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
        return FAIL;
    }

    switch(option&0xf)
    {
        #ifdef QUEUE_STATS_LATENCY
        case QS_GET_TX_LATENCY:
        {
            WLDBG_PRINT_QUEUE_STATS_LATENCY;
            printk("\nFW Packet Latency (microsecond)\n");
            printk("TCQ\t   FW_Min\t   FW_Max\t  FW_Mean\n");
            for(i=0; i<NUM_OF_TCQ; i++)
            {
                if(pQS_Latency->TCQxFwLatency[i].Max)
                {
                    printk("%2d    %10u\t%10u\t%10u\n",i
                    , ENDIAN_SWAP32((int)pQS_Latency->TCQxFwLatency[i].Min)
                    , ENDIAN_SWAP32((int)pQS_Latency->TCQxFwLatency[i].Max)
                    , ENDIAN_SWAP32((int)pQS_Latency->TCQxFwLatency[i].Mean));
                }
            }
            printk("TCQ\t  MAC_Min\t  MAC_Max\t MAC_Mean\n");
            for(i=0; i<NUM_OF_TCQ; i++)
            {
                if(pQS_Latency->TCQxMacLatency[i].Max)
                {
                    printk("%2d    %10u\t%10u\t%10u\n",i
                    , ENDIAN_SWAP32((int)pQS_Latency->TCQxMacLatency[i].Min)
                    , ENDIAN_SWAP32((int)pQS_Latency->TCQxMacLatency[i].Max)
                    , ENDIAN_SWAP32((int)pQS_Latency->TCQxMacLatency[i].Mean));
                }
            }
            printk("TCQ\tMAC_HW_Min\tMAC_HW_Max\tMAC_HW_Mean\n");
            for(i=0; i<NUM_OF_TCQ; i++)
            {
                if(pQS_Latency->TCQxMacHwLatency[i].Max)
                {
                    printk("%2d    %10u\t%10u\t%10u\n",i
                    , ENDIAN_SWAP32((int)pQS_Latency->TCQxMacHwLatency[i].Min)
                    , ENDIAN_SWAP32((int)pQS_Latency->TCQxMacHwLatency[i].Max)
                    , ENDIAN_SWAP32((int)pQS_Latency->TCQxMacHwLatency[i].Mean));
                }
            }
            printk("TCQ\tTotal_Min\tTotal_Max\tTotal_Mean\n");
            for(i=0; i<NUM_OF_TCQ; i++)
            {
                if(pQS_Latency->TCQxTotalLatency[i].Max)
                {
                    printk("%2d    %10u\t%10u\t%10u\n",i
                    , ENDIAN_SWAP32((int)pQS_Latency->TCQxTotalLatency[i].Min)
                    , ENDIAN_SWAP32((int)pQS_Latency->TCQxTotalLatency[i].Max)
                    , ENDIAN_SWAP32((int)pQS_Latency->TCQxTotalLatency[i].Mean)
                    );
                }
            }
            printk("\nQueue Size\n");
            printk("TCQ\t     Min\t      Max\t     Mean\n");
            for(i=0; i<NUM_OF_TCQ; i++)
            {
                if(pQS_Latency->TCQxQSize[i].Max)
                {
                    printk("%2d    %10u\t%10u\t%10u\n",i
                    , ENDIAN_SWAP32((int)pQS_Latency->TCQxQSize[i].Min>>4)
                    , ENDIAN_SWAP32((int)pQS_Latency->TCQxQSize[i].Max>>4)
                    , ENDIAN_SWAP32((int)pQS_Latency->TCQxQSize[i].Mean>>4));
                }
            }
            
            break;
        }
		case QS_GET_RX_LATENCY:
		{
			printk("\nRX: FW Packet Latency (microsecond)\n");
			printk("FW_Min\t   FW_Max\t  FW_Mean\n");
			printk("%10u\t%10u\t%10u\n", ENDIAN_SWAP32((int)pQS_Latency->RxFWLatency.Min)
			, ENDIAN_SWAP32((int)pQS_Latency->RxFWLatency.Max)
			, ENDIAN_SWAP32((int)pQS_Latency->RxFWLatency.Mean));	 
						
			WLDBG_PRINT_QUEUE_STATS_RX_LATENCY;
			break;
		}
        #endif
        #ifdef QUEUE_STATS_CNT_HIST
        case QS_GET_TX_COUNTER:
        {
            int j;
            WLDBG_PRINT_QUEUE_STATS_COUNTERS;
            printk("\n------------------------\n");
            printk("TX: FW Packet Statistics\n");
            printk("------------------------");
            printk("\nNon-AMPDU Packet Counters\n");
            printk("TCQ\tAttempts\tSuccesses\tSuccess_with_Retries\t  Failures\n");
            for(i=0; i<NUM_OF_TCQ; i++)
            {
                if(pQS_Counters->TCQxAttempts[i])
                {
                    printk("%2d    %10u\t%10u\t    %10u\t\t%10u\n",i
                    , ENDIAN_SWAP32((int)pQS_Counters->TCQxAttempts[i])
                    , ENDIAN_SWAP32((int)pQS_Counters->TCQxSuccesses[i])
                    , (ENDIAN_SWAP32((int)pQS_Counters->TCQxRetrySuccesses[i])
                    + ENDIAN_SWAP32((int)pQS_Counters->TCQxMultipleRetrySuccesses[i]))
                    , ENDIAN_SWAP32((int)pQS_Counters->TCQxFailures[i]));
                }
            }
            
            printk("\nPacket Per Second\n");
            printk("TCQ\tPPS_Min\t\t  PPS_Max\t\t PPS_Mean\n");
            for(i=0; i<NUM_OF_TCQ; i++)
            {
                if(pQS_Counters->TCQxPktRates[i].Max)
                {
                    printk("%2d    %10u\t%10u\t     %10u\n",i
                    , ENDIAN_SWAP32((int)pQS_Counters->TCQxPktRates[i].Min)
                    , ENDIAN_SWAP32((int)pQS_Counters->TCQxPktRates[i].Max)
                    , ENDIAN_SWAP32((int)pQS_Counters->TCQxPktRates[i].Mean));
                }
            }
            printk("\nHW Block Ack Stream Counters\n");
            printk("Stream\tEnqueued\tAttempts\tSuccesses\t Retry\t       BAR     Failures\n");
            for(i=0; i<NUM_OF_HW_BA; i++)
            {
                if(pQS_Counters->BAxStreamStats[i].BaPktEnqueued)
                {
                    printk(" %d    %10u      %10u       %10u   %10u  %10u   %10u\n",i
                    ,ENDIAN_SWAP32((int)pQS_Counters->BAxStreamStats[i].BaPktEnqueued)
                    ,ENDIAN_SWAP32((int)pQS_Counters->BAxStreamStats[i].BaPktAttempts)
                    ,ENDIAN_SWAP32((int)pQS_Counters->BAxStreamStats[i].BaPktSuccess)
                    ,ENDIAN_SWAP32((int)pQS_Counters->BAxStreamStats[i].BaRetryCnt)
                    ,ENDIAN_SWAP32((int)pQS_Counters->BAxStreamStats[i].BarCnt)
                    ,ENDIAN_SWAP32((int)pQS_Counters->BAxStreamStats[i].BaPktFailures));
                }
            }
            
            printk("\nSW BA Stream Counters\n");
            printk("QID   Enqueued      TxDone  Total_Retry  QNotReady      QFull  DropNonBa   WrongQid     DropMc   FailHwEnQ\n");
            for(i=0; i<QS_NUM_STA_SUPPORTED; i++)
            {
                if(pQS_Counters->SwBAStats[i].SwBaPktEnqueued)
                {
                    printk("%2d  %10u  %10u   %10u %10u %10u %10u %10u %10u %10u\n",SwBaQIndx[i]
                    ,ENDIAN_SWAP32((int)pQS_Counters->SwBAStats[i].SwBaPktEnqueued)
                    ,ENDIAN_SWAP32((int)pQS_Counters->SwBAStats[i].SwBaPktDone)
                    ,ENDIAN_SWAP32((int)pQS_Counters->SwBAStats[i].SwBaRetryCnt)
                    ,ENDIAN_SWAP32((int)pQS_Counters->SwBAStats[i].SwBaQNotReadyDrop)
                    ,ENDIAN_SWAP32((int)pQS_Counters->SwBAStats[i].SwBaQFullDrop)
                    ,ENDIAN_SWAP32((int)pQS_Counters->SwBAStats[i].SwBaDropNonBa)
                    ,ENDIAN_SWAP32((int)pQS_Counters->SwBAStats[i].SwBaWrongQid)
                    ,ENDIAN_SWAP32((int)pQS_Counters->SwBAStats[i].SwBaDropMc)
                    ,ENDIAN_SWAP32((int)pQS_Counters->SwBAStats[i].SwBaFailHwEnQ));
                }
            }
            for(i=0; i<QS_NUM_STA_SUPPORTED; i++)
            {
                if(pQS_Counters->SwBAStats[i].SwBaPktEnqueued)
                {
                    SWBA_LFTM_STATS_t SBLTS;

                    
                    if(wlFwGetAddrValue(netdev, (UINT32)pQS_Counters->SwBAStats[i].pSBLTS,
                        (sizeof(SWBA_LFTM_STATS_t)>>2), (UINT32*)&SBLTS,0 ) == SUCCESS)
                    {
                        int k;
                        printk("\nQID=%d Life Time Expiration drop=%lu\n", SwBaQIndx[i], SBLTS.SBLT_ExpiredCnt);
                        if(SBLTS.SBLT_ExpiredCnt)
                        {
                            printk("      num_Retry\t   Packets\n");
                            for(k=0; k<63; k++)
                            {
                                if(SBLTS.SBLT_Retry[k])
                                    printk("\t%2d \t%10lu\n",k,SBLTS.SBLT_Retry[k]);
                            }
                        }
                    }
                    else
                    {
                        printk("Could not get the SwBa Life Time Error Info\n");
                    }
                }
            }
            
            printk("\nPer STA counters\n");
            printk("----------------\n");
            printk("MAC address\t\t Attempts\tSuccesses\tSuccess_with_Retries\t  Failures\n");
            for(j=0; j< QS_NUM_STA_SUPPORTED; j++)
            {
                char *ba_str[3]={"[HwBa]","[SwBa]","      "};
                if(!ENDIAN_SWAP16((int)pQS_Counters->StaCounters[j].valid))
                {
                    continue;
                }
                printk("%02x:%02x:%02x:%02x:%02x:%02x%s ",
                    pQS_Counters->StaCounters[j].addr[0],
                    pQS_Counters->StaCounters[j].addr[1],
                    pQS_Counters->StaCounters[j].addr[2],
                    pQS_Counters->StaCounters[j].addr[3],
                    pQS_Counters->StaCounters[j].addr[4],
                    pQS_Counters->StaCounters[j].addr[5],
                    ba_str[wlCheckBa(netdev,pQS_Counters->StaCounters[j].addr)]);
                    
                printk("%10u\t%10u\t%10u\t\t%10u\n"
                    , ENDIAN_SWAP32((int)pQS_Counters->StaCounters[j].TxAttempts)
                    , ENDIAN_SWAP32((int)pQS_Counters->StaCounters[j].TxSuccesses)
                    , (ENDIAN_SWAP32((int)pQS_Counters->StaCounters[j].TxRetrySuccesses)
                    + ENDIAN_SWAP32((int)pQS_Counters->StaCounters[j].TxMultipleRetrySuccesses))
                    , ENDIAN_SWAP32((int)pQS_Counters->StaCounters[j].TxFailures));
            }
            printk("\n\n---------------\n");
			printk("Rx Pkt counters\n");
            printk("---------------\n");
            printk("MAC address\t\tFw_RxEntry\tDrv_RxEntry\tDrv_80211Input\tDrv_Forwarder\n");
			for(j=0; j<4; j++)
			{
			   if(pQS_Counters->rxStaCounters[j].valid)
			   {
				   printk("%02x:%02x:%02x:%02x:%02x:%02x\t",
				   pQS_Counters->rxStaCounters[j].addr[0],
					   pQS_Counters->rxStaCounters[j].addr[1],
					   pQS_Counters->rxStaCounters[j].addr[2],
					   pQS_Counters->rxStaCounters[j].addr[3],
					   pQS_Counters->rxStaCounters[j].addr[4],
					   pQS_Counters->rxStaCounters[j].addr[5]);
				   printk("%10u\t%10u\t%10u\t%10u\n", 
					   ENDIAN_SWAP32((int)pQS_Counters->rxStaCounters[j].rxPktCounts), 
					   rxPktStats_sta[j].RxRecvPollCnt,
					   rxPktStats_sta[j].Rx80211InputCnt,
					   rxPktStats_sta[j].RxfwdCnt);
			   }
			   else
			   {
			   break;
			   }
			}
            break;
        }
        case QS_GET_RETRY_HIST:
        {
            int j;
            UINT32 RetryHist[NUM_OF_RETRY_BIN];
            QS_RETRY_HIST_t qsRH;
            memcpy((void *)&qsRH, (void *)pQS_RetryHist, sizeof(QS_RETRY_HIST_t));
            printk("\nPacket Retry Histogram");
            for(j=0; j<NUM_OF_TCQ; j++)
            {
                if(qsRH.TotalPkts[j])
                {
                    if(wlFwGetAddrValue(netdev, (UINT32)qsRH.TxPktRetryHistogram[j],
                        NUM_OF_RETRY_BIN, (UINT32 *)RetryHist,0 ) == SUCCESS)
                    {
                        printk("\n  num_Retry\t   Packets\tQID=%d\tTotal packets = %lu\n",j, qsRH.TotalPkts[j]);
                        for(i=0; i<NUM_OF_RETRY_BIN; i++)
                        {
                            if(RetryHist[i])
                            {
                                printk("\t%2d \t%10u\n",i, ENDIAN_SWAP32((int)RetryHist[i]));
                            }
                        }
                    }
                }
            }
            break;
        }
        case QS_GET_TX_RATE_HIST:
        {
            int _11G_rate[14]={1,2,5,11,22,6,9,12,18,24,36,48,54,72};
            char *bwStr[3] = {"ht20","ht40", "ht80"};
            char *sgiStr[2] = {"lgi","sgi"};
            if((ENDIAN_SWAP32((int)pQS_RateHist->duration) > 0) && ENDIAN_SWAP16((int)pQS_RateHist->RateHistoram.valid))
            {
                printk("\nRate Histogram (Total samples = %10u)\n", ENDIAN_SWAP32((int)pQS_RateHist->duration));
                printk("\nSTA %02x:%02x:%02x:%02x:%02x:%02x\n",
                    pQS_RateHist->RateHistoram.addr[0],
                    pQS_RateHist->RateHistoram.addr[1],
                    pQS_RateHist->RateHistoram.addr[2],
                    pQS_RateHist->RateHistoram.addr[3],
                    pQS_RateHist->RateHistoram.addr[4],
                    pQS_RateHist->RateHistoram.addr[5]);
                printk("============================\n");
                for(i=0; i<QS_MAX_DATA_RATES_G; i++)
                {
                    if(qsSupportedRatesG[i] != 0xff)
                    {
                        int rdi = qsSupportedRatesG[i];
                        if(ENDIAN_SWAP32((int)pQS_RateHist->RateHistoram.LegacyRates[rdi]) > 0)
                        {
                            if(i == 2)
                            {
                                printk("5.5Mbps \t: %10u\n" 
                                , ENDIAN_SWAP32((int)pQS_RateHist->RateHistoram.LegacyRates[rdi]));
                            }
                            else
                            {
                                printk("%2dMbps  \t: %10u\n",_11G_rate[i] 
                                , ENDIAN_SWAP32((int)pQS_RateHist->RateHistoram.LegacyRates[rdi]));
                            }
                        }
                    }
                }
                for(bw=0; bw<QS_NUM_SUPPORTED_11N_BW; bw++)
                {
                    for(sgi=0; sgi<QS_NUM_SUPPORTED_GI;sgi++)
                    {
                        for(i=0; i<24; i++)
                        {
                            if(qsSupportedMCS[i] != 0xff)
                            {
                                int rdi = qsSupportedMCS[i];
                                if(ENDIAN_SWAP32((int)pQS_RateHist->RateHistoram.HtRates[bw][sgi][rdi]) > 0)
                                {
                                        printk("%4s_%3s_MCS%2d  : %10u\n",bwStr[bw],sgiStr[sgi],i 
                                        , ENDIAN_SWAP32((int)pQS_RateHist->RateHistoram.HtRates[bw][sgi][rdi]));
                                }
                            }
                        }
                    }
                }
                for(nss=0; nss<QS_NUM_SUPPORTED_11AC_NSS; nss++)
                {
                    for(bw=0; bw<QS_NUM_SUPPORTED_11AC_BW; bw++)
                    {
                        for(i=0; i<QS_NUM_SUPPORTED_11AC_MCS; i++)
                        {
                            for(sgi=0; sgi<QS_NUM_SUPPORTED_GI;sgi++)
                            {
                                if(ENDIAN_SWAP32((int)pQS_RateHist->RateHistoram.VHtRates[nss][bw][sgi][i]) > 0)
                                {
                                        printk("%4s_%3s_%1dSS_MCS%2d  : %10u\n",bwStr[bw],sgiStr[sgi],(nss+1),i 
                                        , ENDIAN_SWAP32((int)pQS_RateHist->RateHistoram.VHtRates[nss][bw][sgi][i]));
                                }
                            }
                        }
                    }
                }
                printk("============================\n");
            }
            else
            {
                retval=1;
            }
            break;
        }
        case QS_GET_RX_RATE_HIST:
        {
            int _11G_rate[14]={1,2,5,11,22,6,9,12,18,24,36,48,54,72};
            char *bwStr[3] = {"ht20","ht40", "ht80"};
            char *sgiStr[2] = {"lgi","sgi"};
            {
                printk("\nRx Data Frame Rate Histogram \n");
                printk("============================\n");
                for(i=0; i<QS_MAX_DATA_RATES_G; i++)
                {
                    if(qsSupportedRatesG[i] != 0xff)
                    {
                        int rdi = qsSupportedRatesG[i];
                        if(ENDIAN_SWAP32((int)pQS_RxRateHist->RateHistoram.LegacyRates[rdi]) > 0)
                        {
                            if(i == 2)
                            {
                                printk("5.5Mbps \t: %10u\n" 
                                , ENDIAN_SWAP32((int)pQS_RxRateHist->RateHistoram.LegacyRates[rdi]));
                            }
                            else
                            {
                                printk("%2dMbps  \t: %10u\n",_11G_rate[i] 
                                , ENDIAN_SWAP32((int)pQS_RxRateHist->RateHistoram.LegacyRates[rdi]));
                            }
                        }
                    }
                }
                for(bw=0; bw<QS_NUM_SUPPORTED_11N_BW; bw++)
                {
                    for(sgi=0; sgi<QS_NUM_SUPPORTED_GI;sgi++)
                    {
                        for(i=0; i<24; i++)
                        {
                            if(qsSupportedMCS[i] != 0xff)
                            {
                                int rdi = qsSupportedMCS[i];
                                if(ENDIAN_SWAP32((int)pQS_RxRateHist->RateHistoram.HtRates[bw][sgi][rdi]) > 0)
                                {
                                        printk("%4s_%3s_MCS%2d  : %10u\n",bwStr[bw],sgiStr[sgi],i 
                                        , ENDIAN_SWAP32((int)pQS_RxRateHist->RateHistoram.HtRates[bw][sgi][rdi]));
                                }
                            }
                        }
                    }
                }
                for(nss=0; nss<QS_NUM_SUPPORTED_11AC_NSS; nss++)
                {
                    for(bw=0; bw<QS_NUM_SUPPORTED_11AC_BW; bw++)
                    {
                        for(i=0; i<QS_NUM_SUPPORTED_11AC_MCS; i++)
                        {
                            for(sgi=0; sgi<QS_NUM_SUPPORTED_GI;sgi++)
                            {
                                if(ENDIAN_SWAP32((int)pQS_RxRateHist->RateHistoram.VHtRates[nss][bw][sgi][i]) > 0)
                                {
                                        printk("%4s_%3s_%1dSS_MCS%2d  : %10u\n",bwStr[bw],sgiStr[sgi],(nss+1),i 
                                        , ENDIAN_SWAP32((int)pQS_RxRateHist->RateHistoram.VHtRates[nss][bw][sgi][i]));
                                }
                            }
                        }
                    }
                }
                printk("============================\n");
            }
            break;
        }
        #endif
    }
    MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
    return retval;
}

int wlFwResetQueueStats( struct net_device *netdev)
{
    struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
    FWCmdHdr *pCmd = (FWCmdHdr *)&wlpptr->pCmdBuf[0];

    unsigned long flags; 

    WLDBG_ENTER(DBG_LEVEL_0);

    MWL_SPIN_LOCK(&wlpptr->wlpd_p->locks.fwLock);
    memset(pCmd, 0x00, sizeof(FWCmdHdr));
    pCmd->Cmd      = ENDIAN_SWAP16(HostCmd_CMD_RESET_QUEUE_STATS);
    pCmd->Length   = ENDIAN_SWAP16(sizeof(FWCmdHdr));

#ifdef SOC_W8764
    dispRxPacket = (dispRxPacket+1) & 0x01;
#endif

    WLDBG_DUMP_DATA(DBG_LEVEL_0, (void *) pCmd, sizeof(FWCmdHdr));
    if (wlexecuteCommand(netdev, HostCmd_CMD_RESET_QUEUE_STATS))
    {
        WLDBG_EXIT_INFO(DBG_LEVEL_0, "failed execution");
        MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
        return FAIL;
    }
    wldbgResetQueueStats();
    if(numOfRxSta)
    {
        wlFwSetMacSa(netdev, numOfRxSta, qs_rxMacAddrSave);
    }
    printk("queue_stats reset ok!\n");
    MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
    return 0;
}

int wlFwSetMacSa(struct net_device *netdev, int n, UINT8 *addr)	  
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);

	HostCmd_QSTATS_SET_SA *pCmd =
		(HostCmd_QSTATS_SET_SA *) &wlpptr->pCmdBuf[0];
	int retval = FAIL;
	unsigned long flags;


	MWL_SPIN_LOCK(&wlpptr->wlpd_p->locks.fwLock);
	memset(pCmd, 0x00, sizeof(HostCmd_QSTATS_SET_SA));
	pCmd->CmdHdr.Cmd    = ENDIAN_SWAP16(HostCmd_CMD_QSTATS_SET_SA);
	pCmd->CmdHdr.Length = ENDIAN_SWAP16(sizeof(HostCmd_QSTATS_SET_SA));
	memcpy(pCmd->Addr, addr, (sizeof(IEEEtypes_MacAddr_t)*n));
	pCmd->NumOfAddrs = n;

	WLDBG_DUMP_DATA(DBG_LEVEL_0, (void *) pCmd,
		sizeof(HostCmd_QSTATS_SET_SA));
	retval = wlexecuteCommand(netdev, HostCmd_CMD_QSTATS_SET_SA);
	MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
	return retval;
}


/*Get MAC addr to be kicked out when consecutive tx failure cnt > limit*/
int wlFwGetConsecTxFailAddr(struct net_device *netdev, IEEEtypes_MacAddr_t *addr)	  
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	HostCmd_FW_GET_CONSEC_TXFAIL_ADDR *pCmd =
		(HostCmd_FW_GET_CONSEC_TXFAIL_ADDR *) &wlpptr->pCmdBuf[0];
	int retval = FAIL;
	unsigned long flags;		

	MWL_SPIN_LOCK(&wlpptr->wlpd_p->locks.fwLock);
	memset(pCmd, 0x00, sizeof(HostCmd_FW_GET_CONSEC_TXFAIL_ADDR));
	pCmd->CmdHdr.Cmd    = ENDIAN_SWAP16(HostCmd_CMD_GET_CONSEC_TXFAIL_ADDR);
	pCmd->CmdHdr.Length = ENDIAN_SWAP16(sizeof(HostCmd_FW_GET_CONSEC_TXFAIL_ADDR));


	WLDBG_DUMP_DATA(DBG_LEVEL_0, (void *) pCmd,
		sizeof(HostCmd_FW_GET_CONSEC_TXFAIL_ADDR));
	retval = wlexecuteCommand(netdev, HostCmd_CMD_GET_CONSEC_TXFAIL_ADDR);

	if(!retval)
		memcpy(addr, pCmd->Addr,sizeof(IEEEtypes_MacAddr_t));
		
	
	MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
	return retval;
}

/*Set consective tx failure limit. When consecutive tx failure cnt > limit, client will be kicked out*/
int wlFwSetConsecTxFailLimit(struct net_device *netdev, UINT32 value)   
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	
	HostCmd_FW_TXFAILLIMIT *pCmd =
		(HostCmd_FW_TXFAILLIMIT *) &wlpptr->pCmdBuf[0];
	int retval = FAIL;
	unsigned long flags;
	
	MWL_SPIN_LOCK(&wlpptr->wlpd_p->locks.fwLock);
	memset(pCmd, 0x00, sizeof(HostCmd_FW_TXFAILLIMIT));
	pCmd->CmdHdr.Cmd	= ENDIAN_SWAP16(HostCmd_CMD_SET_TXFAILLIMIT);
	pCmd->CmdHdr.Length = ENDIAN_SWAP16(sizeof(HostCmd_FW_TXFAILLIMIT));
	pCmd->txfaillimit = ENDIAN_SWAP32(value);

	WLDBG_DUMP_DATA(DBG_LEVEL_0, (void *) pCmd, sizeof(HostCmd_FW_TXFAILLIMIT));
	retval = wlexecuteCommand(netdev, HostCmd_CMD_SET_TXFAILLIMIT);
	
	MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
	return retval;
}

int wlFwGetConsecTxFailLimit(struct net_device *netdev, UINT32 *value)   
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	
	HostCmd_FW_TXFAILLIMIT *pCmd =
		(HostCmd_FW_TXFAILLIMIT *) &wlpptr->pCmdBuf[0];
	int retval = FAIL;
	unsigned long flags;
		
	MWL_SPIN_LOCK(&wlpptr->wlpd_p->locks.fwLock);
	memset(pCmd, 0x00, sizeof(HostCmd_FW_TXFAILLIMIT));
	pCmd->CmdHdr.Cmd	= ENDIAN_SWAP16(HostCmd_CMD_GET_TXFAILLIMIT);
	pCmd->CmdHdr.Length = ENDIAN_SWAP16(sizeof(HostCmd_FW_TXFAILLIMIT));
	
	WLDBG_DUMP_DATA(DBG_LEVEL_0, (void *) pCmd, sizeof(HostCmd_FW_TXFAILLIMIT));
	retval = wlexecuteCommand(netdev, HostCmd_CMD_GET_TXFAILLIMIT);
	if(!retval)
		*value = ENDIAN_SWAP32(pCmd->txfaillimit);
	
	MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
	return retval;
}

int wlCheckBa(struct net_device *netdev, UINT8 *addr)
{
	struct wlprivate *priv = NETDEV_PRIV_P(struct wlprivate, netdev);
    int i;
    for(i=0; i < MAX_SUPPORT_AMPDU_TX_STREAM; i++)
    {
        if(memcmp(priv->wlpd_p->Ampdu_tx[i].MacAddr, addr, 6) == 0)
        {
            if(i<2)
                return IS_HW_BA;
            else
                return IS_SW_BA;
        }
    }
    return NONE_BA;
}
#endif


int wlFwSetBWSignalType( struct net_device *netdev, UINT32 mode)
{
	int retval = FAIL;
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	HostCmd_FW_SET_BW_SIGNALLING  *pCmd = (HostCmd_FW_SET_BW_SIGNALLING *) &wlpptr->pCmdBuf[0];
	unsigned long flags;

#ifdef MFG_SUPPORT
	if (wlpptr->mfgEnable)
	{		
		return SUCCESS;
	}
#endif
	MWL_SPIN_LOCK(&wlpptr->wlpd_p->locks.fwLock);
	memset(pCmd, 0x00, sizeof(HostCmd_FW_SET_BW_SIGNALLING));
	pCmd->CmdHdr.Cmd    = ENDIAN_SWAP16(HostCmd_CMD_SET_BW_SIGNALLING);
	pCmd->CmdHdr.Length = ENDIAN_SWAP16(sizeof(HostCmd_FW_SET_BW_SIGNALLING));
	pCmd->Action       =ENDIAN_SWAP32(WL_SET);
	pCmd->Mode = ENDIAN_SWAP32(mode);
	retval = wlexecuteCommand(netdev, HostCmd_CMD_SET_BW_SIGNALLING);
	MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
	return retval;
}


/*Once VHT Operating Mode is received, we update peer VHT channel bandwidth and Rx Nss in peer info*/
int wlFwSetVHTOpMode(struct net_device *netdev,IEEEtypes_MacAddr_t *staaddr, UINT8 vht_NewRxChannelWidth, UINT8 vht_NewRxNss)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	HostCmd_FW_VHT_OP_MODE *pCmd =
		(HostCmd_FW_VHT_OP_MODE *)&wlpptr->pCmdBuf[0];
	int retval = FAIL;

	unsigned long flags; 

	WLDBG_ENTER(DBG_LEVEL_0);

	MWL_SPIN_LOCK(&wlpptr->wlpd_p->locks.fwLock);
	memset(pCmd, 0x00, sizeof(HostCmd_FW_VHT_OP_MODE));
	pCmd->CmdHdr.Cmd    = ENDIAN_SWAP16(HostCmd_CMD_SET_VHT_OP_MODE);
	pCmd->CmdHdr.Length = ENDIAN_SWAP16(sizeof(HostCmd_FW_VHT_OP_MODE));
	pCmd->CmdHdr.macid = wlpptr->vmacSta_p->VMacEntry.macId;

	pCmd->vht_NewRxChannelWidth = vht_NewRxChannelWidth;
	pCmd->vht_NewRxNss = vht_NewRxNss;
	memcpy(pCmd->Addr, staaddr, sizeof(IEEEtypes_MacAddr_t));

	WLDBG_DUMP_DATA(DBG_LEVEL_0, (void *) pCmd, 
		sizeof(HostCmd_FW_VHT_OP_MODE));
	retval = wlexecuteCommand(netdev, HostCmd_CMD_SET_VHT_OP_MODE);
	MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
	return retval;
}


#ifdef WNC_LED_CTRL
int wlFwLedOn(struct net_device *netdev, UINT8 led_on)
{

	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	HostCmd_FW_LED_CTRL *pCmd = (HostCmd_FW_LED_CTRL *) &wlpptr->pCmdBuf[0];
	int retval = FAIL;

	unsigned long flags; 

	MWL_SPIN_LOCK(&wlpptr->wlpd_p->locks.fwLock);
	memset(pCmd, 0x00, sizeof(HostCmd_FW_LED_CTRL));
	pCmd->CmdHdr.Cmd    = ENDIAN_SWAP16(HostCmd_CMD_LED_CTRL);
	pCmd->CmdHdr.Length = ENDIAN_SWAP16(sizeof(HostCmd_FW_LED_CTRL));
	pCmd->led_on = led_on;

	retval = wlexecuteCommand(netdev, HostCmd_CMD_LED_CTRL);
	MWL_SPIN_UNLOCK(&wlpptr->wlpd_p->locks.fwLock);
	return retval;
}
#endif	
