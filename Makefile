PWD=$(shell pwd)
CC=gcc
CSTD=-std=c99 -pedantic -Wall -Werror -Wextra
CFLAGS=-O2 -s -mtune=generic
#CFLAGS=-g -O0 -mtune=generic -fsanitize=address,leak
DOCKER_CONT_NAME=buildenv-qemu-run

all: build
.PHONY: build docker clean

build:
	${CC} ${CSTD} ${CFLAGS} qemu-run.c -o qemu-run.bin

docker:
	docker rmi ${DOCKER_CONT_NAME}:latest || test 1
	docker build --force-rm --no-cache -t ${DOCKER_CONT_NAME} .
	docker run -it -v ${PWD}:/app -w /app ${DOCKER_CONT_NAME} make
	docker rmi -f ${DOCKER_CONT_NAME}:latest

clean:
	rm -rv *.bin
