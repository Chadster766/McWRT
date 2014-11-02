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
#include "keyMgmtSta.h"
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
#include "ap8xLnxXmit.h"
#include "wl_mib.h"
#include "mib.h"
#include "ap8xLnxIntf.h"
#include "ap8xLnxWlLog.h"
#include "ap8xLnxFwcmd.h"
#include "mlmeApi.h"
#if 1//port_it
#define PORT_TO_LINUX_OS   1
#define AP_MULTI_BSS_WEP 1
#endif //port_end

#define PWKMSG1_TKIP   0x8900
#define PWKMSG1_CCMP   0x8A00
#define PWKMSG3_TKIP_1 0x8901
#define PWKMSG3_TKIP_2 0xC901
#define PWKMSG3_TKIP_3 0xC913
#define PWKMSG3_CCMP_1 0xCA01
#define PWKMSG3_CCMP_2 0xCA13
#define GRPMSG1_TKIP_1 0xB103
#define GRPMSG1_TKIP_2 0xE103
#define GRPMSG1_TKIP_3 0xF103
#define GRPMSG1_TKIP_4 0x9103
#define GRPMSG1_TKIP_5 0x5103
#define GRPMSG1_TKIP_6 0xD103
#define GRPMSG1_TKIP_7 0xA103
#define GRPMSG1_TKIP_8 0x9113
#define GRPMSG1_TKIP_9 0x8113
#define GRPMSG1_CCMP_1 0x9203
#define GRPMSG1_CCMP_2 0xA203
#define GRPMSG1_CCMP_3 0xB203
#define GRPMSG1_CCMP_4 0x9213
#define GRPMSG1_CCMP_5 0xA213
#define GRPMSG1_CCMP_6 0xB213
#define GRPMSG1_CCMP_7 0x8213
#define MIC_ERROR_QUIET_TIME_INTERVAL     600 /* 60 sec */
#define MIC_ERROR_QUIET_TIME_INTERVAL_CAL     50 /* 5 sec */
UINT8 KDEOUI[3] = {0x00,0x0F,0xAC};
keyMgmtInfoSta_t     gkeyMgmtInfoSta[NUM_OF_WLMACS];
KeyData_t            gKeyData[NUM_OF_WLMACS];
MRVL_MIB_RSN_GRP_KEY mib_MrvlRSN_GrpKeyUr[NUM_OF_WLMACS];
MRVL_MIB_RSN_GRP_KEY mib_MrvlRSN_GrpKeyUr1[NUM_OF_WLMACS];
UINT8                PSKValueUr[NUM_OF_WLMACS][40];
UINT8                staMib_WPA_PSKValueEnabled[NUM_OF_WLMACS];
UINT8                staMib_WPA2_PSKValueEnabled[NUM_OF_WLMACS];
#ifdef SC_PALLADIUM  /* Increase RSN timeout for Palladium testing */
UINT32               gStaRsnSecuredTimeout = 1000; // 100 seconds for Palladium -  // 10 seconds timeout for completing RSN key handshake
#else
UINT32               gStaRsnSecuredTimeout = 100; // 10 seconds timeout for completing RSN key handshake
#endif

//extern MIC_Error_t  sta_MIC_Error;

UINT8 mib_defaultkeyindex_ur[NUM_OF_WLMACS];
UINT8 WepType_ur[NUM_OF_WLMACS][4];

Boolean keyMgmtBootInit = FALSE;
//extern Boolean gGrpKeyInstalled;


#ifdef REAUTH
Timer timer_reauth;
#endif

extern vmacEntry_t    vmacEntry_parent[2];

#ifndef PORT_TO_LINUX_OS
extern apio_handle_t  WlanHandle;
#endif /* PORT_TO_LINUX_OS */

#ifdef EURUS_SPECIAL_DEBUG_FW
UINT32 gDebug_DoNotRespondToKey1Message = 0;
UINT32 gDebug_DoNotRespondToKey3Message = 0;
UINT32 gDebug_DoNotRespondToGRP1Message = 0;
UINT32 gDebug_CorruptMIC_Msg2 = 0;
UINT32 gDebug_CorruptMIC_Msg4 = 0;
UINT32 gDebug_CorruptMIC_Grp2 = 0;
#endif


#ifndef PORT_TO_LINUX_OS

#else /* PORT_TO_LINUX_OS */

//extern it here for now

extern void KeyMgmtSta_InitSession(vmacEntry_t * vmacEntry_p);
extern struct sk_buff *ieee80211_getDataframe(UINT8 **frm, unsigned int pktlen);
extern void genetate_PTK(vmacApInfo_t *vmacSta_p,UINT8 *PMK, IEEEtypes_MacAddr_t *pAddr1,
						 IEEEtypes_MacAddr_t *pAddr2,
						 UINT8 *pNonce1, UINT8 *pNonce2, UINT8 *pPTK);

extern UINT16 AddRSN_IEWPA2_TO(IEEEtypes_RSN_IE_WPA2_t *thisStaRsnIEWPA2_p,IEEEtypes_RSN_IE_WPA2_t* pNextElement);
extern void AES_UnWrap(WRAPUINT64 *pPlData, WRAPUINT64 *pCipTxt, WRAPUINT64 *pKEK, UINT32 len);
extern void smeSndLinkLostInd(vmacEntry_t *vmacEntry_p, UINT16 reason);
#endif /* PORT_TO_LINUX_OS */

#ifdef PORT_TO_LINUX_OS

#if 1 // for testing purposes
#define TEST_RSN_WPA2   1

#ifdef TEST_RSN_WPA2
UINT8 rsnMultiCastCipher[RSN_CIPHER_VALUE_LEN_MAX] = {0x0, 0x0f, 0xac, 0x04};
UINT8 rsnUniCastCipher[RSN_CIPHER_VALUE_LEN_MAX] = {0x0, 0x0f, 0xac, 0x04};
UINT8 rsnAuthSuite[RSN_SUITE_VALUE_LEN_MAX] = {0x0, 0x0f, 0xac, 0x02};
#else
UINT8 rsnMultiCastCipher[RSN_CIPHER_VALUE_LEN_MAX] = {0x0, 0x50, 0xf2, 0x02};
UINT8 rsnUniCastCipher[RSN_CIPHER_VALUE_LEN_MAX] = {0x0, 0x50, 0xf2, 0x02};
UINT8 rsnAuthSuite[RSN_SUITE_VALUE_LEN_MAX] = {0x0, 0x50, 0xf2, 0x02};
#endif /* TEST_RSN_WPA2 */

#endif /* purpose end */


int keyMgmtTxData(struct sk_buff *skb, struct net_device *netdev, extStaDb_StaInfo_t *pStaInfo, BOOLEAN unencrypt)
{
	struct wlprivate *wlpptr = NETDEV_PRIV_P(struct wlprivate, netdev);
	unsigned long flags;
	UINT32 bcast = 0;

	WLDBG_ENTER(DBG_LEVEL_13);
	if ((netdev->flags & IFF_RUNNING) == 0)
	{
		wlpptr->netDevStats.tx_dropped++;
		WLDBG_EXIT_INFO(DBG_LEVEL_13, "%s: itf not running", netdev->name);
		return -ENETDOWN;
	}
	if ((skb = ieee80211_encap(skb, netdev, unencrypt)) == NULL)
	{
		goto error;
	}
	SPIN_LOCK_IRQSAVE(&wlpptr->wlpd_p->locks.xmitLock, flags);
	if(wlxmit(netdev, skb, IEEE_TYPE_DATA, pStaInfo, bcast, unencrypt))
	{
		WLDBG_INFO(DBG_LEVEL_13, "could not xmit");
		wlpptr->netDevStats.tx_errors++;
		SPIN_UNLOCK_IRQRESTORE(&wlpptr->wlpd_p->locks.xmitLock, flags);
		wlTxDone(netdev);
		goto error1;
	}
	SPIN_UNLOCK_IRQRESTORE(&wlpptr->wlpd_p->locks.xmitLock, flags);
	WLDBG_EXIT(DBG_LEVEL_13);
	return 0;

error:
error1:
	if (skb)
	{
#ifdef AMSDU_AGGREQ_FOR_8K
		if(skb->truesize > MAX_AGGR_SIZE)
		{
			skb_queue_tail(&wlpptr->wlpd_p->aggreQ, skb);
		}
		else
#endif
			dev_kfree_skb_any(skb);
	}
	WLDBG_EXIT_INFO(DBG_LEVEL_13, NULL);
	return 0;
}

extern WL_STATUS keyMgmtTxData_UnEncrypted(struct net_device *dev,struct sk_buff *skb, extStaDb_StaInfo_t *pStaInfo)
{
	WLDBG_INFO(DBG_LEVEL_11,"IEEE_TYPE_Data message txed skb = %x \n", skb);
	if (keyMgmtTxData(skb, dev, NULL/*pStaInfo*/, TRUE))
		dev_kfree_skb_any(skb);
	return(SUCCESS);
}

extern WL_STATUS keyMgmtTxData_Encrypted(struct net_device *dev,struct sk_buff *skb, extStaDb_StaInfo_t *pStaInfo)
{
	//	WL_STATUS status = FAIL;
	WLDBG_INFO(DBG_LEVEL_11,"IEEE_TYPE_Data message txed skb = %x \n", skb);
	if (keyMgmtTxData(skb, dev, pStaInfo, FALSE))
		dev_kfree_skb_any(skb);
	return(SUCCESS);
}

void keyMgmtUpdateRsnIE(UINT8 phymacIndex, 
						UINT8 rsn_modeId,
						UINT32 mCipherId,
						UINT32 uCipherId,
						UINT32 aCipherId)
{
	STA_SYSTEM_MIBS   * pStaSystemMibs;
	STA_SECURITY_MIBS * pStaSecurityMibs;

	pStaSystemMibs   = sme_GetStaSystemMibsPtr(&vmacEntry_parent[phymacIndex]);
	pStaSecurityMibs = sme_GetStaSecurityMibsPtr(&vmacEntry_parent[phymacIndex]);

	if(rsn_modeId == RSN_WPA_ID)
	{
		pStaSecurityMibs->thisStaRsnIE_p->ElemId = 221;
		pStaSecurityMibs->thisStaRsnIE_p->Len = sizeof(IEEEtypes_RSN_IE_t) - 2;
		pStaSecurityMibs->thisStaRsnIE_p->OuiType[0] = 0x0;
		pStaSecurityMibs->thisStaRsnIE_p->OuiType[1] = 0x50;
		pStaSecurityMibs->thisStaRsnIE_p->OuiType[2] = 0xf2;
		pStaSecurityMibs->thisStaRsnIE_p->OuiType[3] = 0x01;
		pStaSecurityMibs->thisStaRsnIE_p->Ver[0] = 0x01;
		pStaSecurityMibs->thisStaRsnIE_p->Ver[1] = 0x0;
		pStaSecurityMibs->thisStaRsnIE_p->GrpKeyCipher[0] = 0x00;
		pStaSecurityMibs->thisStaRsnIE_p->GrpKeyCipher[1] = 0x50;
		pStaSecurityMibs->thisStaRsnIE_p->GrpKeyCipher[2] = 0xf2;
		pStaSecurityMibs->thisStaRsnIE_p->GrpKeyCipher[3] = mCipherId;
		pStaSecurityMibs->thisStaRsnIE_p->PwsKeyCnt[0] = 0x01;
		pStaSecurityMibs->thisStaRsnIE_p->PwsKeyCnt[1] = 0x0;
		pStaSecurityMibs->thisStaRsnIE_p->PwsKeyCipherList[0] = 0x00;
		pStaSecurityMibs->thisStaRsnIE_p->PwsKeyCipherList[1] = 0x50;
		pStaSecurityMibs->thisStaRsnIE_p->PwsKeyCipherList[2] = 0xf2;
		pStaSecurityMibs->thisStaRsnIE_p->PwsKeyCipherList[3] = uCipherId;
		pStaSecurityMibs->thisStaRsnIE_p->AuthKeyCnt[0] = 0x01;
		pStaSecurityMibs->thisStaRsnIE_p->AuthKeyCnt[1] = 0x0;
		pStaSecurityMibs->thisStaRsnIE_p->AuthKeyList[0] = 0x00;
		pStaSecurityMibs->thisStaRsnIE_p->AuthKeyList[1] = 0x50;
		pStaSecurityMibs->thisStaRsnIE_p->AuthKeyList[2] = 0xf2;
		pStaSecurityMibs->thisStaRsnIE_p->AuthKeyList[3] = aCipherId;

		//pStaSecurityMibs->thisStaRsnIE_p->RsnCap[0] = 0x0;
		//pStaSecurityMibs->thisStaRsnIE_p->RsnCap[1] = 0x0;
		memcpy(pStaSecurityMibs->mib_RSNConfig_p->MulticastCipher,
			&pStaSecurityMibs->thisStaRsnIE_p->GrpKeyCipher[0],
			RSN_CIPHER_VALUE_LEN_MAX);

		memcpy(pStaSecurityMibs->mib_RSNConfigUnicastCiphers_p->UnicastCipher,
			&pStaSecurityMibs->thisStaRsnIE_p->PwsKeyCipherList[0],
			RSN_CIPHER_VALUE_LEN_MAX);
        
        /* Reset WPA2 ciphers */    
        memset(pStaSecurityMibs->mib_RSNConfigWPA2_p->MulticastCipher,
                0, RSN_CIPHER_VALUE_LEN_MAX);
        memset(pStaSecurityMibs->mib_RSNConfigWPA2UnicastCiphers_p->UnicastCipher,
                0, RSN_CIPHER_VALUE_LEN_MAX);

    }
	else if(rsn_modeId == RSN_WPA2_ID)
	{
		/* WPA2 */
		pStaSecurityMibs->thisStaRsnIEWPA2_p->ElemId = 48;
		pStaSecurityMibs->thisStaRsnIEWPA2_p->Len = 20;
		pStaSecurityMibs->thisStaRsnIEWPA2_p->Ver[0] = 0x01;
		pStaSecurityMibs->thisStaRsnIEWPA2_p->Ver[1] = 0x0;
		pStaSecurityMibs->thisStaRsnIEWPA2_p->GrpKeyCipher[0] = 0x00;
		pStaSecurityMibs->thisStaRsnIEWPA2_p->GrpKeyCipher[1] = 0x0f;
		pStaSecurityMibs->thisStaRsnIEWPA2_p->GrpKeyCipher[2] = 0xac;
		pStaSecurityMibs->thisStaRsnIEWPA2_p->GrpKeyCipher[3] = mCipherId;

		pStaSecurityMibs->thisStaRsnIEWPA2_p->PwsKeyCnt[0] = 0x01;
		pStaSecurityMibs->thisStaRsnIEWPA2_p->PwsKeyCnt[1] = 0x0;
		pStaSecurityMibs->thisStaRsnIEWPA2_p->PwsKeyCipherList[0] = 0x00;
		pStaSecurityMibs->thisStaRsnIEWPA2_p->PwsKeyCipherList[1] = 0x0f;
		pStaSecurityMibs->thisStaRsnIEWPA2_p->PwsKeyCipherList[2] = 0xac;
		pStaSecurityMibs->thisStaRsnIEWPA2_p->PwsKeyCipherList[3] = uCipherId;

		pStaSecurityMibs->thisStaRsnIEWPA2_p->AuthKeyCnt[0] = 0x01;
		pStaSecurityMibs->thisStaRsnIEWPA2_p->AuthKeyCnt[1] = 0x0;
		pStaSecurityMibs->thisStaRsnIEWPA2_p->AuthKeyList[0] = 0x00;
		pStaSecurityMibs->thisStaRsnIEWPA2_p->AuthKeyList[1] = 0x0f;
		pStaSecurityMibs->thisStaRsnIEWPA2_p->AuthKeyList[2] = 0xac;
		pStaSecurityMibs->thisStaRsnIEWPA2_p->AuthKeyList[3] = aCipherId;

		pStaSecurityMibs->thisStaRsnIEWPA2_p->RsnCap[0] = 0x0;
		pStaSecurityMibs->thisStaRsnIEWPA2_p->RsnCap[1] = 0x0;

		memcpy(pStaSecurityMibs->mib_RSNConfigWPA2_p->MulticastCipher,
			&pStaSecurityMibs->thisStaRsnIEWPA2_p->GrpKeyCipher[0],
			RSN_CIPHER_VALUE_LEN_MAX);

		memcpy(pStaSecurityMibs->mib_RSNConfigWPA2UnicastCiphers_p->UnicastCipher,
			&pStaSecurityMibs->thisStaRsnIEWPA2_p->PwsKeyCipherList[0],
			RSN_CIPHER_VALUE_LEN_MAX);

        /*Reset WPA ciphers */
        memset(pStaSecurityMibs->mib_RSNConfig_p->MulticastCipher,
                0, RSN_CIPHER_VALUE_LEN_MAX);
        memset(pStaSecurityMibs->mib_RSNConfigUnicastCiphers_p->UnicastCipher,
                0, RSN_CIPHER_VALUE_LEN_MAX);

	}



}

