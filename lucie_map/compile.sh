#!/bin/bash
CXX=$(which g++)
CXXFLAGS="-O2 -s -mtune=generic"
#CXXFLAGS="-g -O0 -mtune=generic"
CXXSTD="-std=c++11 -pedantic -Werror"

$CXX $CXXSTD $CXXFLAGS -static -static-libstdc++ -static-libgcc -c -fPIC  map.cc -o map.o || exit 1
ar rcs ../libmap.a map.o || exit 1
rm map.o
