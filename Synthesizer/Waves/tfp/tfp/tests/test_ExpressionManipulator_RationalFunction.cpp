#include "gmock/gmock.h"
#include "tfp/math/Expression.hpp"
#include "tfp/math/ExpressionManipulators.hpp"
#include "tfp/math/VariableTable.hpp"
#include "tfp/util/Reference.hpp"

#define NAME ExpressionManipulator_RationalFunction

using namespace testing;
using namespace tfp;

TEST(NAME, enforce_product_order)
{
    Reference<Expression> e = Expression::parse("a*b");
    TFManipulator m;
    m.enforceProductLHS(e, "a");
    EXPECT_THAT(e->left()->name(), StrEq("a"));
    EXPECT_THAT(e->right()->name(), StrEq("b"));
    m.enforceProductLHS(e, "b");
    EXPECT_THAT(e->left()->name(), StrEq("b"));
    EXPECT_THAT(e->right()->name(), StrEq("a"));

    ASSERT_THAT(e->checkParentConsistencies(), Eq(true));
}

TEST(NAME, eliminate_divisions)
{
    Reference<Expression> e = Expression::parse("a/s");
    TFManipulator m;
    ASSERT_THAT(m.recursivelyCall(&ExpressionManipulator::eliminateDivisionsAndSubtractions, e, "s"), Eq(true));
    ASSERT_THAT(e->op2(), Eq(op::mul));
    ASSERT_THAT(e->left()->name(), StrEq("a"));
    ASSERT_THAT(e->right()->op2(), Eq(op::pow));
    ASSERT_THAT(e->right()->left()->name(), StrEq("s"));
    ASSERT_THAT(e->right()->right()->value(), DoubleEq(-1));

    ASSERT_THAT(e->checkParentConsistencies(), Eq(true));
}

TEST(NAME, eliminate_divisions_with_constant_exponent)
{
    Reference<Expression> e = Expression::parse("a/s^2");
    TFManipulator m;
    ASSERT_THAT(m.recursivelyCall(&ExpressionManipulator::eliminateDivisionsAndSubtractions, e, "s"), Eq(true));
    ASSERT_THAT(e->op2(), Eq(op::mul));
    ASSERT_THAT(e->left()->name(), StrEq("a"));
    ASSERT_THAT(e->right()->op2(), Eq(op::pow));
    ASSERT_THAT(e->right()->left()->name(), StrEq("s"));
    ASSERT_THAT(e->right()->right()->value(), DoubleEq(-2));

    ASSERT_THAT(e->checkParentConsistencies(), Eq(true));
}

TEST(NAME, eliminate_divisions_with_variable_exponent)
{
    Reference<Expression> e = Expression::parse("a/s^x");
    e->dump("eliminate_divisions_with_variable_exponent.dot");
    TFManipulator m;
    ASSERT_THAT(m.recursivelyCall(&ExpressionManipulator::eliminateDivisionsAndSubtractions, e, "s"), Eq(true));
    e->dump("eliminate_divisions_with_variable_exponent.dot", true);
    ASSERT_THAT(e->op2(), Eq(op::mul));
    ASSERT_THAT(e->left()->name(), StrEq("a"));
    ASSERT_THAT(e->right()->op2(), Eq(op::pow));
    ASSERT_THAT(e->right()->left()->name(), StrEq("s"));
    ASSERT_THAT(e->right()->right()->op2(), Eq(op::mul));
    ASSERT_THAT(e->right()->right()->right()->value(), DoubleEq(-1));
    ASSERT_THAT(e->right()->right()->left()->name(), StrEq("x"));

    ASSERT_THAT(e->checkParentConsistencies(), Eq(true));
}

TEST(NAME, eliminate_subtractions)
{
    Reference<Expression> e = Expression::parse("a-s");
    TFManipulator m;
    ASSERT_THAT(m.recursivelyCall(&ExpressionManipulator::eliminateDivisionsAndSubtractions, e, "s"), Eq(true));
    ASSERT_THAT(e->op2(), Eq(op::add));
    ASSERT_THAT(e->left()->name(), StrEq("a"));
    ASSERT_THAT(e->right()->op2(), Eq(op::mul));
    ASSERT_THAT(e->right()->left()->name(), StrEq("s"));
    ASSERT_THAT(e->right()->right()->value(), DoubleEq(-1));

    ASSERT_THAT(e->checkParentConsistencies(), Eq(true));
}

