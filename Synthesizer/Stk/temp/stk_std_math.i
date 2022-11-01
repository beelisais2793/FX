%module stdmath
%{
#include <complex>
#include <valarray>
#include <fenv.h>
%}

%include "std_complex.i"
%include "std_valarray.i"
%template(float_complex) std::complex<float>;
%template(double_complex) std::complex<double>;

%template(float_valarray) std::valarray<float>;

//%include "std_eigen.i"

%include "std_limits.i"

%template(float_numeric_limits) std::numeric_limits<float>;
%template(double_numeric_limits) std::numeric_limits<double>;
%template(int_numeric_limits) std::numeric_limits<int>;
%template(uint_numeric_limits) std::numeric_limits<unsigned int>;
%template(short_numeric_limits) std::numeric_limits<short>;
%template(ushort_numeric_limits) std::numeric_limits<unsigned short>;
%template(long_numeric_limits) std::numeric_limits<long>;
%template(ulong_numeric_limits) std::numeric_limits<unsigned long>;
%template(llong_numeric_limits) std::numeric_limits<long long>;
%template(ullong_numeric_limits) std::numeric_limits<unsigned long long>;


%inline %{

    typedef float f32;
    typedef double f64;
    typedef signed char i8;
    typedef unsigned char u8;
    typedef signed short i16;
    typedef unsigned short u16;
    typedef signed int i32;
    typedef unsigned int u32;
    typedef signed long ilong;
    typedef unsigned long ulong;
    typedef signed long long i64;
    typedef unsigned long long u64;
    
    
    
    
    template<typename T> T abs(const T& x) { return std::abs(x); }
    template<typename T> T fabs(const T& x) { return std::fabs(x); }
    template<typename T> T acos(const T& x) { return std::acos(x); }
    template<typename T> T acosh(const T& x) { return std::acosh(x); }
    template<typename T> T asinh(const T& x) { return std::asinh(x); }
    template<typename T> T asin(const T& x) { return std::asinh(x); }
    template<typename T> T atan(const T& x) { return std::atan(x); }
    template<typename T> T atan2(const T& x,const T& y) { return std::atan2(x,y); }
    template<typename T> T atanh(const T& x) { return std::atanh(x); }
    template<typename T> T cbrt(const T& x) { return std::cbrt(x); }
    template<typename T> T ceil(const T& x) { return std::ceil(x); }    
    template<typename T> T copysign(const T& x, const T& y) { return std::copysign(x,y); }
    template<typename T> T cos(const T& x) { return std::cos(x); }
    template<typename T> T cosh(const T& x) { return std::cosh(x); }
    template<typename T> T erf(const T& x) { return std::erf(x); }
    template<typename T> T erfc(const T& x) { return std::erfc(x); }
    template<typename T> T exp(const T& x) { return std::exp(x); }
    template<typename T> T exp2(const T& x) { return std::exp2(x); }
    template<typename T> T expm1(const T& x) { return std::expm1(x); }
    template<typename T> T fdim(const T & x, const T & y) { return std::fdim(x,y); }
    template<typename T> T floor(const T & x) { return std::floor(x); }
    template<typename T> T fma(const T & x, const T & y, const T& z) { return std::fma(x,y,z); }
    template<typename T> T fmax(const T & x, const T & y) { return std::fmax(x,y); }
    template<typename T> T fmin(const T & x, const T & y) { return std::fmax(x,y); }
    template<typename T> T fmod(const T & x, const T & y) { return std::fmod(x,y); }
    template<typename T> int fpclassify(const T & x) { return std::fpclassify(x); }
    template<typename T> T hypot(const T & x, const T & y) { return std::hypot(x,y); }
    template<typename T> int ilogb(const T & x) { return std::ilogb(x); }
    template<typename T> bool isfinite(const T & x) { return std::isfinite(x); }
    template<typename T> bool isgreater(const T & x, const T & y) { return std::isgreater(x,y); }
    template<typename T> bool isgreaterequal(const T & x, const T & y) { return std::isgreaterequal(x,y); }
    template<typename T> bool isinf(const T & x) { return std::isinf(x); }
    template<typename T> bool isless(const T & x, const T & y) { return std::isless(x,y); }
    template<typename T> bool islessequal(const T & x, const T & y) { return std::islessequal(x,y); }
    template<typename T> bool islessgreater(const T & x, const T & y) { return std::islessgreater(x,y); }
    template<typename T> bool isnan(const T & x) { return std::isnan(x); }
    template<typename T> bool isnormal(const T & x) { return std::isnormal(x); }
    template<typename T> bool isunordered(const T & x, const T& y) { return std::isunordered(x,y); }
    template<typename T> T ldexp(const T & x, int exp) { return std::ldexp(x,exp); }
    template<typename T> T lgamma(const T & x) { return std::lgamma(x); }
    template<typename T> T llrint(const T & x) { return std::llrint(x); }
    template<typename T> T llround(const T & x) { return std::llround(x); }
    template<typename T> T log(const T & x) { return std::log(x); }
    template<typename T> T log10(const T & x) { return std::log10(x); }
    template<typename T> T log1p(const T & x) { return std::log1p(x); }
    template<typename T> T log2(const T & x) { return std::log2(x); }
    template<typename T> T logb(const T & x) { return std::logb(x); }
    template<typename T> T lrint(const T & x) { return std::lrint(x); }
    template<typename T> T lround(const T & x) { return std::lround(x); }
    template<typename T> T nan(const char *tagp) { return std::nan(tagp);}
    template<typename T> T nanf(const char *tagp) { return std::nanf(tagp);}
    template<typename T> T nanl(const char *tagp) { return std::nanl(tagp);}
    template<typename T> T nearbyint(const T &x) { return std::nearbyint(x); }
    template<typename T> T nextafter(const T & x, const T & y) { return std::nextafter(x,y); }
    template<typename T> T nexttoward(const T & x, const T & y) { return std::nexttoward(x,y); }
    template<typename T> T pow(const T & b, const T & e) { return std::pow(b,e); }
    template<typename T> T remainder(const T & n, const T & d) { return std::remainder(n,d); }
    template<typename T> T rint(const T& x) { return std::rint(x); }
    template<typename T> T round(const T& x) { return std::round(x); }
    template<typename T> T scalbln(const T& x, long int n) { return std::scalbln(x,n);}
    template<typename T> T scalbn(const T& x, int n) { return std::scalbln(x,n);}
    template<typename T> bool signbit(const T & x) { return signbit(x); }
    template<typename T> T sin(const T& x) { return std::sin(x); }
    template<typename T> T sinh(const T& x) { return std::sinh(x); }    
    template<typename T> T sqrt(const T& x) { return std::sqrt(x); }
    template<typename T> T square(const T& x) { return x*x; }
    template<typename T> T cube(const T& x) { return x*x*x; }
    template<typename T> T tan(const T& x) { return std::tan(x); }
    template<typename T> T tanh(const T& x) { return std::tanh(x); }        
    template<typename T> T tgamma(const T& x) { return std::tgamma(x); }    
    template<typename T> T trunc(const T& x) { return std::trunc(x); }
    double Huge() { return HUGE_VAL; }
    float Hugef() { return HUGE_VALF; }
    double Infinity() { return INFINITY; }
    double NaN() { return NAN; }

    
%}

