#pragma once 

#include <complex>
#include <vector>
#include <cstring>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <cassert>

#include "samples/sample.hpp"
#include "AudioFFT/AudioFFT.h"
#include "AudioFFT/FFTConvolver.h"
#include <fftw3.h>

//#include "DSP/SimpleResampler.hpp"

//#define KFR_NO_C_COMPLEX_TYPES
//#include "kfr/capi.h"

#include <fftw3.h>

template<typename T>
using complex_vector = sample_vector<std::complex<T>>;

namespace DSP
{
    template<typename T>
    struct Window 
    {   
        sample_vector<T> window;

        Window(size_t i) { window.resize(i); }
        virtual ~Window() = default;

        T& operator[](size_t i) { return window[i]; }

        sample_vector<T> operator * (const sample_vector<T> & v) { return window * v; }
    };
    template<typename T>
    struct Rectangle: public Window<T>
    {
        Rectangle(size_t i) : Window<T>(i) { 
            fill(this->window,1.0f);
            } 
    };
    template<typename T>
    struct Hamming: public Window<T>
    {
        Hamming(size_t n) : Window<T>(n) {            
            for(size_t i = 0; i < this->window.size(); i++)
            {
                this->window[i] = 0.54 - (0.46 * std::cos(2*M_PI*(double)i/(double)n));
            }        
        }
    };
    template<typename T>
    struct Hanning: public Window<T>
    {
        Hanning(size_t n) : Window<T>(n) {            
            for(size_t i = 0; i < this->window.size(); i++)
            {
                this->window[i] = 0.5*(1 - std::cos(2*M_PI*(double)i/(double)n));
            }        
        }
    };
    template<typename T>
    struct Blackman: public Window<T>
    {
        Blackman(size_t n) : Window<T>(n)    
        {            
            for(size_t i = 0; i < this->window.size(); i++)                    
                this->window[i] = 0.42 - (0.5* std::cos(2*M_PI*i/(n)) + (0.08*std::cos(4*M_PI*i/n)));        
        }
    };
    template<typename T>
    struct BlackmanHarris: public Window<T>
    {
        BlackmanHarris(size_t n) : Window<T>(n)    
        {            
            for(size_t i = 0; i < this->window.size(); i++)            
            {   
                double ci = (double) i / (double) n;
                this->window[i] = 0.35875 
                        - 0.48829*std::cos(2*M_PI*(ci))
                        + 0.14128*std::cos(4.0*M_PI*(ci)) 
                        - 0.01168*std::cos(6.0*M_PI*(ci));
            }
        }
    };
    template<typename T>
    struct Gaussian: public Window<T>
    {
        Gaussian(size_t i) : Window<T>(i)
        {
            T a,b,c=0.5;
            for(size_t n = 0; n < this->window.size(); n++)
            {
                a = ((double)n - c*(this->window.size()-1)/(std::sqrt(c)*this->window.size()-1));
                b = -c * std::sqrt(a);
                this->window(n) = std::exp(b);
            }
        }
    };
    template<typename T>
    struct Welch: public Window<T>
    {
        Welch(size_t n) : Window<T>(n)
        {
            for(size_t i = 0; i < this->window.size(); i++)
                this->window[i] = 1.0 - std::sqrt((2.0*(double)i-(double)this->window.size()-1)/((double)this->window.size()));        
        }
    };
    template<typename T>
    struct Parzen: public Window<T>
    {

