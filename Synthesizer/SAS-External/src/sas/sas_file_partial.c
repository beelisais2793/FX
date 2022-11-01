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

/* Inspired by the partial data structure in ProSpect, Copyright (C)
   1997-1999 Sylvain Marchand. */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "sas_file_partial.h"

#include "fileio.h"
#include "ieeefloat.h"

typedef struct vector_s * vector_t;
struct vector_s {
  int n;
  REAL * data;
};

#define REAL_ALLOCATE(n) ((REAL *) malloc ((n) * sizeof (REAL)))
#define VECTOR_VAL(V,I) ((V)->data[I])

static inline sas_file_partial_t
sas_file_partial_make (int birth_time, int allocated)
{
  sas_file_partial_t p;

  assert ((birth_time >= 0) && (allocated >= 0));

  p = (sas_file_partial_t) malloc (sizeof (struct sas_file_partial_s));

  p->start = birth_time;
  p->length = p->allocated = allocated;
  p->frequency = NULL;
  p->amplitude = NULL;
  p->phase = NULL;

  if (p->allocated > 0)
    {
      int i;

      p->frequency = REAL_ALLOCATE (p->allocated);
      p->amplitude = REAL_ALLOCATE (p->allocated);
      p->phase = REAL_ALLOCATE (p->allocated);

      for (i = 0; i < p->allocated; i++)
	{
	  p->frequency[i] = 0;
	  p->amplitude[i] = 0;
	  p->phase[i] = 0;
	}
    }

  return p;
}

void
sas_file_partial_free (sas_file_partial_t p)
{
  assert (p);

  free (p->amplitude);
  free (p->frequency);
  free (p->phase);
  free (p);
}

sas_file_partial_t
sas_file_partial_load (FILE * fp)
{
  sas_file_partial_t p;
  LONG i, start, length;

  assert (fp);

  if ((!IO_Read_BE_LONG (&start, fp)) || (!IO_Read_BE_LONG (&length, fp)))
    return NULL;

  p = sas_file_partial_make (start, length);

  for (i = 0; i < p->length; i++)
    {
      BYTE data[kExtendedLength];

      if (!IO_Read_Str (data, kExtendedLength, fp))
	{
	  sas_file_partial_free (p);
	  return NULL;
	}

      p->frequency[i] = ConvertFromIeeeExtended (data);

      if (!IO_Read_Str (data, kExtendedLength, fp))
	{
	  sas_file_partial_free (p);
	  return NULL;
	}

      p->amplitude[i] = ConvertFromIeeeExtended (data);
    }

  return p;
}

/*-- COMPRESSION --*/

/* Normal high quality (taken from ProSpect). */
static inline int read_compressed_array (int ratio, FILE * fp, REAL * v, int n);

/* Misc filter functions. */
static inline int vlq_read_short (FILE * fp, short * v);
static inline void filter_interp8 (REAL * dst, int dst_size, REAL * src, int src_size);
static inline void filter_reduce8 (REAL * data, int size);
static inline void interp_filter (REAL * dst, int dst_size, REAL * src, int src_size, vector_t filter);
static inline void filter_matlab (REAL * data, int size, vector_t filter);
static inline void FIR_filter_center_preserve_boundaries (REAL * data, int size, vector_t zeros);
static inline void FIR_filter_center_translation (REAL * data, int size, vector_t zeros);

sas_file_partial_t
sas_file_partial_load_compressed (int ratio, FILE * fp)
{
  sas_file_partial_t p;
  LONG start, length;

  assert (fp);

  if ((!IO_Read_BE_LONG (&start, fp)) ||
      (!IO_Read_BE_LONG (&length, fp)))
    return NULL;

  p = sas_file_partial_make (start, length);

  if ((!read_compressed_array (ratio, fp, p->frequency, length)) ||
      (!read_compressed_array (ratio, fp, p->amplitude, length)))
    {
      sas_file_partial_free (p);
      return NULL;
    }

  return p;
}

