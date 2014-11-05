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


/******************* (c) Marvell Semiconductor, Inc., 2001 ********************
 *
 *  Purpose:
 *  This file contains the function prototypes and definitions for the OS
 *  wrapper.
 *
 *  Public Procedures:
 *     ** Initialization
 *     os_OsInit             Initialize OS structures
 *
 *     ** Task management
 *     os_TaskCreate         Creates a new task
 *     os_TaskSuspend        Suspends a task
 *     os_TaskResume         Start or resume a task
 *     os_TaskKill           Kills a task
 *     os_TaskDelay          Delays the currently running task
 *
 *     ** Queues
 *     os_QueueCreate        Create a queue
 *     os_QueueDelete        Delete a queue
 *     os_QueueReadNoBlock   Read a message from a queue without blocking
 *     os_QueueWriteNoBlock  Write a message to a queue without blocking
 *     os_QueuePeek          Retrieve how many messages are in a queue
 *
 *     ** Interrupt Handling
 *     os_InterruptCreate    Create an interrupt
 *     os_InterruptAttach    Attach a hardware interrupt to an ISR
 *     os_InterruptDisable   Disable an interrupt
 *     os_InterruptEnable    Enable an Interrupt
 *     
 *     ** Synchronization
 *     os_SemaphoreInit      Initialize a semaphore
 *     os_SemaphoreDestroy   Destroy a semaphore
 *     os_SemaphoreGet       Get a semaphore
 *     os_SemaphorePut       Return a semaphore
 *
 *     ** Events
 *     os_EventInit          Initialize an event flag
 *     os_EventTrigger       Trigger an event to occur
 *     os_EventWait          Wait for an event to occur
 *     os_EventDestroy       Destroy an event flag
 *     os_EventClear         Clear events from an event flag
 *
 *     ** Timer
 *     os_TimerCreate        Create a timer
 *     os_TimerInit          Initialize a timer
 *     os_TimerDelete        Delete a timer
 *     os_TimerEnable        Enable a timer
 *     os_TimerDisable       Disable a timer
 *     
 *     ** Memory management
 *     os_Malloc             Allocates a requested amount of memory
 *     os_Free               Frees previously allocated memory
 *
 *  Private Procedures:
 *     None.
 *
 *  Notes:
 *     None.
 *****************************************************************************/

#ifndef __OS_IF_H__
#define __OS_IF_H__

/*============================================================================= */
/*                               INCLUDE FILES */
/*============================================================================= */


#include "wltypes.h"

#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/errno.h>
#include <linux/sched.h>
#if LINUX_VERSION_CODE >=KERNEL_VERSION(3,7,0)
#include <linux/kthread.h>
#endif
#define os_START  

/** Events **/
/* Events **/
#define os_EVENT_WAITMODE_OR       0x01
#define os_EVENT_WAITMODE_AND      0x02 
#define os_EVENT_WAITMODE_CLR      0x04

#define os_EVENT_WAITMODE_CLR_AND  (os_EVENT_WAITMODE_AND | os_EVENT_WAITMODE_CLR)
#define os_EVENT_WAITMODE_CLR_OR   (os_EVENT_WAITMODE_OR | os_EVENT_WAITMODE_CLR)

#define OS_EVENT_WAITMODE_CLR_OR   os_EVENT_WAITMODE_CLR_OR

#define os_EnterCriticalSection 
#define os_ExitCriticalSection 
#define __SQRAM__
/*============================================================================= */
/*                          PUBLIC TYPE DEFINITIONS */
/*============================================================================= */
typedef UINT32             os_Vector_t;
typedef void               os_ISR_t(void);
typedef void               os_DSR_t(void);
typedef UINT32             os_Id_t;
typedef int				   os_TickCount_t;
typedef void               os_Timer_t( os_Id_t Handle, UINT32 Data );
typedef UINT32             os_Ucount32;
typedef UINT32             os_Int32;
typedef UINT32		       os_Addrword_t;
typedef int os_TaskEntry_t(void * arg); 
typedef int                os_Priority_t;
typedef UINT32             os_IndicatedEvents_t;
typedef int                os_EventMode_t;
typedef void os_Alarm_t(UINT32 alarm, UINT32 data);

typedef UINT32              OS_DATA;
typedef UINT32              OS_EVENTS;

