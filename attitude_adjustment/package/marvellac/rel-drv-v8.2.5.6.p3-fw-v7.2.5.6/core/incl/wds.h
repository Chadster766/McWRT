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

#ifndef _WDS_H_

#include "IEEE_types.h"
#include "mib.h"
#include "StaDb.h"
#include "hostcmd.h"
#include "wldebug.h"

extern BOOLEAN validWdsIndex(UINT8 wdsIndex);
extern BOOLEAN setWdsPort(WL_PRIV *, UINT8 *pMacAddr, UINT8 wdsIndex, UINT8 wdsPortMode);
extern void getWdsModeStr(char *wdsModeStr, UINT8 wdsPortMode);
extern void AP_InitWdsPorts(WL_PRIV *);
extern BOOLEAN wdsPortActive(WL_PRIV *,UINT8 wdsIndex);
extern void wds_wlDeinit(WL_PRIV *);
extStaDb_StaInfo_t *updateWds(WL_PRIV *, WL_NETDEV *);
extern struct wds_port *getWdsPortFromNetDev(WL_PRIV *wlpptr, WL_NETDEV *);
void setWdsPeerInfo(PeerInfo_t *pWdsPeerInfo, UINT8 Mode);
#endif
