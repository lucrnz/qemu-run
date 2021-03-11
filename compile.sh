#!/bin/bash
CC=$(which gcc)
CFLAGS="-O2 -s -march=native -mtune=native"
#CFLAGS="-g -O0 -mtune=generic"
CSTD="-std=c99 -pedantic -Werror"

call_cc() {
	$CC $CFLAGS $CSTD $@
}

call_cc -c qemu_run.c -o qemu_run.o

g++ *.o -o qemu_run.bin
