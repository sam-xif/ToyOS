/*
 * interrupts.c
 */

#include "kmain.h"
#include "interrupts.h"

void divide_by_zero_fault(struct regs r, unsigned int eip, unsigned int cs, unsigned int eflags)
{
	const char *err = "divide by zero";
	
	knewline(&vidptr);
	kprint(&vidptr, err, LIGHTGREY);
}


void timer_interrupt(struct regs r)
{
	// TODO: Execute timer-related operations here
    /*const char *msg = "timer interrupt fired";
    knewline(&vidptr);
    kprint(&vidptr, msg, LIGHTGREY);*/
}

void kybrd_interrupt(struct regs r)
{    
    // Read from keyboard encoder
    kybrd_ctrl_wait_read();
    byte enc_return_code = kybrd_enc_read();
    kprintn(&vidptr, (const char *)enc_return_code, 1, LIGHTGREY);
}