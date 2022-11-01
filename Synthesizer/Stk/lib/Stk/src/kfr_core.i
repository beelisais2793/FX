%module kfr_core
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


%include "stdint.i"
%include "std_vector.i"
%include "std_complex.i"

%include "kfr_biquad.hpp"
%include "kfr_bessel.hpp"
%include "kfr_butterworth.hpp"
%include "kfr_chebyshev1.hpp"
%include "kfr_chebyshev2.hpp"
%include "kfr_convolve.hpp"
%include "kfr_dct.hpp"
%include "kfr_dft.hpp"
%include "kfr_fileio.hpp"
%include "kfr_fir.hpp"
%include "kfr_resample.hpp"
%include "kfr_window.hpp"
%include "kfrcore.hpp"

namespace kfr {
using b8   = bool;
using f32  = float;
using f64  = double;
using i8   = int8_t;
using i16  = int16_t;
using i32  = int32_t;
using i64  = int64_t;
using u8   = uint8_t;
using u16  = uint16_t;
using u32  = uint32_t;
using u64  = uint64_t;
using umax = uint64_t;
using imax = int64_t;
using fmax = double;
using f80  = long double;
using fbase = SampleType;
}

%inline %{
    typedef int8_t   i8;
    typedef uint8_t   u8;
    typedef int16_t  i16;
    typedef uint16_t  u16;
    typedef int32_t  i32;
    typedef uint32_t u32;
    typedef signed long   i64;
    typedef unsigned long  u64;
    typedef float    f32;
    typedef double   f64;
%};


%inline %{

    template<typename T>
    struct Scalar
    {
        T val;

        Scalar(const T& v) { val = v; }
        Scalar(const Scalar<T> & s) { val = s.val; }

        Scalar<T>& operator = (const Scalar<T> & s)
        {
            val = s.val;
            return *this;
        }
        Scalar<T>& operator = (const T & s)
        {
            val = s;
            return *this;
        }
        
        Scalar<T> operator + (const Scalar<T> & s)
        {
            return Scalar<T>(val + s.val);
        }
        Scalar<T> operator - (const Scalar<T> & s)
        {
            return Scalar<T>(val - s.val);
        }
        Scalar<T> operator * (const Scalar<T> & s)
        {
            return Scalar<T>(val * s.val);
        }
        Scalar<T> operator / (const Scalar<T> & s)
        {
            return Scalar<T>(val / s.val);
        }
        Scalar<T> operator + (const T & s)
        {
            return Scalar<T>(val + s);
        }
        Scalar<T> operator - (const T & s)
        {
            return Scalar<T>(val - s);
        }
        Scalar<T> operator * (const T & s)
        {
            return Scalar<T>(val * s);
        }
        Scalar<T> operator / (const T & s)
        {
            return Scalar<T>(val / s);
        }

        kfr::univector<T> operator + (const kfr::univector<T> & s);
        kfr::univector<T> operator * (const kfr::univector<T> & s);

        void aplus(const T & v) { val += v; }
        void aminus(const T & v) { val -= v; }
        void atimes(const T & v) { val *= v; }
        void adiv(const T & v) { val /= v; }

        void aplus(const Scalar<T> & v) { val += v.val; }
        void aminus(const Scalar<T> & v) { val -= v.val; }
        void atimes(const Scalar<T> & v) { val *= v.val; }
        void adiv(const Scalar<T> & v) { val /= v.val; }

        T    get_value() { return val; }
        void set_value(const T & v) { val = v; }
        
        
        void println() { std::cout << val << std::endl; }  
    };

    template<typename T>
    Scalar<T> abs(const Scalar<T> & scalar)  { 
        return Scalar<T>(std::abs(scalar.val));
    }    
    template<typename T>     
    Scalar<T> fabs(const Scalar<T> & scalar) { 
        return Scalar<T>(std::fabs(scalar.val));         
    }
        
    template<typename T>
    Scalar<T> acos(const Scalar<T> & scalar) { return std::acos(scalar.val); }
    template<typename T>
    Scalar<T> acosh(const Scalar<T> & scalar) { return std::acosh(scalar.val); }
    template<typename T>
    Scalar<T> asinh(const Scalar<T> & scalar) { return std::asinh(scalar.val); }
    template<typename T>
    Scalar<T> atan(const Scalar<T> & scalar) { return std::atan(scalar.val); }
    template<typename T>
    Scalar<T> atan2(const Scalar<T> & scalar,const Scalar<T>& y) { return std::atan2(scalar.val,y.val); }
    template<typename T>
    Scalar<T> atanh(const Scalar<T> & scalar) { return std::atanh(scalar.val); }
    template<typename T>
    Scalar<T> cbrt(const Scalar<T> & scalar) { return std::cbrt(scalar.val); }
    template<typename T>
    Scalar<T> ceil(const Scalar<T> & scalar) { return std::ceil(scalar.val); }    
    template<typename T>
    Scalar<T> copysign(const Scalar<T> & scalar,const Scalar<T>& y) { return std::copysign(scalar.val,y.val); }
    template<typename T>
    Scalar<T> cos(const Scalar<T> & scalar) { return std::cos(scalar.val); }
    template<typename T>
    Scalar<T> cosh(const Scalar<T> & scalar) { return std::cosh(scalar.val); }
    template<typename T>
    Scalar<T> erf(const Scalar<T> & scalar) { return std::erf(scalar.val); }
    template<typename T>
    Scalar<T> erfc(const Scalar<T> & scalar) { return std::erfc(scalar.val); }
    template<typename T>
    Scalar<T> exp(const Scalar<T> & scalar) { return std::exp(scalar.val); }
    template<typename T>
    Scalar<T> exp2(const Scalar<T> & scalar) { return std::exp2(scalar.val); }
    template<typename T>
    Scalar<T> expm1(const Scalar<T> & scalar) { return std::expm1(scalar.val); }
    template<typename T>
    Scalar<T> fdim(const Scalar<T> & scalar,const Scalar<T> & y) { return std::fdim(scalar.val,y.val); }
    template<typename T>
    Scalar<T> floor(const Scalar<T> & scalar) { return std::floor(scalar.val); }
    template<typename T>
    Scalar<T> fma(const Scalar<T> & scalar,const Scalar<T> & y, const Scalar<T>& z) { return std::fma(scalar.val,y.val,z.val); }
    //template<typename T>
    //Scalar<T> fmax(const Scalar<T> & scalar,const Scalar<T> & y) { return std::fmax(scalar.val,y.val); }
    //template<typename T>
    //Scalar<T> fmin(const Scalar<T> & scalar,const Scalar<T> & y) { return std::fmin(scalar.val,y.val); }
    template<typename T>
    Scalar<T> fmod(const Scalar<T> & scalar,const Scalar<T> & y) { return std::fmod(scalar.val,y.val); }
    
    template<typename T>    
    int fpclassify(const Scalar<T> & scalar) { return std::fpclassify(scalar.val); }
    
    template<typename T>
    Scalar<T> hypot(const Scalar<T> & scalar,const Scalar<T> & y) { return std::hypot(scalar.val,y.val); }
    
    template<typename T>
    int ilogb(const Scalar<T> & scalar) { return std::ilogb(scalar.val); }
    
    template<typename T>
    bool isfinite(const Scalar<T> & scalar) { return std::isfinite(scalar.val); }
    template<typename T>
    bool isgreater(const Scalar<T> & scalar,const Scalar<T> & y) { return std::isgreater(scalar.val,y.val); }
    template<typename T>
    bool isgreaterequal(const Scalar<T> & scalar,const Scalar<T> & y) { return std::isgreaterequal(scalar.val,y.val); }
    template<typename T>
    bool isinf(const Scalar<T> & scalar) { return std::isinf(scalar.val); }
    template<typename T>
    bool isless(const Scalar<T> & scalar,const Scalar<T> & y) { return std::isless(scalar.val,y.val); }
    template<typename T>
    bool islessequal(const Scalar<T> & scalar,const Scalar<T> & y) { return std::islessequal(scalar.val,y.val); }
    template<typename T>
    bool islessgreater(const Scalar<T> & scalar,const Scalar<T> & y) { return std::islessgreater(scalar.val,y.val); }
    template<typename T>
    bool isnan(const Scalar<T> & scalar) { return std::isnan(scalar.val); }
    template<typename T>
    bool isnormal(const Scalar<T> & scalar) { return std::isnormal(scalar.val); }
    template<typename T>
    bool isunordered(const Scalar<T> & scalar,const Scalar<T>& y) { return std::isunordered(scalar.val,y.val); }
    
    template<typename T>    
    Scalar<T> ldexp( const Scalar<T> & scalar,int exp) { return std::ldexp(scalar.val,exp); }
    template<typename T>
    Scalar<T> lgamma(const Scalar<T> & scalar) { return std::lgamma(scalar.val); }
    template<typename T>
    Scalar<T> llrint(const Scalar<T> & scalar) { return std::llrint(scalar.val); }
    template<typename T>
    Scalar<T> llround(const Scalar<T> & scalar) { return std::llround(scalar.val); }
    template<typename T>
    Scalar<T> log(const Scalar<T> & scalar) { return std::log(scalar.val); }
    template<typename T>
    Scalar<T> log10(const Scalar<T> & scalar) { return std::log10(scalar.val); }
    template<typename T>
    Scalar<T> log1p(const Scalar<T> & scalar) { return std::log1p(scalar.val); }
    template<typename T>
    Scalar<T> log2(const Scalar<T> & scalar) { return std::log2(scalar.val); }
    template<typename T>
    Scalar<T> logb(const Scalar<T> & scalar) { return std::logb(scalar.val); }
    template<typename T>
    Scalar<T> lrint(const Scalar<T> & scalar) { return std::lrint(scalar.val); }
    template<typename T>
    Scalar<T> lround(const Scalar<T> & scalar) { return std::lround(scalar.val); }
    template<typename T>
    Scalar<T> nan(const char *tagp) { return std::nan(tagp);}
    template<typename T>
    Scalar<T> nanf(const char *tagp) { return std::nanf(tagp);}
    template<typename T>
    Scalar<T> nanl(const char *tagp) { return std::nanl(tagp);}
    template<typename T>
    Scalar<T> nearbyint(const Scalar<T> & scalar) { return std::nearbyint(scalar.val); }
    template<typename T>
    Scalar<T> nextafter(const Scalar<T> & scalar,const Scalar<T> & y) { return std::nextafter(scalar.val,y.val); }
    template<typename T>
    Scalar<T> nexttoward(const Scalar<T> & scalar,const Scalar<T> & y) { return std::nexttoward(scalar.val,y.val); }
    template<typename T>
    Scalar<T> pow(const Scalar<T> & scalar,const Scalar<T> & e) { return std::pow(scalar.val,e.val); }
    template<typename T>
    Scalar<T> remainder(const Scalar<T> & scalar,const Scalar<T> & d) { return std::remainder(scalar.val,d.val); }
    template<typename T>
    Scalar<T> rint(const Scalar<T> & scalar) { return std::rint(scalar.val); }
    template<typename T>
    Scalar<T> round(const Scalar<T> & scalar) { return std::round(scalar.val); }
    template<typename T>
    Scalar<T> scalbln(const Scalar<T> & scalar,long int n) { return std::scalbln(scalar.val,n);}
    template<typename T>
    Scalar<T> scalbn(const Scalar<T> & scalar,int n) { return std::scalbln(scalar.val,n);}
    template<typename T>
    Scalar<T> sin(const Scalar<T> & scalar) { return std::sin(scalar.val); }
    template<typename T>
    Scalar<T> sinh(const Scalar<T> & scalar) { return std::sinh(scalar.val); }    
    template<typename T>
    Scalar<T> sqrt(const Scalar<T> & scalar) { return std::sqrt(scalar.val); }
    template<typename T>
    Scalar<T> square(const Scalar<T> & scalar) { return scalar.val*scalar.val; }
    template<typename T>
    Scalar<T> cube(const Scalar<T> & scalar) { return scalar.val*scalar.val*scalar.val; }
    template<typename T>
    Scalar<T> tan(const Scalar<T> & scalar) { return std::tan(scalar.val); }
    template<typename T>
    Scalar<T> tanh(const Scalar<T> & scalar) { return std::tanh(scalar.val); }        
    template<typename T>
    Scalar<T> tgamma(const Scalar<T> & scalar) { return std::tgamma(scalar.val); }    
    template<typename T>
    Scalar<T> trunc(const Scalar<T> & scalar) { return std::trunc(scalar.val); }


    template<typename T>
    Scalar<T> operator + (const Scalar<T> & a, const Scalar<T> & b) {
        return a + b;
    }
    template<typename T>
    Scalar<T> operator * (const Scalar<T> & a, const Scalar<T> & b) {
        return a * b;
    }
    template<typename T>
    Scalar<T> operator - (const Scalar<T> & a, const Scalar<T> & b) {
        return a - b;
    }
    template<typename T>
    Scalar<T> operator / (const Scalar<T> & a, const Scalar<T> & b) {
        return a / b;
    }

     template<typename T>
    kfr::univector<T> Scalar<T>::operator + (const univector<T> & s)
    {
        return kfr::univector<T>(s + val);
    }
    template<typename T>
    kfr::univector<T> Scalar<T>::operator * (const univector<T> & s)
    {
        return kfr::univector<T>(s * val);
    }
%}

