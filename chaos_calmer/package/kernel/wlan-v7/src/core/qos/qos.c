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


#ifdef QOS_FEATURE

#include "ap8xLnxIntf.h"

#include "wltypes.h"
#include "IEEE_types.h"
#include "mib.h"
#include "qos.h"
#include "ds.h"
#include "osif.h"
#include "keyMgmtCommon.h"
#include "keyMgmt.h"
#include "tkip.h"
#include "macmgmtap.h"
#include "macMgmtMlme.h"
#include "wlmac.h"

#include "bcngen.h"
#include "wl_hal.h"
#include "bcngen.h"


//=============================================================================
//                         Imported PUBLIC VARIABLES
//============================================================================


//=============================================================================
//                         PUBLIC VARIABLES
//============================================================================        

TXOP_t TxOpDb[MAX_QOS_STRMS];
TSPECEntry_t TSpecEntryDb[MAX_QOS_STRMS];
UINT32 SItimerID; //var to hold the timer ID of the Service Interval
UINT32 NumCurrentStreams=0;
mib_QAPEDCATable_t mib_QAPEDCATable[4];
mib_QStaEDCATable_t mib_QStaEDCATable[4];
//extern PktAccPolicy_t (*Qos_GetMatchingTiditcmFp)(apio_bufdescr_t *pBufDescr, UINT32 Aid, UINT32 * AC_prio_p, UINT32 *gTid) ;
extern Status_e AddTxOpEntry(vmacApInfo_t *vmacSta_p,UINT32 Indx, UINT16 Aid, UINT8 ClientMode);
extern struct sk_buff *mlmeApiPrepMgtMsg(UINT32 Subtype, IEEEtypes_MacAddr_t *DestAddr, IEEEtypes_MacAddr_t *SrcAddr);
extern WL_STATUS pool_FreeBuf ( char* ReturnedBuf_p );

UINT32 EDCA_param_set_update_cnt;
#define PROTOCOL_TCP 6
#define PROTOCOL_UDP 17
extern UINT8 mib_wbMode,mib_urMode;

UINT8 AccCategoryQ[8] = {AC_BE_Q,AC_BK_Q,AC_BK_Q,AC_BE_Q,AC_VI_Q,AC_VI_Q,AC_VO_Q,AC_VO_Q};
//=============================================================================
//                         PRIVATE FUNCTIONS
//============================================================================ 
/******************************************************************************
* 
* Name: Qos_InitTCLASTable 
* 
* Description: Will initialise the TCLAS table for each station.
*     
* 
* Conditions For Use: 
*    At AP init. 
* 
* Arguments: 
*                                
* Return Value: 
*    
* 
* Notes: 
*    None. 
* 
* PDL: 
*
* END PDL 
* 
*****************************************************************************/
void  Qos_InitTCLASTable(void)
{
	UINT32 i, j;

	memset(TSpecEntryDb,0, sizeof(TSpecEntryDb));

	for (i=0; i<MAX_AID; i++)
	{
		for (j=0; j< MAX_TCLAS_PER_STN; j++)
		{
			AssocTable[i].Qos_Stn_Data.TCLASEntry[j].gTID = 0xffffffff;
		}
	}
	//Qos_SetTCLASTable();
	return;
}

//=============================================================================
//                         PUBLIC FUNCTIONS
//============================================================================ 

/******************************************************************************
* 
* Name: InitQosParam 
* 
* Description: Will initialise all the counters for QoS.
*     
* 
* Conditions For Use: 
*    When the AP is initialised. 
* 
* Arguments: 
*    None. 
*                                 
* Return Value: 
*    Integer. 1 if successful. 0 if unsuccessful. 
* 
* Notes: 
*    None. 
* 
* PDL: 
*
* END PDL 
* 
*****************************************************************************/
UINT32 InitQosParam(void)
{

	EDCA_param_set_update_cnt =0;
	//EDCA_Beacon_Counter = (EDCA_UPDATE_PERIOD) * mib_StaCfg.DtimPeriod;
	EDCA_Beacon_Counter = 0;
	SItimerID = 0xFFFFFFFF;

	return 1;
}


/******************************************************************************
* 
* Name: GetLog 
* 
* Description: Will return the logarithm of the argument. num=2^(return value) -1
*     
* 
* Conditions For Use:
* 1> When the user issues a EDCA parameter set command.  
*     
* 
* Arguments: 
*    a 32 bit number. 
*                                 
* Return Value: 
*    a 32 bit Integer. num=2^(return value) -1. If it returns zero, it is an error. 
*    If num=1; then  return 1.
* 
* Notes: 
*    None. 
* 
* PDL: 
*
* END PDL 
* 
*****************************************************************************/
UINT32 GetLog(UINT32 num)
{
	UINT32 i;
	num = num+1;
	for (i=0; i<=32; i++)
	{
		num = num>>1;
		if (num== 0)
			return i;
	}
	return 0;
}

/******************************************************************************
* 
* Name: GetChannelCapacity 
* 
* Description: Will return channel capacity used.
*     
* 
* Conditions For Use: 
*    . 
* 
* Arguments: 
*    . 
*                                 
* Return Value: 
*    A number in units of 32 micorseconds..
* 
* Notes: 
*    None. 
* 
* PDL: 
*
* END PDL 
* 
*****************************************************************************/
UINT16 GetChannelCapacity(void)
{
	//currently assign a default value of 3*10^4 of 32 microseconds units.
	//This corressponds to 96% of channel capacity being available.
	return 0x7530;
}

/******************************************************************************
* 
* Name: CopyTSpecElems 
* 
* Description: Copy TSpec elements from the received packet into
*               the Tspec Database
*     
* 
* Conditions For Use: 
*    . 
* 
* Arguments: 
*    pTSpecSrc = Pointer to Tspec packet. 
*    TSpecDst = Pointer to Tspec DB.                             
*                                 
* Return Value: 
*    none
* 
* Notes: 
*    Need to do it this way because IEEEtypes_TSPEC_t is packed and  
*    Mrvl_TSPEC_t is not packed
* 
* PDL: 
*
* END PDL 
* 
*****************************************************************************/
void CopyTSpecElems(Mrvl_TSPEC_t *pTSpecDst, IEEEtypes_TSPEC_t *pTSpecSrc)
{
	WSM_TSPEC_t *pWsmTspecSrc;
	if (pTSpecSrc->ElementId == PROPRIETARY_IE)
	{
		pWsmTspecSrc = (WSM_TSPEC_t *)pTSpecSrc;
		pTSpecDst->ElementId = pWsmTspecSrc->ElementId;
		pTSpecDst->Len = pWsmTspecSrc->Len;
		memcpy(&pTSpecDst->ts_info, &pWsmTspecSrc->ts_info, sizeof(IEEEtypes_TS_info_t));
		pTSpecDst->nom_msdu_size = pWsmTspecSrc->nom_msdu_size;
		pTSpecDst->max_msdu_size = pWsmTspecSrc->max_msdu_size;
		pTSpecDst->min_SI = pWsmTspecSrc->min_SI;//convert to msec
		pTSpecDst->max_SI = pWsmTspecSrc->max_SI;//convert to msec

		if(pWsmTspecSrc->inactive_intrvl)
			pTSpecDst->inactive_intrvl = pWsmTspecSrc->inactive_intrvl;
		else
			pTSpecDst->inactive_intrvl = 0xffffff;

		pTSpecDst->suspen_intrvl = pWsmTspecSrc->suspen_intrvl;
		pTSpecDst->serv_start_time = pWsmTspecSrc->serv_start_time;
		pTSpecDst->min_data_rate = pWsmTspecSrc->min_data_rate;
		pTSpecDst->mean_data_rate = pWsmTspecSrc->mean_data_rate;
		pTSpecDst->peak_data_rate = pWsmTspecSrc->peak_data_rate;
		pTSpecDst->max_burst_size = pWsmTspecSrc->max_burst_size;
		pTSpecDst->delay_bound = pWsmTspecSrc->delay_bound;
		pTSpecDst->min_phy_rate = pWsmTspecSrc->min_phy_rate;
		pTSpecDst->srpl_bw_allow= pWsmTspecSrc->srpl_bw_allow;
		pTSpecDst->med_time = pWsmTspecSrc->med_time;
	}
	else
	{
		pTSpecDst->ElementId = pTSpecSrc->ElementId;
		pTSpecDst->Len = pTSpecSrc->Len;
		memcpy(&pTSpecDst->ts_info, &pTSpecSrc->ts_info, sizeof(IEEEtypes_TS_info_t));
		pTSpecDst->nom_msdu_size = pTSpecSrc->nom_msdu_size;
		pTSpecDst->max_msdu_size = pTSpecSrc->max_msdu_size;
		pTSpecDst->min_SI = pTSpecSrc->min_SI;//convert to msec
		pTSpecDst->max_SI = pTSpecSrc->max_SI;//convert to msec

		if(pTSpecSrc->inactive_intrvl)
			pTSpecDst->inactive_intrvl = pTSpecSrc->inactive_intrvl;
		else
			pTSpecDst->inactive_intrvl = 0xffffff;

		pTSpecDst->suspen_intrvl = pTSpecSrc->suspen_intrvl;
		pTSpecDst->serv_start_time = pTSpecSrc->serv_start_time;
		pTSpecDst->min_data_rate = pTSpecSrc->min_data_rate;
		pTSpecDst->mean_data_rate = pTSpecSrc->mean_data_rate;
		pTSpecDst->peak_data_rate = pTSpecSrc->peak_data_rate;
		pTSpecDst->max_burst_size = pTSpecSrc->max_burst_size;
		pTSpecDst->delay_bound = pTSpecSrc->delay_bound;
		pTSpecDst->min_phy_rate = pTSpecSrc->min_phy_rate;
		pTSpecDst->med_time = pTSpecSrc->med_time;
	}
}

