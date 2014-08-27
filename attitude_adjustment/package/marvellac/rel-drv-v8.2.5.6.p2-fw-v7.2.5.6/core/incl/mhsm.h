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

#ifndef MHSM_
#define MHSM_

#include "wltypes.h"

/******************************************************************/
/*!
 *  \internal
 *  \defgroup marvell_hsm Marvell HSM implementation
 *
 *  Ths group covers functions used for the Marvell Hierarchical
 *  MhsmState_t Machine (HSM).  The MhsmState_t structures and HSM are
 *  separated to allowed multiple instances of an HSM to use the
 *  same state transition structures.
 */
/******************************************************************/
/*!
 *  \internal 
 *  \ingroup marvell_hsm
 *  \file   mhsm.h
 *  \brief  Internal API for the Marvell HSM
 *
 *  This file contains the APIs and structures required for the 
 *  Marvell HSM.
 *
 *******************************************************************/
#define MHSM_ENTER 0xffffffff
#define MHSM_EXIT  0xfffffffe
#define MhsmInState(sm_, state) (((Mhsm_t *)&sm_)->pCurrent == &sm_.state)

#define MhsmInSyncState(sm_, state) (((Mhsm_t *)&sm_)->pCurrent == &sm_.pStates->state)

typedef struct Mhsm_ Mhsm_t;
typedef UINT32 Event_t;

/*! The event to be passed to the Marvell HSM */
typedef struct MhsmEvent_
{
    /*! The event to be processed */
    Event_t event;

    /*! Any addition data needed by the event processor */
    void * pBody;
    unsigned char *info;
    unsigned char *devinfo;
}MhsmEvent_t;

typedef MhsmEvent_t const * (*MhsmFcnPtr)(Mhsm_t * pHsm, MhsmEvent_t * pEvent);

/*!  The Structure for Marvell HSM States */
typedef struct MhsmState_
{
    /*! The depth of this state from the top */
    UINT32 depth;
    
    /*! The parent of this state */
    struct MhsmState_ * pSuper;

    /*! The event handler for the state */
    MhsmFcnPtr pProcessEventFcn;
    
}MhsmState_t;

struct Mhsm_
{
    /*! The current state in this Marvell HSM */
    MhsmState_t * pCurrent;
};

/******************************************************************/
/*!
 *  \internal 
 *  \ingroup marvell_hsm
 *
 *  \brief Initializes an HSM
 *
 *  \param[in] pHsm       Pointer to the HSM to intialize.
 *  \param[in] pStart     Pointer to the Start state of the HSM.
 *
 *  This function intializes an instance of an HSM.  It sets the HSM
 *  to enter the Start state.
 *
 ******************************************************************/
extern void mhsm_initialize(Mhsm_t * pHsm, MhsmState_t * pStart);

/******************************************************************/
/*!
 *  \internal 
 *  \ingroup marvell_hsm
 *
 *  \brief Adds a state to an existing HSM
 *
 *  \param[in] pState     Pointer to the HSM state to add.
 *  \param[in] pSuper     Pointer to the new states super state.
 *  \param[in] pFcn       Function pointer to the event handler.
 *
 ******************************************************************/
extern void mhsm_add(MhsmState_t * pState,
                     MhsmState_t * pSuper,
                     MhsmFcnPtr pFcn);

/******************************************************************/
/*!
 *  \internal 
 *  \ingroup marvell_hsm
 *
 *  \brief Process an event in the HSM
 *
 *  \param[in] pHsm       Pointer to the HSM to handle the event.
 *  \param[in] pEvent     Pointer to the MSM event.
 *
 ******************************************************************/
extern void mhsm_send_event(Mhsm_t * pHsm, MhsmEvent_t *pEvent);

/******************************************************************/
/*!
 *  \internal 
 *  \ingroup marvell_hsm
 *
 *  \brief Transition the HSM from the current state to the provided
 *         destination state.
 *
 *  \param[in] pHsm       Pointer to the HSM to handle the transition.
 *  \param[in] pDest      Pointer to destination state.
 *
 ******************************************************************/
extern void mhsm_transition(Mhsm_t * pHsm, MhsmState_t * pDest);

#endif
