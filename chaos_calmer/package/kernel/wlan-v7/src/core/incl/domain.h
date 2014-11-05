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


#ifndef _DOMAIN_H_
#define _DOMAIN_H_

#define DOMAIN_CODE_FCC     0x10
#define DOMAIN_CODE_IC      0x20
#define DOMAIN_CODE_ETSI    0x30
#define DOMAIN_CODE_SPAIN   0x31
#define DOMAIN_CODE_FRANCE  0x32
#define DOMAIN_CODE_MKK     0x40
#define DOMAIN_CODE_DGT    0x80
#define DOMAIN_CODE_AUS    0x81
#ifdef JAPAN_CHANNEL_SPACING_10_SUPPORT
#define DOMAIN_CODE_MKK2     0x41  /** for japan channel with spacing 10 **/
#endif
#define DOMAIN_CODE_MKK3     0x41  /** for japan channel - 5450-5725 MHz */
#define DOMAIN_CODE_ALL		0xff
int domainGetInfo(unsigned char *ChannelList/* NULL Terminate*/);
int domainChannelValid(unsigned char channel, unsigned char band);

unsigned char domainGetDomain(void);
extern int domainSetDomain(unsigned char domain);
extern int domainGetPowerInfo(unsigned char *info);
#endif /*_DOMAIN_H_*/
