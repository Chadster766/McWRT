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

#ifndef WL_DB_H
#define WL_DB_H
#include "wltypes.h"
#include "wl_hal.h"
#include "osif.h"
#include "apintf.h"

#define DBG_LEVEL_0 (1<<0)
#define DBG_LEVEL_1 (1<<1)
#define DBG_LEVEL_2 (1<<2)
#define DBG_LEVEL_3 (1<<3)
#define DBG_LEVEL_4 (1<<4)
#define DBG_LEVEL_5 (1<<5)
#define DBG_LEVEL_6 (1<<6)
#define DBG_LEVEL_7 (1<<7)
#define DBG_LEVEL_8 (1<<8)
#define DBG_LEVEL_9 (1<<9)
#define DBG_LEVEL_10 (1<<10)
#define DBG_LEVEL_11 (1<<11)
#define DBG_LEVEL_12 (1<<12)
#define DBG_LEVEL_13 (1<<13)
#define DBG_LEVEL_14 (1<<14)
#define DBG_LEVEL_15 (1<<15)

#define DBG_CLASS_PANIC (1<<16)
#define DBG_CLASS_ERROR (1<<17)
#define DBG_CLASS_WARNING (1<<18)
#define DBG_CLASS_ENTER (1<<19)
#define DBG_CLASS_EXIT (1<<20)
#define DBG_CLASS_INFO (1<<21)
#define DBG_CLASS_DATA (1<<22)
#define DBG_CLASS_7 (1<<23)
#define DBG_CLASS_8 (1<<24)
#define DBG_CLASS_9 (1<<25)
#define DBG_CLASS_10 (1<<26)
#define DBG_CLASS_11 (1<<27)
#define DBG_CLASS_12 (1<<28)
#define DBG_CLASS_13 (1<<29)
#define DBG_CLASS_14 (1<<30)
#define DBG_CLASS_15 (1<<31)

extern void wlPrintData(UINT32 classlevel, const char *func, const void *data, int len, const char *format, ... );
extern void wlPrint(UINT32 classlevel, const char *func,const char *format, ... );
extern const char* mac_display(const UINT8 *mac);

#ifdef WL_DEBUG
#define WLDBG_DUMP_DATA(classlevel, data, len)                                \
    wlPrintData(classlevel|DBG_CLASS_DATA,  __FUNCTION__,                           \
                 	data, len, NULL)

#define WLDBG_ENTER(classlevel)                                          \
    wlPrint(classlevel|DBG_CLASS_ENTER,  __FUNCTION__,   NULL)

#define WLDBG_ENTER_INFO(classlevel, ... )                               \
    wlPrint(classlevel|DBG_CLASS_ENTER,  __FUNCTION__,   __VA_ARGS__)

#define WLDBG_EXIT(classlevel)                                           \
    wlPrint(classlevel|DBG_CLASS_EXIT,  __FUNCTION__,   NULL)

#define WLDBG_EXIT_INFO(classlevel, ... )                                \
    wlPrint(classlevel|DBG_CLASS_EXIT,  __FUNCTION__,   __VA_ARGS__)

#define WLDBG_INFO(classlevel, ... )                                     \
    wlPrint(classlevel|DBG_CLASS_INFO,  __FUNCTION__,   __VA_ARGS__)

#define WLDBG_WARNING(classlevel, ... )                                     \
    wlPrint(classlevel|DBG_CLASS_WARNING,  __FUNCTION__,   __VA_ARGS__)

#define WLDBG_ERROR(classlevel, ... )                                    \
    wlPrint(classlevel|DBG_CLASS_ERROR,  __FUNCTION__,   __VA_ARGS__)

#define WLDBG_PANIC(classlevel, ... )                                \
    wlPrint(classlevel|DBG_CLASS_PANIC,  __FUNCTION__,   __VA_ARGS__)
#else

#define WLDBG_DUMP_DATA(classlevel, data, len)
#define WLDBG_ENTER(classlevel)
#define WLDBG_ENTER_INFO(classlevel, ... ) 
#define WLDBG_EXIT(classlevel)
#define WLDBG_EXIT_INFO(classlevel, ... ) 
#define WLDBG_INFO(classlevel, ... )
#define WLDBG_WARNING(classlevel, ... )
#define WLDBG_ERROR(classlevel, ... )
#define WLDBG_PANIC(classlevel, ... )

#endif /* WL_DB_H */

#define WLDBG_INC_DFS_DROP_CNT(i)
#define WLDBG_INC_IFF_DROP_CNT(i)
#define WLDBG_INC_TXQ_DROP_CNT(i)
#define WLDBG_INC_TX_ERROR_CNT(i)
#define WLDBG_INC_TX_OK_CNT(x, i)
#define WLDBG_PRINT_QUEUE_STATS_COUNTERS
#define WLDBG_INC_RX_RECV_POLL_CNT_STA(x)
#define WLDBG_INC_RX_80211_INPUT_CNT_STA(x)
#define WLDBG_INC_RX_FWD_CNT_STA(x)
#define WLDBG_REC_RX_80211_INPUT_PKTS(x)
#define WLDBG_REC_RX_FWD_PKTS(x)
#define WLDBG_REC_TX_Q_DEPTH(x, i)

#define WLDBG_SET_PKT_TIMESTAMP(x)
#define WLDBG_SET_FW_PKT_TIMESTAMP(x, y)
#define WLDBG_REC_PKT_DELTA_TIME(x,tm,y,i)
#define WLDBG_RX_REC_PKT_FWToDRV_TIME(x,curr_tm)
#define WLDBG_RX_REC_PKT_DRV_TIME(x,curr_tm)
#define WLDBG_RX_REC_PKT_TOTAL_TIME(x,curr_tm)
#define WLDBG_PRINT_QUEUE_STATS_LATENCY
#define WLDBG_PRINT_QUEUE_STATS_RX_LATENCY

UINT8 DebugCmdParse(WL_PRIV *wlpptr, UINT8 *str);
inline void timinglog(unsigned int len, int id);
void start_timing_log(WL_PRIV *, unsigned char *s, int id);
extern UINT32 debugmap;
void wlDumpData(const void *data, int len, char *marker);

#define DBGPRINTK printk
#endif /* WL_DB_H */