/*************************************************************************
* Function: keyMgmtStaStartTimer
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern void keyMgmtStaStartTimer(UINT8 *data_p, 
								 UINT8 *timer_p,
								 void *callback,
								 UINT32 ticks)
{
	TimerRemove((Timer *)timer_p);
	TimerInit((Timer *)timer_p);
	TimerFireIn((Timer *)timer_p, 1, 
		callback, 
		(UINT8 *)data_p, 
		ticks);
}

/*************************************************************************
* Function: keyMgmtStaStopTimer
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern void keyMgmtStaStopTimer(keyMgmtInfoSta_t * pKeyMgmtInfoSta, UINT8 *timer_p)
{
	TimerRemove((Timer *)timer_p);
}

//port_it:: need to determin initial parameters
void InitThisStaRsnIeSta(UINT8 phymacIndex)
{
	return;
}

extern void ComputeEAPOL_MIC_DualMacSta(UINT8 *data, UINT16 data_length,
										UINT8 *MIC_Key, UINT8 MIC_Key_length,
										UINT8 *computed_MIC, UINT8* RsnIEBuf, vmacEntry_t * vmacEntry_p)
{
	STA_SECURITY_MIBS * pStaSecurityMibs;

	pStaSecurityMibs = sme_GetStaSecurityMibsPtr(vmacEntry_p);

	if (RsnIEBuf) 
	{
		//zeroize the MIC key field before calculating the data
		memset(data + 77 + sizeof(Hdr_8021x_t), 0x00, EAPOL_MIC_SIZE);
		if (    (RsnIEBuf[0] == 221 && RsnIEBuf[17] == 2)
			|| (RsnIEBuf[0] == 48  && RsnIEBuf[13] == 2) )
		{
			Mrvl_hmac_md5(data,
				(int)data_length,
				MIC_Key,
				(int)MIC_Key_length,
				(void *)computed_MIC);
		}
		else if (   (RsnIEBuf[0] == 221 && RsnIEBuf[17] == 4)
			|| (RsnIEBuf[0] == 48  && RsnIEBuf[13] == 4) )
		{
			Mrvl_hmac_sha1(data,
				(int)data_length,
				MIC_Key,
				(int)MIC_Key_length,
				(void *)computed_MIC);
		}
	}
	else
	{
		memset(data + 77 + sizeof(Hdr_8021x_t), 0, EAPOL_MIC_SIZE);
		if (   (    !pStaSecurityMibs->mib_RSNConfigWPA2_p->WPA2OnlyEnabled
			&& !pStaSecurityMibs->mib_RSNConfigWPA2_p->WPA2Enabled
			&& (pStaSecurityMibs->mib_RSNConfigUnicastCiphers_p->UnicastCipher[3] == 2))

			|| (    (    pStaSecurityMibs->mib_RSNConfigWPA2_p->WPA2OnlyEnabled
			|| pStaSecurityMibs->mib_RSNConfigWPA2_p->WPA2Enabled )
			&& (pStaSecurityMibs->mib_RSNConfigWPA2UnicastCiphers_p->UnicastCipher[3] == 2)) )
		{
			Mrvl_hmac_md5(data,
				(int)data_length,
				MIC_Key,
				(int)MIC_Key_length,
				(void *)computed_MIC);
		}
		else
		{
			Mrvl_hmac_sha1(data,
				(int)data_length,
				MIC_Key,
				(int)MIC_Key_length,
				(void *)computed_MIC);
		}
	}
}

#endif /* PORT_TO_LINUX_OS */

static int isApReplayCounterFresh(keyMgmtInfoSta_t * pKeyMgmtInfoSta, UINT8 * pRxReplayCount )
{
	UINT32 tmpHi;
	UINT32 tmpLo;
	UINT32 rxCountHi;
	UINT32 rxCountLo;

	memcpy(&tmpHi, pRxReplayCount    , 4);
	memcpy(&tmpLo, pRxReplayCount + 4, 4);
	rxCountHi = WORD_SWAP(tmpHi);
	rxCountLo = WORD_SWAP(tmpLo);
	// check hi dword first
	if (rxCountHi > pKeyMgmtInfoSta->apCounterHi)
	{
		return 1; // fresh
	}
	if (rxCountHi < pKeyMgmtInfoSta->apCounterHi)
	{
		return 0; // stale
	}
	// hi dword is equal, check lo dword
	if (rxCountLo > pKeyMgmtInfoSta->apCounterLo)
	{
		return 1; // fresh
	}
	if (rxCountLo < pKeyMgmtInfoSta->apCounterLo)
	{
		return 0; // stale
	}
	// Counters are equal. Check special case of zero.
	if (   (rxCountHi == 0)
		&& (rxCountLo == 0)
		)
	{
		if (pKeyMgmtInfoSta->apCounterZeroDone)
		{
			return 0; // stale
		}
		else
		{
			return 1; // fresh
		}
	}
	// Non-zero but equal.
	return 0; // stale
}

static void updateApReplayCounter(keyMgmtInfoSta_t * pKeyMgmtStaInfo, UINT8 * pRxReplayCount)
{
	UINT32 tmpHi;
	UINT32 tmpLo;
	UINT32 rxCountHi;
	UINT32 rxCountLo;

	memcpy(&tmpHi, pRxReplayCount    , 4);
	memcpy(&tmpLo, pRxReplayCount + 4, 4);
	rxCountHi = WORD_SWAP(tmpHi);
	rxCountLo = WORD_SWAP(tmpLo);
	pKeyMgmtStaInfo->apCounterHi = rxCountHi;
	pKeyMgmtStaInfo->apCounterLo = rxCountLo;
	if (   (rxCountHi == 0)
		&& (rxCountLo == 0)
		)
	{
		pKeyMgmtStaInfo->apCounterZeroDone = 1;
	}
}

Status_e GeneratePWKMsg2(struct sk_buff **skb_pp, EAPOL_KeyMsg_t *pRxEapol, keyMgmtInfoSta_t * pKeyMgmtInfoSta)
{
	EAPOL_KeyMsg_t *    pTx_eapol;
	UINT32              frameLen;
	UINT8               MIC[EAPOL_MIC_SIZE + 4];
	//key_info_t          tmpKeyInfo;

#ifdef PORT_TO_LINUX_OS
	UINT8 *frm;
	struct sk_buff *skb_p;
#endif /* PORT_TO_LINUX_OS */

	vmacEntry_t *       vmacEntry_p      = (vmacEntry_t *)(pKeyMgmtInfoSta->vmacEntry_p);
	STA_SECURITY_MIBS * pStaSecurityMibs = sme_GetStaSecurityMibsPtr(vmacEntry_p);

#ifdef PORT_TO_LINUX_OS
	if ((skb_p = ieee80211_getDataframe(&frm, EAPOL_TX_BUF)) == NULL)
	{
		WLDBG_INFO(DBG_LEVEL_5, "Error: cannot get socket buffer. \n ");
		return FAIL;
	}
	*skb_pp = skb_p;
	pTx_eapol = (EAPOL_KeyMsg_t *)skb_p->data;
#else
	if (apio_alloc(WlanHandle, pTxFrm) != APCTL_OK)
	{
		return FAIL;
	}
	pTx_eapol = (EAPOL_KeyMsg_t *)pTxFrm->framePtr;
#endif /* PORT_TO_LINUX_OS */

	MACADDR_CPY(pTx_eapol->Ether_Hdr.da, GetParentStaBSSID(vmacEntry_p->phyHwMacIndx)/*g_pURbssid*/ );
	MACADDR_CPY(pTx_eapol->Ether_Hdr.sa, vmacEntry_p->vmacAddr);
	pTx_eapol->Ether_Hdr.type = IEEE_ETHERTYPE_PAE;
	if (pStaSecurityMibs->mib_RSNConfigWPA2_p->WPA2OnlyEnabled || pStaSecurityMibs->mib_RSNConfigWPA2_p->WPA2Enabled)
		pTx_eapol->desc_type = 2;
	else
		pTx_eapol->desc_type = 254;
	pTx_eapol->k.key_info16 = 0;
	if (   pStaSecurityMibs->mib_RSNConfigWPA2_p->WPA2OnlyEnabled
		|| pStaSecurityMibs->mib_RSNConfigWPA2_p->WPA2Enabled )
	{
		// WPA2
		if (pStaSecurityMibs->mib_RSNConfigWPA2UnicastCiphers_p->UnicastCipher[3] == 4)
			pTx_eapol->k.key_info.desc_ver = 2;  // CCMP
		else
			pTx_eapol->k.key_info.desc_ver = 1;  // TKIP
	}
	else
	{
		// WPA
		if ( pStaSecurityMibs->mib_RSNConfigUnicastCiphers_p->UnicastCipher[3] == 4)
			pTx_eapol->k.key_info.desc_ver = 2;  // CCMP
		else
			pTx_eapol->k.key_info.desc_ver = 1;  // TKIP
	}
	pTx_eapol->k.key_info.key_type = 1;
	pTx_eapol->k.key_info.key_MIC = 1;
	pTx_eapol->k.key_info16 = SHORT_SWAP(pTx_eapol->k.key_info16);

#ifdef PORT_TO_LINUX_OS
	//pTx_eapol->key_length = SHORT_SWAP((TK_SIZE + TK_SIZE));
	pTx_eapol->key_length = 0;
#else
	memcpy(&pTx_eapol->k.key_info16,
		&tmpKeyInfo.key_info16,
		sizeof(tmpKeyInfo));
	pTx_eapol->key_length = 0;
#endif /* PORT_TO_LINUX_OS */

	pTx_eapol->replay_cnt[0] = pRxEapol->replay_cnt[0];//0x00;
	pTx_eapol->replay_cnt[1] = pRxEapol->replay_cnt[1];//0x01000000;
	memcpy(pTx_eapol->key_nonce, pKeyMgmtInfoSta->SNonce, NONCE_SIZE);
	memset(pTx_eapol->EAPOL_key_IV,0,16);
	memset(pTx_eapol->key_RSC,0,8);
	memset(pTx_eapol->key_ID,0,8);
	if (   pStaSecurityMibs->mib_RSNConfigWPA2_p->WPA2OnlyEnabled
		|| pStaSecurityMibs->mib_RSNConfigWPA2_p->WPA2Enabled )
	{
#ifdef AP_MULTI_BSS
		pTx_eapol->key_material_len = AddRSN_IEWPA2_DualMacUr((IEEEtypes_RSN_IE_WPA2_t*)(&pTx_eapol->key_data), vmacEntry_p->phyHwMacIndx);
#else
		pTx_eapol->key_material_len = AddRSN_IEWPA2_TO( pStaSecurityMibs->thisStaRsnIEWPA2_p,(IEEEtypes_RSN_IE_WPA2_t*)(&pTx_eapol->key_data) );
#endif
	}
	else
	{
#ifdef AP_MULTI_BSS
		pTx_eapol->key_material_len = AddRSN_IE_DualMacUr(&pTx_eapol->key_data, vmacEntry_p->phyHwMacIndx);
#else
		pTx_eapol->key_material_len = AddRSN_IE_TO( pStaSecurityMibs->thisStaRsnIE_p,(IEEEtypes_RSN_IE_t *)&pTx_eapol->key_data);
#endif
	}
	frameLen = 95 + pTx_eapol->key_material_len;
	Insert8021xHdr(&pTx_eapol->hdr_8021x, (UINT16)frameLen);
	pTx_eapol->key_material_len = SHORT_SWAP(pTx_eapol->key_material_len);
#ifndef AP_MULTI_BSS_WEP
	ComputeEAPOL_MIC((UINT8 *)&pTx_eapol->hdr_8021x, frameLen + sizeof(Hdr_8021x_t),
		gkeyMgmtInfoSta[phymac].EAPOL_MIC_Key, EAPOL_MIC_KEY_SIZE, MIC, 0);
#else
	ComputeEAPOL_MIC_DualMacSta((UINT8 *)&pTx_eapol->hdr_8021x, frameLen + sizeof(Hdr_8021x_t),
		pKeyMgmtInfoSta->EAPOL_MIC_Key, EAPOL_MIC_KEY_SIZE, MIC, 0, vmacEntry_p);
#endif
#ifdef EURUS_SPECIAL_DEBUG_FW
	if( gDebug_CorruptMIC_Msg2 )
	{
		MIC[0] = ~(MIC[0]);
	}
#endif
	apppendEAPOL_MIC(pTx_eapol->key_MIC,MIC);
#ifdef PORT_TO_LINUX_OS
	skb_p->len = sizeof(ether_hdr_t) + sizeof (Hdr_8021x_t) + frameLen;
#else
	pTxFrm->frameLen = frameLen + sizeof(ether_hdr_t) + HDR_8021x_LEN;
#endif /* PORT_TO_LINUX_OS */
	return SUCCESS;
}

