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
* File: AssocSta_srv.c
*
*        Client Association Service Function Calls
* Description:  Implementation of the Client MLME Association Services
*
*******************************************************************************************/
#include "wltypes.h"
#include "IEEE_types.h"
#ifdef STA_QOS
#include "qos.h"
#endif
#include "mlmeSta.h"
#include "mlmeApi.h"
#include "wl_mib.h"
#include "wl_hal.h"
#include "wlvmac.h"
#include "StaDb.h"

#include "domain.h"



//=============================================================================
//                         IMPORTED PUBLIC VARIABLES
//=============================================================================
extern SINT32 mlmeApiInitKeyMgmt( vmacStaInfo_t *vStaInfo_p);


/*************************************************************************
* Function: assocSrv_SndAssocCnfm
*
* Description: Send Association Confirmation to SME
*
* Input:
*
* Output:
*
**************************************************************************/
UINT8 gDebug_sendSmeMsgFail[8] = {0,0,0,0,0,0,0,0};
extern UINT32 vht_cap;

static void assocSrv_SndAssocCnfm(vmacStaInfo_t *vStaInfo_p, 
								  UINT16 assocResult)
{
	IEEEtypes_AssocCfrm_t AssocCfrm;

	AssocCfrm.Result   = assocResult;
	if (mlmeApiSndNotification(vStaInfo_p, (UINT8 *)&AssocCfrm, MlmeAssoc_Cnfm) == MLME_FAILURE)
	{
		gDebug_sendSmeMsgFail[assocResult] ++;
	}
}

/*************************************************************************
* Function: assocSrv_SndReAssocCnfm
*
* Description: Send ReAssociation Confirmation to SME
*
* Input:
*
* Output:
*
**************************************************************************/
static void assocSrv_SndReAssocCnfm(vmacStaInfo_t *vStaInfo_p,
									UINT16 reAssocResult)
{
	IEEEtypes_ReassocCfrm_t ReAssocCfrm;

	ReAssocCfrm.Result   = reAssocResult;
	mlmeApiSndNotification(vStaInfo_p, (UINT8 *)&ReAssocCfrm, MlmeReAssoc_Cnfm);
}

/*************************************************************************
* Function: assocSrv_SndDisAssocCnfm
*
* Description: Send DisAssociation Confirmation to SME
*
* Input:
*
* Output:
*
**************************************************************************/
static void assocSrv_SndDisAssocCnfm(vmacStaInfo_t *vStaInfo_p,
									 UINT16 disAssocResult)
{
	IEEEtypes_DisassocCfrm_t DisAssocCfrm;

	DisAssocCfrm.Result   = disAssocResult;
	mlmeApiSndNotification(vStaInfo_p, (UINT8 *)&DisAssocCfrm, MlmeDisAssoc_Cnfm);
}

/*************************************************************************
* Function: assocSrv_SndDisAssocInd
*
* Description: Send DisAssociation Indication to SME
*
* Input:
*
* Output:
*
**************************************************************************/
static void assocSrv_SndDisAssocInd(vmacStaInfo_t *vStaInfo_p,
									UINT16 disAssocResult,
									UINT8 *disAssocPeerAddr )
{
	IEEEtypes_DisassocInd_t DisAssocInd;

	DisAssocInd.Reason = disAssocResult;
	memcpy(&DisAssocInd.PeerStaAddr, disAssocPeerAddr, sizeof(IEEEtypes_MacAddr_t));
	mlmeApiSndNotification(vStaInfo_p, (UINT8 *)&DisAssocInd, MlmeDisAssoc_Ind);
}

/*************************************************************************
* Function: assocSrv_AssocActTimeOut
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern SINT32 assocSrv_AssocActTimeOut(UINT8 *data)
{
	vmacStaInfo_t *vStaInfo_p = (vmacStaInfo_t *)data;

#ifdef ETH_DEBUG
	eprintf("assocSrv_AssocActTimeOut:: ****** Association Time Out \n");
#endif /* ETH_DEBUG */

	/* Notify SME of Time Out */
	assocSrv_SndAssocCnfm(vStaInfo_p, ASSOC_RESULT_TIMEOUT);
	if (vStaInfo_p->macMgmtMain_State == STATE_ASSOCIATING)
	{
		vStaInfo_p->macMgmtMain_State = STATE_AUTHENTICATED_WITH_AP;

	}
	/* L2 Event Notification */
	mlmeApiEventNotification(vStaInfo_p,
		MlmeAssoc_Cnfm,
		(UINT8 *)&vStaInfo_p->macMgmtMlme_ThisStaData.BssId, 
		ETH_EVT_JOIN_TIMEOUT);
	return 0;
}

/*************************************************************************
* Function: assocSrv_DisAssocActTimeOut
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern SINT32 assocSrv_DisAssocActTimeOut(vmacStaInfo_t *vStaInfo_p,
										  UINT8 *peerAddr)
{
	/* Notify SME of Time Out */
	assocSrv_SndDisAssocCnfm(vStaInfo_p, DISASSOC_RESULT_TIMEOUT);
	vStaInfo_p->macMgmtMain_State = STATE_AUTHENTICATED_WITH_AP;
	return 0;
}

/*************************************************************************
* Function: assocSrv_ReAssocActTimeOut
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern SINT32 assocSrv_ReAssocActTimeOut(vmacStaInfo_t *vStaInfo_p,
										 IEEEtypes_MacAddr_t *peerAddr)
{
	/* Notify SME of Time Out */
	assocSrv_SndReAssocCnfm(vStaInfo_p, REASSOC_RESULT_TIMEOUT);
	if (vStaInfo_p->macMgmtMain_State == STATE_REASSOCIATING)
	{
		vStaInfo_p->macMgmtMain_State = STATE_ASSOCIATED;
	}
	return 0;
}

