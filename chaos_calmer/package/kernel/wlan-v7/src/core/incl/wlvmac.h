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
* Description:  Implementation of the STA MLME Module Services
*
*/

#ifndef WLVMAC_MLME
#define WLVMAC_MLME

#define NUM_OF_WLMACS   2
#define MAC_0           0
#define MAC_1           1

/* Maximum number of virtual mac instances */
#define MAX_STA_VMAC_INSTANCE       32
#define MAX_ALLOW_AP_PER_MAC    8


/* Definition of virtual mac id */
typedef SINT32	vmacId_t;
typedef UINT8	phyMacId_t;
typedef SINT8   trunkId_t;
typedef SINT32	halMacId_t;

#define VMAC_MODE_AP                0
#define VMAC_MODE_CLNT_INFRA        1
#define VMAC_MODE_CLNT_ADHOC        2

#define VMAC_SRV_CLIENT_M0       ( 1 << (0 + 8) )
#define VMAC_SRV_CLIENT_M1       ( 1 << (1 + 8) )

/* Definition of virtual mac entry for each instance */
typedef struct vmacEntry_t
{
	UINT8				active;
	IEEEtypes_MacAddr_t	vmacAddr;
	vmacId_t  			id;  	
	phyMacId_t			phyHwMacIndx;
	halMacId_t			macId;
	trunkId_t           trunkId;
	UINT8               modeOfService;  /* AP=0 ; Client=1 */
	SINT8 				(*mlmeMsgEvt)(UINT8 *data0, UINT8 *data1, UINT8 *info);
	SINT8 				(*dot11MsgEvt)(UINT8 *data0, UINT8 *data1, UINT8 *info);
	UINT8				*info_p;	//can point to either vmacApInfo_t or vmacApInfo_t
	void                *privInfo_p;
}vmacEntry_t;

extern SINT32 vmacRegister(vmacEntry_t *entry);
extern vmacEntry_t *vmacGetVMacEntryById(vmacId_t vmacIdNum);
extern vmacEntry_t *vmacGetVMacEntryByAddr(UINT8 *macAddr_p);
extern UINT8 *vmacGetVMacStaInfo(vmacId_t vmacIdNum);
extern UINT32 vmacActiveSrvId(UINT8 phyMacIndx,UINT32 srvId);
extern UINT32 vmacDeActiveSrvId(UINT8 phyMacIndx, UINT32 srvId);
extern UINT32 vmacQueryActiveSrv(UINT8 phyMacIndx, UINT32 srvId);
extern void vmacUnRegister(vmacId_t vmacIdNum);
extern SINT32 vmacStaInfoInit(UINT8 *staDataInfo_p);


/* Extern */
extern vmacEntry_t *vmacTable_p[MAX_STA_VMAC_INSTANCE];

#endif /* WLVMAC_MLME */





