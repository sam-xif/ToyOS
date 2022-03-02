# Makefile to assemble the bootloader

ASM=nasm

# Flags used for assembling the bootloader
BASMFLAGS=-f bin -o

BOOTASM=boot0.asm

boot.bin: $(BOOTASM)
	$(ASM) $(BASMFLAGS) boot.bin $^
	cp boot.bin ../boot.bin

.PHONY: clean

clean:
	rm -f *.o
	rm -f *.bin
	rm -f kernel.tmp
