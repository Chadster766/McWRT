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

/** include files **/
#include "wldebug.h"
#include "apintf.h"
#include "ap8xLnxIntf.h"
#include "ap8xLnxWlLog.h"
#include <stdarg.h>

void wlsyslog(struct net_device *netdev,UINT32 classlevel, const char *format, ... )
{
	unsigned char debugString[1020] = "";		//Reduced from 1024 to 1020 to prevent frame size > 1024bytes warning during compilation
	UINT32 class = classlevel & 0xffff0000;

	va_list a_start;

	if (format != NULL)
	{
		va_start(a_start, format);
		vsprintf(debugString, format, a_start);
		va_end(a_start);
	}

    /* Todo: Prefix log with component later */
	switch (class)
	{
	default:
		printk(KERN_INFO "WLAN(%s): ", netdev->name);
		break;
	}

	if (strlen(debugString) > 0)
	{
		if (debugString[strlen(debugString)-1] == '\n')
			debugString[strlen(debugString)-1] = '\0';
		printk("%s\n", debugString);
	}
}
