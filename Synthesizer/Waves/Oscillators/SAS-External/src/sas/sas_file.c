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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <fcntl.h>

#ifdef _REENTRANT
#include <pthread.h>
#include <errno.h>
#endif

#include "fileio.h"
#include "ieeefloat.h"
#include "sas_common.h"
#include "sas_file.h"
#include "sas_file_format.h"

typedef struct sas_file_cache_cell_s * sas_file_cache_cell_t;
struct sas_file_cache_cell_s {
  /* Handle. */
  sas_file_t f;

  /* File info. */
  char * filename;
  sas_file_format_t format;
  int number_of_frames;

  /* Count of simultaneous accesses on same file. */
  int refcount;

  /* Next cell in the list. */
  sas_file_cache_cell_t next;
};

typedef struct sas_file_cache_s * sas_file_cache_t;
struct sas_file_cache_s {
  /* Simply linked list of cached files. */
  sas_file_cache_cell_t first;

#ifdef _REENTRANT
  /* Lock for concurrent access. */
  pthread_mutex_t lock;
#endif
};

#ifdef _REENTRANT

static struct sas_file_cache_s file_cache_s = {
  NULL, PTHREAD_MUTEX_INITIALIZER
};

#else

static struct sas_file_cache_s file_cache_s = {
  NULL
};

#endif

/* Global cache for SAS files. */
static sas_file_cache_t file_cache = &file_cache_s;

sas_file_t
sas_file_open (const char * filename)
{
  sas_file_cache_cell_t cell;
  int format;
  sas_file_t f;

#ifdef _REENTRANT
  int error;
  error = pthread_mutex_lock (&file_cache->lock);
  assert (error != EDEADLK);
#endif

  cell = file_cache->first;

  while (cell != NULL)
    {
      if (strcmp (cell->filename, filename) == 0)
	{
	  cell->refcount++;
	  f = cell->f;
	  goto open_end;
	}
      cell = cell->next;
    }

  cell = (sas_file_cache_cell_t)
    malloc (sizeof (struct sas_file_cache_cell_s));
  assert (cell);

  cell->filename = (char *)
    malloc ((strlen (filename) + 1) * sizeof (char));
  assert (cell->filename);
  strcpy (cell->filename, filename);

  format = sas_file_format_get_format_number (filename);
  if (format == -1)
    {
      REPORT (fprintf (stderr,
		       "sas_file_open: %s: bad file format\n",
		       filename));
      f = NULL;
      goto open_end;
    }
  cell->format = sas_file_formats[format];

  cell->f = cell->format->open (filename);
  if (!cell->f)
    {
      REPORT (fprintf (stderr,
		       "sas_file_open: %s: can't make handle\n",
		       filename));
      f = NULL;
      goto open_end;
    }

  cell->number_of_frames =
    cell->format->number_of_frames (cell->f);
  cell->refcount = 1;
  cell->next = file_cache->first;
  file_cache->first = cell;

  f = cell->f;

 open_end:
#ifdef _REENTRANT
  error = pthread_mutex_unlock (&file_cache->lock);
  assert (error != EPERM);
#endif

  return f;
}

void
sas_file_close (sas_file_t f)
{
  sas_file_cache_cell_t cell;
  sas_file_cache_cell_t prev;

#ifdef _REENTRANT
  int error;
  error = pthread_mutex_lock (&file_cache->lock);
  assert (error != EDEADLK);
#endif

  prev = NULL;
  cell = file_cache->first;

  while (cell != NULL)
    {
      if (f == cell->f)
	break;
      prev = cell;
      cell = cell->next;
    }

  if (cell == NULL)
    {
      REPORT (fprintf (stderr,
		       "sas_file_close: unknown file handle %p\n",
		       f));
      /* Abort might be better. */
      goto close_end;
    }

  /* Decrement refcount. */
  cell->refcount--;
  if (cell->refcount > 0)
    goto close_end;

  /* Unlink. */
  if (prev)
    prev->next = cell->next;
  else
    file_cache->first = cell->next;

  /* Free cell and file. */
  cell->format->close (cell->f);
  free (cell->filename);
  free (cell);

 close_end:;
#ifdef _REENTRANT
  error = pthread_mutex_unlock (&file_cache->lock);
  assert (error != EPERM);
#endif
}

int
sas_file_number_of_frames (sas_file_t f)
{
  sas_file_cache_cell_t cell;
  int result;

#ifdef _REENTRANT
  int error;
  error = pthread_mutex_lock (&file_cache->lock);
  assert (error != EDEADLK);
#endif

  cell = file_cache->first;

  while (cell != NULL)
    {
      if (cell->f == f)
	break;
      cell = cell->next;
    }

  if (cell == NULL)
    {
      REPORT (fprintf (stderr,
		       "sas_file_number_of_frames: unknown file handle %p\n",
		       f));
      result = -1;
    }
  else
    result = cell->number_of_frames;

#ifdef _REENTRANT
  error = pthread_mutex_unlock (&file_cache->lock);
  assert (error != EPERM);
#endif

  return result;
}

sas_frame_t
sas_file_get_frame (sas_file_t f, sas_frame_t dest, int n)
{
  sas_file_cache_cell_t cell;
  sas_frame_t result;

#ifdef _REENTRANT
  int error;
  error = pthread_mutex_lock (&file_cache->lock);
  assert (error != EDEADLK);
#endif

  cell = file_cache->first;

  while (cell != NULL)
    {
      if (cell->f == f)
	break;
      cell = cell->next;
    }

  if (cell == NULL)
    {
      REPORT (fprintf (stderr,
		       "sas_file_get_frame: unknown file handle %p\n",
		       f));
      result = NULL;
      goto get_frame_end;
    }

  if (n < 0 || n >= cell->number_of_frames)
    {
      REPORT (fprintf (stderr,
		       "sas_file_get_frame: bad frame index %d\n",
		       n));
      result = NULL;
      goto get_frame_end;
    }

  result = cell->format->get_frame (cell->f, dest, n);

 get_frame_end:
#ifdef _REENTRANT
  error = pthread_mutex_unlock (&file_cache->lock);
  assert (error != EPERM);
#endif

  return result;
}

