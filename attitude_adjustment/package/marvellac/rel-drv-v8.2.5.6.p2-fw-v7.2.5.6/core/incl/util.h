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

