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

/*
*
*   Description:  This file implements timer related functions.
*
*/


/*!
* \file    timer.c
* \brief   software timer module
*/

#include <linux/timer.h>
#include <linux/slab.h>
#include <linux/bug.h>

#include "timer.h"


/* CONSTANTS AND MACROS
*/

#define SYSTEM_MAXIMUM_TIMER  1024  /* should move to header file for system adaptation */

#define TIMER_SIGNATURE  0xdeadbeef

#define CHECK_CONVERT_CTRL(ctrl, me) \
	do \
	{ \
		BUG_ON(!me); \
		ctrl = (TIMER_CTRL_T *) me; \
		BUG_ON(ctrl->signature != TIMER_SIGNATURE); \
	} while (0)


#define TIMER_MS_TO_JIFFIES(ms)  ((ms * HZ) / 1000)


/* TYPE DEFINITION
*/

typedef struct timer_ctrl_s
{
	unsigned long signature;
	/* signaure used to validate this timer control block
	*/
	struct timer_list tm;
	/* kernel timer structure
	*/
	void (*callback)(unsigned char *);
	/* call back function
	*/
	unsigned char *data_p;
	/* call back parameter
	*/
	unsigned int ms;
	/* interval for this timer
	*/
	int periodic;
	/* indicate if this is a periodic timer
	*/
	int enable;
	/* indicate if this timer is enable
	*/
} TIMER_CTRL_T;


/* PRIVATE FUNCTION DECLARATION
*/

static void TimerAdd(struct timer_list *tp, void *callback, unsigned long data, unsigned int ms);
static void TimerDel(struct timer_list *tp);
static void TimerWrapFun(unsigned long data);


/* PRIVATE VARIABLES
*/

static int usedtimer = -1;
static TIMER_CTRL_T *timerslot[SYSTEM_MAXIMUM_TIMER];


/* PUBLIC FUNCTION DEFINITION
*/

Timer TimerInit(void)
{
	TIMER_CTRL_T *tmctl;
	int idx;

	/* if this is the first time to call this function, do initialization here
	*/
	if (usedtimer == -1) {

		for (idx = 0; idx < SYSTEM_MAXIMUM_TIMER; idx++)
			timerslot[idx] = NULL;

		usedtimer = 0;
	}

	/* do we still have room for timer
	*/
	if (usedtimer >= SYSTEM_MAXIMUM_TIMER)
		return NULL;

	tmctl = (TIMER_CTRL_T *) kmalloc(sizeof(TIMER_CTRL_T), GFP_ATOMIC);

	if (tmctl == NULL)
		return NULL;

	memset(tmctl, 0, sizeof(TIMER_CTRL_T));

	init_timer(&tmctl->tm);

	tmctl->signature = TIMER_SIGNATURE;

	/* keep this timer to timer slot
	*/
	for (idx = 0; idx < SYSTEM_MAXIMUM_TIMER; idx++) {

		if (timerslot[idx] == NULL)
			break;
	}

	BUG_ON(idx >= SYSTEM_MAXIMUM_TIMER);

	if (idx < SYSTEM_MAXIMUM_TIMER) {

		timerslot[idx] = tmctl;
		usedtimer++;

	} else {

		kfree(tmctl);
		tmctl = NULL;
	}

	return tmctl;
}


void TimerDestroy(Timer me)
{
	TIMER_CTRL_T *tmctl;
	int idx;

	CHECK_CONVERT_CTRL(tmctl, me);

	TimerDel(&tmctl->tm);

	/* remove this timer from timer slot
	*/
	for (idx = 0; idx < SYSTEM_MAXIMUM_TIMER; idx++) {

		if (timerslot[idx] == tmctl) {

			timerslot[idx] = NULL;
			break;
		}
	}

	BUG_ON(idx >= SYSTEM_MAXIMUM_TIMER);

	kfree(tmctl);

	BUG_ON(usedtimer <= 0);

	if (usedtimer > 0)
		usedtimer--;
}


