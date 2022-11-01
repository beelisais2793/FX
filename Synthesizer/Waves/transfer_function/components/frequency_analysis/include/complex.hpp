/* Copyright (C) 2021 Maciej Rozewicz - All Rights Reserved
 * You may use, distribute and modify this code under the
 * terms of the XYZ license, which unfortunately won't be
 * written for another century.
 *
 * You should have received a copy of the XYZ license with
 * this file. If not, please write to: , or visit :
 */

#ifndef COMPONENTS_FREQUENCY_ANALYSIS_INCLUDE_COMPLEX_HPP_
#define COMPONENTS_FREQUENCY_ANALYSIS_INCLUDE_COMPLEX_HPP_

namespace tf_core
{
    class Complex
    {
        public:
            Complex(void) = default;
            Complex(const Complex & obj) = default;
            //Complex(const Complex && obj) = default;
            ~Complex(void) = default;
            Complex(const float real, const float img);

            //Complex operator=(const Complex & rhs) = default;

            Complex operator+(const Complex & rhs) const;
            Complex & operator+=(const Complex & rhs);
            Complex operator-(const Complex & rhs) const;
            Complex & operator-=(const Complex & rhs);
            Complex operator*(const Complex & rhs) const;
            Complex & operator*=(const Complex & rhs);
            Complex operator*(const float rhs) const;
            Complex & operator*=(const float rhs);
            Complex operator/(const Complex & rhs) const;
            Complex & operator/=(const Complex & rhs);
            Complex operator/(const float rhs) const;
            Complex & operator/=(const float rhs);

            Complex Power(const int power) const;
            Complex operator^(const int power) const;

            bool operator==(const Complex & rhs) const;
            bool operator!=(const Complex & rhs) const;

            Complex Conjugate(void) const;

            float Real(void) const;
            float Img(void) const;

            float Abs(void) const;
            float Phase(void) const;

        private:
            float real_ = 0.0f;
            float img_ = 0.0f;
    };
}   //  namespace tf_core

#endif // COMPONENTS_FREQUENCY_ANALYSIS_INCLUDE_COMPLEX_HPP_
