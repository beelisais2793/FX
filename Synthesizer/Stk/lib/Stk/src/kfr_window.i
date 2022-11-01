%module kfr_window
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

%constant int audio_sample_unknown = kfr::audio_sample_type::unknown;
%constant int audio_sample_i8  = kfr::audio_sample_type::i8;
%constant int audio_sample_i16 = kfr::audio_sample_type::i16;
%constant int audio_sample_i24 = kfr::audio_sample_type::i24;
%constant int audio_sample_i32 = kfr::audio_sample_type::i32;
%constant int audio_sample_i64 = kfr::audio_sample_type::i64;
%constant int audio_sample_f32 = kfr::audio_sample_type::f32;
%constant int audio_sample_f64 = kfr::audio_sample_type::f64;

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


%template(float_vector) std::vector<f32>;
%template(double_vector) std::vector<f64>;
%template(int8_vector) std::vector<i8>;
%template(uint8_vector) std::vector<u8>;
%template(int16_vector) std::vector<i16>;
%template(uint16_vector) std::vector<u16>;
%template(int32_vector) std::vector<i32>;
%template(uint32_vector) std::vector<u32>;
%template(int64_vector) std::vector<i64>;


// windows
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