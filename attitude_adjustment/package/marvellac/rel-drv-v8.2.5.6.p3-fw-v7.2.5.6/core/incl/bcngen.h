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

#ifndef _BCNGEN_H_
#define _BCNGEN_H_

#include "wlmac.h"
#include "macmgmtap.h"

#define SETBIT    1
#define RESETBIT  0
#define SET_ERP_PROTECTION 1
#define RESET_ERP_PROTECTION 0

extern macmgmtQ_MgmtMsg_t * BcnBuffer_p;
extern void bcngen_UpdateBeaconBuffer(WL_PRIV *wlpptr,IEEEtypes_StartCmd_t *StartCmd_p);
extern void bcngen_UpdateBitInTim(UINT16 Aid, BOOLEAN Set);
extern void bcngen_BeaconFreeIsr(void);
extern void bcngen_UpdateBeaconErpInfo(WL_PRIV *wlpptr,BOOLEAN SetFlag);
extern void bcngen_EnableBcnFreeIntr(void);

extern UINT16 AddRSN_IEWPA2_TO(IEEEtypes_RSN_IE_WPA2_t *thisStaRsnIEWPA2_p,IEEEtypes_RSN_IE_WPA2_t* pNextElement);

// Add RSN IE to a frame body
UINT16 AddRSN_IE(WL_PRIV *wlpptr,IEEEtypes_RSN_IE_t* pNextElement);

/* IEEE80211H */
void bcngen_AddChannelSwithcAnnouncement_IE(WL_PRIV *wlpptr,IEEEtypes_ChannelSwitchAnnouncementElement_t *pChannelSwitchAnnouncementIE);
void bcngen_RemoveChannelSwithcAnnouncement_IE(WL_PRIV *wlpptr);
void bcngen_AddQuiet_IE(WL_PRIV *wlpptr,IEEEtypes_QuietElement_t *pQuietIE);
void bcngen_RemoveQuiet_IE(WL_PRIV *wlpptr);

#ifdef UR_WPA
void InitThisStaRsnIEUr(WL_PRIV *wlpptr);
#endif

UINT16 AddHT_IE(WL_PRIV *wlpptr,IEEEtypes_HT_Element_t* pNextElement);
UINT16 AddRSN_IE_TO(IEEEtypes_RSN_IE_t *thisStaRsnIE_p, IEEEtypes_RSN_IE_t* pNextElement);
extern UINT16 AddM_IE(WL_PRIV *wlpptr, IEEEtypes_HT_Element_t* pNextElement);
extern UINT16 Add_Generic_AddHT_IE(WL_PRIV *wlpptr, IEEEtypes_Generic_Add_HT_Element_t* pNextElement);
extern UINT16 Add_Generic_HT_IE(WL_PRIV *wlpptr,IEEEtypes_Generic_HT_Element_t* pNextElement);
extern UINT16 AddAddHT_IE(WL_PRIV *wlpptr,IEEEtypes_Add_HT_Element_t* pNextElement);
extern void InitThisStaRsnIE(WL_PRIV *wlpptr);
extern UINT16 AddM_Rptr_IE(WL_PRIV *wlpptr, IEEEtypes_HT_Element_t* pNextElement);
extern UINT8 bcn_reg_domain; /* IEEE80211H */
extern UINT16 AddRSN_IEWPA2MixedMode(WL_PRIV *wlpptr,IEEEtypes_RSN_IE_WPA2MixedMode_t* pNextElement);
UINT16 AddRSN_IEWPA2(WL_PRIV *wlpptr,IEEEtypes_RSN_IE_WPA2_t* pNextElement);
/* WPS2_SUPPORT */
UINT16 Build_AssocResp_WSCIE(WL_PRIV *wlpptr, AssocResp_WSCIE_t *pNextElement);

UINT16 Build_IE_191(WL_PRIV *wlpptr, UINT8*IE_p);
UINT16 Build_IE_192(WL_PRIV *wlpptr, UINT8*IE_p);

#endif /* _BCNGEN_H_ */
