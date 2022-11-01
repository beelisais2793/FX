/*
 * Non-Copyright (c) 2019 SoundBooze <soundbooze@gmail.com>
 *                            _ _                        
 *  ___  ___  _   _ _ __   __| | |__   ___   ___ _______ 
 * / __|/ _ \| | | | '_ \ / _` | '_ \ / _ \ / _ \_  / _ \
 * \__ \ (_) | |_| | | | | (_| | |_) | (_) | (_) / /  __/
 * |___/\___/ \__,_|_| |_|\__,_|_.__/ \___/ \___/___\___|
 *                                                     
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 */

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <loris.h>

int 
xtract_f0 (const double *data, const int N, const void *argv, double *result)
{
  int M, tau, n;
  double sr;
  size_t bytes;
  double f0, err_tau_1, err_tau_x, array_max,
         threshold_peak, threshold_centre,
         *input;

  sr = *(double *)argv;
  if(sr == 0)
    sr = 44100.0;

  input = (double*)malloc(bytes = N * sizeof(double));
  input = (double*)memcpy(input, data, bytes);
  /*  threshold_peak = *((double *)argv+1);
      threshold_centre = *((double *)argv+2);
      printf("peak: %.2\tcentre: %.2\n", threshold_peak, threshold_centre);*/
  /* add temporary dynamic control over thresholds to test clipping effects */

  /* FIX: tweak and  make into macros */
  threshold_peak = .8;
  threshold_centre = .3;
  M = N >> 1;
  err_tau_1 = 0;
  array_max = 0;

  /* Find the array max */
  for(n = 0; n < N; n++)
  {
    if (input[n] > array_max)
      array_max = input[n];
  }

  threshold_peak *= array_max;

  /* peak clip */
  for(n = 0; n < N; n++)
  {
    if(input[n] > threshold_peak)
      input[n] = threshold_peak;
    else if(input[n] < -threshold_peak)
      input[n] = -threshold_peak;
  }

  threshold_centre *= array_max;

  /* Centre clip */
  for(n = 0; n < N; n++)
  {
    if (input[n] < threshold_centre)
      input[n] = 0;
    else
      input[n] -= threshold_centre;
  }

  /* Estimate fundamental freq */
  for (n = 1; n < M; n++)
    err_tau_1 = err_tau_1 + fabs(input[n] - input[n+1]);
  /* FIX: this doesn't pose too much load if it returns 'early', but if it can't find f0, load can be significant for larger block sizes M^2 iterations! */
  for (tau = 2; tau < M; tau++)
  {
    err_tau_x = 0;
    for (n = 1; n < M; n++)
    {
      err_tau_x = err_tau_x + fabs(input[n] - input[n+tau]);
    }
    if (err_tau_x < err_tau_1)
    {
      f0 = sr / (tau + (err_tau_x / err_tau_1));
      *result = f0;
      free(input);
      return 0;
    }
  }

  *result = -0;
  free(input);

  return 0;
}

PartialList *
synthesizestream (double f0, double *in, unsigned int nsamps, double sr)
{
  PartialList *partials = NULL;

  double samples[nsamps];

  memcpy(samples, in, nsamps);

  analyzer_configure(.8 * f0, f0);
  analyzer_setFreqDrift(.2 * f0); 

  partials = createPartialList();
  analyze(samples, nsamps, sr, partials);

  memset(samples, 0, nsamps * sizeof(double));
  nsamps = synthesize(partials, samples, nsamps, sr);

  return partials;
} //destroyPartialList(partials);
