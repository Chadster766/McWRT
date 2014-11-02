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


#ifndef AP8X_VER_H_
#define AP8X_VER_H_

#ifdef WL_KERNEL_26
    #define OS_SUFFIX
#else
    #define OS_SUFFIX "-L24"
#endif

#ifdef SOC_W8363
    #ifdef WLMEM_DISABLED
        #define SOC_SUFFIX "-S"
    #else
        #define SOC_SUFFIX "-x"
    #endif
    #if NUMOFAPS != 1
        #ifdef V4FW
            #define FEATURE_SUFFIX "-MBSS-V4FW"
        #else
            #define FEATURE_SUFFIX "-MBSS"
        #endif
    #else
        #ifdef V4FW
            #define FEATURE_SUFFIX "-V4FW"
        #else
            #define FEATURE_SUFFIX
        #endif
    #endif
#elif defined(SOC_W8366)
    #define FEATURE_SUFFIX
   	#define SOC_SUFFIX	"-W8366"
#elif defined(SOC_W8364)
	#define FEATURE_SUFFIX
   	#define SOC_SUFFIX	"-W8364"
#elif defined(SOC_W8864)
	#define FEATURE_SUFFIX
   	#define SOC_SUFFIX	"-W8864"
#elif defined(SOC_W8764)
	#define FEATURE_SUFFIX
   	#define SOC_SUFFIX	"-W8764"
#else
   	#define SOC_SUFFIX  "-W8363"
#endif

#ifdef AP82S
    #define PLATFORM_SUFFIX "(AP82S)"
#else
    #define PLATFORM_SUFFIX
#endif

#ifdef NO_FW_DOWNLOAD
    #define FEATURE_SUFFIX "-NOFWDL"
#endif

#ifdef WL_KERNEL_26 /* %1d needed for L26; but not ok for L24 */
    #define DRV_NAME      "wdev%1d"
#else
    #define DRV_NAME      "wdev%d"
#endif

#define DRV_NAME_WDS  "%swds%1d"
#define DRV_NAME_VMAC "wdev%1dap%1d"
#define DRV_NAME_CLIENT "wdev%1dsta%1d"

#define DRV_VERSION   "7.2.5.4" SOC_SUFFIX OS_SUFFIX PLATFORM_SUFFIX FEATURE_SUFFIX


#endif /* AP8X_VER_H_ */

