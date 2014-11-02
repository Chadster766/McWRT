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



#ifndef _WDS_H_

#include "IEEE_types.h"
#include "mib.h"
#include "StaDb.h"
#include "idList.h"
#include "ap8xLnxIntf.h"
#include "ap8xLnxVer.h"
#include "hostcmd.h"
#include "wldebug.h"

extern BOOLEAN validWdsIndex(UINT8 wdsIndex);
extern BOOLEAN setWdsPort(struct net_device *netdev, UINT8 *pMacAddr, UINT8 wdsIndex, UINT8 wdsPortMode);
extern void getWdsModeStr(char *wdsModeStr, UINT8 wdsPortMode);
extern void AP_InitWdsPorts(struct wlprivate *wlpptr);
extern void wlprobeInitWds(struct wlprivate *wlpptr);
extern BOOLEAN wdsPortActive(struct net_device *netdev,UINT8 wdsIndex);
extern void wds_wlDeinit(struct net_device *netdev);
extern void setWdsPortMacAddr(struct net_device *netdev, UINT8 *pMacAddr);
extStaDb_StaInfo_t *updateWds(struct net_device *netdev);
extern struct wds_port *getWdsPortFromNetDev(struct wlprivate *wlpptr, struct net_device *netdev);
#endif
