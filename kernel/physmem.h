/**
 * Physical memory management
 */

#ifndef __PHYSMEM_H
#define __PHYSMEM_H

#include "inttypes.h"

struct e820_entry {
    uint64 base;
    uint64 length;
    uint32 type;
    uint32 attributes;
};

typedef struct e820_entry e820_entry_t;

// TODO: add static methods for managing bitmap
// need to also figure out how much memory there is on the system dynamically

// Allocates a page, and returns a pointer to it
// result will be NULL (0) if failure
void *phys_alloc();

// releases a page
void phys_free(void *ptr);

// Initializes the physical memory manager
// This function will pick an arbitrary page or pages to use to store the bitmap and other information
// The stack starts at 90000h and grows down, so a reasonable address to pick would be 90000h
void phys_init();


#endif
