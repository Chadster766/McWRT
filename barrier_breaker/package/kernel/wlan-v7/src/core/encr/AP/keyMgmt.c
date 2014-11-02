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


#include "ap8xLnxIntf.h"

#include "wltypes.h"
#include "mhsm.h"
#include "timer.h"
#include "IEEE_types.h"
#include "osif.h"
#include "mib.h"
#include "ds.h"
#include "macmgmtap.h"

#include "tkip.h"
#include "keyMgmtCommon.h"
#include "keyMgmt.h"
#include "StaDb.h"
#include "md5.h"
#include "sha1.h"
#include "encryptapi.h"
#include "qos.h"
#include "bcngen.h"
#include "wpa.h"
#include "tkip.h"
#include "macMgmtMlme.h"
#include "wldebug.h"
#include "wl_mib.h"
#include "mib.h"
#include "ap8xLnxIntf.h"
#include "ap8xLnxWlLog.h"

struct ieee80211_frame {
	IEEEtypes_FrameCtl_t FrmCtl;
	UINT8	dur[2];
	UINT8	addr1[IEEEtypes_ADDRESS_SIZE];
	UINT8	addr2[IEEEtypes_ADDRESS_SIZE];
	UINT8	addr3[IEEEtypes_ADDRESS_SIZE];
	UINT8	seq[2];
	UINT8	addr4[IEEEtypes_ADDRESS_SIZE];
} PACK;


#define MCBC_STN_ID         (MAX_STNS)
#define KEY_INFO_KEYTYPE    0x0800
#define KEY_INFO_REQUEST    0x0008
#define KEY_INFO_ERROR      0x0004
#define EAPOL_TX_BUF        720



char AuthPSK[4] = {0x00, 0x50, 0xF2, 0x02};
char Auth8021x[4] = {0x00, 0x50, 0xF2, 0x01};
char AuthPSKWPA2[4] = {0x00, 0x0f, 0xac, 0x02};
char Auth8021xWPA2[4] = {0x00, 0x0f, 0xac, 0x01};

static UINT8 gGNonce[32];
static UINT32 lower_counter = 0; //lower 4 byets of the 8 byte replay counter

Boolean gGrpKeyInstalled = FALSE;

extern void KeyMgmt_TimeoutHdlr(void *data_p);
void MICCounterMeasureInvoke(vmacApInfo_t *vmacSta_p);
void keyMgmt_msg(vmacApInfo_t *vmacSta_p, DistTaskMsg_t *pDistMsg);
void * ProcessEAPoLAp(vmacApInfo_t *vmacSta_p,IEEEtypes_8023_Frame_t *pEthFrame, IEEEtypes_MacAddr_t *pMacStaAddr);
extern void CCMP_Init(void);
extern void TKIPInit(vmacApInfo_t *vmacSta_p);
extern int wlFwSetMixedWpaWpa2Mode(struct net_device *netdev,  u_int8_t *staaddr);
extern int wlFwSetWpaAesMode(struct net_device *netdev,  u_int8_t *staaddr);
extern int wlFwSetWpaTkipMode(struct net_device *netdev, u_int8_t *staaddr);
extern int wlFwSetWpaAesGroupK(struct net_device *netdev, UINT8 index);
extern int wlFwSetWpaTkipGroupK(struct net_device *netdev, UINT8 index);
extern struct sk_buff *ieee80211_getDataframe(UINT8 **frm, unsigned int pktlen);
extern WL_STATUS txDataMsg_UnEncrypted(struct net_device *dev,struct sk_buff *skb, extStaDb_StaInfo_t *pStaInfo);
extern void extStaDb_SendGrpKeyMsgToAllSta(vmacApInfo_t *vmacSta_p);
extern extStaDb_Status_e extStaDb_RemoveSta(vmacApInfo_t *vmac_p,IEEEtypes_MacAddr_t *Addr_p);
extern WL_STATUS txDataMsg(struct net_device *dev,struct sk_buff *skb);
extern int wlFwSetWpaWpa2PWK(struct net_device *netdev, extStaDb_StaInfo_t *StaInfo_p);
extern void AES_Wrap(UINT8 *pPlData, WRAPUINT64 *pCipTxt, WRAPUINT64 *pKEK, UINT32 len);

void KeyMgmtTimerAdd(keyMgmthsk_hsm_t *me_p, UINT32 time);
void KeyMgmtTimerDelete(keyMgmthsk_hsm_t *me_p);


void GrpKey_TimeoutHdlr(void *pData)
{
	vmacApInfo_t *vmacSta_p = (vmacApInfo_t *)pData;
	//MIB_802DOT11 *mib = vmacSta_p->Mib802dot11;
	distQ_TimerMsg_t * timerData_p;
	DistTaskMsg_t DistMsg;
	DistTaskMsg_t *pDistMsg = &DistMsg;

	timerData_p = &pDistMsg->msg.distQ_TimerMsg;
	pDistMsg->MsgType = TIMERMSGRECVD;
	timerData_p->PendingData_p.type = GRPKEYTIMEOUTEVENT;

	keyMgmt_msg(vmacSta_p,pDistMsg);
	TimerDisarm(&vmacSta_p->GrpKeytimer);
	/* Group rekey in seconds, base timer is 100ms. */
	TimerRearm(&vmacSta_p->GrpKeytimer, vmacSta_p->Mib802dot11->RSNConfig->GroupRekeyTime*10);
}


void StartGrpKeyTimer(vmacApInfo_t *vmacSta_p)
{
	//MIB_802DOT11 *mib = vmacSta_p->Mib802dot11;
	TimerInit(&vmacSta_p->GrpKeytimer);
	TimerRemove(&vmacSta_p->GrpKeytimer);
	/* Group rekey in seconds, base timer is 100ms. */
	TimerFireIn(&vmacSta_p->GrpKeytimer, 1, GrpKey_TimeoutHdlr, (unsigned char *)vmacSta_p, vmacSta_p->Mib802dot11->RSNConfig->GroupRekeyTime*10);
}

void Disable_GrpKeyTimer(vmacApInfo_t *vmacSta_p)
{
	//MIB_802DOT11 *mib = vmacSta_p->Mib802dot11;
	TimerRemove(&vmacSta_p->GrpKeytimer);
}

void GenerateGrpTransKey(vmacApInfo_t *vmacSta_p)
{
	MIB_802DOT11 *mib = vmacSta_p->Mib802dot11;
	UINT8 inp_data[38];
	UINT8 prefix[] = "Group key expansion";
	UINT8 GTK[32]; //group transient key

	MACADDR_CPY(inp_data, vmacSta_p->macStaAddr);
	generateRand(gGNonce, NONCE_SIZE);
	memcpy(inp_data + 6, gGNonce, NONCE_SIZE);
	generateRand(mib->mib_MrvlRSN_GrpKey->GrpMasterKey, sizeof(mib->mib_MrvlRSN_GrpKey->GrpMasterKey));
	Mrvl_PRF(mib->mib_MrvlRSN_GrpKey->GrpMasterKey, sizeof(mib->mib_MrvlRSN_GrpKey->GrpMasterKey),
		prefix, strlen(prefix), inp_data, sizeof(inp_data), GTK, 32);

	memcpy(mib->mib_MrvlRSN_GrpKey->EncryptKey, GTK, 16);
	memcpy(mib->mib_MrvlRSN_GrpKey->TxMICKey, GTK + 16, 8);
	memcpy(mib->mib_MrvlRSN_GrpKey->RxMICKey, GTK + 16 + 8, 8);
}

