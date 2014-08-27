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
