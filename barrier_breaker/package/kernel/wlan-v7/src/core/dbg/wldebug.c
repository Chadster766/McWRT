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
#include "wldebug.h"
#include "ap8xLnxIntf.h"
#include <stdarg.h>

#include "ap8xLnxFwcmd.h"
#ifdef AP_MAC_LINUX
#define myprint printk(  
#define myfree   kfree
#define mymalloc(x) kmalloc(x,GFP_KERNEL)
#define STRING_TERMINATOR 0
#else
#define myprint (
#define STRING_TERMINATOR 0x0d
#endif
/* default settings */

/** external functions **/

/** external data **/
UINT32 debugmap = 0;

/** internal functions **/
/** public data **/
#ifdef QUEUE_STATS
wldbgPktStats_t wldbgTxACxPktStats[4];
#ifdef QUEUE_STATS_CNT_HIST
wldbgStaTxPktStats_t txPktStats_sta[QS_NUM_STA_SUPPORTED];
wldbgStaRxPktStats_t rxPktStats_sta[QS_NUM_STA_SUPPORTED];
#endif
UINT32 dbgUdpSrcVal = 64;
u_int32_t dbgUdpSrcVal1 = 64;
UINT8 qs_rxMacAddrSave[24];
int numOfRxSta=0;
#endif

/** private data **/
#if 1
UINT32 WLDBG_LEVELS = DBG_LEVEL_0|DBG_LEVEL_1|DBG_LEVEL_2|DBG_LEVEL_3|DBG_LEVEL_4|DBG_LEVEL_5;
#else
#define WLDBG_LEVELS (\
	DBG_LEVEL_0   | \
	DBG_LEVEL_1  | \
	DBG_LEVEL_2| \
	DBG_LEVEL_3   | \
	DBG_LEVEL_4   | \
	DBG_LEVEL_5 )
#endif

#define WLDBG_CLASSES  ( \
	DBG_CLASS_PANIC   | \
	DBG_CLASS_ERROR   | \
	DBG_CLASS_WARNING| \
	DBG_CLASS_INFO| \
	DBG_CLASS_DATA| \
	DBG_CLASS_ENTER   | \
	DBG_CLASS_EXIT   | \
	DBG_CLASS_10)

