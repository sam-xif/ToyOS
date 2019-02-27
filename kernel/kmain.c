/* 
 * kmain.c 
 */

#include "kmain.h"
#include "gdt.h"
#include "idt.h"
#include "interrupts.h"
#include "kybrd_driver.h"

byte *vidptr;


// TODO: Test for multicolor vs monochrome either here or in bootloader

/* Kernel entry point */
void kmain(void)
{
	const char *kernel_loaded = "[STATUS] Kernel loaded at 0x9000";
	const char *load_gdt = " Writing new global descriptor table to 0x0800...";
	const char *done_gdt = "[DONE] Operation completed successfully.";
	
	vidptr = (char*)VIDMEMADDR; 	//video mem begins at 0xb8000 for multicolor. begins at 0xb0000 for monochrome.
	
	// Clear screen
	kcls(&vidptr);
	
	// Print string
	kprintn(&vidptr, kernel_loaded, 8, LIGHTGREEN);
	kprintn(&vidptr, kernel_loaded + 8, kstrlen(kernel_loaded) - 8, LIGHTGREY);
	knewline(&vidptr);
	
	/*
	 * For reference:
	 void gdt_create_entry(		struct gdt_entry *entry,
							uint32 base, 
							uint32 limit, 
							byte ring_level, 
							byte executable,
							byte direction,
							byte read_write_access,
							byte granularity,
							byte size);
	 */
	
	kprintn(&vidptr, kernel_loaded, 8, LIGHTGREEN);
	kprint(&vidptr, load_gdt, LIGHTGREY);
	knewline(&vidptr);
	
	// Load new gdtr
	struct gdt_entry *gdtptr = (struct gdt_entry *)GDTLOC; // 2048 bytes after the start of memory. The first 2048 bytes is occupied by the IDT
	
	struct gdt_entry null_selector;
	*(unsigned long *)(&null_selector) = 0;
	
	struct gdt_entry code_selector;
	struct gdt_entry data_selector;
	
	// Create entries for code segment and data segment
	gdt_create_entry(&code_selector, 0, 0xfffff, 0, 1, 0, 1, 1, 1);
	gdt_create_entry(&data_selector, 0, 0xfffff, 0, 0, 0, 1, 1, 1);
	// TODO: Add entry for TSS and code and data for ring level = 3 (user code)
	
	// Write the first null selector to the GDT
	gdtptr[0] = null_selector;
	gdtptr[1] = code_selector;
	gdtptr[2] = data_selector;
	
	load_gdtr(24, (uint32)gdtptr);
	
	kprintn(&vidptr, done_gdt, 6, LIGHTGREEN);
	kprintn(&vidptr, done_gdt + 6, kstrlen(done_gdt) - 6, LIGHTGREY);
	
	
	// Create the idt and load it to the start of physical memory
	struct idt_entry *idtptr = (struct idt_entry *)IDTLOC;
	
	struct idt_entry divide_by_zero;
	struct idt_entry timer;
	struct idt_entry kybrd;
	
	idt_create_entry(&divide_by_zero, &dbz_wrapper, 0x8, TRAP_GATE, 0);
    // Interrupt gates are in response to hardware events 
    // https://stackoverflow.com/questions/3425085/the-difference-between-call-gate-interrupt-gate-trap-gate
	idt_create_entry(&timer, &timer_interrupt_wrapper, 0x8, TRAP_GATE, 0);
	idt_create_entry(&kybrd, &kybrd_interrupt_wrapper, 0x8, TRAP_GATE, 0);
	
	
	idtptr[0] = divide_by_zero;
	idtptr[32] = timer;
	idtptr[33] = kybrd;
	
	load_idtr(2048, (uint32)idtptr);
	
	// Initialize the PIC microcontroller and enable interrupts
	initialize_pic();
	enable_interrupts();
	
    // Enable the keyboard and try setting LEDs
	kybrd_enable();
	kybrd_set_leds(7);
	
	
	return;
}

/* Clear video memory with space characters */
void kcls(byte **vidptr)
{
    // Reset vid ptr
    *(vidptr) = (char*)VIDMEMADDR;
    
	uint32 j = 0;
	/* this loops clears the screen
	* there are 25 lines each of 80 columns; each element takes 2 bytes */
	while(j < 80 * 25 * 2) {
		/* blank character */
		(*vidptr)[j] = ' ';
		/* attribute-byte - light grey on black screen */
		(*vidptr)[j+1] = LIGHTGREY; 		
		j = j + 2;
	}
    
	return;
}

/* Write string to video memory */
void kprint(byte **vidptr, const char *str, byte color)
{
	uint32 i = 0;
	uint32 j = 0;
	
	/* this loop writes the string to video memory */
	while(str[j] != '\0') {
		/* the character's ascii */
		(*vidptr)[i] = str[j];
		/* attribute-byte: give character black bg and light grey fg */
		(*vidptr)[i+1] = color;
		++j;
		i = i + 2;
	}
	
	// Advance video pointer
	*(vidptr) += kstrlen(str) * 2;
	
	return;
}

/* Write n characters from the pointer str to video memory.
 * WARNING: THIS FUNCTION DOES NOT PERFORM BOUNDS CHECKING ON STR
 *  IT WILL PRINT GARBAGE DATA IF YOU LET IT */
int kprintn(byte **vidptr, const char *str, uint32 num, byte color)
{	
	uint32 i = 0;
	uint32 j = 0;
	
	/* this loop writes the string to video memory */
	while(j < num) {
		/* the character's ascii */
		(*vidptr)[i] = str[j];
		/* attribute-byte: give character black bg and light grey fg */
		(*vidptr)[i+1] = color;
		++j;
		i = i + 2;
	}
	
	// Advance video pointer
	(*vidptr) += num * 2;
	
	// Return number of characters printed
	return num;
}

/* Advance pointer to next line */
void knewline(byte **vidptr)
{
	/* 
	 * Video mem starts at 0xb8000
	 * size of 1 row = 80 * 2, because 80 chars per row and 2 bytes per char.
	 */
	uint32 i = 0;
	
	while ((80 * 2 * i) <= (*vidptr - 0xb8000)) ++i;
	(*vidptr) += (byte *)(80 * 2 * i) - (*vidptr - 0xb8000);
	
	return;
}

/* Get the length of a string */
uint32 kstrlen(const char *str)
{
	uint32 i = 0;
	while (*(str++) != '\0') ++i;
	
	return i;
}
