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
* \file    hal_mib.c
* \brief   Routines to initialize MIB values by reading hardware registers
*/


#include "wltypes.h"
#include "wl_macros.h"
#include "IEEE_types.h"
#include "wl_mib.h"
#include "wl_hal.h"
#include "mib.h"
#include "qos.h"
#include "wlmac.h"

UINT8 dataRates[IEEEtypes_MAX_DATA_RATES_G] = 
{ 2, 4, 11, 22, 12, 18, 24, 36, 48, 72, 96, 108, 44, 144 };

static void hal_InitPhyMIB(MIB_802DOT11 * mib)
{
}

BOOLEAN hal_InitApMIB(MIB_802DOT11 * mib)
{
	hal_InitPhyMIB(mib);
	return (TRUE);
}

BOOLEAN hal_InitStaMIB(MIB_802DOT11 * mib)
{
	MIB_OP_DATA *mibOpData;

	mibOpData = mib->OperationTable;

	hal_InitPhyMIB(mib);
	return (TRUE);
}