/** public functions **/
int DebugBitSet(UINT8 bit)
{
	return (debugmap & (1<<bit));
}
void DebugBitsClear(void)
{
	debugmap =0;
}
const char* mac_display(const UINT8 *mac)
{
	static char etherbuf[18];
	snprintf(etherbuf, sizeof(etherbuf), "%02x:%02x:%02x:%02x:%02x:%02x",
		mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
	return etherbuf;
}

static void ToLowerCase(UINT8 *pStr)
{
	UINT16 i;
	UINT16 lenStr = strlen(pStr);

	for ( i=0; i < lenStr; i++ )
	{
		if ( *(pStr + i) > 0x40 && *(pStr + i) < 0x5b )
		{
			*(pStr + i) = *(pStr + i) + 0x20;

		}
	}

}
/*Utility function to convert ascii to a right number.
* Both decimal and hex (beginning with 0x) can be handled.
*/
extern long atohex2(const char *number);
/*{
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
}*/
UINT32 (*Func0p)(void);
UINT32 (*Func1p)(UINT32);
UINT32 (*Func2p)(UINT32, UINT32);
UINT32 (*Func3p)(UINT32, UINT32, UINT32);
UINT32 (*Func4p)(UINT32, UINT32, UINT32, UINT32);

#define MAX_AGRC    7
#define MAX_STRING 128
typedef struct strArray_t
{
	UINT8 strArray[MAX_AGRC][MAX_STRING];
}strArray_t;

int StringToArray(UINT8 *string_p, strArray_t *strArray)
{
	UINT16 argc = 0;
	UINT8 *s, *s1;
	UINT8 spacefound = 0;
	UINT16   length = MAX_AGRC*MAX_STRING;
	s = s1 = string_p;
	memset(strArray, 0, sizeof(strArray_t));
	length = strlen(string_p)+1;
	while ( length)
	{
		if ( *s == 0x20 || *s == STRING_TERMINATOR)
		{
			if ( *s == STRING_TERMINATOR )
			{
				*s = 0;
				strcpy(strArray->strArray[argc++], s1); 
				return (argc);
			}
			*s = 0;
			spacefound = 1;
			strcpy(strArray->strArray[argc++], s1); 
		}
		s++;
		length--;
		if ( spacefound )
		{
			while ( *s == 0x20 )
			{
				length--;
				s++;
			}
			s1 = s;
		}
		spacefound = 0;
	}
	return (argc);
}

extern u_int32_t debug_tcpack;
extern UINT32 vht_cap;
extern UINT32 SupportedRxVhtMcsSet;
extern UINT32 SupportedTxVhtMcsSet;
extern UINT32 ch_width;
extern UINT32 center_freq0;
extern UINT32 center_freq1;
extern UINT32 basic_vht_mcs;
#ifdef CAP_MAX_RATE
extern u_int32_t MCSCapEnable;
extern u_int32_t MCSCap;
#endif

UINT8 DebugCmdParse(struct net_device *netdev, UINT8 *str)
{
	UINT16 argc;
	strArray_t *strArray_p;
	strArray_p = mymalloc(sizeof(strArray_t));
	if (strArray_p == NULL) return 1;
	ToLowerCase(str);
	argc = StringToArray(str, strArray_p);
		if ( !memcmp(strArray_p->strArray[0], "tcpack", 6) )
		{
			debug_tcpack = atohex2(strArray_p->strArray[1]);
			myprint "debug_tcpack %s\n", debug_tcpack?"enabled":"disabled");
		} 
#ifdef CAP_MAX_RATE
		else if ( !memcmp(strArray_p->strArray[0], "mcscap", 6) )
		{
			MCSCapEnable = atohex2(strArray_p->strArray[1]);
			
			myprint "MCS cap %s. To enable, mcscap 1 <mcs_value>\n", MCSCapEnable?"enabled":"disabled");
			if (MCSCapEnable){
				if(atohex2(strArray_p->strArray[2]) > 23)
				{
					myprint "Pls specify MCS <= 23\n");
					MCSCapEnable=0;
					myprint "MCS cap disabled\n");
				}
				else 
				{
					MCSCap = atohex2(strArray_p->strArray[2]);
					myprint "Rate capped at MCS%d\n", MCSCap);
				}
			}
		}
