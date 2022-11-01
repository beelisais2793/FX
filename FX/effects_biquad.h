#pragma once
#include "Undenormal.hpp"
#include <math.h>

#define MAX_CHANNELS 2

namespace  HammerFX 
{
typedef struct {
    DspFloatType b[4];
    DspFloatType mem[MAX_CHANNELS][4];
    DspFloatType b0;
} Biquad_t;

typedef struct {
    Biquad_t        a1[MAX_CHANNELS], a2[MAX_CHANNELS];
    DspFloatType      x0_tmp[MAX_CHANNELS];
} Hilbert_t;

#ifdef __cplusplus
extern "C" {
#endif

/* Fs = sampling rate, Fc = center frequency, BW = bandwidth (octaves),
 * G = gain (dB), ripple = 0=butterw, 1-100=cheb (s-domain ellipticity %),
 * delay = unitless 0 .. 1, lowpass = flag whether cheb is lowpass filter */
void     set_peq_biquad(DspFloatType Fs, DspFloatType Fc, DspFloatType BW, DspFloatType G, Biquad_t *f);
void     set_bpf_biquad(DspFloatType Fs, DspFloatType Fc, DspFloatType BW, Biquad_t *f);
void     set_lpf_biquad(DspFloatType Fs, DspFloatType Fc, DspFloatType BW, Biquad_t *f);
void     set_phaser_biquad(DspFloatType delay, Biquad_t *f);
void     set_2nd_allpass_biquad(DspFloatType delay, Biquad_t *f);
void     set_rc_lowpass_biquad(DspFloatType Fs, DspFloatType Fc, Biquad_t *f);
void     set_rc_highpass_biquad(DspFloatType Fs, DspFloatType Fc, Biquad_t *f);
void     set_lsh_biquad(DspFloatType Fs, DspFloatType Fc, DspFloatType G, Biquad_t *f);
void     set_hsh_biquad(DspFloatType Fs, DspFloatType Fc, DspFloatType G, Biquad_t *f);
void     set_chebyshev1_biquad(DspFloatType Fs, DspFloatType Fc, DspFloatType ripple,
			       int lowpass, Biquad_t *f);

void     hilbert_transform(DspFloatType in, DspFloatType *x0, DspFloatType *x1, Hilbert_t *h, int curr_channel);
void     hilbert_init(Hilbert_t *h);
void     fir_interpolate_2x(DspFloatType *mem, DspFloatType in, DspFloatType *o1, DspFloatType *o2);
DspFloatType fir_decimate_2x(DspFloatType *mem, DspFloatType in1, DspFloatType in2);

#ifdef __cplusplus
}
#endif


inline DspFloatType do_biquad(DspFloatType x, Biquad_t *f, int c)
{
    Undenormal denormal;
    DspFloatType *mem = f->mem[c], y;
    y = x * f->b0 + mem[0] * f->b[0] + mem[1] * f->b[1]
        + mem[2] * f->b[2] + mem[3] * f->b[3];
    if (isnan(y))
        y=0;
    mem[1] = mem[0];
    mem[0] = x;
    mem[3] = mem[2];
    mem[2] = y;
    return y;
}
}