void KeyMgmtInit(vmacApInfo_t *vmacSta_p)
{
	MIB_802DOT11 *mibShadow = vmacSta_p->ShadowMib802dot11;
	MIB_802DOT11 *mib=vmacSta_p->Mib802dot11;
	MIB_PRIVACY_TABLE *mib_PrivacyTable_p=vmacSta_p->Mib802dot11->Privacy;
	MIB_RSNCONFIG	*mib_RSNConfig_p=vmacSta_p->Mib802dot11->RSNConfig ;
	MIB_RSNCONFIG_UNICAST_CIPHERS	*mib_RSNConfigUnicastCiphers_p=vmacSta_p->Mib802dot11->UnicastCiphers;
	MIB_RSNCONFIG_AUTH_SUITES		*mib_RSNConfigAuthSuites_p=vmacSta_p->Mib802dot11->RSNConfigAuthSuites;
	MIB_RSNCONFIGWPA2			   *mib_RSNConfigWPA2_p=vmacSta_p->Mib802dot11->RSNConfigWPA2;
	MIB_RSNCONFIGWPA2_AUTH_SUITES		*mib_RSNConfigWPA2AuthSuites_p=vmacSta_p->Mib802dot11->WPA2AuthSuites;
	MIB_RSNCONFIGWPA2_UNICAST_CIPHERS *mib_RSNConfigWPA2UnicastCiphers_p= vmacSta_p->Mib802dot11->WPA2UnicastCiphers;	//	UINT8 CipTxt[24], DecTxt[24];
	//APICMDBUF_SET_COUNTER_MEASURE_ENABLED set_ctr_meas_enab;

    vmacSta_p->keyMgmtInitDone = FALSE;

	mib->mib_MrvlRSN_GrpKey->g_KeyIndex = 1; /* Set group key id to 1 default. */
	vmacSta_p->g_IV16 = 0x01;
	vmacSta_p->g_IV32 = 0;

	if (*(mib->mib_wpaWpa2Mode) < 4) /* MRV_8021X For PSK modes use internal WPA state machine*/
	{
		GenerateGrpTransKey(vmacSta_p);
		if ((mib_RSNConfigWPA2_p->WPA2Enabled || mib_RSNConfigWPA2_p->WPA2OnlyEnabled) 
			&& mib_RSNConfigWPA2AuthSuites_p->Enabled &&
			memcmp(mib_RSNConfigWPA2AuthSuites_p->AuthSuites, AuthPSKWPA2, 4) == 0)
		{
			if (!*(mib->mib_WPA2PSKValueEnabled)) 
				PKCS5_PBKDF2(vmacSta_p->Mib802dot11->RSNConfigWPA2->PSKPassPhrase,
				vmacSta_p->Mib802dot11->StationConfig->DesiredSsId,
				strlen(vmacSta_p->Mib802dot11->StationConfig->DesiredSsId),
				vmacSta_p->Mib802dot11->RSNConfigWPA2->PSKValue);
		}
		if ( mib_RSNConfigAuthSuites_p->Enabled &&
			memcmp(mib_RSNConfigAuthSuites_p->AuthSuites, AuthPSK, 4) == 0 )
		{
			if (!*(mib->mib_WPAPSKValueEnabled)) 
			{
				PKCS5_PBKDF2(vmacSta_p->Mib802dot11->RSNConfig->PSKPassPhrase,
					vmacSta_p->Mib802dot11->StationConfig->DesiredSsId,
					strlen(vmacSta_p->Mib802dot11->StationConfig->DesiredSsId),
					vmacSta_p->Mib802dot11->RSNConfig->PSKValue);
			}
		}

		StartGrpKeyTimer(vmacSta_p);
	}

	CCMP_Init();
	TKIPInit(vmacSta_p);

	vmacSta_p->MICCounterMeasureEnabled = 1;

	vmacSta_p->MIC_ErrordisableStaAsso = 0;
	vmacSta_p->MIC_Errorstatus = NO_MIC_FAILURE;    
	if (mib_PrivacyTable_p->RSNEnabled && !mib_RSNConfigWPA2_p->WPA2Enabled && !mib_RSNConfigWPA2_p->WPA2OnlyEnabled)
	{
		if (mib_RSNConfig_p->MulticastCipher[3] == 2)
		{
			WLDBG_INFO(DBG_LEVEL_5, "************** Set WPA GRP TKIP \n");
			wlFwSetWpaTkipGroupK(vmacSta_p->dev, mib->mib_MrvlRSN_GrpKey->g_KeyIndex);
		}
		else
		{
			WLDBG_INFO(DBG_LEVEL_5, "************** Set WPA GRP AES \n");
			wlFwSetWpaAesGroupK(vmacSta_p->dev, mib->mib_MrvlRSN_GrpKey->g_KeyIndex);
		}
		if (mib_RSNConfigUnicastCiphers_p->UnicastCipher[3] == 2)
		{
			WLDBG_INFO(DBG_LEVEL_5, "************** Set WPA PAIRWISE TKIP \n");
			wlFwSetWpaTkipMode(vmacSta_p->dev, vmacSta_p->macStaAddr);
		}
		else
		{
			WLDBG_INFO(DBG_LEVEL_5, "************** Set WPA PAIRWISE AES \n");
			wlFwSetWpaAesMode(vmacSta_p->dev, vmacSta_p->macStaAddr);
		}
	}
	else if (mib_RSNConfigWPA2_p->WPA2Enabled && !mib_RSNConfigWPA2_p->WPA2OnlyEnabled)
	{
		wlFwSetWpaTkipGroupK(vmacSta_p->dev, mib->mib_MrvlRSN_GrpKey->g_KeyIndex);
		wlFwSetMixedWpaWpa2Mode(vmacSta_p->dev, vmacSta_p->macStaAddr);
		WLDBG_INFO(DBG_LEVEL_5, "************** Set Mixed WPA/WPA2 mode \n");
	}
	else if (mib_RSNConfigWPA2_p->WPA2OnlyEnabled)
	{
		if (mib_RSNConfigWPA2_p->MulticastCipher[3] == 2)
		{
			WLDBG_INFO(DBG_LEVEL_5, "************** Set WPA2 GRP TKIP \n");
			wlFwSetWpaTkipGroupK(vmacSta_p->dev, mib->mib_MrvlRSN_GrpKey->g_KeyIndex);
		}
		else
		{
			WLDBG_INFO(DBG_LEVEL_5, "************** Set WPA2 GRP AES \n");
			wlFwSetWpaAesGroupK(vmacSta_p->dev, mib->mib_MrvlRSN_GrpKey->g_KeyIndex);
		}
		if (mib_RSNConfigWPA2UnicastCiphers_p->UnicastCipher[3] == 2)
		{
			WLDBG_INFO(DBG_LEVEL_5, "************** Set WPA2 PAIRWISE TKIP \n");
			wlFwSetWpaTkipMode(vmacSta_p->dev, vmacSta_p->macStaAddr);
		}
		else
		{
			wlFwSetWpaAesMode(vmacSta_p->dev, vmacSta_p->macStaAddr);
			WLDBG_INFO(DBG_LEVEL_5, "************** Set WPA2 AES mode \n");
		}
	}
	//need to update shadow mib, when directly modify run-time mib.
	memcpy(mibShadow->mib_MrvlRSN_GrpKey, mib->mib_MrvlRSN_GrpKey, sizeof(MRVL_MIB_RSN_GRP_KEY));
	memcpy(mibShadow->RSNConfig, mib->RSNConfig, sizeof(MIB_RSNCONFIG));
	memcpy(mibShadow->UnicastCiphers, mib->UnicastCiphers, sizeof(MIB_RSNCONFIG_UNICAST_CIPHERS));
	memcpy(mibShadow->RSNStats, mib->RSNStats, sizeof(MIB_RSNSTATS));
	memcpy(mibShadow->thisStaRsnIE, mib->thisStaRsnIE, sizeof(IEEEtypes_RSN_IE_t));
	memcpy(mibShadow->RSNConfigWPA2, mib->RSNConfigWPA2, sizeof(MIB_RSNCONFIGWPA2));

#ifdef AP_WPA2
	memcpy(mibShadow->WPA2UnicastCiphers, mib->WPA2UnicastCiphers, sizeof(MIB_RSNCONFIGWPA2_UNICAST_CIPHERS));
	memcpy(mibShadow->WPA2UnicastCiphers2, mib->WPA2UnicastCiphers2, sizeof(MIB_RSNCONFIGWPA2_UNICAST_CIPHERS));
	memcpy(mibShadow->thisStaRsnIEWPA2, mib->thisStaRsnIEWPA2, sizeof(IEEEtypes_RSN_IE_WPA2_t));
	memcpy(mibShadow->thisStaRsnIEWPA2MixedMode, mib->thisStaRsnIEWPA2MixedMode, sizeof(IEEEtypes_RSN_IE_WPA2MixedMode_t));
	memcpy(mibShadow->WPA2AuthSuites, mib->WPA2AuthSuites, sizeof(MIB_RSNCONFIGWPA2_AUTH_SUITES));
#endif
#ifdef MRVL_WAPI
    if(mib->Privacy->WAPIEnabled)
    {
        UINT32 *pDW = (UINT32 *)vmacSta_p->wapiPN;
        *pDW++ = 0x5c365c37;
        *pDW++ = 0x5c365c36;
        *pDW++ = 0x5c365c36;
        *pDW = 0x5c365c36;
        pDW = (UINT32 *)vmacSta_p->wapiPN_mc;
        *pDW++ = 0x5c365c36;
        *pDW++ = 0x5c365c36;
        *pDW++ = 0x5c365c36;
        *pDW = 0x5c365c36;
    }    
#endif
	gGrpKeyInstalled = FALSE;

    vmacSta_p->keyMgmtInitDone = TRUE;
}


void KeyMgmtReset(vmacApInfo_t *vmacSta_p)
{
	//MIB_802DOT11 *mib = vmacSta_p->Mib802dot11;
	TimerRemove(&vmacSta_p->GrpKeytimer);
}

void KeyMgmtStartTimer(keyMgmthsk_hsm_t *me_p)
{
	KeyMgmtTimerDelete(me_p);
	KeyMgmtTimerAdd(me_p, KEYMGMTTIMEOUTVAL);
}

void KeyMgmtTimerInit(keyMgmthsk_hsm_t *me_p)
{
	TimerInit(&me_p->keyTimer);
}


void KeyMgmtTimerAdd(keyMgmthsk_hsm_t *me_p, UINT32 time)
{
	TimerFireIn(&me_p->keyTimer, 1, KeyMgmt_TimeoutHdlr, (UINT8 *)me_p, time);
}


void KeyMgmtTimerDelete(keyMgmthsk_hsm_t *me_p)
{
	TimerRemove(&me_p->keyTimer);
}



Status_e GeneratePWKMsg1(vmacApInfo_t *vmacSta_p,keyMgmtInfo_t *pKMgmtStnInfo,
struct sk_buff **skb_pp,
	IEEEtypes_MacAddr_t *StaAddr)