%template(absf) abs<f32>;
%template(cubef) cube<f32>;
%template(sqrtf) sqrt<f32>;
%template(expf) exp<f32>;
%template(exp2f) exp2<f32>;
%template(logf) log<f32>;
%template(log10f) log10<f32>;
%template(log2f) log2<f32>;
%template(logbf) logb<f32>;
%template(powf) pow<f32>;
%template(floorf) floor<f32>;
%template(acosf) acos<f32>;
%template(asinf) asin<f32>;
%template(atanf) atan<f32>;
%template(atan2f) atan2<f32>;
%template(cosf) cos<f32>;
%template(sinf) sin<f32>;
%template(tanf) tan<f32>;
%template(coshf) cosh<f32>;
%template(sinhf) sinh<f32>;
%template(tanhf) tanh<f32>;
%template(lgammaf) lgamma<f32>;
%template(acoshf) acosh<f32>;
%template(asinhf) asinh<f32>;
%template(atanhf) atanh<f32>;
%template(cbrtf) cbrt<f32>;
%template(ceilf) cbrt<f32>;
%template(copysignf) copysign<f32>;
%template(erff) erf<f32>;
%template(erfcf) erfc<f32>;
%template(expm1f) expm1<f32>;
%template(fdimf) fdim<f32>;
%template(fmaf) fma<f32>;
%template(fmaxf) fmax<f32>;
%template(fminf) fmin<f32>;
%template(fmodf) fmod<f32>;
%template(fpclassifyf) fpclassify<f32>;
%template(hypotf) hypot<f32>;
%template(ilogbf) ilogb<f32>;
%template(isfinitef) isfinite<f32>;
%template(isgreaterf) isgreater<f32>;
%template(isgreaterequalf) isgreaterequal<f32>;
%template(isinff) isinf<f32>;
%template(islessf) isless<f32>;
%template(islessequalf) islessequal<f32>;
%template(isnanf) isnan<f32>;
%template(isnormalf) isnormal<f32>;
%template(isunorderedf) isunordered<f32>;
%template(ldexpf) ldexp<f32>;
%template(lgammaf) lgamma<f32>;
%template(llrintf) llrint<f32>;
%template(llroundf) llround<f32>;
%template(log1pf) log1p<f32>;
%template(lrintf) lrint<f32>;
%template(lroundf) lround<f32>;
%template(nanf) nan<f32>;
%template(nanff) nanf<f32>;
%template(nanlf) nanl<f32>;
%template(nearbyintf) nearbyint<f32>;
%template(nextafterf) nextafter<f32>;
%template(nexttowardf) nexttoward<f32>;
%template(remainderf) remainder<f32>;
%template(rintf) rint<f32>;
%template(roundf) round<f32>;
%template(scalblnf) scalbln<f32>;
%template(scalbnf) scalbn<f32>;
%template(squaref) square<f32>;
%template(tgammaf) tgamma<f32>;
%template(truncf) trunc<f32>;