/*============================================================================= */
/*                                PUBLIC DATA */
/*============================================================================= */
/** Status **/
#define os_SUCCESS             0
#define os_FAIL                1
#define os_ERR_NO_HANDLE       0x00000099
#define os_ERR_ILLEGAL_ID      0x00000098
#define os_ERR_HANDLE_IN_USE   0x00000097

#define OS_SUCCESS             0
#define OS_FAIL                1

#ifdef OS_ECOS
#define ENTER_CRITICAL  os_EnterCriticalSection
#define EXIT_CRITICAL   os_ExitCriticalSection
#else
#define ENTER_CRITICAL  os_EnterCriticalSection
#define EXIT_CRITICAL   os_ExitCriticalSection
#endif



/*============================================================================= */
/*                    PUBLIC PROCEDURES (ANSI Prototypes) */
/*============================================================================= */

/******************************************************************************
 *
 * Name: os_OsInit
 *
 * Description:
 *   This routine is called to initialize OS structures.
 *
 * Conditions For Use:
 *   None.
 *
 * Arguments:
 *   None.
 *
 * Return Value:
 *   None.
 *
 * Notes:
 *   None.
 *
 *****************************************************************************/
extern WL_STATUS os_OsInit( void );


/******************************************************************************
 *
 * Name: os_TaskCreate
 *
 * Description:
 *   This routine is called to create a task.
 *
 * Conditions For Use:
 *   The OS initialization routine has already been executed.
 *
 * Arguments:
 *   Arg1 (i  ): TaskId    - Id (index) of the task to be created
 *   Arg2 (i  ): Entry     - Pointer to the routine executed as a task
 *   Arg3 (i  ): Data      - Data supplied to the task routine
 *   Arg4 (i  ): TaskName  - String name of the task
 *   Arg5 (i  ): StackSize - Stack size used by the task
 *   Arg6 (i  ): Priority  - Priority of the task
 *
 * Return Value:
 *   Status indicating success or failure
 *
 * Notes:
 *   None.
 *
 *****************************************************************************/
extern WL_STATUS os_TaskCreate( os_Id_t         *TaskId,
                               os_TaskEntry_t *Entry_p,
                               os_Addrword_t   Data,
                               char           *TaskName_p,
                               os_Ucount32     StackSize,
                               os_Addrword_t   Priority );


/******************************************************************************
 *
 * Name: os_TaskSuspend
 *
 * Description:
 *   This routine is called to suspend a task.
 *
 * Conditions For Use:
 *   The task to be suspended has been created.
 *
 * Arguments:
 *   Arg1 (i  ): TaskId - Id (index) of the task to be suspended
 *
 * Return Value:
 *   None.
 *
 * Notes:
 *   None.
 *
 *****************************************************************************/
extern void os_TaskSuspend( os_Id_t TaskId );


/******************************************************************************
 *
 * Name: os_TaskResume
 *
 * Description:
 *   This routine is called to resume a task.
 *
 * Conditions For Use:
 *   The task to be resumed has been created.
 *
 * Arguments:
 *   Arg1 (i  ): TaskId - Id (index) of the task to be resumed
 *
 * Return Value:
 *   None.
 *
 * Notes:
 *   None.
 *
 *****************************************************************************/
extern void os_TaskResume( os_Id_t TaskId );


/******************************************************************************
 *
 * Name: os_TaskKill
 *
 * Description:
 *   This routine is called to kill a given task.
 *
 * Conditions For Use:
 *   The task to be killed has been created.
 *
 * Arguments:
 *   Arg1 (i  ): TaskId - Id (index) of the task to be killed
 *
 * Return Value:
 *   None.
 *
 * Notes:
 *   None.
 *
 *****************************************************************************/
extern void os_TaskKill( os_Id_t TaskId );


/******************************************************************************
 *
 * Name: os_TaskDelay
 *
 * Description:
 *   This routine is called to put the currently running task in sleep mode
 *   for given time.
 *
 * Conditions For Use:
 *   None.
 *
 * Arguments:
 *   Arg1 (i  ): Count - The number of ticks to dely where 1 tick = 100 us
 *
 * Return Value:
 *   None.
 *
 * Notes:
 *   None.
 *
 * PDL:
 *    Call cyg_thread_delay() to delay the currently task
 * END PDL
 *
 *****************************************************************************/
extern void os_TaskDelay( os_TickCount_t Count );


/******************************************************************************
 *
 * Name: os_QueueCreate
 *
 * Description:
 *   This routine is called to create a message queue for a task.
 *
 * Conditions For Use:
  *   The OS initialization routine has already been executed.
 *
 * Arguments:
 *   Arg1 (i  ): QueueId - Id (index) of the queue to be created
 *
 * Return Value:
 *   Status indicating success or failure
 *
 * Notes:
 *   None.
 *
 *****************************************************************************/
