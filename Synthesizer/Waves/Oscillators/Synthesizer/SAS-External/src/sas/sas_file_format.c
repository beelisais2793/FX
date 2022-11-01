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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include "sas_file_format.h"
#include "sas_file_msc_format.h"

/* FIXME: the following structure could have been defined in the
   corresponding source file. */

static struct sas_file_format_s msc_format = {
  msc_format_check,
  msc_format_open,
  msc_format_close,
  msc_format_number_of_frames,
  msc_format_get_frame
};

static const sas_file_format_t _sas_file_formats[] = {
  &msc_format
};

const int number_of_sas_file_formats =
  sizeof (_sas_file_formats) / sizeof (_sas_file_formats[0]);

const sas_file_format_t * sas_file_formats = _sas_file_formats;

int
sas_file_format_get_format_number (const char * filename)
{
  int i;

  for (i = 0; i < number_of_sas_file_formats; i++)
    if (sas_file_formats[i]->check (filename))
      return i;

  return -1;
}

