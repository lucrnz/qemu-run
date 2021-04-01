CC ?= gcc
CSTD=-std=c99 -Wall -Wextra
CFLAGS_REL=-Ofast -s -flto -mtune=generic
CFLAGS_DBG=-Og -g -D DEBUG -mtune=generic -fsanitize=address,leak
CWD=$(shell pwd)
BINEXT?=bin
OUTBIN?=qemu-run

all: release
.PHONY: genhashes rel_bin dbg_bin clean release debug

lucie_lib.obj:
	CC="${CC}" CFLAGS="${CFLAGS_REL}" OUTDIR="${CWD}" make -C liblucie lucie_lib.obj

lucie_lib_dbg.obj:
	CC="${CC}" CFLAGS="${CFLAGS_DBG}" OUTDIR="${CWD}/liblucie" make -C liblucie lucie_lib.obj
	mv "liblucie/lucie_lib.obj" lucie_lib_dbg.obj

genhashes:
	${CC} ${CFLAGS_REL} -w genhashes.c -o genhashes.${BINEXT}
	chmod -v +x genhashes.${BINEXT}
	./genhashes.${BINEXT}

rel_bin:
	${CC} ${CSTD} ${CFLAGS_REL} -c qemu-run.c -o qemu-run.obj
	${CC} ${CSTD} ${CFLAGS_REL} qemu-run.obj lucie_lib.obj -o ${OUTBIN}.${BINEXT}

dbg_bin:
	${CC} ${CSTD} ${CFLAGS_DBG} -c qemu-run.c -o qemu-run.obj
	${CC} ${CSTD} ${CFLAGS_DBG} qemu-run.obj lucie_lib_dbg.obj -o ${OUTBIN}.${BINEXT}

clean:
	rm -rv *.obj || test 1
	rm -rv *.bin || test 1
	rm -rv *.exe || test 1
	rm -rv config.h || test 1

release: clean genhashes lucie_lib.obj rel_bin clean
debug: clean genhashes lucie_lib_dbg.obj dbg_bin clean