/******************************************************************************
* 
* Name: AddTSpecEntry 
* 
* Description: Will return TID for teh TSpec Added..
*     
* 
* Conditions For Use: 
*    . 
* 
* Arguments: 
*    Pointer to Tspec entry. 
*                                 
* Return Value: 
*    index for the TSpec entry added.
* 
* Notes: 
*    None. 
* 
* PDL: 
*
* END PDL 
* 
*****************************************************************************/
inline UINT32 AddTSpecEntry(IEEEtypes_TSPEC_t * pTSpec,
							IEEEtypes_MacAddr_t *pAddr)
{
	UINT32 indx;

	for (indx=0;indx<MAX_QOS_STRMS;indx++)
	{
		/*Get the next free element or if an entry already exists for that TS
		then we need to overwrite it with the new one*/
		if (TSpecEntryDb[indx].not_free == FALSE)
		{
			MACADDR_CPY(&TSpecEntryDb[indx].src_addr, pAddr);
			//memcpy(&(TSpecEntryDb[tid].TSpec), pTSpec, sizeof(IEEEtypes_TSPEC_t));
			CopyTSpecElems(&TSpecEntryDb[indx].TSpec, pTSpec);
			TSpecEntryDb[indx].not_free = TRUE;
			return indx;
		}
	}
	return 0xFFFFFFFF; //Db is full. Signal error
}

/*
This function is called when an entry in the TspecDb is deleted.
The function decrements the gTid element if the appropriate Tclas Entry in the
AssocTable Db by 1, because an entry in the TspecDb is deleted.
*/
void UpdateAssocTblGtid(UINT32 inGTid)
{
	UINT8 Aid_Cnt, Tid_Cnt;

	for (Aid_Cnt=1; Aid_Cnt < MAX_AID; Aid_Cnt++)
	{
		for (Tid_Cnt=0; Tid_Cnt < MAX_TCLAS_PER_STN; Tid_Cnt++)
		{
			if (AssocTable[Aid_Cnt].Qos_Stn_Data.TCLASEntry[Tid_Cnt].gTID == inGTid)
			{
				AssocTable[Aid_Cnt].Qos_Stn_Data.TCLASEntry[Tid_Cnt].gTID--;
				return ;
			}
		}
	}   
}

/******************************************************************************
* 
* Name: DelTSpecEntry 
* 
* Description: Will return success if the Stream is found
*     
* 
* Conditions For Use: 
*    . 
* 
* Arguments: 
*    TId to be deleted 
*                                 
* Return Value: 
*    Status
* 
* Notes: 
*    None. 
* 
* PDL: 
*
* END PDL 
* 
*****************************************************************************/
Status_e DelTSpecEntry(UINT32 Index)
{
	UINT32 i;

	//TSpecEntryDb[Index].not_free = FALSE;
	/*From the index count the number of elements that need to be 
	moved one entry up
	*/

	for (i=Index+1; (TSpecEntryDb[i].not_free == TRUE) && (i<MAX_QOS_STRMS); i++)
	{
		UpdateAssocTblGtid(i);//update the GTid element in the TClas Db.
	}
	i--;
	memcpy(&TSpecEntryDb[Index], &TSpecEntryDb[Index+1], sizeof(TSPECEntry_t)*(i-Index));
	TSpecEntryDb[i].not_free = FALSE;

	return SUCCESS;
}

/******************************************************************************
* 
* Name: DelTClasEntry 
* 
* Description: Will return success if the Stream is found
*     
* 
* Conditions For Use: 
*    . 
* 
* Arguments: 
*    TId to be deleted 
*                                 
* Return Value: 
*    Status
* 
* Notes: 
*    None. 
* 
* PDL: 
*
* END PDL 
* 
*****************************************************************************/
Status_e DelTClasEntry(UINT32 Tid)
{
	UINT8 Aid_Cnt, Tid_Cnt;
	for (Aid_Cnt=1; Aid_Cnt < MAX_AID; Aid_Cnt++)
	{
		for (Tid_Cnt=0; Tid_Cnt < MAX_TCLAS_PER_STN; Tid_Cnt++)
		{
			if (AssocTable[Aid_Cnt].Qos_Stn_Data.TCLASEntry[Tid_Cnt].gTID == Tid)
			{
				AssocTable[Aid_Cnt].Qos_Stn_Data.TCLASEntry[Tid_Cnt].gTID = 0xffffffff;
				return SUCCESS;
			}
		}
	}
	return FAIL;
}





