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


/******************** (c) Marvell Semiconductor, Inc., 2001 *******************
 *
 * $Header$
 *
 * Purpose:
 *    This file contains the implementations of the function prototypes given
 *    in the associated header file for the OS wrapper.
 *
 * Public Procedures:
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
 *     os_EventPeek
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
 * Private Procedures:
 *     None.
 *
 * Notes:
 *    None.
 *
 *****************************************************************************/

/*=============================================================================*/
/*                               INCLUDE FILES                                 */
/*=============================================================================*/
#include "osif.h"

#include "wldebug.h"




/*=============================================================================*/
/*                          MODULE LEVEL VARIABLES                             */
/*=============================================================================*/

/*=============================================================================*/
/*                         CODED PUBLIC PROCEDURES                             */
/*=============================================================================*/

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
 * PDL:
 *    Initialize all the handles to zero
 *    Call cyg_mempool_var_create() to create a pool of memory
 * END PDL
 *
 *****************************************************************************/
extern WL_STATUS os_OsInit( void )
{
   /*-----------------------------*/
   /* Initialize all handles to 0 */
   /*-----------------------------*/
   return OS_SUCCESS;

} /* End os_OsInit() */


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
 * PDL:
 *    If the task Id is invalid or the task has already been created Then
 *       Return the appropriate error code
 *    End If
 *    Call cyg_thread_create() to create the task
 *    If the task was created Then
 *       Return success status
 *    Else
 *       Return fail status
 *    End If
 * END PDL
 *
 *****************************************************************************/
extern WL_STATUS os_TaskCreate( os_Id_t         *TaskId,
                               os_TaskEntry_t *Entry_p,
                               os_Addrword_t   Data,
                               char           *TaskName_p,
                               os_Ucount32     StackSize,
                               os_Addrword_t   Priority )
{
   /*---------------------------*/
   /* Check for a valid task ID */
   /*---------------------------*/
    UINT32 taskId = 0;
#if LINUX_VERSION_CODE >=KERNEL_VERSION(3,7,0)
	taskId = kthread_create(Entry_p, NULL, TaskName_p);
#else
    taskId = kernel_thread(Entry_p, NULL,0);
#endif
    WLDBG_INFO(DBG_LEVEL_11, " Task kernel creation taskId = %d taskName = %s\n", taskId, TaskName_p);
	*TaskId = taskId;
    return (os_SUCCESS);
} /* End os_TaskCreate() */


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
 * PDL:
 *    If the task Id is valid Then
 *       Call cyg_thread_suspend() to suspend the task
 *    End If
 * END PDL
 *
 *****************************************************************************/
extern void os_TaskSuspend( os_Id_t TaskId )
{

} /* End os_TaskSuspend() */


/******************************************************************************
 *
 * Name: os_TaskResume
 *
 * Description:
 *   This routine is called to resume a task
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
 * PDL:
 *    If the task Id is valid Then
 *       Call cyg_thread_resume() to resume the task
 *    End If
 * END PDL
 *
 *****************************************************************************/
extern void os_TaskResume( os_Id_t TaskId )
{
} /* End os_TaskResume() */


/******************************************************************************
 *
 * Name: os_TaskKill
 *
 * Description:
 *   This routine is called to kill a given task
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
 * PDL:
 *    If the task Id is valid Then
 *       Call cyg_thread_kill() to kill the task
 *       Reset the task handle to zero
 *    End If
 * END PDL
 *
 *****************************************************************************/
extern void os_TaskKill( os_Id_t TaskId )
{

} /* End os_TaskKill() */


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
extern void os_TaskDelay( os_TickCount_t Count )
{
} /* End os_TaskDelay() */


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
 * PDL:
 *    If the queue Id is invalid or the queue has already been created Then
 *       Return the appropriate error code
 *    End If
 *    Call cyg_mbox_create() to create a queue
 *    If the queue was created Then
 *       Return success status
 *    Else
 *       Return fail status
 *    End If
 * END PDL
 *
 *****************************************************************************/