%template(int8_scalar) Scalar<i8>;
%template(uint8_scalar) Scalar<u8>;
%template(int16_scalar) Scalar<i16>;
%template(uint16_scalar) Scalar<u16>;
%template(int32_scalar) Scalar<i32>;
%template(uint32_scalar) Scalar<u32>;
%template(int64_scalar) Scalar<i64>;
%template(uint64_scalar) Scalar<u64>;
%template(f32_scalar) Scalar<f32>;
%template(f64_scalar) Scalar<f64>;

/*
%constant int audio_sample_type_f32 = kfr::audio_sample_type::f32;
%constant int audio_sample_type_f64 = kfr::audio_sample_type::f64;
%constant int audio_sample_type_i8  = kfr::audio_sample_type::i8;
%constant int audio_sample_type_i16 = kfr::audio_sample_type::i16;
%constant int audio_sample_type_i32 = kfr::audio_sample_type::i32;
%constant int audio_sample_type_i64 = kfr::audio_sample_type::i64;
*/

namespace kfr {

    enum class audio_sample_type
    {
        unknown,
        i8,
        i16,
        i24,
        i32,
        i64,
        f32,
        f64,
        first_float = f32
    };

    enum class sample_rate_conversion_quality : int
    {
        draft   = 4,
        low     = 6,
        normal  = 8,
        high    = 10,
        perfect = 12,
    };

    enum class biquad_type
    {
        lowpass,
        highpass,
        bandpass,
        bandstop,
        peak,
        notch,
        lowshelf,
        highshelf
    };

    enum class Speaker : int
    {
        None          = -1,
        Mono          = 0,
        M             = static_cast<int>(Mono),
        Left          = 1,
        L             = static_cast<int>(Left),
        Right         = 2,
        R             = static_cast<int>(Right),
        Center        = 3,
        C             = static_cast<int>(Center),
        Lfe           = 4,
        Ls            = 5,
        LeftSurround  = static_cast<int>(Ls),
        Rs            = 6,
        RightSurround = static_cast<int>(Rs),
        Lc            = 7,
        Rc            = 8,
        S             = 9,
        Cs            = static_cast<int>(S),
        Sl            = 10,
        Sr            = 11,
        Tm            = 12,
        Tfl           = 13,
        Tfc           = 14,
        Tfr           = 15,
        Trl           = 16,
        Trc           = 17,
        Trr           = 18,
        Lfe2          = 19
    };

    enum class SpeakerArrangement : int
    {
        None           = -1,
        Mono           = 0,
        Stereo         = 1,
        StereoSurround = 2,
        StereoCenter   = 3,
        StereoSide     = 4,
        StereoCLfe     = 5,
        Cine30         = 6,
        Music30        = 7,
        Cine31         = 8,
        Music31        = 9,
        Cine40         = 10,
        Music40        = 11,
        Cine41         = 12,
        Music41        = 13,
        Arr50          = 14,
        Arr51          = 15,
        Cine60         = 16,
        Music60        = 17,
        Cine61         = 18,
        Music61        = 19,
        Cine70         = 20,
        Music70        = 21,
        Cine71         = 22,
        Music71        = 23,
        Cine80         = 24,
        Music80        = 25,
        Cine81         = 26,
        Music81        = 27,
        Arr102         = 28
    };

