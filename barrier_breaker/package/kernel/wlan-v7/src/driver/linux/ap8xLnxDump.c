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


/** include files **/
#ifdef AP8X_DUMP
#include <stdarg.h>
#include "wltypes.h"
#include "wl.h" 
#include "IEEE_types.h"
#include "wl_mib.h"
#include "wl_hal.h"
#include "ap8xLnxApi.h"
#include "ap8xLnxFwcmd.h"
#include "ap8xLnxIntf.h"

#include <linux/kernel.h>	
#include <linux/module.h>	
#include <linux/proc_fs.h>	
#include <linux/seq_file.h>	


#define WL_MAX_FILE_NAME_LEN 32
struct dev_dump
{
	unsigned char filename[WL_MAX_FILE_NAME_LEN];
	struct net_device *netdev;
	struct proc_dir_entry *ap8x_dump_proc;
	unsigned char id; //0: BB 1: RFA 2:RFB, 3:RFC, 4:RFD
};
#undef WL_MAX_FILE_NAME_LEN

#undef WL_MAXIMUM_CARDS
#define WL_MAXIMUM_CARDS 2
extern struct proc_dir_entry *ap8x;
extern struct proc_dir_entry *proc_net;


struct register_map
{
	unsigned char name[32];
	unsigned int base;
	unsigned int start;
	unsigned int end;
};
#define RF_SETS_SC 8
#define BB_SETS_SC 1
struct register_map w8764_rf[RF_SETS_SC] = {
	{"RF Path A base",0xa00, 0, 0xca},
	{"RF Path A xcvr",0, 0x100, 0x1ff},
	{"RF Path B base",0xb00, 0, 0xca},
	{"RF Path B xcvr",0, 0x200, 0x2ff},
	{"RF Path C base",0xc00, 0, 0xca},
	{"RF Path C xcvr",0, 0x300, 0x3ff},
	{"RF Path D base",0xd00, 0, 0xca},
	{"RF Path D xcvr",0, 0x400, 0x4ff},
};
struct register_map w8764_bb[BB_SETS_SC] = {
	{"BB",0x000, 0, 0x56c},
};
struct register_map w8864_rf[RF_SETS_SC] = {
	{"RF Path A base",0xa00, 0, 0xff},
	{"RF Path A xcvr",0, 0x100, 0x1ff},
	{"RF Path B base",0xb00, 0, 0xff},
	{"RF Path B xcvr",0, 0x200, 0x2ff},
	{"RF Path C base",0xc00, 0, 0xff},
	{"RF Path C xcvr",0, 0x300, 0x3ff},
	{"RF Path D base",0xd00, 0, 0xff},
	{"RF Path D xcvr",0, 0x400, 0x4ff},
};
struct register_map w8864_bb[BB_SETS_SC] = {
	{"BB",0x000, 0, 0x6db},
};
#define RF_SETS_SJ 1
#define BB_SETS_SJ 1
struct register_map w8366_rf[RF_SETS_SJ] = {
	{"RF",0x00, 0, 0x3ae},
};
struct register_map w8366_bb[BB_SETS_SJ] = {
	{"BB",0x000, 0, 0x1ed},
};
#define RF_SETS_SF 1
#define BB_SETS_SF 1
struct register_map sf_rf[RF_SETS_SF] = {
	{"RF",0x00, 0, 0x80},
};
struct register_map sf_bb[BB_SETS_SF] = {
	{"BB",0x000, 0, 0x160},
};


#ifdef SOC_W8366
#define RF_SETS RF_SETS_SJ
#define BB_SETS BB_SETS_SJ
#define TOTAL_SETS (RF_SETS+BB_SETS)
#define DUMP_BBP_FILE_NAME "%s_dump1"
#define DUMP_RFA_FILE_NAME "%s_dump0"
#define DUMP_RFB_FILE_NAME "%s"
#define DUMP_RFC_FILE_NAME "%s"
#define DUMP_RFD_FILE_NAME "%s"
struct register_map *rf = w8366_rf;
struct register_map *bb = w8366_bb;
#else
#ifdef SOC_W8764
#define RF_SETS RF_SETS_SC
#define BB_SETS BB_SETS_SC
#define TOTAL_SETS (RF_SETS/2+BB_SETS)
#define DUMP_BBP_FILE_NAME "%s_BBP"
#define DUMP_RFA_FILE_NAME "%s_RFA"
#define DUMP_RFB_FILE_NAME "%s_RFB"
#define DUMP_RFC_FILE_NAME "%s_RFC"
#define DUMP_RFD_FILE_NAME "%s_RFD"
#ifdef SOC_W8864
struct register_map *rf = w8864_rf;
struct register_map *bb = w8864_bb;
#else
struct register_map *rf = w8764_rf;
struct register_map *bb = w8764_bb;
#endif
#else
#define RF_SETS RF_SETS_SF
#define BB_SETS BB_SETS_SF
#define TOTAL_SETS (RF_SETS+BB_SETS)
#define DUMP_BBP_FILE_NAME "%s_dump1"
#define DUMP_RFA_FILE_NAME "%s_dump0"
#define DUMP_RFB_FILE_NAME "%s"
#define DUMP_RFC_FILE_NAME "%s"
#define DUMP_RFD_FILE_NAME "%s"
struct register_map *rf = sf_rf;
struct register_map *bb = sf_bb;
#endif
#endif

