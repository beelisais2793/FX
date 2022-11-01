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

/* Inspired by the spectral data structure in ProSpect, Copyright (C)
   1997-1999 Sylvain Marchand. */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "sas.h"
#include "sas_common.h"
#include "sas_frame.h"
#include "sas_synthesizer.h"
#include "sas_file_partial.h"
#include "sas_file_spectral.h"

#include "fileio.h"
#include "ieeefloat.h"

struct sas_file_spectral_s {
  const char * filename;
  double rate;
  sas_file_partial_t * tracks;
  int allocated;
  int used;
  int t_min;
  int t_max;
};

static inline sas_file_spectral_t sas_file_spectral_make (double rate, int allocated);
static inline void sas_file_spectral_add (sas_file_spectral_t sp, sas_file_partial_t p);

sas_file_spectral_t
sas_file_spectral_make_from_msc_file (const char * filename)
{
  sas_file_spectral_t sp;
  int i;
  FILE * fp;
  ULONG id;
  LONG size;

  fp = fopen (filename, "rb");
  if (!fp)
    return NULL;

  if ((!IO_Read_BE_ULONG (&id, fp)) ||
      (id != MakeID ('S', 'M', 'S', 'C')) ||
      (!IO_Read_BE_LONG (&size, fp)) ||
      (size <= 0))
    {
      fclose (fp);
      return NULL;
    }

  sp = sas_file_spectral_make (SAS_SAMPLING_RATE / SAS_SAMPLES,
			       size);

  for (i=0; i < size; i++)
    {
      sas_file_partial_t p;

      p = sas_file_partial_load_compressed (MSC_COMPRESSION_RATIO, fp);
      if (!p)
	{
	  sas_file_spectral_free (sp);
	  fclose (fp);
	  return NULL;
	}

      sas_file_spectral_add (sp, p);
    }

  fclose (fp);

  sp->filename = filename;

  return sp;
}

sas_file_spectral_t
sas_file_spectral_make_from_msm_file (const char * filename)
{
  sas_file_spectral_t sp;
  int i;
  FILE * fp;
  ULONG id;
  LONG size;

  fp = fopen (filename, "rb");
  if (!fp)
    return NULL;

  if ((!IO_Read_BE_ULONG (&id, fp)) ||
      (id != MakeID ('S', 'M', 'S', 'F')) ||
      (!IO_Read_BE_LONG (&size, fp)) ||
      (size <= 0))
    {
      fclose (fp);
      return NULL;
    }

  sp = sas_file_spectral_make (SAS_SAMPLING_RATE / SAS_SAMPLES,
			       size);

  for (i=0; i < size; i++)
    {
      sas_file_partial_t p;

      p = sas_file_partial_load (fp);
      if (!p)
	{
	  sas_file_spectral_free (sp);
	  fclose (fp);
	  return NULL;
	}

      sas_file_spectral_add (sp, p);
    }

  fclose (fp);

  sp->filename = filename;

  return sp;
}

void
sas_file_spectral_free (sas_file_spectral_t sp)
{
  int i;

  assert (sp);

  for (i = 0; i < sp->used; i++)
    sas_file_partial_free (sp->tracks[i]);
  free (sp->tracks);
  free (sp);
}

int
sas_file_spectral_number_of_frames (sas_file_spectral_t sp)
{
  return sp->t_max - sp->t_min;
}

#define MAX_NUMBER_OF_PARTIALS 1024

