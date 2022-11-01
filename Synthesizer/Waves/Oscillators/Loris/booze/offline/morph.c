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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <loris.h>
#include <math.h>
#include <sndfile.h>
#include <stdio.h>
#include <jack/jack.h>

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
}

double *
morphstream (PartialList *src1, PartialList *src2, unsigned int nsamps, double sr)
{
  double *samples = (double *) malloc (sizeof(double) * nsamps);
  unsigned int N = 0;

  LinearEnvelope *reference = 0;
  LinearEnvelope *pitchenv = createLinearEnvelope();

  LinearEnvelope *morphenv = createLinearEnvelope();
  PartialList *mrph = createPartialList();   

  double src1_times[] = {0.1, 1.};
  double src2_times[] = {0.3, 1.};
  double tgt_times[] = {0.2, 0.4};

  /* channelize and distill */
  reference = createF0Estimate(src1, 350, 450, 0.01);
  channelize(src1, reference, 1);
  distill(src1);
  destroyLinearEnvelope(reference);
  reference = 0;

  /* shift pitch of src partials */
  linearEnvelope_insertBreakpoint(pitchenv, 0, -600);
  shiftPitch(src1, pitchenv);
  destroyLinearEnvelope(pitchenv);
  pitchenv = 0;

  /* channelize and distill */
  reference = createF0Estimate(src2, 250, 320, 0.01);
  channelize(src2, reference, 1);
  distill(src2);
  destroyLinearEnvelope(reference);
  reference = 0;

  /* dilating sounds to align onsets */
  dilate(src1, src1_times, tgt_times, 2);
  dilate(src2, src2_times, tgt_times, 2);

  /* perform morph */
  linearEnvelope_insertBreakpoint(morphenv, 0.6, 0);
  linearEnvelope_insertBreakpoint(morphenv, 2, 1);
  morph(src1, src2, morphenv, morphenv, morphenv, mrph);

  /* synthesize and export samples */
  N = synthesize(mrph, samples, nsamps, sr);

  /* cleanup */
  destroyPartialList(mrph);
  destroyPartialList(src1);
  destroyPartialList(src2);
  destroyLinearEnvelope(morphenv);

  return samples;
}

double *
readWav (char *filename, long *len) {

  SF_INFO sndInfo_r;
  double *buffer_r;
  long numFrames;

  SNDFILE *sndFile_r = sf_open(filename, SFM_READ, &sndInfo_r);
  if (sndFile_r == NULL) {
    fprintf(stderr, "sf_open: '%s': %s\n", filename, sf_strerror(sndFile_r));
    exit(EXIT_FAILURE);
  }

  buffer_r = malloc((sndInfo_r.frames * sndInfo_r.channels) * sizeof(double));
  if (buffer_r == NULL) {
    fprintf(stderr, "malloc error\n");
    sf_close(sndFile_r);
    exit(EXIT_FAILURE);
  }

  numFrames = sf_readf_double(sndFile_r, buffer_r, sndInfo_r.frames * sndInfo_r.channels);
  if (numFrames != sndInfo_r.frames) {
    fprintf(stderr, "sf_readf_double problem\n");
    sf_close(sndFile_r);
    free(buffer_r);
    exit(EXIT_FAILURE);
  }

  if (sndInfo_r.channels == 2) {
    double *buffer_m = malloc((sndInfo_r.frames) * sizeof(double));

    for (int i = 0; i < sndInfo_r.frames; i++) {
      buffer_m[i] = 0;

      for(int j = 0; j < sndInfo_r.channels; j++) {
        buffer_m[i] += buffer_r[i * sndInfo_r.channels + j];
      }

      buffer_m[i] /= sndInfo_r.channels;
    }

    sf_close(sndFile_r);
    *len = numFrames;
    return buffer_m;
  }

  sf_close(sndFile_r);
  *len = numFrames;
  return buffer_r;
}

void
writeWav (char *filename, double *y, long numFrames) {

  SF_INFO info;
  info.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
  info.channels = 1;
  info.samplerate = 44100;

  SNDFILE *sndFile = sf_open(filename, SFM_WRITE, &info);
  if (sndFile == NULL) {
    fprintf(stderr, "sfopen: '%s': %s\n", filename, sf_strerror(sndFile));
    exit(EXIT_FAILURE);
  }

  long writtenFrames = sf_writef_double(sndFile, (const double *) y, numFrames);

  if (writtenFrames != numFrames) {
    fprintf(stderr, "sf_writef_double problem\n");
    sf_close(sndFile);
    exit(EXIT_FAILURE);
  }

  sf_write_sync(sndFile);
  sf_close(sndFile);
}

void
morphWav (char *filename, double *src1Signal, double *src2Signal, long src1Len, long src2Len)
{

  double f01 = 0.0;
  double f02 = 0.0;
  double sr = 44100.0;

  xtract_f0(src1Signal, src1Len, &sr, &f01);
  xtract_f0(src2Signal, src2Len, &sr, &f02);

  PartialList *src1 = synthesizestream (f01, src1Signal, src1Len, sr);
  PartialList *src2 = synthesizestream (f02, src2Signal, src2Len, sr);

  double *morphSignal = morphstream (src1, src2, src1Len, sr);
  writeWav(filename, morphSignal, src1Len);

  free(morphSignal);
}

int 
main (int argc, char *argv[])
{

  if (argc != 4) {
    fprintf(stderr, "Usage: %s input1.wav input2.wav output.wav\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  long src1Len; 
  double *src1Signal = readWav(argv[1], &src1Len);

  long src2Len; 
  double *src2Signal = readWav(argv[2], &src2Len);

  morphWav(argv[3], src1Signal, src2Signal, src1Len, src2Len);

  free(src1Signal);
  free(src2Signal);

  exit(EXIT_SUCCESS);;
}
