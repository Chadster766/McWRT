/*
 * YAFFS: Yet another Flash File System . A NAND-flash specific file system.
 *
 * Copyright (C) 2002-2010 Aleph One Ltd.
 *   for Toby Churchill Ltd and Brightstar Engineering
 *
 * Created by Charles Manning <charles@aleph1.co.uk>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 2.1 as
 * published by the Free Software Foundation.
 *
 * Note: Only YAFFS headers are LGPL, YAFFS C code is covered by GPL.
 */

/* This is used to pack YAFFS2 tags, not YAFFS1tags. */

#ifndef __YAFFS_PACKEDTAGS2_H__
#define __YAFFS_PACKEDTAGS2_H__

#include "yaffs_guts.h"
#include "yaffs_ecc.h"

typedef struct {
	unsigned seq_number;
	unsigned obj_id;
	unsigned chunk_id;
	unsigned n_bytes;
} yaffs_PackedTags2TagsPart;

typedef struct {
	yaffs_PackedTags2TagsPart t;
	yaffs_ECCOther ecc;
} yaffs_PackedTags2;

/* Full packed tags with ECC, used for oob tags */
void yaffs_PackTags2(yaffs_PackedTags2 *pt, const yaffs_ext_tags *t, int tagsECC);
void yaffs_unpack_tags2(yaffs_ext_tags *t, yaffs_PackedTags2 *pt, int tagsECC);

/* Only the tags part (no ECC for use with inband tags */
void yaffs_PackTags2TagsPart(yaffs_PackedTags2TagsPart *pt, const yaffs_ext_tags *t);
void yaffs_unpack_tags2tags_part(yaffs_ext_tags *t, yaffs_PackedTags2TagsPart *pt);
#endif
