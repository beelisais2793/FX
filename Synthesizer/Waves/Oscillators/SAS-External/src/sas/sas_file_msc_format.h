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

#ifndef __SAS_FILE_MSC_FORMAT_H__
#define __SAS_FILE_MSC_FORMAT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "sas_file.h"
#include "sas_frame.h"

extern int msc_format_check (const char * filename);

extern sas_file_t msc_format_open (const char * filename);

extern void msc_format_close (sas_file_t f);

extern int msc_format_number_of_frames (sas_file_t f);

extern sas_frame_t msc_format_get_frame (sas_file_t f,
					 sas_frame_t dest,
					 int n);

#ifdef __cplusplus
}
#endif

#endif
