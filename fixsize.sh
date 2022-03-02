#!/bin/bash

export SIZE=$(stat -f%z os.img)

dd if=/dev/zero of=os.img bs=1 count=$((1474560 - $SIZE)) seek=$SIZE
