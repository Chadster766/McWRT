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

#ifndef _AP_INTF_H_
#define _AP_INTF_H_

#include "IEEE_types.h"
#include "wltypes.h"
#include "buildModes.h"
#include "wl_hal.h"
#include "dfs.h"
#include "osif.h"

#define MAX_NUM_RX_DESC        256
/* WME stream classes */
#define	WME_AC_BE	0		/* best effort */
#define	WME_AC_BK	1		/* background */
#define	WME_AC_VI	2		/* video */
#define	WME_AC_VO	3		/* voice */

#define TID_TO_WME_AC(_tid) (      \
	((_tid) == 0 || (_tid) == 3) ? WME_AC_BE : \
	((_tid) < 3) ? WME_AC_BK : \
	((_tid) < 6) ? WME_AC_VI : \
	WME_AC_VO)

#define MAX_SUPPORT_AMPDU_TX_STREAM 4  /* AMPDU_SUPPORT_SBA */

#define MAX_AMPDU_REORDER_BUFFER MAX_AID
#define MAX_AC 4
#define MAX_UP 8
#define MAX_AC_SEQNO 4096

#ifdef MCAST_PS_OFFLOAD_SUPPORT
#define NUM_OF_DESCRIPTOR_DATA (4 + NUMOFAPS) 
#else
#define NUM_OF_DESCRIPTOR_DATA 4
#endif
#define MAX_NUM_AGGR_BUFF 256
#define MAX_NUM_RX_DESC        256
/*3839 ~ 4k*/
#define MAX_AGGR_SIZE		4096
#define MAX_NUM_TX_DESC        256
#define MIN_BYTES_HEADROOM      64
#define NUM_EXTRA_RX_BYTES     (2*MIN_BYTES_HEADROOM)

struct wlhw_data {
	UINT32        fwReleaseNumber;      /* MajNbr:MinNbr:SubMin:PatchLevel */
	UINT8         hwVersion;            /* plain number indicating version */
	UINT8         hostInterface;        /* plain number of interface       */
	UINT16        maxNumTXdesc;         /* max number of TX descriptors    */
	UINT16        maxNumMCaddr;         /* max number multicast addresses  */
	UINT16        numAntennas;          /* number antennas used            */
	UINT16        regionCode;           /* region (eg. 0x10 for USA FCC)   */
	unsigned char    macAddr[IEEEtypes_ADDRESS_SIZE];    /* well known -> AA:BB:CC:DD:EE:FF */
};

struct wlpriv_stats {
	UINT32  skbheaderroomfailure;
	UINT32	tsoframecount;
	UINT32	weakiv_count;
	UINT32	weakiv_threshold_count;

};

typedef struct
{
	WL_PRIV *wlpptr;
	UINT8 MacAddr[6];
	UINT8 AccessCat;
	UINT8 InUse;
	UINT8 DialogToken;
	Timer timer;
	UINT8 initTimer;
	UINT8 AddBaResponseReceive;
	UINT32 TimeOut;
	UINT16 start_seqno;
	/* DYNAMIC_BA_SUPPORT */
	UINT32 txa_avgpps;
	UINT32 txa_ac;
	UINT32 txa_pkts;
	UINT32 txa_lastsample;

	UINT32 ReleaseTimestamp;
}Ampdu_tx_t;

typedef struct
{
	WL_BUFF  *pFrame[MAX_UP][MAX_AMPDU_REORDER_BUFFER];
	UINT16 ExpectedSeqNo[MAX_UP][MAX_AMPDU_REORDER_BUFFER];
	UINT16 CurrentSeqNo[MAX_UP];
	UINT16 ReOrdering[MAX_UP];
	UINT8 AddBaReceive[MAX_UP];
	UINT32 Time[MAX_UP];
	Timer timer[MAX_UP];
	UINT8 timer_init[MAX_UP];
}Ampdu_Pck_Reorder_t;

struct wllocks {   
	DECLARE_LOCK(xmitLock);             /* used to protect TX actions      */
	DECLARE_LOCK(fwLock);               /* used to protect FW commands     */
};

struct _wlprivate_data {
	void  *rootwlpptr;
	UINT16 	vmacIndex;
	UINT8 legAPCount;
	UINT8 nClients;	
	UINT8 n20MClients;					
	UINT8 NonGFSta; 
	UINT8 BcnAddHtAddChannel;
	UINT8 TxGf;
	UINT8 BcnAddHtOpMode;
	UINT8 legClients;
	/* DFS_SUPPORT */
	void  *pdfsApMain ;
	/* DFS_SUPPORT */
	Ampdu_Pck_Reorder_t AmpduPckReorder[MAX_AID+1];
	Ampdu_tx_t Ampdu_tx[MAX_SUPPORT_AMPDU_TX_STREAM];
	UINT8 Global_DialogToken;
	struct wllocks	locks;			/* various spinlocks			*/
};

struct wlprivate {
	struct net_device        *netDev;          /* the net_device struct        */
	struct net_device_stats  *netDevStats;     /* net_device statistics        */
	struct pci_dev           *pPciDev;         /* for access to pci cfg space  */
	void                     *ioBase0;         /* MEM Base Address Register 0  */
	void                     *ioBase1;         /* MEM Base Address Register 1  */
	unsigned short           *pCmdBuf;         /* pointer to CmdBuf (virtual)  */
	dma_addr_t               pPhysCmdBuf;      /* pointer to CmdBuf (physical) */
	Bool_t                   inSendCmd;
	struct wlhw_data		hwData; 		   /* Adapter HW specific info 	*/
	struct vmacApInfo_t 	*vmacSta_p;
	void                    *clntParent_priv_p;
	int (*wlreset)(struct net_device *netdev);
	struct wlprivate 			*rootpriv;
	struct wlprivate			*vdev[NUMOFAPS+1]; //+1 station
	struct _wlprivate_data 	*wlpd_p;
	UINT8 calTbl[200];
	UINT8 *FwPointer;
	UINT32 FwSize;	
	UINT8 mfgEnable;
	UINT32 cmdFlags;  /* Command flags */
	struct net_device *txNetdev_p;
	UINT32 nextBarNum;
	UINT32 chipversion;
	UINT32 mfgLoaded;
};


#endif /* _AP_INTF_H_ */