/******************************************************************************
* 
* Name: Qos_ProcessADDTSReqTCLAS 
* 
* Description: 1> Will add a TCLAS entry for that ADDTS Req.
*     
* 
* Conditions For Use: 
*    When Assoc Request comes in. 
* 
* Arguments: 
*    QoS Action Frame containing ADDTS Request. 
*    MAC address of the station that sends the ADDTS Req.
*    AID of the Associated Station.
*
*    
*    
*                             
* Return Value: 
*    Status indicating weather the stream was accepted.
* 
* Notes: 
*    None. 
* 
* PDL: 
*
* END PDL 
* 
*****************************************************************************/
inline Status_e ProcessADDTSReqTCLAS(IEEEtypes_ADDTS_Req_t *AddTSReq_p,
									 IEEEtypes_MacAddr_t *pStaAddr, UINT32 Aid, UINT32 gTID)
{
	UINT8 index;
	TCLAS_t *pTCLAS;
	WSM_TCLAS_Elem_t *pWSM_TCLAS_Elem;
	IEEEtypes_TS_info_t *pTsInfo; 
	IEEEtypes_TSPEC_t *pTSpec;
	WSM_TSPEC_t *pWsmTSpec;
	//IEEEtypes_ADDTS_Req_Frm_t *pADDTS_Request_Frame;

	//pADDTS_Request_Frame = (IEEEtypes_ADDTS_Req_Frm_t*)&(QosActElem_p->QoSAction_u.AddTSReq);

	//Check if TCLAS Db is full
	index=0;
	while ((AssocTable[Aid].Qos_Stn_Data.TCLASEntry[index].gTID != 0xffffffff)
		&& (index<MAX_TCLAS_PER_STN))
		index++;
	if (index >= MAX_TCLAS_PER_STN)
		return FAIL; //TCLAS Db full

	//Add only if TSPEC is downlink or Bidirectional.
	//Else simply return with a success.
	if (AddTSReq_p->TSpec.ElementId == PROPRIETARY_IE)
	{
		pWsmTSpec = &AddTSReq_p->TSpec;
		pTsInfo = &pWsmTSpec->ts_info;
	}
	else
	{
		pTSpec = (IEEEtypes_TSPEC_t *)&AddTSReq_p->TSpec;
		pTsInfo = &pTSpec->ts_info;
	}

	if ((pTsInfo->direction == UPLINK) 
		|| (pTsInfo->direction == DIRECTLINK))
		return SUCCESS;


	pTCLAS= (TCLAS_t*)(&(AddTSReq_p->TSpec.ElementId) + AddTSReq_p->TSpec.Len + 2);
	if ((pTCLAS->ElementId == TCLAS) || (pTCLAS->ElementId == PROPRIETARY_IE))
	{
		//Process TCLAS elements here if there are any.
		while (((pTCLAS->ElementId == TCLAS) || (pTCLAS->ElementId == PROPRIETARY_IE))
			&& (index < MAX_TCLAS_PER_STN))
		{
			//There are more elements appended to the ADDTS Request Frame.
			//Look for TCLAS. This is for IEEE 802.11e
			if ((pTCLAS->ElementId == TCLAS) )
			{

				AssocTable[Aid].Qos_Stn_Data.TCLASEntry[index].gTID = gTID;
				memcpy(&(AssocTable[Aid].Qos_Stn_Data.TCLASEntry[index].TCLAS), 
					pTCLAS, pTCLAS->Len + 2);



				pTCLAS = (TCLAS_t *)((UINT8 *)pTCLAS + pTCLAS->Len + 2);
				index++;
			}
#ifdef QOS_WSM_FEATURE
			//The TCLAS element might have come wrapped in a OUI. This is for WiFi WSM
			else if ((pTCLAS->ElementId == PROPRIETARY_IE))
			{
				pWSM_TCLAS_Elem = (WSM_TCLAS_Elem_t *)pTCLAS;
				if (pWSM_TCLAS_Elem->OUI.Subtype != WSM_TCLAS)
					break;
				AssocTable[Aid].Qos_Stn_Data.TCLASEntry[index].gTID = gTID;
#ifdef STA_QOS
				memcpy(&(AssocTable[Aid].Qos_Stn_Data.TCLASEntry[index].TCLAS.usr_priority), 
					&(pWSM_TCLAS_Elem->WSM_Frm_classifier.usr_priority), pWSM_TCLAS_Elem->Len - 6);
#else
				memcpy(&(AssocTable[Aid].Qos_Stn_Data.TCLASEntry[index].TCLAS.usr_priority), 
					&(pWSM_TCLAS_Elem->usr_priority), pWSM_TCLAS_Elem->Len - 6);
#endif
				/* temp code, ignore tclass */
				//AssocTable[Aid].Qos_Stn_Data.TCLASEntry[index].TCLAS.frm_classifier.classif_mask = 0;
				pTCLAS = (TCLAS_t *)((UINT8 *)pTCLAS + pTCLAS->Len + 2);
				index++;
				AssocTable[Aid].Qos_Stn_Data.TClassCnt = 1;  /* just make it non zero */
				AssocTable[Aid].Qos_Stn_Data.DefaultgTID = gTID;  /* current tid as default, this is immaterial, when
																  tclass is present, used only when no tcalss are present*/

			}
#endif //QOS_WSM_FEATURE
		}
		//Now search for TCLAS Processing Elements.
		if (pTCLAS->ElementId == TCLAS_PROCESSING)
		{
			memcpy(&(AssocTable[Aid].Qos_Stn_Data.TCLAS_Processing), 
				pTCLAS, sizeof(TCLAS_Processing_t));
		}
	}
	else
	{
		AssocTable[Aid].Qos_Stn_Data.TClassCnt = 0;
		AssocTable[Aid].Qos_Stn_Data.DefaultgTID = gTID;

	}


	return SUCCESS;
}

BOOLEAN AdmissionCntrl(UINT32 reqBW)
{
	UINT32 i, BWsum;

	BWsum = reqBW;
	//get the current utilized BW
	for (i=0; (i<MAX_QOS_STRMS) &&  TSpecEntryDb[i].not_free; i++)
	{
		BWsum += TSpecEntryDb[i].TSpec.mean_data_rate;
	}
	return BWsum < MAX_QOS_BW ? 1:0;
}

/******************************************************************************
* 
* Name: ProcessADDTSRequest 
* 
* Description: Will return ADDTS Response.
*     
* 
* Conditions For Use: 
*    When Assoc Request comes in. 
* 
* Arguments: 
*    QoS Action Frame containing ADDTS Request. 
*    QoS Resp Frame containing ADDTS Response                             
* Return Value: 
*    Status indicating weather the stream was accepted.
* 
* Notes: 
*    None. 
* 
* PDL: 
*
* END PDL 
* 
*****************************************************************************/
UINT16 ProcessADDTSRequest(vmacApInfo_t *vmacSta_p,IEEEtypes_ADDTS_Req_t *AddTSReq_p,
						   IEEEtypes_MacAddr_t *pStaAddr, UINT16 Aid,
						   UINT32 *TspecIndx, UINT8 ClientMode)
{
	UINT32 index;

	//Send the Tspec entry to the Scheduler function.
	if ((AddTSReq_p->TSpec.ts_info.access_policy == HCCA) ||
		(AddTSReq_p->TSpec.ts_info.access_policy == BOTH))
	{

		if (!AdmissionCntrl(AddTSReq_p->TSpec.mean_data_rate))
		{//check for admission control
			return IEEEtypes_STATUS_QOS_INSUFFICIENT_BW;
		}
		//Add entry into the Tspec Database
		index = AddTSpecEntry((IEEEtypes_TSPEC_t *)&AddTSReq_p->TSpec,
			pStaAddr);
		if (index == 0xFFFFFFFF)
		{//Tspec DB is full!!!
			return IEEEtypes_STATUS_QOS_REFUSED;
		}
		//Add entry tinto the Txop Data Base
		if (AddTxOpEntry(vmacSta_p,index,Aid,ClientMode) == FAIL)
		{//Txop DataBase full!!!
			DelTSpecEntry(index);
			return IEEEtypes_STATUS_QOS_REFUSED;
		}
		//Update TCLAS Table.
		ProcessADDTSReqTCLAS(AddTSReq_p, pStaAddr, Aid, index);
		//Also reset the Delts Counter.
		TSpecEntryDb[index].DelTS_PktCnt = 0;

		NumCurrentStreams++;
		*TspecIndx = index;
		AssocTable[Aid].Qos_Stn_Data.TSpecCnt++;
		return IEEEtypes_STATUS_SUCCESS;
	}
	else
	{//Reject EDCA Tspecs. WME mandates it
		return IEEEtypes_STATUS_QOS_INVALID_PARAMS;
	}

}