static struct dev_dump devdump[WL_MAXIMUM_CARDS][TOTAL_SETS];



/**
 * This function is called at the beginning of a sequence.
 * ie, when:
 *	- the /proc file is read (first time)
 *	- after the function stop (end of sequence)
 *
 */
static void *ap8x_dump_seq_start(struct seq_file *s, loff_t *pos)
{
	static unsigned long counter = 0;

	/* beginning a new sequence ? */
	if ( *pos == 0 )
	{
		/* yes => return a non null value to begin the sequence */
		return &counter;
	}
	else
	{
		/* no => it's the end of the sequence, return end to stop reading */
		*pos = 0;
		return NULL;
	}
}

/**
 * This function is called after the beginning of a sequence.
 * It's called untill the return is NULL (this ends the sequence).
 *
 */
static void *ap8x_dump_seq_next(struct seq_file *s, void *v, loff_t *pos)
{
	unsigned long *tmp_v = (unsigned long *)v;
	(*tmp_v)++;
	(*pos)++;
	return NULL;
}

/**
 * This function is called at the end of a sequence
 * 
 */
static void ap8x_dump_seq_stop(struct seq_file *s, void *v)
{
	/* nothing to do, we use a static value in start() */
}

/**
 * This function is called for each "step" of a sequence
 *
 */
static int ap8x_dump_seq_show(struct seq_file *s, void *v)
{
	//loff_t *spos = (loff_t *) v;
	struct dev_dump *dm_p = (struct dev_dump *)s->private;
	struct net_device *netdev = (struct net_device *)dm_p->netdev;
	struct wlprivate *priv = NETDEV_PRIV_P(struct wlprivate, netdev);
	vmacApInfo_t *vmacSta_p = priv->vmacSta_p;
	MIB_802DOT11 *mib = vmacSta_p->ShadowMib802dot11;
	unsigned long i, val, offset=0, length=0, offset_display;
	int j;
	MIB_PHY_DSSS_TABLE *PhyDSSSTable=mib->PhyDSSSTable;
	UINT8 *mib_rxAntenna_p = mib->mib_rxAntenna;
	UINT8 *mib_guardInterval_p = mib->mib_guardInterval;
	j= 0;
	seq_printf(s, "%s [%2d %2d %2d %2d]", netdev->name,*mib_guardInterval_p, PhyDSSSTable->Chanflag.ChnlWidth, PhyDSSSTable->Chanflag.ExtChnlOffset, *mib_rxAntenna_p);
	if(dm_p->id == 0)
	{
		for(j=0; j<BB_SETS; j++)
		{
			//seq_printf(s, "\n%s\n",bb[j].name);
			seq_printf(s, "\n");
			offset = bb[j].base + bb[j].start;
			length = bb[j].end - bb[j].start +1;
			offset_display = bb[j].start;
			for(i = 0; i < length; i++)
			{
				wlRegBB(netdev, WL_GET,  offset+i, &val);
				if(i %16 ==0)
				{
					seq_printf(s, "\n%04x:",(int)(offset_display+i));
				}
				seq_printf(s, " %02x",(int)val);

			}
		}
	}
	else
	{
		for(j=dm_p->id-2; j<dm_p->id; j++)
		{
			//seq_printf(s, "\n%s\n",rf[j].name);
			seq_printf(s, "\n");
			offset = rf[j].base + rf[j].start;
			length = rf[j].end - rf[j].start +1;
			offset_display = rf[j].start;
			for(i = 0; i < length; i++)
			{
				wlRegRF(netdev, WL_GET,  offset+i, &val);

				if(i %16 ==0)
				{
					seq_printf(s, "\n%04x:",(int)(offset_display+i));
				}
				seq_printf(s, " %02x",(int)val);	
			}
			if(RF_SETS == 1)
				break;
		}
	}
	seq_printf(s, "\n");

	//seq_printf(s, "%Ld\n", *spos);
	return 0;
}