Status_e GeneratePWKMsg4(struct sk_buff **skb_pp, EAPOL_KeyMsg_t *pRxEapol, keyMgmtInfoSta_t * pKeyMgmtInfoSta)
{
	EAPOL_KeyMsg_t *    pTx_eapol;
	UINT32              frameLen;
	UINT8               MIC[EAPOL_MIC_SIZE + 4];
	vmacEntry_t *       vmacEntry_p      = (vmacEntry_t *)(pKeyMgmtInfoSta->vmacEntry_p);
	STA_SECURITY_MIBS * pStaSecurityMibs = sme_GetStaSecurityMibsPtr(vmacEntry_p);
#ifndef PORT_TO_LINUX_OS
	key_info_t          tmpKeyInfo;
#endif /* PORT_TO_LINUX_OS */

#ifdef PORT_TO_LINUX_OS
	UINT8 *frm;
	struct sk_buff *skb;

	if ((skb = ieee80211_getDataframe(&frm, EAPOL_TX_BUF)) == NULL)
	{
		WLDBG_INFO(DBG_LEVEL_5, "Error: cannot get socket buffer. \n ");
		return FAIL;
	}
	*skb_pp = skb;
	pTx_eapol = (EAPOL_KeyMsg_t *)skb->data;
#else
	if (apio_alloc(WlanHandle, pTxFrm) != APCTL_OK)
	{
		return FAIL;
	}
	pTx_eapol = (EAPOL_KeyMsg_t *)pTxFrm->framePtr;
#endif /* PORT_TO_LINUX_OS */

	MACADDR_CPY(pTx_eapol->Ether_Hdr.da, GetParentStaBSSID(vmacEntry_p->phyHwMacIndx));
	MACADDR_CPY(pTx_eapol->Ether_Hdr.sa, vmacEntry_p->vmacAddr);
	pTx_eapol->Ether_Hdr.type = IEEE_ETHERTYPE_PAE;//EAPOL Msg

	if (   pStaSecurityMibs->mib_RSNConfigWPA2_p->WPA2OnlyEnabled
		|| pStaSecurityMibs->mib_RSNConfigWPA2_p->WPA2Enabled)
	{
		pTx_eapol->desc_type = 2;
	}
	else
	{
		pTx_eapol->desc_type = 254;
	}
	pTx_eapol->k.key_info16= 0;
	if (   pStaSecurityMibs->mib_RSNConfigWPA2_p->WPA2OnlyEnabled
		|| pStaSecurityMibs->mib_RSNConfigWPA2_p->WPA2Enabled)
	{
		if ( pStaSecurityMibs->mib_RSNConfigWPA2UnicastCiphers_p->UnicastCipher[3] == 4)
		{
			pTx_eapol->k.key_info.desc_ver = 2;
		}
		else
		{
			pTx_eapol->k.key_info.desc_ver = 1;
		}
	}
	else
	{
		if ( pStaSecurityMibs->mib_RSNConfigUnicastCiphers_p->UnicastCipher[3] == 4)
		{
			pTx_eapol->k.key_info.desc_ver = 2;
		}
		else
		{
			pTx_eapol->k.key_info.desc_ver = 1;
		}
	}
	pTx_eapol->k.key_info.key_type = 1;
	pTx_eapol->k.key_info.key_MIC = 1;
	if (   pStaSecurityMibs->mib_RSNConfigWPA2_p->WPA2OnlyEnabled
		|| pStaSecurityMibs->mib_RSNConfigWPA2_p->WPA2Enabled)
	{
#ifndef PORT_TO_LINUX_OS
		key_info_t          tmpRx_KeyInfo;

		memcpy(&tmpRx_KeyInfo.key_info16,
			&pRxEapol->k.key_info16,
			sizeof(key_info_t));
#endif //port_end
		pRxEapol->k.key_info16 = SHORT_SWAP(pRxEapol->k.key_info16);
		// copy the secure bit from Rx to Tx
		pTx_eapol->k.key_info.secure = pRxEapol->k.key_info.secure;
		pRxEapol->k.key_info16 = SHORT_SWAP(pRxEapol->k.key_info16);
	}
	else
	{
		pTx_eapol->k.key_info.secure = 0;
	}
	pTx_eapol->k.key_info16 = SHORT_SWAP(pTx_eapol->k.key_info16);
#ifndef PORT_TO_LINUX_OS
	memcpy(&pTx_eapol->k.key_info16,
		&tmpKeyInfo.key_info16,
		sizeof(tmpKeyInfo));
#endif /* PORT_TO_LINUX_OS */
	pTx_eapol->key_length = 0;
	pTx_eapol->replay_cnt[0] = pRxEapol->replay_cnt[0];
	pTx_eapol->replay_cnt[1] = pRxEapol->replay_cnt[1];
	memset(pTx_eapol->key_nonce, 0, NONCE_SIZE);
	memset(pTx_eapol->EAPOL_key_IV,0,16);
	memset(pTx_eapol->key_RSC,0,8);
	memset(pTx_eapol->key_ID,0,8);
	pTx_eapol->key_material_len = 0;
	frameLen = 95;
	Insert8021xHdr(&pTx_eapol->hdr_8021x, (UINT16)frameLen);
	pTx_eapol->key_material_len = SHORT_SWAP(pTx_eapol->key_material_len);
#ifndef AP_MULTI_BSS_WEP
	ComputeEAPOL_MIC((UINT8 *)&pTx_eapol->hdr_8021x, frameLen + sizeof(Hdr_8021x_t),
		gkeyMgmtInfoSta[phymac].EAPOL_MIC_Key, EAPOL_MIC_KEY_SIZE, MIC, 0);
#else
	ComputeEAPOL_MIC_DualMacSta((UINT8 *)&pTx_eapol->hdr_8021x, frameLen + sizeof(Hdr_8021x_t),
		pKeyMgmtInfoSta->EAPOL_MIC_Key, EAPOL_MIC_KEY_SIZE, MIC, 0, vmacEntry_p);
#endif
#ifdef EURUS_SPECIAL_DEBUG_FW
	if( gDebug_CorruptMIC_Msg4 )
	{
		MIC[0] = ~(MIC[0]);
	}
#endif
	apppendEAPOL_MIC(pTx_eapol->key_MIC,MIC);
#ifdef PORT_TO_LINUX_OS
	skb->len = sizeof(ether_hdr_t) + sizeof (Hdr_8021x_t) + frameLen;
#else
	pTxFrm->frameLen = frameLen + sizeof(ether_hdr_t) + HDR_8021x_LEN;
#endif /* PORT_TO_LINUX_OS */
	return SUCCESS;
}

Status_e GenerateGrpMsg2(struct sk_buff **skb_pp, EAPOL_KeyMsg_t *pRxEapol, keyMgmtInfoSta_t * pKeyMgmtInfoSta)
{
	EAPOL_KeyMsg_t * pTx_eapol;
	UINT32           frameLen;
	UINT8            MIC[EAPOL_MIC_SIZE + 4];
#ifndef PORT_TO_LINUX_OS
	key_info_t       tmpKeyInfo;
#endif /* PORT_TO_LINUX_OS */

	vmacEntry_t *       vmacEntry_p      = (vmacEntry_t *)(pKeyMgmtInfoSta->vmacEntry_p);
	STA_SECURITY_MIBS * pStaSecurityMibs = sme_GetStaSecurityMibsPtr(vmacEntry_p);

#ifdef PORT_TO_LINUX_OS
	UINT8 *frm;
	struct sk_buff *skb;

	if ((skb = ieee80211_getDataframe(&frm, EAPOL_TX_BUF)) == NULL)
	{
		WLDBG_INFO(DBG_LEVEL_5, "Error: cannot get socket buffer. \n ");
		return FAIL;
	}
	*skb_pp = skb;
	pTx_eapol = (EAPOL_KeyMsg_t *)skb->data;
#else
	if (apio_alloc(WlanHandle, pTxFrm) != APCTL_OK)
	{
		return FAIL;
	}
	pTx_eapol = (EAPOL_KeyMsg_t *)pTxFrm->framePtr;
#endif /* PORT_TO_LINUX_OS */

	MACADDR_CPY(pTx_eapol->Ether_Hdr.da, GetParentStaBSSID(vmacEntry_p->phyHwMacIndx));
	MACADDR_CPY(pTx_eapol->Ether_Hdr.sa, vmacEntry_p->vmacAddr);

	pTx_eapol->Ether_Hdr.type = IEEE_ETHERTYPE_PAE;//EAPOL Msg

	pTx_eapol->desc_type = 254;

	pTx_eapol->k.key_info16 = 0;

	if (   pStaSecurityMibs->mib_RSNConfigWPA2_p->WPA2OnlyEnabled
		|| pStaSecurityMibs->mib_RSNConfigWPA2_p->WPA2Enabled)
	{
		if ( pStaSecurityMibs->mib_RSNConfigWPA2UnicastCiphers_p->UnicastCipher[3] == 4)
		{
			pTx_eapol->k.key_info.desc_ver = 2;
		}
		else
		{
			pTx_eapol->k.key_info.desc_ver = 1;
		}
	}
	else
	{
		if ( pStaSecurityMibs->mib_RSNConfigUnicastCiphers_p->UnicastCipher[3] == 4)
		{
			pTx_eapol->k.key_info.desc_ver = 2;
		}
		else
		{
			pTx_eapol->k.key_info.desc_ver = 1;
		}
	}
	pTx_eapol->k.key_info.key_MIC = 1;
	pTx_eapol->k.key_info.secure = 1;

	pTx_eapol->k.key_info16 = SHORT_SWAP(pTx_eapol->k.key_info16);

#ifndef PORT_TO_LINUX_OS
	memcpy(&pTx_eapol->k.key_info16,
		&tmpKeyInfo.key_info16,
		sizeof(tmpKeyInfo));
#endif/* PORT_TO_LINUX_OS */

	pTx_eapol->key_length = 0;
	pTx_eapol->replay_cnt[0] = pRxEapol->replay_cnt[0];
	pTx_eapol->replay_cnt[1] = pRxEapol->replay_cnt[1];

	memset(pTx_eapol->key_nonce, 0, NONCE_SIZE);
	memset(pTx_eapol->EAPOL_key_IV,0,16);
	memset(pTx_eapol->key_RSC,0,8);
	memset(pTx_eapol->key_ID,0,8);
	pTx_eapol->key_material_len = 0;
	frameLen = 95;
	Insert8021xHdr(&pTx_eapol->hdr_8021x, (UINT16)frameLen);
	pTx_eapol->key_material_len = SHORT_SWAP(pTx_eapol->key_material_len);
#ifndef AP_MULTI_BSS_WEP
	ComputeEAPOL_MIC((UINT8 *)&pTx_eapol->hdr_8021x, frameLen + sizeof(Hdr_8021x_t),
		gkeyMgmtInfoSta[phymac].EAPOL_MIC_Key, EAPOL_MIC_KEY_SIZE, MIC, 0);
#else
	ComputeEAPOL_MIC_DualMacSta((UINT8 *)&pTx_eapol->hdr_8021x, frameLen + sizeof(Hdr_8021x_t),
		pKeyMgmtInfoSta->EAPOL_MIC_Key, EAPOL_MIC_KEY_SIZE, MIC, 0, vmacEntry_p);
#endif

#ifdef EURUS_SPECIAL_DEBUG_FW
	if( gDebug_CorruptMIC_Grp2 )
	{
		MIC[0] = ~(MIC[0]);
	}
#endif
	apppendEAPOL_MIC(pTx_eapol->key_MIC,MIC);
#ifdef PORT_TO_LINUX_OS
	skb->len = sizeof(ether_hdr_t) + sizeof (Hdr_8021x_t) + frameLen;
#else
	pTxFrm->frameLen = frameLen + sizeof(ether_hdr_t) + HDR_8021x_LEN;
#endif /* PORT_TO_LINUX_OS */

	return SUCCESS;
}

MhsmEvent_t const *KeyMgmtStaHsk_top(keyMgmtStahsk_hsm_t *me, MhsmEvent_t *msg)
{
	switch (msg->event)
	{
	case MHSM_ENTER:
		mhsm_transition(&me->super, &me->sta_hsk_start);
		return 0;
	} 
	return msg;
}

MhsmEvent_t const *KeyMgmtStaHsk_Start(keyMgmtStahsk_hsm_t *me, MhsmEvent_t *msg_p)
{
	return msg_p;
}

MhsmEvent_t const *KeyMgmtStaHsk_Recvd_PWKMsg1(keyMgmtStahsk_hsm_t *me, MhsmEvent_t *msg_p)
{
	EAPOL_KeyMsg_t * rx_eapol_ptr;
#ifndef PORT_TO_LINUX_OS
	apio_bufdescr_t  TxBuf;
#endif /* PORT_TO_LINUX_OS */
	UINT8            PTK[100];
	UINT8            phymac;


	keyMgmtInfoSta_t * pKeyMgmtInfoSta = me->keyMgmtInfoSta_p;
	vmacEntry_t *vmacEntry_p     = (vmacEntry_t *)pKeyMgmtInfoSta->vmacEntry_p;		
	vmacStaInfo_t *vStaInfo_p = (vmacStaInfo_t *)vmacEntry_p->info_p; 				

#ifdef PORT_TO_LINUX_OS
	struct sk_buff *skb = NULL;
	struct net_device *dev;

	dev = ((vmacEntry_t *)pKeyMgmtInfoSta->vmacEntry_p)->privInfo_p;
#endif /* PORT_TO_LINUX_OS */

#ifdef EURUS_SPECIAL_DEBUG_FW
	if( gDebug_DoNotRespondToKey1Message )
	{
		return msg_p;
	}
#endif
	
	phymac = ((vmacEntry_t *)pKeyMgmtInfoSta->vmacEntry_p)->phyHwMacIndx;

#ifdef REAUTH
	TimerDisarm(&timer_reauth);
#endif

	rx_eapol_ptr = (EAPOL_KeyMsg_t *)msg_p->pBody;

	if ( ! isApReplayCounterFresh(pKeyMgmtInfoSta, (UINT8 *)&rx_eapol_ptr->replay_cnt[0] ))
	{
		return 0;
	}

	memcpy(pKeyMgmtInfoSta->ANonce, rx_eapol_ptr->key_nonce, NONCE_SIZE);
	generateRand(pKeyMgmtInfoSta->SNonce, NONCE_SIZE);

#ifdef PORT_TO_LINUX_OS
	genetate_PTK(NULL,PSKValueUr[phymac], &rx_eapol_ptr->Ether_Hdr.sa,
		&rx_eapol_ptr->Ether_Hdr.da,
		pKeyMgmtInfoSta->ANonce, pKeyMgmtInfoSta->SNonce, PTK);
#else
	genetate_PTK(0,
		&rx_eapol_ptr->Ether_Hdr.sa,
		&rx_eapol_ptr->Ether_Hdr.da,
		pKeyMgmtInfoSta->ANonce,
		pKeyMgmtInfoSta->SNonce,
		PTK,
		PSKValueUr[phymac]);
#endif /* PORT_TO_LINUX_OS */

	memcpy(pKeyMgmtInfoSta->EAPOL_MIC_Key, PTK, 16);
	memcpy(pKeyMgmtInfoSta->EAPOL_Encr_Key, (PTK + 16), 16);
	memcpy(pKeyMgmtInfoSta->PairwiseTempKey_tmp, (PTK + 16 + 16), TK_SIZE);
	memcpy(pKeyMgmtInfoSta->RSNPwkRxMICKey_tmp, (PTK + 16 + 16 + TK_SIZE), 8);
	memcpy(pKeyMgmtInfoSta->RSNPwkTxMICKey_tmp, (PTK + 16 + 16 + TK_SIZE + 8), 8);

	//construct Message 2
#ifdef PORT_TO_LINUX_OS
        
	if (GeneratePWKMsg2(&skb, rx_eapol_ptr, pKeyMgmtInfoSta) != SUCCESS)
	{
		return msg_p;
	}
#else
	TxBuf.phymac_for_wlanread = phymac;
	if (GeneratePWKMsg2(&TxBuf, rx_eapol_ptr, pKeyMgmtInfoSta) != SUCCESS)
	{
		return msg_p;
	}
#endif/* PORT_TO_LINUX_OS */

	updateApReplayCounter(pKeyMgmtInfoSta, (UINT8 *)&rx_eapol_ptr->replay_cnt[0] );

#ifdef PORT_TO_LINUX_OS
	keyMgmtTxData_UnEncrypted(dev, skb, NULL);
#else
	TxBuf.to_trunk = ((vmacEntry_t *)pKeyMgmtInfoSta->vmacEntry_p)->trunkId;
	apio_urWlanWrite(&TxBuf);
#endif /* PORT_TO_LINUX_OS */
	vStaInfo_p->staSecurityMibs.mib_PrivacyTable_p->RSNLinkStatus = 0;  	
	pKeyMgmtInfoSta->pKeyData->RSNSecured = 0;

	return 0;
}