TEST(NAME, eliminate_subtractions_with_constant_post_factor)
{
    Reference<Expression> e = Expression::parse("a-s*2");
    TFManipulator m;
    ASSERT_THAT(m.recursivelyCall(&ExpressionManipulator::eliminateDivisionsAndSubtractions, e, "s"), Eq(true));
    ASSERT_THAT(e->op2(), Eq(op::add));
    ASSERT_THAT(e->left()->name(), StrEq("a"));
    ASSERT_THAT(e->right()->op2(), Eq(op::mul));
    ASSERT_THAT(e->right()->left()->name(), StrEq("s"));
    ASSERT_THAT(e->right()->right()->value(), DoubleEq(-2));

    ASSERT_THAT(e->checkParentConsistencies(), Eq(true));
}

TEST(NAME, eliminate_subtractions_with_variable_post_factor)
{
    Reference<Expression> e = Expression::parse("a-s*x");
    TFManipulator m;
    ASSERT_THAT(m.recursivelyCall(&ExpressionManipulator::eliminateDivisionsAndSubtractions, e, "s"), Eq(true));
    ASSERT_THAT(e->op2(), Eq(op::add));
    ASSERT_THAT(e->left()->name(), StrEq("a"));
    ASSERT_THAT(e->right()->op2(), Eq(op::mul));
    ASSERT_THAT(e->right()->left()->name(), StrEq("s"));
    ASSERT_THAT(e->right()->right()->op2(), Eq(op::mul));
    ASSERT_THAT(e->right()->right()->left()->name(), StrEq("x"));
    ASSERT_THAT(e->right()->right()->right()->value(), DoubleEq(-1));

    ASSERT_THAT(e->checkParentConsistencies(), Eq(true));
}

TEST(NAME, eliminate_subtractions_with_constant_pre_factor)
{
    Reference<Expression> e = Expression::parse("a-2*s");
    TFManipulator m;
    ASSERT_THAT(m.recursivelyCall(&ExpressionManipulator::eliminateDivisionsAndSubtractions, e, "s"), Eq(true));
    ASSERT_THAT(e->op2(), Eq(op::add));
    ASSERT_THAT(e->left()->name(), StrEq("a"));
    ASSERT_THAT(e->right()->op2(), Eq(op::mul));
    ASSERT_THAT(e->right()->left()->value(), DoubleEq(2));
    ASSERT_THAT(e->right()->right()->op2(), Eq(op::mul));
    ASSERT_THAT(e->right()->right()->left()->name(), StrEq("s"));
    ASSERT_THAT(e->right()->right()->right()->value(), DoubleEq(-1));

    ASSERT_THAT(e->checkParentConsistencies(), Eq(true));
}

TEST(NAME, eliminate_subtractions_with_variable_pre_factor)
{
    Reference<Expression> e = Expression::parse("a-x*s");
    TFManipulator m;
    ASSERT_THAT(m.recursivelyCall(&ExpressionManipulator::eliminateDivisionsAndSubtractions, e, "s"), Eq(true));
    ASSERT_THAT(e->op2(), Eq(op::add));
    ASSERT_THAT(e->left()->name(), StrEq("a"));
    ASSERT_THAT(e->right()->op2(), Eq(op::mul));
    ASSERT_THAT(e->right()->left()->name(), StrEq("x"));
    ASSERT_THAT(e->right()->right()->op2(), Eq(op::mul));
    ASSERT_THAT(e->right()->right()->left()->name(), StrEq("s"));
    ASSERT_THAT(e->right()->right()->right()->value(), DoubleEq(-1));

    ASSERT_THAT(e->checkParentConsistencies(), Eq(true));
}

TEST(NAME, enforce_constant_exponent_on_missing_exponent)
{
    Reference<Expression> e = Expression::parse("1/(s+4)");
    TFManipulator m;
    ASSERT_THAT(m.recursivelyCall(&ExpressionManipulator::enforceConstantExponent, e, "s"), Eq(true));
    ASSERT_THAT(e->right()->op2(), Eq(op::add));
    ASSERT_THAT(e->right()->left()->op2(), Eq(op::pow));
    ASSERT_THAT(e->right()->left()->left()->name(), StrEq("s"));
    ASSERT_THAT(e->right()->left()->right()->value(), DoubleEq(1.0));

    ASSERT_THAT(e->checkParentConsistencies(), Eq(true));
}

