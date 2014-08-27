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

#ifndef list_h
#define list_h


typedef struct ListItem
{
	struct ListItem *nxt;
	struct ListItem *prv;
}ListItem;


typedef struct List
{
	ListItem *head;
	ListItem *tail;
	unsigned int cnt;
}List;
void ListInit(List *me);
ListItem *ListGetItem(List *me);
void ListPutItem(List *me, ListItem *Item);
void ListPutItemFILO(List *me, ListItem *Item);
ListItem *ListDelItem(List *me, ListItem *Item);
ListItem *ListRmvItem(List *me, ListItem *Item);

#endif