extern UINT8 os_QueueCreate( os_Id_t QueueId );


/******************************************************************************
 *
 * Name: os_QueueDelete
 *
 * Description:
 *   This routine is called to delete a given queue.
 *
 * Conditions For Use:
 *   The queue to be deleted has been created.
 *
 * Arguments:
 *   Arg1 (i  ): QueueId - Id (index) of the queue to be deleted
 *
 * Return Value:
 *   None.
 *
 * Notes:
 *   None.
 *
 *****************************************************************************/
extern void os_QueueDelete( os_Id_t QueueId );


/******************************************************************************
 *
 * Name: os_QueueReadNoBlock
 *
 * Description:
 *   This routine is called to read a message from a queue; if there are no
 *   messages on the queue it returns immediately (without blocking).
 *
 * Conditions For Use:
 *   The queue to read from has been created.
 *
 * Arguments:
 *   Arg1 (i  ): QueueId - Id (index) of the queue to read a message from
 *
 * Return Value:
 *   None.
 *
 * Notes:
 *   None.
 *
 *****************************************************************************/
extern void* os_QueueReadNoBlock( os_Id_t QueueId );


/******************************************************************************
 *
 * Name: os_QueueWriteNoBlock
 *
 * Description:
 *   This routine writes a message to a queue; if the message cannot be
 *   written, the routine returns immediately (without blocking).
 *
 * Conditions For Use:
 *   The queue to write to has been created.
 *
 * Arguments:
 *   Arg1 (i  ): QueueId - Id (index) of the queue to write a message to
 *   Arg2 (i  ): Item    - Pointer to the message to be written to the queue
 *
 * Return Value:
 *   Status indicating success or failure
 *
 * Notes:
 *   None.
 *
 *****************************************************************************/
extern WL_STATUS os_QueueWriteNoBlock( os_Id_t QueueId, void *Item_p );


/******************************************************************************
 *
 * Name: os_QueuePeek
 *
 * Description:
 *   This routine is called to find out how many messages are in a queue.
 *
 * Conditions For Use:
 *   The queue to peed at has been created.
 *
 * Arguments:
 *   Arg1 (i  ): QueueId - Id (index) of the queue to peek at
 *
 * Return Value:
 *   The number of messages
 *
 * Notes:
 *   None.
 *
 * PDL:
 *    If the queue Id is valid Then
 *       Call cyg_mbox_peek() to get the number of messages in the queue
 *       Return the number of message found in the queue
 *    Else
 *       Return 0
 *    End If
 * END PDL
 *
 *****************************************************************************/
extern UINT32 os_QueuePeek( os_Id_t QueueId );


/******************************************************************************
 *
 * Name: os_InterruptCreate
 *
 * Description:
 *   This routine is called to create a interrupt object.
 *
 * Conditions For Use:
 *   The OS initialization routine has already been executed.
 *
 * Arguments:
 *   Arg1 (i  ): InterruptId - Id (index) of the interrupt to be created
 *   Arg2 (i  ): Vector      - Vector associated with the interrupt to be
 *                             created
 *   Arg3 (i  ): Priority    - Priority of the interrupt to be created
 *   Arg4 (i  ): *Isr_p      - Pointer to the interrupt service routine
 *                             handler
 *   Arg5 (i  ): *Dsr_p      - Pointer to the deferred service routine
 *                             handler
 *
 * Return Value:
 *   Status indicating success or failure
 *
 * Notes:
 *   None.
 *
 *****************************************************************************/
extern WL_STATUS os_InterruptCreate( os_Id_t        InterruptId,
                                    os_Vector_t    Vector,
                                    os_Priority_t  Priority,
                                    os_ISR_t      *Isr_p,
                                    os_DSR_t      *Dsr_p );


/******************************************************************************
 *
 * Name: os_InterruptAttach
 *
 * Description:
 *   This routine is called to attach an interrupt to the vector specified
 *   in the create routine. This allows for hardware interrupts to be
 *   triggered according to the vector and processed by the ISR/DSR given
 *   in the create routine.
 *
 * Conditions For Use:
 *   The interrupt object has been created.
 *
 * Arguments:
 *   Arg1 (i  ): InterruptId - Id (index) of the interrupt to be attached
 *
 * Return Value:
 *   None.
 *
 * Notes:
 *   None.
 *
 *****************************************************************************/