    /// @brief Seek origin
    enum class seek_origin : int
    {
        current = SEEK_CUR, ///< From the current position
        begin   = SEEK_SET, ///< From the beginning
        end     = SEEK_END, ///< From the end
    };

    struct audio_format
    {
        size_t channels        = 2;
        audio_sample_type type = audio_sample_type::i16;
        kfr::fmax samplerate        = 44100;
        bool use_w64           = false;
    };

    struct audio_format_and_length : audio_format
    {        
        constexpr audio_format_and_length();
        constexpr audio_format_and_length(const audio_format& fmt);

        imax length = 0; // in samples
    };

    constexpr size_t audio_sample_sizeof(audio_sample_type type);
    constexpr size_t audio_sample_bit_depth(audio_sample_type type);

    
    /*
    void CMT_ARCH_NAME::deinterleave(SampleType* out[], const SampleType *in, size_t channels, size_t size);
    void CMT_ARCH_NAME::interleave(SampleType* out, const SampleType* in[], size_t channels, size_t size);
    void CMT_ARCH_NAME::convert(SampleType* out, const SampleTYpe* in, size_t size);
    */
   
    struct fraction
    {
        fraction(i64 num = 0, i64 den = 1);
        void normalize();
        
        i64 numerator;
        i64 denominator;

        fraction operator+() const;
        fraction operator-() const;

        //explicit operator bool() const;
        //explicit operator double() const;
        //explicit operator float() const;
        //explicit operator kfr::signed long long() const;
    };    

    template <typename T>
    struct complex
    {        
        constexpr complex()  = default;
        constexpr complex(T re)  : re(re), im(0) {}
        constexpr complex(T re, T im)  : re(re), im(im) {}
        constexpr complex(const complex&)  = default;
        
        constexpr complex& operator=(const complex&)  = default;
        constexpr complex& operator=(complex&&)  = default;
        constexpr const T& real() const  { return re; }
        constexpr const T& imag() const  { return im; }
        constexpr void real(T value)  { re = value; }
        constexpr void imag(T value)  { im = value; }    
    };
}

%inline %{
    template<typename T> kfr::univector<T> csin(const kfr::univector<kfr::complex<T>> & v) { return kfr::csin(v); }
    template<typename T> kfr::univector<T> ccos(const kfr::univector<kfr::complex<T>> & v) { return kfr::ccos(v); }
    //template<typename T> kfr::univector<T> ctan(const kfr::univector<kfr::complex<T>> & v) { return kfr::ctan(v); }

    template<typename T> kfr::univector<T> csinh(const kfr::univector<kfr::complex<T>> & v) { return kfr::csinh(v); }
    template<typename T> kfr::univector<T> ccosh(const kfr::univector<kfr::complex<T>> & v) { return kfr::ccosh(v); }
    //template<typename T> kfr::univector<T> ctanh(const kfr::univector<kfr::complex<T>> & v) { return kfr::ctanh(v); }

    template<typename T> kfr::univector<T> cabssqr(const kfr::univector<kfr::complex<T>> & v) { return kfr::cabssqr(v); }
    template<typename T> kfr::univector<T> cabs(const kfr::univector<kfr::complex<T>> & v) { return kfr::cabs(v); }
    template<typename T> kfr::univector<T> carg(const kfr::univector<kfr::complex<T>> & v) { return kfr::carg(v); }
    
    template<typename T> kfr::univector<T> clog(const kfr::univector<kfr::complex<T>> & v) { return kfr::clog(v); }
    template<typename T> kfr::univector<T> clog2(const kfr::univector<kfr::complex<T>> & v) { return kfr::clog2(v); }
    template<typename T> kfr::univector<T> clog10(const kfr::univector<kfr::complex<T>> & v) { return kfr::clog10(v); }

    template<typename T> kfr::univector<T> cexp(const kfr::univector<kfr::complex<T>> & v) { return kfr::cexp(v); }
    template<typename T> kfr::univector<T> cexp2(const kfr::univector<kfr::complex<T>> & v) { return kfr::cexp2(v); }
    template<typename T> kfr::univector<T> cexp10(const kfr::univector<kfr::complex<T>> & v) { return kfr::cexp10(v); }

    template<typename T> kfr::univector<T> polar(const kfr::univector<kfr::complex<T>> & v) { return kfr::polar(v); }
    template<typename T> kfr::univector<T> cartesian(const kfr::univector<kfr::complex<T>> & v) { return kfr::cartesian(v); }
    //template<typename T> kfr::univector<T> cabsdup(const kfr::univector<kfr::complex<T>> & v) { return kfr::cabsdup(v); }

    template<typename T> kfr::univector<T> csqrt(const kfr::univector<kfr::complex<T>> & v) { return kfr::csqrt(v); }
    template<typename T> kfr::univector<T> csqr(const kfr::univector<kfr::complex<T>> & v) { return kfr::csqr(v); }

%}


%template(float_vector)  std::vector<f32>;
%template(double_vector) std::vector<f64>;
%template(int8_vector)   std::vector<i8>;
%template(uint8_vector)  std::vector<u8>;
%template(int16_vector)  std::vector<i16>;
%template(uint16_vector) std::vector<u16>;
%template(int32_vector)  std::vector<i32>;
%template(uint32_vector) std::vector<u32>;
%template(int64_vector)  std::vector<i64>;
%template(complex_vector) std::vector<kfr::complex<float>>;
%template(dblcomplex_vector) std::vector<kfr::complex<double>>;

%template(flt_complex) kfr::complex<float>;
%template(dbl_complex) kfr::complex<double>;


namespace kfr
{
    // single channel vector (SampleVector)
    template<typename T> 
    struct univector
    {        
        univector() {}
        univector(size_t s);
        //univector(T* data, size_t size);
        //univector(std::vector<T> & data);
        univector(const univector<T> & u);        
        

        size_t size() const;
        void resize(size_t s);
        T& operator[](size_t i);
        T  operator[](size_t i);

        %extend {
            // lua is 1 based like fortran
            T  __getitem(size_t i) { return (*$self)[i-1]; }
            void __setitem(size_t i, const T & val) { (*$self)[i-1] = val; }

            univector<T> __add__(const univector& b) { return *$self + b; }
            univector<T> __sub__(const univector& b) { return *$self - b; }
            univector<T> __mul__(const univector& b) { return *$self * b; }
            univector<T> __div__(const univector& b) { return *$self / b; }
            univector<T> __unm__() { return -*$self; }
            univector<T> __pow__(const T& b) { return pow(*$self,b); }
            //bool         __eq__(const univector& b) { return (bool)*$self == b; }
            //bool         __lt__(const univector& b) { return *$self < b; }
            //bool         __le__(const univector& b) { return *$self <= b; }

            void fill(const T& val) { for(size_t i = 0; i < $self->size(); i++) (*$self)[i] = val; }
            void print() const { kfr::println(*$self); }
        }
                
        T& at(size_t pos);
        T& front();
        T& back();
        T* data();
            
