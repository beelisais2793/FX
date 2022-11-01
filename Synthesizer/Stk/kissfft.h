#pragma once
#include "kissfft/kiss_fft.h"
#include "kissfft/kiss_fftr.h"
#include <cassert>

namespace FFT {

struct KissFFT
{
    kiss_fft_cfg  config;
    kiss_fft_cfg  iconfig;
    
    KissFFT(size_t nfft)
    {
        config = kiss_fft_alloc(nfft,0,NULL,NULL);
        assert(config != NULL);
        iconfig = kiss_fft_alloc(nfft,1,NULL,NULL);
        assert(iconfig != NULL);        
    }
    ~KissFFT() 
    {
        if(config) free(config);
        if(iconfig) free(iconfig);
    }
    
    void fft(std::vector<kiss_fft_cpx> & in, std::vector<kiss_fft_cpx> & out)
    {
        return forward(in,out);
    }
    void ifft(std::vector<kiss_fft_cpx> & in, std::vector<kiss_fft_cpx> & out)
    {
        return inverse(in,out);
    }
    void stride(std::vector<kiss_fft_cpx> & in, std::vector<kiss_fft_cpx> & out, size_t stride)
    {
        out.resize(in.size());
        kiss_fft_stride(config,in.data(), out.data(), stride);
    }

    void forward(std::vector<kiss_fft_cpx> & in, 
                             std::vector<kiss_fft_cpx> & out)
    {     
        out.resize(in.size());
        kiss_fft(config, in.data(), out.data());     
    }
    void inverse(std::vector<kiss_fft_cpx> & in, 
                             std::vector<kiss_fft_cpx> & out)
    {        
        out.resize(in.size());
        kiss_fft(iconfig, in.data(), out.data());        
    }
};


struct KissFFTR
{
    kiss_fftr_cfg  config;    
    kiss_fftr_cfg  iconfig;    

    KissFFTR(size_t nfft) 
    {
        config = kiss_fftr_alloc(nfft, 0,NULL,NULL);
        assert(config != NULL);
        iconfig = kiss_fftr_alloc(nfft, 1,NULL,NULL);
        assert(iconfig != NULL);
    }
    ~KissFFTR()
    {
        if(config) free(config);
        if(iconfig) free(iconfig);
    }    
    void forward(std::vector<float> & in, std::vector<kiss_fft_cpx> & out)
    {                
        out.resize(in.size());
        kiss_fftr(config, in.data(), out.data());
    }
    void inverse(std::vector<kiss_fft_cpx> & in, std::vector<float> & out)
    {        
        out.resize(in.size());
        kiss_fftri(iconfig, in.data(), out.data());
    }
    void fft(std::vector<float> & in, std::vector<kiss_fft_cpx> & out) { forward(in,out); }
    void ifft(std::vector<kiss_fft_cpx> & in, std::vector<float> & out) { inverse(in,out); }
};


}