#endif
		else if ( !memcmp(strArray_p->strArray[0], "vhtcap", 6) )
		{
			vht_cap = atohex2(strArray_p->strArray[1]);
			SupportedRxVhtMcsSet = atohex2(strArray_p->strArray[2]);
			SupportedTxVhtMcsSet = atohex2(strArray_p->strArray[3]);
			myprint "vht_cap=%x  SupportedRxVhtMcsSet=%x  SupportedTxVhtMcsSet=%x\n",
			    (unsigned int)vht_cap, (unsigned int)SupportedRxVhtMcsSet, (unsigned int)SupportedTxVhtMcsSet);
		} 
		else if ( !memcmp(strArray_p->strArray[0], "vhtopt", 6) )
		{
		    if(argc > 4)
		    {
                basic_vht_mcs = atohex2(strArray_p->strArray[4]);
		    }
		    if (argc > 3)
		    {
                center_freq1 = atohex2(strArray_p->strArray[3]);
		    }
		    if (argc > 2)
		    {
                center_freq0 = atohex2(strArray_p->strArray[2]);
		    }
		    if (argc > 1)
		    {
                ch_width = atohex2(strArray_p->strArray[1]);
		    }
			myprint "ch_width=%d  center_freq0=%d  center_freq1=%d  basic_vht_mcs=%x\n", 
			    (int)ch_width, (int)center_freq0, (int)center_freq1, (unsigned int)basic_vht_mcs);
		} 
		else if ( !memcmp(strArray_p->strArray[0], "read", 4) )
		{
			UINT32 location;
			location = atohex2(strArray_p->strArray[1]);
			myprint "location %x = %x\n", (int)location, (int)(*(volatile unsigned long *)(location)));
		} else if ( !memcmp(strArray_p->strArray[0], "write", 8) )
		{
			UINT32 location, val;
			location = atohex2(strArray_p->strArray[1]);
			val = atohex2(strArray_p->strArray[2]);
			(*(volatile unsigned long *)(location)) = val;
			myprint "write %x to location %x\n", (int)val, (int)location);
		} else if ( !memcmp(strArray_p->strArray[0], "dump", 4) )
		{
			struct wlprivate *priv = NETDEV_PRIV_P(struct wlprivate, netdev);
			{
				unsigned long i, val, offset, length;

				if ( !memcmp(strArray_p->strArray[1], "mm", 2))
				{ 
					offset = atohex2(strArray_p->strArray[2]);
					if (offset>0xffff)
					{
						goto out; 
					}

					length = atohex2(strArray_p->strArray[3]);
					if(!length)
						length = 32;

					printk( "dump mem\n");
					for(i = 0; i < length; i+=4)
					{
						volatile unsigned int val = 0;

						val = *(volatile unsigned int *)((unsigned int)priv->ioBase1 + offset + i);

						if(i %8 ==0)
						{
							printk( "\n%08x: ",(int)(0x80000000 + offset + i));
						}
						printk( "  %08x", val);
					}
				} 
				else if (!memcmp(strArray_p->strArray[1], "rf", 2))
				{
					offset = atohex2(strArray_p->strArray[2]);
					length = atohex2(strArray_p->strArray[3]);
					if(!length)
						length = 32;

					printk(  "dump rf regs\n");
					for(i = 0; i < length; i++)
					{
						wlRegRF(netdev, 0,  offset+i, &val);
						if(i %8 ==0)
						{
							printk(  "\n%02x: ",(int)(offset+i));
						}
						printk(  "  %02x", (int)val);
					}
				}
				else if (!memcmp(strArray_p->strArray[1], "bb", 2))
				{
					offset = atohex2(strArray_p->strArray[2]);
					length = atohex2(strArray_p->strArray[3]);
					if(!length)
						length = 32;

					printk(  "dump bb regs\n");
					for(i = 0; i < length; i++)
					{
						wlRegBB(netdev, 0,  offset+i, &val);
						if(i %8 ==0)
						{
							printk( "\n%02x: ",(int)(offset+i));
						}
						printk( "  %02x", (int)val);
					}
				}
			}
		} else if ( !memcmp(strArray_p->strArray[0], "map", 3) )
		{
#if 1
			extern void wlTxDescriptorDump(struct net_device *netdev);
			wlTxDescriptorDump(netdev);
#else
			UINT8 mac[6];
			int param1, param2, set=0;
			MacAddrString(strArray_p->strArray[1], mac);
			set = atohex2(strArray_p->strArray[2]);
			if(set)
			{
				param1 = atohex2(strArray_p->strArray[3]);
				param2 = atohex2(strArray_p->strArray[4]);
			}
#endif
		}else if ( !memcmp(strArray_p->strArray[0], "help", 4) )
		{
			myprint "read <location>\nwrite <location> <value>\ndump <start location> <length>\nfunc <arg#> <param ...>\n");
		} else
		{
			myprint "No Valid Commands found\n");
		}
out:
		myfree(strArray_p);
		return (0);
}

void wlPrint(UINT32 classlevel,const char *func, const char *format, ... )
{
	unsigned char debugString[1020] = "";	//Reduced from 1024 to 1020 to prevent frame size > 1024bytes warning during compilation
	UINT32 level = classlevel & 0x0000ffff;
	UINT32 class = classlevel & 0xffff0000;
	va_list a_start;


	if ((class & WLDBG_CLASSES) != class)
	{
		return;
	}

	if ((level & WLDBG_LEVELS) != level)
	{
		if(class != DBG_CLASS_PANIC && class != DBG_CLASS_ERROR)
			return; 
	}

	if (format != NULL)
	{
		va_start(a_start, format);
		vsprintf(debugString, format, a_start);
		va_end(a_start);
	}

	switch (class)
	{
	case DBG_CLASS_ENTER:
		myprint  "Enter %s() ...\n", func);
		break;
	case DBG_CLASS_EXIT:
		myprint  "... Exit %s()\n", func);
		break;
	case DBG_CLASS_WARNING:
		myprint  "WARNING:");
		break;
	case DBG_CLASS_ERROR:
		myprint  "ERROR:");
		break;
	case DBG_CLASS_PANIC:
		myprint  "PANIC:");
		break;
	default:
		break;
	}
	if (strlen(debugString) > 0)
	{
		if (debugString[strlen(debugString)-1] == '\n')
			debugString[strlen(debugString)-1] = '\0';
		myprint  "%s(): %s\n",func, debugString);
	}
}