extern UINT8 os_QueueCreate( os_Id_t QueueId )
{
   return (os_SUCCESS);

} /* End os_QueueCreate() */


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
 * PDL:
 *    If the queue Id is valid Then
 *       Call cyg_mbox_delete() to
 *       Reset the queue handle to zero
 *    End If
 * END PDL
 *
 *****************************************************************************/
extern void os_QueueDelete( os_Id_t QueueId )
{

} /* End os_QueueDelete() */


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
 * PDL:
 *    If the queue Id is valid Then
 *       Call cyg_tryget() to read a message
 *       Return a pointer to the message read
 *    Else
 *       Return a null pointer
 *    End If
 * END PDL
 *
 *****************************************************************************/
extern void* os_QueueReadNoBlock( os_Id_t QueueId )
{
    return (NULL);

} /* End os_QueueReadNoBlock() */


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
 * PDL:
 *    If the queue Id is valid Then
 *       Call cyg_tryget() to retrieve message
 *       Return status indicating success or failure of the write
 *    Else
 *       Return OS_FAIL
 *    End If
 * END PDL
 *
 *****************************************************************************/
extern WL_STATUS os_QueueWriteNoBlock( os_Id_t QueueId, void *Item_p )
{
    return OS_SUCCESS;

} /* End os_QueueWriteNoBlock() */


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
extern UINT32 os_QueuePeek( os_Id_t QueueId )
{
   return 0;

} /* End os_QueuePeek() */


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
 * PDL:
 *    If the interrupt Id is invalid or the interrupt has already been
 *       created
 *    Then
 *       Return the appropriate error code
 *    End If
 *    Call cyg_interrupt_create() to create the interrupt
 *    If the interrupt was created Then
 *       Return success status
 *    Else
 *       Return fail status
 *    End If
 * END PDL
 *
 *****************************************************************************/
extern WL_STATUS os_InterruptCreate( os_Id_t        InterruptId,
                                    os_Vector_t    Vector,
                                    os_Priority_t  Priority,
                                    os_ISR_t      *Isr_p,
                                    os_DSR_t      *Dsr_p )
{
   return (os_SUCCESS);

} /* End os_InterruptCreate() */


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
 * PDL:
 *    If the interrupt Id is valid Then
 *       Call cyg_interrupt_attach() to attach interrupt to the hardware
 *    End If
 * END PDL
 *
 *****************************************************************************/
extern void os_InterruptAttach( os_Id_t InterruptId )
{

} /* End os_InterruptAttach() */


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
 * PDL:
 *    Call cyg_interrupt_disable() to disable cpu interrupts
 * END PDL
 *
 *****************************************************************************/
extern void os_InterruptDisable( void )
{
} /* End os_InterruptDisable() */


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
 * PDL:
 *    Call cyg_interrupt_enable() to enable all ISRs
 * END PDL
 *
 *****************************************************************************/
extern void os_InterruptEnable( void )
{
} /* End os_InterruptEnable() */


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
 * PDL:
 *    Call cyg_interrupt_mask() to mask interrupts specified by the given
 *       vector
 * END PDL
 *
 *****************************************************************************/
extern void os_InterruptMask( os_Vector_t Vector )
{
} /* End os_InterruptMask() */


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
 * PDL:
 *    Call cyg_interrupt_mask() to unmask interrupts specified by the given
 *       vector
 * END PDL
 *
 *****************************************************************************/
extern void os_InterruptUnmask( os_Vector_t Vector )
{
} /* End os_InterruptUnmask() */


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
extern void os_InterruptAck( os_Vector_t Vector )
{
} /* End os_InterruptAck() */


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
 * PDL:
 *    If the semaphore Id is valid Then
 *       Call cyg_semaphore_init() to intialize the semaphore
 *    End If
 * END PDL
 *
 *****************************************************************************/

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
 * PDL:
 *    If the semaphore Id is valid Then
 *       Call cyg_semaphore_destroy() to destroy the semaphore
 *    End If
 * END PDL
 *
 *****************************************************************************/
