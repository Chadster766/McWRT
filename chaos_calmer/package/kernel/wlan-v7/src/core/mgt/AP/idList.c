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


/*!
 * \file    idList.c
 * \brief   station id and 802.11 aid management
 */

/*=============================================================================
 *                               INCLUDE FILES
 *=============================================================================
 */

#include "wltypes.h"
#include "List.h"
#include "osif.h"
#include "buildModes.h"
#include "ap8xLnxIntf.h"
UINT32 AssocStationsCnt= 0;
/*=============================================================================
 *                                DEFINITIONS
 *=============================================================================
*/


typedef struct IdListElem_t

{
    struct IdListElem_t *nxt;
    struct IdListElem_t *prv;
    UINT16 Id;
}
IdListElem_t;


/*=============================================================================
 *                         IMPORTED PUBLIC VARIABLES
 *=============================================================================
 */
/*=============================================================================
 *                          MODULE LEVEL VARIABLES
 *=============================================================================
 */
DECLARE_LOCK(idLock);				/*used to protect aid and stnid*/

IdListElem_t AidList[MAX_AID + 1];

IdListElem_t *StnIdList= NULL;


static List FreeStaIdList;
static List StaIdList;

static List FreeAIDList;
static List AIDList;

extern void ListPutItemFILO(List *me, ListItem *Item);
/*============================================================================= 
 *                   PRIVATE PROCEDURES (ANSI Prototypes) 
 *=============================================================================
 */

/*============================================================================= 
 *                         CODED PROCEDURES 
 *=============================================================================
 */

/*
 *Function Name:InitAidList
 *
 *Parameters:
 *
 *Description:
 *
 *Returns:
 *
 */
void InitAidList(void)
{
    UINT32 i;
	SPIN_LOCK_INIT(&idLock);

    ListInit(&FreeAIDList);
    ListInit(&AIDList);
    for (i = 0; i < MAX_AID; i++)
    {
        AidList[i].nxt = NULL;
        AidList[i].prv = NULL;
		AidList[i].Id = MAX_AID - i;
        ListPutItemFILO(&FreeAIDList, (ListItem*)(AidList+i));
    }
}

/*
 *Function Name:AssignAid
 *
 *Parameters:
 *
 *Description:
 *
 *Returns:
 *
 */
UINT32 AssignAid(void)
{
    ListItem *tmp;
    IdListElem_t *tmp1;
	unsigned long idflags;
	SPIN_LOCK_IRQSAVE(&idLock, idflags);
    tmp = ListGetItem(&FreeAIDList); 
    if (tmp)
    {
        tmp1 = (IdListElem_t *)tmp;
        ListPutItemFILO(&AIDList,tmp);
        AssocStationsCnt++;
		SPIN_UNLOCK_IRQRESTORE(&idLock, idflags);
        return tmp1->Id;
    }
	SPIN_UNLOCK_IRQRESTORE(&idLock, idflags);
    return 0;   /* List is empty */
}

/*
 *Function Name:FreeAid
 *
 *Parameters:
 *
 *Description:
 *
 *Returns:
 *
 */

void FreeAid(UINT32 Aid)
{
    ListItem *search;
    IdListElem_t *search1;
	unsigned long idflags;
	SPIN_LOCK_IRQSAVE(&idLock, idflags);
    search = AIDList.head;
    while (search)
    {
        search1 = (IdListElem_t *)search;
        if ((search1->Id == Aid))
        {
            ListPutItemFILO(&FreeAIDList,ListRmvItem(&AIDList,search));
            AssocStationsCnt--;
			SPIN_UNLOCK_IRQRESTORE(&idLock, idflags);
            return ;
        }
        search = search->nxt;
    }
	SPIN_UNLOCK_IRQRESTORE(&idLock, idflags);
}
Status_e ResetAid(UINT16 StnId, UINT16 Aid)
{
   return FAIL;
}
/*
 *Function Name:InitStnId
 *
 *Parameters:
 *
 *Description:
 *
 *Returns:
 *
 */

WL_STATUS InitStnIdList(int max_stns)
{
    UINT32 i;
    ListInit(&FreeStaIdList);
    ListInit(&StaIdList);
    if (StnIdList == NULL)
    {
		StnIdList = malloc((max_stns + 1)*sizeof(IdListElem_t));
	if (StnIdList == NULL) 
		return (OS_FAIL);
	memset(StnIdList, 0, (max_stns + 1)*sizeof(IdListElem_t));
    }
    for (i = 0; i < max_stns; i++)
    {
        StnIdList[i].nxt = NULL;
        StnIdList[i].prv = NULL;
        StnIdList[i].Id = max_stns - i;
        ListPutItemFILO(&FreeStaIdList, (ListItem*)(StnIdList+i));
    }
	return(OS_SUCCESS);
}

void StnIdListCleanup(void)
{
    if (StnIdList){
		free(StnIdList);
	StnIdList = 0;
    	}
}

/*
 *Function Name:AssignStnId
 *
 *Parameters:
 *
 *Description:
 *
 *Returns:
 *
 */

UINT32 AssignStnId(void)
{
    ListItem *tmp;
    IdListElem_t *tmp1;
	unsigned long idflags;
	SPIN_LOCK_IRQSAVE(&idLock, idflags);
    tmp = ListGetItem(&FreeStaIdList); 
    if (tmp)
    {
        tmp1 = (IdListElem_t *)tmp;
        ListPutItemFILO(&StaIdList,tmp);
		SPIN_UNLOCK_IRQRESTORE(&idLock, idflags);
        return tmp1->Id;
    }
	SPIN_UNLOCK_IRQRESTORE(&idLock, idflags);
    return 0;   /* List is empty */
}

/*
 *Function Name:FreeStnId
 *
 *Parameters:
 *
 *Description:
 *
 *Returns:
 *
 */

void FreeStnId(UINT32 StnId)
{
    ListItem *search;
    IdListElem_t *search1;
	unsigned long idflags;
	SPIN_LOCK_IRQSAVE(&idLock, idflags);

    search = StaIdList.head;
    while (search)
    {
        search1 = (IdListElem_t *)search;
        if ((search1->Id == StnId))
        {
            ListPutItemFILO(&FreeStaIdList,ListRmvItem(&StaIdList,search));
			SPIN_UNLOCK_IRQRESTORE(&idLock, idflags);
            return ;
        }
        search = search->nxt;
    }
	SPIN_UNLOCK_IRQRESTORE(&idLock, idflags);
}

