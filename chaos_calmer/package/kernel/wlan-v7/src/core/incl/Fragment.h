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
* Purpose:
*    This file contains the definitions of the fragment module
*
*****************************************************************************/
#ifndef  __FRAGMENT_H__
#define __FRAGMENT_H__
#include "StaDb.h"
//=============================================================================
//                               INCLUDE FILES
//=============================================================================

//=============================================================================
//                                DEFINITIONS
//=============================================================================

//=============================================================================
//                          PUBLIC TYPE DEFINITIONS
//=============================================================================
//=============================================================================
//                    PUBLIC PROCEDURES (ANSI Prototypes)
//=============================================================================
extern struct sk_buff *DeFragPck(struct net_device *dev,struct sk_buff *skb, extStaDb_StaInfo_t **pStaInfo);



#endif/* __FRAGMENT_H__ */
