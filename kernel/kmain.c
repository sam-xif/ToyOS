/* 
 * kmain.c 
 */

#include "kmain.h"
#include "gdt.h"
#include "idt.h"
#include "interrupts.h"
#include "kybrd_driver.h"
#include "vmem.h"
#include "physmem.h"

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
   // kybrd_enable();
//    kybrd_set_leds(7);
    
    const char *getting_scanset_msg = "getting keyboard scanset";
    knewline(&vidptr);
    kprint(&vidptr, getting_scanset_msg, LIGHTGREEN);
    knewline(&vidptr);
 
    if (kybrd_disable_scanning() >= 0) {
        knewline(&vidptr);
        kprint(&vidptr, "Disabled scanning?", LIGHTGREEN);
    } else {
        knewline(&vidptr);
        kprint(&vidptr, "Error when trying to disable scanning", LIGHTGREEN);
    }
	   
    char scanset_str[2];
    int scanset_byte = kybrd_get_scanset();
    if (scanset_byte >= 0) {
        knewline(&vidptr);
        kprint(&vidptr, "Byte: ", LIGHTGREEN);
        byte_to_hex((byte)scanset_byte, &scanset_str);
        kprintn(&vidptr, scanset_str, 2, LIGHTGREEN);
    } else if (scanset_byte == -1) {
        kprint(&vidptr, "Resend requested", LIGHTGREEN);
    } else {
        kprint(&vidptr, "Error white getting scanset", LIGHTGREEN);
    }

// TODO: Rennable scanning here
    knewline(&vidptr);
    kprint(&vidptr, "enabling kbd", LIGHTGREY);
    kybrd_enable();


//    knewline(&vidptr);
//    kprint(&vidptr, "enabling scanning", LIGHTGREY);
    kybrd_enable_scanning();
    
    //set_page_table_root(3);
    //addr_t cr3 = get_page_table_root();
    //knewline(&vidptr);
    //print_bytes(&cr3, 4);

    // Initialize physical memory manager
    phys_init();
	return;
}

void print_bytes(void *buf, int num)
{
    byte *bytes = (byte *)buf;
    char str[2];
    int i;
    for (i = 0; i < num; i++) {
        byte_to_hex(bytes[i], str);
        kprintn(&vidptr, str, 2, LIGHTGREY);
    }
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
		(*vidptr)[j] = '\0';//' ';
		/* attribute-byte - light grey on black screen */
		(*vidptr)[j+1] = '\0';//LIGHTGREY; 		
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

    //while ((int)(*vidptr - VIDMEMADDR) % (80 * 2)) (*vidptr)++;
	
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

/* Converts a value from 0-15 to a hexadecimal string */
int nibble_to_hex(byte value, char *str) 
{
    switch (value) {
        case 0: str[0] = '0'; break;
        case 1: str[0] = '1'; break;
        case 2: str[0] = '2'; break;
        case 3: str[0] = '3'; break;
        case 4: str[0] = '4'; break;
        case 5: str[0] = '5'; break;
        case 6: str[0] = '6'; break;
        case 7: str[0] = '7'; break;
        case 8: str[0] = '8'; break;
        case 9: str[0] = '9'; break;
        case 10: str[0] = 'A'; break;
        case 11: str[0] = 'B'; break;
        case 12: str[0] = 'C'; break;
        case 13: str[0] = 'D'; break;
        case 14: str[0] = 'E'; break;
        case 15: str[0] = 'F'; break;
        default: return -1;
    }
    return 0;
}

int hex_to_nibble(char digit)
{
    switch (digit) {
        case '0': return 0;
        case '1': return 1;
        case '2': return 2;
        case '3': return 3;
        case '4': return 4;
        case '5': return 5;
        case '6': return 6;
        case '7': return 7;
        case '8': return 8;
        case '9': return 9;
        case 'a': case 'A': return 10;
        case 'b': case 'B': return 11;
        case 'c': case 'C': return 12;
        case 'd': case 'D': return 13;
        case 'e': case 'E': return 14;
        case 'f': case 'F': return 15;
        default: return -1;
    }
}

/* Converts one byte to its hexadecimal representation */
int byte_to_hex(byte value, char *str)
{
    if (nibble_to_hex(value & 0x0F, str + sizeof(char)) == -1) {
        kprint(&vidptr, "ERROR", LIGHTGREEN); // TODO: Handle errors better
        return -1;
    }
    
    if (nibble_to_hex((value >> 4) & 0x0F, str) == -1) {
        kprint(&vidptr, "ERROR", LIGHTGREEN);
        return -1;
    }
    
    return 0;
}
