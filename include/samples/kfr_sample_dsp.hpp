#pragma once 


#include <complex>

#include <vector>
#include <cstring>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cstring>

#include "samples/kfr_sample.hpp"
#include "AudioFFT/AudioFFT.h"
#include "AudioFFT/FFTConvolver.h"

#define KFR_NO_C_COMPLEX_TYPES
#include "kfr/capi.h"
#include <fftw3.h>

namespace KfrDSP1
{    

    ////////////////////////////////////////////////////////////////
    // FFTW Complex 2 Complex
    ////////////////////////////////////////////////////////////////
    struct C2CD
    {
        fftw_complex * in;    
        fftw_complex * out;
        size_t size;
        fftw_plan p;

        enum Direction {
            BACKWARD= FFTW_BACKWARD,
            FORWARD = FFTW_FORWARD,
        };

        C2CD(size_t n, Direction dir = FORWARD) {
            in = fftw_alloc_complex(n);
            out= fftw_alloc_complex(n);        
            size = n;
            p = fftw_plan_dft_1d(n, in, out, dir, FFTW_ESTIMATE);
        }
        ~C2CD() {
            fftw_destroy_plan(p);
            fftw_free(in);
            fftw_free(out);    
        }
        void set_input(std::vector<std::complex<double>> & input) {
            for(size_t i = 0; i < size; i++) {
                in[i][0] = input[i].real();
                in[i][1] = input[i].imag();
            }
        }
        std::vector<std::complex<double>> get_output() {
            std::vector<std::complex<double>> r(size);
            for(size_t i = 0; i < size; i++ )
            {
                r[i].real(out[i][0]);
                r[i].imag(out[i][1]);
            }
            return r;
        }
        void Execute() {
            fftw_execute(p);
        }
    };

    struct C2CF
    {
        fftwf_complex * in;    
        fftwf_complex * out;
        size_t size;
        fftwf_plan p;

        enum Direction {
            BACKWARD=FFTW_BACKWARD,
            FORWARD=FFTW_FORWARD,
        };

        C2CF(size_t n, Direction dir = FORWARD) {
            in = fftwf_alloc_complex(n);
            out= fftwf_alloc_complex(n);        
            size = n;
            p = fftwf_plan_dft_1d(n, in, out, dir, FFTW_ESTIMATE);
        }
        ~C2CF() {
            fftwf_destroy_plan(p);
            fftwf_free(in);
            fftwf_free(out);    
        }
        void set_input(std::vector<std::complex<float>> & input) {
            for(size_t i = 0; i < size; i++) {
                in[i][0] = input[i].real();
                in[i][1] = input[i].imag();
            }
        }
        std::vector<std::complex<float>> get_output() {
            std::vector<std::complex<float>> r(size);
            for(size_t i = 0; i < size; i++ )
            {
                r[i].real(out[i][0]);
                r[i].imag(out[i][1]);
            }
            return r;
        }
        void Execute() {
            fftwf_execute(p);
        }
    };

    ////////////////////////////////////////////////////////////////
    // FFTW Complex 2 Real
    ////////////////////////////////////////////////////////////////
    struct C2RD
    {
        fftw_complex * in;    
        double * out;
        size_t size;
        fftw_plan p;

        C2RD(size_t n) {
            in = fftw_alloc_complex(n);
            out= fftw_alloc_real(n);        
            size = n;
            p = fftw_plan_dft_c2r_1d(n, in, out, FFTW_ESTIMATE);
        }
        ~C2RD() {
            fftw_destroy_plan(p);
            fftw_free(in);
            fftw_free(out);    
        }
        void set_input(std::vector<std::complex<double>> & input) {
            for(size_t i = 0; i < i; i++) {
                in[i][0] = input[i].real();
                in[i][1] = input[i].imag();
            }
        }
        std::vector<double> get_output() {
            std::vector<double> r(size);
            memcpy(r.data(),out, size * sizeof(double));
            return r;
        }
        void Execute() {
            fftw_execute(p);
        }
    };

    struct C2RF
    {
        fftwf_complex * in;    
        float * out;
        size_t size;
        fftwf_plan p;

