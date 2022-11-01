#pragma once

#include "FxDSP.hpp"

namespace DSPFX
{

    struct FxFFT 
    {
        FFTConfig * config;

        FxFFT(size_t n) {
            config = FFTInitD(n);
            assert(config != NULL);
        }
        ~FxFFT() {
            if(config) FFTFreeD(config);
        }
        void R2C(const DspFloatType * in, DspFloatType * real, DspFloatType * imag) {
            Error_t err = FFT_R2CD(config, in, real, imag);
        }
        void C2R(const DspFloatType * real, const DspFloatType * imag,DspFloatType * out) {
            Error_t err = IFFT_C2RD(config, real, imag, out);
        }
        void convolve(const DspFloatType * in1, size_t len1, const DspFloatType *in2, size_t len2, DspFloatType * out) {
            FFTConvolveD(config,in1,len1,in2,len2,out);
        }
        void filterConvolve(const DspFloatType * in, size_t len, DspFloatType * real, DspFloatType * imag, DspFloatType * out)
        {
            FFTSplitComplex split;
            split.realp = real;
            split.imagp = imag;
            FFTFilterConvolveD(config,in,len,split,out);
        }        
        void print(DspFloatType * buffer) {
            FFTdemoD(config,buffer);
        }        
    };

}
