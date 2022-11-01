%module kfr2
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

%constant int seek_current = kfr::seek_origin::current;
%constant int seek_begin   = kfr::seek_origin::begin;
%constant int seek_end     = kfr::seek_origin::end;
    
%constant int sample_i8  = kfr::audio_sample_type::i8;
%constant int sample_i16 = kfr::audio_sample_type::i16;
%constant int sample_i24 = kfr::audio_sample_type::i24;
%constant int sample_i32 = kfr::audio_sample_type::i32;
%constant int sample_i64 = kfr::audio_sample_type::i64;
%constant int sample_f32 = kfr::audio_sample_type::f32;
%constant int sample_f64 = kfr::audio_sample_type::f64;

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

%template(Biquad)  DSP::Biquad<SampleType>;
%template(BiquadParams)  DSP::BiQuadParams<SampleType>;
%template(biquad_filter)  DSP::biquad<SampleType>;
%template(notch_params)  DSP::notch_params<SampleType>;
%template(lowpass_params)  DSP::lowpass_params<SampleType>;
%template(highpass_params)  DSP::highpass_params<SampleType>;
%template(peak_params)  DSP::peak_params<SampleType>;
%template(lowshelf_params)  DSP::lowshelf_params<SampleType>;
%template(highshelf_params)  DSP::highshelf_params<SampleType>;
%template(bandpass_params)  DSP::bandpass_params<SampleType>;
%template(NotchFilter) DSP::NotchFilter<SampleType>;
%template(LowpassFilter) DSP::LowPassFilter<SampleType>;
%template(HighpassFilter) DSP::HighPassFilter<SampleType>;
%template(BandpassFilter) DSP::BandPassFilter<SampleType>;
%template(PeakFilter) DSP::PeakFilter<SampleType>;
%template(LowShelfFilter) DSP::LowShelfFilter<SampleType>;
%template(HighShelfFilter) DSP::HighShelfFilter<SampleType>;
%template(normalize_frequency) DSP::normalize_frequency<SampleType>;

%template(linspace) DSP::linspace<SampleType>;
%template(pad) DSP::pad<SampleType>;
%template(slice) DSP::slice<SampleType>;
%template(truncate) DSP::truncate<SampleType>;
%template(reverse) DSP::reverse<SampleType>;

%template(ringbuf_read) DSP::ringbuf_read<SampleType>;
%template(ringbuf_write) DSP::ringbuf_write<SampleType>;

