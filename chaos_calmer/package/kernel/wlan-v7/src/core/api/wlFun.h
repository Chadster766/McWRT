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


#ifndef _WL_FUN_H_
#define _WL_FUN_H_ 

/* PUBLIC FUNCTIONS DECLARATION
 */

BOOLEAN WL_FUN_BssStart(void *info);
BOOLEAN WL_FUN_SetChannel(void *info, int channel);
BOOLEAN WL_FUN_SetESSID(void *info, char *essid);
BOOLEAN WL_FUN_SetRTS(void *info, int rts);
BOOLEAN WL_FUN_SetPrivacyOption(void *info, int privacy);
BOOLEAN WL_FUN_SetWEPKey(void *info, int keyindex, int keyType, char *keyValue);
BOOLEAN WL_FUN_SetAuthType(void *info, int authtype);
BOOLEAN WL_FUN_SetBand(void *info, int band);
BOOLEAN WL_FUN_SetRegionCode(void *info, int region);
BOOLEAN WL_FUN_SetHideSSID(void *info, int hidessid);
BOOLEAN WL_FUN_SetPreamble(void *info, int Preamble);
BOOLEAN WL_FUN_SetGProtect(void *info, int Gprotect);
BOOLEAN WL_FUN_SetBeacon(void *info, int beacon);
BOOLEAN WL_FUN_SetDtim(void *info, int dtim);
BOOLEAN WL_FUN_SetFixedRate(void *info, int fixedrate);
BOOLEAN WL_FUN_SetFixBRate(void *info, int rate);
BOOLEAN WL_FUN_SetFixGRate(void *info, int rate);
BOOLEAN WL_FUN_SetAntenna(void *info, int antenna);
BOOLEAN WL_FUN_SetWPAMode(void *info, int WPAWPA2mode);
#ifdef AP_WPA2
BOOLEAN WL_FUN_SetWPA2OnlyMode(void *info, int WPAWPA2mode);
BOOLEAN WL_FUN_SetWPA2MixMode(void *info, int WPAWPA2mode);
#endif
BOOLEAN WL_FUN_SetAuthSuite(void *info, int authsuite);
BOOLEAN WL_FUN_SetUnicastCipherSuite(void *info, int cipher_suit);
BOOLEAN WL_FUN_SetMulticastCipherSuite(void *info, int cipher_suit);
BOOLEAN WL_FUN_SetWPA2UnicastCipherSuite(void *info, int cipher_suit);
BOOLEAN WL_FUN_SetWPA2UnicastCipherSuite2(void *info, int cipher_suit);
BOOLEAN WL_FUN_SetWPA2MulticastCipherSuite(void *info, int cipher_suit);
BOOLEAN WL_FUN_SetWPAPassPhrase(void *info, char *passphrass);
BOOLEAN WL_FUN_SetWPA2PassPhrase(void *info, char *passphrass);
BOOLEAN WL_FUN_SetGroupReKeyTime(void *info, int grouprekeytime);
BOOLEAN WL_FUN_SetBCAConfig(void *info, int mode, ULONG wlTxPri0, ULONG wlTxPri1, ULONG wlRxPri0, ULONG wlRxPri1);
BOOLEAN WL_FUN_SetBCAConfigTimeShare(void *info, int traffic_type, ULONG timeshareInterval, ULONG btTime);
BOOLEAN WL_FUN_SetBCAConfigTimeShareEnable(void *info, int Enabled);
BOOLEAN WL_FUN_SetWMM(void *info, int wmm);
BOOLEAN WL_FUN_SetWMMAckPolicy(void *info, int wmm_ack_policy);
BOOLEAN WL_FUN_SetWMMParam(void *info, int Indx, int CWmin, int CWmax, int AIFSN, int TXOPLimitB, int TXOPLimit, int Mandatory);
BOOLEAN WL_FUN_SetFilter(void *info, int filter);
BOOLEAN WL_FUN_SetMACFilter(void *info, int no, UCHAR * mac);
BOOLEAN WL_FUN_GetWMMParam(void *info, int Indx, int *CWmin, int *CWmax, int *AIFSN, int *TXOPLimitB, int *TXOPLimit, int *Mandatory);
BOOLEAN WL_FUN_GetSTAList(void *info, char *buf);
BOOLEAN WL_FUN_GetRfReg(void *info, USHORT reg, UCHAR *value);
BOOLEAN WL_FUN_SetRfReg(void *info, USHORT reg, UCHAR value);
#endif