/******************************************************************************
* 
* Name: ProcessADDTSRequestSchedule 
* 
* Description: Will return ADDTS Response.
*     
* 
* Conditions For Use: 
*    When Assoc Request comes in. 
* 
* Arguments: 
*    Pointer to ADDTS Response                             
* Return Value: 
*    Length of the ADDTS Response Structure with schedule added.
* 
* Notes: 
*    None. 
* 
* PDL: 
*
* END PDL 
* 
*****************************************************************************/
UINT8 ProcessADDTSRequestSchedule(IEEEtypes_ADDTS_Rsp_t *pAddTsRspFrm,
								  UINT32 TspecDBindx)
{
	IEEEtypes_Sched_Element_t *pSchedule;
	WSM_Sched_Element_t *pWSM_Schedule;
	TCLAS_t *pTCLAS;
	//IEEEtypes_ADDTS_Req_Frm_t *pADDTS_Request_Frame;
	UINT8 ADDTSRespLen;
	UINT32 tsflow = 0/*, tsfhi*/;
	TXOP_t *pTxOp;

	//pADDTS_Request_Frame = (IEEEtypes_ADDTS_Req_Frm_t*)&(QosActElem_p->QoSAction_u.AddTSReq);
	ADDTSRespLen = 5 + pAddTsRspFrm->TSpec.Len + 2; //THe initial elements of ADDTS Response + TSPEC.
	//Process TCLAS elements here if there are any.

	pTCLAS= (TCLAS_t*)(&(pAddTsRspFrm->TSpec.ElementId) + pAddTsRspFrm->TSpec.Len + 2);

	//Look for TCLAS. This is for IEEE 802.11e

	while ((pTCLAS->ElementId == TCLAS ||pTCLAS->ElementId == PROPRIETARY_IE))
	{
		ADDTSRespLen += pTCLAS->Len + 2;
		pTCLAS = (TCLAS_t *)((UINT8 *)pTCLAS + pTCLAS->Len + 2);

	}
	//Now search for TCLAS Processing Elements.
	if (pTCLAS->ElementId == TCLAS_PROCESSING)
	{
		ADDTSRespLen += pTCLAS->Len + 2;
		pTCLAS = (TCLAS_t *)((UINT8 *)pTCLAS + pTCLAS->Len + 2);
	}

	//Now add schedule element here.
	pTxOp = GetTxOpFrmTspecIndx(TspecDBindx);
	pSchedule =  (IEEEtypes_Sched_Element_t *)pTCLAS;
	pWSM_Schedule = (WSM_Sched_Element_t *)pSchedule;
#ifdef QOS_WSM_FEATURE
	pWSM_Schedule->ElementId = 221;
	pWSM_Schedule->Len = 18;
	pWSM_Schedule->OUI.OUI[0] = 0;
	pWSM_Schedule->OUI.OUI[1] = 0x50;
	pWSM_Schedule->OUI.OUI[2] = 0xf2;
	pWSM_Schedule->OUI.Type = 2;
	pWSM_Schedule->OUI.Subtype = WSM_SCHED;
	pWSM_Schedule->version = 1;
	pWSM_Schedule->sched_info.aggr = 0;
	pWSM_Schedule->sched_info.dir = pAddTsRspFrm->TSpec.ts_info.direction;
	pWSM_Schedule->sched_info.TSID = pAddTsRspFrm->TSpec.ts_info.tsid;
	pWSM_Schedule->serv_intrvl = pTxOp->mySI;//in usec
	pWSM_Schedule->serv_start_time = tsflow;
	pWSM_Schedule->spec_intrvl =  DEFAULT_SPEC_INTERVAL;
#else //QOS_WSM_FEATURE

	//Fill the Schedule.
	pSchedule->ElementId = SCHEDULE;
	pSchedule->Len = 14;
	pSchedule->sched_info.aggr = pAddTsRspFrm->TSpec.ts_info.aggregation;
	pSchedule->sched_info.TSID = pAddTsRspFrm->TSpec.ts_info.tsid;
	pSchedule->sched_info.dir =  pAddTsRspFrm->TSpec.ts_info.direction;
	pSchedule->serv_start_time = pAddTsRspFrm->TSpec.serv_start_time;
	pSchedule->serv_intrvl = pAddTsRspFrm->TSpec.min_SI;
	pSchedule->max_serv_duration =  pTxOp->TXOP >> 5; //Not sure what to put here.
	pSchedule->spec_intrvl =  DEFAULT_SPEC_INTERVAL;
#endif //QOS_WSM_FEATURE

	ADDTSRespLen += pWSM_Schedule->Len + 2; //Note that this will work even if it is not a WSM Schedule.
	return ADDTSRespLen;
}

UINT32 FindTspecIndx(IEEEtypes_MacAddr_t *pStaAddr, UINT32 TsId)
{
	UINT32 i=0;//, NumStreams=0;

	for (i=0; (i<MAX_QOS_STRMS) && (TSpecEntryDb[i].not_free); i++)
		{
		if (!MACADDR_CMP(pStaAddr, TSpecEntryDb[i].src_addr) && 
			TSpecEntryDb[i].TSpec.ts_info.tsid == TsId)
			{
			return i;
			}
		}
	return 0xFFFFFFFF;
	}

Status_e ProcessDELTSRequest(vmacApInfo_t *vmacSta_p,IEEEtypes_MacAddr_t *pStaAddr, UINT32 TsId)
	{
	//get the Tid
	UINT32 TspecIndx;
	extStaDb_StaInfo_t *pStaInfo;

	pStaInfo = extStaDb_GetStaInfo(vmacSta_p,pStaAddr, 1);

	TspecIndx = FindTspecIndx(pStaAddr, TsId);
	if (TspecIndx == 0xFFFFFFFF)
		{
		return FAIL;
		}
	if(pStaInfo)
	{
		AssocTable[pStaInfo->Aid].Qos_Stn_Data.TSpecCnt--;
	}
	DelTSpecEntry(TspecIndx);
	DelTxOpEntry(TspecIndx);
	DelTClasEntry(TspecIndx);
	NumCurrentStreams--;
	TSpecEntryDb[TspecIndx].DelTS_PktCnt = 0;
	return SUCCESS;
}



/******************************************************************************
* 
* Name: Qos_GetMatchingTid 
* 
* Description: Will return TID.
*     
* 
* Conditions For Use: 
*    When data pkt arrives at ethernet receive. 
* 
* Arguments: 
*    apio_bufdescr_t *pBufDescr
*    UINT32 Aid of associated station                            
* Return Value: 
*    TID for the pkt. If no TID, return 0xffffffff
*    Will return the priority for pkt if no TCLAS found but still a QoS pkt.
* 
* Notes: 
*    None. 
* 
* PDL: 
*
* END PDL 
* 
*****************************************************************************/





