/* Copyright (C) 2021 Maciej Rozewicz - All Rights Reserved
 * You may use, distribute and modify this code under the
 * terms of the XYZ license, which unfortunately won't be
 * written for another century.
 *
 * You should have received a copy of the XYZ license with
 * this file. If not, please write to: , or visit :
 */

#ifndef COMPONENTS_POLYNOMIAL_INCLUDE_POLYNOMIAL_HPP_
#define COMPONENTS_POLYNOMIAL_INCLUDE_POLYNOMIAL_HPP_

#include <vector>
#include <string>

#include <Eigen/Dense>

namespace tf_core
{
    class Polynomial
    {
        public:
            using Coefficients = std::vector<float>;

            Polynomial(void);
            Polynomial(const Polynomial & polynomial) = default;
            Polynomial(Polynomial && polynomial) = default;
            ~Polynomial(void) = default;
            Polynomial & operator=(const Polynomial & polynomial) = default;
            Polynomial & operator=(Polynomial && polynomial) = delete;

            explicit Polynomial(const Coefficients & coefficients);

            Polynomial operator+(const Polynomial & rhs) const;
            Polynomial & operator+=(const Polynomial & rhs);
            Polynomial operator-(const Polynomial & rhs) const;
            Polynomial & operator-=(const Polynomial & rhs);
            Polynomial operator*(const Polynomial & rhs) const;
            Polynomial & operator*=(const Polynomial & rhs);
            Polynomial operator*(const float & rhs) const;
            Polynomial & operator*=(const float rhs);
            Polynomial operator/(const float rhs) const;
            Polynomial & operator/=(const float rhs);

            bool operator==(const Polynomial & rhs) const;

            friend Polynomial operator*=(const float lhs, const Polynomial & rhs) {
                auto mult = rhs * lhs;
                return mult;
            }

            Polynomial Power(const size_t n) const;

            const Coefficients & GetCoefficients(void) const;
            size_t GetPolynomialOrder(void) const;

            bool IsStable(void) const;

            std::string ToString(void) const;

        private:
            void ReduceUnnecessaryElements(void);
            Eigen::MatrixXf GetSubHurwitzianMatrixWithOrder(const size_t order) const;

            Coefficients coefficients_;
    };
}   //  namespace tf_core

#endif // COMPONENTS_POLYNOMIAL_INCLUDE_POLYNOMIAL_HPP_
