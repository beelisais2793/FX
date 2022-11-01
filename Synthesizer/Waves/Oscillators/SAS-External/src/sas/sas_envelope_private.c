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

#ifndef __SAS_ENVELOPE_PRIVATE_C__
#define __SAS_ENVELOPE_PRIVATE_C__

#include <assert.h>

#include "sas_envelope.h"

/* Some constants used in standard envelopes, like the amplitude
   threshold. */
#define SAS_ENVELOPE_STDSIZE 512
#define SAS_ENVELOPE_STDBASE (SAS_MAX_AUDIBLE_FREQUENCY / SAS_ENVELOPE_STDSIZE)

struct sas_envelope_s {
  double base;
  int size;
  double * data;

  unsigned int refcount;
  /* 0 if the envelope can be deleted, 1 otherwise. */
  int lock;
};

/* This function has some cost and is called many times in several
   parts of the library, especially during synthesis.  Making it
   inline should improve speed. */
static inline double
sas_envelope_get_value_inline (sas_envelope_t e, double frequency)
{
  int i;
  double t, t2, t3;
  double c0, c1, c2, c3;
  double value;

  assert (e);
  assert (frequency > 0);

  t = frequency / e->base;
  i = (int) t;
  t -= i;

  if ((i < 0) || (i > e->size))
    return 0.0;

  t2 = t * t;
  t3 = t * t2;

  c0 = 0.5 * (      -t + 2.0 * t2 -       t3);
  c1 = 0.5 * ( 2.0     - 5.0 * t2 + 3.0 * t3);
  c2 = 0.5 * (       t + 4.0 * t2 - 3.0 * t3);
  c3 = 0.5 * (                -t2 +       t3);

  value =
    c0 * e->data[i - 2] +
    c1 * e->data[i - 1] +
    c2 * e->data[i] +
    c3 * e->data[i + 1];

  if (value < 0.0)
    value = 0.0;

  return value;
}

#endif