void TimerDestroyAll(void)
{
	int idx;

	for (idx = 0; idx < SYSTEM_MAXIMUM_TIMER; idx++) {

		if (timerslot[idx] != NULL) {

			TimerDel(&timerslot[idx]->tm);
			kfree(timerslot[idx]);
			timerslot[idx] = NULL;
		}
	}

	usedtimer = 0;
}


void TimerFireIn(Timer me, int act, void *callback, unsigned char *data_p, unsigned int ms)
{
	TIMER_CTRL_T *tmctl;

	CHECK_CONVERT_CTRL(tmctl, me);

	tmctl->callback = callback;
	tmctl->data_p = data_p;
	tmctl->ms = ms;
	tmctl->periodic = 0;
	tmctl->enable = act;

	if (act)
		TimerAdd(&tmctl->tm, TimerWrapFun, (unsigned long) tmctl, ms);
}


void TimerFireEvery(Timer me, int act, void *callback, unsigned char *data_p, unsigned int ms)
{
	TIMER_CTRL_T *tmctl;

	CHECK_CONVERT_CTRL(tmctl, me);

	tmctl->callback = callback;
	tmctl->data_p = data_p;
	tmctl->ms = ms;
	tmctl->periodic = 1;
	tmctl->enable = act;

	if (act)
		TimerAdd(&tmctl->tm, TimerWrapFun, (unsigned long) tmctl, ms);
}


void TimerRemove(Timer me)
{
	TIMER_CTRL_T *tmctl;

	CHECK_CONVERT_CTRL(tmctl, me);

	if (tmctl->enable) {

		tmctl->enable = 0;
		TimerDel(&tmctl->tm);
	}
}


void TimerDisarm(Timer me)
{
	TIMER_CTRL_T *tmctl;

	CHECK_CONVERT_CTRL(tmctl, me);

	if (tmctl->enable) {

		tmctl->enable = 0;
		TimerDel(&tmctl->tm);
	}
}


void TimerDisarmFreeData(Timer me)
{
	TIMER_CTRL_T *tmctl;

	CHECK_CONVERT_CTRL(tmctl, me);

	if (tmctl->enable) {

		tmctl->enable = 0;
		TimerDel(&tmctl->tm);
	}

	/* not a good way
	*/

	if (tmctl->data_p != NULL) {

		kfree(tmctl->data_p);
		tmctl->data_p = NULL;
	}
}


void TimerRearm(Timer me, unsigned int ms)
{
	TIMER_CTRL_T *tmctl;

	CHECK_CONVERT_CTRL(tmctl, me);

	if (tmctl->enable)
		TimerDel(&tmctl->tm);

	tmctl->ms = ms;
	tmctl->enable = 1;

	TimerAdd(&tmctl->tm, TimerWrapFun, (unsigned long) tmctl, ms);
}


/* PRIVATE FUNCTION DEFINITION
*/

static void TimerAdd(struct timer_list *tp, void *callback, unsigned long data, unsigned int ms)
{
	BUG_ON(!(tp->base));

	TimerDel(tp);

	tp->function = callback;
	tp->data = data;
	tp->expires = jiffies + TIMER_MS_TO_JIFFIES(ms);
	add_timer(tp);
}


static void TimerDel(struct timer_list *tp)
{
	BUG_ON(!(tp->base));

	if (timer_pending(tp))
		del_timer(tp);
}


static void TimerWrapFun(unsigned long data)
{
	TIMER_CTRL_T *tmctl = (TIMER_CTRL_T *) data;

	BUG_ON(!tmctl);

	if (tmctl->enable) {

		tmctl->enable = 0;

		if (tmctl->callback != NULL)
			tmctl->callback(tmctl->data_p);
	}

	if (tmctl->periodic)
		TimerAdd(&tmctl->tm, TimerWrapFun, data, tmctl->ms);
}
