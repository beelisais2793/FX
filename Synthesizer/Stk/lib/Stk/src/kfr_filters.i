%module kfr_filters
%{
#include <cmath>
#include <vector>
#include <complex>
#include <iostream>
#include <random> 

#include <kfr/kfr.h>
#include <kfr/dsp.hpp>
#include <kfr/dft.hpp>
#include <kfr/io.hpp>
#include <kfr/math.hpp>

using namespace kfr;

#include "dsp.hpp"

typedef kfr::complex<SampleType> complex;
%}

%include "stdint.i"
%include "std_vector.i"
%include "std_complex.i"
%include "dsp.hpp"



%inline %{
typedef int8_t    i8;
typedef uint8_t   u8;
typedef int16_t  i16;
typedef uint16_t u16;
typedef int32_t  i32;
typedef uint32_t u32;
typedef int64_t  i64;
typedef float    f32;
typedef double   f64;
%}

%template(float_vector) std::vector<f32>;
%template(double_vector) std::vector<f64>;
%template(int8_vector) std::vector<i8>;
%template(uint8_vector) std::vector<u8>;
%template(int16_vector) std::vector<i16>;
%template(uint16_vector) std::vector<u16>;
%template(int32_vector) std::vector<i32>;
%template(uint32_vector) std::vector<u32>;
%template(int64_vector) std::vector<i64>;

%template(Vector) DSP::SampleVector<SampleType>;
%template(Vector2D) DSP::SampleMatrix <SampleType>;

%template(biquad)  DSP::Biquad<SampleType>;
%template(biquadparams)  DSP::BiQuadParams<SampleType>;
%template(biquad_filter)  DSP::biquad<SampleType>;
%template(notch_params)  DSP::notch_params<SampleType>;
%template(lowpass_params)  DSP::lowpass_params<SampleType>;
%template(highpass_params)  DSP::highpass_params<SampleType>;
%template(peak_params)  DSP::peak_params<SampleType>;
%template(lowshelf_params)  DSP::lowshelf_params<SampleType>;
%template(highshelf_params)  DSP::highshelf_params<SampleType>;
%template(bandpass_params)  DSP::bandpass_params<SampleType>;
%template(notch_filter) DSP::NotchFilter<SampleType>;
%template(lowpass_filter) DSP::LowPassFilter<SampleType>;
%template(highpass_filter) DSP::HighPassFilter<SampleType>;
%template(bandpass_filter) DSP::BandPassFilter<SampleType>;
%template(peak_filter) DSP::PeakFilter<SampleType>;
%template(lowShelf_filter) DSP::LowShelfFilter<SampleType>;
%template(highshelf_filter) DSP::HighShelfFilter<SampleType>;
%template(normalize_frequency) DSP::normalize_frequency<SampleType>;

%template (fir_filter) DSP::FIRFilter<SampleType>;
%template (fir_bandpass_filter) DSP::FIRBandpassFilter<SampleType>;
%template (fir_lowpass_filter) DSP::FIRLowpassFilter<SampleType>;
%template (fir_highpass_filter) DSP::FIRHighpassFilter<SampleType>;
%template (fir_bandstop_filter) DSP::FIRBandstopFilter<SampleType>;

%template (fir_lowpass)  DSP::fir_lowpass<SampleType>;
%template (fir_highpass) DSP::fir_highpass<SampleType>;
%template (fir_bandpass) DSP::fir_bandpass<SampleType>;
%template (fir_bandstop) DSP::fir_bandstop<SampleType>;

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

%template(dcremove) DSP::dcremove<SampleType>;

%inline %{

    kfr::univector<SampleType> to_univector(const std::vector<SampleType> & v) {
        kfr::univector<SampleType r(v.size());
        std::copy(v.begin(),v.end(),r.begin())
        return r;
    }

    std::vector<SampleType> to_vector(const kfr::univector<SampleType> & v) {
        std::vector<SampleType> r(v.size());
        std::copy(v.begin(),v.end(),r.begin());
        return r;
    }
%}