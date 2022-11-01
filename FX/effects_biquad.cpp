/*
 * GNUitar
 * Biquad
 * Ciulei Bogdan /Dexterus <dexterus@hackernetwork.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * $Id$
 * $Log$
 * Revision 1.32  2006/11/05 18:50:58  alankila
 * - add tone controls into tubeamp and change default speaker type
 *
 * Revision 1.31  2006/09/04 14:42:03  alankila
 * - fix hilbert-transform for > 1 channel: forgot x0_tmp
 *
 * Revision 1.30  2006/08/07 21:43:29  alankila
 * - committing a hopefully working version of biquads on SSE now. Had to
 *   rename struct members for this to succeed, though. :-(
 *
 * Revision 1.29  2006/08/06 20:57:46  alankila
 * - pepper with const declarations
 *
 * Revision 1.28  2006/07/08 18:11:33  alankila
 * - reduce overdrive effect cpu drain by implementing low-pass filtering
 *   in resampler and reusing the static 720 Hz lowpass filter as decimating
 *   filter. Should be 10-20 % faster.
 *
 * Revision 1.27  2006/07/08 16:28:16  alankila
 * - extend hilbert transform with channel information for effects that could
 *   be used on channels separately. We've already allocated space in biquads
 *   for them.
 *
 * Revision 1.26  2006/06/20 16:49:52  alankila
 * - make rotary and phaser more powerful
 *
 * Revision 1.25  2006/06/16 14:44:14  alankila
 * - use full precision for allpass constants
 * - remove SSE version of biquad code, it was buggy.
 *
 * Revision 1.24  2006/05/31 13:52:18  fonin
 * powf() does not exist on Windows, replaced with pow(); fixed C++ style variable declarations+init for Windows sanity.
 *
 * Revision 1.23  2006/05/25 16:54:12  alankila
 * - use 12 dB/oct lowpass filter between stages to help with our treble
 *   problem. Sounds less aliased now.
 *
 * Revision 1.22  2006/05/25 10:30:41  alankila
 * - use SSE for biquad computations.
 *
 * Revision 1.21  2006/05/24 20:17:05  alankila
 * - make inlining actually possible / working
 *
 * Revision 1.20  2006/05/20 14:28:04  alankila
 * - restore mono-phaser back to earlier design
 * - fix hilbert transform's allpass delay
 * - need to figure out what is the proper name for the phasor allpass
 *
 * Revision 1.19  2006/05/13 17:10:06  alankila
 * - move hilbert transform into biquad.c
 * - implement stereo phaser using hilbert transform
 * - clean up remaining struct biquad references and replace them with typedef
 *
 * Revision 1.18  2006/05/13 09:33:16  alankila
 * - more power to phaser, less cpu use, good deal
 *
 * Revision 1.17  2006/05/07 13:22:12  alankila
 * - new bare bones distortion effect: tubeamp
 *
 * Revision 1.16  2006/05/05 18:34:32  alankila
 * - handle denormals to avoid slowdowns for digital silence type situations.
 *
 * Revision 1.15  2005/11/01 12:32:16  alankila
 * - fix highpass
 *
 * Revision 1.14  2005/10/30 11:21:05  alankila
 * - more correct and precise output filtering!
 * - real device seems to have some kind of highpass filtering around 50 Hz
 *   maybe or so, because there's too much bass...
 *
 * Revision 1.13  2005/10/07 12:50:12  alankila
 * - move delay shape computation to where it belongs and change it to bit
 *   smoother
 *
 * Revision 1.12  2005/09/12 22:01:56  alankila
 * - swap a0/b0 around to better coincide with available literature
 * - optimize x1 and x2 terms off chebyshev as they are defined as:
 *       x2 = x0
 *       x1 = 2 * x0
 *   and only used once.
 * - introduce a0 into the chebyshev to resemble each other more
 *
 * Revision 1.11  2005/09/12 08:26:51  alankila
 * - flip the signs of b1 and b2 (but not b0) because the mathematical
 *   difference equation is usually written that way.
 *
 * Revision 1.10  2005/09/10 10:53:38  alankila
 * - remove the need to reserve biquad's mem in caller's side
 *
 * Revision 1.9  2005/09/09 20:22:17  alankila
 * - phasor reimplemented according to a popular algorithm that simulates
 *   high-impedance isolated varying capacitors
 *
 * Revision 1.8  2005/08/14 23:31:22  alankila
 * revert earlier "fix" that does * sizeof(DspFloatType). It was a brainfart.
 *
 * Revision 1.7  2005/08/10 11:01:39  alankila
 * - remove separate chebyshev.c, move the code into biquad.c
 * - fix the copy in .h to agree with DoBiquad's implementation
 * - rename functions:
 *   * DoBiquad       -> do_biquad
 *   * SetEqBiquad    -> set_peq_biquad
 *   * CalcChebyshev2 -> set_chebyshev2_biquad
 * - this change is followed by fixups in effects distort2 & eqbank
 *
 * Revision 1.6  2005/08/07 12:42:05  alankila
 * Do not use << 2 because DspFloatType can be wider than 4.
 * Better say what you mean.
 *
 * Revision 1.5  2005/07/31 10:22:54  fonin
 * Check for NaN values on input and output
 *
 * Revision 1.4  2005/04/06 19:34:20  fonin
 * Code lickup
 *
 * Revision 1.3  2004/10/21 11:13:45  dexterus
 * Fixed calculus error
 * Added support for any numer of channels
 * iniline support delimited to Visual C
 *
 * Revision 1.2  2003/12/28 10:16:08  fonin
 * Code lickup
 *
 *
 */

