CC ?= gcc
CSTD=-std=c99 -Wall -Wextra
CFLAGS_REL=-Os -s -mtune=generic
CFLAGS_DBG=-Og -g -D DEBUG -mtune=generic -fsanitize=address,leak
PWD=$(shell pwd)
BINEXT?=bin
OUTBIN?=qemu-run
DOCKER_CONT_NAME=buildenv-qemu-run

all: release
.PHONY: genhashes rel_bin dbg_bin clean release debug docker docker_win_bin

genhashes:
	${CC} ${CFLAGS_REL} -w genhashes.c -o genhashes.${BINEXT}
	chmod -v +x genhashes.${BINEXT}
	./genhashes.${BINEXT}

rel_bin:
	${CC} ${CSTD} ${CFLAGS_REL} qemu-run.c -o ${OUTBIN}.${BINEXT}

dbg_bin:
	${CC} ${CSTD} ${CFLAGS_DBG} qemu-run.c -o ${OUTBIN}.${BINEXT}

clean:
	rm -rv *.bin || test 1
	rm -rv *.exe || test 1
	rm -rv config.h || test 1
	rm -rv Dockerfile || test 1

release: clean genhashes rel_bin clean
debug: clean genhashes dbg_bin clean

docker:
	which docker >/dev/null
	rm -rv Dockerfile || test 1
	echo "FROM ubuntu:xenial" > Dockerfile
	echo "ENV DEBIAN_FRONTEND=\"noninteractive\"" >> Dockerfile
	echo "RUN apt-get update && apt-get -y upgrade && \\" >> Dockerfile
	echo "apt-get install --no-install-recommends --yes gcc libc6-dev make && apt-get clean" >> Dockerfile
	docker rmi ${DOCKER_CONT_NAME}:latest || test 1
	docker build --force-rm --no-cache -t ${DOCKER_CONT_NAME} .
	docker run -it -v ${PWD}:/app -w /app ${DOCKER_CONT_NAME} make
	docker rmi -f ${DOCKER_CONT_NAME}:latest
	rm -rv Dockerfile || test 1

docker_win_bin:
	which docker >/dev/null
	rm -rv Dockerfile || test 1
	echo "FROM debian:stable" > Dockerfile
	echo "ENV DEBIAN_FRONTEND=\"noninteractive\"" >> Dockerfile
	echo "RUN apt-get update && apt-get -y upgrade && \\" >> Dockerfile
	echo "apt-get install --no-install-recommends --yes gcc-mingw-w64-i686 gcc-mingw-w64-x86-64 make && apt-get clean" >> Dockerfile
	docker rmi ${DOCKER_CONT_NAME}:latest || test 1
	docker build --force-rm --no-cache -t ${DOCKER_CONT_NAME} .
	make genhashes
	docker run -it -e CC=i686-w64-mingw32-gcc-win32 -e OUTBIN=${OUTBIN}_win32 -e BINEXT=exe -v ${PWD}:/app -w /app ${DOCKER_CONT_NAME} make rel_bin
	docker run -it -e CC=x86_64-w64-mingw32-gcc-win32 -e OUTBIN=${OUTBIN}_win64 -e BINEXT=exe -v ${PWD}:/app -w /app ${DOCKER_CONT_NAME} make rel_bin
	docker rmi -f ${DOCKER_CONT_NAME}:latest
	rm -rv Dockerfile || test 1