TEST(NAME, enforce_constant_exponent_on_existing_constant_exponent)
{
    Reference<Expression> e = Expression::parse("1/(s^3+4)");
    e->dump("enforce_constant_exponent_on_existing_constant_exponent.dot");
    TFManipulator m;
    ASSERT_THAT(m.recursivelyCall(&ExpressionManipulator::enforceConstantExponent, e, "s"), Eq(false));
    e->dump("enforce_constant_exponent_on_existing_constant_exponent.dot", true);
    ASSERT_THAT(e->right()->op2(), Eq(op::add));
    ASSERT_THAT(e->right()->left()->op2(), Eq(op::pow));
    ASSERT_THAT(e->right()->left()->left()->name(), StrEq("s"));
    ASSERT_THAT(e->right()->left()->right()->value(), DoubleEq(3.0));

    ASSERT_THAT(e->checkParentConsistencies(), Eq(true));
}

TEST(NAME, enforce_constant_exponent_only_on_trees_that_have_variable)
{
    Reference<Expression> e = Expression::parse("(a+b)^c");
    TFManipulator m;
    ASSERT_THAT(m.recursivelyCall(&ExpressionManipulator::enforceConstantExponent, e, "s"), Eq(false));
    ASSERT_THAT(e->op2(), Eq(op::pow));
    ASSERT_THAT(e->left()->op2(), Eq(op::add));
    ASSERT_THAT(e->checkParentConsistencies(), Eq(true));

    e = Expression::parse("(a+s)^c");
    EXPECT_THROW(m.recursivelyCall(&ExpressionManipulator::enforceConstantExponent, e, "s"), ExpressionManipulator::NonConstantExponentException);
    ASSERT_THAT(e->op2(), Eq(op::pow));
    ASSERT_THAT(e->left()->op2(), Eq(op::add));
    ASSERT_THAT(e->checkParentConsistencies(), Eq(true));
    
    e = Expression::parse("(a+s)^2");
    ASSERT_THAT(m.recursivelyCall(&ExpressionManipulator::enforceConstantExponent, e, "s"), Eq(true));
    ASSERT_THAT(e->op2(), Eq(op::pow));
    ASSERT_THAT(e->left()->op2(), Eq(op::add));
    ASSERT_THAT(e->checkParentConsistencies(), Eq(true));
}

TEST(NAME, enforce_constant_exponent_on_variable_exponent_fails)
{
    Reference<Expression> e = Expression::parse("1/(s^a+4)");
    TFManipulator m;
    EXPECT_THROW(m.recursivelyCall(&ExpressionManipulator::enforceConstantExponent, e, "s"), ExpressionManipulator::NonConstantExponentException);
    ASSERT_THAT(e->checkParentConsistencies(), Eq(true));
}

TEST(NAME, enforce_constant_exponent_on_expression_exponent_fails)
{
    Reference<Expression> e = Expression::parse("1/(s^(a+1)+4)");
    TFManipulator m;
    EXPECT_THROW(m.recursivelyCall(&ExpressionManipulator::enforceConstantExponent, e, "s"), ExpressionManipulator::NonConstantExponentException);
    ASSERT_THAT(e->checkParentConsistencies(), Eq(true));
}

TEST(NAME, enforce_constant_exponent_on_consant_exponent_expression)
{
    Reference<Expression> e = Expression::parse("1/(s^(4+1)+4)");
    TFManipulator m;
    ASSERT_THAT(m.recursivelyCall(&ExpressionManipulator::enforceConstantExponent, e, "s"), Eq(false));
    ASSERT_THAT(e->right()->op2(), Eq(op::add));
    ASSERT_THAT(e->right()->left()->op2(), Eq(op::pow));
    ASSERT_THAT(e->right()->left()->left()->name(), StrEq("s"));
    ASSERT_THAT(e->right()->left()->right()->value(), DoubleEq(5.0));

    ASSERT_THAT(e->checkParentConsistencies(), Eq(true));
}

TEST(NAME, expand_constant_exponents_4)
{
    Reference<Expression> e = Expression::parse("a^4");
    e->dump("expand_constant_exponents_4.dot");
    TFManipulator m;
    ASSERT_THAT(m.recursivelyCall(&ExpressionManipulator::expandConstantExponentsIntoProducts, e, "a"), Eq(true));
    e->dump("expand_constant_exponents_4.dot", true);
    ASSERT_THAT(e->op2(), Eq(op::mul));
    ASSERT_THAT(e->left()->op2(), Eq(op::mul));
    ASSERT_THAT(e->left()->left()->op2(), Eq(op::mul));

    ASSERT_THAT(e->checkParentConsistencies(), Eq(true));
}

