# Makefile to build the kernel

CC=gcc
CFLAGS=-Wall -m32 -c 
ASM=nasm

# Flags used for assembling the kernel
KASMFLAGS=-f win32


LD=ld
LDFLAGS=-T link.ld -m i386pe -o
OBJCOPYFLAGS=-j .text -O binary
OBJCOPY=objcopy

KENTRY=kernel_entry

# Object files produced by c source
COBJ=kmain.o gdt.o idt.o interrupts.o kybrd_driver.o

# object files produced by assembly
ASMOBJ=$(KENTRY).obj exception_wrappers.obj initialize_pic.obj hardware_int_wrappers.obj utils.obj

kernel.bin: $(COBJ) $(ASMOBJ) link.ld
	$(LD) $(LDFLAGS) kernel.tmp $(COBJ) $(ASMOBJ)
	$(OBJCOPY) $(OBJCOPYFLAGS) kernel.tmp kernel.bin
	cp kernel.bin ../kernel.bin

#$(KENTRY).o: $(KENTRY).asm
#	$(ASM) $(KASMFLAGS) $(KENTRY).o $(KENTRY).asm

%.obj: %.asm
	$(ASM) $(KASMFLAGS) $^

%.o: %.c
	$(CC) $(CFLAGS) $^

.PHONY: clean

clean:
	rm *.o -f
	rm *.obj -f
	rm *.bin -f
	rm kernel.tmp -f