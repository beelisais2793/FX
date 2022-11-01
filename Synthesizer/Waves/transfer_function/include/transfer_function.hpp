/* Copyright (C) 2021 Maciej Rozewicz - All Rights Reserved
 * You may use, distribute and modify this code under the
 * terms of the XYZ license, which unfortunately won't be
 * written for another century.
 *
 * You should have received a copy of the XYZ license with
 * this file. If not, please write to: , or visit :
 */

#ifndef INCLUDE_TRANSFER_FUNCTION_HPP_
#define INCLUDE_TRANSFER_FUNCTION_HPP_

#include <memory>
#include <vector>
#include <string>

#include "discretization_method.hpp"

namespace tf_core
{
    class ComplexTransferFunction;
    class CoreTransferFunction;
    class Polynomial;

    using Signal = std::vector<float>;
    using FrequencyCharacteristic = std::vector<std::pair<float, float>>;

    class TransferFunction
    {
        public:
            using CoefficientsVector = std::vector<float>;

            TransferFunction(const CoefficientsVector & num, const CoefficientsVector & den);
            TransferFunction(void) = delete;
            ~TransferFunction(void);

            TransferFunction Series(const TransferFunction & rhs) const;
            TransferFunction Parallel(const TransferFunction & rhs) const;
            TransferFunction Feedback(const TransferFunction & feedback_branch, bool positive = false) const;

            TransferFunction operator+(const TransferFunction & rhs) const;
            TransferFunction operator*(const TransferFunction & rhs) const;

            bool operator==(const TransferFunction & rhs) const;
            bool operator!=(const TransferFunction & rhs) const;

            const CoefficientsVector & GetNum(void) const;
            const CoefficientsVector & GetDen(void) const;

            TransferFunction Discretize(const float discretization_time,
                const DiscretizationMethod discretization_method = DiscretizationMethod::Tustin) const;
            
            Signal SimulateDiscrete(const Signal & input_signal, const float sampling_time) const;
            Signal Step(float simulation_time) const;
            Signal Impulse(float simulation_time) const;

            bool IsStable(void) const;

            FrequencyCharacteristic Bode(const std::vector<float> & omeha) const;
            FrequencyCharacteristic Nyquist(const std::vector<float> & omeha) const;

            std::string ToString(void) const;

        private:
            explicit TransferFunction(const CoreTransferFunction & core_tf);

            std::unique_ptr<Polynomial> num_;
            std::unique_ptr<Polynomial> den_;
            std::unique_ptr<CoreTransferFunction> tf_;
            std::unique_ptr<ComplexTransferFunction> ctf_;
    };
}   //  namespace tf_core

#endif // INCLUDE_TRANSFER_FUNCTION_HPP_
