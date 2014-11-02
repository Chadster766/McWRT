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


/*!
 * \file  idList.h
 * \brief aid & station id assignment to clients
 */

#if !defined(_IDLIST_H_)
#define _IDLIST_H_

void InitAidList(void);
UINT32 AssignAid(void);
void FreeAid(UINT32 Aid);

Status_e ResetAid(UINT16 StnId, UINT16 Aid);
WL_STATUS InitStnIdList(int max_stns);
UINT32 AssignStnId(void);
void FreeStnId(UINT32 StnId);

#endif /* _IDLIST_H_ */