/*************************************************************************
* Function: assocSrv_ClearAssocInfo
*
* Description: Clear informations related to the current association
*
* Input:
*
* Output:
*
**************************************************************************/
static void assocSrv_ClearAssocInfo(vmacStaInfo_t *vStaInfo_p)
{
	mlmeApiSetAIdToMac(vStaInfo_p, 0);
}

/*************************************************************************
* Function: assocSrv_AssocCmd
*
* Description: Perform an Association Process with AP
*
* Input:
*
* Output:
*
**************************************************************************/
extern SINT32 assocSrv_AssocCmd( vmacStaInfo_t *vStaInfo_p, 
								IEEEtypes_AssocCmd_t *AssocCmd_p )
{
	dot11MgtFrame_t *mgtFrame_p;
	UINT8 supportedRateLen = 0;
	UINT8 ssidLen = 0;
	UINT8 extendedRateLen = 0;
	vmacEntry_t  *vmacEntry_p;
	UINT8 i;
	IEEEtypes_InfoElementHdr_t *ieHdr_p;
	UINT32 addedVendorLen;
	struct net_device *clientDev_p;				
	struct wlprivate	*priv_p;				
	MIB_802DOT11 *mib;							
#ifdef IEEE80211_DH
	BOOLEAN isSpectrumMgmt = FALSE ;
	UINT8 j ;
	int maxPwr = 0 ;
	UINT8 *ChannelList = NULL ;
	IEEEtypes_PowerCapabilityElement_t *pwrcap = NULL ;
	DomainCountryInfo *pInfo = NULL ;
	IEEEtypes_SupportedChannelElement_t *supp_chan = NULL ;
	UINT8 *supp = NULL ;
#endif //IEEE80211_DH

#ifdef ETH_DEBUG
	eprintf("assocSrv_AssocCmd:: Entered\n");
#endif /* ETH_DEBUG */

	vmacEntry_p = (vmacEntry_t *)vStaInfo_p->vMacEntry_p;
	clientDev_p = mainNetdev_p[vmacEntry_p->phyHwMacIndx];		
	priv_p = NETDEV_PRIV_P(struct wlprivate, clientDev_p);		
	mib = priv_p->vmacSta_p->Mib802dot11;						

	if (vStaInfo_p->macMgmtMain_State != STATE_AUTHENTICATED_WITH_AP)
	{
		assocSrv_SndAssocCnfm(vStaInfo_p, ASSOC_RESULT_REFUSED);
		return MLME_FAILURE;   
	}
	/* Build mgt frame */
	if((mgtFrame_p = mlmeApiAllocMgtMsg(vmacEntry_p->phyHwMacIndx)) == NULL)
	{
		/* Notify SME of Assoc failure */
		assocSrv_SndAssocCnfm(vStaInfo_p, ASSOC_RESULT_INVALID_PARAMETERS);
		return MLME_FAILURE;
	}
	mlmePrepDefaultMgtMsg_Sta(vStaInfo_p,
		mgtFrame_p, 
		&AssocCmd_p->PeerStaAddr, 
		IEEE_MSG_ASSOCIATE_RQST,
		&(vStaInfo_p->macMgmtMlme_ThisStaData.BssId));
	/* Fill out the MAC body */
	mgtFrame_p->Hdr.FrmBodyLen = 0;
	/* Add Capability Info */
	mgtFrame_p->Body.AssocRqst.CapInfo = AssocCmd_p->CapInfo;
	if(vStaInfo_p->macMgt_StaMode == CLIENT_MODE_B)
	{
		mgtFrame_p->Body.AssocRqst.CapInfo.ShortSlotTime = 0;
	}
	mgtFrame_p->Hdr.FrmBodyLen += sizeof(IEEEtypes_CapInfo_t);
	/* Add Listen Interval */
	mgtFrame_p->Body.AssocRqst.ListenInterval = AssocCmd_p->ListenInterval;
	mgtFrame_p->Hdr.FrmBodyLen += sizeof(IEEEtypes_ListenInterval_t);
	/* Add SSID Attrib */
	ssidLen = util_ListLen(&AssocCmd_p->SsId[0], IEEEtypes_SSID_SIZE);
	syncSrv_AddAttrib(mgtFrame_p, 
		SSID,
		&AssocCmd_p->SsId[0],
		ssidLen);
	supportedRateLen = util_ListLen(&vStaInfo_p->bssDescProfile_p->DataRates[0],
		IEEEtypes_MAX_DATA_RATES_G);
	if(supportedRateLen > MLME_SUPPORT_RATE_IE_MAX)
	{
		extendedRateLen= supportedRateLen - MLME_SUPPORT_RATE_IE_MAX;
		supportedRateLen = MLME_SUPPORT_RATE_IE_MAX;
	}
	/* Supported Rate IE */
	if(supportedRateLen)
	{
		syncSrv_AddAttrib(mgtFrame_p, 
			SUPPORTED_RATES,
			&vStaInfo_p->bssDescProfile_p->DataRates[0],
			supportedRateLen);
	}
#ifdef IEEE80211_DH
	/* If in 5 GHz channel and spectrum management bit is set 
	* in the capabilities field, make sure to include the power 
	* capability and supported channel IEs in the association requrest
	*/
	if( domainChannelValid( vStaInfo_p->JoinChannel, FREQ_BAND_5GHZ) )
	{
		isSpectrumMgmt = (mgtFrame_p->Body.AssocRqst.CapInfo.SpectrumMgmt)?TRUE:FALSE;
		if( isSpectrumMgmt )
		{
			ChannelList = kmalloc( IEEE_80211_MAX_NUMBER_OF_CHANNELS, GFP_KERNEL);
			if( ChannelList == NULL )
			{
				return MLME_FAILURE;   
			}
			pwrcap = kmalloc( sizeof(IEEEtypes_PowerCapabilityElement_t), GFP_KERNEL);
			if( pwrcap == NULL )
			{
				return MLME_FAILURE;   
			}
			pInfo = kmalloc( sizeof(DomainCountryInfo), GFP_KERNEL );
			if( pInfo == NULL )
			{
				return MLME_FAILURE;   
			}
			memset( pInfo, 0, sizeof(DomainCountryInfo));
			domainGetPowerInfo((UINT8 *)pInfo);
			for( i = 0 ; i < pInfo->AChannelLen ; i ++ )
			{
				if(  vStaInfo_p->JoinChannel >= pInfo->DomainEntryA[i].FirstChannelNo && 
					vStaInfo_p->JoinChannel < pInfo->DomainEntryA[i].FirstChannelNo + 
					pInfo->DomainEntryA[i].NoofChannel )
				{
					maxPwr = pInfo->DomainEntryA[i].MaxTransmitPw ;
					break ;
				}
			}
			kfree(pInfo);
			pwrcap->ElementId = PWR_CAP ;
			pwrcap->Len = 2 ;
			pwrcap->MaxTxPwr = maxPwr ? maxPwr : 18 ;
			pwrcap->MinTxPwr = 5 ;
			syncSrv_AddAttrib(mgtFrame_p, 
				PWR_CAP,
				&pwrcap->MaxTxPwr,
				2);
			kfree(pwrcap);
			if( domainGetInfo( ChannelList ) )
			{
				supp_chan = kmalloc( sizeof(IEEEtypes_SupportedChannelElement_t), GFP_KERNEL);
				if( supp_chan == NULL )
				{
					return MLME_FAILURE;   
				}
				supp_chan->ElementId = SUPPORTED_CHANNEL ;
				supp = (UINT8 *)&supp_chan->SupportedChannel[0] ;
				for( i = 0 , j = 0; i < IEEE_80211_MAX_NUMBER_OF_CHANNELS; i ++ )
				{
					if( ChannelList[i] != 0 && 
						domainChannelValid( ChannelList[i], FREQ_BAND_5GHZ))
					{
						*supp = ChannelList[i] ;
						supp ++ ;
						*supp = 1 ;
						supp ++ ;
						j += 2 ;
					}
				}
				supp_chan->Len = j ;
				syncSrv_AddAttrib(mgtFrame_p, 
					SUPPORTED_CHANNEL,
					(UINT8 *)&supp_chan->SupportedChannel[0],
					j);
				kfree(supp_chan);
			}
			kfree( ChannelList );
		}
	}
#endif // IEEE80211_DH

	/* HT IE */
	if ((*(mib->mib_ApMode)== AP_MODE_N_ONLY
			|| *(mib->mib_ApMode) == AP_MODE_BandN
			|| *(mib->mib_ApMode) == AP_MODE_GandN
			|| *(mib->mib_ApMode) == AP_MODE_BandGandN
			|| *(mib->mib_ApMode) == AP_MODE_5GHZ_N_ONLY
#ifdef SOC_W8864
			|| *(mib->mib_ApMode) == AP_MODE_2_4GHZ_11AC_MIXED
			|| *(mib->mib_ApMode) == AP_MODE_5GHZ_Nand11AC
			|| *(mib->mib_ApMode) == AP_MODE_5GHZ_11AC_ONLY
#endif		
			|| *(mib->mib_ApMode) == AP_MODE_AandN))
	{
		if(vStaInfo_p->bssDescProfile_p->HTElement.Len)
		{
#ifdef SOC_W8864	
        	/** Set ldpc coding to 1 for 8864 and newer chips **/
        	vStaInfo_p->bssDescProfile_p->HTElement.HTCapabilitiesInfo.AdvCoding=1;
#else
        	/** always fixed ldpc coding to 0, we do not support ldpc in any of our current chip **/
        	vStaInfo_p->bssDescProfile_p->HTElement.HTCapabilitiesInfo.AdvCoding=0;
#endif
			syncSrv_AddAttrib(mgtFrame_p,
				vStaInfo_p->bssDescProfile_p->HTElement.ElementId,
				(UINT8 *)&vStaInfo_p->bssDescProfile_p->HTElement.HTCapabilitiesInfo,
				vStaInfo_p->bssDescProfile_p->HTElement.Len);
		}
	}	
	
	/* RSN IE (WPA2) */
	if(vStaInfo_p->bssDescProfile_p->Wpa2Element.Len)
	{
		syncSrv_AddAttrib(mgtFrame_p,
			vStaInfo_p->bssDescProfile_p->Wpa2Element.ElemId,
			(UINT8 *)&vStaInfo_p->bssDescProfile_p->Wpa2Element.Ver[0],
			vStaInfo_p->bssDescProfile_p->Wpa2Element.Len);
	}
	/* Extended Supported Rate IE */
	if(extendedRateLen)
	{
		syncSrv_AddAttrib(mgtFrame_p,
			EXT_SUPPORTED_RATES,
			&vStaInfo_p->bssDescProfile_p->DataRates[MLME_SUPPORT_RATE_IE_MAX],
			extendedRateLen);
	}
#ifdef WPA_STA
	if (vStaInfo_p->staSecurityMibs.mib_PrivacyTable_p->RSNEnabled)
	{
		// For WPA/WPA2, Privacy bit in association request frame should be set to
		// zero
		mgtFrame_p->Body.AssocRqst.CapInfo.Privacy = 0;
		mlmeApiInitKeyMgmt(vStaInfo_p);
	}
#endif /* WPA_STA */
	/* Add in Vendor Specific IEs */
	if(vStaInfo_p->bssDescProfile_p->vendorIENum && vStaInfo_p->bssDescProfile_p->vendorTotalLen)
	{
		addedVendorLen = 0;
		for(i=0; i < vStaInfo_p->bssDescProfile_p->vendorIENum; i++)
		{
			ieHdr_p = (IEEEtypes_InfoElementHdr_t *)&vStaInfo_p->bssDescProfile_p->vendorBuf[addedVendorLen];
			if(ieHdr_p->Len)
			{
				syncSrv_AddAttrib(mgtFrame_p, 
					ieHdr_p->ElementId, 
					(&vStaInfo_p->bssDescProfile_p->vendorBuf[addedVendorLen] + sizeof(IEEEtypes_InfoElementHdr_t)),
					ieHdr_p->Len);
			}
			addedVendorLen += ieHdr_p->Len + sizeof(IEEEtypes_InfoElementHdr_t);
		}
	}
#ifdef SOC_W8864
	if ((*(mib->mib_ApMode) == AP_MODE_2_4GHZ_11AC_MIXED
			|| *(mib->mib_ApMode) == AP_MODE_5GHZ_Nand11AC
			|| *(mib->mib_ApMode) == AP_MODE_5GHZ_11AC_ONLY))
	{   /* add IE191 to Assoc REQ*/
        IEEEtypes_VhtCap_t ptr;      
		memcpy((UINT8 *)&ptr.cap, &vht_cap, sizeof(IEEEtypes_VHT_Cap_Info_t));	
        ptr.SupportedRxMcsSet = 0xffea;
        ptr.SupportedTxMcsSet = 0xffea;
        syncSrv_AddAttrib(mgtFrame_p,
            191,
            (UINT8 *)&ptr.cap,
            12);
    }   
#endif
	/* Send mgt frame */
	if (mlmeApiSendMgtMsg_Sta(vStaInfo_p, mgtFrame_p, NULL) == MLME_FAILURE)
	{
		/* Notify SME of Assoc failure */
		assocSrv_SndAssocCnfm(vStaInfo_p, ASSOC_RESULT_REFUSED);
		return MLME_FAILURE;
	}
	/* Start Assoc Timer */           
	mlmeApiStartTimer(vStaInfo_p, 
		(UINT8 *)&vStaInfo_p->assocTimer,
		&assocSrv_AssocActTimeOut,
		ASSOC_TIME);
	vStaInfo_p->macMgmtMain_State = STATE_ASSOCIATING;
	return MLME_SUCCESS;
}