%template(absd) abs<f64>;
%template(sqrtd) sqrt<f64>;
%template(expd) exp<f64>;
%template(exp2d) exp2<f64>;
%template(logd) log<f64>;
%template(log10d) log10<f64>;
%template(log2d) log2<f64>;
%template(logbd) logb<f64>;
%template(powd) pow<f64>;
%template(floord) floor<f64>;
%template(acosd) acos<f64>;
%template(asind) asin<f64>;
%template(atand) atan<f64>;
%template(atan2d) atan2<f64>;
%template(cosd) cos<f64>;
%template(sind) sin<f64>;
%template(tand) tan<f64>;
%template(coshd) cosh<f64>;
%template(sinhd) sinh<f64>;
%template(tanhd) tanh<f64>;
%template(lgammad) lgamma<f64>;
%template(acoshd) acosh<f64>;
%template(asinhd) asinh<f64>;
%template(atanhd) atanh<f64>;
%template(cbrtd) cbrt<f64>;
%template(ceild) cbrt<f64>;
%template(copysignd) copysign<f64>;
%template(erfd) erf<f64>;
%template(erfcd) erfc<f64>;
%template(expm1d) expm1<f64>;
%template(fdimd) fdim<f64>;
%template(fmad) fma<f64>;
%template(fmaxd) fmax<f64>;
%template(fmind) fmin<f64>;
%template(fmodd) fmod<f64>;
%template(fpclassifyd) fpclassify<f64>;
%template(hypotd) hypot<f64>;
%template(ilogbd) ilogb<f64>;
%template(isfinited) isfinite<f64>;
%template(isgreaterd) isgreater<f64>;
%template(isgreaterequald) isgreaterequal<f64>;
%template(isinfd) isinf<f64>;
%template(islessd) isless<f64>;
%template(islessequald) islessequal<f64>;
%template(isnand) isnan<f64>;
%template(isnormald) isnormal<f64>;
%template(isunorderedd) isunordered<f64>;
%template(ldexpd) ldexp<f64>;
%template(lgammad) lgamma<f64>;
%template(llrintd) llrint<f64>;
%template(llroundd) llround<f64>;
%template(log1pd) log1p<f64>;
%template(lrintd) lrint<f64>;
%template(lroundd) lround<f64>;
%template(nand) nan<f64>;
%template(nanfd) nanf<f64>;
%template(nanld) nanl<f64>;
%template(nearbyintd) nearbyint<f64>;
%template(nextafterd) nextafter<f64>;
%template(nexttowardd) nexttoward<f64>;
%template(remainderd) remainder<f64>;
%template(rintd) rint<f64>;
%template(roundd) round<f64>;
%template(scalblnd) scalbln<f64>;
%template(scalbnd) scalbn<f64>;
%template(squared) square<f64>;
%template(tgammad) tgamma<f64>;
%template(truncd) trunc<f64>;


