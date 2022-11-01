%module kfr_io
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

// IO
%template(WavReader) DSP::WavReader<SampleType>;
%template(WavWriter) DSP::WavWriter<SampleType>;
%template(Mp3Reader) DSP::MP3Reader<SampleType>;
%template(FlacReader) DSP::FlacReader<SampleType>;
%template(load_wav) DSP::load_wav<SampleType>;
%template(save_wav) DSP::write_wav<SampleType>;
%template(load_mp3) DSP::load_mp3<SampleType>;
%template(load_flac) DSP::load_flac<SampleType>;


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