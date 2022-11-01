/* libsas - library for Structured Additive Synthesis
   Copyright (C) 1999-2001 Sylvain Marchand
   Copyright (C) 2001-2002 SCRIME, université Bordeaux 1

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA */

#ifndef __SKIP_LIST_C__
#define __SKIP_LIST_C__

#include <stdlib.h>
#include <limits.h>
#include <assert.h>
#include <sys/time.h>
#include <unistd.h>

/* Not thread safe. */

typedef int (* compare_fun_t) (const void * e1, const void * e2);

#define SKIP_LIST_MAX_LEVEL 32
#define MAX_POOL_ENTRIES MAX_PARTIALS_PER_SYNTH /* libsas specific. */
#define SIZEOF_POOL (MAX_POOL_ENTRIES * \
		     (sizeof (struct skip_list_cell_s) + \
		      SKIP_LIST_MAX_LEVEL * sizeof (skip_list_cell_t)))

typedef struct skip_list_cell_s * skip_list_cell_t;
struct skip_list_cell_s {
  /* Forward pointers. */
  skip_list_cell_t * fwps;
  /* Backward pointer (not in original skip lists). */
  skip_list_cell_t prev;
  void * data;
};

/* libsas specific: deported into sas_synthesizer.c. */
/*  typedef struct skip_list_s * skip_list_t; */
struct skip_list_s {
  compare_fun_t compare;
  int level;
  skip_list_cell_t header;
  skip_list_cell_t NIL;
  skip_list_cell_t update[SKIP_LIST_MAX_LEVEL];
  void * cell_pool;
  off_t cell_pool_top;
  off_t initial_cell_pool_top;
};

static inline skip_list_cell_t
skip_list_cell_make (skip_list_t sl, int level, void * data)
{
  skip_list_cell_t slc;
  skip_list_cell_t * fwps;

  slc = (skip_list_cell_t) (sl->cell_pool + sl->cell_pool_top);
  sl->cell_pool_top += sizeof (struct skip_list_cell_s);
  fwps = (skip_list_cell_t *) (sl->cell_pool + sl->cell_pool_top);
  sl->cell_pool_top += level * sizeof (skip_list_cell_t);

  if (sl->cell_pool_top >= SIZEOF_POOL)
    {
      fprintf (stderr, "fatal: size of pool of skip list cells exceeded.\n");
      exit (EXIT_FAILURE);
    }

  slc->fwps = fwps;
  slc->data = data;

  return slc;
}

static inline void
skip_list_cell_free (skip_list_t sl, skip_list_cell_t slc)
{
}

static inline skip_list_t
skip_list_make (compare_fun_t compare)
{
  struct timeval tv;
  skip_list_t sl;
  int i;

  sl = (skip_list_t) malloc (sizeof (struct skip_list_s));
  assert (sl);

  sl->compare = compare;
  sl->level = 1;

  sl->cell_pool = (void *) malloc (SIZEOF_POOL);
  assert (sl->cell_pool);

  sl->cell_pool_top = 0;

  sl->header = skip_list_cell_make (sl, SKIP_LIST_MAX_LEVEL, NULL);
  sl->NIL = skip_list_cell_make (sl, 0, NULL);

  /* Considering pool after insertion of header and NIL. */
  sl->initial_cell_pool_top = sl->cell_pool_top;

  for (i = 0; i < SKIP_LIST_MAX_LEVEL; i++)
    {
      sl->header->fwps[i] = sl->NIL;
      sl->update[i] = sl->header;
    }

  sl->NIL->prev = sl->header;

  /* Changing random seed. */
  gettimeofday (&tv, NULL);
  srandom (tv.tv_sec);

  return sl;
}

static inline void
skip_list_free (skip_list_t sl)
{
  assert (sl);

  free (sl->cell_pool);
  free (sl);
}

static inline void
skip_list_reset (skip_list_t sl)
{
  int i;

  assert (sl);

  sl->level = 1;

  for (i = 0; i < SKIP_LIST_MAX_LEVEL; i++)
    {
      sl->header->fwps[i] = sl->NIL;
      sl->update[i] = sl->header;
    }

  sl->NIL->prev = sl->header;

  sl->cell_pool_top = sl->initial_cell_pool_top;
}

