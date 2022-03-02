# Makefile to create the final OS image
SHELL=/bin/bash
CAT=cat
OUTIMG=os.img
MAKE=make
$(OUTIMG): boot.bin kernel.bin
	$(CAT) $^ > $(OUTIMG)
	./fixsize.sh

boot.bin: 
	$(MAKE) -C boot

kernel.bin: 
	$(MAKE) -C kernel
	
.PHONY: clean

.PHONY: clean-all

clean:
	$(MAKE) -C boot clean
	$(MAKE) -C kernel clean
	
clean-all:
	$(MAKE) -C boot clean
	$(MAKE) -C kernel clean
	rm -f *.bin
	rm -f os.img
