#pragma once

#include "tfp/config.hpp"
#include "tfp/math/RootPolynomial.hxx"
#include "tfp/math/CoefficientPolynomial.hxx"
#include <iostream>

namespace tfp {

template <class T>
class TransferFunction
{
public:
    typedef RootPolynomial<T> PolynomialType;

    struct PFEResultData
    {
        typename Type<T>::ComplexVector poles_;
        typename Type<T>::ComplexVector residuals_;
        typename Type<T>::RealVector directTerms_;
    };

    TransferFunction();
    TransferFunction(const CoefficientPolynomial<T>& numerator, const CoefficientPolynomial<T>& denominator);
    TransferFunction(const RootPolynomial<T>& numerator, const RootPolynomial<T>& denominator);

    RootPolynomial<T>& numerator();
    RootPolynomial<T>& denominator();
    const RootPolynomial<T>& numerator() const;
    const RootPolynomial<T>& denominator() const;
    T factor() const;

    std::complex<T> evaluate(const std::complex<T>& value) const;

    PFEResultData partialFractionExpansion(int numZeroPoles=0) const;
    void inverseLaplaceTransform(const PFEResultData& pfe, T* outTime, T* outAmp, T timeBegin, T timeEnd, int numPoints) const;

    void impulseResponse(T* outTime, T* outAmp, T timeBegin, T timeEnd, int numPoints) const;
    void stepResponse(T* outTime, T* outAmp, T timeBegin, T timeEnd, int numPoints) const;
    void frequencyResponse(T* outFreq, T* outAmp, T* outPhase, T freqStart, T freqEnd, int numPoints) const;

    template <class U>
    friend std::ostream& operator<<(std::ostream& os, const TransferFunction<U>& tf);

private:
    RootPolynomial<T> numerator_;
    RootPolynomial<T> denominator_;
};

} // namespace tfp