%template(abs) DSP::abs<SampleType>;
%template(add) DSP::add<SampleType>;
%template(absmax) DSP::absmax<SampleType>;
%template(absmin) DSP::absmin<SampleType>;
%template(clamp) DSP::clamp<SampleType>;
%template(cube) DSP::cube<SampleType>;
%template(div) DSP::div<SampleType>;
%template(fmadd) DSP::fmadd<SampleType>;
%template(fmsub) DSP::fmsub<SampleType>;
%template(max) DSP::max<SampleType>;
%template(min) DSP::min<SampleType>;
%template(mix) DSP::mix<SampleType>;
%template(mixs) DSP::mixs<SampleType>;
%template(mul) DSP::mul<SampleType>;
%template(neg) DSP::neg<SampleType>;
%template(sqr) DSP::sqr<SampleType>;
%template(sqrt) DSP::sqrt<SampleType>;
%template(exp) DSP::exp<SampleType>;
%template(exp10) DSP::exp10<SampleType>;
%template(exp2) DSP::exp2<SampleType>;
%template(exp_fmadd) DSP::exp_fmadd<SampleType>;
%template(log) DSP::log<SampleType>;
%template(log10) DSP::log10<SampleType>;
%template(log2) DSP::log2<SampleType>;
%template(log_fmadd) DSP::log_fmadd<SampleType>;
%template(logb) DSP::logb<SampleType>;
%template(logm) DSP::logm<SampleType>;
%template(logn) DSP::logn<SampleType>;
%template(pow) DSP::pow<SampleType>;
%template(root) DSP::root<SampleType>;
%template(floor) DSP::floor<SampleType>;
%template(acos) DSP::acos<SampleType>;
%template(asin) DSP::asin<SampleType>;
%template(atan) DSP::atan<SampleType>;
%template(atan2) DSP::atan2<SampleType>;
%template(cos) DSP::cos<SampleType>;
%template(sin) DSP::sin<SampleType>;
%template(tan) DSP::tan<SampleType>;
%template(cosh) DSP::cosh<SampleType>;
%template(coth) DSP::coth<SampleType>;
%template(sinh) DSP::sinh<SampleType>;
%template(tanh) DSP::tanh<SampleType>;
%template(atan2deg) DSP::atan2deg<SampleType>;
%template(cosdeg) DSP::cosdeg<SampleType>;
%template(sindeg) DSP::sindeg<SampleType>;
%template(sinc) DSP::sinc<SampleType>;
%template(gamma) DSP::gamma<SampleType>;
%template(absmaxof) DSP::absmaxof<SampleType>;
%template(dot) DSP::dot<SampleType>;
%template(maxof) DSP::maxof<SampleType>;
%template(minof) DSP::minof<SampleType>;
%template(mean) DSP::mean<SampleType>;
%template(product) DSP::product<SampleType>;
%template(rms) DSP::rms<SampleType>;
%template(sum) DSP::sum<SampleType>;
%template(sumsqr) DSP::sumsqr<SampleType>;
%template(ipow) DSP::ipow<SampleType>;
%template(cos2x) DSP::kcos2x<SampleType>;
%template(sin2x) DSP::ksin2x<SampleType>;
%template(cos3x) DSP::kcos3x<SampleType>;
%template(sin3x) DSP::ksin3x<SampleType>;
%template(reciprocal) DSP::kreciprocal<SampleType>;
%template(acosh) DSP::acosh<SampleType>;
%template(asinh) DSP::asinh<SampleType>;
%template(atanh) DSP::atanh<SampleType>;
%template(cbrt) DSP::cbrt<SampleType>;
%template(ceil) DSP::ceil<SampleType>;
%template(copysign) DSP::copysign<SampleType>;
%template(erf) DSP::erf<SampleType>;
%template(erfc) DSP::erfc<SampleType>;
%template(expm1) DSP::expm1<SampleType>;
%template(fdim) DSP::fdim<SampleType>;
%template(fma) DSP::fma<SampleType>;
%template(fmax) DSP::fmax<SampleType>;
%template(fmin) DSP::fmin<SampleType>;
%template(fmod) DSP::fmod<SampleType>;
%template(fpclassify) DSP::fpclassify<SampleType>;
%template(hypot) DSP::hypot<SampleType>;
%template(ilogb) DSP::ilogb<SampleType>;
%template(isfinite) DSP::isfinite<SampleType>;
%template(isgreater) DSP::isgreater<SampleType>;
%template(isgreaterequal) DSP::isgreaterequal<SampleType>;
%template(isinf) DSP::isinf<SampleType>;
%template(isless) DSP::isless<SampleType>;
%template(islessequal) DSP::islessequal<SampleType>;
%template(isnan) DSP::isnan<SampleType>;
%template(isnormal) DSP::isnormal<SampleType>;
%template(isunordered) DSP::isunordered<SampleType>;
%template(ldexp) DSP::ldexp<SampleType>;
%template(lgamma) DSP::lgamma<SampleType>;
%template(llrint) DSP::llrint<SampleType>;
%template(llround) DSP::llround<SampleType>;
%template(log1p) DSP::log1p<SampleType>;
%template(lrint) DSP::lrint<SampleType>;
%template(lround) DSP::lround<SampleType>;
%template(nan) DSP::nan<SampleType>;
%template(nanf) DSP::nanf<SampleType>;
%template(nanl) DSP::nanl<SampleType>;
%template(nearbyint) DSP::nearbyint<SampleType>;
%template(nextafter) DSP::nextafter<SampleType>;
%template(nexttoward) DSP::nexttoward<SampleType>;
%template(remainder) DSP::remainder<SampleType>;
%template(rint) DSP::rint<SampleType>;
%template(round) DSP::round<SampleType>;
%template(scalbln) DSP::scalbln<SampleType>;
%template(scalbn) DSP::scalbn<SampleType>;
%template(square) DSP::square<SampleType>;
%template(tgamma) DSP::tgamma<SampleType>;
%template(trunc) DSP::trunc<SampleType>;

%template(make_complex) DSP::make_complex<SampleType>;
%template(make_univector) DSP::make_univec<SampleType>;

