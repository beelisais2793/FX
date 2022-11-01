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
#include <math.h>

#include "sas_synthesizer.h"
#include "sas_envelope.h"
#include "sas_frame.h"
#include "sas_synthesizer_statistics.h"

#include "sas_envelope_private.c"

/* Comment out next line when profiling, if you want to disable
   function inlining. */
//#define inline

/* Resonator algorithm: fastest sinusoidal synthesis without tables in
   memory. */
/* FIXME: still produces odd results. */
//#define USE_RESONATOR

#define MIN(x,y) (((y)<(x))?(y):(x))
#define MAX(x,y) (((y)>(x))?(y):(x))
#define SQR(x) ((x) * (x))
#define DISTANCE(x,y,z) (sqrt (SQR (x) + SQR (y) + SQR (z)))

#define MAX_PARTIALS_PER_SOURCE 1024
#define MAX_PARTIALS_PER_SYNTH MAX_PARTIALS_PER_SOURCE * 5

#define INTERPOLATION_STEPS 8
#define STEP_SAMPLES (SAS_SAMPLES / INTERPOLATION_STEPS)

#define FREQCOEFF ((2.0 * M_PI) / SAS_SAMPLING_RATE)

#define MIN_BARK 0.2
#define MAX_BARK 27.0
#define MIN_DB (-100.0) /* 20 * log10 (MIN_AMP) */
#define MIN_AMP 1e-5 /* 10 ^ (MIN_DB / 20) */
#define DB_DIFF 10.0
/* Line coefficients in mask contributions. */
#define LEFT_LINE_COEFF 27.0
#define RIGHT_LINE_COEFF (-15.0)

#define SOUND_CELERITY     350.0 /* m/s */
#define MAX_PROPAGATION_DISTANCE 2000.0 /* m */
#define MAX_PROPAGATED_FRAMES \
  ((int) ((MAX_PROPAGATION_DISTANCE / SOUND_CELERITY) * \
          (SAS_SAMPLING_RATE / SAS_SAMPLES)))

#ifdef USE_RESONATOR
/* Trying to fix the "resonator bug". */
#define BELOW_MIN_AMP (0.1 * MIN_AMP)
#else
#define BELOW_MIN_AMP 0.0
#endif

typedef struct partial_s * partial_t;
typedef struct masking_partial_s * masking_partial_t;
typedef struct pool_of_masking_partials_s * pool_of_masking_partials_t;
typedef struct skip_list_s * skip_list_t;

struct sas_synthesizer_s {
  /* Simply linked list of sources. */
  sas_source_t sources;
  int number_of_sources;
  /* 1 / (log_2(number of sources) + 1). */
  double amplitude_factor;
  /* Pointers to the harmonics currently linked. */
  partial_t * tracks;
  /* Number of partials in the array above. */
  int allocated;
  int active_tracks;
  int audible_tracks;
  int masked_tracks;
  /* Interpolated audibility function. */
  sas_envelope_t threshold;
  /* Partials sorted by decreasing amplitudes (for qsort). */
  partial_t * tracks2;
  /* Spectral mask of partials. */
  skip_list_t mask;
  pool_of_masking_partials_t pool;
};

struct sas_source_s {
  sas_update_callback_t update;
  void * call_data;
  /* MAX_PROPAGATED_FRAMES frames.  (Circular buffer.) */
  sas_frame_t * propagated_frames;
  /* Current emission point in the circular buffer above. */
  int emission_index;
  /* The current position of the source. */
  struct sas_position_s position;
  /* Distance of the source with regard to the listener. */
  double distance;
  /* Cosine of the angle between the listener and the source. */
  double cos_angle;
  /* Doppler factor.  (From the listener point of view.) */
  double doppler;
  /* Left channel amplitude ratio.  (From the listener point of view.)  */
  double l_ratio;
  /* Right channel amplitude ratio.  (From the listener point of view.) */
  double r_ratio;
  /* A pointer to the MAX_PARTIALS_PER_SOURCE harmonics. */
  partial_t tracks;
  /* Number of harmonics considered active. */
  int active_tracks;
  /* Number of harmonics linked into synthesizer. */
  int linked_tracks;
  /* Next source in synthesizer. */
  sas_source_t next;
  /* If the deletion of the source has been requested, but was
     delayed. */
  int delayed_free;
};

