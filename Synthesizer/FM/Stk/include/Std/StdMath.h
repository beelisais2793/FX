#pragma once 


#include <vector>
#include <complex>
#include <limits>
#include <valarray>
#include <random>
#include <chrono>
#include <iostream>

namespace Std {

template<class T>
struct NumericLimits
{
    std::numeric_limits<T> nl;

    bool is_specialized() { return nl.is_specialized; }
    T min() { return nl.min(); }
    T max() { return nl.max(); }
    int digits() { return nl.digits; }
    int digits10() { return nl.digits10; }
    bool is_signed() { return nl.is_signed; }
    bool is_integer() { return nl.is_integer; }
    bool is_exact() { return nl.is_exact; }
    int radix() { return nl.radix; }
    T epsilon() { return nl.epsilon(); }
    T round_error() { return nl.round_error(); }
    int min_exponent() { return nl.min_exponent; }
    int min_exponent10() { return nl.min_exponent10; }
    int max_exponent() { return nl.max_exponent; }
    int max_exponent10() { return nl.max_exponent10; }
    bool has_infinity() { return nl.has_infinity; }
    bool has_quiet_NaN() { return nl.has_quiet_NaN; }
    bool has_signaling_NaN() { return nl.has_signaling_NaN; }
    bool has_denorm_loss() { return nl.has_denorm_loss; }
    T infinity() { return nl.infinity(); }
    T quiet_NaN() { return nl.quiet_NaN(); }
    T signaling_NaN() { return nl.signaling_NaN(); }
    T denorm_min() { return nl.denorm_min(); }
    bool is_iec5599() { return nl.is_iec559; }
    bool is_bounded() { return nl.is_bounded; }
    bool is_modulo() { return nl.is_modulo; }
    bool traps() { return nl.traps; }
    bool tinyness_before() { return nl.tinyness_before; }
};

// for completeness
template<class T>
struct Math
{
    std::numeric_limits<T> limits;

    T abs(const T& x) { return std::abs(x); }
    T fabs(const T& x) { return std::fabs(x); }
    T acos(const T& x) { return std::acos(x); }
    T acosh(const T& x) { return std::acosh(x); }
    T asinh(const T& x) { return std::asinh(x); }
    T atan(const T& x) { return std::atan(x); }
    T atan2(const T& x,const T& y) { return std::atan2(x,y); }
    T atanh(const T& x) { return std::atanh(x); }
    T cbrt(const T& x) { return std::cbrt(x); }
    T ceil(const T& x) { return std::ceil(x); }    
    T copysign(const T& x, const T& y) { return std::copysign(x,y); }
    T cos(const T& x) { return std::cos(x); }
    T cosh(const T& x) { return std::cosh(x); }
    T erf(const T& x) { return std::erf(x); }
    T erfc(const T& x) { return std::erfc(x); }
    T exp(const T& x) { return std::exp(x); }
    T exp2(const T& x) { return std::exp2(x); }
    T expm1(const T& x) { return std::expm1(x); }
    T fdim(const T & x, const T & y) { return std::fdim(x,y); }
    T floor(const T & x) { return std::floor(x); }
    T fma(const T & x, const T & y, const T& z) { return std::fma(x,y,z); }
    T fmax(const T & x, const T & y) { return std::fmax(x,y); }
    T fmin(const T & x, const T & y) { return std::fmax(x,y); }
    T fmod(const T & x, const T & y) { return std::fmod(x,y); }
    int fpclassify(const T & x) { return std::fpclassify(x); }
    T hypot(const T & x, const T & y) { return std::hypot(x,y); }
    int ilogb(const T & x) { return std::ilogb(x); }
    bool isfinite(const T & x) { return std::isfinite(x); }
    bool isgreater(const T & x, const T & y) { return std::isgreater(x,y); }
    bool isgreaterequal(const T & x, const T & y) { return std::isgreaterequal(x,y); }
    bool isinf(const T & x) { return std::isinf(x); }
    bool isless(const T & x, const T & y) { return std::isless(x,y); }
    bool islessequal(const T & x, const T & y) { return std::islessequal(x,y); }
    bool islessgreater(const T & x, const T & y) { return std::islessgreater(x,y); }
    bool isnan(const T & x) { return std::isnan(x); }
    bool isnormal(const T & x) { return std::isnormal(x); }
    bool isunordered(const T & x, const T& y) { return std::isunordered(x,y); }
    T ldexp(const T & x, int exp) { return std::ldexp(x,exp); }
    T lgamma(const T & x) { return std::lgamma(x); }
    T llrint(const T & x) { return std::llrint(x); }
    T llround(const T & x) { return std::llround(x); }
    T log(const T & x) { return std::log(x); }
    T log10(const T & x) { return std::log10(x); }
    T log1p(const T & x) { return std::log1p(x); }
    T log2(const T & x) { return std::log2(x); }
    T logb(const T & x) { return std::logb(x); }
    T lrint(const T & x) { return std::lrint(x); }
    T lround(const T & x) { return std::lround(x); }
    T nan(const char *tagp) { return std::nan(tagp);}
    T nanf(const char *tagp) { return std::nanf(tagp);}
    T nanl(const char *tagp) { return std::nanl(tagp);}
    T nearbyint(const T &x) { return std::nearbyint(x); }
    T nextafter(const T & x, const T & y) { return std::nextafter(x,y); }
    T nexttoward(const T & x, const T & y) { return std::nexttoward(x,y); }
    T pow(const T & b, const T & e) { return std::pow(b,e); }
    T remainder(const T & n, const T & d) { return std::remainder(n,d); }
    T rint(const T& x) { return std::rint(x); }
    T round(const T& x) { return std::round(x); }
    T scalbln(const T& x, long int n) { return std::scalbln(x,n);}
    T scalbn(const T& x, int n) { return std::scalbln(x,n);}
    bool signbit(const T & x) { return signbit(x); }
    T sin(const T& x) { return std::sin(x); }
    T sinh(const T& x) { return std::sinh(x); }    
    T sqrt(const T& x) { return std::sqrt(x); }
    T square(const T& x) { return x*x; }
    T cube(const T& x) { return x*x*x; }
    T tan(const T& x) { return std::tan(x); }
    T tanh(const T& x) { return std::tanh(x); }        
    T tgamma(const T& x) { return std::tgamma(x); }    
    T trunc(const T& x) { return std::trunc(x); }
    double Huge() { return HUGE_VAL; }
    float Hugef() { return HUGE_VALF; }
    double Infinity() { return INFINITY; }
    double NaN() { return NAN; }
};
}