%template(window_hann) DSP::make_window_hann<SampleType>;
%template(window_hamming) DSP::make_window_hamming<SampleType>;
%template(window_blackman) DSP::make_window_blackman<SampleType>;
%template(window_blackman_harris) DSP::make_window_blackman_harris<SampleType>;
%template(window_gaussian) DSP::make_window_gaussian<SampleType>;
%template(window_triangular) DSP::make_window_triangular<SampleType>;
%template(window_bartlett) DSP::make_window_bartlett<SampleType>;
%template(window_cosine) DSP::make_window_cosine<SampleType>;
%template(window_bartlett_hann) DSP::make_window_bartlett_hann<SampleType>;
%template(window_bohman) DSP::make_window_bohman<SampleType>;
%template(window_lanczos) DSP::make_window_lanczos<SampleType>;
%template(window_flattop) DSP::make_window_flattop<SampleType>;
%template(window_kaiser) DSP::make_window_kaiser<SampleType>;

%template(window_hann_ptr) DSP::make_window_hann_ptr<SampleType>;
%template(window_hamming_ptr) DSP::make_window_hamming_ptr<SampleType>;
%template(window_blackman_ptr) DSP::make_window_blackman_ptr<SampleType>;
%template(window_blackman_harris_ptr) DSP::make_window_blackman_harris_ptr<SampleType>;
%template(window_gaussian_ptr) DSP::make_window_gaussian_ptr<SampleType>;
%template(window_triangular_ptr) DSP::make_window_triangular_ptr<SampleType>;
%template(window_bartlett_ptr) DSP::make_window_bartlett_ptr<SampleType>;
%template(window_cosine_ptr) DSP::make_window_cosine_ptr<SampleType>;
%template(window_bartlett_hann_ptr) DSP::make_window_bartlett_hann_ptr<SampleType>;
%template(window_bohman_ptr) DSP::make_window_bohman_ptr<SampleType>;
%template(window_lanczos_ptr) DSP::make_window_lanczos_ptr<SampleType>;
%template(window_flattop_ptr) DSP::make_window_flattop_ptr<SampleType>;
%template(window_kaiser_ptr) DSP::make_window_kaiser_ptr<SampleType>;

%template(slice) DSP::slice<SampleType>;
%template(truncate) DSP::truncate<SampleType>;
%template(reverse) DSP::reverse<SampleType>;
%template(ringbuf_read) DSP::ringbuf_read<SampleType>;
%template(ringbuf_write) DSP::ringbuf_write<SampleType>;

%template(dftf) DSP::run_dft<SampleType>;
%template(realdftf) DSP::run_realdft<SampleType>;
%template(idftf) DSP::run_idft<SampleType>;
%template(irealdftf) DSP::run_irealdft<SampleType>;

%template(dftd) DSP::run_dft<f64>;
%template(realdftd) DSP::run_realdft<f64>;
%template(idftd) DSP::run_idft<f64>;
%template(irealdftd) DSP::run_irealdft<f64>;




%template(energy_to_loudness) DSP::energy_to_loudness<SampleType>;
%template(loudness_to_energy) DSP::loudness_to_energy<SampleType>;

%template(dcremove) DSP::dcremove<SampleType>;

%template(WavReader) DSP::WavReader<SampleType>;
%template(WavWriter) DSP::WavWriter<SampleType>;
%template(Mp3Reader) DSP::MP3Reader<SampleType>;
%template(FlacReader) DSP::FlacReader<SampleType>;

%template (random) DSP::random<SampleType>;
%template (plot_save) DSP::plot_save<SampleType>;
%template (plot_show) DSP::plot_show<SampleType>;

%template (sinewave) DSP::sinewave<SampleType>;
%template (squarewave) DSP::squarewave<SampleType>;
%template (trianglewave) DSP::trianglewave<SampleType>;
%template (sawtoothwave) DSP::sawtoothwave<SampleType>;

%template (generate_sine) DSP::generate_sin<SampleType>;
%template (generate_linear) DSP::generate_linear<SampleType>;
%template (generate_exp) DSP::generate_exp<SampleType>;
%template (generate_exp2) DSP::generate_exp2<SampleType>;
%template (generate_cossin) DSP::generate_cossin<SampleType>;

%template (autocorrelate) DSP::autocorrelate<SampleType>;
%template (correlate) DSP::correlate<SampleType>;