struct partial_s {
  /* The source that contains this partial. */
  sas_source_t source;
  /* Link to the corresponding track in synthesizer (if not NULL). */
  partial_t * link;
  /* Future amplitude. */
  double a;
  /* Future frequency. */
  double f;
  /* Interpolation envelope for frequency. */
  double fenv[4];
  /* Interpolation envelope for amplitude. */
  double aenv[4];
  /* Update state of partial. */
  int state;
  /* Synthesis state. */
  double v1, v2;
};

struct masking_partial_s {
  partial_t p;
  /* The minimum of left and right volumes in dB. */
  double min_vdB;
  /* The maximum of Left and right volumes in dB. */
  double max_vdB;
  /* Frequency in Bark. */
  double freqB;
};

struct pool_of_masking_partials_s {
  masking_partial_t partials;
  int allocated;
  int used;
};

/* Coefficients for the interpolation of amplitude and frequency
   during synthesis. */
static double icoeffs[INTERPOLATION_STEPS][4];

/*======================================================================*/
/* Local functions */

/* Compare function for masking partials, used as a callback in skip
   lists. */
static inline int
compare_frequencies (const void * e1, const void * e2)
{
  masking_partial_t mp1;
  masking_partial_t mp2;

  mp1 = (masking_partial_t) e1;
  mp2 = (masking_partial_t) e2;

  /* Masking partials sorted by increasing frequencies. */

  /* Note: should return an integer, so be careful with floating point
     numbers.  This expression is similar to one found in the GNU C
     library documentation. */
  return (mp2->freqB < mp1->freqB) - (mp1->freqB < mp2->freqB);
}

/* Include this here since it needs compare_frequencies.  Inclusion
   needed for optimization (inlining, specialization, etc.). */
#include "skip_list.c"

/* Interpolation of amplitudes and frequencies.  Only called by
   sas_synthesizer_synthesize. */
static inline double
interpolate_value (double * envelope, int step)
{
  return (icoeffs[step][0] * envelope[0] +
	  icoeffs[step][1] * envelope[1] +
	  icoeffs[step][2] * envelope[2] +
	  icoeffs[step][3] * envelope[3]);
}

static inline void
sas_synthesizer_source_delayed_free (sas_synthesizer_t s, sas_source_t source)
{
  int i;
  sas_source_t current, prev;

  current = s->sources;
  prev = NULL;

  while (current != NULL)
    {
      if (current == source)
	break;
      prev = current;
      current = current->next;
    }

  if (current == NULL)
    return;

  if (prev == NULL)
    s->sources = current->next;
  else
    prev->next = current->next;

  for (i = 0; i < source->linked_tracks; i++)
    {
      /* Unlink partials. */
      partial_t p = source->tracks + i;
      *(p->link) = NULL;
    }

  for (i = 0; i < MAX_PROPAGATED_FRAMES; i++)
    sas_frame_free (source->propagated_frames[i]);

  free (source->propagated_frames);
  free (source->tracks);
  free (source);

  s->number_of_sources--;
  if (s->number_of_sources == 0)
    s->amplitude_factor = 0.0;
  else
    s->amplitude_factor =
      1.0 / ((log (s->number_of_sources) * (1.0 / M_LN2)) + 1.0);
}

static inline double
amplitude_threshold (sas_synthesizer_t s, double frequency)
{
  return sas_envelope_get_value_inline (s->threshold, frequency);
}

static inline void
shift_envelope (double * envelope, double value)
{
  envelope[0] = envelope[1];
  envelope[1] = envelope[2];
  envelope[2] = envelope[3];
  envelope[3] = value;
}

#define ALPHA 0.05

static inline void
update_source_spatial_information (sas_source_t source)
{
  double previous_distance;
  double new_cos_angle;
  double new_doppler;
  /* Signed speed for Doppler. */
  double sspeed;
  double pow2cos;

  previous_distance = source->distance;

  source->distance =
    DISTANCE (source->position.x,
	      source->position.y,
	      source->position.z);

  new_cos_angle =
    (source->distance > 0.0) ?
    source->position.x / source->distance :
    0.0;

  /* FIXME: why does this interpolation still produce clicks? */
  source->cos_angle =
    (1.0 - ALPHA) * source->cos_angle +
    ALPHA * new_cos_angle;

  pow2cos = pow (2.0, source->cos_angle);
  source->r_ratio =  0.5 * pow2cos;
  source->l_ratio = 0.5 / pow2cos;

  sspeed = (source->distance - previous_distance) *
    (SAS_SAMPLING_RATE / SAS_SAMPLES);

  new_doppler =
    ((-SOUND_CELERITY <= sspeed) && (sspeed <= SOUND_CELERITY)) ?
    (SOUND_CELERITY - sspeed) / SOUND_CELERITY :
    1.0;

  source->doppler = (1.0 - ALPHA) * source->doppler + ALPHA * new_doppler;
}