void Qos_SetTCLASTable(void)
{
	UINT32 i, j;
	for (i=0; i<=MAX_AID; i++)
	{
		AssocTable[i].CapInfo.QoS = 1;
		for (j=0; j< MAX_TCLAS_PER_STN; j++)
		{
			if (j==0)
			{
				AssocTable[i].Qos_Stn_Data.TCLASEntry[j].gTID = j+i;
				AssocTable[i].Qos_Stn_Data.TCLASEntry[j].TCLAS.frm_classifier.classif_mask = 0x08;
				AssocTable[i].Qos_Stn_Data.TCLASEntry[j].TCLAS.frm_classifier.classif_type = 0x01;
				AssocTable[i].Qos_Stn_Data.TCLASEntry[j].TCLAS.frm_classifier.classif_params.classif_0.src_addr[0]
				= 0x00;
				AssocTable[i].Qos_Stn_Data.TCLASEntry[j].TCLAS.frm_classifier.classif_params.classif_0.src_addr[1]
				= 0x0b;
				AssocTable[i].Qos_Stn_Data.TCLASEntry[j].TCLAS.frm_classifier.classif_params.classif_0.src_addr[2]
				= 0xdb;
				AssocTable[i].Qos_Stn_Data.TCLASEntry[j].TCLAS.frm_classifier.classif_params.classif_0.src_addr[3]
				= 0xa3;
				AssocTable[i].Qos_Stn_Data.TCLASEntry[j].TCLAS.frm_classifier.classif_params.classif_0.src_addr[4]
				= 0x18;
				AssocTable[i].Qos_Stn_Data.TCLASEntry[j].TCLAS.frm_classifier.classif_params.classif_0.src_addr[5]
				= 0x5b;

				AssocTable[i].Qos_Stn_Data.TCLASEntry[j].TCLAS.frm_classifier.classif_params.classif_0.dst_addr[0]
				= 0x00;
				AssocTable[i].Qos_Stn_Data.TCLASEntry[j].TCLAS.frm_classifier.classif_params.classif_0.dst_addr[1]
				= 0x07;
				AssocTable[i].Qos_Stn_Data.TCLASEntry[j].TCLAS.frm_classifier.classif_params.classif_0.dst_addr[2]
				= 0x40;
				AssocTable[i].Qos_Stn_Data.TCLASEntry[j].TCLAS.frm_classifier.classif_params.classif_0.dst_addr[3]
				= 0x91;
				AssocTable[i].Qos_Stn_Data.TCLASEntry[j].TCLAS.frm_classifier.classif_params.classif_0.dst_addr[4]
				= 0x43;
				AssocTable[i].Qos_Stn_Data.TCLASEntry[j].TCLAS.frm_classifier.classif_params.classif_0.dst_addr[5]
				= 0xf2;

				AssocTable[i].Qos_Stn_Data.TCLASEntry[j].TCLAS.frm_classifier.classif_params.classif_0.type
					= 0x08;

				AssocTable[i].Qos_Stn_Data.TCLASEntry[j].TCLAS.frm_classifier.classif_params.classif_1_IPv4.src_IP_addr[0]
				= 0xc0;
				AssocTable[i].Qos_Stn_Data.TCLASEntry[j].TCLAS.frm_classifier.classif_params.classif_1_IPv4.src_IP_addr[1]
				= 0xa8;
				AssocTable[i].Qos_Stn_Data.TCLASEntry[j].TCLAS.frm_classifier.classif_params.classif_1_IPv4.src_IP_addr[2]
				= 0x00;
				AssocTable[i].Qos_Stn_Data.TCLASEntry[j].TCLAS.frm_classifier.classif_params.classif_1_IPv4.src_IP_addr[3]
				= 0x65;

				//AssocTable[i].Qos_Stn_Data.TCLASEntry[j].TCLAS.frm_classifier.classif_params.classif_1_IPv4.dst_port
				//    = 0xd204;

			}
		}
	}
	return;
}

/******************************************************************************
* Name: ClearQoSDB
*
* Description:
*    Delete Entries in the TSpecEntryDb, TCLASEntry and TxOpDb Databases
*
* Conditions for Use:
*    
* Arguments:
*    MAC Addr of the Sta
*
* Return Value:
*    The number of entries deleted
*
* Notes:
*    This function is called whenever AP receives a Association Req from Client
*
* PDL:
*
*****************************************************************************/

UINT32 ClearQoSDB(IEEEtypes_MacAddr_t *pStaAddr)
{
	UINT32 i, tid2del, entries_del=0;

	for (i=0;i<MAX_QOS_STRMS;i++)
	{
		if (!MACADDR_CMP(pStaAddr, TSpecEntryDb[i].src_addr))
		{
			tid2del = TSpecEntryDb[i].gTID;
			DelTSpecEntry(tid2del);
			DelTxOpEntry(tid2del);
			DelTClasEntry(tid2del);
			entries_del++;
			TSpecEntryDb[tid2del].DelTS_PktCnt=0;
		}
	}
	return entries_del;
}

#ifdef QOS_WSM_FEATURE

/******************************************************************************
* 
* Name: QoS_AppendWMEInfoElem
* 
* Description: Will append the WME Information Element to the Beacon/Probe Response/Association Response.
*     
* 
* Conditions For Use: 
*    When the WMEQoSOptImpl or WSMQsSOptImpl are true. 
* 
* Arguments: 
*    Pointer in the Beacon frame where the Information Element needs to exist.. 
*                                 
* Return Value: 
*    The offset to the buffer after the new information element is added. 
* 
* Notes: 
*    None. 
* 
* PDL: 
*
* END PDL 
* 
*****************************************************************************/
UINT8 QoS_AppendWMEInfoElem(vmacApInfo_t *vmacSta_p,UINT8 *pBcnBuf)
{
	MIB_STA_CFG  *mib_StaCfg_p=vmacSta_p->Mib802dot11->StationConfig;
	WME_info_elem_t *pWMEInfoElem = (WME_info_elem_t *)pBcnBuf;
	//BcnWMEInfoElemLocation_p =  (UINT8*) pBcnBuf;

	if (mib_StaCfg_p->WSMQoSOptImpl)
	{
		pWMEInfoElem->ElementId =  221;//WSM_QOS_CAPABILITY;
		pWMEInfoElem->Len = WME_INFO_LEN;
		pWMEInfoElem->OUI.OUI[0] = 0x0;
		pWMEInfoElem->OUI.OUI[1] = 0x50;
		pWMEInfoElem->OUI.OUI[2] = 0xf2;
		pWMEInfoElem->OUI.Type = 2;
		pWMEInfoElem->OUI.Subtype = 0;
		pWMEInfoElem->version = 1;
#ifndef WMM_PS_SUPPORT
		pWMEInfoElem->QoS_info.EDCA_param_set_update_cnt = EDCA_param_set_update_cnt;
#endif
		return(pWMEInfoElem->Len + 2);
	}
	else
		return 0;
}

/******************************************************************************
* 
* Name: bcngen_UpdateWSMQosCapElem
* 
* Description: Will append the WSM Qos Capability Info Element to the Beacon.
*     
* 
* Conditions For Use: 
*    When the WSMQoSOptImpl are true. 
* 
* Arguments: 
*    None. 
*                                 
* Return Value: 
*    The offset to the beacon buffer after the new information element is added. 
* 
* Notes: 
*    None. 
* 
* PDL: 
*
* END PDL 
* 
*****************************************************************************/
UINT16 Qos_UpdateWSMQosCapElem(vmacApInfo_t *vmacSta_p,UINT8 *pBcnBuf)
{
	MIB_STA_CFG  *mib_StaCfg_p=vmacSta_p->Mib802dot11->StationConfig;
	WSM_QoS_Cap_Elem_t *pWSMQosCapElem = (WSM_QoS_Cap_Elem_t *)pBcnBuf;

	if (mib_StaCfg_p->WSMQoSOptImpl)
	{
		pWSMQosCapElem->ElementId =  221;//WSM_QOS_CAPABILITY;
		pWSMQosCapElem->Len = WSM_QOS_CAP;
		pWSMQosCapElem->OUI.OUI[0] = 0x0;
		pWSMQosCapElem->OUI.OUI[1] = 0x50;
		pWSMQosCapElem->OUI.OUI[2] = 0xf2;
		pWSMQosCapElem->OUI.Type = 2;
		pWSMQosCapElem->OUI.Subtype = WSM_CAPABILITY;
		pWSMQosCapElem->version = 1;
		pWSMQosCapElem->QoS_info.Q_ack = 0;
		pWSMQosCapElem->QoS_info.TXOP_req = 0; 
		pWSMQosCapElem->QoS_info.Q_req = 1; 
		return sizeof(WSM_QoS_Cap_Elem_t);
	}
	else
		return 0;
}