        C2RF(size_t n) {
            in = fftwf_alloc_complex(n);
            out= fftwf_alloc_real(n);        
            size = n;
            p = fftwf_plan_dft_c2r_1d(n, in, out, FFTW_ESTIMATE);
        }
        ~C2RF() {
            fftwf_destroy_plan(p);
            fftwf_free(in);
            fftwf_free(out);    
        }
        void set_input(std::vector<std::complex<float>> & input) {
            for(size_t i = 0; i < i; i++) {
                in[i][0] = input[i].real();
                in[i][1] = input[i].imag();
            }
        }
        std::vector<float> get_output() {
            std::vector<float> r(size);
            memcpy(r.data(),out, size*sizeof(float));
            return r;
        }
        void Execute() {
            fftwf_execute(p);
        }
    };


    ////////////////////////////////////////////////////////////////
    // FFTW Real 2 Complex
    ////////////////////////////////////////////////////////////////
    struct R2CD
    {
        double       * in;    
        fftw_complex * out;
        size_t size;
        fftw_plan p;

        R2CD(size_t n) {
            in = fftw_alloc_real(n);
            out= fftw_alloc_complex(n);        
            size = n;
            p = fftw_plan_dft_r2c_1d(n, in, out, FFTW_ESTIMATE);
        }
        ~R2CD() {
            fftw_destroy_plan(p);
            fftw_free(in);
            fftw_free(out);    
        }
        void set_input(std::vector<double> & input) {
            memcpy(in,input.data(),size*sizeof(double));
        }
        std::vector<std::complex<double>> get_output() {
            std::vector<std::complex<double>> r(size);
            for(size_t i = 0; i < size; i++) {
                r[i].real(out[i][0]);
                r[i].imag(out[i][1]);
            }
            return r;
        }
        void Execute() {
            fftw_execute(p);
        }
    };

    struct R2CF
    {
        float * in;    
        fftwf_complex * out;
        size_t size;
        fftwf_plan p;

        R2CF(size_t n) {
            in = fftwf_alloc_real(n);
            out= fftwf_alloc_complex(n);        
            size = n;
            p = p = fftwf_plan_dft_r2c_1d(n, in, out, FFTW_ESTIMATE);
        }
        ~R2CF() {
            fftwf_destroy_plan(p);
            fftwf_free(in);
            fftwf_free(out);    
        }
        void set_input(std::vector<float> & input) {
            memcpy(in,input.data(),size*sizeof(float));
        }
        std::vector<std::complex<float>> get_output() {
            std::vector<std::complex<float>> r(size);
            for(size_t i = 0; i < size; i++) {
                r[i].real(out[i][0]);
                r[i].imag(out[i][1]);
            }
                
            return r;
        }    
        void Execute() {
            fftwf_execute(p);
        }
    };

    ////////////////////////////////////////////////////////////////
    // FFTW R2R DCT/DST
    ////////////////////////////////////////////////////////////////
    enum R2RKind
    {
        DCTI = FFTW_REDFT00,
        DCTII= FFTW_REDFT01,
        DCTIII=FFTW_REDFT10,
        DCTIV=FFTW_REDFT11,
        DSTI=FFTW_REDFT11,
        DSTII=FFTW_REDFT00,
        DSTIII=FFTW_REDFT10,
        DSTIV=FFTW_REDFT11,
    };

    struct R2RD
    {
        double       * in;    
        double       * out;
        size_t size;
        fftw_plan p;

        R2RD(size_t n, R2RKind type = DCTI) {
            in = fftw_alloc_real(n);
            out= fftw_alloc_real(n);        
            size = n;
            p = fftw_plan_r2r_1d(n, in, out, (fftw_r2r_kind)type, FFTW_ESTIMATE);
        }
        ~R2RD() {
            fftw_destroy_plan(p);
            fftw_free(in);
            fftw_free(out);    
        }
        void set_input(std::vector<double> & input) {
            memcpy(in,input.data(),size*sizeof(double));
        }
        std::vector<double> get_output() {
            std::vector<double> r(size);
            for(size_t i = 0; i < size; i++) {
                r[i] = out[i];                
            }
            return r;
        }
        void Execute() {
            fftw_execute(p);
        }
    };

    struct R2RF
    {
        float * in;    
        float * out;
        size_t size;
        fftwf_plan p;

