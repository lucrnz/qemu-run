#!/bin/bash
# @TODO: Make an actual Makefile

CC=$(which gcc)
CFLAGS="-O2 -s -pipe -fno-plt -mtune=generic"
#CFLAGS="-g -O0 -mtune=generic -fsanitize=address,leak"
CSTD="-std=c99 -pedantic -Wall -Werror -Wextra"

call_cc() {
	$CC $CFLAGS $CSTD $@
}

call_cc qemu-run.c -o qemu-run.bin
