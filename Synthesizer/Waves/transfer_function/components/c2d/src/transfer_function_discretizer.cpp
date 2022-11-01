/* Copyright (C) 2021 Maciej Rozewicz - All Rights Reserved
 * You may use, distribute and modify this code under the
 * terms of the XYZ license, which unfortunately won't be
 * written for another century.
 *
 * You should have received a copy of the XYZ license with
 * this file. If not, please write to: , or visit :
 */

#include "transfer_function_discretizer.hpp"

namespace tf_core
{
    CoreTransferFunction TransferFunctionDiscretizer::Discretize(const CoreTransferFunction & tf,
        const float discretization_time,
        const DiscretizationMethod discretization_method) {
        auto [num_num, num_den] = tf_core::PolynomialDiscretizer::Discretize(tf.GetNum(), discretization_time, discretization_method);
        auto [den_num, den_den] = tf_core::PolynomialDiscretizer::Discretize(tf.GetDen(), discretization_time, discretization_method);

        auto num = CoreTransferFunction(num_num, num_den);
        auto inverted_den = CoreTransferFunction(den_den, den_num);

        auto result = num * inverted_den;

        return CoreTransferFunction(result);
    }

}   //  namespace tf_core
