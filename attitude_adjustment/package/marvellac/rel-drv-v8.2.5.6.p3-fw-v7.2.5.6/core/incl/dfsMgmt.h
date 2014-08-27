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

/******************** (c) Marvell Semiconductor, Inc., 2001 *******************
*
*
* Purpose:
*    This file contains the function prototypes and definitions for the
*    DFS Management Service Module.
*
* Notes:
*    None.
*
*****************************************************************************/

#ifndef _DFSMGMT_H_
#define _DFSMGMT_H_

/* DFS_SUPPORT */
#include "wltypes.h"
#include "IEEE_types.h"
#include "dfs.h"

typedef enum _DfsCmd_e
{
	DFS_CMD_CHANNEL_CHANGE,
	DFS_CMD_RADAR_DETECTION,
	DFS_CMD_WL_RESET
}DfsCmdType_e;
typedef UINT8 DfsCmdType_t;

typedef struct _DfsCmd_t
{
	DfsCmdType_t  CmdType;
	union
	{
		DfsChanInfo       chInfo;
		UINT8   			radarCmd;
	} Body;
} PACK_END DfsCmd_t;

#endif //_DFSMGMT_H_