TEST(NAME, expand_constant_exponents_1)
{
    Reference<Expression> e = Expression::parse("a^1");
    TFManipulator m;
    ASSERT_THAT(m.recursivelyCall(&ExpressionManipulator::expandConstantExponentsIntoProducts, e, "a"), Eq(false));
    ASSERT_THAT(e->type(), Eq(Expression::VARIABLE));
    ASSERT_THAT(e->name(), StrEq("a"));

    ASSERT_THAT(e->checkParentConsistencies(), Eq(true));
}

TEST(NAME, expand_constant_exponents_0)
{
    Reference<Expression> e = Expression::parse("a^0");
    TFManipulator m;
    ASSERT_THAT(m.recursivelyCall(&ExpressionManipulator::expandConstantExponentsIntoProducts, e, "a"), Eq(true));
    ASSERT_THAT(e->type(), Eq(Expression::CONSTANT));
    ASSERT_THAT(e->value(), DoubleEq(1.0));

    ASSERT_THAT(e->checkParentConsistencies(), Eq(true));
}

TEST(NAME, expand_constant_exponents_negative_1)
{
    Reference<Expression> e = Expression::parse("a^-1");
    TFManipulator m;
    ASSERT_THAT(m.recursivelyCall(&ExpressionManipulator::expandConstantExponentsIntoProducts, e, "a"), Eq(false));
    ASSERT_THAT(e->op2(), Eq(op::pow));
    ASSERT_THAT(e->left()->name(), StrEq("a"));
    ASSERT_THAT(e->right()->value(), DoubleEq(-1));

    ASSERT_THAT(e->checkParentConsistencies(), Eq(true));
}

TEST(NAME, expand_constant_exponents_negative_4)
{
    Reference<Expression> e = Expression::parse("a^-4");
    TFManipulator m;
    ASSERT_THAT(m.recursivelyCall(&ExpressionManipulator::expandConstantExponentsIntoProducts, e, "a"), Eq(true));
    e->dump("expand_constant_exponents_negative_4.dot");
    ASSERT_THAT(e->op2(), Eq(op::pow));
    ASSERT_THAT(e->right()->value(), DoubleEq(-1));
    ASSERT_THAT(e->left()->op2(), Eq(op::mul));
    ASSERT_THAT(e->left()->left()->op2(), Eq(op::mul));
    ASSERT_THAT(e->left()->left()->left()->op2(), Eq(op::mul));

    ASSERT_THAT(e->checkParentConsistencies(), Eq(true));
}

TEST(NAME, dont_expand_scalar_into_sum_if_tree_is_not_relevant)
{
    Reference<Expression> e = Expression::parse("a*(b+c)");
    TFManipulator m;
    ASSERT_THAT(m.recursivelyCall(&ExpressionManipulator::expand, e, "a"), Eq(false));
    ASSERT_THAT(e->checkParentConsistencies(), Eq(true));
}

TEST(NAME, expand_scalar_into_sum)
{
    Reference<Expression> e = Expression::parse("b*(a+c)");
    TFManipulator m;
    ASSERT_THAT(m.recursivelyCall(&ExpressionManipulator::expand, e, "a"), Eq(true));
    ASSERT_THAT(e->op2(), Eq(op::add));
    ASSERT_THAT(e->left()->op2(), Eq(op::mul));
    ASSERT_THAT(e->left()->left()->name(), StrEq("b"));
    ASSERT_THAT(e->left()->right()->name(), StrEq("a"));
    ASSERT_THAT(e->right()->op2(), Eq(op::mul));
    ASSERT_THAT(e->right()->left()->name(), StrEq("b"));
    ASSERT_THAT(e->right()->right()->name(), StrEq("c"));

    ASSERT_THAT(e->checkParentConsistencies(), Eq(true));
}

