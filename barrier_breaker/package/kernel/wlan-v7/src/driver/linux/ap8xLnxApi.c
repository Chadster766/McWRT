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

#include	<linux/module.h>
#include	<linux/kernel.h>
#include	<linux/net.h>
#include	<linux/netdevice.h>
#include	<linux/wireless.h>
#include	<linux/pci.h>
#include 	<linux/delay.h>
#include    <linux/string.h>

#include	<net/iw_handler.h>
#include	<asm/processor.h>
#include	<asm/uaccess.h>

#include "wl.h" 
#include "wldebug.h"
#include "ap8xLnxApi.h"
#include "ap8xLnxVer.h"
#include "IEEE_types.h"
#include "wl_mib.h"
#include "wl_hal.h"
#include "ap8xLnxFwcmd.h"
#include "ap8xLnxWlLog.h"
#include "wlApi.h"
#include "qos.h"
#include "ap8xLnxIoctl.h"
#include "ap8xLnxFwdl.h"
#include "StaDb.h"
#include "domain.h" // Added by ARUN to support 802.11d
#include "wlvmac.h"
#include "macmgmtap.h"
#include "macMgmtMlme.h"
#include "idList.h"
#include "keyMgmtSta.h"
#include "bcngen.h"
#include "wlFun.h"
#ifdef EWB
#include "ewb_hash.h"
#endif

#ifdef WDS_FEATURE
#include "wds.h"
#endif

#ifdef CLIENT_SUPPORT
#include "linkmgt.h"
#include "mlme.h"
#include "mlmeApi.h"
#endif

#ifdef MPRXY
#include "ap8xLnxMPrxy.h"
#endif

static int getMacFromString(unsigned char *macAddr, const char *pStr);
static int IPAsciiToNum(unsigned int * IPAddr, const char * pIPStr);

#ifdef SOC_W8864
void ratetable_print_SOCW8864(UINT8* pTbl);
#else
void ratetable_print_SOCW8764(UINT8* pTbl);	
#endif


#define WPAHEX64

#define MAX_IOCTL_PARAMS		3
#define MAX_IOCTL_PARAM_LEN		64

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,0,6)
#define NIPQUAD(addr) \
        ((unsigned char *)&addr)[0], \
        ((unsigned char *)&addr)[1], \
        ((unsigned char *)&addr)[2], \
        ((unsigned char *)&addr)[3]
#endif

/*  
* Statistics flags (bitmask in (struct iw_quality *)->updated)  
*/  
#ifndef IW_QUAL_QUAL_UPDATED  
#define IW_QUAL_QUAL_UPDATED    0x01    /* Value was updated since last read */  
#define IW_QUAL_LEVEL_UPDATED   0x02  
#define IW_QUAL_NOISE_UPDATED   0x04  
#define IW_QUAL_QUAL_INVALID    0x10    /* Driver doesn't provide value */  
#define IW_QUAL_LEVEL_INVALID   0x20  
#define IW_QUAL_NOISE_INVALID   0x40  
#endif /* IW_QUAL_QUAL_UPDATED */  
#ifndef IW_QUAL_DBM
#define IW_QUAL_DBM 0x08;
#endif
#ifndef IW_QUAL_ALL_UPDATED  
#define IW_QUAL_ALL_UPDATED     (IW_QUAL_QUAL_UPDATED | IW_QUAL_LEVEL_UPDATED | IW_QUAL_NOISE_UPDATED)  
#endif  
#ifndef IW_QUAL_ALL_INVALID  
#define IW_QUAL_ALL_INVALID     (IW_QUAL_QUAL_INVALID | IW_QUAL_LEVEL_INVALID | IW_QUAL_NOISE_INVALID)  
#endif
static int wlioctl_priv_wlparam(struct net_device *dev, 
struct iw_request_info *info,
	void *wrqu, char *extra);

static int getMacFromString(unsigned char *macAddr, const char *pStr);
#ifdef WPAHEX64
static void HexStringToHexDigi(char* outHexData, char* inHexString, USHORT Len);
#endif
static int IsHexKey( char* keyStr );


#ifdef CLIENT_SUPPORT
static MRVL_SCAN_ENTRY  siteSurvey[IEEEtypes_MAX_BSS_DESCRIPTS];
static MRVL_SCAN_ENTRY  siteSurveyEntry ;
#endif

static IEEEtypes_MacAddr_t bcastMacAddr = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
#define EXT_FW_SIZE 256*1024
static UINT8 ExtFwImage[EXT_FW_SIZE];

/* Since the scan cmd's return data is the largest one, 
   use its size for cmd buffer size */
static char cmdGetBuf[MAX_SCAN_BUF_SIZE];

UINT16 dfs_chirp_count_min = 5;
UINT16 dfs_chirp_time_interval = 1000;
UINT16 dfs_pw_filter       = 0x00;
UINT16 dfs_min_num_radar   = 5;
UINT16 dfs_min_pri_count   = 4;


int LoadExternalFw(struct wlprivate *priv, char *filename)
{
	mm_segment_t old_fs;
	int i = 0, j; 
    
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,18)
	struct file* filp;
#else
	int fd;
#endif

#ifdef DEFAULT_MFG_MODE
	if (priv->mfgLoaded) {
		printk("read img file: already done\n");
		return 1;
	}
#endif

	printk("read img file...\n");

	old_fs=get_fs();
	set_fs(KERNEL_DS);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,18)
	filp=filp_open(filename,0,0);
	if(!IS_ERR(filp))
	{
		while((j = vfs_read(filp, &ExtFwImage[i++], 0x01,&filp->f_pos)) != 0)
		{
			if (i >= EXT_FW_SIZE)
			{
				printk("ERROR: Firmware download failed! - Firmware size exceeds image memory = %d bytes. \n", 
					EXT_FW_SIZE);
				return 0;
			}
		}
		filp_close(filp, current->files);
	}
#else
	fd = sys_open(param[1], O_RDONLY, 0);
	if (fd >=0)
	{       
		while((j = sys_read(fd, &ExtFwImage[i++], 0x01)) != 0)
		{
			if (i >= EXT_FW_SIZE)
			{
				printk("ERROR: Firmware download failed! - Firmware size exceeds image memory = %d bytes. \n", 
					EXT_FW_SIZE);
				return 0;
			}
		}
		sys_close(fd);
	}
#endif			
	set_fs(old_fs);

	if(!i)
	{
		/* No file is loaded */
		return 0;
	}

	priv->FwPointer = &ExtFwImage[0];
	priv->FwSize = i-1;

	printk("FW len = %d\n", (int)priv->FwSize);
	return 1;
}



static const long frequency_list[] =
{
	2412, 2417, 2422, 2427, 2432, 2437, 2442,
	2447, 2452, 2457, 2462, 2467, 2472, 2484
};

static const int index_to_rate[] =
{
	2, 4, 11, 22, 12, 18, 24, 36, 48, 72, 96, 108
};

static unsigned short PhyRate[][5] =
{
	{2,   13,  15,  27,  30},   //0
	{4,   26,  29,  54,  60},   //1
	{11,  39,  43,  81,  90},   //2
	{22,  52,  58,  108, 120},  //3
	{44,  78,  87,  162, 180},  //4
	{12,  104, 115, 216, 240},  //5
	{18,  117, 130, 243, 270},  //6
	{24,  130, 144, 270, 300},  //7
	{36,  26,  29,  54,  60},   //8
	{48,  52,  58,  108, 120},	//9
	{72,  78,  87,  162, 180},  //10
	{96,  104, 116, 216, 240},  //11
	{108, 156, 173, 324, 360},  //12
	{0,   208, 231, 432, 480},  //13
	{0,   234, 260, 486, 540},  //14
	{0,   260, 289, 540, 600},  //15

	{0,  39,  43,  81,  90},    //16
	{0,  78,  87,  162, 180},	//17
	{0,  117, 130, 243, 270},   //18
	{0,  156, 173, 324, 360},   //19
	{0,  234, 260, 486, 540},   //20
	{0,  312, 347, 648, 720},   //21
	{0,  351, 390, 729, 810},   //22
	{0,  390, 433, 810, 900},   //23

};

/*20Mhz: Nss1_LGI, Nss1_SGI, Nss2_LGI, Nss2_SGI, Nss3_LGI, Nss3_SGI, */
static unsigned short PhyRate_11ac20M[][6] =
{
	{13, 15, 26, 29, 39, 44},  			// 0
	{26, 29, 52, 58, 78, 87},  			// 1
	{39, 44, 78, 87, 117, 130},  		// 2
	{52, 58, 104, 116, 156, 174},  		// 3
	{78, 87, 156, 174, 234, 260},  		// 4
	{104, 116, 208, 231, 312, 347}, 	// 5
	{117, 130, 234, 260, 351, 390}, 	// 6
	{130, 145, 260, 289, 390, 434}, 	// 7
	{156, 174, 312, 347, 468, 520}, 	// 8
	{2, 2, 2, 2, 520, 578},				// 9, Nss 1 and Nss 2 mcs9 not valid. 
};

/*40Mhz: Nss1_LGI, Nss1_SGI, Nss2_LGI, Nss2_SGI, Nss3_LGI, Nss3_SGI, */
static unsigned short PhyRate_11ac40M[][6] =
{
	{27, 30, 54, 60, 81, 90},  			// 0
	{54, 60, 108, 120, 162, 180},  		// 1
	{81, 90, 162, 180, 243, 270},  		// 2
	{108, 120, 216, 240, 324, 360},  	// 3
	{162, 180, 324, 360, 486, 540},  	// 4
	{216, 240, 432, 480, 648, 720}, 	// 5
	{243, 270, 486, 540, 729, 810}, 	// 6
	{270, 300, 540, 600, 810, 900}, 	// 7
	{324, 360, 648, 720, 972, 1080}, 	// 8
	{360, 400, 720, 800, 1080, 1200},	// 9
};

/*80Mhz: Nss1_LGI, Nss1_SGI, Nss2_LGI, Nss2_SGI, Nss3_LGI, Nss3_SGI, */
static unsigned short PhyRate_11ac80M[][6] =
{
	{59,  65, 117, 130, 175, 195},  	// 0
	{117, 130, 234, 260, 351, 390},  	// 1
	{175, 195, 351, 390, 527, 585},  	// 2
	{234, 260, 468, 520, 702, 780},  	// 3
	{351, 390, 702, 780, 1053, 1170},  	// 4
	{468, 520, 936, 1040, 1404, 1560}, 	// 5
	{527, 585, 1053, 1170, 2, 2}, 		// 6, Nss 3 mcs6 not valid. 
	{585, 650, 1170, 1300, 1755, 1950}, // 7
	{702, 780, 1404, 1560, 2106, 2340}, // 8
	{780, 867, 1560, 1733, 2340, 2600},	// 9

};

int legacyRateToId(int rate)
{
	int i;

	for (i=0; i<13; i++)
	{
		if (PhyRate[i][0] == rate)
			return i;
	}
	return -1;
}


/*
#ifdef SOC_W8764
#if defined(SOC_W8897) || defined(SOC_W8864)
typedef PACK_START struct dbRateInfo_t1
{
        UINT32   Format:     2;  //0 = Legacy format, 1 = 11n format, 2 = 11ac format
        UINT32   Stbc:       1;  //0 = Use standard guard interval,1 = Use short guard interval
        UINT32   rsvd1:      1;
        UINT32   Bandwidth:  2;  //0 = Use 20 MHz channel,1 = Use 40 MHz channel, 2 = Use 80 MHz
        UINT32   rsvd2:      1;
        UINT32   ShortGI:    1;  //0 = Use standard guard interval,1 = Use short guard interval
        UINT32   RateIDMCS:  7;
        UINT32   Preambletype:1; //Preambletype 0= Long, 1= Short;
        UINT32   PowerId:6;              // Power ID
        UINT32   AdvCoding:  1;  // 0 = LDPC off ,1 = LDPC on
        UINT32   TxBfFrame: 1;   // 0: beam forming off; 1: beam forming on
        UINT32   AntSelect:  2;  //Bitmap to select one of the transmit antennae
        UINT32   AntSelect2: 1;            // bit 2 of antenna selection field 
        UINT32   ant3: 1;
        UINT32   ant4: 4;
}PACK_END dbRateInfo_t1;
#else
typedef PACK_START struct dbRateInfo_t1
{
	UINT32 Format: 		1; //0 = Legacy format, 1 = Hi-throughput format
	UINT32 ShortGI: 	1; //0 = Use standard guard interval,1 = Use short guard interval
	UINT32 Bandwidth: 	1; //0 = Use 20 MHz channel,1 = Use 40 MHz channel
	UINT32 RateIDMCS: 	7;
	UINT32 AdvCoding: 	1; //ldpc
	UINT32 AntSelect: 	2; //Bitmap to select one of the transmit antenna
	UINT32 ActSubChan: 	2; //Active subchannel for 40 MHz mode 00:lower, 01= upper, 10= both on lower and upper
	UINT32 Preambletype: 1; //Preambletype 0= Long, 1= Short;
	UINT32 PowerId: 	4; // only lower 4 bits used - TRPC power
	UINT32 AntSelect2: 	1; // bit 2 of antenna selection field 
	UINT32 reserved: 	1;
	UINT32 TxBfFrame: 	1; // 0= beam forming off; 1= beam forming on
	UINT32 GreenField:	1; // 1=GF on, 0=GF off
	UINT32 count: 		4;
	UINT32 rsvd2: 		3;
	UINT32 drop: 		1;
}PACK_END dbRateInfo_t1;
#endif
#else
typedef	PACK_START struct dbRateInfo_t1
{
	UINT16	Format:		1; //0 = Legacy format, 1 = Hi-throughput format
	UINT16	ShortGI:	1; //0 = Use standard guard interval,1 = Use short guard interval
	UINT16	Bandwidth:	1; //0 = Use 20 MHz channel,1 = Use 40 MHz channel
	UINT16	RateIDMCS:	6; //= RateID[3:0]; Legacy format,= MCS[5:0]; HT format
	UINT16	AdvCoding:	2; //AdvCoding 0 = No AdvCoding,1 = LDPC,2 = RS,3 = Reserved
	UINT16	AntSelect:	2; //Bitmap to select one of the transmit antenna
	UINT16	ActSubChan:	2; //Active subchannel for 40 MHz mode 00:lower, 01= upper, 10= both on lower and upper
	UINT16  Preambletype:1;//Preambletype 0= Long, 1= Short;
	UINT16  PowerId:     4; // only lower 4 bits used - TRPC power idx 
	UINT16  AntSelect2:  1; // bit 2 of antenna selection field 
	UINT16  reserved:    1;
	UINT16  TxBfFrame:   1; // 1=BF on, 0=BF off (SF3 only)
	UINT16  GreenField:  1; // 1=GF on, 0=GF off (SJay only) 
}PACK_END dbRateInfo_t1;
#endif
*/

#if defined (SOC_W8366) || defined (SOC_W8364) || defined (SOC_W8764)
static UINT16 getPhyRate(dbRateInfo_t *pRateTbl)
{
	UINT8 index = 0;
	UINT8 Nss_11ac=0;	//0:Nss==1, 1:Nss==2, 2:Nss==3
	UINT8 Rate_11ac=0; 

	if (pRateTbl->Format == 1)			
	{
		index = (pRateTbl->Bandwidth << 1) | pRateTbl->ShortGI;
		index += 1;
	}
	else if (pRateTbl->Format == 2)		
	{
		Rate_11ac = pRateTbl->RateIDMCS & 0xf;	//11ac, Rate[3:0]
		Nss_11ac = pRateTbl->RateIDMCS >> 4;	//11ac, Rate[6:4] = NssCode
		index = (Nss_11ac << 1) | pRateTbl->ShortGI;
	}

	if (pRateTbl->Format !=2)
		return PhyRate[pRateTbl->RateIDMCS][index]/2;
	else{
		if(pRateTbl->Bandwidth==0)
			return PhyRate_11ac20M[Rate_11ac][index]/2;
		else if(pRateTbl->Bandwidth==1)
			return PhyRate_11ac40M[Rate_11ac][index]/2;
		else
			return PhyRate_11ac80M[Rate_11ac][index]/2;
	}
	
}

static char getCliModeString(extStaDb_StaInfo_t *pStaInfo)
{
	char buf;

	switch (pStaInfo->ClientMode)
	{
	case BONLY_MODE:
		buf = 'B';
		break;

	case GONLY_MODE:
		buf = 'G';
		break;							

	case NONLY_MODE:
		buf = 'N';
		break;

	case AONLY_MODE:
		buf = 'A';
		break;

	default:
		buf = 0;
		break;

	}	
	return buf; 
}
#endif

static int rateChecked(int rate, int mode)
{
	int i;
	int minRateIndex = 0;
	int maxRateIndex = 0;

	if (mode == AP_MODE_B_ONLY)
	{
		maxRateIndex = 4;	
	} 
	else if (mode == AP_MODE_G_ONLY)
	{
		//minRateIndex = 4;
		maxRateIndex = 12;
	} 
	else if (mode == AP_MODE_A_ONLY)
	{
		minRateIndex = 4;
		maxRateIndex = 12;
	} 
	else if ((mode == AP_MODE_N_ONLY) ||(mode == AP_MODE_5GHZ_N_ONLY))
	{
		if (((rate >= 256) && (rate <= 279)) || (rate == 288))
			return 1;
		else
			maxRateIndex = 12;
	}
#ifdef SOC_W8864	
	else if(mode & AP_MODE_11AC)
	{
	    // SC3 supports only 3 streams
	    if(((rate&0x30)>>4) > 2)
	    {
	        return 0;
	    }
	    //11ac rate is between mcs0 and mcs9
	    if((rate&0xf) > 9)
	    {
	        return 0;
	    }
	    return 1;
	}
#endif	
	else
		return 0;

	for (i=minRateIndex; i<maxRateIndex; i++)
	{
		if (index_to_rate[i] == rate)
			return 1;
	}
	return 0;
}

struct iw_statistics *wlGetStats(struct net_device *dev)
{
#ifdef CLIENT_SUPPORT
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, dev);
	vmacApInfo_t     *vmacSta_p  = wlpptr->vmacSta_p;
	iw_linkInfo_t    *linkInfo_p = NULL;        

	WLDBG_ENTER(DBG_LEVEL_1);
	if (vmacSta_p->VMacEntry.modeOfService == VMAC_MODE_CLNT_INFRA)
	{
		wlFwGetHwStatsForWlStats(dev, &wlpptr->wlpd_p->wStats);
		linkInfo_p = mlmeApiGetStaLinkInfo(dev);
		if (linkInfo_p)
		{
			wlpptr->wlpd_p->wStats.qual.level = - linkInfo_p->wStats.qual.level;
			wlpptr->wlpd_p->wStats.qual.noise = - linkInfo_p->wStats.qual.noise;
			wlpptr->wlpd_p->wStats.qual.qual  = linkInfo_p->wStats.qual.qual;
			wlpptr->wlpd_p->wStats.qual.updated = IW_QUAL_ALL_UPDATED;
			wlpptr->wlpd_p->wStats.status = 0;
		}
	}

	WLDBG_EXIT(DBG_LEVEL_1);
	return &wlpptr->wlpd_p->wStats;
#else
	return NULL;
#endif
}

static int wlconfig_commit(struct net_device *dev,struct iw_request_info *info,char *cwrq, char *extra)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, dev);
#ifdef WFA_TKIP_NEGATIVE
	vmacApInfo_t *vmacSta_p = wlpptr->vmacSta_p;
	MIB_802DOT11 *mib = vmacSta_p->ShadowMib802dot11;
#endif

	WLDBG_ENTER_INFO(DBG_LEVEL_1, "");

#ifdef WFA_TKIP_NEGATIVE
	/* Perform checks on the validity of configuration combinations */
	/* Check the validity of the opmode and security mode combination */
	if ((*(mib->mib_wpaWpa2Mode) & 0x0F) == 1 &&
		(*(mib->mib_ApMode)== AP_MODE_N_ONLY
		|| *(mib->mib_ApMode) == AP_MODE_BandN
		|| *(mib->mib_ApMode) == AP_MODE_GandN
		|| *(mib->mib_ApMode) == AP_MODE_BandGandN
		|| *(mib->mib_ApMode) == AP_MODE_5GHZ_N_ONLY
#ifdef SOC_W8864
		|| *(mib->mib_ApMode) == AP_MODE_2_4GHZ_11AC_MIXED
		|| *(mib->mib_ApMode) == AP_MODE_5GHZ_Nand11AC
#endif		
		|| *(mib->mib_ApMode) == AP_MODE_AandN)) /*WPA-TKIP or WPA-AES mode */
	{
		printk("HT mode not supported when WPA is enabled\n");
		WLSYSLOG(dev, WLSYSLOG_CLASS_ALL, "HT mode not supported when WPA is enabled\n");
		WLSNDEVT(dev, IWEVCUSTOM,(IEEEtypes_MacAddr_t *)&wlpptr->hwData.macAddr[0], "HT mode not supported when WPA is enabled\n");
		return -EINVAL;
	}
	if ((mib->Privacy->PrivInvoked == 1) &&
		(*(mib->mib_ApMode)== AP_MODE_N_ONLY
		|| *(mib->mib_ApMode) == AP_MODE_BandN
		|| *(mib->mib_ApMode) == AP_MODE_GandN
		|| *(mib->mib_ApMode) == AP_MODE_BandGandN
		|| *(mib->mib_ApMode) == AP_MODE_5GHZ_N_ONLY
#ifdef SOC_W8864		
		|| *(mib->mib_ApMode) == AP_MODE_2_4GHZ_11AC_MIXED
		|| *(mib->mib_ApMode) == AP_MODE_5GHZ_Nand11AC
#endif		
		|| *(mib->mib_ApMode) == AP_MODE_AandN))
	{
		printk("HT mode not supported when WEP is enabled\n");
		WLSYSLOG(dev, WLSYSLOG_CLASS_ALL, "HT mode not supported when WEP is enabled\n");
		WLSNDEVT(dev, IWEVCUSTOM,(IEEEtypes_MacAddr_t *)&wlpptr->hwData.macAddr[0], "HT mode not supported when WEP is enabled\n");
		return -EINVAL;
	}
#endif    

	if (dev->flags & IFF_RUNNING)
	{   
		return (wlpptr->wlreset(dev));
	}
	else
	{
		/* If not master device (if master device private wlpptr->master is always NULL). */
		if(wlpptr->master)
		{           
			mib_Update();
		}
		else
		{
			printk("failed wlconfig_commit netdev = %s \n", dev->name);
			return -EPERM;
		}
	}

	WLDBG_EXIT(DBG_LEVEL_1);
	return 0;
}

static int wlget_name(struct net_device *dev, struct iw_request_info *info, char *cwrq, char *extra)
{
	WLDBG_ENTER_INFO(DBG_LEVEL_1, "");

	strcpy(cwrq, "IEEE802.11-DS");

	WLDBG_EXIT(DBG_LEVEL_1);

	return 0;
}

static int wlset_freq(struct net_device *dev, struct iw_request_info *info, struct iw_freq *fwrq, char *extra)
{

	struct wlprivate *priv = NETDEV_PRIV_P(struct wlprivate, dev);
	vmacApInfo_t *vmacSta_p = priv->vmacSta_p;
	MIB_802DOT11 *mib = vmacSta_p->ShadowMib802dot11;
	MIB_PHY_DSSS_TABLE *PhyDSSSTable=mib->PhyDSSSTable;
	UINT8 *mib_extSubCh_p = mib->mib_extSubCh;
	int rc = 0;
	int channel=0;

	WLDBG_ENTER_INFO(DBG_LEVEL_1, "");

	if(priv->master){
		printk("This parameter cannot be set to virtual interface %s, please use %s instead!\n", dev->name, priv->master->name); 
		rc = -EOPNOTSUPP; 
		return rc; 
	} 

	if((fwrq->e == 1) && (fwrq->m >= (int) 2.412e8) &&
		(fwrq->m <= (int) 2.487e8))
	{
		int f = fwrq->m / 100000;
		int c = 0;
		while((c < 14) && (f != frequency_list[c]))
			c++;
		fwrq->e = 0;
		fwrq->m = c + 1;
	}

	if((fwrq->m > 1000) || (fwrq->e > 0))
		rc = -EOPNOTSUPP;
	else
	{
		channel = fwrq->m;

		if(channel)
		{
#ifdef MRVL_DFS
			/*Check if the target channel is a DFS channel and in NOL.
			* If so, do not let the channel to change.
			*/
			if( DfsPresentInNOL(  dev, channel ) )
			{
#ifdef DEBUG_PRINT
				printk("Target channel :%d is already in NOL\n", channel );
#endif
				rc = -EOPNOTSUPP;
				return rc ;
			}
#endif
			if(domainChannelValid(channel, channel <= 14?FREQ_BAND_2DOT4GHZ:FREQ_BAND_5GHZ))
			{
				PhyDSSSTable->CurrChan = channel;
				PhyDSSSTable->powinited = 0;
				/* Set 20MHz BW for channel 14,For ch165 and ch140 so as to avoid overlapping channel pairs */
				if (PhyDSSSTable->CurrChan == 14 ||PhyDSSSTable->CurrChan == 165 ||PhyDSSSTable->CurrChan == 140)
				{
					if ((PhyDSSSTable->Chanflag.ChnlWidth == CH_AUTO_WIDTH) || 
						(PhyDSSSTable->Chanflag.ChnlWidth == CH_40_MHz_WIDTH))
						PhyDSSSTable->Chanflag.ChnlWidth = CH_20_MHz_WIDTH;
				}

				PhyDSSSTable->Chanflag.ExtChnlOffset=EXT_CH_ABOVE_CTRL_CH;
				if(((PhyDSSSTable->Chanflag.ChnlWidth==CH_40_MHz_WIDTH) || (PhyDSSSTable->Chanflag.ChnlWidth==CH_AUTO_WIDTH)))
				{
					switch(PhyDSSSTable->CurrChan)
					{
					case 1:
					case 2:
					case 3:
					case 4:
						PhyDSSSTable->Chanflag.ExtChnlOffset=EXT_CH_ABOVE_CTRL_CH;
						break;
					case 5: /* AutoBW: for CH5 let it be CH5-10, rather than CH5-1 */
						/* Now AutoBW use 5-1 instead of 5-9 for wifi cert convenience*/
						/* if(*mib_extSubCh_p==0)
							PhyDSSSTable->Chanflag.ExtChnlOffset=EXT_CH_ABOVE_CTRL_CH;
						else if(*mib_extSubCh_p==1)
							PhyDSSSTable->Chanflag.ExtChnlOffset=EXT_CH_BELOW_CTRL_CH;
						else if(*mib_extSubCh_p==2)
							PhyDSSSTable->Chanflag.ExtChnlOffset=EXT_CH_ABOVE_CTRL_CH;
						break;*/
					case 6: /* AutoBW: for CH6 let it be CH6-2, rather than CH6-10 */
					case 7: /* AutoBW: for CH7 let it be CH7-3, rather than CH7-11 */
					case 8:
					case 9:
					case 10:
						if(*mib_extSubCh_p==0)
							PhyDSSSTable->Chanflag.ExtChnlOffset=EXT_CH_BELOW_CTRL_CH;
						else if(*mib_extSubCh_p==1)
							PhyDSSSTable->Chanflag.ExtChnlOffset=EXT_CH_BELOW_CTRL_CH;
						else if(*mib_extSubCh_p==2)
							PhyDSSSTable->Chanflag.ExtChnlOffset=EXT_CH_ABOVE_CTRL_CH;
						break;
					case 11:
					case 12:
					case 13:
					case 14:
						PhyDSSSTable->Chanflag.ExtChnlOffset=EXT_CH_BELOW_CTRL_CH;
						break;
						/* for 5G */
					case 36:
						PhyDSSSTable->Chanflag.ExtChnlOffset=EXT_CH_ABOVE_CTRL_CH;
						break;
					case 40:
						PhyDSSSTable->Chanflag.ExtChnlOffset=EXT_CH_BELOW_CTRL_CH;
						break;
					case 44:
						PhyDSSSTable->Chanflag.ExtChnlOffset=EXT_CH_ABOVE_CTRL_CH;
						break;
					case 48:
						PhyDSSSTable->Chanflag.ExtChnlOffset=EXT_CH_BELOW_CTRL_CH;
						break;
					case 52:
						PhyDSSSTable->Chanflag.ExtChnlOffset=EXT_CH_ABOVE_CTRL_CH;
						break;
					case 56:
						PhyDSSSTable->Chanflag.ExtChnlOffset=EXT_CH_BELOW_CTRL_CH;
						break;
					case 60:
						PhyDSSSTable->Chanflag.ExtChnlOffset=EXT_CH_ABOVE_CTRL_CH;
						break;
					case 64:
						PhyDSSSTable->Chanflag.ExtChnlOffset=EXT_CH_BELOW_CTRL_CH;
						break;

					case 100:
						PhyDSSSTable->Chanflag.ExtChnlOffset=EXT_CH_ABOVE_CTRL_CH;
						break;
					case 104:
						PhyDSSSTable->Chanflag.ExtChnlOffset=EXT_CH_BELOW_CTRL_CH;
						break;
					case 108:
						PhyDSSSTable->Chanflag.ExtChnlOffset=EXT_CH_ABOVE_CTRL_CH;
						break;
					case 112:
						PhyDSSSTable->Chanflag.ExtChnlOffset=EXT_CH_BELOW_CTRL_CH;
						break;
#ifdef FCC_15E_INTERIM_PLAN
					case 116:
						PhyDSSSTable->Chanflag.ExtChnlOffset=EXT_CH_BELOW_CTRL_CH;
						break;
#else
					case 116:
						PhyDSSSTable->Chanflag.ExtChnlOffset=EXT_CH_ABOVE_CTRL_CH;
						break;
#endif
					case 120:
						PhyDSSSTable->Chanflag.ExtChnlOffset=EXT_CH_BELOW_CTRL_CH;
						break;
					case 124:
						PhyDSSSTable->Chanflag.ExtChnlOffset=EXT_CH_ABOVE_CTRL_CH;
						break;
					case 128:
						PhyDSSSTable->Chanflag.ExtChnlOffset=EXT_CH_BELOW_CTRL_CH;
						break;
					case 132:
						PhyDSSSTable->Chanflag.ExtChnlOffset=EXT_CH_ABOVE_CTRL_CH;
						break;
					case 136:
						PhyDSSSTable->Chanflag.ExtChnlOffset=EXT_CH_BELOW_CTRL_CH;
						break;
					case 140:
						PhyDSSSTable->Chanflag.ExtChnlOffset=EXT_CH_BELOW_CTRL_CH;
						break;

					case 149:
						PhyDSSSTable->Chanflag.ExtChnlOffset=EXT_CH_ABOVE_CTRL_CH;
						break;
					case 153:
						PhyDSSSTable->Chanflag.ExtChnlOffset=EXT_CH_BELOW_CTRL_CH;
						break;
					case 157:
						PhyDSSSTable->Chanflag.ExtChnlOffset=EXT_CH_ABOVE_CTRL_CH;
						break;
					case 161:
						PhyDSSSTable->Chanflag.ExtChnlOffset=EXT_CH_BELOW_CTRL_CH;
						break;
					case 165:
						PhyDSSSTable->Chanflag.ExtChnlOffset=EXT_CH_BELOW_CTRL_CH;
						break;
					}
				}
				if(PhyDSSSTable->CurrChan<=14)
					PhyDSSSTable->Chanflag.FreqBand=FREQ_BAND_2DOT4GHZ;
				else
					PhyDSSSTable->Chanflag.FreqBand=FREQ_BAND_5GHZ;
			}
			else
			{
				WLDBG_INFO(DBG_LEVEL_1,  "Invalid channel %d for domain %x\n", channel,domainGetDomain());
				rc = -EOPNOTSUPP;
			}
		}
		else
		{
			printk("WARNING: wlset_freq is called with zero channel value!\n");
			rc = -EOPNOTSUPP;
		}
	}
	WLDBG_EXIT(DBG_LEVEL_1);

	return rc;
}

static int wlget_freq(struct net_device *dev, struct iw_request_info *info, struct iw_freq *fwrq, char *extra)
{
	struct wlprivate *priv = NETDEV_PRIV_P(struct wlprivate, dev);
	vmacApInfo_t *vmacSta_p = priv->vmacSta_p;
	MIB_802DOT11 *mib = vmacSta_p->ShadowMib802dot11;
	MIB_PHY_DSSS_TABLE *PhyDSSSTable=mib->PhyDSSSTable;

	WLDBG_ENTER_INFO(DBG_LEVEL_1, "");

	fwrq->m = PhyDSSSTable->CurrChan;
	fwrq->e = 0;

	WLDBG_EXIT(DBG_LEVEL_1);

	return 0;
}

static int wlget_sens(struct net_device *dev,
struct iw_request_info *info,
union iwreq_data *wrqu, char *extra)
{
#ifdef CLIENT_SUPPORT
	struct wlprivate   *wlpptr     = NETDEV_PRIV_P(struct wlprivate, dev);
	vmacApInfo_t       *vmacSta_p  = wlpptr->vmacSta_p;
	iw_linkInfo_t      *linkInfo_p = NULL;

	if (vmacSta_p->VMacEntry.modeOfService == VMAC_MODE_CLNT_INFRA)
	{
		linkInfo_p = mlmeApiGetStaLinkInfo(dev);
		if (linkInfo_p)
		{
			wrqu->sens.fixed = 1;
			wrqu->sens.value = linkInfo_p->wStats.qual.qual;
		}
	}
#endif
	return 0;
}


static int wlget_range(struct net_device *dev, struct iw_request_info *info, struct iw_point *dwrq, char *extra)
{
#ifdef CLIENT_SUPPORT
	struct wlprivate *wlpptr     = NETDEV_PRIV_P(struct wlprivate, dev);
	vmacApInfo_t     *vmacSta_p  = wlpptr->vmacSta_p;
	iw_linkInfo_t    *linkInfo_p = NULL;
	struct iw_range  *range;

	WLDBG_ENTER_INFO(DBG_LEVEL_1, "");
	range = (struct iw_range *) extra;
	dwrq->length = sizeof(struct iw_range);
	memset(range, 0, sizeof(struct iw_range));

	range->we_version_compiled = WIRELESS_EXT;
	range->throughput = 0;
	range->min_nwid   = 0x00;
	range->max_nwid   = 0x1FF;

	if (vmacSta_p->VMacEntry.modeOfService == VMAC_MODE_CLNT_INFRA)
	{
		linkInfo_p = mlmeApiGetStaLinkInfo(dev);
		if (linkInfo_p)
		{
			range->sensitivity = linkInfo_p->max_qual.qual;

			range->max_qual.qual  = linkInfo_p->max_qual.qual;
			range->max_qual.level = linkInfo_p->max_qual.level;
			range->max_qual.noise = linkInfo_p->max_qual.noise;
			range->max_qual.updated = IW_QUAL_ALL_UPDATED;

			range->avg_qual.qual  = linkInfo_p->avg_qual.qual;
			range->avg_qual.level = linkInfo_p->avg_qual.level;
			range->avg_qual.noise = linkInfo_p->avg_qual.noise;
			range->avg_qual.updated = IW_QUAL_ALL_UPDATED;
		}
	}

	WLDBG_EXIT(DBG_LEVEL_1);
#endif
	return 0;
}

static int wlget_stats(struct net_device *dev, struct iw_request_info *info, struct iw_statistics *stats, char *extra)
{
#ifdef CLIENT_SUPPORT
	struct wlprivate *wlpptr     = NETDEV_PRIV_P(struct wlprivate, dev);
	vmacApInfo_t     *vmacSta_p  = wlpptr->vmacSta_p;
	iw_linkInfo_t    *linkInfo_p = NULL;


	WLDBG_ENTER_INFO(DBG_LEVEL_1, "");

	if (vmacSta_p->VMacEntry.modeOfService == VMAC_MODE_CLNT_INFRA)
	{
		linkInfo_p = mlmeApiGetStaLinkInfo(dev);
		memset(stats, 0, sizeof(struct iw_statistics));

		if (linkInfo_p)
		{
			stats->qual.level = linkInfo_p->wStats.qual.level;
			stats->qual.noise = linkInfo_p->wStats.qual.noise;
			stats->qual.qual  = linkInfo_p->wStats.qual.qual;
			stats->qual.updated = IW_QUAL_ALL_UPDATED | IW_QUAL_DBM;
		}

		wlFwGetHwStatsForWlStats(dev, stats);
	}

	WLDBG_EXIT(DBG_LEVEL_1);
#endif
	return 0;
}

static int wlset_scan(struct net_device *dev,
struct iw_request_info *info,
struct iw_param *srq,
	char *extra)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, dev);
	struct iw_request_info infoScan;
	UINT32 wrqu = 0;
	struct scanParam{
		int Param;
		int Value;
	} scanParam;
	scanParam.Param = WL_PARAM_STASCAN;
	scanParam.Value = 0x01;
	wlpptr->cmdFlags = srq->flags;
	wlioctl_priv_wlparam(dev,
		(struct iw_request_info *) &infoScan,
		(void * )&wrqu, (char *) &scanParam);
	return 0;
}

#ifdef CLIENT_SUPPORT
static UINT32 add_IE(UINT8 *buf, UINT32 bufsize, const UINT8 *ie, UINT32 ielen,
					 const char *header, UINT32 header_len)
{
	UINT8 *strEnd;
	int i;

	if (bufsize < header_len)
		return 0;
	strEnd = buf;
	memcpy(strEnd, header, header_len);
	bufsize -= header_len;
	strEnd += header_len;
	for (i = 0; i < ielen && bufsize > 2; i++) {
		strEnd += sprintf(strEnd, "%02x", ie[i]);
		bufsize -= 2;
	}
	return (i == ielen ? strEnd - (UINT8 *)buf : 0);
}
#endif
static int wlget_scan(struct net_device *netdev,
struct iw_request_info *info,
struct iw_point *srq,
	char *extra)
{
#ifdef CLIENT_SUPPORT
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	vmacApInfo_t *vmacSta_p = wlpptr->vmacSta_p;
#define MAX_IE_LENGTH  512
	UINT8                   buffer[MAX_IE_LENGTH];
	char *                  current_ev = extra;
	struct iw_event         iwe;
	char *                  end_buf = current_ev + 4096;
	static const char wpa_header[]  = "WPA_IE = ";
	static const char wps_header[]  = "WPS_IE = ";
	static const char wpa2_header[] = "WPA2_IE = ";
	static char headerGrpCipherTkip[] = "Multicast Cipher = TKIP ";
	static char headerUniCipherTkip[] = "Unicast Cipher = TKIP ";
	static char headerGrpCipherAes[] = "Multicast Cipher = AES ";
	static char headerUniCipherAes[] = "Unicast Cipher = AES ";
	static char headerGrpCipherUnknown[] = "Multicast Cipher = Unknown ";
	static char headerUniCipherUnknown[] = "Unicast Cipher = Unknown ";
	/* Read the entries one by one */

	scanDescptHdr_t *curDescpt_p = NULL;
	IEEEtypes_SsIdElement_t *ssidIE_p;
	IEEEtypes_DsParamSet_t *dsPSetIE_p;
	IEEEtypes_SuppRatesElement_t       *PeerSupportedRates_p    = NULL;
	IEEEtypes_ExtSuppRatesElement_t    *PeerExtSupportedRates_p = NULL;
	IEEEtypes_HT_Element_t             *pHT     = NULL;
	IEEEtypes_Add_HT_Element_t         *pHTAdd  = NULL;
	IEEEtypes_Generic_HT_Element_t     *pHTGen  = NULL;
	UINT32 LegacyRateBitMap         = 0;
	IEEEtypes_RSN_IE_t *RSN_p       = NULL;
	IEEEtypes_RSN_IE_t *RSNWps_p    = NULL;
	WSC_HeaderIE_t     *WPS_p       = NULL;
	IEEEtypes_RSN_IE_WPA2_t *wpa2IE_p = NULL;
	UINT8 scannedChannel            = 0;
	UINT16  parsedLen               = 0;
	UINT8   scannedSSID[33];
	UINT8   i=0, j = 0;
	UINT8   apType[10];
	UINT8   mdcnt;
	BOOLEAN apGonly = FALSE;

	if (!vmacSta_p->busyScanning)
	{ 
		for(i=0; i < tmpNumScanDesc[vmacSta_p->VMacEntry.phyHwMacIndx]; i++)
		{
			curDescpt_p = (scanDescptHdr_t *)(&tmpScanResults[vmacSta_p->VMacEntry.phyHwMacIndx][0] + parsedLen);

			iwe.cmd = SIOCGIWAP;
			iwe.u.ap_addr.sa_family = ARPHRD_ETHER;        
			memcpy(iwe.u.ap_addr.sa_data, curDescpt_p->bssId, ETH_ALEN);
#if LINUX_VERSION_CODE >=KERNEL_VERSION(2,6,27)
			current_ev = iwe_stream_add_event(info, current_ev, end_buf, &iwe, IW_EV_ADDR_LEN);
#else
			current_ev = iwe_stream_add_event(current_ev, end_buf, &iwe, IW_EV_ADDR_LEN);
#endif
			memset(buffer, 0, MAX_IE_LENGTH);
			memset(&scannedSSID[0], 0, sizeof(scannedSSID));
			memset(&apType[0], 0, sizeof(apType));
			sprintf(&apType[0], "Mode = ");
			mdcnt = strlen(apType);;
			scannedChannel = 0;
			apGonly = FALSE;
			/* Add the SSID */
			if ((ssidIE_p = (IEEEtypes_SsIdElement_t *)smeParseIeType(SSID,
				(((UINT8 *)curDescpt_p) +sizeof(scanDescptHdr_t)),
				curDescpt_p->length + sizeof(curDescpt_p->length) - sizeof(scanDescptHdr_t))) != NULL)
			{
				memcpy(&scannedSSID[0], &ssidIE_p->SsId[0], ssidIE_p->Len);                
				iwe.u.data.length = le16_to_cpu(ssidIE_p->Len);
				if (iwe.u.data.length > 32)
					iwe.u.data.length = 32;
				iwe.cmd = SIOCGIWESSID;
				iwe.u.data.flags = 1;
#if LINUX_VERSION_CODE >=KERNEL_VERSION(2,6,27)
				current_ev = iwe_stream_add_point(info, current_ev, end_buf, &iwe, &ssidIE_p->SsId[0]);
#else
				current_ev = iwe_stream_add_point(current_ev, end_buf, &iwe, &ssidIE_p->SsId[0]);
#endif

			}

			/* Add mode */            
			iwe.cmd = SIOCGIWMODE;
			if (curDescpt_p->CapInfo.Ess || curDescpt_p->CapInfo.Ibss) 
			{
				if (curDescpt_p->CapInfo.Ess)
					iwe.u.mode = IW_MODE_MASTER;
				else
					iwe.u.mode = IW_MODE_ADHOC;
#if LINUX_VERSION_CODE >=KERNEL_VERSION(2,6,27)
				current_ev = iwe_stream_add_event(info, current_ev, end_buf, &iwe, IW_EV_UINT_LEN);
#else
				current_ev = iwe_stream_add_event(current_ev, end_buf, &iwe, IW_EV_UINT_LEN);
#endif
			}

			if ((dsPSetIE_p = (IEEEtypes_DsParamSet_t *)smeParseIeType(DS_PARAM_SET,
				(((UINT8 *)curDescpt_p) +sizeof(scanDescptHdr_t)),
				curDescpt_p->length + sizeof(curDescpt_p->length) - sizeof(scanDescptHdr_t))) != NULL)
			{
				scannedChannel = dsPSetIE_p->CurrentChan; 
				/* Add frequency */
				iwe.cmd = SIOCGIWFREQ;
				iwe.u.freq.m = dsPSetIE_p->CurrentChan;
				iwe.u.freq.e = 0;
#if LINUX_VERSION_CODE >=KERNEL_VERSION(2,6,27)
				current_ev = iwe_stream_add_event(info, current_ev, end_buf,
#else
				current_ev = iwe_stream_add_event(current_ev, end_buf,
#endif
					&iwe, IW_EV_FREQ_LEN);
			}

			/* Add quality statistics */
			iwe.cmd = IWEVQUAL;
			iwe.u.qual.updated = 0x10; 
			iwe.u.qual.level = (__u8) le16_to_cpu(-curDescpt_p->rssi);
			iwe.u.qual.noise = (__u8) le16_to_cpu(-0x95);
			if (iwe.u.qual.level > iwe.u.qual.noise)
				iwe.u.qual.qual = iwe.u.qual.level - iwe.u.qual.noise;
			else
				iwe.u.qual.qual = 0;
#if LINUX_VERSION_CODE >=KERNEL_VERSION(2,6,27)
			current_ev = iwe_stream_add_event(info, current_ev, end_buf, &iwe, IW_EV_QUAL_LEN);
#else
			current_ev = iwe_stream_add_event(current_ev, end_buf, &iwe, IW_EV_QUAL_LEN);
#endif

			/* Add encryption capability */
			iwe.cmd = SIOCGIWENCODE;
			if (curDescpt_p->CapInfo.Privacy)
				iwe.u.data.flags = IW_ENCODE_ENABLED | IW_ENCODE_NOKEY;
			else
				iwe.u.data.flags = IW_ENCODE_DISABLED;
			iwe.u.data.length = 0;
#if LINUX_VERSION_CODE >=KERNEL_VERSION(2,6,27)
			current_ev = iwe_stream_add_point(info, current_ev, end_buf, &iwe, &scannedSSID[0]);
#else
			current_ev = iwe_stream_add_point(current_ev, end_buf, &iwe, &scannedSSID[0]);
#endif

			PeerSupportedRates_p = (IEEEtypes_SuppRatesElement_t*) smeParseIeType(SUPPORTED_RATES,
				(((UINT8 *)curDescpt_p) +sizeof(scanDescptHdr_t)),
				curDescpt_p->length + sizeof(curDescpt_p->length) - sizeof(scanDescptHdr_t));

			PeerExtSupportedRates_p = (IEEEtypes_ExtSuppRatesElement_t *) smeParseIeType(EXT_SUPPORTED_RATES,
				(((UINT8 *)curDescpt_p) + sizeof(scanDescptHdr_t)),
				curDescpt_p->length + sizeof(curDescpt_p->length) - sizeof(scanDescptHdr_t));

			/* Add rates */
			iwe.cmd = SIOCGIWRATE;
			iwe.u.bitrate.fixed = iwe.u.bitrate.disabled = 0;
			if (PeerSupportedRates_p)
			{
				char *  current_val = current_ev + IW_EV_LCP_LEN;
				for (j = 0; j < PeerSupportedRates_p->Len; j ++) 
				{                    
					/* Bit rate given in 500 kb/s units (+ 0x80) */
					iwe.u.bitrate.value = ((PeerSupportedRates_p->Rates[j] & 0x7f) * 500000);
#if LINUX_VERSION_CODE >=KERNEL_VERSION(2,6,27)
					current_val = iwe_stream_add_value(info, current_ev, current_val,
#else
					current_val = iwe_stream_add_value(current_ev, current_val,
#endif
						end_buf, &iwe, IW_EV_PARAM_LEN);
				}
				/* Check if we added any event */
				if ((current_val - current_ev) > IW_EV_LCP_LEN)
					current_ev = current_val;

			}
			if (PeerExtSupportedRates_p)
			{
				char *  current_val = current_ev + IW_EV_LCP_LEN;
				for (j = 0; j < PeerExtSupportedRates_p->Len; j ++) 
				{                    
					/* Bit rate given in 500 kb/s units (+ 0x80) */
					iwe.u.bitrate.value = ((PeerExtSupportedRates_p->Rates[j] & 0x7f) * 500000);
#if LINUX_VERSION_CODE >=KERNEL_VERSION(2,6,27)
					current_val = iwe_stream_add_value(info, current_ev, current_val,
#else
					current_val = iwe_stream_add_value(current_ev, current_val,
#endif
						end_buf, &iwe, IW_EV_PARAM_LEN);
				}
				/* Check if we added any event */
				if ((current_val - current_ev) > IW_EV_LCP_LEN)
					current_ev = current_val;

			}

			/* Add WPA. */
			if ((RSN_p = linkMgtParseWpaIe((((UINT8 *)curDescpt_p) +sizeof(scanDescptHdr_t)),
				curDescpt_p->length + sizeof(curDescpt_p->length) - sizeof(scanDescptHdr_t))))
			{
				memset(&iwe, 0, sizeof(iwe));
				iwe.cmd = IWEVCUSTOM;
				iwe.u.data.length = add_IE(buffer, sizeof(buffer), (UINT8 *)RSN_p, RSN_p->Len + 2,
					wpa_header, sizeof(wpa_header) - 1);
				if (iwe.u.data.length != 0) 
				{
#if LINUX_VERSION_CODE >=KERNEL_VERSION(2,6,27)
					current_ev = iwe_stream_add_point(info, current_ev, end_buf, &iwe, buffer);
#else
					current_ev = iwe_stream_add_point(current_ev, end_buf, &iwe, buffer);
#endif
				}

				memset(&iwe, 0, sizeof(iwe));
				iwe.cmd = IWEVCUSTOM;
				if (RSN_p->GrpKeyCipher[3] == RSN_TKIP_ID)
				{
					iwe.u.data.length = sizeof(headerGrpCipherTkip);
#if LINUX_VERSION_CODE >=KERNEL_VERSION(2,6,27)
					current_ev = iwe_stream_add_point(info, current_ev, end_buf, &iwe, headerGrpCipherTkip);
#else
					current_ev = iwe_stream_add_point(current_ev, end_buf, &iwe, headerGrpCipherTkip);
#endif
				}
				else if (RSN_p->GrpKeyCipher[3] == RSN_AES_ID)
				{
					iwe.u.data.length = sizeof(headerGrpCipherAes);
#if LINUX_VERSION_CODE >=KERNEL_VERSION(2,6,27)
					current_ev = iwe_stream_add_point(info, current_ev, end_buf, &iwe, headerGrpCipherAes);
#else
					current_ev = iwe_stream_add_point(current_ev, end_buf, &iwe, headerGrpCipherAes);
#endif
				}
				else
				{
					iwe.u.data.length = sizeof(headerGrpCipherUnknown);
#if LINUX_VERSION_CODE >=KERNEL_VERSION(2,6,27)
					current_ev = iwe_stream_add_point(info, current_ev, end_buf, &iwe, headerGrpCipherUnknown);
#else
					current_ev = iwe_stream_add_point(current_ev, end_buf, &iwe, headerGrpCipherUnknown);
#endif
				}

				memset(&iwe, 0, sizeof(iwe));
				iwe.cmd = IWEVCUSTOM;
				if (RSN_p->PwsKeyCipherList[3] == RSN_TKIP_ID)
				{
					iwe.u.data.length = sizeof(headerUniCipherAes);
#if LINUX_VERSION_CODE >=KERNEL_VERSION(2,6,27)
					current_ev = iwe_stream_add_point(info, current_ev, end_buf, &iwe, headerUniCipherTkip);
#else
					current_ev = iwe_stream_add_point(current_ev, end_buf, &iwe, headerUniCipherTkip);
#endif
				}
				else if (RSN_p->PwsKeyCipherList[3] == RSN_AES_ID)
				{
					iwe.u.data.length = sizeof(headerUniCipherAes);
#if LINUX_VERSION_CODE >=KERNEL_VERSION(2,6,27)
					current_ev = iwe_stream_add_point(info, current_ev, end_buf, &iwe, headerUniCipherAes);
#else
					current_ev = iwe_stream_add_point(current_ev, end_buf, &iwe, headerUniCipherAes);
#endif
				}
				else
				{
					iwe.u.data.length = sizeof(headerUniCipherUnknown);
#if LINUX_VERSION_CODE >=KERNEL_VERSION(2,6,27)
					current_ev = iwe_stream_add_point(info, current_ev, end_buf, &iwe, headerUniCipherUnknown);
#else
					current_ev = iwe_stream_add_point(current_ev, end_buf, &iwe, headerUniCipherUnknown);
#endif
				}
			}
			/* Add WPA */
			if ((RSNWps_p = linkMgtParseWpsIe((((UINT8 *)curDescpt_p) +sizeof(scanDescptHdr_t)),
				curDescpt_p->length + sizeof(curDescpt_p->length) - sizeof(scanDescptHdr_t))))
			{   
				UINT16 DevPasswdId = 0;
				memset(&iwe, 0, sizeof(iwe));
				iwe.cmd = IWEVCUSTOM;
				iwe.u.data.length = add_IE(buffer, sizeof(buffer), (UINT8 *)RSNWps_p, RSNWps_p->Len + 2,
					wps_header, sizeof(wps_header) - 1);
				WPS_p = linkMgtParseWpsInfo(0x1012, (UINT8 *)RSNWps_p, curDescpt_p->length + sizeof(curDescpt_p->length) - sizeof(scanDescptHdr_t));
				/* Do nothing with this for now. Maybe needed later to identify PIN/PBC. */
				DevPasswdId = *((UINT16 *) ((UINT8 *)WPS_p + sizeof(WSC_HeaderIE_t)));
				if (iwe.u.data.length != 0) 
				{
#if LINUX_VERSION_CODE >=KERNEL_VERSION(2,6,27)
					current_ev = iwe_stream_add_point(info, current_ev, end_buf, &iwe, buffer);
#else
					current_ev = iwe_stream_add_point(current_ev, end_buf, &iwe, buffer);
#endif
				}
			}
			/* Add WPA2 */
			if ((wpa2IE_p = (IEEEtypes_RSN_IE_WPA2_t *) smeParseIeType(RSN_IEWPA2, 
				(((UINT8 *)curDescpt_p) +sizeof(scanDescptHdr_t)),
				curDescpt_p->length + sizeof(curDescpt_p->length) - sizeof(scanDescptHdr_t))))
			{

				memset(&iwe, 0, sizeof(iwe));
				iwe.cmd = IWEVCUSTOM;
				iwe.u.data.length = add_IE(buffer, sizeof(buffer), (UINT8 *)wpa2IE_p, wpa2IE_p->Len + 2,
					wpa2_header, sizeof(wpa2_header) - 1);
				if (iwe.u.data.length != 0) 
				{
#if LINUX_VERSION_CODE >=KERNEL_VERSION(2,6,27)
					current_ev = iwe_stream_add_point(info, current_ev, end_buf, &iwe, buffer);
#else
					current_ev = iwe_stream_add_point(current_ev, end_buf, &iwe, buffer);
#endif
				}

				memset(&iwe, 0, sizeof(iwe));
				iwe.cmd = IWEVCUSTOM;                
				if (wpa2IE_p->GrpKeyCipher[3] == RSN_TKIP_ID)
				{
					iwe.u.data.length = sizeof(headerGrpCipherTkip);
#if LINUX_VERSION_CODE >=KERNEL_VERSION(2,6,27)
					current_ev = iwe_stream_add_point(info, current_ev, end_buf, &iwe, headerGrpCipherTkip);
#else
					current_ev = iwe_stream_add_point(current_ev, end_buf, &iwe, headerGrpCipherTkip);
#endif
				}
				else if (wpa2IE_p->GrpKeyCipher[3] == RSN_AES_ID)
				{
					iwe.u.data.length = sizeof(headerGrpCipherAes);
#if LINUX_VERSION_CODE >=KERNEL_VERSION(2,6,27)
					current_ev = iwe_stream_add_point(info, current_ev, end_buf, &iwe, headerGrpCipherAes);
#else
					current_ev = iwe_stream_add_point(current_ev, end_buf, &iwe, headerGrpCipherAes);
#endif
				}
				else
				{
					iwe.u.data.length = sizeof(headerGrpCipherUnknown);
#if LINUX_VERSION_CODE >=KERNEL_VERSION(2,6,27)
					current_ev = iwe_stream_add_point(info, current_ev, end_buf, &iwe, headerGrpCipherUnknown);
#else
					current_ev = iwe_stream_add_point(current_ev, end_buf, &iwe, headerGrpCipherUnknown);
#endif
				}

				memset(&iwe, 0, sizeof(iwe));
				iwe.cmd = IWEVCUSTOM;                
				if (wpa2IE_p->PwsKeyCipherList[3] == RSN_TKIP_ID)
				{
					iwe.u.data.length = sizeof(headerUniCipherAes);
#if LINUX_VERSION_CODE >=KERNEL_VERSION(2,6,27)
					current_ev = iwe_stream_add_point(info, current_ev, end_buf, &iwe, headerUniCipherTkip);
#else
					current_ev = iwe_stream_add_point(current_ev, end_buf, &iwe, headerUniCipherTkip);
#endif
				}
				else if (wpa2IE_p->PwsKeyCipherList[3] == RSN_AES_ID)
				{
					iwe.u.data.length = sizeof(headerUniCipherAes);
#if LINUX_VERSION_CODE >=KERNEL_VERSION(2,6,27)
					current_ev = iwe_stream_add_point(info, current_ev, end_buf, &iwe, headerUniCipherAes);
#else
					current_ev = iwe_stream_add_point(current_ev, end_buf, &iwe, headerUniCipherAes);
#endif
				}
				else
				{
					iwe.u.data.length = sizeof(headerUniCipherUnknown);
#if LINUX_VERSION_CODE >=KERNEL_VERSION(2,6,27)
					current_ev = iwe_stream_add_point(info, current_ev, end_buf, &iwe, headerUniCipherUnknown);
#else
					current_ev = iwe_stream_add_point(current_ev, end_buf, &iwe, headerUniCipherUnknown);
#endif
				}
			}

			LegacyRateBitMap = GetAssocRespLegacyRateBitMap(PeerSupportedRates_p, PeerExtSupportedRates_p);

			if (scannedChannel <=14)
			{
				if (PeerSupportedRates_p)
				{
					int j;
					for (j=0; (j < PeerSupportedRates_p->Len) && !apGonly ; j++)
					{
						/* Only look for 6 Mbps as basic rate - consider this to be G only. */
						if (PeerSupportedRates_p->Rates[j] == 0x8c)
						{
							sprintf(&apType[mdcnt++], "G");
							apGonly = TRUE;
						}                            
					}
				}
				if (!apGonly)
				{
					if (LegacyRateBitMap & 0x0f)
						sprintf(&apType[mdcnt++], "B");
					if (PeerSupportedRates_p && PeerExtSupportedRates_p)
						sprintf(&apType[mdcnt++], "G");
				}
			}
			else
			{
				if (LegacyRateBitMap & 0x1fe0)
					sprintf(&apType[mdcnt++], "A"); 
			}

			pHT = (IEEEtypes_HT_Element_t *)smeParseIeType(HT, 
				(((UINT8 *)curDescpt_p) +sizeof(scanDescptHdr_t)),
				curDescpt_p->length + sizeof(curDescpt_p->length) - sizeof(scanDescptHdr_t));

			pHTAdd = (IEEEtypes_Add_HT_Element_t *) smeParseIeType(ADD_HT, 
				(((UINT8 *)curDescpt_p) +sizeof(scanDescptHdr_t)),
				curDescpt_p->length + sizeof(curDescpt_p->length) - sizeof(scanDescptHdr_t));
			// If cannot find HT element then look for High Throughput elements using PROPRIETARY_IE.
			if (pHT == NULL)
			{
				pHTGen = linkMgtParseHTGenIe((((UINT8 *)curDescpt_p) +sizeof(scanDescptHdr_t)),
					curDescpt_p->length + sizeof(curDescpt_p->length) - sizeof(scanDescptHdr_t));
			}

			if (pHT || pHTGen)
			{
				sprintf(&apType[mdcnt++], "N");
			}

			memset(&iwe, 0, sizeof(iwe));
			iwe.cmd = IWEVCUSTOM;
			iwe.u.data.length = sizeof(apType);
#if LINUX_VERSION_CODE >=KERNEL_VERSION(2,6,27)
			current_ev = iwe_stream_add_point(info, current_ev, end_buf, &iwe, apType);
#else
			current_ev = iwe_stream_add_point(current_ev, end_buf, &iwe, apType);
#endif
			parsedLen += curDescpt_p->length + sizeof(curDescpt_p->length);
		}
		srq->length = current_ev - extra;
		srq->flags  = wlpptr->cmdFlags;
		return 0;
	}
	else
	{
		printk(".");
		mdelay(60);
		return -EAGAIN;
	}
#endif
	return 0;
}

static int wlset_essid(struct net_device *dev, struct iw_request_info *info, struct iw_point *dwrq, char *extra)
{
	struct wlprivate	*priv = NETDEV_PRIV_P(struct wlprivate, dev);
	vmacApInfo_t *vmacSta_p = priv->vmacSta_p;
	MIB_802DOT11 *mib = vmacSta_p->ShadowMib802dot11;
	int rc = 0;

	WLDBG_ENTER_INFO(DBG_LEVEL_1, "");

	memset(&(mib->StationConfig->DesiredSsId[0]),0,32);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
	memcpy(&(mib->StationConfig->DesiredSsId[0]), extra,	((UINT32)dwrq->length));
#else
	memcpy(&(mib->StationConfig->DesiredSsId[0]), extra,	((UINT32)dwrq->length - 1));
#endif
	WLDBG_EXIT(DBG_LEVEL_1);


	return rc;
}

static int wlget_essid(struct net_device *dev,
struct iw_request_info *info,
struct iw_point *dwrq, char *extra)
{
	struct wlprivate *priv = NETDEV_PRIV_P(struct wlprivate, dev);
	vmacApInfo_t *vmacSta_p = priv->vmacSta_p;
	MIB_802DOT11 *mib = vmacSta_p->ShadowMib802dot11;
	ULONG 	SsidLen = 0;

	WLDBG_ENTER_INFO(DBG_LEVEL_1, "");

	SsidLen = strlen(&(mib->StationConfig->DesiredSsId[0]));
	memcpy(extra, &(mib->StationConfig->DesiredSsId[0]), SsidLen);

	dwrq->length = SsidLen;
	dwrq->flags = 1;

	WLDBG_EXIT(DBG_LEVEL_1);

	return 0;
}

static int wlset_bssid(struct net_device *dev, struct iw_request_info *info, struct sockaddr *ap_addr, char *extra)
{
	struct wlprivate *priv = NETDEV_PRIV_P(struct wlprivate, dev);
	vmacApInfo_t *vmacSta_p = priv->vmacSta_p;
	MIB_802DOT11 *mib = vmacSta_p->ShadowMib802dot11;
	int rc = 0;

	memcpy(&(mib->StationConfig->DesiredBSSId[0]), ap_addr->sa_data, MAC_ADDR_SIZE);

	WLDBG_EXIT(DBG_LEVEL_1);
	return rc;
}

static int wlset_rts(struct net_device *dev,
struct iw_request_info *info,
struct iw_param *vwrq, char *extra)
{
	struct wlprivate *priv = NETDEV_PRIV_P(struct wlprivate, dev);
	vmacApInfo_t *vmacSta_p = priv->vmacSta_p;
	MIB_802DOT11 *mib = vmacSta_p->ShadowMib802dot11;
	int rc = 0;

	WLDBG_ENTER (DBG_LEVEL_1 );
	if(priv->master){
		printk("This parameter cannot be set to virtual interface %s, please use %s instead!\n", dev->name, priv->master->name); 
		rc = -EOPNOTSUPP; 
		return rc; 
	} 
    /* turn off RTS/CTS for 11ac taffic when rts threshold is set to 0. 
       The actual rts threshold will be still set to 2437 */
    if(vwrq->value == 0)
    {
        wlFwSetRTSThreshold(dev, 0);
    }

	if((vwrq->value < 255) || (vwrq->value > 2346)) 
		vwrq->value = 2347;
	*(mib->mib_RtsThresh) = vwrq->value;
	WLDBG_EXIT(DBG_LEVEL_1);

	return rc;
}

static int wlget_rts(struct net_device *dev,
struct iw_request_info *info,
struct iw_param *vwrq, char *extra)
{
	struct wlprivate *priv = NETDEV_PRIV_P(struct wlprivate, dev);
	vmacApInfo_t *vmacSta_p = priv->vmacSta_p;
	MIB_802DOT11 *mib = vmacSta_p->ShadowMib802dot11;

	WLDBG_ENTER_INFO(DBG_LEVEL_1, "");
	if (*(mib->mib_RtsThresh) > 2346)
		vwrq->disabled = 1;
	else
	{
		vwrq->disabled = 0;
		vwrq->fixed = 1;
		vwrq->value = *(mib->mib_RtsThresh);
	}

	WLDBG_EXIT(DBG_LEVEL_1);

	return 0;
}

static int wlget_frag(struct net_device *dev,
struct iw_request_info *info,
struct iw_param *vwrq, char *extra)
{
	WLDBG_ENTER_INFO(DBG_LEVEL_1, "");

	vwrq->disabled = 1;

	WLDBG_EXIT(DBG_LEVEL_1);

	return 0;
}

static int wlget_wap(struct net_device *dev,
struct iw_request_info *info,
struct sockaddr *awrq, char *extra)
{
	WLDBG_ENTER_INFO(DBG_LEVEL_1, "");

	memcpy(awrq->sa_data, dev->dev_addr, 6);
	WLDBG_EXIT(DBG_LEVEL_1);

	return 0;
}

static int wlset_encode(struct net_device *dev,
struct iw_request_info *info,
struct iw_point *dwrq, char *extra)
{
	struct wlprivate	*priv = NETDEV_PRIV_P(struct wlprivate, dev);
	vmacApInfo_t *vmacSta_p = priv->vmacSta_p;
	MIB_802DOT11 *mib = vmacSta_p->ShadowMib802dot11;
	int rc = 0;

	WLDBG_INFO(DBG_LEVEL_1, "wlset_encode: enter\n");
	if (dwrq->flags & IW_ENCODE_DISABLED)
	{
		WLDBG_INFO(DBG_LEVEL_1, "wlset_encode: IW_ENCODE_DISABLED\n");

		mib->Privacy->RSNEnabled = 0;
		mib->RSNConfigWPA2->WPA2Enabled = 0;
		mib->RSNConfigWPA2->WPA2OnlyEnabled = 0;

		mib->AuthAlg->Enable = 0;
		mib->StationConfig->PrivOption = 0;
		mib->Privacy->PrivInvoked = 0;
		mib->AuthAlg->Type = 0;
		WL_FUN_SetAuthType((void *) priv, 0);
		if (WL_FUN_SetPrivacyOption((void *)priv, 0))
		{
		}else
			rc = -EIO;
	}else
	{
		WLDBG_INFO(DBG_LEVEL_1, "wlset_encode: IW_ENCODE_ENABLED\n");

		mib->Privacy->RSNEnabled = 0;
		mib->RSNConfigWPA2->WPA2Enabled = 0;
		mib->RSNConfigWPA2->WPA2OnlyEnabled = 0;

		mib->AuthAlg->Enable = 1;
		mib->StationConfig->PrivOption = 1;
		mib->Privacy->PrivInvoked = 1;
		if (WL_FUN_SetPrivacyOption((void *)priv, 1))
		{
		}else
			rc = -EIO;

		if(dwrq->flags & IW_ENCODE_OPEN)
		{
			int index = (dwrq->flags & IW_ENCODE_INDEX) - 1;

			if((index < 0) || (index >3))
				*(mib->mib_defaultkeyindex) = index = 0;
			else
				*(mib->mib_defaultkeyindex) = index;

			WLDBG_INFO(DBG_LEVEL_1, "wlset_encode: IW_ENCODE_OPEN\n");
			mib->AuthAlg->Type = 0;
			WL_FUN_SetAuthType((void *) priv, 0);
		}
		if(dwrq->flags & IW_ENCODE_RESTRICTED)
		{
			int index = (dwrq->flags & IW_ENCODE_INDEX) - 1;

			if((index < 0) || (index >3))
				*(mib->mib_defaultkeyindex) = index = 0;
			else
				*(mib->mib_defaultkeyindex) = index;

			WLDBG_INFO(DBG_LEVEL_1, "wlset_encode: IW_ENCODE_RESTRICTED\n");
			mib->AuthAlg->Type = 1;
			WL_FUN_SetAuthType((void *) priv, 1);
		}
		if(dwrq->length >0)
		{
			int index = (dwrq->flags & IW_ENCODE_INDEX) - 1;
			int wep_type = 1;
			UCHAR tmpWEPKey[16];

			if(dwrq->length > 13)
				return -EINVAL;

			if((index < 0) || (index >3))
				*(mib->mib_defaultkeyindex) = index = 0;
			else
				*(mib->mib_defaultkeyindex) = index;

			if(dwrq->length == 5)
			{
				wep_type =1;
				mib->WepDefaultKeys[index].WepType = wep_type;

			}
			if(dwrq->length == 13)
			{
				wep_type = 2;
				mib->WepDefaultKeys[index].WepType = wep_type;
			}
			memset(mib->WepDefaultKeys[index].WepDefaultKeyValue, 0, 13);
			memcpy(tmpWEPKey, extra, dwrq->length);
			memcpy(mib->WepDefaultKeys[index].WepDefaultKeyValue, tmpWEPKey, dwrq->length);
			if (WL_FUN_SetWEPKey((void *)priv, index, wep_type, tmpWEPKey))
			{
				WLDBG_INFO(DBG_LEVEL_1, "wlset_encode: WL_FUN_SetWEPKey TRUE length = %d index = %d type = %d\n", dwrq->length, index, wep_type);
				WLDBG_INFO(DBG_LEVEL_1, "wep key = %x %x %x %x %x %x %x %x %x %x %x %x %x \n",
					mib->WepDefaultKeys[index].WepDefaultKeyValue[0],
					mib->WepDefaultKeys[index].WepDefaultKeyValue[1],
					mib->WepDefaultKeys[index].WepDefaultKeyValue[2],
					mib->WepDefaultKeys[index].WepDefaultKeyValue[3],
					mib->WepDefaultKeys[index].WepDefaultKeyValue[4],
					mib->WepDefaultKeys[index].WepDefaultKeyValue[5],
					mib->WepDefaultKeys[index].WepDefaultKeyValue[6],
					mib->WepDefaultKeys[index].WepDefaultKeyValue[7],
					mib->WepDefaultKeys[index].WepDefaultKeyValue[8],
					mib->WepDefaultKeys[index].WepDefaultKeyValue[9],
					mib->WepDefaultKeys[index].WepDefaultKeyValue[10],
					mib->WepDefaultKeys[index].WepDefaultKeyValue[11],
					mib->WepDefaultKeys[index].WepDefaultKeyValue[12]);

			}else
				rc = -EIO;
		}
	}

	return rc;
}

static int wlget_encode(struct net_device *dev,
struct iw_request_info *info,
struct iw_point *dwrq, char *extra)
{
	struct wlprivate	*priv = NETDEV_PRIV_P(struct wlprivate, dev);
	vmacApInfo_t *vmacSta_p = priv->vmacSta_p;
	MIB_802DOT11 *mib = vmacSta_p->ShadowMib802dot11;
	int rc = 0;
	int index = (dwrq->flags & IW_ENCODE_INDEX) - 1;

	WLDBG_INFO(DBG_LEVEL_1, "wlget_encode: enter\n");
	if (mib->Privacy->PrivInvoked )
	{
		if (mib->AuthAlg->Type )
			dwrq->flags = IW_ENCODE_RESTRICTED;
		else
			dwrq->flags = IW_ENCODE_OPEN;
	}
	else
	{
		dwrq->flags = IW_ENCODE_DISABLED;
	}

	if (index < 0 ||index >3)
		index = 0;
	//to show key
	memcpy(extra, mib->WepDefaultKeys[index].WepDefaultKeyValue, 16);
	//not show key
	//dwrq->flags |= IW_ENCODE_NOKEY;
	//memset(extra, 0, 16);
	if (mib->WepDefaultKeys[index].WepType ==1)
		dwrq->length =5;
	if (mib->WepDefaultKeys[index].WepType ==2)
		dwrq->length = 13;

	if (dwrq->length > 16) {
		dwrq->length=0;
	}
	return rc;
}
typedef struct param_applicable_t
{
	UINT16 command;
	UINT16   applicable;
}param_applicable;
static param_applicable priv_wlparam[] =
{
	{WL_PARAM_AUTHTYPE, 0}, 
	{WL_PARAM_BAND, 1},
	{WL_PARAM_REGIONCODE, 1},
	{WL_PARAM_HIDESSID, 0},
	{WL_PARAM_PREAMBLE, 0},
	{WL_PARAM_GPROTECT, 1},
	{WL_PARAM_BEACON, 1},
	{WL_PARAM_DTIM, 0},
	{WL_PARAM_FIXRATE, 1},
	{WL_PARAM_ANTENNA, 1},
	{WL_PARAM_WPAWPA2MODE, 0},
#ifdef MRVL_WAPI
	{WL_PARAM_WAPIMODE, 0},
#endif
	{WL_PARAM_AUTHSUITE, 0},
	{WL_PARAM_GROUPREKEYTIME, 0},
	{WL_PARAM_WMM, 1},
	{WL_PARAM_WMMACKPOLICY, 0},
	{WL_PARAM_FILTER, 0},
	{WL_PARAM_INTRABSS, 0},
	{WL_PARAM_AMSDU, 0},
	{WL_PARAM_HTBANDWIDTH, 1},
	{WL_PARAM_GUARDINTERVAL, 1},
	{WL_PARAM_EXTSUBCH, 1},
	{WL_PARAM_HTPROTECT, 1},
	{WL_PARAM_GETFWSTAT, 1},
	{WL_PARAM_AGINGTIME, 1},
	{WL_PARAM_AUTOCHANNEL, 1},
	{WL_PARAM_AMPDUFACTOR, 0},
	{WL_PARAM_AMPDUDENSITY, 0},
	{WL_PARAM_CARDDEVINFO, 0},
	{WL_PARAM_INTEROP, 0},
	{WL_PARAM_OPTLEVEL, 0},
	{WL_PARAM_REGIONPWR, 1},
	{WL_PARAM_ADAPTMODE, 0},
	{WL_PARAM_SETKEYS, 0},
	{WL_PARAM_DELKEYS, 0},
	{WL_PARAM_MLME_REQ, 0},
	{WL_PARAM_COUNTERMEASURES, 0},
	{WL_PARAM_CSADAPTMODE , 0},
	{WL_PARAM_DELWEPKEY, 0},
	{WL_PARAM_WDSMODE, 0},
	{WL_PARAM_STRICTWEPSHARE, 0},
	{WL_PARAM_11H_CSA_CHAN, 1},
	{WL_PARAM_11H_CSA_COUNT, 1},
	{WL_PARAM_11H_CSA_MODE, 1},
	{WL_PARAM_11H_CSA_START, 1},
	{WL_PARAM_SPECTRUM_MGMT, 1},
	{WL_PARAM_POWER_CONSTRAINT, 1},
	{WL_PARAM_11H_DFS_MODE, 1},
	{WL_PARAM_11D_MODE, 1},
	{WL_PARAM_TXPWRFRACTION, 1},
	{WL_PARAM_DISABLEASSOC, 0},
	{WL_PARAM_PSHT_MANAGEMENTACT, 0},
	{WL_PARAM_STAMODE, 0},
	{WL_PARAM_STASCAN, 0},
	{WL_PARAM_AMPDU_TX, 0},
	{WL_PARAM_11HCACTIMEOUT, 1},
	{WL_PARAM_11hNOPTIMEOUT, 1},
	{WL_PARAM_11hDFSMODE, 1},
	{WL_PARAM_MCASTPRXY, 0},
	{WL_PARAM_11H_STA_MODE, 0},
	{WL_PARAM_RSSI, 0},
	{WL_PARAM_INTOLERANT, 1},
	{WL_PARAM_TXQLIMIT, 0},
	{WL_PARAM_RXINTLIMIT, 0},
	{WL_PARAM_LINKSTATUS, 0},
	{WL_PARAM_ANTENNATX, 1},
	{WL_PARAM_RXPATHOPT, 1},
	{WL_PARAM_HTGF, 1},
	{WL_PARAM_HTSTBC, 1},
	{WL_PARAM_3X3RATE, 1},
	{WL_PARAM_AMSDU_FLUSHTIME, 1},
	{WL_PARAM_AMSDU_MAXSIZE, 1},
	{WL_PARAM_AMSDU_ALLOWSIZE, 1},
	{WL_PARAM_AMSDU_PKTCNT, 1},		
};

int is_the_param_applicable(UINT16 cmd)
{
	int i;
	for (i =0; i<sizeof(priv_wlparam)/4;i++)
	{
		if(priv_wlparam[i].command == cmd)
			return priv_wlparam[i].applicable;
	}
	return 0;
}

static int wlioctl_priv_wlparam(struct net_device *dev,
struct iw_request_info *info,
	void *wrqu, char *extra)
{
	struct wlprivate *priv = NETDEV_PRIV_P(struct wlprivate, dev);
	vmacApInfo_t *vmacSta_p = priv->vmacSta_p;
	MIB_802DOT11 *mib = vmacSta_p->ShadowMib802dot11;
	MIB_802DOT11 *mib1 = vmacSta_p->Mib802dot11;
	int param = *((int *) extra);
	int value = *((int *)(extra + sizeof(int)));
	int rc = 0;
	MIB_PHY_DSSS_TABLE *PhyDSSSTable=mib->PhyDSSSTable;
	MIB_SPECTRUM_MGMT	*mib_SpectrumMagament_p=mib->SpectrumMagament;

	WLDBG_ENTER_INFO(DBG_LEVEL_1,"cmd: %d, value: %d\n", param, value );
	if(is_the_param_applicable(param) && priv->master){
		printk("This parameter cannot be set to virtual interface %s, please use %s instead!\n", dev->name, priv->master->name); 
		rc = -EOPNOTSUPP; 
		return rc; 
	} 

	switch (param)
	{
	case WL_PARAM_AUTHTYPE:
		if (value < 0 || value > 2)
		{
			rc = -EOPNOTSUPP;
			break;
		}
		mib->AuthAlg->Type = (UCHAR)value;
		break;

	case WL_PARAM_BAND:
		if (value < 0 /*|| (UCHAR)value > 7*/)
		{
			rc = -EOPNOTSUPP;
			break;
		}
#if 0 /*WFA_TKIP_NEGATIVE*/
		/* Check the validity of the opmode and security mode combination */
		if ((*(mib->mib_wpaWpa2Mode) & 0x0F) == 1 &&
			(value == AP_MODE_N_ONLY
			|| value == AP_MODE_BandN
			|| value == AP_MODE_GandN
			|| value == AP_MODE_BandGandN
			|| value == AP_MODE_AandN)) /*WPA-TKIP or WPA-AES mode */
		{
			printk("HT mode not supported when WPA is enabled\n");
			WLSYSLOG(dev, WLSYSLOG_CLASS_ALL, "HT mode not supported when WPA is enabled\n");
			WLSNDEVT(dev, IWEVCUSTOM,(IEEEtypes_MacAddr_t *)&priv->hwData.macAddr[0], "HT mode not supported when WPA is enabled\n");

			rc = -EOPNOTSUPP;
			break;
		}
#endif

		*(mib->mib_ApMode) = (UCHAR)value;
#ifdef BRS_SUPPORT
		switch (*(mib->mib_ApMode))
		{
		case AP_MODE_B_ONLY:
			*(mib->BssBasicRateMask) = MRVL_BSSBASICRATEMASK_B;
			*(mib->NotBssBasicRateMask) = MRVL_NOTBSSBASICRATEMASK_B;
			*(mib->mib_shortSlotTime) = FALSE;
			break;

		case AP_MODE_G_ONLY:
			*(mib->BssBasicRateMask) = MRVL_BSSBASICRATEMASK_G;
			*(mib->NotBssBasicRateMask) = MRVL_NOTBSSBASICRATEMASK_G;
			*(mib->mib_shortSlotTime) = TRUE;
			break;

		case AP_MODE_A_ONLY:
		case AP_MODE_AandN:
		case AP_MODE_5GHZ_N_ONLY:
#ifdef SOC_W8864		
        case AP_MODE_5GHZ_11AC_ONLY:
		case AP_MODE_5GHZ_Nand11AC:
#endif		
			*(mib->BssBasicRateMask) = MRVL_BSSBASICRATEMASK_A;
			*(mib->NotBssBasicRateMask) = MRVL_NOTBSSBASICRATEMASK_A;
			*(mib->mib_shortSlotTime) = TRUE;
			break;

		case AP_MODE_N_ONLY:
#ifdef SOC_W8864		
		case AP_MODE_2_4GHZ_11AC_MIXED:
#endif		
		case AP_MODE_MIXED:
		default:
			*(mib->BssBasicRateMask) = MRVL_BSSBASICRATEMASK_BGN;
			*(mib->NotBssBasicRateMask) = MRVL_NOTBSSBASICRATEMASK_BGN;
			*(mib->mib_shortSlotTime) = TRUE;
			break;
		}
#endif
#if 0   //per customer request,remove this WAR
		/*If 2G, we disable AMSDU and only enable AMPDU in GUI Auto aggregation mode.
		* If AMSDU is turned on, UDP Tx to MBAir in 2G has low thpt due to MBAir going to sleep in middle of traffic.
		* TODO: Still debugging 9/18/2013
		*/
		if(*(mib->mib_AmpduTx)){
			if(*(mib->mib_ApMode)<= AP_MODE_BandGandN
#ifdef SOC_W8864
			|| *(mib->mib_ApMode) == AP_MODE_2_4GHZ_11AC_MIXED
#endif		
			)
			{	
				*(mib->pMib_11nAggrMode) &= WL_MODE_AMPDU_TX;
			}
			else{
				//keep the ampdu setting
				*(mib->pMib_11nAggrMode) = (*(mib->pMib_11nAggrMode)&WL_MODE_AMPDU_TX)|*(mib->mib_amsdutx);	
			}
		}
#endif		
		break;

	case WL_PARAM_REGIONCODE:
		domainSetDomain(value);
#ifdef IEEE80211_DH
		mib_SpectrumMagament_p->countryCode = value ;
#endif
		*(mib->mib_regionCode) = value;
		break;

	case WL_PARAM_HIDESSID:
		if (value)
			*(mib->mib_broadcastssid) = 0;
		else
			*(mib->mib_broadcastssid) = 1;
		break;

	case WL_PARAM_PREAMBLE:
		switch( (UCHAR)value)
		{
		case 0:
			mib->StationConfig->mib_preAmble   = PREAMBLE_AUTO_SELECT;
			break;
		case 1:
			mib->StationConfig->mib_preAmble   = PREAMBLE_SHORT;
			break;
		case 2:
			mib->StationConfig->mib_preAmble   = PREAMBLE_LONG;
			break;
		default:
			rc = -EOPNOTSUPP;
			break;
		}
		break;

	case WL_PARAM_GPROTECT:
		if (value)
			*(mib->mib_forceProtectiondisable) = 0;
		else
			*(mib->mib_forceProtectiondisable) = 1;
		break;

	case WL_PARAM_BEACON:
		if (value < 20 || value > 1000)
		{
			rc = -EOPNOTSUPP;
			break;
		}

		*(mib->mib_BcnPeriod)= cpu_to_le16(value);
		break;

	case WL_PARAM_DTIM:
		if (value < 1 || value > 255)
		{
			rc = -EOPNOTSUPP;
			break;
		}

		mib->StationConfig->DtimPeriod= (UCHAR)value;
		break;

	case WL_PARAM_FIXRATE:
		if (value < 0 || value > 2)
		{
			rc = -EOPNOTSUPP;
			break;
		}
		*(mib->mib_enableFixedRateTx) = (UCHAR)value;
		break;

	case WL_PARAM_ANTENNA:
		if (value < 0 || value > 7)
		{
			rc = -EOPNOTSUPP;
			break;
		}
		/* 0:ABC(Auto), 1:A, 5:B, 4:C 2:AB, 3:ABC, 6:BC, 7:AC */
		*(mib->mib_rxAntenna) = (UCHAR)value;
		break;

	case WL_PARAM_ANTENNATX:
#ifdef SOC_W8764
		if (value < 0 || value > 0xf)
#else
		if (value < 0 || value > 7)
#endif
		{
			rc = -EOPNOTSUPP;
			break;
		}
		/* 0:AB(Auto), 1:A, 2:B, 3:AB, 7:ABC */
		*(mib->mib_txAntenna) = (UCHAR)value;
		break;

	case WL_PARAM_FILTER:
		if (value < 0 || value > 2)
		{
			rc = -EOPNOTSUPP;
			break;
		}
		*(mib->mib_wlanfiltertype) = (UCHAR)value;
		break;

	case WL_PARAM_WMM:
		*(mib->QoSOptImpl)= (UCHAR)value;
		break;

	case WL_PARAM_WPAWPA2MODE:
		{
#ifdef MRVL_WPS_CLIENT
			vmacEntry_t  *vmacEntry_p = NULL;
			STA_SYSTEM_MIBS *pStaSystemMibs ;
#endif
			if ((value & 0x0000000F) < 0 || (value & 0x0000000F) > 6)
			{
				rc = -EOPNOTSUPP;
				break;
			}
#if 0 /* WFA_TKIP_NEGATIVE */
			/* Check the validity of security mode and operating mode combination */
			/* WPA-TKIP and WPA-AES not allowed when HT modes are enabled */
			if (((value & 0x0000000F) == 1) &&
				(*(mib->mib_ApMode)== AP_MODE_N_ONLY
				|| *(mib->mib_ApMode) == AP_MODE_BandN
				|| *(mib->mib_ApMode) == AP_MODE_GandN
				|| *(mib->mib_ApMode) == AP_MODE_BandGandN
				|| *(mib->mib_ApMode) == AP_MODE_AandN))
			{
				printk("This Security mode not supported when HT mode is enabled\n");

				WLSYSLOG(dev, WLSYSLOG_CLASS_ALL, "This Security mode not supported when HT mode is enabled\n");
				WLSNDEVT(dev, IWEVCUSTOM, (IEEEtypes_MacAddr_t *)&priv->hwData.macAddr[0], "This Security mode not supported when HT mode is enabled\n");

				rc = -EOPNOTSUPP;
				break;
			}
#endif
			*(mib->mib_wpaWpa2Mode) = (UCHAR)value;	

#ifdef MRVL_WPS_CLIENT
			if( vmacSta_p->VMacEntry.modeOfService == VMAC_MODE_CLNT_INFRA )
			{			
				if((vmacEntry_p = sme_GetParentVMacEntry(vmacSta_p->VMacEntry.phyHwMacIndx)) != NULL)
				{	
					pStaSystemMibs = sme_GetStaSystemMibsPtr(vmacEntry_p);
					if( pStaSystemMibs != NULL)
					{
						pStaSystemMibs->mib_StaCfg_p->wpawpa2Mode = value ;
					}
				}
			}
#endif

#ifdef MRVL_WSC
			if ((value == 0) || ((value & 0x0000000F) == 0))
#else
			if (value == 0)
#endif
			{
				mib->Privacy->RSNEnabled = 0;
				mib->Privacy->RSNLinkStatus = 0;		
				mib->RSNConfigWPA2->WPA2Enabled = 0;
				mib->RSNConfigWPA2->WPA2OnlyEnabled = 0;
			}
			else
			{   
				mib->Privacy->PrivInvoked = 0; /* WEP disable */
				mib->AuthAlg->Type = 0; /* Reset WEP to open mode */
				mib->Privacy->RSNEnabled = 1;
				mib->Privacy->RSNLinkStatus = 0;		
				mib->RSNConfigWPA2->WPA2Enabled = 0;
				mib->RSNConfigWPA2->WPA2OnlyEnabled = 0;
				*(mib->mib_WPAPSKValueEnabled) = 0; //PSK

				mib->RSNConfig->MulticastCipher[0] = 0x00;
				mib->RSNConfig->MulticastCipher[1] = 0x50;
				mib->RSNConfig->MulticastCipher[2] = 0xF2;
				mib->RSNConfig->MulticastCipher[3] = 0x02;          // TKIP

				mib->UnicastCiphers->UnicastCipher[0] = 0x00;
				mib->UnicastCiphers->UnicastCipher[1] = 0x50;
				mib->UnicastCiphers->UnicastCipher[2] = 0xF2;
				mib->UnicastCiphers->UnicastCipher[3] = 0x02;        // TKIP
				mib->UnicastCiphers->Enabled = TRUE;

				mib->RSNConfigAuthSuites->AuthSuites[0] = 0x00;
				mib->RSNConfigAuthSuites->AuthSuites[1] = 0x50;
				mib->RSNConfigAuthSuites->AuthSuites[2] = 0xF2;

				if ((value & 0x0000000F) == 4 ||(value & 0x0000000F) == 6)
					mib->RSNConfigAuthSuites->AuthSuites[3] = 0x01;      // Auth8021x
				else
					mib->RSNConfigAuthSuites->AuthSuites[3] = 0x02;      // AuthPSK

				mib->RSNConfigAuthSuites->Enabled = TRUE;

				*(mib->mib_cipherSuite) = 2;

				if ((value & 0x0000000F) == 2 ||(value & 0x0000000F) == 5)
				{
					mib->RSNConfigWPA2->WPA2Enabled = 1;
					mib->RSNConfigWPA2->WPA2OnlyEnabled = 1;

					mib->RSNConfigWPA2->MulticastCipher[0] = 0x00;
					mib->RSNConfigWPA2->MulticastCipher[1] = 0x0F;
					mib->RSNConfigWPA2->MulticastCipher[2] = 0xAC;
					mib->RSNConfigWPA2->MulticastCipher[3] = 0x04;      // AES

					mib->WPA2UnicastCiphers->UnicastCipher[0] = 0x00;
					mib->WPA2UnicastCiphers->UnicastCipher[1] = 0x0F;
					mib->WPA2UnicastCiphers->UnicastCipher[2] = 0xAC;
					mib->WPA2UnicastCiphers->UnicastCipher[3] = 0x04;   // AES
					mib->WPA2UnicastCiphers->Enabled = TRUE;

					mib->WPA2AuthSuites->AuthSuites[0] = 0x00;
					mib->WPA2AuthSuites->AuthSuites[1] = 0x0F;
					mib->WPA2AuthSuites->AuthSuites[2] = 0xAC;

					if ((value & 0x0000000F) == 5)
						mib->WPA2AuthSuites->AuthSuites[3] = 0x01;          // Auth8021x
					else
						mib->WPA2AuthSuites->AuthSuites[3] = 0x02;          // AuthPSK

					mib->WPA2AuthSuites->Enabled = TRUE;

					*(mib->mib_cipherSuite) = 4;

				} 
				else if ((value & 0x0000000F) == 3 ||(value & 0x0000000F) == 6)
				{
					mib->RSNConfigWPA2->WPA2Enabled = 1;
					mib->RSNConfigWPA2->WPA2OnlyEnabled = 0;
					mib->RSNConfigWPA2->MulticastCipher[0] = 0x00;
					mib->RSNConfigWPA2->MulticastCipher[1] = 0x0F;
					mib->RSNConfigWPA2->MulticastCipher[2] = 0xAC;
					mib->RSNConfigWPA2->MulticastCipher[3] = 0x02;      // TKIP 

					mib->UnicastCiphers->UnicastCipher[0] = 0x00;
					mib->UnicastCiphers->UnicastCipher[1] = 0x50;
					mib->UnicastCiphers->UnicastCipher[2] = 0xF2;
					mib->UnicastCiphers->UnicastCipher[3] = 0x02;        // TKIP
					mib->UnicastCiphers->Enabled = TRUE;

					mib->WPA2UnicastCiphers->UnicastCipher[0] = 0x00;
					mib->WPA2UnicastCiphers->UnicastCipher[1] = 0x0F;
					mib->WPA2UnicastCiphers->UnicastCipher[2] = 0xAC;
					mib->WPA2UnicastCiphers->UnicastCipher[3] = 0x04;   // AES
					mib->WPA2UnicastCiphers->Enabled = TRUE;

					mib->WPA2AuthSuites->AuthSuites[0] = 0x00;
					mib->WPA2AuthSuites->AuthSuites[1] = 0x0F;
					mib->WPA2AuthSuites->AuthSuites[2] = 0xAC;

					if ((value & 0x0000000F) == 6)
						mib->WPA2AuthSuites->AuthSuites[3] = 0x01;          // Auth8021x                        
					else
						mib->WPA2AuthSuites->AuthSuites[3] = 0x02;          // AuthPSK

					mib->WPA2AuthSuites->Enabled = TRUE;

					*(mib->mib_cipherSuite) = 4;

				}
			}

			WLDBG_INFO(DBG_LEVEL_1,  "mib->Privacy->RSNEnabled %d\n", mib->Privacy->RSNEnabled);
			WLDBG_INFO(DBG_LEVEL_1,  "mib->RSNConfigWPA2->WPA2Enabled %d\n", mib->RSNConfigWPA2->WPA2Enabled);
			WLDBG_INFO(DBG_LEVEL_1,  "mib->RSNConfigWPA2->WPA2OnlyEnabled %d\n", mib->RSNConfigWPA2->WPA2OnlyEnabled);
			WLDBG_INFO(DBG_LEVEL_1,  "mib->mib_wpaWpa2Mode %x\n", *(mib->mib_wpaWpa2Mode));

			break;

#ifdef MRVL_WAPI
	case WL_PARAM_WAPIMODE:
		if (value == 0)
			mib->Privacy->WAPIEnabled = (UCHAR)value;
		else
			printk("Note: wapimode only can be enabled by wapid\n");		
		break;
#endif

	case WL_PARAM_GROUPREKEYTIME:
		if (value < 0 )
		{
			rc = -EOPNOTSUPP;
			break;
		}
		if (value)
			mib->RSNConfig->GroupRekeyTime = cpu_to_le32(value);
		else /* disable rekey */
			mib->RSNConfig->GroupRekeyTime = cpu_to_le32(0xffffffff/10);

		WLDBG_INFO(DBG_LEVEL_1,  "mib->RSNConfig->GroupRekeyTime %d\n", mib->RSNConfig->GroupRekeyTime);

		}
		break;

	case WL_PARAM_INTRABSS:
		if (value < 0 || value > 1)
		{
			rc = -EOPNOTSUPP;
			break;
		}
		*(mib->mib_intraBSS) = (UCHAR)value;
		break;

	case WL_PARAM_AMSDU:
		if (value < 0 || value > 2)
		{
			rc = -EOPNOTSUPP;
			break;
		}

		*(mib->mib_amsdutx)= value;
#if 0		
		/*If 2G, we disable AMSDU and only enable AMPDU in GUI Auto aggregation mode.
		* If AMSDU is turned on, UDP Tx to MBAir in 2G has low thpt due to MBAir going to sleep in middle of traffic.
		* TODO: Still debugging 9/18/2013
		*/
		if(*(mib->mib_ApMode)<= AP_MODE_BandGandN
#ifdef SOC_W8864
		|| *(mib->mib_ApMode) == AP_MODE_2_4GHZ_11AC_MIXED
#endif		
		)
		{
			*(mib->pMib_11nAggrMode) &= WL_MODE_AMPDU_TX;
		}
		else
#endif		
		{
			//keep the ampdu setting
			*(mib->pMib_11nAggrMode) = (*(mib->pMib_11nAggrMode)&WL_MODE_AMPDU_TX)|(UCHAR)value;
		}
		
		break;

	case WL_PARAM_HTBANDWIDTH:
		switch(value)
		{
		case 0:
			PhyDSSSTable->Chanflag.ChnlWidth = CH_AUTO_WIDTH;
			break;
		case 1:
			PhyDSSSTable->Chanflag.ChnlWidth = CH_10_MHz_WIDTH;
			break;
		case 2:
			PhyDSSSTable->Chanflag.ChnlWidth = CH_20_MHz_WIDTH;
			break;
		case 3:
			PhyDSSSTable->Chanflag.ChnlWidth = CH_40_MHz_WIDTH;
			break;
		default:
			rc = -EOPNOTSUPP;
			break;				
		}
#ifdef INTOLERANT40
		*(mib->USER_ChnlWidth) = PhyDSSSTable->Chanflag.ChnlWidth;
		if ((*(mib->USER_ChnlWidth) == CH_40_MHz_WIDTH) || (*(mib->USER_ChnlWidth) == CH_AUTO_WIDTH))
			*(mib->mib_FortyMIntolerant) = 0;
		else
			*(mib->mib_FortyMIntolerant) = 1;
#endif
#ifdef COEXIST_20_40_SUPPORT
		if ((PhyDSSSTable->Chanflag.ChnlWidth == CH_AUTO_WIDTH) || 
			(PhyDSSSTable->Chanflag.ChnlWidth == CH_40_MHz_WIDTH))
		{
			if (PhyDSSSTable->CurrChan == 14)
				*(mib->USER_ChnlWidth )= 0;
			else
				*(mib->USER_ChnlWidth )= 1;
		}
		else
			*(mib->USER_ChnlWidth )= 0;
#endif
		break;

	case WL_PARAM_WMMACKPOLICY:
		if (value < 0 || value > 3)
		{
			rc = -EOPNOTSUPP;
			break;
		}
		*(mib->mib_wmmAckPolicy) = (UCHAR)value;
		break;

	case WL_PARAM_GUARDINTERVAL:
		if (value < 0 || value > 2)
		{
			rc = -EOPNOTSUPP;
			break;
		}
		*(mib->mib_guardInterval) = (UCHAR)value;
		break;

	case WL_PARAM_EXTSUBCH:
		if (value < 0 || value > 2)
		{
			rc = -EOPNOTSUPP;
			break;
		}
		switch(PhyDSSSTable->CurrChan)
		{
		case 1:
		case 2:
		case 3:
		case 4:
			if( value == 1)
				return -EINVAL;
			break;
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
		case 10:
			break;
		case 11:
		case 12:
		case 13:
		case 14:
			if( value == 2)
				return -EINVAL;
			break;
		}
		*(mib->mib_extSubCh) = (UCHAR)value;
		break;

	case WL_PARAM_HTPROTECT:
		if (value < 0 || value > 4)
		{
			rc = -EOPNOTSUPP;
			break;
		}
		*(mib->mib_htProtect) = (UCHAR)value;
		break;

	case WL_PARAM_GETFWSTAT:
		wlFwGetHwStats(dev, NULL);
		break;

	case WL_PARAM_AGINGTIME:
		if (value < 60 || value > 86400)
		{
			rc = -EOPNOTSUPP;
			break;
		}
		*(mib->mib_agingtime) = value;
		break;
	case WL_PARAM_ANTENNATX2:
		*(mib->mib_txAntenna2)= value;
		break;
	case WL_PARAM_CDD:
		if (value < 0 || value > 1)
		{
			rc = -EINVAL;
			break;
		}

		*(mib->mib_CDD)= value;
		break;
	case WL_PARAM_ACS_THRESHOLD:
		if (value < 0 )
		{
			rc = -EINVAL;
			break;
		}

		*(mib->mib_acs_threshold)= value;
		break;
	case WL_PARAM_AUTOCHANNEL:
		if (value < 0 || value > 2)
		{
			rc = -EINVAL;
			break;
		}
		*(mib->mib_autochannel)= value;
		break;
	case WL_PARAM_AMPDUFACTOR:
		if (value < 0 || value > 3)
		{
			rc = -EINVAL;
			break;
		}
		*(mib->mib_ampdu_factor)= value;
		break;
	case WL_PARAM_AMPDUDENSITY:
		if (value < 0 || value > 7)
		{
			rc = -EINVAL;
			break;
		}
		*(mib->mib_ampdu_density)= value;
		break;
#ifdef INTEROP
	case WL_PARAM_INTEROP:
		*(mib->mib_interop)= value;
		break;
#endif
	case WL_PARAM_OPTLEVEL:
		*(mib->mib_optlevel)= value;
		break;

	case WL_PARAM_REGIONPWR:
		{
			int i;
			if (value < MINTXPOWER || value > 18)
			{
				rc = -EINVAL;
				break;
			}

			*(mib->mib_MaxTxPwr)= value;
			for (i = 0; i < TX_POWER_LEVEL_TOTAL; i++)
			{
				if(mib->PhyDSSSTable->maxTxPow[i]>*(mib->mib_MaxTxPwr))
					mib->PhyDSSSTable->maxTxPow[i]= *(mib->mib_MaxTxPwr);
			}	
		}
		break;

#ifdef PWRFRAC
	case WL_PARAM_TXPWRFRACTION:
		{
			if (value < 0 || value > 5)
			{
				rc = -EINVAL;
				break;
			}

			*(mib->mib_TxPwrFraction)= value;
		}
		break;
#endif

	case WL_PARAM_ADAPTMODE:
		{
			if (value < 0 || value > 1)
			{
				rc = -EINVAL;
				break;
			}

			*(mib->mib_RateAdaptMode)= value;
		}
		break;
#ifdef WDS_FEATURE
	case WL_PARAM_WDSMODE:
		{
			if (value < 0 || value > 1)
			{
				rc = -EINVAL;
				break;
			}

			*(mib->mib_wdsEnable) = value;
		}
		break;
#endif
	case WL_PARAM_DISABLEASSOC:
		{
			if (value < 0 || value > 1)
			{
				rc = -EINVAL;
				break;
			}

			*(mib->mib_disableAssoc) = value;
		}
		break;
	case WL_PARAM_CSADAPTMODE:
		{
			if (value < 0 || value > 3)
			{
				rc = -EINVAL;
				break;
			}

			*(mib->mib_CSMode)= value;
		}
		break;

		/* MRV_8021X */
	case WL_PARAM_SETKEYS:
		{
#ifndef CLIENT_SUPPORT
#define GetParentStaBSSID(x) NULL
#endif
			struct wlreq_key wk;
			extStaDb_StaInfo_t *pStaInfo=NULL;
#ifdef CLIENT_SUPPORT
			vmacEntry_t  *vmacEntry_p = NULL;
			STA_SECURITY_MIBS * pStaSecurityMibs = NULL;
			keyMgmtInfoSta_t  *pKeyMgmtInfoSta = NULL;
#endif

			if ((char*)value == NULL)
			{
				rc = -EINVAL;
				break;
			}
#ifdef MRVL_WPS_CLIENT
			if( vmacSta_p->VMacEntry.modeOfService == VMAC_MODE_CLNT_INFRA )
			{
				if( (vmacEntry_p = sme_GetParentVMacEntry(vmacSta_p->VMacEntry.phyHwMacIndx)) == NULL)
				{
					rc = -EFAULT;
					break;
				}
			}
#endif //MRVL_WPS_CLIENT

			if (copy_from_user((char*)&wk, (char *)value, sizeof(wk)))
			{
				rc = -EINVAL;
				break;
			}

#ifdef MRVL_WPS_CLIENT
			if( vmacSta_p->VMacEntry.modeOfService == VMAC_MODE_CLNT_INFRA )
			{
				pStaSecurityMibs = sme_GetStaSecurityMibsPtr(vmacEntry_p);
				pKeyMgmtInfoSta = sme_GetKeyMgmtInfoStaPtr(vmacEntry_p);
				if( pStaSecurityMibs == NULL || pKeyMgmtInfoSta == NULL )
				{
					rc = -EINVAL;
					break;
				}
			}
#endif //MRVL_WPS_CLIENT

			if (wk.ik_keyix == WL_KEYIX_NONE)
			{
				if (extStaDb_SetRSNPwkAndDataTraffic(vmacSta_p,
					vmacSta_p->VMacEntry.modeOfService == VMAC_MODE_CLNT_INFRA ? 
					(IEEEtypes_MacAddr_t *)GetParentStaBSSID((vmacEntry_p)->phyHwMacIndx) : 
				(IEEEtypes_MacAddr_t *)wk.ik_macaddr, 
					&wk.ik_keydata[0],
					(UINT32*)&wk.ik_keydata[16],
					(UINT32*)&wk.ik_keydata[24]) != STATE_SUCCESS)
				{
					rc = -EOPNOTSUPP;
					break;
				}
				if (extStaDb_SetPairwiseTSC(vmacSta_p,
					vmacSta_p->VMacEntry.modeOfService == VMAC_MODE_CLNT_INFRA ?
					(IEEEtypes_MacAddr_t *)GetParentStaBSSID((vmacEntry_p)->phyHwMacIndx) :
				(IEEEtypes_MacAddr_t *)wk.ik_macaddr, 
					0, 0x0001) != STATE_SUCCESS)
				{
					rc = -EOPNOTSUPP;
					break;
				}

				if ((pStaInfo = extStaDb_GetStaInfo(
					vmacSta_p,
					vmacSta_p->VMacEntry.modeOfService == VMAC_MODE_CLNT_INFRA ?
					(IEEEtypes_MacAddr_t *)GetParentStaBSSID((vmacEntry_p)->phyHwMacIndx) :
				(IEEEtypes_MacAddr_t *)wk.ik_macaddr, 
					1)) == NULL)
				{
					rc = -EOPNOTSUPP;
					break;
				}
#ifdef CLIENT_SUPPORT
				if( vmacSta_p->VMacEntry.modeOfService == VMAC_MODE_CLNT_INFRA )
				{
					if (   !pStaSecurityMibs->mib_RSNConfigWPA2_p->WPA2OnlyEnabled
						&& !pStaSecurityMibs->mib_RSNConfigWPA2_p->WPA2Enabled )
					{
						//WPA
						AddRSN_IE_TO(pStaSecurityMibs->thisStaRsnIE_p,(IEEEtypes_RSN_IE_t*)(&pStaInfo->keyMgmtStateInfo.RsnIEBuf[0]) );
						if (pStaSecurityMibs->mib_RSNConfigUnicastCiphers_p->UnicastCipher[3] == 2)
						{
							// TKIP
							wlFwSetWpaTkipMode_STA(dev,  (UINT8 *)&pStaInfo->Addr);
						}
						else if ((pStaSecurityMibs->mib_RSNConfigUnicastCiphers_p->UnicastCipher[3] == 4))
						{
							// AES
							wlFwSetWpaAesMode_STA(dev,  (UINT8 *)&pStaInfo->Addr);
						}
					}
					else
					{
						// WPA2
						AddRSN_IEWPA2_TO(pStaSecurityMibs->thisStaRsnIEWPA2_p,(IEEEtypes_RSN_IE_WPA2_t*)(&pStaInfo->keyMgmtStateInfo.RsnIEBuf[0]) );
						if (pStaSecurityMibs->mib_RSNConfigWPA2UnicastCiphers_p->UnicastCipher[3] == 4)
						{
							// AES
							wlFwSetWpaAesMode_STA(dev,  (UINT8 *)&pStaInfo->Addr);
						}
						else
						{
							// TKIP
							//Not sure if this is correct setting for firmware in this case????
							wlFwSetMixedWpaWpa2Mode_STA(dev,  (UINT8 *)&pStaInfo->Addr);
						}
					}

					wlFwSetWpaWpa2PWK_STA(dev, pStaInfo);
					if( pKeyMgmtInfoSta )
						pKeyMgmtInfoSta->pKeyData->RSNDataTrafficEnabled = 1 ;
					if( pStaInfo )
						pStaInfo->keyMgmtStateInfo.RSNDataTrafficEnabled = 1 ;
					break ;
				}
				else
#endif 
				{
					if( vmacSta_p->VMacEntry.modeOfService == VMAC_MODE_AP )
					{
						wlFwSetWpaWpa2PWK(dev, pStaInfo);
						break;
					}
				}
			}
			else if ((0 < wk.ik_keyix) &&  (wk.ik_keyix< 4))
			{
				if (wk.ik_type == WL_CIPHER_TKIP)
				{
					if( vmacSta_p->VMacEntry.modeOfService == VMAC_MODE_AP )
					{
						memcpy(mib1->mib_MrvlRSN_GrpKey->EncryptKey, &wk.ik_keydata[0], 16);
						memcpy(mib1->mib_MrvlRSN_GrpKey->TxMICKey, &wk.ik_keydata[16], 8);
						memcpy(mib1->mib_MrvlRSN_GrpKey->RxMICKey, &wk.ik_keydata[24], 8);
						mib1->mib_MrvlRSN_GrpKey->g_IV16 = 0x0001;
						mib1->mib_MrvlRSN_GrpKey->g_IV32 = 0;
						mib1->mib_MrvlRSN_GrpKey->g_KeyIndex = (UINT8)wk.ik_keyix;
						wlFwSetWpaTkipGroupK(dev, mib1->mib_MrvlRSN_GrpKey->g_KeyIndex);
						//need to update shadow mib, when directly modify run-time mib.
						memcpy(mib->mib_MrvlRSN_GrpKey, 
							mib1->mib_MrvlRSN_GrpKey, sizeof(MRVL_MIB_RSN_GRP_KEY));
					}
#ifdef CLIENT_SUPPORT
					else if( vmacSta_p->VMacEntry.modeOfService == VMAC_MODE_CLNT_INFRA )
					{
						ENCR_TKIPSEQCNT TkipTsc;
						memcpy( mib_MrvlRSN_GrpKeyUr1[vmacEntry_p->phyHwMacIndx].EncryptKey,
							&wk.ik_keydata[0], TK_SIZE );
						memcpy(mib_MrvlRSN_GrpKeyUr1[vmacEntry_p->phyHwMacIndx].TxMICKey, 
							&wk.ik_keydata[16], 8 );
						memcpy(mib_MrvlRSN_GrpKeyUr1[vmacEntry_p->phyHwMacIndx].RxMICKey, 
							&wk.ik_keydata[24], 8);
						mib_MrvlRSN_GrpKeyUr1[vmacEntry_p->phyHwMacIndx].g_IV16 = 0x0001;
						mib_MrvlRSN_GrpKeyUr1[vmacEntry_p->phyHwMacIndx].g_IV32 = 0;

						TkipTsc.low = mib_MrvlRSN_GrpKeyUr1[vmacEntry_p->phyHwMacIndx].g_IV16;
						TkipTsc.high = mib_MrvlRSN_GrpKeyUr1[vmacEntry_p->phyHwMacIndx].g_IV32;

						wlFwSetWpaTkipGroupK_STA(dev,
							GetParentStaBSSID(vmacEntry_p->phyHwMacIndx),
							&mib_MrvlRSN_GrpKeyUr1[vmacEntry_p->phyHwMacIndx].EncryptKey[0],
							TK_SIZE,
							(UINT8 *)&mib_MrvlRSN_GrpKeyUr1[vmacEntry_p->phyHwMacIndx].RxMICKey,
							MIC_KEY_LENGTH,
							(UINT8 *)&mib_MrvlRSN_GrpKeyUr1[vmacEntry_p->phyHwMacIndx].TxMICKey,
							MIC_KEY_LENGTH,
							TkipTsc, wk.ik_keyix);

						if( pKeyMgmtInfoSta )
							pKeyMgmtInfoSta->pKeyData->RSNSecured = 1 ;
					}
#endif
					break;
				}
				else if (wk.ik_type == WL_CIPHER_CCMP)
				{
					if( vmacSta_p->VMacEntry.modeOfService == VMAC_MODE_AP )
					{
						memcpy(mib1->mib_MrvlRSN_GrpKey->EncryptKey, &wk.ik_keydata[0], 16);
						mib1->mib_MrvlRSN_GrpKey->g_KeyIndex = (UINT8)wk.ik_keyix;
						wlFwSetWpaAesGroupK(dev, mib1->mib_MrvlRSN_GrpKey->g_KeyIndex);
						//need to update shadow mib, when directly modify run-time mib.
						memcpy(mib->mib_MrvlRSN_GrpKey, 
							mib1->mib_MrvlRSN_GrpKey, sizeof(MRVL_MIB_RSN_GRP_KEY));
					}
#ifdef CLIENT_SUPPORT
					else if( vmacSta_p->VMacEntry.modeOfService == VMAC_MODE_CLNT_INFRA )
					{
						memcpy( mib_MrvlRSN_GrpKeyUr1[vmacEntry_p->phyHwMacIndx].EncryptKey,
							&wk.ik_keydata[0], TK_SIZE );
						wlFwSetWpaAesGroupK_STA(dev,
							GetParentStaBSSID(vmacEntry_p->phyHwMacIndx),
							&mib_MrvlRSN_GrpKeyUr1[vmacEntry_p->phyHwMacIndx].EncryptKey[0],
							TK_SIZE);

					}
#endif
					break;
				}
				else
				{
					rc = -ENOTSUPP;
					break;
				}
			}
			else
			{
				rc = -ENOTSUPP;
				break;
			}
		}
		break;

	case WL_PARAM_DELKEYS:
		{
			struct wlreq_del_key wk;

			if ((char*)value == NULL)
			{
				rc = -EINVAL;
				break;
			}
			if (copy_from_user((char*)&wk, (char *)value, sizeof(wk)))
			{
				rc = -EINVAL;
				break;
			}

			if (extStaDb_SetRSNDataTrafficEnabled(vmacSta_p,(IEEEtypes_MacAddr_t *)wk.idk_macaddr, FALSE) != STATE_SUCCESS)
			{
				break;
			}
		}
		break;
	case WL_PARAM_MLME_REQ:
		{
			struct wlreq_mlme mlme;

			if ((char*)value == NULL)
			{
				rc = -EINVAL;
				break;
			}

			if (copy_from_user((char*)&mlme, (char *)value, sizeof(mlme)))
			{
				rc = -EINVAL;
				break;
			}

			switch(mlme.im_op)
			{
			case WL_MLME_DEAUTH:
				macMgmtMlme_SendDeauthenticateMsg(vmacSta_p, &mlme.im_macaddr, 0,
					mlme.im_reason);
				break;
			default:
				rc = -EOPNOTSUPP;
				break;
			}
		}
		break;
	case WL_PARAM_COUNTERMEASURES:
		{
			if (value)
			{
				vmacSta_p->MIC_ErrordisableStaAsso = 1;                           
				macMgmtMlme_SendDeauthenticateMsg(vmacSta_p,&bcastMacAddr, 0, 
					IEEEtypes_REASON_MIC_FAILURE);
				extStaDb_RemoveAllStns(vmacSta_p,IEEEtypes_REASON_MIC_FAILURE);
			}
			else
			{
				vmacSta_p->MIC_ErrordisableStaAsso = 0;                           
			}
		}
		break;
		/* MRV_8021X */

	case WL_PARAM_DELWEPKEY:
		{
			if (value < 0 || value > 3)
			{
				rc = -EINVAL;
				break;
			}
			WLDBG_INFO(DBG_LEVEL_1, "wep key = %x %x %x %x %x %x %x %x %x %x %x %x %x \n",
				mib->WepDefaultKeys[value].WepDefaultKeyValue[0],
				mib->WepDefaultKeys[value].WepDefaultKeyValue[1],
				mib->WepDefaultKeys[value].WepDefaultKeyValue[2],
				mib->WepDefaultKeys[value].WepDefaultKeyValue[3],
				mib->WepDefaultKeys[value].WepDefaultKeyValue[4],
				mib->WepDefaultKeys[value].WepDefaultKeyValue[5],
				mib->WepDefaultKeys[value].WepDefaultKeyValue[6],
				mib->WepDefaultKeys[value].WepDefaultKeyValue[7],
				mib->WepDefaultKeys[value].WepDefaultKeyValue[8],
				mib->WepDefaultKeys[value].WepDefaultKeyValue[9],
				mib->WepDefaultKeys[value].WepDefaultKeyValue[10],
				mib->WepDefaultKeys[value].WepDefaultKeyValue[11],
				mib->WepDefaultKeys[value].WepDefaultKeyValue[12]);

			memset(mib->WepDefaultKeys[value].WepDefaultKeyValue, 0, 13);
			WLDBG_INFO(DBG_LEVEL_1, "wep key = %x %x %x %x %x %x %x %x %x %x %x %x %x \n",
				mib->WepDefaultKeys[value].WepDefaultKeyValue[0],
				mib->WepDefaultKeys[value].WepDefaultKeyValue[1],
				mib->WepDefaultKeys[value].WepDefaultKeyValue[2],
				mib->WepDefaultKeys[value].WepDefaultKeyValue[3],
				mib->WepDefaultKeys[value].WepDefaultKeyValue[4],
				mib->WepDefaultKeys[value].WepDefaultKeyValue[5],
				mib->WepDefaultKeys[value].WepDefaultKeyValue[6],
				mib->WepDefaultKeys[value].WepDefaultKeyValue[7],
				mib->WepDefaultKeys[value].WepDefaultKeyValue[8],
				mib->WepDefaultKeys[value].WepDefaultKeyValue[9],
				mib->WepDefaultKeys[value].WepDefaultKeyValue[10],
				mib->WepDefaultKeys[value].WepDefaultKeyValue[11],
				mib->WepDefaultKeys[value].WepDefaultKeyValue[12]);
		}
		break;

	case WL_PARAM_STRICTWEPSHARE:
		{
			if (value < 0 || value > 1)
			{
				rc = -EINVAL;
				break;
			}

			*(mib->mib_strictWepShareKey) = value;
		}
		break;
#ifdef IEEE80211_DH
	case WL_PARAM_11H_DFS_MODE:
		{
			if (value < 0 || value > 3)
			{
				rc = -EINVAL;
				break;
			}
			if ((PhyDSSSTable->Chanflag.FreqBand == FREQ_BAND_5GHZ ) &&
				(PhyDSSSTable->CurrChan >= 52)) 
			{
				wlFwSetRadarDetection(dev, value );
			}
			else
			{
				rc = -EOPNOTSUPP;
				break;
			}
		}
		break ;

	case WL_PARAM_11H_CSA_CHAN :
		{
			if(!domainChannelValid(value, FREQ_BAND_5GHZ))
			{
				rc = -EINVAL;
				break;
			}
			mib_SpectrumMagament_p->csaChannelNumber = value ;
		}
		break ;

	case WL_PARAM_11H_CSA_COUNT :
		{
			if (value < 0 || value > 255)
			{
				rc = -EINVAL;
				break;
			}
			mib_SpectrumMagament_p->csaCount = value ;
		}
		break ;

	case WL_PARAM_11H_CSA_MODE :
		{
			if (value < 0 || value > 1)
			{
				rc = -EINVAL;
				break;
			}
			mib_SpectrumMagament_p->csaMode = value ;
		}
		break ;

	case WL_PARAM_11H_CSA_START :
		{
			int i ;
			IEEEtypes_ChannelSwitchCmd_t ChannelSwitchCmd;
			if (value < 0 || value > 1)
			{
				rc = -EINVAL;
				break;
			}
			if( value == 0 ) 
			{
				break;
			}

			if (PhyDSSSTable->Chanflag.FreqBand != FREQ_BAND_5GHZ ) {
				WLDBG_INFO(DBG_LEVEL_1,  "wlioctl_priv_wlparam: wrong band %d\n", 
					PhyDSSSTable->Chanflag.FreqBand );
				rc = -EOPNOTSUPP;
				break;
			}
			if (mib->StationConfig->SpectrumManagementRequired != TRUE)
			{
				WLDBG_INFO(DBG_LEVEL_1,  "wlioctl_priv_wlparam: spectrum management disabled\n");
				rc = -EOPNOTSUPP;
				break;
			}
			if(!domainChannelValid(mib_SpectrumMagament_p->csaChannelNumber, FREQ_BAND_5GHZ))
			{
				WLDBG_INFO(DBG_LEVEL_1,  "wlioctl_priv_wlparam: wrong channel:%d\n",
					mib_SpectrumMagament_p->csaChannelNumber);
				rc = -EOPNOTSUPP;
				break;
			}
			ChannelSwitchCmd.Mode = mib_SpectrumMagament_p->csaMode; 
			ChannelSwitchCmd.ChannelNumber = mib_SpectrumMagament_p->csaChannelNumber ;
			ChannelSwitchCmd.ChannelSwitchCount = mib_SpectrumMagament_p->csaCount;

			/* Send Channel Switch Command to all the AP virtual interfaces */
			for( i = 0 ; i <= MAX_VMAC_INSTANCE_AP; i ++ )
			{
				if( priv->vdev[i] && priv->vdev[i]->flags & IFF_RUNNING )
				{
					struct net_device *vdev = priv->vdev[i] ;
					struct wlprivate *vpriv = NETDEV_PRIV_P(struct wlprivate, vdev);
					SendChannelSwitchCmd(vpriv->vmacSta_p, &ChannelSwitchCmd);
				}
			}

		}
		break ;

	case WL_PARAM_SPECTRUM_MGMT:
		{
			if (value < 0 || value > 2)
			{
				rc = -EINVAL;
				break;
			}
			mib_SpectrumMagament_p->spectrumManagement = value ;
			mib->StationConfig->SpectrumManagementRequired = value ? TRUE : FALSE;
			/* If spectrum management is enabled, set power constraint and
			* country info.
			*/
			if( value ) 
			{
				mib_SpectrumMagament_p->multiDomainCapability = 1 ;
			}
		}
		break ;

	case WL_PARAM_POWER_CONSTRAINT:
		{
			if (value < 0 || value > 30)
			{
				rc = -EINVAL;
				break;
			}
			if (PhyDSSSTable->Chanflag.FreqBand != FREQ_BAND_5GHZ ) {
				WLDBG_INFO(DBG_LEVEL_1,  "wlioctl_priv_wlparam: wrong Freq band :%d\n", 
					PhyDSSSTable->Chanflag.FreqBand );
				rc = -EOPNOTSUPP;
				break;
			}
			mib_SpectrumMagament_p->powerConstraint = value ;
		}
		break ;

	case WL_PARAM_11D_MODE:
		{
			if (value < 0 || value > 2)
			{
				rc = -EINVAL;
				break;
			}
			mib_SpectrumMagament_p->multiDomainCapability = value ;
		}
		break ;
#ifdef CLIENT_SUPPORT
	case WL_PARAM_11H_STA_MODE:
		{
			vmacEntry_t  *vmacEntry_p = NULL;
			STA_SYSTEM_MIBS *pStaSystemMibs ;

			if (value < 0 || value > 1)
			{
				rc = -EINVAL;
				break;
			}
			if((vmacEntry_p = sme_GetParentVMacEntry(vmacSta_p->VMacEntry.phyHwMacIndx)) == NULL)
			{
				rc = -EFAULT;
				break;
			}
			pStaSystemMibs = sme_GetStaSystemMibsPtr(vmacEntry_p);
			if( pStaSystemMibs == NULL)
			{
				rc = -EFAULT;
				break;
			}
			pStaSystemMibs->mib_StaCfg_p->sta11hMode = value ;
		}
		break ;
#endif //CLIENT_SUPPORT
#endif //IEEE80211_DH
#ifdef MRVL_DFS 
		/* This code is for simulating a radar generation 
		* to validate the DFS SM logic 
		*/
	case WL_PARAM_11HCACTIMEOUT:
		{
			if (value < 5 || value > 60)
			{
				rc = -EINVAL;
				break;
			}
			*(mib->mib_CACTimeOut) = value ;
		}
		break ;
	case WL_PARAM_11hNOPTIMEOUT:
		{
			if (value < 5 || value > 1800)
			{
				rc = -EINVAL;
				break;
			}
			*(mib->mib_NOPTimeOut) = value ;
		}
		break ;
#endif // MRVL_DFS


	case WL_PARAM_PSHT_MANAGEMENTACT:
		{
			extern BOOLEAN macMgmtMlme_SendMimoPsHtManagementAction(vmacApInfo_t *vmacSta_p,IEEEtypes_MacAddr_t *Addr, UINT8 mode);
			extern int wlFwSetMimoPsHt(struct net_device *netdev, UINT8 *addr, UINT8 enable, UINT8 mode);
			UINT8 addr[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
			UINT8 i;
			UINT32 entries;
			UINT8 *staBuf = NULL;
			UINT8 *listBuf = NULL;
			extStaDb_StaInfo_t *pStaInfo;

			switch (value)
			{
			case 0:
			case 1:
			case 3:
				*(mib->mib_psHtManagementAct) = (UINT8 )value;
				if (macMgmtMlme_SendMimoPsHtManagementAction(vmacSta_p, (IEEEtypes_MacAddr_t *)&addr, *(mib->mib_psHtManagementAct)) == TRUE)
				{
					entries = extStaDb_entries(vmacSta_p, 0);
					staBuf = kmalloc(entries*sizeof(STA_INFO) ,GFP_KERNEL);
					if (staBuf != NULL)
					{
						extStaDb_list(vmacSta_p, staBuf, 1);

						if(entries)
						{
							listBuf = staBuf;
							for(i=0; i<entries; i++)
							{
								if ( (pStaInfo = extStaDb_GetStaInfo(vmacSta_p, (IEEEtypes_MacAddr_t *)listBuf, 0)) != NULL )
								{
									if ((pStaInfo->State == ASSOCIATED) && (pStaInfo->ClientMode == NONLY_MODE))
									{
										UINT8 enable = 1;
										UINT8 mode =*(mib->mib_psHtManagementAct);

										if (mode == 3)
										{
											enable = 0;
											mode = 0;
										}
										wlFwSetMimoPsHt(dev, listBuf, enable, mode);
									}
									listBuf += sizeof(STA_INFO);
								}
							}
						}
					} 
					kfree(staBuf);
				}
				break;
			default:
				rc = -EINVAL;
				break;
			}
		}   
		break ;
#ifdef AMPDU_SUPPORT
	case WL_PARAM_AMPDU_TX:
		{
			switch (value)
			{
			case 0:
			case 1:
			case 2:
			case 3:
#ifndef AMPDU_SUPPORT_TX_CLIENT
				if (vmacSta_p->VMacEntry.modeOfService == VMAC_MODE_CLNT_INFRA)
					*(mib->mib_AmpduTx) = 0;
				else
#endif
					*(mib->mib_AmpduTx) = (UINT8 )value;
				if(*(mib->mib_AmpduTx))
				{
					*(mib->pMib_11nAggrMode) |= WL_MODE_AMPDU_TX;
				}
				else
				{
					*(mib->pMib_11nAggrMode) &= ~WL_MODE_AMPDU_TX;
				}
				break;
			default:
				rc = -EINVAL;
				break;
			}
		}   
		break ;
#endif
	case WL_PARAM_TXQLIMIT:
		{
			vmacSta_p->txQLimit = value;
		}   
		break ;
	case WL_PARAM_RXINTLIMIT:
		{
			vmacSta_p->work_to_do = value;
		}   
		break ;
#if defined ( INTOLERANT40) ||defined (COEXIST_20_40_SUPPORT)

	case WL_PARAM_INTOLERANT:
		{
			if (value < 0 || value > 1)
				rc = -EINVAL;

			*(mib->mib_HT40MIntoler) = (UINT8 )value;
		}   
		break ;
#endif
#ifdef CLIENT_SUPPORT
	case WL_PARAM_STAMODE:
		{
			vmacEntry_t  *vmacEntry_p     = NULL;
			struct net_device *staDev      = NULL;
			struct wlprivate  *stapriv     = NULL;
			vmacApInfo_t      *vmacSta_p   = NULL;
			if (value < 0)
			{
				rc = -EOPNOTSUPP;
				break;
			}
			*(mib->mib_STAMode) = (UCHAR)value;
			vmacEntry_p = sme_GetParentVMacEntry(((vmacApInfo_t *) priv->vmacSta_p)->VMacEntry.phyHwMacIndx);
			staDev = (struct net_device *)vmacEntry_p->privInfo_p;
			stapriv = NETDEV_PRIV_P(struct wlprivate, staDev);
			vmacSta_p = stapriv->vmacSta_p;
			wlset_mibChannel(vmacEntry_p , *(mib->mib_STAMode));
		}
		break;
#endif
#ifdef CLIENT_SUPPORT
	case WL_PARAM_STASCAN:
		{
			UINT8   bcAddr1[6]={0xff, 0xff, 0xff, 0xff, 0xff, 0xff};  /* BROADCAST BSSID */
			UINT8   ieBuf[2+IEEE_80211_MAX_NUMBER_OF_CHANNELS];
			UINT16  ieBufLen = 0;
			IEEEtypes_InfoElementHdr_t *IE_p;
			vmacEntry_t  *vmacEntry_p = NULL;
			struct net_device *staDev      = NULL;
			struct wlprivate  *stapriv     = NULL;
			vmacApInfo_t      *vmacSta_p   = NULL;
			MIB_802DOT11      *mib         = NULL;
			UINT8  mlmeAssociatedFlag;
			UINT8  mlmeBssid[6];
			UINT8   currChnlIndex=0;
			UINT8   chnlListLen= 0;
			UINT8   chnlScanList[IEEEtypes_MAX_CHANNELS+IEEEtypes_MAX_CHANNELS_A];
			UINT8   i=0;
			MIB_PHY_DSSS_TABLE *PhyDSSSTable;
			UINT8   mainChnlList[IEEEtypes_MAX_CHANNELS+IEEEtypes_MAX_CHANNELS_A];
			if (value != 1)
			{
				rc = -EINVAL;
				break;
			}

			vmacEntry_p = sme_GetParentVMacEntry(((vmacApInfo_t *) priv->vmacSta_p)->VMacEntry.phyHwMacIndx);
			staDev = (struct net_device *)vmacEntry_p->privInfo_p;
			stapriv = NETDEV_PRIV_P(struct wlprivate, staDev);
			vmacSta_p = stapriv->vmacSta_p;
			mib = vmacSta_p->Mib802dot11;	
			//when this command issued on AP mode, system would crash because of no STA interface
			//so the following checking is necessary.
			if(*(mib->mib_STAMode) == CLIENT_MODE_DISABLE)
			{
				rc = -EOPNOTSUPP;
				break;
			}

			memset(&mainChnlList[0], 0, (IEEEtypes_MAX_CHANNELS+IEEEtypes_MAX_CHANNELS_A));
			memset(&chnlScanList[0], 0, (IEEEtypes_MAX_CHANNELS+IEEEtypes_MAX_CHANNELS_A));

			PhyDSSSTable=mib->PhyDSSSTable;

			/* Stop Autochannel on AP first */
			StopAutoChannel(vmacSta_p);

			/* get range to scan */
			domainGetInfo(mainChnlList);

			if((*(vmacSta_p->Mib802dot11->mib_STAMode) == CLIENT_MODE_AUTO) || 
				(*(vmacSta_p->Mib802dot11->mib_STAMode) == CLIENT_MODE_N))
			{
				for (i=0; i < IEEEtypes_MAX_CHANNELS; i++)
				{
					if (mainChnlList[i] > 0)
					{
						chnlScanList[currChnlIndex] = mainChnlList[i];
						currChnlIndex ++;
					}
				}

				for (i=0; i < IEEEtypes_MAX_CHANNELS_A; i++)
				{
					if (mainChnlList[i+IEEEtypes_MAX_CHANNELS] > 0)
					{
						chnlScanList[currChnlIndex] = mainChnlList[i+IEEEtypes_MAX_CHANNELS];
						currChnlIndex ++;
					}
				}
				chnlListLen = currChnlIndex;
			}
			else if(*(vmacSta_p->Mib802dot11->mib_STAMode) == CLIENT_MODE_N_24)
			{
				for ( i=0; i < IEEEtypes_MAX_CHANNELS; i++ ) {
					chnlScanList[i] = mainChnlList[i];
				}
				chnlScanList[i] = 0;
				chnlListLen = IEEEtypes_MAX_CHANNELS;
			}
			else if (*(vmacSta_p->Mib802dot11->mib_STAMode) == CLIENT_MODE_N_5)
			{
				for ( i=0; i < IEEEtypes_MAX_CHANNELS_A; i++ ) {
					chnlScanList[i] = mainChnlList[i+IEEEtypes_MAX_CHANNELS];
				}
				chnlScanList[i] = 0;
				chnlListLen = IEEEtypes_MAX_CHANNELS_A;
			}

			ieBufLen = 0;
			/* Build IE Buf */
			IE_p = (IEEEtypes_InfoElementHdr_t *)&ieBuf[ieBufLen];

			/* SSID element */
			/* For scan all SSIDs to be scanned */

			/* DS_PARAM_SET element */
			IE_p->ElementId = DS_PARAM_SET;
			IE_p->Len = chnlListLen;
			ieBufLen += sizeof(IEEEtypes_InfoElementHdr_t);
			memcpy((char *)&ieBuf[ieBufLen], &chnlScanList[0],chnlListLen);

			ieBufLen += IE_p->Len;
			IE_p = (IEEEtypes_InfoElementHdr_t *)&ieBuf[ieBufLen];

			if((vmacEntry_p = sme_GetParentVMacEntry(((vmacApInfo_t *) priv->vmacSta_p)->VMacEntry.phyHwMacIndx)) == NULL)
			{
				rc = -EFAULT;
				break;
			}

			if(!smeGetStaLinkInfo(vmacEntry_p->id, &mlmeAssociatedFlag, 
				&mlmeBssid[0]))
			{
				rc = -EFAULT;
				break;
			}

			/* Set a flag indicating usr initiated scan */
			vmacSta_p->gUserInitScan = TRUE;

			if (!mlmeAssociatedFlag && (staDev->flags & IFF_RUNNING))
			{
				//printk("stopping BSS \n");
				linkMgtStop(vmacEntry_p->phyHwMacIndx);
				smeStopBss(vmacEntry_p->phyHwMacIndx);
			}

			if (smeSendScanRequest(vmacEntry_p->phyHwMacIndx, 0, 3, 200, &bcAddr1[0],
				&ieBuf[0], ieBufLen) == MLME_SUCCESS)
			{
				/*set the busy scanning flag */
				vmacSta_p->busyScanning = 1;
				break;
			}
			else
			{
				/* Reset a flag indicating usr initiated scan */
				vmacSta_p->gUserInitScan = FALSE;
				rc = -EALREADY;
				break;
			}
		}
		break;
#endif
#ifdef MPRXY
	case WL_PARAM_MCASTPRXY:
		{
			if (value < 0 || value > 1)
			{
				rc = -EOPNOTSUPP;
				break;
			}
			*(mib->mib_MCastPrxy) = (UCHAR)value;

			/*mcast proxy is turned on*/
			if (*(mib->mib_MCastPrxy))
			{
				/*If mib is same as default,  use 10 to set limit*/
				if (*(mib->mib_consectxfaillimit)== CONSECTXFAILLIMIT)
				{
					*(mib->mib_consectxfaillimit) = _CONSECTXFAILLIMIT;
					wlFwSetConsecTxFailLimit(dev, _CONSECTXFAILLIMIT);				
				}
					
			}
			else 
			{	/*Set back to default value*/
				if (*(mib->mib_consectxfaillimit)== _CONSECTXFAILLIMIT)
				{
					*(mib->mib_consectxfaillimit) = CONSECTXFAILLIMIT;
					wlFwSetConsecTxFailLimit(dev, CONSECTXFAILLIMIT);
				}
			}
		}
		break;
#endif
#ifdef RXPATHOPT
	case WL_PARAM_RXPATHOPT:
		if (value < 0 || value > 1500)
		{
			rc = -EOPNOTSUPP;
			break;
		}
		*(mib->mib_RxPathOpt) = value;
		break;
#endif
	case WL_PARAM_HTGF:
		if (value < 0 || value > 1)
		{
			rc = -EOPNOTSUPP;
			break;
		}
#ifdef SOC_W8363
		rc = -EOPNOTSUPP;
#else
		*(mib->mib_HtGreenField) = value;
#endif
		break;

	case WL_PARAM_HTSTBC:
		if (value < 0 || value > 1)
		{
			rc = -EOPNOTSUPP;
			break;
		}
#if defined(SOC_W8366) || defined (SOC_W8764)
		//currently, W8366 supports stbc. 
		*(mib->mib_HtStbc) = value;
#else
		rc = -EOPNOTSUPP;
#endif
		break;

	case WL_PARAM_3X3RATE:
		if (value < 0 || value > 1)
		{
			rc = -EOPNOTSUPP;
			break;
		}
#if defined(SOC_W8366)||defined(SOC_W8764)
		*(mib->mib_3x3Rate) = value;
#else
		rc = -EOPNOTSUPP;
#endif
		break;
	case WL_PARAM_AMSDU_FLUSHTIME:
		*(mib->mib_amsdu_flushtime)=value;
		break;
	case WL_PARAM_AMSDU_MAXSIZE:
		*(mib->mib_amsdu_maxsize)=value;
		break;
	case WL_PARAM_AMSDU_ALLOWSIZE:
		*(mib->mib_amsdu_allowsize)=value;
		break;
	case WL_PARAM_AMSDU_PKTCNT:
		*(mib->mib_amsdu_pktcnt)=value;
		break;
	default:
		WLDBG_INFO(DBG_LEVEL_1,  "%s: get_mp31ep_param: unknown param %d\n", dev->name, param);
		rc = -EOPNOTSUPP;
		break;

	}

	WLDBG_EXIT(DBG_LEVEL_1);

	return rc;
}

static int wlioctl_priv_get_wlparam(struct net_device *dev,
struct iw_request_info *info,
	void *wrqu, char *extra)
{
	struct wlprivate *priv = NETDEV_PRIV_P(struct wlprivate, dev);
	vmacApInfo_t *vmacSta_p = priv->vmacSta_p;
	MIB_802DOT11 *mib = vmacSta_p->ShadowMib802dot11;
	int *param = (int *) extra;
	int rc = 0;
#ifdef CLIENT_SUPPORT
	UINT8 AssociatedFlag = 0;
	UINT8 bssId[6];    
	STA_SYSTEM_MIBS *pStaSystemMibs ;
	vmacEntry_t  *vmacEntry_p = NULL;
#endif //CLIENT_SUPPORT
	MIB_PHY_DSSS_TABLE *PhyDSSSTable=mib->PhyDSSSTable;
	MIB_SPECTRUM_MGMT	*mib_SpectrumMagament_p=mib->SpectrumMagament;

	WLDBG_ENTER(DBG_LEVEL_1);

	switch (*param)
	{
	case WL_PARAM_AUTHTYPE:
		*param = mib->AuthAlg->Type;
		break;

	case WL_PARAM_BAND:
		*param = *(mib->mib_ApMode);
		break;

	case WL_PARAM_REGIONCODE:
		*param = domainGetDomain();
		break;

	case WL_PARAM_HIDESSID:
		if (*(mib->mib_broadcastssid))
			*param = 0;
		else
			*param = 1;
		break;

	case WL_PARAM_PREAMBLE:
		switch(mib->StationConfig->mib_preAmble)
		{
		case PREAMBLE_AUTO_SELECT:
			*param  = 0;
			break;
		case PREAMBLE_SHORT:
			*param  = 1;
			break;
		case PREAMBLE_LONG:
			*param  = 2;
			break;
		default:
			break;
		}
		break;

	case WL_PARAM_GPROTECT:
		if (*(mib->mib_forceProtectiondisable))
			*param = 0;
		else
			*param = 1;
		break;

	case WL_PARAM_BEACON:
		*param = le16_to_cpu(*(mib->mib_BcnPeriod));
		break;

	case WL_PARAM_DTIM:
		*param = mib->StationConfig->DtimPeriod;
		break;

	case WL_PARAM_FIXRATE:
		*param = *(mib->mib_enableFixedRateTx);
		break;

	case WL_PARAM_ANTENNA:
		*param = *(mib->mib_rxAntenna);
		break;

	case WL_PARAM_ANTENNATX:
		*param = *(mib->mib_txAntenna);
		break;

	case WL_PARAM_FILTER:
		*param = *mib->mib_wlanfiltertype;
		break;

	case WL_PARAM_WMM:
		*param =*(mib->QoSOptImpl);
		break;

	case WL_PARAM_WPAWPA2MODE:
		*param = *(mib->mib_wpaWpa2Mode);
		break;

#ifdef MRVL_WAPI
	case WL_PARAM_WAPIMODE:
		*param = mib->Privacy->WAPIEnabled;
		break;
#endif

	case WL_PARAM_GROUPREKEYTIME:
		*param = le32_to_cpu(mib->RSNConfig->GroupRekeyTime);
		break;

	case WL_PARAM_INTRABSS:
		*param = *(mib->mib_intraBSS);
		break;

	case WL_PARAM_AMSDU:
		*param = *(mib->pMib_11nAggrMode) & WL_MODE_AMSDU_TX_MASK;
		break;

	case WL_PARAM_HTBANDWIDTH:
		switch(PhyDSSSTable->Chanflag.ChnlWidth)
		{
		case CH_AUTO_WIDTH:
			*param=	0;
			break;
		case CH_10_MHz_WIDTH:
			*param=	1;
			break;
		case CH_20_MHz_WIDTH:
			*param=	2;
			break;
		case CH_40_MHz_WIDTH:
			*param=	3;
			break;				
		default:
			rc = -EOPNOTSUPP;
			break;				
		}
		break;

	case WL_PARAM_WMMACKPOLICY:
		*param=	*(mib->mib_wmmAckPolicy);
		break;

	case WL_PARAM_GUARDINTERVAL:
		*param= *(mib->mib_guardInterval);
		break;

	case WL_PARAM_EXTSUBCH:
		*param= *(mib->mib_extSubCh);
		break;

	case WL_PARAM_HTPROTECT:
		*param=	*(mib->mib_htProtect);
		break;

	case WL_PARAM_GETFWSTAT:
		wlFwGetHwStats(dev,NULL);
		break;

	case WL_PARAM_AGINGTIME:
		*param= *(mib->mib_agingtime);
		break;
	case WL_PARAM_ANTENNATX2:
		*param = *(mib->mib_txAntenna2);
		break;
	case WL_PARAM_CDD:
		*param = *(mib->mib_CDD);
		break;
	case WL_PARAM_ACS_THRESHOLD:
		*param = *(mib->mib_acs_threshold);
		break;
	case WL_PARAM_AUTOCHANNEL:
		*param = *(mib->mib_autochannel);
		break;
	case WL_PARAM_AMPDUFACTOR:
		*param = *(mib->mib_ampdu_factor);
		break;
	case WL_PARAM_AMPDUDENSITY:
		*param = *(mib->mib_ampdu_density);
		break;
#ifdef INTEROP
	case WL_PARAM_INTEROP:
		*param  = *(mib->mib_interop);
		break;
#endif
	case WL_PARAM_CARDDEVINFO:
		{
			*param  = priv->wlpd_p->CardDeviceInfo;

		}break;
	case WL_PARAM_OPTLEVEL:
		*param = *(mib->mib_optlevel);
		break;
	case WL_PARAM_REGIONPWR:
		*param = *(mib->mib_MaxTxPwr);
		break;
	case WL_PARAM_ADAPTMODE:
		*param = *(mib->mib_RateAdaptMode);
		break;
	case WL_PARAM_CSADAPTMODE:
		*param = *(mib->mib_CSMode);
		break;
#ifdef IEEE80211_DH
	case WL_PARAM_11H_CSA_CHAN:
		*param = mib_SpectrumMagament_p->csaChannelNumber ;
		break;
	case WL_PARAM_11H_CSA_COUNT:
		*param = mib_SpectrumMagament_p->csaCount ;
		break;
	case WL_PARAM_11H_CSA_MODE:
		*param = mib_SpectrumMagament_p->csaMode ;
		break;
	case WL_PARAM_SPECTRUM_MGMT:
		*param = mib_SpectrumMagament_p->spectrumManagement ;
		break;
	case WL_PARAM_POWER_CONSTRAINT:
		*param = mib_SpectrumMagament_p->powerConstraint ;
		break;
	case WL_PARAM_11D_MODE :
		*param = mib_SpectrumMagament_p->multiDomainCapability ;
		break;
#ifdef CLIENT_SUPPORT
	case WL_PARAM_11H_STA_MODE :
		if((vmacEntry_p = sme_GetParentVMacEntry(vmacSta_p->VMacEntry.phyHwMacIndx)) != NULL)
		{
			pStaSystemMibs = sme_GetStaSystemMibsPtr(vmacEntry_p);
			if( pStaSystemMibs != NULL)
			{
				*param = pStaSystemMibs->mib_StaCfg_p->sta11hMode ;
			}
		}
		break;
#endif //CLIENT_SUPPORT
#endif //IEEE80211_DH
#ifdef WDS_FEATURE
	case WL_PARAM_WDSMODE:
		*param = *(mib->mib_wdsEnable);
		break;
#endif
	case WL_PARAM_DISABLEASSOC:
		*param = *(mib->mib_disableAssoc);
		break;
	case WL_PARAM_STRICTWEPSHARE:
		*param = *(mib->mib_strictWepShareKey);
		break;

#ifdef PWRFRAC
	case WL_PARAM_TXPWRFRACTION:
		*param = *(mib->mib_TxPwrFraction);
		break;
#endif

	case WL_PARAM_PSHT_MANAGEMENTACT:
		*param = *(mib->mib_psHtManagementAct);
		break;

#ifdef CLIENT_SUPPORT
	case WL_PARAM_STAMODE:
		*param = *(mib->mib_STAMode);
		break;
#endif
#ifdef AMPDU_SUPPORT
	case WL_PARAM_AMPDU_TX:
		*param = *(mib->mib_AmpduTx);
		break;
#endif
#ifdef MRVL_DFS
	case WL_PARAM_11HCACTIMEOUT:
		*param = *(mib->mib_CACTimeOut) ;
		break ;
	case WL_PARAM_11hNOPTIMEOUT:
		*param = *(mib->mib_NOPTimeOut) ;
		break ;
	case WL_PARAM_11hDFSMODE:
		if( priv->wlpd_p->pdfsApMain )
			*param = DfsGetCurrentState(priv->wlpd_p->pdfsApMain);
		else
			*param = 0 ;
		break ;
#endif //MRVL_DFS
	case WL_PARAM_TXQLIMIT:
		{
			*param = vmacSta_p->txQLimit;
		}
		break;
	case WL_PARAM_RXINTLIMIT:
		{
			*param = vmacSta_p->work_to_do;
		}
		break;
#ifdef INTOLERANT40
	case WL_PARAM_INTOLERANT:
		{
			*param = *(mib->mib_HT40MIntoler);
		}   
		break ;
#endif
#ifdef MPRXY
	case WL_PARAM_MCASTPRXY:
		*param = *(mib->mib_MCastPrxy);
		break;
#endif
	case WL_PARAM_RSSI:
		*param = -(*(mib->mib_Rssi));
		break;

#ifdef CLIENT_SUPPORT
	case WL_PARAM_LINKSTATUS:
		if((vmacEntry_p = sme_GetParentVMacEntry(vmacSta_p->VMacEntry.phyHwMacIndx)) != NULL)
		{
			vmacStaInfo_t *vStaInfo_p = (vmacStaInfo_t *)vmacEntry_p->info_p; 		
						
			if(vStaInfo_p->staSecurityMibs.mib_PrivacyTable_p->RSNEnabled)
			{
				*param = vStaInfo_p->staSecurityMibs.mib_PrivacyTable_p->RSNLinkStatus;	
			}
			else
			{
				smeGetStaLinkInfo(vmacEntry_p->id,
				&AssociatedFlag,
				&bssId[0]);
				*param = AssociatedFlag;
			}
		}
		break;
#endif
#ifdef RXPATHOPT
	case WL_PARAM_RXPATHOPT:
		*param = *(mib->mib_RxPathOpt);
		break;
#endif
	case WL_PARAM_HTGF:
		*param = *(mib->mib_HtGreenField);
		break;

	case WL_PARAM_HTSTBC:
		*param = *(mib->mib_HtStbc);
		break;

	case WL_PARAM_3X3RATE:
		*param = *(mib->mib_3x3Rate);
		break;
	case WL_PARAM_AMSDU_FLUSHTIME:
		*param = *(mib->mib_amsdu_flushtime);
		break;
	case WL_PARAM_AMSDU_MAXSIZE:
		*param = *(mib->mib_amsdu_maxsize);
		break;
	case WL_PARAM_AMSDU_ALLOWSIZE:
		*param = *(mib->mib_amsdu_allowsize);
		break;
	case WL_PARAM_AMSDU_PKTCNT:
		*param = *(mib->mib_amsdu_pktcnt);
		break;
	default:
		WLDBG_INFO(DBG_LEVEL_1,  "%s: get_mp31ep_param: unknown param %d\n", dev->name, *param);
		rc = -EOPNOTSUPP;
		break;
	}

	WLDBG_EXIT(DBG_LEVEL_1);
	return rc;
}

static int wlioctl_priv_bss_start(struct net_device *dev,
struct iw_request_info *info,
	void *wrqu, char *extra)
{
	int rc = 0;

	WLDBG_ENTER_INFO(DBG_LEVEL_1, "");
	if(wlFwApplySettings(dev))
		return -EIO;
	WLDBG_EXIT(DBG_LEVEL_1);

	return rc;
}

static const iw_handler	wlhandler[] =
{
	(iw_handler) wlconfig_commit,	/* SIOCSIWCOMMIT */
	(iw_handler) wlget_name,		/* SIOCGIWNAME */
	(iw_handler) NULL,						/* SIOCSIWNWID */
	(iw_handler) NULL,						/* SIOCGIWNWID */
	(iw_handler) wlset_freq,			/* SIOCSIWFREQ */
	(iw_handler) wlget_freq,			/* SIOCGIWFREQ */
	(iw_handler) NULL,						/* SIOCSIWMODE */
	(iw_handler) NULL,						/* SIOCGIWMODE */
	(iw_handler) NULL, 					/* SIOCSIWSENS */
	(iw_handler) wlget_sens, 			/* SIOCGIWSENS */
	(iw_handler) NULL,						/* SIOCSIWRANGE */
	(iw_handler) wlget_range,			/* SIOCGIWRANGE */
	(iw_handler) NULL,						/* SIOCSIWPRIV */
	(iw_handler) NULL,						/* SIOCGIWPRIV */
	(iw_handler) NULL,						/* SIOCSIWSTATS */
	(iw_handler) wlget_stats,			/* SIOCGIWSTATS */
#if WIRELESS_EXT > 15
	iw_handler_set_spy,					/* SIOCSIWSPY */
	iw_handler_get_spy,					/* SIOCGIWSPY */
	iw_handler_set_thrspy,					/* SIOCSIWTHRSPY */
	iw_handler_get_thrspy,					/* SIOCGIWTHRSPY */
#else /* WIRELESS_EXT > 15 */
	(iw_handler) NULL,						/* SIOCSIWSPY */
	(iw_handler) NULL,						/* SIOCGIWSPY */
	(iw_handler) NULL,						/* -- hole -- */
	(iw_handler) NULL,						/* -- hole -- */
#endif /* WIRELESS_EXT > 15 */
	(iw_handler) wlset_bssid,				/* SIOCSIWAP */
	(iw_handler) wlget_wap,			/* SIOCGIWAP */
	(iw_handler) NULL,						/* -- hole -- */
	(iw_handler) NULL,						/* SIOCGIWAPLIST */
	(iw_handler) wlset_scan,						/* SIOCSIWSCAN */
	(iw_handler) wlget_scan,						/* SIOCGIWSCAN */
	(iw_handler) wlset_essid,			/* SIOCSIWESSID */
	(iw_handler) wlget_essid,			/* SIOCGIWESSID */
	(iw_handler) NULL,						/* SIOCSIWNICKN */
	(iw_handler) NULL,						/* SIOCGIWNICKN */
	(iw_handler) NULL,						/* -- hole -- */
	(iw_handler) NULL,						/* -- hole -- */
	(iw_handler) NULL,						/* SIOCSIWRATE */
	(iw_handler) NULL,						/* SIOCGIWRATE */
	(iw_handler) wlset_rts,			/* SIOCSIWRTS */
	(iw_handler) wlget_rts,			/* SIOCGIWRTS */
	(iw_handler) NULL,						/* SIOCSIWFRAG */
	(iw_handler) wlget_frag,			/* SIOCGIWFRAG */
	(iw_handler) NULL,						/* SIOCSIWTXPOW */
	(iw_handler) NULL,						/* SIOCGIWTXPOW */
	(iw_handler) NULL,						/* SIOCSIWRETRY */
	(iw_handler) NULL,						/* SIOCGIWRETRY */
	(iw_handler) wlset_encode,		/* SIOCSIWENCODE */
	(iw_handler) wlget_encode,		/* SIOCGIWENCODE */
	(iw_handler) NULL,						/* SIOCSIWPOWER */
	(iw_handler) NULL,						/* SIOCGIWPOWER */
};

static const iw_handler	wlprivate_handler[] =
{
	/* SIOCIWFIRSTPRIV + */
	(iw_handler) wlioctl_priv_wlparam,		/* 0 */
	(iw_handler) wlioctl_priv_get_wlparam, 	/* 1 */
	(iw_handler) wlioctl_priv_bss_start,			/* 2 */
	(iw_handler) NULL, 
	(iw_handler) NULL,
	(iw_handler) NULL,
	(iw_handler) NULL,
	(iw_handler) NULL,
	(iw_handler) NULL,
	(iw_handler) NULL,
	//(iw_handler) wlset_staMacFilter, 
	//(iw_handler) wlset_staMacFilter,

};


static const struct iw_priv_args wlprivate_args[] = 
{
	//{ WL_IOCTL_BSS_START, 0, 0, "bssstart" },
	{ WL_IOCTL_GET_VERSION, 0, IW_PRIV_TYPE_CHAR | 128, "version" },
	{ WL_IOCTL_SET_TXRATE, IW_PRIV_TYPE_CHAR | 128, 0, "txrate" },
	{ WL_IOCTL_GET_TXRATE, 0, IW_PRIV_TYPE_CHAR | 128, "gettxrate" },
	{ WL_IOCTL_SET_CIPHERSUITE, IW_PRIV_TYPE_CHAR | 128, 0, "ciphersuite" },
	{ WL_IOCTL_GET_CIPHERSUITE, 0, IW_PRIV_TYPE_CHAR | 128, "getciphersuite" },
	{ WL_IOCTL_SET_PASSPHRASE, IW_PRIV_TYPE_CHAR | 128, 0, "passphrase" },
	{ WL_IOCTL_GET_PASSPHRASE, 0, IW_PRIV_TYPE_CHAR | 128, "getpassphrase" },
	{ WL_IOCTL_SET_FILTERMAC, IW_PRIV_TYPE_CHAR | 128, 0, "filtermac" }, 
	{ WL_IOCTL_GET_FILTERMAC, 0, IW_PRIV_TYPE_CHAR | 2560, "getfiltermac" },
	{ WL_IOCTL_SET_WMMEDCAAP, IW_PRIV_TYPE_CHAR | 128, 0, "wmmedcaap" },
	{ WL_IOCTL_GET_WMMEDCAAP, 0, IW_PRIV_TYPE_CHAR | 128, "getwmmedcaap" },
	{ WL_IOCTL_SET_WMMEDCASTA, IW_PRIV_TYPE_CHAR | 128, 0, "wmmedcasta" },
	{ WL_IOCTL_GET_WMMEDCASTA, 0, IW_PRIV_TYPE_CHAR | 128, "getwmmedcasta" },
	{ WL_IOCTL_SET_BSSID, IW_PRIV_TYPE_CHAR | 128, 0, "bssid" },
	{ WL_IOCTL_GET_BSSID, 0, IW_PRIV_TYPE_CHAR | 128, "getbssid" },
	{ WL_IOCTL_SET_CLIENT, IW_PRIV_TYPE_CHAR | 128, 0, "macclone" },

	{ WL_IOCTL_GET_STALISTEXT, 0, IW_PRIV_TYPE_CHAR | 2560, "getstalistext" },
	{ WL_IOCTL_SET_TXPOWER, IW_PRIV_TYPE_CHAR | 128, 0, "txpower" },
	{ WL_IOCTL_GET_TXPOWER, 0, IW_PRIV_TYPE_CHAR | 128, "gettxpower" },
	{ WL_IOCTL_GETCMD, IW_PRIV_TYPE_CHAR | 128 , IW_PRIV_TYPE_CHAR | 1024, "getcmd" },
	{ WL_IOCTL_SET_WDS_PORT, IW_PRIV_TYPE_CHAR | 128, 0, "setwds" },
	{ WL_IOCTL_GET_WDS_PORT, 0, IW_PRIV_TYPE_CHAR | 128, "getwds" },
	{ WL_IOCTL_SETCMD, IW_PRIV_TYPE_CHAR | 1536, 0, "setcmd" },
	{ WL_IOCTL_GET_STASCAN, 0, IW_PRIV_TYPE_CHAR | 2560, "getstascan" },

	/* --- sub-ioctls handlers --- */
	{ WL_IOCTL_WL_PARAM,
	IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "" },
	{ WL_IOCTL_WL_GET_PARAM,
	0, IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, "" },
	/* --- sub-ioctls definitions --- */
	{ WL_PARAM_BAND,
	IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "opmode" },
	{ WL_PARAM_BAND,
	0, IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, "getopmode" },
	{ WL_PARAM_REGIONCODE,
	IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "regioncode" },
	{ WL_PARAM_REGIONCODE,
	0, IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, "getregioncode" },
	{ WL_PARAM_HIDESSID,
	IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "hidessid" },
	{ WL_PARAM_HIDESSID,
	0, IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, "gethidessid" },
	{ WL_PARAM_PREAMBLE,
	IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "preamble" },
	{ WL_PARAM_PREAMBLE,
	0, IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, "getpreamble" },
	{ WL_PARAM_GPROTECT,
	IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "gprotect" },
	{ WL_PARAM_GPROTECT,
	0, IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, "getgprotect" },
	{ WL_PARAM_BEACON,
	IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "bcninterval" },
	{ WL_PARAM_BEACON,
	0, IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, "getbcninterval" },
	{ WL_PARAM_DTIM,
	IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "dtim" },
	{ WL_PARAM_DTIM,
	0, IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, "getdtim" },
	{ WL_PARAM_FIXRATE,
	IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "fixrate" },
	{ WL_PARAM_FIXRATE, 
	0, IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, "getfixrate" },
	{ WL_PARAM_ANTENNA,
	IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "rxantenna" },
	{ WL_PARAM_ANTENNA,
	0, IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, "getrxantenna" },
	{ WL_PARAM_WPAWPA2MODE,
	IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "wpawpa2mode" },
	{ WL_PARAM_WPAWPA2MODE,
	0, IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, "getwpawpa2mode" },
#ifdef MRVL_WAPI
	{ WL_PARAM_WAPIMODE,
	IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "wapimode" },
	{ WL_PARAM_WAPIMODE,
	0, IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, "getwapimode" },
#endif
	{ WL_PARAM_GROUPREKEYTIME,
	IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "grouprekey" },
	{ WL_PARAM_GROUPREKEYTIME,
	0, IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, "getgrouprekey" },
	{ WL_PARAM_WMM,
	IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "wmm" },
	{ WL_PARAM_WMM,
	0, IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, "getwmm" },
	{ WL_PARAM_FILTER,
	IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "filter" },
	{ WL_PARAM_FILTER,
	0, IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, "getfilter" },
	{ WL_PARAM_INTRABSS,
	IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "intrabss" },
	{ WL_PARAM_INTRABSS,
	0, IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, "getintrabss" },
	{ WL_PARAM_AMSDU,
	IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "amsdu" },
	{ WL_PARAM_AMSDU,
	0, IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, "getamsdu" },
	{ WL_PARAM_HTBANDWIDTH,
	IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "htbw" },
	{ WL_PARAM_HTBANDWIDTH,
	0, IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, "gethtbw" },
	{ WL_PARAM_WMMACKPOLICY,
	IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "wmmackpolicy" },
	{ WL_PARAM_WMMACKPOLICY,
	0, IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, "getwmmackpolicy" },
	{ WL_PARAM_GUARDINTERVAL,
	IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "guardint" },
	{ WL_PARAM_GUARDINTERVAL,
	0, IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, "getguardint" },
	{ WL_PARAM_EXTSUBCH,
	IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "extsubch" },
	{ WL_PARAM_EXTSUBCH,
	0, IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, "getextsubch" },
	{ WL_PARAM_HTPROTECT,
	IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "htprotect" },
	{ WL_PARAM_HTPROTECT,
	0, IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, "gethtprotect" },
	{ WL_PARAM_GETFWSTAT,
	0, IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, "getfwstat" },
	{ WL_PARAM_AGINGTIME,
	IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "agingtime" },
	{ WL_PARAM_AGINGTIME,
	0, IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, "getagingtime" },
	{ WL_PARAM_ANTENNATX2,
	IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "txantenna2" },
	{ WL_PARAM_ANTENNATX2,
	0, IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, "gettxantenna2" },
	{ WL_PARAM_AUTOCHANNEL,
	IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "autochannel" },
	{ WL_PARAM_AUTOCHANNEL,
	0, IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, "getautochannel" },
	{ WL_PARAM_AMPDUFACTOR,
	IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "ampdufactor" },
	{ WL_PARAM_AMPDUFACTOR,
	0, IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, "getampdufactor" },
	{ WL_PARAM_AMPDUDENSITY,
	IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "ampduden" },
	{ WL_PARAM_AMPDUDENSITY,
	0, IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, "getampduden" },
	{ WL_PARAM_CARDDEVINFO,
	0, IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, "getdeviceinfo" },
#ifdef INTEROP
	{ WL_PARAM_INTEROP,
	IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "interop" },
	{ WL_PARAM_INTEROP,
	0, IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, "getinterop" },
#endif
	{ WL_PARAM_OPTLEVEL,
	IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "optlevel" },
	{ WL_PARAM_OPTLEVEL,
	0, IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, "getoptlevel" },
	{ WL_PARAM_REGIONPWR,
	IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "maxtxpower" },
	{ WL_PARAM_REGIONPWR,
	0, IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, "getmaxtxpower" },
	{ WL_PARAM_ADAPTMODE,
	IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "ratemode" },
	{ WL_PARAM_ADAPTMODE,
	0, IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, "getratemode" },
	{ WL_PARAM_CSADAPTMODE,
	IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "csmode" },
	{ WL_PARAM_CSADAPTMODE,
	0, IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, "getcsmode" },
	{ WL_PARAM_DELWEPKEY,
	IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "delwepkey" },
	{ WL_PARAM_WDSMODE,
	IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "wdsmode" },
	{ WL_PARAM_WDSMODE,
	0, IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, "getwdsmode" },
	{ WL_PARAM_STRICTWEPSHARE,
	IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "strictshared" },
	{ WL_PARAM_STRICTWEPSHARE,
	0, IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, "getstrictshared" },
	{ WL_PARAM_DISABLEASSOC,
	IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "disableassoc" },
	{ WL_PARAM_DISABLEASSOC,
	0, IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, "getdisableassoc" },
	{ WL_PARAM_11H_DFS_MODE,
	IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "11hdfsmode" },
	{ WL_PARAM_11H_CSA_CHAN,
	IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "11hcsachan" },
	{ WL_PARAM_11H_CSA_CHAN,
	0, IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, "get11hcsachan" },
	{ WL_PARAM_11H_CSA_COUNT,
	IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "11hcsacount" },
	{ WL_PARAM_11H_CSA_COUNT,
	0, IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, "get11hcsacount" },
	{ WL_PARAM_11H_CSA_MODE,
	IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "11hcsamode" },
	{ WL_PARAM_11H_CSA_MODE,
	0, IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, "get11hcsamode" },
	{ WL_PARAM_11H_CSA_START,
	IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "11hcsastart" },
	{ WL_PARAM_SPECTRUM_MGMT,
	IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "11hspecmgt" },
	{ WL_PARAM_SPECTRUM_MGMT,
	0, IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, "get11hspecmgt" },
	{ WL_PARAM_POWER_CONSTRAINT,
	IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "11hpwrconstr" },
	{ WL_PARAM_POWER_CONSTRAINT,
	0, IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, "get11hpwrconstr" },
	{ WL_PARAM_11D_MODE,
	IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "11dmode" },
	{ WL_PARAM_11D_MODE,
	0, IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, "get11dmode" },
	{ WL_PARAM_11H_STA_MODE,
	IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "11hstamode" },
	{ WL_PARAM_11H_STA_MODE,
	0, IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, "get11hstamode" },
	{ WL_PARAM_TXPWRFRACTION,
	IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "pwrfraction" },
	{ WL_PARAM_TXPWRFRACTION,
	0, IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, "getpwrfraction" },
	{ WL_PARAM_PSHT_MANAGEMENTACT,
	IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "mimops" },
	{ WL_PARAM_PSHT_MANAGEMENTACT,
	0, IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, "getmimops" },
	{ WL_PARAM_STAMODE,
	IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "stamode" },
	{ WL_PARAM_STAMODE,
	0, IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, "getstamode" },
	{ WL_PARAM_STASCAN,
	IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "stascan" },
	{ WL_PARAM_AMPDU_TX,
	IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "ampdutx" },
	{ WL_PARAM_AMPDU_TX,
	0, IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, "getampdutx" },
	{ WL_PARAM_11HCACTIMEOUT,
	IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "11hCACTimeOut" },
	{ WL_PARAM_11HCACTIMEOUT,
	0, IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, "get11hCACTout" },
	{ WL_PARAM_11hNOPTIMEOUT,
	IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "11hNOPTimeOut" },
	{ WL_PARAM_11hNOPTIMEOUT,
	0, IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, "get11hNOPTout" },
	{ WL_PARAM_11hDFSMODE,
	0, IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, "get11hDFSMode" },
	{ WL_PARAM_TXQLIMIT,
	IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "txqlimit" },
	{ WL_PARAM_TXQLIMIT,
	0, IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, "gettxqlimit" },
	{ WL_PARAM_RXINTLIMIT,
	IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "rxintlimit" },
	{ WL_PARAM_RXINTLIMIT,
	0, IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, "getrxintlimit" },
	{ WL_PARAM_INTOLERANT,
	IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "intoler" },
	{ WL_PARAM_INTOLERANT,
	0, IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, "getintoler" },
	{ WL_PARAM_MCASTPRXY,
	IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "mcastproxy" },
	{ WL_PARAM_MCASTPRXY,
	0, IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, "getmcastproxy" },
	{ WL_PARAM_RSSI,
	0, IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, "getrssi" },
	{ WL_PARAM_LINKSTATUS,
	0, IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, "getlinkstatus" },
	{ WL_PARAM_ANTENNATX,
	IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "txantenna" },
	{ WL_PARAM_ANTENNATX,
	0, IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, "gettxantenna" },
	{ WL_PARAM_RXPATHOPT,
	IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "rxpathopt" },
	{ WL_PARAM_RXPATHOPT,
	0, IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, "getrxpathopt" },
	{ WL_PARAM_HTGF,
	IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "htgf" },
	{ WL_PARAM_HTGF,
	0, IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, "gethtgf" },
	{ WL_PARAM_HTSTBC,
	IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "htstbc" },
	{ WL_PARAM_HTSTBC,
	0, IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, "gethtstbc" },
	{ WL_PARAM_3X3RATE,
	IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "3x3rate" },
	{ WL_PARAM_3X3RATE,
	0, IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, "get3x3rate" },
	{ WL_PARAM_AMSDU_FLUSHTIME,
	IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "amsduft" },
	{ WL_PARAM_AMSDU_FLUSHTIME,
	0, IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, "getamsduft" },
	{ WL_PARAM_AMSDU_MAXSIZE,
	IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "amsdums" },
	{ WL_PARAM_AMSDU_MAXSIZE,
	0, IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, "getamsdums" },
	{ WL_PARAM_AMSDU_ALLOWSIZE,
	IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "amsduas" },
	{ WL_PARAM_AMSDU_ALLOWSIZE,
	0, IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, "getamsduas" },
	{ WL_PARAM_AMSDU_PKTCNT,
	IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "amsdupc" },
	{ WL_PARAM_AMSDU_PKTCNT,
	0, IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, "getamsdupc" },
	{ WL_PARAM_CDD,
	IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "cdd" },
	{ WL_PARAM_CDD,
	0, IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, "getcdd" },
	{ WL_PARAM_ACS_THRESHOLD,
	IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, 0, "acsthrd" },
	{ WL_PARAM_ACS_THRESHOLD,
	0, IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, "getacsthrd" },
};


const struct iw_handler_def wlDefHandler =
{
num_standard:	sizeof(wlhandler)/sizeof(iw_handler),
num_private:	sizeof(wlprivate_handler)/sizeof(iw_handler),
num_private_args: sizeof(wlprivate_args)/sizeof(struct iw_priv_args),
standard:	(iw_handler *) wlhandler,
private:	(iw_handler *) wlprivate_handler,
private_args:	(struct iw_priv_args *) wlprivate_args,
};

int wlSetupWEHdlr(struct net_device *netdev)
{
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,20)
	netdev->get_wireless_stats = wlGetStats;
#endif
	netdev->wireless_handlers = (struct iw_handler_def *)&wlDefHandler;
	return 0;
}


static int atoi(const char *num_str)
{
	int val = 0;

	for (;; num_str++)
	{
		switch (*num_str)
		{
		case '0'...'9':
			val = 10*val+(*num_str-'0');
			break;
		default:
			return val;
		}
	}
}

static long atohex(const char *number)
{
	long   n = 0;

	if (*number == '0' && (*(number + 1) == 'x' || *(number + 1) == 'X') )
		number += 2;
	while (*number <= ' ' && *number > 0)
		++number;
	while ((*number>='0' && *number<='9') ||
		(*number>='A' && *number<='F') ||
		(*number>='a' && *number<='f'))
	{
		if (*number>='0' && *number<='9')
		{
			n = (n * 0x10) + ((*number++) - '0');
		} else if (*number>='A' && *number<='F')
		{
			n = (n * 0x10) + ((*number++) - 'A' + 10);
		} else/* if (*number>='a' && *number<='f')*/
		{
			n = (n * 0x10) + ((*number++) - 'a' + 10);
		}
	}
	return n;
}
long atohex2(const char *number)
{
	long   n = 0;

	while (*number <= ' ' && *number > 0)
		++number;
	if (*number == 0)
		return n;
	if (*number == '0' && (*(number + 1) == 'x' || *(number + 1) == 'X') )
		n = atohex(number+2);
	else
		n = atoi(number);
	return n;
}
static param_applicable priv_iocmd[] =
{
	{	WL_IOCTL_BSS_START			, 0},
	{	WL_IOCTL_SET_TXRATE			, 1},
	{	WL_IOCTL_SET_CIPHERSUITE	, 0},
	{	WL_IOCTL_SET_PASSPHRASE		, 0},
	{	WL_IOCTL_SET_FILTERMAC		, 0},
	{	WL_IOCTL_SET_BSSID			, 0},
	{	WL_IOCTL_SET_TXPOWER		, 1},
	{   WL_IOCTL_SET_APPIE			, 0},
	{   WL_IOCTL_SET_CLIENT			, 0},
	{ 	WL_IOCTL_SET_WDS_PORT       , 0},
};
int is_the_cmd_applicable(UINT16 cmd)
{
	int i;
	for (i =0; i<sizeof(priv_iocmd)/4;i++)
	{
		if(priv_iocmd[i].command == cmd)
			return priv_iocmd[i].applicable;
	}
	return 0;
}

int wlIoctlSet(struct net_device *netdev, int cmd, char *param_str, int param_len, char *ret_str, UINT16 *ret_len)
{
	struct wlprivate *priv = NETDEV_PRIV_P(struct wlprivate, netdev);
	vmacApInfo_t *vmacSta_p = priv->vmacSta_p;
	MIB_802DOT11 *mib = vmacSta_p->ShadowMib802dot11;
	int rc = 0;
	char *bufBack = cmdGetBuf;
#ifdef MRVL_WPS_CLIENT
	UINT8 desireBSSID[6];
	int count = 0 ;
	char    *ptr = NULL ;
#endif
	char (*param)[64] = kmalloc(sizeof(*param) * 65, GFP_KERNEL);


	WLDBG_ENTER(DBG_LEVEL_1);
	if(is_the_cmd_applicable(cmd) && priv->master){
		rc = -EOPNOTSUPP; 
		kfree(param);
		return rc; 
	} 

	if (ret_str != NULL)
	{
		ret_str[0] = cmdGetBuf[0] = '\0';
		*ret_len = 1;
	}
	memset(&param[0][0], 0, 65*64);
	switch (cmd)
	{
	case WL_IOCTL_SET_TXRATE:
		{
			int rate=2;
#ifdef BRS_SUPPORT
			UINT32 rateMask = 0;
			UCHAR i;
			UCHAR len = 0;
			UCHAR *ptr;

			/* get arg numbers */
			ptr = param_str;
			while ((*ptr != 0))
			{	

				while ((*ptr != ' ') && (*ptr != 0))
				{
					ptr++;
				}
				if (*ptr == 0)
					break;

				len++;

				while ((*ptr == ' ') && (*ptr != 0))
				{
					ptr++;	
				}
			}
			//printk("len %d\n", len);
#endif
			sscanf(param_str, "%s %s %s %s %s %s %s %s %s %s %s %s %s %s\n",  
				param[0], param[1], param[2], param[3], param[4], param[5], param[6], param[7], param[8], param[9], param[10], param[11], param[12], param[13]);

			rate = atoi(param[1]);

			if (strcmp(param[0], "b") == 0)
			{
				if (!rateChecked(rate, AP_MODE_B_ONLY))
				{
					rc = -EFAULT;
					break;
				}
				WLDBG_INFO(DBG_LEVEL_1,"%d\n", rate);

				*(mib->mib_txDataRate) = (UCHAR)rate;
			} else if (strcmp(param[0], "g") == 0)
			{
				if (!rateChecked(rate, AP_MODE_G_ONLY))
				{
					rc = -EFAULT;
					break;
				}
				WLDBG_INFO(DBG_LEVEL_1,"%d\n", rate);

				*(mib->mib_txDataRateG)= (UCHAR)rate;

			} else if (strcmp(param[0], "n") == 0)
			{
				if ((rate > 271) && !(*(mib->mib_3x3Rate)))
				{
					rc = -EFAULT;
					break;
				}
				if (!rateChecked(rate, AP_MODE_N_ONLY))
				{
					rc = -EFAULT;
					break;
				}
				WLDBG_INFO(DBG_LEVEL_1,"%d\n", rate);

				*(mib->mib_txDataRateN)= (UCHAR)rate;
				if (rate > 0xff)
					*(mib->mib_FixedRateTxType) = 1;
				else
					*(mib->mib_FixedRateTxType) = 0;

			} else if (strcmp(param[0], "a") == 0)
			{
				if (!rateChecked(rate, AP_MODE_A_ONLY))
				{
					rc = -EFAULT;
					break;
				}

				WLDBG_INFO(DBG_LEVEL_1,"%d\n", rate);

				*(mib->mib_txDataRateA)= (UCHAR)rate;

			} else if (strcmp(param[0], "mcbc") == 0)
			{
				if (rate > 0xff)
				{
					/* HT MCS rate */
					if (!rateChecked(rate, AP_MODE_N_ONLY))
					{
						rc = -EFAULT;
						break;
					}
					*(mib->mib_MultiRateTxType) = 1;
				}
				else /* G rate */
				{
					if (!rateChecked(rate, AP_MODE_G_ONLY))
					{
						rc = -EFAULT;
						break;
					}
					*(mib->mib_MultiRateTxType) = 0;
				}

				WLDBG_INFO(DBG_LEVEL_1,"%d\n", rate);

				*(mib->mib_MulticastRate)= (UCHAR)rate;

			} else if (strcmp(param[0], "mgt") == 0)
			{
				if (!rateChecked(rate, AP_MODE_G_ONLY))
				{
					rc = -EFAULT;
					break;
				}

				WLDBG_INFO(DBG_LEVEL_1,"%d\n", rate);

				*(mib->mib_ManagementRate)= (UCHAR)rate;
			} 
#ifdef BRS_SUPPORT		
			else if ((strcmp(param[0], "brs") == 0) || (strcmp(param[0], "srs") == 0))
			{
				if (len > 12)
				{
					rc = -EFAULT;
					break;
				}

				for (i=0; i< len; i++)
				{ 

					rate = atoi(param[1+i]);

					if (!rateChecked(rate, AP_MODE_G_ONLY) )
					{
						rc = -EFAULT;
						break;
					}
					IEEEToMrvlRateBitMapConversion((UCHAR)rate, &rateMask);
				}
				if (rc == -EFAULT)
					break;

				if (strcmp(param[0], "brs") == 0)
				{
					*(mib->BssBasicRateMask)	= rateMask;				
					(*(mib->NotBssBasicRateMask)) &= ~rateMask;
				}
				else
				{
					if ((rateMask | ~(*(mib->BssBasicRateMask))) & *(mib->BssBasicRateMask))
					{
						/* some basic rate is added */
						rc = -EFAULT;
						break;
					}
					*(mib->NotBssBasicRateMask)	= rateMask;
				} 
			}
#endif
#if defined(SOC_W8864)
			else if ((strcmp(param[0], "vht") == 0))
			{
			    rate = atohex2(param[1]);
                if (!rateChecked(rate, AP_MODE_11AC) )
                {
                    rc = -EFAULT;
                    break;
                }
			    *(mib->mib_txDataRateVHT)= (UCHAR)rate;
			    *(mib->mib_FixedRateTxType) = 2;
			}
#endif
			else
			{
				rc = -EFAULT;
				break;
			}
		}
		break;

	case WL_IOCTL_SET_CIPHERSUITE:
		{
			sscanf(param_str, "%s %s\n", param[0], param[1]);

			if (strcmp(param[0], "wpa") == 0)
			{
				if (strcmp(param[1], "tkip") == 0)
				{
#if 0 /*WFA_TKIP_NEGATIVE*/
					if ((*(mib->mib_ApMode)== AP_MODE_N_ONLY
						|| *(mib->mib_ApMode) == AP_MODE_BandN
						|| *(mib->mib_ApMode) == AP_MODE_GandN
						|| *(mib->mib_ApMode) == AP_MODE_BandGandN
						|| *(mib->mib_ApMode) == AP_MODE_AandN))
					{
						printk("This cipher configuration is not supported when HT mode is enabled\n");
						WLSYSLOG(netdev, WLSYSLOG_CLASS_ALL, "This cipher configuration is not supported when HT mode is enabled\n");
						WLSNDEVT(netdev, IWEVCUSTOM,(IEEEtypes_MacAddr_t *)&priv->hwData.macAddr[0], 
							"This cipher configuration is not supported when HT mode is enabled\n");

						rc = -EOPNOTSUPP;
						break;
					}                
#endif                    
					*(mib->mib_cipherSuite) = 2;

					mib->RSNConfig->MulticastCipher[0] = 0x00;
					mib->RSNConfig->MulticastCipher[1] = 0x50;
					mib->RSNConfig->MulticastCipher[2] = 0xF2;
					mib->RSNConfig->MulticastCipher[3] = 0x02;          // TKIP

					mib->UnicastCiphers->UnicastCipher[0] = 0x00;
					mib->UnicastCiphers->UnicastCipher[1] = 0x50;
					mib->UnicastCiphers->UnicastCipher[2] = 0xF2;
					mib->UnicastCiphers->UnicastCipher[3] = 0x02;        // TKIP
					mib->UnicastCiphers->Enabled = TRUE;

				} 
				else if (strcmp(param[1], "aes-ccmp") == 0)
				{
#if 0 /*WFA_TKIP_NEGATIVE*/
					if ((*(mib->mib_ApMode)== AP_MODE_N_ONLY
						|| *(mib->mib_ApMode) == AP_MODE_BandN
						|| *(mib->mib_ApMode) == AP_MODE_GandN
						|| *(mib->mib_ApMode) == AP_MODE_BandGandN
						|| *(mib->mib_ApMode) == AP_MODE_AandN))
					{
						printk("This cipher configuration is not supported when HT mode is enabled\n");
						WLSYSLOG(netdev, WLSYSLOG_CLASS_ALL, "This cipher configuration is not supported when HT mode is enabled\n");
						WLSNDEVT(netdev, IWEVCUSTOM,(IEEEtypes_MacAddr_t *)&priv->hwData.macAddr[0], 
							"This cipher configuration is not supported when HT mode is enabled\n");
						rc = -EOPNOTSUPP;
						break;
					}                
#endif
					/* If mixed mode only allow WPA TKIP for multicast and unicast. */
					if (mib->RSNConfigWPA2->WPA2Enabled && !mib->RSNConfigWPA2->WPA2OnlyEnabled)
					{
						*(mib->mib_cipherSuite) = 4;

						mib->RSNConfig->MulticastCipher[0] = 0x00;
						mib->RSNConfig->MulticastCipher[1] = 0x50;
						mib->RSNConfig->MulticastCipher[2] = 0xF2;
						mib->RSNConfig->MulticastCipher[3] = 0x02;          // TKIP

						mib->UnicastCiphers->UnicastCipher[0] = 0x00;
						mib->UnicastCiphers->UnicastCipher[1] = 0x50;
						mib->UnicastCiphers->UnicastCipher[2] = 0xF2;
						mib->UnicastCiphers->UnicastCipher[3] = 0x02;        // TKIP
						mib->UnicastCiphers->Enabled = TRUE;
					}
					else
					{
						*(mib->mib_cipherSuite) = 4;

						mib->RSNConfig->MulticastCipher[0] = 0x00;
						mib->RSNConfig->MulticastCipher[1] = 0x50;
						mib->RSNConfig->MulticastCipher[2] = 0xF2;
						mib->RSNConfig->MulticastCipher[3] = 0x04;           // AES

						mib->UnicastCiphers->UnicastCipher[0] = 0x00;
						mib->UnicastCiphers->UnicastCipher[1] = 0x50;
						mib->UnicastCiphers->UnicastCipher[2] = 0xF2;
						mib->UnicastCiphers->UnicastCipher[3] = 0x04;        // AES 
						mib->UnicastCiphers->Enabled = TRUE;
					}
				}
				else
				{
					rc = -EFAULT;
				}

				WLDBG_INFO(DBG_LEVEL_1, "mib->RSNConfig->MulticastCipher: %02x %02x %02x %02x\n", 
					mib->RSNConfig->MulticastCipher[0],
					mib->RSNConfig->MulticastCipher[1],
					mib->RSNConfig->MulticastCipher[2],
					mib->RSNConfig->MulticastCipher[3]);
				WLDBG_INFO(DBG_LEVEL_1, "mib->RSNConfig->UnicastCiphers: %02x %02x %02x %02x\n", 
					mib->UnicastCiphers->UnicastCipher[0],
					mib->UnicastCiphers->UnicastCipher[1],
					mib->UnicastCiphers->UnicastCipher[2],
					mib->UnicastCiphers->UnicastCipher[3]);
				WLDBG_INFO(DBG_LEVEL_1, "mib->UnicastCiphers->Enabled %d\n", mib->UnicastCiphers->Enabled);
			} 
			else if (strcmp(param[0], "wpa2") == 0)
			{
				if (strcmp(param[1], "aes-ccmp") == 0)
				{
					/* If mixed mode only allow WPA2 TKIP for multicast. */
					if (mib->RSNConfigWPA2->WPA2Enabled && !mib->RSNConfigWPA2->WPA2OnlyEnabled)
					{
						mib->RSNConfigWPA2->MulticastCipher[0] = 0x00;
						mib->RSNConfigWPA2->MulticastCipher[1] = 0x0F;
						mib->RSNConfigWPA2->MulticastCipher[2] = 0xAC;
						mib->RSNConfigWPA2->MulticastCipher[3] = 0x02;      // TKIP
					}
					else
					{
						mib->RSNConfigWPA2->MulticastCipher[0] = 0x00;
						mib->RSNConfigWPA2->MulticastCipher[1] = 0x0F;
						mib->RSNConfigWPA2->MulticastCipher[2] = 0xAC;
						mib->RSNConfigWPA2->MulticastCipher[3] = 0x04;      // AES
					}

					*(mib->mib_cipherSuite) = 4;

					mib->WPA2UnicastCiphers->UnicastCipher[0] = 0x00;
					mib->WPA2UnicastCiphers->UnicastCipher[1] = 0x0F;
					mib->WPA2UnicastCiphers->UnicastCipher[2] = 0xAC;
					mib->WPA2UnicastCiphers->UnicastCipher[3] = 0x04;   // AES
					mib->WPA2UnicastCiphers->Enabled = TRUE;

				} 
				else if (strcmp(param[1], "tkip") == 0)
				{
#if 0 /*WFA_TKIP_NEGATIVE*/
					if ((*(mib->mib_ApMode)== AP_MODE_N_ONLY
						|| *(mib->mib_ApMode) == AP_MODE_BandN
						|| *(mib->mib_ApMode) == AP_MODE_GandN
						|| *(mib->mib_ApMode) == AP_MODE_BandGandN
						|| *(mib->mib_ApMode) == AP_MODE_AandN))
					{
						printk("This cipher configuration is not supported when HT mode is enabled\n");
						WLSYSLOG(netdev, WLSYSLOG_CLASS_ALL, "This cipher configuration is not supported when HT mode is enabled\n");
						WLSNDEVT(netdev, IWEVCUSTOM,(IEEEtypes_MacAddr_t *)&priv->hwData.macAddr[0], 
							"This cipher configuration is not supported when HT mode is enabled\n");
						rc = -EOPNOTSUPP;
						break;
					}                
#endif
					mib->RSNConfigWPA2->MulticastCipher[0] = 0x00;
					mib->RSNConfigWPA2->MulticastCipher[1] = 0x0F;
					mib->RSNConfigWPA2->MulticastCipher[2] = 0xAC;
					mib->RSNConfigWPA2->MulticastCipher[3] = 0x02;      // TKIP

					*(mib->mib_cipherSuite) = 2;


					mib->WPA2UnicastCiphers->UnicastCipher[0] = 0x00;
					mib->WPA2UnicastCiphers->UnicastCipher[1] = 0x0F;
					mib->WPA2UnicastCiphers->UnicastCipher[2] = 0xAC;
					mib->WPA2UnicastCiphers->UnicastCipher[3] = 0x02;   // TKIP
					mib->WPA2UnicastCiphers->Enabled = TRUE;
				} 
				else
				{
					rc = -EFAULT;
				}

				WLDBG_INFO(DBG_LEVEL_1, "mib->RSNConfigWPA2->MulticastCipher: %02x %02x %02x %02x\n", 
					mib->RSNConfigWPA2->MulticastCipher[0],
					mib->RSNConfigWPA2->MulticastCipher[1],
					mib->RSNConfigWPA2->MulticastCipher[2],
					mib->RSNConfigWPA2->MulticastCipher[3]);
				WLDBG_INFO(DBG_LEVEL_1, "mib->WPA2UnicastCiphers->UnicastCiphers: %02x %02x %02x %02x\n", 
					mib->WPA2UnicastCiphers->UnicastCipher[0],
					mib->WPA2UnicastCiphers->UnicastCipher[1],
					mib->WPA2UnicastCiphers->UnicastCipher[2],
					mib->WPA2UnicastCiphers->UnicastCipher[3]);
				WLDBG_INFO(DBG_LEVEL_1, "mib->WPA2UnicastCiphers->Enabled %d\n", mib->WPA2UnicastCiphers->Enabled);

			} 
			else
			{
				rc = -EFAULT;
			}

			WLDBG_INFO(DBG_LEVEL_1, "*(mib->mib_cipherSuite): %d\n", *(mib->mib_cipherSuite));
		}
		break;

	case WL_IOCTL_SET_PASSPHRASE:
		sscanf(param_str, "%s %s\n",  param[0], param[1]);

		if (strcmp(param[0], "wpa") == 0)
		{
			char *p;
			int len;

			p = strstr(param_str, "wpa");
			p += 4;
			len = strlen(p);
#ifdef WPAHEX64
			if ((len <= 7) || (len > 64)) 
#else
			if ((len <= 7) || (len > 63))
#endif
			{
				rc = -EFAULT;
				break;
			}	
#ifdef WPAHEX64
			if (len == 64)
			{
				if (!IsHexKey(p))
				{
					rc = -EFAULT;
					break;
				}
				memset(mib->RSNConfig->PSKValue, 0, 32);
				HexStringToHexDigi(mib->RSNConfig->PSKValue, p, 32);
				memset(mib->RSNConfig->PSKPassPhrase, 0, 65);
				strcpy(mib->RSNConfig->PSKPassPhrase, p);

				*(mib->mib_WPAPSKValueEnabled) = 1;
				break;
			}		
#endif

			memset(mib->RSNConfig->PSKPassPhrase, 0, 65);
			strcpy(mib->RSNConfig->PSKPassPhrase, p);
			WLDBG_INFO(DBG_LEVEL_1, "mib->RSNConfig->PSKPassPhrase: %s\n", mib->RSNConfig->PSKPassPhrase);
		} else if (strcmp(param[0], "wpa2") == 0)
		{
			char *p;
			int len;

			p = strstr(param_str, "wpa2");
			p += 5;
			len = strlen(p);
#ifdef WPAHEX64
			if ((len <= 7) || (len > 64)) 
#else
			if ((len <= 7) || (len > 63))
#endif  
			{
				rc = -EFAULT;
				break;
			}	
#ifdef WPAHEX64
			if (len == 64)
			{
				if (!IsHexKey(p))
				{
					rc = -EFAULT;
					break;
				}
				memset(mib->RSNConfigWPA2->PSKValue, 0, 32);
				HexStringToHexDigi(mib->RSNConfigWPA2->PSKValue, p, 32);
				memset(mib->RSNConfigWPA2->PSKPassPhrase, 0, 65);
				strcpy(mib->RSNConfigWPA2->PSKPassPhrase, p);

				*(mib->mib_WPA2PSKValueEnabled) = 1;
				break;
			}		
#endif

			memset(mib->RSNConfigWPA2->PSKPassPhrase, 0, 65);
			strcpy(mib->RSNConfigWPA2->PSKPassPhrase, p);
			WLDBG_INFO(DBG_LEVEL_1, "mib->RSNConfigWPA2->PSKPassPhrase: %s\n", mib->RSNConfigWPA2->PSKPassPhrase);
		} else
			rc = -EFAULT;

		break;

	case WL_IOCTL_SET_FILTERMAC:
		{
			UINT8 *mib_wlanfilterno_p = mib->mib_wlanfilterno;
			UINT8 MacAddr[6], i, SameMAC=0;

			sscanf(param_str, "%s %s\n",  param[0], param[1]);

			if(strcmp(param[0], "deleteall") == 0)
			{
				*mib_wlanfilterno_p = 0;
				memset(mib->mib_wlanfiltermac, 0, FILERMACNUM*6);
				break;
			}

			if ((strlen((char *)param[1]) != 12) || (!IsHexKey((char *)param[1]))) 
			{
				rc = -EFAULT;
				break;
			}	
			getMacFromString(MacAddr, param[1]);

			if (strcmp(param[0], "add") == 0)
			{

				for(i=0; i<FILERMACNUM; i++)
				{
					if(memcmp(mib->mib_wlanfiltermac+i*6, MacAddr, 6)==0)
					{
						SameMAC = 1;
						break;
					}
				}

				if(SameMAC == 0)
				{
					if (*mib_wlanfilterno_p < FILERMACNUM)
					{
						memcpy((mib->mib_wlanfiltermac+*mib_wlanfilterno_p*6), MacAddr, 6);
						(*mib_wlanfilterno_p)++;
					}
					else
						rc = -EFAULT;
				}
			}
			else if(strcmp(param[0], "del") == 0)
			{
				for(i=0; i<FILERMACNUM; i++)
				{
					if(memcmp(mib->mib_wlanfiltermac+i*6, MacAddr, 6)==0)
					{
						(*mib_wlanfilterno_p)--;
						if (*mib_wlanfilterno_p == 0)
						{
							if (i != 0)
							{
								rc = -EFAULT;
								break;
							}
							else
								memset(mib->mib_wlanfiltermac, 0, 6);
						}
						else
						{
							if (i > *mib_wlanfilterno_p)
							{
								rc = -EFAULT;
								break;
							}
							else
							{
								memcpy(mib->mib_wlanfiltermac+i*6, mib->mib_wlanfiltermac+((i+1) *6), 
									(*mib_wlanfilterno_p - i) * 6);
								memset(mib->mib_wlanfiltermac+*mib_wlanfilterno_p*6, 0, 6);
							}
						}
						break;
					}
				}
			}
			else
				rc = -EFAULT;
		}
		break;

	case WL_IOCTL_SET_BSSID:
		{
			MIB_OP_DATA *mib_OpData = mib->OperationTable;	
			UINT8 MacAddr[6];

			sscanf(param_str, "%s\n",  param[0]);

			if (strlen((char *)param[0]) != 12) 
			{
				rc = -EFAULT;
				break;
			}	
			getMacFromString(MacAddr, param[0]);
			memcpy(mib_OpData->StaMacAddr, MacAddr, 6);				
			memcpy(netdev->dev_addr, MacAddr, 6);

			/*Unlike vmac, parent interface macBssId is not updated in SendStartCmd. So we update here*/
			if(priv->master==NULL)
				memcpy(vmacSta_p->macBssId, MacAddr, 6);		
			
			
		}
		break;

#ifdef CLIENT_SUPPORT
	case WL_IOCTL_SET_CLIENT:
		{
			/* Set this mib to control mode of 11a, 11b, 11g, 11n, */
			extern const char* mac_display(const UINT8 *mac);
			UINT8   clientAddr[6];
			UINT8   enable;
			struct wlprivate    *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
			vmacApInfo_t        *vmacSta_p;
			vmacEntry_t  *vmacEntry_p = NULL;
			struct wlprivate *wlMPrvPtr = wlpptr;
			UINT8  mlmeAssociatedFlag;
			UINT8  mlmeBssid[6];

			/* Get VMAC structure of the master*/
			if(!wlpptr->master)
			{
				printk("Device %s is not a client device \n", netdev->name);
				rc = -EFAULT;
				break;
			}

			vmacSta_p = wlMPrvPtr->vmacSta_p;

			sscanf(param_str, "%s %s \n", param[0], param[1]);

			enable = atohex(param[0]);
			
			if (enable == 0) 
			{
				*(mib->mib_STAMacCloneEnable) = 0;
				//printk("maccloneing disabled mib_STAMacCloneEnable = %x \n", *(mib->mib_STAMacCloneEnable));
			}
			else if (enable == 1)
			{            
				*(mib->mib_STAMacCloneEnable) = 1;
				//printk("maccloneing enabled mib_STAMacCloneEnable = %x \n", *(mib->mib_STAMacCloneEnable));
				break;
			}
			else
			{
				printk("macclone: invalid set option. \n");
				rc = -EFAULT;
				break;
			}

			if((vmacEntry_p = sme_GetParentVMacEntry(((vmacApInfo_t *) priv->vmacSta_p)->VMacEntry.phyHwMacIndx)) == NULL)
				break;


			smeGetStaLinkInfo(vmacEntry_p->id, &mlmeAssociatedFlag, &mlmeBssid[0]);
			wlFwRemoveMacAddr(vmacSta_p->dev, &vmacEntry_p->vmacAddr[0]);	
			if (mlmeAssociatedFlag)
				 	cleanupAmpduTx(vmacSta_p, (UINT8 *)&mlmeBssid[0]);
				
			if (strlen((char *)param[1]) == 12)
			{					
				getMacFromString(clientAddr, param[1]);
				memcpy(&vmacEntry_p->vmacAddr[0], clientAddr, 6);
			}
			else
			{
				
				/*The method to generate wdev0sta0 mac addr is same as in wlInit for client*/
				/*If GUI is used to config client, it comes to here too and we have to assign correct wdev0sta0 mac addr*/
				/*eventhough it is already initialized in wlInit*/
				int i, index, bssidmask = 0;
				UINT8 macaddr[6]= {0x00, 0x00, 0x00,0x00, 0x00, 0x00};
				memcpy(macaddr, wlpptr->master->dev_addr, 6);
			#if defined(MBSS)
				for (index = 0; index< NUMOFAPS; index++)
			#else	
				for (index = 0; index< 1; index++)
			#endif			
				{
					//uses mac addr bit 41 & up as mbss addresses
					for (i = 1; i<32; i++)
					{
						if((bssidmask & (1<<i))==0)
							break;	
					}
					if(i)
					{
						macaddr[0]=wlpptr->master->dev_addr[0] |((i<<2)|0x2);
					}
					bssidmask |=1<<i;
				}
				memcpy(&vmacEntry_p->vmacAddr[0], &macaddr[0], 6);
	
			}

			 
			/*If we change wdev0sta0 mac addr, we also change these areas.*/
			/*macStaAddr is used for mac addr comparison in tx and rx*/
			memcpy(netdev->dev_addr, &vmacEntry_p->vmacAddr[0], 6); 
			memcpy(&wlpptr->hwData.macAddr[0], &vmacEntry_p->vmacAddr[0], 6);
			memcpy(&vmacSta_p->macStaAddr[0], &vmacEntry_p->vmacAddr[0], 6);
			memcpy(&vmacSta_p->macBssId[0], &vmacEntry_p->vmacAddr[0], 6);
			memcpy(&vmacSta_p->VMacEntry.vmacAddr[0], &vmacEntry_p->vmacAddr[0], 6);
			
			printk("Mac cloning disabled : Mac Client Addr = %s\n",
				mac_display(&vmacEntry_p->vmacAddr[0]));

		}
		break;
#endif /* CLIENT_SUPPORT */

#ifdef WDS_FEATURE
	case WL_IOCTL_SET_WDS_PORT:
		{
			UINT8 MacAddr[6], index=0;
			UINT32 wdsPortMode = 0;
			sscanf(param_str, "%s %s %s\n",  param[0], param[1], param[2]);

			if (strlen((char *)param[0]) == 3) 
			{
				if (strcmp(param[0], "off") == 0)
				{
					*(mib->mib_wdsEnable) = 0;
				}
				else
					rc = -EFAULT;
				break;
			}
			if ((strlen((char *)param[1]) != 12) || (!IsHexKey((char *)param[1]))) 
			{
				rc = -EFAULT;
				break;
			}	
			if (!getMacFromString(MacAddr, param[1]))
			{
				rc = -EFAULT;
				break;
			}

			if (strlen((char *)param[0]) != 1) 
			{
				rc = -EFAULT;
				break;
			}
			index = atoi(param[0]);

			if (strlen((char *)param[2]) != 1) 
			{
#ifdef SOC_W8864			
			    if(strlen((char *)param[2]) ==3)
			    {
			        if(strcmp((char *)param[2], "ac1") == 0)
			        {
                        wdsPortMode =  AC_1SS_MODE;
                    } 
                    else if(strcmp((char *)param[2], "ac2") == 0)
                    {
                        wdsPortMode =  AC_2SS_MODE;
                    }
                    else
                    {
                        wdsPortMode =  AC_3SS_MODE;
                    }
                }
                else
#endif                    
                {
    				wdsPortMode =  0xFF;
				}
			}
			else
			{
				switch((char )param[2][0])
				{
				case 'b':
					wdsPortMode =  BONLY_MODE;
					break;
				case 'g':
					wdsPortMode =  GONLY_MODE;
					break;
				case 'a':
					wdsPortMode =  AONLY_MODE;
					break;
				case 'n':
					wdsPortMode =  NONLY_MODE;
					break;
				default:
					wdsPortMode =  0xFF;
					break;
				}
			}
			if (!setWdsPort(netdev, MacAddr, index, wdsPortMode))
			{
				rc = -ENODEV;
			}
		}
		break;
#endif

	case WL_IOCTL_SET_WMMEDCAAP:
		{
			extern mib_QAPEDCATable_t mib_QAPEDCATable[4];
			int index, cw_min, cw_max, aifsn, tx_op_lim_b, tx_op_lim;

			sscanf(param_str, "%s %s %s %s %s %s\n",  param[0], param[1], param[2], param[3], param[4], param[5]);
			index = atoi(param[0]);
			if ((index < 0) || (index > 3))
			{
				rc = -EFAULT;
				break;
			}
			cw_min = atoi(param[1]);
			cw_max = atoi(param[2]);
			if (/*(cw_min < BE_CWMIN) || (cw_max > BE_CWMAX) ||*/ (cw_min > cw_max))
			{
				rc = -EFAULT;
				break;
			}
			aifsn = atoi(param[3]);
			tx_op_lim_b = atoi(param[4]);
			tx_op_lim = atoi(param[5]);

			mib_QAPEDCATable[index].QAPEDCATblIndx = index;
			mib_QAPEDCATable[index].QAPEDCATblCWmin = cw_min;
			mib_QAPEDCATable[index].QAPEDCATblCWmax = cw_max;
			mib_QAPEDCATable[index].QAPEDCATblAIFSN = aifsn; 
			mib_QAPEDCATable[index].QAPEDCATblTXOPLimit = tx_op_lim;
			mib_QAPEDCATable[index].QAPEDCATblTXOPLimitBAP = tx_op_lim_b;

			//printk("WMM: %d %d %d %d %d %d %d\n", index, cw_min, cw_max, aifsn, tx_op_lim_b, tx_op_lim);
		}
		break;

	case WL_IOCTL_SET_WMMEDCASTA:
		{
			extern mib_QStaEDCATable_t mib_QStaEDCATable[4];
			int index, cw_min, cw_max, aifsn, tx_op_lim_b, tx_op_lim, acm;

			sscanf(param_str, "%s %s %s %s %s %s %s\n",  param[0], param[1], param[2], param[3], param[4], param[5], param[6]);
			index = atoi(param[0]);
			if ((index < 0) || (index > 3) )
			{
				rc = -EFAULT;
				break;
			}
			cw_min = atoi(param[1]);
			cw_max = atoi(param[2]);
			if (/*(cw_min < BE_CWMIN) || (cw_max > BE_CWMAX) ||*/ (cw_min > cw_max))
			{
				rc = -EFAULT;
				break;
			}
			aifsn = atoi(param[3]);
			tx_op_lim_b = atoi(param[4]);
			tx_op_lim = atoi(param[5]);
			acm = atoi(param[6]);

			mib_QStaEDCATable[index].QStaEDCATblIndx = index;
			mib_QStaEDCATable[index].QStaEDCATblCWmin = cw_min;
			mib_QStaEDCATable[index].QStaEDCATblCWmax = cw_max;
			mib_QStaEDCATable[index].QStaEDCATblAIFSN = aifsn; 
			mib_QStaEDCATable[index].QStaEDCATblTXOPLimit = tx_op_lim;
			mib_QStaEDCATable[index].QStaEDCATblTXOPLimitBSta = tx_op_lim_b;
			mib_QStaEDCATable[index].QStaEDCATblMandatory = acm;
			//printk("WMM: %d %d %d %d %d %d %d\n", index, cw_min, cw_max, aifsn, tx_op_lim_b, tx_op_lim, acm);
		}
		break;

	case WL_IOCTL_SET_TXPOWER:
		{
#if defined(SOC_W8366)||defined(SOC_W8764)
			UINT16 i, setcap;
#ifdef SOC_W8864
			sscanf(param_str, "%s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s\n",  param[0], param[1], param[2], param[3], param[4], param[5]
			,param[6], param[7], param[8], param[9], param[10], param[11], param[12], param[13], param[14], param[15], param[16]);
#else
			sscanf(param_str, "%s %s %s %s %s %s %s %s %s %s %s %s %s\n",  param[0], param[1], param[2], param[3], param[4], param[5]
			,param[6], param[7], param[8], param[9], param[10], param[11], param[12]);
#endif
			setcap = atoi(param[0]);
			for (i =0; i<TX_POWER_LEVEL_TOTAL; i++)
			{
				if(setcap)
				{
					mib->PhyDSSSTable->powinited |=2;
					mib->PhyDSSSTable->maxTxPow[i]=atohex2(param[i+1]);
				}
				else
				{
					mib->PhyDSSSTable->powinited |=1;
					mib->PhyDSSSTable->targetPowers[i]=atohex2(param[i+1]);
				}
			}
#else
			int bw, chan, pw1, pw2, pw3, pw4;

			sscanf(param_str, "%s %s %s %s %s %s\n",  param[0], param[1], param[2], param[3], param[4], param[5]);
			bw = atoi(param[0]);
			if ((bw != 20) && (bw != 40) )
			{
				rc = -EFAULT;
				break;
			}
			chan = atoi(param[1]);

			if (*(mib->mib_ApMode) & AP_MODE_A_ONLY)		
			{
				int i;

				if(bw == 20)
				{
					for (i=0; i<PWTAGETRATETABLE20M_5G_ENTRY; i++)
					{
						if (mib->PowerTagetRateTable20M_5G[i*4] == chan)
						{
							mib->PowerTagetRateTable20M_5G[i*4 + 1] = atohex2(param[2]);
							mib->PowerTagetRateTable20M_5G[i*4 + 2] = atohex2(param[3]);
							mib->PowerTagetRateTable20M_5G[i*4 + 3] = atohex2(param[4]);
							break;
						}	
					}
					if (i >= PWTAGETRATETABLE20M_5G_ENTRY)
					{
						rc = -EFAULT;
						break;
					}
				}
				if(bw == 40)
				{
					for (i=0; i<PWTAGETRATETABLE40M_5G_ENTRY; i++)
					{
						if (mib->PowerTagetRateTable40M_5G[i*4] == chan)
						{
							mib->PowerTagetRateTable40M_5G[i*4 + 1] = atohex2(param[2]);
							mib->PowerTagetRateTable40M_5G[i*4 + 2] = atohex2(param[3]);
							mib->PowerTagetRateTable40M_5G[i*4 + 3] = atohex2(param[4]);
							break;
						}	
					}
					if (i >= PWTAGETRATETABLE40M_5G_ENTRY)
					{
						rc = -EFAULT;
						break;
					}
				}

			}
			else
			{ //2.4G
				if (bw == 20)
				{
					if ((chan < 1) || (chan > 14) )
					{
						rc = -EFAULT;
						break;
					}
				}
				else
				{
					if ((chan < 1) || (chan > 9) )
					{
						rc = -EFAULT;
						break;
					}
				}
				pw1 = atohex2(param[2]);
				pw2 = atohex2(param[3]);
				pw3 = atohex2(param[4]);
				pw4 = atohex2(param[5]);

				if (bw == 20)
				{

					mib->PowerTagetRateTable20M[(chan-1)*4 + 0] = pw1;
					mib->PowerTagetRateTable20M[(chan-1)*4 + 1] = pw2;
					mib->PowerTagetRateTable20M[(chan-1)*4 + 2] = pw3;
					mib->PowerTagetRateTable20M[(chan-1)*4 + 3] = pw4;
				}
				else
				{
					mib->PowerTagetRateTable40M[(chan-1)*4 + 0] = pw1;
					mib->PowerTagetRateTable40M[(chan-1)*4 + 1] = pw2;
					mib->PowerTagetRateTable40M[(chan-1)*4 + 2] = pw3;
					mib->PowerTagetRateTable40M[(chan-1)*4 + 3] = pw4;
				}
			}
			//printk("txpower: %d %d 0x%x 0x%x 0x%x 0x%x\n", bw, chan, pw1, pw2, pw3, pw4);
#endif
		}
		break;

	case WL_IOCTL_SETCMD:
		{
			sscanf(param_str, "%s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s\n",
			param[0],param[1],param[2],param[3],param[4],param[5],param[6],param[7],param[8],param[9],param[10],param[11],param[12],param[13],param[14],param[15],param[16],param[17],param[18],param[19],
			param[20],param[21],param[22],param[23],param[24],param[25],param[26],param[27],param[28],param[29],param[30],param[31],param[32],param[33],param[34],param[35],param[36],param[37],param[38],param[39],
			param[40],param[41],param[42],param[43],param[44],param[45],param[46],param[47],param[48],param[49],param[50],param[51],param[52],param[53],param[54],param[55],param[56],param[57],param[58],param[59],
			param[60],param[61],param[62],param[63],param[64]);
			if((strcmp(param[0], "scanchannels") == 0))
			{
				int i;
				int offset = (*(vmacSta_p->Mib802dot11->mib_ApMode) <AP_MODE_A_ONLY)?0:IEEEtypes_MAX_CHANNELS;
				for (i =0; i<IEEE_80211_MAX_NUMBER_OF_CHANNELS; i++)
				{
					vmacSta_p->ChannelList[i+offset] = atohex2(param[i+1]);
				}
			}
#ifdef MFG_SUPPORT
			else if ((strcmp(param[0], "extfw") == 0) || (strcmp(param[0], "mfgfw") == 0))
			{
				int mfgCmd = 0;
				if (strcmp(param[0], "mfgfw") == 0) 
					mfgCmd = 1;		    


				if(!LoadExternalFw(priv, param[1]))
				{
					/* No file is loaded */
					rc = -EFAULT;
					break;
				}

				if (netdev->flags & IFF_RUNNING)
				{
					if (mfgCmd)
						priv->mfgEnable = 1;

					/* Only load one time for mfgfw */
					if (!priv->mfgLoaded)
						rc = priv->wlreset(netdev);							
					else
						rc= 0;

					if (mfgCmd)
						priv->mfgLoaded = 1;
					else
						priv->mfgLoaded = 0;
				} 
				else
					rc = -EFAULT;

				if (rc)						   
				{
					if (mfgCmd)	{
						priv->mfgEnable = 0;
						priv->mfgLoaded = 0;
					}
					printk("FW download failed.\n");
				}
				else
				{
					if (!priv->mfgLoaded)
						printk("FW download ok.\n");
				}
				break;
			} 
			else if	(strcmp(param[0], "mfg") == 0)
			{
				extern int wlFwMfgCmdIssue(struct net_device *netdev, char *pData,  char *pDataOut);

				char *pOut = cmdGetBuf;
				char *pIn = param_str + strlen(param[0]) + 1;
				int len = *(UINT16 *)(pIn+2);

				wlFwMfgCmdIssue(netdev, pIn, (pOut+4));

				*(int *)&pOut[0] = len;
				*ret_len = len + sizeof(int);


				bufBack = pOut;

				break;
			}
			else if	(strcmp(param[0], "fwrev") == 0)
			{
				wlPrepareFwFile(netdev);
				if (netdev->flags & IFF_RUNNING)
					rc = priv->wlreset(netdev);
				else
					rc = -EFAULT;

				break;
			}
			else
#endif
#ifdef AMPDU_SUPPORT
				if( strcmp(param[0], "addba") == 0)
				{
					extern void AddbaTimerProcess(UINT8 *data);
					char macaddr[6];
					//char macaddr2[6];
					int tid;
					UINT32 seqNo = 0;
					int stream;

					if ((strlen((char *)param[1]) != 12) || (!IsHexKey((char *)param[1]))) 
					{
						rc = -EFAULT;
						break;
					}	
					getMacFromString(macaddr, param[1]);
					tid = atohex2(param[2]);
					stream = atohex2(param[3]);
#ifdef SOC_W8864
					if ((stream > 7) || (tid > 7))
					{
						rc = -EFAULT;
						break;
					}
#else
					if ((stream > 1) || (tid > 7))
					{
						rc = -EFAULT;
						break;
					}
#endif

					if(priv->wlpd_p->Ampdu_tx[stream].InUse != 1)
					{
						priv->wlpd_p->Ampdu_tx[stream].MacAddr[0]=macaddr[0];
						priv->wlpd_p->Ampdu_tx[stream].MacAddr[1]=macaddr[1];
						priv->wlpd_p->Ampdu_tx[stream].MacAddr[2]=macaddr[2];
						priv->wlpd_p->Ampdu_tx[stream].MacAddr[3]=macaddr[3];
						priv->wlpd_p->Ampdu_tx[stream].MacAddr[4]=macaddr[4];
						priv->wlpd_p->Ampdu_tx[stream].MacAddr[5]=macaddr[5];
						priv->wlpd_p->Ampdu_tx[stream].AccessCat = tid;
						priv->wlpd_p->Ampdu_tx[stream].InUse = 1;
						priv->wlpd_p->Ampdu_tx[stream].TimeOut = 0;
						priv->wlpd_p->Ampdu_tx[stream].AddBaResponseReceive=0;
						priv->wlpd_p->Ampdu_tx[stream].DialogToken=priv->wlpd_p->Global_DialogToken;
						priv->wlpd_p->Global_DialogToken=(priv->wlpd_p->Global_DialogToken+1)%63;
						if(priv->wlpd_p->Ampdu_tx[stream].initTimer==0)
						{
							TimerInit(&priv->wlpd_p->Ampdu_tx[stream].timer);
							priv->wlpd_p->Ampdu_tx[stream].initTimer= 1;
						}
						TimerDisarm(&priv->wlpd_p->Ampdu_tx[stream].timer);
						priv->wlpd_p->Ampdu_tx[stream].vmacSta_p = vmacSta_p;
						TimerFireIn(&priv->wlpd_p->Ampdu_tx[stream].timer, 1, &AddbaTimerProcess, (UINT8 *)&priv->wlpd_p->Ampdu_tx[stream], 10);
					}
					else
					{
						printk("Stream %x is already in use \n",stream);
						break;
					}
#ifdef SOC_W8764  // Added to allow manual ampdu  setup for Client mode.          			            
					if (vmacSta_p->VMacEntry.modeOfService == VMAC_MODE_CLNT_INFRA)
						SendAddBAReqSta(vmacSta_p, macaddr, tid,1, seqNo,priv->wlpd_p->Ampdu_tx[stream].DialogToken);
					else
#endif
						SendAddBAReq(vmacSta_p,macaddr, tid,1, seqNo,priv->wlpd_p->Ampdu_tx[stream].DialogToken);  /** Only support immediate ba **/
					//	wlFwCreateBAStream(64, 64 , macaddr,	10, tid, 1,  stream);  //for mike stupid code
				}
				else if(strcmp(param[0],"ampdu_stat") == 0)
				{
					int i,j;

					for(i=0; i < MAX_SUPPORT_AMPDU_TX_STREAM; i++)
					{
						printk(": ");
						for(j=0;j<6;j++)
						{
							printk("%x ",priv->wlpd_p->Ampdu_tx[i].MacAddr[j]);
						}
						printk("tid %x Inuse %x timeout %d pps %d\n",priv->wlpd_p->Ampdu_tx[i].AccessCat,priv->wlpd_p->Ampdu_tx[i].InUse,
							(int)priv->wlpd_p->Ampdu_tx[i].TimeOut,(int)priv->wlpd_p->Ampdu_tx[i].txa_avgpps );
						printk("\n");
					}

				}
				else if( strcmp(param[0], "delba") == 0)
				{
					char macaddr2[6];
					int tid;
					int i;

					if ((strlen((char *)param[1]) != 12) || (!IsHexKey((char *)param[1]))) 
					{
						rc = -EFAULT;
						break;
					}	
					getMacFromString(macaddr2, param[1]);
					tid = atohex2(param[2]);
					if (tid > 7)
					{
						rc = -EFAULT;
						break;
					}

					for(i=0;i<7;i++)
					{
						printk(" AMacaddr2 %x %x %x %x %x %x\n",priv->wlpd_p->Ampdu_tx[i].MacAddr[0],priv->wlpd_p->Ampdu_tx[i].MacAddr[1],priv->wlpd_p->Ampdu_tx[i].MacAddr[2],priv->wlpd_p->Ampdu_tx[i].MacAddr[3],priv->wlpd_p->Ampdu_tx[i].MacAddr[4],priv->wlpd_p->Ampdu_tx[i].MacAddr[5]);
						printk(" Macaddr2 %x %x %x %x %x %x\n",macaddr2[0],macaddr2[1],macaddr2[2],macaddr2[3],macaddr2[4],macaddr2[5]);
						printk(" tid = %x , In Use = %x \n*******\n",priv->wlpd_p->Ampdu_tx[i].AccessCat,priv->wlpd_p->Ampdu_tx[i].InUse);
						disableAmpduTx(vmacSta_p,macaddr2, tid);
					}
				}
				else if( strcmp(param[0], "del2ba") == 0)  /** fake command use by WIFI testbed **/
				{
					char macaddr2[6];
					int tid;

					if ((strlen((char *)param[1]) != 12) || (!IsHexKey((char *)param[1]))) 
					{
						rc = -EFAULT;
						break;
					}	
					getMacFromString(macaddr2, param[1]);
					tid = atohex2(param[2]);
					if (tid > 7)
					{
						rc = -EFAULT;
						break;
					}

					SendDelBA2(vmacSta_p, macaddr2, tid);
				}
				else if( strcmp(param[0], "AmpduRxDisable") == 0)
				{
					UINT8 option;

					if ((strlen((char *)param[1]) != 1) || (!IsHexKey((char *)param[1]))) 
					{
						rc = -EFAULT;
						break;
					}	

					option = atohex2(param[1]);

					vmacSta_p->Ampdu_Rx_Disable_Flag = option;

				}

				else if(strcmp(param[0], "rifs") == 0)
				{
					UINT8 QNum;
					QNum= atohex2(param[1]);
					*(mib->mib_rifsQNum) = QNum;
					//wlFwSetRifs(netdev, QNum);
				}
#ifdef COEXIST_20_40_SUPPORT
				else if(strcmp(param[0], "intolerant40") == 0)
				{
					UINT8 protection2040;
					UINT8 mode;

					if ((strlen((char *)param[1]) != 1) || (!IsHexKey((char *)param[1]))) 
					{
						printk(" intolerant40 = %x HT40MIntoler=%x \n",*(vmacSta_p->Mib802dot11->mib_FortyMIntolerant),*(vmacSta_p->Mib802dot11->mib_HT40MIntoler));
						printk("shadow intolerant40 = %x HT40MIntoler=%x \n",*(vmacSta_p->ShadowMib802dot11->mib_FortyMIntolerant),*(vmacSta_p->ShadowMib802dot11->mib_HT40MIntoler));
					}	
					else
					{

						protection2040 = atohex2(param[1]);
						mode =  atohex2(param[2]);
						if(protection2040==0)
						{
							*(vmacSta_p->ShadowMib802dot11->mib_HT40MIntoler) = 0;  /** 20/40 coexist protection mechanism off **/
							printk("Setting 20/40 Coexist off\n");

						}
						if(protection2040==1)
						{
							*(vmacSta_p->ShadowMib802dot11->mib_HT40MIntoler) = 1;  /** 20/40 coexist protection mechanism on **/
							printk("Setting 20/40 Coexist on\n");

						}

						else if(protection2040==2)
						{
							*(vmacSta_p->ShadowMib802dot11->mib_FortyMIntolerant) = 1;
							printk("Setting tolerant AP\n");
						}
						else if(protection2040==3)
						{
							extern int wlFwSet11N_20_40_Switch(struct net_device * netdev, UINT8 mode);

							*(vmacSta_p->ShadowMib802dot11->mib_FortyMIntolerant) = mode;
							*(mib->USER_ChnlWidth )= mode;

							wlFwSet11N_20_40_Switch(vmacSta_p->dev, mode);
							printk("Setting 20/40 with bw %d\n",mode);
						}
					}

				}
				else if(strcmp(param[0],"TriggerScanInterval")== 0)
				{
					UINT16 TriggerScanInterval;

					TriggerScanInterval = atohex2(param[1]);

					printk("Set TriggerScanInterval to %x\n",TriggerScanInterval);

					*(mib->mib_Channel_Width_Trigger_Scan_Interval)=TriggerScanInterval;

				}
#endif

#ifdef EXPLICIT_BF

				else if( strcmp(param[0], "SetBF") == 0)
				{
					extern int wlFwSet11N_BF_Mode(struct net_device *netdev, UINT8 bf_option, UINT8 bf_csi_steering, UINT8 bf_mcsfeedback, UINT8 bf_mode,
						UINT8 bf_interval, UINT8 bf_slp, UINT8 bf_power);
					UINT8 option, csi_steering,mcsfeedback,mode,interval,slp, power;

					if (strcmp(param[1],"help")== 0) 
					{
						printk("Usage: SetBF csi_steering mcsfeedback mode interval slp power \n");
						printk(" Eg. SetBF  0 3 0 0 1 1 255\n");
						printk(" Option          : 0 Auto, send NDPA every second\n");
						printk("                     : 1 Send NDPA manually\n");
						printk("CSI steering : 0 csi steering no feedback\n");
						printk("                      : 1 csi steering fb csi\n");
						printk("                      : 2 csi steering fb no compress bf\n");
						printk("                      : 3 csi steering fb compress bf\n");
						printk("Mcsfeedback   : 0 MCS feedback off,  1 MCS feedback on\n");
						printk("Mode             : 0 NDPA\n");
						printk("                      : 1 Control Wrapper \n");
						printk("Interval         : in ~20msec\n");
						printk("slp                 : 1 ON 0 OFF\n");
						printk("power            : trpc power id for NDP, use 0xff to take pid from last transmitted data pck \n");



						rc = -EFAULT;
						break;
					}	


					option=atohex2(param[1]);
					csi_steering=atohex2(param[2]);
					mcsfeedback=atohex2(param[3]);
					mode=atohex2(param[4]);
					interval=atohex2(param[5]);
					slp=atohex2(param[6]);
					power=atohex2(param[7]);


					printk("Set 11n BF mode option=%d csi_steer=%d mcsfb=%d mode=%d interval=%d slp=%d, power=%d\n",
						option,csi_steering,mcsfeedback,mode,interval,slp,power);

					wlFwSet11N_BF_Mode(vmacSta_p->dev, option,csi_steering,mcsfeedback,mode,interval,slp,power);	  


				}

				else if(strcmp(param[0], "NoAck") == 0)
				{
					extern int wlFwSetNoAck(struct net_device *netdev, UINT8 Enable);
					UINT8 Enable;
					Enable = atohex2(param[1]);
					printk("Set NoACK= %x\n", Enable);
					wlFwSetNoAck(netdev, Enable);
				}

				else if(strcmp(param[0], "NoSteer") == 0)
				{
					extern int wlFwSetNoSteer(struct net_device *netdev, UINT8 Enable);
					UINT8 Enable;
					Enable = atohex2(param[1]);
					printk("Set NoSteer = %x\n", Enable);
					wlFwSetNoSteer(netdev, Enable);
				}
				else if(strcmp(param[0], "SetCDD") == 0)
				{
					extern int wlFwSetCDD(struct net_device *netdev, UINT32 cdd_mode);
					UINT32 cdd_mode;
					cdd_mode = atohex2(param[1]);
					printk("Set CDD= %x\n", (unsigned int)cdd_mode);
					wlFwSetCDD(netdev, cdd_mode);
				}
				else if(strcmp(param[0], "SetTxHOP") == 0)
				{
					extern int wlFwSetTxHOP(struct net_device *netdev, UINT8 Enable, UINT8  TxHopStatus);
					UINT8 Enable, TxHopStatus;
					Enable = atohex2(param[1]);
					TxHopStatus = atohex2(param[2]);
					printk("Set Txhop= %x status=%x \n", Enable, TxHopStatus);
					wlFwSetTxHOP(netdev, Enable,  TxHopStatus);
				}
				else if	((strcmp(param[0], "get_bftype") == 0) )
				{
					printk("bftype is %d\n",  (int)*(mib->mib_bftype));
					break;
				}
				else if	((strcmp(param[0], "set_bftype") == 0) )
				{
					int val = atoi(param[1]);
					*(mib->mib_bftype) = val;
					printk("bftype is %d\n",  (int)*(mib->mib_bftype));
					break;
				}
				else if	((strcmp(param[0], "get_bwSignaltype") == 0) )
				{
					printk("bw_Signaltype is %d\n",  (int)*(mib->mib_bwSignaltype));
					break;
				}

				else if	((strcmp(param[0], "set_bwSignaltype") == 0) )
				{
					int val = atoi(param[1]);
					*(mib->mib_bwSignaltype) = val;
					printk("bw_Signaltype is %d\n",  (int)*(mib->mib_bwSignaltype));
					break;
				}



#endif
				else if	((strcmp(param[0], "get_weakiv_threshold") == 0) )
				{
					printk("weakiv_threshold is %d\n",  (int)*(mib->mib_weakiv_threshold));
					break;
				}
				else if	((strcmp(param[0], "set_weakiv_threshold") == 0) )
				{
					int val = atoi(param[1]);
					*(mib->mib_weakiv_threshold) = val;
					printk("weakiv_threshold is %d\n",  (int)*(mib->mib_weakiv_threshold));
					break;
				}

#ifdef POWERSAVE_OFFLOAD
				else if(strcmp(param[0],"SetTim")== 0)
				{
					UINT16 Aid;
					UINT32  Set;

					Aid = atohex2(param[1]);
					Set = atohex2(param[2]);

					printk("SetTim\n");

					wlFwSetTIM(netdev, Aid, Set);

				}
				else if(strcmp(param[0],"SetPowerSaveStation")==0)
				{
					UINT8 NoOfStations;

					printk("SetPowerSaveStation\n");

					NoOfStations=atohex2(param[1]);

					wlFwSetPowerSaveStation(netdev, NoOfStations);

				}
				else if(strcmp(param[0],"GetTim")==0)
				{
					printk(" Get TIM:\n");
					wlFwGetTIM(netdev);
				}
#endif
				else
#endif
					if (strcmp(param[0], "getbcn") == 0)
					{
#define LINECHAR	16
						UINT16 len = 0;
						UINT8 *pBcn, *p;
						UINT8 i;
						UINT16 lineLen;

						pBcn = kmalloc(MAX_BEACON_SIZE ,GFP_KERNEL);
						if (pBcn == NULL)
						{
							rc = -EFAULT;
							break;
						}			

						if (wlFwGetBeacon(netdev, pBcn, &len) == FAIL)
						{
							rc = -EFAULT;
							kfree(pBcn);
							break;
						}

						sprintf(bufBack, "Beacon: len %d\n", len );			
						p = bufBack + strlen(bufBack);
						lineLen = (len/LINECHAR == 0?len/LINECHAR:1+len/LINECHAR);
						for (i=0; i<lineLen ;i++)
						{
							sprintf(p, "%04d: %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n", i*LINECHAR, 
								pBcn[i*LINECHAR+0],
								pBcn[i*LINECHAR+1],
								pBcn[i*LINECHAR+2],
								pBcn[i*LINECHAR+3],
								pBcn[i*LINECHAR+4],
								pBcn[i*LINECHAR+5],
								pBcn[i*LINECHAR+6],
								pBcn[i*LINECHAR+7],
								pBcn[i*LINECHAR+8],
								pBcn[i*LINECHAR+9],
								pBcn[i*LINECHAR+10],
								pBcn[i*LINECHAR+11],
								pBcn[i*LINECHAR+12],
								pBcn[i*LINECHAR+13],
								pBcn[i*LINECHAR+14],
								pBcn[i*LINECHAR+15]);
							p = bufBack + strlen(bufBack);
						}

						*ret_len = strlen(bufBack);
						printk("%s", bufBack);
						kfree(pBcn);
						break;
					}
					else if	((strcmp(param[0], "annex") == 0) || (strcmp(param[0], "readeepromhdr") == 0))
					{
						UINT16 len;
						int i;
						int annex;
						int	index;

						annex = atohex2(param[1]);
						index = atohex2(param[2]);

						if (strcmp(param[0], "readeepromhdr") == 0)
							annex = 255;	

						if (wlFwGetCalTable(netdev, (UINT8)annex, (UINT8)index) == FAIL)
						{
							rc = -EFAULT;
							break;
						}

						if ((priv->calTbl[0] == annex) || (annex == 0) || (annex == 255))
						{
							char tmpStr[16];
							len = priv->calTbl[2] | (priv->calTbl[3] << 8);
							if (annex == 255)
							{
								len = 128;
								sprintf(bufBack, "EEPROM header(128 bytes) \n");
							}
							else
								sprintf(bufBack, "Annex %d\n", annex);
							for (i=0; i<len/4;i++)
							{
								memset(tmpStr, 0, 16);
								sprintf(tmpStr, "%02x %02x %02x %02x\n", priv->calTbl[i*4], 
									priv->calTbl[i*4+1],
									priv->calTbl[i*4+2],
									priv->calTbl[i*4+3]);
								strcat(bufBack, tmpStr);
							}
						}
						else
							sprintf(bufBack, "No Annex %d\n", annex);

						*ret_len = strlen(bufBack);
						printk("%s", bufBack);
						break;
					}
#if defined (SOC_W8366) || defined (SOC_W8364) || defined (SOC_W8764)
					else if	((strcmp(param[0], "or") == 0) )
					{
						unsigned long i, reg, val, set=WL_GET ;

						for(i = 0; i < 4; i++)
						{
						    /* for RF */
						    printk("\nRF BASE %c registers \n", 'A' + i); 
						    for(reg = 0xA00+(0x100*i) ; reg <= 0xAFF+(0x100*i) ; reg++)
						    {
							    wlRegRF(netdev, set,  reg, &val);
							    printk("0x%02X	0x%02X\n", (int)(reg - (0xA00 + (0x100*i))), (int)val);
						    }
                        }
                       for(i = 0; i < 4; i++)
                       {
							printk("\nRF XCVR path %c registers \n", 'A' + i);
							for(reg = 0x100+(0x100*i) ; reg <= 0x1FF+(0x100*i) ; reg++)
							{
								wlRegRF(netdev, set,  reg, &val);
                                printk("0x%03X	0x%02X\n", (int)(reg), (int)val);
							}																    
						}

						/* for BBP */
#ifdef SOC_W8864
                        printk("\nBBU Registers \n");
						for(reg=0x00;reg<= 0x6DB;reg++)
#else
						for(reg=0x00;reg<= 0x56C;reg++)
#endif
						{
							wlRegBB(netdev, set,  reg, &val);
                            if (reg < 0x100)
                                printk("0x%02X	0x%02X\n", (int)reg, (int)val);
                            else
							    printk("0x%03X	0x%02X\n", (int)reg, (int)val);
						}

                    }
#endif
					else if ((strcmp(param[0], "getaddrtable") == 0))
					{
						wlFwGetAddrtable(netdev);
					}
					else if ((strcmp(param[0], "getfwencrinfo") == 0))
					{
						char macaddr[6];
						getMacFromString(macaddr, param[1]);
						wlFwGetEncrInfo(netdev, macaddr);
					}
					else if	((strcmp(param[0], "setreg") == 0) )
					{
						unsigned long reg, val, set=WL_GET ;
						reg = atohex2(param[2]);
						if(param[3][0])
						{
							val = atohex2(param[3]);
							set = WL_SET;
						}

						if (strcmp(param[1], "mac") == 0)
						{
							if(set == WL_SET)
							{
								PciWriteMacReg(netdev, reg, val);
							} else
								val = PciReadMacReg(netdev, reg);
							printk("%s mac reg %x = %x\n", set?"Set":"Get", (int)reg, (int)val);
							break;
						}
						else if (strcmp(param[1], "rf") == 0)
						{
							wlRegRF(netdev, set,  reg, &val);
							printk("%s rf reg %x = %x\n", set?"Set":"Get", (int)reg, (int)val);
							break;
						}
						else if (strcmp(param[1], "bb") == 0)
						{
							wlRegBB(netdev, set,  reg, &val);
							printk( "%s bb reg %x = %x\n", set?"Set":"Get", (int)reg, (int)val);
							break;
						} 
						else if (strcmp(param[1], "cau") == 0)
						{
							wlRegCAU(netdev, set,  reg, &val);
							printk( "%s cau reg %x = %x\n", set?"Set":"Get", (int)reg, (int)val);
							break;
						} 
						else if (strcmp(param[1], "addr0") == 0)
						{
							if(set == WL_SET)
								*(volatile unsigned int *)((unsigned int)priv->ioBase0 + reg) = val;
							else
								val = *(volatile unsigned int *)((unsigned int)priv->ioBase0 + reg);
							printk( "%s addr %x = %x\n", set?"Set":"Get", (int)reg+0xc0000000, (int)val);
							break;
						} 
						else if (strcmp(param[1], "addr1") == 0)
						{
							if(set == WL_SET)
								*(volatile unsigned int *)((unsigned int)priv->ioBase1 + reg) = val;
							else
								val = *(volatile unsigned int *)((unsigned int)priv->ioBase1 + reg);
							printk( "%s addr %x = %x\n", set?"Set":"Get", (int)reg+0x80000000, (int)val);
							break;
						} 
						else if (strcmp(param[1], "addr") == 0)
						{
							UINT32 *addr_val = kmalloc( 64 * sizeof(UINT32), GFP_KERNEL );
							if( addr_val == NULL )
							{
								rc = -EFAULT;
								break; 
							}
							memset(addr_val, 0, 64 * sizeof(UINT32));
							addr_val[0] = val;
							if(set == WL_SET)
							{
								wlFwGetAddrValue(netdev, reg, 4, addr_val, 1);
							}
							else
								wlFwGetAddrValue(netdev, reg, 4, addr_val, 0);
							printk( "%s addr %x = %x\n", set?"Set":"Get", (int)reg, (int)addr_val[0]);
							kfree(addr_val);
							break;
						} 
						else 
						{
							rc = -EFAULT;
							break; 
						}
					}
					else if	((strcmp(param[0], "debug") == 0) )
					{
						DebugCmdParse(netdev, param_str+6);
						break;
					}
					else if	((strcmp(param[0], "memdump") == 0) )
					{
						unsigned long i, val, offset, length,j=0 ;
						unsigned char *buf=NULL;

						if (strcmp(param[1], "mm") == 0)
						{ 
							int k;
							offset = atohex2(param[2])& 0xfffffffc;
							/*if (offset>0xafff || offset < 0xa000)
							{
							rc = -EFAULT;
							break; 
							} */

							length = atohex2(param[3])*4;
							if(!length)
								length = 32;
							buf = kmalloc(length*10+100, GFP_KERNEL);
							if (buf == NULL) 
							{
								rc = -EFAULT;
								break; 
							}

							sprintf(buf+j, "dump mem\n");
							j = strlen(buf);
							for(k = 0; k < length; k+=256)
							{
								for(i = 0; i < 256; i+=4)
								{
									volatile unsigned int val = 0;

									val = *(volatile unsigned int *)((unsigned int)priv->ioBase1 + offset + i+k);

									//val = PciReadMacReg(netdev, offset+i);
									if(i %16 ==0)
									{
										sprintf(buf+j, "\n0x%08x",(int)(0x80000000 + offset + i+k));
										j = strlen(buf);
									}
									sprintf(buf+j, "  %08x", val);
									j = strlen(buf);
								}
								printk("%s\n", buf);
								j =0;
							}
							if (buf != NULL)
								kfree(buf);
						} 
						else if (strcmp(param[1], "ms") == 0)
						{ 
							int k;
							offset = atohex2(param[2])& 0xfffffffc;

							length = atohex2(param[3])*4;
							if(!length)
								length = 32;
							buf = kmalloc(length*10+100, GFP_KERNEL);
							if (buf == NULL) 
							{
								rc = -EFAULT;
								break; 
							}

							sprintf(buf+j, "dump mem\n");
							j = strlen(buf);
							for(k = 0; k < length; k+=256)
							{
								for(i = 0; i < 256; i+=4)
								{
									volatile unsigned int val = 0;

									val = *(volatile unsigned int *)((unsigned int)priv->ioBase0 + offset + i+k);

									if(i %16 ==0)
									{
										sprintf(buf+j, "\n0x%08x",(int)(0xC0000000 + offset + i+k));
										j = strlen(buf);
									}
									sprintf(buf+j, "  %08x", val);
									j = strlen(buf);
								}
								printk("%s\n", buf);
								j =0;
							}
							if (buf != NULL)
								kfree(buf);
						} 
						else if (strcmp(param[1], "rf") == 0)
						{
							offset = atohex2(param[2]);
							length = atohex2(param[3]);
							if(!length)
								length = 32;
							buf = kmalloc(length*10+100, GFP_KERNEL);
							if (buf == NULL) 
							{
								rc = -EFAULT;
								break; 
							}

							sprintf(buf+j, "dump rf regs\n");
							j = strlen(buf);
							for(i = 0; i < length; i++)
							{
								wlRegRF(netdev, WL_GET,  offset+i, &val);
								if(i %8 ==0)
								{
									sprintf(buf+j, "\n%02x: ",(int)(offset+i));
									j = strlen(buf);
								}
								sprintf(buf+j, "  %02x", (int)val);
								j = strlen(buf);
							}
							printk("%s\n\n", buf);
							if (buf != NULL)
								kfree(buf);
						}
						else if (strcmp(param[1], "bb") == 0)
						{
							offset = atohex2(param[2]);
							length = atohex2(param[3]);
							if(!length)
								length = 32;
							buf = kmalloc(length*10+100, GFP_KERNEL);
							if (buf == NULL) 
							{
								rc = -EFAULT;
								break; 
							}

							sprintf(buf+j, "dump bb regs\n");
							j = strlen(buf);
							for(i = 0; i < length; i++)
							{
								wlRegBB(netdev, WL_GET,  offset+i, &val);
								if(i %8 ==0)
								{
									sprintf(buf+j, "\n%02x: ",(int)(offset+i));
									j = strlen(buf);
								}
								sprintf(buf+j,  "  %02x", (int)val);
								j = strlen(buf);
							}
							printk("%s\n\n", buf);
							if (buf != NULL)
								kfree(buf);
						}
						else if (strcmp(param[1], "addr") == 0)
						{	
							UINT32 addr;
							UINT32 *addr_val = kmalloc( 64 * sizeof(UINT32), GFP_KERNEL );
							if( addr_val == NULL )
							{
								rc = -EFAULT;
								break; 
							}
							memset(addr_val, 0, 64 * sizeof(UINT32));
							addr = atohex2(param[2])& 0xfffffffc; // 4 byte boundary
							// length is unit of 4 bytes
							length = atohex2(param[3]);
							if(!length)
								length = 32;
							if( length > 64 )
								length = 64 ;
							if(wlFwGetAddrValue(netdev, addr, length, addr_val,0 ) )
							{
								printk("Could not get the memory address value\n");
								rc = -EFAULT;
								kfree(addr_val);
								break; 
							}
							buf = kmalloc(length*16+100, GFP_KERNEL);
							if (buf == NULL) 
							{
								rc = -EFAULT;
								kfree(addr_val);
								break; 
							}
							j += sprintf(buf+j, "dump addr\n");
							for(i = 0; i < length; i++)
							{
								if(i %2 ==0)
								{
									j += sprintf(buf+j, "\n%08x: ",(int)(addr + i *4 ));
								}
								j += sprintf(buf+j, "  %08x", (int)addr_val[i]);
							}
							printk("%s\n\n", buf);
							if (buf != NULL)
								kfree(buf);
							kfree(addr_val);
						} 
						break;
					}
#ifdef MRVL_WPS_CLIENT
					else if ((strcmp(param[0], "bssid") == 0) )
					{
						char *tmpptr;
						tmpptr = param[1];
#ifdef WL_KERNEL_26
						ptr = strsep(&tmpptr, ":" );
#else
						ptr = strtok( param[1], ":" );
#endif
						if( ptr )
						{
							desireBSSID[count ++] = atohex(ptr);
#ifdef WL_KERNEL_26
							while ((ptr = strsep(&tmpptr, ":" )) != NULL )
#else
							while ((ptr = strtok(param[1], ":" )) != NULL )
#endif
							{
								desireBSSID[count ++] = atohex(ptr);
							}
							memcpy (mib->StationConfig->DesiredBSSId, desireBSSID, IEEEtypes_ADDRESS_SIZE );
#ifdef DBG
							printk("BSSID IS :%02X:%02X:%02X:%02X:%02X:%02X\n",
								desireBSSID[0],
								desireBSSID[1],
								desireBSSID[2],
								desireBSSID[3],
								desireBSSID[4],
								desireBSSID[5]);
#endif
						}
						break;
					}
#endif //MRVL_WPS_CLIENT
#ifdef EWB
					else if (strcmp(param[0], "ewbtable") == 0)
					{
						int i, j;
						hash_entry *pEntry;

						for (i=0; i < HASH_ENTRY_COLUMN_MAX; i++)
						{
							pEntry = &hashTable[i];
							for(j=0; j < HASH_ENTRY_ROW_MAX; j++)
							{   
								if(pEntry && pEntry->nwIpAddr)
								{
									printk("Index [%d,%d] \t IP=%x \t MAC=%02X:%02X:%02X:%02X:%02X:%02X\n",
										i,j,(int)pEntry->nwIpAddr,pEntry->hwAddr[0],pEntry->hwAddr[1],pEntry->hwAddr[2],
										pEntry->hwAddr[3],pEntry->hwAddr[4],pEntry->hwAddr[5]);

									pEntry = (hash_entry *)pEntry->nxtEntry;
								}
								else
									break;
							}
						}
					}
#endif /* EWB */
#if defined (SOC_W8366) || defined (SOC_W8364) || defined (SOC_W8764)
					else if( strcmp(param[0], "getratetable") == 0)
					{
						UINT32 size;		
						UINT8 *pRateTable = NULL;			
						
						if(param[1])
						{						
							size = RATEINFO_DWORD_SIZE * RATE_ADAPT_MAX_SUPPORTED_RATES;
							pRateTable = kmalloc(size ,GFP_KERNEL);
							
							if(pRateTable)
							{
								char macaddr[6];

								getMacFromString(macaddr, param[1]);
								wlFwGetRateTable(netdev, (UINT8 *)macaddr,(UINT8 *)pRateTable, size);

								printk( "%02x %02x %02x %02x %02x %02x: Client\n", 
								(int)macaddr[0], 
								(int)macaddr[1], 
								(int)macaddr[2], 
								(int)macaddr[3], 
								(int)macaddr[4], 
								(int)macaddr[5] 
								);

							#ifdef SOC_W8864
								ratetable_print_SOCW8864((UINT8 *)pRateTable);		
							#else
								ratetable_print_SOCW8764((UINT8 *)pRateTable);
							#endif
							
								kfree(pRateTable);
								pRateTable = NULL;		
							}
						}
						
						*ret_len = strlen(bufBack);
					}
#endif
#ifdef DYNAMIC_BA_SUPPORT
					else if	((strcmp(param[0], "get_ampdu_bamgmt") == 0) )
					{
						printk("AMPDU Bandwidth mgmt status is %d\n",  (int)*(mib->mib_ampdu_bamgmt));
						break;
					}
					else if	((strcmp(param[0], "set_ampdu_bamgmt") == 0) )
					{
						int val = atoi(param[1]);
						if (val != 0 && val != 1 )
						{
							printk("incorrect status values \n");
							break;
						}
						*(mib->mib_ampdu_bamgmt) = val;
						printk("AMPDU Bandwidth mgmt status is %d\n",  (int)*(mib->mib_ampdu_bamgmt));
						break;
					}

					else if	((strcmp(param[0], "get_ampdu_mintraffic") == 0) )
					{
						printk("AMPDU Min Traffic \n -------------------- \n");
						printk("AC_BK = %d \n", (int)*(mib->mib_ampdu_mintraffic[1]));
						printk("AC_BE = %d \n", (int)*(mib->mib_ampdu_mintraffic[0]));
						printk("AC_VI = %d \n", (int)*(mib->mib_ampdu_mintraffic[2]));
						printk("AC_VO = %d \n", (int)*(mib->mib_ampdu_mintraffic[3]));
						break;
					}
					else if	((strcmp(param[0], "set_ampdu_mintraffic") == 0) )
					{
						if (!atoi(param[1]) || !atoi(param[2]) ||
							!atoi(param[3]) || !atoi(param[4]))
							printk("Some values are set to Zero !!!!! \n");

						*(mib->mib_ampdu_mintraffic[1]) = atoi(param[1]);
						*(mib->mib_ampdu_mintraffic[0]) = atoi(param[2]);
						*(mib->mib_ampdu_mintraffic[2]) = atoi(param[3]);
						*(mib->mib_ampdu_mintraffic[3]) = atoi(param[4]);
						printk("Now AMPDU Min Traffic \n -------------------- \n");
						printk("AC_BK = %d \n", (int)*(mib->mib_ampdu_mintraffic[1]));
						printk("AC_BE = %d \n", (int)*(mib->mib_ampdu_mintraffic[0]));
						printk("AC_VI = %d \n", (int)*(mib->mib_ampdu_mintraffic[2]));
						printk("AC_VO = %d \n", (int)*(mib->mib_ampdu_mintraffic[3]));
						break;
					}

					else if	((strcmp(param[0], "get_ampdu_low_ac_threshold") == 0) )
					{
						printk("AMPDU Low Threshold \n -------------------- \n");
						printk("AC_BK = %d \n", (int)*(mib->mib_ampdu_low_AC_thres[1]));
						printk("AC_BE = %d \n", (int)*(mib->mib_ampdu_low_AC_thres[0]));
						printk("AC_VI = %d \n", (int)*(mib->mib_ampdu_low_AC_thres[2]));
						printk("AC_VO = %d \n", (int)*(mib->mib_ampdu_low_AC_thres[3]));
						break;
					}
					else if	((strcmp(param[0], "set_ampdu_low_ac_threshold") == 0) )
					{
						if (!atoi(param[1]) || !atoi(param[2]) ||
							!atoi(param[3]) || !atoi(param[4]))
							printk("Some values are set to Zero !!!!! \n");

						*(mib->mib_ampdu_low_AC_thres[1]) = atoi(param[1]);
						*(mib->mib_ampdu_low_AC_thres[0]) = atoi(param[2]);
						*(mib->mib_ampdu_low_AC_thres[2]) = atoi(param[3]);
						*(mib->mib_ampdu_low_AC_thres[3]) = atoi(param[4]);
						printk("Now AMPDU Low Threshold \n -------------------- \n");
						printk("AC_BK = %d \n", (int)*(mib->mib_ampdu_low_AC_thres[1]));
						printk("AC_BE = %d \n", (int)*(mib->mib_ampdu_low_AC_thres[0]));
						printk("AC_VI = %d \n", (int)*(mib->mib_ampdu_low_AC_thres[2]));
						printk("AC_VO = %d \n", (int)*(mib->mib_ampdu_low_AC_thres[3]));
						break;
					}
#endif /* DYNAMIC_BA_SUPPORT */
					else if (strcmp(param[0], "dfstest") == 0)
					{
						extern UINT8 dfs_test_mode;

						dfs_test_mode = atohex2(param[1]);

						printk("dfstest : dfs_test_mode = %x \n", dfs_test_mode);
					}
					else if (strcmp(param[0], "dfschirp") == 0)
					{ 

						dfs_chirp_count_min = atohex2(param[1]);
                        dfs_chirp_time_interval = atohex2(param[2]);
                        dfs_pw_filter   = atohex2(param[3]);
                        dfs_min_num_radar = atohex2(param[4]);
                        dfs_min_pri_count = atohex2(param[5]);
                        

						printk("dfschirp : dfs_chirp_count_min = %d, dfs_chirp_time_interval = %d units of 10ms, dfs_pw_filter = %d dfs_min_num_radar = %d dfs_min_pri_count = %d \n", 
                               dfs_chirp_count_min, dfs_chirp_time_interval, dfs_pw_filter, dfs_min_num_radar, dfs_min_pri_count);
					}
#ifdef MPRXY
					else if (strcmp(param[0], "ipmcgrp") == 0)
					{
						UINT32 McIPAddr;
						UINT8 UcMACAddr[6];
						UINT8 i,j;
						BOOLEAN IPMcEntryExists = FALSE;
						BOOLEAN UcMACEntryExists = FALSE;
						BOOLEAN IPMFilterEntryExists = FALSE;
						UINT32 tempIPAddr;

						if (!IPAsciiToNum((unsigned int*)&McIPAddr, (const char *)&param[2]))
						{
							rc = -EFAULT;
							break;
						}

						if (McIPAddr == 0 &&
							((strcmp(param[1], "add") == 0) ||(strcmp(param[1], "del") == 0) ||
							(strcmp(param[1], "delgrp") == 0) ||(strcmp(param[1], "addipmfilter") == 0) ||(strcmp(param[1], "delipmfilter") == 0)))
						{
							rc = -EFAULT;
							break;
						}

						if (!getMacFromString(UcMACAddr, param[3]) &&
							((strcmp(param[1], "add") == 0) ||(strcmp(param[1], "del") == 0)))
						{
							rc = -EFAULT;
							break;
						}

						if (strcmp(param[1], "add") == 0)                    
						{
							for (i=0; i<MAX_IP_MCAST_GRPS; i++)
							{
								if(mib->mib_IPMcastGrpTbl[i]->mib_McastIPAddr == McIPAddr)
								{
									IPMcEntryExists = TRUE;

									if (mib->mib_IPMcastGrpTbl[i]->mib_MAddrCount < MAX_UCAST_MAC_IN_GRP)
									{
										/*check if unicast adddress entry already exists in table*/
										for (j=0; j<MAX_UCAST_MAC_IN_GRP; j++)
										{
											if (memcmp((char *)&mib->mib_IPMcastGrpTbl[i]->mib_UCastAddr[j],
												(char *)&UcMACAddr, 6) == 0)
											{
												UcMACEntryExists = TRUE;
												break;
											}
										}

										if (UcMACEntryExists == FALSE)
										{
											/* Add the MAC address into the table */
											memcpy ((char *)&mib->mib_IPMcastGrpTbl[i]->mib_UCastAddr[mib->mib_IPMcastGrpTbl[i]->mib_MAddrCount],
												(char *)&UcMACAddr, 6);
											mib->mib_IPMcastGrpTbl[i]->mib_MAddrCount ++;
											break;
										}
									}
									else
									{
										rc = -EFAULT;
										break;
									}
								}
							}

							/* if IP multicast group entry does not exist */
							if (IPMcEntryExists == FALSE)
							{
								/*check if space available in table */
								if (*(mib->mib_IPMcastGrpCount) < MAX_IP_MCAST_GRPS)
								{
									mib->mib_IPMcastGrpTbl[*(mib->mib_IPMcastGrpCount)]->mib_McastIPAddr = McIPAddr;

									/* Add the MAC address into the table */
									i = *(mib->mib_IPMcastGrpCount);

									memcpy ((char *)&mib->mib_IPMcastGrpTbl[i]->mib_UCastAddr[mib->mib_IPMcastGrpTbl[i]->mib_MAddrCount],
										(char *)&UcMACAddr, 6);

									/* increment unicast mac address count */
									mib->mib_IPMcastGrpTbl[i]->mib_MAddrCount ++;

									/*increment the IP multicast group slot by 1 */
									*(mib->mib_IPMcastGrpCount) = *(mib->mib_IPMcastGrpCount) + 1;
								}
								else
								{
									rc = -EFAULT;
									break;
								}
							}
						}
						else if (strcmp(param[1], "del") == 0)                    
						{
							/* check if IP Multicast group entry already exists */
							for(i=0; i<MAX_IP_MCAST_GRPS; i++)                        
							{
								/*match IP multicast grp address with entry */
								if(mib->mib_IPMcastGrpTbl[i]->mib_McastIPAddr == McIPAddr)
								{
									/*find the unicast address entry in the IP multicast group */
									for (j=0; j<MAX_UCAST_MAC_IN_GRP; j++)
									{
										if (memcmp ((char *)&mib->mib_IPMcastGrpTbl[i]->mib_UCastAddr[j],
											(char *)&UcMACAddr, 6) == 0)
										{
											/*decrement the count for unicast mac entries */
											mib->mib_IPMcastGrpTbl[i]->mib_MAddrCount --;

											/*if this is the very first entry, slot zero */
											if (mib->mib_IPMcastGrpTbl[i]->mib_MAddrCount == 0)
											{
												/* set the entry to zero */
												memset((char *)&mib->mib_IPMcastGrpTbl[i]->mib_UCastAddr[j],
													0,
													6);
												break;
											}
											else
											{
												/*if this is other than slot zero */
												/* set the entry to zero */
												memset((char *)&mib->mib_IPMcastGrpTbl[i]->mib_UCastAddr[j],
													0,
													6);
												/* move up entries to fill the vacant spot */
												memcpy((char *)&mib->mib_IPMcastGrpTbl[i]->mib_UCastAddr[j],
													(char *)&mib->mib_IPMcastGrpTbl[i]->mib_UCastAddr[j+1],
													(mib->mib_IPMcastGrpTbl[i]->mib_MAddrCount -j) * 6);
												/* clear the last unicast entry since all entries moved up by 1 */
												memset((char *)&mib->mib_IPMcastGrpTbl[i]->mib_UCastAddr[mib->mib_IPMcastGrpTbl[i]->mib_MAddrCount],
													0,
													6);
												break;
											}
										}
									}
								}
							}
						}
						else if (strcmp(param[1], "delgrp") == 0)
						{
							/* check if IP Multicast group entry already exists */
							for(i=0; i<MAX_IP_MCAST_GRPS; i++)
							{
								/*match IP multicast grp address with entry */
								if(mib->mib_IPMcastGrpTbl[i]->mib_McastIPAddr == McIPAddr)
								{
									/*decrement the count of IP multicast groups */
									*(mib->mib_IPMcastGrpCount) = *(mib->mib_IPMcastGrpCount) -1;

									/* if this is first entry i.e. slot zero */
									/* set the entire group entry to zero */
									/* set the entry to zero */
									if (i ==0)
									{
										memset((char *)mib->mib_IPMcastGrpTbl[i],
											0,
											sizeof (MIB_IPMCAST_GRP_TBL));
										break;
									}
									else
									{
										/* if this is a slot other than zero */
										/* set the entry to zero */
										memset((char *)mib->mib_IPMcastGrpTbl[i],
											0,
											sizeof (MIB_IPMCAST_GRP_TBL));

										/* move up entries to fill the vacant spot */
										memcpy((char *)&mib->mib_IPMcastGrpTbl[i],
											(char *)&mib->mib_IPMcastGrpTbl[i+1],
											(*(mib->mib_IPMcastGrpCount) -i) * sizeof(MIB_IPMCAST_GRP_TBL));

										/* clear the last unicast entry since all entries moved up by 1 */
										memset((char *)mib->mib_IPMcastGrpTbl[*(mib->mib_IPMcastGrpCount)],
											0,
											sizeof(MIB_IPMCAST_GRP_TBL));
									}
								}
							}
						}
						else if (strcmp(param[1], "getgrp") == 0)
						{
							/* check if IP Multicast group entry already exists */
							for(i=0; i<MAX_IP_MCAST_GRPS; i++)
							{
								/*match IP multicast grp address with entry */
								if(mib->mib_IPMcastGrpTbl[i]->mib_McastIPAddr == McIPAddr)
								{
									tempIPAddr = htonl(mib->mib_IPMcastGrpTbl[i]->mib_McastIPAddr);

									for (j=0; j<MAX_UCAST_MAC_IN_GRP; j++)
										printk("%u.%u.%u.%u %02x%02x%02x%02x%02x%02x\n",
										NIPQUAD(tempIPAddr), 
										mib->mib_IPMcastGrpTbl[i]->mib_UCastAddr[j][0],
										mib->mib_IPMcastGrpTbl[i]->mib_UCastAddr[j][1],
										mib->mib_IPMcastGrpTbl[i]->mib_UCastAddr[j][2],
										mib->mib_IPMcastGrpTbl[i]->mib_UCastAddr[j][3],
										mib->mib_IPMcastGrpTbl[i]->mib_UCastAddr[j][4],
										mib->mib_IPMcastGrpTbl[i]->mib_UCastAddr[j][5]);
								}
							}
						}
						else if (strcmp(param[1], "getallgrps") == 0)
						{
							/* check if IP Multicast group entry already exists */
							for(i=0; i<MAX_IP_MCAST_GRPS; i++)
							{
								if (mib->mib_IPMcastGrpTbl[i]->mib_McastIPAddr)
								{
									tempIPAddr = htonl(mib->mib_IPMcastGrpTbl[i]->mib_McastIPAddr);
								
									printk("IP Multicast Group: %u.%u.%u.%u \t Cnt:%d\n",NIPQUAD(tempIPAddr),
												mib->mib_IPMcastGrpTbl[i]->mib_MAddrCount);	

									for (j=0; j<MAX_UCAST_MAC_IN_GRP; j++)
									{
										printk("%u.%u.%u.%u %02x%02x%02x%02x%02x%02x\n",
											NIPQUAD(tempIPAddr), 
											mib->mib_IPMcastGrpTbl[i]->mib_UCastAddr[j][0],
											mib->mib_IPMcastGrpTbl[i]->mib_UCastAddr[j][1],
											mib->mib_IPMcastGrpTbl[i]->mib_UCastAddr[j][2],
											mib->mib_IPMcastGrpTbl[i]->mib_UCastAddr[j][3],
											mib->mib_IPMcastGrpTbl[i]->mib_UCastAddr[j][4],
											mib->mib_IPMcastGrpTbl[i]->mib_UCastAddr[j][5]); 
									}
								}
							}
						}
						else if (strcmp(param[1], "addipmfilter") == 0)
						{
							/* check if IP Multicast address entry already exists */
							for (i=0; i<MAX_IP_MCAST_GRPS; i++)                        
							{
								/*match IP multicast address with entry */
								if(*(mib->mib_IPMFilteredAddress[i]) == McIPAddr)
								{
									IPMFilterEntryExists = TRUE;
									break;                            
								}
							}

							if (!IPMFilterEntryExists)
							{
								/*create a entry */
								/*check if space available in table */
								if (*(mib->mib_IPMFilteredAddressIndex) < MAX_IP_MCAST_GRPS)
								{
									*(mib->mib_IPMFilteredAddress[*(mib->mib_IPMFilteredAddressIndex)]) = McIPAddr;

									/*increment the IP multicast filter address index by 1 */
									*(mib->mib_IPMFilteredAddressIndex) = *(mib->mib_IPMFilteredAddressIndex) + 1;
								}
								else
								{
									rc = -EFAULT;
									break;
								}
							}
						}
						else if (strcmp(param[1], "delipmfilter") == 0)
						{
							/* check if IP Multicast Filter entry already exists */
							for(i=0; i<MAX_IP_MCAST_GRPS; i++)
							{
								/*match IP multicast grp address with entry */
								if(*(mib->mib_IPMFilteredAddress[i])== McIPAddr)
								{
									/* set the entry to zero */
									*(mib->mib_IPMFilteredAddress[i]) = 0;

									/*decrement the count of IP multicast groups */
									*(mib->mib_IPMFilteredAddressIndex) = *(mib->mib_IPMFilteredAddressIndex) -1;

									/* move up entries to fill the vacant spot */
									for (j=0; j < (*(mib->mib_IPMFilteredAddressIndex)-i); j ++)
										*(mib->mib_IPMFilteredAddress[i+j]) = *(mib->mib_IPMFilteredAddress[i+j+1]);

									/* clear the last entry since all entries moved up by 1 */
									*(mib->mib_IPMFilteredAddress[*(mib->mib_IPMFilteredAddressIndex)]) = 0;

									break;
								}
							}
						}
						else if (strcmp(param[1], "getipmfilter") == 0)
						{
							for(i=0; i<MAX_IP_MCAST_GRPS; i++)
							{
								tempIPAddr = htonl(*(mib->mib_IPMFilteredAddress[i]));

								printk("%u.%u.%u.%u \n", NIPQUAD(tempIPAddr));
							}
						}

						else
						{
							rc = -EFAULT;
							break;
						}
					}
#endif /*MPRXY*/
					else if	((strcmp(param[0], "rptrmode") == 0) )
					{
						struct wlprivate    *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
						vmacApInfo_t        *vmacSta_p;
						struct wlprivate *wlMPrvPtr = wlpptr;
						UINT8 *p = bufBack;
						char macaddr[6];
						extStaDb_StaInfo_t *pStaInfo;
						int val;

						/* Get VMAC structure of the master*/
						if(!wlpptr->master)
						{
							printk("Device %s is not a client device \n", netdev->name);
							rc = -EFAULT;
							break;
						}
						vmacSta_p = wlMPrvPtr->vmacSta_p;

						if (strlen(param[1]) == 0) {
							sprintf(p, "mode: %d\n", *(mib->mib_RptrMode));
							printk("mode: %d\n", *(mib->mib_RptrMode));
						}
						else if	((strcmp(param[1], "0") == 0) ||  (strcmp(param[1], "1") == 0))
						{
							val = atoi(param[1]);

							if (val < 0 || val > 1)
							{
								rc = -EOPNOTSUPP;
								break;
							}							
							*(mib->mib_RptrMode) = val;					
							if (vmacSta_p->VMacEntry.modeOfService == VMAC_MODE_CLNT_INFRA)
							{
								if (val)
									*(mib->mib_STAMacCloneEnable) = 2;
								else
									*(mib->mib_STAMacCloneEnable) = 0;
							}						
						}	
						else if	((strcmp(param[1], "devicetype") == 0) )
						{
							if (strlen(param[2]) > (MAXRPTRDEVTYPESTR-1))
							{
								rc = -EOPNOTSUPP;
								break;
							}

							if (strlen(param[2]) != 0) {
								memcpy(mib->mib_RptrDeviceType, param[2], strlen(param[2]));
							} 
							else {
								sprintf(p, "DeviceType: %s\n", mib->mib_RptrDeviceType);							
								printk("DeviceType: %s\n", mib->mib_RptrDeviceType);
							}
						}								
						else if	((strcmp(param[1], "agingtime") == 0) )
						{
							if (strlen(param[2]) != 0) {
								val = atoi(param[2]);
								if (val < 60 || val > 86400)
								{
									rc = -EOPNOTSUPP;
									break;
								}
								*(mib->mib_agingtimeRptr) = val;								
							} 
							else {
								sprintf(p, "agingtime: %d\n", (int)*mib->mib_agingtimeRptr);							
								printk("agingtime: %d\n", (int)*mib->mib_agingtimeRptr);
							}
						}		
						else if	((strcmp(param[1], "listmac") == 0) )	
						{
							extern UINT16 ethStaDb_list(vmacApInfo_t *vmac_p);
							ethStaDb_list(vmacSta_p);
						}
						else if	((strcmp(param[1], "addmac") == 0) )	
						{
							getMacFromString(macaddr, param[2]);
							if ((pStaInfo = extStaDb_GetStaInfo(vmacSta_p,(IEEEtypes_MacAddr_t *)macaddr, 0)) != NULL )
							{
								pStaInfo->StaType = 0x02;
							}						}
						else if	((strcmp(param[1], "delmac") == 0) )	
						{
							getMacFromString(macaddr, param[2]);
							if ((pStaInfo = extStaDb_GetStaInfo(vmacSta_p,(IEEEtypes_MacAddr_t *)macaddr, 0)) != NULL )
							{
								pStaInfo->StaType = 0;
								ethStaDb_RemoveStaPerWlan(vmacSta_p, (IEEEtypes_MacAddr_t *)macaddr);
							}
						} else {
							rc = -EFAULT;
							break;
						}
						*ret_len = strlen(bufBack);
						break;  
					}
					else if	((strcmp(param[0], "loadtxpwrtable") == 0))
					{
						struct file *filp=NULL;
						mm_segment_t oldfs;
						char buff[120], *s;
						int len, index=0, i, value=0;

						oldfs = get_fs();
						set_fs(KERNEL_DS);

						filp = filp_open(param[1], O_RDONLY, 0);
						// if (filp != NULL) // Note: this one doesn't work and will cause crash
						if (!IS_ERR(filp))   // MUST use this one, important!!!
						{
							printk("loadtxpwrtable open <%s>: OK\n", param[1]);

							/* reset the whole table */
							for (i=0; i<IEEE_80211_MAX_NUMBER_OF_CHANNELS; i++)
								memset(mib->PhyTXPowerTable[i], 0, sizeof(MIB_TX_POWER_TABLE));

							while(1) {
								s = buff;
								while((len = vfs_read(filp, s, 0x01, &filp->f_pos)) == 1)
								{
									if (*s == '\n') { 
										/* skip blank line */
										if (s == buff)
											break;

										/* parse this line and assign value to data structure */
										*s = '\0';
										printk("index=<%d>: <%s>\n", index, buff);
#ifdef SOC_W8864
										/* 8864 total param: ch + setcap + 16 txpower + CDD + tx2 = 16 */
										sscanf(buff, "%s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s\n",  param[0], param[1], param[2], param[3], param[4], param[5]
										,param[6], param[7], param[8], param[9], param[10], param[11], param[12], param[13], param[14], param[15], param[16], param[17]
                                        ,param[18], param[19]);

										if (strcmp(param[18], "on") == 0)
											value = 1;
										else if (strcmp(param[18], "off") == 0)
											value = 0;
#else
										/* total param: ch + setcap + 12 txpower + CDD + tx2 = 16 */
										sscanf(buff, "%s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s\n",  param[0], param[1], param[2], param[3], param[4], param[5]
										,param[6], param[7], param[8], param[9], param[10], param[11], param[12], param[13], param[14], param[15]);

										if (strcmp(param[14], "on") == 0)
											value = 1;
										else if (strcmp(param[14], "off") == 0)
											value = 0;
#endif
										else {
											printk("txpower table format error: CCD should be on|off\n");
											break;
										}
										mib->PhyTXPowerTable[index]->CDD = value;

#ifdef SOC_W8864
										mib->PhyTXPowerTable[index]->txantenna2 = atohex2(param[19]);
#else
										mib->PhyTXPowerTable[index]->txantenna2 = atohex2(param[15]);
#endif

										mib->PhyTXPowerTable[index]->Channel = atoi(param[0]);
										mib->PhyTXPowerTable[index]->setcap = atoi(param[1]);

										for (i =0; i<TX_POWER_LEVEL_TOTAL; i++)
										{
											mib->PhyTXPowerTable[index]->TxPower[i] = atohex2(param[i+2]);
										}								

										index++;
										break;
									}
									else
										s++;
								}
								if (len <= 0)
									break;
							}

							filp_close(filp, current->files);
						}
						else
							printk("loadtxpwrtable open <%s>: FAIL\n", param[1]);

						set_fs(oldfs);
						break;
					}
					else if	((strcmp(param[0], "gettxpwrtable") == 0))
					{
						int index;
						printk("txpower table:\n");
						for (index=0; index<IEEE_80211_MAX_NUMBER_OF_CHANNELS; index++)
						{
							if (mib->PhyTXPowerTable[index]->Channel == 0)
								break;

#ifdef SOC_W8864
							printk("%d %d 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x %d %d\n",
#else
							printk("%d %d 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x %d %d\n",
#endif
								mib->PhyTXPowerTable[index]->Channel,
								mib->PhyTXPowerTable[index]->setcap,
								mib->PhyTXPowerTable[index]->TxPower[0],
								mib->PhyTXPowerTable[index]->TxPower[1],
								mib->PhyTXPowerTable[index]->TxPower[2],
								mib->PhyTXPowerTable[index]->TxPower[3],
								mib->PhyTXPowerTable[index]->TxPower[4],
								mib->PhyTXPowerTable[index]->TxPower[5],
								mib->PhyTXPowerTable[index]->TxPower[6],
								mib->PhyTXPowerTable[index]->TxPower[7],
								mib->PhyTXPowerTable[index]->TxPower[8],
								mib->PhyTXPowerTable[index]->TxPower[9],
								mib->PhyTXPowerTable[index]->TxPower[10],
								mib->PhyTXPowerTable[index]->TxPower[11],
#ifdef SOC_W8864 
								mib->PhyTXPowerTable[index]->TxPower[12],
								mib->PhyTXPowerTable[index]->TxPower[13],
								mib->PhyTXPowerTable[index]->TxPower[14],
								mib->PhyTXPowerTable[index]->TxPower[15],
#endif
								mib->PhyTXPowerTable[index]->CDD,
								mib->PhyTXPowerTable[index]->txantenna2
								);
						}
						break;
					}
#ifdef SSU_SUPPORT
					else if (strcmp(param[0], "ssutest") == 0)
					{
						UINT32 *pSsuPci = (UINT32*)priv->pSsuBuf;
                        UINT16 i = 0;
                        UINT16 dump = 0;
                        UINT16 fft_length, fft_skip, adc_dec, time, buffer_size; 

						dump = atohex2(param[1]);
                        
                        if (dump)
                        {
                            printk("ssu dump location phys = %x virt = %x len = %d \n", (UINT32)priv->wlpd_p->pPhysSsuBuf, (UINT32)priv->pSsuBuf, 1024);
                            i = 0;
                            while (i < 1024)
                            {
                                if ((i!=0) && !(i%8)) printk("\n");
                                printk("%08x ", pSsuPci[i++]);                                
                            }
                            printk("\n");
                            for (i=0; i<2048; i++) pSsuPci[i] = 0;

                        }
                        else
                        {

                            #define  SSU_FFT_SIZE_64  0
                            #define  SSU_FFT_SIZE_128 1
                            #define  SSU_FFT_SIZE_192 2
                            #define  SSU_FFT_SIZE_256 3

                            #define  SSU_FFT_SKIP_0   0
                            #define  SSU_FFT_SKIP_64  1
                            #define  SSU_FFT_SKIP_128 2
                            #define  SSU_FFT_SKIP_192 3

                            #define  SSU_ADC_DECIMATION_0 0
                            #define  SSU_ADC_DECIMATION_2 1
                            #define  SSU_ADC_DECIMATION_4 2
                            #define  SSU_ADC_DECIMATION_8 3

                            UINT16 fft_length_lookup[4]   = {64, 128, 192, 256};
                            UINT16 adc_dec_lookup[4]      = {1, 2, 4, 8 };


                            //BW = 80, Nsel = 256	
                            //BW = 40, Nsel = 128	
                            //BW = 20, Nsel = 64	
                            //D = valid values in table below

                            // BW (MHz)      D 
                            // 20            2, 4, 8
                            // 40            2, 4, 8
                            // 80            8

                            UINT16 fft_length, fft_skip, adc_dec, time, bw; 

						    fft_length = atohex2(param[2]) & 0x03;
                            fft_skip   = atohex2(param[3]) & 0x03;
                            adc_dec    = atohex2(param[4]) & 0x03;
                            time       = atohex2(param[5]);

                            printk("SSU Input fft_length = %x fft_skip = %x adc_dec = %x time = %d \n", fft_length, fft_skip, adc_dec, time);
                            

                            if (mib->PhyDSSSTable->Chanflag.ChnlWidth == CH_20_MHz_WIDTH)
                            {
                                fft_length = SSU_FFT_SIZE_64; /* only FFT size 64 supported for 20 MHz */
                                bw = 20;
                            }
                            else if (mib->PhyDSSSTable->Chanflag.ChnlWidth == CH_40_MHz_WIDTH)
                            {
                                fft_length = SSU_FFT_SIZE_128;
                                if (adc_dec == SSU_ADC_DECIMATION_0)
                                    adc_dec = SSU_ADC_DECIMATION_2;
                                bw = 40;
                            }
                            else if ((mib->PhyDSSSTable->Chanflag.ChnlWidth == CH_80_MHz_WIDTH) ||
                                     (mib->PhyDSSSTable->Chanflag.ChnlWidth == CH_AUTO_WIDTH  ))
                            {
                                fft_length = SSU_FFT_SIZE_256;
                                adc_dec = SSU_ADC_DECIMATION_8; /* only decimation of 8 supported for 80 MHZ */                                
                                bw = 80;
                            }
                            else
                            {
                                fft_length = 0;
                                adc_dec = 0;
                                bw = 20;
                            }                                

                            buffer_size = 28 + (2*fft_length_lookup[fft_length] + 16*bw/adc_dec_lookup[adc_dec]);

                            printk("SSU BW = %d FFT size = %d  ADC size = %d packet size = %d \n",
                                   bw,
                                   2*fft_length_lookup[fft_length],
                                   16*bw/adc_dec_lookup[adc_dec],
                                   buffer_size);

                            if (wlFwSetSpectralAnalysis(netdev, 4, buffer_size, (UINT32)priv->wlpd_p->pPhysSsuBuf,
                                                        fft_length, fft_skip, adc_dec, time))
                                printk("SSU Error - command error.\n");
                            else
                                printk("ssutest : start \n");

                        }
						
					}
#endif
#ifdef QUEUE_STATS
					else if	((strcmp(param[0], "qstats") == 0))
					{
    #ifdef QUEUE_STATS_CNT_HIST
					    if((strcmp(param[1], "pktcount") == 0))
					    {
					        if(strlen(param[2]) > 0)
					        {
					            dbgUdpSrcVal = atoi(param[2]);
					        }
                            wlFwGetQueueStats(netdev, QS_GET_TX_COUNTER);
					    }
					    else if((strcmp(param[1], "retry_histogram") == 0))
					    {
                            wlFwGetQueueStats(netdev, QS_GET_RETRY_HIST);
					    }
					    else if((strcmp(param[1], "txrate_histogram") == 0))
					    {
					        int indx;
					        for(indx=0; indx<QS_NUM_STA_SUPPORTED; indx++)
					        {
					            //use upper nibble for STA index
                                if(wlFwGetQueueStats(netdev, (QS_GET_TX_RATE_HIST|(indx<<4))) == 1)
                                {
                                    if(indx == 0)
                                    {
                                        printk("\nRate Histogram => no available data\n");
                                    }
                                    break;
                                }
					        }
					    }
					    else if((strcmp(param[1], "rxrate_histogram") == 0))
					    {
                            if(wlFwGetQueueStats(netdev, QS_GET_RX_RATE_HIST) == 1)
                            {
                                printk("\nRx Rate Histogram => no available data\n");
                            }
					    }
						else if((strcmp(param[1], "addrxmac") == 0))
					    {
					        int k;
					        for(k=0; k<QS_NUM_STA_SUPPORTED; k++)
					        {
					            if(strlen(param[k+2]) == 12)
					            {
                                    getMacFromString(rxPktStats_sta[k].addr, param[k+2]);
                                    rxPktStats_sta[k].valid=1;
        							printk( "Added Rx STA: %02x %02x %02x %02x %02x %02x\n", 
        								rxPktStats_sta[k].addr[0], 
        								rxPktStats_sta[k].addr[1], 
        								rxPktStats_sta[k].addr[2], 
        								rxPktStats_sta[k].addr[3], 
        								rxPktStats_sta[k].addr[4], 
        								rxPktStats_sta[k].addr[5]);
        						    memcpy(&qs_rxMacAddrSave[k*6],rxPktStats_sta[k].addr, 6);
								}
								else
								{
								    break;
								}
							}
							numOfRxSta = k;
							wlFwSetMacSa(netdev, numOfRxSta,(UINT8 *)qs_rxMacAddrSave);
                        }
						else if((strcmp(param[1], "addtxmac") == 0))
					    {
					        int k;
					        for(k=0; k<QS_NUM_STA_SUPPORTED; k++)
					        {
					            if(strlen(param[k+2]) == 12)
					            {
                                    getMacFromString(txPktStats_sta[k].addr, param[k+2]);
                                    txPktStats_sta[k].valid=1;
                                    printk( "Added Tx STA: %02x %02x %02x %02x %02x %02x\n", 
                                        (int)txPktStats_sta[k].addr[0], 
                                        (int)txPktStats_sta[k].addr[1], 
                                        (int)txPktStats_sta[k].addr[2], 
                                        (int)txPktStats_sta[k].addr[3], 
                                        (int)txPktStats_sta[k].addr[4], 
                                        (int)txPktStats_sta[k].addr[5]);
					            }
					            else
					            {
    					            break;
					            }
					        }
                        }
    #endif
    #ifdef QUEUE_STATS_LATENCY
					    if((strcmp(param[1], "txlatency") == 0))
					    {
                            wlFwGetQueueStats(netdev, QS_GET_TX_LATENCY);
					    }
						if((strcmp(param[1], "rxlatency") == 0))
					    {
                            wlFwGetQueueStats(netdev, QS_GET_RX_LATENCY);
					    }
    #endif
                        if ((strcmp(param[1], "reset") == 0))
                        {
                            wlFwResetQueueStats(netdev);
                        }
					}
#endif

#ifdef SOC_W8864
					else if(strcmp(param[0], "rccal") == 0)
					{
						extern int wlFwSetRCcal( struct net_device *netdev);
						wlFwSetRCcal(netdev);
                        printk("RC Cal done\n");
					}
					else if(strcmp(param[0], "gettemp") == 0)
					{
						extern int wlFwGetTemp( struct net_device *netdev);
						wlFwGetTemp(netdev);
					}
#endif
					/* Cmd to set limit number of stations that can assoc to a virtual interface. Each virtual interface has a separate limit.
					  * "macMgmtMlme_AssocReAssocReqHandler" function will check the limit. If over limit, error status is sent in assoc resp
					 **/
					else if	((strcmp(param[0], "maxsta") == 0))
					{  	
						int val;
						//Only take virtual interface as input
						if(!is_the_cmd_applicable(cmd) && !priv->master)
						{	printk("Error. Please enter virtual interface instead\n");
							rc = -EOPNOTSUPP; 
							kfree(param); 
							return rc; 
						} 
						val = atoi(param[1]);
						if (val < 1 || val > MAX_STNS)
						{
							printk("Incorrect value. Value between 1 to 64 only. Default is 64\n");
							break;
						}
						*(mib->mib_maxsta) = (UINT8 )val;
						printk("Configure %s max station limit = %d\n", netdev->name, (int)*(mib->mib_maxsta));
						break;
					}
					else if	((strcmp(param[0], "getmaxsta") == 0))
					{
						//Only take virtual interface as input
						if(!is_the_cmd_applicable(cmd) && !priv->master)
						{	printk("Error. Please enter virtual interface instead\n");
							rc = -EOPNOTSUPP; 
							kfree(param); 
							return rc; 
						} 
						printk("Max station limit in %s is %d\n", netdev->name, (int)*(mib->mib_maxsta));
						break;
					}
					else if	((strcmp(param[0], "txfaillimit") == 0))
					{
						int val;
						//Only take parent interface as input
						if(priv->master)
						{	
							printk("Error. Please enter parent interface %s instead\n", priv->master->name);
							rc = -EOPNOTSUPP; 
							kfree(param); 
							return rc; 
						} 
									
						val = atoi(param[1]);
						if (val >= 0)
							*(mib->mib_consectxfaillimit) = val;
						else{
							printk("Error. Please enter value >= 0\n");
							break;
						}
									
						if(!wlFwSetConsecTxFailLimit(netdev, *(mib->mib_consectxfaillimit)))
						{	
							if (*(mib->mib_consectxfaillimit))
								printk("Config %s txfail limit > %d\n", netdev->name, (int)*(mib->mib_consectxfaillimit));
							else
								printk("txfail limit is disabled\n");
						}

						break;
					}
					else if ((strcmp(param[0], "gettxfaillimit") == 0))
					{
						UINT32 val;
						
						//Only take parent interface as input
						if(priv->master)
						{	
							printk("Error. Please enter parent interface %s instead\n", priv->master->name);
							rc = -EOPNOTSUPP; 
							kfree(param); 
							return rc; 
						} 
						if(!wlFwGetConsecTxFailLimit(netdev, (UINT32*)&val))
						{
							if(val)
								printk("Consecutive txfail limit > %d\n", (int)val);
							else
								printk("txfail limit is disabled\n");
						}
					
						break;
					}
					#ifdef MRVL_WAPI
					else if	((strcmp(param[0], "wapi") == 0))
					{
						char macaddr[6];
						u16 auth_type;

						if (strcmp(param[1], "ucast_rekey") == 0) {
							auth_type = 0x00F2;
							if (!getMacFromString(macaddr, param[2]))
							{
								rc = -EFAULT;
								break;
							}
						}
						else if (strcmp(param[1], "mcast_rekey") == 0) {
							auth_type = 0x00F4;
							memcpy(macaddr, bcastMacAddr, 6);
						}
						else 
						{
							rc = -EFAULT;
							break;
						}

						macMgmtMlme_WAPI_event(netdev, IWEVASSOCREQIE, auth_type, macaddr, netdev->dev_addr, NULL);
					}
					#endif
#ifdef WNC_LED_CTRL
					else if ((strcmp(param[0], "led") == 0))
					{
						if (strcmp(param[1], "on") == 0) {
							printk("set led on ...\n");
							wlFwLedOn(netdev, 1);
						} else if (strcmp(param[1], "off") == 0) {
							printk("set led off ...\n");
							wlFwLedOn(netdev, 0);
						} else {
							rc = -EFAULT;
						}	
						break;
					}
#endif

#ifdef CLIENT_SUPPORT
					/*Set client mode to send Probe Req during tx or not*/
					else if ((strcmp(param[0], "fastreconnect") == 0))
					{
						ProbeReqOnTx = atoi(param[1]);
						if (ProbeReqOnTx < 0 || ProbeReqOnTx > 1)
						{
							printk("Pls submit value 0 or 1 only\n");
							ProbeReqOnTx = 0;
							rc = -EOPNOTSUPP;
						}
												
						break;
					}
#endif

					else {
						rc = -EFAULT;
						break;
					}
		}
		break;

#ifdef MRVL_WSC
	case WL_IOCTL_SET_APPIE:
		{
			WSC_COMB_IE_t APWSCIE;
			UINT16	ieType = 0;
			struct wlreq_set_appie *appie = (struct wlreq_set_appie *)param_str ;

			memset(&APWSCIE, 0, sizeof(WSC_COMB_IE_t));

			if (appie == NULL)
			{
				break ;
			}

			if (appie->appBufLen == 8)
			{
				memset(&vmacSta_p->thisbeaconIE, 0, sizeof(WSC_BeaconIE_t));
				memset(&vmacSta_p->thisprobeRespIE, 0, sizeof(WSC_ProbeRespIE_t));
				vmacSta_p->WPSOn = 0 ;
			}

			if (appie->appBufLen < 8)
			{
				printk("incorrect wsc_ie at the driver\n");
				break ;
			}

			if (appie->appFrmType == WL_APPIE_FRAMETYPE_BEACON && (appie->appBufLen > 8))
			{
				ieType = 0 ;
				APWSCIE.beaconIE.ElementId = 221 ;
				APWSCIE.beaconIE.Len = appie->appBuf[1];
				APWSCIE.beaconIE.OUI[0] = 0x00;
				APWSCIE.beaconIE.OUI[1] = 0x50;
				APWSCIE.beaconIE.OUI[2] = 0xF2;
				APWSCIE.beaconIE.OUI[3] = 0x04;

				memcpy(&APWSCIE.beaconIE.WSCData[0], &appie->appBuf[6], appie->appBufLen - 6);
				memcpy(&vmacSta_p->thisbeaconIE, &APWSCIE.beaconIE, sizeof(WSC_BeaconIE_t));
				vmacSta_p->WPSOn = 1 ;
			}
			else if (appie->appFrmType == WL_APPIE_FRAMETYPE_PROBE_RESP && (appie->appBufLen > 8))
			{
				ieType = 1 ;
				APWSCIE.probeRespIE.ElementId = 221 ;
				APWSCIE.probeRespIE.Len = appie->appBuf[1];
				APWSCIE.probeRespIE.OUI[0] = 0x00;
				APWSCIE.probeRespIE.OUI[1] = 0x50;
				APWSCIE.probeRespIE.OUI[2] = 0xF2;
				APWSCIE.probeRespIE.OUI[3] = 0x04;
				memcpy(&APWSCIE.probeRespIE.WSCData[0], &appie->appBuf[6], appie->appBufLen - 6);
				memcpy(&vmacSta_p->thisprobeRespIE, &APWSCIE.probeRespIE, sizeof(WSC_ProbeRespIE_t));
				vmacSta_p->WPSOn = 1 ;
			}
			else
			{
				/* Remove beacon IE */
				memset(&vmacSta_p->thisbeaconIE, 0, sizeof(WSC_BeaconIE_t));
				ieType = 0;
				if(wlFwSetWscIE(netdev, ieType, &APWSCIE ))
				{
					WLDBG_EXIT_INFO(DBG_LEVEL_1, "Failed setting WSC IE");
				}

				/* Remove Probe response IE */
				memset(&vmacSta_p->thisprobeRespIE, 0, sizeof(WSC_ProbeRespIE_t));
				ieType = 1;
				if(wlFwSetWscIE(netdev, ieType, &APWSCIE ))
				{
					WLDBG_EXIT_INFO(DBG_LEVEL_1, "Failed setting WSC IE");
				}

				vmacSta_p->WPSOn = 0 ;
				break;
			}

			if(wlFwSetWscIE(netdev, ieType, &APWSCIE ))
			{
				WLDBG_EXIT_INFO(DBG_LEVEL_1, "Failed setting WSC IE");
			}
		}
		break;
#endif //MRVL_WSC

#ifdef MRVL_WAPI 
	/* allow multiple IEs, all contents are prepared by upper layer (caller), can be used for generic IE */
	case WL_IOCTL_SET_WAPI:
		{
			WAPI_COMB_IE_t WAPIIE;
			UINT16 ieType = 0;
			struct wlreq_set_appie *appie = (struct wlreq_set_appie *)param_str ;

			/* Note: parame_str points to ioctl data from wapid:
				u32  io_packet;
				struct  _iodata
				{
					u32 wDataLen;
					char pbData[96];
				}iodata;

				use wlreq_set_appie to parse the data because its data struct is same.
			*/

			/* wapi ioctl (from wapid to driver) coming in */

			memset(&WAPIIE, 0, sizeof(WAPI_COMB_IE_t));

			if (appie == NULL)
			{
				break ;
			}

			if (appie->appFrmType == P80211_PACKET_WAPIFLAG) {			
				mib->Privacy->WAPIEnabled = 1;
				vmacSta_p->Mib802dot11->Privacy->WAPIEnabled = 1;
				wlFwSetApBeacon(netdev);
			}
			else if (appie->appFrmType == P80211_PACKET_SETKEY) {			
					struct wlreq_wapi_key *wk = (struct wlreq_wapi_key *)appie->appBuf;
					int gkey=0;
					extern int wlFwSetWapiKey(struct net_device *netdev, struct wlreq_wapi_key *wapi_key, int groupkey);

					/* for mcst key, use bssid to replace bcast MAC */
					if ( memcmp(wk->ik_macaddr, bcastMacAddr, 6) == 0 ) {
						memcpy(wk->ik_macaddr, vmacSta_p->macBssId, 6);
						gkey=1;
					}

				    wlPrintData(DBG_LEVEL_1|DBG_CLASS_DATA,  __FUNCTION__, wk->ik_keydata, 32, NULL);

					/* appie->appBuf = wapid's pbData: MAC + 1 + Keyindex + key + key-mic */
					if( vmacSta_p->VMacEntry.modeOfService == VMAC_MODE_AP )
					{
						wlFwSetWapiKey(netdev, wk, gkey);
					}
#ifdef CLIENT_SUPPORT
					/* to do */
#endif

				break;
			}
			else if (appie->appFrmType == WL_APPIE_FRAMETYPE_BEACON && (appie->appBufLen > 8))
			{
				ieType = 0 ;
				WAPIIE.beaconIE.Len = appie->appBufLen;	// the len already counts IE-id and IE-length (1 byte each)
				memcpy(&WAPIIE.beaconIE.WAPIData[0], &appie->appBuf[0], appie->appBufLen );
				memcpy(&vmacSta_p->thisbeaconIEs, &WAPIIE.beaconIE, sizeof(WAPI_BeaconIEs_t));
			}
			else if (appie->appFrmType == WL_APPIE_FRAMETYPE_PROBE_RESP && (appie->appBufLen > 8))
			{
				ieType = 1 ;
				WAPIIE.probeRespIE.Len = appie->appBufLen;
				memcpy(&WAPIIE.probeRespIE.WAPIData[0], &appie->appBuf[0], appie->appBufLen);
				memcpy(&vmacSta_p->thisprobeRespIEs, &WAPIIE.probeRespIE, sizeof(WAPI_ProbeRespIEs_t));
			}
			else
			{
				#if 0 /* disable wapip, todo */
				mib->Privacy->WAPIEnabled = 0;
				/* Remove beacon IE */
				memset(&vmacSta_p->thisbeaconIEs, 0, sizeof(WAPI_BeaconIEs_t));
				ieType = 0;
				if(wlFwSetWapiIE(netdev, ieType, &WAPIIE ))
				{
					WLDBG_EXIT_INFO(DBG_LEVEL_1, "Failed setting APPS IE");
				}

				/* Remove Probe response IE */
				memset(&vmacSta_p->thisprobeRespIEs, 0, sizeof(WAPI_ProbeRespIEs_t));
				ieType = 1;
				if(wlFwSetWapiIE(netdev, ieType, &WAPIIE ))
				{
					WLDBG_EXIT_INFO(DBG_LEVEL_1, "Failed setting APPS IE");
				}
				#endif
				break;
			}

			if(wlFwSetWapiIE(netdev, ieType, &WAPIIE ))
			{
				WLDBG_EXIT_INFO(DBG_LEVEL_1, "Failed setting APPS IE");
			}
		}
		break;
#endif //MRVL_WAPI

	default:

		if (cmd >= SIOCSIWCOMMIT && cmd <= SIOCGIWPOWER)
		{
			rc = -EOPNOTSUPP;
			break;
		}

		WLDBG_INFO(DBG_LEVEL_1,  "unsupported ioctl(0x%04x)\n", cmd);

		rc = -EOPNOTSUPP;

		break;

	}

	if (ret_str != NULL)
	{
		if (copy_to_user(ret_str, bufBack, *ret_len))	  
		{
			rc = -EFAULT;
		}
	}

	WLDBG_EXIT(DBG_LEVEL_1);
	kfree(param);
	return rc;
}

int wlIoctlGet(struct net_device *netdev, int cmd, char *param_str,
			   int param_len, char *ret_str, UINT16 *ret_len)
{
	struct wlprivate *priv = NETDEV_PRIV_P(struct wlprivate, netdev);
	vmacApInfo_t *vmacSta_p = priv->vmacSta_p;
	MIB_802DOT11 *mib = vmacSta_p->ShadowMib802dot11;
	int rc = 0;
	char *buf=cmdGetBuf;
	char param[MAX_IOCTL_PARAMS][MAX_IOCTL_PARAM_LEN];
#ifdef MPRXY
	UINT32 tempIPAddr;
	UINT8 * tmpStr= NULL;
	UINT32 i,j;
#endif

	WLDBG_ENTER(DBG_LEVEL_1);
	switch (cmd)
	{
	case WL_IOCTL_GET_VERSION:
		{
			UINT8 *pVer = (UINT8 *)&priv->hwData.fwReleaseNumber;

			sprintf(buf, "Driver version: %s, Firmware version: %d.%d.%d.%d\n", DRV_VERSION, *(pVer+3), *(pVer+2), *(pVer+1), *pVer);
		}

		break;
	case WL_IOCTL_GET_TXRATE:
		{
#ifdef SOC_W8864		
			int b_rate=2, g_rate=2, n_rate=0, a_rate, vht_rate, m_rate, manage_rate, i=0;
#else			
			int b_rate=2, g_rate=2, n_rate=0, a_rate, m_rate, manage_rate, i=0;
#endif			
			char *p = buf;
			int rateMask;

			if(*(mib->mib_enableFixedRateTx)== 0)
			{
				sprintf(buf, "Auto Rate\n");
			}else
			{
				b_rate = *(mib->mib_txDataRate);
				g_rate = *(mib->mib_txDataRateG);
				a_rate = *(mib->mib_txDataRateA);
#ifdef SOC_W8864				
				vht_rate = *(mib->mib_txDataRateVHT);
#endif
				if(*(mib->mib_FixedRateTxType))
					n_rate = *(mib->mib_txDataRateN) + 256;
				else 
					n_rate = *(mib->mib_txDataRateN);
#ifdef SOC_W8864
				sprintf(buf, "B Rate: %d, G Rate: %d, A Rate: %d, N Rate: %d, vht Rate: 0x%x\n", b_rate, g_rate, a_rate, n_rate, vht_rate);
#else
                sprintf(buf, "B Rate: %d, G Rate: %d, A Rate: %d, N Rate: %d\n", b_rate, g_rate, a_rate, n_rate);
#endif
			}
			if(*(mib->mib_MultiRateTxType))
				m_rate = *(mib->mib_MulticastRate)+256;
			else
				m_rate = *(mib->mib_MulticastRate);
			manage_rate = *(mib->mib_ManagementRate);
			p += strlen(buf);
			sprintf(p, "Multicast Rate: %d, Management Rate: %d\n", m_rate, manage_rate);
#ifdef BRS_SUPPORT
			p = buf + strlen(buf);
			sprintf(p, "BSS Basic Rate: ");

			p = buf + strlen(buf);

			rateMask = *(mib->BssBasicRateMask);
			i = 0;
			while(rateMask) 
			{
				if (rateMask & 0x01)
				{
					if (mib->StationConfig->OpRateSet[i])
					{		
						sprintf(p, "%d ", mib->StationConfig->OpRateSet[i]);
						p = buf + strlen(buf);
					}
				}
				rateMask >>= 1;
				i++;
			}

			p = buf + strlen(buf);
			sprintf(p, "\nNot BSS Basic Rate: ");

			p = buf + strlen(buf);
			rateMask = *(mib->NotBssBasicRateMask);
			i = 0;
			while(rateMask) 
			{
				if (rateMask & 0x01)
				{
					if (mib->StationConfig->OpRateSet[i])
					{		
						sprintf(p, "%d ", mib->StationConfig->OpRateSet[i]);
						p = buf + strlen(buf);
					}
				}
				rateMask >>= 1;
				i++;
			}
#endif
		}
		break;

	case WL_IOCTL_GET_CIPHERSUITE:
		sprintf(buf,"\n");
		if (mib->RSNConfigWPA2->WPA2Enabled && !mib->RSNConfigWPA2->WPA2OnlyEnabled)
		{
			strcat(buf,"Mixed Mode  ");
			if (mib->UnicastCiphers->UnicastCipher[3]==0x02)
				strcat(buf,"wpa:tkip  ");
			else if (mib->UnicastCiphers->UnicastCipher[3]==0x04)
				strcat(buf,"wpa:aes  ");
			else
				strcat(buf,"wpa:  ciphersuite undefined ");

			if (mib->WPA2UnicastCiphers->UnicastCipher[3]==0x04)
				strcat(buf,"wpa2:aes  ");
			else if (mib->WPA2UnicastCiphers->UnicastCipher[3]==0x02)
				strcat(buf,"wpa2:tkip  ");
			else
				strcat(buf,"wpa2:ciphersuite undefined  ");

			if (mib->RSNConfig->MulticastCipher[3]== 0x02)
				strcat(buf,"multicast:tkip \n");
			else if (mib->RSNConfig->MulticastCipher[3]==0x04)
				strcat(buf,"multicast:aes \n");
			else
				strcat(buf,"multicast:ciphersuite undefined \n");
		}
		else
		{
			if ((mib->UnicastCiphers->UnicastCipher[3]==0x02) &&
				(mib->RSNConfig->MulticastCipher[3]   == 0x02))
				strcat(buf,"wpa:tkip  ");
			else if ((mib->UnicastCiphers->UnicastCipher[3]==0x04) &&
				(mib->RSNConfig->MulticastCipher[3]   ==0x04))
				strcat(buf,"wpa:aes  ");
			else
				strcat(buf,"wpa:ciphersuite undefined  ");

			if ((mib->WPA2UnicastCiphers->UnicastCipher[3]==0x04) &&
				(mib->RSNConfigWPA2->MulticastCipher[3]   ==0x04))
				strcat(buf,"wpa2:aes \n");
			else if ((mib->WPA2UnicastCiphers->UnicastCipher[3]==0x02) &&
				(mib->RSNConfigWPA2->MulticastCipher[3]   ==0x02))
				strcat(buf,"wpa2:tkip \n");
			else
				strcat(buf,"wpa2:ciphersuite undefined \n");
		}

		break;

	case WL_IOCTL_GET_PASSPHRASE:
		sprintf(buf, "wpa: %s, wpa2: %s\n", mib->RSNConfig->PSKPassPhrase, mib->RSNConfigWPA2->PSKPassPhrase);
		break;

	case WL_IOCTL_GET_FILTERMAC:
		{
			UCHAR buf1[48], *filter_buf=mib->mib_wlanfiltermac;
			char *out_buf=buf;
			int i;

			sprintf(out_buf, "\n");
			out_buf++;
			for(i=0; i<FILERMACNUM; i++)
			{
				sprintf(buf1, "MAC %d: %02x:%02x:%02x:%02x:%02x:%02x\n", (i+1), *(filter_buf+i*6),
					*(filter_buf+i*6+1), *(filter_buf+i*6+2), *(filter_buf+i*6+3),
					*(filter_buf+i*6+4), *(filter_buf+i*6+5));
				sprintf(out_buf,"%s", buf1);																
				out_buf += strlen(buf1);
			}

		}
		break;

	case WL_IOCTL_GET_BSSID:
		{
			MIB_OP_DATA *mib_OpData = mib->OperationTable;

			sprintf(buf, "MAC %02x:%02x:%02x:%02x:%02x:%02x\n", 
				mib_OpData->StaMacAddr[0],
				mib_OpData->StaMacAddr[1],
				mib_OpData->StaMacAddr[2],
				mib_OpData->StaMacAddr[3],
				mib_OpData->StaMacAddr[4],
				mib_OpData->StaMacAddr[5]);

		}
		break;

	case WL_IOCTL_GET_WMMEDCAAP:
		{
			extern mib_QAPEDCATable_t mib_QAPEDCATable[4];
			int cw_min, cw_max, aifsn, tx_op_lim_b, tx_op_lim;
			char strName[4][6] = {"AC_BE", "AC_BK", "AC_VI", "AC_VO"};
			char strBuf[4][256];
			int i;

			for (i=0; i<4; i++)
			{
				cw_min = mib_QAPEDCATable[i].QAPEDCATblCWmin;
				cw_max = mib_QAPEDCATable[i].QAPEDCATblCWmax;
				aifsn = mib_QAPEDCATable[i].QAPEDCATblAIFSN; 
				tx_op_lim = mib_QAPEDCATable[i].QAPEDCATblTXOPLimit;
				tx_op_lim_b = mib_QAPEDCATable[i].QAPEDCATblTXOPLimitBAP;

				sprintf(&(strBuf[i][0]), "\n%s %d %d %d %d %d\n", strName[i], cw_min, cw_max, aifsn, tx_op_lim_b, tx_op_lim);
			}
			sprintf(buf, "%s%s%s%s", strBuf[0], strBuf[1], strBuf[2], strBuf[3]);
		}
		break;

	case WL_IOCTL_GET_WMMEDCASTA:
		{
			extern mib_QStaEDCATable_t mib_QStaEDCATable[4];
			int cw_min, cw_max, aifsn, tx_op_lim_b, tx_op_lim, acm;
			char strName[4][6] = {"AC_BE", "AC_BK", "AC_VI", "AC_VO"};
			char strBuf[4][256];
			int i;

			for (i=0; i<4; i++)
			{
				cw_min = mib_QStaEDCATable[i].QStaEDCATblCWmin;
				cw_max = mib_QStaEDCATable[i].QStaEDCATblCWmax;
				aifsn = mib_QStaEDCATable[i].QStaEDCATblAIFSN; 
				tx_op_lim = mib_QStaEDCATable[i].QStaEDCATblTXOPLimit;
				tx_op_lim_b = mib_QStaEDCATable[i].QStaEDCATblTXOPLimitBSta;
				acm = mib_QStaEDCATable[i].QStaEDCATblMandatory;
				sprintf(&(strBuf[i][0]), "\n%s %d %d %d %d %d %d\n", strName[i], cw_min, cw_max, aifsn, tx_op_lim_b, tx_op_lim, acm);
			}
			sprintf(buf, "%s%s%s%s", strBuf[0], strBuf[1], strBuf[2], strBuf[3]);
		}
		break;
	case WL_IOCTL_GET_STALISTEXT:
		{
			UCHAR *sta_buf, *show_buf, buf1[64];
			char *out_buf = buf;
			int i, entries;
			extStaDb_StaInfo_t *pStaInfo;
			char tmpBuf[48];
			entries = extStaDb_entries(vmacSta_p,0);

			memset(tmpBuf, 0, sizeof(tmpBuf));

			sta_buf = kmalloc(entries *64,GFP_KERNEL);
			if (sta_buf == NULL) 
			{
				rc = -EFAULT;
				break; 
			}

			extStaDb_list(vmacSta_p,sta_buf, 1);

			if(entries)
			{
				show_buf = sta_buf;
				sprintf(out_buf, "\n");
				out_buf++;
				for(i=0; i<entries; i++)
				{
					if ( (pStaInfo = extStaDb_GetStaInfo(vmacSta_p,(IEEEtypes_MacAddr_t *)show_buf, 0)) == NULL )
					{
						kfree(sta_buf);
						rc = -EFAULT;
						return rc;
					}
					switch (pStaInfo->ClientMode)
					{
					case BONLY_MODE:
						strcpy(&tmpBuf[0], "b ");
						break;

					case GONLY_MODE:
						strcpy(&tmpBuf[0], "g ");
						break;							

					case NONLY_MODE:
						strcpy(&tmpBuf[0], "n ");
						break;

					case AONLY_MODE:
						strcpy(&tmpBuf[0], "a ");
						break;

					default:
						strcpy(&tmpBuf[0], "NA ");
						break;

					}	

					switch (pStaInfo->State)
					{

					case UNAUTHENTICATED:
						strcat(tmpBuf, "UNAUTHENTICATED ");
						break;

					case SME_INIT_AUTHENTICATING:
					case EXT_INIT_AUTHENTICATING:
						strcat(tmpBuf, "AUTHENTICATING ");
						break;

					case AUTHENTICATED:
						strcat(tmpBuf, "AUTHENTICATED ");
						break;

					case SME_INIT_DEAUTHENTICATING:
					case EXT_INIT_DEAUTHENTICATING:
						strcat(tmpBuf, "DEAUTHENTICATING ");
						break;

					case SME_INIT_ASSOCIATING:
					case EXT_INIT_ASSOCIATING:
						strcat(tmpBuf, "ASSOCIATING ");
						break;

					case ASSOCIATED:
						{
							int flagPsk = 0;
							if ((mib->Privacy->RSNEnabled == 1) || (mib->RSNConfigWPA2->WPA2Enabled == 1))
							{

								if (*(mib->mib_wpaWpa2Mode) < 4) /* For PSK modes use internal WPA state machine */
								{
									if (pStaInfo->keyMgmtHskHsm.super.pCurrent!= NULL)
									{
										if (pStaInfo->keyMgmtHskHsm.super.pCurrent== &pStaInfo->keyMgmtHskHsm.hsk_end)
										{
											strcat(tmpBuf, "PSK-PASSED ");
											flagPsk = 1;
										}
									}
								}		
							}
							if (!flagPsk)
								strcat(tmpBuf, "ASSOCIATED ");
						}
						break;

					case SME_INIT_REASSOCIATING:
					case EXT_INIT_REASSOCIATING:
						strcat(tmpBuf, "REASSOCIATING ");
						break;

					case SME_INIT_DEASSOCIATING:
					case EXT_INIT_DEASSOCIATING:
						strcat(tmpBuf, "DEASSOCIATING ");
						break;
					default:
						break;
					}

#ifdef SOC_W8764
					sprintf(buf1, "%d: %02x:%02x:%02x:%02x:%02x:%02x %s Rate %d Mbps, RSSI %d   A %d  B %d  C %d  D %d\n", 
						i+1, 
						*show_buf, *(show_buf+1), *(show_buf+2), *(show_buf+3), *(show_buf+4), *(show_buf+5),
						tmpBuf, 
						//pStaInfo->RateInfo.RateIDMCS,
						(int)getPhyRate((dbRateInfo_t *)&(pStaInfo->RateInfo)),
						pStaInfo->RSSI,
                        pStaInfo->RSSI_path.a,
                        pStaInfo->RSSI_path.b,
                        pStaInfo->RSSI_path.c,
                        pStaInfo->RSSI_path.d);
#else
					sprintf(buf1, "%d: %02x:%02x:%02x:%02x:%02x:%02x %s Rate %d Mbps, RSSI %d\n", 
						i+1, 
						*show_buf, *(show_buf+1), *(show_buf+2), *(show_buf+3), *(show_buf+4), *(show_buf+5),
						tmpBuf, 
						//pStaInfo->RateInfo.RateIDMCS,
						(int)getPhyRate((dbRateInfo_t *)&(pStaInfo->RateInfo)),
						pStaInfo->RSSI);	   
#endif


					show_buf += sizeof(STA_INFO);
					strcpy(out_buf, buf1);
					out_buf += strlen(buf1);
				}
			} else
			{
				out_buf[0] = 0;
			}
			kfree(sta_buf);
		}
		break;

	case WL_IOCTL_GET_TXPOWER:
		{
			int i;
			char *out_buf=buf;
#if defined(SOC_W8366)||defined(SOC_W8764)
			UINT16 powlist[TX_POWER_LEVEL_TOTAL];
#ifdef SOC_W8864
			UINT16 tmp_bw = mib->PhyDSSSTable->Chanflag.ChnlWidth;
#else
			UINT16 tmp_bw = (mib->PhyDSSSTable->Chanflag.ChnlWidth==CH_AUTO_WIDTH)?CH_40_MHz_WIDTH:mib->PhyDSSSTable->Chanflag.ChnlWidth;	
#endif

			wlFwGettxpower(netdev, powlist, mib->PhyDSSSTable->CurrChan,
				mib->PhyDSSSTable->Chanflag.FreqBand, tmp_bw,
				mib->PhyDSSSTable->Chanflag.ExtChnlOffset);
			sprintf(out_buf, "\nCurrent Channel Power level list (FW) :");       
			out_buf += strlen("\nCurrent Channel Power level list (FW) :");
			for (i = 0; i < TX_POWER_LEVEL_TOTAL; i++)
			{
				sprintf(out_buf, "0x%02x ",powlist[i]);
				out_buf += strlen("0x00 ");
			}
			sprintf(out_buf, "\n");
			out_buf++;
#else
			char strBuf[14][32];
			if (*(mib->mib_ApMode) & AP_MODE_A_ONLY)
			{
				sprintf(out_buf, "\n20M(5G):\n");
				out_buf += strlen("\n20M(5G):\n");
				for (i=0; i<PWTAGETRATETABLE20M_5G_ENTRY; i++)
				{
					sprintf(out_buf, "%03d 0x%02x 0x%02x 0x%02x\n", 
						mib->PowerTagetRateTable20M_5G[i*4 + 0],
						mib->PowerTagetRateTable20M_5G[i*4 + 1],
						mib->PowerTagetRateTable20M_5G[i*4 + 2],
						mib->PowerTagetRateTable20M_5G[i*4 + 3]);
					out_buf += strlen("000 0x00 0x00 0x00\n");
				}
				sprintf(out_buf, "\n40M(5G):\n");
				out_buf += strlen("\n40M(5G):\n");
				for (i=0; i<PWTAGETRATETABLE40M_5G_ENTRY; i++)
				{ 
					sprintf(out_buf, "%03d 0x%02x 0x%02x 0x%02x\n",
						mib->PowerTagetRateTable40M_5G[i*4 + 0],
						mib->PowerTagetRateTable40M_5G[i*4 + 1],
						mib->PowerTagetRateTable40M_5G[i*4 + 2],
						mib->PowerTagetRateTable40M_5G[i*4 + 3]);
					out_buf += strlen("000 0x00 0x00 0x00\n");
				}
			}
			else
			{  // 2.4G
				for (i=0; i<14; i++)
				{ 
					sprintf(&(strBuf[i][0]), "%02d:  0x%02x 0x%02x 0x%02x 0x%02x\n", i+1,
						mib->PowerTagetRateTable20M[i*4 + 0],
						mib->PowerTagetRateTable20M[i*4 + 1],
						mib->PowerTagetRateTable20M[i*4 + 2],
						mib->PowerTagetRateTable20M[i*4 + 3]);
				}
				sprintf(out_buf, "\n20M(2.4G):\n%s%s%s%s%s%s%s%s%s%s%s%s%s%s", 
					strBuf[0], strBuf[1], strBuf[2], strBuf[3], strBuf[4], strBuf[5], strBuf[6],
					strBuf[7], strBuf[8], strBuf[9], strBuf[10], strBuf[11], strBuf[12], strBuf[13]);

				memset(&(strBuf[0][0]), 0, sizeof(strBuf));
				out_buf += strlen(out_buf);

				for (i=0; i<9; i++)
				{ 
					sprintf(&(strBuf[i][0]), "%02d:  0x%02x 0x%02x 0x%02x 0x%02x\n", i+1,
						mib->PowerTagetRateTable40M[i*4 + 0],
						mib->PowerTagetRateTable40M[i*4 + 1],
						mib->PowerTagetRateTable40M[i*4 + 2],
						mib->PowerTagetRateTable40M[i*4 + 3]);
				}
				sprintf(out_buf, "\n40M(2.4G):\n%s%s%s%s%s%s%s%s%s", 
					strBuf[0], strBuf[1], strBuf[2], strBuf[3], strBuf[4], strBuf[5], strBuf[6], strBuf[7], strBuf[8]);
			}
#endif
		}
		break;

		/*MRV_8021X*/
#ifdef GENERIC_GETIE
	case WL_IOCTL_GET_IE:
		{
			struct wlreq_ie IEReq;

			memset (IEReq.IE, 0, sizeof(IEReq.IE));
			memcpy(&IEReq, param_str, param_len);

			if (IEReq.IEtype == RSN_IEWPA2)
			{
				if (extStaDb_GetRSN_IE(vmacSta_p,&IEReq.macAddr, (UINT8 *)&IEReq.IE) != STATE_SUCCESS)
				{
					rc = -EFAULT;
					break;
				}

				if (IEReq.IE[1] > 0)
				{
					IEReq.IELen = IEReq.IE[1] + 2;
					*ret_len = IEReq.IE[1] + 2 + sizeof(IEReq.macAddr) + 2; /*2 bytes for IE type and IE len */
				}
				else
				{
					IEReq.IELen = 0;
					*ret_len = sizeof(IEReq.macAddr);
				}   
				if (copy_to_user(ret_str, &IEReq, *ret_len))
					rc = -EFAULT;
				return rc;
			}
		}
		break;

	case WL_IOCTL_GET_SCAN_BSSPROFILE:
		{
			scanDescptHdr_t *curDescpt_p = NULL;
			UINT16  parsedLen = 0;
			int i ;

			printk("INSIDE getbssprofile\n");
			printk("Found :%d number of scan respults\n", tmpNumScanDesc[vmacSta_p->VMacEntry.phyHwMacIndx] );
			if (vmacSta_p->busyScanning)
			{ 
				rc = -EFAULT;
				break ;
			}
			for(i=0; i < tmpNumScanDesc[vmacSta_p->VMacEntry.phyHwMacIndx]; i++)
			{
				curDescpt_p = (scanDescptHdr_t *)(&tmpScanResults[vmacSta_p->VMacEntry.phyHwMacIndx][0] + parsedLen);

				if( (smeSetBssProfile( 0, curDescpt_p->bssId, curDescpt_p->CapInfo,
					(((UINT8 *)curDescpt_p) +sizeof(scanDescptHdr_t)),
					curDescpt_p->length + sizeof(curDescpt_p->length) - sizeof(scanDescptHdr_t),
					FALSE)) == MLME_SUCCESS)
				{
					memset(&siteSurveyEntry, 0, sizeof(MRVL_SCAN_ENTRY));
					//					smeCopyBssProfile( 0, &siteSurvey[i] );
					smeCopyBssProfile( 0, &siteSurveyEntry );
					/* Only accept if WPS IE is present */
					if( siteSurveyEntry.result.wps_ie_len > 0 )
					{
						memcpy( &siteSurvey[i], &siteSurveyEntry, sizeof(MRVL_SCAN_ENTRY));
#ifdef MRVL_WPS_DEBUG
						printk("THE BSS PROFILE :[%02X:%02X:%02X:%02X:%02X:%02X]%d\n", 
							siteSurvey[i].result.bssid[0], siteSurvey[i].result.bssid[1],
							siteSurvey[i].result.bssid[2], siteSurvey[i].result.bssid[3],
							siteSurvey[i].result.bssid[4], siteSurvey[i].result.bssid[5],i );
#endif
					}
				}

				parsedLen += curDescpt_p->length + sizeof(curDescpt_p->length);
			}
			*ret_len = sizeof(MRVL_SCAN_ENTRY)*tmpNumScanDesc[vmacSta_p->VMacEntry.phyHwMacIndx] ;
			if (copy_to_user(ret_str, &siteSurvey[0], *ret_len))
				rc = -EFAULT;
			return rc;
		}
		break;
#else /*GENERIC_GETIE*/
	case WL_IOCTL_GET_RSNIE:
		{
			struct wlreq_rsnie rsnIEReq;

			memset (rsnIEReq.rsnIE, 0, sizeof(rsnIEReq.rsnIE));
			memcpy(&rsnIEReq, param_str, param_len);
			if (extStaDb_GetRSN_IE(vmacSta_p,&rsnIEReq.macAddr, (UINT8 *)&rsnIEReq.rsnIE) != STATE_SUCCESS)
			{
				rc = -EFAULT;
				break;
			}

			*ret_len = rsnIEReq.rsnIE[1] + 2 + sizeof(rsnIEReq.macAddr);
			if (copy_to_user(ret_str, &rsnIEReq, *ret_len))
				rc = -EFAULT;
			return rc;
		}
		break;

#ifdef MRVL_WSC //MRVL_WSC_IE
	case WL_IOCTL_GET_WSCIE:
		{
			struct wlreq_wscie wscIEReq;

			memset (wscIEReq.wscIE, 0, sizeof(wscIEReq.wscIE));
			memcpy(&wscIEReq, param_str, param_len);
			if (extStaDb_GetWSC_IE(vmacSta_p,  &wscIEReq.macAddr, (UINT8 *)wscIEReq.wscIE) != STATE_SUCCESS)
			{
				rc = -EFAULT;
				break;
			}

			*ret_len = wscIEReq.wscIE[1] + 2 + sizeof(wscIEReq.macAddr);
			if (copy_to_user(ret_str, &wscIEReq, *ret_len))
				rc = -EFAULT;
			return rc;
		}
		break;
#endif //MRVL_WSC_IE
#endif /* GENERIC_GETIE */


#ifdef WDS_FEATURE
	case WL_IOCTL_GET_WDS_PORT:
		{
			UINT8 index=0;
			UINT8 wdsModeStr[12];
			char *out_buf=buf;
			sprintf(out_buf, "\n");
			out_buf += strlen(out_buf);
			for (index = 0; validWdsIndex(index) ; index++)
			{
				getWdsModeStr(wdsModeStr, priv->vmacSta_p->wdsPort[index].wdsPortMode);

				sprintf(out_buf, "ap0wds%x HWaddr %x:%x:%x:%x:%x:%x  802.%s Port %s \n",
					index,
					priv->vmacSta_p->wdsPort[index].wdsMacAddr[0],
					priv->vmacSta_p->wdsPort[index].wdsMacAddr[1],
					priv->vmacSta_p->wdsPort[index].wdsMacAddr[2],
					priv->vmacSta_p->wdsPort[index].wdsMacAddr[3],
					priv->vmacSta_p->wdsPort[index].wdsMacAddr[4],
					priv->vmacSta_p->wdsPort[index].wdsMacAddr[5],
					wdsModeStr, priv->vmacSta_p->wdsPort[index].active?"Active":"Inactive");
				out_buf += strlen(out_buf); 
			}
		}
		break;
#endif
	case WL_IOCTL_GETCMD :
		{
			param_str[param_len] = '\0';
			*param[0] = '\0';
			*param[1] = '\0';
			sscanf(param_str, "%s %s\n", param[0], param[1]);

#ifdef MRVL_DFS
			if((strcmp(param[0], "get11hNOCList") == 0))
			{
				if( priv->wlpd_p->pdfsApMain )
				{
					DfsPrintNOLChannelDetails( priv->wlpd_p->pdfsApMain, buf, 200 );
				}
				else
				{
					rc = -EFAULT;
					break ;
				}
			}
#endif //MRVL_DFS
#if defined(CLIENT_SUPPORT) && defined (MRVL_WSC)
			if((strcmp(param[0], "getbssprofile") == 0))
			{
				scanDescptHdr_t *curDescpt_p = NULL;
				UINT16  parsedLen = 0;
				int i ;

				printk("INSIDE getbssprofile\n");
				printk("Found :%d number of scan respults\n", tmpNumScanDesc[vmacSta_p->VMacEntry.phyHwMacIndx] );
				if (vmacSta_p->busyScanning)
				{ 
					rc = -EFAULT;
					break ;
				}
				for(i=0; i < tmpNumScanDesc[vmacSta_p->VMacEntry.phyHwMacIndx]; i++)
				{
					curDescpt_p = (scanDescptHdr_t *)(&tmpScanResults[vmacSta_p->VMacEntry.phyHwMacIndx][0] + parsedLen);

					if( (smeSetBssProfile( 0, curDescpt_p->bssId, curDescpt_p->CapInfo,
						(((UINT8 *)curDescpt_p) +sizeof(scanDescptHdr_t)),
						curDescpt_p->length + sizeof(curDescpt_p->length) - sizeof(scanDescptHdr_t),
						FALSE)) == MLME_SUCCESS)
					{
						memset(&siteSurveyEntry, 0, sizeof(MRVL_SCAN_ENTRY));
						//					smeCopyBssProfile( 0, &siteSurvey[i] );
						smeCopyBssProfile( 0, &siteSurveyEntry );
						/* Only accept if WPS IE is present */
						if( siteSurveyEntry.result.wps_ie_len > 0 )
						{
							memcpy( &siteSurvey[i], &siteSurveyEntry, sizeof(MRVL_SCAN_ENTRY));
#ifdef MRVL_WPS_DEBUG
							printk("THE BSS PROFILE :[%02X:%02X:%02X:%02X:%02X:%02X]%d\n", 
								siteSurvey[i].result.bssid[0], siteSurvey[i].result.bssid[1],
								siteSurvey[i].result.bssid[2], siteSurvey[i].result.bssid[3],
								siteSurvey[i].result.bssid[4], siteSurvey[i].result.bssid[5],i );
#endif
						}
					}

					parsedLen += curDescpt_p->length + sizeof(curDescpt_p->length);
				}
				*ret_len = sizeof(MRVL_SCAN_ENTRY)*tmpNumScanDesc[vmacSta_p->VMacEntry.phyHwMacIndx] ;
				if (copy_to_user(ret_str, &siteSurvey[0], *ret_len))
					rc = -EFAULT;
				return rc;
			}
#endif //MRVL_WSC
#ifdef MPRXY
			if (strcmp(param[0], "ipmcgrp") == 0)
			{
				if ( strcmp(param[1], "getallgrps") == 0 )
				{
					tmpStr = buf;
					sprintf(tmpStr,"\n");
					tmpStr += strlen(tmpStr);

					/* check if IP Multicast group entry already exists */
					for(i=0; i<MAX_IP_MCAST_GRPS; i++)
					{
						if (mib->mib_IPMcastGrpTbl[i]->mib_McastIPAddr)
						{
							tempIPAddr = htonl(mib->mib_IPMcastGrpTbl[i]->mib_McastIPAddr);

							for (j=0; j<MAX_UCAST_MAC_IN_GRP; j++)
							{
								sprintf(tmpStr, "%u.%u.%u.%u %02x%02x%02x%02x%02x%02x\n",
									NIPQUAD(tempIPAddr), 
									mib->mib_IPMcastGrpTbl[i]->mib_UCastAddr[j][0],
									mib->mib_IPMcastGrpTbl[i]->mib_UCastAddr[j][1],
									mib->mib_IPMcastGrpTbl[i]->mib_UCastAddr[j][2],
									mib->mib_IPMcastGrpTbl[i]->mib_UCastAddr[j][3],
									mib->mib_IPMcastGrpTbl[i]->mib_UCastAddr[j][4],
									mib->mib_IPMcastGrpTbl[i]->mib_UCastAddr[j][5]);

								tmpStr = buf + strlen(buf);
							}
						}
					}
				}
			}
#endif
		}
		break;

#ifdef CLIENT_SUPPORT
	case WL_IOCTL_GET_STASCAN:
		{
			scanDescptHdr_t *curDescpt_p = NULL;
			IEEEtypes_SsIdElement_t *ssidIE_p;
			IEEEtypes_DsParamSet_t *dsPSetIE_p;
			IEEEtypes_SuppRatesElement_t       *PeerSupportedRates_p = NULL;
			IEEEtypes_ExtSuppRatesElement_t    *PeerExtSupportedRates_p = NULL;
			IEEEtypes_HT_Element_t             *pHT = NULL;
			IEEEtypes_Add_HT_Element_t         *pHTAdd = NULL;
			IEEEtypes_Generic_HT_Element_t     *pHTGen = NULL;
			UINT32 LegacyRateBitMap = 0;
			IEEEtypes_RSN_IE_t *RSN_p = NULL;
			IEEEtypes_RSN_IE_WPA2_t *wpa2IE_p = NULL;
			UINT8 scannedChannel=0;
			UINT16  parsedLen = 0;
			UINT8   scannedSSID[33];
			UINT8   i=0;
			UINT8   mdcnt = 0;
			UINT8 apType[6];
			UINT8 encryptType[10];
			UINT8 cipherType[6];
			BOOLEAN apGonly = FALSE;
			char *out_buf = buf;            


			/* Fill the output buffer */
			sprintf(out_buf, "\n");
			out_buf++;

			for(i=0; i < tmpNumScanDesc[vmacSta_p->VMacEntry.phyHwMacIndx]; i++)
			{
				curDescpt_p = (scanDescptHdr_t *)(&tmpScanResults[vmacSta_p->VMacEntry.phyHwMacIndx][0] + parsedLen);

				memset(&scannedSSID[0], 0, sizeof(scannedSSID));
				memset(&apType[0], 0, sizeof(apType));                
				sprintf(&encryptType[0], "None");
				sprintf(&cipherType[0], " ");
				mdcnt = 0;
				scannedChannel = 0;
				apGonly = FALSE;

				if ((ssidIE_p = (IEEEtypes_SsIdElement_t *)smeParseIeType(SSID,
					(((UINT8 *)curDescpt_p) +sizeof(scanDescptHdr_t)),
					curDescpt_p->length + sizeof(curDescpt_p->length) - sizeof(scanDescptHdr_t))) != NULL)
				{
					memcpy(&scannedSSID[0], &ssidIE_p->SsId[0], ssidIE_p->Len);
				}
				if ((dsPSetIE_p = (IEEEtypes_DsParamSet_t *)smeParseIeType(DS_PARAM_SET,
					(((UINT8 *)curDescpt_p) +sizeof(scanDescptHdr_t)),
					curDescpt_p->length + sizeof(curDescpt_p->length) - sizeof(scanDescptHdr_t))) != NULL)
				{
					scannedChannel = dsPSetIE_p->CurrentChan;
				}

				if (curDescpt_p->CapInfo.Privacy)
					sprintf(&encryptType[0], "WEP");

				PeerSupportedRates_p = (IEEEtypes_SuppRatesElement_t*) smeParseIeType(SUPPORTED_RATES,
					(((UINT8 *)curDescpt_p) +sizeof(scanDescptHdr_t)),
					curDescpt_p->length + sizeof(curDescpt_p->length) - sizeof(scanDescptHdr_t));

				PeerExtSupportedRates_p = (IEEEtypes_ExtSuppRatesElement_t *) smeParseIeType(EXT_SUPPORTED_RATES,
					(((UINT8 *)curDescpt_p) + sizeof(scanDescptHdr_t)),
					curDescpt_p->length + sizeof(curDescpt_p->length) - sizeof(scanDescptHdr_t));

				LegacyRateBitMap = GetAssocRespLegacyRateBitMap(PeerSupportedRates_p, PeerExtSupportedRates_p);

				if (scannedChannel <=14)
				{
					if (PeerSupportedRates_p)
					{
						int j;
						for (j=0; (j < PeerSupportedRates_p->Len) && !apGonly ; j++)
						{
							/* Only look for 6 Mbps as basic rate - consider this to be G only. */
							if (PeerSupportedRates_p->Rates[j] == 0x8c)
							{
								sprintf(&apType[mdcnt++], "G");
								apGonly = TRUE;
							}                            
						}
					}
					if (!apGonly)
					{
						if (LegacyRateBitMap & 0x0f)
							sprintf(&apType[mdcnt++], "B");
						if (PeerSupportedRates_p && PeerExtSupportedRates_p)
							sprintf(&apType[mdcnt++], "G");
					}
				}
				else
				{
					if (LegacyRateBitMap & 0x1fe0)
						sprintf(&apType[mdcnt++], "A"); 
				}

				pHT = (IEEEtypes_HT_Element_t *)smeParseIeType(HT, 
					(((UINT8 *)curDescpt_p) +sizeof(scanDescptHdr_t)),
					curDescpt_p->length + sizeof(curDescpt_p->length) - sizeof(scanDescptHdr_t));

				pHTAdd = (IEEEtypes_Add_HT_Element_t *) smeParseIeType(ADD_HT, 
					(((UINT8 *)curDescpt_p) +sizeof(scanDescptHdr_t)),
					curDescpt_p->length + sizeof(curDescpt_p->length) - sizeof(scanDescptHdr_t));
				// If cannot find HT element then look for High Throughput elements using PROPRIETARY_IE.
				if (pHT == NULL)
				{
					pHTGen = linkMgtParseHTGenIe((((UINT8 *)curDescpt_p) +sizeof(scanDescptHdr_t)),
						curDescpt_p->length + sizeof(curDescpt_p->length) - sizeof(scanDescptHdr_t));
				}
				if ((RSN_p = linkMgtParseWpaIe((((UINT8 *)curDescpt_p) +sizeof(scanDescptHdr_t)),
					curDescpt_p->length + sizeof(curDescpt_p->length) - sizeof(scanDescptHdr_t))))
				{
					sprintf(&encryptType[0], "WPA");

					if (RSN_p->PwsKeyCipherList[3] == RSN_TKIP_ID)
						sprintf(&cipherType[0], "TKIP");
					else if (RSN_p->PwsKeyCipherList[3] == RSN_AES_ID)
						sprintf(&cipherType[0], "AES");
				}

				if ((wpa2IE_p = (IEEEtypes_RSN_IE_WPA2_t *) smeParseIeType(RSN_IEWPA2, 
					(((UINT8 *)curDescpt_p) +sizeof(scanDescptHdr_t)),
					curDescpt_p->length + sizeof(curDescpt_p->length) - sizeof(scanDescptHdr_t))))
				{
					// RSN_AES_ID, RSN_TKIP_ID
					if ((wpa2IE_p->GrpKeyCipher[3] == RSN_TKIP_ID) && 
						(wpa2IE_p->PwsKeyCipherList[3] == RSN_AES_ID))
						sprintf(&encryptType[0], "WPA-WPA2");
					else
						sprintf(&encryptType[0], "WPA2");

					if (wpa2IE_p->PwsKeyCipherList[3] == RSN_TKIP_ID)
						sprintf(&cipherType[0], "TKIP");
					else if (wpa2IE_p->PwsKeyCipherList[3] == RSN_AES_ID)
						sprintf(&cipherType[0], "AES");
				}

				if (pHT || pHTGen)
				{
					sprintf(&apType[mdcnt++], "N");
				}


				parsedLen += curDescpt_p->length + sizeof(curDescpt_p->length);

				sprintf(out_buf, "#%d SSID=%-32s %02x:%02x:%02x:%02x:%02x:%02x %d -%d %s %s %s\n",
					i+1,
					(const char *)&scannedSSID[0],
					curDescpt_p->bssId[0],
					curDescpt_p->bssId[1],
					curDescpt_p->bssId[2],
					curDescpt_p->bssId[3],
					curDescpt_p->bssId[4],
					curDescpt_p->bssId[5],
					scannedChannel, 
					curDescpt_p->rssi, apType, encryptType,cipherType);

				out_buf += strlen(out_buf); 
			}
		}
		break;
#endif /* CLIENT SUPPORT */

	default:
		if (cmd >= SIOCSIWCOMMIT && cmd <= SIOCGIWPOWER)
		{
			rc = -EOPNOTSUPP;
			break;
		}

		WLDBG_INFO(DBG_LEVEL_1,  "unsupported ioctl(0x%04x)\n", cmd);

		rc = -EOPNOTSUPP;

		break;
	}

	if (rc == 0)
	{
		*ret_len = strlen(buf);


		if (copy_to_user(ret_str, buf, *ret_len))
			rc = -EFAULT;
	}
	WLDBG_EXIT(DBG_LEVEL_1);

	return rc;
}

int wlIoctl(struct net_device *dev , struct ifreq  *rq, int cmd)
{
	struct iwreq *wrq = (struct iwreq *)rq;
	int rc = 0;
	char *param = NULL;


	WLDBG_ENTER(DBG_LEVEL_1);


	if (wrq->u.data.length > 0)
	{
		param = kmalloc(wrq->u.data.length, GFP_KERNEL);

		if (param == NULL)
			return -ENOMEM;

		if (copy_from_user(param, wrq->u.data.pointer, wrq->u.data.length))
		{
			kfree(param);
			return -ENOMEM;
		}
	}

	if (IW_IS_SET(cmd))
	{
		rc = wlIoctlSet(dev, cmd, param, wrq->u.data.length, wrq->u.data.pointer, &wrq->u.data.length);
	} else if (IW_IS_GET(cmd))
	{
		rc = wlIoctlGet(dev, cmd, param, wrq->u.data.length,
			wrq->u.data.pointer, &wrq->u.data.length);
	} else
		rc = -EOPNOTSUPP;

	if (param != NULL)
		kfree(param);

	WLDBG_EXIT(DBG_LEVEL_1);

	return rc;
}

/* Helper functions */

int getMacFromString(unsigned char *macAddr, const char *pStr)
{
	int nAddr = 0;
	UINT32 endofstr;

	memset(macAddr, 0, 6);


	if ((endofstr = strlen(pStr)) > 0)
	{
		endofstr += (UINT32)pStr;
		while ((UINT32)pStr < endofstr)
		{
			if (*pStr=='%' && *(pStr+1)=='3' && (*(pStr+2)=='A' || *(pStr+2)=='a'))
			{
				pStr = pStr+3;
				continue;
			}

			if (*pStr>='A' && *pStr<='F')
			{
				macAddr[nAddr] = *pStr - 'A' + 10;
			} else if (*pStr>='a' && *pStr<='f')
			{
				macAddr[nAddr] = *pStr - 'a' + 10;
			} else if (*pStr>='0' && *pStr<='9')
			{
				macAddr[nAddr] = *pStr - '0';
			} else
			{
				return 0;
			}

			pStr++;

			if (*pStr>='A' && *pStr<='F')
			{
				macAddr[nAddr] = (macAddr[nAddr] << 4) | (*pStr - 'A' + 10);
			} else if (*pStr>='a' && *pStr<='f')
			{
				macAddr[nAddr] = (macAddr[nAddr] << 4) | (*pStr - 'a' + 10);
			} else if (*pStr>='0' && *pStr<='9')
			{
				macAddr[nAddr] = (macAddr[nAddr] << 4) | (*pStr - '0');
			} else
			{
				return 0;
			}

			pStr++;
			nAddr++;
		}

		if(nAddr != 6)
		{
			return 0;
		}
	} else
	{
		return 0;
	}

	return 1;
}

#ifdef WPAHEX64	 
#define _U	0x01	/* upper */
#define _L	0x02	/* lower */
#define _D	0x04	/* digit */
#define _C	0x08	/* cntrl */
#define _P	0x10	/* punct */
#define _S	0x20	/* white space (space/lf/tab) */
#define _X	0x40	/* hex digit */
#define _SP	0x80	/* hard space (0x20) */

unsigned char _ctype[] = {
	_C,_C,_C,_C,_C,_C,_C,_C,			/* 0-7 */
	_C,_C|_S,_C|_S,_C|_S,_C|_S,_C|_S,_C,_C,		/* 8-15 */
	_C,_C,_C,_C,_C,_C,_C,_C,			/* 16-23 */
	_C,_C,_C,_C,_C,_C,_C,_C,			/* 24-31 */
	_S|_SP,_P,_P,_P,_P,_P,_P,_P,			/* 32-39 */
	_P,_P,_P,_P,_P,_P,_P,_P,			/* 40-47 */
	_D,_D,_D,_D,_D,_D,_D,_D,			/* 48-55 */
	_D,_D,_P,_P,_P,_P,_P,_P,			/* 56-63 */
	_P,_U|_X,_U|_X,_U|_X,_U|_X,_U|_X,_U|_X,_U,	/* 64-71 */
	_U,_U,_U,_U,_U,_U,_U,_U,			/* 72-79 */
	_U,_U,_U,_U,_U,_U,_U,_U,			/* 80-87 */
	_U,_U,_U,_P,_P,_P,_P,_P,			/* 88-95 */
	_P,_L|_X,_L|_X,_L|_X,_L|_X,_L|_X,_L|_X,_L,	/* 96-103 */
	_L,_L,_L,_L,_L,_L,_L,_L,			/* 104-111 */
	_L,_L,_L,_L,_L,_L,_L,_L,			/* 112-119 */
	_L,_L,_L,_P,_P,_P,_P,_C,			/* 120-127 */
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,		/* 128-143 */
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,		/* 144-159 */
	_S|_SP,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,   /* 160-175 */
	_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,       /* 176-191 */
	_U,_U,_U,_U,_U,_U,_U,_U,_U,_U,_U,_U,_U,_U,_U,_U,       /* 192-207 */
	_U,_U,_U,_U,_U,_U,_U,_P,_U,_U,_U,_U,_U,_U,_U,_L,       /* 208-223 */
	_L,_L,_L,_L,_L,_L,_L,_L,_L,_L,_L,_L,_L,_L,_L,_L,       /* 224-239 */
	_L,_L,_L,_L,_L,_L,_L,_P,_L,_L,_L,_L,_L,_L,_L,_L};      /* 240-255 */

#define tolower(c) __tolower(c)
#define toupper(c) __toupper(c)
#define __ismask(x) (_ctype[(int)(unsigned char)(x)])
#define islower(c)	((__ismask(c)&(_L)) != 0)
#define isupper(c)	((__ismask(c)&(_U)) != 0)

	static inline unsigned char __tolower(unsigned char c)
	{
		if (isupper(c))
			c -= 'A'-'a';
		return c;
	}

	static inline unsigned char __toupper(unsigned char c)
	{
		if (islower(c))
			c -= 'a'-'A';
		return c;
	}

	static void HexStringToHexDigi(char* outHexData, char* inHexString, USHORT Len)
	{
		char    HexString[]="0123456789ABCDEF";
		UCHAR   i, HiNible, LoNible;

		for (i=0; i<Len; ++i){
			HiNible = strchr(HexString, toupper(inHexString[2*i])) - HexString;
			LoNible = strchr(HexString, toupper(inHexString[2*i+1])) - HexString;
			outHexData[i] = (HiNible << 4) + LoNible;
		}           
	}
#endif
	static int IsHexKey( char* keyStr )
	{
		while (*keyStr != '\0')
		{
			if ( (*keyStr>='0' && *keyStr<='9') || ( *keyStr>='A' && *keyStr <= 'F' )
				|| ( *keyStr>='a' && *keyStr <= 'f' )
				)
			{
				keyStr++;
				continue;
			}
			else
				return 0;
		} 
		return 1;
	}

#define isdigit(c)      ((__ismask(c)&(_D)) != 0)
#define isspace(c)      ((__ismask(c)&(_S)) != 0)
#define isascii(c) (((unsigned char)(c))<=0x7f)
#define isxdigit(c)     ((__ismask(c)&(_D|_X)) != 0)

	static int IPAsciiToNum(unsigned int * IPAddr, const char * pIPStr)
	{
		unsigned long val;
		int base, n;
		char c;
		unsigned int parts[4];
		unsigned int *pp = parts;

		c = *pIPStr;

		for (;;) {
			/*
			* Collect number up to ``.''.
			* Values are specified as for C:
			* 0x=hex, 0=octal, isdigit=decimal.
			*/
			if (!isdigit(c))
				return (0);
			val = 0; base = 10;
			if (c == '0') {
				c = *++pIPStr;
				if (c == 'x' || c == 'X')
					base = 16, c = *++pIPStr;
				else
					base = 8;
			}
			for (;;) {
				if (isascii(c) && isdigit(c)) {
					val = (val * base) + (c - '0');
					c = *++pIPStr;
				} else if (base == 16 && isascii(c) && isxdigit(c)) {
					val = (val << 4) |
						(c + 10 - (islower(c) ? 'a' : 'A'));
					c = *++pIPStr;
				} else
					break;
			}
			if (c == '.') {
				/*
				* Internet format:
				*	a.b.c.d
				*	a.b.c	(with c treated as 16 bits)
				*	a.b	(with b treated as 24 bits)
			 */
				if (pp >= parts + 3)
					return (0);
				*pp++ = val;
				c = *++pIPStr;
			} else
				break;
		}
		/*
		* Check for trailing characters.
		*/
		if (c != '\0' && (!isascii(c) || !isspace(c)))
			return (0);
		/*
		* Concoct the address according to
		* the number of parts specified.
		*/
		n = pp - parts + 1;
		switch (n) {

case 0:
	return (0);		/* initial nondigit */

case 1:				/* a -- 32 bits */
	break;

case 2:				/* a.b -- 8.24 bits */
	if (val > 0xffffff)
		return (0);
	val |= parts[0] << 24;
	break;

case 3:				/* a.b.c -- 8.8.16 bits */
	if (val > 0xffff)
		return (0);
	val |= (parts[0] << 24) | (parts[1] << 16);
	break;

case 4:				/* a.b.c.d -- 8.8.8.8 bits */
	if (val > 0xff)
		return (0);
	val |= (parts[0] << 24) | (parts[1] << 16) | (parts[2] << 8);
	break;
		}
		if (IPAddr)
			*IPAddr = val;
		return (1);

	}



	enum {
		BAND24G =0,
		BAND5G =1,
	};

	typedef struct wmm_q_status_s{
		int bkQLen;                //BK queue length
		int beQLen;                //BE queue length
		int viQLen;                //VI queue length
		int voQLen;                //VO queue length
	}wmm_q_status_t; 


	typedef struct wifi_qos_cap_s{
		char wmmSupport;                  // 1 means yes, 0 means no. Status configured by users.
		char sharePhyQueue;               // 1 means yes, 0 means no. It indicates whether multiple logical devices share one physical queue.
		char band;                        // 1 means interface work on 5g band, 0 means on 24g band.            	
		wmm_q_status_t defaultQLen;       // default queue length
	}wifi_qos_cap_t;

	/***************************************************************************************
	* Function: get_wifi_qos_cap 
	*
	* Description:
	*     This function is used to get QoS capability of a Wi-Fi interface.  
	*
	* Parameters:
	*     ifName: (in) interface name .
	*     cap     : (out) QoS capability of input interface.Memory should been allocated by caller.
	* 
	* Return Values: 
	*	NULL   : Interface does not exsit ,or function fails.
	*    Pointer: used as a handler to get queue status.
	*
	***************************************************************************************/
	extern struct wlprivate_data global_private_data[];


	struct net_device *get_wifi_qos_cap(char* ifName, wifi_qos_cap_t* cap)
	{
		UINT8 rootname[sizeof(DRV_NAME)];
		struct net_device *netdev;
		int i;
		struct wlprivate *priv;
		vmacApInfo_t *vmacSta_p;
		MIB_PHY_DSSS_TABLE *PhyDSSSTable;
		for(i=0; i<MAX_CARDS_SUPPORT; i++)
		{
			netdev = global_private_data[i].rootdev;
			memset(rootname, 0, sizeof(DRV_NAME));
			sprintf(rootname,  DRV_NAME, i);
			if(netdev && (!memcmp(rootname, ifName,strlen(rootname))))
			{
				priv  = NETDEV_PRIV_P(struct wlprivate, netdev);
				vmacSta_p = priv->vmacSta_p;
				PhyDSSSTable=vmacSta_p->Mib802dot11->PhyDSSSTable;
				if(vmacSta_p->Mib802dot11->QoSOptImpl)
					cap->wmmSupport = 1;
				else
					cap->wmmSupport = 0;
				cap->sharePhyQueue = 1;
				cap->defaultQLen.bkQLen = MAX_NUM_TX_DESC;
				cap->defaultQLen.beQLen = MAX_NUM_TX_DESC;
				cap->defaultQLen.viQLen = MAX_NUM_TX_DESC;
				cap->defaultQLen.voQLen = MAX_NUM_TX_DESC;
				if(PhyDSSSTable->Chanflag.FreqBand== FREQ_BAND_5GHZ)
					cap->band =BAND5G;
				else
					cap->band = BAND24G;
				return netdev;

			}
		}
		return NULL;
	}


	/***************************************************************************************
	* Function: get_wifi_wmm_queue_status 
	*
	* Description:
	*     This function is to get instantaneous status of WMM queue.  
	*
	* Parameters:
	*       pdev  :(in)  interface handler returned by get_wifi_qos_cap().    
	*       status:(out) instantaneous status.
	*
	* Return Values: 
	*      0:    successful
	*	-1:    failed
	*
	* Note:
	*     Efficiency must be considered. This function will be invoked tons of times per second.
	*
	***************************************************************************************/
	int get_wifi_wmm_queue_status(struct net_device *pdev, wmm_q_status_t *status)
	{
		struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, pdev);
		if(wlpptr && wlpptr->wlpd_p)
		{
			memcpy((char *)status, (char *)&(wlpptr->wlpd_p->fwDescCnt[0]), sizeof(wmm_q_status_t));
			return 0;
		}
		return -1;
	}


#ifdef SOC_W8864
void ratetable_print_SOCW8864(UINT8* pTbl)
{
	dbRateInfo_t *pRateTbl;
	int j, Rate, Nss; 
	
	printk( "%3s %6s %5s %5s %5s %5s %5s %4s %2s %5s %4s %5s %5s\n",
	"Num", 
	"Fmt", 
	"STBC", 
	"BW", 
	"SGI",
	"Nss",
	"RateId", 
	"GF/Pre", 
	"PId", 
	"LDPC",
	"BF",
	"TxAnt",
	"Rate");

	j = 0;
	// Check 1st 32bit DWORD of rate info
	while ((pTbl[0]!=0) || (pTbl[1]!=0) || (pTbl[2]!=0)  || (pTbl[4]!=0))
	{	
		pRateTbl = (dbRateInfo_t *)pTbl;
		
		if (pRateTbl->Format == 2)
		{
			Rate = pRateTbl->RateIDMCS & 0xf;	//11ac, Rate[3:0]
			Nss = pRateTbl->RateIDMCS >> 4;		//11ac, Rate[6:4] = NssCode
			++Nss;		//Add 1 to correct Nss representation
		}
		else
		{	
			Rate = pRateTbl->RateIDMCS;
			Nss = 0;
			if(pRateTbl->Format == 1)
			{
				if((pRateTbl->RateIDMCS >= 0) && (pRateTbl->RateIDMCS < 8))
					Nss = 1;
				else if ((pRateTbl->RateIDMCS >= 8) && (pRateTbl->RateIDMCS < 16))
					Nss = 2;
				else if ((pRateTbl->RateIDMCS >= 16) && (pRateTbl->RateIDMCS < 24))
					Nss = 3;
			}
		}
		printk( "%3d %5d %5d %5d %5d %5d %5d %5d %5d %5d %5d %5d %5d\n",
		(int)j,
		(int)pRateTbl->Format, 
		(int)pRateTbl->Stbc, 
		(int)pRateTbl->Bandwidth, 
		(int)pRateTbl->ShortGI, 
		(int)Nss, 
		(int)Rate, 
		(int)pRateTbl->Preambletype, 
		(int)pRateTbl->PowerId,
		(int)pRateTbl->AdvCoding,
		(int)pRateTbl->BF,
		(int)pRateTbl->AntSelect,
		(int)getPhyRate((dbRateInfo_t *)pRateTbl));
	
		j++;
		pTbl+=(2*sizeof(dbRateInfo_t));	//SOC_W8864 rate parameter is 2 DWORD. Multiply by 2 because dbRateInfo_t is only 1 DWORD
	}
	printk( "\n");

}
#else
void ratetable_print_SOCW8764(UINT8* pTbl)
{
	dbRateInfo_t *pRateTbl;
	int j; 

	printk( "%3s %6s %5s %5s %5s %5s %5s %5s %5s %5s %5s %5s %5s %5s %5s %5s %5s %5s\n",
		"Num", 
		"Fmt", 
		"GI", 
		"BW", 
		"RId", 
		"AdC", 
		"Ant", 
		"SCh", 
		"PT",
		"PId",
		"SAnt2",
		"Rsvr",
		"TxBf",
		"GF",
		"Cnt",
		"Rsvd2",
		"Drop",
		"Rate");


	j = 0;
	// Check 1st 32bit DWORD of rate info
	while ((pTbl[0]!=0) || (pTbl[1]!=0) || (pTbl[2]!=0)  || (pTbl[4]!=0))
	{	
		pRateTbl = (dbRateInfo_t *)pTbl;
		
		printk( "%3d %5d %5d %5d %5d %5d %5d %5d %5d %5d %5d %5d %5d %5d %5d %5d %5d %5d\n",
		(int)j,
		(int)pRateTbl->Format, 
		(int)pRateTbl->ShortGI, 
		(int)pRateTbl->Bandwidth, 
		(int)pRateTbl->RateIDMCS, 
		(int)pRateTbl->AdvCoding, 
		(int)pRateTbl->AntSelect, 
		(int)pRateTbl->ActSubChan, 
		(int)pRateTbl->Preambletype,
		(int)pRateTbl->PowerId,
		(int)pRateTbl->AntSelect2,
        (int)pRateTbl->reserved,
		(int)pRateTbl->TxBfFrame,
		(int)pRateTbl->GreenField,
		(int)pRateTbl->count,
		(int)pRateTbl->rsvd2,
		(int)pRateTbl->drop,
		(int)getPhyRate((dbRateInfo_t *)pRateTbl));
	
		j++;
		pTbl+=sizeof(dbRateInfo_t);	
	}
	printk( "\n");
}
#endif

	
	EXPORT_SYMBOL(get_wifi_wmm_queue_status);
	EXPORT_SYMBOL(get_wifi_qos_cap);