{
	EAPOL_KeyMsg_t *tx_eapol_ptr;
	UINT16 frameLen;
	UINT8 *frm;
	struct sk_buff *skb;

	if ((skb = ieee80211_getDataframe(&frm, EAPOL_TX_BUF)) == NULL)
	{
		WLDBG_INFO(DBG_LEVEL_5, "Error: cannot get socket buffer. \n ");
		return FAIL;
	}
	*skb_pp = skb;
	tx_eapol_ptr = (EAPOL_KeyMsg_t *)skb->data;

	MACADDR_CPY(tx_eapol_ptr->Ether_Hdr.da, StaAddr);
	MACADDR_CPY(tx_eapol_ptr->Ether_Hdr.sa, &vmacSta_p->macStaAddr);
	tx_eapol_ptr->Ether_Hdr.type = IEEE_ETHERTYPE_PAE; //EAPOL Msg

	if ( pKMgmtStnInfo->RsnIEBuf[0] == 221 )
		tx_eapol_ptr->desc_type = 254;
	else if ( pKMgmtStnInfo->RsnIEBuf[0] == 48 ) 
		tx_eapol_ptr->desc_type = 2;
	else
		return FAIL;

	if ( (pKMgmtStnInfo->RsnIEBuf[0] == 221 && pKMgmtStnInfo->RsnIEBuf[17] == 2)
		|| (pKMgmtStnInfo->RsnIEBuf[0] == 48 && pKMgmtStnInfo->RsnIEBuf[13] == 2) 
		)
	{//TKIP
		tx_eapol_ptr->k.key_info.desc_ver = 1;
		tx_eapol_ptr->key_length = SHORT_SWAP((TK_SIZE + TK_SIZE));
	}
	else if ( (pKMgmtStnInfo->RsnIEBuf[0] == 221 && pKMgmtStnInfo->RsnIEBuf[17] == 4)
		|| (pKMgmtStnInfo->RsnIEBuf[0] == 48 && pKMgmtStnInfo->RsnIEBuf[13] == 4) 
		)
	{//CCMP
		tx_eapol_ptr->k.key_info.desc_ver = 2;
		tx_eapol_ptr->key_length = SHORT_SWAP(TK_SIZE);
	}
	else
		return FAIL;

	tx_eapol_ptr->k.key_info.key_type = 1;
	tx_eapol_ptr->k.key_info.key_index = 0;
	tx_eapol_ptr->k.key_info.install = 0;
	tx_eapol_ptr->k.key_info.key_ack = 1;
	tx_eapol_ptr->k.key_info.key_MIC = 0;
	tx_eapol_ptr->k.key_info.secure = 0;
	tx_eapol_ptr->k.key_info.error = 0;
	tx_eapol_ptr->k.key_info.request = 0;
	tx_eapol_ptr->k.key_info.encryptedKeyData = 0; //WPA2
	tx_eapol_ptr->k.key_info.rsvd = 0;

	tx_eapol_ptr->k.key_info16 = SHORT_SWAP(tx_eapol_ptr->k.key_info16);

	tx_eapol_ptr->replay_cnt[0] = 0;
	tx_eapol_ptr->replay_cnt[1] = WORD_SWAP(pKMgmtStnInfo->counter);

	memcpy(tx_eapol_ptr->key_nonce, pKMgmtStnInfo->ANonce, NONCE_SIZE);
	memset(tx_eapol_ptr->EAPOL_key_IV, 0, 16);
	memset(tx_eapol_ptr->key_RSC, 0, 8);
	memset(tx_eapol_ptr->key_ID, 0, 8);
	memset(tx_eapol_ptr->key_MIC, 0, 16);
	tx_eapol_ptr->key_material_len = 0;

	frameLen = 95 + tx_eapol_ptr->key_material_len;
	Insert8021xHdr(&(tx_eapol_ptr->hdr_8021x), (UINT16)frameLen);
	tx_eapol_ptr->key_material_len = SHORT_SWAP(tx_eapol_ptr->key_material_len);
	skb->len = sizeof(ether_hdr_t) + sizeof (Hdr_8021x_t) + frameLen;
	return SUCCESS;
}

Status_e GeneratePWKMsg3(vmacApInfo_t *vmacSta_p,keyMgmtInfo_t *pKMgmtStnInfo,
struct sk_buff **skb_pp, IEEEtypes_MacAddr_t *StaAddr)
{
	MIB_802DOT11 *mib = vmacSta_p->Mib802dot11;
	MIB_RSNCONFIGWPA2			   *mib_RSNConfigWPA2_p=vmacSta_p->Mib802dot11->RSNConfigWPA2;
	EAPOL_KeyMsg_t *tx_eapol_ptr;
	UINT16 frameLen;
	UINT8 MIC[EAPOL_MIC_SIZE + 4];
	EAPOL_KeyDataWPA2_t keyDataWPA2;
	EAPOL_WPA2_GTK_IE_t GTK_IE;
	UINT8 key[16] __attribute__ ((aligned(8)));
	UINT8 cipherText[256] __attribute__ ((aligned(8)));
	UINT8 plnText[256] __attribute__ ((aligned(8)));
	UINT8 RsnIE_len, PadLen;
	UINT8 *frm;
	struct sk_buff *skb;
	if ((skb = ieee80211_getDataframe(&frm, EAPOL_TX_BUF)) == NULL)
	{
		WLDBG_INFO(DBG_LEVEL_5, "Error: cannot get socket buffer. \n ");
		return FAIL;
	}
	*skb_pp = skb;
	tx_eapol_ptr = (EAPOL_KeyMsg_t *)skb->data;

	MACADDR_CPY(tx_eapol_ptr->Ether_Hdr.da, StaAddr);
	MACADDR_CPY(tx_eapol_ptr->Ether_Hdr.sa, &vmacSta_p->macStaAddr);
	tx_eapol_ptr->Ether_Hdr.type = IEEE_ETHERTYPE_PAE;

	if ( pKMgmtStnInfo->RsnIEBuf[0] == 221 )
		tx_eapol_ptr->desc_type = 254;
	else if ( pKMgmtStnInfo->RsnIEBuf[0] == 48 ) 
		tx_eapol_ptr->desc_type = 2;
	else
		return FAIL;

	if ( (pKMgmtStnInfo->RsnIEBuf[0] == 221 && pKMgmtStnInfo->RsnIEBuf[17] == 2)
		|| (pKMgmtStnInfo->RsnIEBuf[0] == 48 && pKMgmtStnInfo->RsnIEBuf[13] == 2)
		)
	{//TKIP
		tx_eapol_ptr->k.key_info.desc_ver = 1;
		tx_eapol_ptr->key_length = SHORT_SWAP((TK_SIZE + TK_SIZE));
	}
	else if ( (pKMgmtStnInfo->RsnIEBuf[0] == 221 && pKMgmtStnInfo->RsnIEBuf[17] == 4)
		|| (pKMgmtStnInfo->RsnIEBuf[0] == 48 && pKMgmtStnInfo->RsnIEBuf[13] == 4) )
	{
		tx_eapol_ptr->k.key_info.desc_ver = 2;
		tx_eapol_ptr->key_length = SHORT_SWAP(TK_SIZE);
	}
	else
		return FAIL;

	tx_eapol_ptr->k.key_info.key_type = 1;
	tx_eapol_ptr->k.key_info.key_index = 0;
	tx_eapol_ptr->k.key_info.install = 1;
	tx_eapol_ptr->k.key_info.key_ack = 1;
	tx_eapol_ptr->k.key_info.key_MIC = 1;
	tx_eapol_ptr->k.key_info.error = 0;
	tx_eapol_ptr->k.key_info.request = 0;

	if ( *(pKMgmtStnInfo->RsnIEBuf) == 48 )
	{
		tx_eapol_ptr->k.key_info.secure = 1;
		tx_eapol_ptr->k.key_info.encryptedKeyData = 1; //WPA2
	}
	else
	{
		tx_eapol_ptr->k.key_info.secure = 0;
		tx_eapol_ptr->k.key_info.encryptedKeyData = 0; //WPA
	}

	tx_eapol_ptr->k.key_info.rsvd = 0;

	tx_eapol_ptr->k.key_info16 = SHORT_SWAP(tx_eapol_ptr->k.key_info16);

	tx_eapol_ptr->replay_cnt[0] = 0;
	tx_eapol_ptr->replay_cnt[1] = WORD_SWAP(pKMgmtStnInfo->counter);

	memcpy(tx_eapol_ptr->key_nonce,
		pKMgmtStnInfo->ANonce, NONCE_SIZE);
	memset(tx_eapol_ptr->EAPOL_key_IV, 0, 16);
	memset(tx_eapol_ptr->key_RSC, 0, 8);

	memset(tx_eapol_ptr->key_ID, 0, 8);

	if ( *(pKMgmtStnInfo->RsnIEBuf) == 221 )
	{//TKIP
		tx_eapol_ptr->key_material_len = AddRSN_IE( vmacSta_p,(IEEEtypes_RSN_IE_t*)(&tx_eapol_ptr->key_data) );
	}
	else if ( *(pKMgmtStnInfo->RsnIEBuf) == 48 )
	{
		if (mib_RSNConfigWPA2_p->WPA2Enabled)
			tx_eapol_ptr->key_material_len = AddRSN_IEWPA2MixedMode(vmacSta_p,(IEEEtypes_RSN_IE_WPA2MixedMode_t*)(&tx_eapol_ptr->key_data) );
		else
			tx_eapol_ptr->key_material_len = AddRSN_IEWPA2( vmacSta_p,(IEEEtypes_RSN_IE_WPA2_t*)(&tx_eapol_ptr->key_data) );
	}
	else
		return FAIL;

	RsnIE_len = tx_eapol_ptr->key_material_len;
	if ( *(pKMgmtStnInfo->RsnIEBuf) == 48 )
	{
		keyDataWPA2.type = 0xdd;
		keyDataWPA2.length = 22;
		keyDataWPA2.OUI[0] = 0x00;
		keyDataWPA2.OUI[1] = 0x0F;
		keyDataWPA2.OUI[2] = 0xAC;
		keyDataWPA2.dataType = 1;
		GTK_IE.keyID_Tx = mib->mib_MrvlRSN_GrpKey->g_KeyIndex; //5;
		GTK_IE.rsvd = 0;
		memcpy(GTK_IE.GTK, mib->mib_MrvlRSN_GrpKey->EncryptKey, 16); //WPA2

		if (mib_RSNConfigWPA2_p->MulticastCipher[3] != 2) 
		{
			keyDataWPA2.length = 22;
			tx_eapol_ptr->key_material_len = RsnIE_len + 24;   
			memcpy(tx_eapol_ptr->key_data + RsnIE_len, (UINT8*)&keyDataWPA2, 6);
			memcpy(tx_eapol_ptr->key_data + RsnIE_len + 6, (UINT8*)&GTK_IE, 18);
		}
		else
		{
			keyDataWPA2.length = 22 + 16;
			tx_eapol_ptr->key_material_len = RsnIE_len + 24 + 16;   
			memcpy(tx_eapol_ptr->key_data + RsnIE_len, (UINT8*)&keyDataWPA2, 6);
			memcpy(tx_eapol_ptr->key_data + RsnIE_len + 6, (UINT8*)&GTK_IE, 18);
			memcpy(tx_eapol_ptr->key_data + RsnIE_len + 24, mib->mib_MrvlRSN_GrpKey->TxMICKey, 8); //WPA
			memcpy(tx_eapol_ptr->key_data + RsnIE_len + 32, mib->mib_MrvlRSN_GrpKey->RxMICKey, 8); //WPA
		}

		PadLen = ((8-((tx_eapol_ptr->key_material_len)%8))%8);
		if (PadLen)
		{
			*(tx_eapol_ptr->key_data + tx_eapol_ptr->key_material_len) = 0xdd;
			memset(tx_eapol_ptr->key_data + tx_eapol_ptr->key_material_len + 1, 0, PadLen - 1);
			tx_eapol_ptr->key_material_len += PadLen;   
		}


		memcpy(key, pKMgmtStnInfo->EAPOL_Encr_Key, 16);
		memcpy(plnText, tx_eapol_ptr->key_data, tx_eapol_ptr->key_material_len);
		AES_Wrap(plnText, (WRAPUINT64 *)cipherText, (WRAPUINT64 *)key, tx_eapol_ptr->key_material_len);
		tx_eapol_ptr->key_material_len += 8;   
		memcpy(tx_eapol_ptr->key_data, cipherText, tx_eapol_ptr->key_material_len);
	}    