sas_frame_t
sas_file_spectral_get_frame (sas_file_spectral_t sp, sas_frame_t dest, int n)
{
  int length;
  int harmonics;
  double fundamental;
  double amplitudes[MAX_NUMBER_OF_PARTIALS];
  double frequencies[MAX_NUMBER_OF_PARTIALS];
  int coefficients[MAX_NUMBER_OF_PARTIALS];

  length = sp->t_max - sp->t_min;
  assert (n < length);

  /* Find out fundamental and number of harmonics. */
  {
    int t;
    sas_file_partial_t p;
    double f_min, f_max;
    double fp, ip;
    int i;

    t = sp->t_min + n;

    f_min = SAS_MAX_AUDIBLE_FREQUENCY;
    f_max = 0;

    for (i = 0; i < sp->used; i++)
      {
	p = sp->tracks[i];

	if ((p->start <= t) && (t < (p->start + p->length)))
	  {
	    if (p->frequency[t - p->start] < f_min)
	      f_min = p->frequency[t - p->start];

	    if (p->frequency[t - p->start] > f_max)
	      f_max = p->frequency[t - p->start];
	  }
      }

    assert (f_min > 0);

    fp = modf (f_max / f_min, &ip);
    if (fp >= 0.5)
      {
	ip += 1;
	fp -= 1;
      }

    harmonics = (int) ip;
    fundamental = f_min;
  }

  /* Adjust number of harmonics. */
  if (harmonics > MAX_NUMBER_OF_PARTIALS)
    {
      fprintf
	(stderr,
	 "sas_file_spectral_get_frame: %s: too much harmonics in frame %d\n",
	 sp->filename, n);
      harmonics = MAX_NUMBER_OF_PARTIALS;
      /* return NULL; */
    }

  /* Compute amplitudes. */
  {
    int t;
    sas_file_partial_t p;
    REAL frequency, coefficient;
    int i;

    frequency = 0;
    coefficient = 0;

    t = sp->t_min + n;

    /* Zero arrays. */
    for (i = 0; i < harmonics; i++)
      {
	amplitudes[i] = 0.0;
	frequencies[i] = 0.0;
	coefficients[i] = 0;
      }

    /* Scan partials. */
    for (i = 0; i < sp->used; i++)
      {
	p = sp->tracks[i];

	if ((p->start <= t) && (t < (p->start + p->length)))
	  {
	    double fp, ip;
	    int h;

	    fp = modf (p->frequency[t - p->start] / fundamental, &ip);
	    if (fp >= 0.5)
	      {
		ip += 1;
		fp -= 1;
	      }

	    h = (int) ip - 1;

	    assert (h < harmonics);

	    /* On harmonic conflict, keep the highest amplitude partial. */
	    if (amplitudes[h] < p->amplitude[t - p->start])
	      {
		amplitudes[h] = p->amplitude[t - p->start];
		frequency -= frequencies[h];
		coefficient -= coefficients[h];
		frequencies[h] = p->frequency[t - p->start];
		coefficients[h] = ip;
		frequency += frequencies[h];
		coefficient += coefficients[h];
	      }
	  }
      }

    frequency /= coefficient;

    fundamental = frequency;
  }

  /* We may have a new fundamental; should we recompute the amplitudes
     of harmonics?. */

  /* Fill frame. */
  {
    sas_envelope_t C;
    double A;
    int i;

    /* SM idea: use the following formula to determine properly
       perceived amplitude.  Not implemented.

       A = (1/sqrt(2)) * sqrt(sum(a_i^2)) */

    /* A */
    A = 0.0;
    for (i = 0; i < harmonics; i++)
      A += amplitudes[i];
    if (A > 1.0)
      A = 1.0;
    sas_frame_set_amplitude (dest, A);

    /* F */
    sas_frame_set_frequency (dest, fundamental);

    /* C */
    C = sas_envelope_make (fundamental, harmonics, amplitudes);
    sas_envelope_adjust_for_color (C);
    sas_frame_set_color (dest, C);

    /* W */
    /* Keep identity mapping. */
  }

  return dest;
}

static inline sas_file_spectral_t
sas_file_spectral_make (double rate, int allocated)
{
  sas_file_spectral_t sp;

  sp = (sas_file_spectral_t) malloc (sizeof (struct sas_file_spectral_s));

  sp->filename = NULL;
  sp->rate = rate;
  sp->allocated = allocated;
  sp->tracks = (sas_file_partial_t *)
    malloc (allocated * sizeof (sas_file_partial_t));
  assert (sp->tracks);
  sp->used = 0;
  sp->t_min = 0;
  sp->t_max = 0;

  return sp;
}

static inline void
sas_file_spectral_add (sas_file_spectral_t sp, sas_file_partial_t p)
{
  int i;

  for (i = 0; i < p->length; i++)
    {
      if (p->amplitude[i] < 0)
	p->amplitude[i] = 0;
      if (p->frequency[i] <= 0)
	p->frequency[i] = 1; /* Fix it (inherited from ProSpect). */
    }

  if (sp->used >= sp->allocated)
    {
      sp->allocated *= 2;
      sp->tracks = (sas_file_partial_t *)
	realloc (sp->tracks, sp->allocated * sizeof (sas_file_partial_t));
    }

  sp->tracks[sp->used++] = p;

  if (sp->used == 1)
    {
      sp->t_min = p->start;
      sp->t_max = p->start + p->length;
    }
  else
    {
      if (p->start < sp->t_min)
	sp->t_min = p->start;
      if (p->start + p->length > sp->t_max)
	sp->t_max = p->start + p->length;
    }
}

