#pragma once

#include "tfp/config.hpp"
#include "tfp/util/Type.hpp"
#include "Eigen/Eigenvalues"
#include <vector>

namespace tfp {

template <class T> class CoefficientPolynomial;
template <class T> class RootPolynomial;

template <class T>
class UniqueRootIterator
{
public:
    const typename Type<T>::Complex& get() const;
    bool hasNext();
    void next();

private:
    UniqueRootIterator(const RootPolynomial<T>* rootPolynomial);

    const RootPolynomial<T>* rootPolynomial_;
    int index_;
};

/*!
 * @brief A polynomial function consisting of roots (as opposed to
 * coefficients, see CoefficientPolynomial).
 */
template <class T>
class RootPolynomial
{
public:
    RootPolynomial();
    RootPolynomial(int size);
    RootPolynomial(const typename Type<T>::ComplexVector& roots, T factor);
    RootPolynomial(const CoefficientPolynomial<T>& coeffs);

    /*!
     * @brief Resizes the internal storage of roots. If expanding, the new
     * slots remain unintialised, so you must set them by calling root().
     */
    void resize(int size);

    //! Returns the number of roots (**including** multiple roots)
    int size() const;
    //! Sets the value of a root at the specified index
    void setRoot(int index, const typename Type<T>::Complex& root);
    //! Sets the real part of the root at the specified index
    void setRoot(int index, T real);
    //! Retrieves a root at the specified index.
    const typename Type<T>::Complex& root(int index) const;

    /*!
     * @brief Returns the multiplicity of the specified root. When setting the
     * value of a root, it is checked against the existing roots. If an
     * identical root is found, then its multiplicity is increased.
     */
    int multiplicity(int index);

    UniqueRootIterator<T> uniqueRoots();

    void setFactor(T factor);

    /*!
     * @brief  Returns the constant factor which was factored out when
     * calculating the monic polynomial. E.g.
     *
     * 2s + 4  -->  2(s+2)    factor=2
     */
    T factor() const;

    /*!
     * Computes the polynomial coefficients with the specified roots.
     * This was ported from matlab's poly() function
     * Type ">> edit poly" and scroll to line 35.
     * @param roots Roots.
     * @return Polynomial coefficients.
     */
    CoefficientPolynomial<T> poly() const;

    typename Type<T>::Complex evaluate(const typename Type<T>::Complex& value) const;

    template <class U>
    friend std::ostream& operator<<(std::ostream& os, const RootPolynomial<U>& polynomial);
    friend class UniqueRootIterator<T>;

private:
    void updateMultiplicities();

    typename Type<T>::ComplexVector roots_;
    std::vector<T> multiplicities_;
    T factor_;
    bool multiplicityDirty_;
};

} // namespace tfp
