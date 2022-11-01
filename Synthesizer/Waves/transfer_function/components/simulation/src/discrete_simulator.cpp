/* Copyright (C) 2021 Maciej Rozewicz - All Rights Reserved
 * You may use, distribute and modify this code under the
 * terms of the XYZ license, which unfortunately won't be
 * written for another century.
 *
 * You should have received a copy of the XYZ license with
 * this file. If not, please write to: , or visit :
 */

#include "discrete_simulator.hpp"

#include <algorithm>
#include <numeric>

namespace tf_core
{
    Signal DiscreteSimulator::Simulate(const CoreTransferFunction & tf, const Signal & input_signal) {
        auto input_history_size = tf.GetNum().GetPolynomialOrder();
        auto output_history_size = tf.GetDen().GetPolynomialOrder() - 1u;

        Signal input_history(input_history_size, 0.0f);
        Signal output_history(output_history_size, 0.0f);

        Signal output_signal(input_signal.size(), 0.0f);

        std::transform(input_signal.begin(), input_signal.end(), output_signal.begin(),
            [&](float input) {
                std::rotate(input_history.rbegin(), input_history.rbegin() + 1u, input_history.rend());
                *input_history.rbegin() = input;

                auto index = 0u;
                auto input_sum = std::accumulate(input_history.begin(), input_history.end(), 0.0f,
                    [=, &index] (float result, float input) {
                        return result + input * tf.GetNum().GetCoefficients().at(index++);
                    }
                );

                index = 0u;
                auto output_sum = std::accumulate(output_history.begin(), output_history.end(), 0.0f,
                    [=, &index] (float result, float output) {
                        return result + output * tf.GetDen().GetCoefficients().at(index++);
                    }
                );

                auto output = (input_sum - output_sum) / *(tf.GetDen().GetCoefficients().end() - 1u);

                std::rotate(output_history.rbegin(), output_history.rbegin() + 1u, output_history.rend());
                *output_history.rbegin() = output;

                return output;
            }
        );

        return output_signal;
    }
}   //  namespace tf_core
