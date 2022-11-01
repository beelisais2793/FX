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

#ifndef __SAS_FRAME_H__
#define __SAS_FRAME_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Abstract data type for SAS frames. */
typedef struct sas_frame_s * sas_frame_t;

#include "sas_envelope.h"

/* Returns a newly allocated SAS frame with amplitude 0,
   frequency 440, constant color 0 and identity warp. */
extern sas_frame_t sas_frame_make (void);

extern void sas_frame_free (sas_frame_t f);

/* Copies amplitude and frequency of 'f' into 'dest', and share the
   color and warp envelopes of 'f' with 'dest'.  Previous envelopes of
   'dest' are freed (see 'sas_envelope_free'). */
extern inline void sas_frame_copy (sas_frame_t dest,
				   sas_frame_t f);

//extern inline void sas_frame_set_amplitude (sas_frame_t f,
extern void sas_frame_set_amplitude (sas_frame_t f,
					    double amplitude);

extern inline double sas_frame_get_amplitude (sas_frame_t f);

//extern inline void sas_frame_set_frequency (sas_frame_t f,
extern void sas_frame_set_frequency (sas_frame_t f,
					    double frequency);

extern inline double sas_frame_get_frequency (sas_frame_t f);

/* Replaces the color envelope of 'f' by 'color'.  Previous color
   envelope of 'f' is freed (see 'sas_envelope_free'). */
//extern inline void sas_frame_set_color (sas_frame_t f,
extern void sas_frame_set_color (sas_frame_t f,
					sas_envelope_t color);

extern inline sas_envelope_t sas_frame_get_color (sas_frame_t f);

/* Replaces the warp envelope of 'f' by 'warp'.  Previous warp
   envelope of 'f' is freed (see 'sas_envelope_free'). */
//extern inline void sas_frame_set_warp (sas_frame_t f,
extern void sas_frame_set_warp (sas_frame_t f,
				       sas_envelope_t warp);

extern inline sas_envelope_t sas_frame_get_warp (sas_frame_t f);

/* Fills 'dest' with the result of morphing 'f1' and 'f2' by
   coefficient 'coeff'.  The coefficient should be a number between 0
   (in which case the morphing results in 'f1') and 1 (in which case
   the morphing results in 'f2'). */
extern void sas_frame_morphing (sas_frame_t dest,
				sas_frame_t f1,
				sas_frame_t f2,
				double coeff);

/* Fills 'dest' with the result of 'f' filtered by 'filter'. */
extern void sas_frame_filter (sas_frame_t dest,
			      sas_frame_t f,
			      sas_frame_t filter);

#ifdef __cplusplus
}
#endif

#endif
