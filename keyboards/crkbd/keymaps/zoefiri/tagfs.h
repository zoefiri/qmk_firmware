#pragma once

#include <stdio.h>
#include <stdlib.h>

//   #######################################################################,
//   ########################################################################,
//   ###                                                                   ###
//   ###  available pages & overhead in bytes can be computed via:         ###
//   ###  overhead = (fs_size_bytes / page_size_bytes*8)*2 + (10kb + 16b)  ###
//   ###                           ,;                    |         |       ###
//   ###  pagemaps size -----------' (there's 2 pagemaps)'         |       ###
//   ###  fs metadata size ----------------------------------------'       ###
//   ###                                                                   ###
//   ###  avail_pages = (fs_size_bytes - overhead) / page_size_bytes       ###
//   ###                                                                   ###
//   '########################################################################
//    '#######################################################################

// ## map of page usage for 8 sequential pages ##
typedef char pmap_8;

// ## 32 character alloc for each tag's name ##
typedef char tagname_t[32];

// ## 8 character alloc for each filetype identifier ##
typedef char ftype[8];

// ## dynamic sized data from the fileheader ##
typedef struct {
    uint8_t *data;
} f_data;

// ## metadata for fs ##
// occupies static size of 20
typedef struct {
    uint32_t  next_free_page;     // next free page              #x# 4b
    uint32_t  size;               // amount of pages in fs       #x# 4b
    uint32_t  used;               // amount of pages occupied    #x# 4b

    uint32_t  tagtable_startpage; // page that holds tagfile hdr #x# 4b
    tagname_t tags[256];          // amount of occupied bytes    #x# 32b *  256b = 8kb
    ftype     ftypes[256];        // 0x0 "nil"                   #x# 8b  *  256b = 2kb
                                  //                             #
                                  //                             ####### 10,256b, 10kb + 16b
} fs_meta_t;

// ## pagemap for page usage and inode page locations ##
// notably, this has a size of (fs_size_bytes / page_size_bytes*8)
// and is stored at fs_size_bytes - pagemap_size.
typedef struct {
    pmap_8* usage_map;
    pmap_8* inode_map;
} pmap_t;


// ## inode ##
// -x- static header size is 57b  -x-
typedef struct {
    uint8_t  ftype;     // file type                                        #x# 1b
    uint32_t flen;      // amount of occupied bytes                         #x# 4b
    uint32_t next_page; // 0x0 "nil"                                        #x# 4b
    uint8_t  tags[20];  // these are indexes to the tagname arr in fs_meta  #x# 20b
    char     name[35];  // 35-char filename limit                           #x# 35b
                        //                                                  ######## 64b
} f_header;

// ## file page ##
typedef struct {
    uint32_t next_page; // next occupied page (0x0 "nil")
    uint8_t* data;      // file data
} f_page;

// ## tag_flist ##
typedef struct {
    uint8_t   tag;     // tag index this is for
    uint16_t  tagged;  // number of files tagged with this tag
    uint32_t* files;   // list of file startpages with this tag
} tag_flist;


// ## tag file (it's just a list of tag_flist) ##
typedef tag_flist* tagfile_t;
