#pragma once

#include "tfp/math/RootPolynomial.hxx"
#include "tfp/math/CoefficientPolynomial.hpp"

namespace tfp {

// ----------------------------------------------------------------------------
template <class T>
UniqueRootIterator<T>::UniqueRootIterator(const RootPolynomial<T>* rootPolynomial) :
    rootPolynomial_(rootPolynomial),
    index_(0)
{
}

// ----------------------------------------------------------------------------
template <class T>
const typename Type<T>::Complex& UniqueRootIterator<T>::get() const
{
    return rootPolynomial_->roots_[index_];
}

// ----------------------------------------------------------------------------
template <class T>
bool UniqueRootIterator<T>::hasNext()
{
    return index_ < rootPolynomial_->roots_.size();
}

// ----------------------------------------------------------------------------
template <class T>
void UniqueRootIterator<T>::next()
{
    while (true)
    {
        ++index_;
        if (hasNext() == false)
            break;

        if (rootPolynomial_->roots_[index_].first >= 1)
            break;
    }
}

// ----------------------------------------------------------------------------
template <class T>
RootPolynomial<T>::RootPolynomial() :
    factor_(1),
    multiplicityDirty_(true)
{
}

// ----------------------------------------------------------------------------
template <class T>
RootPolynomial<T>::RootPolynomial(int size) :
    roots_(size, 1),
    factor_(1),
    multiplicityDirty_(true)
{
}

// ----------------------------------------------------------------------------
template <class T>
RootPolynomial<T>::RootPolynomial(const typename Type<T>::ComplexVector& roots, T factor) :
    roots_(roots),
    factor_(factor),
    multiplicityDirty_(true)
{
}

// ----------------------------------------------------------------------------
template <class T>
RootPolynomial<T>::RootPolynomial(const CoefficientPolynomial<T>& polynomial) :
    roots_(polynomial.roots().roots_),
    factor_(polynomial.coefficients_(0)),
    multiplicityDirty_(true)
{
}

// ----------------------------------------------------------------------------
template <class T>
void RootPolynomial<T>::resize(int size)
{
    roots_.resize(size, Eigen::NoChange);
}

// ----------------------------------------------------------------------------
template <class T>
int RootPolynomial<T>::size() const
{
    return roots_.size();
}

// ----------------------------------------------------------------------------
template <class T>
void RootPolynomial<T>::setRoot(int index, const typename Type<T>::Complex& root)
{
    multiplicityDirty_ = true;
    roots_[index] = root;
}

// ----------------------------------------------------------------------------
template <class T>
void RootPolynomial<T>::setRoot(int index, T root)
{
    multiplicityDirty_ = true;
    roots_[index] = typename Type<T>::Complex(root, 0);
}

// ----------------------------------------------------------------------------
template <class T>
const typename Type<T>::Complex& RootPolynomial<T>::root(int index) const
{
    return roots_[index];
}

// ----------------------------------------------------------------------------
template <class T>
int RootPolynomial<T>::multiplicity(int index)
{
    updateMultiplicities();

    int multiplicity = multiplicities_[index];
    if (multiplicity < 1)
        multiplicity = multiplicities_[-multiplicity];
    return multiplicity;
}

// ----------------------------------------------------------------------------
template <class T>
void RootPolynomial<T>::updateMultiplicities()
{
    if (multiplicityDirty_)
    {
        multiplicities_.resize(roots_.size());
        for (int i = 0; i != multiplicities_.size(); ++i)
            multiplicities_[i] = 1;

        for (int i = 0; i != roots_.size(); ++i)
        {
            for (int j = i + 1; j < roots_.size(); ++j)
            {
                if (roots_[i] == roots_[j])
                {
                    if (multiplicities_[i] >= 1)
                    {
                        multiplicities_[i]++;
                        multiplicities_[j] = -i;
                    }
                }
            }
        }

        multiplicityDirty_ = false;
    }
}

// ----------------------------------------------------------------------------
template <class T>
UniqueRootIterator<T> RootPolynomial<T>::uniqueRoots()
{
    updateMultiplicities();
    return UniqueRootIterator<T>(this);
}

// ----------------------------------------------------------------------------
template <class T>
void RootPolynomial<T>::setFactor(T factor)
{
    factor_ = factor;
}

// ----------------------------------------------------------------------------
template <class T>
T RootPolynomial<T>::factor() const
{
    return factor_;
}

// ----------------------------------------------------------------------------
template <class T>
CoefficientPolynomial<T> RootPolynomial<T>::poly() const
{
    // Need complex buffers for the computation, even though the resulting
    // coefficients are real (since we're only dealing with complex-conjugate)
    // roots (real functions)
    typename Type<T>::ComplexVector coeffs = Type<T>::ComplexVector::Zero(roots_.size() + 1);
    typename Type<T>::ComplexVector temp(roots_.size() + 1);
    coeffs(0) = typename Type<T>::Complex(1, 0);

    for (int i = 0; i < roots_.size(); ++i)
    {
        // multiply coefficients with current root and store in temp buffer
        for (int j = 0; j < coeffs.size(); ++j) {
            temp(j) = roots_(i) * coeffs(j);
        }
        // subtract temp buffer from coefficients
        for (int j = 1; j < coeffs.size(); ++j) {
            coeffs(j) -= temp(j - 1);
        }
    }

    // There should be no more complex values.
    CoefficientPolynomial<T> ret;
    ret.resize(coeffs.size());
    for (int i = 0; i < coeffs.size(); ++i)
        ret.setCoefficient(i, coeffs(i).real() * factor_);

    return ret;
}

// ----------------------------------------------------------------------------
template <class T>
typename Type<T>::Complex RootPolynomial<T>::evaluate(const typename Type<T>::Complex& value) const
{
    if (roots_.size() == 0)
        return typename Type<T>::Complex(factor_, 0);

    typename Type<T>::ComplexArray factors(roots_.size(), 1);
    factors = value - typename Type<T>::ComplexArray(roots_);
    typename Type<T>::Complex ret = factors(0);
    for (int i = 1; i < factors.size(); ++i)
        ret *= factors(i);
    return ret * factor_;
}

// ----------------------------------------------------------------------------
template <class U>
std::ostream& operator<<(std::ostream& os, const RootPolynomial<U>& polynomial)
{
    os << polynomial.roots_;
    return os;
}

} // namespace tfp
