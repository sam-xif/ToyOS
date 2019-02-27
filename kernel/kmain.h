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

/* Returns a pointer to the next available character for writing */
void kprint(byte **vidptr, const char *str, byte color);	/* Write string to video memory */
int kprintn(byte **vidptr, const char *str, uint32 num, byte color); /* Print n chars of string to video memory */
void knewline(byte **vidptr); /* moves the pointer to the next line */
uint32 kstrlen(const char *str); /* Get length of string */

/* NOT IMPLEMENTED */
char * itoa(int value, char *str, int base); /* integer to ASCII */

#endif /* _KMAIN_H */