/*************************************************************************
* Function: assocSrv_ReAssocCmd
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern SINT32 assocSrv_ReAssocCmd( vmacStaInfo_t *vStaInfo_p,
								  IEEEtypes_ReassocCmd_t *ReassocCmd_p )
{
	dot11MgtFrame_t *mgtFrame_p;
	vmacEntry_t  *vmacEntry_p;
	UINT8 ssidLen = 0;
	UINT8 supportedRateLen = 0;
	UINT8 extendedRateLen = 0;
#ifdef IEEE80211_DH
	BOOLEAN isSpectrumMgmt = FALSE ;
	UINT8 i, j ;
	int maxPwr = 0 ;
	UINT8 *ChannelList = NULL ;
	IEEEtypes_PowerCapabilityElement_t *pwrcap = NULL ;
	DomainCountryInfo *pInfo = NULL ;
	IEEEtypes_SupportedChannelElement_t *supp_chan = NULL ;
	UINT8 *supp = NULL ;
#endif //IEEE80211_DH

	vmacEntry_p = (vmacEntry_t *)vStaInfo_p->vMacEntry_p;

	if (vStaInfo_p->macMgmtMain_State != STATE_ASSOCIATED)
	{
		assocSrv_SndReAssocCnfm(vStaInfo_p, REASSOC_RESULT_REFUSED);
		return MLME_FAILURE;
	}


	/* Build mgt frame */
	if((mgtFrame_p = mlmeApiAllocMgtMsg(vmacEntry_p->phyHwMacIndx)) == NULL)
	{
		return MLME_FAILURE;
	}

	mlmePrepDefaultMgtMsg_Sta(vStaInfo_p,
		mgtFrame_p, 
		&ReassocCmd_p->NewApAddr, 
		IEEE_MSG_REASSOCIATE_RQST,
		&(vStaInfo_p->macMgmtMlme_ThisStaData.BssId));
	memset(&(mgtFrame_p->Body), 0, sizeof(mgtFrame_p->Body));
	mgtFrame_p->Hdr.FrmBodyLen = 0;
	/* Add Capability Info */
	mgtFrame_p->Body.ReassocRqst.CapInfo = ReassocCmd_p->CapInfo;
	mgtFrame_p->Hdr.FrmBodyLen += sizeof(IEEEtypes_CapInfo_t);
	/* Add Listen Interval */
	mgtFrame_p->Body.ReassocRqst.ListenInterval = ReassocCmd_p->ListenInterval;
	mgtFrame_p->Hdr.FrmBodyLen += sizeof(IEEEtypes_ListenInterval_t);
	/* Add AP Address */
	memcpy(&mgtFrame_p->Body.ReassocRqst.CurrentApAddr, vStaInfo_p->macMgmtMlme_ThisStaData.BssId, sizeof(IEEEtypes_MacAddr_t));
	mgtFrame_p->Hdr.FrmBodyLen += sizeof(IEEEtypes_MacAddr_t);
	/* Add SSID Attrib */
	ssidLen = util_ListLen(&ReassocCmd_p->SsId[0], IEEEtypes_SSID_SIZE);
	syncSrv_AddAttrib(mgtFrame_p, 
		SSID,
		&ReassocCmd_p->SsId[0],
		ssidLen);
	/* Add Support Rates Attrib */
	supportedRateLen = util_ListLen(&vStaInfo_p->bssDescProfile_p->DataRates[0],
		IEEEtypes_MAX_DATA_RATES_G);

	if(supportedRateLen > MLME_SUPPORT_RATE_IE_MAX)
	{
		extendedRateLen= supportedRateLen - MLME_SUPPORT_RATE_IE_MAX;
		supportedRateLen = MLME_SUPPORT_RATE_IE_MAX;
	}
	if(supportedRateLen)
	{
		syncSrv_AddAttrib(mgtFrame_p, 
			SUPPORTED_RATES,
			&vStaInfo_p->bssDescProfile_p->DataRates[0],
			supportedRateLen);
	}