        R2RF(size_t n, R2RKind type = DCTI) {
            in = fftwf_alloc_real(n);
            out= fftwf_alloc_real(n);        
            size = n;
            p = p = fftwf_plan_r2r_1d(n, in, out, (fftw_r2r_kind)type, FFTW_ESTIMATE);
        }
        ~R2RF() {
            fftwf_destroy_plan(p);
            fftwf_free(in);
            fftwf_free(out);    
        }
        void set_input(std::vector<float> & input) {
            memcpy(in,input.data(),size*sizeof(float));
        }
        std::vector<float> get_output() {
            std::vector<float> r(size);
            for(size_t i = 0; i < size; i++) {
                r[i] = out[i];                
            }
                
            return r;
        }    
        void Execute() {
            fftwf_execute(p);
        }
    };


    ////////////////////////////////////////////////////////////////
    // FFTW Convolution
    ////////////////////////////////////////////////////////////////    
    std::vector<float> FFTW_Convolution(std::vector<float> & xin, std::vector<float> & yout)
    {
        int M = xin.size();
        int N = yout.size();
        float x[M] = {0,1,0,0};
        float y[N] = {0,0,1,0,0,0,0,0};
        float in_a[M+N-1];
        std::complex<float> out_a[M+N-1];
        float in_b[M+N-1];
        std::complex<float> out_b[M+N-1];
        std::complex<float> in_rev[M+N-1];
        std::vector<float> out_rev(M+N-1);

        // Plans for forward FFTs
        fftwf_plan plan_fwd_a = fftwf_plan_dft_r2c_1d (M+N-1, in_a, reinterpret_cast<fftwf_complex*>(&out_a), FFTW_MEASURE);
        fftwf_plan plan_fwd_b = fftwf_plan_dft_r2c_1d (M+N-1, in_b, reinterpret_cast<fftwf_complex*>(&out_b), FFTW_MEASURE);

        // Plan for reverse FFT
        fftwf_plan plan_rev = fftwf_plan_dft_c2r_1d (M+N-1,reinterpret_cast<fftwf_complex*>(&in_rev[0]), out_rev.data(), FFTW_MEASURE);

        // Prepare padded input data
        std::memcpy(in_a, xin.data(), sizeof(float) * M);
        std::memset(in_a + M, 0, sizeof(float) * (N-1));
        std::memset(in_b, 0, sizeof(float) * (M-1));
        std::memcpy(in_b + (M-1), yout.data(), sizeof(float) * N);
        
        // Calculate the forward FFTs
        fftwf_execute(plan_fwd_a);
        fftwf_execute(plan_fwd_b);

        // Multiply in frequency domain
        for( int idx = 0; idx < M+N-1; idx++ ) {
            in_rev[idx] = out_a[idx] * out_b[idx];
        }

        // Calculate the backward FFT
        fftwf_execute(plan_rev);

        // Clean up
        fftwf_destroy_plan(plan_fwd_a);
        fftwf_destroy_plan(plan_fwd_b);
        fftwf_destroy_plan(plan_rev);

        return out_rev;
    }

    ////////////////////////////////////////////////////////////////
    // FFTW Deconvolution
    ////////////////////////////////////////////////////////////////
    std::vector<float> FFTW_Deconvolution(std::vector<float> & xin, std::vector<float> & yout)
    {
        int M = xin.size();
        int N = yout.size();
        float x[M] = {0,1,0,0};
        float y[N] = {0,0,1,0,0,0,0,0};
        float in_a[M+N-1];
        std::complex<float> out_a[M+N-1];
        float in_b[M+N-1];
        std::complex<float> out_b[M+N-1];
        std::complex<float> in_rev[M+N-1];
        std::vector<float> out_rev(M+N-1);

        // Plans for forward FFTs
        fftwf_plan plan_fwd_a = fftwf_plan_dft_r2c_1d (M+N-1, in_a, reinterpret_cast<fftwf_complex*>(&out_a), FFTW_MEASURE);
        fftwf_plan plan_fwd_b = fftwf_plan_dft_r2c_1d (M+N-1, in_b, reinterpret_cast<fftwf_complex*>(&out_b), FFTW_MEASURE);

        // Plan for reverse FFT
        fftwf_plan plan_rev = fftwf_plan_dft_c2r_1d (M+N-1,reinterpret_cast<fftwf_complex*>(&in_rev[0]), out_rev.data(), FFTW_MEASURE);

        // Prepare padded input data
        std::memcpy(in_a, xin.data(), sizeof(float) * M);
        std::memset(in_a + M, 0, sizeof(float) * (N-1));
        std::memset(in_b, 0, sizeof(float) * (M-1));
        std::memcpy(in_b + (M-1), yout.data(), sizeof(float) * N);
        
        // Calculate the forward FFTs
        fftwf_execute(plan_fwd_a);
        fftwf_execute(plan_fwd_b);

        // Multiply in frequency domain
        for( int idx = 0; idx < M+N-1; idx++ ) {
            in_rev[idx] = out_a[idx] / out_b[idx];
        }

        // Calculate the backward FFT
        fftwf_execute(plan_rev);

        // Clean up
        fftwf_destroy_plan(plan_fwd_a);
        fftwf_destroy_plan(plan_fwd_b);
        fftwf_destroy_plan(plan_rev);

        return out_rev;
    }