        univector<T>& operator = (const univector<T>& u);
        
    };    
}
%inline %{

    kfr::univector<SampleType> to_univector(const std::vector<SampleType> & v) {
        kfr::univector<SampleType> r(v.size());
        std::copy(v.begin(),v.end(),r.begin());
        return r;
    }

    std::vector<SampleType> to_vector(const kfr::univector<SampleType> & v) {
        std::vector<SampleType> r(v.size());
        std::copy(v.begin(),v.end(),r.begin());
        return r;
    }

    template<typename X> X rol(X x, X y) { return kfr::rol(x,y); }
    template<typename X> X ror(X x, X y) { return kfr::ror(x,y); }
    template<typename X> X shl(X x, X y) { return kfr::shl(x,y); }
    template<typename X> X shr(X x, X y) { return kfr::rol(x,y); }

    template<typename T> kfr::univector<T> bitwiseand(const kfr::univector<T> & a, const kfr::univector<T> & b) { kfr::univector<T> r; r = kfr::bitwiseand(a,b); return r; }
    template<typename T> kfr::univector<T> bitwiseandnot(const kfr::univector<T> & a,const kfr::univector<T> & b) { kfr::univector<T> r; r = kfr::bitwiseandnot(a,b); return r; }
    template<typename T> kfr::univector<T> bitwisenot(const kfr::univector<T> & a) { kfr::univector<T> r; r = kfr::bitwisenot(a); return r; }
    template<typename T> kfr::univector<T> bitwiseor(const kfr::univector<T> & a,const kfr::univector<T> & b) { kfr::univector<T> r; r = kfr::bitwiseor(a,b); return r; }
    template<typename T> kfr::univector<T> bitwisexor(const kfr::univector<T> & a,const kfr::univector<T> & b) { kfr::univector<T> r; r = kfr::bitwisexor(a,b); return r; }

    template<typename T>    
    kfr::univector<T> linspace(T start, T stop, size_t size, bool endpoint=false,bool trunc=false)
    {   
        kfr::univector<T> r; 
        r = kfr::linspace(start,stop,size,endpoint,trunc); 
        return r; 
    }

    template<typename T>    
    kfr::univector<T> pad(const kfr::univector<T> & in, const T & fill_value = T(0))
    { 
        kfr::univector<T> r; 
        r = kfr::padded(in,fill_value); 
        return r; 
    }

    template<typename T>    
    kfr::univector<T> slice(const kfr::univector<T> & v, size_t start, size_t end=kfr::max_size_t)
    {   
        kfr::univector<T> r;        
        r = v.slice(start,end);
        return r;
    }

    template<typename T>    
    kfr::univector<T> truncate(const kfr::univector<T> & v, size_t size)
    {   
        kfr::univector<T> r; 
        r = v.truncate();
        return r;
    }

    template<typename T>    
    kfr::univector<T> reverse(const kfr::univector<T> & v)
    {   
        kfr::univector<T> r;         
        r = kfr::reverse(v);
        return r;
    }


    template<typename T>    
    T& ringbuf_read(kfr::univector<T> &v,size_t & cursor, T& value) { v.ringbuf_read(cursor,value); return value; }

    template<typename T>    
    void ringbuf_write(kfr::univector<T> &v, size_t & cursor, T& value) { v.ringbuf_write(cursor,value); }
    
    template<typename T> kfr::univector<T> abs(const kfr::univector<T>& v) { return kfr::abs(v); }
    template<typename T> kfr::univector<T> add(const kfr::univector<T> & a,const kfr::univector<T> & b) { kfr::univector<T> r; r = kfr::add(a,b); return r; }
    template<typename T> kfr::univector<T> add(const kfr::univector<T> & a,const T & b) { kfr::univector<T> r; r = kfr::add(a,b); return r; }
    template<typename T> kfr::univector<T> absmax(const kfr::univector<T> & a,const kfr::univector<T> & b) { kfr::univector<T> r; r = kfr::absmax(a,b); return r; }
    template<typename T> kfr::univector<T> absmax(const kfr::univector<T> & a,const T & b) { kfr::univector<T> r; r = kfr::absmax(a,b); return r; }
    template<typename T> kfr::univector<T> absmin(const kfr::univector<T> & a,const kfr::univector<T> & b) { kfr::univector<T> r; r = kfr::absmin(a,b); return r; }
    template<typename T> kfr::univector<T> absmin(const kfr::univector<T> & a,const T & b) { kfr::univector<T> r; r = kfr::absmin(a,b); return r; }    
    template<typename T> kfr::univector<T> clamp(const kfr::univector<T> & a,const kfr::univector<T> & lo, const kfr::univector<T> &hi) { kfr::univector<T> r; r = kfr::clamp(a,lo,hi); return r; }
    template<typename T> kfr::univector<T> clamp(const kfr::univector<T> & a,const T& lo, const T &hi) { kfr::univector<T> r; r = kfr::clamp(a,lo,hi); return r; }
    template<typename T> kfr::univector<T> cube(const kfr::univector<T> & a) { kfr::univector<T> r; r = kfr::cub(a); return r; }
    template<typename T> kfr::univector<T> div(const kfr::univector<T> & a,const kfr::univector<T> & b) { kfr::univector<T> r; r = kfr::div(a,b); return r; }    
    template<typename T> kfr::univector<T> fmadd(const kfr::univector<T> & a,const kfr::univector<T> & y, const kfr::univector<T> & z) { kfr::univector<T> r; r = kfr::fmadd(a,y,z); return r; }
    template<typename T> kfr::univector<T> fmsub(const kfr::univector<T> & a,const kfr::univector<T> & y, const kfr::univector<T> & z) { kfr::univector<T> r; r = kfr::fmsub(a,y,z); return r; }    
    template<typename T> kfr::univector<T> max(const kfr::univector<T> & a,const kfr::univector<T> & b) { kfr::univector<T> r; r = kfr::max(a,b); return r; }
    template<typename T> kfr::univector<T> max(const kfr::univector<T> & a, const T & b) { kfr::univector<T> r; r = kfr::max(a,b); return r; }
    template<typename T> kfr::univector<T> min(const kfr::univector<T> & a, const kfr::univector<T> & b) { kfr::univector<T> r; r = kfr::min(a,b); return r; }
    template<typename T> kfr::univector<T> min(const kfr::univector<T> & a, const T & b) { kfr::univector<T> r; r = kfr::min(a,b); return r; }
    template<typename T> kfr::univector<T> mix(const kfr::univector<T> & a, const T& c, const kfr::univector<T> & y) { kfr::univector<T> r; r = kfr::mix(c,a,y); return r; }
    template<typename T> kfr::univector<T> mixs(const kfr::univector<T> & a, const T& c, const kfr::univector<T> & y) { kfr::univector<T> r; r = kfr::mixs(c,a,y); return r; }
    template<typename T> kfr::univector<T> mul(const kfr::univector<T> & a,const kfr::univector<T> & b) { kfr::univector<T> r; r = kfr::mul(a,b); return r; }
    template<typename T> kfr::univector<T> mul(const kfr::univector<T> & a, const T & b) { kfr::univector<T> r; r = kfr::mul(a,b); return r; }
    template<typename T> kfr::univector<T> neg(const kfr::univector<T> & a) { kfr::univector<T> r; r = kfr::neg(a); return r; }        
    template<typename T> kfr::univector<T> sqr(const kfr::univector<T> & a) { kfr::univector<T> r; r = kfr::sqr(a); return r; }
    template<typename T> kfr::univector<T> sqrt(const kfr::univector<T> & a) { kfr::univector<T> r; r = kfr::sqrt(a); return r; }
    template<typename T> kfr::univector<T> exp(const kfr::univector<T> & a) { kfr::univector<T> r; r = kfr::exp(a); return r; }
    template<typename T> kfr::univector<T> exp10(const kfr::univector<T> & a) { kfr::univector<T> r; r = kfr::exp10(a); return r; }
    template<typename T> kfr::univector<T> exp2(const kfr::univector<T> & a) { kfr::univector<T> r; r = kfr::exp2(a); return r; }
    template<typename T> kfr::univector<T> exp_fmadd(const kfr::univector<T> & a,const kfr::univector<T> & y, const kfr::univector<T> & z) { kfr::univector<T> r; r = kfr::exp_fmadd(a,y,z); return r; }
    template<typename T> kfr::univector<T> log(const kfr::univector<T> & a) { kfr::univector<T> r; r = kfr::log(a); return r; }
    template<typename T> kfr::univector<T> log10(const kfr::univector<T> & a) { kfr::univector<T> r; r = kfr::log10(a); return r; }
    template<typename T> kfr::univector<T> log2(const kfr::univector<T> & a) { kfr::univector<T> r; r = kfr::log2(a); return r; }
    template<typename T> kfr::univector<T> log_fmadd(const kfr::univector<T> & a,const kfr::univector<T> & y, const kfr::univector<T> & z) { kfr::univector<T> r; r = kfr::log_fmadd(a,y,z); return r; }
    template<typename T> kfr::univector<T> logb(const kfr::univector<T> & a) { kfr::univector<T> r; r = kfr::logb(a); return r; }
    template<typename T> kfr::univector<T> logm(const kfr::univector<T> & a,const kfr::univector<T> & b) { kfr::univector<T> r; r = kfr::logm(a,b); return r; }
    template<typename T> kfr::univector<T> logn(const kfr::univector<T> & a,const kfr::univector<T> & b) { kfr::univector<T> r; r = kfr::logn(a,b); return r; }
    template<typename T> kfr::univector<T> pow(const kfr::univector<T> & a,const T & y) { kfr::univector<T> r; r = kfr::pow(a,y); return r; }
    template<typename T> kfr::univector<T> pow(const kfr::univector<T> & a,const kfr::univector<T> & y) { kfr::univector<T> r; r = kfr::pow(a,y); return r; }
    template<typename T> kfr::univector<T> root(const kfr::univector<T> & a,const kfr::univector<T> & y) { kfr::univector<T> r; r = kfr::root(a,y); return r; }
    template<typename T> kfr::univector<T> floor(const kfr::univector<T> & a) { kfr::univector<T> r; r = kfr::floor(a); return r; }        
    template<typename T> kfr::univector<T> acos(const kfr::univector<T> & a) { kfr::univector<T> r; r = kfr::acos(a); return r; }
    template<typename T> kfr::univector<T> asin(const kfr::univector<T> & a) { kfr::univector<T> r; r = kfr::asin(a); return r; }
    template<typename T> kfr::univector<T> atan(const kfr::univector<T> & a) { kfr::univector<T> r; r = kfr::atan(a); return r; }
    template<typename T> kfr::univector<T> atan2(const kfr::univector<T> & a,const T & b) { kfr::univector<T> r; r = kfr::atan2(a,b); return r; }
    template<typename T> kfr::univector<T> atan2(const kfr::univector<T> & a,const kfr::univector<T> & b) { kfr::univector<T> r; r = kfr::atan2(a,b); return r; }
    template<typename T> kfr::univector<T> atan2deg(const kfr::univector<T> & a,const T & b) { kfr::univector<T> r; r = kfr::atan2deg(a,b); return r; }
    template<typename T> kfr::univector<T> atan2deg(const kfr::univector<T> & a,const kfr::univector<T> & b) { kfr::univector<T> r; r = kfr::atan2deg(a,b); return r; }
    template<typename T> kfr::univector<T> atandeg(const kfr::univector<T> & a) { kfr::univector<T> r; r = kfr::atandeg(a); return r; }
    template<typename T> kfr::univector<T> cos(const kfr::univector<T> & a) { kfr::univector<T> r; r = kfr::cos(a); return r; }
    template<typename T> kfr::univector<T> sin(const kfr::univector<T> & a) { kfr::univector<T> r; r = kfr::sin(a); return r; }    
    template<typename T> kfr::univector<T> cosdeg(const kfr::univector<T> & a) { kfr::univector<T> r; r = kfr::cosdeg(a); return r; }        
    template<typename T> kfr::univector<T> sindeg(const kfr::univector<T> & a) { kfr::univector<T> r; r = kfr::cosdeg(a); return r; }    
    template<typename T> kfr::univector<T> sinc(const kfr::univector<T> & a) { kfr::univector<T> r; r = kfr::sinc(a); return r; }
    template<typename T> kfr::univector<T> tan(const kfr::univector<T> & a) { kfr::univector<T> r; r = kfr::tan(a); return r; }        
    template<typename T> kfr::univector<T> cosh(const kfr::univector<T> & a) { kfr::univector<T> r; r = kfr::cosh(a); return r; }
    template<typename T> kfr::univector<T> coth(const kfr::univector<T> & a) { kfr::univector<T> r; r = kfr::coth(a); return r; }    
    template<typename T> kfr::univector<T> sinh(const kfr::univector<T> & a) { kfr::univector<T> r; r = kfr::sinh(a); return r; }    
    template<typename T> kfr::univector<T> tanh(const kfr::univector<T> & a) { kfr::univector<T> r; r = kfr::tanh(a); return r; }
    template<typename T> kfr::univector<T> gamma(const kfr::univector<T> & a) { kfr::univector<T> r; r = kfr::gamma(a); return r; }

    template<typename T> T absmaxof(const kfr::univector<T> & a) { return kfr::absmaxof(a); }
    template<typename T> T absminof(const kfr::univector<T> & a) { return kfr::absminof(a); }
    template<typename T> T dot(const kfr::univector<T> & a,const kfr::univector<T> & b) { return kfr::dotproduct(a,b); }
    template<typename T> T maxof(const kfr::univector<T> & a) { return kfr::maxof(a); }
    template<typename T> T minof(const kfr::univector<T> & a) { return kfr::minof(a); }
    template<typename T> T mean(const kfr::univector<T> & a) { return kfr::mean(a); }
    template<typename T> T product(const kfr::univector<T> & a) { return kfr::product(a); }
    template<typename T> T rms(const kfr::univector<T> & a) { return kfr::rms(a); }
    template<typename T> T sum(const kfr::univector<T> & a) { return kfr::sum(a); }
    template<typename T> T sumsqr(const kfr::univector<T> & a) { return kfr::sumsqr(a); }

    // doesn't compile
    //template<typename T>    
    //kfr::univector<T> div(const kfr::univector<T> & a,const T b) { kfr::univector<T> r; r = kfr::div<T>(a,b); return r; }

    template<typename T>    
    kfr::univector<T> ipow(const kfr::univector<T> & v, int base) { kfr::univector<T> r; r = kfr::ipow(v,base); return r; }

    template<typename T>    
    T kcos2x(const T s, const T c) {return kfr::cos2x<SampleType>(s,c); }

    template<typename T>    
    T kcos3x(const T & s, const T & c) {return kfr::cos3x(s,c); }

    template<typename T>    
    T ksin2x(const T & s, const T & c) {return kfr::sin2x(s,c); }

    template<typename T>    
    T ksin3x(const T & s, const T & c) {return kfr::sin3x(s,c); }

    template<typename T>    
    kfr::univector<T> cossin(const kfr::univector<T> & v) { kfr::univector<T> r; r = kfr::cossin(v); return r; }

    template<typename T>    
    kfr::univector<T> sincos(const kfr::univector<T> & v) { kfr::univector<T> r; r = kfr::sincos(v); return r; }

    template<typename T>    
    T kreciprocal(const T & v) { return kfr::reciprocal(v); }

    template<typename T>    
    T rem(const T v,const T b) { return kfr::rem(v,b); }    

    template<typename T>    
    T satadd(const T v,const T y) { return kfr::satadd(v,y); }

    template<typename T>    
    T satsub(const T v,const T  y) { return kfr::satsub(v,y); }

    //? dont know how to make these work yet.
    template<typename T>    
    kfr::univector<T> fastcos(const kfr::univector<T> & v) { kfr::univector<T> r; r = kfr::fastcos(v); return r; }

    template<typename T>    
    kfr::univector<T> fastcosdeg(const kfr::univector<T> & v) { kfr::univector<T> r; r = kfr::fastcosdeg(v); return r; }

    template<typename T>    
    kfr::univector<T> fastsin(const kfr::univector<T> & v) { kfr::univector<T> r; r = kfr::fastsin(v); return r; }

    template<typename T>    
    kfr::univector<T> fastsindeg(const kfr::univector<T> & v) { kfr::univector<T> r; r = kfr::fastsindeg(v); return r; }        

    template<typename T>    
    kfr::univector<T> coshsinh(const kfr::univector<T> & v) { kfr::univector<T> r; r = kfr::coshsinh(v); return r; }

    template<typename T>    
    kfr::univector<T> sinhcosh(const kfr::univector<T> & v) { kfr::univector<T> r; r = kfr::sinhcosh(v); return r; }

    template<typename T>    
    kfr::univector<T> cossindeg(const kfr::univector<T> & v) { kfr::univector<T> r; r = kfr::cossindeg(v); return r; }    

    template<typename T>    
    kfr::univector<T> sincosdeg(const kfr::univector<T> & v) { kfr::univector<T> r; r = kfr::sincosdeg(v); return r; }    

    // I dont understand the kfr random at all yet
    template<typename T>    
    kfr::univector<T> random(size_t s) 
    {
        std::default_random_engine generator;
        std::uniform_real_distribution<T> distrib(0.0,1.0);
        kfr::univector<T> r(s);    
        for(size_t i = 0; i < s; i++)
            r[i] = distrib(generator);
        return r;
    }   

    template<typename T>    
    kfr::univector<T> random(size_t s, T min, T max) 
    {
        std::default_random_engine generator;
        std::uniform_real_distribution<T> distrib(min,max);
        kfr::univector<T> r(s);    
        for(size_t i = 0; i < s; i++)
            r[i] = distrib(generator);
        return r;
    }  

    template<typename T>
    void plot_save(const kfr::univector<T> & v, const std::string& name="", const std::string& options="") {
            kfr::plot_save(name,v,options);
        }
    template<typename T>    
    void plot_show(const kfr::univector<T> & v, const std::string& name="", const std::string&  options="") {
        kfr::plot_show(name,v,options);
    }

    template<typename T> kfr::univector<T> make_univec(const T * data, size_t s) { return kfr::univector<T>(kfr::make_univector<T>(data,s));  }


%}