/* libsas specific: specializing compare function for partial masking. */

static inline int
skip_list_wrap_compare (skip_list_t sl, void * e1, skip_list_cell_t slc)
{
  return (slc == sl->NIL) ? INT_MIN : compare_frequencies (e1, slc->data);
}

/* Original version. */

/*  static inline int */
/*  skip_list_wrap_compare (skip_list_t sl, void * e1, skip_list_cell_t slc) */
/*  { */
/*    return (slc == sl->NIL) ? INT_MIN : sl->compare (e1, slc->data); */
/*  } */

static inline void *
skip_list_search (skip_list_t sl, void * data)
{
  skip_list_cell_t slc;
  skip_list_cell_t last_search_cell;
  int i;

  assert (sl);

  last_search_cell = sl->update[0]->fwps[0];
  if (skip_list_wrap_compare (sl, data, last_search_cell) == 0)
    return last_search_cell->data;

  slc = sl->header;

  for (i = sl->level - 1; i >= 0; i--)
    {
      while (skip_list_wrap_compare (sl, data, slc->fwps[i]) > 0)
	slc = slc->fwps[i];

      sl->update[i] = slc;
    }

  slc = slc->fwps[0];

  return (skip_list_wrap_compare (sl, data, slc) == 0) ?
    slc->data : NULL;
}

static inline int
random_level (void)
{
  static unsigned int random_bits = 0;
  static unsigned int bits_left = 0;

  register int level;
  register int b;

  level = 1;

  do
    {
      if (bits_left == 0)
	{
	  random_bits = random ();
	  bits_left = sizeof (random_bits);
	};

      b = random_bits & 1;

      if (!b)
	level++;

      random_bits >>= 1;
      bits_left--;
    }
  while (!b);

  return (level <= SKIP_LIST_MAX_LEVEL) ?
    level : SKIP_LIST_MAX_LEVEL;
};

static inline void
skip_list_insert (skip_list_t sl, void * data)
{
  skip_list_cell_t slc;
  int level;
  int i;

  if (skip_list_search (sl, data) != NULL)
    /* libsas specific: allow partials with same frequencies, so don't
       return. */
    ;

  level = random_level ();

  if (sl->level < level)
    {
      for (i = sl->level; i < level; i++)
	sl->update[i] = sl->header;
      sl->level = level;
    }

  slc = skip_list_cell_make (sl, level, data);

  slc->prev = sl->update[0]->fwps[0]->prev;
  sl->update[0]->fwps[0]->prev = slc;

  for (i = 0; i < level; i++)
    {
      slc->fwps[i] = sl->update[i]->fwps[i];
      sl->update[i]->fwps[i] = slc;
    }
}

static inline void
skip_list_remove (skip_list_t sl, void * data)
{
  skip_list_cell_t slc;
  int i;

  /* libsas specific: we don't need to search. */

/*    if (skip_list_search (sl, data) == NULL) */
/*      return; */

  slc = sl->update[0]->fwps[0];

  slc->fwps[0]->prev = slc->prev;

  for (i = 0; i < sl->level; i++)
    {
      if (sl->update[i]->fwps[i] != slc)
	break;
      sl->update[i]->fwps[i] = slc->fwps[i];
    }

  skip_list_cell_free (sl, slc);

  while ((sl->level > 0) && (sl->header->fwps[sl->level - 1] == sl->NIL))
    sl->level--;
}

static inline void *
skip_list_previous (skip_list_t sl, void * data)
{
  skip_list_cell_t slc;

  /* libsas specific: we don't need to search. */

/*    if (skip_list_search (sl, data) == NULL) */
/*      return NULL; */

  slc = sl->update[0]->fwps[0]->prev;

  return (slc == sl->header) ? NULL : slc->data;
}

static inline void *
skip_list_next (skip_list_t sl, void * data)
{
  skip_list_cell_t slc;

  /* libsas specific: we don't need to search. */

/*    if (skip_list_search (sl, data) == NULL) */
/*      return NULL; */

  slc = sl->update[0]->fwps[0]->fwps[0];

  return (slc == sl->NIL) ? NULL : slc->data;
}

#endif