static inline int
read_compressed_array (int ratio, FILE * fp, REAL * v, int n)
{
  int i;
  BYTE data[kExtendedLength];
  double base;
  double delta;
  short previous = 0;

  /* Decrunching. */

  if (!IO_Read_Str (data, kExtendedLength, fp))
    return 0;
  base = ConvertFromIeeeExtended (data);

  if (!IO_Read_Str (data, kExtendedLength, fp))
    return 0;
  delta = ConvertFromIeeeExtended (data);

  if ((1 + (n - 1) / ratio) >= (2 * 4 + 1))
    {
      /* Use interpolating filter. */
      REAL * sv;

      sv = REAL_ALLOCATE ( 1 + (n - 1) / ratio);

      for (i = 0; i < n; i += ratio)
        {
          short s;

          if (!vlq_read_short (fp, &s))
	    return 0;

          s += previous;

          previous = s;

          sv[i / ratio] = base + (s * delta) / 16383.0;
        }

      /* Resampling. */

      filter_interp8 (v, ((n - 1) / ratio) * ratio + 1,
		      sv, 1 + (n - 1) / ratio);

      /* Read remaining (n % ratio) values as they are... */

      for (i = i - ratio + 1; i < n; i++)
        {
          short s;

          if (!vlq_read_short (fp, &s))
	    return 0;

          s += previous;

          previous = s;

          v[i] = base + (s * delta) / 16383.0;
        }

      free (sv);
    }
  else
    {
      /* Use smoothing filter. */

      for (i = 0; i < n; i += ratio)
        {
          int j;
          short s;
          double value;

          if (!vlq_read_short (fp, &s))
	    return 0;

          s += previous;

          previous = s;

          value = (s * delta) / 16383.0;

          for (j = 0; (((i + j) < n) && (j < ratio)); j++)
	    v[i + j] = value;
        }

      /* Read remaining (n % ratio) values as they are... */

      for (i = i - ratio + 1; i < n; i++)
        {
          short s;

          if (!vlq_read_short (fp, &s))
	    return 0;

          s += previous;

          previous = s;

          v[i] = (s * delta) / 16383.0;
        }

      /* Resampling. */

      filter_reduce8 (v, n);

      for (i = 0; i < n; i++)
	v[i] += base;
    }

  for (i = 0; i < n; i++)
    if (v[i] < 0)
      v[i] = 0;

  return 1;
}

static inline int
vlq_read_short (FILE * fp, short * v)
{
  UBYTE c;
  short s;

  if (!IO_Read_UBYTE (&c, fp))
    return 0;

  s = c & 0x7f;

  if (c & 0x80)
    {
      *v = s - 64;
      return 1;
    }

  if (!IO_Read_UBYTE (&c, fp))
    return false;

  s = (s << 8) | (c & 0xff);

  *v = s - 16384;

  return 1;
}

#include "sas_file_partial_filter_data.c"

static struct vector_s s_filter_reduce8 = {129, s_filter_reduce8_data};
static struct vector_s s_filter_interp8 = {65, s_filter_interp8_data};

static inline void
filter_reduce8 (REAL * data, int size)
{
  FIR_filter_center_preserve_boundaries (data, size, &s_filter_reduce8);
}

static inline void
filter_interp8 (REAL * dst, int dst_size, REAL * src, int src_size)
{
  assert ( ((dst_size-1) % (src_size-1)) == 0 );
  assert ( ((dst_size-1) / (src_size-1)) == 8 );

  interp_filter (dst, dst_size, src, src_size, &s_filter_interp8);
}

static inline void
interp_filter (REAL * dst,
	       int dst_size,
	       REAL * src,
	       int src_size,
	       vector_t filter)
{
  int i;
  int r;
  int l;
  double alpha;
  int order, gap, size;
  REAL * odata, * od;

  assert (dst && (dst_size > 1));
  assert (src && (src_size > 1));

  assert (((dst_size - 1) % (src_size - 1)) == 0);

  /* Design filter. */

  r = (dst_size - 1) / (src_size - 1);
  l = 4;
  alpha = 0.5;

  if (r == 1)
    {
      for (i = 0; i < dst_size; i++)
	*dst++ = *src++;
      return;
    }

  order = 2 * l * r + 1;
  gap = order - 1;
  size = dst_size - 1 + r;

  assert ((2 * l + 1) <= src_size);

  /* Apply filter. */

  odata = REAL_ALLOCATE (gap + size + gap);
  od = REAL_ALLOCATE (gap + 2 * l * r + gap);

  for (i = 0; i < gap + size + gap; i++)
    odata[i] = 0;

  for (i = 0; i < src_size; i++)
    odata[gap + i * r] = src[i];

  for (i = 0; i < gap + 2 * l * r + gap; i++)
    od[i] = 0;

  for (i = 0; i < 2 * l; i++)
    od[gap + i * r] = 2 * src[0] - src[2 * l - i];

  filter_matlab (od, 2 * l * r, filter);

  for (i = 0; i < gap; i++)
    odata[i] = od[gap + 2 * l * r + i];

  filter_matlab (odata, size, filter);

  for (i = 0; i < (src_size - l) * r; i++)
    odata[gap + i] = odata[gap + l * r + i];

  for (i = 0; i < gap; i++)
    od[i] = odata[gap + size + i];

  for (i = 0; i < 2 * l * r; i++)
    od[gap + i] = 0;

  for (i = 0; i < 2 * l; i++)
    od[gap + i * r] = 2 * src[src_size - 1] - src[src_size - 2 - i];

  filter_matlab (od, 2 * l * r, filter);

  for (i = 0; i < l * r; i++)
    odata[gap + size - l * r + i] = od[gap + i];

  for (i = 0; i < dst_size; i++)
    dst[i] = odata[gap + i];

  free (odata);
  free (od);
}