%template (univector) kfr::univector<SampleType>;

%template(abs) abs<SampleType>;
%template(add) add<SampleType>;
%template(absmax) absmax<SampleType>;
%template(absmin) absmin<SampleType>;
%template(clamp) clamp<SampleType>;
%template(cube) cube<SampleType>;
%template(div) div<SampleType>;
%template(fmadd) fmadd<SampleType>;
%template(fmsub) fmsub<SampleType>;
%template(max) max<SampleType>;
%template(min) min<SampleType>;
%template(mix) mix<SampleType>;
%template(mixs) mixs<SampleType>;
%template(mul) mul<SampleType>;
%template(neg) neg<SampleType>;
%template(sqr) sqr<SampleType>;
%template(sqrt) sqrt<SampleType>;
%template(exp) exp<SampleType>;
%template(exp10) exp10<SampleType>;
%template(exp2) exp2<SampleType>;
%template(exp_fmadd) exp_fmadd<SampleType>;
%template(log) log<SampleType>;
%template(log10) log10<SampleType>;
%template(log2) log2<SampleType>;
%template(log_fmadd) log_fmadd<SampleType>;
%template(logb) logb<SampleType>;
%template(logm) logm<SampleType>;
%template(logn) logn<SampleType>;
%template(pow) pow<SampleType>;
%template(root) root<SampleType>;
%template(floor) floor<SampleType>;
%template(acos) acos<SampleType>;
%template(asin) asin<SampleType>;
%template(atan) atan<SampleType>;
%template(atan2) atan2<SampleType>;
%template(cos) cos<SampleType>;
%template(sin) sin<SampleType>;
%template(tan) tan<SampleType>;
%template(cosh) cosh<SampleType>;
%template(coth) coth<SampleType>;
%template(sinh) sinh<SampleType>;
%template(tanh) tanh<SampleType>;
%template(atan2deg) atan2deg<SampleType>;
%template(cosdeg) cosdeg<SampleType>;
%template(sindeg) sindeg<SampleType>;
%template(sinc) sinc<SampleType>;
%template(gamma) gamma<SampleType>;
%template(absmaxo) absmaxof<SampleType>;
%template(dot) dot<SampleType>;
%template(maxo) maxof<SampleType>;
%template(mino) minof<SampleType>;
%template(mean) mean<SampleType>;
%template(prdocut) product<SampleType>;
%template(rms) rms<SampleType>;
%template(sum) sum<SampleType>;
%template(sumsqr) sumsqr<SampleType>;
%template(ipow) ipow<SampleType>;
%template(cos2x) kcos2x<SampleType>;
%template(sin2x) ksin2x<SampleType>;
%template(cos3x) kcos3x<SampleType>;
%template(sin3x) ksin3x<SampleType>;
%template(reciprocal) kreciprocal<SampleType>;

