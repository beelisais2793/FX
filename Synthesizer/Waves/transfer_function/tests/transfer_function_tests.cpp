#include "gtest/gtest.h"

#include <memory>
#include <vector>

#include "transfer_function.hpp"

class TransferFunctionTests : public ::testing::Test
{
    protected:
        void SetUp(void) override
        {}
};

TEST_F(TransferFunctionTests, TestConstructor) {
    std::vector<float> num = {1.0f};
    std::vector<float> den = {1.0f, 2.0f};
    EXPECT_NO_THROW(auto tf = tf_core::TransferFunction(num, den));
}

TEST_F(TransferFunctionTests, TestGetters) {
    tf_core::TransferFunction::CoefficientsVector num = {1.0f};
    tf_core::TransferFunction::CoefficientsVector den = {1.0f};
    auto tf = tf_core::TransferFunction(num, den);

    EXPECT_EQ(num, tf.GetNum());
    EXPECT_EQ(den, tf.GetDen());
}

TEST_F(TransferFunctionTests, TestEqualOperator) {
    tf_core::TransferFunction::CoefficientsVector num_a = {1.0f};
    tf_core::TransferFunction::CoefficientsVector den_a = {1.0f, 2.0f};
    auto a = tf_core::TransferFunction(num_a, den_a);

    tf_core::TransferFunction::CoefficientsVector num_b = {1.0f};
    tf_core::TransferFunction::CoefficientsVector den_b = {1.0f, 2.0f};
    auto b = tf_core::TransferFunction(num_b, den_b);

    EXPECT_TRUE(a == b);
}

TEST_F(TransferFunctionTests, TestNonEqualOperator) {
    tf_core::TransferFunction::CoefficientsVector num_a = {1.0f};
    tf_core::TransferFunction::CoefficientsVector den_a = {1.0f, 2.0f};
    auto a = tf_core::TransferFunction(num_a, den_a);

    tf_core::TransferFunction::CoefficientsVector num_b = {1.0f};
    tf_core::TransferFunction::CoefficientsVector den_b = {1.0f, 3.0f};
    auto b = tf_core::TransferFunction(num_b, den_b);

    EXPECT_TRUE(a != b);
}

TEST_F(TransferFunctionTests, TestParallelNested) {
    tf_core::TransferFunction::CoefficientsVector num_a = {1.0f};
    tf_core::TransferFunction::CoefficientsVector den_a = {1.0f, 2.0f};
    auto a = tf_core::TransferFunction(num_a, den_a);

    tf_core::TransferFunction::CoefficientsVector num_b = {1.0f};
    tf_core::TransferFunction::CoefficientsVector den_b = {1.0f, 3.0f};
    auto b = tf_core::TransferFunction(num_b, den_b);

    auto connection = a.Parallel(b).Parallel(b);

    tf_core::TransferFunction::CoefficientsVector expected_num = {3.0f, 16.0f, 21.0f};
    tf_core::TransferFunction::CoefficientsVector expected_den = {1.0f, 8.0f, 21.0f, 18.0f};
    auto expected_output = tf_core::TransferFunction(expected_num, expected_den);
    EXPECT_EQ(expected_output, connection);

    auto connection_operator = a + b + b;
    EXPECT_EQ(expected_output, connection_operator);
}

TEST_F(TransferFunctionTests, TestSeriesNested) {
    tf_core::TransferFunction::CoefficientsVector num_a = {1.0f};
    tf_core::TransferFunction::CoefficientsVector den_a = {1.0f, 2.0f};
    auto a = tf_core::TransferFunction(num_a, den_a);

    tf_core::TransferFunction::CoefficientsVector num_b = {1.0f};
    tf_core::TransferFunction::CoefficientsVector den_b = {1.0f, 3.0f};
    auto b = tf_core::TransferFunction(num_b, den_b);

    auto connection = a.Series(b).Series(b);

    tf_core::TransferFunction::CoefficientsVector expected_num = {1.0f};
    tf_core::TransferFunction::CoefficientsVector expected_den = {1.0f, 8.0f, 21.0f, 18.0f};
    auto expected_output = tf_core::TransferFunction(expected_num, expected_den);
    EXPECT_EQ(expected_output, connection);

    auto connection_operator = a * b * b;
    EXPECT_EQ(expected_output, connection_operator);
}

