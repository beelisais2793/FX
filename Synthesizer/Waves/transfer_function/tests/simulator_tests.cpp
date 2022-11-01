#include "gtest/gtest.h"

#include <memory>

#include "discrete_simulator.hpp"
#include "transfer_function_discretizer.hpp"

/****************** Polynomial Fraction ******************/
class DiscreteSimulatorTests : public ::testing::Test
{
    protected:
        void SetUp(void) override
        {}

        void TearDown(void) override
        {}
};

TEST_F(DiscreteSimulatorTests, TestFirstOrderStepResponse) {
    auto num = tf_core::Polynomial({1.0f});
    auto den = tf_core::Polynomial({1.0f, 2.0f});
    auto tf = tf_core::CoreTransferFunction(num, den);

    auto discrete_tf = tf_core::TransferFunctionDiscretizer::Discretize(tf, 1.0f, tf_core::DiscretizationMethod::Tustin);

    const auto vector_size = 24u;
    std::vector<float> input_signal(vector_size, 1.0f);
    auto output_signal = tf_core::DiscreteSimulator::Simulate(discrete_tf, input_signal);

    std::vector<float> expected_output {
        0.200000000000000f,
        0.520000000000000f,
        0.712000000000000f,
        0.827200000000000f,
        0.896320000000000f,
        0.937792000000000f,
        0.962675200000000f,
        0.977605120000000f,
        0.986563072000000f,
        0.991937843200000f,
        0.995162705920000f,
        0.997097623552000f,
        0.998258574131200f,
        0.998955144478720f,
        0.999373086687232f,
        0.999623852012339f,
        0.999774311207403f,
        0.999864586724442f,
        0.999918752034665f,
        0.999951251220799f,
        0.999970750732480f,
        0.999982450439488f,
        0.999989470263693f,
        0.999993682158216f
    };

    for (auto idx = 0u; idx < vector_size; idx++)
        EXPECT_FLOAT_EQ(expected_output.at(idx), output_signal.at(idx));
}