    ////////////////////////////////////////////////////////////////
    // FFTW Cross Correlation
    ////////////////////////////////////////////////////////////////
    std::vector<float> FFTW_CrossCorrelation(std::vector<float> & xin, std::vector<float> & yout)
    {
        int M = xin.size();
        int N = yout.size();
        float x[M] = {0,1,0,0};
        float y[N] = {0,0,1,0,0,0,0,0};
        float in_a[M+N-1];
        std::complex<float> out_a[M+N-1];
        float in_b[M+N-1];
        std::complex<float> out_b[M+N-1];
        std::complex<float> in_rev[M+N-1];
        std::vector<float> out_rev(M+N-1);

        // Plans for forward FFTs
        fftwf_plan plan_fwd_a = fftwf_plan_dft_r2c_1d (M+N-1, in_a, reinterpret_cast<fftwf_complex*>(&out_a), FFTW_MEASURE);
        fftwf_plan plan_fwd_b = fftwf_plan_dft_r2c_1d (M+N-1, in_b, reinterpret_cast<fftwf_complex*>(&out_b), FFTW_MEASURE);

        // Plan for reverse FFT
        fftwf_plan plan_rev = fftwf_plan_dft_c2r_1d (M+N-1,reinterpret_cast<fftwf_complex*>(&in_rev[0]), out_rev.data(), FFTW_MEASURE);

        // Prepare padded input data
        std::memcpy(in_a, xin.data(), sizeof(float) * M);
        std::memset(in_a + M, 0, sizeof(float) * (N-1));
        std::memset(in_b, 0, sizeof(float) * (M-1));
        std::memcpy(in_b + (M-1), yout.data(), sizeof(float) * N);
        
        // Calculate the forward FFTs
        fftwf_execute(plan_fwd_a);
        fftwf_execute(plan_fwd_b);

         // Multiply in frequency domain
        for( int idx = 0; idx < M+N-1; idx++ ) {
            in_rev[idx] = std::conj(out_a[idx]) * out_b[idx]/(float)(M+N-1);
        }

        // Calculate the backward FFT
        fftwf_execute(plan_rev);

        // Clean up
        fftwf_destroy_plan(plan_fwd_a);
        fftwf_destroy_plan(plan_fwd_b);
        fftwf_destroy_plan(plan_rev);

        return out_rev;
    }


    ////////////////////////////////////////////////////////////////
    // FFTW Resampler
    ////////////////////////////////////////////////////////////////

    struct FFTResampler
    {
        int inFrameSize;
        int inWindowSize;
        int inSampleRate;
        float *inWindowing;
        fftwf_plan inPlan;
        int outFrameSize;
        int outWindowSize;
        int outSampleRate;
        float *outWindowing;
        fftwf_plan outPlan;
        float *inFifo;
        float *synthesisMem;
        fftwf_complex *samples;
        int pos;
       

