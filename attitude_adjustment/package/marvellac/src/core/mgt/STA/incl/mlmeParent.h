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

/*******************************************************************************************
*
* File: mlmeParent.h
*        MLME Parent Module Header
* Description:  Implementation of the MLME Parent Module Services
*
*******************************************************************************************/

#ifndef MAC_MLME_PARENT
#define MAC_MLME_PARENT
#include "wlvmac.h"

extern vmacId_t parentGetVMacId( UINT8 macIndex );
extern vmacEntry_t * mlmeStaInit_Parent(UINT8   macIndex, 
                                    UINT8 * macAddr, 
                                    void  * callBackFunc_p);

#endif /* MAC_MLME_PARENT */





