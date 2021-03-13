#!/bin/bash
CC=$(which gcc)
CFLAGS="-O2 -s -pipe -fno-plt -mtune=generic"
#CFLAGS="-g -O0 -mtune=generic"
CSTD="-std=c99 -pedantic -Werror"

call_cc() {
	$CC $(pkg-config --cflags --libs glib-2.0) $CFLAGS $CSTD $@
}

call_cc qemu-run.c -o qemu-run.bin
