/* Copyright (C) 2021 Maciej Rozewicz - All Rights Reserved
 * You may use, distribute and modify this code under the
 * terms of the XYZ license, which unfortunately won't be
 * written for another century.
 *
 * You should have received a copy of the XYZ license with
 * this file. If not, please write to: , or visit :
 */

#include "polynomial.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace tf_core
{
    Polynomial::Polynomial(void)
        : coefficients_{{0.0f}} {
    }

    Polynomial::Polynomial(const Polynomial::Coefficients & coefficients)
        : coefficients_{coefficients} {
        ReduceUnnecessaryElements();
    }

    Polynomial Polynomial::operator+(const Polynomial & rhs) const {
        auto output_size = std::max(this->GetPolynomialOrder(), rhs.GetPolynomialOrder());
        Coefficients new_coefficients(output_size);
        std::copy(this->coefficients_.begin(), this->coefficients_.end(), new_coefficients.begin());

        for (auto idx = 0u; idx < rhs.GetPolynomialOrder(); idx++)
            new_coefficients.at(idx) += rhs.coefficients_.at(idx);

        return Polynomial(new_coefficients);
    }

    Polynomial & Polynomial::operator+=(const Polynomial & rhs) {
        auto sum = (*this) + rhs;
        this->coefficients_ = sum.GetCoefficients();
        ReduceUnnecessaryElements();
        return *this;
    }

    Polynomial Polynomial::operator-(const Polynomial & rhs) const {
        auto output_size = std::max(this->GetPolynomialOrder(), rhs.GetPolynomialOrder());
        Coefficients new_coefficients(output_size);
        std::copy(this->coefficients_.begin(), this->coefficients_.end(), new_coefficients.begin());

        for (auto idx = 0u; idx < rhs.GetPolynomialOrder(); idx++)
            new_coefficients.at(idx) -= rhs.coefficients_.at(idx);

        return Polynomial(new_coefficients);
    }

    Polynomial & Polynomial::operator-=(const Polynomial & rhs) {
        auto sum = (*this) - rhs;
        this->coefficients_ = sum.GetCoefficients();
        ReduceUnnecessaryElements();
        return *this;
    }

    Polynomial Polynomial::operator*(const Polynomial & rhs) const {
        auto new_polynomial_size = this->GetPolynomialOrder() + rhs.GetPolynomialOrder() - 1u;
        Coefficients new_coeficients(new_polynomial_size, 0.0f);

        for (auto a_idx = 0u; a_idx < this->GetPolynomialOrder(); a_idx++) {
            for (auto b_idx = 0u; b_idx < rhs.GetPolynomialOrder(); b_idx++) {
                new_coeficients.at(a_idx + b_idx) += this->coefficients_.at(a_idx) * rhs.coefficients_.at(b_idx);
            }
        }

        return Polynomial(new_coeficients);
    }

    Polynomial & Polynomial::operator*=(const Polynomial & rhs) {
        auto mult = (*this) * rhs;
        this->coefficients_ = mult.coefficients_;
        ReduceUnnecessaryElements();
        return *this;
    }

    Polynomial Polynomial::operator*(const float & rhs) const {
        Coefficients new_coeficients = this->coefficients_;
        std::transform(new_coeficients.begin(), new_coeficients.end(), new_coeficients.begin(),
            [=](auto coefficient) {
                return coefficient * rhs;
            }
        );

        return Polynomial(new_coeficients);
    }

    Polynomial & Polynomial::operator*=(const float rhs) {
        auto mult = (*this) * rhs;
        this->coefficients_ = mult.coefficients_;
        ReduceUnnecessaryElements();

        return (*this);
    }

    Polynomial Polynomial::operator/(const float rhs) const {
        if (std::abs(rhs) < 1e-9f)
            throw std::invalid_argument("Polynomial::operator/(const float & rhs) ; division by something close to 0.0f");
        
        Coefficients new_coeficients = this->coefficients_;
        std::transform(new_coeficients.begin(), new_coeficients.end(), new_coeficients.begin(),
            [=](auto coefficient) {
                return coefficient / rhs;
            }
        );

        return Polynomial(new_coeficients);
    }

    Polynomial & Polynomial::operator/=(const float rhs) {
        auto div = (*this) / rhs;
        this->coefficients_ = div.coefficients_;
        ReduceUnnecessaryElements();

        return (*this);
    }

    bool Polynomial::operator==(const Polynomial & rhs) const {
        return this->coefficients_ == rhs.coefficients_;
    }

    Polynomial Polynomial::Power(const size_t n) const {
        auto output = Polynomial({1.0f});

        for (auto i = 0u; i < n; i++)
            output *= (*this);

        return output;
    }

    const Polynomial::Coefficients & Polynomial::GetCoefficients(void) const {
        return coefficients_;
    }

    size_t Polynomial::GetPolynomialOrder(void) const {
        return coefficients_.size();
    }

    void Polynomial::ReduceUnnecessaryElements(void) {
        auto unnecessery_elements_number = 0u;

        for (auto it = coefficients_.rbegin(); it != coefficients_.rend(); it++) {
            if (*it == 0.0f)
                unnecessery_elements_number++;
            else
                break;
        }

        if ((unnecessery_elements_number > 0u) && (unnecessery_elements_number < coefficients_.size()))
        {
            coefficients_.resize(coefficients_.size() - unnecessery_elements_number);
        }
        else if (unnecessery_elements_number == coefficients_.size())
        {
            coefficients_ = {0.0f};
        }
        else
        {
            // Do nothing
        }
    }

    bool Polynomial::IsStable(void) const {
        auto first_negative = std::find_if(coefficients_.begin(), coefficients_.end(), [](float coefficient){ return coefficient <= 0.0f; });
        if (first_negative != coefficients_.end()) {
            return false;
        } else {
            for (auto index = 0u; index < GetPolynomialOrder(); index++) {
                auto sub_hurwitz = GetSubHurwitzianMatrixWithOrder(index);
                auto determinant = sub_hurwitz.determinant();
                if (determinant <= 0.0f)
                    return false;
            }
            return true;
        }
    }

    Eigen::MatrixXf Polynomial::GetSubHurwitzianMatrixWithOrder(const size_t order) const {
        auto sub_hurwitz = Eigen::MatrixXf(order, order);
        
        auto get_element = [this](int n) { return ((n >= 0) && (n < static_cast<int>(GetPolynomialOrder()))) ? coefficients_.at(n) : 0.0f; };

        for (auto row = 0; row < static_cast<int>(order); row++) {
            auto index = static_cast<int>(GetPolynomialOrder()) - 1 - (2 * row);
            for (auto col = 0; col < static_cast<int>(order); col++) {
                sub_hurwitz(col, row) = get_element(index + col);
            }
        }

        return sub_hurwitz;
    }

    std::string Polynomial::ToString(void) const {
        std::string output = "";
        auto idx = 0u;
        std::for_each(coefficients_.rbegin(), coefficients_.rend(),
            [&](float coeficient) {
                auto sign = (coeficient < 0.0f) ? std::string(" - ") : std::string(" + ");
                auto abs_coefficient = std::to_string(std::abs(coeficient));
                auto power = coefficients_.size() - 1u - idx;

                if (idx != 0u)
                {
                    output.append(sign);
                    output.append(abs_coefficient);
                }
                else
                {
                    output.append(std::to_string(coeficient));
                }                

                switch (power)
                {
                    case 0u:
                    {
                        break;
                    }
                    case 1:
                    {
                        output.append(std::string(" s"));
                        break;
                    }
                    default:
                    {
                        output.append(std::string(" s^")).append(std::to_string(power));
                    }
                }
                idx++;
            }
        );

        return output;
    }
}  // namespace tf_core