void wlPrintData(UINT32 classlevel, const char *func,const void *data, int len, const char *format, ... )
{
	unsigned char debugString[992] = "";	//Reduced from 1024 to 992 to prevent frame size > 1024bytes warning during compilation
	unsigned char debugData[16] = "";
	unsigned char *memptr = (unsigned char *) data;
	UINT32 level = classlevel & 0x0000ffff;
	UINT32 class = classlevel & 0xffff0000;
	int currByte = 0;
	int numBytes = 0;
	int offset = 0;
	va_list a_start;


	if ((class & WLDBG_CLASSES) != class)
	{
		return; 
	}

	if ((level & WLDBG_LEVELS) != level)
	{
		return; 
	}

	if (format != NULL)
	{
		va_start(a_start, format);
		vsprintf(debugString, format, a_start);
		va_end(a_start);
	}

	if (strlen(debugString) > 0)
	{
		if (debugString[strlen(debugString)-1] == '\n')
			debugString[strlen(debugString)-1] = '\0';
		myprint  "%s() %s\n", func, debugString);
	}
	for (currByte = 0; currByte < len; currByte=currByte+8)
	{
		if ((currByte + 8) < len)
		{
			myprint  "%s() 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n", 
				func,
				* (memptr + currByte + 0),
				* (memptr + currByte + 1),
				* (memptr + currByte + 2),
				* (memptr + currByte + 3),
				* (memptr + currByte + 4),
				* (memptr + currByte + 5),
				* (memptr + currByte + 6),
				* (memptr + currByte + 7));
		}
		else
		{
			numBytes = len - currByte;
			offset = currByte;
			sprintf(debugString, "%s() ", func);
			for (currByte = 0; currByte < numBytes; currByte++ )
			{
				sprintf(debugData, "0x%02x ", * (memptr + offset + currByte));
				strcat(debugString, debugData);
			}
			myprint  "%s\n", debugString);
			break; 
		}
	}
}

#ifdef QUEUE_STATS
#ifdef QUEUE_STATS_LATENCY
static int initCnt[4]={0,0,0,0};
UINT8 rx_initCnt[3]={0,0,0};
rx_stats_q_stats_t rx_QueueStats;

void wldbgRecPktTime(u_int32_t pkt_tm, u_int32_t cur_tm, int ac_id)
{
    u_int32_t delta;

    if((pkt_tm > 0) && (cur_tm > pkt_tm))
    {
        delta = cur_tm - pkt_tm;
        if(wldbgTxACxPktStats[ac_id].TxPktLatency_Max == 0)
        {
            wldbgTxACxPktStats[ac_id].TxPktLatency_Min = delta;
            wldbgTxACxPktStats[ac_id].TxPktLatency_Max = delta;
            wldbgTxACxPktStats[ac_id].TxPktLatency_Mean = (delta>>3);
        }
        else
        {
            if( delta > wldbgTxACxPktStats[ac_id].TxPktLatency_Max)
            {
                wldbgTxACxPktStats[ac_id].TxPktLatency_Max = delta;
            }
            else if(delta < wldbgTxACxPktStats[ac_id].TxPktLatency_Min)
            {
                wldbgTxACxPktStats[ac_id].TxPktLatency_Min = delta;
            }
            if(initCnt[ac_id] < 7)
            {
                wldbgTxACxPktStats[ac_id].TxPktLatency_Mean += (delta>>3);
                initCnt[ac_id]++;
            }
            else
            {
                wldbgTxACxPktStats[ac_id].TxPktLatency_Mean = ((wldbgTxACxPktStats[ac_id].TxPktLatency_Mean*5+delta*3)>>3);
            }
        }
    }
}