/* Returns the amplitude factor of partials with regard to (1) their
   frequency and (2) the distance between their source and the
   listener. */
static inline double
compute_distance_attenuation_factor (partial_t p)
{
  double mu;
  double h;

  h = 50.0;  /* 50% humidity. */

  /* Evans and Bazley.  (Air at 20°C.) */
  mu = (85.0 / h) * SQR (p->f / 1000) * 0.0001 * 8.7;
  return exp (-mu * p->source->distance) / (p->source->distance + 1.0);
}

/* Real update of a source. */
static inline void
update_source (sas_synthesizer_t s, sas_source_t source)
{
  int distance_index;
  sas_frame_t frame;
  sas_position_t pos;
  double frameA;
  double frameF;
  sas_envelope_t frameC;
  sas_envelope_t frameW;
  partial_t p;
  int harmonics;
  double amp;
  int active;
  int birth;
  int death;
  int links;
  int i;

  harmonics = 0;
  amp = 0.0;
  active = 0;

  /* Call client for new frame and position. */

  frame = NULL;
  pos = NULL;

  source->update (s, source, &frame, &pos, source->call_data);

  if (frame == NULL || pos == NULL)
    {
      /* Source not updated.  Freeze the emitted frame. */
      active = source->active_tracks;
      goto after_harmonic_scan;
    }

  /* Safe-copy the updated frame and position. */

  sas_frame_copy (source->propagated_frames[source->emission_index], frame);
  source->position.x = pos->x;
  source->position.y = pos->y;
  source->position.z = pos->z;

  /* Find the frame that the listener hears. */

  update_source_spatial_information (source);

  if (source->distance >= MAX_PROPAGATION_DISTANCE)
    goto after_harmonic_scan;

  distance_index =
    source->emission_index +
    (source->distance * MAX_PROPAGATED_FRAMES / MAX_PROPAGATION_DISTANCE);
  distance_index %= MAX_PROPAGATED_FRAMES;

  frame = source->propagated_frames[distance_index];
  if ((frameA = sas_frame_get_amplitude (frame)) == 0.0)
    goto after_harmonic_scan;
  frameF = sas_frame_get_frequency (frame);
  frameC = sas_frame_get_color (frame);
  frameW = sas_frame_get_warp (frame);

  /* Scan harmonics. */

  for (i = 0, p = source->tracks;
       (frameF * (i + 1) < SAS_MAX_AUDIBLE_FREQUENCY) &&
	 (i < MAX_PARTIALS_PER_SOURCE);
       i++, p++)
    {
      p->f = sas_envelope_get_value_inline (frameW, frameF * (i + 1));

      if (p->f < SAS_MAX_AUDIBLE_FREQUENCY)
	{
	  p->a = sas_envelope_get_value_inline (frameC, p->f);

	  /* For human ears, minimal audible amplitude depends on
	     frequency. */
	  if (p->a * frameA < amplitude_threshold (s, p->f))
	    p->a = BELOW_MIN_AMP;  /* Inaudible harmonic. */
	  else
	    {
	      /* SM's idea: use the following formula to determine
		 properly perceived amplitude.  Not implemented.

		 A = (1/sqrt(2)) * sqrt(sum(a_i^2)). */
	      amp += p->a;
	      harmonics = i + 1;
	    }
	}
    }

  /* Global amplitude factor is normalized with respect to amplitude
     parameter and color amplitudes. */
  amp = (harmonics > 0 && amp > 0.0) ? frameA / amp : 0.0;

  /* SM's proposition: divide by [log2(number of sources) + 1] instead
     of number of sources.  Implemented. */
  amp *= s->amplitude_factor;

  for (i = 0, p = source->tracks; i < harmonics; i++, p++)
    {
      p->a *= amp;
      p->a *= compute_distance_attenuation_factor (p);
      p->f *= source->doppler;
    }

  active = MIN (harmonics, source->active_tracks);

 after_harmonic_scan:

  /* Update tracks (partials). */

  i = 0;
  p = source->tracks;
  birth = 0;
  death = 0;
  links = 0;

  /* Harmonics that were considered active last time, and are still
     active now.  We enter here only if active > 0. */
  while (i < active)
    {
      p->state++;

      if (p->link != NULL)
	{
	  /* Adult harmonic, already in synthesizer, just update
             interpolation envelopes. */
	  shift_envelope (p->aenv, p->a);
	  shift_envelope (p->fenv, p->f);
	}
      else
	{
	  /* Active harmonics not in synthesizer, look if it should be
             inserted. */
	  switch (p->state)
	    {
	    case 1:
	      /* Young harmonics, not in synthesizer until it enters
                 state 2. */
	      shift_envelope (p->aenv, p->a);
	      shift_envelope (p->fenv, p->f);
	      break;

	    case 2:
	      /* Young harmonics, should be inserted now. */

	      /* FIXME: maybe allocation limit has been reached.
		 There will be an audible problem if we don't have
		 enough tracks in synthesizer to store partials. */
	      if (s->active_tracks < s->allocated)
		{
		  /* New track in synthesizer. */
		  shift_envelope (p->aenv, p->a);
		  shift_envelope (p->fenv, p->f);

		  /* Guess first values in interpolation tables. */
		  p->aenv[0] = 2.0 * p->aenv[1] - p->aenv[2];
		  //p->fenv[0] = 2.0 * p->fenv[1] - p->fenv[2];
		  p->fenv[0] = p->f;
		  p->fenv[1] = p->f;
		  p->fenv[2] = p->f;
		  p->fenv[3] = p->f;

		  {
		    /* Initialize sinusoidal parameters. */
#ifdef USE_RESONATOR
		    /* We can't setup initial phases with the
                       resonator. */
		    p->v1 = 0.0;
		    p->v2 = sin (FREQCOEFF * p->fenv[1]);
#else
		    double phi; /* Initial phase. */

		    phi = ((double) random ()) / RAND_MAX;
		    p->v1 = cos (phi);
		    p->v2 = sin (phi);
#endif
		  }

		  p->link = s->tracks + s->active_tracks;
		  s->tracks[s->active_tracks] = p;
		  s->active_tracks++;
		  links++;
		}
	      break;

	    default:
	      fprintf (stderr, "update_source: fatal: unlinked adult harmonic.\n");
	      exit (EXIT_FAILURE);
	      break;
	    }
	}

      i++;
      p++;
    }

  /* Birth of harmonics.  We enter here only if source->active_tracks
     < harmonics. */
  while (i < harmonics)
    {
      /* Currently in state 0, not inserted into synthesizer until it
         enters state 2. */
      p->state = 0;

      /* Update interpolation envelopes. */
      /* Partial should fade in, so ignore current amplitude. */
      shift_envelope (p->aenv, BELOW_MIN_AMP);
      shift_envelope (p->fenv, p->f);

      i++;
      p++;
      birth++;
    }

  /* Harmonics that begin death cycle.  We enter here only if
     harmonics < source->active_tracks. */
  while (i < source->active_tracks)
    {
      /* State will go negative from here, unless the harmonic
         reappears as a young one. */
      p->state = 0;

      /* Partial should fade out. */
      p->a = BELOW_MIN_AMP;
      shift_envelope (p->aenv, p->a);
      shift_envelope (p->fenv, p->f);

      i++;
      p++;
      death++;
    }

  /* Harmonics that were already dying last time, but still are in
     synthesizer.  We enter here only if MAX (harmonics,
     source->active_tracks) < source->linked_tracks. */
  while (i < source->linked_tracks)
    {
      p->state--;

      if (p->state == -2)
	{
	  /* Can be safely removed from synthesizer now. */
	  *(p->link) = NULL;
	  p->link = NULL;
	  links--;
	}
      else
	{
	  /* Not old enough, don't remove it from synthesizer, guess
	     how amplitude should be interpolated. */
	  shift_envelope (p->aenv, 2.0 * p->aenv[3] - p->aenv[2]);
	  shift_envelope (p->fenv, p->f);
	}

      i++;
      p++;
    }

  source->active_tracks += birth - death;
  source->linked_tracks += links;

  /* One step in the circular buffer of emitted frames. */
  source->emission_index = (source->emission_index == 0) ?
    MAX_PROPAGATED_FRAMES - 1 : source->emission_index - 1;

  /* Look if a source on which a deletion request is pending can be
     deleted. */
  if (source->delayed_free && source->linked_tracks == 0)
    sas_synthesizer_source_delayed_free (s, source);
}

