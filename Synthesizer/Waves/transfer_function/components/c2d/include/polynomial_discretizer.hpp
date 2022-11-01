/* Copyright (C) 2021 Maciej Rozewicz - All Rights Reserved
 * You may use, distribute and modify this code under the
 * terms of the XYZ license, which unfortunately won't be
 * written for another century.
 *
 * You should have received a copy of the XYZ license with
 * this file. If not, please write to: , or visit :
 */

#ifndef COMPONENTS_C2D_INCLUDE_POLYNOMIAL_DISCRETIZER_HPP_
#define COMPONENTS_C2D_INCLUDE_POLYNOMIAL_DISCRETIZER_HPP_

#include <tuple>
#include <vector>

#include "discretization_method.hpp"
#include "polynomial.hpp"
#include "polynomial_fraction.hpp"

namespace tf_core
{
    class PolynomialDiscretizer
    {
        public:
            PolynomialDiscretizer(void) = default;
            ~PolynomialDiscretizer(void) = default;

            using DiscretizedPolynomial = std::tuple<Polynomial, Polynomial>;

            static DiscretizedPolynomial Discretize(const Polynomial & continuous,
                const float discretization_time,
                const DiscretizationMethod discretization_method);

        private:
            static PolynomialFraction GetApproximation(const float discretization_time,
                const DiscretizationMethod discretization_method);
    };
}   //  namespace tf_core

#endif // COMPONENTS_C2D_INCLUDE_POLYNOMIAL_DISCRETIZER_HPP_
