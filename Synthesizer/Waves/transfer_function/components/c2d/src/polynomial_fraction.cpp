/* Copyright (C) 2021 Maciej Rozewicz - All Rights Reserved
 * You may use, distribute and modify this code under the
 * terms of the XYZ license, which unfortunately won't be
 * written for another century.
 *
 * You should have received a copy of the XYZ license with
 * this file. If not, please write to: , or visit :
 */

#include "polynomial_fraction.hpp"

namespace tf_core
{
    PolynomialFraction::PolynomialFraction(const Polynomial & num, const Polynomial & den)
        : num_{num}
        , den_{den} {
    }

    PolynomialFraction PolynomialFraction::operator+(const PolynomialFraction & rhs) const {
        auto new_den = this->den_ * rhs.den_;
        auto new_num = this->num_ * rhs.den_ + this->den_ * rhs.num_;

        return PolynomialFraction(new_num, new_den);
    }

    PolynomialFraction & PolynomialFraction::operator+=(const PolynomialFraction & rhs) {
        auto sum = (*this) + rhs;
        
        this->num_ = sum.GetNum();
        this->den_ = sum.GetDen();

        return (*this);
    }

    PolynomialFraction PolynomialFraction::operator-(const PolynomialFraction & rhs) const {
        auto new_den = this->den_ * rhs.den_;
        auto new_num = this->num_ * rhs.den_ - this->den_ * rhs.num_;

        return PolynomialFraction(new_num, new_den);
    }

    PolynomialFraction & PolynomialFraction::operator-=(const PolynomialFraction & rhs) {
        auto diff = (*this) - rhs;

        this->num_ = diff.GetNum();
        this->den_ = diff.GetDen();

        return (*this);
    }

    PolynomialFraction PolynomialFraction::operator*(const float & rhs) {
        return PolynomialFraction(num_ * rhs, den_);
    }

    bool PolynomialFraction::operator==(const PolynomialFraction & rhs) const {
        return (this->num_ == rhs.num_) && (this->den_ == rhs.den_);
    }
    bool PolynomialFraction::operator!=(const PolynomialFraction & rhs) const {
        return !(*this == rhs);
    }

    PolynomialFraction PolynomialFraction::Power(const size_t n) {
        return PolynomialFraction(num_.Power(n), den_.Power(n));
    }

    const Polynomial & PolynomialFraction::GetNum(void) const {
        return num_;
    }

    const Polynomial & PolynomialFraction::GetDen(void) const {
        return den_;
    }
}   // namespace tf_core