static inline void
filter_matlab (REAL * data, int size, vector_t filter)
{
  int i;
  int order, gap;
  REAL * d;

  assert (data && (size > 0) && filter);

  order = filter->n;
  gap = order - 1;

  d = REAL_ALLOCATE (gap + size + gap);

  for (i = 0; i < gap; i++)
    d[i] = 0;

  for (i = 0; i < size; i++)
    d[gap + i] = data[gap + i];

  for (i = 0; i < gap; i++)
    d[gap + size + i] = 0;

  for (i = gap; i < gap + size + gap; i++)
    {
      int j;

      data[i] = 0;

      for (j = 0; j < order; j++)
	{
	  data[i] += d[i - j] * VECTOR_VAL (filter, j);
	}
    }

  /* Initial conditions. */
  for (i = 0; i < gap; i++)
    data[gap + i] += data[i];

  free (d);
}

static inline void
FIR_filter_center_preserve_boundaries (REAL * data, int size, vector_t zeros)
{
  int i;
  int gap;
  REAL * odata, * od;

  assert (data && (size > 1));
  assert (zeros && (zeros->n % 2));

  if (size < zeros->n)
    {
      FIR_filter_center_translation (data, size, zeros);
      return;
    }

  gap = zeros->n - 1;

  odata = REAL_ALLOCATE (gap + size + gap);

  for (i = 0; i < gap;  i++)
    odata[i] = 0;

  for (i = 0; i < size; i++)
    odata[gap + i] = data[i];

  for (i = 0; i < gap;  i++)
    odata[gap + size + i] = 0;

  od = REAL_ALLOCATE (gap + gap + gap);

  for (i = 0; i < gap; i++)
    od[i] = 0;

  for (i = 0; i < gap; i++)
    od[gap + i] = 2 * data[0] - data[gap - i];

  for (i = 0; i < gap; i++)
    od[gap + gap + i] = 0;

  filter_matlab (od, gap, zeros);

  for (i = 0; i < gap; i++)
    odata[i] = od[gap + gap + i];

  filter_matlab (odata, size, zeros);

  for (i = 0; i < size - gap / 2; i++)
    odata[gap + i] = odata[gap + gap / 2 + i];

  for (i = 0; i < gap; i++)
    od[i] = odata[gap + size + i];

  for (i = 0; i < gap; i++)
    od[gap + i] = 2 * data[size - 1] - data[size - 2 - i];

  filter_matlab (od, gap, zeros);

  for (i = 0; i < gap / 2; i++)
    odata[gap + size - gap / 2 + i] = od[gap + i];

  for (i = 0; i < size; i++)
    data[i] = odata[gap + i];

  free (odata);
  free (od);
}

static inline void
FIR_filter_center_translation (REAL * data, int size, vector_t zeros)
{
  int i;
  REAL * tmp1, * tmp2;
  REAL base1, base2;

  assert (data && (size > 0) && zeros);

  assert (zeros->n % 2);  /* Odd filter please! */

  /* Translation method (to match signal values at the boundaries,
     assuming close-to-zero derivatives...). */

  /* First translation: begining of the signal. */

  tmp1 = REAL_ALLOCATE (zeros->n - 1 + size);

  for (i = 0; i < zeros->n / 2; i++)
    tmp1[i] = tmp1[((zeros->n - 1 + size) - 1) - i] = 0;

  base1 = data[0];

  for (i = 0; i < size; i++)
    tmp1[zeros->n / 2 + i] = data[i] - base1;

  /* Second translation: begining of the signal. */

  tmp2 = REAL_ALLOCATE (zeros->n - 1 + size);

  for (i = 0; i < zeros->n / 2; i++)
    tmp2[i] = tmp2[((zeros->n - 1 + size) - 1) - i] = 0;

  base2 = data[size - 1];

  for (i = 0; i < size; i++)
    tmp2[zeros->n / 2 + i] = data[i] - base2;

  /* Average of the two filters. */

  for (i = 0; i < size; i++)
    {
      int k;
      REAL y = 0;

      for (k = - zeros->n / 2; k <= zeros->n / 2; k++)
	y += VECTOR_VAL (zeros, zeros->n / 2 + k) * tmp1[zeros->n / 2 + i + k];

      data[i] = base1 + y;
    }

  for (i = 0; i < size; i++)
    {
      int k;
      REAL y = 0;

      for (k = - zeros->n / 2; k <= zeros->n / 2; k++)
	y += VECTOR_VAL (zeros, zeros->n / 2 + k) * tmp2[zeros->n / 2 + i + k];

      data[i] = (data[i] + (base2 + y)) / 2;
    }

  free (tmp2);
  free (tmp1);
}

