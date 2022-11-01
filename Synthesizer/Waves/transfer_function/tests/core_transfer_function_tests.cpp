#include "gtest/gtest.h"

#include <memory>

#include "core_transfer_function.hpp"

/****************** Core Transfer Function ******************/
class CoreTransferFunctionTests : public ::testing::Test
{
    protected:
        void SetUp(void) override
        {}

        void TearDown(void) override
        {}
};

TEST_F(CoreTransferFunctionTests, TestConstructor) {
    tf_core::Polynomial num = tf_core::Polynomial({1.0f});
    tf_core::Polynomial den = tf_core::Polynomial({1.0f});
    EXPECT_NO_THROW(auto tf = tf_core::CoreTransferFunction(num, den));
}

TEST_F(CoreTransferFunctionTests, TestGetters) {
    tf_core::Polynomial num = tf_core::Polynomial({1.0f});
    tf_core::Polynomial den = tf_core::Polynomial({1.0f});
    auto tf = tf_core::CoreTransferFunction(num, den);

    EXPECT_EQ(num, tf.GetNum());
    EXPECT_EQ(den, tf.GetDen());
}

TEST_F(CoreTransferFunctionTests, TestEqualOperator) {
    tf_core::Polynomial num_a = tf_core::Polynomial({1.0f});
    tf_core::Polynomial den_a = tf_core::Polynomial({1.0f, 2.0f});
    auto a = tf_core::CoreTransferFunction(num_a, den_a);

    tf_core::Polynomial num_b = tf_core::Polynomial({1.0f});
    tf_core::Polynomial den_b = tf_core::Polynomial({1.0f, 2.0f});
    auto b = tf_core::CoreTransferFunction(num_b, den_b);

    EXPECT_TRUE(a == b);
}

TEST_F(CoreTransferFunctionTests, TestNonEqualOperator) {
    tf_core::Polynomial num_a = tf_core::Polynomial({1.0f});
    tf_core::Polynomial den_a = tf_core::Polynomial({1.0f, 2.0f});
    auto a = tf_core::CoreTransferFunction(num_a, den_a);

    tf_core::Polynomial num_b = tf_core::Polynomial({1.0f});
    tf_core::Polynomial den_b = tf_core::Polynomial({1.0f, 3.0f});
    auto b = tf_core::CoreTransferFunction(num_b, den_b);

    EXPECT_TRUE(a != b);
}

TEST_F(CoreTransferFunctionTests, TestParallel) {
    tf_core::Polynomial num_a = tf_core::Polynomial({1.0f});
    tf_core::Polynomial den_a = tf_core::Polynomial({1.0f, 2.0f});
    auto a = tf_core::CoreTransferFunction(num_a, den_a);

    tf_core::Polynomial num_b = tf_core::Polynomial({1.0f});
    tf_core::Polynomial den_b = tf_core::Polynomial({1.0f, 3.0f});
    auto b = tf_core::CoreTransferFunction(num_b, den_b);

    auto parallel_connection = a.Parallel(b);

    auto expected_num = tf_core::Polynomial({2.0f, 5.0f});
    auto expected_den = tf_core::Polynomial({1.0f, 5.0f, 6.0f});
    auto expected_output = tf_core::CoreTransferFunction(expected_num, expected_den);

    EXPECT_EQ(expected_output, parallel_connection);
}

TEST_F(CoreTransferFunctionTests, TestPlusOperator) {
    tf_core::Polynomial num_a = tf_core::Polynomial({1.0f});
    tf_core::Polynomial den_a = tf_core::Polynomial({1.0f, 2.0f});
    auto a = tf_core::CoreTransferFunction(num_a, den_a);

    tf_core::Polynomial num_b = tf_core::Polynomial({1.0f});
    tf_core::Polynomial den_b = tf_core::Polynomial({1.0f, 3.0f});
    auto b = tf_core::CoreTransferFunction(num_b, den_b);

    auto parallel_connection = a + b;

    auto expected_num = tf_core::Polynomial({2.0f, 5.0f});
    auto expected_den = tf_core::Polynomial({1.0f, 5.0f, 6.0f});
    auto expected_output = tf_core::CoreTransferFunction(expected_num, expected_den);

    EXPECT_EQ(expected_output, parallel_connection);
}

TEST_F(CoreTransferFunctionTests, TestSeries) {
    tf_core::Polynomial num_a = tf_core::Polynomial({1.0f});
    tf_core::Polynomial den_a = tf_core::Polynomial({1.0f, 2.0f});
    auto a = tf_core::CoreTransferFunction(num_a, den_a);

    tf_core::Polynomial num_b = tf_core::Polynomial({2.0f});
    tf_core::Polynomial den_b = tf_core::Polynomial({1.0f, 3.0f});
    auto b = tf_core::CoreTransferFunction(num_b, den_b);

    auto parallel_connection = a.Series(b);

    auto expected_num = tf_core::Polynomial({2.0f});
    auto expected_den = tf_core::Polynomial({1.0f, 5.0f, 6.0f});
    auto expected_output = tf_core::CoreTransferFunction(expected_num, expected_den);

    EXPECT_EQ(expected_output, parallel_connection);
}