	frameLen = 95 + tx_eapol_ptr->key_material_len;
	tx_eapol_ptr->key_material_len = SHORT_SWAP(tx_eapol_ptr->key_material_len);

	Insert8021xHdr(&(tx_eapol_ptr->hdr_8021x), (UINT16)frameLen);
	ComputeEAPOL_MIC(vmacSta_p,(UINT8*)&(tx_eapol_ptr->hdr_8021x),
		frameLen + sizeof(Hdr_8021x_t), pKMgmtStnInfo->EAPOL_MIC_Key,
		EAPOL_MIC_KEY_SIZE, MIC, pKMgmtStnInfo->RsnIEBuf);

	apppendEAPOL_MIC((UINT8*)&(tx_eapol_ptr->key_MIC), MIC);

	skb->len = sizeof(ether_hdr_t) + sizeof (Hdr_8021x_t) + frameLen;
	return SUCCESS;

}

Status_e GenerateGrpMsg1(vmacApInfo_t *vmacSta_p,keyMgmtInfo_t *pKMgmtStnInfo,
struct sk_buff **skb_pp,
	IEEEtypes_MacAddr_t *StaAddr)
{
	MIB_802DOT11 *mib = vmacSta_p->Mib802dot11;
	MIB_RSNCONFIGWPA2			   *mib_RSNConfigWPA2_p=vmacSta_p->Mib802dot11->RSNConfigWPA2;
	EAPOL_KeyMsg_t *pTx_eapol;
	UINT8 MIC[EAPOL_MIC_SIZE + 4];
	UINT16 frameLen;
	EAPOL_KeyDataWPA2_t keyDataWPA2;
	EAPOL_WPA2_GTK_IE_t GTK_IE;
	UINT8 key[16] __attribute__ ((aligned(8)));
	UINT8 cipherText[256] __attribute__ ((aligned(8)));
	UINT8 plnText[256] __attribute__ ((aligned(8)));
	UINT8 *frm;
	struct sk_buff *skb;    
	if ((skb = ieee80211_getDataframe(&frm, EAPOL_TX_BUF)) == NULL)
	{
		WLDBG_INFO(DBG_LEVEL_5, "Error: cannot get socket buffer. \n ");
		return FAIL;
	}
	*skb_pp = skb;
	pTx_eapol = (EAPOL_KeyMsg_t *)skb->data;

	MACADDR_CPY(pTx_eapol->Ether_Hdr.da, StaAddr);
	MACADDR_CPY(pTx_eapol->Ether_Hdr.sa, &vmacSta_p->macStaAddr);
	pTx_eapol->Ether_Hdr.type = IEEE_ETHERTYPE_PAE; //EAPOL Msg

	if ( pKMgmtStnInfo->RsnIEBuf[0] == 221 )
		pTx_eapol->desc_type = 254;
	else if ( pKMgmtStnInfo->RsnIEBuf[0] == 48 ) 
		pTx_eapol->desc_type = 2;
	else
		return FAIL;

	if ( (pKMgmtStnInfo->RsnIEBuf[0] == 221 && pKMgmtStnInfo->RsnIEBuf[17] == 2)
		|| (pKMgmtStnInfo->RsnIEBuf[0] == 48 && pKMgmtStnInfo->RsnIEBuf[13] == 2)
		)
	{//TKIP
		pTx_eapol->k.key_info.desc_ver = 1;
		pTx_eapol->key_length = SHORT_SWAP((TK_SIZE + TK_SIZE));
		pTx_eapol->key_material_len = TK_SIZE + TK_SIZE;
		memcpy(pTx_eapol->key_data + 16, mib->mib_MrvlRSN_GrpKey->TxMICKey, 8);
		memcpy(pTx_eapol->key_data + 16 + 8, mib->mib_MrvlRSN_GrpKey->RxMICKey, 8);
		generateRand(pTx_eapol->EAPOL_key_IV, 16);
	}
	else if ( (pKMgmtStnInfo->RsnIEBuf[0] == 221 && pKMgmtStnInfo->RsnIEBuf[17] == 4)
		|| (pKMgmtStnInfo->RsnIEBuf[0] == 48 && pKMgmtStnInfo->RsnIEBuf[13] == 4)
		)
	{
		pTx_eapol->k.key_info.desc_ver = 2;
		pTx_eapol->key_length = SHORT_SWAP(TK_SIZE);
		pTx_eapol->key_material_len = TK_SIZE + 8;
		memset(pTx_eapol->EAPOL_key_IV, 0, 16);
	}
	else
		return FAIL;
	pTx_eapol->k.key_info.key_type = 0;
	pTx_eapol->k.key_info.key_index = mib->mib_MrvlRSN_GrpKey->g_KeyIndex; //mib->mib_MrvlRSN_GrpKey->g_KeyIndex;
	pTx_eapol->k.key_info.install = 0;
	pTx_eapol->k.key_info.key_ack = 1;
	pTx_eapol->k.key_info.key_MIC = 1;
	pTx_eapol->k.key_info.secure = 1;
	pTx_eapol->k.key_info.error = 0;
	pTx_eapol->k.key_info.request = 0;

	if ( *(pKMgmtStnInfo->RsnIEBuf) == 48 )
		pTx_eapol->k.key_info.encryptedKeyData = 1; //WPA2
	else
		pTx_eapol->k.key_info.encryptedKeyData = 0; //WPA

	pTx_eapol->k.key_info.rsvd = 0;

	pTx_eapol->k.key_info16 = SHORT_SWAP(pTx_eapol->k.key_info16);

	pTx_eapol->replay_cnt[0] = 0;
	pTx_eapol->replay_cnt[1] = WORD_SWAP(pKMgmtStnInfo->counter);

	memcpy(pTx_eapol->key_nonce, gGNonce, NONCE_SIZE);
	//generateRand(pTx_eapol->EAPOL_key_IV, 16);

	memset(pTx_eapol->key_RSC, 0, 8);
	memset(pTx_eapol->key_ID, 0, 8);

	if ( *(pKMgmtStnInfo->RsnIEBuf) != 48 )
	{
		memcpy(pTx_eapol->key_data, mib->mib_MrvlRSN_GrpKey->EncryptKey, 16); //WPA
	}
	else
	{
		keyDataWPA2.type = 0xdd;
		keyDataWPA2.length = 22;
		keyDataWPA2.OUI[0] = 0x00;
		keyDataWPA2.OUI[1] = 0x0F;
		keyDataWPA2.OUI[2] = 0xAC;
		keyDataWPA2.dataType = 1;
		GTK_IE.keyID_Tx = mib->mib_MrvlRSN_GrpKey->g_KeyIndex; //5;
		GTK_IE.rsvd = 0;
		memcpy(GTK_IE.GTK, mib->mib_MrvlRSN_GrpKey->EncryptKey, 16); //WPA2
		pTx_eapol->key_material_len = 32;   
		memcpy(pTx_eapol->key_data, (UINT8*)&keyDataWPA2, 6);
		memcpy(pTx_eapol->key_data+6, (UINT8*)&GTK_IE, 18);

		if (mib_RSNConfigWPA2_p->MulticastCipher[3] == 2) 
		{
			keyDataWPA2.length = 22 + 16;
			pTx_eapol->key_material_len = 32+16;   
			memcpy(pTx_eapol->key_data, (UINT8*)&keyDataWPA2, 6);
			memcpy(pTx_eapol->key_data+6, (UINT8*)&GTK_IE, 18);
			memcpy(pTx_eapol->key_data+24, mib->mib_MrvlRSN_GrpKey->TxMICKey, 8); //WPA
			memcpy(pTx_eapol->key_data+32, mib->mib_MrvlRSN_GrpKey->RxMICKey, 8); //WPA
		}

	}

	if ( (pKMgmtStnInfo->RsnIEBuf[0] == 221 && pKMgmtStnInfo->RsnIEBuf[17] == 2)
		|| (pKMgmtStnInfo->RsnIEBuf[0] == 48 && pKMgmtStnInfo->RsnIEBuf[13] == 2)
		)
	{
		EncryptGrpKey((UINT8*)&(pKMgmtStnInfo->EAPOL_Encr_Key),
			(UINT8*)&(pTx_eapol->EAPOL_key_IV),
			(UINT8*)&(pTx_eapol->key_data), pTx_eapol->key_material_len);
	}
	else if ( (pKMgmtStnInfo->RsnIEBuf[0] == 221 && pKMgmtStnInfo->RsnIEBuf[17] == 4)
		|| (pKMgmtStnInfo->RsnIEBuf[0] == 48 && pKMgmtStnInfo->RsnIEBuf[13] == 4)
		)
	{
		memcpy(key, pKMgmtStnInfo->EAPOL_Encr_Key, 16);
		memcpy(plnText, pTx_eapol->key_data, pTx_eapol->key_material_len);
		AES_Wrap(plnText, (WRAPUINT64 *)cipherText, (WRAPUINT64 *)key, pTx_eapol->key_material_len - 8);
		memcpy(pTx_eapol->key_data, cipherText, pTx_eapol->key_material_len);
	}
	else
		return FAIL;

	frameLen = 95 + pTx_eapol->key_material_len;
	pTx_eapol->key_material_len = SHORT_SWAP(pTx_eapol->key_material_len);

	Insert8021xHdr(&(pTx_eapol->hdr_8021x), (UINT16)frameLen);
	ComputeEAPOL_MIC(vmacSta_p,(UINT8*)&(pTx_eapol->hdr_8021x), frameLen + sizeof(Hdr_8021x_t),
		pKMgmtStnInfo->EAPOL_MIC_Key, EAPOL_MIC_KEY_SIZE, MIC, pKMgmtStnInfo->RsnIEBuf);
	apppendEAPOL_MIC((UINT8*)&(pTx_eapol->key_MIC), MIC);

	skb->len = sizeof(ether_hdr_t) + sizeof (Hdr_8021x_t) + frameLen;
	return SUCCESS;
}

