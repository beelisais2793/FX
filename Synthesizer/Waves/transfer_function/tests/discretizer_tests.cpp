#include "gtest/gtest.h"

#include <memory>

#include "core_transfer_function.hpp"
#include "discretization_method.hpp"
#include "polynomial_discretizer.hpp"
#include "polynomial_fraction.hpp"
#include "transfer_function_discretizer.hpp"

/****************** Polynomial Fraction ******************/
class PolynomialFractionTests : public ::testing::Test
{
    protected:
        void SetUp(void) override
        {}

        void TearDown(void) override
        {}
};

TEST_F(PolynomialFractionTests, TestConstructor) {
    tf_core::Polynomial num = tf_core::Polynomial({1.0f});
    tf_core::Polynomial den = tf_core::Polynomial({1.0f});
    EXPECT_NO_THROW(auto polynomial_fraction = tf_core::PolynomialFraction(num, den));
}

TEST_F(PolynomialFractionTests, TestConstructorValues) {
    tf_core::Polynomial num = tf_core::Polynomial({1.0f, 2.0f});
    tf_core::Polynomial den = tf_core::Polynomial({1.0f, 3.0f});
    auto polynomial_fraction = tf_core::PolynomialFraction(num, den);

    EXPECT_EQ(num, polynomial_fraction.GetNum());
    EXPECT_EQ(den, polynomial_fraction.GetDen());
}

TEST_F(PolynomialFractionTests, TestEqualOperatorEqual) {
    tf_core::Polynomial num = tf_core::Polynomial({1.0f, 2.0f});
    tf_core::Polynomial den = tf_core::Polynomial({1.0f, 3.0f});
    auto a = tf_core::PolynomialFraction(num, den);
    auto b = tf_core::PolynomialFraction(num, den);

    EXPECT_TRUE(a == b);
}

TEST_F(PolynomialFractionTests, TestEqualOperatorNonequal) {
    tf_core::Polynomial num_a = tf_core::Polynomial({1.0f, 2.0f});
    tf_core::Polynomial den_a = tf_core::Polynomial({1.0f, 3.0f});
    auto a = tf_core::PolynomialFraction(num_a, den_a);
    tf_core::Polynomial num_b = tf_core::Polynomial({1.0f, 2.0f});
    tf_core::Polynomial den_b = tf_core::Polynomial({1.0f, 4.0f});
    auto b = tf_core::PolynomialFraction(num_b, den_b);

    EXPECT_FALSE(a == b);
}

TEST_F(PolynomialFractionTests, TestNONEqualOperatorEqual) {
    tf_core::Polynomial num = tf_core::Polynomial({1.0f, 2.0f});
    tf_core::Polynomial den = tf_core::Polynomial({1.0f, 3.0f});
    auto a = tf_core::PolynomialFraction(num, den);
    auto b = tf_core::PolynomialFraction(num, den);

    EXPECT_FALSE(a != b);
}

TEST_F(PolynomialFractionTests, TestNonEqualOperatorNonequal) {
    tf_core::Polynomial num_a = tf_core::Polynomial({1.0f, 2.0f});
    tf_core::Polynomial den_a = tf_core::Polynomial({1.0f, 3.0f});
    auto a = tf_core::PolynomialFraction(num_a, den_a);
    tf_core::Polynomial num_b = tf_core::Polynomial({1.0f, 2.0f});
    tf_core::Polynomial den_b = tf_core::Polynomial({1.0f, 4.0f});
    auto b = tf_core::PolynomialFraction(num_b, den_b);

    EXPECT_TRUE(a != b);
}

TEST_F(PolynomialFractionTests, TestPlusOperator) {
    tf_core::Polynomial num_a = tf_core::Polynomial({1.0f, 2.0f});
    tf_core::Polynomial den_a = tf_core::Polynomial({1.0f, 3.0f});
    auto a = tf_core::PolynomialFraction(num_a, den_a);
    tf_core::Polynomial num_b = tf_core::Polynomial({1.0f, 2.0f});
    tf_core::Polynomial den_b = tf_core::Polynomial({1.0f, 3.0f});
    auto b = tf_core::PolynomialFraction(num_b, den_b);

    auto sum = a + b;

    auto expected_num = tf_core::Polynomial({2.0f, 10.0f, 12.0f});
    auto expected_den = tf_core::Polynomial({1.0f, 6.0f, 9.0f});
    auto expected_sum = tf_core::PolynomialFraction(expected_num, expected_den);

    EXPECT_TRUE(sum == expected_sum);
}

