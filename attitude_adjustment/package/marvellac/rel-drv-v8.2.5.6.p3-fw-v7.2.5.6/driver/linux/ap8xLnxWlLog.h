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

#ifndef WLSYSLOG_H
#define WLSYSLOG_H

#include "wltypes.h"
#include "logmsg.h"

extern void wlsyslog(WL_NETDEV *netdev,UINT32 classlevel, const char *format, ... );

#define WLSYSLOG(netdev, classlevel, ... ) wlsyslog(netdev, classlevel, __VA_ARGS__)

#endif /* WLSYSLOG_H */