/**
 * This structure gather "function" to manage the sequence
 *
 */
static struct seq_operations ap8x_dump_seq_ops = {
	.start = ap8x_dump_seq_start,
	.next  = ap8x_dump_seq_next,
	.stop  = ap8x_dump_seq_stop,
	.show  = ap8x_dump_seq_show
};

/**
 * This function is called when the /proc file is open.
 *
 */
static int ap8x_dump_open(struct inode *inode, struct file *file)
{
	//return single_open(file, ap8x_dump_seq_show,NULL);
	int result;
	struct seq_file *s;
	result = seq_open(file, &ap8x_dump_seq_ops);
	s = (struct seq_file *)file->private_data;
#if LINUX_VERSION_CODE >=KERNEL_VERSION(3,10,0)
	s->private = PDE_DATA(inode);
#else
	s->private = PROC_I(inode)->pde->data;
#endif
	return result;
};

/**
 * This structure gather "function" that manage the /proc file
 *
 */
static struct file_operations ap8x_dump_file_ops = {
	.owner   = THIS_MODULE,
	.open    = ap8x_dump_open,
	.read    = seq_read,
	.llseek  = seq_lseek,
	.release = seq_release
};


static int cardid=0;
int ap8x_dump_proc_register(struct net_device *dev)
{
	int i;
	if(cardid >=WL_MAXIMUM_CARDS)
	{
		printk("Error: more than %d cards not supported\n", WL_MAXIMUM_CARDS);
		return 1;
	}
	if (ap8x==NULL)
	{
		ap8x = proc_mkdir("ap8x", proc_net);
		if(!ap8x)
			return 1;
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,30)
		ap8x->owner = THIS_MODULE;
#endif
	}
	for (i = 0; i <TOTAL_SETS; i++)
	{	
		devdump[cardid][i].netdev = dev;
		switch (i)
		{
		case 0:
			sprintf(devdump[cardid][i].filename,  DUMP_BBP_FILE_NAME, dev->name);
			break;
		case 1:
			sprintf(devdump[cardid][i].filename,  DUMP_RFA_FILE_NAME, dev->name);
			break;
		case 2:
			sprintf(devdump[cardid][i].filename,  DUMP_RFB_FILE_NAME, dev->name);
			break;
		case 3:
			sprintf(devdump[cardid][i].filename,  DUMP_RFC_FILE_NAME, dev->name);
			break;
		default:
			sprintf(devdump[cardid][i].filename,  DUMP_RFD_FILE_NAME, dev->name);
			break;
		}
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,10,0)
		devdump[cardid][i].ap8x_dump_proc = proc_create_data(devdump[cardid][i].filename, 0666 , ap8x, &ap8x_dump_file_ops, &devdump[cardid][i]);
		if(!devdump[cardid][i].ap8x_dump_proc )
		{
			printk("create_procfs_file %s failed\n", devdump[cardid][i].filename);
			return 1;
		}
#else
		devdump[cardid][i].ap8x_dump_proc = create_proc_entry(devdump[cardid][i].filename, 0666 , ap8x);
		if(!devdump[cardid][i].ap8x_dump_proc )
		{	
			printk("create_procfs_file %s failed\n", devdump[cardid][i].filename);
			return 1;
		}

		devdump[cardid][i].ap8x_dump_proc->nlink = 1;
		devdump[cardid][i].ap8x_dump_proc->proc_fops = &ap8x_dump_file_ops;
		devdump[cardid][i].ap8x_dump_proc->data = &devdump[cardid][i];
#endif
		devdump[cardid][i].id = 2*i;
	}
	cardid++;
	return 0;
}
int ap8x_dump_proc_unregister(struct net_device *dev)
{
	int i,j;
	for (i = 0; i < WL_MAXIMUM_CARDS; i++)
	{
		if (devdump[i][0].netdev == dev)
			break;
	}
	for (j = 0; j <TOTAL_SETS; j++)
		remove_proc_entry(devdump[i][j].filename, ap8x);
	cardid--;
	return 0;
}
#endif