#ifdef IEEE80211_DH
	/* If in 5 GHz channel and spectrum management bit is set 
	* in the capabilities field, make sure to include the power 
	* capability and supported channel IEs in the association requrest
	*/
	if( domainChannelValid( vStaInfo_p->JoinChannel, FREQ_BAND_5GHZ) )
	{
		isSpectrumMgmt = (mgtFrame_p->Body.AssocRqst.CapInfo.SpectrumMgmt)?TRUE:FALSE;
		if( isSpectrumMgmt )
		{
			ChannelList = kmalloc( IEEE_80211_MAX_NUMBER_OF_CHANNELS, GFP_KERNEL);
			if( ChannelList == NULL )
			{
				return MLME_FAILURE;   
			}
			pwrcap = kmalloc( sizeof(IEEEtypes_PowerCapabilityElement_t), GFP_KERNEL);
			if( pwrcap == NULL )
			{
				return MLME_FAILURE;   
			}
			pInfo = kmalloc( sizeof(DomainCountryInfo), GFP_KERNEL );
			if( pInfo == NULL )
			{
				return MLME_FAILURE;   
			}
			memset( pInfo, 0, sizeof(DomainCountryInfo));
			domainGetPowerInfo((UINT8 *)pInfo);
			for( i = 0 ; i < pInfo->AChannelLen ; i ++ )
			{
				if(  vStaInfo_p->JoinChannel >= pInfo->DomainEntryA[i].FirstChannelNo && 
					vStaInfo_p->JoinChannel < pInfo->DomainEntryA[i].FirstChannelNo + 
					pInfo->DomainEntryA[i].NoofChannel )
				{
					maxPwr = pInfo->DomainEntryA[i].MaxTransmitPw ;
					break ;
				}
			}
			kfree(pInfo);
			pwrcap->ElementId = PWR_CAP ;
			pwrcap->Len = 2 ;
			pwrcap->MaxTxPwr = maxPwr ? maxPwr : 18 ;
			pwrcap->MinTxPwr = 5 ;
			syncSrv_AddAttrib(mgtFrame_p, 
				PWR_CAP,
				&pwrcap->MaxTxPwr,
				2);
			kfree(pwrcap);
			if( domainGetInfo( ChannelList ) )
			{
				supp_chan = kmalloc( sizeof(IEEEtypes_SupportedChannelElement_t), GFP_KERNEL);
				if( supp_chan == NULL )
				{
					return MLME_FAILURE;   
				}
				supp_chan->ElementId = SUPPORTED_CHANNEL ;
				supp = (UINT8 *)&supp_chan->SupportedChannel[0] ;
				for( i = 0 , j = 0; i < IEEE_80211_MAX_NUMBER_OF_CHANNELS; i ++ )
				{
					if( ChannelList[i] != 0 && 
						domainChannelValid( ChannelList[i], FREQ_BAND_5GHZ))
					{
						*supp = ChannelList[i] ;
						supp ++ ;
						*supp = 1 ;
						supp ++ ;
						j += 2 ;
					}
				}
				supp_chan->Len = j ;
				syncSrv_AddAttrib(mgtFrame_p, 
					SUPPORTED_CHANNEL,
					(UINT8 *)&supp_chan->SupportedChannel[0],
					j);
				kfree(supp_chan);
			}
			kfree( ChannelList );
		}
	}