        FFTResampler(size_t inSampleRate, size_t outSampleRate, size_t nFFT)
        {
            
            pos = 0;
            if (outSampleRate < inSampleRate) {
                nFFT = nFFT * inSampleRate * 128 / outSampleRate;
            }
            else {
                nFFT = nFFT * outSampleRate * 128 / inSampleRate;
            }
            nFFT += (nFFT % 2);

            inFrameSize = nFFT;
            inWindowSize = nFFT * 2;
            inSampleRate = inSampleRate;
            outSampleRate = outSampleRate;
            outFrameSize = inFrameSize * outSampleRate / inSampleRate;
            outWindowSize = outFrameSize * 2;        

            outWindowing = (float *) fftwf_alloc_real(outFrameSize);
            inFifo = (float *) fftwf_alloc_real(std::max(inWindowSize, outWindowSize));
            samples = (fftwf_complex *) fftwf_alloc_complex(std::max(inWindowSize, outWindowSize));
            inWindowing = (float *) fftwf_alloc_real(inFrameSize);
            synthesisMem = (float *) fftwf_alloc_real(outFrameSize);
                    
            inPlan = fftwf_plan_dft_r2c_1d(inWindowSize,inFifo,samples,FFTW_ESTIMATE);        
            outPlan = fftwf_plan_dft_c2r_1d(outWindowSize,samples,synthesisMem,FFTW_ESTIMATE);
            
            if ((inFifo == NULL) || (inPlan == NULL) || (outPlan == NULL)
                || (samples == NULL)
                || (synthesisMem == NULL) || (inWindowing == NULL) || (outWindowing == NULL)
                ) {
                    assert(1==0);
            }
            float norm = 1.0f / inWindowSize;
            for (size_t i = 0; i < inFrameSize; i++) {
                double t = std::sin(.5 * M_PI * (i + .5) / inFrameSize);
                inWindowing[i] = (float) std::sin(.5 * M_PI * t * t) * norm;
            }
            for (size_t i = 0; i < outFrameSize; i++) {
                double t = std::sin(.5 * M_PI * (i + .5) / outFrameSize);
                outWindowing[i] = (float) std::sin(.5 * M_PI * t * t);
            }    
        }
        
        ~FFTResampler()
        {   
            if (inFifo) {
                free(inFifo);
                inFifo = NULL;
            }

            if (inPlan) {
                fftwf_destroy_plan(inPlan);
                inPlan = NULL;
            }

            if (outPlan) {
                fftwf_destroy_plan(outPlan);
                outPlan = NULL;
            }

            if (samples) {
                fftw_free(samples);
                samples = NULL;
            }

            if (synthesisMem) {
                fftw_free(synthesisMem);
                synthesisMem = NULL;
            }

            if (inWindowing) {
                fftw_free(inWindowing);
                inWindowing = NULL;
            }

            if (outWindowing) {
                fftw_free(outWindowing);
                outWindowing = NULL;
            }    
        }

        void reset()
        {        
            pos = 0;
        }

        

        int Process(const float *input, float *output)
        {
            if ((input == NULL) || (output == NULL)) {
                return -1;
            }
            float *inFifo = inFifo;
            float *synthesis_mem = synthesisMem;
            for (size_t i = 0; i < inFrameSize; i++) {
                inFifo[i] *= inWindowing[i];
                inFifo[inWindowSize - 1 - i] = input[inFrameSize - 1 - i] * inWindowing[i];
            }
            fftwf_execute(inPlan);
            if (outWindowSize < inWindowSize) {
                int half_output = (outWindowSize / 2);
                int diff_size = inWindowSize - outWindowSize;
                memset(samples + half_output, 0, diff_size * sizeof(fftw_complex));
            }
            else if (outWindowSize > inWindowSize) {
                int half_input = inWindowSize / 2;
                int diff_size = outWindowSize - inWindowSize;
                memmove(samples + half_input + diff_size, samples + half_input,
                        half_input * sizeof(fftw_complex));
                memset(samples + half_input, 0, diff_size * sizeof(fftw_complex));
            }
            fftwf_execute(outPlan);
            for (size_t i = 0; i < outFrameSize; i++) {
                output[i] = inFifo[i] * outWindowing[i] + synthesis_mem[i];
                inFifo[outWindowSize - 1 - i] *= outWindowing[i];
            }
            memcpy(synthesis_mem, inFifo + outFrameSize, outFrameSize * sizeof(float));
            memcpy(inFifo, input, inFrameSize * sizeof(float));
            if (pos == 0) {
                pos++;
                return 0;
            }
            pos++;
            return 1;
        }
    };

    ////////////////////////////////////////////////////////////////
    // KFR Complex DFT
    ////////////////////////////////////////////////////////////////
    struct CDFT
    {
        KFR_DFT_PLAN_F32 * plan;
        size_t size;