#include "effects_biquad.h"


using namespace HammerFX;

/* peaking band equalizer */
void
set_peq_biquad(DspFloatType Fs, DspFloatType Fc, DspFloatType BW, DspFloatType G, Biquad_t *f)
{
    DspFloatType          k, om, alpha, a0, BWoct;
    
    k = pow(10, G / 40);	/* relative gain */
    BWoct = BW / (Fc - BW / 2);	/* bandwidth in octaves */
    om = 2 * M_PI * Fc / Fs;	/* normalized frequency in radians */
    alpha = sinh(log(2) / 2 * BWoct * om / sin(om)) * sin(om);
    
    a0 = 1 + alpha / k;
    f->b0 = (1 + alpha * k) / a0;
    f->b[0] = -2 * cos(om)    / a0;
    f->b[1] = (1 - alpha * k) / a0;
    f->b[2] = -f->b[0];
    f->b[3] = -(1 - alpha / k) / a0;
}

/* low pass filter */
void
set_lpf_biquad(DspFloatType Fs, DspFloatType Fc, DspFloatType BW, Biquad_t *f)
{
    DspFloatType om, alpha, a0;
    
    om = 2 * M_PI * Fc / Fs;
    alpha = sinh(log(2) / 2 * BW * om / sin(om)) * sin(om);
    
    a0 = 1 + alpha;
    f->b0 = (1 - cos(om)) / 2 / a0;
    f->b[0] = 1 - cos(om);
    f->b[1] = f->b0;
    f->b[2] = 2 * cos(om) / a0;
    f->b[3] = -(1 - alpha)  / a0;
}

/* band pass filter */
void
set_bpf_biquad(DspFloatType Fs, DspFloatType Fc, DspFloatType BW, Biquad_t *f)
{
    DspFloatType om, alpha, a0;
    
    om = 2 * M_PI * Fc / Fs;
    alpha = sinh(log(2) / 2 * BW * om / sin(om)) * sin(om);
    
    a0 = 1 + alpha;
    f->b0 = alpha        / a0;
    f->b[0] = 0;
    f->b[1] = -f->b0;
    f->b[2] = 2 * cos(om) / a0;
    f->b[3] = -(1 - alpha)  / a0;
}

/* 2nd order allpass filter, delay can vary from 0 to 1 */
void
set_phaser_biquad(DspFloatType a, Biquad_t *f)
{
    f->b0 = a * a;
    f->b[0] = a;
    f->b[1] = 1;
    f->b[2] = -a;
    f->b[3] = -a * a;
}

/* A 2nd order allpass, delay can vary from 0 to 1 */
void
set_2nd_allpass_biquad(DspFloatType a, Biquad_t *f)
{
    f->b0 = a * a;
    f->b[0] = 0;
    f->b[1] = -1;
    f->b[2] = 0;
    f->b[3] = f->b0;
}

void
set_rc_lowpass_biquad(DspFloatType sample_rate, DspFloatType freq, Biquad_t *f)
{
    DspFloatType rc = 1 / (2 * M_PI * freq);
    DspFloatType ts = 1.0 / sample_rate;

    f->b0 = ts / (ts + rc);
    f->b[0] = 0;
    f->b[1] = 0;
    f->b[2] = rc / (ts + rc);
    f->b[3] = 0;
}

