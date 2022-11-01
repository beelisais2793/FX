#pragma once

#include "tfp/math/TransferFunction.hxx"
#include "tfp/math/CoefficientPolynomial.hpp"
#include "tfp/math/RootPolynomial.hpp"
#include <cmath>

namespace tfp {

// ----------------------------------------------------------------------------
template <class T>
TransferFunction<T>::TransferFunction()
{
}

// ----------------------------------------------------------------------------
template <class T>
TransferFunction<T>::TransferFunction(const CoefficientPolynomial<T>& numerator,
                                      const CoefficientPolynomial<T>& denominator) :
    numerator_(numerator.roots()),
    denominator_(denominator.roots())
{
}

// ----------------------------------------------------------------------------
template<class T>
TransferFunction<T>::TransferFunction(const PolynomialType& numerator,
                                      const PolynomialType& denominator) :
    numerator_(numerator),
    denominator_(denominator)
{
}

// ----------------------------------------------------------------------------
template <class T>
RootPolynomial<T>& TransferFunction<T>::numerator()
{
    return numerator_;
}

// ----------------------------------------------------------------------------
template <class T>
RootPolynomial<T>& TransferFunction<T>::denominator()
{
    return denominator_;
}

// ----------------------------------------------------------------------------
template <class T>
const RootPolynomial<T>& TransferFunction<T>::numerator() const
{
    return numerator_;
}

// ----------------------------------------------------------------------------
template <class T>
const RootPolynomial<T>& TransferFunction<T>::denominator() const
{
    return denominator_;
}

// ----------------------------------------------------------------------------
template <class T>
T TransferFunction<T>::factor() const
{
    return numerator_.factor() / denominator_.factor();
}

// ----------------------------------------------------------------------------
template <class T>
std::complex<T> TransferFunction<T>::evaluate(const std::complex<T>& value) const
{
    return numerator_.evaluate(value) / denominator_.evaluate(value);
}

// ----------------------------------------------------------------------------
template <class T>
typename TransferFunction<T>::PFEResultData
TransferFunction<T>::partialFractionExpansion(int numZeroPoles) const
{
    PFEResultData result;

    // Create poles vector. Add as many zero poles as specified
    result.poles_.resize(denominator_.size() + numZeroPoles, 1);
    for (int i = 0; i < denominator_.size(); ++i)
        result.poles_(i) = denominator_.root(i);
    for (int i = 0; i < numZeroPoles; ++i)
        result.poles_(denominator_.size() + i) = typename Type<T>::Complex(0, 0);

    // Calculate the direct terms, if necessary
    int numeratorDegree   = numerator_.size();
    int denominatorDegree = result.poles_.size();
    if(numeratorDegree >= denominatorDegree)
    {
        // TODO need to calculate the direct terms
    }

    // Calculate residuals
    result.residuals_.resize(denominatorDegree, 1);
    typename Type<T>::ComplexArray otherPoles(denominatorDegree - 1, 1);
    for (int m = 0; m < denominatorDegree; ++m)
    {
        for (int i = 0, j = 0; j < denominatorDegree; ++j)
        {
            if (j == m)
                continue;
            otherPoles(i) = result.poles_(j);
            ++i;
        }

        RootPolynomial<T> aTilde(otherPoles, 1);
        std::complex<T> pvB = numerator_.evaluate(result.poles_(m));
        std::complex<T> pvA = aTilde.evaluate(result.poles_(m));
        std::complex<T> pvD = pvB / pvA;
        result.residuals_(m) = pvD / denominator_.factor();
    }

    return result;
}

// ----------------------------------------------------------------------------
template <class T>
void TransferFunction<T>::inverseLaplaceTransform(
        const PFEResultData& pfe,
        T* outTime,
        T* outAmp,
        T timeBegin,
        T timeEnd,
        int numPoints
    ) const
{
    /*
     *                            a
     *  E(t)ae^(-at)    o--o    -----
     *                          s + a
     */
    for (int i = 0; i < numPoints; ++i)
    {
        outTime[i] = (timeEnd - timeBegin) * T(i) / (numPoints-1);
        outAmp[i] = 0;
        for (int m = 0; m < pfe.residuals_.size(); ++m)
        {
            typename Type<T>::Complex a = pfe.poles_(m);
            typename Type<T>::Complex k = pfe.residuals_(m);

            // We're only interested in calculating the real part, since the
            // imaginary parts will cancel out to 0 if the function is real.
            //     Equivalent to:   outAmp[i] += k * std::exp(a * outTime[i]);
            outAmp[i] += std::exp(a.real()*outTime[i]) * (k.real() * std::cos(a.imag()*outTime[i]) - k.imag()*std::sin(a.imag()*outTime[i]));
            //outAmp[i] += (k * std::exp(a * outTime[i])).real();
        }
    }
}

// ----------------------------------------------------------------------------
template <class T>
void TransferFunction<T>::impulseResponse(T* outTime, T* outAmp, T timeBegin, T timeEnd, int numPoints) const
{
    PFEResultData pfe = partialFractionExpansion();
    return inverseLaplaceTransform(pfe, outTime, outAmp, timeBegin, timeEnd, numPoints);
}

// ----------------------------------------------------------------------------
template <class T>
void TransferFunction<T>::stepResponse(T* outTime, T* outAmp, T timeBegin, T timeEnd, int numPoints) const
{
    // Add 1 zero-pole, which is the equivalent of integrating the function
    // in the time domain (because this is the step response)
    PFEResultData pfe = partialFractionExpansion(1);
    return inverseLaplaceTransform(pfe, outTime, outAmp, timeBegin, timeEnd, numPoints);
}

// ----------------------------------------------------------------------------
template <class T>
void TransferFunction<T>::frequencyResponse(T* outFreq, T* outAmp, T* outPhase, T freqStart, T freqEnd, int numPoints) const
{
    for (int i = 0; i < numPoints; ++i)
    {
        outFreq[i] = freqStart * std::pow(freqEnd / freqStart, T(i) / (numPoints-1));
        typename Type<T>::Complex result = evaluate(typename Type<T>::Complex(0, outFreq[i]));
        outAmp[i] = 20 * std::log10(std::abs(result));
        outPhase[i] = std::arg(result) * 180.0 / M_PI;
    }
}

// ----------------------------------------------------------------------------
template <class U>
std::ostream& operator<<(std::ostream& os, const TransferFunction<U>& tf)
{
    os << tf.numerator_ << '\n' << tf.denominator_;
    return os;
}

} // namespace tfp
