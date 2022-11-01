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
#include <assert.h>
#include <errno.h>

#include "sas_common.h"
#include "sas_frame.h"
#include "sas_file.h"
#include "sas_file_msc_format.h"
#include "sas_file_spectral.h"

#include "fileio.h"
#include "ieeefloat.h"

typedef struct cache_data_s * cache_data_t;
struct cache_data_s {
  sas_file_spectral_t sp;
  int number_of_frames;
};

int
msc_format_check (const char * filename)
{
  FILE *fp;
  ULONG id;
  int ret;

  fp = fopen (filename, "rb");

  if (!fp)
    {
      REPORT (perror ("msc_format_check_file: fopen failed"));
      return -1;
    }

  ret = (IO_Read_BE_ULONG (&id, fp) &&
	 (id == MakeID ('S', 'M', 'S', 'C')));

  fclose (fp);

  return ret;
}

sas_file_t
msc_format_open (const char * filename)
{
  sas_file_spectral_t sp;
  cache_data_t cdata;

  sp = sas_file_spectral_make_from_msc_file (filename);
  if (sp == NULL)
    return NULL;

  cdata = (cache_data_t) malloc (sizeof (struct cache_data_s));
  assert (cdata);
  cdata->sp = sp;
  cdata->number_of_frames = sas_file_spectral_number_of_frames (sp);

  return cdata;
}

void
msc_format_close (sas_file_t h)
{
  cache_data_t cdata;

  cdata = (cache_data_t) h;
  sas_file_spectral_free (cdata->sp);
  free (cdata);
}

int
msc_format_number_of_frames (sas_file_t h)
{
  cache_data_t cdata;

  cdata = (cache_data_t) h;
  return cdata->number_of_frames / MSC_COMPRESSION_RATIO;
}

sas_frame_t
msc_format_get_frame (sas_file_t h, sas_frame_t dest, int n)
{
  cache_data_t cdata;

  cdata = (cache_data_t) h;
  return sas_file_spectral_get_frame (cdata->sp,
				      dest,
				      n * MSC_COMPRESSION_RATIO);
}