static inline void
update_sources (sas_synthesizer_t s)
{
  sas_source_t current;

  current = s->sources;
  while (current != NULL)
    {
      update_source (s, current);
      current = current->next;
    }
}

/* Compare function for partials; used as a callback for qsort;
   decreasing amplitudes. */
static int
compare_amplitudes (const void * e1, const void * e2)
{
  partial_t p1;
  partial_t p2;

  p1 = *((partial_t *) e1);
  p2 = *((partial_t *) e2);

  /* Note: should return an integer, so be careful with floating point
     numbers.  This expression is similar to one found in the GNU C
     library documentation. */
  return (p1->a < p2->a) - (p2->a < p1->a);
}

/* Just in the case of profiling.  This function will be included into
   the profile if you define the keyword 'inline' as nothing (see top
   of file) and don't use high optimization options. */
static inline void
my_qsort (void * base,
	  size_t nmemb,
	  size_t size,
	  int (* compar)(const void *, const void *))
{
  qsort (base, nmemb, size, compar);
}

static inline void
update_tracks (sas_synthesizer_t s)
{
  partial_t * src;
  partial_t * dst;
  int closed_tracks;
  int i;

  closed_tracks = 0;
  s->audible_tracks = 0;

  for (i = 0, dst = src = s->tracks; i < s->active_tracks; i++, src++)
    {
      partial_t p;

      p = *src;

      if (p)
	{
	  /* Amplitude selection: only keep audible partials in
             s->tracks2. */
	  if (p->a > BELOW_MIN_AMP)
	    s->tracks2[s->audible_tracks++] = p;

	  /* Shift-compact (a closed track leaves a gap). */
	  *dst = p;
	  p->link = dst;
	  dst++;
	}
      else
	closed_tracks++;
    }

  /* Note: do not use libc qsort on s->tracks since there are backward
     links in partials: blind permutation of two tracks produces
     semantic inconsistencies.  Instead, use s->tracks2. */

  /* Sort by decreasing amplitudes. */
  my_qsort (s->tracks2,
	    s->audible_tracks,
	    sizeof (partial_t),
	    compare_amplitudes);

  s->active_tracks -= closed_tracks;
}