extern void os_SemaphoreDestroy( os_Id_t SemId )
{
} /* End os_SemaphoreDestroy() */


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
 * PDL:
 *    If the semaphore Id is valid Then
 *       Call cyg_semaphore_trywait() to claim a semaphore (and wait for it
 *          if necessary)
 *    End If
 * END PDL
 *
 *****************************************************************************/


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
 *   Arg1 (i  ): SemId - Id (index) of the semaphore to be returned
 *
 * Return Value:
 *   None.
 *
 * Notes:
 *   By returning a semaphore, a waiting task is woken to claim the
 *   semaphore; if there are no waiting tasks, the semaphore count is
 *   incremented.
 *
 * PDL:
 *    If the semaphore Id is valid Then
 *       Call cyg_semaphore_post() to
 *    End If
 * END PDL
 *
 *****************************************************************************/


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
 * PDL:
 *    If the event Id is valid Then
 *       Call cyg_flag_init() to intialize the event
 *    End If
 * END PDL
 *
 *****************************************************************************/
extern void os_EventInit( os_Id_t EventId )
{
} /* End os_EventInit() */


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
 * PDL:
 *    If the event Id is valid Then
 *       Call cyg_flag_setbits() to trigger the given event(s)
 *    End If
 * END PDL
 *
 *****************************************************************************/
extern void os_EventTrigger( os_Id_t              EventId,
                             os_IndicatedEvents_t EventsToTrigger )
{
} /* End os_EventTrigger() */


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
 * PDL:
 *    If the event Id is valid Then
 *       Call cyg_flag_setbits() to trigger the given events
 *    End If
 * END PDL
 *
 *****************************************************************************/
extern os_IndicatedEvents_t os_EventWait(
                                        os_Id_t              EventId,
                                        os_IndicatedEvents_t EventsToWaitFor,
                                        os_EventMode_t       Mode )
{
   return (0);

} /* End os_EventWait() */


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
 * PDL:
 *    If the event Id is valid Then
 *       Call cyg_flag_destroy() to destroy the event
 *    End If
 * END PDL
 *
 *****************************************************************************/
extern void os_EventDestroy( os_Id_t EventId )
{

} /* End os_EventDestroy() */


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
 * PDL:
 *    If the event Id is valid Then
 *       Call cyg_flag_maskbits() to clear the events
 *    End If
 * END PDL
 *
 *****************************************************************************/
extern void os_EventClear( os_Id_t               EventId,
                           os_IndicatedEvents_t  EventsToClear )
{
} /* End os_EventClear() */


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
 *   Arg1 (i  ): EventId - Id (index) of the event flag for which an
 *                         event(s) is to be waited on
 *
 * Return Value:
 *   events.
 *
 * Notes:
 *   None.
 *
 * PDL:
 *    If the event Id is valid Then
 *       Call cyg_flag_setbits() to trigger the given events
 *    End If
 * END PDL
 *
 *****************************************************************************/
extern os_IndicatedEvents_t os_EventPeek(os_Id_t EventId )
{
   return (0);

} /* End os_EventPeek() */


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
 * PDL:
 *    If the timer Id is invalid or the timer has already been
 *       created
 *    Then
 *       Return the appropriate error code
 *    End If
 *    Call cyg_counter_create() to create a counter
 *    Call cyg_alarm_create() to create a task
 *    If the counter and timer were created Then
 *       Return success status
 *    Else
 *       Return fail status
 *    End If
 * END PDL
 *
 *****************************************************************************/
extern WL_STATUS os_TimerCreate( os_Id_t        TimerId,
                                os_Alarm_t    *TimerFunc_p,
                                os_Addrword_t  Data )

{
   return (os_SUCCESS);

} /* End os_TimerCreate() */


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
 * PDL:
 *    If the timer Id is valid Then
 *       Call cyg_thread_create() to create a task
 *    End If
 * END PDL
 *
 *****************************************************************************/
extern void os_TimerInit( os_Id_t         TimerId,
                          os_TickCount_t  Trigger,
                          os_TickCount_t  Interval )
{
} /* End os_TimerInit() */


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
 * PDL:
 *    If the timer Id is valid Then
 *       Call cyg_alarm_delete()
 *       Clear the handle for the given timer Id
 *    End If
 * END PDL
 *
 *****************************************************************************/