TEST_F(TransferFunctionTests, TestSeriesParallelNested) {
    tf_core::TransferFunction::CoefficientsVector num_a = {1.0f};
    tf_core::TransferFunction::CoefficientsVector den_a = {1.0f, 2.0f};
    auto a = tf_core::TransferFunction(num_a, den_a);

    tf_core::TransferFunction::CoefficientsVector num_b = {1.0f};
    tf_core::TransferFunction::CoefficientsVector den_b = {1.0f, 3.0f};
    auto b = tf_core::TransferFunction(num_b, den_b);

    auto connection = a.Series(b).Parallel(b);

    tf_core::TransferFunction::CoefficientsVector expected_num = {2.0f, 8.0f, 6.0f};
    tf_core::TransferFunction::CoefficientsVector expected_den = {1.0f, 8.0f, 21.0f, 18.0f};
    auto expected_output = tf_core::TransferFunction(expected_num, expected_den);
    EXPECT_EQ(expected_output, connection);

    auto connection_operator = a * b + b;
    EXPECT_EQ(expected_output, connection_operator);
}

TEST_F(TransferFunctionTests, TestFeedbackNegative) {
    tf_core::TransferFunction::CoefficientsVector num_a = {1.0f};
    tf_core::TransferFunction::CoefficientsVector den_a = {1.0f, 2.0f};
    auto a = tf_core::TransferFunction(num_a, den_a);

    tf_core::TransferFunction::CoefficientsVector num_b = {1.0f};
    tf_core::TransferFunction::CoefficientsVector den_b = {1.0f, 3.0f};
    auto b = tf_core::TransferFunction(num_b, den_b);

    auto parallel_connection = a.Feedback(b);

    tf_core::TransferFunction::CoefficientsVector expected_num = {1.0f, 3.0f};
    tf_core::TransferFunction::CoefficientsVector expected_den = {2.0f, 5.0f, 6.0f};
    auto expected_output = tf_core::TransferFunction(expected_num, expected_den);

    EXPECT_EQ(expected_output, parallel_connection);
}

TEST_F(TransferFunctionTests, TestDiscretization) {
    tf_core::TransferFunction::CoefficientsVector num = {1.0f};
    tf_core::TransferFunction::CoefficientsVector den = {1.0f, 2.0f};
    auto tf = tf_core::TransferFunction(num, den);

    auto discretization_time = 1.0f;

    // Forward
    auto tf_discrete_forward = tf.Discretize(discretization_time, tf_core::DiscretizationMethod::Forward);
    tf_core::TransferFunction::CoefficientsVector expected_forward_num = {1.0f};
    tf_core::TransferFunction::CoefficientsVector expected_forward_den = {-1.0f, 2.0f};
    EXPECT_EQ(tf_discrete_forward.GetNum(), expected_forward_num);
    EXPECT_EQ(tf_discrete_forward.GetDen(), expected_forward_den);

    // Backward
    auto tf_discrete_backward = tf.Discretize(discretization_time, tf_core::DiscretizationMethod::Backward);
    tf_core::TransferFunction::CoefficientsVector expected_backward_num = {0.0f, 1.0f};
    tf_core::TransferFunction::CoefficientsVector expected_backward_den = {-2.0f, 3.0f};
    EXPECT_EQ(tf_discrete_backward.GetNum(), expected_backward_num);
    EXPECT_EQ(tf_discrete_backward.GetDen(), expected_backward_den);

    // Tustin
    auto tf_discrete_tustin = tf.Discretize(discretization_time, tf_core::DiscretizationMethod::Tustin);
    tf_core::TransferFunction::CoefficientsVector expected_tustin_num = {1.0f, 1.0f};
    tf_core::TransferFunction::CoefficientsVector expected_tustin_den = {-3.0f, 5.0f};
    EXPECT_EQ(tf_discrete_tustin.GetNum(), expected_tustin_num);
    EXPECT_EQ(tf_discrete_tustin.GetDen(), expected_tustin_den);
}