void
set_rc_highpass_biquad(DspFloatType sample_rate, DspFloatType freq, Biquad_t *f)
{
    DspFloatType rc = 1 / (2 * M_PI * freq);
    DspFloatType ts = 1.0 / sample_rate;

    f->b0 = 1;
    f->b[0] = -1;
    f->b[1] = 0;
    f->b[2] = rc / (ts + rc);
    f->b[3] = 0;
}

void
set_chebyshev1_biquad(DspFloatType Fs, DspFloatType Fc, DspFloatType ripple, int lowpass, Biquad_t *f)
{
    DspFloatType          x, y, z, c, v, t, r, om, m, x0, y1p, y2, k, d, tt, tt2, a0;
    
    om = 2 * M_PI * Fc / Fs;
    
    c = -cos(M_PI / 4);
    v =  sin(M_PI / 4);
    if (ripple > 0) {
        t = 100.0 / (100.0 - ripple);
        x = sqrt(t * t - 1);
        t = 1 / x;
        r = t + sqrt(t / x);
        y = 0.5 * log(r + 1);
        z = 0.5 * log(r - 1);
        t = exp(z);
        z = (t + 1 / t) / 2;
        t = exp(y);
        c *= (t - 1 / t) / 2 / z;
        v *= (t + 1 / t) / 2 / z;
    }
    tt = 2 * tan(0.5);
    tt2 = tt * tt;
    m = c * c + v * v;
    d = 4 - 4 * c * tt + m * tt2;
    x0 = tt2 / d;
    y1p = (8 - 2 * m * tt2) / d;
    y2 = (-4 - 4 * c * tt - m * tt2) / d;
    if (lowpass)
        k = sin(0.5 - om / 2) / sin(0.5 + om / 2);
    else
        k = -cos(om / 2 + 0.5) / cos(om / 2 - 0.5);
    
    a0 = 1 + k * (y1p - y2 * k);
    f->b0 = (x0 - k * (2 - k) * x0)             / a0;
    f->b[0] = 2 * f->b0;
    f->b[1] =     f->b0;
    f->b[2] = (k * (2 + y1p * k - 2 * y2) + y1p) / a0;
    f->b[3] = (-k * (k + y1p) + y2)              / a0;
    if (!lowpass) {
        f->b[0] = -f->b[0];
        f->b[2] = -f->b[2];
    }
}

void
set_lsh_biquad(DspFloatType Fs, DspFloatType Fc, DspFloatType G, Biquad_t *f)
{
    DspFloatType b0, b1, b2, a0, a1, a2, omega, cs, sn, beta, A;

    A = pow(10, G / 40);
    omega = 2 * M_PI * Fc / Fs;
    cs = cos(omega);
    sn = sin(omega);
    beta = sqrt(A + A);
    
    b0 = A * ((A + 1) - (A - 1) * cs + beta * sn);
    b1 = 2 * A * ((A - 1) - (A + 1) * cs);
    b2 = A * ((A + 1) - (A - 1) * cs - beta * sn);
    a0 = (A + 1) + (A - 1) * cs + beta * sn;
    a1 = -2 * ((A - 1) + (A + 1) * cs);
    a2 = (A + 1) + (A - 1) * cs - beta * sn;

    f->b0 = b0 / a0;
    f->b[0] = b1 / a0;
    f->b[1] = b2 / a0;
    f->b[2] = -a1 / a0;
    f->b[3] = -a2 / a0;
}

void
set_hsh_biquad(DspFloatType Fs, DspFloatType Fc, DspFloatType G, Biquad_t *f)
{
    DspFloatType b0, b1, b2, a0, a1, a2, omega, cs, sn, beta, A;

    A = pow(10, G / 40);
    omega = 2 * M_PI * Fc / Fs;
    cs = cos(omega);
    sn = sin(omega);
    beta = sqrt(A + A);

    b0 = A * ((A + 1) + (A - 1) * cs + beta * sn);
    b1 = -2 * A * ((A - 1) + (A + 1) * cs);
    b2 = A * ((A + 1) + (A - 1) * cs - beta * sn);
    a0 = (A + 1) - (A - 1) * cs + beta * sn;
    a1 = 2 * ((A - 1) - (A + 1) * cs);
    a2 = (A + 1) - (A - 1) * cs - beta * sn;
 
    f->b0 = b0 / a0;
    f->b[0] = b1 / a0;
    f->b[1] = b2 / a0;
    f->b[2] = -a1 / a0;
    f->b[3] = -a2 / a0;
}

