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

#ifndef __EWB_HASH_H__
#define __EWB_HASH_H__


#define HASH_MAX_BUCKET         256
//#define HASH_ENTRY_MAX          HASH_MAX_BUCKET
#define HASH_ENTRY_COLUMN_MAX   256
#define HASH_ENTRY_ROW_MAX      64


typedef struct hash_entry{
	unsigned char *prvEntry;
    unsigned char   numInRow;
	unsigned long    nwIpAddr;
	unsigned char    hwAddr[6];
	unsigned char *nxtEntry;
}hash_entry;

typedef struct hash_table{
	hash_entry entry[HASH_MAX_BUCKET];
}hash_table;


extern int wetUpdateHashEntry(unsigned long key, unsigned char *clntMac);
extern unsigned char *wetGetHashEntryValue(unsigned long key);
extern int wetClearHashEntry(unsigned long key);
extern void wetHashInit(void);
extern void wetHashDeInit(void);

extern hash_entry hashTable[HASH_ENTRY_COLUMN_MAX];
#endif /* __EWB_HASH_H__ */