TEST(NAME, expand_expression_into_sum)
{
    Reference<Expression> e = Expression::parse("(a+b)*(c+d)");
    e->dump("expand_expression_into_sum.dot");
    Reference<VariableTable> vt = e->generateVariableTable();
    vt->set("a", 3.0);
    vt->set("b", 7.0);
    vt->set("c", 13.0);
    vt->set("d", 17.0);
    double beforeResult = e->evaluate(vt);

    // We're expecting a*(c+d) + b*(c+d)
    TFManipulator m;
    ASSERT_THAT(m.recursivelyCall(&ExpressionManipulator::expand, e, "a"), Eq(true));
    e->dump("expand_expression_into_sum.dot", true);
    ASSERT_THAT(e->op2(), Eq(op::add));
    ASSERT_THAT(e->left()->op2(), Eq(op::mul));
    ASSERT_THAT(e->right()->op2(), Eq(op::mul));
    ASSERT_THAT(e->left()->left()->op2(), Eq(op::add));
    ASSERT_THAT(e->right()->left()->op2(), Eq(op::add));

    // Make sure subtrees were actually duplicated
    ASSERT_THAT(e->left()->left(), Ne(e->right()->left()));
    EXPECT_THAT(e->evaluate(vt), DoubleEq(beforeResult));
    ASSERT_THAT(e->checkParentConsistencies(), Eq(true));
}

TEST(NAME, expand_binomial_product)
{
    Reference<Expression> e = Expression::parse("(a+b)*(a+c)");
    Reference<VariableTable> vt = e->generateVariableTable();
    vt->set("a", 3.0);
    vt->set("b", 7.0);
    vt->set("c", 13.0);
    double beforeResult = e->evaluate(vt);

    TFManipulator m;
    ASSERT_THAT(m.recursivelyCall(&ExpressionManipulator::expand, e, "a"), Eq(true));
    e->dump("expand_binomial_product.dot");
    ASSERT_THAT(e->op2(), Eq(op::add));
    ASSERT_THAT(e->left()->op2(), Eq(op::add));
    ASSERT_THAT(e->left()->left()->op2(), Eq(op::mul));
    ASSERT_THAT(e->left()->right()->op2(), Eq(op::mul));
    ASSERT_THAT(e->right()->op2(), Eq(op::add));
    ASSERT_THAT(e->right()->left()->op2(), Eq(op::mul));
    ASSERT_THAT(e->right()->right()->op2(), Eq(op::mul));
    ASSERT_THAT(e->evaluate(vt), DoubleEq(beforeResult));

    ASSERT_THAT(e->checkParentConsistencies(), Eq(true));
}

TEST(NAME, expand_binomial_exponent)
{
    Reference<Expression> e = Expression::parse("(a+b)^2");
    Reference<VariableTable> vt = e->generateVariableTable();
    vt->set("a", 3.0);
    vt->set("b", 7.0);
    double beforeResult = e->evaluate(vt);

    TFManipulator m;
    ASSERT_THAT(m.recursivelyCall(&ExpressionManipulator::expandConstantExponentsIntoProducts, e, "a"), Eq(true));
    ASSERT_THAT(m.recursivelyCall(&ExpressionManipulator::expand, e, "a"), Eq(true));
    e->dump("expand_binomial_exponent.dot");
    ASSERT_THAT(e->op2(), Eq(op::add));
    ASSERT_THAT(e->left()->op2(), Eq(op::add));
    ASSERT_THAT(e->left()->left()->op2(), Eq(op::mul));
    ASSERT_THAT(e->left()->right()->op2(), Eq(op::mul));
    ASSERT_THAT(e->right()->op2(), Eq(op::add));
    ASSERT_THAT(e->right()->left()->op2(), Eq(op::mul));
    ASSERT_THAT(e->right()->right()->op2(), Eq(op::mul));
    ASSERT_THAT(e->evaluate(vt), DoubleEq(beforeResult));

    ASSERT_THAT(e->checkParentConsistencies(), Eq(true));
}

TEST(NAME, factor_in_single_addition)
{
    Reference<Expression> e = Expression::parse("a+b");
    e->dump("factor_in_single_addition.dot");
    TFManipulator m;
    ASSERT_THAT(m.factorIn(e, Expression::make("c")), Eq(true));
    e->dump("factor_in_single_addition.dot", true);
    ASSERT_THAT(e->op2(), Eq(op::add));
    ASSERT_THAT(e->left()->op2(), Eq(op::mul));
    ASSERT_THAT(e->left()->left()->name(), StrEq("a"));
    ASSERT_THAT(e->left()->right()->name(), StrEq("c"));
    ASSERT_THAT(e->right()->op2(), Eq(op::mul));
    ASSERT_THAT(e->right()->left()->name(), StrEq("b"));
    ASSERT_THAT(e->right()->right()->name(), StrEq("c"));

    ASSERT_THAT(e->checkParentConsistencies(), Eq(true));
}

