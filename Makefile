# Makefile to create the final OS image

CAT=cat
OUTIMG=os.img

$(OUTIMG): boot.bin kernel.bin
	$(CAT) $^ > $(OUTIMG)

boot.bin: 
	mingw32-make -C boot

kernel.bin: 
	mingw32-make -C kernel
	
.PHONY: clean

.PHONY: clean-all

clean:
	mingw32-make -C boot clean
	mingw32-make -C kernel clean
	
clean-all:
	mingw32-make -C boot clean
	mingw32-make -C kernel clean
	rm *.bin -f
	rm os.img -f