/******************************************************************************
*
* Name: wldbgRecRxMinMaxMean
*
* Description:
*    basic routine to classify the value x to be min or max the basic_stats_t 
*    variable, then calculate the mean.
*
* Conditions For Use:
*    The stats module has been initialized.
*
* Arguments:
*    Arg1 (i  ): x - the value
*                pStats  - pointer to a basic_stats_t variable
*                n    - pointer to control counter
* Return Value:
*    None.
*
* Notes:
*    None.
*
*****************************************************************************/
void wldbgRecRxMinMaxMean(UINT32 x,  basic_stats_t* pStats, UINT8 *n)
{
    if(pStats->Max== 0)
    {
        pStats->Min= x;
        pStats->Max = x;
        pStats->Mean = (x>>3);
        (*n) = 1;
        
    }
    else
    {
        if( x > pStats->Max)
        {
            pStats->Max = x;
        }
        else if(x < pStats->Min)
        {
            pStats->Min = x;
        }
        if((*n) < 8)
        {
            pStats->Mean += (x>>3);
            (*n) += 1;
        }
        else
        {
            pStats->Mean = ((pStats->Mean*5+x*3)>>3);
        }
    }
}
#endif

void wldbgPrintPktStats(int option)
{
    int i;
    char *ac[4] = {"BK","BE","VI","VO"};

#ifdef QUEUE_STATS_LATENCY
    if(option == 1)
    {
        printk("\nDRV Packet Latency (microsecond)\n");
        printk("ACQ\t DRV_Min\t   DRV_Max\t   DRV_Mean\n");
        for(i=0; i<4; i++)
        {
            if(wldbgTxACxPktStats[i].TxPktLatency_Max)
            {
                printk("%s    %10u\t%10u\t%10u\n",ac[i], 
                    wldbgTxACxPktStats[i].TxPktLatency_Min,
                    wldbgTxACxPktStats[i].TxPktLatency_Max,
                    wldbgTxACxPktStats[i].TxPktLatency_Mean
                    );
			}
        }
    }
	else if (option == 2)
	{
		printk("\nRX: Fw-To-Drv DMA Packet Latency (microsecond)\n");
        printk("FWtoDRV_Min\t   FWtoDRV_Max\t   FWtoDRV_Mean\n");
        printk("%10u\t%10u\t%10u\n", 
                rx_QueueStats.Latency.FwToDrvLatency.Min,
                rx_QueueStats.Latency.FwToDrvLatency.Max,
                rx_QueueStats.Latency.FwToDrvLatency.Mean
                );
		printk("\nRX: Drv Packet Latency (microsecond)\n");
        printk("DRV_Min\t   DRV_Max\t   DRV_Mean\n");
        printk("%10u\t%10u\t%10u\n", 
                rx_QueueStats.Latency.DrvLatency.Min,
                rx_QueueStats.Latency.DrvLatency.Max,
                rx_QueueStats.Latency.DrvLatency.Mean
                );
		printk("\nRX: Total Packet Latency (microsecond)\n");
        printk("Total_Min\t   Total_Max\t   Total_Mean\n");
        printk("%10u\t%10u\t%10u\n", 
                rx_QueueStats.Latency.TotalLatency.Min,
                rx_QueueStats.Latency.TotalLatency.Max,
                rx_QueueStats.Latency.TotalLatency.Mean
                );	
	}
#endif        
#ifdef QUEUE_STATS_CNT_HIST
    if(option == 0)
    {
        printk("---------------------\n");    
        printk("DRV Packet Statistics\n");
        printk("---------------------");
        printk("\nTx Packet Counters\n");
        printk("AC\t   TxOk\t\t    DfsDrop\t  IffDrop\t TxQDrop\t ErrorCnt\tMaxQueueDepth\n");
        for(i=0; i<4; i++)
        {
            if(wldbgTxACxPktStats[i].TxOkCnt)
            {
                printk("%s    %10u\t%10u\t%10u\t%10u\t%10u\t%10u\n",ac[i],
                    wldbgTxACxPktStats[i].TxOkCnt,
                    wldbgTxACxPktStats[i].TxDfsDropCnt,
                    wldbgTxACxPktStats[i].TxIffDropCnt,
                    wldbgTxACxPktStats[i].TxTxqDropCnt,
                    wldbgTxACxPktStats[i].TxErrorCnt,
                    wldbgTxACxPktStats[i].txQdepth
                    );
            }
        }
        printk("\nTCP/UDP Counters\n");
        printk("AC \t   TCP\t\t     UDP\t\t ICMP\t\t UDP_SRC_PORT=%d\n", dbgUdpSrcVal1);
        for(i=0; i<4; i++)
        {
            if(wldbgTxACxPktStats[i].TxOkCnt)
            {
                printk("%s    %10u\t%10u\t    %10u\t\t%10u\n",ac[i],
                    wldbgTxACxPktStats[i].TCPCnt,
                    wldbgTxACxPktStats[i].UDPCnt,
                    wldbgTxACxPktStats[i].ICMPCnt,
                    wldbgTxACxPktStats[i].SMARTBITS_UDPCnt
                    );
            }
        }
        
        printk("\nDrv Per STA Counters\n");
        printk("MAC address\t\tTx_Pkt_In\tTx_Pkt_Out\t   TxQDrop\n");
        for(i=0; i<4; i++)
        {
            if(txPktStats_sta[i].valid)
            {
                printk( "%02x:%02x:%02x:%02x:%02x:%02x\t", 
                    txPktStats_sta[i].addr[0], 
                    txPktStats_sta[i].addr[1], 
                    txPktStats_sta[i].addr[2], 
                    txPktStats_sta[i].addr[3], 
                    txPktStats_sta[i].addr[4], 
                    txPktStats_sta[i].addr[5]);
                printk("%10u\t%10u\t%10u\n", txPktStats_sta[i].TxEnQCnt,txPktStats_sta[i].TxOkCnt,txPktStats_sta[i].TxqDropCnt);
            }
        }
    }
#endif    
}

