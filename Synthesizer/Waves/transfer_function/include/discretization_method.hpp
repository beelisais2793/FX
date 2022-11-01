/* Copyright (C) 2021 Maciej Rozewicz - All Rights Reserved
 * You may use, distribute and modify this code under the
 * terms of the XYZ license, which unfortunately won't be
 * written for another century.
 *
 * You should have received a copy of the XYZ license with
 * this file. If not, please write to: , or visit :
 */

#ifndef INCLUDE_DISCRETIZATION_METHOD_HPP_
#define INCLUDE_DISCRETIZATION_METHOD_HPP_

namespace tf_core
{
    enum class DiscretizationMethod
    {
        Forward = 0u,
        Backward = 1u,
        Tustin = 2u
    };
}   //  namespace tf_core

#endif // INCLUDE_DISCRETIZATION_METHOD_HPP_
