/*
 * genatags.c
 * 
 * Copyright 2012 Emilio López <turl@linux-sunxi.org>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>

#include "genatags.h"
#include "atags.h"
#include "omap_tags.h"

/* TODO: check malloc errors */
genatags *genatags_initialize(void)
{
    /* Allocate a chunk of memory */
    void *mem = malloc(GENATAGS_INITIAL_SIZE);

    /* And our return struct */
    genatags *atags = malloc(sizeof(genatags));

    atags->mem = mem;
    atags->size = GENATAGS_INITIAL_SIZE;
    atags->used = 0;

    return atags;
}

/* TODO: check realloc errors */
struct tag *genatags_add(genatags *atags, int size)
{
    struct tag *ptr;    
    size <<= 2;
    
    if(atags->used + size >= atags->size)
    {
        atags->mem = realloc(atags->mem,
            atags->size + GENATAGS_INITIAL_SIZE);
        atags->size += GENATAGS_INITIAL_SIZE;
    }

    ptr = (struct tag *)((char *)atags->mem + atags->used);
    atags->used += size;
    return ptr;
}

int genatags_dump(genatags *atags, FILE* fp)
{
    return fwrite(atags->mem, 1, atags->used, fp);
}

int genatags_core(genatags *atags, u32 flags, u32 pagesize, u32 rootdev)
{
    struct tag *ptag = genatags_add(atags, tag_size(tag_core));

    ptag->hdr.tag = ATAG_CORE;
    ptag->hdr.size = tag_size(tag_core);

    ptag->u.core.flags = flags;
    ptag->u.core.pagesize = pagesize;
    ptag->u.core.rootdev = rootdev;

    return 0;
}

int genatags_mem(genatags *atags, u32 size, u32 start)
{
    struct tag *ptag = genatags_add(atags, tag_size(tag_mem32));

    ptag->hdr.tag = ATAG_MEM;
    ptag->hdr.size = tag_size(tag_mem32);

    ptag->u.mem.size = size;
    ptag->u.mem.start = start;

    return 0;
}

int genatags_initrd(genatags *atags, u32 size, u32 start)
{
    struct tag *ptag = genatags_add(atags, tag_size(tag_initrd));

    ptag->hdr.tag = ATAG_INITRD2;
    ptag->hdr.size = tag_size(tag_initrd);

    ptag->u.initrd.size = size;
    ptag->u.initrd.start = start;

    return 0;
}

int genatags_cmdline(genatags *atags, char *cmdline)
{
    /* This tag does not match the struct length */
    int length = (strlen(cmdline) >> 2) + 1;
    struct tag *ptag = genatags_add(atags, tag_size(tag_cmdline) + length);

    ptag->hdr.tag = ATAG_CMDLINE;
    ptag->hdr.size = tag_size(tag_cmdline) + length;

    strcpy(ptag->u.cmdline.cmdline, cmdline);

    return 0;
}

int genatags_none(genatags *atags)
{
    struct tag *ptag = genatags_add(atags, sizeof(struct tag_header) >> 2);

    ptag->hdr.tag = ATAG_NONE;
    ptag->hdr.size = 0;

    return 0;
}
