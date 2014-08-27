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


#ifndef _BUILD_MODES_H_
#define _BUILD_MODES_H_

#define OEMSSID "MarvellAP%1x%1x" /* default SSID */
#define OEM_UR_SSID "Marvell_AP" /* default SSID of AP to connect UR */
#define OEMCHANNEL 6/** default channel **/
#define MAX_PMAC 2
#define MAX_STNS  64 /** Maximum allow associated station **/
#define MAX_AID (MAX_STNS*MAX_PMAC)

#define ENABLE_ERP_PROTECTION 1
#define ERP 1
#define DISABLE_B_AP_CHECK
#define CONSECTXFAILLIMIT		500			//Default value. Consecutive tx fail cnt > limit to kick out client. Zero to disable.
#define _CONSECTXFAILLIMIT		500			//Use this when enabling mcastproxy with mib_consectxfaillimit value set as default

#ifdef QOS_FEATURE
//#define RX_QOSDATA_SUPPORT 
//#define ETHER_RX_QOSDATA_SUPPORT
#ifdef UAPSD
#define WMM_PS_SUPPORT
#endif
#endif

#endif /* _BUILD_MODES_H_ */
