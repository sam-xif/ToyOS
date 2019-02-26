/*
 * interrupts.h
 */
 
#ifndef _INTERRUPTS_H 
#define _INTERRUPTS_H

extern void initialize_pic(void);
extern void generate_interrupt(uint32 interrupt);

extern void timer_interrupt_wrapper();
extern void kybrd_interrupt_wrapper();

extern void dbz_wrapper();

struct regs {
   unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;
} __attribute__((packed));

// Define interrupt handling procedures here
void divide_by_zero_fault(struct regs r, unsigned int eip, unsigned int cs, unsigned int eflags);


// IRQs
void timer_interrupt(struct regs r); /* IRQ 0 */
void kybrd_interrupt(struct regs r); /* IRQ 1 */

#endif /* _INTERRUPTS_H */