%template (DCTPlan) DSP::DCTPlan<SampleType>;
%template (DFTPlan) DSP::DFTPlan<SampleType>;
%template (DFTRealPlan) DSP::DFTRealPlan<SampleType>;

%template (resample) DSP::resample<SampleType>;
%template (convert_sample) DSP::convert_sample<SampleType>;
%template (amp_to_dB) DSP::amp_to_dB<SampleType>;

%template (FIRFilter) DSP::FIRFilter<SampleType>;
%template (FIRBandpassFilter) DSP::FIRBandpassFilter<SampleType>;
%template (FIRLowpassFilter) DSP::FIRLowpassFilter<SampleType>;
%template (FIRHighpassFilter) DSP::FIRHighpassFilter<SampleType>;
%template (FIRBandstopFilter) DSP::FIRBandstopFilter<SampleType>;

%template (fir_lowpass)  DSP::fir_lowpass<SampleType>;
%template (fir_highpass) DSP::fir_highpass<SampleType>;
%template (fir_bandpass) DSP::fir_bandpass<SampleType>;
%template (fir_bandstop) DSP::fir_bandstop<SampleType>;

%template (BesselFilter) DSP::BesselFilter<SampleType>;
%template (BesselLowPassFilter) DSP::BesselLowPassFilter<SampleType>;
%template (BesselHighPassFilter) DSP::BesselHighPassFilter<SampleType>;
%template (BesselBandPassFilter) DSP::BesselBandPassFilter<SampleType>;
%template (BesselBandStopFilter) DSP::BesselBandStopFilter<SampleType>;

%template (ButterworthFilter) DSP::ButterworthFilter<SampleType>;
%template (ButterworthLowPassFilter) DSP::ButterworthLowPassFilter<SampleType>;
%template (ButterworthHighPassFilter) DSP::ButterworthHighPassFilter<SampleType>;
%template (ButterworthBandPassFilter) DSP::ButterworthBandPassFilter<SampleType>;
%template (ButterworthBandStopFilter) DSP::ButterworthBandStopFilter<SampleType>;

%template (Chevyshev1Filter)         DSP::Chebyshev1Filter<SampleType>;
%template (Chevyshev1LowPassFilter)  DSP::Chebyshev1LowPassFilter<SampleType>;
%template (Chevyshev1HighPassFilter) DSP::Chebyshev1HighPassFilter<SampleType>;
%template (Chevyshev1BandPassFilter) DSP::Chebyshev1BandPassFilter<SampleType>;
%template (Chevyshev1BandStopFilter) DSP::Chebyshev1BandStopFilter<SampleType>;

%template (Chevyshev2Filter) DSP::Chebyshev2Filter<SampleType>;
%template (Chevyshev2LowPassFilter) DSP::Chebyshev2LowPassFilter<SampleType>;
%template (Chevyshev2HighPassFilter) DSP::Chebyshev2HighPassFilter<SampleType>;
%template (Chevyshev2BandPassFilter) DSP::Chebyshev2BandPassFilter<SampleType>;
%template (Chevyshev2BandStopFilter) DSP::Chebyshev2BandStopFilter<SampleType>;

%constant int audio_sample_unknown = kfr::audio_sample_type::unknown;
%constant int audio_sample_i8  = kfr::audio_sample_type::i8;
%constant int audio_sample_i16 = kfr::audio_sample_type::i16;
%constant int audio_sample_i24 = kfr::audio_sample_type::i24;
%constant int audio_sample_i32 = kfr::audio_sample_type::i32;
%constant int audio_sample_i64 = kfr::audio_sample_type::i64;
%constant int audio_sample_f32 = kfr::audio_sample_type::f32;
%constant int audio_sample_f64 = kfr::audio_sample_type::f64;

%template(interleave) DSP::do_interleave<SampleType>;
%template(deinterleave) DSP::do_deinterleave<SampleType>; 

%template(bessel_lowpass) DSP::bessel_lowpass<SampleType>;
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

%template (convolve) DSP::convolve<SampleType>;
%template (ConvolveFilter) DSP::ConvolveFilter<SampleType>;

%template(load_wav) DSP::load_wav<SampleType>;
%template(save_wav) DSP::write_wav<SampleType>;
%template(load_mp3) DSP::load_mp3<SampleType>;
%template(load_flac) DSP::load_flac<SampleType>;

