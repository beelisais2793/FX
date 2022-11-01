#include "gtest/gtest.h"

#include <memory>

#include "complex.hpp"
#include "complex_transfer_function.hpp"
#include "polynomial.hpp"

/********************* Complex Numbers *********************/
class ComplexNumbersTests : public ::testing::Test
{
    protected:
        void SetUp(void) override
        {}

        void TearDown(void) override
        {}
};

TEST_F(ComplexNumbersTests, TestConstructor) {
    auto real = 1.0f;
    auto img = 6.0f;

    auto c = tf_core::Complex(real, img);

    EXPECT_FLOAT_EQ(real, c.Real());
    EXPECT_FLOAT_EQ(img, c.Img());
}

TEST_F(ComplexNumbersTests, TestEqualOperator) {
    auto real = 1.0f;
    auto img = 6.0f;
    auto c = tf_core::Complex(real, img);
    auto d = tf_core::Complex(real, img);

    EXPECT_TRUE(c == d);
    EXPECT_FALSE(c != d);
}

TEST_F(ComplexNumbersTests, TestNonEqualOperator) {
    auto real_c = 1.0f;
    auto img_c = 6.0f;
    auto c = tf_core::Complex(real_c, img_c);
    auto real_d = 1.0f;
    auto img_d = 5.0f;
    auto d = tf_core::Complex(real_d, img_d);

    EXPECT_TRUE(c != d);
    EXPECT_FALSE(c == d);
}

TEST_F(ComplexNumbersTests, TestAbsAndPhase) {
    auto real = 1.0f;
    auto img = 6.0f;

    auto c = tf_core::Complex(real, img);
    auto abs = c.Abs();
    auto phase = c.Phase();

    auto expected_abs = 6.082762530298219f;
    auto expected_phase = 1.405647649380270f;

    EXPECT_FLOAT_EQ(abs, expected_abs);
    EXPECT_FLOAT_EQ(phase, expected_phase);
}

TEST_F(ComplexNumbersTests, TestConjugate) {
    auto real = 1.0f;
    auto img = 6.0f;

    auto c = tf_core::Complex(real, img);
    auto conj = c.Conjugate();

    auto expected_conj = tf_core::Complex(real, -img);

    EXPECT_EQ(conj, expected_conj);
}

TEST_F(ComplexNumbersTests, TestPlusOperator) {
    auto real_c = 1.0f;
    auto img_c = 6.0f;
    auto c = tf_core::Complex(real_c, img_c);
    auto real_d = 1.0f;
    auto img_d = 5.0f;
    auto d = tf_core::Complex(real_d, img_d);

    auto sum = c + d;

    auto expected_sum = tf_core::Complex(real_c + real_d, img_c + img_d);

    EXPECT_EQ(sum, expected_sum);
}

TEST_F(ComplexNumbersTests, TestPlusEQualOperator) {
    auto real_c = 1.0f;
    auto img_c = 6.0f;
    auto c = tf_core::Complex(real_c, img_c);
    auto real_d = 1.0f;
    auto img_d = 5.0f;
    auto d = tf_core::Complex(real_d, img_d);

    c += d;

    auto expected_sum = tf_core::Complex(real_c + real_d, img_c + img_d);

    EXPECT_EQ(c, expected_sum);
}

TEST_F(ComplexNumbersTests, TestMinusOperator) {
    auto real_c = 1.0f;
    auto img_c = 6.0f;
    auto c = tf_core::Complex(real_c, img_c);
    auto real_d = 1.0f;
    auto img_d = 5.0f;
    auto d = tf_core::Complex(real_d, img_d);

    auto sum = c - d;

    auto expected_sum = tf_core::Complex(real_c - real_d, img_c - img_d);

    EXPECT_EQ(sum, expected_sum);
}

TEST_F(ComplexNumbersTests, TestMinusEQualOperator) {
    auto real_c = 1.0f;
    auto img_c = 6.0f;
    auto c = tf_core::Complex(real_c, img_c);
    auto real_d = 1.0f;
    auto img_d = 5.0f;
    auto d = tf_core::Complex(real_d, img_d);

    c -= d;

    auto expected_sum = tf_core::Complex(real_c - real_d, img_c - img_d);

    EXPECT_EQ(c, expected_sum);
}

TEST_F(ComplexNumbersTests, TestMultiplyOperator) {
    auto real_c = 1.0f;
    auto img_c = 6.0f;
    auto c = tf_core::Complex(real_c, img_c);
    auto real_d = 1.0f;
    auto img_d = 5.0f;
    auto d = tf_core::Complex(real_d, img_d);

    auto mul = c * d;
    auto mulf = c * 3.0f;

    auto expected = tf_core::Complex(-29.0f, 11.0f);
    auto expectedf = tf_core::Complex(real_c * 3.0f, img_c * 3.0f);

    EXPECT_EQ(mul, expected);
    EXPECT_EQ(mulf, expectedf);
}