%template(linspace) linspace<SampleType>;
%template(pad) pad<SampleType>;
%template(slice) slice<SampleType>;
%template(truncate) truncate<SampleType>;
%template(reverse) reverse<SampleType>;

// univector
%template(slice) slice<SampleType>;
%template(truncate) truncate<SampleType>;
%template(reverse) reverse<SampleType>;
%template(ringbuf_read) ringbuf_read<SampleType>;
%template(ringbuf_write) ringbuf_write<SampleType>;
%template (random) random<SampleType>;

// Plot
%template (plot_save) plot_save<SampleType>;
%template (plot_show) plot_show<SampleType>;

%template(make_univector) make_univec<SampleType>;

%template(acosh) acosh<SampleType>;
%template(asinh) asinh<SampleType>;
%template(atanh) atanh<SampleType>;
%template(cbrt) cbrt<SampleType>;
%template(ceil) ceil<SampleType>;
%template(copysign) copysign<SampleType>;
%template(er) erf<SampleType>;
%template(erfc) erfc<SampleType>;
%template(expm1) expm1<SampleType>;
%template(fdim) fdim<SampleType>;
%template(fma) fma<SampleType>;
//%template(fmax) fmax<SampleType>;
//%template(fmin) fmin<SampleType>;
%template(fmod) fmod<SampleType>;
%template(fpclassify) fpclassify<SampleType>;
%template(hypot) hypot<SampleType>;
%template(ilogb) ilogb<SampleType>;
%template(isfinite) isfinite<SampleType>;
%template(isgreater) isgreater<SampleType>;
%template(isgreaterequal) isgreaterequal<SampleType>;
%template(isin) isinf<SampleType>;
%template(isless) isless<SampleType>;
%template(islessequal) islessequal<SampleType>;
%template(isnan) isnan<SampleType>;
%template(isnormal) isnormal<SampleType>;
%template(isunordered) isunordered<SampleType>;
%template(ldexp) ldexp<SampleType>;
%template(lgamma) lgamma<SampleType>;
%template(llrint) llrint<SampleType>;
%template(llround) llround<SampleType>;
%template(log1p) log1p<SampleType>;
%template(lrint) lrint<SampleType>;
%template(lround) lround<SampleType>;
%template(nan) nan<SampleType>;
%template(nanf) nanf<SampleType>;
%template(nanl) nanl<SampleType>;
%template(nearbyint) nearbyint<SampleType>;
%template(nextafter) nextafter<SampleType>;
%template(nexttoward) nexttoward<SampleType>;
%template(remainder) remainder<SampleType>;
%template(rint) rint<SampleType>;
%template(round) round<SampleType>;
%template(scalbln) scalbln<SampleType>;
%template(scalbn) scalbn<SampleType>;
%template(square) square<SampleType>;
%template(tgamma) tgamma<SampleType>;
%template(trunc) trunc<SampleType>;

/*
%template (fastcos) fastcos<f32>;
%template (fastcosdeg) fastcosdeg<f32>;
%template (fastsin) fastsin<f32>;
%template (fastsindeg) fastsindeg<f32>;
%template (coshsing) coshsinh<f32>;
%template (sinhcosh) sinhcosh<f32>;
%template (cossindeg) cossindeg<f32>;
%template (sincosdeg) fastsindeg<f32>;
*/
/*
%template(sataddi64) satadd<i64>;
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
*/

%inline %{

    template<typename T>
    struct SampleVector
    {
        std::vector<kfr::univector<T>> samples;
        size_t                         channels;

        SampleVector(size_t channels) {
            samples.resize(channels);
            this->channels = channels;
        }
        
        T& operator()(size_t ch, size_t i) { return samples[ch][i]; }
        
        size_t num_channels() const { return channels; }
        size_t size() const { return samples[0].size(); }
        
        kfr::univector<T> get_channel(size_t channel) { return samples[channel]; }
        void set_channel(size_t channel, kfr::univector<T> & v) { samples[channel] = v; }

        kfr::univector<T> __getitem(size_t i ) { return samples[i]; }
        void __setitem(size_t i, kfr::univector<T> & v) { samples[i] = v; }

    };

    /*
    template<typename T>
    SampleVector<T> deinterleave(size_t channels, kfr::univector<T> & v) {
        SampleVector<T> r(channels);        
        for(size_t i = 0; i < channels; i++) {
            r.samples[i].resize(v.size()/channels)
            for(size_t j = i; j < v.size(); j += channels)
                r[channels][j] = v[j];
        }
        return r;
    }
    template<typename T>
    void interleave(SampleVector<T> & samples, kfr::univector<T> & out) {
        out.resize(samples.channels * samples[0].size());        
        for(size_t i = 0; i < samples.channels; i++)            
            for(size_t j = i; j < samples[i].size(); i+=samples.channels)
                out[j*channels + i] = samples[i][j];
        }
    */
    template<typename T>
    void copy(kfr::univector<T> & dst, std::vector<T> & src) {
        std::copy(src.begin(),src.end(),dst.begin());
    }
    template<typename T>
    void copy(std::vector<T> & dst, kfr::univector<T> & src) {
        std::copy(src.begin(),src.end(),dst.begin());
    }

%}