void KeyMgmtCleanUp(vmacApInfo_t *vmacSta_p,IEEEtypes_MacAddr_t *pStaAddr)
{
	extStaDb_StaInfo_t *pStaInfo;
	os_SemaphoreGet(sysinfo_STALIST_SEM);
	if ( (pStaInfo = extStaDb_GetStaInfo(vmacSta_p,pStaAddr, 0)) == NULL )
	{
		os_SemaphorePut(sysinfo_STALIST_SEM);
		/* Station not known, do nothing */
		return;
	}
	if (extStaDb_RemoveSta(vmacSta_p, (IEEEtypes_MacAddr_t *)pStaAddr) == LOCATE_FAILURE)
	{
		os_SemaphorePut(sysinfo_STALIST_SEM);
		/* Station not known, do nothing */
		return ;
	}

	/* Per FAE, this improves chances of group rekey disconnect failure */
	TimerRemove(&pStaInfo->keyMgmtHskHsm.keyTimer);

	os_SemaphorePut(sysinfo_STALIST_SEM);
}

MhsmEvent_t const *KeyMgmtHsk_top(keyMgmthsk_hsm_t *me, MhsmEvent_t *msg)
{
	switch (msg->event)
	{
	case MHSM_ENTER:
		mhsm_transition(&me->super, &me->hsk_start);
		return 0;
	} 
	return msg;
}      

MhsmEvent_t const *KeyMgmtHsk_Start(keyMgmthsk_hsm_t *me, MhsmEvent_t *msg_p)
{
	extStaDb_StaInfo_t *pStaInfo;//points to the 1st element of the data base
	//apio_bufdescr_t TxBuf;
#ifdef AP_MAC_LINUX
	struct sk_buff *skb = NULL;
	struct net_device *dev;
	dev = me->vmacSta_p->dev;
#endif
	switch (msg_p->event)
	{
	case STA_ASSO_EVT:
		pStaInfo = me->pData;//points to the 1st element of the data base
		pStaInfo->keyMgmtStateInfo.counter = lower_counter++;
		generateRand(pStaInfo->keyMgmtStateInfo.ANonce, NONCE_SIZE);
		if (GeneratePWKMsg1(me->vmacSta_p,&pStaInfo->keyMgmtStateInfo, &skb, &pStaInfo->Addr) != SUCCESS)
		{
			return msg_p;
		}
		mhsm_transition(&pStaInfo->keyMgmtHskHsm.super, &pStaInfo->keyMgmtHskHsm.waiting_4_msg_2);
		pStaInfo->keyMgmtHskHsm.timeout_ctr = 0;
		KeyMgmtStartTimer(me);
		txDataMsg_UnEncrypted(dev, skb, pStaInfo);
		return 0;
	} 
	return msg_p;
}

MhsmEvent_t const *KeyMgmtHsk_Wait_4_Msg2(keyMgmthsk_hsm_t *me, MhsmEvent_t *msg_p)
{
	MIB_RSNCONFIG	*mib_RSNConfig_p;
	MIB_RSNCONFIG_AUTH_SUITES		*mib_RSNConfigAuthSuites_p;
	MIB_RSNCONFIGWPA2			   *mib_RSNConfigWPA2_p;
	MIB_RSNCONFIGWPA2_AUTH_SUITES		*mib_RSNConfigWPA2AuthSuites_p;
	extStaDb_StaInfo_t *pStaInfo;//points to the 1st element of the data base
	//	apio_bufdescr_t TxBuf;
	EAPOL_KeyMsg_t *rx_eapol_ptr;
	UINT8 rx_MIC[EAPOL_MIC_SIZE], MIC[EAPOL_MIC_SIZE + 4];
	UINT8 PTK[100];
	UINT8 *PMK = NULL;
    UINT16 packetLen = 0;
#ifdef AP_MAC_LINUX
	struct sk_buff *skb = NULL;
	struct net_device *dev;
	dev = me->vmacSta_p->dev;
#endif

	mib_RSNConfig_p=me->vmacSta_p->Mib802dot11->RSNConfig ;
	mib_RSNConfigAuthSuites_p=me->vmacSta_p->Mib802dot11->RSNConfigAuthSuites;
	mib_RSNConfigWPA2_p=me->vmacSta_p->Mib802dot11->RSNConfigWPA2;
	mib_RSNConfigWPA2AuthSuites_p=me->vmacSta_p->Mib802dot11->WPA2AuthSuites;
	pStaInfo = me->pData;
	switch (msg_p->event)
	{
	case MSGRECVD_EVT:
		rx_eapol_ptr = (EAPOL_KeyMsg_t *)msg_p->pBody;
		memcpy(pStaInfo->keyMgmtStateInfo.SNonce, rx_eapol_ptr->key_nonce, NONCE_SIZE);
		
		if ( pStaInfo->keyMgmtStateInfo.RsnIEBuf[0] == 221 )
		{
			if ( mib_RSNConfigAuthSuites_p->Enabled &&
				memcmp(mib_RSNConfigAuthSuites_p->AuthSuites, AuthPSK, 4) == 0 )
			{
				PMK = mib_RSNConfig_p->PSKValue;
				//PMK = vmacSta_p->Mib802dot11->RSNConfig->PSKValue;
			}
			else if ( mib_RSNConfigAuthSuites_p->Enabled &&
				memcmp(mib_RSNConfigAuthSuites_p->AuthSuites, Auth8021x, 4) == 0 )
				PMK = pStaInfo->keyMgmtStateInfo.PMK;
			else
				return 0;
		}
		else if ( pStaInfo->keyMgmtStateInfo.RsnIEBuf[0] == 48 )
		{
			if ((mib_RSNConfigWPA2_p->WPA2Enabled || mib_RSNConfigWPA2_p->WPA2OnlyEnabled) 
				&& mib_RSNConfigWPA2AuthSuites_p->Enabled &&
				memcmp(mib_RSNConfigWPA2AuthSuites_p->AuthSuites, AuthPSKWPA2, 4) == 0)
				PMK = mib_RSNConfigWPA2_p->PSKValue;
			else if ((mib_RSNConfigWPA2_p->WPA2Enabled || mib_RSNConfigWPA2_p->WPA2OnlyEnabled) 
				&& mib_RSNConfigWPA2AuthSuites_p->Enabled &&
				memcmp(mib_RSNConfigWPA2AuthSuites_p->AuthSuites, Auth8021xWPA2, 4) == 0)
				PMK = pStaInfo->keyMgmtStateInfo.PMK;
			else
				return 0;
		}
		else
			return 0;
		genetate_PTK(me->vmacSta_p,PMK, &rx_eapol_ptr->Ether_Hdr.sa,
			&rx_eapol_ptr->Ether_Hdr.da,
			pStaInfo->keyMgmtStateInfo.ANonce,
			pStaInfo->keyMgmtStateInfo.SNonce, PTK);
		
		memcpy(pStaInfo->keyMgmtStateInfo.EAPOL_MIC_Key, PTK, 16);
		memcpy(pStaInfo->keyMgmtStateInfo.EAPOL_Encr_Key, (PTK + 16), 16);
		memcpy(pStaInfo->keyMgmtStateInfo.PairwiseTempKey1_tmp, (PTK + 16 + 16), TK_SIZE);
		memcpy(pStaInfo->keyMgmtStateInfo.RSNPwkTxMICKey_tmp, (PTK + 16 + 16 + TK_SIZE), 8);
		memcpy(pStaInfo->keyMgmtStateInfo.RSNPwkRxMICKey_tmp, (PTK + 16 + 16 + TK_SIZE + 8), 8);

		memcpy(rx_MIC, rx_eapol_ptr->key_MIC, EAPOL_MIC_SIZE);

        packetLen = (UINT16)SHORT_SWAP(rx_eapol_ptr->hdr_8021x.pckt_body_len) + sizeof(Hdr_8021x_t);

        /* discard packet quietly if packet length is larger than 256 bytes */
        /* underlying MIC algorithm works with a text size of 300 bytes and anything larger than 256 is abnormal */
        if (packetLen > 300)
        {
			WLDBG_INFO(DBG_LEVEL_1, "Abnormal packet length \n");            
			return msg_p;
        }

		ComputeEAPOL_MIC(me->vmacSta_p,(UINT8*)&(rx_eapol_ptr->hdr_8021x),
			packetLen,
			pStaInfo->keyMgmtStateInfo.EAPOL_MIC_Key, EAPOL_MIC_KEY_SIZE, MIC, 
			pStaInfo->keyMgmtStateInfo.RsnIEBuf);

		if (checkEAPOL_MIC(MIC, rx_MIC, EAPOL_MIC_SIZE) != SUCCESS) //verify MIC
		{
			// free buffer,
			WLDBG_INFO(DBG_LEVEL_5, "EAPoL Comparison MIC fail 4 \n");
			WLSYSLOG(me->vmacSta_p->dev,WLSYSLOG_CLASS_ALL, WLSYSLOG_MSG_PSK_MSG2_FAILMIC "%02x%02x%02x%02x%02x%02x\n",
				rx_eapol_ptr->Ether_Hdr.sa[0], rx_eapol_ptr->Ether_Hdr.sa[1], 
				rx_eapol_ptr->Ether_Hdr.sa[2], rx_eapol_ptr->Ether_Hdr.sa[3],
				rx_eapol_ptr->Ether_Hdr.sa[4], rx_eapol_ptr->Ether_Hdr.sa[5]); 
			WLSNDEVT(me->vmacSta_p->dev,IWEVCUSTOM, &rx_eapol_ptr->Ether_Hdr.sa, WLSYSLOG_MSG_PSK_MSG2_FAILMIC);
			return msg_p;
		}

		if (CompareRSN_IE((UINT8*)rx_eapol_ptr->key_data, pStaInfo->keyMgmtStateInfo.RsnIEBuf) != SUCCESS)
		{
			macMgmtMlme_SendDeauthenticateMsg(me->vmacSta_p,&pStaInfo->Addr, pStaInfo->StnId,
				IEEEtypes_REASON_IE_4WAY_DIFF);
			KeyMgmtCleanUp(me->vmacSta_p,&pStaInfo->Addr);
			//free buffer
			WLDBG_INFO(DBG_LEVEL_5, "Compare RSN_IE fail \n");
			WLSYSLOG(me->vmacSta_p->dev,WLSYSLOG_CLASS_ALL, WLSYSLOG_MSG_PSK_MSG2_FAILRSNIE "%02x%02x%02x%02x%02x%02x\n",
				rx_eapol_ptr->Ether_Hdr.sa[0], rx_eapol_ptr->Ether_Hdr.sa[1], 
				rx_eapol_ptr->Ether_Hdr.sa[2], rx_eapol_ptr->Ether_Hdr.sa[3],
				rx_eapol_ptr->Ether_Hdr.sa[4], rx_eapol_ptr->Ether_Hdr.sa[5]);
			WLSNDEVT(me->vmacSta_p->dev,IWEVCUSTOM, &rx_eapol_ptr->Ether_Hdr.sa, WLSYSLOG_MSG_PSK_MSG2_FAILRSNIE);
			return msg_p;
		}
		pStaInfo->keyMgmtStateInfo.counter = lower_counter++;
		if (GeneratePWKMsg3(me->vmacSta_p,&pStaInfo->keyMgmtStateInfo, &skb, &pStaInfo->Addr) != SUCCESS)
		{
			WLDBG_INFO(DBG_LEVEL_5, "GeneratePWKMsg3 fail. \n");
			return msg_p;
		}
		me->timeout_ctr = 0;
		mhsm_transition(&me->super, &me->waiting_4_msg_4);
		KeyMgmtStartTimer(me);
		txDataMsg_UnEncrypted(dev,skb, pStaInfo);
		return 0;
	case Timeout:
		WLDBG_INFO(DBG_LEVEL_5, "Timeout \n");
		me->timeout_ctr++;
		if (me->timeout_ctr > 2)
		{
			macMgmtMlme_SendDeauthenticateMsg(me->vmacSta_p,&pStaInfo->Addr, pStaInfo->StnId, IEEEtypes_REASON_4WAY_HANDSHK_TIMEOUT);
			KeyMgmtCleanUp(me->vmacSta_p,&pStaInfo->Addr);
			mhsm_transition(&me->super, &me->hsk_end);
		}
		else
		{
			pStaInfo->keyMgmtStateInfo.counter = lower_counter++;
			if (GeneratePWKMsg1(me->vmacSta_p,&pStaInfo->keyMgmtStateInfo, &skb, &pStaInfo->Addr) != SUCCESS)
			{
				return msg_p;
			}
			KeyMgmtStartTimer(me); 
			txDataMsg_UnEncrypted(dev, skb, pStaInfo);
		}
		return 0;
	}
	return msg_p;
}