#ifdef QUEUE_STATS_CNT_HIST
void wldbgRecPerStatxPktStats(UINT8* addr, UINT8 type)
{
    int l;
    for(l=0; l<4; l++)
    {
        if(txPktStats_sta[l].valid)
        {
            if(*(UINT32 *)(&addr[2]) == *(UINT32 *)(&txPktStats_sta[l].addr[2]))
            {
                if(type == QS_TYPE_TX_EN_Q_CNT)
                {
                    txPktStats_sta[l].TxEnQCnt++;
                }
                else if(type == QS_TYPE_TX_OK_CNT_CNT)
                {
                    txPktStats_sta[l].TxOkCnt++;
                }
                else if(type == QS_TYPE_TX_Q_DROPE_CNT)
                {
                    txPktStats_sta[l].TxqDropCnt++;
                }
                break;
            }
        }
    }
}
#endif

void wldbgResetQueueStats(void)
{
#ifdef QUEUE_STATS_CNT_HIST
    int i;
    
    for(i=0; i<4; i++)
    {
        txPktStats_sta[i].TxOkCnt = 0;
        txPktStats_sta[i].TxEnQCnt = 0;
        rxPktStats_sta[i].Rx80211InputCnt = 0;
        rxPktStats_sta[i].RxfwdCnt = 0;
        rxPktStats_sta[i].RxRecvPollCnt = 0;
    }
#endif    
    memset(wldbgTxACxPktStats , 0x0, (sizeof(wldbgPktStats_t)*4)); 
    dbgUdpSrcVal1=dbgUdpSrcVal;
#ifdef QUEUE_STATS_LATENCY
    memset(initCnt, 0x0, (sizeof(int)*4));
	memset(rx_initCnt, 0x0,(sizeof(int)*3));
	memset(&rx_QueueStats , 0x0, (sizeof(rx_stats_q_stats_t)));

#endif    
}

#endif
/** private functions **/
