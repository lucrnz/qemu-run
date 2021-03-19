CC=gcc
CSTD=-std=c99 -pedantic -Wall -Werror -Wextra
CFLAGS=-O2 -s -pipe -fno-plt -mtune=generic
#CFLAGS=-g -O0 -mtune=generic -fsanitize=address,leak

all: build
.PHONY: build

build:
	${CC} ${CSTD} ${CFLAGS} qemu-run.c -o qemu-run.bin

clean:
	rm -rv *.bin

