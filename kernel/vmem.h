/**
 * Virtual memory management
 */

#ifndef __VMEM_H
#define __VMEM_H

#include "inttypes.h"

/**
 * Generic virtual memory interface
 */

// TODO: what about 64-bit?
typedef uint32 addr_t;


struct pg_dir_ent {
    addr_t       addr  : 20; 
    unsigned int avl   : 4; // available bytes
    unsigned int ps    : 1; // must be 0
    unsigned int avl_2 : 1; // available bit
    unsigned int a     : 1; // accessed
    unsigned int pcd   : 1; // cache disable
    unsigned int pwt   : 1; // write through  caching
    unsigned int us    : 1; // user/supervisor
    unsigned int rw    : 1; // read/write
    unsigned int present : 1; //present
} __attribute__((packed));

struct pg_tbl_ent {
    addr_t       addr  : 20;
    unsigned int avl   : 3;
    unsigned int g     : 1;
    unsigned int pat   : 1;
    unsigned int dirty : 1;
    unsigned int a     : 1;
    unsigned int pcd   : 1;
    unsigned int pwt   : 1;
    unsigned int us    : 1;
    unsigned int rw    : 1;
    unsigned int present     : 1;
} __attribute__((packed));

typedef struct pg_dir_ent pg_dir_ent_t;
typedef struct pg_tbl_ent pg_tbl_ent_t;

void enable_paging();
void disable_paging();

// Must be implemented in assembly
extern void set_page_table_root(addr_t addr);
extern addr_t get_page_table_root();

/**
 * Links the given virtual page number to the given physical page number
 */
int link_page(addr_t vpn, addr_t ppn);

/**
 * Unlinks the given virtual page
 */
int unlink_page(addr_t vpn);

#endif /* __VMEM_H */

