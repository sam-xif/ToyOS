/*
 * idt.c
 */

#include "inttype.h"
#include "idt.h"

void idt_create_entry(	struct idt_entry *entry,
						uint32 offset,
						uint16 target_selector,
						byte type,
						byte ring_level)
{
	entry->offset_low = offset & 0x0000ffff;
	entry->target_selector = target_selector;
	
	uint16 access = 1; /* equal 1 to set the present bit, which we shift 7 to the left in the next line */
	access <<= 7;
	access |= type & 0x0f;
	access |= ring_level << 5;
	entry->access = 0;
	entry->access |= access << 8;
	
	entry->offset_high = (offset & 0xffff0000) >> 16;
	
	return;
}