extern void os_InterruptAttach( os_Id_t InterruptId );


/******************************************************************************
 *
 * Name: os_InterruptDisable
 *
 * Description:
 *   This routine is called to disable all ISRs.
 *
 * Conditions For Use:
 *   None.
 *
 * Arguments:
 *   None.
 *
 * Return Value:
 *   None.
 *
 * Notes:
 *   None.
 *
 *****************************************************************************/
extern void os_InterruptDisable( void );


/******************************************************************************
 *
 * Name: os_InterruptEnable
 *
 * Description:
 *   This routine is called to enable all ISRs
 *
 * Conditions For Use:
 *   None.
 *
 * Arguments:
 *   None.
 *
 * Return Value:
 *   None.
 *
 * Notes:
 *   None.
 *
 *****************************************************************************/
extern void os_InterruptEnable( void );


/******************************************************************************
 *
 * Name: os_InterruptMask
 *
 * Description:
 *   This routine is called to mask (disable) given interrupts specified by
 *   the supplied vector.
 *
 * Conditions For Use:
 *   The interrupt object has been created.
 *
 * Arguments:
 *   Arg1 (i  ): Vector - Value whose bits indicate which interrupts to
 *                        mask
 *
 * Return Value:
 *   None.
 *
 * Notes:
 *   None.
 *
 *****************************************************************************/
extern void os_InterruptMask( os_Vector_t Vector );


/******************************************************************************
 *
 * Name: os_InterruptUnmask
 *
 * Description:
 *   This routine is called to unmask (enable) given interrupts specified by
 *   the supplied vector.
 *
 * Conditions For Use:
 *   The interrupt object has been created.
 *
 * Arguments:
 *   Arg1 (i  ): Vector - Value whose bits indicate which interrupts to
 *                        unmask
 *
 * Return Value:
 *   None.
 *
 * Notes:
 *   None.
 *
 *****************************************************************************/
extern void os_InterruptUnmask( os_Vector_t Vector );


/******************************************************************************
 *
 * Name: os_InterruptAck
 *
 * Description:
 *   This routine is used to acknowledge receipt of an interrupt; it is
 *   called from within an ISR.
 *
 * Conditions For Use:
 *   None.
 *
 * Arguments:
 *   Arg1 (i  ): Vector - Value indicating the interrupt to acknowledge
 *
 * Return Value:
 *   None.
 *
 * Notes:
 *   This should be called from within an ISR. Interrupts must be
 *   acknowledged using this routine to prevent the interrupt from
 *   reoccurring.
 *
 * PDL:
 *    Call cyg_interrupt_acknowledge() to acknowledge the interrupt
 * END PDL
 *
 *****************************************************************************/
extern void os_InterruptAck( os_Vector_t Vector );


/******************************************************************************
 *
 * Name: os_SemaphoreInit
 *
 * Description:
 *   This routine is called to initialize a semaphore.
 *
 * Conditions For Use:
 *   None.
 *
 * Arguments:
 *   Arg1 (i  ): SemId - Id (index) of the semaphore to be initialized
 *   Arg2 (i  ): Count - The count the semaphore is initialized to
 *
 * Return Value:
 *   None.
 *
 * Notes:
 *   None.
 *
 *****************************************************************************/
#define  os_SemaphoreInit(x, y)


/******************************************************************************
 *
 * Name: os_SemaphoreDestroy
 *
 * Description:
 *   This routine is called to destroy a semaphore.
 *
 * Conditions For Use:
 *   The semaphore has been initialized.
 *
 * Arguments:
 *   Arg1 (i  ): SemId - Id (index) of the semaphore to be destroyed
 *
 * Return Value:
 *   None.
 *
 * Notes:
 *   This operation must not be performed while there are any threads
 *   waiting on the semaphore.
 *
 *****************************************************************************/
extern void os_SemaphoreDestroy( os_Id_t SemId );


/******************************************************************************
 *
 * Name: os_SemaphoreGet
 *
 * Description:
 *   This routine is called to claim a semaphore; if one is not available,
 *   then the calling task waits until the semaphore is available.
 *
 * Conditions For Use:
 *   The semaphore has been initialized.
 *
 * Arguments:
 *   Arg1 (i  ): SemId - Id (index) of the semaphore to be claimed
 *
 * Return Value:
 *   None.
 *
 * Notes:
 *   When a semaphore gets claimed by a task, the semaphore count is
 *   decremented; when the count goes to zero, any tasks wishing to claim
 *   a semaphore must wait on the semaphore.
 *
 *****************************************************************************/