TEST_F(PolynomialFractionTests, TestPlusEqualOperator) {
    tf_core::Polynomial num_a = tf_core::Polynomial({1.0f, 2.0f});
    tf_core::Polynomial den_a = tf_core::Polynomial({1.0f, 3.0f});
    auto a = tf_core::PolynomialFraction(num_a, den_a);
    tf_core::Polynomial num_b = tf_core::Polynomial({1.0f, 2.0f});
    tf_core::Polynomial den_b = tf_core::Polynomial({1.0f, 3.0f});
    auto b = tf_core::PolynomialFraction(num_b, den_b);

    a += b;

    auto expected_num = tf_core::Polynomial({2.0f, 10.0f, 12.0f});
    auto expected_den = tf_core::Polynomial({1.0f, 6.0f, 9.0f});
    auto expected_sum = tf_core::PolynomialFraction(expected_num, expected_den);

    EXPECT_TRUE(a == expected_sum);
}

TEST_F(PolynomialFractionTests, TestMinuOperator) {
    tf_core::Polynomial num_a = tf_core::Polynomial({1.0f, 2.0f});
    tf_core::Polynomial den_a = tf_core::Polynomial({1.0f, 3.0f});
    auto a = tf_core::PolynomialFraction(num_a, den_a);
    tf_core::Polynomial num_b = tf_core::Polynomial({1.0f, 2.0f});
    tf_core::Polynomial den_b = tf_core::Polynomial({1.0f, 3.0f});
    auto b = tf_core::PolynomialFraction(num_b, den_b);

    auto sum = a - b;

    auto expected_num = tf_core::Polynomial({0.0f});
    auto expected_den = tf_core::Polynomial({1.0f, 6.0f, 9.0f});
    auto expected_sum = tf_core::PolynomialFraction(expected_num, expected_den);

    EXPECT_TRUE(sum == expected_sum);
}

TEST_F(PolynomialFractionTests, TestMinusEqualOperator) {
    tf_core::Polynomial num_a = tf_core::Polynomial({1.0f, 2.0f});
    tf_core::Polynomial den_a = tf_core::Polynomial({1.0f, 3.0f});
    auto a = tf_core::PolynomialFraction(num_a, den_a);
    tf_core::Polynomial num_b = tf_core::Polynomial({1.0f, 2.0f});
    tf_core::Polynomial den_b = tf_core::Polynomial({1.0f, 3.0f});
    auto b = tf_core::PolynomialFraction(num_b, den_b);

    a -= b;

    auto expected_num = tf_core::Polynomial({0.0f});
    auto expected_den = tf_core::Polynomial({1.0f, 6.0f, 9.0f});
    auto expected_sum = tf_core::PolynomialFraction(expected_num, expected_den);

    EXPECT_TRUE(a == expected_sum);
}

TEST_F(PolynomialFractionTests, TestMultiplyOperator) {
    tf_core::Polynomial num = tf_core::Polynomial({1.0f, 2.0f});
    tf_core::Polynomial den = tf_core::Polynomial({1.0f, 4.0f});
    auto a = tf_core::PolynomialFraction(num, den);

    const auto m = 4.0f;
    auto result = a * m;

    auto expected_num = num * m;
    auto expected_den = den;
    auto expected_result = tf_core::PolynomialFraction(expected_num, expected_den);

    EXPECT_EQ(result, expected_result);
}

TEST_F(PolynomialFractionTests, TestPower) {
    tf_core::Polynomial num = tf_core::Polynomial({1.0f, 2.0f});
    tf_core::Polynomial den = tf_core::Polynomial({1.0f, 4.0f});
    auto a = tf_core::PolynomialFraction(num, den);

    const auto m = 2u;
    auto result = a.Power(m);

    auto expected_num = num.Power(m);
    auto expected_den = tf_core::Polynomial({1.0f, 8.0f, 16.0f});
    auto expected_result = tf_core::PolynomialFraction(expected_num, expected_den);

    EXPECT_EQ(result, expected_result);
}

/**************** Polynomial Discretizer ****************/
class PolynomialDiscretizerTests : public ::testing::TestWithParam<tf_core::DiscretizationMethod>
{
    protected:
        void SetUp(void) override
        {}

        void TearDown(void) override
        {}
};

TEST_P(PolynomialDiscretizerTests, TestConstructor) {
    EXPECT_NO_THROW(auto polynomial_discretizer = tf_core::PolynomialDiscretizer());
}

TEST_P(PolynomialDiscretizerTests, TestZeroOrderPolynomial) {
    auto polynomial = tf_core::Polynomial({1.0f});
    auto discretization_time = 0.5f;
    auto method = GetParam();

    switch (GetParam()) {
        case tf_core::DiscretizationMethod::Forward:
        {
            auto [num, den] = tf_core::PolynomialDiscretizer::Discretize(polynomial, discretization_time, method);
            EXPECT_EQ(num, polynomial);
            EXPECT_EQ(den, tf_core::Polynomial({1.0f}));
            break;
        }
        case tf_core::DiscretizationMethod::Backward:
        {
            auto [num, den] = tf_core::PolynomialDiscretizer::Discretize(polynomial, discretization_time, method);
            EXPECT_EQ(num, polynomial);
            EXPECT_EQ(den, tf_core::Polynomial({1.0f}));
            break;
        }
        case tf_core::DiscretizationMethod::Tustin:
        {
            auto [num, den] = tf_core::PolynomialDiscretizer::Discretize(polynomial, discretization_time, method);
            EXPECT_EQ(num, polynomial);
            EXPECT_EQ(den, tf_core::Polynomial({1.0f}));
            break;
        }
        default:
        {
            EXPECT_THROW(tf_core::PolynomialDiscretizer::Discretize(polynomial, discretization_time, method), std::invalid_argument);
        }
    }
}