/* Frequency in [0,MAX_AUDIBLE_FREQUENCY] to Bark in [MIN_BARK,
   MAX_BARK]. */
static inline double
f2B (double f)
{
  return (f <= 0.0) ? MIN_BARK :
    ((f <= 500.0) ? f * 0.01 : 9.0 + 4.0 * M_LOG2E * log (f * 0.001));
}

/* Amplitude in [0,1] to dB in [MIN_DB,0]. */
static inline double
a2dB (double amplitude)
{
  return (amplitude <= MIN_AMP) ? MIN_DB : 20.0 * log10 (amplitude);
}

static inline masking_partial_t
masking_partial_make (sas_synthesizer_t s, partial_t p)
{
  masking_partial_t mp;
  double vdB_left;
  double vdB_right;

  if (s->pool->used == s->pool->allocated)
    {
      s->pool->allocated += MAX_PARTIALS_PER_SYNTH;
      s->pool->partials = (masking_partial_t)
	realloc (s->pool->partials,
		 s->pool->allocated * sizeof (struct masking_partial_s));
    }

  mp = s->pool->partials + s->pool->used++;
  mp->p = p;
  mp->freqB = f2B (p->f);
  vdB_left = a2dB (p->a * p->source->l_ratio);
  vdB_right = a2dB (p->a * p->source->r_ratio);

  if (vdB_left < vdB_right)
    {
      mp->min_vdB = vdB_left;
      mp->max_vdB = vdB_right;
    }
  else
    {
      mp->min_vdB = vdB_right;
      mp->max_vdB = vdB_left;
    }

  return mp;
}

/* Updates mask with a partial p.  Returns 0 if p is masked, 1
   otherwise.  Should be called with partials of decreasing
   amplitude. */
