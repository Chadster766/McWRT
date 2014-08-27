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
*   Description:  This file defines timer related functions.
*
*/
#ifndef timer_h
#define timer_h

/* TYPE DEFINITION
*/

typedef void *Timer;


/* PUBLIC FUNCTION DECLARATION
*/

Timer TimerInit(void);
void TimerDestroy(Timer me);
void TimerDestroyAll(void);
void TimerFireIn(Timer me, int act, void *callback, unsigned char *data_p, unsigned int ms);
void TimerFireEvery(Timer me, int act, void *callback, unsigned char *data_p, unsigned int ms);
void TimerRemove(Timer me);
void TimerDisarm(Timer me);
void TimerDisarmFreeData(Timer me);
void TimerRearm(Timer me, unsigned int ms);

#endif