MhsmEvent_t const *KeyMgmtStaHsk_Recvd_PWKMsg3(keyMgmtStahsk_hsm_t *me, MhsmEvent_t *msg_p) 
{
	EAPOL_KeyMsg_t * rx_eapol_ptr;
#ifndef PORT_TO_LINUX_OS
	apio_bufdescr_t  TxBuf;
	key_info_t       tmpKeyInfo;
#endif /* PORT_TO_LINUX_OS */
	UINT8            rx_MIC[EAPOL_MIC_SIZE];
	UINT8            MIC[EAPOL_MIC_SIZE + 4];
	UINT8          * pGtk;
	UINT8			*pGtkEnd;
	UINT8            cipherText[256]  __attribute__ ((aligned(8)));
	UINT8            plnText[256]  __attribute__ ((aligned(8)));
	UINT32           keyLen;
	keyMgmtInfoSta_t * pKeyMgmtInfoSta = me->keyMgmtInfoSta_p;
	vmacEntry_t *      vmacEntry_p     = (vmacEntry_t *)pKeyMgmtInfoSta->vmacEntry_p;
	vmacStaInfo_t *vStaInfo_p = (vmacStaInfo_t *)vmacEntry_p->info_p; 		
	STA_SECURITY_MIBS *pStaSecurityMibs= sme_GetStaSecurityMibsPtr(vmacEntry_p);
#ifdef PORT_TO_LINUX_OS
	struct sk_buff *skb = NULL;
	struct net_device *dev;

	dev = ((vmacEntry_t *)pKeyMgmtInfoSta->vmacEntry_p)->privInfo_p;
#endif /* PORT_TO_LINUX_OS */

#ifdef EURUS_SPECIAL_DEBUG_FW
	if( gDebug_DoNotRespondToKey3Message )
	{
		return msg_p;
	}
#endif
	rx_eapol_ptr = (EAPOL_KeyMsg_t *)msg_p->pBody;
	if ( ! isApReplayCounterFresh(pKeyMgmtInfoSta, (UINT8 *)&rx_eapol_ptr->replay_cnt[0] ) )
	{
		return 0;
	}
	//received MSG3 from AP
	if (memcmp(&rx_eapol_ptr->key_nonce, pKeyMgmtInfoSta->ANonce, NONCE_SIZE) != 0)
	{
#ifndef PORT_TO_LINUX_OS
		EVTBUF_EVT_CLIENT_SUBTYPE_RSN_FAIL_ANONCE_DIFF evtBufAnonceDiff;

		evtBufAnonceDiff.phyIndex = vmacEntry_p->phyHwMacIndx;
		evtBufAnonceDiff.bssIndex = 0;
		memcpy(evtBufAnonceDiff.authenticatorMACAddr,
			GetParentStaBSSID(vmacEntry_p->phyHwMacIndx),
			6);
		memcpy(evtBufAnonceDiff.supplicantMACAddr,
			vmacEntry_p->vmacAddr,
			6);
		evtBufAnonceDiff.messageType = RSN_PWK_MSG_3;
		eventGenerate(EVT_CLIENT,
			EVT_CLIENT_SUBTYPE_RSN_FAIL_ANONCE_DIFF,
			sizeof(evtBufAnonceDiff),
			&evtBufAnonceDiff);
#endif /* PORT_TO_LINUX_OS */
		return msg_p;
	}
	memcpy(rx_MIC,rx_eapol_ptr->key_MIC, EAPOL_MIC_SIZE);
#ifndef AP_MULTI_BSS_WEP
	ComputeEAPOL_MIC((UINT8 *)&rx_eapol_ptr->hdr_8021x,
		SHORT_SWAP(rx_eapol_ptr->hdr_8021x.pckt_body_len) + sizeof(Hdr_8021x_t),
		pKeyMgmtInfoSta->EAPOL_MIC_Key,
		EAPOL_MIC_KEY_SIZE,
		MIC,
		0);
#else
	ComputeEAPOL_MIC_DualMacSta((UINT8 *)&rx_eapol_ptr->hdr_8021x,
		SHORT_SWAP(rx_eapol_ptr->hdr_8021x.pckt_body_len) + sizeof(Hdr_8021x_t),
		pKeyMgmtInfoSta->EAPOL_MIC_Key,
		EAPOL_MIC_KEY_SIZE, MIC, 0, vmacEntry_p);
#endif

	if (checkEAPOL_MIC(MIC, rx_MIC, EAPOL_MIC_SIZE) != SUCCESS)
	{
#ifndef PORT_TO_LINUX_OS
		EVTBUF_EVT_CLIENT_SUBTYPE_RSN_FAIL_MIC_DIFF    evtBufMicDiff;

		evtBufMicDiff.phyIndex = vmacEntry_p->phyHwMacIndx;
		evtBufMicDiff.bssIndex = 0;
		memcpy(evtBufMicDiff.authenticatorMACAddr,
			GetParentStaBSSID(vmacEntry_p->phyHwMacIndx),
			6);
		memcpy(evtBufMicDiff.supplicantMACAddr,
			vmacEntry_p->vmacAddr,
			6);
		evtBufMicDiff.messageType = RSN_PWK_MSG_3;
		eventGenerate(EVT_CLIENT,
			EVT_CLIENT_SUBTYPE_RSN_FAIL_MIC_DIFF,
			sizeof(evtBufMicDiff),
			&evtBufMicDiff);
#endif /* PORT_TO_LINUX_OS */

		//verify MIC
		return msg_p;
	}

#ifndef PORT_TO_LINUX_OS
	memcpy(&tmpKeyInfo.key_info16,
		&rx_eapol_ptr->k.key_info16,
		sizeof(tmpKeyInfo));
#endif /* PORT_TO_LINUX_OS */

	if (SHORT_SWAP(rx_eapol_ptr->k.key_info16) & ENCRYPTEDKEYDATA)
	{
		pKeyMgmtInfoSta->pKeyData->RSNSecured = 1;
		mib_MrvlRSN_GrpKeyUr[vmacEntry_p->phyHwMacIndx].g_IV16 = rx_eapol_ptr->key_RSC[1] << 8;
		mib_MrvlRSN_GrpKeyUr[vmacEntry_p->phyHwMacIndx].g_IV16 |= rx_eapol_ptr->key_RSC[0];
		mib_MrvlRSN_GrpKeyUr[vmacEntry_p->phyHwMacIndx].g_IV32 = 0xFFFFFFFF;

		//Decrypt the group key
		if (!pStaSecurityMibs->mib_RSNConfigWPA2_p->WPA2OnlyEnabled
			&& !pStaSecurityMibs->mib_RSNConfigWPA2_p->WPA2Enabled )
		{
			//WPA
			if (pStaSecurityMibs->mib_RSNConfigUnicastCiphers_p->UnicastCipher[3] == 2) 
			{
				// TKIP
				EncryptGrpKey(pKeyMgmtInfoSta->EAPOL_Encr_Key,
					rx_eapol_ptr->EAPOL_key_IV,
					rx_eapol_ptr->key_data,
					SHORT_SWAP(rx_eapol_ptr->key_length));
			}
		}
		else
		{
			// WPA2
			keyLen = SHORT_SWAP(rx_eapol_ptr->key_material_len) & 0xFFFF;
			if (pStaSecurityMibs->mib_RSNConfigWPA2UnicastCiphers_p->UnicastCipher[3] == 4)
			{
				// AES
				memcpy(cipherText, rx_eapol_ptr->key_data, keyLen);
				AES_UnWrap((WRAPUINT64 *)&plnText[0], (WRAPUINT64 *)&cipherText[0], (WRAPUINT64 *)&pKeyMgmtInfoSta->EAPOL_Encr_Key[0], keyLen);
				memcpy(rx_eapol_ptr->key_data, plnText+8, keyLen-8);
			}
			else
			{
				// TKIP
				EncryptGrpKey(pKeyMgmtInfoSta->EAPOL_Encr_Key,
					rx_eapol_ptr->EAPOL_key_IV,
					rx_eapol_ptr->key_data,
					keyLen);
			}
			//Get a pointer to the GTK information
			pGtk = rx_eapol_ptr->key_data;
			pGtkEnd = pGtk + keyLen;
			pGtk += ((IEEEtypes_RSN_IE_WPA2_t*)pGtk)->Len + 2;
			
			while(pGtk < pGtkEnd)
			{  			
				if(!memcmp(((EAPOL_KeyDataWPA2_t*)pGtk)->OUI, KDEOUI, 3) && 
					(((EAPOL_KeyDataWPA2_t*)pGtk)->dataType ==1) &&
					 (((EAPOL_KeyDataWPA2_t*)pGtk)->type == 0xdd))
				{								
			pGtk += ((EAPOL_KeyDataWPA2_t*)pGtk)->length + 2 - TK_SIZE;
					break;
				}    			 
				pGtk += ((EAPOL_KeyDataWPA2_t*)pGtk)->length + 2;
			}


			// handle Mixed case 
			if (pStaSecurityMibs->mib_RSNConfigWPA2_p->MulticastCipher[3] == 4)
			{
				// AES
				memcpy(mib_MrvlRSN_GrpKeyUr[vmacEntry_p->phyHwMacIndx].EncryptKey, pGtk, TK_SIZE);
#ifdef PORT_TO_LINUX_OS
				wlFwSetWpaAesMode_STA(mainNetdev_p[vmacEntry_p->phyHwMacIndx],  GetParentStaBSSID(vmacEntry_p->phyHwMacIndx));
				wlFwSetWpaAesGroupK_STA(mainNetdev_p[vmacEntry_p->phyHwMacIndx], 
					GetParentStaBSSID(vmacEntry_p->phyHwMacIndx), 
					&mib_MrvlRSN_GrpKeyUr[vmacEntry_p->phyHwMacIndx].EncryptKey[0], 
					TK_SIZE);
#ifdef V6FW
				wlFwSetWpaAesMode_STA(mainNetdev_p[vmacEntry_p->phyHwMacIndx],  (UINT8 *)vmacEntry_p->vmacAddr);

				wlFwSetWpaAesGroupK_STA(mainNetdev_p[vmacEntry_p->phyHwMacIndx], 
					(UINT8 *)vmacEntry_p->vmacAddr, 
					&mib_MrvlRSN_GrpKeyUr[vmacEntry_p->phyHwMacIndx].EncryptKey[0], 
					TK_SIZE);
#endif
#endif /* PORT_TO_LINUX_OS */
			}
			else
			{
				// Tkip
				memcpy(mib_MrvlRSN_GrpKeyUr[vmacEntry_p->phyHwMacIndx].EncryptKey, pGtk-TK_SIZE, TK_SIZE);
				memcpy(mib_MrvlRSN_GrpKeyUr[vmacEntry_p->phyHwMacIndx].RxMICKey, pGtk-TK_SIZE + 16, 8);
				memcpy(mib_MrvlRSN_GrpKeyUr[vmacEntry_p->phyHwMacIndx].TxMICKey, pGtk-TK_SIZE + 16 + 8, 8);
#ifdef PORT_TO_LINUX_OS
				{
					ENCR_TKIPSEQCNT	TkipTsc;

					TkipTsc.low = mib_MrvlRSN_GrpKeyUr[vmacEntry_p->phyHwMacIndx].g_IV16;
					TkipTsc.high = mib_MrvlRSN_GrpKeyUr[vmacEntry_p->phyHwMacIndx].g_IV32;

					wlFwSetMixedWpaWpa2Mode_STA(mainNetdev_p[vmacEntry_p->phyHwMacIndx],  GetParentStaBSSID(vmacEntry_p->phyHwMacIndx));

					wlFwSetWpaTkipGroupK_STA(mainNetdev_p[vmacEntry_p->phyHwMacIndx], 
						GetParentStaBSSID(vmacEntry_p->phyHwMacIndx), 
						&mib_MrvlRSN_GrpKeyUr[vmacEntry_p->phyHwMacIndx].EncryptKey[0], 
						TK_SIZE,
						(UINT8 *)&mib_MrvlRSN_GrpKeyUr[vmacEntry_p->phyHwMacIndx].RxMICKey,
						MIC_KEY_LENGTH,
						(UINT8 *)&mib_MrvlRSN_GrpKeyUr[vmacEntry_p->phyHwMacIndx].TxMICKey,
						MIC_KEY_LENGTH,
						TkipTsc, 0);
#ifdef V6FW
					wlFwSetMixedWpaWpa2Mode_STA(mainNetdev_p[vmacEntry_p->phyHwMacIndx],  (UINT8 *)vmacEntry_p->vmacAddr);

					wlFwSetWpaTkipGroupK_STA(mainNetdev_p[vmacEntry_p->phyHwMacIndx], 
						(UINT8 *)vmacEntry_p->vmacAddr, 
						&mib_MrvlRSN_GrpKeyUr[vmacEntry_p->phyHwMacIndx].EncryptKey[0], 
						TK_SIZE,
						(UINT8 *)&mib_MrvlRSN_GrpKeyUr[vmacEntry_p->phyHwMacIndx].RxMICKey,
						MIC_KEY_LENGTH,
						(UINT8 *)&mib_MrvlRSN_GrpKeyUr[vmacEntry_p->phyHwMacIndx].TxMICKey,
						MIC_KEY_LENGTH,
						TkipTsc, 0);
#endif
				}
#endif /* PORT_TO_LINUX_OS */
			}
            /* Enable traffic for WPA2 mode. */
            pKeyMgmtInfoSta->pKeyData->RSNDataTrafficEnabled = 1;
		}
	}
	pKeyMgmtInfoSta->pKeyData->TxIV16 = 1;
	pKeyMgmtInfoSta->pKeyData->TxIV32 = 0;
	pKeyMgmtInfoSta->pKeyData->RxIV32 = 0xFFFFFFFF;

#ifndef PORT_TO_LINUX_OS
	TxBuf.phymac_for_wlanread = vmacEntry_p->phyHwMacIndx;
#endif /* PORT_TO_LINUX_OS */

	//construct Message 4
#ifdef PORT_TO_LINUX_OS
	if (GeneratePWKMsg4(&skb, rx_eapol_ptr, pKeyMgmtInfoSta) != SUCCESS)
#else
	if (GeneratePWKMsg4(&TxBuf, rx_eapol_ptr, pKeyMgmtInfoSta) != SUCCESS)
#endif /* PORT_TO_LINUX_OS */
	{
		return msg_p;
	}

	updateApReplayCounter(pKeyMgmtInfoSta, (UINT8 *)&rx_eapol_ptr->replay_cnt[0] );
	memcpy(pKeyMgmtInfoSta->pKeyData->PairwiseTempKey,
		pKeyMgmtInfoSta->PairwiseTempKey_tmp,
		TK_SIZE);
	memcpy(pKeyMgmtInfoSta->pKeyData->RSNPwkTxMICKey,
		pKeyMgmtInfoSta->RSNPwkTxMICKey_tmp,
		8);
	memcpy(pKeyMgmtInfoSta->pKeyData->RSNPwkRxMICKey,
		pKeyMgmtInfoSta->RSNPwkRxMICKey_tmp,
		8);

#ifdef PORT_TO_LINUX_OS
	{
		struct wlprivate *wlpptrSta = NETDEV_PRIV_P(struct wlprivate, dev);
		extStaDb_StaInfo_t *StaInfo_p = NULL;

        if ((StaInfo_p = extStaDb_GetStaInfo(wlpptrSta->vmacSta_p,(IEEEtypes_MacAddr_t *)GetParentStaBSSID(((vmacEntry_t *)pKeyMgmtInfoSta->vmacEntry_p)->phyHwMacIndx), 0)) != NULL)
		{
			keyMgmtTxData_UnEncrypted(dev, skb, NULL);

			/* Set the key to FW */
			AddRSN_IEWPA2_TO( pStaSecurityMibs->thisStaRsnIEWPA2_p,(IEEEtypes_RSN_IE_WPA2_t*)(&StaInfo_p->keyMgmtStateInfo.RsnIEBuf[0]) );
			memcpy(&StaInfo_p->keyMgmtStateInfo.PairwiseTempKey1[0],
				&pKeyMgmtInfoSta->pKeyData->PairwiseTempKey[0],
				TK_SIZE);
			memcpy(&StaInfo_p->keyMgmtStateInfo.RSNPwkTxMICKey[0],
				&pKeyMgmtInfoSta->pKeyData->RSNPwkTxMICKey[0],
				8);
			memcpy(&StaInfo_p->keyMgmtStateInfo.RSNPwkRxMICKey[0],
				&pKeyMgmtInfoSta->pKeyData->RSNPwkRxMICKey[0],
				8);
			StaInfo_p->keyMgmtStateInfo.TxIV16 = pKeyMgmtInfoSta->pKeyData->TxIV16;
			StaInfo_p->keyMgmtStateInfo.TxIV32 = pKeyMgmtInfoSta->pKeyData->TxIV32;
			StaInfo_p->keyMgmtStateInfo.RxIV32 = pKeyMgmtInfoSta->pKeyData->RxIV32;


			if (!pStaSecurityMibs->mib_RSNConfigWPA2_p->WPA2OnlyEnabled
				&& !pStaSecurityMibs->mib_RSNConfigWPA2_p->WPA2Enabled )
			{
				//WPA
				AddRSN_IE_TO(pStaSecurityMibs->thisStaRsnIE_p, (IEEEtypes_RSN_IE_t*)(&StaInfo_p->keyMgmtStateInfo.RsnIEBuf[0]));
				if (pStaSecurityMibs->mib_RSNConfigUnicastCiphers_p->UnicastCipher[3] == 2) 
				{
					// TKIP
					wlFwSetWpaTkipMode_STA(mainNetdev_p[vmacEntry_p->phyHwMacIndx],  (UINT8 *)&StaInfo_p->Addr);
				}
				else if ((pStaSecurityMibs->mib_RSNConfigUnicastCiphers_p->UnicastCipher[3] == 4)) 
				{
					// AES
					wlFwSetWpaAesMode_STA(mainNetdev_p[vmacEntry_p->phyHwMacIndx],  (UINT8 *)&StaInfo_p->Addr);
				}
			}
			else
			{
				// WPA2
				AddRSN_IEWPA2_TO( pStaSecurityMibs->thisStaRsnIEWPA2_p,(IEEEtypes_RSN_IE_WPA2_t*)(&StaInfo_p->keyMgmtStateInfo.RsnIEBuf[0]) );
				if (pStaSecurityMibs->mib_RSNConfigWPA2UnicastCiphers_p->UnicastCipher[3] == 4)
				{
					// AES
					wlFwSetWpaAesMode_STA(mainNetdev_p[vmacEntry_p->phyHwMacIndx],  (UINT8 *)&StaInfo_p->Addr);
				}
				else
				{
					// TKIP
					//Not sure if this is correct setting for firmware in this case????
					wlFwSetMixedWpaWpa2Mode_STA(mainNetdev_p[vmacEntry_p->phyHwMacIndx],  (UINT8 *)&StaInfo_p->Addr);
				}
			}

			wlFwSetWpaWpa2PWK_STA(mainNetdev_p[vmacEntry_p->phyHwMacIndx], StaInfo_p);
		}
		else
		{
			return msg_p;
		}
#ifdef V6FW /* Duplicate the station database settings for Client MAC address entry*/
		if ((StaInfo_p = extStaDb_GetStaInfo(wlpptrSta->vmacSta_p,&((vmacEntry_t *)pKeyMgmtInfoSta->vmacEntry_p)->vmacAddr, 0)) != NULL)
		{
			AddRSN_IEWPA2_TO( pStaSecurityMibs->thisStaRsnIEWPA2_p,(IEEEtypes_RSN_IE_WPA2_t*)(&StaInfo_p->keyMgmtStateInfo.RsnIEBuf[0]) );
			memcpy(&StaInfo_p->keyMgmtStateInfo.PairwiseTempKey1[0],
				&pKeyMgmtInfoSta->pKeyData->PairwiseTempKey[0],
				TK_SIZE);
			memcpy(&StaInfo_p->keyMgmtStateInfo.RSNPwkTxMICKey[0],
				&pKeyMgmtInfoSta->pKeyData->RSNPwkTxMICKey[0],
				8);
			memcpy(&StaInfo_p->keyMgmtStateInfo.RSNPwkRxMICKey[0],
				&pKeyMgmtInfoSta->pKeyData->RSNPwkRxMICKey[0],
				8);

			StaInfo_p->keyMgmtStateInfo.TxIV16 = pKeyMgmtInfoSta->pKeyData->TxIV16;
			StaInfo_p->keyMgmtStateInfo.TxIV32 = pKeyMgmtInfoSta->pKeyData->TxIV32;
			StaInfo_p->keyMgmtStateInfo.RxIV32 = pKeyMgmtInfoSta->pKeyData->RxIV32;


			if (!pStaSecurityMibs->mib_RSNConfigWPA2_p->WPA2OnlyEnabled
				&& !pStaSecurityMibs->mib_RSNConfigWPA2_p->WPA2Enabled )
			{
				//WPA
				AddRSN_IE_TO(pStaSecurityMibs->thisStaRsnIE_p, (IEEEtypes_RSN_IE_t*)(&StaInfo_p->keyMgmtStateInfo.RsnIEBuf[0]));
				if (pStaSecurityMibs->mib_RSNConfigUnicastCiphers_p->UnicastCipher[3] == 2) 
				{
					// TKIP
					wlFwSetWpaTkipMode_STA(mainNetdev_p[vmacEntry_p->phyHwMacIndx],  (UINT8 *)&StaInfo_p->Addr);
				}
				else if ((pStaSecurityMibs->mib_RSNConfigUnicastCiphers_p->UnicastCipher[3] == 4)) 
				{
					// AES
					wlFwSetWpaAesMode_STA(mainNetdev_p[vmacEntry_p->phyHwMacIndx],  (UINT8 *)&StaInfo_p->Addr);
				}
			}
			else
			{
				// WPA2
				AddRSN_IEWPA2_TO( pStaSecurityMibs->thisStaRsnIEWPA2_p,(IEEEtypes_RSN_IE_WPA2_t*)(&StaInfo_p->keyMgmtStateInfo.RsnIEBuf[0]) );
				if (pStaSecurityMibs->mib_RSNConfigWPA2UnicastCiphers_p->UnicastCipher[3] == 4)
				{
					// AES
					wlFwSetWpaAesMode_STA(mainNetdev_p[vmacEntry_p->phyHwMacIndx],  (UINT8 *)&StaInfo_p->Addr);
				}
				else
				{
					// TKIP
					//Not sure if this is correct setting for firmware in this case????
					wlFwSetMixedWpaWpa2Mode_STA(mainNetdev_p[vmacEntry_p->phyHwMacIndx],  (UINT8 *)&StaInfo_p->Addr);
				}
			}

			wlFwSetWpaWpa2PWK_STA(mainNetdev_p[vmacEntry_p->phyHwMacIndx], StaInfo_p);
		}
		else
		{
			return msg_p;
		}
#endif /* V6FW*/
	}
#else
	pKeyMgmtInfoSta->pKeyData->RSNDataTrafficEnabled = 0; // make sure that this message goes out un-encrypted.
	TxBuf.to_trunk = vmacEntry_p->trunkId;
	apio_urWlanWrite(&TxBuf);
#endif /* PORT_TO_LINUX_OS */

	//pKeyMgmtInfoSta->pKeyData->RSNDataTrafficEnabled = 1;
#ifndef PORT_TO_LINUX_OS
	if(sme_isParentSession(vmacEntry_p)){
		EurusSetTrunkIdActive(vmacEntry_p->trunkId, vmacEntry_p->phyHwMacIndx, TRUE, STA_TRUNK_MODE);
	}
#endif /* PORT_TO_LINUX_OS */

	if (pKeyMgmtInfoSta->pKeyData->RSNSecured != 0)
	{
#ifdef PORT_TO_LINUX_OS
		keyMgmtStaStopTimer(pKeyMgmtInfoSta, (UINT8 *)&me->rsnSecuredTimer);
#else
		EVTBUF_EVT_CLIENT_SUBTYPE_RSN_SECURED  evtBufRsnSecured;

		if (me->rsnSecuredTimer.active)
		{
			TimerRemove(&me->rsnSecuredTimer);
		}
		evtBufRsnSecured.phyIndex = vmacEntry_p->phyHwMacIndx;
		evtBufRsnSecured.bssIndex = 0;
		memcpy(evtBufRsnSecured.authenticatorMACAddr,
			GetParentStaBSSID(vmacEntry_p->phyHwMacIndx),
			6);
		memcpy(evtBufRsnSecured.supplicantMACAddr,
			vmacEntry_p->vmacAddr,
			6);
		eventGenerate(EVT_CLIENT,
			EVT_CLIENT_SUBTYPE_RSN_SECURED,
			sizeof(evtBufRsnSecured),
			&evtBufRsnSecured);
#endif /* PORT_TO_LINUX_OS */

		vStaInfo_p->staSecurityMibs.mib_PrivacyTable_p->RSNLinkStatus = 1;  	
	}
	return 0;
}