TEST(NAME, factor_in_complex)
{
    Reference<Expression> e = Expression::parse("a+b*g+c+d*e^(3+f)");
    e->dump("factor_in_complex.dot");
    TFManipulator m;
    ASSERT_THAT(m.factorIn(e, Expression::make("s")), Eq(true));
    e->dump("factor_in_complex.dot", true);

    ASSERT_THAT(e->checkParentConsistencies(), Eq(true));
}

TEST(NAME, factor_negative_exponents_in_addition)
{
    Reference<Expression> e = Expression::parse("a+s^-3");
    Reference<VariableTable> vt = e->generateVariableTable();
    vt->set("a", 3.0);
    vt->set("s", 5.0);
    double resultBefore = e->evaluate(vt);
    e->dump("factor_negative_exponents_in_addition.dot");
    TFManipulator m;
    ASSERT_THAT(m.recursivelyCall(&ExpressionManipulator::factorNegativeExponents, e, "s"), Eq(true));
    e->dump("factor_negative_exponents_in_addition.dot", true);
    
    ASSERT_THAT(e->op2(), Eq(op::mul));
    ASSERT_THAT(e->left()->op2(), Eq(op::add));
    ASSERT_THAT(e->left()->right()->value(), DoubleEq(1.0));
    ASSERT_THAT(e->left()->left()->op2(), Eq(op::mul));
    ASSERT_THAT(e->left()->left()->left()->name(), StrEq("a"));
    ASSERT_THAT(e->left()->left()->right()->op2(), Eq(op::pow));
    ASSERT_THAT(e->left()->left()->right()->left()->name(), StrEq("s"));
    ASSERT_THAT(e->left()->left()->right()->right()->value(), DoubleEq(3.0));
    ASSERT_THAT(e->right()->op2(), Eq(op::pow));
    ASSERT_THAT(e->right()->left()->name(), StrEq("s"));
    ASSERT_THAT(e->right()->right()->value(), DoubleEq(-3.0));
    
    ASSERT_THAT(e->evaluate(vt), DoubleEq(resultBefore));
    ASSERT_THAT(e->checkParentConsistencies(), Eq(true));
}

TEST(NAME, factor_negative_exponents_does_nothing_if_already_factored)
{
    Reference<Expression> e = Expression::parse("s^-3*(a*s^3+1)");
    Reference<VariableTable> vt = e->generateVariableTable();
    vt->set("a", 3.0);
    vt->set("s", 5.0);
    double resultBefore = e->evaluate(vt);

    TFManipulator m;
    ASSERT_THAT(m.recursivelyCall(&ExpressionManipulator::factorNegativeExponents, e, "s"), Eq(false));

    ASSERT_THAT(e->evaluate(vt), DoubleEq(resultBefore));
    ASSERT_THAT(e->checkParentConsistencies(), Eq(true));
}

TEST(NAME, factor_negative_exponents_from_deniminator_into_numerator_leaves_factor_in_denominator)
{
    Reference<Expression> e = Expression::parse("a/(2*s^-3)");
    Reference<VariableTable> vt = e->generateVariableTable();
    vt->set("a", 3.0);
    vt->set("s", 5.0);
    double resultBefore = e->evaluate(vt);

    TFManipulator m;
    e->dump("factor_negative_exponents_from_deniminator_into_numerator_leaves_factor_in_denominator.dot");
    ASSERT_THAT(m.factorNegativeExponentsToNumerator(e->right(), e->left(), "s"), Eq(true));
    e->dump("factor_negative_exponents_from_deniminator_into_numerator_leaves_factor_in_denominator.dot", true);
    ASSERT_THAT(e->right()->value(), DoubleEq(2.0));
    ASSERT_THAT(e->left()->op2(), Eq(op::mul));
    ASSERT_THAT(e->left()->left()->name(), StrEq("a"));
    ASSERT_THAT(e->left()->right()->op2(), Eq(op::pow));
    ASSERT_THAT(e->left()->right()->left()->name(), StrEq("s"));
    ASSERT_THAT(e->left()->right()->right()->value(), DoubleEq(3.0));

    ASSERT_THAT(e->evaluate(vt), DoubleEq(resultBefore));
    ASSERT_THAT(e->checkParentConsistencies(), Eq(true));
}

