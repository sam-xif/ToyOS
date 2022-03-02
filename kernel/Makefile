# Makefile to build the kernel

CC=gcc
LOCAL_CFLAGS=-Wall -m32 -c -fleading-underscore 
ASM=nasm

# Flags used for assembling the kernel
LOCAL_KASMFLAGS=-f elf32

LD=ld
LOCAL_LDFLAGS=-T link.ld -o
LOCAL_OBJCOPYFLAGS=-j .text -O binary
OBJCOPY=objcopy

KENTRY=kernel_entry

# Object files produced by c source
COBJ=kmain.o gdt.o idt.o interrupts.o kybrd_driver.o

# object files produced by assembly
ASMOBJ=$(KENTRY).o exception_wrappers.o initialize_pic.o hardware_int_wrappers.o utils.o

kernel.bin: $(COBJ) $(ASMOBJ) link.ld
	$(LD) $(LOCAL_LDFLAGS) kernel.tmp $(COBJ) $(ASMOBJ)
	$(OBJCOPY) $(LOCAL_OBJCOPYFLAGS) kernel.tmp kernel.bin
	cp kernel.bin ../kernel.bin

#$(KENTRY).o: $(KENTRY).asm
#	$(ASM) $(KASMFLAGS) $(KENTRY).o $(KENTRY).asm

%.o: %.asm
	$(ASM) $(LOCAL_KASMFLAGS) $^
 
%.o: %.c
	$(CC) $(LOCAL_CFLAGS) $^

.PHONY: clean

clean:
	rm -f *.o
	rm -f *.obj 
	rm -f *.bin
	rm -f kernel.tmp
