CC ?= gcc
CSTD=-std=c99 -Wall -Wextra
CFLAGS_REL=-Os -s -mtune=generic
CFLAGS_DBG=-g -O0 -D DEBUG -mtune=generic -fsanitize=address,leak
PWD=$(shell pwd)
DOCKER_CONT_NAME=buildenv-qemu-run

all: release
.PHONY: rm_config_h rm_bin genhashes rel_bin dbg_bin clean release debug docker

rm_config_h:
	rm -rv config.h || test 1

rm_bin:
	rm -rv *.bin || test 1

genhashes:
	${CC} ${CFLAGS_REL} -w genhashes.c -o genhashes.bin
	chmod -v +x genhashes.bin
	./genhashes.bin

rel_bin:
	${CC} ${CSTD} ${CFLAGS_REL} qemu-run.c -o qemu-run.bin

dbg_bin:
	${CC} ${CSTD} ${CFLAGS_DBG} qemu-run.c -o qemu-run.bin

clean: rm_config_h rm_bin
release: clean genhashes rel_bin clean
debug: clean genhashes dbg_bin clean

docker:
	echo -e "FROM ubuntu:xenial" > Dockerfile
	echo -e "ENV DEBIAN_FRONTEND=\"noninteractive\"" >> Dockerfile
	echo -e "RUN apt-get update && \\" >> Dockerfile
	echo -e "\tapt-get install --no-install-recommends --yes gcc libc6-dev make && \\" >> Dockerfile
	echo -e "\tapt-get clean\n" >> Dockerfile
	docker rmi ${DOCKER_CONT_NAME}:latest || test 1
	docker build --force-rm --no-cache -t ${DOCKER_CONT_NAME} .
	docker run -it -v ${PWD}:/app -w /app ${DOCKER_CONT_NAME} make
	docker rmi -f ${DOCKER_CONT_NAME}:latest
	rm -rv Dockerfile || test 1