#endif // IEEE80211_DH
	if(extendedRateLen)
	{
		syncSrv_AddAttrib(mgtFrame_p,
			EXT_SUPPORTED_RATES,
			&vStaInfo_p->bssDescProfile_p->DataRates[MLME_SUPPORT_RATE_IE_MAX],
			extendedRateLen);
	}

#ifdef WPA_STA
	if (vStaInfo_p->staSecurityMibs.mib_PrivacyTable_p->RSNEnabled)
	{
		((KeyData_t *)vStaInfo_p->keyMgmtInfoSta_p->pKeyData)->RSNDataTrafficEnabled = 0;
		syncSrv_AddAttrib(mgtFrame_p, RSN_IE, "\x00\x10\x18\x01\x00", 3);/*add dummy params for interop issues*/
		syncSrv_AddAttrib(mgtFrame_p, RSN_IE, &vStaInfo_p->thisStaRsnIE_p->OuiType[0],
			vStaInfo_p->thisStaRsnIE_p->Len);
	}
#endif /* WPA_STA */
#ifdef STA_QOS
	if (vStaInfo_p->staSystemMibs.mib_StaCfg_p->QoSOptImpl)
	{  

		syncSrv_AddAttrib(mgtFrame_p, QOS_CAPABILITY, &vStaInfo_p->thisStaQoSCapElem_p->QoS_info[0],
			vStaInfo_p->thisStaQoSCapElem_p->Len);
#ifdef QOS_WSM_FEATURE
		if (vStaInfo_p->staSystemMibs.mib_StaCfg_p->WSMQoSOptImpl) 
		{
			syncSrv_AddAttrib(mgtFrame_p, PROPRIETARY_IE, &gThisStaWSMQoSCapElem.OUI[0],
				gThisStaWSMQoSCapElem.Len);
			//Add a WME Info Elem here as well.
			syncSrv_AddAttrib(mgtFrame_p, PROPRIETARY_IE, &gThisStaWMEQoSCapElem.OUI[0],
				gThisStaWMEQoSCapElem.Len);
		}
#endif /* QOS_WSM_FEATURE */
	}
