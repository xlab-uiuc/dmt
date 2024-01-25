# -*- Makefile -*-
# Copyright 2010-2011, Georgia Institute of Technology, USA.
# See COPYING for license.
CC = gcc -flto -fwhole-program
CFLAGS = -g -std=c99 -Wall -O3 -mrdrnd -mno-avx
LDLIBS = -lm -lrt

## Uncomment if you have libnuma and want a numactl-aware malloc.
#LDLIBS += -lnuma
#CPPFLAGS += -DHAVE_LIBNUMA

## Uncomment to use a file-backed mapping for large data.

## Uncomment to build OpenMP examples.
BUILD_OPENMP = Yes
CFLAGS_OPENMP = -fopenmp

## Uncomment to build the MPI code using default compilers, etc.
# BUILD_MPI = Yes