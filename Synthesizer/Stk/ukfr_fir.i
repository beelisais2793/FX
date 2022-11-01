%{
#include <cmath>
#include <vector>
#include <complex>
#include <iostream>
#include <random> 

#include <kfr/kfr.h>
#include <kfr/dft.hpp>
#include <kfr/io.hpp>
#include <kfr/math.hpp>
#include "kfrcore.hpp"

using namespace kfr;
%}

namespace kfr::CMT_ARCH_NAME
{
    template <typename T, size_t Size>
    using fir_taps = univector<T, Size>;

    template <typename T, typename U = T>
    class fir_filter 
    {
    public:
        fir_filter(const univector<T>& taps);
        void set_taps(const univector<T>& taps);
        void reset() final;            
        void apply(kfr::univector<T> & vector);
        void apply(const kfr::univector<T> & src, kfr::univector<T> & dst);           
    };
}

namespace kfr {
    template <typename U, typename T> fir_filter<U>* make_fir_filter(const univector<T>& taps);
}

%template (fir_filter) DSP::FIRFilter<SampleType>;
%template (fir_bandpass_filter) DSP::FIRBandpassFilter<SampleType>;
%template (fir_lowpass_filter) DSP::FIRLowpassFilter<SampleType>;
%template (fir_highpass_filter) DSP::FIRHighpassFilter<SampleType>;
%template (fir_bandstop_filter) DSP::FIRBandstopFilter<SampleType>;
%template (fir_lowpass)  DSP::fir_lowpass<SampleType>;
%template (fir_highpass) DSP::fir_highpass<SampleType>;
%template (fir_bandpass) DSP::fir_bandpass<SampleType>;
%template (fir_bandstop) DSP::fir_bandstop<SampleType>;