#endif /* STA_QOS */
	/* Transmit Mgt Frame */
	if (mlmeApiSendMgtMsg_Sta(vStaInfo_p, mgtFrame_p, NULL) == MLME_FAILURE)
	{
		/* Notify SME of Failure */
		assocSrv_SndReAssocCnfm(vStaInfo_p, REASSOC_RESULT_REFUSED);
		return MLME_FAILURE;
	}
	vStaInfo_p->macMgmtMain_State = STATE_REASSOCIATING;
	return MLME_SUCCESS;
}

/*************************************************************************
* Function: assocSrv_DisAssocCmd
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern SINT32 assocSrv_DisAssocCmd(vmacStaInfo_t *vStaInfo_p,
								   IEEEtypes_DisassocCmd_t *DisassocCmd_p )
{
	dot11MgtFrame_t *mgtFrame_p;
	vmacEntry_t  *vmacEntry_p;

	vmacEntry_p = (vmacEntry_t *)vStaInfo_p->vMacEntry_p;
	/* Are we Associated first of all? */
	if (vStaInfo_p->macMgmtMain_State != STATE_ASSOCIATED)
	{
		assocSrv_SndDisAssocCnfm(vStaInfo_p, DISASSOC_RESULT_REFUSED);
		return MLME_FAILURE;
	}

	/* Build mgt frame */
	if((mgtFrame_p = mlmeApiAllocMgtMsg(vmacEntry_p->phyHwMacIndx)) == NULL)
	{
		return MLME_FAILURE;
	}

	mlmePrepDefaultMgtMsg_Sta(vStaInfo_p,
		mgtFrame_p, 
		&DisassocCmd_p->PeerStaAddr, 
		IEEE_MSG_DISASSOCIATE,
		&(vStaInfo_p->macMgmtMlme_ThisStaData.BssId));
	mgtFrame_p->Hdr.FrmBodyLen = 0;
	/* Add Reason Code */
	mgtFrame_p->Body.DisAssoc.ReasonCode = DisassocCmd_p->Reason;
	mgtFrame_p->Hdr.FrmBodyLen += sizeof(IEEEtypes_DisAssoc_t);
	/* Transmit Mgt Frame */
	if (mlmeApiSendMgtMsg_Sta(vStaInfo_p, mgtFrame_p, NULL) == MLME_FAILURE)
	{
		/* Notify SME of Failure */
		assocSrv_SndDisAssocCnfm(vStaInfo_p, DISASSOC_RESULT_REFUSED);
	}
	/* L2 Event Notification */
	mlmeApiEventNotification(vStaInfo_p,
		MlmeDisAssoc_Req,
		&DisassocCmd_p->PeerStaAddr[0], 
		DisassocCmd_p->Reason);
	vStaInfo_p->macMgmtMain_State = STATE_AUTHENTICATED_WITH_AP;
	/* Clear out data related to association with this AP */
	assocSrv_ClearAssocInfo(vStaInfo_p);
	/* Notify SME of Success */
	assocSrv_SndDisAssocCnfm(vStaInfo_p, DISASSOC_RESULT_SUCCESS);
	/*Milind. 09/29/05*/
	/*Free the AssocTable data structure that has been currently assigned to this*/
	/*peer station to which the WB was associated/joined*/
	mlmeApiFreePeerStationStaInfoAndAid(&(DisassocCmd_p->PeerStaAddr), vmacEntry_p);
	return MLME_SUCCESS;
} 

/*************************************************************************
* Function: assocSrv_RecvAssocRsp
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern SINT32 assocSrv_RecvAssocRsp( vmacStaInfo_t *vStaInfo_p,
									dot11MgtFrame_t *MgmtMsg_p )
{
	//    UINT8 *attrib_p;
#ifdef STA_QOS
#ifdef QOS_WSM_FEATURE
    MhsmEvent_t TsMsg;
	WME_param_elem_t *WME_param_elem = NULL;;
#endif /* QOS_WSM_FEATURE */
#endif /* STA_QOS */
	vmacEntry_t  *vmacEntry_p;

	vmacEntry_p = (vmacEntry_t *)vStaInfo_p->vMacEntry_p;


#ifdef ETH_DEBUG
	eprintf("assocSrv_RecvMsgRsp:: Entered\n");
#endif /* ETH_DEBUG */
	mlmeApiStopTimer(vStaInfo_p, 
		(UINT8 *)&vStaInfo_p->assocTimer);
	vStaInfo_p->aId = MgmtMsg_p->Body.AssocRsp.AId;
	/* L2 Event Notification */
	mlmeApiEventNotification(vStaInfo_p,
		MlmeAssoc_Cnfm,
		(UINT8 *)&MgmtMsg_p->Hdr.SrcAddr[0], 
		MgmtMsg_p->Body.AssocRsp.StatusCode);
	if (MgmtMsg_p->Body.AssocRsp.StatusCode != IEEEtypes_STATUS_SUCCESS)
	{
		/* Handle ASSOCIATION Failure */
		if (vStaInfo_p->macMgmtMain_State == STATE_ASSOCIATING)
		{
			/* Set back to Authenticated state */
			vStaInfo_p->macMgmtMain_State = STATE_AUTHENTICATED_WITH_AP;
			/* Notify SME of association failure */
			assocSrv_SndAssocCnfm(vStaInfo_p, ASSOC_RESULT_REFUSED);
		}
		return MLME_FAILURE;
	}
	/* Record the information given by the association. */
	mlmeApiSetAIdToMac(vStaInfo_p,
		MgmtMsg_p->Body.AssocRsp.AId);
