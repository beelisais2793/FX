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

#ifndef __SAS_FILE_FORMAT_H__
#define __SAS_FILE_FORMAT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "sas_frame.h"
#include "sas_file.h"

typedef struct sas_file_format_s * sas_file_format_t;

struct sas_file_format_s {
  /* Checks if file matches format. */
  int (* check) (const char * filename);

  /* Opens the file and returns an opaque handle for future
     transactions. */
  sas_file_t (* open) (const char * filename);

  void (* close) (sas_file_t f);

  int (* number_of_frames) (sas_file_t f);

  /* Fills dest with the n-th frame of the file.  Returns dest on
     success, NULL on failure. */
  sas_frame_t (* get_frame) (sas_file_t f, sas_frame_t dest, int n);
};

/* Global number of known file formats for SAS data. */
extern const int number_of_sas_file_formats;

/* Global vector of number_of_sas_file_formats pointers to
   sas_file_format_s structures.  Currently implemented formats:
   ".msc". */
extern const sas_file_format_t * sas_file_formats;

/* Checks filename against known file formats.  Returns the index of
   the corresponding sas_file_format_s structure in sas_file_formats,
   or -1 if none matched. */
extern int sas_file_format_get_format_number (const char * filename);

#ifdef __cplusplus
}
#endif

#endif
