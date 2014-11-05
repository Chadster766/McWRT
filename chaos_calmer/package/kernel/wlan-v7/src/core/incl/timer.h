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


/*
*
*   Description:  This file defined a Timer list object 
*
*/
#ifndef timer_h
#define timer_h

#define CALL_BACK 0
#define NO_CALL_BACK 1

#define TIMER_100MS     HZ/10
#define TIMER_10MS      HZ/100
#define TIMER_1MS       HZ/1000

typedef struct timer_list Timer;
void TimerFireIn(Timer *me, int act, void *callback, unsigned char *data_p, unsigned int ticks);
void TimerFireInByJiffies(Timer *me, int act, void *callback, unsigned char *data_p, unsigned int Jiffies);
void TimerFireEvery(Timer *me, int act, void *callback, unsigned char *data_p, unsigned int ticks);
void TimerDisarm(Timer *me);
void TimerDisarmByJiffies(Timer *me, int freemallocdata);
void TimerRearm(Timer *me, unsigned int ticks);
void TimerInit(Timer *me);
void TimerRemove(Timer *me);


#endif