MhsmEvent_t const *KeyMgmtHsk_Wait_4_Msg4(keyMgmthsk_hsm_t *me, MhsmEvent_t *msg_p) 
{
	extStaDb_StaInfo_t *pStaInfo;//points to the 1st element of the data base
	//	apio_bufdescr_t TxBuf;
	EAPOL_KeyMsg_t *rx_eapol_ptr;
	UINT8 rx_MIC[EAPOL_MIC_SIZE], MIC[EAPOL_MIC_SIZE + 4];
    UINT16 packetLen=0;
#ifdef AP_MAC_LINUX
	struct sk_buff *skb = NULL;
	struct net_device *dev;
	dev = me->vmacSta_p->dev;
#endif

	pStaInfo = me->pData;
	switch (msg_p->event)
	{
	case MSGRECVD_EVT:
		rx_eapol_ptr = (EAPOL_KeyMsg_t *)msg_p->pBody;

        packetLen = (UINT16)SHORT_SWAP(rx_eapol_ptr->hdr_8021x.pckt_body_len) + sizeof(Hdr_8021x_t);

        /* discard packet quietly if packet length is larger than 256 bytes */
        /* underlying MIC algorithm works with a text size of 300 bytes and anything larger than 256 is abnormal */
        if (packetLen > 300)
        {
			WLDBG_INFO(DBG_LEVEL_1, "Abnormal packet length \n");            
			return msg_p;
        }

		memcpy(rx_MIC, rx_eapol_ptr->key_MIC, EAPOL_MIC_SIZE);
		ComputeEAPOL_MIC(me->vmacSta_p,(UINT8*)&(rx_eapol_ptr->hdr_8021x),
			packetLen,
			pStaInfo->keyMgmtStateInfo.EAPOL_MIC_Key, EAPOL_MIC_KEY_SIZE, MIC, 
			pStaInfo->keyMgmtStateInfo.RsnIEBuf);

		if (checkEAPOL_MIC(MIC, rx_MIC, EAPOL_MIC_SIZE) != SUCCESS) //verify MIC
		{
			WLDBG_INFO(DBG_LEVEL_5, "EAPoL Comparison MIC fail. \n");
			WLSYSLOG(me->vmacSta_p->dev,WLSYSLOG_CLASS_ALL, WLSYSLOG_MSG_PSK_MSG4_FAILMIC "%02x%02x%02x%02x%02x%02x\n",
				rx_eapol_ptr->Ether_Hdr.sa[0], rx_eapol_ptr->Ether_Hdr.sa[1], 
				rx_eapol_ptr->Ether_Hdr.sa[2], rx_eapol_ptr->Ether_Hdr.sa[3],
				rx_eapol_ptr->Ether_Hdr.sa[4], rx_eapol_ptr->Ether_Hdr.sa[5]);
			WLSNDEVT(me->vmacSta_p->dev,IWEVCUSTOM, &rx_eapol_ptr->Ether_Hdr.sa, WLSYSLOG_MSG_PSK_MSG4_FAILMIC);
			return msg_p;
		}

		memcpy(pStaInfo->keyMgmtStateInfo.PairwiseTempKey1, pStaInfo->keyMgmtStateInfo.PairwiseTempKey1_tmp, TK_SIZE);
		memcpy(pStaInfo->keyMgmtStateInfo.RSNPwkTxMICKey, pStaInfo->keyMgmtStateInfo.RSNPwkTxMICKey_tmp, 8);
		memcpy(pStaInfo->keyMgmtStateInfo.RSNPwkRxMICKey, pStaInfo->keyMgmtStateInfo.RSNPwkRxMICKey_tmp, 8);

		pStaInfo->keyMgmtStateInfo.RSNDataTrafficEnabled = TRUE;
		pStaInfo->keyMgmtStateInfo.TxIV16 = 0x0001;
		pStaInfo->keyMgmtStateInfo.TxIV32 = 0;
		pStaInfo->keyMgmtStateInfo.RxIV32 = 0xFFFFFFFF;
		if ( (pStaInfo->keyMgmtStateInfo.RsnIEBuf[0] == 221 && pStaInfo->keyMgmtStateInfo.RsnIEBuf[17] == 2)
			|| (pStaInfo->keyMgmtStateInfo.RsnIEBuf[0] == 48  && pStaInfo->keyMgmtStateInfo.RsnIEBuf[13] == 2))
		{   
			// TKIP disable non -Marvell client aggregation
			if(!pStaInfo->IsStaMSTA)
			{
				if(pStaInfo->aggr11n.threshold)
				{
					pStaInfo->aggr11n.threshold = 0;
					pStaInfo->aggr11n.thresholdBackUp = pStaInfo->aggr11n.threshold;
				}
			}
		}
		wlFwSetWpaWpa2PWK(me->vmacSta_p->dev, pStaInfo);

		WLSYSLOG(me->vmacSta_p->dev,WLSYSLOG_CLASS_ALL, WLSYSLOG_MSG_PSK_SUCCESS "%02x%02x%02x%02x%02x%02x\n",
			rx_eapol_ptr->Ether_Hdr.sa[0], rx_eapol_ptr->Ether_Hdr.sa[1], 
			rx_eapol_ptr->Ether_Hdr.sa[2], rx_eapol_ptr->Ether_Hdr.sa[3],
			rx_eapol_ptr->Ether_Hdr.sa[4], rx_eapol_ptr->Ether_Hdr.sa[5]);
		WLSNDEVT(me->vmacSta_p->dev,IWEVCUSTOM, &rx_eapol_ptr->Ether_Hdr.sa, WLSYSLOG_MSG_PSK_SUCCESS);

		if ( pStaInfo->keyMgmtStateInfo.RsnIEBuf[0] == 48 )
		{
			KeyMgmtTimerDelete(me);
			mhsm_transition(&me->super, &me->hsk_end);
			gGrpKeyInstalled =  TRUE;
			return 0;
		}
		else
		{
			pStaInfo->keyMgmtStateInfo.counter = lower_counter++;
			if (GenerateGrpMsg1(me->vmacSta_p,&pStaInfo->keyMgmtStateInfo, &skb, &pStaInfo->Addr) != SUCCESS)
			{
				//free buffer
				return 0;
			}
			me->timeout_ctr = 0;
			mhsm_transition(&me->super, &me->waiting_4_grpmsg_2);
			KeyMgmtStartTimer(me);
			txDataMsg(dev, skb);
			return 0;
		}

	case Timeout:
		me->timeout_ctr++;
		if (me->timeout_ctr > 2)
		{
			macMgmtMlme_SendDeauthenticateMsg(me->vmacSta_p,&pStaInfo->Addr, pStaInfo->StnId, IEEEtypes_REASON_4WAY_HANDSHK_TIMEOUT);
			KeyMgmtCleanUp(me->vmacSta_p,&pStaInfo->Addr);
			mhsm_transition(&me->super, &me->hsk_end);
		}
		else
		{
			pStaInfo->keyMgmtStateInfo.counter = lower_counter++;
			if (GeneratePWKMsg3(me->vmacSta_p,&pStaInfo->keyMgmtStateInfo, &skb, &pStaInfo->Addr) != SUCCESS)
			{
				return msg_p;
			}
			KeyMgmtStartTimer(me);
			txDataMsg(dev, skb);
		}
		return 0;
	}
	return msg_p;
}