%template(Biquad)           DSP::Biquad<SampleType>;
%template(biquadparams)     DSP::BiQuadParams<SampleType>;
%template(biquad_filter)    DSP::biquad<SampleType>;
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

%template(dft) DSP::run_dft<SampleType>;
%template(realdft) DSP::run_realdft<SampleType>;
%template(idft) DSP::run_idft<SampleType>;
%template(irealdft) DSP::run_irealdft<SampleType>;

%template(energy_to_loudness) DSP::energy_to_loudness<SampleType>;
%template(loudness_to_energy) DSP::loudness_to_energy<SampleType>;

%template(float_wavreader) DSP::WavReader<SampleType>;
%template(float_wavwriter) DSP::WavWriter<SampleType>;
%template(float_mp3_reader) DSP::MP3Reader<SampleType>;
%template(float_flac_reader) DSP::FlacReader<SampleType>;

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
%template (float_dct_plan) DSP::DCTPlan<SampleType>;
%template (float_dft_plan) DSP::DFTPlan<SampleType>;
%template (float_dft_real_plan) DSP::DFTRealPlan<SampleType>;
%template (resample) DSP::resample<SampleType>;
%template (convert_sample) DSP::convert_sample<SampleType>;
//%template (amp_to_dB) DSP::amp_to_dB<SampleType>;

%template(interleave) DSP::do_interleave<SampleType>;
%template(deinterleave) DSP::do_deinterleave<SampleType>; 

%template(load_wav) DSP::load_wav<SampleType>;
%template(save_wav) DSP::write_wav<SampleType>;
%template(load_mp3) DSP::load_mp3<SampleType>;
%template(load_flac) DSP::load_flac<SampleType>;



namespace kfr::CMT_ARCH_NAME
{
    namespace intrinsics {
        template <typename T> univector<T> convolve(const univector_ref<const T>& src1, const univector_ref<const T>& src2);
        template <typename T> univector<T> correlate(const univector_ref<const T>& src1, const univector_ref<const T>& src2);
        template <typename T> univector<T> autocorrelate(const univector_ref<const T>& src1);
    } 
        
    template <typename T, univector_tag Tag1, univector_tag Tag2> univector<T> convolve(const univector<T, Tag1>& src1, const univector<T, Tag2>& src2);
    template <typename T, univector_tag Tag1, univector_tag Tag2> univector<T> correlate(const univector<T, Tag1>& src1, const univector<T, Tag2>& src2);        
    template <typename T, univector_tag Tag1>  univector<T> autocorrelate(const univector<T, Tag1>& src);

}
namespace kfr {
    template <typename T> class convolve_filter
    {
    public:
        
        explicit convolve_filter(size_t size, size_t block_size = 1024);
        explicit convolve_filter(const univector_ref<const T>& data, size_t block_size = 1024);
        
        void set_data(const univector_ref<const T>& data);
        void reset() final;
        /// Apply filter to multiples of returned block size for optimal processing efficiency.
        size_t input_block_size() const { return block_size; }    

        void apply(univector<T> & in);
        void apply(univector<T> & in, univector<T> & out);

    };

    template <typename T> filter<T>* make_convolve_filter(const univector_ref<const T>& taps, size_t block_size);    
}

%template (convolve_filter) kfr::convolve_filter<SampleType>;
%template (make_convolve_filter) kfr::make_convolve_filter<SampleType>;

namespace kfr {
    
    
    enum class dft_type
    {
        both,
        direct,
        inverse
    };

    enum class dft_order
    {
        normal,
        internal, // possibly bit/digit-reversed, implementation-defined, faster to compute
    };

    enum class dft_pack_format
    {
        Perm, // {X[0].r, X[N].r}, ... {X[i].r, X[i].i}, ... {X[N-1].r, X[N-1].i}
        CCs // {X[0].r, 0}, ... {X[i].r, X[i].i}, ... {X[N-1].r, X[N-1].i},  {X[N].r, 0}
    };    

    template <typename T>
    struct dft_plan
    {
        size_t size;
        size_t temp_size;

        explicit dft_plan(size_t size, dft_order order = dft_order::normal);
        ~dft_plan() {}

        void dump() const;
        void execute(complex<T>* out, const complex<T>* in, u8* temp,bool inverse = false) const;        
        

        template <bool inverse> void execute(complex<T>* out, const complex<T>* in, u8* temp, cbool_t<inverse> inv) const;

        template <univector_tag Tag1, univector_tag Tag2, univector_tag Tag3>
        void execute(univector<complex<T>, Tag1>& out, const univector<complex<T>, Tag2>& in,univector<u8, Tag3>& temp, bool inverse = false) const;
        
        template <bool inverse, univector_tag Tag1, univector_tag Tag2, univector_tag Tag3>
        void execute(univector<complex<T>, Tag1>& out, const univector<complex<T>, Tag2>& in,
                                    univector<u8, Tag3>& temp, cbool_t<inverse> inv) const;

        
        size_t data_size;
        
    };

    template <typename T>
    struct dft_plan_real
    {
        size_t size;
    
        explicit dft_plan_real(size_t size, dft_pack_format fmt = dft_pack_format::CCs);
                                
        void execute(complex<T>* out, const T* in, u8* temp) const;
        void execute(T* out, const complex<T>* in, u8* temp) const;
        template <univector_tag Tag1, univector_tag Tag2, univector_tag Tag3>
        void execute(univector<complex<T>, Tag1>& out, const univector<T, Tag2>& in,
                                    univector<u8, Tag3>& temp) const;
        template <univector_tag Tag1, univector_tag Tag2, univector_tag Tag3>
        void execute(univector<T, Tag1>& out, const univector<complex<T>, Tag2>& in,
                                    univector<u8, Tag3>& temp) const;
                
    };

    /// @brief DCT type 2 (unscaled)
    template <typename T>
    struct dct_plan
    {
        dct_plan(size_t size);
        void execute(T* out, const T* in, u8* temp, bool inverse = false) const;

        template <univector_tag Tag1, univector_tag Tag2, univector_tag Tag3>
        void execute(univector<T, Tag1>& out, const univector<T, Tag2>& in,
                                    univector<u8, Tag3>& temp, bool inverse = false) const;
    };

    
}

%template(dft_plan) kfr::dft_plan<SampleType>;
%template(dft_plan_real) kfr::dft_plan_real<SampleType>;
%template(dct_plan) kfr::dct_plan<SampleType>;

namespace kfr::CMT_ARCH_NAME 
{

    template <typename T>
    struct samplerate_converter
    {    
        using itype = i64;
        using ftype = subtype<T>;
        static size_t filter_order(sample_rate_conversion_quality quality);
        static ftype sidelobe_attenuation(sample_rate_conversion_quality quality);
        static ftype transition_width(sample_rate_conversion_quality quality);
        static ftype window_param(sample_rate_conversion_quality quality);        
        samplerate_converter(sample_rate_conversion_quality quality, itype interpolation_factor, itype decimation_factor, ftype scale = ftype(1), ftype cutoff = 0.5f);
        itype input_position_to_intermediate(itype in_pos) const;
        itype output_position_to_intermediate(itype out_pos) const;
        itype input_position_to_output(itype in_pos) const;
        itype output_position_to_input(itype out_pos) const;
        itype output_size_for_input(itype input_size) const;
        itype input_size_for_output(itype output_size) const;
        size_t skip(size_t output_size, univector<T> input);        
        size_t process(kfr::univector<T> output, univector<T> input);
        double get_fractional_delay() const;
        size_t get_delay() const;

        ftype kaiser_beta;
        itype depth;
        itype taps;
        size_t order;
        itype interpolation_factor;
        itype decimation_factor;
        //univector<T> filter;
        //univector<T> delay;
        itype input_position;
        itype output_position;
    };
}

