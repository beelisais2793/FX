/* Copyright (C) 2021 Maciej Rozewicz - All Rights Reserved
 * You may use, distribute and modify this code under the
 * terms of the XYZ license, which unfortunately won't be
 * written for another century.
 *
 * You should have received a copy of the XYZ license with
 * this file. If not, please write to: , or visit :
 */

#ifndef COMPONENTS_C2D_INCLUDE_TRANSFER_FUNCTION_DISCRETIZER_HPP_
#define COMPONENTS_C2D_INCLUDE_TRANSFER_FUNCTION_DISCRETIZER_HPP_

#include <tuple>
#include <vector>

#include "discretization_method.hpp"
#include "polynomial.hpp"
#include "polynomial_discretizer.hpp"
#include "polynomial_fraction.hpp"
#include "core_transfer_function.hpp"

namespace tf_core
{
    class TransferFunctionDiscretizer
    {
        public:
            TransferFunctionDiscretizer() = delete;
            TransferFunctionDiscretizer(const TransferFunctionDiscretizer & obj) = delete;
            TransferFunctionDiscretizer(const TransferFunctionDiscretizer && obj) = delete;
            ~TransferFunctionDiscretizer(void) = default;

            static CoreTransferFunction Discretize(const CoreTransferFunction & tf,
                const float discretization_time,
                const DiscretizationMethod discretization_method);
    };
}   //  namespace tf_core

#endif // COMPONENTS_C2D_INCLUDE_TRANSFER_FUNCTION_DISCRETIZER_HPP_
