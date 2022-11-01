/* Copyright (C) 2021 Maciej Rozewicz - All Rights Reserved
 * You may use, distribute and modify this code under the
 * terms of the XYZ license, which unfortunately won't be
 * written for another century.
 *
 * You should have received a copy of the XYZ license with
 * this file. If not, please write to: , or visit :
 */

#include "polynomial_discretizer.hpp"

#include <stdexcept>

namespace tf_core
{
    PolynomialDiscretizer::DiscretizedPolynomial PolynomialDiscretizer::Discretize(const Polynomial & continuous,
        const float discretization_time,
        const DiscretizationMethod discretization_method) {
        auto discrete_polynomial = PolynomialFraction(Polynomial({0.0f}), Polynomial({1.0f}));
        auto s = GetApproximation(discretization_time, discretization_method);

        for (auto idx = 0u; idx < continuous.GetPolynomialOrder(); idx++)
            discrete_polynomial += s.Power(idx) * continuous.GetCoefficients().at(idx);
        
        return std::make_tuple(discrete_polynomial.GetNum(), discrete_polynomial.GetDen());
    }

    PolynomialFraction PolynomialDiscretizer::GetApproximation(const float discretization_time,
        const DiscretizationMethod discretization_method) {
        switch (discretization_method) {
            case DiscretizationMethod::Forward:
                return PolynomialFraction(Polynomial({-1.0f, 1.0f}), Polynomial({discretization_time}));
            case DiscretizationMethod::Backward:
                return PolynomialFraction(Polynomial({-1.0f, 1.0f}), Polynomial({0.0f, discretization_time}));
            case DiscretizationMethod::Tustin:
                return PolynomialFraction(Polynomial({-1.0f, 1.0f}), Polynomial({1.0f, 1.0f})) * (2.0f / discretization_time);
            default:
                throw std::invalid_argument("PolynomialDiscretizer::GetApproximation - Invali discretization method");
        }
    }
}   // namespace tf_core