MhsmEvent_t const *KeyMgmtStaHsk_Recvd_GrpMsg1(keyMgmtStahsk_hsm_t *me, MhsmEvent_t *msg_p) 
{
	EAPOL_KeyMsg_t * rx_eapol_ptr;
#ifndef PORT_TO_LINUX_OS
	apio_bufdescr_t  TxBuf;
#endif /* PORT_TO_LINUX_OS */
	UINT8            rx_MIC[EAPOL_MIC_SIZE];
	UINT8            MIC[EAPOL_MIC_SIZE + 4];
	UINT8            keyLen;
	UINT8            cipherText[256]  __attribute__ ((aligned(8)));
	UINT8            plnText[256]  __attribute__ ((aligned(8)));
	keyMgmtInfoSta_t * pKeyMgmtInfoSta = me->keyMgmtInfoSta_p;
	vmacEntry_t *      vmacEntry_p     = (vmacEntry_t *)pKeyMgmtInfoSta->vmacEntry_p;
	vmacStaInfo_t *vStaInfo_p = (vmacStaInfo_t *)vmacEntry_p->info_p; 		
	STA_SECURITY_MIBS *pStaSecurityMibs= sme_GetStaSecurityMibsPtr(vmacEntry_p);

#ifdef PORT_TO_LINUX_OS
	struct sk_buff *skb = NULL;
	struct net_device *dev;
	dev = ((vmacEntry_t *)pKeyMgmtInfoSta->vmacEntry_p)->privInfo_p;
#endif /* PORT_TO_LINUX_OS */

#ifdef EURUS_SPECIAL_DEBUG_FW
	if( gDebug_DoNotRespondToGRP1Message )
	{
		return msg_p;
	}
#endif

	rx_eapol_ptr = (EAPOL_KeyMsg_t *)msg_p->pBody;

	if ( ! isApReplayCounterFresh(pKeyMgmtInfoSta, (UINT8 *)&rx_eapol_ptr->replay_cnt[0] ))
	{
		return 0;
	}
	memcpy(rx_MIC,rx_eapol_ptr->key_MIC, EAPOL_MIC_SIZE);
#ifndef AP_MULTI_BSS_WEP
	ComputeEAPOL_MIC((UINT8 *)&rx_eapol_ptr->hdr_8021x,
		SHORT_SWAP(rx_eapol_ptr->hdr_8021x.pckt_body_len) + sizeof(Hdr_8021x_t),
		gkeyMgmtInfoSta[phymac].EAPOL_MIC_Key,
		EAPOL_MIC_KEY_SIZE, MIC, 0);
#else
	ComputeEAPOL_MIC_DualMacSta((UINT8 *)&rx_eapol_ptr->hdr_8021x,
		SHORT_SWAP(rx_eapol_ptr->hdr_8021x.pckt_body_len) + sizeof(Hdr_8021x_t),
		pKeyMgmtInfoSta->EAPOL_MIC_Key,
		EAPOL_MIC_KEY_SIZE, MIC, 0, vmacEntry_p);
#endif
	if (checkEAPOL_MIC(MIC, rx_MIC, EAPOL_MIC_SIZE) != SUCCESS)
	{
#ifndef PORT_TO_LINUX_OS
		EVTBUF_EVT_CLIENT_SUBTYPE_RSN_FAIL_MIC_DIFF    evtBufMicDiff;

		evtBufMicDiff.phyIndex = vmacEntry_p->phyHwMacIndx;
		evtBufMicDiff.bssIndex = 0;
		memcpy(evtBufMicDiff.authenticatorMACAddr,
			GetParentStaBSSID(vmacEntry_p->phyHwMacIndx),
			6);
		memcpy(evtBufMicDiff.supplicantMACAddr,
			vmacEntry_p->vmacAddr,
			6);
		evtBufMicDiff.messageType = RSN_GRP_MSG_1;
		eventGenerate(EVT_CLIENT,
			EVT_CLIENT_SUBTYPE_RSN_FAIL_MIC_DIFF,
			sizeof(evtBufMicDiff),
			&evtBufMicDiff);
#endif /* PORT_TO_LINUX_OS */
		return msg_p;
	}

	//Decrypt the group key
	if (!pStaSecurityMibs->mib_RSNConfigWPA2_p->WPA2OnlyEnabled
		&& !pStaSecurityMibs->mib_RSNConfigWPA2_p->WPA2Enabled)
	{
		// WPA
		if (pStaSecurityMibs->mib_RSNConfigUnicastCiphers_p->UnicastCipher[3] == 2)
		{
			// TKIP
			EncryptGrpKey(pKeyMgmtInfoSta->EAPOL_Encr_Key,
				rx_eapol_ptr->EAPOL_key_IV, rx_eapol_ptr->key_data,
				SHORT_SWAP(rx_eapol_ptr->key_length));
			memcpy(mib_MrvlRSN_GrpKeyUr[vmacEntry_p->phyHwMacIndx].EncryptKey, rx_eapol_ptr->key_data, 16);
			memcpy(mib_MrvlRSN_GrpKeyUr[vmacEntry_p->phyHwMacIndx].RxMICKey, rx_eapol_ptr->key_data + 16, 8);
			memcpy(mib_MrvlRSN_GrpKeyUr[vmacEntry_p->phyHwMacIndx].TxMICKey, rx_eapol_ptr->key_data + 16 + 8, 8);
		}
		else
		{      
			// CCMP unicast
			keyLen = SHORT_SWAP(rx_eapol_ptr->key_material_len) & 0xFFFF;
			memcpy(cipherText, rx_eapol_ptr->key_data, keyLen);
			AES_UnWrap((WRAPUINT64 *)&plnText[0], (WRAPUINT64 *)&cipherText[0], (WRAPUINT64 *)&pKeyMgmtInfoSta->EAPOL_Encr_Key[0], keyLen);
			memcpy(rx_eapol_ptr->key_data, plnText+8, keyLen-8);

			if (pStaSecurityMibs->mib_RSNConfig_p->MulticastCipher[3] == 4)
			{
				// CCMP group
				memcpy(mib_MrvlRSN_GrpKeyUr[vmacEntry_p->phyHwMacIndx].EncryptKey, rx_eapol_ptr->key_data, TK_SIZE);
			}
			else
			{
				memcpy(mib_MrvlRSN_GrpKeyUr[vmacEntry_p->phyHwMacIndx].EncryptKey, rx_eapol_ptr->key_data, 16);
				memcpy(mib_MrvlRSN_GrpKeyUr[vmacEntry_p->phyHwMacIndx].RxMICKey, rx_eapol_ptr->key_data + 16, 8);
				memcpy(mib_MrvlRSN_GrpKeyUr[vmacEntry_p->phyHwMacIndx].TxMICKey, rx_eapol_ptr->key_data + 16 + 8, 8);
			}
		}    
	}
	else
	{
		// WPA2
		keyLen = SHORT_SWAP(rx_eapol_ptr->key_material_len) & 0xFFFF;
		if (pStaSecurityMibs->mib_RSNConfigWPA2UnicastCiphers_p->UnicastCipher[3] == 2) 
		{
			//TKIP
			EncryptGrpKey(pKeyMgmtInfoSta->EAPOL_Encr_Key,
				rx_eapol_ptr->EAPOL_key_IV, rx_eapol_ptr->key_data,
				keyLen);
		}
		else
		{
			// CCMP
			memcpy(cipherText, rx_eapol_ptr->key_data, keyLen);
			AES_UnWrap((WRAPUINT64 *)&plnText[0], (WRAPUINT64 *)&cipherText[0], (WRAPUINT64 *)&pKeyMgmtInfoSta->EAPOL_Encr_Key[0], keyLen);
			memcpy(rx_eapol_ptr->key_data, plnText+8, keyLen-8);//first 8 bytes should be rejected
		}

		if (pStaSecurityMibs->mib_RSNConfigWPA2_p->WPA2OnlyEnabled)
		{
			//handle it according to 802.11i GTK frame format
			UINT8 *pGtk;
			pGtk = rx_eapol_ptr->key_data + ((EAPOL_KeyDataWPA2_t*)rx_eapol_ptr->key_data)->length + 2 - TK_SIZE;
			// handle Mixed case 
			if (pStaSecurityMibs->mib_RSNConfigWPA2_p->MulticastCipher[3] == 4)
			{
				// AES
				memcpy(mib_MrvlRSN_GrpKeyUr[vmacEntry_p->phyHwMacIndx].EncryptKey, pGtk, TK_SIZE);
			}
			else
			{
				// Tkip
				memcpy(mib_MrvlRSN_GrpKeyUr[vmacEntry_p->phyHwMacIndx].EncryptKey, pGtk-TK_SIZE, TK_SIZE);
				memcpy(mib_MrvlRSN_GrpKeyUr[vmacEntry_p->phyHwMacIndx].RxMICKey, pGtk-TK_SIZE + 16, 8);
				memcpy(mib_MrvlRSN_GrpKeyUr[vmacEntry_p->phyHwMacIndx].TxMICKey, pGtk-TK_SIZE + 16 + 8, 8);
			}
		}
		else
		{
			memcpy(mib_MrvlRSN_GrpKeyUr[vmacEntry_p->phyHwMacIndx].EncryptKey, rx_eapol_ptr->key_data, TK_SIZE);
		}
	}
#ifdef PORT_TO_LINUX_OS
	if ((pStaSecurityMibs->mib_RSNConfigWPA2_p->MulticastCipher[3] == 4) || 
        (pStaSecurityMibs->mib_RSNConfig_p->MulticastCipher[3] == 4)) 
	{
		wlFwSetWpaAesGroupK_STA(mainNetdev_p[vmacEntry_p->phyHwMacIndx], 
			GetParentStaBSSID(vmacEntry_p->phyHwMacIndx), 
			&mib_MrvlRSN_GrpKeyUr[vmacEntry_p->phyHwMacIndx].EncryptKey[0], 
			TK_SIZE);
#ifdef V6FW
		wlFwSetWpaAesGroupK_STA(mainNetdev_p[vmacEntry_p->phyHwMacIndx], 
			(UINT8 *)vmacEntry_p->vmacAddr, 
			&mib_MrvlRSN_GrpKeyUr[vmacEntry_p->phyHwMacIndx].EncryptKey[0], 
			TK_SIZE);
#endif
	}
	else
	{
		// Tkip
		ENCR_TKIPSEQCNT	TkipTsc;
		TkipTsc.low = mib_MrvlRSN_GrpKeyUr[vmacEntry_p->phyHwMacIndx].g_IV16;
		TkipTsc.high = mib_MrvlRSN_GrpKeyUr[vmacEntry_p->phyHwMacIndx].g_IV32;

		wlFwSetWpaTkipGroupK_STA(mainNetdev_p[vmacEntry_p->phyHwMacIndx], 
			GetParentStaBSSID(vmacEntry_p->phyHwMacIndx), 
			&mib_MrvlRSN_GrpKeyUr[vmacEntry_p->phyHwMacIndx].EncryptKey[0], 
			TK_SIZE,
			(UINT8 *)&mib_MrvlRSN_GrpKeyUr[vmacEntry_p->phyHwMacIndx].RxMICKey,
			MIC_KEY_LENGTH,
			(UINT8 *)&mib_MrvlRSN_GrpKeyUr[vmacEntry_p->phyHwMacIndx].TxMICKey,
			MIC_KEY_LENGTH,
			TkipTsc, 0);
#ifdef V6FW
		wlFwSetWpaTkipGroupK_STA(mainNetdev_p[vmacEntry_p->phyHwMacIndx], 
			(UINT8 *)vmacEntry_p->vmacAddr, 
			&mib_MrvlRSN_GrpKeyUr[vmacEntry_p->phyHwMacIndx].EncryptKey[0], 
			TK_SIZE,
			(UINT8 *)&mib_MrvlRSN_GrpKeyUr[vmacEntry_p->phyHwMacIndx].RxMICKey,
			MIC_KEY_LENGTH,
			(UINT8 *)&mib_MrvlRSN_GrpKeyUr[vmacEntry_p->phyHwMacIndx].TxMICKey,
			MIC_KEY_LENGTH,
			TkipTsc, 0);
#endif

	}
#endif /* PORT_TO_LINUX_OS */

	pKeyMgmtInfoSta->pKeyData->RSNDataTrafficEnabled = 1;

#ifndef PORT_TO_LINUX_OS
	if (sme_isParentSession(vmacEntry_p))
	{
		EurusSetTrunkIdActive(vmacEntry_p->trunkId, vmacEntry_p->phyHwMacIndx, TRUE, STA_TRUNK_MODE);
	}
#endif /* PORT_TO_LINUX_OS */
	mib_MrvlRSN_GrpKeyUr[vmacEntry_p->phyHwMacIndx].g_IV16 = rx_eapol_ptr->key_RSC[1] << 8;
	mib_MrvlRSN_GrpKeyUr[vmacEntry_p->phyHwMacIndx].g_IV16 |= rx_eapol_ptr->key_RSC[0];
	mib_MrvlRSN_GrpKeyUr[vmacEntry_p->phyHwMacIndx].g_IV32 = 0xFFFFFFFF;
#ifndef PORT_TO_LINUX_OS
	TxBuf.phymac_for_wlanread = vmacEntry_p->phyHwMacIndx;
#endif /* PORT_TO_LINUX_OS */
	//construct Message Grp Msg2
#ifdef PORT_TO_LINUX_OS
	if (GenerateGrpMsg2(&skb, rx_eapol_ptr, pKeyMgmtInfoSta) != SUCCESS)
#else
	if (GenerateGrpMsg2(&TxBuf, rx_eapol_ptr, pKeyMgmtInfoSta) != SUCCESS)
#endif /* PORT_TO_LINUX_OS */
	{
		return msg_p;
	}
	updateApReplayCounter(pKeyMgmtInfoSta, (UINT8 *)&rx_eapol_ptr->replay_cnt[0] );
#ifdef PORT_TO_LINUX_OS
	{
		struct wlprivate *wlpptrSta = NETDEV_PRIV_P(struct wlprivate, dev);
		extStaDb_StaInfo_t *StaInfo_p = NULL;

        if ((StaInfo_p = extStaDb_GetStaInfo(wlpptrSta->vmacSta_p,(IEEEtypes_MacAddr_t *)GetParentStaBSSID(((vmacEntry_t *)pKeyMgmtInfoSta->vmacEntry_p)->phyHwMacIndx), 0)) != NULL)
		{
			keyMgmtTxData_Encrypted(dev, skb, StaInfo_p);
		}
		else
		{
			return msg_p;
		}
	}
#else
	TxBuf.to_trunk = vmacEntry_p->trunkId;
	apio_urWlanWrite(&TxBuf);
#endif /* PORT_TO_LINUX_OS */

	if (pKeyMgmtInfoSta->pKeyData->RSNSecured == 0)
	{
#ifndef PORT_TO_LINUX_OS
		EVTBUF_EVT_CLIENT_SUBTYPE_RSN_SECURED  evtBufRsnSecured;
#endif /* PORT_TO_LINUX_OS */

		pKeyMgmtInfoSta->pKeyData->RSNSecured = 1;

#ifdef PORT_TO_LINUX_OS
		keyMgmtStaStopTimer(pKeyMgmtInfoSta, (UINT8 *)&me->rsnSecuredTimer);

#else
		if (me->rsnSecuredTimer.active)
		{
			TimerRemove(&me->rsnSecuredTimer);
		}
		evtBufRsnSecured.phyIndex = vmacEntry_p->phyHwMacIndx;
		evtBufRsnSecured.bssIndex = 0;
		memcpy(evtBufRsnSecured.authenticatorMACAddr,
			GetParentStaBSSID(vmacEntry_p->phyHwMacIndx),
			6);
		memcpy(evtBufRsnSecured.supplicantMACAddr,
			vmacEntry_p->vmacAddr,
			6);
		eventGenerate(EVT_CLIENT,
			EVT_CLIENT_SUBTYPE_RSN_SECURED,
			sizeof(evtBufRsnSecured),
			&evtBufRsnSecured);
#endif /* PORT_TO_LINUX_OS */
		
		vStaInfo_p->staSecurityMibs.mib_PrivacyTable_p->RSNLinkStatus = 1;  	
	}
	return 0;
}

