/*
 * Copyright (c) 2000-2005 by Marvell International Ltd.
 * 
* If you received this File from Marvell, you may opt to use, redistribute 
* and/or modify this File in accordance with the terms and conditions of the 
* General Public License Version 2, June 1991 (the “GPL License”), a copy of 
* which is available along with the File in the license.txt file or by writing 
* to the Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, 
* MA 02111-1307 or on the worldwide web at http://www.gnu.org/licenses/gpl.txt. 
*
* THE FILE IS DISTRIBUTED AS-IS, WITHOUT WARRANTY OF ANY KIND, AND THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE ARE 
* EXPRESSLY DISCLAIMED. The GPL License provides additional details about 
* this warranty disclaimer.
*
*/
#ifndef WL_KERNEL_26
#include <linux/config.h>
#endif
#include <linux/version.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/if_arp.h>
#include <linux/random.h>

#include "ewb_hash.h"
#define HW_ADDR_LEN				6

/* Globally defined table for testing */
hash_entry hashTable[HASH_ENTRY_COLUMN_MAX] __attribute__ ((section (".data")));
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
void wetHashInit(void)
{
    memset(&hashTable, 0, sizeof(hash_entry) * HASH_ENTRY_COLUMN_MAX);
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
void wetHashDeInit(void)
{
    int i;
	hash_entry *pEntry;
    hash_entry *pNextEntry;

    for(i=0; (i < HASH_ENTRY_COLUMN_MAX); i++)
	{
    	pEntry = &hashTable[i];
        pNextEntry = (hash_entry *)pEntry->nxtEntry;
        
        while(pNextEntry)            
        {
            pEntry = pNextEntry;
            pNextEntry = (hash_entry *)pEntry->nxtEntry;
            kfree(pEntry);
        }
	}
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
int wetDoHash(unsigned long key, int prime)
{
	unsigned long hash;

    hash = key >> 24;

	return hash % prime;
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
hash_entry * wetSearchHashEntry(unsigned long key)
{
	int index;
    int i;
	hash_entry *pEntry;

    /* do not accept keys with zero value */
    if (key == 0)
        return NULL;
    
	index = wetDoHash(key, HASH_MAX_BUCKET);
	pEntry = &hashTable[index];

    for(i=0; (i < HASH_ENTRY_ROW_MAX); i++)
	{
        if(pEntry->nwIpAddr == key)
        {
            return pEntry;
        }
		if(pEntry->nxtEntry == NULL)
		{
            return NULL;
		}
		pEntry = (hash_entry *)pEntry->nxtEntry;
	}
    
	return NULL;
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
int wetAddHashEntry(hash_entry *currentEntry, 
					unsigned long key, 
					unsigned char * clntMac)
{
	hash_entry *nwEntry;
	
	if((clntMac == NULL) || (currentEntry == NULL))
	{
		return -1;
	}

	if(currentEntry->nxtEntry != NULL)
	{
		/* This is not the last entry in the link list */
		return -1;
	}

	if((currentEntry->nxtEntry = (unsigned char *)(kmalloc(sizeof(hash_entry), GFP_ATOMIC)))
		== NULL)
	{
		return -1;
	}
	nwEntry = (hash_entry *)currentEntry->nxtEntry;
	nwEntry->prvEntry = (unsigned char *)currentEntry;
	nwEntry->nxtEntry = NULL;
    nwEntry->nwIpAddr = key;

	return 0;
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
int wetUpdateHashEntry(unsigned long key, unsigned char *clntMac)
{
	int index;
    int i;
	hash_entry *pEntry;

    if((clntMac == NULL) || (key == 0))
    {
        return -1;
    }

	index = wetDoHash(key, HASH_MAX_BUCKET);
	pEntry = &hashTable[index];

    if(pEntry->numInRow == 0)
    {
        pEntry->numInRow = 1;
        pEntry->prvEntry = NULL;
        pEntry->nxtEntry = NULL;
        pEntry->nwIpAddr = key;
        goto updateEntry;
    }

    for(i=0; i < HASH_ENTRY_ROW_MAX; i++)
	{
        if(pEntry->nwIpAddr == key)
        {
            goto updateEntry;
        }

		if(pEntry->nxtEntry == NULL)
		{
			if(wetAddHashEntry(pEntry, key, clntMac) < 0)
            {
                return -1;
            }

            hashTable[index].numInRow++;
            //goto updateEntry;
		}
		pEntry = (hash_entry *)pEntry->nxtEntry;
	}

    return -1;
    
updateEntry:
//	pEntry->nwIpAddr = key;
	memcpy(pEntry->hwAddr, clntMac, HW_ADDR_LEN);

	return 0;
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
unsigned char *wetGetHashEntryValue(unsigned long key)
{
	hash_entry *pEntry;

	if((pEntry = wetSearchHashEntry(key)) == NULL)
	{
		return NULL;
	}

	return pEntry->hwAddr;
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
int wetClearHashEntry(unsigned long key)
{
	hash_entry *delEntry;
	hash_entry *pEntry;

	if ((delEntry = wetSearchHashEntry(key)) == NULL)
	{
		return -1;
	}
    
    if (delEntry->prvEntry != NULL)
    {
        pEntry = (hash_entry *)delEntry->prvEntry;
        pEntry->nxtEntry = delEntry->nxtEntry;
        pEntry = (hash_entry *)delEntry->nxtEntry;
        /* Handle last node in list */
        if (pEntry)
	        pEntry->prvEntry = delEntry->prvEntry;

	    kfree(delEntry);
    }
    else
    {
        delEntry->nwIpAddr = 0;
        memset(delEntry->hwAddr, 0, 6);
        delEntry->numInRow--;
    }

	return 0;
}
