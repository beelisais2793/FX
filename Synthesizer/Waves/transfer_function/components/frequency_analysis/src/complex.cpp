/* Copyright (C) 2021 Maciej Rozewicz - All Rights Reserved
 * You may use, distribute and modify this code under the
 * terms of the XYZ license, which unfortunately won't be
 * written for another century.
 *
 * You should have received a copy of the XYZ license with
 * this file. If not, please write to: , or visit :
 */

#include "complex.hpp"

#include <cmath>

namespace tf_core
{
    Complex::Complex(const float real, const float img)
        : real_{real}
        , img_{img} {
    }

    Complex Complex::operator+(const Complex & rhs) const {
        return Complex(this->Real() + rhs.Real(), this->Img() + rhs.Img());
    }

    Complex & Complex::operator+=(const Complex & rhs) {
        *this = (*this) + rhs;
        return *this;
    }

    Complex Complex::operator-(const Complex & rhs) const {
        return Complex(this->Real() - rhs.Real(), this->Img() - rhs.Img());
    }

    Complex & Complex::operator-=(const Complex & rhs) {
        *this = (*this) - rhs;
        return *this;
    }

    Complex Complex::operator*(const Complex & rhs) const {
        auto new_real = this->Real() * rhs.Real() - this->Img() * rhs.Img();
        auto new_img = this->Real() * rhs.Img() + this->Img() * rhs.Real();
        return Complex(new_real, new_img);
    }

    Complex & Complex::operator*=(const Complex & rhs) {
        *this = (*this) * rhs;
        return *this;
    }

    Complex Complex::operator*(const float rhs) const {
        return Complex(this->Real() * rhs, this->Img() * rhs);
    }

    Complex & Complex::operator*=(const float rhs) {
        *this = (*this) * rhs;
        return *this;
    }

    Complex Complex::operator/(const Complex & rhs) const {
        return ((*this) * rhs.Conjugate()) / std::pow(rhs.Abs(), 2.0f);
    }

    Complex & Complex::operator/=(const Complex & rhs) {
        *this = (*this) / rhs;
        return *this;
    }

    Complex Complex::operator/(const float rhs) const {
        return Complex(this->Real() / rhs, this->Img() / rhs);
    }

    Complex & Complex::operator/=(const float rhs) {
        *this = (*this) / rhs;
        return *this;
    }

    Complex Complex::Power(const int power) const {
        auto output = Complex(1.0f, 0.0f);
        for (auto idx = 0; idx < power; idx++)
            output *= (*this);
        
        return output;
    }

    Complex Complex::operator^(const int power) const {
        return this->Power(power);
    }

    bool Complex::operator==(const Complex & rhs) const {
        return ((this->Real() == rhs.Real()) && (this->Img() == rhs.Img()));
    }

    bool Complex::operator!=(const Complex & rhs) const {
        return !((*this) == rhs);
    }

    Complex Complex::Conjugate(void) const {
        return Complex(Real(), -Img());
    }

    float Complex::Real(void) const {
        return real_;
    }

    float Complex::Img(void) const {
        return img_;
    }

    float Complex::Abs(void) const {
        return std::hypot(Real(), Img());
    }

    float Complex::Phase(void) const {
        return std::atan2(Img(), Real());
    }
}   //  namespace tf_core
