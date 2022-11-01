#pragma once

#include "tfp/config.hpp"
#include "tfp/util/Type.hpp"
#include "kiss_fft/kiss_fft.h"

#include <string>
#include <cstdlib>

#ifdef TFP_PLATFORM_WINDOWS
#   define isnan(x) _isnan(x)
#   define isinf(x) (!_finite(x))
#   define fpu_error(x) (isinf(x) || isnan(x))
#else
#   define fpu_error(x) (std::isinf(x) || std::isnan(x))
#endif

// We require an overload of atof() for std::string. It just forwards c_str()
namespace std {
    double atof(const std::string& str);
}

namespace tfp {

extern const double pi;

template <typename T> int sign(T val) {
    return (T(0) < val) - (val < T(0));
}

#if !HAVE_ASINH
TFP_PUBLIC_API double asinh(const double d);
TFP_PUBLIC_API Type<Real>::Complex asinh(const Type<Real>::Complex& c);
#endif
#if !HAVE_ACOSH
TFP_PUBLIC_API double acosh(const double d);
TFP_PUBLIC_API Type<Real>::Complex acosh(const Type<Real>::Complex& c);
#endif
#if !HAVE_ATANH
TFP_PUBLIC_API double atanh(const double d);
TFP_PUBLIC_API Type<Real>::Complex atanh(const Type<Real>::Complex& c);
#endif

TFP_PUBLIC_API void complexMagnitude(double* magnitude, const kiss_fft_cpx* transformedSignal, unsigned len);
TFP_PUBLIC_API void complexPhase(double* phase, const kiss_fft_cpx* transformedSignal, unsigned len);
TFP_PUBLIC_API void copyDoublesIntoCpxBuffer(kiss_fft_cpx* target, const double* src, unsigned len);
TFP_PUBLIC_API void multiplyCpxBuffer(kiss_fft_cpx* target, const double product, unsigned len);

template <class T, class U>
void copyBuffer(T* target, const U* src, unsigned len)
{
    while(len--)
        *target++ = *src++;
}

} // namespace tfp
