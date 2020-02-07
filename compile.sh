#!/bin/bash
CC=$(which gcc)
CFLAGS="-O2 -s -march=native -mtune=native"
#CFLAGS="-g -O0 -mtune=generic"
CSTD="-std=c99 -pedantic -Werror"

call_cc() {
	$CC $CSTD $CFLAGS -L"$(pwd)" $@
}

call_cc qemu_run.c -lmap -o qemu_run.bin

