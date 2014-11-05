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



#include "wltypes.h"
#if 1
extern UINT8 util_CopyList(UINT8 *Dest_p, UINT8 *Src_p, UINT32 MaxSize)
{
   UINT32  i = 0;

   while (i < MaxSize  &&  Src_p[i] != '\0')
   {
      Dest_p[i] = Src_p[i];
      i++;
   }

   if (i < MaxSize)
   {
      Dest_p[i] = '\0';
   }
   return (i);
} // End util_CopyList()
#endif

extern UINT8 util_ListLen(UINT8 *List_p, UINT32 MaxSize)
{
   UINT32  i = 0;

   while (i < MaxSize  &&  (*List_p) != 0)
   {
      i++;
      if (i < MaxSize)
      {
         List_p++;
      }
   }
   return i;
} // End util_ListLen()

UINT8 util_GetIndexByRate(UINT8 rate)
{
    switch (rate)
    {
    case 2:
        return 0;
    case 4:
        return 1;
    case 11:
        return 2;
    case 22:
        return 3;
    case 44:
        return 4;
    case 12:
        return 5;
    case 18:
        return 6;
    case 24:
        return 7;
    case 36:
        return 8;
    case 48:
        return 9;
    case 72:
        return 10;
    case 96:
        return 11;
    case 108:
        return 12;
    case 144:
        return 13;
    default:
        return 12;
    }
}

UINT8 util_GetRateByIndex(UINT8 index)
{
    switch (index)
    {
    case 0:
        return 2;
    case 1:
    	return 4;
    case 2:
    	return 11;
    case 3:
    	return 22;
    case 4:
    	return 44;
    case 5:
    	return 12;
    case 6:
    	return 18;
    case 7:
    	return 24;
    case 8:
    	return 36;
    case 9:
    	return 48;
    case 10:
    	return 72;
    case 11:
    	return 96;
    case 12:
    	return 108;
    case 13:
    	return 144;
    default:
    	return 108;
    }        
}


