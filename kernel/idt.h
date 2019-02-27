/*
 * idt.h
 */
 
#ifndef _IDT_H
#define _IDT_H

#include "inttypes.h"

#define IDTLOC 0x0
#define INTERRUPT_GATE 	14
#define TRAP_GATE 		15

/* Defined in kernel_entry.asm */
extern void load_idtr(uint16 size, uint32 offset);

struct idt_entry {
	uint16 offset_low;
	uint16 target_selector;
	uint16 access;
	uint16 offset_high;
} __attribute__((packed));


/* Creates an entry for the interrupt descriptor table. */
/* For reference: https://www-s.acm.illinois.edu/sigops/2007/roll_your_own/i386/idt.html */
void idt_create_entry(	struct idt_entry *entry,
						uint32 offset,
						uint16 target_selector,
						byte type,
						byte ring_level);



#endif /* _IDT_H */