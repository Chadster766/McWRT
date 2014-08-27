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

#ifndef	AP8X_API_H_
#define	AP8X_API_H_
#include "osif.h"

int wldo_ioctl(WL_NETDEV *dev , struct ifreq  *rq, int cmd);
struct iw_statistics *wlGetStats(WL_NETDEV *dev);

extern int wlIoctl(WL_NETDEV *dev , struct ifreq  *rq, int cmd);
extern int wlSetupWEHdlr(WL_NETDEV *netdev);

#endif /* AP8X_API_H_ */





