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

#include "mhsm.h"

#define MHSM_ASSERT(x) while(!x);

#define MAX_MHSM_DEPTH 6

/*
** Local copies of enter / exit events so we can insert these where
** needed without needing to insert in the middle of the queue
*/
const MhsmEvent_t mhsm_exit  = {MHSM_EXIT,  NULL};
const MhsmEvent_t mhsm_enter = {MHSM_ENTER, NULL};
static void mhsm_process_event(Mhsm_t * pHsm, Event_t event, void * pData);

void mhsm_initialize(Mhsm_t * pHsm, MhsmState_t * pStart)
{
    /* initialize the rest of the HSM */
    pHsm->pCurrent          = pStart;

    /* Enter the Start state to start the HSM */
    mhsm_process_event(pHsm, MHSM_ENTER, NULL);
}

void mhsm_add(MhsmState_t * pState, MhsmState_t * pSuper, MhsmFcnPtr pFcn)
{
    /* setup the states variables */
    pState->pSuper           = pSuper;
    pState->pProcessEventFcn = pFcn;

    /*
    ** if the state has no super state, then the state is the
    ** top state.  The top state has a depth of 0
    */
    if (pSuper == NULL)
    {
        pState->depth            = 0;
    }
    else
    {
        pState->depth            = pSuper->depth + 1;
    }
    
    /* make sure the max depth is not exceeded */
    MHSM_ASSERT((pState->depth < MAX_MHSM_DEPTH));
}

void mhsm_send_event(Mhsm_t * pHsm, MhsmEvent_t *pEvent)
{
    MhsmState_t * pTemp = pHsm->pCurrent;

    MhsmEvent_t   tempEvent;

    tempEvent.event = pEvent->event;
    tempEvent.pBody = pEvent->pBody;
    tempEvent.devinfo = pEvent->devinfo;
    tempEvent.info = pEvent->info;
	

    /*
    ** keep giving the event to the super state
    ** until it is consumed
    */
    while ((*(pTemp->pProcessEventFcn))(pHsm, &tempEvent) &&
           pTemp->pSuper != NULL)
    {
        pTemp = pTemp->pSuper;
    }
}

static void mhsm_process_event(Mhsm_t * pHsm, Event_t event, void * pData)
{
    MhsmState_t * pTemp = pHsm->pCurrent;

    MhsmEvent_t   tempEvent;

    tempEvent.event = event;
    tempEvent.pBody = pData;

    /*
    ** keep giving the event to the super state
    ** until it is consumed
    */
    while ((*(pTemp->pProcessEventFcn))(pHsm, &tempEvent) &&
           pTemp->pSuper != NULL)
    {
        pTemp = pTemp->pSuper;
    }
}


void mhsm_transition(Mhsm_t * pHsm, MhsmState_t * pDest)
{
    int i;
    MhsmState_t * ppBackTrack[MAX_MHSM_DEPTH];
    int backTrackCnt = 0;
    MhsmState_t *pFrom = pHsm->pCurrent;
    MhsmState_t *pTo   = pDest;
    
    
    if (pTo != pFrom)
    {
        /* 
        ** We need to get the states to the same depth to find
        ** the common parent.  Equalize the depths.
        */
        if (pFrom->depth > pTo->depth)
        {
            /* the src state is deeper than the
            ** destination, so find the parent
            ** that is equal to the destination depth.
            */
            i = pFrom->depth - pTo->depth;
            for (; i > 0; i--)
            {
                (*(pFrom->pProcessEventFcn))(pHsm, (MhsmEvent_t *)&mhsm_exit);
                pFrom = pFrom->pSuper;
            }
        }
        else if (pFrom->depth < pTo->depth)
        {
            /* the destination state is deeper than the
            ** src, so find the parent
            ** that is equal to the src depth.
            */
            i = pTo->depth - pFrom->depth;
            for (; i> 0; i--)
            {
                pTo = pTo->pSuper;
                ppBackTrack[backTrackCnt] = pTo;
                backTrackCnt++;
            }
        }
        
        /*
        ** No we have the src & dest states at a common depth
        ** we can easily find the common parent
        */
        while (pTo != pFrom)
        {
            /*
            ** We need to keep track of the to states so we can
            ** enter them in the correct order
            */
            pTo   = pTo->pSuper;
            ppBackTrack[backTrackCnt] = pTo;
            backTrackCnt++;

            /* The from states we can exit now */
            (*(pFrom->pProcessEventFcn))(pHsm, (MhsmEvent_t *)&mhsm_exit);
            pFrom = pFrom->pSuper;
        }

        if (backTrackCnt)
        {
            /* Enter down until just before the destination state */
            /*
            ** the backtrack array holds the parent.  The parent has already
            ** had an enter performed, so go find the children 
            */
            backTrackCnt--;
        
            while (backTrackCnt)
            {
                backTrackCnt--;
                (*(ppBackTrack[backTrackCnt]->pProcessEventFcn))(
                    pHsm,
                    (MhsmEvent_t *)&mhsm_enter);
            }

            /* assign the current state before doing the final enter
            ** to accomodate some of our state machines that have
            ** a transition in the enter event
            */
            pHsm->pCurrent = pDest;

            /*
            ** we've accounted for all exit /enters except the dest state.
            ** now enter the dest state.
            */
            (*(pDest->pProcessEventFcn))(pHsm, (MhsmEvent_t *)&mhsm_enter);
        }
        else
        {
            /* Assign the current state.  The new state is a parent
            ** of the current state so no new enters need to be done.
            */
            pHsm->pCurrent = pDest;
 
        }
    }
}




