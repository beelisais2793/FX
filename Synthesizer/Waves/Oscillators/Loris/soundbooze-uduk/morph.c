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

#include <loris.h>
#include <stdlib.h>

double *
morphstream (PartialList *src1, PartialList *src2, unsigned int nsamps, double sr)
{
  double *samples = (double *) malloc (sizeof(double) * nsamps);
  unsigned int N = 0;

  LinearEnvelope *reference = 0;
  LinearEnvelope *pitchenv = createLinearEnvelope();

  LinearEnvelope *morphenv = createLinearEnvelope();
  PartialList *mrph = createPartialList();   

  double src1_times[] = {0.4, 1.};
  double src2_times[] = {0.2, 1.};
  double tgt_times[] = {0.3, 1.2};

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
