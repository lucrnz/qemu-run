CC ?= gcc
CSTD = -std=c99 -Wall -Wextra

ifdef DEBUG
override CFLAGS ?= -g -Og -D DEBUG -mtune=generic # -fsanitize=address,undefined
else
override CFLAGS ?= -s -Ofast -flto -mtune=native
endif

all: genhashes.bin liblucie.o qemu-run.bin
.PHONY: clean format

liblucie.o:
	${CC} ${CSTD} ${CFLAGS} -c "liblucie/lucie_lib.c" -o liblucie.o

genhashes.bin:
	${CC} ${CFLAGS} -w genhashes.c -o $@
	./$@

qemu-run.bin:
	${CC} ${CSTD} ${CFLAGS} -c qemu-run.c -o qemu-run.o
	${CC} ${CSTD} ${CFLAGS} qemu-run.o liblucie.o -o $@

clean:
	rm -rv *.o || test 1
	rm -rv *.bin || test 1
	rm -rv config.h || test 1

format:
	clang-format -i *.h
	clang-format -i *.c