#define  os_SemaphoreGet(x)


/******************************************************************************
 *
 * Name: os_SemaphorePut
 *
 * Description:
 *   This routine is called to return a semaphore.
 *
 * Conditions For Use:
 *   The semaphore has been initialized.
 *
 * Arguments:
 *   None.
 *
 * Return Value:
 *   None.
 *
 * Notes:
 *   By returning a semaphore, a waiting task is woken to claim the
 *   semaphore; if there are no waiting tasks, the semaphore count is
 *   incremented.
 *
 *****************************************************************************/
#define  os_SemaphorePut(x)


/******************************************************************************
 *
 * Name: os_EventInit
 *
 * Description:
 *   This routine is called to initialize an event flag.
 *
 * Conditions For Use:
 *   None.
 *
 * Arguments:
 *   Arg1 (i  ): EventId - Id (index) of the event to be initialized
 *
 * Return Value:
 *   None.
 *
 * Notes:
 *   None.
 *
 *****************************************************************************/
extern void os_EventInit( os_Id_t EventId );


/******************************************************************************
 *
 * Name: os_EventTrigger
 *
 * Description:
 *   This routine is called to trigger an event(s) for a given event flag.
 *
 * Conditions For Use:
 *   The event has been initialized.
 *
 * Arguments:
 *   Arg1 (i  ): EventId         - Id (index) of the event flag for which an
 *                                 event(s) is to be triggered
 *   Arg2 (i  ): EventsToTrigger - The event(s) to be triggered
 *
 * Return Value:
 *   None.
 *
 * Notes:
 *   None.
 *
 *****************************************************************************/
extern void os_EventTrigger( os_Id_t              EventId,
                             os_IndicatedEvents_t EventsToTrigger );


/******************************************************************************
 *
 * Name: os_EventWait
 *
 * Description:
 *   This routine is called to wait on a given event(s) for a given event
 *   flag.
 *
 * Conditions For Use:
 *   The event has been initialized.
 *
 * Arguments:
 *   Arg1 (i  ): EventId         - Id (index) of the event flag for which an
 *                                 event(s) is to be waited on
 *   Arg2 (i  ): EventsToWaitFor - The event(s) to be waited on
 *   Arg3 (i  ): Mode            - Mode specifying the conditions for waking
 *                                 up on the event(s) waited for
 *
 * Return Value:
 *   None.
 *
 * Notes:
 *   None.
 *
 *****************************************************************************/
extern os_IndicatedEvents_t os_EventWait(
                                        os_Id_t              EventId,
                                        os_IndicatedEvents_t EventsToWaitFor,
                                        os_EventMode_t       Mode );


/******************************************************************************
 *
 * Name: os_EventDestroy
 *
 * Description:
 *   This routine is called to destroy an event.
 *
 * Conditions For Use:
 *   The event has been initialized.
 *
 * Arguments:
 *   Arg1 (i  ): EventId - Id (index) of the event flag that is to be
 *                         destroyed
 *
 * Return Value:
 *   None.
 *
 * Notes:
 *   None.
 *
 *****************************************************************************/
extern void os_EventDestroy( os_Id_t EventId );

/******************************************************************************
 *
 * Name: os_EventPeek
 *
 * Description:
 *   This routine is called to peek for a given event flag
 *
 * Conditions For Use:
 *   The event has been initialized.
 *
 * Arguments:
 *   Arg1 (i  ): EventId         - Id (index) of the event flag for which an
 *                                 event(s) is to be waited on
 *
 * Return Value:
 *   events.
 *
 * Notes:
 *   None.
 *
 *****************************************************************************/
extern os_IndicatedEvents_t os_EventPeek(os_Id_t EventId );

/******************************************************************************
 *
 * Name: os_EventClear
 *
 * Description:
 *   This routine is called to clear events in an event flag.
 *
 * Conditions For Use:
 *   The event has been initialized.
 *
 * Arguments:
 *   Arg1 (i  ): EventId       - Id (index) of the event flag for which an
 *                               event(s) is to be cleared
 *   Arg2 (i  ): EventsToClear - The event(s) to be cleared
 *
 * Return Value:
 *   None.
 *
 * Notes:
 *   None.
 *
 *****************************************************************************/