%template(samplerate_converter) kfr::CMT_ARCH_NAME::samplerate_converter<SampleType>;

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

    namespace CMT_ARCH_NAME
    {

        template <typename T, size_t maxfiltercount = 4>
        class biquad_filter
        {
        public:            
            biquad_filter(const std::vector<biquad_params<T>>& bq);
            void apply(kfr::univector<T> & vector);
            void apply(const kfr::univector<T> & src, kfr::univector<T> & dst);
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
        
        template <typename T> T energy_to_loudness(T energy);
        template <typename T> T loudness_to_energy(T loudness);
        template <typename T>struct integrated_vec 
        {
            integrated_vec();
            void push(T mean_square);
            void reset();
            T get() const;
        };

        template <typename T>
        struct lra_vec 
        {
            lra_vec();
            void push(T mean_square);
            void reset();
            void get(T& low, T& high) const;
        };

        template <typename T> expression_pointer<T> make_kfilter(int samplerate);
        
        template <typename T>
        struct ebu_channel
        {
        public:
            
            ebu_channel(int sample_rate, Speaker speaker, int packet_size_factor = 1, T input_gain = 1);
            void reset();
            void process_packet(const T* src);
            Speaker get_speaker() ;
        };

        template <typename T>
        struct ebu_r128
        {
        public:

            ebu_r128(int sample_rate, const std::vector<Speaker>& channels, int packet_size_factor = 1);
            int sample_rate() const;
            size_t packet_size() const;

            void get_values(T& loudness_momentary, T& loudness_short, T& loudness_intergrated, T& loudness_range_low,
                            T& loudness_range_high);

            const ebu_channel<T>& operator[](size_t index) const;
            size_t count() const;

            void process_packet(const std::initializer_list<univector_dyn<T>>& source);
            void process_packet(const std::initializer_list<univector_ref<T>>& source);            
            void process_packet(const std::vector<univector<T>>& source);
            void start();
            void stop();
            void reset();
            lra_buffer;
        };        

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

        template <typename T>
        struct zpk
        {
            univector<complex<T>> z;
            univector<complex<T>> p;
            T k;
        };

        template <typename T> zpk<T> chebyshev1(int N, identity<T> rp);
        template <typename T> zpk<T> chebyshev2(int N, identity<T> rs);
        template <typename T> zpk<T> butterworth(int N);
        template <typename T> zpk<T> bessel(int N);

        template <typename T> zpk<T> iir_lowpass(const zpk<T>& filter, identity<T> frequency, identity<T> fs = 2.0);
        template <typename T> zpk<T> iir_highpass(const zpk<T>& filter, identity<T> frequency, identity<T> fs = 2.0);
        template <typename T> zpk<T> iir_bandpass(const zpk<T>& filter, identity<T> lowfreq, identity<T> highfreq, identity<T> fs = 2.0);
        template <typename T> zpk<T> iir_bandstop(const zpk<T>& filter, identity<T> lowfreq, identity<T> highfreq, identity<T> fs = 2.0);
 
        template <typename T> std::vector<biquad_params<T>> to_sos(const zpk<T>& filter);        

        template <typename T, int maximum = 1>
        struct color
        {
            constexpr static T min = std::numeric_limits<T>::min();
            constexpr static T max = std::numeric_limits<T>::max();            
            
            constexpr explicit color(T grey, T alpha = maximum);
            constexpr color(T r, T g, T b, T a = maximum);
            constexpr color(const color&);

            // @brief Convert from 0xAARRGGBB representation (BGRA in little endian)
            static constexpr color from_argb(uint32_t c);            
            constexpr color() = default;

            constexpr color lighter(double n = 1.2) const noexcept;
            constexpr color darker(double n = 1.2) const noexcept;

            constexpr T lightness() const;
            constexpr T lightness_perc() const;
            constexpr color normalize() const;

            constexpr void apply_red(T r) noexcept;
            constexpr void apply_green(T g) noexcept;
            constexpr void apply_blue(T b) noexcept;
            constexpr void apply_alpha(T a) noexcept;

            constexpr color with_red(T r) const noexcept;
            constexpr color with_green(T g) const noexcept;
            constexpr color with_blue(T b) const noexcept;
            constexpr color with_alpha(T a) const noexcept;
            constexpr color with_alpha_premul(T a) const noexcept;

            constexpr bool operator==(const color& c) const;
            constexpr bool operator!=(const color& c) const;
        };

    } 
    template <typename U, typename T> fir_filter<U>* make_fir_filter(const univector<T>& taps);
}

%inline %{

        template <typename T> T f_note_to_hertz(const T& input) {
            return kfr::note_to_hertz<T>(input);
        }    
        template <typename T> T f_hertz_to_note(const T& input) {
            return kfr::hertz_to_note<T>(input);
        }    
        template <typename T> T f_amp_to_dB(const T& input) {
            return kfr::amp_to_dB<T>(input);
        }    
        template <typename T> T f_dB_to_amp(const T& input) {
            return kfr::dB_to_amp<T>(input);
        }    
        template <typename T> T f_power_to_dB(const T& input) {
            return kfr::power_to_dB<T>(input);
        }    
        template <typename T> T f_dB_to_power(const T& input) {
            return kfr::dB_to_power<T>(input);
        }    
        
        /*
        template<typename T> kfr::complex<T> goertzal(kfr::complex<T> & result, T  omega) {
            kfr::complex<T> r(result);
            kfr::goertzal(r,omega);
            return r;
        }
        */
        

        template <typename T> T waveshaper_hardclip(T & input, double clip_level) 
        {            
            return kfr::waveshaper_hardclip(input,clip_level);
        }
        template <typename T> kfr::univector<T> waveshaper_hardclip(kfr::univector<T> & input, double clip_level) 
        {            
            kfr::univector r(input.size());
            for(size_t i = 0; i < input.size(); i++)
                r[i] = kfr::waveshaper_hardclip(input[i],clip_level);
            return r;
        }

        template <typename T> T waveshaper_tanh(T & input, double sat) 
        {            
            return kfr::waveshaper_tanh(input,sat);
        }
        template <typename T> kfr::univector<T> waveshaper_tanh(kfr::univector<T> & input, double sat) 
        {            
            kfr::univector r(input.size());
            for(size_t i = 0; i < input.size(); i++)
                r[i] = kfr::waveshaper_tanh(input[i],sat);
            return r;
        }

        template <typename T> T waveshaper_saturate_I(T & input, double sat) 
        {            
            return kfr::waveshaper_saturate_I(input,sat);
        }
        template <typename T> kfr::univector<T> waveshaper_saturate_I(kfr::univector<T> & input, double sat) 
        {            
            kfr::univector r(input.size());
            for(size_t i = 0; i < input.size(); i++)
                r[i] = kfr::waveshaper_saturate_I(input[i],sat);
            return r;
        }

        template <typename T> T waveshaper_saturate_II(T & input, double sat) 
        {            
            return kfr::waveshaper_saturate_II(input, sat);
        }
        template <typename T> kfr::univector<T> waveshaper_saturate_II(kfr::univector<T> & input, double sat) 
        {            
            kfr::univector r(input.size());
            for(size_t i = 0; i < input.size(); i++)
                r[i] = kfr::waveshaper_saturate_II(input[i],sat);
            return r;
        }
        /*
        template <typename T> T waveshaper_poly(T & input) 
        {            
            return kfr::waveshaper_poly(input);
        }
        template <typename T> kfr::univector<T> waveshaper_poly(kfr::univector<T> & input) 
        {            
            kfr::univector r(input.size());
            for(size_t i = 0; i < input.size(); i++)
                r[i] = kfr::waveshaper_poly(input[i]);
            return r;
        }
        */

%}

%template(waveshaper_hardclip) waveshaper_hardclip<SampleType>;
%template(waveshaper_tanh) waveshaper_tanh<SampleType>;
%template(waveshaper_saturateI) waveshaper_saturate_I<SampleType>;
%template(waveshaper_saturateII) waveshaper_saturate_II<SampleType>;
//%template(waveshaper_poly) waveshaper_poly<SampleType>;


%template(note_to_hertz) f_note_to_hertz<SampleType>;
%template(hertz_to_note) f_hertz_to_note<SampleType>;
%template(amp_to_dB) f_amp_to_dB<SampleType>;
%template(dB_to_amp) f_dB_to_amp<SampleType>;
%template(power_to_dB) f_power_to_dB<SampleType>;
%template(dB_to_power) f_dB_to_power<SampleType>;