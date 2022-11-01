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

namespace kfr
{
    template <typename T>
    struct biquad_params
    {        
        constexpr biquad_params(const biquad_params<T>& bq);
        constexpr static bool is_pod;
        
        constexpr biquad_params();
        constexpr biquad_params(T a0, T a1, T a2, T b0, T b1, T b2);

        T a0;
        T a1;
        T a2;
        T b0;
        T b1;
        T b2;
        biquad_params<T> normalized_a0() const;
        biquad_params<T> normalized_b0() const;
        biquad_params<T> normalized_all() const;
    };

    template <typename T>
        struct zpk
        {
            univector<complex<T>> z;
            univector<complex<T>> p;
            T k;
        };

    namespace CMT_ARCH_NAME
    {

        template <typename T, size_t maxfiltercount = 4>
        class biquad_filter
        {
        public:            
            biquad_filter(const std::vector<biquad_params<T>>& bq);
            void apply(kfr::univector<T> & vector);
            void apply(kfr::univector<T> & dst, const kfr::univector<T> & dst);
            void reset();        
        };

        template <typename T = fbase> biquad_params<T> biquad_allpass(identity<T> frequency, identity<T> Q);
        template <typename T = fbase> biquad_params<T> biquad_lowpass(identity<T> frequency, identity<T> Q);
        template <typename T = fbase> biquad_params<T> biquad_highpass(identity<T> frequency, identity<T> Q);
        template <typename T = fbase> biquad_params<T> biquad_bandpass(identity<T> frequency, identity<T> Q);
        template <typename T = fbase> biquad_params<T> biquad_notch(identity<T> frequency, identity<T> Q);
        template <typename T = fbase> biquad_params<T> biquad_peak(identity<T> frequency, identity<T> Q, identity<T> gain);
        template <typename T = fbase> biquad_params<T> biquad_lowshelf(identity<T> frequency, identity<T> gain);
        template <typename T = fbase> biquad_params<T> biquad_highshelf(identity<T> frequency, identity<T> gain);       

        
        template <typename T> zpk<T> chebyshev1(int N, identity<T> rp);
        template <typename T> zpk<T> chebyshev2(int N, identity<T> rs);
        template <typename T> zpk<T> butterworth(int N);
        template <typename T> zpk<T> bessel(int N);

        template <typename T> zpk<T> iir_lowpass(const zpk<T>& filter, identity<T> frequency, identity<T> fs = 2.0);
        template <typename T> zpk<T> iir_highpass(const zpk<T>& filter, identity<T> frequency, identity<T> fs = 2.0);
        template <typename T> zpk<T> iir_bandpass(const zpk<T>& filter, identity<T> lowfreq, identity<T> highfreq, identity<T> fs = 2.0);
        template <typename T> zpk<T> iir_bandstop(const zpk<T>& filter, identity<T> lowfreq, identity<T> highfreq, identity<T> fs = 2.0);
 
        template <typename T> std::vector<biquad_params<T>> to_sos(const zpk<T>& filter);        

    }

    template <typename T> expression_pointer<T> make_kfilter(int samplerate);
}

%template(biquad_params) kfr::biquad_params<SampleType>;
%template(biquad_filter) kfr::CMT_ARCH_NAME::biquad_filter<SampleType>;
%template(biquad_allpass) kfr::CMT_ARCH_NAME::biquad_allpass<SampleType>;
%template(biquad_lowpass) kfr::CMT_ARCH_NAME::biquad_lowpass<SampleType>;
%template(biquad_highpass) kfr::CMT_ARCH_NAME::biquad_highpass<SampleType>;
%template(biquad_bandpass) kfr::CMT_ARCH_NAME::biquad_bandpass<SampleType>;
%template(biquad_notch) kfr::CMT_ARCH_NAME::biquad_notch<SampleType>;
%template(biquad_peak) kfr::CMT_ARCH_NAME::biquad_peak<SampleType>;
%template(biquad_lowshelf) kfr::CMT_ARCH_NAME::biquad_lowshelf<SampleType>;
%template(biquad_highshelf) kfr::CMT_ARCH_NAME::biquad_highshelf<SampleType>;


%template(chebyshev1)      kfr::CMT_ARCH_NAME::chebyshev1<SampleType>;
%template(chebyshev2)      kfr::CMT_ARCH_NAME::chebyshev2<SampleType>;
%template(butterworth)      kfr::CMT_ARCH_NAME::butterworth<SampleType>;
%template(bessel)      kfr::CMT_ARCH_NAME::bessel<SampleType>;
%template(iir_lowpass)      kfr::CMT_ARCH_NAME::iir_lowpass<SampleType>;
%template(iir_highpass)      kfr::CMT_ARCH_NAME::iir_highpass<SampleType>;
%template(iir_bandpass)      kfr::CMT_ARCH_NAME::iir_bandpass<SampleType>;
%template(iir_bandstop)      kfr::CMT_ARCH_NAME::iir_bandstop<SampleType>;
%template(to_sos)      kfr::CMT_ARCH_NAME::to_sos<SampleType>;
%template(make_kfilter)      kfr::make_kfilter<SampleType>;