MhsmEvent_t const *KeyMgmtHsk_Wait_4_GrpMsg2(keyMgmthsk_hsm_t *me, MhsmEvent_t *msg_p) 
{
	extStaDb_StaInfo_t *pStaInfo;//points to the 1st element of the data base
	//	apio_bufdescr_t TxBuf;
	EAPOL_KeyMsg_t *rx_eapol_ptr;
	UINT8 rx_MIC[EAPOL_MIC_SIZE], MIC[EAPOL_MIC_SIZE + 4];
    UINT16 packetLen=0;
#ifdef AP_MAC_LINUX
	struct sk_buff *skb = NULL;
	struct net_device *dev;
	dev = me->vmacSta_p->dev;
#endif

	pStaInfo = me->pData;
	switch (msg_p->event)
	{
	case MSGRECVD_EVT:
		rx_eapol_ptr = (EAPOL_KeyMsg_t *)msg_p->pBody;

        packetLen = (UINT16)SHORT_SWAP(rx_eapol_ptr->hdr_8021x.pckt_body_len) + sizeof(Hdr_8021x_t);

        /* discard packet quietly if packet length is larger than 256 bytes */
        /* underlying MIC algorithm works with a text size of 300 bytes and anything larger than 256 is abnormal */
        if (packetLen > 300)
        {
			WLDBG_INFO(DBG_LEVEL_1, "Abnormal packet length \n");            
			return msg_p;
        }

		memcpy(rx_MIC, rx_eapol_ptr->key_MIC, EAPOL_MIC_SIZE);
		ComputeEAPOL_MIC(me->vmacSta_p,(UINT8*)&(rx_eapol_ptr->hdr_8021x),
			packetLen,
			pStaInfo->keyMgmtStateInfo.EAPOL_MIC_Key, EAPOL_MIC_KEY_SIZE, MIC, 
			pStaInfo->keyMgmtStateInfo.RsnIEBuf);

		if (checkEAPOL_MIC(MIC, rx_MIC, EAPOL_MIC_SIZE) != SUCCESS) //verify MIC
		{
			WLDBG_INFO(DBG_LEVEL_5, "EAPoL Comparison MIC fail. \n");
			WLSYSLOG(me->vmacSta_p->dev,WLSYSLOG_CLASS_ALL, WLSYSLOG_MSG_PSK_GRPKEY_FAILMIC "%02x%02x%02x%02x%02x%02x\n",
				rx_eapol_ptr->Ether_Hdr.sa[0], rx_eapol_ptr->Ether_Hdr.sa[1], 
				rx_eapol_ptr->Ether_Hdr.sa[2], rx_eapol_ptr->Ether_Hdr.sa[3],
				rx_eapol_ptr->Ether_Hdr.sa[4], rx_eapol_ptr->Ether_Hdr.sa[5]);
			WLSNDEVT(me->vmacSta_p->dev,IWEVCUSTOM, &rx_eapol_ptr->Ether_Hdr.sa, WLSYSLOG_MSG_PSK_GRPKEY_FAILMIC);
			return msg_p;
		}
		KeyMgmtTimerDelete(me);
		mhsm_transition(&me->super, &me->hsk_end);
		gGrpKeyInstalled =  TRUE;

		WLSYSLOG(me->vmacSta_p->dev,WLSYSLOG_CLASS_ALL, WLSYSLOG_MSG_PSK_GRPKEY_SUCCESS  "%02x%02x%02x%02x%02x%02x\n",
			rx_eapol_ptr->Ether_Hdr.sa[0], rx_eapol_ptr->Ether_Hdr.sa[1], 
			rx_eapol_ptr->Ether_Hdr.sa[2], rx_eapol_ptr->Ether_Hdr.sa[3],
			rx_eapol_ptr->Ether_Hdr.sa[4], rx_eapol_ptr->Ether_Hdr.sa[5]);
		WLSNDEVT(me->vmacSta_p->dev,IWEVCUSTOM, &rx_eapol_ptr->Ether_Hdr.sa, WLSYSLOG_MSG_PSK_GRPKEY_SUCCESS);

		return 0;
	case Timeout:
		WLDBG_INFO(DBG_LEVEL_5, "Timeout \n");

		me->timeout_ctr++;
		if (me->timeout_ctr > 8)//increasing the timeout value. Ori value is 5 and change to 8 for safe way.
		{
			macMgmtMlme_SendDeauthenticateMsg(me->vmacSta_p,&pStaInfo->Addr, pStaInfo->StnId, IEEEtypes_REASON_GRP_KEY_UPD_TIMEOUT);
			KeyMgmtCleanUp(me->vmacSta_p,&pStaInfo->Addr);
			mhsm_transition(&me->super, &me->hsk_end);
		}
		else
		{
			pStaInfo->keyMgmtStateInfo.counter = lower_counter++;
			if (GenerateGrpMsg1(me->vmacSta_p,&pStaInfo->keyMgmtStateInfo, &skb, &pStaInfo->Addr) != SUCCESS)
			{
				return msg_p;
			}
			KeyMgmtStartTimer(me);
			txDataMsg(dev, skb);
		}
		return 0;
	}
	return msg_p;
}

MhsmEvent_t const *KeyMgmtHsk_End(keyMgmthsk_hsm_t *me, MhsmEvent_t *msg_p) 
{
	extStaDb_StaInfo_t *pStaInfo;//points to the 1st element of the data base
#ifdef AP_MAC_LINUX
	struct sk_buff *skb = NULL;
	struct net_device *dev;
	dev = me->vmacSta_p->dev;
#endif

	pStaInfo = me->pData;
	switch (msg_p->event)
	{
	case GRPKEYTIMEOUT_EVT:
		WLDBG_INFO(DBG_LEVEL_5, "KeyMgmtHsk_End : GRPKEYTIMEOUT_EVT \n");

		WLSYSLOG(me->vmacSta_p->dev,WLSYSLOG_CLASS_ALL, WLSYSLOG_MSG_PSK_GRPKEY_UPDATE "%02x%02x%02x%02x%02x%02x\n",
			pStaInfo->Addr[0], pStaInfo->Addr[1], 
			pStaInfo->Addr[2], pStaInfo->Addr[3],
			pStaInfo->Addr[4], pStaInfo->Addr[5]);

		WLSNDEVT(me->vmacSta_p->dev,IWEVCUSTOM, &bcast, WLSYSLOG_MSG_PSK_GRPKEY_UPDATE);
		pStaInfo->keyMgmtStateInfo.counter = lower_counter++;
		if (GenerateGrpMsg1(me->vmacSta_p,&pStaInfo->keyMgmtStateInfo, &skb, &pStaInfo->Addr) != SUCCESS)
		{
			return 0;
		}
		me->timeout_ctr = 0;
		mhsm_transition(&me->super, &me->waiting_4_grpmsg_2);
		KeyMgmtStartTimer(me);
		txDataMsg(dev, skb);
		return 0;
	case UPDATEKEYS_EVT:
		WLDBG_INFO(DBG_LEVEL_5, "KeyMgmtHsk_End : UPDATEKEYS_EVT \n");
		me->timeout_ctr = 0;
		mhsm_transition(&me->super, &me->waiting_4_msg_2);
		return 0;
	}
	return msg_p;
}

void KeyMgmtHskCtor(vmacApInfo_t *vmacSta_p, extStaDb_StaInfo_t *pStaInfo)
{
	keyMgmthsk_hsm_t *me = &pStaInfo->keyMgmtHskHsm;

	mhsm_add(&me->sTop, NULL, (MhsmFcnPtr)KeyMgmtHsk_top);

	mhsm_add(&me->hsk_start, 
		&me->sTop, (MhsmFcnPtr)KeyMgmtHsk_Start);
	mhsm_add(&me->waiting_4_msg_2, &me->sTop, 
		(MhsmFcnPtr)KeyMgmtHsk_Wait_4_Msg2);
	mhsm_add(&me->waiting_4_msg_4,  &me->sTop, 
		(MhsmFcnPtr)KeyMgmtHsk_Wait_4_Msg4);
	mhsm_add(&me->waiting_4_grpmsg_2,  &me->sTop, 
		(MhsmFcnPtr)KeyMgmtHsk_Wait_4_GrpMsg2);
	mhsm_add(&me->hsk_end, &me->sTop, 
		(MhsmFcnPtr)KeyMgmtHsk_End);
	//with init, delete is not needed.
	KeyMgmtTimerInit(me);
	/* For a corner in case previous instance of timer not cleaned up */
	//KeyMgmtTimerDelete(me);
	me->pData = pStaInfo;
	me->vmacSta_p = vmacSta_p;
}      

void ProcessKeyMgmtData(vmacApInfo_t *vmacSta_p,void *pBuffDesc,
						IEEEtypes_MacAddr_t *SourceAddr,
						MhsmEvent_t *msg)
{
	MIB_RSNSTATS *mib_RSNStats_p=vmacSta_p->Mib802dot11->RSNStats;
	extStaDb_StaInfo_t *pStaInfo;

	pStaInfo = extStaDb_GetStaInfo(vmacSta_p,SourceAddr, 0);
	if (pStaInfo && pStaInfo->State == ASSOCIATED)
	{
		WLDBG_INFO(DBG_LEVEL_5, "ProcessKeyMgmtData: Station ASSOCIATED \n");
		if (msg->event == MSGRECVD_EVT)
		{
			EAPOL_KeyMsg_t *rx_eapol_ptr;

			rx_eapol_ptr = (EAPOL_KeyMsg_t *)msg->pBody;
            /* peteh: only accept MIC failure report comes from controlled port */
			if (rx_eapol_ptr->k.key_info16 & KEY_INFO_ERROR &&
				pStaInfo->keyMgmtStateInfo.RSNDataTrafficEnabled == TRUE)
			{
				mib_RSNStats_p->TKIPRemoteMICFailures++;
				MICCounterMeasureInvoke(vmacSta_p);
				return;
			}
			if (rx_eapol_ptr->k.key_info16 & KEY_INFO_REQUEST)
			{
			}

			if (WORD_SWAP(rx_eapol_ptr->replay_cnt[1]) != pStaInfo->keyMgmtStateInfo.counter &&
				rx_eapol_ptr->k.key_info.key_type == 1)
			{
				return;
			}
		}
		mhsm_send_event(&pStaInfo->keyMgmtHskHsm.super, msg);
	}
}

