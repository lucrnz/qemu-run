FROM ubuntu:xenial
ENV DEBIAN_FRONTEND="noninteractive"
RUN apt-get update && \
	apt-get install --no-install-recommends --yes gcc libc6-dev make && \
	apt-get clean

