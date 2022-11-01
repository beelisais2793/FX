#include "gmock/gmock.h"
#include "tfp/math/Expression.hpp"
#include "tfp/util/Reference.hpp"

#define NAME ExpressionOptimiser

using namespace testing;
using namespace tfp;

TEST(NAME, simplify_constant_expressions)
{
    tfp::Reference<Expression> e = Expression::make(op::add,
        Expression::make(4.0),
        Expression::make(op::mul,
            Expression::make(2.0),
            Expression::make(3.0)));
    e->optimise();
    ASSERT_THAT(e->type(), Eq(Expression::CONSTANT));
    EXPECT_THAT(e->value(), DoubleEq(10.0));

    ASSERT_THAT(e->checkParentConsistencies(), Eq(true));
}

TEST(NAME, simplify_constant_expression_with_negate)
{
    Reference<Expression> e = Expression::make(op::pow,
        Expression::make("a"),
        Expression::make(op::negate,
            Expression::make(1.0)));
    e->optimise();
    ASSERT_THAT(e->type(), Eq(Expression::FUNCTION2));
    ASSERT_THAT(e->right()->value(), DoubleEq(-1));

    ASSERT_THAT(e->checkParentConsistencies(), Eq(true));
}

TEST(NAME, simplify_constant_expressions_exponent)
{
    tfp::Reference<Expression> e = Expression::parse("2^5");
    ASSERT_THAT(e->type(), Eq(Expression::CONSTANT));
    EXPECT_THAT(e->value(), DoubleEq(32.0));

    ASSERT_THAT(e->checkParentConsistencies(), Eq(true));
}

TEST(NAME, simplify_constant_expressions_with_variables)
{
    tfp::Reference<Expression> e = Expression::make(op::mul,
        Expression::make("a"),
        Expression::make(op::add,
            Expression::make(3.0),
            Expression::make(2.0)));
    e->optimise();
    ASSERT_THAT(e->op2(), Eq(op::mul));
    ASSERT_THAT(e->right()->type(), Eq(Expression::CONSTANT));
    EXPECT_THAT(e->right()->value(), DoubleEq(5.0));

    ASSERT_THAT(e->checkParentConsistencies(), Eq(true));
}

TEST(NAME, constant_addition_chain_with_variable_in_middle)
{
    Reference<Expression> e = Expression::make(op::add,
        Expression::make(1.0),
        Expression::make(op::add,
            Expression::make("a"),
            Expression::make(1.0)));
    e->optimise();
    ASSERT_THAT(e->left()->name(), StrEq("a"));
    ASSERT_THAT(e->right()->value(), DoubleEq(2.0));

    ASSERT_THAT(e->checkParentConsistencies(), Eq(true));
}

TEST(NAME, constant_subtraction_chain_with_variable_in_middle)
{
    Reference<Expression> e = Expression::make(op::sub,
        Expression::make(1.0),
        Expression::make(op::sub,
            Expression::make("a"),
            Expression::make(1.0)));
    e->optimise();
    ASSERT_THAT(e->op2(), Eq(op::sub));
    ASSERT_THAT(e->left()->type(), Eq(Expression::VARIABLE));
    ASSERT_THAT(e->left()->name(), StrEq("a"));
    ASSERT_THAT(e->right()->value(), DoubleEq(2.0));

    ASSERT_THAT(e->checkParentConsistencies(), Eq(true));
}

TEST(NAME, constant_multiplication_chain_with_variable_in_middle)
{
    Reference<Expression> e = Expression::make(op::mul,
        Expression::make(2.0),
        Expression::make(op::mul,
            Expression::make("a"),
            Expression::make(2.0)));
    e->dump("optimise_multiplication_chain_with_variable_in_middle.dot");
    e->optimise();
    e->dump("optimise_multiplication_chain_with_variable_in_middle.dot", true);
    ASSERT_THAT(e->left()->name(), StrEq("a"));
    ASSERT_THAT(e->right()->value(), DoubleEq(4.0));

    ASSERT_THAT(e->checkParentConsistencies(), Eq(true));
}

TEST(NAME, constant_division_chain_with_variable_in_middle)
{
    Reference<Expression> e = Expression::make(op::div,
        Expression::make(2.0),
        Expression::make(op::mul,
            Expression::make("a"),
            Expression::make(2.0)));
    e->optimise();
    ASSERT_THAT(e->left()->type(), Eq(Expression::VARIABLE));
    ASSERT_THAT(e->left()->name(), StrEq("a"));
    ASSERT_THAT(e->right()->value(), DoubleEq(16.0));

    ASSERT_THAT(e->checkParentConsistencies(), Eq(true));
}

TEST(NAME, constant_addition_chain_with_variable_at_end)
{
    Reference<Expression> e = Expression::make(op::add,
        Expression::make(1.0),
        Expression::make(op::add,
            Expression::make(1.0),
            Expression::make("a")));
    e->optimise();
    ASSERT_THAT(e->left()->value(), DoubleEq(2.0));
    ASSERT_THAT(e->right()->name(), StrEq("a"));

    ASSERT_THAT(e->checkParentConsistencies(), Eq(true));
}

