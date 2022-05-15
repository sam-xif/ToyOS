/**
 * Defines kernel-level memory management utility
 */

#ifndef __MALLOC_H
#define __MALLOC_H

#include "inttypes.h"

/**
 * Requests a block of memory to be given to the kernel
 */
void *request(uint32 size);

/**
 * Frees an allocated block of memory
 */
void free(void *ptr);

#endif /* __MALLOC_H */