#ifdef STA_QOS
	if (vStaInfo_p->staSystemMibs.mib_StaCfg_p->QoSOptImpl)
	{//search for QOS Action Element ID
		Qos_InitTCLASTable();//Initialize the QoS database
#ifdef QOS_WSM_FEATURE
		//Also initialise the the EDCA parameters here.
		attrib_p = &MgmtMsg_p->Body.AssocRsp.AId;
		attrib_p += sizeof(IEEEtypes_AId_t);
		staInfo_p->macMgmtMlme_ThisStaData.IsStaQosSTA = 0;
		while((attrib_p = syncSrv_ParseAttribWithinFrame( MgmtMsg_p, attrib_p, PROPRIETARY_IE))!=NULL)
		{
			//if(WME_param_elem = (WME_param_elem_t*)syncSrv_ParseAttrib(MgmtMsg_p, PROPRIETARY_IE))
			//check if it is a WME/WSM Info Element.
			WME_param_elem= attrib_p;
			if(!memcmp(WME_param_elem->OUI.OUI, WiFiOUI, 3))
			{
				//Check if it is a WME element
				if(WME_param_elem->OUI.Type==2)
				{
					//check if it is a WME Param Element
					if(WME_param_elem->OUI.Subtype==1)
					{
						QoS_UpdateStnEDCAParameters(WME_param_elem);
						//Update the QoS Info Parameters.
						memcpy(&(Qos_Stn_Data[0].QoS_Info),&(WME_param_elem->QoS_info),sizeof(QoS_Info_t));
						vStaInfo_p->macMgmtMlme_ThisStaData.IsStaQosSTA = 1;
						break;
					}
				}
			}
			//Now process to teh next element pointer.
			attrib_p += (2 + *((UINT8 *)(attrib_p + 1)));
		}
#endif /* QOS_WSM_FEATURE */
	}
#endif /* STA_QOS */

	if(vStaInfo_p->staSecurityMibs.mib_PrivacyTable_p->RSNEnabled)
	{
		KeyMgmtResetCounter(vStaInfo_p->keyMgmtInfoSta_p);
		CounterMeasureInit_Sta(&vStaInfo_p->keyMgmtInfoSta_p->sta_MIC_Error, TRUE);
	}
	else
	{
		CounterMeasureInit_Sta(&vStaInfo_p->keyMgmtInfoSta_p->sta_MIC_Error, FALSE);
	}
	vStaInfo_p->macMgmtMain_State = STATE_ASSOCIATED;
	vStaInfo_p->AssociatedFlag=1;
	vStaInfo_p->Station_p->ChipCtrl.GreenFieldSet = 0;    
	/* Remove AP from station database, somtimes added by AP state machine. */
	mlmeApiDelStaDbEntry(vStaInfo_p, (UINT8 *)&vStaInfo_p->macMgmtMlme_ThisStaData.BssId);
	/* Add Sta Db Entry */
	mlmeApiAddStaDbEntry(vStaInfo_p, MgmtMsg_p);

	/* Notify SME */
	assocSrv_SndAssocCnfm(vStaInfo_p, ASSOC_RESULT_SUCCESS);
	memcpy(vStaInfo_p->macMgmtMlme_ThisStaData.BssId, 
		MgmtMsg_p->Hdr.SrcAddr,
		sizeof(IEEEtypes_MacAddr_t));
#ifdef MRVL_WPS_CLIENT
	if (vStaInfo_p->staSecurityMibs.mib_PrivacyTable_p->RSNEnabled && 
		vStaInfo_p->staSystemMibs.mib_StaCfg_p->wpawpa2Mode < 4 )
#else
	if (vStaInfo_p->staSecurityMibs.mib_PrivacyTable_p->RSNEnabled)
#endif
	{
		// Start RSN key handshake session
		mlmeApiStartKeyMgmt(vStaInfo_p);
	}
	return MLME_SUCCESS;
}

