#!/bin/bash
CXX=$(which g++)
CXXFLAGS="-O2 -s -mtune=generic"
#CXXFLAGS="-g -O0 -mtune=generic"
CXXSTD="-std=c++11 -pedantic -Werror -static"

$CXX $CXXSTD $CXXFLAGS -c -fPIC lucie_map.cc -o lucie_map.o && \
ar rcs liblucie_map.a lucie_map.o