/******************************************************************************
* 
* Name: QoS_AppendWMEParamElem
* 
* Description: Will append the WME Param  Element to the Beacon/Probe Response/Association Response.
*     
* 
* Conditions For Use: 
*    When the WSMQoSOptImpl are true. 
* 
* Arguments: 
*    None. 
*                                 
* Return Value: 
*    The offset to the buffer after the new information element is added. 
* 
* Notes: 
*    None. 
* 
* PDL: 
*
* END PDL 
* 
*****************************************************************************/
WME_param_elem_t WMEParamElem;
void InitWMEParamElem(vmacApInfo_t *vmacSta_p)
{
	MIB_802DOT11 *mib=vmacSta_p->Mib802dot11;
	WMEParamElem.ElementId =  221;//WSM_QOS_CAPABILITY;
	WMEParamElem.Len = WME_PARAM_LEN;
	WMEParamElem.OUI.OUI[0] = 0x0;
	WMEParamElem.OUI.OUI[1] = 0x50;

	WMEParamElem.OUI.OUI[2] = 0xf2;
	WMEParamElem.OUI.Type = 2;
	WMEParamElem.OUI.Subtype = 1;
	WMEParamElem.version = 1;
	WMEParamElem.QoS_info.Q_ack = 0;
	WMEParamElem.QoS_info.TXOP_req = 0; 
	WMEParamElem.QoS_info.Q_req =  0 ; //PROCESS_TXOP_REQ;
#ifdef UAPSD_SUPPORT
	WMEParamElem.QoS_info.more_data_ack = 1; //uapsd notification
#endif

	WMEParamElem.QoS_info.EDCA_param_set_update_cnt = EDCA_param_set_update_cnt;
	WMEParamElem.rsvd = 0;
	//Updae EDCA for BE
	WMEParamElem.AC_BE.ACI_AIFSN.AIFSN = mib_QStaEDCATable[0].QStaEDCATblAIFSN;
	WMEParamElem.AC_BE.ACI_AIFSN.ACI = 0;
	WMEParamElem.AC_BE.ACI_AIFSN.ACM = mib_QStaEDCATable[0].QStaEDCATblMandatory;
	WMEParamElem.AC_BE.ECW_min_max.ECW_min = 
		GetLog(mib_QStaEDCATable[0].QStaEDCATblCWmin);
	WMEParamElem.AC_BE.ECW_min_max.ECW_max = 
		GetLog(mib_QStaEDCATable[0].QStaEDCATblCWmax);
	WMEParamElem.AC_BE.TXOP_lim = mib_QStaEDCATable[0].QStaEDCATblTXOPLimit;
	//Update EDCA for BK
	WMEParamElem.AC_BK.ACI_AIFSN.AIFSN = mib_QStaEDCATable[1].QStaEDCATblAIFSN;
	WMEParamElem.AC_BK.ACI_AIFSN.ACI = 1;
	WMEParamElem.AC_BK.ACI_AIFSN.ACM = mib_QStaEDCATable[1].QStaEDCATblMandatory;
	WMEParamElem.AC_BK.ECW_min_max.ECW_min = 
		GetLog(mib_QStaEDCATable[1].QStaEDCATblCWmin);
	WMEParamElem.AC_BK.ECW_min_max.ECW_max = 
		GetLog(mib_QStaEDCATable[1].QStaEDCATblCWmax);
	WMEParamElem.AC_BK.TXOP_lim = mib_QStaEDCATable[1].QStaEDCATblTXOPLimit;
	//Update EDCA for VI
	WMEParamElem.AC_VI.ACI_AIFSN.AIFSN = mib_QStaEDCATable[2].QStaEDCATblAIFSN;
	WMEParamElem.AC_VI.ACI_AIFSN.ACI = 2;
	WMEParamElem.AC_VI.ACI_AIFSN.ACM = mib_QStaEDCATable[2].QStaEDCATblMandatory;
	WMEParamElem.AC_VI.ECW_min_max.ECW_min = 
		GetLog(mib_QStaEDCATable[2].QStaEDCATblCWmin);
	WMEParamElem.AC_VI.ECW_min_max.ECW_max = 
		GetLog(mib_QStaEDCATable[2].QStaEDCATblCWmax);
	WMEParamElem.AC_VI.TXOP_lim = mib_QStaEDCATable[2].QStaEDCATblTXOPLimit;
	//Update EDCA for VO
	WMEParamElem.AC_VO.ACI_AIFSN.AIFSN = mib_QStaEDCATable[3].QStaEDCATblAIFSN;
	WMEParamElem.AC_VO.ACI_AIFSN.ACI = 3;
	WMEParamElem.AC_VO.ACI_AIFSN.ACM = mib_QStaEDCATable[3].QStaEDCATblMandatory;
	WMEParamElem.AC_VO.ECW_min_max.ECW_min = 
		GetLog(mib_QStaEDCATable[3].QStaEDCATblCWmin);
	WMEParamElem.AC_VO.ECW_min_max.ECW_max = 
		GetLog(mib_QStaEDCATable[3].QStaEDCATblCWmax);
	WMEParamElem.AC_VO.TXOP_lim = mib_QStaEDCATable[3].QStaEDCATblTXOPLimit;

	if(*(mib->mib_ApMode) == AP_MODE_B_ONLY)

	{
		WMEParamElem.AC_BE.TXOP_lim = mib_QStaEDCATable[0].QStaEDCATblTXOPLimitBSta;
		WMEParamElem.AC_BK.TXOP_lim = mib_QStaEDCATable[1].QStaEDCATblTXOPLimitBSta;
		WMEParamElem.AC_VI.TXOP_lim = mib_QStaEDCATable[2].QStaEDCATblTXOPLimitBSta;
		WMEParamElem.AC_VO.TXOP_lim = mib_QStaEDCATable[3].QStaEDCATblTXOPLimitBSta;

	}
	else
	{
		WMEParamElem.AC_BE.TXOP_lim = mib_QStaEDCATable[0].QStaEDCATblTXOPLimit;
		WMEParamElem.AC_BK.TXOP_lim = mib_QStaEDCATable[1].QStaEDCATblTXOPLimit;
		WMEParamElem.AC_VI.TXOP_lim = mib_QStaEDCATable[2].QStaEDCATblTXOPLimit;
		WMEParamElem.AC_VO.TXOP_lim = mib_QStaEDCATable[3].QStaEDCATblTXOPLimit;


	}


}

UINT16 AddWMEParam_IE(WME_param_elem_t * pNextElement)
{
	memcpy( pNextElement, &WMEParamElem, sizeof(WME_param_elem_t) );
	return ( sizeof(WME_param_elem_t) );
}