MhsmEvent_t const *KeyMgmtStaHsk_RecvdMICFailEvt(keyMgmtStahsk_hsm_t *me, MhsmEvent_t *msg_p) 
{
#ifndef PORT_TO_LINUX_OS
	apio_bufdescr_t TxBuf;
#endif /* PORT_TO_LINUX_OS */

	keyMgmtInfoSta_t * pKeyMgmtInfoSta = me->keyMgmtInfoSta_p;

#ifdef PORT_TO_LINUX_OS
	struct sk_buff *skb = NULL;
	struct net_device *dev;
	dev = ((vmacEntry_t *)pKeyMgmtInfoSta->vmacEntry_p)->privInfo_p;
#endif /* PORT_TO_LINUX_OS */

	//construct Message MIC Fail Report

#ifdef PORT_TO_LINUX_OS
	keyMgmtTxData_UnEncrypted(dev, skb, NULL);
#else
	TxBuf.phymac_for_wlanread = vmacEntry_p->phyHwMacIndx;
	TxBuf.to_trunk = vmacEntry_p->trunkId;
	apio_urWlanWrite(&TxBuf);
#endif /* PORT_TO_LINUX_OS */
	return 0;
}

MhsmEvent_t const *KeyMgmtStaHsk_End(keyMgmtStahsk_hsm_t *me, MhsmEvent_t *msg_p) 
{
	return msg_p;
}

void KeyMgmtStaHskCtor(keyMgmtStahsk_hsm_t * me)
{

	mhsm_add(&me->sTop, NULL, (MhsmFcnPtr)KeyMgmtStaHsk_top);

	mhsm_add(&me->sta_hsk_start,&me->sTop,
		(MhsmFcnPtr)KeyMgmtStaHsk_Start);
	mhsm_add(&me->recvd_pwk_msg_1, &me->sTop, 
		(MhsmFcnPtr)KeyMgmtStaHsk_Recvd_PWKMsg1);
	mhsm_add(&me->recvd_pwk_msg_3, &me->sTop, 
		(MhsmFcnPtr)KeyMgmtStaHsk_Recvd_PWKMsg3);
	mhsm_add(&me->recvd_grp_msg_1,  &me->sTop, 
		(MhsmFcnPtr)KeyMgmtStaHsk_Recvd_GrpMsg1);
	mhsm_add(&me->sta_hsk_end, &me->sTop, 
		(MhsmFcnPtr)KeyMgmtStaHsk_End);
}

void KeyMgmtResetCounter(keyMgmtInfoSta_t *keyMgmtInfo_p)
{
	if(keyMgmtInfo_p)
	{
		keyMgmtInfo_p->staCounterHi = 0;
		keyMgmtInfo_p->staCounterLo = 0;
	}
}

// This routine must be called after mlmeStaInit_UR
// It assumes that parent session structures are initialized (vmacEntry_parent and mlmeStaInfo)
void KeyMgmtInitSta(UINT8 phymac)
{
	STA_SYSTEM_MIBS   * pStaSystemMibs;
	STA_SECURITY_MIBS * pStaSecurityMibs;

	gkeyMgmtInfoSta[phymac].pKeyData = &gKeyData[phymac];
	gkeyMgmtInfoSta[phymac].vmacEntry_p = &vmacEntry_parent[phymac];
	gkeyMgmtInfoSta[phymac].keyMgmtStaHskHsm.keyMgmtInfoSta_p = &gkeyMgmtInfoSta[phymac]; // point back to self from HSM.
	KeyMgmtStaHskCtor(&gkeyMgmtInfoSta[phymac].keyMgmtStaHskHsm);
	mhsm_initialize(&gkeyMgmtInfoSta[phymac].keyMgmtStaHskHsm.super,&gkeyMgmtInfoSta[phymac].keyMgmtStaHskHsm.sTop);

	pStaSystemMibs   = sme_GetStaSystemMibsPtr(&vmacEntry_parent[phymac]);
	pStaSecurityMibs = sme_GetStaSecurityMibsPtr(&vmacEntry_parent[phymac]);

	if (   pStaSecurityMibs->mib_RSNConfigWPA2_p->WPA2OnlyEnabled
		|| pStaSecurityMibs->mib_RSNConfigWPA2_p->WPA2Enabled)
	{
		if (!staMib_WPA2_PSKValueEnabled[phymac])
		{
			PKCS5_PBKDF2(pStaSecurityMibs->mib_RSNConfigWPA2_p->PSKPassPhrase,
				pStaSystemMibs->mib_StaCfg_p->DesiredSsId, strlen(pStaSystemMibs->mib_StaCfg_p->DesiredSsId),
				PSKValueUr[phymac]);
		}
		else
		{
			memcpy(PSKValueUr[phymac],
				pStaSecurityMibs->mib_RSNConfigWPA2_p->PSKValue,
				RSN_PSK_VALUE_LEN_MAX);
		}

	}
	else
	{
		if (!staMib_WPA_PSKValueEnabled[phymac])
		{
			PKCS5_PBKDF2(pStaSecurityMibs->mib_RSNConfig_p->PSKPassPhrase,
				pStaSystemMibs->mib_StaCfg_p->DesiredSsId, strlen(pStaSystemMibs->mib_StaCfg_p->DesiredSsId),
				PSKValueUr[phymac]);

		}
		else
		{
			memcpy(PSKValueUr[phymac],
				pStaSecurityMibs->mib_RSNConfig_p->PSKValue,
				RSN_PSK_VALUE_LEN_MAX);
		}
	}

	KeyMgmtSta_InitSession(&vmacEntry_parent[phymac]);
	gkeyMgmtInfoSta[phymac].sta_MIC_Error.disableStaAsso = 0;
	gkeyMgmtInfoSta[phymac].sta_MIC_Error.MICCounterMeasureEnabled = 1;
	gkeyMgmtInfoSta[phymac].sta_MIC_Error.status = NO_MIC_FAILURE;
	KeyMgmtResetCounter(&gkeyMgmtInfoSta[phymac]);
}

// This routine must be called after mlmeStaInit_UR
// It assumes that parent session structures are initialized (vmacEntry_parent and mlmeStaInfo)
void KeyMgmtInit_vSta(UINT8  phymac)
{
	STA_SECURITY_MIBS * pStaSecurityMibs;

	pStaSecurityMibs = sme_GetStaSecurityMibsPtr(&vmacEntry_parent[phymac]);
	if (pStaSecurityMibs->mib_PrivacyTable_p->RSNEnabled)
	{
		InitThisStaRsnIeSta(phymac);
		//        gGrpKeyInstalled = FALSE;
		KeyMgmtInitSta(phymac);
		keyMgmtBootInit = TRUE;    
	}
}

void KeyMgmtSta_InitSession(vmacEntry_t * vmacEntry_p)
{
#ifndef PORT_TO_LINUX_OS
	STA_SECURITY_MIBS * pStaSecurityMibs = sme_GetStaSecurityMibsPtr(vmacEntry_p);
#endif /* PORT_TO_LINUX_OS */
	keyMgmtInfoSta_t  * pKeyMgmtInfoSta;
	UINT8               phymac;
	vmacStaInfo_t *vStaInfo_p = (vmacStaInfo_t *)vmacEntry_p->info_p; 		
	
	phymac = vmacEntry_p->phyHwMacIndx;
#ifndef PORT_TO_LINUX_OS
	if (   !pStaSecurityMibs->mib_RSNConfigWPA2_p->WPA2OnlyEnabled
		&& !pStaSecurityMibs->mib_RSNConfigWPA2_p->WPA2Enabled
		&& (   (pStaSecurityMibs->mib_RSNConfigUnicastCiphers_p->UnicastCipher[3] == 2)
		|| (pStaSecurityMibs->mib_RSNConfigUnicastCiphers_p->UnicastCipher[3] == 4) ) )
	{
		if(pStaSecurityMibs->mib_RSNConfigUnicastCiphers_p->UnicastCipher[3] == 2)
		{
			//Tkip
			ProcessWPAPcktStaUnicastFp[phymac] = ProcessTKIPPcktStaUr;
			DoWPAAndSchedFrameStaFp   [phymac] = DoTKIPAndSchedFrameStaUr; 
			ProcessWPAPcktStaBcastFp  [phymac] = ProcessTKIPPcktStaUr;
		} 
		else if(pStaSecurityMibs->mib_RSNConfig_p->MulticastCipher[3] == 2)
		{
			//Mixed
			ProcessWPAPcktStaUnicastFp[phymac] = ProcessCCMPPcktStaUr;
			DoWPAAndSchedFrameStaFp   [phymac] = DoCCMPAndSchedFrameStaUr;
			ProcessWPAPcktStaBcastFp  [phymac] = ProcessTKIPPcktStaUr;  
		}
		else
		{
			// AES
			ProcessWPAPcktStaUnicastFp[phymac] = ProcessCCMPPcktStaUr;
			DoWPAAndSchedFrameStaFp   [phymac] = DoCCMPAndSchedFrameStaUr;
			ProcessWPAPcktStaBcastFp  [phymac] = ProcessCCMPPcktStaUr;          
		}
	}
	else
	{    // care Mixed mode here    
		if (   (pStaSecurityMibs->mib_RSNConfigWPA2_p->MulticastCipher[3] == 4)
			&& (pStaSecurityMibs->mib_RSNConfigWPA2UnicastCiphers_p->UnicastCipher[3] == 4))
		{
			ProcessWPAPcktStaUnicastFp[phymac] = ProcessCCMPPcktStaUr;
			DoWPAAndSchedFrameStaFp   [phymac] = DoCCMPAndSchedFrameStaUr;
			ProcessWPAPcktStaBcastFp  [phymac] = ProcessCCMPPcktStaUr;
		}
		else if (   (pStaSecurityMibs->mib_RSNConfigWPA2_p->MulticastCipher[3] == 2)
			&& (pStaSecurityMibs->mib_RSNConfigWPA2UnicastCiphers_p->UnicastCipher[3] == 4) )
		{
			//Tkip for Multi, CCMP for uni
			ProcessWPAPcktStaUnicastFp[phymac] = ProcessCCMPPcktStaUr;;
			DoWPAAndSchedFrameStaFp   [phymac] = DoCCMPAndSchedFrameStaUr;   
			ProcessWPAPcktStaBcastFp  [phymac] = ProcessTKIPPcktStaUr;
		}
		else
		{
			//Tkip for Multi, Tkip for uni
			ProcessWPAPcktStaUnicastFp[phymac] = ProcessTKIPPcktStaUr;;
			DoWPAAndSchedFrameStaFp   [phymac] = DoTKIPAndSchedFrameStaUr;   
			ProcessWPAPcktStaBcastFp  [phymac] = ProcessTKIPPcktStaUr;
		}
	}
#endif /* PORT_TO_LINUX_OS */

	pKeyMgmtInfoSta = sme_GetKeyMgmtInfoStaPtr(vmacEntry_p);
	pKeyMgmtInfoSta->pKeyData->RSNDataTrafficEnabled = 0;
	pKeyMgmtInfoSta->pKeyData->RSNSecured            = 0;
	vStaInfo_p->staSecurityMibs.mib_PrivacyTable_p->RSNLinkStatus = 0;  	
#ifdef PORT_TO_LINUX_OS
	keyMgmtStaStopTimer(pKeyMgmtInfoSta, (UINT8 *)&pKeyMgmtInfoSta->keyMgmtStaHskHsm.rsnSecuredTimer);
#else
	if (pKeyMgmtInfoSta->keyMgmtStaHskHsm.rsnSecuredTimer.active)
	{
		TimerRemove(&pKeyMgmtInfoSta->keyMgmtStaHskHsm.rsnSecuredTimer);
	}
	else
	{
		TimerInit(&pKeyMgmtInfoSta->keyMgmtStaHskHsm.rsnSecuredTimer);
	}
#endif /* PORT_TO_LINUX_OS */
}