TEST_F(ComplexNumbersTests, TestMultiplyEqualOperator) {
    auto real_c = 1.0f;
    auto img_c = 6.0f;
    auto c = tf_core::Complex(real_c, img_c);
    auto real_d = 1.0f;
    auto img_d = 5.0f;
    auto d = tf_core::Complex(real_d, img_d);

    c *= d;

    auto expected = tf_core::Complex(-29.0f, 11.0f);

    EXPECT_EQ(c, expected);

    c *= 3.0f;
    auto expectedf = tf_core::Complex(-29.0f * 3.0f, 11.0f * 3.0f);
    EXPECT_EQ(c, expectedf);
}

TEST_F(ComplexNumbersTests, TestDivideOperator) {
    auto real_c = 1.0f;
    auto img_c = 6.0f;
    auto c = tf_core::Complex(real_c, img_c);
    auto real_d = 1.0f;
    auto img_d = 5.0f;
    auto d = tf_core::Complex(real_d, img_d);

    auto div = c / d;
    auto divf = c / 3.0f;

    auto expected = tf_core::Complex(1.192307692307692f, 0.038461538461538f);
    auto expectedf = tf_core::Complex(real_c / 3.0f, img_c / 3.0f);

    EXPECT_EQ(div, expected);
    EXPECT_EQ(divf, expectedf);
}

TEST_F(ComplexNumbersTests, TestDivideEqualOperator) {
    auto real_c = 1.0f;
    auto img_c = 6.0f;
    auto c = tf_core::Complex(real_c, img_c);
    auto real_d = 1.0f;
    auto img_d = 5.0f;
    auto d = tf_core::Complex(real_d, img_d);

    c /= d;

    auto expected = tf_core::Complex(1.192307692307692f, 0.038461538461538f);

    EXPECT_EQ(c, expected);

    c /= 3.0f;
    auto expectedf = tf_core::Complex(1.192307692307692f / 3.0f, 0.038461538461538f / 3.0f);
    EXPECT_EQ(c, expectedf);
}

TEST_F(ComplexNumbersTests, TestPower) {
    auto real_c = 1.0f;
    auto img_c = 6.0f;
    auto c = tf_core::Complex(real_c, img_c);

    EXPECT_EQ(c.Power(0), tf_core::Complex(1.0f, 0.0f));
    EXPECT_EQ(c.Power(1), tf_core::Complex(1.0f, 6.0f));
    EXPECT_EQ(c.Power(2), tf_core::Complex(-35.0f, 12.0f));
    EXPECT_EQ(c.Power(3), tf_core::Complex(-107.0f, -198.0f));

    EXPECT_EQ(c^0, tf_core::Complex(1.0f, 0.0f));
    EXPECT_EQ(c^1, tf_core::Complex(1.0f, 6.0f));
    EXPECT_EQ(c^2, tf_core::Complex(-35.0f, 12.0f));
    EXPECT_EQ(c^3, tf_core::Complex(-107.0f, -198.0f));
}

/********************* Complex TF *********************/
class ComplexTransferFunctionTests : public ::testing::Test
{
    protected:
        void SetUp(void) override
        {}

        void TearDown(void) override
        {}
};

TEST_F(ComplexTransferFunctionTests, TestConstructor) {
    tf_core::Polynomial num = tf_core::Polynomial({1.0f});
    tf_core::Polynomial den = tf_core::Polynomial({1.0f, 2.0f});

    auto tf = tf_core::CoreTransferFunction(num, den);

    EXPECT_NO_THROW(auto ctf = tf_core::ComplexTransferFunction(tf));
}

TEST_F(ComplexTransferFunctionTests, TestCalculateBode) {
    tf_core::Polynomial num = tf_core::Polynomial({1.0f});
    tf_core::Polynomial den = tf_core::Polynomial({1.0f, 2.0f});

    auto tf = tf_core::CoreTransferFunction(num, den);
    auto ctf = tf_core::ComplexTransferFunction(tf);

    tf_core::Frequencies omega = { 
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

    auto bode = ctf.CalculateBode(omega);

    for (auto idx = 0u; idx < bode.size(); idx++) {
        auto [mag, phase] = bode.at(idx);
        EXPECT_NEAR(mag, expected_mag.at(idx), 0.001f);
        EXPECT_NEAR(phase, expected_phase.at(idx), 0.01f);
    }
}

TEST_F(ComplexTransferFunctionTests, TestCalculateNyquist) {
    tf_core::Polynomial num = tf_core::Polynomial({1.0f});
    tf_core::Polynomial den = tf_core::Polynomial({1.0f, 2.0f});

    auto tf = tf_core::CoreTransferFunction(num, den);
    auto ctf = tf_core::ComplexTransferFunction(tf);

    tf_core::Frequencies omega = { 
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

    auto nyquist = ctf.CalculateNyquist(omega);

    for (auto idx = 0u; idx < nyquist.size(); idx++) {
        auto [re, im] = nyquist.at(idx);
        EXPECT_NEAR(re, expected_re.at(idx), 0.001f);
        EXPECT_NEAR(im, expected_im.at(idx), 0.01f);
    }
}