UINT16 QoS_AppendWMEParamElem(vmacApInfo_t *vmacSta_p,UINT8 *pBcnBuf)
{
	MIB_802DOT11 *mib=vmacSta_p->Mib802dot11;
	MIB_STA_CFG  *mib_StaCfg_p=vmacSta_p->Mib802dot11->StationConfig;
	WME_param_elem_t *pWMEParamElem = (WME_param_elem_t *)pBcnBuf;
	//BcnWMEParamElemLocation_p = pBcnBuf;
	if (mib_StaCfg_p->WSMQoSOptImpl)
	{
		pWMEParamElem->ElementId =  221;//WSM_QOS_CAPABILITY;
		pWMEParamElem->Len = WME_PARAM_LEN;
		pWMEParamElem->OUI.OUI[0] = 0x0;
		pWMEParamElem->OUI.OUI[1] = 0x50;

		pWMEParamElem->OUI.OUI[2] = 0xf2;
		pWMEParamElem->OUI.Type = 2;
		pWMEParamElem->OUI.Subtype = 1;
		pWMEParamElem->version = 1;
#ifdef WMM_PS_SUPPORT
		pWMEParamElem->QoS_info.U_APSD = 1;
		pWMEParamElem->QoS_info.Reserved=0;
		pWMEParamElem->QoS_info.EDCA_param_set_update_cnt = EDCA_param_set_update_cnt;
#else


		pWMEParamElem->QoS_info.Q_ack = 0;
		pWMEParamElem->QoS_info.TXOP_req = 0; 
		pWMEParamElem->QoS_info.Q_req =  0 ; //PROCESS_TXOP_REQ;
#ifdef UAPSD_SUPPORT
		pWMEParamElem->QoS_info.more_data_ack = 1; //uapsd notification
#endif
		pWMEParamElem->QoS_info.EDCA_param_set_update_cnt = EDCA_param_set_update_cnt;
#endif
		pWMEParamElem->rsvd = 0;
		//Updae EDCA for BE
		pWMEParamElem->AC_BE.ACI_AIFSN.AIFSN = mib_QStaEDCATable[0].QStaEDCATblAIFSN;
		pWMEParamElem->AC_BE.ACI_AIFSN.ACI = 0;
		pWMEParamElem->AC_BE.ACI_AIFSN.ACM = mib_QStaEDCATable[0].QStaEDCATblMandatory;
		pWMEParamElem->AC_BE.ECW_min_max.ECW_min = 
			GetLog(mib_QStaEDCATable[0].QStaEDCATblCWmin);
		pWMEParamElem->AC_BE.ECW_min_max.ECW_max = 
			GetLog(mib_QStaEDCATable[0].QStaEDCATblCWmax);
		pWMEParamElem->AC_BE.TXOP_lim = mib_QStaEDCATable[0].QStaEDCATblTXOPLimit;
		//Update EDCA for BK
		pWMEParamElem->AC_BK.ACI_AIFSN.AIFSN = mib_QStaEDCATable[1].QStaEDCATblAIFSN;
		pWMEParamElem->AC_BK.ACI_AIFSN.ACI = 1;
		pWMEParamElem->AC_BK.ACI_AIFSN.ACM = mib_QStaEDCATable[1].QStaEDCATblMandatory;
		pWMEParamElem->AC_BK.ECW_min_max.ECW_min = 
			GetLog(mib_QStaEDCATable[1].QStaEDCATblCWmin);
		pWMEParamElem->AC_BK.ECW_min_max.ECW_max = 
			GetLog(mib_QStaEDCATable[1].QStaEDCATblCWmax);
		pWMEParamElem->AC_BK.TXOP_lim = mib_QStaEDCATable[1].QStaEDCATblTXOPLimit;
		//Update EDCA for VI
		pWMEParamElem->AC_VI.ACI_AIFSN.AIFSN = mib_QStaEDCATable[2].QStaEDCATblAIFSN;
		pWMEParamElem->AC_VI.ACI_AIFSN.ACI = 2;
		pWMEParamElem->AC_VI.ACI_AIFSN.ACM = mib_QStaEDCATable[2].QStaEDCATblMandatory;
		pWMEParamElem->AC_VI.ECW_min_max.ECW_min = 
			GetLog(mib_QStaEDCATable[2].QStaEDCATblCWmin);
		pWMEParamElem->AC_VI.ECW_min_max.ECW_max = 
			GetLog(mib_QStaEDCATable[2].QStaEDCATblCWmax);
		pWMEParamElem->AC_VI.TXOP_lim = mib_QStaEDCATable[2].QStaEDCATblTXOPLimit;
		//Update EDCA for VO
		pWMEParamElem->AC_VO.ACI_AIFSN.AIFSN = mib_QStaEDCATable[3].QStaEDCATblAIFSN;
		pWMEParamElem->AC_VO.ACI_AIFSN.ACI = 3;
		pWMEParamElem->AC_VO.ACI_AIFSN.ACM = mib_QStaEDCATable[3].QStaEDCATblMandatory;
		pWMEParamElem->AC_VO.ECW_min_max.ECW_min = 
			GetLog(mib_QStaEDCATable[3].QStaEDCATblCWmin);
		pWMEParamElem->AC_VO.ECW_min_max.ECW_max = 
			GetLog(mib_QStaEDCATable[3].QStaEDCATblCWmax);
		pWMEParamElem->AC_VO.TXOP_lim = mib_QStaEDCATable[3].QStaEDCATblTXOPLimit;

		if(*(mib->mib_ApMode) == AP_MODE_B_ONLY)

		{
			pWMEParamElem->AC_BE.TXOP_lim = mib_QStaEDCATable[0].QStaEDCATblTXOPLimitBSta;
			pWMEParamElem->AC_BK.TXOP_lim = mib_QStaEDCATable[1].QStaEDCATblTXOPLimitBSta;
			pWMEParamElem->AC_VI.TXOP_lim = mib_QStaEDCATable[2].QStaEDCATblTXOPLimitBSta;
			pWMEParamElem->AC_VO.TXOP_lim = mib_QStaEDCATable[3].QStaEDCATblTXOPLimitBSta;

		}
		else
		{
			pWMEParamElem->AC_BE.TXOP_lim = mib_QStaEDCATable[0].QStaEDCATblTXOPLimit;
			pWMEParamElem->AC_BK.TXOP_lim = mib_QStaEDCATable[1].QStaEDCATblTXOPLimit;
			pWMEParamElem->AC_VI.TXOP_lim = mib_QStaEDCATable[2].QStaEDCATblTXOPLimit;
			pWMEParamElem->AC_VO.TXOP_lim = mib_QStaEDCATable[3].QStaEDCATblTXOPLimit;


		}


		return(pWMEParamElem->Len + 2);
	}
	else
		return 0;
}
#endif//QOS_WSM_FEATURE

/******************************************************************************
* 
* Name: bcngen_UpdateQosCapElem
* 
* Description: Will append the Qos Capability Info Element to the Beacon.
*     
* 
* Conditions For Use: 
*    When the QoSOptImpl and the QBSSLoadOptImpl are true. 
* 
* Arguments: 
*    None. 
*                                 
* Return Value: 
*    The offset to the beacon buffer after the new information element is added. 
* 
* Notes: 
*    None. 
* 
* PDL: 
*
* END PDL 
* 
*****************************************************************************/
#ifndef QOS_WSM_FEATURE
UINT16 Qos_UpdateQosCapElem(vmacApInfo_t *vmacSta_p,UINT8 *pBcnBuf)
{
	MIB_STA_CFG  *mib_StaCfg_p=vmacSta_p->Mib802dot11->StationConfig;
	QoS_Cap_Elem_t *pQosCapElem = (QoS_Cap_Elem_t *)pBcnBuf;

	if (*(mib->QoSOptImpl))
	{
		pQosCapElem->ElementId =  QOS_CAPABILITY;
		pQosCapElem->Len = QOS_CAP;
		pQosCapElem->QoS_info.EDCA_param_set_update_cnt = EDCA_param_set_update_cnt;
		pQosCapElem->QoS_info.Q_ack = mib_StaCfg_p->QAckOptImpl;
		pQosCapElem->QoS_info.TXOP_req = PROCESS_TXOP_REQ; 
		pQosCapElem->QoS_info.Q_req = PROCESS_QUEUE_REQ; 
		return sizeof(QoS_Cap_Elem_t);
	}
	else
		return 0;
}
#endif


/******************************************************************************
* 
* Name: GetTspec 
* 
* Description: 
*   This routine returns the Tspec entry corresponding to the MAC Addr 
*   and tsid
* 
* Conditions For Use: 
*   There should be a Tspec entry for that MAC addr and tsid 
* 
* Arguments: 
*   MAC Addr, TS id 
* 
* Return Value: 
*   Pointer to the TSpec entry or NULL
* 
* Notes: 
*   None. 
* 
* PDL: 
* 
*****************************************************************************/
Mrvl_TSPEC_t * GetTspec(IEEEtypes_MacAddr_t *pAddr, UINT8 req_tsid)
{
	UINT32 i;
	for (i=0;i<MAX_QOS_STRMS;i++)
	{
		if (TSpecEntryDb[i].not_free &&
			!MACADDR_CMP(pAddr, TSpecEntryDb[i].src_addr) &&
			(TSpecEntryDb[i].TSpec.ts_info.tsid == req_tsid))
		{
			return &TSpecEntryDb[i].TSpec;
		}
	}
	return NULL;
}

/******************************************************************************
* 
* Name: GetTxOpFrmTspecIndx 
* 
* Description: 
*   This routine returns the TxOp entry corresponding to the TScpec Indx
* 
* Conditions For Use: 
*   There should be a Tspec entry for that MAC addr and tsid 
* 
* Arguments: 
*   The Tspec Index 
* 
* Return Value: 
*   Pointer to the TxOp entry or NULL
* 
* Notes: 
*   None. 
* 
* PDL: 
* 
*****************************************************************************/
TXOP_t * GetTxOpFrmTspecIndx(UINT32 Indx)
{
	UINT32 i;
	for (i=0;i<MAX_QOS_STRMS;i++)
	{
		if (TxOpDb[i].gTID == Indx)
		{
			return &TxOpDb[i];
		}
	}
	return NULL;
}


