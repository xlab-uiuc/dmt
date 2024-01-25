/* -*- mode: C; mode: folding; fill-column: 70; -*- */
/* Copyright 2010-2011,  Georgia Institute of Technology, USA. */
/* See COPYING for license. */
#include "compat.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <limits.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/mman.h>


#if !defined(MAP_POPULATE)
#define MAP_POPULATE 0
#endif
#if !defined(MAP_NOSYNC)
#define MAP_NOSYNC 0
#endif
/* Unused... for now. */
#if !defined(MAP_HUGETLB)
#define MAP_HUGETLB 0
#endif

extern void *xmalloc (size_t);

#if defined(__MTA__)||defined(USE_MMAP_LARGE)||defined(USE_MMAP_LARGE_EXT)
#define MAX_LARGE 32
static int n_large_alloc = 0;
static struct {
  void * p;
  size_t sz;
  int fd;
} large_alloc[MAX_LARGE];

static int installed_handler = 0;
static void (*old_abort_handler)(int);

static void
exit_handler (void)
{
  int k;
  for (k = 0; k < n_large_alloc; ++k) {
    if (large_alloc[k].p)
      munmap (large_alloc[k].p, large_alloc[k].sz);
    if (large_alloc[k].fd >= 0)
      close (large_alloc[k].fd);
    large_alloc[k].p = NULL;
    large_alloc[k].fd = -1;
  }
}

static void
abort_handler (int passthrough)
{
  exit_handler ();
  if (old_abort_handler) old_abort_handler (passthrough);
}
#endif

#if !defined(MAP_ANONYMOUS) && defined(MAP_ANON)
#define MAP_ANONYMOUS MAP_ANON
#endif

#define ALIGNMET (8UL << 30)

void *
xmalloc_large (size_t sz)
{
  void *memptr;
  printf("Allocating %zu MB\n", sz >> 20);
  if (posix_memalign(&memptr, ALIGNMET, sz)) {
    printf("ENOMEM\n");
    exit(1);
  }
  memset(memptr, 0, sz);
  return memptr;
}

void
xfree_large (void *p)
{
  free (p);
}

void *
xmalloc_large_ext (size_t sz)
{
  return xmalloc_large (sz);
}
