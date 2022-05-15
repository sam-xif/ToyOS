/*
 * kmain.h
 */


#ifndef _KMAIN_H
#define _KMAIN_H

#include "inttypes.h"

#define LIGHTGREY 	0x07
#define LIGHTGREEN	0x0a

#define VIDMEMADDR	0xb8000
#define KERNELADDR	0x9000

#define CODE_SEGMENT_SELECTOR 0x8
#define DATA_SEGMENT_SELECTOR 0x10

// globsl variable which points to next open character in video memory
extern byte *vidptr;

extern void enable_interrupts(void);

void kmain(void);			/* Kernel entry point */


/* Functions for printing text to video memory */

void kcls(byte **vidptr);	/* Clear video memory */

/* Write string to video memory */
void kprint(byte **vidptr, const char *str, byte color);	

/* Print num chars of str to video memory */
int kprintn(byte **vidptr, const char *str, uint32 num, byte color); 

/* moves the video pointer to the next line */
void knewline(byte **vidptr); 

/* Get length of string. Behavior is only defined for null-terminated strings */
uint32 kstrlen(const char *str); 

// prints bytes as hex
void print_bytes(void *buf, int num);

/* NOT IMPLEMENTED */
int itoa(int value, char *str, int base); /* integer to ASCII */

int nibble_to_hex(byte value, char *str);
int byte_to_hex(byte value, char *str);

#endif /* _KMAIN_H */
