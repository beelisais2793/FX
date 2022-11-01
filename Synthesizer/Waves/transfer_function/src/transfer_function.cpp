/* Copyright (C) 2021 Maciej Rozewicz - All Rights Reserved
 * You may use, distribute and modify this code under the
 * terms of the XYZ license, which unfortunately won't be
 * written for another century.
 *
 * You should have received a copy of the XYZ license with
 * this file. If not, please write to: , or visit :
 */

#include "transfer_function.hpp"

#include <tuple>

#include "complex_transfer_function.hpp"
#include "core_transfer_function.hpp"
#include "discrete_simulator.hpp"
#include "polynomial.hpp"
#include "transfer_function_discretizer.hpp"

namespace tf_core
{
    TransferFunction::TransferFunction(const std::vector<float> & num, const std::vector<float> & den)
        : num_{std::make_unique<Polynomial>(num)}
        , den_{std::make_unique<Polynomial>(den)}
        , tf_{std::make_unique<CoreTransferFunction>(*num_, *den_)}
        , ctf_{std::make_unique<ComplexTransferFunction>(*tf_)} {
        std::ignore = num;
        std::ignore = den;
    }

    TransferFunction::TransferFunction(const CoreTransferFunction & core_tf)
        : num_{std::make_unique<Polynomial>(core_tf.GetNum())}
        , den_{std::make_unique<Polynomial>(core_tf.GetDen())}
        , tf_{std::make_unique<CoreTransferFunction>(core_tf.GetNum(), core_tf.GetDen())}  { 
    }

    TransferFunction::~TransferFunction(void) = default;

    TransferFunction TransferFunction::Series(const TransferFunction & rhs) const {
        return TransferFunction((*tf_) * (*rhs.tf_));
    }

    TransferFunction TransferFunction::Parallel(const TransferFunction & rhs) const {
        return TransferFunction((*tf_) + (*rhs.tf_));
    }

    TransferFunction TransferFunction::Feedback(const TransferFunction & feedback_branch, bool positive) const {
        return TransferFunction(tf_->Feedback(*feedback_branch.tf_, positive));
    }

    TransferFunction TransferFunction::operator+(const TransferFunction & rhs) const {
        return TransferFunction((*tf_) + (*rhs.tf_));
    }

    TransferFunction TransferFunction::operator*(const TransferFunction & rhs) const {
        return TransferFunction((*tf_) * (*rhs.tf_));
    }

    bool TransferFunction::operator==(const TransferFunction & rhs) const {
        return (*tf_) == (*rhs.tf_);
    }

    bool TransferFunction::operator!=(const TransferFunction & rhs) const {
        return (*tf_) != (*rhs.tf_);
    }

    const TransferFunction::CoefficientsVector & TransferFunction::GetNum(void) const {
        return num_->GetCoefficients();
    }

    const TransferFunction::CoefficientsVector & TransferFunction::GetDen(void) const {
        return den_->GetCoefficients();
    }

    TransferFunction TransferFunction::Discretize(const float discretization_time, const DiscretizationMethod discretization_method) const {
        auto discrete_tf = tf_core::TransferFunctionDiscretizer::Discretize(*tf_, discretization_time, discretization_method);
        return TransferFunction(discrete_tf);
    }

    Signal TransferFunction::SimulateDiscrete(const Signal & input_signal, const float sampling_time) const {
        auto discrete_tf = Discretize(sampling_time);
        return DiscreteSimulator::Simulate(*discrete_tf.tf_, input_signal);
    }

    Signal TransferFunction::Step(float simulation_time) const {
        float characteristics_samples_number_ = 100.0f;

        auto sampling_time = simulation_time / 100.0f;
        auto discrete_tf = Discretize(sampling_time);
        
        auto input_signal = Signal(static_cast<size_t>(characteristics_samples_number_), 1.0f);

        return DiscreteSimulator::Simulate(*discrete_tf.tf_, input_signal);
    }

    Signal TransferFunction::Impulse(float simulation_time) const {
        float characteristics_samples_number_ = 100.0f;

        auto sampling_time = simulation_time / 100.0f;
        auto discrete_tf = Discretize(sampling_time);
        
        auto input_signal = Signal(static_cast<size_t>(characteristics_samples_number_), 0.0f);
        input_signal.at(0u) = 100.0f;
        
        return DiscreteSimulator::Simulate(*discrete_tf.tf_, input_signal);
    }

    FrequencyCharacteristic TransferFunction::Bode(const std::vector<float> & omega) const {
        return ctf_->CalculateBode(omega);
    }

    FrequencyCharacteristic TransferFunction::Nyquist(const std::vector<float> & omega) const {
        return ctf_->CalculateNyquist(omega);
    }

    bool TransferFunction::IsStable(void) const {
        return den_->IsStable();
    }

    std::string TransferFunction::ToString(void) const {
        auto num_string = num_->ToString();
        auto num_string_length = static_cast<int>(num_string.size());

        auto den_string = den_->ToString();
        auto den_string_length = static_cast<int>(den_string.size());
        
        auto length_difference = std::abs(den_string_length - num_string_length);
        auto offset_size = length_difference / 2u;
        auto line_size = std::max(num_string_length, den_string_length);

        std::string output("");
        auto offset = std::string(offset_size, ' ');
        auto line = std::string(line_size, '-');

        if (num_string_length < den_string_length) {
            output.append(offset + num_string + offset);
            output.append("\n");
            output.append(line);
            output.append("\n");
            output.append(den_string);
        } else {
            output.append(num_string);
            output.append("\n");
            output.append(line);
            output.append("\n");
            output.append(offset + den_string + offset);
        }

        return output;
    }
}   //  namespace tf_core
