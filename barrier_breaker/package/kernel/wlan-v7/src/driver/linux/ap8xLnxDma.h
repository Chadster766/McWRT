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

#ifndef AP8X_DMA_H_
#define AP8X_DMA_H_


#ifdef DMA_ENABLE
	#ifdef WL_DEBUG
		extern int mvDmaCopy(const char *func, int line,  void *dst, void *src, int byteCount);
		#define MEMCPY mvDmaCopy(__FUNCTION__,__LINE__,
	#else
		extern int mvDmaCopy(void *dst, void *src, int byteCount);
		#define MEMCPY mvDmaCopy(
	#endif
#else
	#define MEMCPY memcpy(
#endif

#endif /*AP8X_DMA_H_*/