        CDFT(size_t n) {
            plan = kfr_dft_create_plan_f32(n);
            size = n;
        }
        ~CDFT() {
            if(plan) kfr_dft_delete_plan_f32(plan);
        }

        void forward(std::vector<std::complex<float>> & input, std::vector<std::complex<float>> & output)
        {
            // you cant do anything with it even if you include the c complex.h
            std::vector<kfr_c32> c_in(size*2),c_out(size*2);
            std::vector<uint8_t> temp(kfr_dft_get_temp_size_f32(plan));
            memcpy(c_in.data(),input.data(),input.size() * sizeof(kfr_c32));
            kfr_dft_execute_f32(plan,c_out.data(),c_in.data(),temp.data());
            output.resize(size);
            memcpy(output.data(),c_out.data(),size * sizeof(kfr_c32));
        }
        void inverse(std::vector<std::complex<float>> & input, std::vector<std::complex<float>> & output)
        {
            std::vector<kfr_c32> c_in(size*2),c_out(size*2);
            std::vector<uint8_t> temp(kfr_dft_get_temp_size_f32(plan));
            memcpy(c_in.data(),input.data(),input.size() * sizeof(kfr_c32));
            kfr_dft_execute_inverse_f32(plan,c_out.data(),c_in.data(),temp.data());
            output.resize(size);
            memcpy(output.data(),c_out.data(),size * sizeof(kfr_c32));
        }
        void dump() 
        {
            kfr_dft_dump_f32(plan);
        }
    };

    struct CDFT64
    {
        KFR_DFT_PLAN_F64 * plan;
        size_t size;

        CDFT64(size_t n) {
            plan = kfr_dft_create_plan_f64(n);
            size = n;
        }
        ~CDFT64() {
            if(plan) kfr_dft_delete_plan_f64(plan);
        }

        void forward(std::vector<std::complex<double>> & input, std::vector<std::complex<double>> & output)
        {
            std::vector<kfr_c64> c_in(size*2),c_out(size*2);
            std::vector<uint8_t> temp(kfr_dft_get_temp_size_f64(plan));
            memcpy(c_in.data(),input.data(),input.size() * sizeof(kfr_c64));
            kfr_dft_execute_f64(plan,c_out.data(),c_in.data(),temp.data());
            output.resize(size);
            memcpy(output.data(),c_out.data(),size * sizeof(kfr_c64));
        }
        void inverse(std::vector<std::complex<double>> & input, std::vector<std::complex<double>> & output)
        {
            std::vector<kfr_c64> c_in(size*2),c_out(size*2);
            std::vector<uint8_t> temp(kfr_dft_get_temp_size_f64(plan));
            memcpy(c_in.data(),input.data(),input.size() * sizeof(kfr_c64));
            kfr_dft_execute_inverse_f64(plan,c_out.data(),c_in.data(),temp.data());
            output.resize(size);
            memcpy(output.data(),c_out.data(),size * sizeof(kfr_c64));
        }
        void dump() 
        {
            kfr_dft_dump_f64(plan);
        }
    };

    ////////////////////////////////////////////////////////////////
    // Kfr Real DFT
    ////////////////////////////////////////////////////////////////
    struct RDFT
    {
        KFR_DFT_REAL_PLAN_F32 * plan;
        size_t size;

        RDFT(size_t n) {
            plan = kfr_dft_real_create_plan_f32(n,Perm);
            size = n;
        }
        ~RDFT() {
            if(plan) kfr_dft_real_delete_plan_f32(plan);
        }

        void forward(std::vector<float> & input, std::vector<std::complex<float>> & output)
        {
            std::vector<kfr_c32> c_out(size*2);
            std::vector<uint8_t> temp(kfr_dft_real_get_temp_size_f32(plan));            
            kfr_dft_real_execute_f32(plan,c_out.data(),input.data(),temp.data());
            output.resize(size);
            memcpy(output.data(),c_out.data(),size * sizeof(kfr_c32));
        }
        void inverse(std::vector<std::complex<float>> & input, std::vector<float> & output)
        {
            std::vector<kfr_c32> c_in(size*2);
            std::vector<uint8_t> temp(kfr_dft_real_get_temp_size_f32(plan));
            memcpy(c_in.data(),input.data(),input.size() * sizeof(kfr_c32));
            kfr_dft_real_execute_inverse_f32(plan,output.data(),c_in.data(),temp.data());            
        }
        void dump() 
        {
            kfr_dft_real_dump_f32(plan);
        }
    };
    struct RDFT64
    {
        KFR_DFT_REAL_PLAN_F64 * plan;
        size_t size;

