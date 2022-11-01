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

#ifndef __SAS_ENVELOPE_H__
#define __SAS_ENVELOPE_H__

#ifdef __cplusplus
extern "C" {
#endif


/* Abstract data type for color and warp envelopes. */
typedef struct sas_envelope_s * sas_envelope_t;

/* Allocates a new envelope which can be shared across SAS frames.
   The envelope has #size points, the first point corresponding to
   frequency 'base'.  The frequency difference between consecutive
   points is also 'base' (linear spectrum).  The values of the
   envelope are copied from 'values', which should contain at least
   #size elements. */
extern sas_envelope_t sas_envelope_make (double base,
					 int size,
					 double * values);

/* Increments the reference counter of an envelope.  Call this
   function when you want to keep an envelope in a SAS frame. */
extern void sas_envelope_keep (sas_envelope_t e);

/* Decrements the reference counter of an envelope, and eventually
   deletes the envelope from memory.  Call this function when an
   envelope is no more needed in a SAS frame. */
extern void sas_envelope_free (sas_envelope_t e);

/* Color envelopes should be adjusted when created, such that
   extremities interpolate well.  This function hides the details of
   the adjustment. */
//extern inline void sas_envelope_adjust_for_color (sas_envelope_t e);
extern void sas_envelope_adjust_for_color (sas_envelope_t e);

/* Warp envelopes should be adjusted when created, such that
   extremities interpolate well.  This function hides the details of
   the adjustment. */
extern void sas_envelope_adjust_for_warp (sas_envelope_t e);

/* Returns the base of an envelope. */
extern double sas_envelope_get_base (sas_envelope_t e);

/* Returns the number of control values of an envelope. */
extern int sas_envelope_get_size (sas_envelope_t e);

/* Returns the value of an envelope at a given frequency, using
   interpolation if the frequency is not a multiple of the base of the
   envelope. */
extern inline double sas_envelope_get_value (sas_envelope_t e,
					     double frequency);

/* Returns an envelope corresponding to the constant color map
   C(f)=0.0. */
extern sas_envelope_t sas_envelope_color_0 (void);

/* Returns an envelope corresponding to the identity warp map
   W(f)=f. */
extern sas_envelope_t sas_envelope_warp_identity (void);

/* Returns an envelope corresponding to the spectrum amplitude
   threshold, that is the minimal audible amplitude value for each
   frequency (for human ears). */
extern sas_envelope_t sas_envelope_amplitude_threshold (void);

#ifdef __cplusplus
}
#endif

#endif
