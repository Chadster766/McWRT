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


/*
*
* Purpose:
*    This file proivdes general purpose utilities.
*
* Public Procedures:
*    util_CopyList     Copies a list that is terminated by a zero entry
*    util_ListLen      Returns the length of a list that is terminated by a
*                      zero entry
*
*/
#ifndef _UTIL_H_
#define _UTIL_H_

//=============================================================================
//                               INCLUDE FILES
//=============================================================================

//=============================================================================
//                    PUBLIC PROCEDURES (ANSI Prototypes)
//=============================================================================

/******************************************************************************
 *
 * Name: util_CopyList
 *
 * Description:
 *    Routine to copy a list of bytes that is represented by an array where
 *    an entry of 0 indicates the end of the list if the list does not
 *    contain the maximum number of entries.
 *
 * Conditions For Use:
 *    None.
 *
 * Arguments:
 *    Arg1 (  o): Dest_p  - Pointer to the location a list is to be copied to
 *    Arg2 (i  ): Src_p   - Pointer to the location of the list to be copied
 *    Arg3 (i  ): MaxSize - The maximum allowed length of the list
 *
 * Return Value:
 *    The length of the list.
 *
 * Notes:
 *    The length is computed by assuming that as soon as there is a zero
 *    in the array, that indicates the end of the data in the array.
 *
 *****************************************************************************/

extern UINT8 util_CopyList(UINT8 *Dest_p, UINT8 *Src_p, UINT32 MaxSize);

/******************************************************************************
 *
 * Name: util_ListLen
 *
 * Description:
 *    Routine to determine the length of a list of bytes that is represented
 *    by an array where an entry of 0 indicates the end of the list if the
 *    list does not contain the maximum number of entries.
 *
 * Conditions For Use:
 *    None.
 *
 * Arguments:
 *    Arg1 (i  ): List_p  - Pointer to the list of interest
 *    Arg2 (i  ): MaxSize - The maximum allowed length of the list
 *
 * Return Value:
 *    The length of the list.
 *
 * Notes:
 *    The length is computed by assuming that as soon as there is a zero
 *    in the array, that indicates the end of the data in the array.
 *
 *****************************************************************************/

extern UINT8 util_ListLen(UINT8 *List_p, UINT32 MaxSize);

#endif