void ProcessKeyMgmtDataStaUr(EAPOL_KeyMsg_t *pEAPoL, MhsmEvent_t *msg, vmacEntry_t * thisVmacEntry_p)
{
	keyMgmtStahsk_hsm_t *  me_p;
	keyMgmtInfoSta_t    *  pKeyMgmtInfo;
#ifndef PORT_TO_LINUX_OS
	key_info_t             tmpKeyInfo;
#endif /* PORT_TO_LINUX_OS */


	pKeyMgmtInfo = sme_GetKeyMgmtInfoStaPtr(thisVmacEntry_p);
	me_p = &(pKeyMgmtInfo->keyMgmtStaHskHsm);

#ifndef PORT_TO_LINUX_OS
	memcpy(&tmpKeyInfo.key_info16,
		&pEAPoL->k.key_info16,
		sizeof(key_info_t));
#endif /* PORT_TO_LINUX_OS */

	switch (pEAPoL->k.key_info16)
	{
	case PWKMSG1_TKIP:
	case PWKMSG1_CCMP:
		KeyMgmtStaHsk_Recvd_PWKMsg1(me_p, msg);
		break;
	case PWKMSG3_TKIP_1:
	case PWKMSG3_TKIP_2:
	case PWKMSG3_TKIP_3:
	case PWKMSG3_CCMP_1:
	case PWKMSG3_CCMP_2:
		KeyMgmtStaHsk_Recvd_PWKMsg3(me_p, msg);
		break;
	case GRPMSG1_TKIP_1:
	case GRPMSG1_TKIP_2:
	case GRPMSG1_TKIP_3:
	case GRPMSG1_TKIP_4:
	case GRPMSG1_TKIP_5:
	case GRPMSG1_TKIP_6:
	case GRPMSG1_TKIP_7:
	case GRPMSG1_TKIP_8:
	case GRPMSG1_TKIP_9:
	case GRPMSG1_CCMP_1:
	case GRPMSG1_CCMP_2:
	case GRPMSG1_CCMP_3:
	case GRPMSG1_CCMP_4:
	case GRPMSG1_CCMP_5:
	case GRPMSG1_CCMP_6:
	case GRPMSG1_CCMP_7:
		KeyMgmtStaHsk_Recvd_GrpMsg1(me_p, msg);
		
		break;
	default:
		break;
	}
}

Status_e SendMICFailReport_sta(keyMgmtInfoSta_t * keyMgmtInfoSta_p, BOOLEAN isUnicast)
{
	EAPOL_KeyMsg_t *pTx_eapol;
	UINT32 frameLen;
	UINT8 MIC[EAPOL_MIC_SIZE + 4];
	//apio_bufdescr_t TxFrm;
	vmacEntry_t *vmacEntry_p = (vmacEntry_t *)(keyMgmtInfoSta_p->vmacEntry_p);

#ifdef PORT_TO_LINUX_OS
	STA_SECURITY_MIBS * pStaSecurityMibs = sme_GetStaSecurityMibsPtr(vmacEntry_p);
	UINT8 *frm;
	struct sk_buff *skb;
	struct net_device *dev;
	dev = ((vmacEntry_t *)keyMgmtInfoSta_p->vmacEntry_p)->privInfo_p;
#endif /* PORT_TO_LINUX_OS */

	if((keyMgmtInfoSta_p == NULL) || (vmacEntry_p == NULL))
	{
		return FAIL;
	}
#ifdef PORT_TO_LINUX_OS
	if ((skb = ieee80211_getDataframe(&frm, EAPOL_TX_BUF)) == NULL)
	{
		WLDBG_INFO(DBG_LEVEL_5, "Error: cannot get socket buffer. \n ");
		return FAIL;
	}
	pTx_eapol = (EAPOL_KeyMsg_t *)skb->data;
#else
	if (apio_alloc(WlanHandle, &TxFrm) != APCTL_OK)
	{
		return FAIL;
	}
	pTx_eapol = (EAPOL_KeyMsg_t *)TxFrm.framePtr;
#endif /* PORT_TO_LINUX_OS */
	if(keyMgmtInfoSta_p->staCounterHi == 0xffffffff 
		&& keyMgmtInfoSta_p->staCounterLo == 0xffffffff)
	{
		smeSndLinkLostInd(vmacEntry_p, IEEEtypes_REASON_UNSPEC);
		KeyMgmtResetCounter(keyMgmtInfoSta_p);
		return FAIL;
	}
	MACADDR_CPY(pTx_eapol->Ether_Hdr.da, GetParentStaBSSID(vmacEntry_p->phyHwMacIndx));
	MACADDR_CPY(pTx_eapol->Ether_Hdr.sa, &vmacEntry_p->vmacAddr[0]);
	pTx_eapol->Ether_Hdr.type = IEEE_ETHERTYPE_PAE;//EAPOL Msg
	pTx_eapol->desc_type = 254;

#ifdef PORT_TO_LINUX_OS
	if (pStaSecurityMibs->mib_RSNConfigWPA2_p->WPA2OnlyEnabled || pStaSecurityMibs->mib_RSNConfigWPA2_p->WPA2Enabled
		|| pStaSecurityMibs->mib_RSNConfigUnicastCiphers_p->UnicastCipher[3] == 4)
#else
	if (mib_RSNConfigWPA2_p[MAC_0]->WPA2OnlyEnabled || mib_RSNConfigWPA2_p[MAC_0]->WPA2Enabled
		|| mib_RSNConfigUnicastCiphers_p[MAC_0]->UnicastCipher[3] == 4)
#endif/* PORT_TO_LINUX_OS */
	{
		pTx_eapol->k.key_info.desc_ver = 2;
	}
	else
	{
		pTx_eapol->k.key_info.desc_ver = 1;
	}
	pTx_eapol->k.key_info.key_type = isUnicast;
	pTx_eapol->k.key_info.key_index = 0;
	pTx_eapol->k.key_info.install = 0;
	pTx_eapol->k.key_info.key_ack = 0;
	pTx_eapol->k.key_info.key_MIC = 1;
	pTx_eapol->k.key_info.secure = 1;
	pTx_eapol->k.key_info.error = 1;
	pTx_eapol->k.key_info.request = 1;
	pTx_eapol->k.key_info.rsvd = 0;
	pTx_eapol->k.key_info16 = SHORT_SWAP(pTx_eapol->k.key_info16);
	pTx_eapol->key_length = 0;
	pTx_eapol->replay_cnt[0] = WORD_SWAP(keyMgmtInfoSta_p->staCounterHi);
	pTx_eapol->replay_cnt[1] = WORD_SWAP(keyMgmtInfoSta_p->staCounterLo);
	if(keyMgmtInfoSta_p->staCounterLo++ == 0)
	{
		keyMgmtInfoSta_p->staCounterHi++;
	}
	memset(pTx_eapol->key_nonce, 0, NONCE_SIZE);
	memset(pTx_eapol->EAPOL_key_IV,0,16);
	memset(pTx_eapol->key_RSC,0,8);
	memset(pTx_eapol->key_ID,0,8);
	pTx_eapol->key_material_len = 0;
	frameLen = 95;
	Insert8021xHdr(&pTx_eapol->hdr_8021x, (UINT16)frameLen);
	pTx_eapol->key_material_len = SHORT_SWAP(pTx_eapol->key_material_len);
	ComputeEAPOL_MIC_DualMacSta((UINT8 *)&pTx_eapol->hdr_8021x, frameLen + sizeof(Hdr_8021x_t),
		keyMgmtInfoSta_p->EAPOL_MIC_Key, EAPOL_MIC_KEY_SIZE, MIC, 0, vmacEntry_p);
	apppendEAPOL_MIC(pTx_eapol->key_MIC,MIC);
#ifdef PORT_TO_LINUX_OS
	keyMgmtTxData_UnEncrypted(dev, skb, NULL);
#else
	TxFrm.phymac_for_wlanread = vmacEntry_p->phyHwMacIndx;
	TxFrm.frameLen = frameLen + sizeof(ether_hdr_t) + HDR_8021x_LEN;
	TxFrm.to_trunk = vmacEntry_p->trunkId;
	apio_urWlanWrite(&TxFrm);
#endif /* PORT_TO_LINUX_OS */
	return SUCCESS;
}

void CounterMeasureInit_Sta(MIC_Error_t  *sta_MIC_Error_p, BOOLEAN optEnabled)
{
#ifdef PORT_TO_LINUX_OS
	keyMgmtStaStopTimer(NULL, (UINT8 *)&sta_MIC_Error_p->timer);
#else
	if(sta_MIC_Error_p->timer.active)
	{
		TimerRemove(&sta_MIC_Error_p->timer);
	}
#endif /* PORT_TO_LINUX_OS */
	sta_MIC_Error_p->status = NO_MIC_FAILURE;
	sta_MIC_Error_p->disableStaAsso = 0;
	if(optEnabled)
	{
		sta_MIC_Error_p->MICCounterMeasureEnabled = 1;
	}
	else
	{
		sta_MIC_Error_p->MICCounterMeasureEnabled = 0;
	}

}

void MicErrTimerExp_Sta(UINT8 *data)
{
	MIC_Error_t  *sta_MIC_Error_p = (MIC_Error_t  *)data;

	if(sta_MIC_Error_p)
	{
		sta_MIC_Error_p->status = NO_MIC_FAILURE;
		sta_MIC_Error_p->disableStaAsso = 0;
	}
}

void MICCounterMeasureInvoke_Sta(vmacEntry_t *vmacEntry_p, BOOLEAN isUnicast)
{
	MIC_Fail_State_e status;
	keyMgmtInfoSta_t * keyMgmtInfo_p;
    UINT8 AssociatedFlag = 0;
    UINT8 bssId[6];
#ifdef PORT_TO_LINUX_OS
	STA_SECURITY_MIBS * pStaSecurityMibs;
#endif /* PORT_TO_LINUX_OS */

	if(vmacEntry_p == NULL)
	{
		return;
	}
    smeGetStaLinkInfo(vmacEntry_p->id,
        &AssociatedFlag,
        &bssId[0]);
    if (!AssociatedFlag)
    {
        return;
    }
	if((keyMgmtInfo_p = sme_GetKeyMgmtInfoStaPtr(vmacEntry_p)) == NULL)
	{
		return;
	} 
    if (!keyMgmtInfo_p->pKeyData->RSNDataTrafficEnabled)
    { 
        return;        
    }
#ifdef PORT_TO_LINUX_OS
	pStaSecurityMibs = sme_GetStaSecurityMibsPtr(vmacEntry_p);
#endif /* PORT_TO_LINUX_OS */

	if (keyMgmtInfo_p->sta_MIC_Error.MICCounterMeasureEnabled)
	{
		ENTER_CRITICAL;
		status = keyMgmtInfo_p->sta_MIC_Error.status;
		EXIT_CRITICAL;

		switch (status)
		{
		case NO_MIC_FAILURE:
			SendMICFailReport_sta(keyMgmtInfo_p, isUnicast);
#ifdef PORT_TO_LINUX_OS
            keyMgmtStaStopTimer(NULL, (UINT8 *)&keyMgmtInfo_p->sta_MIC_Error.timer);
			keyMgmtStaStartTimer((UINT8 *)&keyMgmtInfo_p->sta_MIC_Error, 
				(UINT8 *)&keyMgmtInfo_p->sta_MIC_Error.timer,
				&MicErrTimerExp_Sta,
				MIC_ERROR_QUIET_TIME_INTERVAL);
#else
			if(!keyMgmtInfo_p->sta_MIC_Error.timer.active)
			{
				TimerInit(&keyMgmtInfo_p->sta_MIC_Error.timer);
				TimerFireIn(&keyMgmtInfo_p->sta_MIC_Error.timer, 1, 
					&MicErrTimerExp_Sta, (UINT8 *)&keyMgmtInfo_p->sta_MIC_Error, 
					MIC_ERROR_QUIET_TIME_INTERVAL );
			}
			else
			{
				TimerRearm(&keyMgmtInfo_p->sta_MIC_Error.timer, MIC_ERROR_QUIET_TIME_INTERVAL);
			}
#endif /* PORT_TO_LINUX_OS */
			ENTER_CRITICAL;
			keyMgmtInfo_p->sta_MIC_Error.status = FIRST_MIC_FAIL_IN_60_SEC;
			EXIT_CRITICAL;
			break;
		case FIRST_MIC_FAIL_IN_60_SEC:
			keyMgmtInfo_p->sta_MIC_Error.disableStaAsso = 1;
#ifdef PORT_TO_LINUX_OS
			if(pStaSecurityMibs)
			{
				pStaSecurityMibs->mib_RSNStats_p->TKIPCounterMeasuresInvoked++;
			}
#else
			mib_RSNStats_p[MAC_0]->TKIPCounterMeasuresInvoked++;
#endif /* PORT_TO_LINUX_OS */
			SendMICFailReport_sta(keyMgmtInfo_p, isUnicast);
			/*send DeAuth msg and terminate Link */
			smeSndLinkLostInd(vmacEntry_p, IEEEtypes_REASON_MIC_FAILURE);
			ENTER_CRITICAL;
			keyMgmtInfo_p->sta_MIC_Error.status = SECOND_MIC_FAIL_IN_60_SEC;
			EXIT_CRITICAL;
			//start timer for 60 seconds
#ifdef PORT_TO_LINUX_OS
            keyMgmtStaStopTimer(NULL, (UINT8 *)&keyMgmtInfo_p->sta_MIC_Error.timer);
			keyMgmtStaStartTimer((UINT8 *)&keyMgmtInfo_p->sta_MIC_Error, 
				(UINT8 *)&keyMgmtInfo_p->sta_MIC_Error.timer,
				&MicErrTimerExp_Sta,
				MIC_ERROR_QUIET_TIME_INTERVAL+MIC_ERROR_QUIET_TIME_INTERVAL_CAL);
#else
			if(!keyMgmtInfo_p->sta_MIC_Error.timer.active)
			{
				TimerInit(&keyMgmtInfo_p->sta_MIC_Error.timer);
				TimerFireIn(&keyMgmtInfo_p->sta_MIC_Error.timer, 1, 
					&MicErrTimerExp_Sta, (UINT8 *)&keyMgmtInfo_p->sta_MIC_Error, 
					MIC_ERROR_QUIET_TIME_INTERVAL+MIC_ERROR_QUIET_TIME_INTERVAL_CAL);
			}
			else
			{
				TimerRearm(&keyMgmtInfo_p->sta_MIC_Error.timer, MIC_ERROR_QUIET_TIME_INTERVAL+MIC_ERROR_QUIET_TIME_INTERVAL_CAL);
			}
#endif /* PORT_TO_LINUX_OS */
			break;
		case SECOND_MIC_FAIL_IN_60_SEC:
			//Reset timer for 60 seconds
			SendMICFailReport_sta(keyMgmtInfo_p, isUnicast);
#ifdef PORT_TO_LINUX_OS
            keyMgmtStaStopTimer(NULL, (UINT8 *)&keyMgmtInfo_p->sta_MIC_Error.timer);
			keyMgmtStaStartTimer((UINT8 *)&keyMgmtInfo_p->sta_MIC_Error, 
				(UINT8 *)&keyMgmtInfo_p->sta_MIC_Error.timer,
				&MicErrTimerExp_Sta,
				MIC_ERROR_QUIET_TIME_INTERVAL+MIC_ERROR_QUIET_TIME_INTERVAL_CAL);
#else
			if(!keyMgmtInfo_p->sta_MIC_Error.timer.active)
			{
				TimerInit(&keyMgmtInfo_p->sta_MIC_Error.timer);
				TimerFireIn(&keyMgmtInfo_p->sta_MIC_Error.timer, 1, 
					&MicErrTimerExp_Sta, (UINT8 *)&keyMgmtInfo_p->sta_MIC_Error, 
					MIC_ERROR_QUIET_TIME_INTERVAL+MIC_ERROR_QUIET_TIME_INTERVAL_CAL);
			}
			else
			{
				TimerRearm(&keyMgmtInfo_p->sta_MIC_Error.timer, MIC_ERROR_QUIET_TIME_INTERVAL+MIC_ERROR_QUIET_TIME_INTERVAL_CAL);
			}
#endif /* PORT_TO_LINUX_OS */
			break;
		default:
			break;
		}
	}
	return;
}

