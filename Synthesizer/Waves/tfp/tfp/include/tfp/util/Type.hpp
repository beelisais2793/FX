#pragma once

#include <Eigen/Dense>
#include <complex>

namespace tfp {

template <class T>
struct Type
{
    typedef std::complex<T> Complex;
    typedef Eigen::Array<T, Eigen::Dynamic, 1> RealArray;
    typedef Eigen::Array<Complex, Eigen::Dynamic, 1> ComplexArray;
    typedef Eigen::Matrix<T, Eigen::Dynamic, 1> RealVector;
    typedef Eigen::Matrix<Complex, Eigen::Dynamic, 1> ComplexVector;
};

} // namespace tfp