void
hilbert_transform(DspFloatType input, DspFloatType *x0, DspFloatType *x1, Hilbert_t *h, int curr_channel)
{
    int i;
    DspFloatType x0i, x1i;

    x0i = h->x0_tmp[curr_channel];
    h->x0_tmp[curr_channel] = input;
    x1i = input;

    for (i = 0; i < 4; i += 1) {
        x0i = do_biquad(x0i, &h->a1[i], curr_channel);
        x1i = do_biquad(x1i, &h->a2[i], curr_channel);
    }
    *x0 = x0i;
    *x1 = x1i;
}

/* Setup allpass sections to produce hilbert transform.
 * There value were searched with a genetic algorithm by
 * Olli Niemitalo <o@iki.fi>
 * 
 * http://www.biochem.oulu.fi/~oniemita/dsp/hilbert/
 *
 * The difference between the outputs of passing signal through
 * a1 allpass delay + 1 sample delay and a2 allpass delay
 * is shifted by 90 degrees over 99 % of the frequency band.
 */
void
hilbert_init(Hilbert_t *h)
{
    set_2nd_allpass_biquad(0.6923877778065, &h->a1[0]);
    set_2nd_allpass_biquad(0.9360654322959, &h->a1[1]);
    set_2nd_allpass_biquad(0.9882295226860, &h->a1[2]);
    set_2nd_allpass_biquad(0.9987488452737, &h->a1[3]);

    set_2nd_allpass_biquad(0.4021921162426, &h->a2[0]);
    set_2nd_allpass_biquad(0.8561710882420, &h->a2[1]);
    set_2nd_allpass_biquad(0.9722909545651, &h->a2[2]);
    set_2nd_allpass_biquad(0.9952884791278, &h->a2[3]);
}

/* Obtains two interpolated samples in out1 and out2. You need to imagine that the
 * signal entering this function is upsampled by injecting 0s between input samples
 * and then lowpass-filtered by the FIR coefficients given above. Two FIR convolutions
 * are calculated, one for out1 and out2.
 * 
 * For out1 calculation, only the even coefficients matter, because the samples at the
 * odd coefficients are zero by upsampling. Only the 8th coefficient is 1 and
 * therefore it consists of only one value, the input delayed by 3 samples.
 *
 * For out2 calculation, the sample itself is zero by upsampling, so 8th term is
 * ignored, however the other terms now evaluate to real input samples. Therefore
 * computing it looks more complicated and all the other FIR terms are needed.
 * out2 is "delayed" by 3.5 samples.
 */
void
fir_interpolate_2x(DspFloatType *history, DspFloatType in, DspFloatType *out1, DspFloatType *out2)
{
    *out1 = history[2];
    *out2 = 0.6147129043790 * (history[2] + history[3])
          - 0.1534261286990 * (history[1] + history[4])
          + 0.0485966537018 * (history[0] + history[5])
          - 0.0103402076432 * (in         + history[6]);

    /* might be possible to optimize this, but I'm not sure it's worth it. */
    history[6] = history[5];
    history[5] = history[4];
    history[4] = history[3];
    history[3] = history[2];
    history[2] = history[1];
    history[1] = history[0];
    history[0] = in;
}

/* The same coefficients as before can be used to calculate the decimation. In
 * decimating direction, the input consists of two samples and the output is one sample.
 * The two samples enter the history buffer, and are expected to follow this sequence:
 * 
 *   in1, in2, history[0], history[1], history[2], history[3], ...
 * 
 * It would also be possible to calculate decimator by reusing the interpolator code
 * and ignoring out1, but this is more efficient. The decimator also delays input by
 * 1.5 samples in output rate. */
DspFloatType
fir_decimate_2x(DspFloatType *history, DspFloatType in1, DspFloatType in2)
{
    DspFloatType out;
    
    out = 0.6147129043790 * (history[1] + history[2])
        - 0.1534261286990 * (history[0] + history[3])
        + 0.0485966537018 * (in2        + history[4])
        - 0.0103402076432 * (in1        + history[5]);
    
    history[4] = history[2];
    history[2] = history[0];
    history[0] = in1;
    
    history[5] = history[3];
    history[3] = history[1];
    history[1] = in2;

    return out;
}