/* Copyright (C) 2021 Maciej Rozewicz - All Rights Reserved
 * You may use, distribute and modify this code under the
 * terms of the XYZ license, which unfortunately won't be
 * written for another century.
 *
 * You should have received a copy of the XYZ license with
 * this file. If not, please write to: , or visit :
 */

#include "complex_transfer_function.hpp"

#include <algorithm>
#include <numeric>

namespace tf_core
{
    ComplexTransferFunction::ComplexTransferFunction(const CoreTransferFunction & tf)
        : tf_{tf} {
    }

    Characteristic ComplexTransferFunction::CalculateBode(const Frequencies & omega) const {
        auto complex_characteristic = CalculateCharacteristics(omega);

        Characteristic bode_characteristic(omega.size());
        std::transform(complex_characteristic.begin(), complex_characteristic.end(), bode_characteristic.begin(),
            [](const Complex v) {
                return std::pair<float, float>(v.Abs(), v.Phase());
            }
        );

        return bode_characteristic;
    }

    Characteristic ComplexTransferFunction::CalculateNyquist(const Frequencies & omega) const {
        auto complex_characteristic = CalculateCharacteristics(omega);

        Characteristic nyquist_characteristic(omega.size());
        std::transform(complex_characteristic.begin(), complex_characteristic.end(), nyquist_characteristic.begin(),
            [](const Complex v) {
                return std::pair<float, float>(v.Real(), v.Img());
            }
        );

        return nyquist_characteristic;
    }

    ComplexCharacteristic ComplexTransferFunction::CalculateCharacteristics(const Frequencies & omega) const {
        ComplexCharacteristic characteristic(omega.size());

        std::transform(omega.begin(), omega.end(), characteristic.begin(),
            [this](const float w) {
                return CalculateValue(w);
            }
        );

        return characteristic;
    }

    Complex  ComplexTransferFunction::CalculateValue(const float w) const {
        const auto omega = Complex(0.0f, w);

        auto init_num = Complex(0.0f, 0.0f);
        int pow_num = 0;
        auto num = std::accumulate(tf_.GetNum().GetCoefficients().begin(), tf_.GetNum().GetCoefficients().end(), init_num,
            [=,&pow_num](Complex sum, const float coefficient) {
                auto w_power = omega^pow_num;
                pow_num++;

                return (sum + w_power * coefficient);
            }
        );

        auto init_den = Complex(0.0f, 0.0f);
        int pow_den = 0;
        auto den = std::accumulate(tf_.GetDen().GetCoefficients().begin(), tf_.GetDen().GetCoefficients().end(), init_den,
            [=,&pow_den](Complex sum, const float coefficient) {
                auto w_power = omega^pow_den;
                pow_den++;

                return (sum + w_power * coefficient);
            }
        );

        return (num / den);
    }
}   //  namespace tf_core