TEST(NAME, factor_negative_exponents_on_chain_of_additions)
{
    Reference<Expression> e = Expression::parse("a+b*s^-2+c*s^-3");
    TFManipulator m;
    e->dump("factor_negative_exponents_on_chain_of_additions.dot");
    ASSERT_THAT(m.recursivelyCall(&ExpressionManipulator::factorNegativeExponents, e, "s"), Eq(true));
    e->dump("factor_negative_exponents_on_chain_of_additions.dot", true);
    ASSERT_THAT(m.recursivelyCall(&ExpressionManipulator::factorNegativeExponents, e, "s"), Eq(true));
    e->dump("factor_negative_exponents_on_chain_of_additions.dot", true);
    ASSERT_THAT(m.recursivelyCall(&ExpressionManipulator::factorNegativeExponents, e, "s"), Eq(false));
}

TEST(NAME, lala)
{
    Reference<Expression> e = Expression::parse("s^3*2*(1+s*(s+1))");
    TFManipulator m;
    e->dump("wtf.dot");
    m.recursivelyCall(&TFManipulator::expandConstantExponentsIntoProducts, e, "s");
    m.recursivelyCall(&TFManipulator::expand, e, "s");
    e->dump("wtf.dot", true);
}

TEST(NAME, compute_transfer_function_coefficient_expressions)
{
    Reference<Expression> e = Expression::parse("1/(1/s^3 - 4/(1+s)^2 - 8/(a+4))");
    //Reference<Expression> e = Expression::parse("(s+a)^2 / (((s+b)^2 * s^2)*(a+b*s)*s)");

    /*
     * Solved on paper:
     *                      s^3 + 2s^4 + s^5
     *  G(s) = ---------------------------------------------
     *         1 + 2s + s^2 + (-4-x)s^3 + (-2x)s^4 + (-x)s^5
     * where:
     *     x = 8/(a+4)
     *
     * b0 = 0
     * b1 = 0
     * b2 = 0
     * b3 = 1
     * b4 = 2
     * b5 = 1
     * a0 = 1
     * a1 = 2
     * a2 = 1
     * a3 = -4-8/(a+4)
     * a4 = -16/(a+4)
     * a5 = -8/(a+4)
     */
    e->dump("compute_transfer_function_coefficient_expressions.dot");
    TFManipulator m;
    TFManipulator::TFCoefficients tfc = m.calculateTransferFunctionCoefficients(e, "s");

    for (std::size_t i = 0; i != tfc.numerator.size(); ++i)
    {
        std::stringstream ss; ss << "numerator, degree " <<i;
        tfc.numerator[i]->dump("compute_transfer_function_coefficient_expressions.dot", true, ss.str().c_str());
    }
    for (std::size_t i = 0; i != tfc.denominator.size(); ++i)
    {
        std::stringstream ss; ss << "denominator, degree " <<i;
        tfc.denominator[i]->dump("compute_transfer_function_coefficient_expressions.dot", true, ss.str().c_str());
    }

    Reference<VariableTable> vt = new VariableTable;
    vt->set("a", 7.2);
    ASSERT_THAT(tfc.numerator.size(), Eq(6u));
    ASSERT_THAT(tfc.denominator.size(), Eq(6u));

    ASSERT_THAT(tfc.numerator[0]->evaluate(vt), DoubleEq(0.0));
    ASSERT_THAT(tfc.numerator[1]->evaluate(vt), DoubleEq(0.0));
    ASSERT_THAT(tfc.numerator[2]->evaluate(vt), DoubleEq(0.0));
    ASSERT_THAT(tfc.numerator[3]->evaluate(vt), DoubleEq(1.0));
    ASSERT_THAT(tfc.numerator[4]->evaluate(vt), DoubleEq(2.0));
    ASSERT_THAT(tfc.numerator[5]->evaluate(vt), DoubleEq(1.0));

    ASSERT_THAT(tfc.denominator[0]->evaluate(vt), DoubleEq(1.0));
    ASSERT_THAT(tfc.denominator[1]->evaluate(vt), DoubleEq(2.0));
    ASSERT_THAT(tfc.denominator[2]->evaluate(vt), DoubleEq(1.0));
    ASSERT_THAT(tfc.denominator[3]->evaluate(vt), DoubleEq(-4.0 - 8.0 / (7.2 + 4.0)));
    ASSERT_THAT(tfc.denominator[4]->evaluate(vt), DoubleEq(-16.0 / (7.2 + 4.0)));
    ASSERT_THAT(tfc.denominator[5]->evaluate(vt), DoubleEq(-8.0 / (7.2 + 4.0)));
}

