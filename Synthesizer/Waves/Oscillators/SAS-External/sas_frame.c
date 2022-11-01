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

#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include "sas_frame.h"
#include "sas_envelope.h"
#include "sas_synthesizer.h"

#include "sas_envelope_private.c"

struct sas_frame_s {
  double amplitude;
  double frequency;
  sas_envelope_t color;
  sas_envelope_t warp;
};

sas_frame_t
sas_frame_make (void)
{
  sas_frame_t f;

  f = (sas_frame_t) malloc (sizeof (struct sas_frame_s));
  assert (f);
  f->amplitude = 0.0;
  f->frequency = 440.0;
  f->color = sas_envelope_color_0 ();
  sas_envelope_keep (f->color);
  f->warp = sas_envelope_warp_identity ();
  sas_envelope_keep (f->warp);

  return f;
}

void
sas_frame_free (sas_frame_t f)
{
  assert (f);
  sas_envelope_free (f->color);
  sas_envelope_free (f->warp);
  free (f);
}

void
sas_frame_copy (sas_frame_t dest, sas_frame_t f)
{
  assert (dest);
  assert (f);

  dest->amplitude = f->amplitude;
  dest->frequency = f->frequency;
  sas_frame_set_color (dest, sas_frame_get_color (f));
  sas_frame_set_warp (dest, sas_frame_get_warp (f));
}

void
sas_frame_set_amplitude (sas_frame_t f, double amplitude)
{
  assert (f);
  f->amplitude = amplitude;
}

double
sas_frame_get_amplitude (sas_frame_t f)
{
  assert (f);
  return f->amplitude;
}

void
sas_frame_set_frequency (sas_frame_t f, double frequency)
{
  assert (f);
  f->frequency = frequency;
}

double
sas_frame_get_frequency (sas_frame_t f)
{
  assert (f);
  return f->frequency;
}

void
sas_frame_set_color (sas_frame_t f, sas_envelope_t e)
{
  assert (f);

  if (f->color != e)
    {
      sas_envelope_free (f->color);
      sas_envelope_keep (e);
      f->color = e;
    }
}

sas_envelope_t
sas_frame_get_color (sas_frame_t f)
{
  assert (f);
  return f->color;
}

void
sas_frame_set_warp (sas_frame_t f, sas_envelope_t e)
{
  assert (f);
  if (f->warp != e)
    {
      sas_envelope_free (f->warp);
      sas_envelope_keep (e);
      f->warp = e;
    }
}

sas_envelope_t
sas_frame_get_warp (sas_frame_t f)
{
  assert (f);
  return f->warp;
}

/* a and b should be positive or null for the following formula to
   work. */
#define MORPH(a,b,x) (pow ((a), 1.0 - (x)) * pow ((b), (x)))

/* a and b should not be positive for the following formula to work,
   but the response is different than the previous one. */
//#define MORPH(a,b,x) (((a) * (1.0 - (x))) + ((b) * (x)))

void
sas_frame_morphing (sas_frame_t dest,
		    sas_frame_t f1,
		    sas_frame_t f2,
		    double coeff)
{
  double cvalues[SAS_ENVELOPE_STDSIZE];
  double wvalues[SAS_ENVELOPE_STDSIZE];
  sas_envelope_t C, C1, C2;
  sas_envelope_t W, W1, W2;
  int i;

  assert (dest);
  assert (f1);
  assert (f2);
  assert (0 <= coeff && coeff <= 1);

  dest->amplitude = MORPH (f1->amplitude, f2->amplitude, coeff);
  dest->frequency = MORPH (f1->frequency, f2->frequency, coeff);

  C1 = sas_frame_get_color (f1);
  W1 = sas_frame_get_warp (f1);
  C2 = sas_frame_get_color (f2);
  W2 = sas_frame_get_warp (f2);

  for (i = 0; i < SAS_ENVELOPE_STDSIZE; i++)
    {
      double frequency;

      frequency = SAS_ENVELOPE_STDBASE * (i + 1);

      cvalues[i] = MORPH (sas_envelope_get_value_inline (C1, frequency),
			  sas_envelope_get_value_inline (C2, frequency),
			  coeff);

      wvalues[i] = MORPH (sas_envelope_get_value_inline (W1, frequency),
			  sas_envelope_get_value_inline (W2, frequency),
			  coeff);
    }

  C = sas_envelope_make (SAS_ENVELOPE_STDBASE, SAS_ENVELOPE_STDSIZE, cvalues);
  W = sas_envelope_make (SAS_ENVELOPE_STDBASE, SAS_ENVELOPE_STDSIZE, wvalues);
  sas_envelope_adjust_for_color (C);
  sas_envelope_adjust_for_warp (W);
  sas_frame_set_color (dest, C);
  sas_frame_set_warp (dest, W);
}

void
sas_frame_filter (sas_frame_t dest, sas_frame_t f, sas_frame_t filter)
{
  double cvalues[SAS_ENVELOPE_STDSIZE];
  sas_envelope_t C, Cf, Cfilter;
  int i;

  assert (dest);
  assert (f);
  assert (filter);

  dest->amplitude = f->amplitude;
  dest->frequency = f->frequency;

  Cf = sas_frame_get_color (f);
  Cfilter = sas_frame_get_color (filter);

  for (i = 0; i < SAS_ENVELOPE_STDSIZE; i++)
    {
      double frequency;

      frequency = SAS_ENVELOPE_STDBASE * (i + 1);

      cvalues[i] = sas_envelope_get_value_inline (Cf, frequency) *
	sas_envelope_get_value_inline (Cfilter, frequency);
    }

  C = sas_envelope_make (SAS_ENVELOPE_STDBASE, SAS_ENVELOPE_STDSIZE, cvalues);
  sas_envelope_adjust_for_color (C);
  sas_frame_set_color (dest, C);

  sas_frame_set_warp (dest, sas_frame_get_warp (f));
}