%template(Biquad)           DSP::Biquad<SampleType>;
%template(biquadparams)     DSP::BiQuadParams<SampleType>;
%template(biquad)    DSP::biquad<SampleType>;
%template(notch_params)     DSP::notch_params<SampleType>;
%template(lowpass_params)   DSP::lowpass_params<SampleType>;
%template(highpass_params)  DSP::highpass_params<SampleType>;
%template(peak_params)      DSP::peak_params<SampleType>;
%template(lowshelf_params)  DSP::lowshelf_params<SampleType>;
%template(highshelf_params) DSP::highshelf_params<SampleType>;
%template(bandpass_params)  DSP::bandpass_params<SampleType>;
%template(notch_filter)     DSP::NotchFilter<SampleType>;
%template(lowpass_filter)   DSP::LowPassFilter<SampleType>;
%template(highpass_filter)  DSP::HighPassFilter<SampleType>;
%template(bandpass_filter)  DSP::BandPassFilter<SampleType>;
%template(peak_filter) DSP::PeakFilter<SampleType>;
%template(lowshelf_filter)  DSP::LowShelfFilter<SampleType>;
%template(highshelf_filter) DSP::HighShelfFilter<SampleType>;

%template(normalize_frequency) DSP::normalize_frequency<SampleType>;

%template(lowpassfilter)   DSP::lowpassfilter<SampleType>;
%template(highpassfilter)  DSP::highpassfilter<SampleType>;
%template(bandpassfilter)  DSP::bandpassfilter<SampleType>;
%template(peakfilter)      DSP::peakfilter<SampleType>;
%template(lowshelffilter)  DSP::lowshelffilter<SampleType>;
%template(highshelffilter) DSP::highshelffilter<SampleType>;
%template(notchfilter)     DSP::notchfilter<SampleType>;

%template (bessel_filter) DSP::BesselFilter<SampleType>;
%template (bessel_lowpass_filter) DSP::BesselLowPassFilter<SampleType>;
%template (bessel_highpass_filter) DSP::BesselHighPassFilter<SampleType>;
%template (bessel_bandpass_filter) DSP::BesselBandPassFilter<SampleType>;
%template (bessel_bandstop_filter) DSP::BesselBandStopFilter<SampleType>;

%template (butterworth_filter) DSP::ButterworthFilter<SampleType>;
%template (butterworth_lowpass_filter) DSP::ButterworthLowPassFilter<SampleType>;
%template (butterworth_highpass_filter) DSP::ButterworthHighPassFilter<SampleType>;
%template (butterworth_bandpass_filter) DSP::ButterworthBandPassFilter<SampleType>;
%template (butterworth_bandstop_filter) DSP::ButterworthBandStopFilter<SampleType>;

%template (chevyshev1_filter)         DSP::Chebyshev1Filter<SampleType>;
%template (chevyshev1_lowpass_filter)  DSP::Chebyshev1LowPassFilter<SampleType>;
%template (chevyshev1_highpass_filter) DSP::Chebyshev1HighPassFilter<SampleType>;
%template (chevyshev1_bandpass_filter) DSP::Chebyshev1BandPassFilter<SampleType>;
%template (chevyshev1_bandptop_filter) DSP::Chebyshev1BandStopFilter<SampleType>;

%template (chevyshev2_filter) DSP::Chebyshev2Filter<SampleType>;
%template (chevyshev2_lowpass_filter) DSP::Chebyshev2LowPassFilter<SampleType>;
%template (chevyshev2_highpass_filter) DSP::Chebyshev2HighPassFilter<SampleType>;
%template (chevyshev2_bandpass_filter) DSP::Chebyshev2BandPassFilter<SampleType>;
%template (chevyshev2_bandstop_filter) DSP::Chebyshev2BandStopFilter<SampleType>;

%template(bessel_lowpass)  DSP::bessel_lowpass<SampleType>;
%template(bessel_highpass) DSP::bessel_highpass<SampleType>;
%template(bessel_bandpass) DSP::bessel_bandpass<SampleType>;
%template(bessel_bandstop) DSP::bessel_bandstop<SampleType>;

%template(butterworth_lowpass) DSP::butterworth_lowpass<SampleType>;
%template(butterworth_highpass) DSP::butterworth_highpass<SampleType>;
%template(butterworth_bandpass) DSP::butterworth_bandpass<SampleType>;
%template(butterworth_bandstop) DSP::butterworth_bandstop<SampleType>;

%template(chebyshev1_lowpass)  DSP::chebyshev1_lowpass<SampleType>;
%template(chebyshev1_highpass) DSP::chebyshev1_highpass<SampleType>;
%template(chebyshev1_bandpass) DSP::chebyshev1_bandpass<SampleType>;
%template(chebyshev1_bandstop) DSP::chebyshev1_bandstop<SampleType>;

%template(chebyshev2_lowpass)  DSP::chebyshev2_lowpass<SampleType>;
%template(chebyshev2_highpass) DSP::chebyshev2_highpass<SampleType>;
%template(chebyshev2_bandpass) DSP::chebyshev2_bandpass<SampleType>;
%template(chebyshev2_bandstop) DSP::chebyshev2_bandstop<SampleType>;