/******************************************************************************
* 
* Name: QoS_UpdateEDCAParameters 
* 
* Description: 
*   This routine will update the EDCA_param_set_update_cnt
* 
* Conditions For Use: 
*   If the scheduler or user decides to change the EDCA parameters. 
* 
* Arguments: 
*   None for now.  
* 
* Return Value:
* 1 if success. 0 if fail 
*   
* 
* Notes: 
*    
* Owner: Milind
*
* PDL:      * 
*****************************************************************************/
UINT32 QoS_UpdateEDCAParameters(vmacApInfo_t *vmacSta_p)
{    
	UINT8 *NextElementPtr=NULL;
	UINT32  totalLen;
	if(BcnWMEInfoElemLocation_p != NULL)
	{
		NextElementPtr = (UINT8 *) BcnWMEInfoElemLocation_p;
		totalLen = (UINT32)NextElementPtr - (UINT32)BcnBuffer_p -
			sizeof(IEEEtypes_MgmtHdr_t);
		if ((UINT32)(totalLen+WME_INFO_LEN+2) < BcnBuffer_p->Hdr.FrmBodyLen)
			return 0; //This is not the last element in the beacon

	}
	else if(BcnWMEParamElemLocation_p != NULL)
	{
		NextElementPtr = (UINT8 *) BcnWMEParamElemLocation_p;
		totalLen = (UINT32)NextElementPtr - (UINT32)BcnBuffer_p -
			sizeof(IEEEtypes_MgmtHdr_t);
		if ((UINT32)(totalLen+WME_PARAM_LEN + 2) < BcnBuffer_p->Hdr.FrmBodyLen)
			return 0; //This is not the last element in the beacon
	}

	EDCA_param_set_update_cnt++;
	BcnWMEParamElemLocation_p = NextElementPtr;
	BcnWMEInfoElemLocation_p = NULL;
	//Comment out for now.(09/17/04)
	NextElementPtr  +=QoS_AppendWMEParamElem(vmacSta_p,BcnWMEParamElemLocation_p);

	//Always append the WME Parameter Element to teh beacon.
	totalLen = (UINT32)NextElementPtr - (UINT32)BcnBuffer_p -
		sizeof(IEEEtypes_MgmtHdr_t);
	BcnBuffer_p->Hdr.FrmBodyLen = totalLen;
	//Also update the Probe Response
	//comment out for now.(09/17/04)
	QoS_AppendWMEParamElem(vmacSta_p,PrbWMEParamElemLocation_p);

	//Update the QoS Parameters for the AP.
	return 1;
}


/******************************************************************************
* 
* Name: QoS_ReAppendWMEInfoElem 
* 
* Description: 
*   This routine will remove the WME Capability Element 
*   and reappend the WME Info Element 
* 
* Conditions For Use: 
*   When the EDCA_Beacon_Counter becomes zero. 
* 
* Arguments: 
*   None for now..  
* 
* Return Value: 
*   
* 
* Notes: 
*   
*    
* Owner: Milind
*
* PDL:      * 
*****************************************************************************/
void QoS_ReAppendWMEInfoElem(vmacApInfo_t *vmacSta_p)
{
	UINT8 *NextElementPtr=NULL;
	UINT32  totalLen;
	if(BcnWMEParamElemLocation_p != NULL)
	{
		NextElementPtr = (UINT8 *) BcnWMEParamElemLocation_p;
		totalLen = (UINT32)NextElementPtr - (UINT32)BcnBuffer_p -
			sizeof(IEEEtypes_MgmtHdr_t);
		if ((UINT32)(totalLen+WME_PARAM_LEN + 2) < BcnBuffer_p->Hdr.FrmBodyLen)
			return; //This is not the last element in the beacon
	}
	else if(BcnWMEInfoElemLocation_p != NULL)
	{
		NextElementPtr = (UINT8 *) BcnWMEInfoElemLocation_p;
		totalLen = (UINT32)NextElementPtr - (UINT32)BcnBuffer_p -
			sizeof(IEEEtypes_MgmtHdr_t);
		if ((UINT32)(totalLen+WME_INFO_LEN+2) < BcnBuffer_p->Hdr.FrmBodyLen)
			return; //This is not the last element in the beacon
	}
	BcnWMEInfoElemLocation_p = NextElementPtr;
	BcnWMEParamElemLocation_p = NULL;
	NextElementPtr  +=QoS_AppendWMEInfoElem(vmacSta_p,NextElementPtr);
	totalLen = (UINT32)NextElementPtr - (UINT32)BcnBuffer_p -
		sizeof(IEEEtypes_MgmtHdr_t);
	BcnBuffer_p->Hdr.FrmBodyLen = totalLen;
	return;
}

UINT8 Qos_GetDSCPPriority(UINT8 *Databufptr)
{
	IEEE802_1QTag_t *IEEE802_1QTag_p = NULL;
	IEEEtypes_8023_Frame_t  *DataEtherFrame_p;
	IEEEtypes_IPv4_Hdr_t *IPv4_p = NULL;
	llc_snap_hdr_t *llc_p;
	UINT16 typelen, type,TagControl;
	UINT32 AC_prio,Pri;

	AC_prio = 0;

	DataEtherFrame_p = (IEEEtypes_8023_Frame_t *)(Databufptr);
	type = SHORT_SWAP(DataEtherFrame_p->Hdr.type);
	//while loop to look at the rules of teh classifier. 


	//First check for 802.1D tag.
	if (type == IEEE802_11Q_TYPE)
	{
		IEEE802_1QTag_p= (IEEE802_1QTag_t *)((UINT8 *)DataEtherFrame_p + sizeof(ether_hdr_t)
			-  sizeof(UINT16));
		TagControl = SHORT_SWAP(IEEE802_1QTag_p->Control);
		Pri = (TagControl & 0xE000)>>13;
		switch (Pri)
		{
		case 0: 
		case 1:
		case 2:
		case 4:
		case 5:
		case 6:
		case 7:
			AC_prio = Pri;
			break;
		default:
			AC_prio = 0;
			break;
		}
	}
	else
	{
		//check for IP Tag.
		typelen = SHORT_SWAP(DataEtherFrame_p->Hdr.type);

		if (typelen <= MAX_ETHER_PKT_SIZE)
		{
			//pkt has LLC Header
			llc_p =  (llc_snap_hdr_t *)((UINT8 *)DataEtherFrame_p + 
				sizeof(ether_hdr_t));
			type =   SHORT_SWAP(llc_p->Type);
			IPv4_p = (IEEEtypes_IPv4_Hdr_t *)((UINT8 *)DataEtherFrame_p + 
				sizeof(ether_hdr_t) +
				sizeof(llc_snap_hdr_t));

		}
		else
		{
			type  =  SHORT_SWAP(DataEtherFrame_p->Hdr.type);
			IPv4_p = (IEEEtypes_IPv4_Hdr_t *)((UINT8 *)DataEtherFrame_p+ sizeof(ether_hdr_t));
		}
		if (type == 0x800)//IP Pkt
		{
			Pri = (IPv4_p->tos & 0xE0)>>5;
			switch (Pri)
			{
			case 0:
			case 1:
			case 2:
			case 3:
			case 4:
			case 5:
			case 6:
			case 7:
				AC_prio = Pri;
				break;
			default:
				AC_prio = 0;
				break;
			}
		}
		else
		{
			AC_prio = 0;
		}
	}

	//no TCLAS match. So return -1
	return AC_prio;
}

/******************************************************************************
* 
* Name: Qos_UpdateDELTSCounter
* 
* Description: Will reset the counter of a TS on receipt of it's pkt.
*     
* 
* Conditions For Use: 
*    When the QoSOptImpl are true. 
* 
* Arguments: 
*    None. 
*                                 
* Return Value: 
*    . 
* 
* Notes: 
*    None. 
* 
* PDL: 
*
* END PDL 
* 
*****************************************************************************/

#endif

