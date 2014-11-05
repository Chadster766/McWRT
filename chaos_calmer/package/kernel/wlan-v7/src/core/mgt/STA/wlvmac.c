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

/*******************************************************************************************
*
* File: mlmeChild.h
*        Virtual Mac Module
* Description:  Implementation of the Virtual MAC Module Services
*******************************************************************************************/
#include "mlmeSta.h"
#include "wl_mib.h"
#include "wl_hal.h"

#include "mlmeApi.h"
#include "mlmeSta.h"
#include "wlvmac.h"

/* Global Declaration */
vmacEntry_t *vmacTable_p[MAX_STA_VMAC_INSTANCE];

/* Bits MAP of what vMac Services is actively running */
UINT32   vMacActiveSrvBitMap[NUM_OF_WLMACS] = {0,0};



/*************************************************************************
* Function: vmacStaInfoInit
*
* Description: Inital Client MLME Information Structure.
*              The MLME Module is at liberty to init the
*              Station MLME Information Structure to
*			   Any foreseen state or condition it want
* Input:       
*              
* Output:
**************************************************************************/
extern SINT32 vmacStaInfoInit(UINT8 *staDataInfo_p)
{
    vmacStaInfo_t *vStaInfo_p = (vmacStaInfo_t *)staDataInfo_p;
    
    if(vStaInfo_p == NULL)
    {
        return 1;
    }
	vStaInfo_p->staSecurityMibs.mib_PrivacyTable_p->RSNLinkStatus = 0;  	
	vStaInfo_p->AssociatedFlag = 0;
	vStaInfo_p->IBssStartFlag = 0;
	vStaInfo_p->Adhoc_Active = 0;
	vStaInfo_p->ScanForAnyBeacons = TRUE;
	vStaInfo_p->NumScanChannels = 0;
	vStaInfo_p->counterInt = 0;
	vStaInfo_p->misMatchBssidCount = 0;
	vStaInfo_p->bcnCount = 0;
	vStaInfo_p->linkQuality = 0;
	vStaInfo_p->rxBcnCnt	= 0;
	vStaInfo_p->rxBcnPeriod = 100;
	vStaInfo_p->smeMain_State = STATE_IDLE;
	vStaInfo_p->macMgmtMain_State   = STATE_IDLE;
    vStaInfo_p->macMgmtMain_PostScanState = STATE_IDLE;
	vStaInfo_p->PostScanState = STATE_IDLE;
    vStaInfo_p->g_rcvdProbeRsp = 1;
    vStaInfo_p->cmdHistory = 0;
	vStaInfo_p->macMgt_StaMode = *vStaInfo_p->mib_StaMode_p;
	/* Init the STA State Machine Services */
	macMgtSyncSrvStaInit(vStaInfo_p);
	/* bt_move this outside for Configuration */
	vStaInfo_p->macMgmtMain_PwrMode = PWR_MODE_ACTIVE;
#ifdef buildModes_RETRIES
	vStaInfo_p->ContinueScanning =  TRUE;
	vStaInfo_p->JoinRetryCount = 0;
	vStaInfo_p->AssocRetryCount = 0;
#endif /* buildModes_RETRIES */
	vStaInfo_p->AuthRetryCount = 0;
	SPIN_LOCK_INIT(&vStaInfo_p->ScanResultsLock);
	return 0;
}

/*************************************************************************
* Function: vmacRegister
*
* Description: Register your Station MLME Information Structure
* Input:       
*              
* Output:
**************************************************************************/
extern SINT32 vmacRegister(vmacEntry_t *entry)
{
	vmacId_t vmacIdNum;

	for(vmacIdNum=0; vmacIdNum < MAX_STA_VMAC_INSTANCE; vmacIdNum++)
	{
		if(vmacTable_p[vmacIdNum] == NULL)
		{
			entry->id = vmacIdNum;
			vmacTable_p[vmacIdNum] = entry;
			return vmacIdNum;
		}
	}
	return -1;
}

/*************************************************************************
* Function: vmacUnRegister
*
* Description: UnRegister your id and Station MLME Information Structure
* Input:       
*              
* Output:
**************************************************************************/
extern void vmacUnRegister(vmacId_t vmacIdNum)
{
	if(vmacIdNum <MAX_STA_VMAC_INSTANCE)
	{
		vmacTable_p[vmacIdNum] = NULL;
	}
}