static inline int
add_partial_to_mask (sas_synthesizer_t s, partial_t p)
{
  masking_partial_t new_mp;
  masking_partial_t mp_lowf;
  masking_partial_t mp_highf;
  double v_lowf, v_highf, v;

  new_mp = masking_partial_make (s, p);

  skip_list_insert (s->mask, new_mp);

  v_lowf = ((mp_lowf = skip_list_previous (s->mask, new_mp)) == NULL) ?
    MIN_DB :
    RIGHT_LINE_COEFF * (new_mp->freqB - mp_lowf->freqB) +
    mp_lowf->min_vdB - DB_DIFF;

  v_highf = ((mp_highf = skip_list_next (s->mask, new_mp)) == NULL) ?
    MIN_DB :
    LEFT_LINE_COEFF * (new_mp->freqB - mp_highf->freqB) +
    mp_highf->min_vdB - DB_DIFF;

  v = MAX (v_lowf, v_highf);

  if (new_mp->min_vdB - DB_DIFF < v)
    /* The partial's contribution to the mask is already masked. */
    skip_list_remove (s->mask, new_mp);

  /* Is new_mp audible? */
  return (new_mp->max_vdB > v);
}

static inline void
reset_mask (sas_synthesizer_t s)
{
  skip_list_reset (s->mask);
  s->pool->used = 0;
}

static inline void
update_mask (sas_synthesizer_t s)
{
  int i;

  reset_mask (s);
  s->masked_tracks = 0;

  for (i = 0; i < s->audible_tracks; i++)
    {
      partial_t p;

      p = s->tracks2[i];

      if (add_partial_to_mask (s, p) == 0)
	{
	  /* The partial is masked. */
	  s->masked_tracks++;
	  p->aenv[3] = BELOW_MIN_AMP;
	}
    }

  s->audible_tracks -= s->masked_tracks;
}

#ifndef USE_RESONATOR

/* Normal partial synthesis. */
static inline void
partial_forward_synthesis (partial_t p,
			   double a,
			   double a_next,
			   double f,
			   double * buffer)
{
  int i;
  double omega;
  double r_exp, i_exp;
  double r_inc, i_inc;
  double l_a, r_a;
  double a_inc;
  double l_a_inc, r_a_inc;

  l_a = p->source->l_ratio * a;
  r_a = p->source->r_ratio * a;

  /* Linear increment between a and a_next. */
  a_inc = (a_next - a) / STEP_SAMPLES;
  l_a_inc = p->source->l_ratio * a_inc;
  r_a_inc = p->source->r_ratio * a_inc;

  r_exp = p->v1;
  i_exp = p->v2;

  omega = FREQCOEFF * f;

  r_inc = cos (omega);
  i_inc = sin (omega);

  for (i = 0; i < STEP_SAMPLES; i++)
    {
      double r;

      *buffer++ += l_a * i_exp;
      *buffer++ += r_a * i_exp;
      l_a += l_a_inc;
      r_a += r_a_inc;

      r = r_exp;
      r_exp = r * r_inc - i_exp * i_inc;
      i_exp = r * i_inc + i_exp * r_inc;
    }

  p->v1 = r_exp;
  p->v2 = i_exp;
}

/* Fast forward in the case of a silent partial. */
static inline void
partial_fast_forward (partial_t p, double f)
{
  double omega;
  double r_exp, i_exp;
  double r_inc, i_inc;

  r_exp = p->v1;
  i_exp = p->v2;

  omega = (FREQCOEFF * STEP_SAMPLES) * f;

  r_inc = cos (omega);
  i_inc = sin (omega);

  {
    double r;

    r = r_exp;
    r_exp = r * r_inc - i_exp * i_inc;
    i_exp = r * i_inc + i_exp * r_inc;
  }

  p->v1 = r_exp;
  p->v2 = i_exp;
}

#else

