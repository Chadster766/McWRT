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
*   Description:  This file implemented functionality of
*                 a double link list       
*                                          
*/

/*!
 * \file    List.c
 * \brief   a double link list object
 */
#include "List.h"
#include "osif.h"

#ifndef NULL
#define NULL ((void *)0)
#endif

void ListPutItemFILO(List *me, ListItem *Item);


/*!
 * Initialize a list
 * @param me Pointer to the list
 */
void ListInit(List *me)
{
    me->head = NULL;
    me->tail = NULL;
    me->cnt = 0;
}

/*!
 * get a node from a list
 * @param me Pointer to the list
 * @return Pointer to the node if success, NULL if fail 
 */
ListItem *ListGetItem(List *me)
{
    ListItem *Item;
    os_EnterCriticalSection;
	if(!me->cnt)
    {
        os_ExitCriticalSection;
        return NULL;
    }
    Item = me->tail;
	if (me->tail->prv) {
		me->tail = me->tail->prv;
		me->tail->nxt = NULL;
	} else {
		me->head = me->tail = NULL;	/* Was the only item in the list */
	}
	Item->nxt = Item->prv = NULL;
	me->cnt--;
    os_ExitCriticalSection;
	return Item;
}

/*!
 * put a node to a list
 * @param me Pointer to the list
 * @param Item Pointer to the node
 */
void ListPutItem(List *me, ListItem *Item)
{
    if (Item == NULL)
        return;
    os_EnterCriticalSection;
   	Item->nxt  = me->head;	/* Attach item at the head of the list */
	Item->prv  = NULL;      /* Nullify the Backward pointer */

	if (me->head )			/* Re-align the backward pointer of the existing head item */
		me->head->prv = Item;
	else
		me->tail = Item;	/* If the list was empty Then */
	me->head = Item;		/* first item is both head & tail in the list */
	me->cnt++;				/* increment item count */
    os_ExitCriticalSection;
	return;
}
void ListPutItemFILO(List *me, ListItem *Item)
{
    if (Item == NULL)
        return;
    os_EnterCriticalSection;
   	Item->prv  = me->tail;	/* Attach item at the head of the list */
	Item->nxt  = NULL;      /* Nullify the Backward pointer */

	if (me->tail )			/* Re-align the backward pointer of the existing head item */
		me->tail->nxt = Item;
	else
		me->head = Item;	/* If the list was empty Then */
	me->tail = Item;		/* first item is both head & tail in the list */
	me->cnt++;				/* increment item count */
    os_ExitCriticalSection;
	return;
}

/*!
 * remove a node form a list
 * @param me Pointer to the list
 * @param Item pointer to the node
 * @return Pointer to the node if success, NULL if fail 
 */
ListItem *ListRmvItem(List *me, ListItem *Item)
{
    if(Item == NULL)
        return NULL;
    if (me->cnt == 0)
        return NULL;
    os_EnterCriticalSection;
    if (Item->prv && Item->nxt)
    {   /*not head neither tail */
        Item->prv->nxt = Item->nxt;
        Item->nxt->prv = Item->prv;
    }
    else
    {
        if(Item->prv)
        {
			/*this is tail */
            /*Item->prv->nxt->nxt = NULL; */
            Item->prv->nxt = NULL;
			me->tail = Item->prv;
        }
        else if (Item->nxt)
        {	/*this is head */
			/*Item->nxt->prv->prv = NULL; */
			Item->nxt->prv = NULL;
			me->head = Item->nxt; 
        }else
        {   /*only one item in the list */
            me->head = me->tail = NULL;
        }
    }
    Item->nxt = NULL;
    Item->prv = NULL;
    me->cnt--;
    os_ExitCriticalSection;
    return Item;
}
