/*
*  link.ld
*/
OUTPUT_FORMAT(elf32-i386)
phys = 0x9000;
SECTIONS
{
	.text phys : AT(phys) {
		kernel_entry.o(.text)
		*(EXCLUDE_FILE (*kernel_entry.o) .text)
		*(.data)
		*(.rdata)
		*(.rodata)
	}
}

