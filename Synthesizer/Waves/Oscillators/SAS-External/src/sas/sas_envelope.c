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
#include <math.h>
#include <assert.h>

#include "sas_common.h"
#include "sas_envelope.h"
#include "sas_synthesizer.h"

/* Contains the definitions of structure "sas_envelope_s" and inline
   function "sas_envelope_get_value_inline".  To be used in libsas
   only. */
#include "sas_envelope_private.c"

static sas_envelope_t color_0 = NULL;
static sas_envelope_t warp_identity = NULL;
static sas_envelope_t amplitude_threshold = NULL;

sas_envelope_t
sas_envelope_make (double base, int size, double * values)
{
  sas_envelope_t e;
  int i;

  /* Maybe we should keep a pool of envelopes, so that malloc is
     called less often.  Anyway, GNU libc's malloc already does a very
     good job on that. */
  e = (sas_envelope_t) malloc (sizeof (struct sas_envelope_s));
  assert (e);

  e->base = base;
  e->size = size;
  /* For interpolation purposes, data keeps two values on both sides. */
  e->data = (double *) calloc (2 + e->size + 2, sizeof (double));
  assert (e->data);
  e->data += 2;
  for (i = 0; i < e->size; i++)
    e->data[i] = values[i];

  /* The envelope is eligible for deletion, unless it is stored in a
     frame, which will result in incrementing refcount. */
  e->refcount = 0;
  e->lock = 0;

  return e;
}

void
sas_envelope_keep (sas_envelope_t e)
{
  assert (e);
  e->refcount++;

  REPORT (fprintf (stderr,
		   "keeping envelope %p (refcount: %u)\n",
		   e, e->refcount));
}

void
sas_envelope_free (sas_envelope_t e)
{
  assert (e);

  e->refcount--;
  if (e->refcount <= 0 && !e->lock)
    {
      REPORT (fprintf (stderr, "deleting envelope %p from memory\n", e));
      e->data -= 2;
      free (e->data);
      free (e);
    }
  else
    REPORT (fprintf (stderr,
		     "liberating envelope %p (refcount: %u)\n",
		     e, e->refcount));
}

void
sas_envelope_adjust_for_color (sas_envelope_t e)
{
  e->data[-2] = - e->data[0];
  e->data[-1] = 0.0;
  e->data[e->size] = 0.0;
  e->data[e->size + 1] = - e->data[e->size - 1];
}

void
sas_envelope_adjust_for_warp (sas_envelope_t e)
{
  e->data[-2] = - e->data[0];
  e->data[-1] = 0.0;
  e->data[e->size] =
    2.0 * e->data[e->size - 1] - e->data[e->size - 2];
  e->data[e->size + 1] =
    2.0 * e->data[e->size - 1] - e->data[e->size - 3];
}

double
sas_envelope_get_base (sas_envelope_t e)
{
  return e->base;
}

int
sas_envelope_get_size (sas_envelope_t e)
{
  return e->size;
}

double
sas_envelope_get_value (sas_envelope_t e, double frequency)
{
  return sas_envelope_get_value_inline (e, frequency);
}

sas_envelope_t
sas_envelope_color_0 (void)
{
  double values[1];

  if (color_0)
    return color_0;

  values[0] = 0.0;

  color_0 =
    sas_envelope_make (SAS_MAX_AUDIBLE_FREQUENCY, 1, values);
  color_0->lock = 1;
  sas_envelope_adjust_for_color (color_0);

  return color_0;
}

sas_envelope_t
sas_envelope_warp_identity (void)
{
  double values[1];

  if (warp_identity)
    return warp_identity;

  values[0] = SAS_MAX_AUDIBLE_FREQUENCY;

  warp_identity =
    sas_envelope_make (SAS_MAX_AUDIBLE_FREQUENCY, 1, values);
  warp_identity->lock = 1;
  sas_envelope_adjust_for_warp (warp_identity);

  return warp_identity;
}

sas_envelope_t
sas_envelope_amplitude_threshold (void)
{
  int i;
  double values[SAS_ENVELOPE_STDSIZE];

  if (amplitude_threshold)
    return amplitude_threshold;

  for (i = 0; i < SAS_ENVELOPE_STDSIZE; i++)
    {
      double f;
      double x;
      double v;
      double a;

      f = SAS_ENVELOPE_STDBASE * (i + 1);
      x = 0.001 * f;
      v = (3.64 * pow (x, -0.8)) -
	(6.5 * exp (-0.6 * pow (x - 3.3, 2))) +
	(0.001 * pow (x, 4)) - 119.3;
      a = pow (10, v * 0.05);
      values[i] = a;
    }

  amplitude_threshold =
    sas_envelope_make (SAS_ENVELOPE_STDBASE,
		       SAS_ENVELOPE_STDSIZE,
		       values);
  amplitude_threshold->lock = 1;

  /* Warp-like interpolation on both sides of the envelope. */
  sas_envelope_adjust_for_warp (amplitude_threshold);

  return amplitude_threshold;
}

