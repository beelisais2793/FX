// This file is part of DSP library containing useful reusable
// signal processing utility classes.
//
// Copyright (C) 2018 Duncan Crutchley
// Contact <dac1976github@outlook.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License and GNU Lesser General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License
// and GNU Lesser General Public License along with this program. If
// not, see <http://www.gnu.org/licenses/>.

/*!
 * \file dsp_roots.hpp
 * \brief File containing generic accurate definitions for sqrt(2) for any floating point type.
 */
#pragma once

#include <type_traits>
#include <cmath>
#include <cassert>
#include <stdexcept>
#include <iterator>
#include <vector>
#include <complex>
#include <algorithm>
#include <functional>

/*! \brief Macro to use to assert a value.*/
#define DSP_ASSERT_THROW(x, s)                                                                     \
    do                                                                                             \
    {                                                                                              \
        assert(x);                                                                                 \
        if (!(x))                                                                                  \
            throw std::runtime_error(s);                                                           \
    } while (false)

/*! \brief dsp namespace */
namespace dsp
{

/*!
 * \brief Function to compute Pi using acos(-1).
 * \return Result represented as a floating point value of type T.
 *
 * This function only has computational cost when first called. All
 * subsequent calls simply return the static const result from
 * the first time the function was called.
 *
 * If called with anything other than a flaoting point type for T
 * then a static_assert will fire reporting an invalid type.
 */
template <typename FloatType> FloatType Pi()
{
    static_assert(std::is_floating_point<FloatType>::value, "invalid floating point type");
    static const auto result = std::acos(static_cast<FloatType>(-1));
    return result;
}

/*!
 * \brief Function to compute Pi/2.
 * \return Result represented as a floating point value of type T.
 *
 * This function only has computational cost when first called. All
 * subsequent calls simply return the static const result from
 * the first time the function was called.
 *
 * If called with anything other than a flaoting point type for T
 * then a static_assert will fire reporting an invalid type.
 */
template <typename FloatType> FloatType HalfPi()
{
    static const auto result = Pi<FloatType>() / static_cast<FloatType>(2);
    return result;
}

/*!
 * \brief Function to compute Pi/4.
 * \return Result represented as a floating point value of type T.
 *
 * This function only has computational cost when first called. All
 * subsequent calls simply return the static const result from
 * the first time the function was called.
 *
 * If called with anything other than a flaoting point type for T
 * then a static_assert will fire reporting an invalid type.
 */
template <typename FloatType> FloatType QuarterPi()
{
    static const auto result = HalfPi<FloatType>() / static_cast<FloatType>(2);
    return result;
}

/*!
 * \brief Function to compute 2Pi.
 * \return Result represented as a floating point value of type T.
 *
 * This function only has computational cost when first called. All
 * subsequent calls simply return the static const result from
 * the first time the function was called.
 *
 * If called with anything other than a flaoting point type for T
 * then a static_assert will fire reporting an invalid type.
 */
template <typename FloatType> FloatType TwoPi()
{
    static const auto result = static_cast<FloatType>(2) * Pi<FloatType>();
    return result;
}

/*!
 * \brief Function to compute 1/Pi.
 * \return Result represented as a floating point value of type T.
 *
 * This function only has computational cost when first called. All
 * subsequent calls simply return the static const result from
 * the first time the function was called.
 *
 * If called with anything other than a flaoting point type for T
 * then a static_assert will fire reporting an invalid type.
 */
template <typename FloatType> FloatType OneOverPi()
{
    static const auto result = static_cast<FloatType>(1) / Pi<FloatType>();
    return result;
}

/*!
 * \brief Function to compute 2/Pi.
 * \return Result represented as a floating point value of type T.
 *
 * This function only has computational cost when first called. All
 * subsequent calls simply return the static const result from
 * the first time the function was called.
 *
 * If called with anything other than a flaoting point type for T
 * then a static_assert will fire reporting an invalid type.
 */
template <typename FloatType> FloatType TwoOverPi()
{
    static const auto result = static_cast<FloatType>(2) * OneOverPi<FloatType>();
    return result;
}

/*!
 * \brief Function to compute 3*Pi/2.
 * \return Result represented as a floating point value of type T.
 *
 * This function only has computational cost when first called. All
 * subsequent calls simply return the static const result from
 * the first time the function was called.
 *
 * If called with anything other than a flaoting point type for T
 * then a static_assert will fire reporting an invalid type.
 */
template <typename FloatType> FloatType ThreeOverTwoPi()
{
    static const auto result = static_cast<FloatType>(3. / 2.) * Pi<FloatType>();
    return result;
}

/*!
 * \brief Function to compute 2/sqrt(Pi).
 * \return Result represented as a floating point value of type T.
 *
 * This function only has computational cost when first called. All
 * subsequent calls simply return the static const result from
 * the first time the function was called.
 */
template <typename FloatType> FloatType TwoOverSqrtPi()
{
    static const auto result = static_cast<FloatType>(2) / std::sqrt(Pi<FloatType>());
    return result;
}


/*!
 * \brief Perform discrete convolution on 2 data ranges.
 * \param[in] first1 - Start of the first range.
 * \param[in] last1 - End of the first range.
 * \param[in] first2 - Start of the second range.
 * \param[in] last2 - End of the second range.
 * \param[out] result - Start of the result range.
 *
 * Iterator result must point to a range with space for
 * M + N - 1 elements, where range 1 contains M elements
 * and range 2 contains N elements. None of the input or
 * output ranges should overlap otherwise the behaviour
 * is undefined.
 *
 * Formula for the discrete convolution, y(n), of finite
 * sequences x(n) and h(n), where x(n) has M values and
 * h(n) has N values, is:
 *
 * y(n) = SUM[k=0..n]{x(k)*h(N-1-k)}
 * for n=0..M+N-2
 */
template <typename InIter1, typename InIter2, typename OutIter>
void Convolve(InIter1 first1, InIter1 last1, InIter2 first2, InIter2 last2, OutIter result)
{
    using value_t = typename std::iterator_traits<OutIter>::value_type;

    auto M = std::distance(first1, last1);
    DSP_ASSERT_THROW(M > 0, "range 1 invalid");

    auto N = std::distance(first2, last2);
    DSP_ASSERT_THROW(N > 0, "range 2 invalid");

    auto K = M + N - 1;

    for (auto k = 0; k < K; ++k)
    {
        *result   = value_t(0);
        auto kMin = k >= N - 1 ? k - (N - 1) : 0;
        auto kMax = k < M - 1 ? k : M - 1;

        for (auto kSub = kMin; kSub <= kMax; ++kSub)
        {
            *result +=
                static_cast<value_t>(*std::next(first1, kSub) * *std::next(first2, k - kSub));
        }

        std::advance(result, 1);
    }
}

/*!
 * \brief Zeroth-order modified Bessel function of the first kind
 * \param[in] x - Value to compute bessel function for.
 * \return Bessel value of x.
 *
 * We'll use this to create a Kaiser window.
 */
template <typename FloatType> FloatType Bessel(FloatType x)
{
    static_assert(std::is_floating_point<FloatType>::value, "invalid floating point type");

    FloatType sum{0};

    for (int i = 1; i < 10; ++i)
    {
        auto xToIpower = std::pow(x / 2, static_cast<FloatType>(i));
        int  factorial = 1;

        for (int j = 1; j <= i; ++j)
        {
            factorial *= j;
        }

        sum += std::pow(xToIpower / factorial, static_cast<FloatType>(2));
    }

    return sum + static_cast<FloatType>(1);
}

/*!
 * \brief Sinc function.
 * \param[in] x - Value to compute sinc function for.
 * \param[in] limitThreshold - at x = 0 sinc(x) is defined as 1 use this limit to control when to
 * set sinc = 1.
 * \return Sinc value.
 *
 * This is the unnormalised "classic" version of sinc.
 */
template <typename FloatType>
FloatType Sinc(FloatType x, FloatType limitThreshold = FloatType(1.e-9))
{
    static_assert(std::is_floating_point<FloatType>::value, "invalid floating point type");

    if (std::abs(x) < limitThreshold)
    {
        return static_cast<FloatType>(1);
    }

    return sin(x) / x;
}

/*!
 * \brief Normalised sinc function.
 * \param[in] x - Value to compute sinc function for.
 * \param[in] limitThreshold - at x = 0 sinc(x) is defined as 1 use this limit to control when to
 * set sinc = 1.
 * \return Normalised sinc value.
 *
 * This is the normalised version of sinc.
 */
template <typename FloatType>
FloatType SincNorm(FloatType x, FloatType limitThreshold = FloatType(1.e-9))
{
    static_assert(std::is_floating_point<FloatType>::value, "invalid floating point type");

    if (std::abs(x) < limitThreshold)
    {
        return FloatType(1);
    }

    auto X = Pi<FloatType>() * x;
    return sin(X) / X;
}

/*!
 * \brief Sinusoidal equation.
 * \param[in] amplitude - Peak amplitude of sine wave.
 * \param[in] time - Time point in seconds.
 * \param[in] frequency - Signal frequency in Hz.
 * \param[in] phase - Phase offset in radians.
 * \param[in] offset - Amplitude offset.
 * \return Point on sine wave for given time.
 *
 * y(t) = A.sin(2.Pi.f.t + p) + o
 */
template <typename FloatType>
FloatType Sine(FloatType amplitude, FloatType time, FloatType frequency, FloatType phase,
               FloatType offset)
{
    static_assert(std::is_floating_point<FloatType>::value, "invalid floating point type");
    return (amplitude * sin((TwoPi<FloatType>() * frequency * time) + phase)) + offset;
}

/*!
 * \brief Computes GCD of two unsigned values using Binary GCD algorithm.
 * \param[in] a - First value.
 * \param[in] b - Second value.
 * \return The GCD of the two inputs.
 */
template <typename UintType> UintType Gcd(UintType a, UintType b)
{
    static_assert(std::is_unsigned<UintType>::value, "invalid unsigned type");

    if (a == b)
    {
        return a;
    }

    if (0 == a)
    {
        return b;
    }

    if (0 == b)
    {
        return a;
    }

    if (~a & 1)
    {
        if (b & 1)
        {
            return Gcd(a >> 1, b);
        }
        else
        {
            return Gcd(a >> 1, b >> 1) << 1;
        }
    }

    if (~b & 1)
    {
        return Gcd(a, b >> 1);
    }

    if (a > b)
    {
        return Gcd((a - b) >> 1, b);
    }

    return Gcd((b - a) >> 1, a);
}

/*!
 * \brief Check if a positive integer is a power of 2.
 * \param[in] n - the number to test.
 * \return True if a power of 2 false otherwise.
 */
template <typename IntType> bool IsPowerOf2(IntType n)
{
    static_assert(std::is_integral<IntType>::value, "invalid integer type");

    return (n > 0) && ((n & (n - 1)) == 0);
}

/*!
 * \brief Function to compute sqrt(2) for any floating point type.
 * \return Result represented as a floating point value of type T.
 *
 * This function only has computational cost when first called. All
 * subsequent calls simply return the static const result from
 * the first time the function was called.
 *
 * If called with anything other than a flaoting point type for T
 * then a static_assert will fire reporting an invalid type.
 */
template <typename FloatType> FloatType SqrtTwo()
{
    static_assert(std::is_floating_point<FloatType>::value, "invalid floating point type");
    static const auto result = std::sqrt(static_cast<FloatType>(2));
    return result;
}

/*!
 * \brief Function to compute 2sqrt(2) for any floating point type.
 * \return Result represented as a floating point value of type T.
 *
 * This function only has computational cost when first called. All
 * subsequent calls simply return the static const result from
 * the first time the function was called.
 *
 * If called with anything other than a flaoting point type for T
 * then a static_assert will fire reporting an invalid type.
 */
template <typename FloatType> FloatType TwoSqrtTwo()
{
    static const auto result = static_cast<FloatType>(2) * SqrtTwo<FloatType>();
    return result;
}

/*!
 * \brief Function to compute 1/sqrt(2) for any floating point type.
 * \return Result represented as a floating point value of type T.
 *
 * This function only has computational cost when first called. All
 * subsequent calls simply return the static const result from
 * the first time the function was called.
 *
 * If called with anything other than a flaoting point type for T
 * then a static_assert will fire reporting an invalid type.
 */
template <typename FloatType> FloatType OneOverSqrtTwo()
{
    static const auto result = static_cast<FloatType>(1) / SqrtTwo<FloatType>();
    return result;
}

/*! \brief Struct holding params to define a sinusoidal tone. */
struct ToneParams
{
    /*! \brief Peak amplitude of sine wave. */
    double amplitude{0};
    /*! \brief Signal frequency in Hz. */
    double frequency{0};
    /*! \brief Phase offset in radians. */
    double phase{0};
    /*! \brief Amplitude (DC) offset. */
    double offset{0};
};

/*!
 * \brief Sinusoidal single tone generator.
 * \param[in] params - Struct holding params to define a sinusoidal signal.
 * \param[in] sampleRate - Sample rate in Hz of generated samples.
 * \param[in] count - Number of samples to generate.
 * \return Series of samples representing a sinusoisal signal.
 *
 * The sampleRate should be >= 2*frequency so that Nyquist's
 * theorem holds.
 */
template <typename FloatType>
std::vector<FloatType> Tone(ToneParams const& params, double sampleRate, size_t count)
{
    static_assert(std::is_floating_point<FloatType>::value, "invalid floating point type");
    std::vector<FloatType> samples(count, static_cast<FloatType>(0));
    auto                   timeInterval = static_cast<FloatType>(1. / sampleRate);
    auto                   time         = static_cast<FloatType>(0);

    for (auto& sample : samples)
    {
        sample = Sine(static_cast<FloatType>(params.amplitude),
                      time,
                      static_cast<FloatType>(params.frequency),
                      static_cast<FloatType>(params.phase),
                      static_cast<FloatType>(params.offset));
        time += timeInterval;
    }

    return samples;
}

/*!
 * \brief Sinusoidal multi-tone generator.
 * \param[in] allParams - Struct holding params to define a sinusoidal signal.
 * \param[in] sampleRate - Sample rate in Hz of generated samples.
 * \param[in] count - Number of samples to generate.
 * \return Series of samples representing a sinusoisal signal.
 *
 * The sampleRate should be >= 2*max(frequency) so that Nyquist's
 * theorem holds.
 */
template <typename FloatType>
std::vector<FloatType> MultiTone(std::vector<ToneParams> const& allParams, double sampleRate,
                                 size_t count)
{
    static_assert(std::is_floating_point<FloatType>::value, "invalid floating point type");
    std::vector<FloatType> samples(count, static_cast<FloatType>(0));

    auto timeInterval = static_cast<FloatType>(1. / sampleRate);
    auto time         = static_cast<FloatType>(0);

    for (auto& sample : samples)
    {
        for (auto const& params : allParams)
        {
            sample += Sine(static_cast<FloatType>(params.amplitude),
                           time,
                           static_cast<FloatType>(params.frequency),
                           static_cast<FloatType>(params.phase),
                           static_cast<FloatType>(params.offset));
        }

        time += timeInterval;
    }

    return samples;
}

/*!
 * \brief Function to compute 1/2sqrt(2) for any floating point type.
 * \return Result represented as a floating point value of type T.
 *
 * This function only has computational cost when first called. All
 * subsequent calls simply return the static const result from
 * the first time the function was called.
 *
 * If called with anything other than a flaoting point type for T
 * then a static_assert will fire reporting an invalid type.
 */
template <typename FloatType> FloatType OneOverTwoSqrtTwo()
{
    static const auto result = static_cast<FloatType>(1) / TwoSqrtTwo<FloatType>();
    return result;
}

/*!
 * \brief Generate a vector of window coefficients.
 * \param[in,out] windowCoeffs - Pre-sized vector to receive window coefficients.
 * \param[in] evalCoeff - function or functor to compute a single window coefficient.
 *
 * Uses symmetry to optimise computation time needed to generate the
 * window coefficients.
 */
template <typename FloatType, typename EvalFunc>
void WindowGenerator(std::vector<FloatType>& windowCoeffs, EvalFunc evalCoeff)
{
    auto size = windowCoeffs.size();
    DSP_ASSERT_THROW(size > 1, "invalid window size");
    auto sizeMinusOne = static_cast<FloatType>(size - 1);
    auto halfSize     = size >> 1;

    for (size_t n = 0, nRev = size - 1; n < halfSize; ++n, --nRev)
    {
        windowCoeffs[n]    = evalCoeff(static_cast<FloatType>(n), sizeMinusOne);
        windowCoeffs[nRev] = windowCoeffs[n];
    }

    if (1 == size % 2)
    {
        windowCoeffs[halfSize] = evalCoeff(static_cast<FloatType>(halfSize), sizeMinusOne);
    }
}

/*!
 * \brief Flat top equation coefficient evaluator
 * \param[in] n - The current window coefficient index as a floating point type T.
 * \param[in] sizeMinusOne - The window length minus 1 as a floating point type T.
 * \param[in] eqCoeffsFirst - Iterator to first flat-top equation coefficient.
 * \param[in] eqCoeffsLast - Iterator to one past the final flat-top equation coefficient.
 * \return The nth flat-top window coefficient as a floating point type T.
 *
 * Formula:
 *
 * w(n) = a0 - a1.cos(2.pi.n / N-1)
 *           + a2.cos(4.pi.n / N-1)
 *           - a3.cos(6.pi.n / N-1)
 *           + a4.cos(8.pi.n / N-1)
 *           - ...
 */
template <typename FloatType, typename Iter>
FloatType EvaluateFlatTopCoefficient(FloatType n, FloatType sizeMinusOne, Iter eqCoeffsFirst,
                                     Iter eqCoeffsLast)
{
    static const auto TWO_PI      = TwoPi<FloatType>();
    auto              numEqCoeffs = std::distance(eqCoeffsFirst, eqCoeffsLast);
    DSP_ASSERT_THROW(numEqCoeffs > 1, "invalid number of equation coefficients");
    auto twoPiN      = TWO_PI * n;
    auto sign        = static_cast<FloatType>(-1);
    auto windowCoeff = *eqCoeffsFirst;
    int  i           = 1;

    for (auto itr = std::next(eqCoeffsFirst); itr != eqCoeffsLast;
         std::advance(itr, 1), sign *= static_cast<FloatType>(-1), ++i)
    {
        windowCoeff += sign * *itr * std::cos((static_cast<FloatType>(i) * twoPiN) / sizeMinusOne);
    }

    return windowCoeff;
}

/*! \brief Flat-top generator: ISO 18431-1. */
class FlatTop1Generator final
{
public:
    /*!
     * \brief Generate a vector of window coefficients.
     * \param[in,out] windowCoeffs - Pre-sized vector to receive window coefficients.
     */
    template <typename FloatType> void operator()(std::vector<FloatType>& windowCoeffs) const
    {
        WindowGenerator(windowCoeffs,
                        std::bind(&FlatTop1Generator::EvaluateCoefficient<FloatType>,
                                  std::placeholders::_1,
                                  std::placeholders::_2));
    }

private:
    /*!
     * \brief Evaluate nth window coefficient.
     * \param[in] n - The current window coefficient index as a floating point type T.
     * \param[in] sizeMinusOne - The window length minus 1 as a floating point type T.
     * \return The nth window coefficient as a floating type T.
     */
    template <typename FloatType>
    static FloatType EvaluateCoefficient(FloatType n, FloatType sizeMinusOne)
    {
        static const std::array<FloatType, 5> eqCoeffs = {static_cast<FloatType>(1),
                                                          static_cast<FloatType>(1.933),
                                                          static_cast<FloatType>(1.286),
                                                          static_cast<FloatType>(0.388),
                                                          static_cast<FloatType>(0.0322)};
        return EvaluateFlatTopCoefficient(n, sizeMinusOne, eqCoeffs.begin(), eqCoeffs.end());
    }
};

/*! \brief Flat-top generator: 2 point. */
class FlatTop2Generator final
{
public:
    /*!
     * \brief Generate a vector of window coefficients.
     * \param[in,out] windowCoeffs - Pre-sized vector to receive window coefficients.
     */
    template <typename FloatType> void operator()(std::vector<FloatType>& windowCoeffs) const
    {
        WindowGenerator(windowCoeffs,
                        std::bind(&FlatTop2Generator::EvaluateCoefficient<FloatType>,
                                  std::placeholders::_1,
                                  std::placeholders::_2));
    }

private:
    /*!
     * \brief Evaluate nth window coefficient.
     * \param[in] n - The current window coefficient index as a floating point type T.
     * \param[in] sizeMinusOne - The window length minus 1 as a floating point type T.
     * \return The nth window coefficient as a floating type T.
     */
    template <typename FloatType>
    static FloatType EvaluateCoefficient(FloatType n, FloatType sizeMinusOne)
    {
        static const std::array<FloatType, 3> eqCoeffs = {static_cast<FloatType>(0.2810639),
                                                          static_cast<FloatType>(0.5208972),
                                                          static_cast<FloatType>(0.1980399)};
        return EvaluateFlatTopCoefficient(n, sizeMinusOne, eqCoeffs.begin(), eqCoeffs.end());
    }
};

/*! \brief Flat-top generator: alternate 4 point. */
class FlatTop3Generator final
{
public:
    /*!
     * \brief Generate a vector of window coefficients.
     * \param[in,out] windowCoeffs - Pre-sized vector to receive window coefficients.
     */
    template <typename FloatType> void operator()(std::vector<FloatType>& windowCoeffs) const
    {
        WindowGenerator(windowCoeffs,
                        std::bind(&FlatTop3Generator::EvaluateCoefficient<FloatType>,
                                  std::placeholders::_1,
                                  std::placeholders::_2));
    }

private:
    /*!
     * \brief Evaluate nth window coefficient.
     * \param[in] n - The current window coefficient index as a floating point type T.
     * \param[in] sizeMinusOne - The window length minus 1 as a floating point type T.
     * \return The nth window coefficient as a floating type T.
     */
    template <typename FloatType>
    static FloatType EvaluateCoefficient(FloatType n, FloatType sizeMinusOne)
    {
        static const std::array<FloatType, 5> eqCoeffs = {static_cast<FloatType>(0.21557895),
                                                          static_cast<FloatType>(0.41663158),
                                                          static_cast<FloatType>(0.277263158),
                                                          static_cast<FloatType>(0.083578947),
                                                          static_cast<FloatType>(0.006947368)};
        return EvaluateFlatTopCoefficient(n, sizeMinusOne, eqCoeffs.begin(), eqCoeffs.end());
    }
};

/*! \brief Flat-top generator: 3 point HP P301. */
class FlatTop4Generator final
{
public:
    /*!
     * \brief Generate a vector of window coefficients.
     * \param[in,out] windowCoeffs - Pre-sized vector to receive window coefficients.
     */
    template <typename FloatType> void operator()(std::vector<FloatType>& windowCoeffs) const
    {
        WindowGenerator(windowCoeffs,
                        std::bind(&FlatTop4Generator::EvaluateCoefficient<FloatType>,
                                  std::placeholders::_1,
                                  std::placeholders::_2));
    }

private:
    /*!
     * \brief Evaluate nth window coefficient.
     * \param[in] n - The current window coefficient index as a floating point type T.
     * \param[in] sizeMinusOne - The window length minus 1 as a floating point type T.
     * \return The nth window coefficient as a floating type T.
     */
    template <typename FloatType>
    static FloatType EvaluateCoefficient(FloatType n, FloatType sizeMinusOne)
    {
        static const std::array<FloatType, 4> eqCoeffs = {static_cast<FloatType>(0.9994484),
                                                          static_cast<FloatType>(1.911456),
                                                          static_cast<FloatType>(1.076578),
                                                          static_cast<FloatType>(0.183162)};
        return EvaluateFlatTopCoefficient(n, sizeMinusOne, eqCoeffs.begin(), eqCoeffs.end());
    }
};

/*! \brief Flat-top generator: HP 4 point. */
class FlatTop5Generator final
{
public:
    /*!
     * \brief Generate a vector of window coefficients.
     * \param[in,out] windowCoeffs - Pre-sized vector to receive window coefficients.
     */
    template <typename FloatType> void operator()(std::vector<FloatType>& windowCoeffs) const
    {
        WindowGenerator(windowCoeffs,
                        std::bind(&FlatTop5Generator::EvaluateCoefficient<FloatType>,
                                  std::placeholders::_1,
                                  std::placeholders::_2));
    }

private:
    /*!
     * \brief Evaluate nth window coefficient.
     * \param[in] n - The current window coefficient index as a floating point type T.
     * \param[in] sizeMinusOne - The window length minus 1 as a floating point type T.
     * \return The nth window coefficient as a floating type T.
     */
    template <typename FloatType>
    static FloatType EvaluateCoefficient(FloatType n, FloatType sizeMinusOne)
    {
        static const std::array<FloatType, 5> eqCoeffs = {static_cast<FloatType>(1),
                                                          static_cast<FloatType>(1.869032),
                                                          static_cast<FloatType>(1.195972),
                                                          static_cast<FloatType>(0.035928),
                                                          static_cast<FloatType>(0.030916)};
        return EvaluateFlatTopCoefficient(n, sizeMinusOne, eqCoeffs.begin(), eqCoeffs.end());
    }
};

/*! \brief Flat-top generator: Modified HP P401 5 point. */
class FlatTop6Generator final
{
public:
    /*!
     * \brief Generate a vector of window coefficients.
     * \param[in,out] windowCoeffs - Pre-sized vector to receive window coefficients.
     */
    template <typename FloatType> void operator()(std::vector<FloatType>& windowCoeffs) const
    {
        WindowGenerator(windowCoeffs,
                        std::bind(&FlatTop6Generator::EvaluateCoefficient<FloatType>,
                                  std::placeholders::_1,
                                  std::placeholders::_2));
    }

private:
    /*!
     * \brief Evaluate nth window coefficient.
     * \param[in] n - The current window coefficient index as a floating point type T.
     * \param[in] sizeMinusOne - The window length minus 1 as a floating point type T.
     * \return The nth window coefficient as a floating type T.
     */
    template <typename FloatType>
    static FloatType EvaluateCoefficient(FloatType n, FloatType sizeMinusOne)
    {
        static const std::array<FloatType, 6> eqCoeffs = {static_cast<FloatType>(1),
                                                          static_cast<FloatType>(1.93774046310203),
                                                          static_cast<FloatType>(1.32530734987255),
                                                          static_cast<FloatType>(0.43206975880342),
                                                          static_cast<FloatType>(0.04359135851569),
                                                          static_cast<FloatType>(0.00015175580171)};
        return EvaluateFlatTopCoefficient(n, sizeMinusOne, eqCoeffs.begin(), eqCoeffs.end());
    }
};

/*! \brief Flat-top generator: Rohde & Schwartz 4 point. */
class FlatTop7Generator final
{
public:
    /*!
     * \brief Generate a vector of window coefficients.
     * \param[in,out] windowCoeffs - Pre-sized vector to receive window coefficients.
     */
    template <typename FloatType> void operator()(std::vector<FloatType>& windowCoeffs) const
    {
        WindowGenerator(windowCoeffs,
                        std::bind(&FlatTop7Generator::EvaluateCoefficient<FloatType>,
                                  std::placeholders::_1,
                                  std::placeholders::_2));
    }

private:
    /*!
     * \brief Evaluate nth window coefficient.
     * \param[in] n - The current window coefficient index as a floating point type T.
     * \param[in] sizeMinusOne - The window length minus 1 as a floating point type T.
     * \return The nth window coefficient as a floating type T.
     */
    template <typename FloatType>
    static FloatType EvaluateCoefficient(FloatType n, FloatType sizeMinusOne)
    {
        static const std::array<FloatType, 5> eqCoeffs = {static_cast<FloatType>(0.1881999),
                                                          static_cast<FloatType>(0.36923),
                                                          static_cast<FloatType>(0.28702),
                                                          static_cast<FloatType>(0.13077),
                                                          static_cast<FloatType>(0.02488)};
        return EvaluateFlatTopCoefficient(n, sizeMinusOne, eqCoeffs.begin(), eqCoeffs.end());
    }
};

/*! \brief Hann window generator. */
class HannGenerator final
{
public:
    /*!
     * \brief Generate a vector of window coefficients.
     * \param[in,out] windowCoeffs - Pre-sized vector to receive window coefficients.
     */
    template <typename FloatType> void operator()(std::vector<FloatType>& windowCoeffs) const
    {
        WindowGenerator(windowCoeffs,
                        std::bind(&HannGenerator::EvaluateCoefficient<FloatType>,
                                  std::placeholders::_1,
                                  std::placeholders::_2));
    }

private:
    /*!
     * \brief Evaluate nth window coefficient.
     * \param[in] n - The current window coefficient index as a floating point type T.
     * \param[in] sizeMinusOne - The window length minus 1 as a floating point type T.
     * \return The nth window coefficient as a floating type T.
     */
    template <typename FloatType>
    static FloatType EvaluateCoefficient(FloatType n, FloatType sizeMinusOne)
    {
        static const auto TWO_PI = TwoPi<FloatType>();
        auto              twoPiN = TWO_PI * n;
        return static_cast<FloatType>(0.5) *
               (static_cast<FloatType>(1) - std::cos(twoPiN / sizeMinusOne));
    }
};

/*! \brief Hamming window generator. */
class HammingGenerator final
{
public:
    /*!
     * \brief Generate a vector of window coefficients.
     * \param[in,out] windowCoeffs - Pre-sized vector to receive window coefficients.
     */
    template <typename FloatType> void operator()(std::vector<FloatType>& windowCoeffs) const
    {
        WindowGenerator(windowCoeffs,
                        std::bind(&HammingGenerator::EvaluateCoefficient<FloatType>,
                                  std::placeholders::_1,
                                  std::placeholders::_2));
    }

private:
    /*!
     * \brief Evaluate nth window coefficient.
     * \param[in] n - The current window coefficient index as a floating point type T.
     * \param[in] sizeMinusOne - The window length minus 1 as a floating point type T.
     * \return The nth window coefficient as a floating type T.
     */
    template <typename FloatType>
    static FloatType EvaluateCoefficient(FloatType n, FloatType sizeMinusOne)
    {
        static const auto a0     = static_cast<FloatType>(0.53836);
        static const auto a1     = static_cast<FloatType>(0.46164);
        static const auto TWO_PI = TwoPi<FloatType>();
        auto              twoPiN = TWO_PI * n;
        return a0 - (a1 * std::cos(twoPiN / sizeMinusOne));
    }
};

/*! \brief Rectangle window generator. */
struct RectangleGenerator final
{
    /*!
     * \brief Generate a vector of window coefficients.
     * \param[in,out] windowCoeffs - Pre-sized vector to receive window coefficients.
     */
    template <typename FloatType> void operator()(std::vector<FloatType>& windowCoeffs) const
    {
        auto size = windowCoeffs.size();
        DSP_ASSERT_THROW(size > 1, "invalid window size");
        std::fill(windowCoeffs.begin(), windowCoeffs.end(), static_cast<FloatType>(1));
    }
};

/*! \brief Bartlett window generator. */
class BartlettGenerator final
{
public:
    /*!
     * \brief Generate a vector of window coefficients.
     * \param[in,out] windowCoeffs - Pre-sized vector to receive window coefficients.
     */
    template <typename FloatType> void operator()(std::vector<FloatType>& windowCoeffs) const
    {
        WindowGenerator(windowCoeffs,
                        std::bind(&BartlettGenerator::EvaluateCoefficient<FloatType>,
                                  std::placeholders::_1,
                                  std::placeholders::_2));
    }

private:
/*!
     * \brief Evaluate nth window coefficient.
     * \param[in] n - The current window coefficient index as a floating point type T.
     * \param[in] sizeMinusOne - The window length minus 1 as a floating point type T.
     * \return The nth window coefficient as a floating type T.
     */
    template <typename FloatType>
    static FloatType EvaluateCoefficient(FloatType n, FloatType sizeMinusOne)
    {
        auto commonTerm = sizeMinusOne / 2;
        auto numerator  = n - commonTerm;
        return static_cast<FloatType>(1) - std::abs(numerator / commonTerm);
    }
};

/*! \brief Exact Blackman window generator. */
class ExactBlackmanGenerator final
{
public:
    /*!
     * \brief Generate a vector of window coefficients.
     * \param[in,out] windowCoeffs - Pre-sized vector to receive window coefficients.
     */
    template <typename FloatType> void operator()(std::vector<FloatType>& windowCoeffs) const
    {
        WindowGenerator(windowCoeffs,
                        std::bind(&ExactBlackmanGenerator::EvaluateCoefficient<FloatType>,
                                  std::placeholders::_1,
                                  std::placeholders::_2));
    }

private:
    /*!
     * \brief Evaluate nth window coefficient.
     * \param[in] n - The current window coefficient index as a floating point type T.
     * \param[in] sizeMinusOne - The window length minus 1 as a floating point type T.
     * \return The nth window coefficient as a floating type T.
     */
    template <typename FloatType>
    static FloatType EvaluateCoefficient(FloatType n, FloatType sizeMinusOne)
    {
        static const auto a0      = static_cast<FloatType>(7938. / 18608.);
        static const auto a1      = static_cast<FloatType>(9240. / 18608.);
        static const auto a2      = static_cast<FloatType>(1430. / 18608.);
        static const auto TWO_PI  = TwoPi<FloatType>();
        static const auto FOUR_PI = static_cast<FloatType>(2) * TwoPi<FloatType>();
        auto              twoPiN  = TWO_PI * n;
        auto              fourPiN = FOUR_PI * n;
        return a0 - (a1 * std::cos(twoPiN / sizeMinusOne)) +
               (a2 * std::cos(fourPiN / sizeMinusOne));
    }
};

/*! \brief Blackman window generator. */
class BlackmanGenerator final
{
public:
    /*!
     * \brief Generate a vector of window coefficients.
     * \param[in,out] windowCoeffs - Pre-sized vector to receive window coefficients.
     */
    template <typename FloatType> void operator()(std::vector<FloatType>& windowCoeffs) const
    {
        WindowGenerator(windowCoeffs,
                        std::bind(&BlackmanGenerator::EvaluateCoefficient<FloatType>,
                                  std::placeholders::_1,
                                  std::placeholders::_2));
    }

private:
    /*!
     * \brief Evaluate nth window coefficient.
     * \param[in] n - The current window coefficient index as a floating point type T.
     * \param[in] sizeMinusOne - The window length minus 1 as a floating point type T.
     * \return The nth window coefficient as a floating type T.
     */
    template <typename FloatType>
    static FloatType EvaluateCoefficient(FloatType n, FloatType sizeMinusOne)
    {
        static const auto a0      = static_cast<FloatType>(0.42);
        static const auto a1      = static_cast<FloatType>(0.5);
        static const auto a2      = static_cast<FloatType>(0.08);
        static const auto TWO_PI  = TwoPi<FloatType>();
        static const auto FOUR_PI = static_cast<FloatType>(2) * TwoPi<FloatType>();
        auto              twoPiN  = TWO_PI * n;
        auto              fourPiN = FOUR_PI * n;
        return a0 - (a1 * std::cos(twoPiN / sizeMinusOne)) +
               (a2 * std::cos(fourPiN / sizeMinusOne));
    }
};

/*! \brief Kaiser window generator. */
class KaiserGenerator final
{
public:
    /*!
     * \brief Generate Kaiser window coefficients.
     * \param[in] beta - Controls side-lobe roll-off, where beta == Pi*alpha.
     */
    explicit KaiserGenerator(double beta)
        : m_beta(beta)
    {
        DSP_ASSERT_THROW(m_beta > 0, "beta <= 0");
    }
    /*!
     * \brief Generate a vector of window coefficients.
     * \param[in,out] windowCoeffs - Pre-sized vector to receive window coefficients.
     */
    template <typename FloatType> void operator()(std::vector<FloatType>& windowCoeffs) const
    {
        WindowGenerator(windowCoeffs,
                        std::bind(&KaiserGenerator::EvaluateCoefficient<FloatType>,
                                  this,
                                  std::placeholders::_1,
                                  std::placeholders::_2));
    }

private:
    /*!
     * \brief Evaluate nth window coefficient.
     * \param[in] n - The current window coefficient index as a floating point type T.
     * \param[in] sizeMinusOne - The window length minus 1 as a floating point type T.
     * \return The nth window coefficient as a floating type T.
     */
    template <typename FloatType>
    FloatType EvaluateCoefficient(FloatType n, FloatType sizeMinusOne) const
    {
        auto term = ((static_cast<FloatType>(2) * n) / sizeMinusOne) - static_cast<FloatType>(1);
        auto arg = static_cast<FloatType>(m_beta) * sqrt(static_cast<FloatType>(1) - (term * term));
        return Bessel(arg) / Bessel(static_cast<FloatType>(m_beta));
    }

private:
    /*! \brief beta controls side-lobe roll-off, where beta == Pi*alpha. */
    double m_beta{1.};
};

/*! \brief Lanczos (sinc) window generator. */
class LanczosGenerator final
{
public:
    /*!
     * \brief Generate a vector of window coefficients.
     * \param[in,out] windowCoeffs - Pre-sized vector to receive window coefficients.
     */
    template <typename FloatType> void operator()(std::vector<FloatType>& windowCoeffs) const
    {
        WindowGenerator(windowCoeffs,
                        std::bind(&LanczosGenerator::EvaluateCoefficient<FloatType>,
                                  std::placeholders::_1,
                                  std::placeholders::_2));
    }

private:
    /*!
     * \brief Evaluate nth window coefficient.
     * \param[in] n - The current window coefficient index as a floating point type T.
     * \param[in] sizeMinusOne - The window length minus 1 as a floating point type T.
     * \return The nth window coefficient as a floating type T.
     */
    template <typename FloatType>
    static FloatType EvaluateCoefficient(FloatType n, FloatType sizeMinusOne)
    {
        auto arg = ((static_cast<FloatType>(2) * n) / sizeMinusOne) - static_cast<FloatType>(1);
        return SincNorm(arg);
    }
};

/*!
 * \brief Top-level "Window Function" class use this class instead of the above helper classes.
 *
 * Template typename T defines the window coefficient data type.
 */
template <typename FloatType> class WindowFunction final
{
public:
    /*! \brief Default constructor. */
    WindowFunction() = default;
    /*! \brief Destructor. */
    ~WindowFunction() = default;
    /*! \brief Default copy constructor. */
    WindowFunction(WindowFunction const&) = default;
    /*! \brief Default move constructor. */
    WindowFunction(WindowFunction&&) = default;
    /*! \brief Default copy assignment operator. */
    WindowFunction& operator=(WindowFunction const&) = default;
    /*! \brief Default move assignment operator. */
    WindowFunction& operator=(WindowFunction&&) = default;

    /*!
     * \brief Initialisation constructor.
     * \param[in] generator - Window function generator.
     * \param[in] size - Size required for the window.
     * \param[in] ignoreLastValue - If size is odd and window is to be applied to data for FFT
     * processing set this to true, else set to false.
     */
    template <typename Generator>
    WindowFunction(Generator const& generator, size_t size, bool ignoreLastValue)
        : m_windowCoefficients(size, static_cast<FloatType>(1))
        , m_ignoreLastValue(ignoreLastValue && (size % 2 == 1))
        , m_effectiveSize(ignoreLastValue ? m_windowCoefficients.size() - 1
                                          : m_windowCoefficients.size())
    {
        static_assert(std::is_floating_point<FloatType>::value, "invalid floating point type");
        generator(m_windowCoefficients);
        ComputeGains();
    }

    /*!
     * \brief Initialisation method.
     * \param[in] generator - Window function generator.
     * \param[in] size - Size required for the window.
     * \param[in] ignoreLastValue - If size is odd and window is to be applied to data for FFT
     * processing set this to true, else set to false.
     */
    template <typename Generator>
    void Initialise(Generator const& generator, size_t size, bool ignoreLastValue)
    {
        *this = std::move(WindowFunction(generator, size, ignoreLastValue));
    }

    /*!
     * \brief Get the coherent gain of the window coefficients.
     * \return The value as a floating point type T.
     */
    FloatType CoherentGain() const
    {
        return m_coherentGain;
    }

    /*!
     * \brief Get the power gain of the window coefficients.
     * \return The value as a floating point type T.
     */
    FloatType PowerGain() const
    {
        return m_powerGain;
    }

    /*!
     * \brief Get the combined gain of the window coefficients.
     * \return The value as a floating point type T.
     */
    FloatType CombinedGain() const
    {
        return m_coherentGain * m_powerGain;
    }

    /*!
     * \brief Apply gain correction to data range.
     * \param[in] dataFirst - Iterator to the first data sample to scale by the window.
     * \param[in] dataLast - Iterator to the one past the final data sample to scale by the window.
     * \param[in,out] resultFirst - Iterator the start of the output container to receive the scaled
     * data.
     * \param[in] gain - Gain to correct data for.
     */
    template <typename InIter, typename OutIter>
    static void ApplyGainCorrection(InIter dataFirst, InIter dataLast, OutIter resultFirst,
                                    FloatType gain)
    {
        for (auto itr = dataFirst; itr != dataLast;
             std::advance(itr, 1), std::advance(resultFirst, 1))
        {
            *resultFirst = *itr / gain;
        }
    }

    /*!
     * \brief Get the effective noise bandwidth of the window coefficients.
     * \return The value as a floating point type T.
     */
    FloatType EffectiveNoiseBandwidth() const
    {
        return m_enbw;
    }

    /*!
     * \brief Get the actual length the window.
     * \return The number of window coefficients.
     */
    size_t ActualSize() const
    {
        return m_windowCoefficients.size();
    }

    /*!
     * \brief Get the effective length the window.
     * \return The number of window coefficients to be used.
     *
     * When the window has sodd length and is to be used for FFT processing
     * this will be 1 less than the actual size.
     */
    size_t EffectiveSize() const
    {
        return m_effectiveSize;
    }

    /*!
     * \brief Get a copy of the window coefficients.
     * \return The number of window coefficients.
     */
    std::vector<FloatType> Coefficients() const
    {
        if (m_windowCoefficients.size() == m_effectiveSize)
        {
            return m_windowCoefficients;
        }
        else
        {
            return std::vector<FloatType>(
                m_windowCoefficients.begin(),
                std::next(m_windowCoefficients.begin(), static_cast<int>(m_effectiveSize)));
        }
    }

    /*!
     * \brief Apply the window coefficients to a block of data.
     * \param[in] dataFirst - Iterator to the first data sample to scale by the window.
     * \param[in] dataLast - Iterator to the one past the final data sample to scale by the window.
     * \param[in,out] resultFirst - Iterator the start of the output container to receive the scaled
     * data.
     *
     * This function can be used safely for in-place trasnformations.
     */
    template <typename InIter, typename OutIter>
    void operator()(InIter dataFirst, InIter dataLast, OutIter resultFirst) const
    {
        auto inputLength = std::distance(dataFirst, dataLast);
        DSP_ASSERT_THROW(inputLength == static_cast<decltype(inputLength)>(m_effectiveSize),
                         "invalid data size");

        // Generic lambda function to allow multiplying mixed types.
        // This is because we may want to window complex data not just
        // real valued data.
        auto multiply = [](auto const& x, auto const& y) { return x * y; };

        // Transform the data by applying the window coefficients using
        // piecewise multiplication.
        std::transform(dataFirst, dataLast, m_windowCoefficients.begin(), resultFirst, multiply);
    }

private:
    /*! \brief Compute the window gains. */
    void ComputeGains()
    {
        auto size = EffectiveSize();

        for (size_t i = 0; i < size; ++i)
        {
            m_coherentGain += m_windowCoefficients[i];
            m_enbw += m_windowCoefficients[i] * m_windowCoefficients[i];
        }

        auto enbwDivisor = m_coherentGain * m_coherentGain;

        if (std::abs(enbwDivisor) > static_cast<FloatType>(1.e-9))
        {
            m_enbw = (static_cast<FloatType>(size) * m_enbw) / enbwDivisor;
        }

        m_coherentGain /= static_cast<FloatType>(size);
        m_powerGain = m_coherentGain * m_coherentGain * m_enbw;
    }

private:
    /*! \brief Vector of window coefficients. */
    std::vector<FloatType> m_windowCoefficients{};
    /*! \brief Flag to define if we need to ignore the last window coefficient. */
    bool m_ignoreLastValue{false};
    /*! \brief The effective size of the window. */
    size_t m_effectiveSize{0};
    /*! \brief The coeherent gain of the window. */
    FloatType m_coherentGain{0.};
    /*! \brief The power gain of the window. */
    FloatType m_powerGain{0.};
    /*! \brief The effective noise bandwidth of the window. */
    FloatType m_enbw{0.};
};

/*! \brief Convenience typedef to WindowFunction<float>. */
using window_fn_f = WindowFunction<float>;
/*! \brief Convenience typedef to WindowFunction<double>. */
using window_fn_d = WindowFunction<double>;
/*! \brief Convenience typedef to WindowFunction<long double>. */
using window_fn_ld = WindowFunction<long double>;


/*! \brief Class to perform FFTs on complex data */
template <typename FloatType> class ComplexFFT final
{
public:
    /*! \brief Typedef to complex floating point value */
    using complex_type = std::complex<FloatType>;
    /*! \brief Typedef to complex floating point vector */
    using complex_vector = std::vector<complex_type>;
    /*! \brief Typedef to real floating point vector */
    using real_vector = std::vector<FloatType>;

public:
    /*! \brief Default constructor. */
    ComplexFFT()
    {
        static_assert(std::is_floating_point<FloatType>::value,
                      "FloatType must be either float, double or long double");
    }

    /*!
     * \brief Perform a forward FFT on the input range
     * \param[in] first - iterator to start of data samples.
     * \param[in] last - iterator to one past the end of the data samples.
     *
     * Input data range must be implicitly convertible to std::complex<FloatType>.
     */
    template <typename Iter> static complex_vector Forward(Iter first, Iter last)
    {
        auto N = static_cast<size_t>(std::distance(first, last));

        DSP_ASSERT_THROW(IsPowerOf2(N), "FFT size not a power of 2");

        complex_vector data(first, last);

        // Forward FFT of data.
        CooleyTukeyFFT(data);

        return data;
    }

    /*!
     * \brief Perform a forward FFT on complex data vector in-place.
     * \param[in] data - vector of complex data.
     */
    static void Forward(complex_vector& data)
    {
        DSP_ASSERT_THROW(IsPowerOf2(data.size()), "FFT size not a power of 2");

        // Forward FFT of data.
        CooleyTukeyFFT(data);
    }

    /*!
     * \brief Normalise complex FFT spectrum in-place.
     * \param[in] cplxFft - vector of complex FFT data.
     *
     * Normalise a complex FFT result by dividing
     * real and complex parts by the FFT size.
     */
    static void Normalise(complex_vector& cplxFft)
    {
        auto N = static_cast<FloatType>(cplxFft.size());

        for (auto& z : cplxFft)
        {
            z /= N;
        }
    }

    /*!
     * \brief Denormalise complex FFT spectrum in-place.
     * \param[in] cplxFft - vector of normalised complex FFT data.
     *
     * Denormalise a normalised complex FFT result by multiplying
     * real and complex parts by the FFT size.
     */
    static void Denormalise(complex_vector& cplxFft)
    {
        auto N = static_cast<FloatType>(cplxFft.size());

        for (auto& z : cplxFft)
        {
            z *= N;
        }
    }

    /*!
     * \brief Perform an inverse of complex FFT data in-place.
     * \param[in] cplxFft - complex data vector containing denormalised complex FFT.
     */
    static void Inverse(complex_vector& cplxFft)
    {
        DSP_ASSERT_THROW(IsPowerOf2(cplxFft.size()), "FFT size not a power of 2");

        // Conjugation lambda designed for speed.
        auto conjugate = [](complex_vector& cplxVector) {
            static constexpr auto minusOne = static_cast<FloatType>(-1);

            for (auto& z : cplxVector)
            {
                z.imag(z.imag() * minusOne);
            }
        };

        // Conjugate the FFT BIN values.
        conjugate(cplxFft);

        // Forward FFT of conjugates.
        CooleyTukeyFFT(cplxFft);

        // Conjugate the values again.
        conjugate(cplxFft);

        // Scale the numbers
        Normalise(cplxFft);
    }

    /*!
     * \brief Convert complex FFT data to a magnitude only spectrum in-place.
     * \param[in] cplxFft - complex data vector containing normalised complex FFT.
     * \param[in] zeroUnused - zero unused BINs in complex vector.
     * \param[in] fullSpectrum - keep the full spectrum.
     *
     * After calling this only the real part of each complex value
     * is valid, the imaginary part will be 0. Also because the power
     * is spread over a positive and negative half spectra we multiply
     * the value in each BIN by 2 to get the correct amplitude. The
     * negative half of the FFT is not needed unless we want the full
     * spectrum so if we only want the half spectrum we use so only the
     * first N/2 values.
     */
    static void ToMagnitude(complex_vector& cplxFft, bool zeroUnused = false,
                            bool fullSpectrum = false)
    {
        static constexpr auto two      = static_cast<FloatType>(2);
        static constexpr auto zero     = static_cast<FloatType>(0);
        auto                  halfSize = fullSpectrum ? cplxFft.size() : cplxFft.size() / 2;
        auto                  endItr   = std::next(cplxFft.begin(), static_cast<int32_t>(halfSize));

        for (auto zItr = cplxFft.begin(); zItr != endItr; ++zItr)
        {
            if (zItr != cplxFft.begin())
            {
                // This BIN does not contain the DC so
                // we multiply this by scalar.
                *zItr *= two;
            }

            auto magnitude = std::abs(*zItr);
            zItr->real(magnitude);
            zItr->imag(zero);
        }

        if (zeroUnused)
        {
            for (auto zItr = endItr; zItr != cplxFft.end(); ++zItr)
            {
                *zItr = {zero, zero};
            }
        }
    }

    /*!
     * \brief Convert complex FFT data to a magnitude only spectrum.
     * \param[in] cplxFft - complex data vector containing normalised complex FFT.
     * \param[out] magFft - The magnitude only spectrum as a real valued vector.
     * \param[in] fullSpectrum - keep the full spectrum.
     */
    static void ToMagnitude(complex_vector const& cplxFft, real_vector& magFft,
                            bool fullSpectrum = false)
    {
        static constexpr auto two      = static_cast<FloatType>(2);
        auto                  halfSize = fullSpectrum ? cplxFft.size() : cplxFft.size() / 2;
        auto                  endItr   = std::next(cplxFft.begin(), static_cast<int32_t>(halfSize));
        magFft.resize(halfSize);
        size_t index = 0;

        for (auto zItr = cplxFft.begin(); zItr != endItr; ++zItr, ++index)
        {
            if (zItr != cplxFft.begin())
            {
                // This BIN does not contain the DC so
                // we multiply this by scalar.
                magFft[index] = std::abs(*zItr * two);
            }
            else
            {
                magFft[index] = std::abs(*zItr);
            }
        }
    }

    /*!
     * \brief Convert complex FFT data to power spectrum in-place.
     * \param[in] cplxFft - complex data vector containing normalised complex FFT.
     * \param[in] zeroUnused - zero unused BINs in complex vector.
     * \param[in] fullSpectrum - keep the full spectrum.
     *
     * The power spectrum is computed as re[n]^2 + im[n]^2. Only the real
     * part of each complex value is valid, the imaginary part will be 0.
     * The negative half of the FFT is not needed unless we want the full
     * spectrum so if we only want the half spectrum we use so only the
     * first N/2 values.
     */
    static void ToPower(complex_vector& cplxFft, bool zeroUnused = false, bool fullSpectrum = false)
    {
        static constexpr auto zero     = static_cast<FloatType>(0);
        auto                  halfSize = fullSpectrum ? cplxFft.size() : cplxFft.size() / 2;
        auto                  endItr   = std::next(cplxFft.begin(), static_cast<int32_t>(halfSize));

        for (auto zItr = cplxFft.begin(); zItr != endItr; ++zItr)
        {
            auto power = std::norm(*zItr);
            zItr->real(power);
            zItr->imag(zero);
        }

        if (zeroUnused)
        {
            for (auto zItr = endItr; zItr != cplxFft.end(); ++zItr)
            {
                *zItr = {zero, zero};
            }
        }
    }

    /*!
     * \brief Convert complex FFT data to power spectrum.
     * \param[in] cplxFft - complex data vector containing normalised complex FFT.
     * \param[out] powerSpectrum - The power spectrum as a real valued vector.
     * \param[in] fullSpectrum - keep the full spectrum.
     */
    static void ToPower(complex_vector const& cplxFft, real_vector& powerSpectrum,
                        bool fullSpectrum = false)
    {
        auto halfSize = fullSpectrum ? cplxFft.size() : cplxFft.size() / 2;
        auto endItr   = std::next(cplxFft.begin(), static_cast<int32_t>(halfSize));
        powerSpectrum.resize(halfSize);
        size_t index = 0;

        for (auto zItr = cplxFft.begin(); zItr != endItr; ++zItr, ++index)
        {
            powerSpectrum[index] = std::norm(*zItr);
        }
    }

    /*!
     * \brief Convert power spectrum to PSD (power spectral density) in-place.
     * \param[in] powerSpectrum - complex data vector containing the power spectrum.
     * \param[in] binWidthHz - the bin width in Hz of the spectral data.
     * \param[in] zeroUnused - zero unused BINs in complex vector.
     * \param[in] fullSpectrum - keep the full spectrum.
     *
     * The real valued power spectrum stored in a complex vector
     * is converted to PSD values by dividing by BIN width. After
     * calling this function the half powerSpectrum contains the PSD in
     * its real components. The negative half of the FFT is not needed
     * unless we want the full spectrum so if we only want the half spectrum
     * we use so only the first N/2 values.
     */
    static void ToPsd(complex_vector& powerSpectrum, FloatType binWidthHz, bool zeroUnused = false,
                      bool fullSpectrum = false)
    {
        static constexpr auto zero = static_cast<FloatType>(0);
        auto halfSize              = fullSpectrum ? powerSpectrum.size() : powerSpectrum.size() / 2;
        auto endItr = std::next(powerSpectrum.begin(), static_cast<int32_t>(halfSize));

        for (auto psIter = powerSpectrum.begin(); psIter != endItr; std::advance(psIter, 1))
        {
            auto psd = psIter->real() / binWidthHz;
            psIter->real(psd);
        }

        if (zeroUnused)
        {
            for (auto psIter = endItr; psIter != powerSpectrum.end(); ++psIter)
            {
                *psIter = {zero, zero};
            }
        }
    }

    /*!
     * \brief Convert power spectrum to PSD (power spectral density) in-place.
     * \param[in] powerSpectrum - real valued data vector containing the power spectrum.
     * \param[in] binWidthHz - the bin width in Hz of the spectral data.
     *
     * After this function is called the powerSpectrum will contain the PSD values.
     */
    static void ToPsd(real_vector& powerSpectrum, FloatType binWidthHz)
    {
        static constexpr auto one = static_cast<FloatType>(1);

        for (auto& v : powerSpectrum)
        {
            v /= binWidthHz;
        }
    }

    /*!
     * \brief Convert power spectrum to PSD (power spectral density).
     * \param[in] powerSpectrum - complex data vector containing the power spectrum.
     * \param[in] binWidthHz - the bin width in Hz of the spectral data.
     * \param[out] psd - the real valued vector containing the PSD values.
     * \param[in] fullSpectrum - keep the full spectrum.
     *
     * The real valued power spectrum stored in a complex vector is converted to
     * PSD values by dividing by BIN width. The negative half of the FFT is not
     * needed  unless we want the full spectrum so if we only want the half spectrum
     * we use so only the first N/2 values.
     */
    static void ToPsd(complex_vector const& powerSpectrum, FloatType binWidthHz, real_vector& psd,
                      bool fullSpectrum = false)
    {
        auto halfSize = fullSpectrum ? powerSpectrum.size() : powerSpectrum.size() / 2;
        auto endItr   = std::next(powerSpectrum.begin(), static_cast<int32_t>(halfSize));
        psd.resize(halfSize);
        size_t index = 0;

        for (auto psIter = powerSpectrum.begin(); psIter != endItr;
             std::advance(psIter, 1), ++index)
        {
            psd[index] = psIter->real() / binWidthHz;
        }
    }

    /*!
     * \brief Convert power spectrum to PSD (power spectral density).
     * \param[in] powerSpectrum - real valued data vector containing te power spectrum.
     * \param[in] binWidthHz - the bin width in Hz of the spectral data.
     * \param[out] psd - the real valued vector containing the PSD values.
     *
     * The real valued power spectrum stored in a real valued vector
     * is converted to PSD values by dividing by BIN width.
     */
    static void ToPsd(const real_vector& powerSpectrum, FloatType binWidthHz, real_vector& psd)
    {
        psd.resize(powerSpectrum.size());
        size_t index = 0;

        for (auto psIter = powerSpectrum.begin(); psIter != powerSpectrum.end();
             std::advance(psIter, 1), ++index)
        {
            psd[index] = *psIter / binWidthHz;
        }
    }

    /*!
     * \brief Convert power spectrum to 3 BIN summed magnitude spectrum in-place.
     * \param[in] powerSpectrum - complex data vector containing the power spectrum.
     * \param[in] zeroUnused - zero unused BINs in complex vector.
     * \param[in] fullSpectrum - keep the full spectrum.
     *
     * The real valued power spectrum stored in a complex vector
     * is converted to 3-BIN summed magnitude spectrum. After
     * calling this function the powerSpectrum contains the result in
     * its real components. The negative half of the FFT is not needed
     * unless we want the full spectrum so if we only want the half spectrum
     * we use so only the first N/2 values.
     */
    static void To3BinSum(complex_vector& powerSpectrum, bool zeroUnused = false,
                          bool fullSpectrum = false)
    {
        static constexpr auto       one     = static_cast<FloatType>(1);
        static constexpr auto       zero    = static_cast<FloatType>(0);
        static const auto           sqrt2   = dsp::SqrtTwo<FloatType>();
        static const complex_vector convVec = {{one, zero}, {one, zero}, {one, zero}};
        auto           halfSize = fullSpectrum ? powerSpectrum.size() : powerSpectrum.size() / 2;
        auto           endItr   = std::next(powerSpectrum.begin(), static_cast<int32_t>(halfSize));
        complex_vector convRes(halfSize + convVec.size() - 1);

        Convolve(powerSpectrum.begin(), endItr, convVec.begin(), convVec.end(), convRes.begin());

        auto resItr = std::next(convRes.begin());
        auto resEnd = std::next(resItr, static_cast<int32_t>(halfSize));
        auto outItr = powerSpectrum.begin();

        while (resItr != resEnd)
        {
            // Convert from RMS power to peak magnitude.
            outItr->real(std::sqrt(resItr->real()) * sqrt2);
            std::advance(outItr, 1);
            std::advance(resItr, 1);
        }

        if (zeroUnused)
        {
            for (auto psIter = endItr; psIter != powerSpectrum.end(); ++psIter)
            {
                *psIter = {zero, zero};
            }
        }
    }

    /*!
     * \brief Convert power spectrum to 3 BIN summed magnitude spectrum in-place.
     * \param[in] powerSpectrum - real valued data vector containing the power spectrum.
     *
     * After this function is called the powerSpectrum will contain the 3 BIN summed
     * magnitude values.
     */
    static void To3BinSum(real_vector& powerSpectrum)
    {
        static constexpr auto    one     = static_cast<FloatType>(1);
        static const real_vector convVec = {one, one, one};
        auto                     sqrt2   = dsp::SqrtTwo<FloatType>();
        auto                     size    = powerSpectrum.size();
        real_vector              convRes(size + convVec.size() - 1);

        Convolve(powerSpectrum.begin(),
                 powerSpectrum.end(),
                 convVec.begin(),
                 convVec.end(),
                 convRes.begin());

        auto resBegin = std::next(convRes.begin());
        auto resEnd   = std::next(resBegin, static_cast<int32_t>(size));

        // Convert from RMS power to peak magnitude.
        auto transformer = [sqrt2](auto x) { return std::sqrt(x) * sqrt2; };

        std::transform(resBegin, resEnd, powerSpectrum.begin(), transformer);
    }

    /*!
     * \brief Convert power spectrum to 3 BIN summed magnitude spectrum.
     * \param[in] powerSpectrum - complex data vector containing the power spectrum.
     * \param[out] magFft - the real valued vector containing the 3 BIN summed values.
     * \param[in] fullSpectrum - keep the full spectrum.
     *
     * The real valued power spectrum stored in a complex vector is converted to
     * 3 BIN summed magnitude values. The negative half of the FFT is not needed
     * unless we want the full spectrum so if we only want the half spectrum
     * we use so only the first N/2 values.
     */
    static void To3BinSum(complex_vector const& powerSpectrum, real_vector& magFft,
                          bool fullSpectrum = false)
    {
        static constexpr auto       one     = static_cast<FloatType>(1);
        static constexpr auto       zero    = static_cast<FloatType>(0);
        static const complex_vector convVec = {{one, zero}, {one, zero}, {one, zero}};
        auto                        sqrt2   = dsp::SqrtTwo<FloatType>();
        auto           halfSize = fullSpectrum ? powerSpectrum.size() : powerSpectrum.size() / 2;
        auto           endItr   = std::next(powerSpectrum.begin(), static_cast<int32_t>(halfSize));
        complex_vector convRes(halfSize + convVec.size() - 1);
        magFft.resize(halfSize);

        Convolve(powerSpectrum.begin(), endItr, convVec.begin(), convVec.end(), convRes.begin());

        auto resBegin = std::next(convRes.begin());
        auto resEnd   = std::next(resBegin, halfSize);

        auto transformer = [sqrt2](auto const& z) { return std::sqrt(z.real()) * sqrt2; };

        std::transform(resBegin, resEnd, magFft.begin(), transformer);
    }

    /*!
     * \brief Convert power spectrum to 3 BIN summed magnitude spectrum.
     * \param[in] powerSpectrum - real valued data vector containing the power spectrum.
     * \param[out] magFft - the real valued vector containing the 3 BIN summed values.
     *
     * The real valued power spectrum stored in a real valued vector
     * is converted to 3 BIN summed magnitude values.
     */
    static void To3BinSum(real_vector const& powerSpectrum, real_vector& magFft)
    {
        static constexpr auto    one     = static_cast<FloatType>(1);
        static const real_vector convVec = {one, one, one};
        auto                     sqrt2   = dsp::SqrtTwo<FloatType>();
        auto                     size    = powerSpectrum.size();
        magFft.resize(size + convVec.size() - 1);

        Convolve(powerSpectrum.begin(),
                 powerSpectrum.end(),
                 convVec.begin(),
                 convVec.end(),
                 magFft.begin());

        std::rotate(magFft.begin(), std::next(magFft.begin()), magFft.end());
        magFft.resize(size);

        // Convert from RMS power to peak magnitude.
        auto transformer = [sqrt2](auto x) { return std::sqrt(x) * sqrt2; };

        std::transform(magFft.begin(), magFft.end(), magFft.begin(), transformer);
    }

private:
    /*!
     * \brief Perform Cooley-Tukey algorithm to compute the FFT spectrum in-place.
     * \param[in] data - complex vector of data to have FFT perfromed on it.
     *
     * After this function is the data vector contains the FFT spectrum.
     */
    static void CooleyTukeyFFT(complex_vector& data)
    {
        static constexpr auto one = static_cast<FloatType>(1);
        static const auto     pi  = Pi<FloatType>();

        // DFT.
        auto N      = static_cast<uint32_t>(data.size());
        auto fN     = static_cast<FloatType>(N);
        auto k      = N;
        auto thetaT = pi / fN;
        auto phiT   = complex_type(std::cos(thetaT), std::sin(thetaT));

        while (k > 1)
        {
            auto n = k;
            k >>= 1;
            phiT   = phiT * phiT;
            auto T = complex_type(one);

            for (uint32_t l = 0; l < k; ++l)
            {
                for (uint32_t a = l; a < N; a += n)
                {
                    uint32_t b = a + k;
                    auto     t = data[a] - data[b];
                    data[a] += data[b];
                    data[b] = t * T;
                }

                T *= phiT;
            }
        }

        // Decimate.
        auto m = static_cast<uint32_t>(std::log2(fN));

        for (uint32_t a = 0; a < N; ++a)
        {
            auto b = a;

            // Reverse bits.
            b = (((b & 0xaaaaaaaa) >> 1) | ((b & 0x55555555) << 1));
            b = (((b & 0xcccccccc) >> 2) | ((b & 0x33333333) << 2));
            b = (((b & 0xf0f0f0f0) >> 4) | ((b & 0x0f0f0f0f) << 4));
            b = (((b & 0xff00ff00) >> 8) | ((b & 0x00ff00ff) << 8));
            b = ((b >> 16) | (b << 16)) >> (32 - m);

            if (b > a)
            {
                std::swap(data[a], data[b]);
            }
        }
    }
};

/*! \brief Class to generate real-valued FFT spectrum using 3 BIN summing. */
template <typename FloatType> class ThreeBinSumFft final
{
    /*! \brief Typedef to window function. */
    using window_fn = WindowFunction<FloatType>;
    /*! \brief Typedef to complex floating point value */
    using complex_type = std::complex<FloatType>;
    /*! \brief Typedef to complex floating point vector */
    using complex_vector = std::vector<complex_type>;
    /*! \brief Typedef to real floating point vector */
    using real_vector = std::vector<FloatType>;
    /*! \brief Convenience typedef to ComplexFFT<FloatType>. */
    using complex_fft = ComplexFFT<FloatType>;

public:
    /*! \brief Default constructor. */
    ThreeBinSumFft() = default;
    /*! \brief Destructor. */
    ~ThreeBinSumFft() = default;
    /*! \brief Default copy constructor. */
    ThreeBinSumFft(ThreeBinSumFft const&) = default;
    /*! \brief Default move constructor. */
    ThreeBinSumFft(ThreeBinSumFft&&) = default;
    /*! \brief Default copy assignment operator. */
    ThreeBinSumFft& operator=(ThreeBinSumFft const&) = default;
    /*! \brief Default move assignment operator. */
    ThreeBinSumFft& operator=(ThreeBinSumFft&&) = default;

    /*!
     * \brief Initialisation constructor.
     * \param[in] generator - the window function generator functor.
     * \param[in] fftSize - the number of samples used by the FFT.
     */
    template <typename Generator>
    ThreeBinSumFft(Generator const& generator, size_t fftSize)
        : m_windowFunction(generator, fftSize + 1, true)
        , m_workspace(fftSize)
    {
        DSP_ASSERT_THROW(IsPowerOf2(fftSize), "FFT size not a power of 2");
    }

    /*!
     * \brief Initialiser method.
     * \param[in] generator - the window function generator functor.
     * \param[in] fftSize - the number of samples used by the FFT.
     */
    template <typename Generator> void Initialise(Generator const& generator, size_t fftSize)
    {
        *this = std::move(ThreeBinSumFft(generator, fftSize));
    }

    /*!
     * \brief Perform the FFT on the real-valued signal data samples.
     * \param[in] signalFirst - iterator to first sample of signal data.
     * \param[in] signalLast - iterator to one past the last sample of signal data.
     * \param[out] realSpectra - real-valued vector to hold result spectrum.
     * \param[in] fullSpectrum - keep the full spectrum.
     * \param[out] phases - (optional) bin by bin spectrum phases.
     */
    template <typename Iter>
    void operator()(Iter signalFirst, Iter signalLast, real_vector& realSpectra,
                    bool fullSpectrum = false, real_vector* phases = nullptr)
    {
        auto signalLength = std::distance(signalFirst, signalLast);
        DSP_ASSERT_THROW(signalLength == static_cast<decltype(signalLength)>(m_workspace.size()),
                         "signal length is incorrect");

        // Window the data.
        m_windowFunction(signalFirst, signalLast, realSpectra.begin());

        // Copy data into workspace.
        auto convertToComplex = [](auto x) { return complex_type{x, 0}; };
        std::transform(
            realSpectra.begin(), realSpectra.end(), m_workspace.begin(), convertToComplex);

        // Perform the forward FFT on the windowed data.
        complex_fft::Forward(m_workspace);

        // Typically you would normalise the FFT spectrum at this stage using:
        // complex_fft::Normalise(m_workspace);
        // To minimise the amount of looping over the spectrum we will wrap up
        // normalisation and window gain correction into one step.

        // Convert FFT spectrum to power spectrum, converting to real valued vector
        // as this speeds up amount of processing that needs doing below.
        complex_fft::ToPower(m_workspace, realSpectra, fullSpectrum);

        // Compute phases if required.
        if (nullptr != phases)
        {
            phases->resize(realSpectra.size());

            for (size_t i = 0; i < phases->size(); ++i)
            {
                (*phases)[i] = std::arg(m_workspace[i]);
            }
        }

        // Apply window gain correction and normalisation to power spectrum.
        auto normScalar = static_cast<FloatType>(m_workspace.size() * m_workspace.size());
        window_fn::ApplyGainCorrection(realSpectra.begin(),
                                       realSpectra.end(),
                                       realSpectra.begin(),
                                       m_windowFunction.CombinedGain() * normScalar);

        // Perform 3-BIN summing of corrected power spectrum.
        complex_fft::To3BinSum(realSpectra);
    }

    /*!
     * \brief Perform the FFT on the complex signal data samples.
     * \param[in] signal - vector of complex signal data.
     * \param[out] realSpectra - real-valued vector to hold result spectrum.
     * \param[in] fullSpectrum - keep the full spectrum.
     * \param[out] phases - (optional) bin by bin spectrum phases.
     */
    void operator()(complex_vector const& signal, real_vector& realSpectra,
                    bool fullSpectrum = false, real_vector* phases = nullptr)
    {
        DSP_ASSERT_THROW(signal.size() == m_workspace.size(), "signal length is incorrect");

        // Window the data.
        m_windowFunction(signal.begin(), signal.end(), m_workspace.begin());

        // Perform the forward FFT on the windowed data.
        complex_fft::Forward(m_workspace);

        // Typically you would normalise the FFT spectrum at this stage using:
        // complex_fft::Normalise(m_workspace);
        // To minimise the amount of looping over the spectrum we will wrap up
        // normalisation and window gain correction into one step.

        // Convert FFT spectrum to power spectrum, converting to real valued vector
        // as this speeds up amount of processing that needs doing below.
        complex_fft::ToPower(m_workspace, realSpectra, fullSpectrum);

        // Compute phases if required.
        if (nullptr != phases)
        {
            phases->resize(realSpectra.size());

            for (size_t i = 0; i < phases->size(); ++i)
            {
                (*phases)[i] = std::arg(m_workspace[i]);
            }
        }

        // Apply window gain correction and normalisation to power spectrum.
        auto normScalar = static_cast<FloatType>(m_workspace.size() * m_workspace.size());
        window_fn::ApplyGainCorrection(realSpectra.begin(),
                                       realSpectra.end(),
                                       realSpectra.begin(),
                                       m_windowFunction.CombinedGain() * normScalar);

        // Perform 3-BIN summing of corrected power spectrum.
        complex_fft::To3BinSum(realSpectra);
    }

private:
    /*! \brief Window function. */
    window_fn m_windowFunction{};
    /*! \brief Complex data vector workspace. */
    complex_vector m_workspace{};
};

/*! \brief Class to generate real-valued (magnitude) FFT spectrum from real valued data. */
template <typename FloatType> class MagnitudeFft final
{
    /*! \brief Typedef to window function. */
    using window_fn = WindowFunction<FloatType>;
    /*! \brief Typedef to complex floating point value */
    using complex_type = std::complex<FloatType>;
    /*! \brief Typedef to complex floating point vector */
    using complex_vector = std::vector<complex_type>;
    /*! \brief Typedef to real floating point vector */
    using real_vector = std::vector<FloatType>;
    /*! \brief Convenience typedef to ComplexFFT<FloatType>. */
    using complex_fft = ComplexFFT<FloatType>;

public:
    /*! \brief Default constructor. */
    MagnitudeFft() = default;
    /*! \brief Destructor. */
    ~MagnitudeFft() = default;
    /*! \brief Default copy constructor. */
    MagnitudeFft(MagnitudeFft const&) = default;
    /*! \brief Default move constructor. */
    MagnitudeFft(MagnitudeFft&&) = default;
    /*! \brief Default copy assignment operator. */
    MagnitudeFft& operator=(MagnitudeFft const&) = default;
    /*! \brief Default move assignment operator. */
    MagnitudeFft& operator=(MagnitudeFft&&) = default;

    /*!
     * \brief Initialisation constructor.
     * \param[in] generator - the window function generator functor.
     * \param[in] fftSize - the number of samples used by the FFT.
     */
    template <typename Generator>
    MagnitudeFft(Generator const& generator, size_t fftSize)
        : m_workspace(fftSize)
        , m_windowFunction(generator, fftSize + 1, true)
    {
        DSP_ASSERT_THROW(IsPowerOf2(fftSize), "FFT size not a power of 2");
    }

    /*!
     * \brief Initialiser method.
     * \param[in] generator - the window function generator functor.
     * \param[in] fftSize - the number of samples used by the FFT.
     */
    template <typename Generator> void Initialise(Generator const& generator, size_t fftSize)
    {
        *this = std::move(MagnitudeFft(generator, fftSize));
    }

    /*!
     * \brief Perform the FFT on the real-valued signal data samples.
     * \param[in] signalFirst - iterator to first sample of signal data.
     * \param[in] signalLast - iterator to one past the last sample of signal data.
     * \param[out] realSpectra - real-valued vector to hold result spectrum.
     * \param[in] fullSpectrum - keep the full spectrum.
     * \param[out] phases - (optional) bin by bin spectrum phases.
     */
    template <typename Iter>
    void operator()(Iter signalFirst, Iter signalLast, real_vector& realSpectra,
                    bool fullSpectrum = false, real_vector* phases = nullptr)
    {
        auto signalLength = std::distance(signalFirst, signalLast);
        DSP_ASSERT_THROW(signalLength == static_cast<decltype(signalLength)>(m_workspace.size()),
                         "signal length is incorrect");

        // Window the data.
        m_windowFunction(signalFirst, signalLast, realSpectra.begin());

        // Copy data into workspace.
        auto convertToComplex = [](auto x) { return complex_type{x, 0}; };
        std::transform(
            realSpectra.begin(), realSpectra.end(), m_workspace.begin(), convertToComplex);

        // Perform the forward FFT on the windowed data.
        complex_fft::Forward(m_workspace);

        // Typically you would normalise the FFT spectrum at this stage using:
        // complex_fft::Normalise(m_workspace);
        // To minimise the amount of looping over the spectrum we will wrap up
        // normalisation and window gain correction into one step.

        // Convert FFT spectrum to power spectrum, converting to real valued vector
        // as this speeds up amount of processing that needs doing below.
        complex_fft::ToMagnitude(m_workspace, realSpectra, fullSpectrum);

        // Compute phases if required.
        if (nullptr != phases)
        {
            phases->resize(realSpectra.size());

            for (size_t i = 0; i < phases->size(); ++i)
            {
                (*phases)[i] = std::arg(m_workspace[i]);
            }
        }

        // Apply window gain correction and normalisation to power spectrum.
        auto normScalar = static_cast<FloatType>(m_workspace.size());
        window_fn::ApplyGainCorrection(realSpectra.begin(),
                                       realSpectra.end(),
                                       realSpectra.begin(),
                                       m_windowFunction.CoherentGain() * normScalar);
    }

    /*!
     * \brief Perform the FFT on the complex signal data samples.
     * \param[in] signal - vector of complex signal data.
     * \param[out] realSpectra - real-valued vector to hold result spectrum.
     * \param[in] fullSpectrum - keep the full spectrum.
     * \param[out] phases - (optional) bin by bin spectrum phases.
     */
    void operator()(complex_vector const& signal, real_vector& realSpectra,
                    bool fullSpectrum = false, real_vector* phases = nullptr)
    {
        DSP_ASSERT_THROW(signal.size() == m_workspace.size(), "signal length is incorrect");

        // Window the data.
        m_windowFunction(signal.begin(), signal.end(), m_workspace.begin());

        // Perform the forward FFT on the windowed data.
        complex_fft::Forward(m_workspace);

        // Typically you would normalise the FFT spectrum at this stage using:
        // complex_fft::Normalise(m_workspace);
        // To minimise the amount of looping over the spectrum we will wrap up
        // normalisation and window gain correction into one step.

        // Convert FFT spectrum to power spectrum, converting to real valued vector
        // as this speeds up amount of processing that needs doing below.
        complex_fft::ToMagnitude(m_workspace, realSpectra, fullSpectrum);

        // Compute phases if required.
        if (nullptr != phases)
        {
            phases->resize(realSpectra.size());

            for (size_t i = 0; i < phases->size(); ++i)
            {
                (*phases)[i] = std::arg(m_workspace[i]);
            }
        }

        // Apply window gain correction and normalisation to power spectrum.
        auto normScalar = static_cast<FloatType>(m_workspace.size());
        window_fn::ApplyGainCorrection(realSpectra.begin(),
                                       realSpectra.end(),
                                       realSpectra.begin(),
                                       m_windowFunction.CoherentGain() * normScalar);
    }

private:
    /*! \brief Window function. */
    window_fn m_windowFunction{};
    /*! \brief Complex data vector workspace. */
    complex_vector m_workspace{};
};

/*! \brief Class to perform convolution of 2 ranges using FFTs. */
template <typename FloatType> class FftConvolve final
{
    /*! \brief Typedef to complex floating point value */
    using complex_type = std::complex<FloatType>;
    /*! \brief Typedef to complex floating point vector */
    using complex_vector = std::vector<complex_type>;
    /*! \brief Typedef to real floating point vector */
    using real_vector = std::vector<FloatType>;
    /*! \brief Convenience typedef to ComplexFFT<FloatType>. */
    using complex_fft = ComplexFFT<FloatType>;

public:
    /*! \brief Default constructor. */
    FftConvolve() = default;
    /*! \brief Destructor. */
    ~FftConvolve() = default;
    /*! \brief Default copy constructor. */
    FftConvolve(FftConvolve const&) = default;
    /*! \brief Default move constructor. */
    FftConvolve(FftConvolve&&) = default;
    /*! \brief Default copy assignment operator. */
    FftConvolve& operator=(FftConvolve const&) = default;
    /*! \brief Default move assignment operator. */
    FftConvolve& operator=(FftConvolve&&) = default;

    /*!
     * \brief Initialisation constructor.
     * \param[in] signalLength - data length for range of signal's samples.
     * \param[in] kernelLength - data length for filter kernel.
     */
    FftConvolve(size_t signalLength, size_t kernelLength)
    {
        DSP_ASSERT_THROW(signalLength > 0, "signalLength <= 0");
        DSP_ASSERT_THROW(kernelLength > 0, "kernelLength <= 0");
        m_discreteConvolutionLength = signalLength + kernelLength - 1;
        auto powerOf2 =
            static_cast<size_t>(std::floor(std::log2(m_discreteConvolutionLength)) + 0.5);
        size_t workspaceLength = static_cast<size_t>(1) << (powerOf2 + 1);
        m_workspace1.resize(workspaceLength, 0);
        m_workspace2.resize(workspaceLength, 0);
    }

    /*!
     * \brief Initialiser method.
     * \param[in] signalLength - data length for range of signal's samples.
     * \param[in] kernelLength - data length for filter kernel.
     */
    void Initialise(size_t signalLength, size_t kernelLength)
    {
        *this = std::move(FftConvolve(signalLength, kernelLength));
    }

    /*!
     * \brief Perform convolution on 2 data ranges.
     * \param[in] signalFirst - Start of the signal sample range, length N.
     * \param[in] signalLast - End of the signal sample range, length N.
     * \param[in] kernelFirst - Start of the filter kernel range, length M.
     * \param[in] kernelLast - End of the filter kernel range, length M.
     * \param[out] result - Start of the result range must be length N + M - 1.
     *
     * Performs convolution using FFTs.
     *
     * 1. Size workspaces to power of 2 greater than N + M - 1.
     * 2. Fill work space with data ranges adding appropriate 0 padding to the end of the
     * workspaces.
     * 3. Perform forward FFTs on the workspaces.
     * 4. Piecewise multiplication of 2 spectras complex values.
     * 5. Perform inverse FFT on multiplied result.
     * 6. Copy real values out of the complex result.
     */
    template <typename InIter1, typename InIter2, typename OutIter>
    void operator()(InIter1 signalFirst, InIter1 signalLast, InIter2 kernelFirst,
                    InIter2 kernelLast, OutIter result)
    {
        // TODO: This all works but rewrite to use overlap and add method where
        // if the input signal lengths are particularly big then we don't want the
        // FFT convolution object to create large workspaces (and large FFTs) so
        // using overlap and add breaks the problem down into smaller chunks of
        // source data requiring smaller sized FFTs and workspace and stitches
        // the results back together. So we may want an FFT size limit under
        // which we use a single FFT for the convolution but if data length requires
        // an FFT above the size limit we break down into a series of smaller FFTs
        // based on picking an FFT size bigger than the shortest data range but with
        // enough spare samples to sensibly convolve with the other data range.
        // We probably in this case want to say range 1 should always be the
        // source data and range 2 is the kernel to be convolved with, e.g. the filter
        // kernel.
        //
        // The current method below is at least 3 times faster than using discrete
        // convolution.
        auto signalLength = std::distance(signalFirst, signalLast);
        DSP_ASSERT_THROW(signalLength <= static_cast<decltype(signalLength)>(m_workspace1.size()),
                         "signal length is incorrect");
        auto kernelLength = std::distance(kernelFirst, kernelLast);
        DSP_ASSERT_THROW(kernelLength <= static_cast<decltype(kernelLength)>(m_workspace2.size()),
                         "kernel length is incorrect");

        std::fill(
            std::next(m_workspace1.begin(), signalLength), m_workspace1.end(), complex_type{0, 0});
        std::fill(
            std::next(m_workspace2.begin(), kernelLength), m_workspace2.end(), complex_type{0, 0});

        auto convertToComplex = [](auto x) { return complex_type{x, 0}; };

        std::transform(signalFirst, signalLast, m_workspace1.begin(), convertToComplex);
        std::transform(kernelFirst, kernelLast, m_workspace2.begin(), convertToComplex);

        complex_fft::Forward(m_workspace1);
        complex_fft::Forward(m_workspace2);

        auto crossMultiply = [](auto const& x, auto const& y) {
            auto re = x.real() * y.real() - x.imag() * y.imag();
            auto im = x.imag() * y.real() + x.real() * y.imag();
            return complex_type{re, im};
        };

        std::transform(m_workspace1.begin(),
                       m_workspace1.end(),
                       m_workspace2.begin(),
                       m_workspace1.begin(),
                       crossMultiply);

        complex_fft::Inverse(m_workspace1);

        auto outFirst = m_workspace1.begin();
        auto outLast  = std::next(outFirst, static_cast<int>(m_discreteConvolutionLength));

        auto convertToReal = [](auto const& z) { return z.real(); };
        std::transform(outFirst, outLast, result, convertToReal);
    }

private:
    /*! \brief Length we'd expected due to discrete convolution. */
    size_t m_discreteConvolutionLength{0};
    /*! \brief Complex data vector workspace 1. */
    complex_vector m_workspace1{};
    /*! \brief Complex data vector workspace 2. */
    complex_vector m_workspace2{};
};

/*! \brief Convenience typedef to ComplexFFT<float>. */
using complex_fft_f = ComplexFFT<float>;
/*! \brief Convenience typedef to ComplexFFT<double>. */
using complex_fft_d = ComplexFFT<double>;
/*! \brief Convenience typedef to ComplexFFT<long double>. */
using complex_fft_ld = ComplexFFT<long double>;
/*! \brief Convenience typedef to ThreeBinSumFft<float>. */
using three_bin_sum_fft_f = ThreeBinSumFft<float>;
/*! \brief Convenience typedef to ThreeBinSumFft<double>. */
using three_bin_sum_fft_d = ThreeBinSumFft<double>;
/*! \brief Convenience typedef to ThreeBinSumFft<long double>. */
using three_bin_sum_fft_ld = ThreeBinSumFft<long double>;
/*! \brief Convenience typedef to MagnitudeFft<float>. */
using magnitude_fft_f = MagnitudeFft<float>;
/*! \brief Convenience typedef to MagnitudeFft<double>. */
using magnitude_fft_d = MagnitudeFft<double>;
/*! \brief Convenience typedef to MagnitudeFft<long double>. */
using magnitude_fft_ld = MagnitudeFft<long double>;
/*! \brief Convenience typedef to MagnitudeFft<float>. */
using fft_convolve_f = FftConvolve<float>;
/*! \brief Convenience typedef to MagnitudeFft<double>. */
using fft_convolve_d = FftConvolve<double>;
/*! \brief Convenience typedef to MagnitudeFft<long double>. */
using fft_convolve_ld = FftConvolve<long double>;


/*!
 * \brief FIR Low Pass Filter
 * \param[in] numTaps - Number of taps, i.e. number of filter coefficients.
 * \param[in] cutoffFreqHz - Cut-off frequency in Hz above which we'll filter out the signal.
 * \param[in] samplingFreqHz - Frequency the signal data has been sampled at in Hz.
 * \param[in] windowGenerator - A window generator functor to apply to the filter coefficients.
 * \return A vector filter coefficients.
 *
 * It is recommended to use a Kaiser window, e.g. KaiserGenerator. You can specify an odd or
 * even number of taps  but it is often better to use an odd number of taps to have a single
 * centre point around which the filter is symmetrical.
 */
template <typename FloatType, typename WinGenType>
std::vector<FloatType> FirLowPassFilter(size_t numTaps, FloatType cutoffFreqHz,
                                        FloatType samplingFreqHz, WinGenType const& windowGenerator)
{
    DSP_ASSERT_THROW(numTaps > 2, "numTaps too small");
    DSP_ASSERT_THROW(cutoffFreqHz > 0, "cutoffFreqHz <= 0");
    DSP_ASSERT_THROW(samplingFreqHz > 0, "samplingFreqHz  <= 0");
    auto availableBandwidthHz = samplingFreqHz / 2;
    DSP_ASSERT_THROW(cutoffFreqHz <= availableBandwidthHz, "cutoffFreqHz too high");
    static const auto      PI = Pi<FloatType>();
    std::vector<FloatType> filterCoeffs(numTaps);
    auto                   normalizedCutoffFreq   = cutoffFreqHz / availableBandwidthHz;
    auto                   numTapsMinusOneOverTwo = static_cast<FloatType>(numTaps - 1) / 2;
    size_t                 i                      = 0;

    for (auto& filterCoeff : filterCoeffs)
    {
        auto arg    = static_cast<FloatType>(i) - numTapsMinusOneOverTwo;
        filterCoeff = normalizedCutoffFreq * Sinc(normalizedCutoffFreq * arg * PI);
        ++i;
    }

    WindowFunction<FloatType> window(windowGenerator, numTaps, false);
    window(filterCoeffs.begin(), filterCoeffs.end(), filterCoeffs.begin());
    return filterCoeffs;
}

/*!
 * \brief FIR High Pass Filter
 * \param[in] numTaps - Number of taps, i.e. number of filter coefficients.
 * \param[in] cutoffFreqHz - Cut-off frequency in Hz below which we'll filter out the signal.
 * \param[in] samplingFreqHz - Frequency the signal data has been sampled at in Hz.
 * \param[in] windowGenerator - A window generator functor to apply to the filter coefficients.
 * \return A vector filter coefficients.
 *
 * It is recommended to use a Kaiser window, e.g. KaiserGenerator. The number of taps should be
 * odd for a high pass filter else the resultant filter will have undesirable zeroes and the
 * filtered signal will be attenuated.
 */
template <typename FloatType, typename WinGenType>
std::vector<FloatType> FirHighPassFilter(size_t numTaps, FloatType cutoffFreqHz,
                                         FloatType         samplingFreqHz,
                                         WinGenType const& windowGenerator)
{
    DSP_ASSERT_THROW(numTaps > 2, "numTaps too small");
    DSP_ASSERT_THROW(cutoffFreqHz > 0, "cutoffFreqHz <= 0");
    DSP_ASSERT_THROW(samplingFreqHz > 0, "samplingFreqHz  <= 0");
    auto availableBandwidthHz = samplingFreqHz / 2;
    DSP_ASSERT_THROW(cutoffFreqHz <= availableBandwidthHz, "cutoffFreqHz too high");
    bool isOdd = numTaps % 2 == 1;
    DSP_ASSERT_THROW(isOdd, "numTaps should be an odd number for high pass filter");
    static const auto      PI = Pi<FloatType>();
    std::vector<FloatType> filterCoeffs(numTaps);
    auto                   normalizedCutoffFreq   = cutoffFreqHz / availableBandwidthHz;
    auto                   numTapsMinusOneOverTwo = static_cast<FloatType>(numTaps - 1) / 2;
    size_t                 i                      = 0;

    for (auto& filterCoeff : filterCoeffs)
    {
        auto arg = static_cast<FloatType>(i) - numTapsMinusOneOverTwo;
        filterCoeff =
            Sinc(arg * PI) - (normalizedCutoffFreq * Sinc(normalizedCutoffFreq * arg * PI));
        ++i;
    }

    WindowFunction<FloatType> window(windowGenerator, numTaps, false);
    window(filterCoeffs.begin(), filterCoeffs.end(), filterCoeffs.begin());
    return filterCoeffs;
}

/*!
 * \brief FIR Band Pass Filter
 * \param[in] numTaps - Number of taps, i.e. number of filter coefficients.
 * \param[in] centreFreqHz - Centre frequency in Hz around which we'll retain the signal content.
 * \param[in] bandwidthHz - Bandwidth in Hz used with the centre frequency to define the pass band.
 * \param[in] samplingFreqHz - Frequency the signal data has been sampled at in Hz.
 * \param[in] windowGenerator - A window generator functor to apply to the filter coefficients.
 * \return A vector filter coefficients.
 *
 * It is recommended to use a Kaiser window, e.g. KaiserGenerator. You can specify an odd or
 * even number of taps  but it is often better to use an odd number of taps to have a single
 * centre point around which the filter is symmetrical.
 */
template <typename FloatType, typename WinGenType>
std::vector<FloatType> FirBandPassFilter(size_t numTaps, FloatType centreFreqHz,
                                         FloatType bandwidthHz, FloatType samplingFreqHz,
                                         WinGenType const& windowGenerator)
{
    DSP_ASSERT_THROW(numTaps > 2, "numTaps too small");
    DSP_ASSERT_THROW(centreFreqHz > 0, "cutoffFreqHz <= 0");
    DSP_ASSERT_THROW(samplingFreqHz > 0, "samplingFreqHz  <= 0");
    auto availableBandwidthHz = samplingFreqHz / 2;
    DSP_ASSERT_THROW(centreFreqHz <= availableBandwidthHz, "cutoffFreqHz too high");
    DSP_ASSERT_THROW(bandwidthHz > 0, "bandwidthHz  <= 0");
    DSP_ASSERT_THROW(bandwidthHz <= availableBandwidthHz, "bandwidthHz too high");
    static const auto      PI = Pi<FloatType>();
    std::vector<FloatType> filterCoeffs(numTaps);
    auto                   normalizedCentreFreq = centreFreqHz / availableBandwidthHz;
    auto                   normalizedBandwidth  = bandwidthHz / availableBandwidthHz;
    auto   normalizedLowCutoffFreq              = normalizedCentreFreq - (normalizedBandwidth / 2);
    auto   normalizeHighCutoffFreq              = normalizedCentreFreq + (normalizedBandwidth / 2);
    auto   numTapsMinusOneOverTwo               = static_cast<FloatType>(numTaps - 1) / 2;
    size_t i                                    = 0;

    for (auto& filterCoeff : filterCoeffs)
    {
        auto arg    = static_cast<FloatType>(i) - numTapsMinusOneOverTwo;
        filterCoeff = std::abs(arg) < FloatType(1.e-3) ? FloatType(0.)
                                                       : (cos(normalizedLowCutoffFreq * arg * PI) -
                                                          cos(normalizeHighCutoffFreq * arg * PI)) /
                                                             PI / arg;
        ++i;
    }

    WindowFunction<FloatType> window(windowGenerator, numTaps, false);
    window(filterCoeffs.begin(), filterCoeffs.end(), filterCoeffs.begin());
    return filterCoeffs;
}

/*!
 * \brief FIR Notch Filter
 * \param[in] numTaps - Number of taps, i.e. number of filter coefficients.
 * \param[in] centreFreqHz - Centre frequency in Hz around which we'll retain the signal content.
 * \param[in] bandwidthHz - Bandwidth in Hz used with the centre frequency to define the rejection
 * band.
 * \param[in] samplingFreqHz - Frequency the signal data has been sampled at in Hz.
 * \param[in] windowGenerator - A window generator functor to apply to the filter coefficients.
 * \return A vector filter coefficients.
 *
 * It is recommended to use a Kaiser window. The number of taps should be odd for a high pass
 * filter else the resultant filter will have undesirable zeroes and the filtered signal will
 * be attenuated.
 */
template <typename FloatType, typename WinGenType>
std::vector<FloatType> FirNotchFilter(size_t numTaps, FloatType centreFreqHz, FloatType bandwidthHz,
                                      FloatType samplingFreqHz, WinGenType const& windowGenerator)
{
    DSP_ASSERT_THROW(numTaps > 2, "numTaps too small");
    DSP_ASSERT_THROW(centreFreqHz > 0, "cutoffFreqHz <= 0");
    DSP_ASSERT_THROW(samplingFreqHz > 0, "samplingFreqHz  <= 0");
    auto availableBandwidthHz = samplingFreqHz / 2;
    DSP_ASSERT_THROW(centreFreqHz <= availableBandwidthHz, "cutoffFreqHz too high");
    DSP_ASSERT_THROW(bandwidthHz > 0, "bandwidthHz  <= 0");
    DSP_ASSERT_THROW(bandwidthHz <= availableBandwidthHz, "bandwidthHz too high");
    static const auto      PI = Pi<FloatType>();
    std::vector<FloatType> filterCoeffs(numTaps);
    auto                   normalizedCentreFreq = centreFreqHz / availableBandwidthHz;
    auto                   normalizedBandwidth  = bandwidthHz / availableBandwidthHz;
    auto   normalizedLowCutoffFreq              = normalizedCentreFreq - (normalizedBandwidth / 2);
    auto   normalizeHighCutoffFreq              = normalizedCentreFreq + (normalizedBandwidth / 2);
    auto   numTapsMinusOneOverTwo               = static_cast<FloatType>(numTaps - 1) / 2;
    size_t i                                    = 0;

    for (auto& filterCoeff : filterCoeffs)
    {
        auto arg    = static_cast<FloatType>(i) - numTapsMinusOneOverTwo;
        filterCoeff = Sinc(arg * PI) -
                      (normalizeHighCutoffFreq * Sinc(normalizeHighCutoffFreq * arg * PI)) -
                      (normalizedLowCutoffFreq * Sinc(normalizedLowCutoffFreq * arg * PI));
        ++i;
    }

    WindowFunction<FloatType> window(windowGenerator, numTaps, false);
    window(filterCoeffs.begin(), filterCoeffs.end(), filterCoeffs.begin());
    return filterCoeffs;
}

/*! \brief Filter holder class to manage applying filter coefficients to a signal. */
template <typename FloatType> class FilterHolder final
{
    /*! \brief Typedef to FftConvolve functor. */
    using fft_convolve_t = FftConvolve<FloatType>;

public:
    /*! \brief Default constructor. */
    FilterHolder() = default;
    /*! \brief Destructor. */
    ~FilterHolder() = default;
    /*! \brief Default copy constructor. */
    FilterHolder(FilterHolder const&) = default;
    /*! \brief Default move constructor. */
    FilterHolder(FilterHolder&&) = default;
    /*! \brief Default copy assignment operator. */
    FilterHolder& operator=(FilterHolder const&) = default;
    /*! \brief Default move assignment operator. */
    FilterHolder& operator=(FilterHolder&&) = default;

    /*!
     * \brief Initialisation constructor.
     * \param[in] signalLength - th length of the signal samples to be filtered.
     * \param[in] filterCoeffs - the filter coefficients to hold and apply.
     * \param[in] useFastConvolution - choose whether to use fast FFT based convolution or not.
     */
    FilterHolder(size_t signalLength, std::vector<FloatType> const& filterCoeffs,
                 bool useFastConvolution)
        : m_signalLength(signalLength)
        , m_filterCoeffs(filterCoeffs)
        , m_useFastConvolution(useFastConvolution)
        , m_filteredSignal(signalLength + m_filterCoeffs.size() - 1)
    {
        DSP_ASSERT_THROW(m_signalLength > 2, "signalLength is too small");
        DSP_ASSERT_THROW(!m_filterCoeffs.empty(), "filterCoeffs is empty");

        if (m_useFastConvolution)
        {
            m_fftConvolve = fft_convolve_t(signalLength, filterCoeffs.size());
        }
    }

    /*!
     * \brief Initialiser method.
     * \param[in] signalLength - th length of the signal samples to be filtered.
     * \param[in] filterCoeffs - the filter coefficients to hold and apply.
     * \param[in] useFastConvolution - choose whether to use fast FFT based convolution or not.
     */
    void Initialise(size_t signalLength, std::vector<FloatType> const& filterCoeffs,
                    bool useFastConvolution)
    {
        *this = std::move(FilterHolder(signalLength, filterCoeffs, useFastConvolution));
    }

    /*!
     * \brief Function operator used to apply filter to signal.
     * \param[in] signalFirst - Iterator to first signal sample.
     * \param[in] signalLast - Iterator to one past last actual signal sample.
     * \param[in] removeDelay - When true the delay introduced while applying the filter will ne
     * removed.
     * \param[out] resultFirst - Iterator to start of correctly sized result container.
     *
     * If the user opts to not remove the delay then the returned vector will contain N + M - 1
     * samples, where N is the number of signal samples and M is the number of filter coefficients.
     * If the user opts to remove the delay then the returned vector will contain N samples.
     */
    template <typename IterIn, typename OutIter>
    void operator()(IterIn signalFirst, IterIn signalLast, OutIter resultFirst, bool removeDelay)
    {
        auto signalLen = std::distance(signalFirst, signalLast);
        DSP_ASSERT_THROW(signalLen == static_cast<decltype(signalLen)>(m_signalLength),
                         "signal sample range incorrect");

        if (m_useFastConvolution)
        {
            m_fftConvolve(signalFirst,
                          signalLast,
                          m_filterCoeffs.begin(),
                          m_filterCoeffs.end(),
                          m_filteredSignal.begin());
        }
        else
        {

            Convolve(signalFirst,
                     signalLast,
                     m_filterCoeffs.begin(),
                     m_filterCoeffs.end(),
                     m_filteredSignal.begin());
        }

        if (removeDelay)
        {
            auto offset = m_filteredSignal.size() - m_signalLength;
            offset >>= 1;
            auto first = std::next(m_filteredSignal.begin(), static_cast<int>(offset));
            auto last  = std::next(first, signalLen);

            std::copy(first, last, resultFirst);
        }
    }

private:
    /*! \brief Copy of the filter coefficients. */
    size_t m_signalLength{0};
    /*! \brief Copy of the filter coefficients. */
    std::vector<FloatType> m_filterCoeffs{};
    /*! \brief Use fast FFT convolution flag. */
    bool m_useFastConvolution{true};
    /*! \brief FFT convolution functor. */
    fft_convolve_t m_fftConvolve{};
    /*! \brief Workspace vector holding the filtered signal. */
    std::vector<FloatType> m_filteredSignal{};
};

/*! \brief Convenience typedef to FilterHolder<float>. */
using filter_hldr_f = FilterHolder<float>;
/*! \brief Convenience typedef to FilterHolder<double>. */
using filter_hldr_d = FilterHolder<double>;
/*! \brief Convenience typedef to FilterHolder<long double>. */
using filter_holder_ld = FilterHolder<long double>;



/*!
 * \brief Compute closest resample up and down factors given a real valued resample factor.
 * \param[in] requiredResampleFactor - The resample factor we ideally want to use, should be != 1.
 * \param[in] maxNumerator - The maximum numerator to allow in the final result.
 * \param[in] maxDenominator - The maximum denominator to allow in the final result.
 * \return A pair containing the upsample factor and the downsample factor.
 *
 * This function uses the concept of mediants to interval bisect between an upper and lower
 * bound either side of the requiredResampleFactor until the termination conditions are
 * met and a suitable pair of resample factors is found.
 */
inline std::pair<size_t, size_t> ComputeResampleFactors(double requiredResampleFactor,
                                                        size_t maxNumerator   = 128,
                                                        size_t maxDenominator = 128)
{
    DSP_ASSERT_THROW(requiredResampleFactor > 0, "requiredResampleFactor <= 0");
    std::pair<size_t, size_t> factors(0, 0);
    auto                      n_a   = static_cast<size_t>(std::floor(requiredResampleFactor) + 0.5);
    size_t                    d_a   = 1;
    auto                      n_b   = static_cast<size_t>(std::ceil(requiredResampleFactor) + 0.5);
    size_t                    d_b   = 1;
    double                    error = std::numeric_limits<double>::max();

    while (true)
    {
        auto n_m = n_a + n_b;
        auto d_m = d_a + d_b;

        // Make sure we reduce the numerator and denominator as much as possible.
        // Although in practice we may never need this here as n_m and d_m are
        // likely already in their lowest form.
        auto g = Gcd(n_m, d_m);

        if (g > 1)
        {
            n_m /= g;
            d_m /= g;
        }

        if ((n_m > maxNumerator) || (d_m > maxDenominator))
        {
            break;
        }

        double m       = static_cast<double>(n_m) / static_cast<double>(d_m);
        auto   absDiff = std::abs(m - requiredResampleFactor);

        if (absDiff < error)
        {
            error   = absDiff;
            factors = {n_m, d_m};
        }

        if (m <= requiredResampleFactor)
        {
            n_a = n_m;
            d_a = d_m;
        }
        else
        {
            n_b = n_m;
            d_b = d_m;
        }
    }

    return factors;
}

/*! \brief Resampling class uses FIR filter and Kaiser window.
 *
 * This class is designed to be created upfront and then reused repeatedly to
 * perform ongoing resampling of a given signal and so it creates its internal
 * objects, such as filters, window function and workspace buffers upfront.
 *
 * Workspace buffers are allocated by this class the first time it resamples signal
 * samples and will only resize the workspace if required on subsequent calls to
 * resample more signal samples.
 *
 * Therefore creation and first call of this class to perform resampling is slower
 * than subsequent calls due to setting everything up.
 *
 * The general algorithm is as follows:
 * 1. Fill upsample buffer with signal samples spaced with zero padding samples.
 * 2. Correct for signal attenuation caused by zero padding upsampling.
 * 3. Low pass filter the upsample buffer.
 * 4. Downsample by skipping not required samples.
 * 5. Return the resampled signal.
 */
template <typename FloatType> class Resample final
{
    /*! \brief Typedef to FilterHolder. */
    using filter_hldr_t = FilterHolder<FloatType>;

public:
    /*! \brief Default constructor. */
    Resample() = default;
    /*! \brief Destructor. */
    ~Resample() = default;
    /*! \brief Default copy constructor. */
    Resample(Resample const&) = default;
    /*! \brief Default move constructor. */
    Resample(Resample&&) = default;
    /*! \brief Default copy assignment operator. */
    Resample& operator=(Resample const&) = default;
    /*! \brief Default move assignment operator. */
    Resample& operator=(Resample&&) = default;

    /*!
     * \brief Initialisation constructor.
     * \param[in] signalLength - The number of signal samples to be resampled each time.
     * \param[in] upsampleFactor - Factor by which we need to upsample by >= 1.
     * \param[in] downsampleFactor - Factor by which we need to downsample by >= 1.
     * \param[in] samplingFreqHz - Sampling frequency of signal in Hz which we wish to resample.
     * \param[in] maxCutoffFreqHz - The low pass filter max cutoff freq, typically new sample rate /
     * 2 when start freq is 0Hz.
     * \param[in] numFilterTaps - The number of filter coefficients we
     * require.
     * \param[in] kaiserWindowBeta - Beta parameter controlling side lobes of Kaiser
     * window.
     * \param[in] useFastConvolution - choose whether to use fast FFT based convolution or
     * not.
     */
    Resample(size_t signalLength, size_t upsampleFactor, size_t downsampleFactor,
             FloatType samplingFreqHz, FloatType maxCutoffFreqHz, size_t numFilterTaps,
             double kaiserWindowBeta, bool useFastConvolution)
        : m_signalLength(signalLength)
        , m_upsampleFactor(upsampleFactor)
        , m_downsampleFactor(downsampleFactor)
    {
        DSP_ASSERT_THROW(m_signalLength > 0, "too few signal samples");
        DSP_ASSERT_THROW(m_upsampleFactor > 0, "invalid upsample factor");
        DSP_ASSERT_THROW(m_downsampleFactor > 0, "invalid downsample factor");

        ComputeResampledSize();
        ResizeWorkspace();

        auto upsampleLength  = m_upsampleFactor * m_signalLength;
        auto upsampledFreqHz = samplingFreqHz * static_cast<FloatType>(m_upsampleFactor);
        auto resampledFreqHz = samplingFreqHz * static_cast<FloatType>(upsampleFactor) /
                               static_cast<FloatType>(downsampleFactor);
        auto cutoffFreqHz = std::min(samplingFreqHz, resampledFreqHz) / 2;

        if (upsampleFactor > downsampleFactor)
        {
            maxCutoffFreqHz = std::min(cutoffFreqHz, maxCutoffFreqHz);
        }
        else
        {
            maxCutoffFreqHz = std::max(cutoffFreqHz, maxCutoffFreqHz);
        }

        m_filterHolder = filter_hldr_t(
            upsampleLength,
            FirLowPassFilter(
                numFilterTaps, maxCutoffFreqHz, upsampledFreqHz, KaiserGenerator(kaiserWindowBeta)),
            useFastConvolution);
    }

    /*!
     * \brief Initialisation method.
     * \param[in] upsampleFactor - Factor by which we need to upsample by >= 1.
     * \param[in] downsampleFactor - Factor by which we need to downsample by >= 1.
     * \param[in] samplingFreqHz - Sampling frequency of signal in Hz which we wish to resample.
     * \param[in] maxCutoffFreqHz - The low pass filter cutoff freq, typically new sample rate / 2
     * when start freq is 0Hz.
     * \param[in] numFilterTaps - The number of filter coefficients we
     * require. \param[in] kaiserWindowBeta - Beta parameter controlling side lobes of Kaiser
     * window. \param[in] useFastConvolution - choose whether to use fast FFT based convolution or
     * not.
     */
    void Initialise(size_t upsampleFactor, size_t downsampleFactor, FloatType samplingFreqHz,
                    FloatType maxCutoffFreqHz, size_t numFilterTaps, double kaiserWindowBeta,
                    bool useFastConvolution)
    {
        *this = std::move(Resample(upsampleFactor,
                                   downsampleFactor,
                                   samplingFreqHz,
                                   maxCutoffFreqHz,
                                   numFilterTaps,
                                   kaiserWindowBeta,
                                   useFastConvolution));
    }

    /*!
     * \brief Get the original data size.
     * \return The size of the signal data.
     */
    size_t DataSize() const
    {
        return m_signalLength;
    }

    /*!
     * \brief Get the resampled data size.
     * \return The size required for the resampled container.
     */
    size_t ResampledSize() const
    {
        return m_resampledLength;
    }

    /*!
     * \brief Function operator to perform resampling.
     * \param[in] signalFirst - Iterator to first signal sample.
     * \param[in] signalLast - Iterator to one past the last signal sample.
     * \param[out] resultFirst - Iterator to start of correctly sized result container.
     *
     * The signal samples should be from a uniformly sampled source.
     *
     * This class will perform upsampling and downsampling as specifed and as required.
     *
     * The number of resampled samples returned in a vector will be:
     * floor(N * U / D), where N is number of signal samples, U is the upsample factor
     * and D is the downsample factor.
     *
     * The sample rate of the resampled data is equivalent to:
     * S * U / D, where S is the signals original sample rate in Hz.
     *
     * To compute S to allocate a correctly size result vector call
     * ResampledSize method.
     */
    template <typename IterIn, typename OutIter>
    void operator()(IterIn signalFirst, IterIn signalLast, OutIter resultFirst)
    {
        auto signalLength = std::distance(signalFirst, signalLast);
        DSP_ASSERT_THROW(signalLength == static_cast<decltype(signalLength)>(m_signalLength),
                         "sample length is incorrect");

        if (m_upsampleFactor > 1)
        {
            // Make sure upsample buffer starts zeroed.
            std::fill(
                m_workspaceBuffer.begin(), m_workspaceBuffer.end(), static_cast<FloatType>(0));

            // Fill upsample buffer with signal samples with zero padding.
            auto upIter = m_workspaceBuffer.begin();

            for (auto itr = signalFirst; (itr != signalLast) && (upIter < m_workspaceBuffer.end());
                 std::advance(itr, 1), std::advance(upIter, static_cast<int>(m_upsampleFactor)))
            {
                // Note: we correct for attenuation by upsample factor, caused by inserting zeroes.
                *upIter = *itr * static_cast<FloatType>(m_upsampleFactor);
            }

            // Low pass filter the workspace buffer.
            m_filterHolder(m_workspaceBuffer.begin(),
                           m_workspaceBuffer.end(),
                           m_workspaceBuffer.begin(),
                           true);

            // If required perform downsampling.
            if (m_downsampleFactor > 1)
            {
                // Fill the result buffer by dropping samples as required.
                auto upIter     = m_workspaceBuffer.begin();
                auto resultIter = resultFirst;

                for (auto i = 0; i < m_resampledLength; ++i,
                          std::advance(upIter, static_cast<int>(m_downsampleFactor)),
                          std::advance(resultIter, 1))
                {
                    *resultIter = *upIter;
                }
            }
            // Else return upsample buffer as is copied ito the result buffer.
            else
            {
                std::copy(m_workspaceBuffer.begin(), m_workspaceBuffer.end(), resultFirst);
            }
        }
        else
        {
            // Low pass filter the signal data.
            m_filterHolder(signalFirst, signalLast, m_workspaceBuffer.begin(), true);

            // Fill the result buffer by dropping samples as required.
            auto resultIter = resultFirst;

            for (auto itr = m_workspaceBuffer.begin(); itr < m_workspaceBuffer.end();
                 std::advance(itr, static_cast<int>(m_downsampleFactor)),
                      std::advance(resultIter, 1))
            {
                *resultIter = *itr;
            }
        }
    }

private:
    /*! \brief Compute the resampled data size. */
    void ComputeResampledSize()
    {
        auto upsampleLength = m_upsampleFactor * m_signalLength;
        m_resampledLength =
            static_cast<size_t>(std::floor((static_cast<double>(upsampleLength) /
                                            static_cast<double>(m_downsampleFactor))) +
                                0.5);
    }
    /*! \brief Resize the workspace buffers. */
    void ResizeWorkspace()
    {
        if (m_upsampleFactor > 1)
        {
            auto upsampleLength = m_upsampleFactor * m_signalLength;
            m_workspaceBuffer.resize(upsampleLength);
        }
        else
        {
            m_workspaceBuffer.resize(m_signalLength);
        }
    }

private:
    /*! \brief The number of signal samples to resample. */
    size_t m_signalLength{0};
    /*! \brief The upsample factor. */
    size_t m_upsampleFactor{1};
    /*! \brief The downsample factor. */
    size_t m_downsampleFactor{1};
    /*! \brief The number of samples in the resampled result. */
    size_t m_resampledLength{0};
    /*! \brief Filter holder for our kaiser window low pass FIR filter. */
    filter_hldr_t m_filterHolder{};
    /*! \brief Workspace buffer. */
    std::vector<FloatType> m_workspaceBuffer{};
};

/*! \brief Convenience typedef to Resample<float>. */
using resample_f = Resample<float>;
/*! \brief Convenience typedef to Resample<double>. */
using resample_d = Resample<double>;
/*! \brief Convenience typedef to Resample<long double>. */
using resample_ld = Resample<long double>;

} // namespace dsp