extern void os_EventClear( os_Id_t               EventId,
                           os_IndicatedEvents_t  EventsToClear );


/******************************************************************************
 *
 * Name: os_TimerCreate
 *
 * Description:
 *   This routine is called to create a timer.
 *
 * Conditions For Use:
 *   The OS initialization routine has already been executed.
 *
 * Arguments:
 *   Arg1 (i  ): TimerId   - Id (index) of the timer to be created
 *   Arg2 (i  ): TimerFunc - Pointer to the routine called when a timer
 *                           expires
 *   Arg3 (i  ): Data      - Data supplied to the timer function
 *
 * Return Value:
 *   Status indicating success or failure
 *
 * Notes:
 *   None.
 *
 *****************************************************************************/
extern WL_STATUS os_TimerCreate( os_Id_t        TimerId,
                                os_Alarm_t    *TimerFunc_p,
                                os_Addrword_t  Data );


/******************************************************************************
 *
 * Name: os_TimerInit
 *
 * Description:
 *   This routine is called to initialize a timer.
 *
 * Conditions For Use:
 *   The timer has been created.
 *
 * Arguments:
 *   Arg1 (i  ): TimerId  - Id (index) of the timer to be initialized
 *   Arg2 (i  ): Trigger  - The tick at which the timer is to trigger
 *   Arg3 (i  ): Interval - Number of ticks before the timer triggers again
 *                          (if 0, the timer is disabled after the initial
 *                          trigger).
 *
 * Return Value:
 *   None.
 *
 * Notes:
 *   None.
 *
 *****************************************************************************/
extern void os_TimerInit( os_Id_t         TimerId,
                          os_TickCount_t  Trigger,
                          os_TickCount_t  Interval );


/******************************************************************************
 *
 * Name: os_TimerDelete
 *
 * Description:
 *   This routine is called to delete a timer.
 *
 * Conditions For Use:
 *   The timer has been created.
 *
 * Arguments:
 *   Arg1 (i  ): TimerId - Id (index) of the timer to be deleted
 *
 * Return Value:
 *   None.
 *
 * Notes:
 *   None.
 *
 *****************************************************************************/
extern void os_TimerDelete( os_Id_t TimerId );


/******************************************************************************
 *
 * Name: os_TimerEnable
 *
 * Description:
 *   This routine is called to enable a timer.
 *
 * Conditions For Use:
 *   The timer has been created.
 *
 * Arguments:
 *   Arg1 (i  ): TimerId - Id (index) of the timer to be enabled
 *
 * Return Value:
 *   None.
 *
 * Notes:
 *   None.
 *
 *****************************************************************************/
extern void os_TimerEnable( os_Id_t TimerId );


/******************************************************************************
 *
 * Name: os_TimerDisable
 *
 * Description:
 *   This routine is called to disable a timer.
 *
 * Conditions For Use:
 *   The timer has been created.
 *
 * Arguments:
 *   Arg1 (i  ): TimerId - Id (index) of the timer to be disabled
 *
 * Return Value:
 *   None.
 *
 * Notes:
 *   None.
 *
 *****************************************************************************/
extern void os_TimerDisable( os_Id_t TimerId );



/******************************************************************************
 *
 * Name: os_CurrentTime
 *
 * Description:
 *   Return current time.
 *
 * Conditions For Use:
 *   The OS initialization routine has already been executed.
 *
 * Arguments:
 *  None
 *
 *
 * Return Value:
 *    Current Time 
 *
 *****************************************************************************/
extern UINT32 os_CurrentTime(void);

/******************************************************************************
 *
 * Name: os_Malloc
 *
 * Description:
 *   This routine is called to allocate memory.
 *
 * Conditions For Use:
 *   The OS initialization routine has already been executed.
 *
 * Arguments:
 *   Arg1 (i  ): Size - size of the memory to allocate
 *
 * Return Value:
 *   Pointer to the allocated memory
 *
 * Notes:
 *   None.
 *
 *****************************************************************************/
typedef void *MHANDLE;
extern void os_var_pool_create( void *, UINT32, MHANDLE *, void * );
extern void *os_var_alloc( MHANDLE handle, UINT32 size );
extern void os_var_free( MHANDLE handle, void *buf_p );
extern void os_fix_pool_create( void *, UINT32, UINT32, MHANDLE *, void * );
extern void *os_fix_alloc( MHANDLE handle );
extern void os_fix_free( MHANDLE handle, void *buf_p );

#endif /* __OS_IF_H__ */
