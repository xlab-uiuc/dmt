/* Copyright (C) 2010 The Trustees of Indiana University.                  */
/*                                                                         */
/* Use, modification and distribution is subject to the Boost Software     */
/* License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at */
/* http://www.boost.org/LICENSE_1_0.txt)                                   */
/*                                                                         */
/*  Authors: Jeremiah Willcock                                             */
/*           Andrew Lumsdaine                                              */
#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#ifndef __STDC_CONSTANT_MACROS
#define __STDC_CONSTANT_MACROS
#endif
#include "splittable_mrg.h"
#include "graph_generator.h"
#include <stdint.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifdef __MTA__
#include <sys/mta_task.h>
#endif
#ifdef GRAPH_GENERATOR_MPI
#include <mpi.h>
#endif
#ifdef GRAPH_GENERATOR_OMP
#include <omp.h>
#endif
#include "utils.h"

#if defined(_OPENMP)
#define OMP(x_) _Pragma(x_)
#else
#define OMP(x_)
#endif
#include <stdlib.h>

#define ALIGNMET (8UL << 30)

int posix_memalign(void **memptr, size_t alignment, size_t size);


void *
do_xmalloc_large (size_t sz)
{
  void *memptr;
  printf("Allocating %zu MB\n", sz >> 20);
  if (posix_memalign(&memptr, 8UL<<30, sz)) {
    printf("ENOMEM\n");
    exit(1);
  }
  return memptr;
}

void *
xmalloc (size_t sz)
{
  void * out;
  out = do_xmalloc_large (sz);
  if (!out) {
    fprintf(stderr, "Out of memory trying to allocate %zu byte(s)\n", sz);
    abort ();
  }
  memset(out, 0, sz);
  return out;
}

void *
xcalloc (size_t n, size_t sz)
{
  void * out;
  out = do_xmalloc_large ( n * sz);
  if (!out) {
    fprintf(stderr,
	    "Out of memory trying to allocate/clear %zu items of %zu byte(s)\n",
	    n, sz);
    abort ();
  }
  memset(out, 0, n * sz);
  return out;
}
void
xfree (void * p, size_t sz)
{
  free (p);
}


/* Spread the two 64-bit numbers into five nonzero values in the correct
 * range. */
void make_mrg_seed(uint64_t userseed1, uint64_t userseed2, uint_fast32_t* seed) {
  seed[0] = (uint32_t)(userseed1 & UINT32_C(0x3FFFFFFF)) + 1;
  seed[1] = (uint32_t)((userseed1 >> 30) & UINT32_C(0x3FFFFFFF)) + 1;
  seed[2] = (uint32_t)(userseed2 & UINT32_C(0x3FFFFFFF)) + 1;
  seed[3] = (uint32_t)((userseed2 >> 30) & UINT32_C(0x3FFFFFFF)) + 1;
  seed[4] = (uint32_t)((userseed2 >> 60) << 4) + (uint32_t)(userseed1 >> 60) + 1;
}
