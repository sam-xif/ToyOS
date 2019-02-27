/*
 * gdt.c
 */
 
#include "gdt.h"

/* Fill a gdt_entry struct with the specified values */
void gdt_create_entry(	struct gdt_entry *entry,
						uint32 base, 
						uint32 limit, 
						byte ring_level, 
						byte executable,
						byte direction,
						byte read_write_access,
						byte granularity,
						byte size)
{
	entry->limit_low = limit & 0xffff;
	entry->base_low = base & 0xffff;
	entry->base_mid = base & 0x00ff0000 >> 16;
	
	byte access = 1; /* equal 1 to set the present bit, which we shift 7 to the left in the next line */
	access <<= 7;
	access |= ring_level << 5;
	access |= 1 << 4;
	access |= executable << 3;
	access |= direction << 2;
	access |= read_write_access << 1;
	entry->access = access;
	
	entry->limit_high = (limit & 0x0f0000) >> 16;
	
	byte flags = 0;
	flags |= granularity << 3;
	flags |= size << 2;
	entry->flags = flags & 0x0f;
	
	entry->base_high = (base & 0xff000000) >> 24;
	
	return;
}