extern void os_TimerDelete( os_Id_t TimerId )
{
} /* End os_TimerDelete() */


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
 * PDL:
 *    If the timer Id is valid Then
 *       Call cyg_alarm_enable() to enable the timer
 *    End If
 * END PDL
 *
 *****************************************************************************/
extern void os_TimerEnable( os_Id_t TimerId )
{
} /* End os_TimerEnable() */


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
 * PDL:
 *    If the timer Id is valid Then
 *       cyg_alarm_disable() to disable the timer
 *    End If
 * END PDL
 *
 *****************************************************************************/
extern void os_TimerDisable( os_Id_t TimerId )
{
} /* End os_TimerDisable() */


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
extern UINT32 os_CurrentTime(void)
{
    return 0;
}


/******************************************************************************
 *
 * Name: os_var_pool_create
 *
 * Description:
 *   This routine is called to create a memory for dynamic memory allocations.
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
 *   This routine will block until the memory becomes available.
 *
 * PDL:
 *    Call cyg_mempool_var_alloc() to allocate the memory
 * END PDL
 *
 *****************************************************************************/
void os_var_pool_create( void *base, UINT32 size, MHANDLE *handle, void *var)
{
} /* End os_var_pool_create() */


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
 *   This routine will block until the memory becomes available.
 *
 * PDL:
 *    Call cyg_mempool_var_alloc() to allocate the memory
 * END PDL
 *
 *****************************************************************************/
void *os_var_alloc( MHANDLE handle, UINT32 Size )
{
	return NULL;
} /* End os_var_alloc() */


/******************************************************************************
 *
 * Name: os_Free
 *
 * Description:
 *   This routine is called to free previously allocated memory.
 *
 * Conditions For Use:
 *   Only used for memory allocated by the os_var_free() routine.
 *
 * Arguments:
 *   Arg1 (i  ): MemBuffer_p - pointer to the memory to free
 *
 * Return Value:
 *   None.
 *
 * Notes:
 *   The memory is freed back into the memory pool allocated at the
 *   OS initialization.
 *
 * PDL:
 *    If the given pointer to memory is not NULL Then
 *       Call cyg_mempool_var_free() to free the memory pointed to
 *    End If
 * END PDL
 *
 *****************************************************************************/
void os_var_free( MHANDLE handle, void *p )
{
} /* End os_var_free() */

/******************************************************************************
 *
 * Name: os_fix_pool_create
 *
 * Description:
 *   This routine is called to create a memory for dynamic memory allocations.
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
 *   This routine will block until the memory becomes available.
 *
 * PDL:
 *    Call cyg_mempool_fix_alloc() to allocate the memory
 * END PDL
 *
 *****************************************************************************/
void os_fix_pool_create( void *base, UINT32 size, UINT32 bufSz, MHANDLE *handle, void *fix)
{
} /* End os_fix_pool_create() */


/******************************************************************************
 *
 * Name: os_fix_alloc
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
 *   This routine will block until the memory becomes available.
 *
 * PDL:
 *    Call cyg_mempool_fix_alloc() to allocate the memory
 * END PDL
 *
 *****************************************************************************/
void *os_fix_alloc( MHANDLE handle )
{
   return NULL;

} /* End os_Malloc() */


/******************************************************************************
 *
 * Name: os_Free
 *
 * Description:
 *   This routine is called to free previously allocated memory.
 *
 * Conditions For Use:
 *   Only used for memory allocated by the os_fix_free() routine.
 *
 * Arguments:
 *   Arg1 (i  ): MemBuffer_p - pointer to the memory to free
 *
 * Return Value:
 *   None.
 *
 * Notes:
 *   The memory is freed back into the memory pool allocated at the
 *   OS initialization.
 *
 * PDL:
 *    If the given pointer to memory is not NULL Then
 *       Call cyg_mempool_fix_free() to free the memory pointed to
 *    End If
 * END PDL
 *
 *****************************************************************************/
void os_fix_free( MHANDLE handle, void *p )
{
} /* End os_fix_free() */


