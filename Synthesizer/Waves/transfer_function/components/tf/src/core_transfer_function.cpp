/* Copyright (C) 2021 Maciej Rozewicz - All Rights Reserved
 * You may use, distribute and modify this code under the
 * terms of the XYZ license, which unfortunately won't be
 * written for another century.
 *
 * You should have received a copy of the XYZ license with
 * this file. If not, please write to: , or visit :
 */

#include "core_transfer_function.hpp"

namespace tf_core
{
    CoreTransferFunction::CoreTransferFunction(const Polynomial & num, const Polynomial & den)
        : num_{num}
        , den_{den} {
    }

    CoreTransferFunction CoreTransferFunction::Series(const CoreTransferFunction & rhs) const {
        auto new_num = this->num_ * rhs.num_;
        auto new_den = this->den_ * rhs.den_;

        return CoreTransferFunction(new_num, new_den);
    }

    CoreTransferFunction CoreTransferFunction::Parallel(const CoreTransferFunction & rhs) const {
        auto new_num = this->num_ * rhs.den_ + rhs.num_ * this->den_;
        auto new_den = this->den_ * rhs.den_;
        
        return CoreTransferFunction(new_num, new_den);
    }

    CoreTransferFunction CoreTransferFunction::Feedback(const CoreTransferFunction & feedback_branch, bool positive) const {
        auto feedback_sign = positive ? -1.0f : 1.0f;

        auto new_num = this->num_ * feedback_branch.den_;
        auto new_den = this->den_ * feedback_branch.den_ + this->num_ * feedback_branch.num_ * feedback_sign;

        return CoreTransferFunction(new_num, new_den);
    }

    CoreTransferFunction CoreTransferFunction::operator+(const CoreTransferFunction & rhs) const {
        return this->Parallel(rhs);
    }

    CoreTransferFunction CoreTransferFunction::operator*(const CoreTransferFunction & rhs) const {
        return this->Series(rhs);
    }

    bool CoreTransferFunction::operator==(const CoreTransferFunction & rhs) const {
        return (this->num_ == rhs.num_) && (this->den_ == rhs.den_);
    }

    bool CoreTransferFunction::operator!=(const CoreTransferFunction & rhs) const {
        return !((*this) == rhs);
    }

    const Polynomial & CoreTransferFunction::GetNum(void) const {
        return num_;
    }

    const Polynomial & CoreTransferFunction::GetDen(void) const {
        return den_;
    }
}   // namespace tf_core
