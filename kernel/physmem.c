#include "physmem.h"
#include "kmain.h"

void phys_init()
{
    void *base = (void *)0x90000;
    uint32 len = *(uint32 *)base;

    knewline(&vidptr);
    print_bytes(&len, 4);

    e820_entry_t *entries = (e820_entry_t *)(base + 4);

    // swap for bubble sort
    e820_entry_t swap;
    int i;
    for (i = 0; i < len - 1; i++) {
        if (entries[i].base > entries[i + 1].base) {
            swap = entries[i];
            entries[i] = entries[i + 1];
            entries[i + 1] = swap;
            i = 0;
        }
    }

    // process entries...

    // create bitmap
    // need to know how much memory is in the system
}