        Parzen(size_t n) : Window<T>(n)
        {
            for(size_t i = 0; i < this->window.size(); i++)
                this->window[i] = 1.0 - std::abs((2.0*(double)i-this->window.size()-1)/(this->window.size()));        
        }    
    };
    template<typename T>
    struct Tukey: public Window<T>
    {
        Tukey(size_t num_samples, T alpha) : Window<T>(num_samples)
        {            
            T value = (-1*(num_samples/2)) + 1;
            double n2 = (double)num_samples / 2.0;
            for(size_t i = 0; i < this->window.size(); i++)
            {    
                if(value >= 0 && value <= (alpha * (n2))) 
                    this->window[i] = 1.0; 
                else if(value <= 0 && (value >= (-1*alpha*(n2)))) 
                    this->vector.vector[i] = 1.0;
                else 
                    this->vector.vector[i] = 0.5 * (1 + std::cos(M_PI *(((2.0*value)/(alpha*(double)num_samples))-1)))        ;
                value = value + 1;
            }     
        }
    };

    
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
        void set_input(complex_vector<double> & input) {
            for(size_t i = 0; i < size; i++) {
                in[i][0] = input[i].real();
                in[i][1] = input[i].imag();
            }
        }
        void set_input(double * buffer) {
            memcpy(in,buffer,2*size*sizeof(double));
        }
        complex_vector<double> get_output() {
            complex_vector<double> r(size);
            for(size_t i = 0; i < size; i++ )
            {
                r[i].real(out[i][0]);
                r[i].imag(out[i][1]);
            }
            return r;
        }
        void get_output(double * output)
        {
            for(size_t i = 0; i < size; i++ )
            {
                output[2*i] = out[i][0];
                output[2*i+1] = out[i][1];
            }
        }
        void get_output(complex_vector<double>&  output)
        {
            if(output.size() != size) output.resize(size);
            for(size_t i = 0; i < size; i++ )
            {
                output[i].real(out[i][0]);
                output[i].imag(out[i][1]);
            }
        }
        void normalize() {
            for(size_t i = 0; i < size; i++) {
                out[i][0] /= (double)size;    
                out[i][1] /= (double)size;
            }
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
        void set_input(complex_vector<float> & input) {
            for(size_t i = 0; i < size; i++) {
                in[i][0] = input[i].real();
                in[i][1] = input[i].imag();
            }
        }
        void set_input(float * buffer) {
            memcpy(in,buffer,2*size*sizeof(float));
        }
        complex_vector<float> get_output() {
            complex_vector<float> r(size);
            for(size_t i = 0; i < size; i++ )
            {
                r[i].real(out[i][0]);
                r[i].imag(out[i][1]);
            }
            return r;
        }
        void get_output(float * output)
        {
            for(size_t i = 0; i < size; i++ )
            {
                output[2*i]   = out[i][0];
                output[2*i+1] = out[i][1];
            }
        }
        void get_output(complex_vector<float>& output)
        {
            if(output.size() != size) output.resize(size);
            for(size_t i = 0; i < size; i++ )
            {
                output[i].real(out[i][0]);
                output[i].imag(out[i][1]);
            }
        }
        void normalize() {
            for(size_t i = 0; i < size; i++) {
                out[i][0] /= (float)size;    
                out[i][1] /= (float)size;
            }
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

        C2RD() {
            in = NULL;
            out = NULL;
            size = 0;
        }
        C2RD(size_t n) {
            init(n);
        }
        ~C2RD() {
            fftw_destroy_plan(p);
            fftw_free(in);
            fftw_free(out);    
        }
        void init(size_t n) {
            if(in != NULL) fftw_destroy_plan(p);
            if(in != NULL) fftw_free(in);
            if(out!= NULL) fftw_free(out);
            in = NULL;
            out = NULL;
            size = n;
            in = fftw_alloc_complex(n);
            out= fftw_alloc_real(n);                    
            p = fftw_plan_dft_c2r_1d(n, in, out, FFTW_ESTIMATE);
        }
        void set_input(complex_vector<double> & input) {
            for(size_t i = 0; i < size; i++) {
                in[i][0] = input[i].real();
                in[i][1] = input[i].imag();
            }
        }
        void set_input(double * buffer) {
            memcpy(in,buffer,2*size*sizeof(double));
        }
        sample_vector<double> get_output() {
            sample_vector<double> r(size);
            memcpy(r.data(),out, size * sizeof(double));
            return r;
        }
        void get_output(double * output)
        {
            for(size_t i = 0; i < size; i++ )
            {
                output[i] = out[i];                
            }
        }
        void get_output( sample_vector<double> & output)
        {
            if(output.size() != size) output.resize(size);
            for(size_t i = 0; i < size; i++ )
            {
                output[i] = out[i];                
            }
        }
        void normalize() {
            for(size_t i = 0; i < size; i++) {
                out[i] /= (double)size;                    
            }
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

        C2RF() {
            in = NULL;
            out = NULL;
            size = 0;
        }
        C2RF(size_t n) {
            in = NULL;
            out = NULL;
            size = 0;
            init(n);
        }
        ~C2RF() {
            fftwf_destroy_plan(p);
            fftwf_free(in);
            fftwf_free(out);    
        }
        void init(size_t n) {
            if(in != NULL) fftwf_destroy_plan(p);
            if(in != NULL) fftwf_free(in);
            if(out != NULL) fftwf_free(out);
            in = NULL;
            out = NULL;
            size = n;
            in = fftwf_alloc_complex(n);
            out= fftwf_alloc_real(n);                    
            p = fftwf_plan_dft_c2r_1d(n, in, out, FFTW_ESTIMATE);
        }
        void set_input(complex_vector<float> & input) {
            for(size_t i = 0; i < size; i++) {
                in[i][0] = input[i].real();
                in[i][1] = input[i].imag();
            }
        }
        void set_input(float * buffer) {
            memcpy(in,buffer,2*size*sizeof(float));
        }
        sample_vector<float> get_output() {
            sample_vector<float> r(size);
            memcpy(r.data(),out, size*sizeof(float));
            return r;
        }
        void get_output(float * output)
        {
            for(size_t i = 0; i < size; i++ )
            {
                output[i] = out[i];
            }
        }
        void get_output( sample_vector<float> & output)
        {
            if(output.size() != size) output.resize(size);
            for(size_t i = 0; i < size; i++ )
            {
                output[i] = out[i];
            }
        }
        void normalize()
        {
            for(size_t i = 0; i < size; i++) 
                out[i] /= (float)size;                
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

        R2CD() {
            in = NULL;
            out = NULL;
            size= 0;
        }
        R2CD(size_t n) {
            in = NULL;
            out = NULL;
            size= 0;
            init(n);            
        }
        ~R2CD() {
            fftw_destroy_plan(p);
            fftw_free(in);
            fftw_free(out);    
        }
        void init(size_t n) {
            if(in != NULL) fftw_destroy_plan(p);
            if(in != NULL) fftw_free(in);
            if(out != NULL) fftw_free(out);
            in = NULL;
            out = NULL;
            size = n;
            in = fftw_alloc_real(n);
            out= fftw_alloc_complex(n);                    
            p = fftw_plan_dft_r2c_1d(n, in, out, FFTW_ESTIMATE);
        }
        void set_input(sample_vector<double> & input) {
            memcpy(in,input.data(),size*sizeof(double));
        }
        void set_input(double * buffer) {
            memcpy(in,buffer,size*sizeof(double));
        }
        complex_vector<double> get_output() {
            complex_vector<double> r(size);
            for(size_t i = 0; i < size; i++) {
                r[i].real(out[i][0]);
                r[i].imag(out[i][1]);
            }
            return r;
        }
        void get_output(double * output)
        {
            for(size_t i = 0; i < size; i++)
            {
                output[2*i]   = out[i][0];
                output[2*i+1] = out[i][1];
            }
        }
        void get_output(complex_vector<double> & output) {
            if(output.size() != size) output.resize(size);
            for(size_t i = 0; i < size; i++)
            {
                output[i].real(out[i][0]);
                output[i].imag(out[i][1]);
            }            
        }
        void normalize() {
            for(size_t i = 0; i < size; i++) {
                out[i][0] /= (double)size;    
                out[i][1] /= (double)size;
            }
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

        R2CF() {
            in = NULL;
            out = NULL;
            size = 0;
        }
        R2CF(size_t n) {
            in = NULL;
            out = NULL;
            size = 0;
            init(n);            
        }
        ~R2CF() {
            fftwf_destroy_plan(p);
            fftwf_free(in);
            fftwf_free(out);    
        }
        void init(size_t n) {
            if(in != NULL) fftwf_destroy_plan(p);
            if(in != NULL) fftwf_free(in);
            if(out != NULL) fftwf_free(out);
            in = NULL;
            out = NULL;
            size = n;
            in = fftwf_alloc_real(n);
            out= fftwf_alloc_complex(n);                    
            p = fftwf_plan_dft_r2c_1d(n, in, out, FFTW_ESTIMATE);
        }
        void set_input(sample_vector<float> & input) {
            memcpy(in,input.data(),size*sizeof(float));
        }
        void set_input(float * buffer) {
            memcpy(in,buffer,size*sizeof(float));
        }
        complex_vector<float> get_output() {
            complex_vector<float> r(size);
            for(size_t i = 0; i < size; i++) {
                r[i].real(out[i][0]);
                r[i].imag(out[i][1]);
            }
                
            return r;
        }    
        void get_output(float * output)
        {
            for(size_t i = 0; i < size; i++ )
            {
                output[2*i]   = out[i][0];
                output[2*i+1] = out[i][1];
            }
        }
        void get_output( complex_vector<float> & output)
        {
            if(output.size() != size) output.resize(size);
            for(size_t i = 0; i < size; i++ )
            {
                output[i].real(out[i][0]);
                output[i].imag(out[i][1]);
            }
        }
        void normalize() {
            for(size_t i = 0; i < size; i++) {
                out[i][0] /= (float)size;    
                out[i][1] /= (float)size;
            }
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

        R2RD() {
            in = out = NULL;
            size = 0;
        }
        R2RD(size_t n, R2RKind type = DCTI) {
            init(n,type);
        }
        ~R2RD() {
            fftw_destroy_plan(p);
            fftw_free(in);
            fftw_free(out);    
        }
        void init(size_t n, R2RKind type) {
            if(in != NULL) fftw_destroy_plan(p);
            if(in != NULL) fftw_free(in);
            if(out != NULL) fftw_free(out);
            in = out = NULL;            
            size = n;            
            in = fftw_alloc_real(n);
            out= fftw_alloc_real(n);                    
            p = fftw_plan_r2r_1d(n, in, out, (fftw_r2r_kind)type, FFTW_ESTIMATE);
        }
        void set_input(sample_vector<double> & input) {
            memcpy(in,input.data(),size*sizeof(double));
        }
        void set_input(double * buffer) {
            memcpy(in,buffer,size*sizeof(double));
        }
        sample_vector<double> get_output() {
            sample_vector<double> r(size);
            for(size_t i = 0; i < size; i++) {
                r[i] = out[i];                
            }
            return r;
        }
        void get_output(double * output)
        {
            for(size_t i = 0; i < size; i++ )
            {
                output[i] = out[i];                
            }
        }
        void get_output( sample_vector<double> & output)
        {
            if(output.size() != size) output.resize(size);
            for(size_t i = 0; i < size; i++ )
            {
                output[i] = out[i];                
            }
        }
        void normalize() {
            for(size_t i = 0; i < size; i++)
                out[i] /= (double)size;    
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

        R2RF() {
            in = out = NULL;
            size = 0;
        }
        R2RF(size_t n, R2RKind type = DCTI) {
            in = out = NULL;
            size = 0;
            init(n,type);            
        }
        ~R2RF() {
            fftwf_destroy_plan(p);
            fftwf_free(in);
            fftwf_free(out);    
        }
        void init(size_t n, R2RKind type) {
            if(in != NULL) fftwf_destroy_plan(p);
            if(in != NULL) fftwf_free(in);
            if(out != NULL) fftwf_free(out);
            in = out = NULL;            
            size = n;            
            in = fftwf_alloc_real(n);
            out= fftwf_alloc_real(n);                    
            p = fftwf_plan_r2r_1d(n, in, out, (fftw_r2r_kind)type, FFTW_ESTIMATE);
        }
        void set_input(sample_vector<float> & input) {
            memcpy(in,input.data(),size*sizeof(float));
        }
        void set_input(float * buffer) {
            memcpy(in,buffer,size*sizeof(float));
        }
        sample_vector<float> get_output() {
            sample_vector<float> r(size);
            for(size_t i = 0; i < size; i++) {
                r[i] = out[i];                
            }                
            return r;
        }
        void get_output(float * output)
        {
            for(size_t i = 0; i < size; i++ )
            {
                output[i] = out[i];                
            }
        }
        void get_output( sample_vector<float> & output)
        {
            if(output.size() != size) output.resize(size);
            for(size_t i = 0; i < size; i++ )
            {
                output[i] = out[i];                
            }
        }
        void normalize() {
            for(size_t i = 0; i < size; i++)
                out[i] /= (float)size;    
        }    
        void Execute() {
            fftwf_execute(p);            
        }
    };


    ////////////////////////////////////////////////////////////////
    // FFTW Convolution
    ////////////////////////////////////////////////////////////////    
    sample_vector<float> FFTW_Convolution(sample_vector<float> x, sample_vector<float> y) {
        int M = x.size();
        int N = y.size();       
        float in_a[M+N-1];
        complex_vector<float> out_a[M+N-1];
        float in_b[M+N-1];
        complex_vector<float> out_b[M+N-1];
        complex_vector<float> in_rev[M+N-1];
        sample_vector<float> out_rev(M+N-1);

        // Plans for forward FFTs
        fftwf_plan plan_fwd_a = fftwf_plan_dft_r2c_1d (M+N-1, in_a, reinterpret_cast<fftwf_complex*>(&out_a), FFTW_MEASURE);
        fftwf_plan plan_fwd_b = fftwf_plan_dft_r2c_1d (M+N-1, in_b, reinterpret_cast<fftwf_complex*>(&out_b), FFTW_MEASURE);

        // Plan for reverse FFT
        fftwf_plan plan_rev = fftwf_plan_dft_c2r_1d (M+N-1,reinterpret_cast<fftwf_complex*>(&in_rev), out_rev.data(), FFTW_MEASURE);

        // Prepare padded input data
        std::memcpy(in_a, x.data(), sizeof(float) * M);
        std::memset(in_a + M, 0, sizeof(float) * (N-1));
        std::memset(in_b, 0, sizeof(float) * (M-1));
        std::memcpy(in_b + (M-1), y.data(), sizeof(float) * N);
        
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

    void blockconvolve(sample_vector<float> h, sample_vector<float> x, sample_vector<float>& y, sample_vector<float> & ytemp)    
    {
        int i;
        int M = h.size();
        int L = x.size();
        y = FFTW_Convolution(h,x);      
        for (i=0; i<M; i++) {
            y[i] += ytemp[i];                     /* add tail of previous block */
            ytemp[i] = y[i+L];                    /* update tail for next call */
        }        
    }


    ////////////////////////////////////////////////////////////////
    // FFTW Deconvolution
    ////////////////////////////////////////////////////////////////
    sample_vector<float> FFTW_Deconvolution(sample_vector<float> & xin, sample_vector<float> & yout)
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
        sample_vector<float> out_rev(M+N-1);

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
    sample_vector<float> FFTW_CrossCorrelation(sample_vector<float> & xin, sample_vector<float> & yout)
    {
        int M = xin.size();
        int N = yout.size();        
        float in_a[M+N-1];
        std::complex<float> out_a[M+N-1];
        float in_b[M+N-1];
        std::complex<float> out_b[M+N-1];
        std::complex<float> in_rev[M+N-1];
        sample_vector<float> out_rev(M+N-1);

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
    // AudioFFT
    ////////////////////////////////////////////////////////////////
    complex_vector<float> audiofft_forward(sample_vector<float> & in)
    {
        audiofft::AudioFFT fft;
        size_t s = in.size();
        if(s % 2 != 0)
        {
            s = (size_t)std::pow(2,std::log((double)s)+1.0);
        }
        sample_vector<float> temp(s);
        sample_vector<float> real(s);
        sample_vector<float> imag(s);
        zeros(temp);
        zeros(real);
        zeros(imag);
        memcpy(temp.data(),in.data(),in.size()*sizeof(float));
        fft.init(in.size());
        fft.fft(temp.data(),real.data(),imag.data());
        complex_vector<float> out(s);
        for(size_t i = 0; i < s; i++)
        {
            out[i].real(real[i]);
            out[i].imag(imag[i]);
        }
        return out;
    }

    sample_vector<float> audiofft_inverse(complex_vector<float> & in)
    {
        audiofft::AudioFFT fft;
        size_t s = in.size();
        if(s % 2 != 0)
        {
            s = (size_t)std::pow(2,std::log((double)s)+1.0);
        }
        sample_vector<float> temp(s);
        sample_vector<float> real(s);
        sample_vector<float> imag(s);
        zeros(temp);
        zeros(real);
        zeros(imag);
        for(size_t i = 0; i < in.size(); i++)
        {
            real[i] = in[i].real();
            imag[i] = in[i].imag();
        }
        fft.init(in.size());
        fft.ifft(temp.data(),real.data(),imag.data());        
        return temp;
    }

    ////////////////////////////////////////////////////////////////
    // FFTConvolver
    ////////////////////////////////////////////////////////////////
    sample_vector<float> fftconvolver(sample_vector<float>& in, size_t block_size, sample_vector<float> & ir)
    {
        sample_vector<float> out(in.size());
        fftconvolver::FFTConvolver conv;
        conv.init(block_size,ir.data(),ir.size());
        conv.process(in.data(),out.data(),in.size());
        return out;
    }
    template<typename T>
    void swap(sample_vector<T> & vector,size_t i, size_t j)
    {
        T x = vector.vector(i);
        vector.vector(i) = vector.vector(j);
        vector.vector(j) = x;
    }        

    template<typename T>
    void shift(sample_vector<T> & vector) 
    {
        size_t half = vector.vector.size() / 2; 
        size_t start= half; 
        if(2*half < vector.vector.size()) start++;
        for(size_t i = 0; i < half; i++)
        {
            swap(vector,i, i+start);
            
        }
        if(start != half)
        {
            for(size_t i = 0; i < half; i++)            
            {
                swap(vector,i+start-1,i+start);
            }
        }
    }

    template<typename T>    
    void ishift(sample_vector<T> & vector) 
    {
        size_t half = vector.vector.size() / 2; 
        size_t start= half; 
        if(2*half < vector.vector.size()) start++;
        for(size_t i = 0; i < half; i++)
        {
            swap(vector,i,i+start);
        }
        if(start != half)
        {
            for(size_t i = 0; i < half; i++)            
            {
                swap(vector,half,i);
            }
        }
    }

    template<typename T>    
    T quadratic_peak_pos(sample_vector<T> & vector,size_t pos)
    {
        T s0,s1,s2;
        size_t x0,x2;        
        if(pos == 0 || pos == vector.size()-1) return pos; 
        x0 = (pos < 1)? pos:pos-1;
        x2 = (pos+1 < vector.size()) ? pos+1:pos;
        if(x0 == pos) return vector.vector(pos) <= vector.vector(x2)? pos:x2;
        if(x2 == pos) return vector.vector(pos) <= vector.vector(x0)? pos:x0;
        s0 = vector.vector(x0);
        s1 = vector.vector(pos);
        s2 = vector.vector(x2);
        return pos + 0.5 *(s0-s2) / (s0 - 2*s1 + s2);
    }

    template<typename T>    
    T quadratic_peak_mag(sample_vector<T> & vector,size_t pos)
    {
        T x0,x1,x2;
        size_t index = (size_t)(pos - 0.5) + 1;
        if(pos >= vector.size() || pos < (T)0) return (T)0;
        if((T)index == pos) return vector.vector(index);
        x0 = vector.vector(index-1);
        x1 = vector.vector(index);
        x2 = vector.vector(index+1);
        return x1 - 0.25 * (x0 - x2) * (pos - index);
    }

    template<typename T>    
    T median(sample_vector<T> & vector)
    {        
        size_t n = vector.vector.size();
        size_t high,low;
        size_t median;
        size_t middle,ll,hh;

        low = 0;
        high = n-1;
        median = (low+high)/2;

        for(;;)        
        {
            if(high <= low)
                return vector.vector(median);

            if(high == low+1)
            {
                if(vector.vector(low) > vector.vector(high))
                    swap(vector,low,high);
                return vector.vector(median);
            }
            middle = (low+high)/2;
            if(vector.vector(middle) > vector.vector(high)) swap(vector,middle,high);
            if(vector.vector(low) > vector.vector(high)) swap(vector,low,high);
            if(vector.vector(middle) > vector.vector(low)) swap(vector,middle,low);
            swap(vector,middle,low+1);

            ll=low+1;
            hh=high;
            for(;;)
            {
                do ll++; while( vector.vector(low) > vector.vector(ll));
                do hh--; while( vector.vector(hh) > vector.vector(low));
                if(hh < ll) break;
                swap(vector,ll,hh);
            }
            swap(vector,low,hh);
            if(hh <= median) low = ll;
            if(hh >= median) high = hh-1;
        }
    }

    template<typename T>    
    T moving_threshold(sample_vector<T> & input, size_t post, size_t pre, size_t pos)
    {
        size_t length = input.size();
        size_t win_length = post+pre+1;
        sample_vector<T> vector = input;
        if(pos < post+1)
        {
            for(size_t k = 0; k < post +1 - pos; k++)
                vector.vector(k) = 0;
            for(size_t k = post+1-pos; k < win_length; k++)
                vector.vector(k) = vector.vector(k+pos-post);            
        }
        else if(pos + pre < length)
        {
            for(size_t k = 0; k < win_length; k++)
                vector.vector(k) = vector.vector(k+pos-post);
        }
        else 
        {
            for(size_t k = 0; k < length - pos + post; k++)
                vector.vector(k) = vector.vector(k+pos-post);
            for(size_t k = length - pos + post; k < win_length; k++)
                vector.vector(k) = 0;
        }        
        return median(vector);
    }

    template<typename T> 
    T zero_crossing_rate(sample_vector<T> & vector)
    {
        T zcr = 0;
        for(size_t i = 1; i < vector.vector.size(); i++)
        {
            bool current = vector.vector(i) > 0;
            bool prev    = vector.vector(i-1) > 0;
            if(current != prev) zcr++;
        }    
        return zcr;   
    }

    template<typename T> 
    void autocorr(sample_vector<T> & vector,sample_vector<T> & output)
    {
        T tmp;
        output.resize(vector.size());
        for(size_t i = 0; i < vector.size(); i++)
        {
            tmp = (T)0;
            for(size_t j = 0; j < vector.size(); j++)
                tmp += vector.vector(j-i) * vector.vector(j);                
            
            output.vector.vector(i) = tmp / (T)(vector.size()-1);
        }
    }


    template<typename T>
    void push(sample_vector<T> & vector,const T& new_elem)
    {
        for(size_t i = 0; i < vector.vector.size()-1; i++) vector.vector(i) = vector.vector(i+1);
        vector.vector(vector.vector.size()-1) = new_elem;
    }

    template<typename T>
    void clamp(sample_vector<T> & vector,T absmax) { 
        for(size_t i = 0; i < vector.size(); i++)
        {
            if( vector.vector(i) > absmax) vector.vector(i) = absmax;
            else if(vector.vector(i) < -absmax) vector.vector(i) = -absmax;
        }
    }


    template<typename T> 
    bool peakpick(sample_vector<T> & vector,size_t pos)
    {
        bool tmp = false;
        tmp = (vector.vector(pos) > vector.vector(pos-1)) && (vector.vector(pos) > vector.vector(pos+1)) && vector.vector(pos) > 0;
        return tmp;
    }

    template<typename T> 
    T RMS(sample_vector<T> & vector)
    {
        T sum = vector.vector.pow(2).sum();        
        return std::sqrt(sum/vector.vector.size());
    }

    template<typename T> 
    T peak_energy(sample_vector<T> & vector)
    {
        T peak = -10000;
        T as;         
        for(size_t i = 0; i < vector.vector.size(); i++)
        {
            T as = std::fabs(vector.vector(i));
            if(as > peak) peak = as;
        }
        return peak;
    }

    template<typename T> 
    T min(sample_vector<T> & vector) { 
        T min = 1e120;
        for(size_t i = 0; i < vector.size(); i++)
            if(vector.vector(i) < min) min = vector.vector(i);
        return min;            
        //return vector.vector(vector.vector.minCoeff()); 
    }

    template<typename T> 
    T max(sample_vector<T> & vector) { 
        T max = -1e120;
        for(size_t i = 0; i < vector.size(); i++)
            if(vector.vector(i) > max) max = vector.vector(i);
        return max;
        
    }

    template<typename T> 
    sample_vector<T>& weighted_copy(sample_vector<T> & vector,const sample_vector<T> & weight, sample_vector<T> & out)
    {
        out.vector.vector = vector.vector * weight.vector.vector;
        return out;
    }    

    template<typename T> 
    T level_lin(sample_vector<T> & vector) 
    {
        T energy = vector.vector.sqrt().sum();
        return energy/vector.size();
    }

    template<typename T> 
    T local_hfc(sample_vector<T> & vector)
    {
        T hfc = 0;
        for(size_t j = 0; j < vector.size(); j++)
            hfc += (j+1) * vector.vector(j);
        return hfc;
    }

    template<typename T> 
    void min_removal(sample_vector<T> & vector)
    {
        T m = min(vector);
        vector.vector -= m;
    }

    template<typename T> 
    T alpha_norm(sample_vector<T> & vector,sample_vector<T> & out, T alpha)
    {
        T tmp = vector.vector.abs().pow(alpha).sum();
        return std::pow(tmp/vector.size(),1.0/alpha);
    }

    template<typename T> 
    void alpha_normalize(sample_vector<T> & vector,sample_vector<T> & out, T alpha)
    {
        T tmp = alpha_norm(out,alpha);
        out.vector.vector = out.vector.vector / tmp;         
    }    

    template<typename T> 
    T unwrap2pi(T phase)
    {
        return phase + 2*M_PI*(1.0 + std::floor(-(phase+M_PI)/(2*M_PI)));
    }

    template<typename T> 
    T quadfrac(T s0, T s1, T s2, T pf)
    {
        T tmp = s0 + (pf/2)*(pf*(s0-2.0*s1+s2)-3.0*s0+4*s1-s2);
        return tmp;
    }

    template<typename T> 
    T freqtomidi(T freq)
    {
        T midi;
        if(freq < 2.0 || freq > 100000) return 0;        
        midi = 12*std::log(midi/6.875) / 0.6931471805599453 -3;
        return midi;
    }

    template<typename T> 
    T miditofreq(T midi)
    {
        if(midi > 140) return 0;
        T freq = std::exp(((midi + 3) / 12)*0.6931471805599453)*6.875;
        return freq;
    }

    template<typename T> 
    T bintofreq(T bin, T sample_rate, T fft_size)
    {
        T freq = sample_rate / fft_size;
        return freq * std::max(bin,(T)0);
    }


    template<typename T> 
    T bintomidi(T bin, T sample_rate, T fft_size)
    {
        T midi = bintofreq(bin,sample_rate,fft_size);
        return freqtomidi(midi);
    }

    template<typename T> 
    T freqtobin(T freq, T sample_rate, T fft_size)
    {
        T bin = fft_size / sample_rate;
        return std::max(freq,(T)0)*bin;
    }

    template<typename T> 
    T miditobin(T midi, T sample_rate, T fft_size)
    {
        T freq = miditofeq(midi);
        return freqtobin(freq,sample_rate, fft_size);
    }
    template<typename T> 
    bool is_power_of_two(uint64_t a)
    {
        if((a & (a-1))==0)
            return true;
        return false;
    }
    template<typename T> 
    uint64_t next_power_of_two(uint64_t a)
    {
        uint64_t i = 1;
        while(i < a) i <<= 1;
        return i;
    }

    template<typename T> 
    T hztomel(T freq)
    {
        T lin_space = (T)3/(T)200;
        T split_hz = (T)1000;
        T split_mel = split_hz * lin_space;
        T log_space = (T)27 / std::log(6.4);
        assert(freq >= (T)0);
        if(freq < split_hz)        
            return freq * lin_space;        
        else
            return split_mel + log_space * std::log(freq/split_hz);
    }
    template<typename T> 
    T hztomel_htk(T freq)
    {        
        T split_hz = (T)700;        
        T log_space = (T)1127;
        assert(freq >= (T)0);
        return log_space * std::log(1 + freq/split_hz);
    }
        
    template<typename T> 
    T meltohz(T mel)
    {
        T lin_space = (T)200/(T)3;
        T split_hz  = (T)1000;
        T split_mel = split_hz / lin_space;
        T log_spacing = std::pow(6.4,1.0/27.0);
        assert(mel >= 0);
        if( mel < split_mel) return lin_space * mel;
        return split_hz * std::pow(log_spacing, mel-split_mel);
    }

    template<typename T> 
    T meltohz_htk(T mel)
    {
        T split_hz = (T)700;
        T log_space = (T)1/(T)1127;
        assert(mel >= 0);
        return split_hz * (std::exp(mel *log_space) -(T)1);
    }

    template<typename T> 
    T db_spl(sample_vector<T> & vector) { return 10 * std::log10(level_lin(vector)); }

    template<typename T> 
    T level_detect(sample_vector<T> & vector,T threshold) { 
        T db = db_spl(vector);
        if(db < threshold) return 1;
        return db;
    }

    template<typename T> using complex_vector = sample_vector<std::complex<T>>;

    
    
    template<typename T>
    // r = frac
    // x = [i]
    // y = [i+1]
    T linear_interpolate(T x, T y, T r)
    {        
        return r +*x (1.0-r)*y;
        
    }
    template<typename T>
    T cubic_interpolate(T finpos, T xm1, T x0, T x1, T x2)
    {
        //T xm1 = x [inpos - 1];
        //T x0  = x [inpos + 0];
        //T x1  = x [inpos + 1];
        //T x2  = x [inpos + 2];
        T a = (3 * (x0-x1) - xm1 + x2) / 2;
        T b = 2*x1 + xm1 - (5*x0 + x2) / 2;
        T c = (x1 - xm1) / 2;
        return (((a * finpos) + b) * finpos + c) * finpos + x0;
    }
    // original
    template<typename T>
    // x = frac
    // y0 = [i-1]
    // y1 = [i]
    // y2 = [i+1]
    // y3 = [i+2]
    T hermite1(T x, T y0, T y1, T y2, T y3)
    {
        // 4-point, 3rd-order Hermite (x-form)
        T c0 = y1;
        T c1 = 0.5f * (y2 - y0);
        T c2 = y0 - 2.5f * y1 + 2.f * y2 - 0.5f * y3;
        T c3 = 1.5f * (y1 - y2) + 0.5f * (y3 - y0);
        return ((c3 * x + c2) * x + c1) * x + c0;
    }

    // james mccartney
    template<typename T>
    // x = frac
    // y0 = [i-1]
    // y1 = [i]
    // y2 = [i+1]
    // y3 = [i+2]
    T hermite2(T x, T y0, T y1, T y2, T y3)
    {
        // 4-point, 3rd-order Hermite (x-form)
        T c0 = y1;
        T c1 = 0.5f * (y2 - y0);
        T c3 = 1.5f * (y1 - y2) + 0.5f * (y3 - y0);
        T c2 = y0 - y1 + c1 - c3;
        return ((c3 * x + c2) * x + c1) * x + c0;
    }

    // james mccartney
    template<typename T>
    // x = frac
    // y0 = [i-1]
    // y1 = [i]
    // y2 = [i+1]
    // y3 = [i+2]
    T hermite3(T x, T y0, T y1, T y2, T y3)
    {
            // 4-point, 3rd-order Hermite (x-form)
            T c0 = y1;
            T c1 = 0.5f * (y2 - y0);
            T y0my1 = y0 - y1;
            T c3 = (y1 - y2) + 0.5f * (y3 - y0my1 - y2);
            T c2 = y0my1 + c1 - c3;

            return ((c3 * x + c2) * x + c1) * x + c0;
    }

    // laurent de soras
    template<typename T>
    // x[i-1]
    // x[i]
    // x[i+1]
    // x[i+2]    
    inline T hermite4(T frac_pos, T xm1, T x0, T x1, T x2)
    {
        const T    c     = (x1 - xm1) * 0.5f;
        const T    v     = x0 - x1;
        const T    w     = c + v;
        const T    a     = w + v + (x2 - x0) * 0.5f;
        const T    b_neg = w + a;

        return ((((a * frac_pos) - b_neg) * frac_pos + c) * frac_pos + x0);
    }

    /// Sndfile   
    /// Resampler
    /// SpeexResampler

    template<typename T>
    sample_vector<T> upsample2x(sample_vector<T> & in)
    {
        sample_vector<T> out(in.size()*2);
        zeros(out);
        for(size_t i = 0; i < in.size(); i++)
            out[i*2] = in[i];
        return out;
    }
    template<typename T>
    sample_vector<T> upsample4x(sample_vector<T> & in)
    {
        sample_vector<T> out(in.size()*4);
        zeros(out);
        for(size_t i = 0; i < in.size(); i++)
            out[i*4] = in[i];
        return out;
    }
    template<typename T>
    sample_vector<T> downsample2x(sample_vector<T> & in)
    {
        sample_vector<T> out(in.size()/2);
        zeros(out);
        for(size_t i = 0; i < in.size()/2; i++)
            out[i] = in[i*2];
        return out;
    }
    template<typename T>
    sample_vector<T> downsample4x(sample_vector<T> & in)
    {
        sample_vector<T> out(in.size()/4);
        zeros(out);
        for(size_t i = 0; i < in.size()/4; i++)
            out[i] = in[i*4];
        return out;
    }   

    float triangular_window(float n, float N) {
        return 1.0f - fabsf((n - (N - 1) * 0.5f) / (N * 0.5f));
    }

    float hann_window(float n, float N) {
        const float pi_weight = 2.0f * 3.14159265358979323846f;
        return (0.5f - 0.5f * cosf(pi_weight * n / (N - 1)));
    }

    complex_vector<float> stft(sample_vector<float> data, size_t window_size, size_t hop_size) {
        complex_vector<float> output;
        int data_size = data.size();    
        size_t result_size = (data_size / hop_size);
        float *_data = (float *) calloc(window_size, sizeof(float));
        fftwf_complex *_stft = (fftwf_complex *) fftwf_alloc_complex(window_size);
        fftwf_complex * result = (fftwf_complex *) fftwf_alloc_complex(window_size * result_size);
        if (_data == NULL || _stft == NULL || result == NULL) {
            printf("No memory left\n");
            exit(-1);
        }
        fftwf_plan forward = fftwf_plan_dft_r2c_1d(window_size, _data, _stft, FFTW_ESTIMATE);    
        size_t idx = 0;
        for (size_t pos = 0; pos < data_size; pos += hop_size) {
            for (size_t i = 0; i < window_size; ++i) {
                if (pos + i < data_size)
                    _data[i] = hann_window(i, window_size) * data[pos + i];
                else
                    _data[i] = 0;
            }
            fftwf_execute(forward);
            memcpy(result + idx * window_size, _stft, sizeof(fftwf_complex) * window_size);
            idx++;
        }
        fftwf_destroy_plan(forward);
        output.resize(window_size);
        for(size_t i = 0; i < result_size*window_size; i++)
        {
            output[i].real(result[i][0]);
            output[i].imag(result[i][1]);
        }
        fftwf_free(_data);
        fftwf_free(_stft);
        fftwf_free(result);
        return output;
    }

    sample_vector<float> istft(complex_vector<float> data, size_t window_size, size_t hop_size) {
        sample_vector<float> output;
        int data_size = data.size();
        size_t result_size = data_size * hop_size + (window_size - hop_size);
        float *frame = (float *) fftwf_alloc_real(window_size);
        fftwf_complex *slice = (fftwf_complex *) fftwf_alloc_complex(window_size);
        float *result = (float *) fftwf_alloc_real(result_size);
        if (frame == NULL || slice == NULL || result == NULL) {
            printf("Out of memory\n");
            exit(-1);
        }
        fftwf_plan istft = fftwf_plan_dft_c2r_1d(window_size, slice, frame, FFTW_ESTIMATE);    
        for (size_t i = 0; i < data_size; ++i) {
            memcpy(slice, data.data() + i * window_size, sizeof(fftw_complex) * window_size);
            fftwf_execute(istft);
            for (size_t pos = 0; pos < window_size; ++pos) {
                size_t r_pos = pos + i * hop_size;
                result[r_pos] += frame[pos] * hann_window(pos, window_size);
            }
        }    
        fftwf_destroy_plan(istft);
        output.resize(result_size);
        memcpy(output.data(),result,result_size*sizeof(float));
        fftwf_free(frame);
        fftwf_free(slice);
        fftwf_free(result);
        return output;
    }
}