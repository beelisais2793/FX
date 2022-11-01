%module kfr_core_int
%{
#include <cmath>
#include <vector>
#include <iostream>
#include <random> 

#include <kfr/kfr.h>
#include <kfr/dsp.hpp>
#include <kfr/dft.hpp>
#include <kfr/io.hpp>
#include <kfr/math.hpp>

using namespace kfr;

#include "dsp.hpp"

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


%template(int8_vector) std::vector<i8>;
%template(uint8_vector) std::vector<u8>;
%template(int16_vector) std::vector<i16>;
%template(uint16_vector) std::vector<u16>;
%template(int32_vector) std::vector<i32>;
%template(uint32_vector) std::vector<u32>;
%template(int64_vector) std::vector<i64>;

%template(Int8Scalar) Scalar<i8>;
%template(UInt8Scalar) Scalar<u8>;
%template(Int16Scalar) Scalar<i16>;
%template(UInt16Scalar) Scalar<u16>;
%template(Int32Scalar) Scalar<i32>;
%template(UInt32Scalar) Scalar<u32>;
%template(Int64Scalar) Scalar<i64>;
%template(UInt64Scalar) Scalar<u64>;

%template(roli32) rol<i32>;
%template(ro4i32) ror<i32>;
%template(shli32) shl<i32>;
%template(shri32) shr<i32>;

%template(remi32)    rem<i32>;
%template(sataddi32) satadd<i32>;
%template(satsubi32) satsub<i32>;

%template(bitwiseandi32) bitwiseand<i32>;
%template(bitwiseori32) bitwiseor<i32>;
%template(bitwisexori32) bitwisexor<i32>;
%template(bitwiseandnoti32) bitwiseandnot<i32>;
%template(bitwisenoti32) bitwisenot<i32>;


%template(roli64) rol<i64>;
%template(ro4i64) ror<i64>;
%template(shli64) shl<i64>;
%template(shri64) shr<i64>;

%template(remi64)    rem<i64>;
%template(sataddi64) satadd<i64>;
%template(satsubi64) satsub<i64>;

%template(bitwiseandi64) bitwiseand<i64>;
%template(bitwiseori64) bitwiseor<i64>;
%template(bitwisexori64) bitwisexor<i64>;
%template(bitwiseandnoti64) bitwiseandnot<i64>;
%template(bitwisenoti64) bitwisenot<i64>;

%template(absi32) DSP::abs<i32>;
%template(addi32) DSP::add<i32>;
%template(absmaxi32) DSP::absmax<i32>;
%template(absmini32) DSP::absmin<i32>;
%template(clampi32) DSP::clamp<i32>;
%template(cubei32) DSP::cube<i32>;
%template(divi32) DSP::div<i32>;
%template(fmaddi32) DSP::fmadd<i32>;
%template(fmsubi32) DSP::fmsub<i32>;
%template(maxi32) DSP::max<i32>;
%template(mini32) DSP::min<i32>;
%template(mixi32) DSP::mix<i32>;
%template(mixsi32) DSP::mixs<i32>;
%template(muli32) DSP::mul<i32>;
%template(negi32) DSP::neg<i32>;
%template(sqri32) DSP::sqr<i32>;
%template(absmaxof) DSP::absmaxof<i32>;
%template(maxof) DSP::maxof<i32>;
%template(minof) DSP::minof<i32>;
%template(mean) DSP::mean<i32>;
%template(product) DSP::product<i32>;
%template(sum) DSP::sum<i32>;
%template(ipow) DSP::ipow<i32>;
%template(ilogb) DSP::ilogb<i32>;

%template(WavReader) DSP::WavReader<i32>;
%template(WavWriter) DSP::WavWriter<i32>;
%template(Mp3Reader) DSP::MP3Reader<i32>;
%template(FlacReader) DSP::FlacReader<i32>;

%template(load_wav) DSP::load_wav<i32>;
%template(save_wav) DSP::write_wav<i32>;
%template(load_mp3) DSP::load_mp3<i32>;
%template(load_flac) DSP::load_flac<i32>;