/*************************************************************************
* Function: assocSrv_RecvReAssocRsp
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern SINT32 assocSrv_RecvReAssocRsp(vmacStaInfo_t *vStaInfo_p, 
									  dot11MgtFrame_t *MgmtMsg_p )
{
#ifdef STA_QOS
    MhsmEvent_t TsMsg;
#endif
	vmacEntry_t  *vmacEntry_p;
	extStaDb_StaInfo_t *wbStaInfo_p;
	UINT32 local_urAid;
	UINT8 *attrib_p;
	IEEEtypes_SuppRatesElement_t *PeerSupportedRates_p;
	IEEEtypes_ExtSuppRatesElement_t *PeerExtSupportedRates_p;

	vmacEntry_p = (vmacEntry_t *)vStaInfo_p->vMacEntry_p; 
	/* Since we are not doing ReAssoc, just silently discard */
	if (vStaInfo_p->macMgmtMain_State != STATE_REASSOCIATING)
	{
		return MLME_FAILURE;
	}
	if (MgmtMsg_p->Body.ReassocRsp.StatusCode != IEEEtypes_STATUS_SUCCESS)
	{
		/* Failure handler */
		/* Notify SME of ReAssoc Refused */
		assocSrv_SndReAssocCnfm(vStaInfo_p, REASSOC_RESULT_REFUSED);
		return MLME_FAILURE;
	}
	if(vStaInfo_p->staSecurityMibs.mib_PrivacyTable_p->RSNEnabled)
	{
		KeyMgmtResetCounter(vStaInfo_p->keyMgmtInfoSta_p);
		CounterMeasureInit_Sta(&vStaInfo_p->keyMgmtInfoSta_p->sta_MIC_Error, TRUE);
	}
	else
	{
		CounterMeasureInit_Sta(&vStaInfo_p->keyMgmtInfoSta_p->sta_MIC_Error, FALSE);
	}
	vStaInfo_p->macMgmtMain_State = STATE_ASSOCIATED;
	vStaInfo_p->AssociatedFlag=1;
	vStaInfo_p->Station_p->ChipCtrl.GreenFieldSet = 0;    
	/* Remove AP from station database, somtimes added by AP state machine. */
	mlmeApiDelStaDbEntry(vStaInfo_p, (UINT8 *)&vStaInfo_p->macMgmtMlme_ThisStaData.BssId);

	/* Notify SME of ReAssoc Success */
	assocSrv_SndReAssocCnfm(vStaInfo_p, REASSOC_RESULT_SUCCESS);

	if (vStaInfo_p->staSecurityMibs.mib_PrivacyTable_p->RSNEnabled)
	{
		// Start RSN key handshake session
		mlmeApiStartKeyMgmt(vStaInfo_p);
	}

	if (mlmeApiGetPeerStationStaInfoAndAid(&(MgmtMsg_p->Hdr.SrcAddr),
		&wbStaInfo_p, &local_urAid) == FALSE)
	{
		/*there is no entry in the station database for the peer station with this*/
		/*mac address. So first create one*/
		/*first get the set of supported rates of the peer station*/

		/*get a pointer to the starting IE for this Mgt frame*/
		attrib_p = (UINT8 *)&MgmtMsg_p->Body.AssocRsp.AId;
		attrib_p += sizeof(IEEEtypes_AId_t);
		/*now get a pointer to the set of Supported Rates and Ext Supported Rates*/
		PeerSupportedRates_p = syncSrv_ParseAttribWithinFrame(MgmtMsg_p, attrib_p, SUPPORTED_RATES);

		PeerExtSupportedRates_p = syncSrv_ParseAttribWithinFrame(MgmtMsg_p, attrib_p, EXT_SUPPORTED_RATES);

		if (mlmeApiCreatePeerStationInfoForWBMode(&(MgmtMsg_p->Hdr.SrcAddr),
			PeerSupportedRates_p, PeerExtSupportedRates_p, 
			vmacEntry_p->phyHwMacIndx) == 
			TRUE)
		{

			mlmeApiGetPeerStationStaInfoAndAid(&(MgmtMsg_p->Hdr.SrcAddr),
				&wbStaInfo_p, &local_urAid);
		}
		else
		{
			/*Could not create an entry for this station*/
			return MLME_FAILURE;
		}
	}
	/*Set the state of the Sta Info to WB_ASSOCIATED*/
	mlmeApiSetPeerStationStateForWB(wbStaInfo_p, WB_ASSOCIATED);
	return MLME_SUCCESS;
}

/*************************************************************************
* Function: assocSrv_RecvDisAssocMsg
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern SINT32 assocSrv_RecvDisAssocMsg(vmacStaInfo_t *vStaInfo_p, 
									   dot11MgtFrame_t *MgmtMsg_p)
{
	vmacEntry_t  *vmacEntry_p;
	vmacEntry_p = (vmacEntry_t *)vStaInfo_p->vMacEntry_p;
	/* Check BSSID */
	if (memcmp(vStaInfo_p->macMgmtMlme_ThisStaData.BssId,
		MgmtMsg_p->Hdr.SrcAddr,
		sizeof(IEEEtypes_MacAddr_t)))
	{
		/* BSSID don't match so silently discard packet */
		return MLME_FAILURE;
	}

	/* L2 Event Notification */
	mlmeApiEventNotification(vStaInfo_p,
		MlmeDisAssoc_Ind,
		&MgmtMsg_p->Hdr.SrcAddr[0], 
		MgmtMsg_p->Body.DisAssoc.ReasonCode);

	/* Clear out data related to association with this AP */
	assocSrv_ClearAssocInfo(vStaInfo_p);
	/* Notify SME of DisAssoc */
	assocSrv_SndDisAssocInd(vStaInfo_p,
		MgmtMsg_p->Body.DisAssoc.ReasonCode, 
		MgmtMsg_p->Hdr.SrcAddr );
	/*Milind. 09/29/05*/
	/*Free the AssocTable data structure that has been currently assigned to this*/
	/*peer station to which the WB was associated/joined*/
	mlmeApiFreePeerStationStaInfoAndAid(&(MgmtMsg_p->Hdr.SrcAddr), vmacEntry_p);
	//macMgtMlme_Free_AssocTblInfo(macMgtMlme_geturAid(vmacEntry_p->phyHwMacIndx));

	return MLME_SUCCESS;
}

/*************************************************************************
* Function: assocSrv_Reset
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern SINT32 assocSrv_Reset(vmacStaInfo_t *vStaInfo_p)
{
	mlmeApiStopTimer(vStaInfo_p, 
		(UINT8 *)&vStaInfo_p->assocTimer);
	/* Init the Association state machines */
	AssocSrvStaCtor(&vStaInfo_p->assocsrv);
    mhsm_initialize(&vStaInfo_p->assocsrv.super,&vStaInfo_p->assocsrv.sTop);
	return MLME_SUCCESS;
}