TEST(NAME, compute_transfer_function)
{
    Reference<Expression> e = Expression::parse("1/(1/s^3 - 4/(1+s)^2 - 8/(a+4))");
    /*
     * Solved on paper:
     *                      s^3 + 2s^4 + s^5
     *  G(s) = ---------------------------------------------
     *         1 + 2s + s^2 + (-4-x)s^3 + (-2x)s^4 + (-x)s^5
     * where:
     *     x = 8/(a+4)
     *
     * b0 = 0
     * b1 = 0
     * b2 = 0
     * b3 = 1
     * b4 = 2
     * b5 = 1
     * a0 = 1
     * a1 = 2
     * a2 = 1
     * a3 = -4-8/(a+4)
     * a4 = -16/(a+4)
     * a5 = -8/(a+4)
     */
    e->dump("compute_transfer_function.dot");
    TFManipulator m;
    TFManipulator::TFCoefficients tfc = m.calculateTransferFunctionCoefficients(e, "s");

    for (std::size_t i = 0; i != tfc.numerator.size(); ++i)
    {
        std::stringstream ss; ss << "numerator, degree " <<i;
        tfc.numerator[i]->dump("compute_transfer_function.dot", true, ss.str().c_str());
    }
    for (std::size_t i = 0; i != tfc.denominator.size(); ++i)
    {
        std::stringstream ss; ss << "denominator, degree " <<i;
        tfc.denominator[i]->dump("compute_transfer_function.dot", true, ss.str().c_str());
    }

    Reference<VariableTable> vt = new VariableTable;
    vt->set("a", 7.2);
    TransferFunction<double> tf = m.calculateTransferFunction(tfc, vt);
    ASSERT_THAT(tf.numerator().size(), Eq(5));
    ASSERT_THAT(tf.denominator().size(), Eq(5));
}

TEST(NAME, active_lowpass_filter)
{
    /*
     *                    C
     *             +------||------+
     *             |              |
     *             |      G2      |
     *             o-----\/\/\----o
     *             |              |
     * Vin   G1    |  |'-.        |
     * O---\/\/\---o--| -  '-.    |     Vout
     *                |        >--o------O
     *             +--| +  .-'
     *            _|_ |.-'
     *                                               1
     *                                              --- = y2 = G1 + G2 + sC
     * Vin        I2        V2                       z2
     *  o---->----o---->----o-.  -1
     *      G1    |    z2       '-.          Vout
     *            |                 o---->----o
     *            |             .-'      A    |
     *             \        o-'  1           /
     *              '-,                   ,-'
     *                  '-------<-------'
     *                       G2 + sC
     *
     * P1 = -G1*z2*A
     * L1 = -A*(G2+s*C)*z2
     */
    Reference<Expression> e = Expression::parse("P1/(1-L1)");
    e->dump("active_lowpass_filter.dot");

    Reference<VariableTable> vt = new VariableTable;
    vt->set("P1", "-G1*z2*A");
    vt->set("L1", "-A*(G2+s*C)*z2");
    vt->set("z2", "1/(G1 + G2 + s*C)");
    e->insertSubstitutions(vt);
    e->dump("active_lowpass_filter.dot", true, "Substitution");

    TFManipulator m;
    TFManipulator::TFCoefficients tfc = m.calculateTransferFunctionCoefficients(e, "s");
    for (std::size_t i = 0; i != tfc.numerator.size(); ++i)
    {
        std::stringstream ss; ss << "numerator, degree " <<i;
        tfc.numerator[i]->dump("compute_transfer_function.dot", true, ss.str().c_str());
    }
    for (std::size_t i = 0; i != tfc.denominator.size(); ++i)
    {
        std::stringstream ss; ss << "denominator, degree " <<i;
        tfc.denominator[i]->dump("compute_transfer_function.dot", true, ss.str().c_str());
    }


    TransferFunction<double> tf = m.calculateTransferFunction(tfc, vt);
    ASSERT_THAT(tf.numerator().size(), Eq(5));
    ASSERT_THAT(tf.denominator().size(), Eq(5));
}
