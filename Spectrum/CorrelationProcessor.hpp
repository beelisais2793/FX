#pragma once

#include <fftw3.h>

// sola = https://github.com/srheaume/sola
// psola
// pitch-detection = autocorrelation
// libxcorr = cross correlation

struct XCorr
{
    fftwf_plan plan_fwd_a;
    fftwf_plan plan_fwd_b;
    fftwf_plan plan_rev;

    size_t M,N;
    float *in_a;
    float *in_b;
    float *out_rev;
    std::complex<float> *out_a[M+N-1];    
    std::complex<float> *out_b[M+N-1];
    std::complex<float> *in_rev[M+N-1];

    XCorr(size_t m, size_t n) {
        M = m;
        N = n;
    
        in_a = fftwf_alloc_real(M+N-1);
        in_b = fftwf_alloc_real(M+N-1);
        out_rev = fftwf_alloc_real(M+N-1);

        out_a = fftwf_alloc_complex(M+N-1);
        out_b = fftwf_alloc_complex(M+N-1);
        in_rev= fftwf_alloc_complex(M+N-1);

        // Plans for forward FFTs
        plan_fwd_a = fftwf_plan_dft_r2c_1d (M+N-1, in_a,
            reinterpret_cast<fftwf_complex*>(&out_a), FFTW_MEASURE);
        plan_fwd_b = fftwf_plan_dft_r2c_1d (M+N-1, in_b,
            reinterpret_cast<fftwf_complex*>(&out_b), FFTW_MEASURE);

        // Plan for reverse FFT
        plan_rev = fftwf_plan_dft_c2r_1d (M+N-1, reinterpret_cast<fftwf_complex*>(&in_rev), out_rev, FFTW_MEASURE);
    }
    ~XCorr() {
        if(in_a) fftwf_free(in_a);
        if(in_b) fftwf_free(in_b);
        if(out_rev) fftwf_free(out_rev);
        if(out_a) fftwf_free(out_a);
        if(out_b) fftwf_free(out_b);
        if(in_rev) fftwf_free(in_rev);
        fftwf_destroy_plan(plan_fwd_a);
        fftwf_destroy_plan(plan_fwd_b);
        fftwf_destroy_plan(plan_rev);
    }
    void Process(float * x, float * y, float * out)
    {    
        // Prepare padded input data
        std::memcpy(in_a, x, sizeof(float) * M);
        std::memset(in_a + M, 0, sizeof(float) * (N-1));
        std::memset(in_b, 0, sizeof(float) * (M-1));
        std::memcpy(in_b + (M-1), y, sizeof(float) * N);

        // Calculate the forward FFTs
        fftwf_execute(plan_fwd_a);
        fftwf_execute(plan_fwd_b);

        // Multiply in frequency domain
        for( int idx = 0; idx < M+N-1; idx++ ) {
            in_rev[idx] = std::conj(out_a[idx]) * out_b[idx]/(float)(M+N-1);
        }

        // Calculate the backward FFT
        fftwf_execute(plan_rev);

        memcpy(out, out_rev,(M+N-1)*sizeof(float));        
    }
}


void FFTW_XCorr(double * signala, double * signalb, double * result, int N)
{
    fftw_complex * signala_ext = (fftw_complex *) fftw_malloc(sizeof(fftw_complex) * (2 * N - 1));
    fftw_complex * signalb_ext = (fftw_complex *) fftw_malloc(sizeof(fftw_complex) * (2 * N - 1));
    fftw_complex * out_shifted = (fftw_complex *) fftw_malloc(sizeof(fftw_complex) * (2 * N - 1));
    fftw_complex * outa = (fftw_complex *) fftw_malloc(sizeof(fftw_complex) * (2 * N - 1));
    fftw_complex * outb = (fftw_complex *) fftw_malloc(sizeof(fftw_complex) * (2 * N - 1));
    fftw_complex * out = (fftw_complex *)  fftw_malloc(sizeof(fftw_complex) * (2 * N - 1));

    fftw_plan pa = fftw_plan_dft_1d(2 * N - 1, signala_ext, outa, FFTW_FORWARD, FFTW_ESTIMATE);
    fftw_plan pb = fftw_plan_dft_1d(2 * N - 1, signalb_ext, outb, FFTW_FORWARD, FFTW_ESTIMATE);
    fftw_plan px = fftw_plan_dft_1d(2 * N - 1, out, result, FFTW_BACKWARD, FFTW_ESTIMATE);

    //zeropadding
    memset (signala_ext, 0, sizeof(fftw_complex) * (N - 1));
    memcpy (signala_ext + (N - 1), signala, sizeof(fftw_complex) * N);
    memcpy (signalb_ext, signalb, sizeof(fftw_complex) * N);
    memset (signalb_ext + N, 0, sizeof(fftw_complex) * (N - 1));

    fftw_execute(pa);
    fftw_execute(pb);

    fftw_complex scale = 1.0/(2 * N -1);
    for (int i = 0; i < 2 * N - 1; i++)
        out[i] = outa[i] * conj(outb[i]) * scale;

    fftw_execute(px);

    fftw_destroy_plan(pa);
    fftw_destroy_plan(pb);
    fftw_destroy_plan(px);

    fftw_free(signala_ext);
    fftw_free(signalb_ext);
    fftw_free(out_shifted);
    fftw_free(out);
    fftw_free(outa);
    fftw_free(outb);

    fftw_cleanup();

    return;
}
 