#pragma once
#include "vcvfft.hpp"
#include "SndFile.hpp"

namespace FIR::Convolution
{
    struct vqueue
    {
        std::vector<float> q;

        vqueue(size_t n) {
            q.resize(n);
        }
        void insert(float f) {
            memcpy(q.data()+1,q.data(),(q.size()-1)*sizeof(float));
            q[0] = f;
        }
        float& operator[](size_t i) { return q[i]; }
    };

    struct FIRFilter
    {
        vqueue delay;
        std::vector<float> taps;

        FIRFilter(std::vector<float> & _taps) {
            delay.resize(_taps.size());
            taps = _taps;
        }
        double Tick(double I, double A=1, double X=1, double Y=1) {
            double r = I;
            for(size_t i = 0; i < taps.size(); i++ )
                r += taps[i] * delay[taps.size()-i-1];
            delay.insert(I);
            return A*r;
        }
    };

    struct FIRFilterLinearConvolve
    {        
        std::vector<float> taps,temp,ola;

        FIRFilterLinearConvolve(size_t n, std::vector<float> & _taps) {            
            taps = _taps;
            temp.resize(_taps.size()+ n - 1);
            ola.resize(temp.size());
        }
        void linear_convolution(size_t N, float * temp, float * x)
        {                       
            int M = taps.size();
            //Compute Convolution
            for(int i=0;i<M+N-1;i++)
            {  
                y[i]=0;                
                for(j=0;j<=i;j++)
                    temp[i]+=x[j]*taps[i-j];
            }
            return y;
        }
        void ProcessBlock(size_t n, float * input, float * output) {
            linear_convolution(n,ola.data(),input);
            for(size_t i = 0; i < n; i++) output[i] = ola[i];
            for(size_t i = 0; i < ola.size()-n-1; i++)
            {
                ola[i] = ola[i+n];
                ola[i+n] = 0;
            }
        }
    };

    struct FIRFilterBlockConvolve
    {        
        std::vector<float> taps,temp,ola;

        FIRFilterBlockConvolve(size_t n, std::vector<float> & _taps) {            
            taps = _taps;
            temp.resize(_taps.size()+ n - 1);
            ola.resize(temp.size());
        }
         void FFTConvolution(float * x, float * y, float * out_rev) {
            int M = x.size();
            int N = y.size();       
            float in_a[M+N-1];
            float in_b[M+N-1];
            std::complex<float> out_a[M+N-1];            
            std::complex<float> out_b[M+N-1];
            std::complex<float> in_rev[M+N-1];            

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
        }
        void blockconvolve(float* h, float* x, float* y, float* ytemp)    
        {
            int i;
            int M = h.size();
            int L = x.size();
            FFTW_Convolution(h,x,ytemp);      
            for (i=0; i<M; i++) {
                y[i] += ytemp[i];                     /* add tail of previous block */
                ytemp[i] = y[i+L];                    /* update tail for next call */
            }        
        }
        void ProcessBlock(size_t n, float * input, float * output) {
            blockconvolve(taps.data(),input,output,ola.data());                        
        }
    };
}