TEST_F(TransferFunctionTests, TestFirstOrderStepResponse) {
    tf_core::TransferFunction::CoefficientsVector num = {1.0f};
    tf_core::TransferFunction::CoefficientsVector den = {1.0f, 2.0f};
    auto tf = tf_core::TransferFunction(num, den);

    const auto vector_size = 24u;
    std::vector<float> input_signal(vector_size, 1.0f);
    auto output_signal = tf.SimulateDiscrete(input_signal, 1.0f);

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

TEST_F(TransferFunctionTests, TestToString) {
    tf_core::TransferFunction::CoefficientsVector num = {1.0f, 1.0f};
    tf_core::TransferFunction::CoefficientsVector den = {1.0f, 2.0f, 3.0f};
    auto tf = tf_core::TransferFunction(num, den);

    auto result = tf.ToString();

    auto expected = std::string("       1.000000 s + 1.000000       \n------------------------------------\n3.000000 s^2 + 2.000000 s + 1.000000");

    EXPECT_EQ(result, expected);
}

TEST_F(TransferFunctionTests, TestToString2) {
    tf_core::TransferFunction::CoefficientsVector num = {1.0f, 2.0f, 3.0f};
    tf_core::TransferFunction::CoefficientsVector den = {1.0f, 1.0f};
    auto tf = tf_core::TransferFunction(num, den);

    auto result = tf.ToString();

    auto expected = std::string("3.000000 s^2 + 2.000000 s + 1.000000\n------------------------------------\n       1.000000 s + 1.000000       ");

    EXPECT_EQ(result, expected);
}


TEST_F(TransferFunctionTests, TestStep) {
    tf_core::TransferFunction::CoefficientsVector num = {1.0f};
    tf_core::TransferFunction::CoefficientsVector den = {1.0f, 2.0f};
    auto tf = tf_core::TransferFunction(num, den);

    auto response_length = 1.0f;
    auto step_response = tf.Step(response_length);

    tf_core::Signal expected = {
        0.002493765586035f,
        0.007468859024509f,
        0.012419139029374f,
        0.017344729358404f,
        0.022245753152128f,
        0.027122332936905f,
        0.031974590627993f,
        0.036802647532592f,
        0.041606624352878f,
        0.046386641189023f
    };

    for (auto idx = 0u; idx < expected.size(); idx++)
        EXPECT_FLOAT_EQ(step_response.at(idx), expected.at(idx));
}

TEST_F(TransferFunctionTests, TestImpulse) {
    tf_core::TransferFunction::CoefficientsVector num = {1.0f};
    tf_core::TransferFunction::CoefficientsVector den = {1.0f, 2.0f};
    auto tf = tf_core::TransferFunction(num, den);

    auto response_length = 1.0f;
    auto impulse_response = tf.Impulse(response_length);

    tf_core::Signal expected = {
        0.249376558603491f,
        0.497509343847364f,
        0.495028000486529f,
        0.492559032903055f,
        0.490102379372367f,
        0.487657978477742f,
        0.485225769108775f,
        0.482805690459854f,
        0.480397682028632f,
        0.478001683614524f
    };

    for (auto idx = 0u; idx < expected.size(); idx++)
        EXPECT_FLOAT_EQ(impulse_response.at(idx), expected.at(idx));
}

TEST_F(TransferFunctionTests, TestBode) {
    tf_core::TransferFunction::CoefficientsVector num = {1.0f};
    tf_core::TransferFunction::CoefficientsVector den = {1.0f, 2.0f};
    auto tf = tf_core::TransferFunction(num, den);

    std::vector<float> omega = { 
        0.001f, 0.01f, 0.1f, 1.0f, 10.0f, 100.0f, 1000.0f
    };

    std::vector<float> expected_mag = {
        0.999998000006000f,
        0.999800059980007f,
        0.980580675690920f,
        0.447213595499958f,
        0.049937616943892f,
        0.004999937501172f,
        0.000499993750117f
    };

    std::vector<float> expected_phase = {
        -0.001999997333340f,
        -0.019997333973151f,
        -0.197395559849881f,
        -1.107148717794090f,
        -1.520837931072954f,
        -1.565796368460938f,
        -1.565796368460938f
    };

    auto bode = tf.Bode(omega);

    for (auto idx = 0u; idx < bode.size(); idx++) {
        auto [mag, phase] = bode.at(idx);
        EXPECT_NEAR(mag, expected_mag.at(idx), 0.001f);
        EXPECT_NEAR(phase, expected_phase.at(idx), 0.01f);
    }
}

TEST_F(TransferFunctionTests, TestNyquist) {
    tf_core::TransferFunction::CoefficientsVector num = {1.0f};
    tf_core::TransferFunction::CoefficientsVector den = {1.0f, 2.0f};
    auto tf = tf_core::TransferFunction(num, den);

    std::vector<float> omega = { 
        0.001f, 0.01f, 0.1f, 1.0f, 10.0f, 100.0f, 1000.0f
    };

    std::vector<float> expected_re = {
        0.999996000016000f,
        0.999600159936026f,
        0.961538461538462f,
        0.200000000000000f,
        0.002493765586035f,
        0.000024999375016f,
        0.000024999375016f
    };

    std::vector<float> expected_im = {
        -0.001999992000032f,
        -0.019992003198720f,
        -0.192307692307692f,
        -0.400000000000000f,
        -0.049875311720698f,
        -0.004999875003125f,
        -0.004999875003125f
    };

    auto nyquist = tf.Nyquist(omega);

    for (auto idx = 0u; idx < nyquist.size(); idx++) {
        auto [re, im] = nyquist.at(idx);
        EXPECT_NEAR(re, expected_re.at(idx), 0.001f);
        EXPECT_NEAR(im, expected_im.at(idx), 0.01f);
    }
}


TEST_F(TransferFunctionTests, TestIsStable) {
    tf_core::TransferFunction::CoefficientsVector num = {1.0f};
    tf_core::TransferFunction::CoefficientsVector den = {1.0f, 2.0f};
    auto tf = tf_core::TransferFunction(num, den);

    EXPECT_TRUE(tf.IsStable());
}