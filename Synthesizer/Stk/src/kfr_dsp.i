%module kfr_dsp
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

%include "kfr_core.i"

