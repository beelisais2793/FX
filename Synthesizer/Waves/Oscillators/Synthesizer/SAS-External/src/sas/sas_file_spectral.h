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

#ifndef __SAS_FILE_SPECTRAL_H__
#define __SAS_FILE_SPECTRAL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "sas_frame.h"

typedef struct sas_file_spectral_s * sas_file_spectral_t;

#define MSC_COMPRESSION_RATIO 8

extern sas_file_spectral_t
sas_file_spectral_make_from_msc_file (const char * filename);

extern sas_file_spectral_t
sas_file_spectral_make_from_msm_file (const char * filename);

extern void sas_file_spectral_free (sas_file_spectral_t sp);

extern int sas_file_spectral_number_of_frames (sas_file_spectral_t sp);

extern sas_frame_t sas_file_spectral_get_frame (sas_file_spectral_t sp,
						sas_frame_t dest,
						int n);

#ifdef __cplusplus
}
#endif

#endif