TEST(NAME, constant_subtraction_chain_with_variable_at_end)
{
    Reference<Expression> e = Expression::make(op::sub,
        Expression::make(2.0),
        Expression::make(op::sub,
            Expression::make(1.0),
            Expression::make("a")));
    e->optimise();
    ASSERT_THAT(e->op2(), Eq(op::add));
    ASSERT_THAT(e->left()->name(), StrEq("a"));
    ASSERT_THAT(e->right()->value(), DoubleEq(1.0));

    ASSERT_THAT(e->checkParentConsistencies(), Eq(true));
}

TEST(NAME, constant_multiplication_chain_with_variable_at_end)
{
    Reference<Expression> e = Expression::make(op::mul,
        Expression::make(2.0),
        Expression::make(op::mul,
            Expression::make(2.0),
            Expression::make("a")));
    e->optimise();
    ASSERT_THAT(e->left()->value(), DoubleEq(4.0));
    ASSERT_THAT(e->right()->name(), StrEq("a"));

    ASSERT_THAT(e->checkParentConsistencies(), Eq(true));
}

TEST(NAME, constant_division_chain_with_variable_at_end)
{
    Reference<Expression> e = Expression::make(op::div,
        Expression::make(2.0),
        Expression::make(op::mul,
            Expression::make(2.0),
            Expression::make("a")));
    e->optimise();
    ASSERT_THAT(e->left()->type(), Eq(Expression::VARIABLE));
    ASSERT_THAT(e->left()->name(), StrEq("a"));
    ASSERT_THAT(e->right()->value(), DoubleEq(4.0));

    ASSERT_THAT(e->checkParentConsistencies(), Eq(true));
}

TEST(NAME, constant_recursive_redundant_additions)
{
    Reference<Expression> e = Expression::make(op::add,
        Expression::make(op::add,
            Expression::make(5.0),
            Expression::make("a")),
        Expression::make(op::add,
            Expression::make(3.0),
            Expression::make("b")));
    e->dump("constant_recursive_redundant_additions.dot");
    e->optimise();
    e->dump("constant_recursive_redundant_additions.dot", true);

    ASSERT_THAT(e->op2(), Eq(op::add));
    ASSERT_THAT(e->right()->name(), StrEq("b"));
    ASSERT_THAT(e->left()->op2(), Eq(op::add));
    ASSERT_THAT(e->left()->left()->value(), DoubleEq(8.0));
    ASSERT_THAT(e->left()->right()->name(), StrEq("a"));

    ASSERT_THAT(e->checkParentConsistencies(), Eq(true));
}

TEST(NAME, constant_recursive_redundant_multiplications)
{
    Reference<Expression> e = Expression::make(op::mul,
        Expression::make(op::mul,
            Expression::make(3.0),
            Expression::make("a")),
        Expression::make(op::mul,
            Expression::make(5.0),
            Expression::make("b")));
    e->dump("optimise_recursive_redundant_multiplications.dot");
    e->optimise();
    e->dump("optimise_recursive_redundant_multiplications.dot", true);

    ASSERT_THAT(e->op2(), Eq(op::mul));
    ASSERT_THAT(e->right()->name(), StrEq("b"));
    ASSERT_THAT(e->left()->op2(), Eq(op::mul));
    ASSERT_THAT(e->left()->left()->value(), DoubleEq(15.0));
    ASSERT_THAT(e->left()->right()->name(), StrEq("a"));

    ASSERT_THAT(e->checkParentConsistencies(), Eq(true));
}

TEST(NAME, constant_chain_of_additions_with_2_variables_in_middle)
{
    Reference<Expression> e = Expression::make(op::add,
        Expression::make(5.0),
        Expression::make(op::add,
            Expression::make("a"),
            Expression::make(op::add,
                Expression::make("b"),
                Expression::make(8.0))));
    e->dump("constant_chain_of_additions_with_2_variables_in_middle.dot");
    e->optimise();
    e->dump("constant_chain_of_additions_with_2_variables_in_middle.dot", true);
    ASSERT_THAT(e->op2(), Eq(op::add));
    ASSERT_THAT(e->left()->name(), StrEq("a"));
    ASSERT_THAT(e->right()->left()->name(), StrEq("b"));
    ASSERT_THAT(e->right()->right()->value(), DoubleEq(13.0));

    ASSERT_THAT(e->checkParentConsistencies(), Eq(true));
}

TEST(NAME, constant_chain_of_products_with_2_variables_in_middle)
{
    Reference<Expression> e = Expression::make(op::mul,
        Expression::make(3.0),
        Expression::make(op::mul,
            Expression::make("a"),
            Expression::make(op::mul,
                Expression::make("b"),
                Expression::make(2.0))));
    e->dump("constant_chain_of_products_with_2_variables_in_middle.dot");
    e->optimise();
    e->dump("constant_chain_of_products_with_2_variables_in_middle.dot", true);
    ASSERT_THAT(e->op2(), Eq(op::mul));
    ASSERT_THAT(e->left()->name(), StrEq("a"));
    ASSERT_THAT(e->right()->left()->name(), StrEq("b"));
    ASSERT_THAT(e->right()->right()->value(), DoubleEq(6.0));

    ASSERT_THAT(e->checkParentConsistencies(), Eq(true));
}
