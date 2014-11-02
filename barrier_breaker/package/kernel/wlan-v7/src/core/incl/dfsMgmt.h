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

#ifdef MRVL_DFS
//=============================================================================
//                               INCLUDE FILES
//=============================================================================
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
#endif //MRVL_DFS

#endif //_DFSMGMT_H_