void KeyMgmt_TimeoutHdlr(void *data_p)
{
	distQ_TimerMsg_t * timerData_p;
	extStaDb_StaInfo_t *pStaInfo;//points to the 1st element of the data base
	DistTaskMsg_t DistMsg;
	DistTaskMsg_t *pDistMsg =  &DistMsg;

	pDistMsg->MsgType = TIMERMSGRECVD;
	timerData_p = &pDistMsg->msg.distQ_TimerMsg;
	pStaInfo = ((keyMgmthsk_hsm_t *)data_p)->pData;
	memcpy(timerData_p->PendingData_p.StnAddr, pStaInfo->Addr, sizeof(IEEEtypes_MacAddr_t));
	timerData_p->PendingData_p.type = KEYMGMTTIMEOUTEVENT;

	//Process Key Management msg 
	keyMgmt_msg(((keyMgmthsk_hsm_t *)data_p)->vmacSta_p,pDistMsg);
}

void keyMgmt_Timeout(vmacApInfo_t *vmacSta_p,distQ_TimerMsg_t *TimerMsg_p)
{
	MIB_802DOT11 *mib = vmacSta_p->Mib802dot11;
	MIB_RSNCONFIG	*mib_RSNConfig_p=vmacSta_p->Mib802dot11->RSNConfig ;
	MIB_RSNCONFIGWPA2			   *mib_RSNConfigWPA2_p=vmacSta_p->Mib802dot11->RSNConfigWPA2;
	MhsmEvent_t msg;

	msg.event = Timeout;
#ifdef AP_MAC_LINUX
	msg.devinfo = (void *) vmacSta_p;
#endif

	if (TimerMsg_p->PendingData_p.type == KEYMGMTTIMEOUTEVENT)
	{
		ProcessKeyMgmtData(vmacSta_p,NULL, &TimerMsg_p->PendingData_p.StnAddr,
			&msg);
	}
	else if (TimerMsg_p->PendingData_p.type == GRPKEYTIMEOUTEVENT)
	{
		/* To disable broadcast/multicast packet at least until first client finish re-key */
		gGrpKeyInstalled = FALSE;

		vmacSta_p->g_IV16 = 1; //this needs to be reset. I dont know why????
		vmacSta_p->g_IV32 = 0;
		vmacSta_p->MICCounterMeasureEnabled = 0; //disable MIC counter measure module
		GenerateGrpTransKey(vmacSta_p);
		if ((!mib_RSNConfigWPA2_p->WPA2Enabled && !mib_RSNConfigWPA2_p->WPA2OnlyEnabled) ||
			(mib_RSNConfigWPA2_p->WPA2Enabled && !mib_RSNConfigWPA2_p->WPA2OnlyEnabled))
		{
			if (mib_RSNConfig_p->MulticastCipher[3] == 2)
				wlFwSetWpaTkipGroupK(vmacSta_p->dev, mib->mib_MrvlRSN_GrpKey->g_KeyIndex);
			else
				wlFwSetWpaAesGroupK(vmacSta_p->dev, mib->mib_MrvlRSN_GrpKey->g_KeyIndex);
		}
		else if (mib_RSNConfigWPA2_p->WPA2OnlyEnabled)
		{
			wlFwSetWpaAesGroupK(vmacSta_p->dev, mib->mib_MrvlRSN_GrpKey->g_KeyIndex);
		}
		extStaDb_SendGrpKeyMsgToAllSta(vmacSta_p);
		vmacSta_p->MICCounterMeasureEnabled = 1; //enable MIC counter measure module
	}
}

void MICCounterMeasureInvoke(vmacApInfo_t *vmacSta_p)
{
	MIC_Fail_State_e status;
	static BOOLEAN MIC_ErrorTimerInit = FALSE;

	if (vmacSta_p->MICCounterMeasureEnabled)
	{
		if (!MIC_ErrorTimerInit)
		{
			TimerInit(&vmacSta_p->MIC_Errortimer);
			MIC_ErrorTimerInit = TRUE;
		}
		ENTER_CRITICAL;
		status = vmacSta_p->MIC_Errorstatus;
		EXIT_CRITICAL;

		switch (status)
		{
		case NO_MIC_FAILURE:
			TimerFireIn(&vmacSta_p->MIC_Errortimer, 1, &MicErrTimerExpCb, (unsigned char *)vmacSta_p, 600);
			ENTER_CRITICAL;
			vmacSta_p->MIC_Errorstatus = FIRST_MIC_FAIL_IN_60_SEC;
			EXIT_CRITICAL;
			break;
		case FIRST_MIC_FAIL_IN_60_SEC:

			vmacSta_p->MIC_ErrordisableStaAsso = 1;                           
			TimerRemove(&vmacSta_p->MIC_Errortimer);
			vmacSta_p->Mib802dot11->RSNStats->TKIPCounterMeasuresInvoked++;
			//send broadcast Deauthenticate msg
			macMgmtMlme_SendDeauthenticateMsg(vmacSta_p,&bcast, MCBC_STN_ID, 
				IEEEtypes_REASON_MIC_FAILURE);
			extStaDb_RemoveAllStns(vmacSta_p,IEEEtypes_REASON_MIC_FAILURE);
			ENTER_CRITICAL;
			vmacSta_p->MIC_Errorstatus = SECOND_MIC_FAIL_IN_60_SEC;
			EXIT_CRITICAL;
			//start timer for 60 seconds
			TimerRearm(&vmacSta_p->MIC_Errortimer, 600);
			break;
		case SECOND_MIC_FAIL_IN_60_SEC:
			ENTER_CRITICAL;
			vmacSta_p->MIC_Errorstatus = NO_MIC_FAILURE;
			EXIT_CRITICAL;
			break;
		default:
			break;

		}
	}
	return;
}

void SendKeyMgmtInitEvent(vmacApInfo_t *vmacSta_p)
{
	distQ_TimerMsg_t * timerData_p;
	DistTaskMsg_t DistMsg;
	DistTaskMsg_t *pDistMsg = &DistMsg;

	timerData_p = &pDistMsg->msg.distQ_TimerMsg;
	pDistMsg->MsgType = KEYMGMTINITMSGRECVD;

	//Process Key Management msg 
	keyMgmt_msg(vmacSta_p,pDistMsg);
}


void keyMgmt_msg(vmacApInfo_t *vmacSta_p, DistTaskMsg_t *pDistMsg)
{
	MIB_PRIVACY_TABLE *mib_PrivacyTable_p=vmacSta_p->Mib802dot11->Privacy;
	if ( mib_PrivacyTable_p->RSNEnabled ) 
	{
		if ( pDistMsg->MsgType == STA_ASSOMSGRECVD ) 
		{
			StaAssocStateMsg_t *staAssocMsg_p;

			staAssocMsg_p = &pDistMsg->msg.StaAssocStateMsg;
			WLDBG_INFO(DBG_LEVEL_5, "keyMgmt_msg: STA_ASSOMSGRECVD for = %x:%x:%x:%x:%x:%x \n",
				staAssocMsg_p->staMACAddr[0],
				staAssocMsg_p->staMACAddr[1],
				staAssocMsg_p->staMACAddr[2],
				staAssocMsg_p->staMACAddr[3],
				staAssocMsg_p->staMACAddr[4],
				staAssocMsg_p->staMACAddr[5]);
			ProcessEAPoLAp(vmacSta_p, NULL, &staAssocMsg_p->staMACAddr);
		}
		else if ( pDistMsg->MsgType == TIMERMSGRECVD ) 
		{
			distQ_TimerMsg_t *distTimerMsg_p = &pDistMsg->msg.distQ_TimerMsg;
			WLDBG_INFO(DBG_LEVEL_5, "keyMgmt_msg: TIMERMSGRECVD \n");
			keyMgmt_Timeout(vmacSta_p,distTimerMsg_p);
		}
		else if (pDistMsg->MsgType==KEYMGMTINITMSGRECVD) 
		{
			WLDBG_INFO(DBG_LEVEL_5, "keyMgmt_msg: KEYMGMTINITMSGRECVD \n");
			KeyMgmtInit(vmacSta_p);
		}
	}

}


/******************************************************************************
*
* Name: ProcessEAPoLAp 
*
* Description:
*   This routine is called to do 802.1x authentication and key managment. 
*
* Conditions For Use:
*   Should pass an EAPoL packet to this function.
*
* Arguments:
*   apio_bufdescr_t pointer to the incoming packet buffer.
*   Mac address pointer to the source Mac.  
*
* Return Value:
*   apio_bufdescr_t pointer to the outgoing packet buffer.
*
* Notes:
*   None.
*
*****************************************************************************/
void * ProcessEAPoLAp(vmacApInfo_t *vmacSta_p,IEEEtypes_8023_Frame_t *pEthFrame, IEEEtypes_MacAddr_t *pMacStaAddr)
{
	EAPOL_KeyMsg_t *pEAPoL;    
	MhsmEvent_t msg;

	if ( pEthFrame == NULL )
	{
        if (vmacSta_p->keyMgmtInitDone == TRUE)
	    	msg.event = STA_ASSO_EVT; /*Sta association event */
        else
		    return NULL; /* reject EAPOL */
	}
	else
	{
		pEAPoL = (EAPOL_KeyMsg_t *)pEthFrame;

        /* Do not proceed if keyMgmtInit is not complete */
		if ((pEAPoL->hdr_8021x.pckt_type == 0x03)
            && vmacSta_p->keyMgmtInitDone == TRUE)
		{/*key data */
			msg.event = MSGRECVD_EVT;
			msg.pBody = (unsigned char *)pEAPoL;
		}
		else
		{/*reject all other EAPoL packet for now.... */
			return NULL;
		}

	}
#ifdef AP_MAC_LINUX
	msg.devinfo = (void *) vmacSta_p;
#endif
	ProcessKeyMgmtData(vmacSta_p,NULL, pMacStaAddr, &msg);

	return NULL;
}