TEST_F(CoreTransferFunctionTests, TestMultiplyOperator) {
    tf_core::Polynomial num_a = tf_core::Polynomial({1.0f});
    tf_core::Polynomial den_a = tf_core::Polynomial({1.0f, 2.0f});
    auto a = tf_core::CoreTransferFunction(num_a, den_a);

    tf_core::Polynomial num_b = tf_core::Polynomial({2.0f});
    tf_core::Polynomial den_b = tf_core::Polynomial({1.0f, 3.0f});
    auto b = tf_core::CoreTransferFunction(num_b, den_b);

    auto parallel_connection = a * b;

    auto expected_num = tf_core::Polynomial({2.0f});
    auto expected_den = tf_core::Polynomial({1.0f, 5.0f, 6.0f});
    auto expected_output = tf_core::CoreTransferFunction(expected_num, expected_den);

    EXPECT_EQ(expected_output, parallel_connection);
}

TEST_F(CoreTransferFunctionTests, TestParallelNested) {
    tf_core::Polynomial num_a = tf_core::Polynomial({1.0f});
    tf_core::Polynomial den_a = tf_core::Polynomial({1.0f, 2.0f});
    auto a = tf_core::CoreTransferFunction(num_a, den_a);

    tf_core::Polynomial num_b = tf_core::Polynomial({1.0f});
    tf_core::Polynomial den_b = tf_core::Polynomial({1.0f, 3.0f});
    auto b = tf_core::CoreTransferFunction(num_b, den_b);

    auto parallel_connection = a.Parallel(b).Parallel(b);

    auto expected_num = tf_core::Polynomial({3.0f, 16.0f, 21.0f});
    auto expected_den = tf_core::Polynomial({1.0f, 8.0f, 21.0f, 18.0f});
    auto expected_output = tf_core::CoreTransferFunction(expected_num, expected_den);
    EXPECT_EQ(expected_output, parallel_connection);

    auto parallel_connection_operator = a + b + b;
    EXPECT_EQ(expected_output, parallel_connection_operator);
}

TEST_F(CoreTransferFunctionTests, TestSeriesNested) {
    tf_core::Polynomial num_a = tf_core::Polynomial({1.0f});
    tf_core::Polynomial den_a = tf_core::Polynomial({1.0f, 2.0f});
    auto a = tf_core::CoreTransferFunction(num_a, den_a);

    tf_core::Polynomial num_b = tf_core::Polynomial({1.0f});
    tf_core::Polynomial den_b = tf_core::Polynomial({1.0f, 3.0f});
    auto b = tf_core::CoreTransferFunction(num_b, den_b);

    auto connection = a.Series(b).Series(b);

    auto expected_num = tf_core::Polynomial({1.0f});
    auto expected_den = tf_core::Polynomial({1.0f, 8.0f, 21.0f, 18.0f});
    auto expected_output = tf_core::CoreTransferFunction(expected_num, expected_den);
    EXPECT_EQ(expected_output, connection);

    auto connection_operator = a * b * b;
    EXPECT_EQ(expected_output, connection_operator);
}

TEST_F(CoreTransferFunctionTests, TestSeriesParallelNested) {
    tf_core::Polynomial num_a = tf_core::Polynomial({1.0f});
    tf_core::Polynomial den_a = tf_core::Polynomial({1.0f, 2.0f});
    auto a = tf_core::CoreTransferFunction(num_a, den_a);

    tf_core::Polynomial num_b = tf_core::Polynomial({1.0f});
    tf_core::Polynomial den_b = tf_core::Polynomial({1.0f, 3.0f});
    auto b = tf_core::CoreTransferFunction(num_b, den_b);

    auto connection = a.Series(b).Parallel(b);

    auto expected_num = tf_core::Polynomial({2.0f, 8.0f, 6.0f});
    auto expected_den = tf_core::Polynomial({1.0f, 8.0f, 21.0f, 18.0f});
    auto expected_output = tf_core::CoreTransferFunction(expected_num, expected_den);
    EXPECT_EQ(expected_output, connection);

    auto connection_operator = a * b + b;
    EXPECT_EQ(expected_output, connection_operator);
}

TEST_F(CoreTransferFunctionTests, TestFeedbackNegative) {
    tf_core::Polynomial num_a = tf_core::Polynomial({1.0f});
    tf_core::Polynomial den_a = tf_core::Polynomial({1.0f, 2.0f});
    auto a = tf_core::CoreTransferFunction(num_a, den_a);

    tf_core::Polynomial num_b = tf_core::Polynomial({1.0f});
    tf_core::Polynomial den_b = tf_core::Polynomial({1.0f, 3.0f});
    auto b = tf_core::CoreTransferFunction(num_b, den_b);

    auto parallel_connection = a.Feedback(b);

    auto expected_num = tf_core::Polynomial({1.0f, 3.0f});
    auto expected_den = tf_core::Polynomial({2.0f, 5.0f, 6.0f});
    auto expected_output = tf_core::CoreTransferFunction(expected_num, expected_den);

    EXPECT_EQ(expected_output, parallel_connection);
}