INSTANTIATE_TEST_CASE_P(
    PolynomialDiscretizerTestsMethods,
    PolynomialDiscretizerTests,
    ::testing::Values(
        tf_core::DiscretizationMethod::Forward,
        tf_core::DiscretizationMethod::Backward,
        tf_core::DiscretizationMethod::Tustin,
        static_cast<tf_core::DiscretizationMethod>(4u)
    )
);

/**************** Polynomial Discretizer ****************/
class TransferFunctionDiscretizerTests : public ::testing::TestWithParam<tf_core::DiscretizationMethod>
{
    protected:
        void SetUp(void) override
        {}

        void TearDown(void) override
        {}
};

TEST_P(TransferFunctionDiscretizerTests, TestZeroOrderTransferFunction) {
    auto num = tf_core::Polynomial({2.0f});
    auto den = tf_core::Polynomial({1.0f});
    auto tf = tf_core::CoreTransferFunction(num, den);

    auto discretization_time = 0.5f;
    auto method = GetParam();

    switch (GetParam()) {
        case tf_core::DiscretizationMethod::Forward:
        {
            auto tf_discrete = tf_core::TransferFunctionDiscretizer::Discretize(tf, discretization_time, method);
            EXPECT_EQ(tf_discrete.GetNum(), num);
            EXPECT_EQ(tf_discrete.GetDen(), tf_core::Polynomial({1.0f}));
            break;
        }
        case tf_core::DiscretizationMethod::Backward:
        {
            auto tf_discrete = tf_core::TransferFunctionDiscretizer::Discretize(tf, discretization_time, method);
            EXPECT_EQ(tf_discrete.GetNum(), num);
            EXPECT_EQ(tf_discrete.GetDen(), tf_core::Polynomial({1.0f}));
            break;
        }
        case tf_core::DiscretizationMethod::Tustin:
        {
            auto tf_discrete = tf_core::TransferFunctionDiscretizer::Discretize(tf, discretization_time, method);
            EXPECT_EQ(tf_discrete.GetNum(), num);
            EXPECT_EQ(tf_discrete.GetDen(), tf_core::Polynomial({1.0f}));
            break;
        }
        default:
        {
            EXPECT_THROW(tf_core::TransferFunctionDiscretizer::Discretize(tf, discretization_time, method), std::invalid_argument);
        }    
    }
}

TEST_P(TransferFunctionDiscretizerTests, TestFirstOrderTransferFunction) {
    auto num = tf_core::Polynomial({1.0f});
    auto den = tf_core::Polynomial({1.0f, 2.0f});
    auto tf = tf_core::CoreTransferFunction(num, den);

    auto discretization_time = 1.0f;
    auto method = GetParam();

    switch (GetParam()) {
        case tf_core::DiscretizationMethod::Forward:
        {
            auto tf_discrete = tf_core::TransferFunctionDiscretizer::Discretize(tf, discretization_time, method);
            EXPECT_EQ(tf_discrete.GetNum(), tf_core::Polynomial({1.0f}));
            EXPECT_EQ(tf_discrete.GetDen(), tf_core::Polynomial({-1.0f, 2.0f}));
            break;
        }
        case tf_core::DiscretizationMethod::Backward:
        {
            auto tf_discrete = tf_core::TransferFunctionDiscretizer::Discretize(tf, discretization_time, method);
            EXPECT_EQ(tf_discrete.GetNum(), tf_core::Polynomial({0.0f, 1.0f}));
            EXPECT_EQ(tf_discrete.GetDen(), tf_core::Polynomial({-2.0f, 3.0f}));
            break;
        }
        case tf_core::DiscretizationMethod::Tustin:
        {
            auto tf_discrete = tf_core::TransferFunctionDiscretizer::Discretize(tf, discretization_time, method);
            EXPECT_EQ(tf_discrete.GetNum(), tf_core::Polynomial({1.0f, 1.0f}));
            EXPECT_EQ(tf_discrete.GetDen(), tf_core::Polynomial({-3.0f, 5.0f}));
            break;
        }
        default:
        {
            EXPECT_THROW(tf_core::TransferFunctionDiscretizer::Discretize(tf, discretization_time, method), std::invalid_argument);
        }    
    }
}

INSTANTIATE_TEST_CASE_P(
    TransferFunctionDiscretizerTestsMethods,
    TransferFunctionDiscretizerTests,
    ::testing::Values(
        tf_core::DiscretizationMethod::Forward,
        tf_core::DiscretizationMethod::Backward,
        tf_core::DiscretizationMethod::Tustin,
        static_cast<tf_core::DiscretizationMethod>(4u)
    )
);