/* Normal partial synthesis with resonator algorithm. */
static inline void
partial_forward_synthesis (partial_t p,
			   double a,
			   double a_next,
			   double f,
			   double * buffer)
{
  int i;
  double fn;
  double fn_1;
  double c2;
  double l_a, r_a;
  double a_inc;
  double l_a_inc, r_a_inc;

  l_a = p->source->l_ratio * a;
  r_a = p->source->r_ratio * a;

  /* Linear increment between a and a_next. */
  a_inc = (a_next - a) / STEP_SAMPLES;
  l_a_inc = p->source->l_ratio * a_inc;
  r_a_inc = p->source->r_ratio * a_inc;

  fn = p->v1;
  fn_1 = p->v2;

  c2 = 2.0 * cos (FREQCOEFF * f);

  for (i = 0; i < STEP_SAMPLES; i++)
    {
      double fnew;

      *buffer++ += l_a * fn;
      *buffer++ += r_a * fn;
      l_a += l_a_inc;
      r_a += r_a_inc;

      fnew = fn * c2 - fn_1;
      fn_1 = fn;
      fn = fnew;
    }

  p->v1 = fn;
  p->v2 = fn_1;
}

/* Fast forward in the case of a silent partial (resonator version). */
static inline void
partial_fast_forward (partial_t p, double f)
{
  int i;
  double fn;
  double fn_1;
  double c2;

  fn = p->v1;
  fn_1 = p->v2;

  c2 = 2.0 * cos (FREQCOEFF * f);

  for (i = 0; i < STEP_SAMPLES; i++)
    {
      double fnew;

      fnew = fn * c2 - fn_1;
      fn_1 = fn;
      fn = fnew;
    }

  p->v1 = fn;
  p->v2 = fn_1;
}

#endif

/*======================================================================*/
/* Interface */

sas_synthesizer_t
sas_synthesizer_make (void)
{
  sas_synthesizer_t s;
  int step;

  s = (sas_synthesizer_t) malloc (sizeof (struct sas_synthesizer_s));
  assert (s);

  s->sources = NULL;
  s->number_of_sources = 0;
  s->amplitude_factor = 0.0;

  s->allocated = MAX_PARTIALS_PER_SYNTH;
  s->tracks = (partial_t *) malloc (s->allocated * sizeof (partial_t));
  assert (s->tracks);

  s->active_tracks = 0;
  s->masked_tracks = 0;
  s->audible_tracks = 0;

  s->threshold = sas_envelope_amplitude_threshold ();

  s->tracks2 =  (partial_t *) malloc (s->allocated * sizeof (partial_t));
  assert (s->tracks2);

  s->mask = skip_list_make (compare_frequencies);

  s->pool = (pool_of_masking_partials_t)
    malloc (sizeof (struct pool_of_masking_partials_s));
  s->pool->allocated = MAX_PARTIALS_PER_SYNTH;
  s->pool->partials = (masking_partial_t)
    malloc (s->pool->allocated * sizeof (struct masking_partial_s));
  s->pool->used = 0;

  /* Compute the constant coefficients for the interpolation of
     amplitude and frequency during synthesis. */
  for (step = 0; step < INTERPOLATION_STEPS; step++)
    {
      double t0, t1, t2;

      t0 = ((double) step) / INTERPOLATION_STEPS;

      t1 = t0 * t0;
      t2 = t0 * t1;

      icoeffs[step][0] = 0.5 * (      -t0 + 2.0 * t1 -       t2);
      icoeffs[step][1] = 0.5 * ( 2.0      - 5.0 * t1 + 3.0 * t2);
      icoeffs[step][2] = 0.5 * (       t0 + 4.0 * t1 - 3.0 * t2);
      icoeffs[step][3] = 0.5 * (                 -t1 +       t2);
    }

  return s;
}

void
sas_synthesizer_free (sas_synthesizer_t s)
{
  assert (s);

  while (s->sources != NULL)
    sas_synthesizer_source_delayed_free (s, s->sources);

  free (s->tracks);
  free (s->tracks2);
  skip_list_free (s->mask);
  free (s->pool);
  free (s);
}

