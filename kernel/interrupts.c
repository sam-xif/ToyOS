/*
 * interrupts.c
 */

#include "kmain.h"
#include "interrupts.h"
#include "kybrd_driver.h"

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
    // TODO: Debug
    char low, high;
    nibble_to_hex(enc_return_code & 0x0f, &low);
    nibble_to_hex((enc_return_code >> 4) & 0x0f, &high); 
    kprintn(&vidptr, &high, 1, LIGHTGREY);
    kprintn(&vidptr, &low, 1, LIGHTGREY);

    // TODO: Need to write the ascii value of the key pressed to some input buffer

}
