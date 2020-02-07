#!/bin/bash
CC=$(which gcc)
CFLAGS="-O3 -march=native -mtune=native"
#CFLAGS="-g -O0 -mtune=generic"
CSTD="-std=c2x -pedantic -Werror"

$CC $CSTD $CFLAGS qemu_run.c -o qemu_run
