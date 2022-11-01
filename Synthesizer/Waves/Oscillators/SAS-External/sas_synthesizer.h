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

#ifndef __SAS_SYNTHESIZER_H__
#define __SAS_SYNTHESIZER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "sas_frame.h"

/* The sampling rate at which the temporal signal is output from a SAS
   synthesizer. */
#define SAS_SAMPLING_RATE 44100.0

/* Nyquist's theorem. */
#define SAS_MAX_AUDIBLE_FREQUENCY (SAS_SAMPLING_RATE / 2.0)

/* The number of audio samples computed in a call to
   sas_synthesizer_synthesize, for each output channel (left and
   right). */
#define SAS_SAMPLES 512

/* Abstract data type for SAS synthesizers. */
typedef struct sas_synthesizer_s * sas_synthesizer_t;

/* Abstract data type for sources (or voices) in SAS synthesizers.  A
   source is always associated to an emitted SAS frame and a position
   with regard to the listener.  This represents the current state of
   the source in the synthesizer.

   Note: in the current implementation, a change in the position is
   interpreted as being the listener who moves with regard to the
   source, not the contrary.  This has consequences on how the sound
   coming from the source is synthesized. */
typedef struct sas_source_s * sas_source_t;

/* Concrete data types for 3D positions of sources with regard to the
   listener.  Unit is meter. */
typedef struct sas_position_s * sas_position_t;
struct sas_position_s {
  double x;
  double y;
  double z;
};

/* Type of the callbacks used to update the sources in a synthesizer.
   The client should give such a callback when adding a source to a
   synthesizer (see below).  At synthesis time, the callback is called
   with the address of a frame pointer and the address of a position
   pointer, both set to NULL before the call.  The callback has to set
   those addresses to the new frame and position of the source, as
   fast as possible.  After the callback has returned, if either frame
   or position is NULL, this is interpreted as the client not being
   able to update the source on time; the source is still synthesized
   but its synthesis parameters are not updated.  Otherwise, the new
   frame and position are safely copied into the synthesizer and taken
   care of.  The 'call_data' pointer is the same as the one given at
   creation time of the source.  This can be used by the client to
   pass arbitrary data to the callback, most likely extra data
   bound to the source. */
typedef void (* sas_update_callback_t) (sas_synthesizer_t s,
					sas_source_t source,
					sas_frame_t * frame,
					sas_position_t * pos,
					void * call_data);

/* Allocates a new SAS synthesizer with no source. */
extern sas_synthesizer_t sas_synthesizer_make (void);

/* Deletes a SAS synthesizer from memory, together with all of its
   sources. */
extern void sas_synthesizer_free (sas_synthesizer_t s);

/* Allocates a new source in a synthesizer.  The position argument
   corresponds to the initial position of the source with regard to
   the listener.  The source is initially silent. */
extern sas_source_t sas_synthesizer_source_make (sas_synthesizer_t s,
						 sas_position_t pos,
						 sas_update_callback_t update,
						 void * call_data);

/* Deletes a source from a SAS synthesizer.  The source can't be used
   anymore. */
extern void sas_synthesizer_source_free (sas_synthesizer_t s,
					 sas_source_t source);

/* Calls each source's update callback, and fills 'buffer' with 2 *
   SAS_SAMPLES samples computed by the forward synthesis of the
   sources in the synthesizer.  The left and right channels are
   interleaved in 'buffer'. */
extern void sas_synthesizer_synthesize (sas_synthesizer_t s, double * buffer);

#ifdef __cplusplus
}
#endif

#endif