sas_source_t
sas_synthesizer_source_make (sas_synthesizer_t s,
			     sas_position_t pos,
			     sas_update_callback_t update,
			     void * call_data)
{
  sas_source_t source;
  int i;

  assert (s);
  assert (pos);
  assert (update);
  assert (call_data);

  source = (sas_source_t) malloc (sizeof (struct sas_source_s));
  assert (source);

  source->update = update;
  source->call_data = call_data;

  source->propagated_frames = (sas_frame_t *)
    malloc (MAX_PROPAGATED_FRAMES * sizeof (sas_frame_t));
  assert (source->propagated_frames);

  for (i = 0; i < MAX_PROPAGATED_FRAMES; i++)
    source->propagated_frames[i] = sas_frame_make ();

  source->emission_index = 0;

  source->position.x = pos->x;
  source->position.y = pos->y;
  source->position.z = pos->z;

  source->distance =
    DISTANCE (source->position.x,
	      source->position.y,
	      source->position.z);

  source->cos_angle =
    (source->distance > 0.0) ?
    source->position.x / source->distance :
    0.0;

  source->doppler = 1.0;

  update_source_spatial_information (source);

  source->tracks = (partial_t)
    malloc (MAX_PARTIALS_PER_SOURCE * sizeof (struct partial_s));
  assert (source->tracks);

  source->active_tracks = 0;
  source->linked_tracks = 0;

  for (i = 0; i < MAX_PARTIALS_PER_SOURCE; i++)
    {
      partial_t p;

      p = source->tracks + i;
      p->source = source;
      p->link = NULL;
      p->a = 0.0;
      p->f = 440.0;
      p->aenv[0] = p->aenv[1] = p->aenv[2] = p->aenv[3] = p->a;
      p->fenv[0] = p->fenv[1] = p->fenv[2] = p->fenv[3] = p->f;
      p->state = 0;
      p->v1 = 0.0;
      p->v2 = 0.0;
    }

  source->next = s->sources;
  s->sources = source;

  source->delayed_free = 0;

  s->number_of_sources++;
  s->amplitude_factor =
    1.0 / ((log (s->number_of_sources) * (1.0 / M_LN2)) + 1.0);

  return source;
}

void
sas_synthesizer_source_free (sas_synthesizer_t s, sas_source_t source)
{
  assert (s);
  assert (source);
  assert (source->delayed_free == 0);

  if (source->linked_tracks == 0)
    sas_synthesizer_source_delayed_free (s, source);
  else
    {
      /* The source still has active partials in synthesizer.  Program
         the mute of partials and the source deletion in the future. */
      int i;

      for (i = 0; i < MAX_PROPAGATED_FRAMES; i++)
	sas_frame_set_amplitude (source->propagated_frames[i], 0.0);

      source->delayed_free = 1;
    }
}

void
sas_synthesizer_synthesize (sas_synthesizer_t s, double * buffer)
{
  int i;
  partial_t * src;

  assert (s);
  assert (buffer);

  /* Clear buffer. */
  for (i = 0; i < 2 * SAS_SAMPLES; i++)
    buffer[i] = 0.0;

  update_sources (s);
  update_tracks (s);
  update_mask (s);

  for (i = 0, src = s->tracks; i < s->active_tracks; i++, src++)
    {
      partial_t p;
      int step;
      double inta[INTERPOLATION_STEPS + 1];
      double intf[INTERPOLATION_STEPS + 1];

      p = *src;

      /* Compute the INTERPOLATION_STEPS + 1 values for amplitude and
	 frequency. */
      inta[0] = p->aenv[1];
      intf[0] = p->fenv[1];

      /* FIXME: does the compiler use pipelining features of the CPU?  */
      for (step = 1; step < INTERPOLATION_STEPS; step++)
	{
	  inta[step] = interpolate_value (p->aenv, step);
	  intf[step] = interpolate_value (p->fenv, step);
	}

      inta[INTERPOLATION_STEPS] = p->aenv[2];
      intf[INTERPOLATION_STEPS] = p->fenv[2];

      /* Synthesize. */
      for (step = 0; step < INTERPOLATION_STEPS; step++)
	{
	  double a, a_next;
	  double f;

	  a = inta[step];
	  a_next = inta[step + 1];
	  f = intf[step];

	  if (a < MIN_AMP && a_next < MIN_AMP)
	    /* Partial is not audible.  Don't fill buffer, but update
	       parameters. */
	    partial_fast_forward (p, f);
	  else
	    /* Partial is audible.  Fill buffer. */
	    partial_forward_synthesis (p, a, a_next, f,
				       buffer + step * 2 * STEP_SAMPLES);
	}
    }
}

void
sas_synthesizer_statistics (sas_synthesizer_t s,
			    struct sas_synthesizer_statistics_s * stats)
{
  assert (s);
  assert (stats);

  stats->number_of_sources = s->number_of_sources;
  stats->number_of_active_tracks = s->active_tracks;
  stats->number_of_masked_tracks = s->masked_tracks;
  stats->number_of_audible_tracks = s->audible_tracks;
}

