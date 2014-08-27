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
*   Description:  This file implemented a Timer list object 
*                 functionality
*                                          
*/


/*!
* \file    timer.c
* \brief   software timer module
*/
#include "wltypes.h"
#include "osif.h"
#include "timer.h"

#include "wldebug.h"

void TimerRemove(Timer *me);

/*!
* Initialize a timer
* @param me Pointer to the timer
*/
void TimerInit(Timer *me)
{
	if(timer_pending(me))
		return;
	init_timer(me);
	me->function = NULL;
}

/*************************************************************************
* Function:
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
static void TimerAdd(Timer *me, int act, void *callback, unsigned char *data_p, unsigned int ticks)
{
	me->function = callback;
	me->data = (UINT32) data_p;
	me->expires=jiffies + ticks * TIMER_100MS;    
	if(timer_pending(me))
		return;
	add_timer(me);
}
static void TimerByJiffiesAdd(Timer *me, int act, void *callback, unsigned char *data_p, unsigned int Jiffies)
{
	me->function = callback;
	me->data = (UINT32) data_p;
	me->expires=jiffies + Jiffies;    
	if(timer_pending(me))
		return;
	add_timer(me);
}

/*!
* Start a one shot timer 
* @param me Pointer to the timer
* @param act none-zero to active the timer
* @param callback Pointer to the callback function, which will be called if the timer expired
* @param data_p Pointer to user defined data, which is useful by user when implementing callback function
* @param ticks Timer alarm in ticks
*/
void TimerFireIn(Timer *me, int act, void *callback, unsigned char *data_p, unsigned int ticks)
{
	if (act)
	{
		/* Remove in case Timer is already added */
		TimerRemove(me);
		TimerAdd(me, act, callback, data_p, ticks);
	}
}
void TimerFireInByJiffies(Timer *me, int act, void *callback, unsigned char *data_p, unsigned int Jiffies)
{
	if (act)
	{
		/* Remove in case Timer is already added */
		TimerRemove(me);
		TimerByJiffiesAdd(me, act, callback, data_p, Jiffies);
	}
}

/*!
* Start a periodic timer 
* @param me Pointer to the timer
* @param act none-zero to active the timer
* @param callback Pointer to the callback function, which will be called if the timer expired
* @param data_p Pointer to user defined data, which is useful by user when implementing callback function
* @param ticks Timer alarm in ticks
*/
void TimerFireEvery(Timer *me, int act, void *callback, unsigned char *data_p, unsigned int ticks)
{
	/* This is not used in current code.  In order to trigger a callback periodically 
	the user will need to rearm the timer within the callback.  */
	if (act)
	{
		TimerRemove(me);
		TimerAdd(me, act, callback, data_p, ticks);
	}
}

/*!
* Stop a timer 
* @param me Pointer to the timer
*/
void TimerDisarm(Timer *me)
{
	if(me->function)
	{
		if(timer_pending(me))
		{
			del_timer(me);
			me->function = NULL;
		}
	}
}
void TimerDisarmByJiffies(Timer *me, int freedata)
{
	if(me->function)
	{
		if(timer_pending(me))
		{
			del_timer(me);
			me->function = NULL;
			if(freedata && me->data)
				free(me->data);
		}
	}
}

/*!
* Remove a timer 
* @param me Pointer to the timer
*/
void TimerRemove(Timer *me)
{
	if(me->function)
	{
		if(timer_pending(me))
		{
			del_timer(me);
			me->function = NULL;
		}
	}
}

/*!
* Restart a timer 
* @param me Pointer to the timer
* @param ticks Timer alarm in ticks
*/
void TimerRearm(Timer *me, unsigned int ticks)
{
	if (me->function)
	{
		me->expires=jiffies + ticks * TIMER_100MS;    
		if(timer_pending(me))
			return;
		add_timer(me);
	}
}

