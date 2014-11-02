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


/** include files **/
#include "wldebug.h"
#include "ap8xLnxIntf.h"
#include "ap8xLnxWlLog.h"
#include <stdarg.h>

void wlsyslog(struct net_device *netdev,UINT32 classlevel, const char *format, ... )
{
	unsigned char debugString[1020] = "";		//Reduced from 1024 to 1020 to prevent frame size > 1024bytes warning during compilation
	//UINT32 level = classlevel & 0x0000ffff;
	UINT32 class = classlevel & 0xffff0000;

	va_list a_start;

    /* Todo: Add log category later
	if ((class & WLDBG_CLASSES) != class)
	{
		return;
	}

	if ((level & WLDBG_LEVELS) != level)
	{
		if(class != DBG_CLASS_PANIC && class != DBG_CLASS_ERROR)
			return; 
	} */

	if (format != NULL)
	{
		va_start(a_start, format);
		vsprintf(debugString, format, a_start);
		va_end(a_start);
	}

    /* Todo: Prefix log with component later */
	switch (class)
	{
	/*case DBG_CLASS_ENTER:
		myprint  "Enter %s() ...\n", func);
		break;
	case DBG_CLASS_EXIT:
		myprint  "... Exit %s()\n", func);
		break;
	case DBG_CLASS_WARNING:
		myprint  "WARNING:");
		break;
	case DBG_CLASS_ERROR:
		myprint  "ERROR:");
		break;
	case DBG_CLASS_PANIC:
		myprint  "PANIC:");
		break;*/
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

