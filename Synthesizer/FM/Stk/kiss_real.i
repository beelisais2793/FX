%module kiss_real
%{ 
#include <kiss_fft.h>
#include <kiss_fftr.h>
#include <cassert>
using namespace std;
%}

%include "stl.i"
%include "stdint.i"

%import <kiss_fft.h>
%include <kiss_fftr.h>

using namespace std;

%template(real_vector) vector<float>;
%template(complex_vector) vector<kiss_fft_cpx>;

%inline 
%{
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
    void forward(vector<float> & in, vector<kiss_fft_cpx> & out)
    {                
        kiss_fftr(config, in.data(), out.data());
    }
    void inverse(vector<kiss_fft_cpx> & in, vector<float> & out)
    {        
        kiss_fftri(iconfig, in.data(), out.data());
    }

};
%}