/*************************************************************************
* Function: vmacInitRegTable
*
* Description: Init the Registration table
* Input:       
*              
* Output:
**************************************************************************/
extern void vmacInitRegTable(void)
{
	UINT32 i;
    
	for(i=0; i < MAX_STA_VMAC_INSTANCE; i++)
	{
		vmacTable_p[i] = NULL;
	}
}

/*************************************************************************
* Function: vmacGetVMacEntryById
*
* Description: Get function to retrieve the vMac Entry by vMac Id
* Input:       
*              
* Output:
**************************************************************************/
extern vmacEntry_t *vmacGetVMacEntryById(vmacId_t vmacIdNum)
{
	if(vmacIdNum >= MAX_STA_VMAC_INSTANCE || vmacIdNum < 0)
	{
		return NULL;
	}
    return vmacTable_p[vmacIdNum];
}

/*************************************************************************
* Function: vmacGetVMacEntryById
*
* Description: Get function to retrieve the vMac Entry by vMac Id
* Input:       
*              
* Output:
**************************************************************************/
extern vmacEntry_t *vmacGetVMacEntryByAddr(UINT8 *macAddr_p)
{
	UINT8 i;
	UINT8 nullAddr[6] = {0,0,0,0,0,0};

	if(memcmp(macAddr_p, nullAddr, 6) == 0)
	{
		return NULL;
	}
	for(i = 0; i < MAX_STA_VMAC_INSTANCE; i++)
	{
        if (vmacTable_p[i] == NULL)
        {
            continue;
        }
		if(memcmp(macAddr_p, vmacTable_p[i]->vmacAddr, IEEEtypes_ADDRESS_SIZE) == 0)
		{
            return vmacTable_p[i];
		}

	}
    return NULL;
}

/*************************************************************************
* Function: vmacGetVMacStaInfo
*
* Description: Get function to retrieve the vMac Entry's StaInfo 
* by Station Info
* Input:       
*              
* Output:
**************************************************************************/
extern UINT8 *vmacGetVMacStaInfo( vmacId_t vmacIdNum )
{
	if(vmacIdNum >= MAX_STA_VMAC_INSTANCE || vmacIdNum < 0)
	{
		return NULL;
	}
    if (vmacTable_p[vmacIdNum] == NULL)
    {
		return NULL;
    }
    return vmacTable_p[vmacIdNum]->info_p;
}

/*************************************************************************
* Function: vmacGetVMacEntryByTrunkId
*
* Description: Get function to retrieve the vMac Entry by TrunkId
* Input:       
*              
* Output:
**************************************************************************/
extern vmacEntry_t *vmacGetVMacEntryByTrunkId(trunkId_t trunkId)
{
	UINT8 i;

	for(i = 0; i < MAX_STA_VMAC_INSTANCE; i++)
	{
        if (vmacTable_p[i] == NULL)
        {
            continue;
        }
		if(vmacTable_p[i]->trunkId == trunkId)
		{
			return vmacTable_p[i];
		}
	}
    return NULL;
}

/*************************************************************************
* Function: vmacActiveSrvId
*
* Description: Set the vMac Service Id to Active
* Input:       
*              
* Output:
**************************************************************************/
extern UINT32 vmacActiveSrvId(UINT8 phyMacIndx, UINT32 srvId)
{
    vMacActiveSrvBitMap[phyMacIndx] |= srvId;
    return vMacActiveSrvBitMap[phyMacIndx];
}

/*************************************************************************
* Function: vmacDeActiveSrvId
*
* Description: UnSet the vMac Service Id to Idle
* Input:       
*              
* Output:
**************************************************************************/
extern UINT32 vmacDeActiveSrvId(UINT8 phyMacIndx, UINT32 srvId)
{
	vMacActiveSrvBitMap[phyMacIndx] &= ~srvId;
    return vMacActiveSrvBitMap[phyMacIndx];
}

/*************************************************************************
* Function: vmacQueryActiveSrv
*
* Description: Return the Active Services Bits Map
* Input:       
*              
* Output:
**************************************************************************/
extern UINT32 vmacQueryActiveSrv(UINT8 phyMacIndx, UINT32 srvIdQuery)
{
    return (vMacActiveSrvBitMap[phyMacIndx] & srvIdQuery);
}

