#include "tfp/math/Math.hpp"
#include <cmath>
#include <complex>

// ----------------------------------------------------------------------------
namespace std {
    double atof(const std::string& str) {
        return atof(str.c_str());
    }
}

namespace tfp {

const double pi = 2 * std::acos(0.0);

// ----------------------------------------------------------------------------
#if !HAVE_ASINH
double asinh(const double d)
{
    return std::log(d + std::sqrt(d*d + 1));
}
Type<Real>::Complex asinh(const Type<Real>::Complex& c)
{
    return std::log(c + std::sqrt(c*c + Type<Real>::Complex(1, 0)));
}
#endif

// ----------------------------------------------------------------------------
#if !HAVE_ACOSH
double acosh(const double d)
{
    return std::log(d + std::sqrt(d*d - 1));
}
Type<Real>::Complex acosh(const Type<Real>::Complex& c)
{
    return std::log(c + std::sqrt(c*c - Type<Real>::Complex(1, 0)));
}
#endif

// ----------------------------------------------------------------------------
#if !HAVE_ATANH
double atanh(const double d)
{
    return (std::log(1+d) - std::log(1-d)) / 2;
}
Type<Real>::Complex atanh(const Type<Real>::Complex& c)
{
    return (std::log(Type<Real>::Complex(1, 0)+c) - std::log(Type<Real>::Complex(1, 0)-c)) / Type<Real>::Complex(2, 0);
}
#endif

// ----------------------------------------------------------------------------
void complexMagnitude(double* magnitude, const kiss_fft_cpx* transformedSignal, unsigned len)
{
    while(len--)
    {
        magnitude[len] = std::sqrt(
            transformedSignal[len].r * transformedSignal[len].r +
            transformedSignal[len].i * transformedSignal[len].i
        );
    }
}

// ----------------------------------------------------------------------------
void complexPhase(double* phase, const kiss_fft_cpx* transformedSignal, unsigned len)
{
    while(len--)
        phase[len] = std::atan2(transformedSignal[len].i, transformedSignal[len].r);
}

// ----------------------------------------------------------------------------
void copyDoublesIntoCpxBuffer(kiss_fft_cpx* target, const double* src, unsigned len)
{
    while(len--)
    {
        target->i = 0;
        (target++)->r = *src++;
    }
}

// ----------------------------------------------------------------------------
void multiplyCpxBuffer(kiss_fft_cpx* target, const double product, unsigned len)
{
    while(len--)
    {
        target->i *= product;
        target->r *= product;
        ++target;
    }
}

} // namespace tfp