%constant int fe_divbyzero = FE_DIVBYZERO;
%constant int fe_inexact = FE_INEXACT;
%constant int fe_invalid = FE_INVALID;
%constant int fe_overflow = FE_OVERFLOW;
%constant int fe_underflow = FE_UNDERFLOW;
%constant int fe_all_except = FE_ALL_EXCEPT;
%constant int fe_downward = FE_DOWNWARD;
%constant int fe_tonearest = FE_TONEAREST;
%constant int fe_towardzero = FE_TOWARDZERO;
%constant int fe_upward = FE_UPWARD;
%constant int fe_dfl_env = FE_DFL_ENV;


/*
%inline %{
typedef struct
  {
    unsigned short int __control_word;
    unsigned short int __glibc_reserved1;
    unsigned short int __status_word;
    unsigned short int __glibc_reserved2;
    unsigned short int __tags;
    unsigned short int __glibc_reserved3;
    unsigned int __eip;
    unsigned short int __cs_selector;
    unsigned int __opcode:11;
    unsigned int __glibc_reserved4:5;
    unsigned int __data_offset;
    unsigned short int __data_selector;
    unsigned short int __glibc_reserved5;
    // comment out below if 32bit
    unsigned int __mxcsr;
  }
  fenv_t;

  typedef unsigned short int fexcept_t;
%}
*/
// fenv.h
int  feclearexcept(int);
int  fegetexceptflag(fexcept_t *, int);
int  feraiseexcept(int);
int  fesetexceptflag(const fexcept_t *, int);
int  fetestexcept(int);
int  fegetround(void);
int  fesetround(int);
int  fegetenv(fenv_t *);
int  feholdexcept(fenv_t *);
int  fesetenv(const fenv_t *);
int  feupdateenv(const fenv_t *);

%constant int char_bit = CHAR_BIT;
%constant int schar_min = SCHAR_MIN;
%constant int schar_max = SCHAR_MAX;
%constant int uchar_max = UCHAR_MAX;
%constant int char_min = CHAR_MIN;
%constant int char_max = CHAR_MAX;
%constant int mb_len_max = MB_LEN_MAX;
%constant int shrt_min = SHRT_MIN;
%constant int shrt_max = SHRT_MAX;
%constant int ushrt_max = USHRT_MAX;
%constant int int_min = INT_MIN;
%constant int int_max = INT_MAX;
%constant int uint_max = UINT_MAX;
%constant int long_min = LONG_MIN;
%constant int long_max = LONG_MAX;
%constant int ulong_max = ULONG_MAX;
%constant int llong_min = LLONG_MIN;
%constant int llong_max = LLONG_MAX;
%constant int ullong_max = ULLONG_MAX;



