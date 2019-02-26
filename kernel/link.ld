/*
*  link.ld
*/
OUTPUT_FORMAT(pe-i386)
ENTRY(start)
phys = 0x9000;
SECTIONS
{
	.text phys : AT(phys) {
		kernel_entry.obj(.text)
		*(EXCLUDE_FILE (*kernel_entry.obj) .text)
		*(.data)
		*(.rdata)
		*(.rodata)
	}
}