SINT32 testMic_Sta(UINT8 count0, UINT8 count1)
{
	return SendMICFailReport_sta(&gkeyMgmtInfoSta[0], count0);
}

static void keyMgmtStaRsnSecuredTimeoutHandler(UINT8 * ctx)
{
#ifndef PORT_TO_LINUX_OS
	EVTBUF_EVT_CLIENT_SUBTYPE_DISCONNECT   evtBufLink;
#endif /* PORT_TO_LINUX_OS */
	vmacEntry_t                          * vmacEntry_p;
	keyMgmtInfoSta_t                     * pKeyMgmtInfoSta;

	pKeyMgmtInfoSta = (keyMgmtInfoSta_t *)ctx;
	vmacEntry_p     = (vmacEntry_t *)(pKeyMgmtInfoSta->vmacEntry_p);

	if (pKeyMgmtInfoSta->pKeyData->RSNSecured == 0)
	{
#ifndef PORT_TO_LINUX_OS
		// generate disconnect event
		memset((void *)&evtBufLink,
			0,
			sizeof(EVTBUF_EVT_CLIENT_SUBTYPE_DISCONNECT));
		evtBufLink.phyIndex = vmacEntry_p->phyHwMacIndx;

		eventGenerate(EVT_CLIENT, 
			EVT_CLIENT_SUBTYPE_DISCONNECT, 
			sizeof(evtBufLink), 
			&evtBufLink);
#endif /* PORT_TO_LINUX_OS */

		// Declare link loss
		smeSndLinkLostInd(vmacEntry_p, IEEEtypes_REASON_UNSPEC);
	}
}

extern void keyMgmtSta_StartSession(vmacEntry_t * vmacEntry_p)
{
	keyMgmtInfoSta_t  * pKeyMgmtInfoSta;
	Timer             * pTimer;
#ifdef PORT_TO_LINUX_OS
	STA_SECURITY_MIBS * pStaSecurityMibs;
#endif

	pKeyMgmtInfoSta = sme_GetKeyMgmtInfoStaPtr(vmacEntry_p);
	pTimer          = &pKeyMgmtInfoSta->keyMgmtStaHskHsm.rsnSecuredTimer;

	// start timer to check for completion of handshake

#ifdef PORT_TO_LINUX_OS

	pStaSecurityMibs = sme_GetStaSecurityMibsPtr(vmacEntry_p);

	keyMgmtStaStartTimer((UINT8 *)pKeyMgmtInfoSta, 
		(UINT8 *)pTimer,
		&keyMgmtStaRsnSecuredTimeoutHandler,
		gStaRsnSecuredTimeout);
#else
	if (pTimer->active)
	{
		TimerRearm(pTimer, gStaRsnSecuredTimeout);
	}
	else
	{
		TimerInit(pTimer);
		TimerFireIn(pTimer,
			1,
			keyMgmtStaRsnSecuredTimeoutHandler,
			(UINT8 *)pKeyMgmtInfoSta,
			gStaRsnSecuredTimeout);
	}
#endif /* PORT_TO_LINUX_OS */

	// reset the authenticator replay counter
	pKeyMgmtInfoSta->apCounterLo       = 0;
	pKeyMgmtInfoSta->apCounterHi       = 0;
	pKeyMgmtInfoSta->apCounterZeroDone = 0;
}

#ifdef PORT_TO_LINUX_OS
void * ProcessEAPoLSta(IEEEtypes_8023_Frame_t *pEAPoLPckt, IEEEtypes_MacAddr_t *staAddr_p)
{
	EAPOL_KeyMsg_t *pEAPoL;
	MhsmEvent_t msg;
	STA_SECURITY_MIBS * pStaSecurityMibs;
	vmacEntry_t * thisVmacEntry_p;

	if((thisVmacEntry_p = vmacGetVMacEntryByAddr((UINT8 *)staAddr_p)) == NULL)
	{
		return NULL;
	}

	pStaSecurityMibs = sme_GetStaSecurityMibsPtr(thisVmacEntry_p);

	if ( ! pStaSecurityMibs->mib_PrivacyTable_p->RSNEnabled)
	{
		return NULL;
	}

	pEAPoL = (EAPOL_KeyMsg_t *)pEAPoLPckt;
	if (pEAPoL->hdr_8021x.pckt_type == 0x03)/*key data */
	{
		msg.event = MSGRECVD_EVT;
		msg.pBody = (unsigned char *)pEAPoL;
	}
	else/*reject all other EAPoL packet for now.... */
	{
		return NULL;
	}

	ProcessKeyMgmtDataStaUr(pEAPoL, &msg, thisVmacEntry_p);

	return NULL;
}

extern MIB_AUTH_ALG                        * staMib_AuthAlg_p[NUM_OF_WLMACS];
extern MIB_PRIVACY_TABLE                   * staMib_PrivacyTable_p[NUM_OF_WLMACS];
#ifdef WPA_STA
#ifdef WPA2
extern MIB_RSNCONFIG                       * staMib_RSNConfig_p[NUM_OF_WLMACS];
extern MIB_RSNCONFIG_UNICAST_CIPHERS       * staMib_RSNConfigUnicastCiphers_p[NUM_OF_WLMACS];
extern MIB_RSNSTATS                        * staMib_RSNStats_p[NUM_OF_WLMACS];
extern IEEEtypes_RSN_IE_t *staMib_thisStaRsnIE_p[NUM_OF_WLMACS];
#ifdef AP_WPA2
extern IEEEtypes_RSN_IE_WPA2_t *staMib_thisStaRsnIEWPA2_p[NUM_OF_WLMACS];
extern MIB_RSNCONFIGWPA2                   * staMib_RSNConfigWPA2_p[NUM_OF_WLMACS];
extern MIB_RSNCONFIGWPA2_UNICAST_CIPHERS   * staMib_RSNConfigWPA2UnicastCiphers_p[NUM_OF_WLMACS];
extern MIB_RSNCONFIGWPA2_UNICAST_CIPHERS   * staMib_RSNConfigWPA2UnicastCiphers2_p[NUM_OF_WLMACS];
extern MIB_RSNCONFIGWPA2_AUTH_SUITES       * staMib_RSNConfigWPA2AuthSuites_p[NUM_OF_WLMACS];
#endif
#endif
#endif

UINT8   tmpClientSSID[NUM_OF_WLMACS][32];
#define DEFAULT_PASS_PHRASE "1234567890"

void defaultKeyMgmtInit(UINT8 phymacIndex)
{
	STA_SYSTEM_MIBS   * pStaSystemMibs;
	STA_SECURITY_MIBS * pStaSecurityMibs;

	//BOOLEAN pskPassPhrase = TRUE;
	UINT8 rsnPskMaterial[RSN_PSK_PASS_PHRASE_LEN_MAX];
	UINT8 rsn_mode;
	UINT32 mCipherId;
	UINT32 uCipherId;
	UINT32 aCipherId;

#ifdef RSN_RESOLVE
	struct net_device *staDev      = NULL;
	vmacEntry_t       *vmacEntry_p = NULL;
	struct wlprivate  *stapriv     = NULL;
	vmacApInfo_t      *vmacSta_p   = NULL;
	MIB_802DOT11      *mib         = NULL;

	vmacEntry_p = sme_GetParentVMacEntry(phymacIndex);
	staDev = (struct net_device *)vmacEntry_p->privInfo_p;
	stapriv = NETDEV_PRIV_P(struct wlprivate, staDev);
	vmacSta_p = stapriv->vmacSta_p;
	mib = vmacSta_p->Mib802dot11;	
#endif /* RSN_RESOLVE */

	pStaSystemMibs   = sme_GetStaSystemMibsPtr(&vmacEntry_parent[phymacIndex]);
	pStaSecurityMibs = sme_GetStaSecurityMibsPtr(&vmacEntry_parent[phymacIndex]);

	memset(&rsnPskMaterial[0], 0, RSN_PSK_PASS_PHRASE_LEN_MAX);
	strcpy(&rsnPskMaterial[0], DEFAULT_PASS_PHRASE);
	staMib_WPA_PSKValueEnabled [phymacIndex] = 0;
	staMib_WPA2_PSKValueEnabled[phymacIndex] = 0;

	/* Set SSID */
	memset(&pStaSystemMibs->mib_StaCfg_p->DesiredSsId[0], 0, 33);
	strcpy(&pStaSystemMibs->mib_StaCfg_p->DesiredSsId[0], tmpClientSSID[phymacIndex]);

	/* Set Mibs */
	staMib_PrivacyTable_p[phymacIndex]->RSNEnabled = mib->Privacy->RSNEnabled;
	staMib_AuthAlg_p[phymacIndex]->Type = mib->AuthAlg->Type;
	staMib_PrivacyTable_p[phymacIndex]->PrivInvoked = mib->Privacy->PrivInvoked;
    staMib_WPA_PSKValueEnabled[phymacIndex] = *(mib->mib_WPAPSKValueEnabled);

	staMib_RSNConfigWPA2_p[phymacIndex]->WPA2Enabled = mib->RSNConfigWPA2->WPA2Enabled;
	staMib_RSNConfigWPA2_p[phymacIndex]->WPA2OnlyEnabled = mib->RSNConfigWPA2->WPA2OnlyEnabled;
    staMib_WPA2_PSKValueEnabled[phymacIndex] = *(mib->mib_WPA2PSKValueEnabled);

	if (mib->Privacy->RSNEnabled)
	{
		if (staMib_RSNConfigWPA2_p[phymacIndex]->WPA2Enabled && staMib_RSNConfigWPA2_p[phymacIndex]->WPA2OnlyEnabled)
		{
			rsn_mode= RSN_WPA2_ID;

			rsnMultiCastCipher[0] = 0x00;
			rsnMultiCastCipher[1] = 0x0f;
			rsnMultiCastCipher[2] = 0xac;
			rsnMultiCastCipher[3] = 0x04;
			mCipherId = RSN_AES_ID;

			rsnUniCastCipher[0]   = 0x00;
			rsnUniCastCipher[1]   = 0x0f;
			rsnUniCastCipher[2]   = 0xac;
			rsnUniCastCipher[3]   = 0x04;
			uCipherId = RSN_AES_ID;

			rsnAuthSuite[0]       = 0x00;
			rsnAuthSuite[1]       = 0x0f;
			rsnAuthSuite[2]       = 0xac;
			rsnAuthSuite[3]       = 0x02;
			aCipherId = RSN_PSK_ID;

			memcpy(staMib_RSNConfigWPA2_p[phymacIndex]->MulticastCipher,
				rsnMultiCastCipher,
				RSN_CIPHER_VALUE_LEN_MAX);
			memcpy(staMib_RSNConfigWPA2UnicastCiphers_p[phymacIndex]->UnicastCipher,
				rsnUniCastCipher,
				RSN_CIPHER_VALUE_LEN_MAX);
			staMib_RSNConfigWPA2UnicastCiphers_p[phymacIndex]->Enabled = TRUE;
			memcpy(staMib_RSNConfigWPA2UnicastCiphers2_p[phymacIndex]->UnicastCipher,
				rsnUniCastCipher,
				RSN_CIPHER_VALUE_LEN_MAX);
			staMib_RSNConfigWPA2UnicastCiphers2_p[phymacIndex]->Enabled = TRUE;
			/* Set Auth Suite */
			memcpy(staMib_RSNConfigWPA2AuthSuites_p[phymacIndex]->AuthSuites,
				rsnAuthSuite, 
				RSN_SUITE_VALUE_LEN_MAX);
			staMib_RSNConfigWPA2AuthSuites_p[phymacIndex]->Enabled = TRUE;

			memcpy(staMib_RSNConfigWPA2_p[phymacIndex]->PSKPassPhrase, 
				mib->RSNConfigWPA2->PSKPassPhrase,
				RSN_PSK_PASS_PHRASE_LEN_MAX);

			memcpy(staMib_RSNConfigWPA2_p[phymacIndex]->PSKValue, 
				mib->RSNConfigWPA2->PSKValue,
				RSN_PSK_VALUE_LEN_MAX);
		}
		else
		{
			rsn_mode = RSN_WPA_ID;
			rsnMultiCastCipher[0] = 0x00;
			rsnMultiCastCipher[1] = 0x50;
			rsnMultiCastCipher[2] = 0xf2;
			rsnMultiCastCipher[3] = 0x02;
			mCipherId = RSN_TKIP_ID;

			rsnUniCastCipher[0]   = 0x00;
			rsnUniCastCipher[1]   = 0x50;
			rsnUniCastCipher[2]   = 0xf2;
			rsnUniCastCipher[3]   = 0x02;
			uCipherId = RSN_TKIP_ID;

			rsnAuthSuite[0]       = 0x00;
			rsnAuthSuite[1]       = 0x50;
			rsnAuthSuite[2]       = 0xf2;
			rsnAuthSuite[3]       = 0x02;
			aCipherId = RSN_PSK_ID;

			memcpy(staMib_RSNConfig_p[phymacIndex]->MulticastCipher,
				rsnMultiCastCipher,
				RSN_CIPHER_VALUE_LEN_MAX);
			memcpy(staMib_RSNConfigUnicastCiphers_p[phymacIndex]->UnicastCipher,
				rsnUniCastCipher,
				RSN_CIPHER_VALUE_LEN_MAX);
			staMib_RSNConfigUnicastCiphers_p[phymacIndex]->Enabled = TRUE;

			memcpy(staMib_RSNConfig_p[phymacIndex]->PSKPassPhrase, 
				mib->RSNConfig->PSKPassPhrase,
				RSN_PSK_PASS_PHRASE_LEN_MAX);

			memcpy(staMib_RSNConfig_p[phymacIndex]->PSKValue, 
				mib->RSNConfig->PSKValue,
				RSN_PSK_VALUE_LEN_MAX);
		}
		KeyMgmtInitSta(phymacIndex);
		keyMgmtUpdateRsnIE(phymacIndex,
			rsn_mode,
			mCipherId,
			uCipherId,
			aCipherId);
	}


}
#endif/* PORT_TO_LINUX_OS */
