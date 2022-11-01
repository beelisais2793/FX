/* Copyright (C) 2021 Maciej Rozewicz - All Rights Reserved
 * You may use, distribute and modify this code under the
 * terms of the XYZ license, which unfortunately won't be
 * written for another century.
 *
 * You should have received a copy of the XYZ license with
 * this file. If not, please write to: , or visit :
 */

#ifndef COMPONENTS_FREQUENCY_ANALYSIS_INCLUDE_COMPLEX_TRANSFER_FUNCTION_HPP_
#define COMPONENTS_FREQUENCY_ANALYSIS_INCLUDE_COMPLEX_TRANSFER_FUNCTION_HPP_

#include <vector>

#include "core_transfer_function.hpp"
#include "complex.hpp"

namespace tf_core
{
    using Frequencies = std::vector<float>;
    using Characteristic = std::vector<std::pair<float, float>>;
    using ComplexCharacteristic = std::vector<Complex>;

    class ComplexTransferFunction
    {
        public:
            explicit ComplexTransferFunction(const CoreTransferFunction & tf);
            ComplexTransferFunction(void) = delete;
            ~ComplexTransferFunction(void) = default;

            Characteristic CalculateBode(const Frequencies & omega) const;
            Characteristic CalculateNyquist(const Frequencies & omega) const;

        private:
            ComplexCharacteristic CalculateCharacteristics(const Frequencies & omega) const;
            Complex CalculateValue(const float w) const;

            CoreTransferFunction tf_;
    };
}   //  namespace tf_core

#endif // COMPONENTS_FREQUENCY_ANALYSIS_INCLUDE_COMPLEX_TRANSFER_FUNCTION_HPP_
