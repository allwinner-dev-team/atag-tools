/*
 * atagblob.c
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

#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>
#include <stdlib.h>

#include "genatags.h"

int main(int argc, char **argv)
{
    int option_index = 0;
    int c = 0;
    
    u32 core_flags = 0, core_pagesize = 0, core_rootdev = 0;
    u32 mem_size = 0, mem_start = 0;
    u32 initrd_size = 0, initrd_start = 0;
    
    char cmdline[1024] = "";
    char output[1024] = "";
    char *endptr;
    
    genatags *tags;
    FILE* fp;
    
    static struct option long_options[] = {
        {"core-flags",     required_argument, NULL, 'f' },
        {"core-pagesize",  required_argument, NULL, 'p' },
        {"core-rootdev",   required_argument, NULL, 'r' },
        {"mem-size",       required_argument, NULL, 's' },
        {"mem-start",      required_argument, NULL, 't' },
        {"initrd-size",    required_argument, NULL, 'i' },
        {"initrd-start",   required_argument, NULL, 'a' },
        {"cmdline",        required_argument, NULL, 'c' },
        {"output",         required_argument, NULL, 'o' },
        {0, 0, 0, 0 }
    };

    while ((c = getopt_long(argc, argv, "",
        long_options, &option_index)) != -1)
    {
        switch(c)
        {
            case 'f': /* flags */
                core_flags = strtol(optarg, &endptr, 0);
                break;
            case 'p': /* pagesize */
                core_pagesize = strtol(optarg, &endptr, 0);
                break;
            case 'r': /* rootdev */
                core_rootdev = strtol(optarg, &endptr, 0);
                break;
            case 's': /* size */
                mem_size = strtol(optarg, &endptr, 0);
                break;
            case 't': /* start */
                mem_start = strtol(optarg, &endptr, 0);
                break;
            case 'i': /* size */
                initrd_size = strtol(optarg, &endptr, 0);
                break;
            case 'a': /* start */
                initrd_start = strtol(optarg, &endptr, 0);
                break;
            case 'c': /* cmdline */
                strcpy(cmdline, optarg);
                break;
            case 'o': /* output */
                strcpy(output, optarg);
                break;
        }
    }
    
    if(strcmp(output, "") == 0)
    {
        fprintf(stderr, "ERROR: no output file specified\n");
        return 1;
    }
    
    fp = fopen(output, "w");
    if(fp == NULL)
    {
        fprintf(stderr, "ERROR: could not open output file\n");
        return 2;
    }
    
    /* ATAGs start with a core tag */
    tags = genatags_initialize();
    genatags_core(tags, core_flags, core_pagesize, core_rootdev);
    
    if(mem_size != 0 && mem_start != 0)
        genatags_mem(tags, mem_size, mem_start);
    
    if(initrd_size != 0 && initrd_start != 0)
        genatags_initrd(tags, initrd_size, initrd_start);
    
    if(strcmp(cmdline, "") != 0)
        genatags_cmdline(tags, cmdline);
    
    /* and end with a none tag */
    genatags_none(tags);
    
    /* Now write what we've got */
    genatags_dump(tags, fp);
    
    fclose(fp);
    
    return 0;
}
