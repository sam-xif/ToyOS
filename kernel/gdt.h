/*
 * gdt.h
 */
 
 
#ifndef _GDT_H
#define _GDT_H

#include "inttypes.h"

#define GDTLOC 0x800

/* Defined in kernel_entry.asm */
extern void load_gdtr(uint16 size, uint32 offset);

struct gdt_entry {
	uint16 limit_low;
	uint16 base_low;
	byte   base_mid;
	byte   access;
	byte   limit_high:4;
	byte   flags:4;
	byte   base_high;
} __attribute__ ((packed));

/* Creates a GDT entry from the given parameters */
/* For reference: http://wiki.osdev.org/Global_Descriptor_Table */
void gdt_create_entry(		struct gdt_entry *entry,
							uint32 base, 
							uint32 limit, 
							byte ring_level, 
							byte executable,
							byte direction,
							byte read_write_access,
							byte granularity,
							byte size);
							
							
void gdt_setup_tss_selector(uint32 base, uint32 limit);

#endif /* _GDT_H */