        RDFT64(size_t n) {
            plan = kfr_dft_real_create_plan_f64(n,Perm);
            size = n;
        }
        ~RDFT64() {
            if(plan) kfr_dft_real_delete_plan_f64(plan);
        }

        void forward(std::vector<double> & input, std::vector<std::complex<double>> & output)
        {
            std::vector<kfr_c64> c_out(size*2);
            std::vector<uint8_t> temp(kfr_dft_real_get_temp_size_f64(plan));            
            kfr_dft_real_execute_f64(plan,c_out.data(),input.data(),temp.data());
            output.resize(size);
            memcpy(output.data(),c_out.data(),size * sizeof(kfr_c64));
        }
        void inverse(std::vector<std::complex<double>> & input, std::vector<double> & output)
        {
            std::vector<kfr_c64> c_in(size*2);
            std::vector<uint8_t> temp(kfr_dft_real_get_temp_size_f64(plan));
            memcpy(c_in.data(),input.data(),input.size() * sizeof(kfr_c64));
            kfr_dft_real_execute_inverse_f64(plan,output.data(),c_in.data(),temp.data());            
        }
        void dump() 
        {
            kfr_dft_real_dump_f64(plan);
        }
    };

    
    ////////////////////////////////////////////////////////////////
    // Kfr FIR
    ////////////////////////////////////////////////////////////////
    struct FIR
    {
        KFR_FILTER_F32 * plan;

        FIR(const float * taps, size_t n) {
            kfr_filter_create_fir_plan_f32(taps,n);
        }
        ~FIR()
        {
            if(plan) kfr_filter_delete_plan_f32(plan);
        }
        void Process(size_t n, float * input, float * output) {
            kfr_filter_process_f32(plan,output,input,n);
        }
    };

    struct FIR64
    {
        KFR_FILTER_F64 * plan;
        FIR64(const double * taps, size_t n) {
            kfr_filter_create_fir_plan_f64(taps,n);
        }
        ~FIR64()
        {
            if(plan) kfr_filter_delete_plan_f64(plan);
        }
        void Process(size_t n, double * input, double * output) {
            kfr_filter_process_f64(plan,output,input,n);
        }
    };

    ////////////////////////////////////////////////////////////////
    // Kfr Convolution
    ////////////////////////////////////////////////////////////////
    struct CONV
    {
        KFR_FILTER_F32 * plan;
        CONV(const float * taps, size_t n, size_t block) {
            kfr_filter_create_convolution_plan_f32(taps,n,block);
        }
        ~CONV()
        {
            if(plan) kfr_filter_delete_plan_f32(plan);
        }
        void Process(size_t n, float * input, float * output) {
            kfr_filter_process_f32(plan,output,input,n);
        }
    };

    struct CONV64
    {
        KFR_FILTER_F64 * plan;
        CONV64(const double * taps, size_t n, size_t block) {
            kfr_filter_create_convolution_plan_f64(taps,n,block);
        }
        ~CONV64()
        {
            if(plan) kfr_filter_delete_plan_f64(plan);
        }
        void Process(size_t n, double * input, double * output) {
            kfr_filter_process_f64(plan,output,input,n);
        }
    };

    ////////////////////////////////////////////////////////////////
    // Kfr IIR
    ////////////////////////////////////////////////////////////////
    struct IIR
    {
        KFR_FILTER_F32 * plan;
        IIR(const float * sos, size_t n) {
            kfr_filter_create_iir_plan_f32(sos,n);
        }
        ~IIR()
        {
            if(plan) kfr_filter_delete_plan_f32(plan);
        }
        void Process(size_t n, float * input, float * output) {
            kfr_filter_process_f32(plan,output,input,n);
        }
    };

    struct IIR64
    {
        KFR_FILTER_F64 * plan;
        IIR64(const double * sos, size_t n) {
            kfr_filter_create_iir_plan_f64(sos,n);
        }
        ~IIR64()
        {
            if(plan) kfr_filter_delete_plan_f64(plan);
        }
        void Process(size_t n, double * input, double * output) {
            kfr_filter_process_f64(plan,output,input,n);
        }
    };

    
    
}