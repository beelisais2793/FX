#include "gmock/gmock.h"
#include "tfp/math/Expression.hpp"
#include "tfp/util/Reference.hpp"

#define NAME ExpressionParser

using namespace testing;
using namespace tfp;

TEST(NAME, simple_test)
{
    tfp::Reference<Expression> e = Expression::parse("a*b+3");
    ASSERT_THAT(e, NotNull());
    EXPECT_THAT(e->parent(), IsNull());
    EXPECT_THAT(e->type(), Eq(Expression::FUNCTION2));
    EXPECT_THAT(e->op2(), Eq(op::add));
    ASSERT_THAT(e->left(), NotNull());
    ASSERT_THAT(e->right(), NotNull());
    EXPECT_THAT(e->right()->type(), Expression::CONSTANT);
    EXPECT_THAT(e->right()->value(), DoubleEq(3.0));
    EXPECT_THAT(e->left()->parent(), Eq(e));
    EXPECT_THAT(e->right()->parent(), Eq(e));
    Expression* f = e->left();
    ASSERT_THAT(f->left(), NotNull());
    ASSERT_THAT(f->right(), NotNull());
    EXPECT_THAT(f->type(), Eq(Expression::FUNCTION2));
    EXPECT_THAT(f->op2(), Eq(op::mul));
    EXPECT_THAT(f->left()->type(), Eq(Expression::VARIABLE));
    EXPECT_THAT(f->left()->name(), StrEq("a"));
    EXPECT_THAT(f->left()->type(), Eq(Expression::VARIABLE));
    EXPECT_THAT(f->right()->name(), StrEq("b"));
    EXPECT_THAT(f->left()->parent(), Eq(f));
    EXPECT_THAT(f->right()->parent(), Eq(f));
}

TEST(NAME, mul_before_add)
{
    tfp::Reference<Expression> e = Expression::parse("a+b*c");
    EXPECT_THAT(e->type(), Eq(Expression::FUNCTION2));
    EXPECT_THAT(e->op2(), Eq(op::add));
    EXPECT_THAT(e->left()->name(), StrEq("a"));
    EXPECT_THAT(e->right()->type(), Eq(Expression::FUNCTION2));
    EXPECT_THAT(e->right()->op2(), Eq(op::mul));
    EXPECT_THAT(e->right()->left()->name(), StrEq("b"));
    EXPECT_THAT(e->right()->right()->name(), StrEq("c"));
}

TEST(NAME, power_before_mul)
{
    tfp::Reference<Expression> e = Expression::parse("a*b^c");
    EXPECT_THAT(e->type(), Eq(Expression::FUNCTION2));
    EXPECT_THAT(e->op2(), Eq(op::mul));
    EXPECT_THAT(e->left()->name(), StrEq("a"));
    EXPECT_THAT(e->right()->type(), Eq(Expression::FUNCTION2));
    EXPECT_THAT(e->right()->op2(), Eq(op::pow));
    EXPECT_THAT(e->right()->left()->name(), StrEq("b"));
    EXPECT_THAT(e->right()->right()->name(), StrEq("c"));
}

TEST(NAME, power_before_add)
{
    tfp::Reference<Expression> e = Expression::parse("a+b^c");
    EXPECT_THAT(e->type(), Eq(Expression::FUNCTION2));
    EXPECT_THAT(e->op2(), Eq(op::add));
    EXPECT_THAT(e->left()->name(), StrEq("a"));
    EXPECT_THAT(e->right()->type(), Eq(Expression::FUNCTION2));
    EXPECT_THAT(e->right()->op2(), Eq(op::pow));
    EXPECT_THAT(e->right()->left()->name(), StrEq("b"));
    EXPECT_THAT(e->right()->right()->name(), StrEq("c"));
}

TEST(NAME, mul_before_sub)
{
    tfp::Reference<Expression> e = Expression::parse("a-b*c");
    EXPECT_THAT(e->type(), Eq(Expression::FUNCTION2));
    EXPECT_THAT(e->op2(), Eq(op::sub));
    EXPECT_THAT(e->left()->name(), StrEq("a"));
    EXPECT_THAT(e->right()->type(), Eq(Expression::FUNCTION2));
    EXPECT_THAT(e->right()->op2(), Eq(op::mul));
    EXPECT_THAT(e->right()->left()->name(), StrEq("b"));
    EXPECT_THAT(e->right()->right()->name(), StrEq("c"));
}

TEST(NAME, power_before_sub)
{
    tfp::Reference<Expression> e = Expression::parse("a-b^c");
    EXPECT_THAT(e->type(), Eq(Expression::FUNCTION2));
    EXPECT_THAT(e->op2(), Eq(op::sub));
    EXPECT_THAT(e->left()->name(), StrEq("a"));
    EXPECT_THAT(e->right()->type(), Eq(Expression::FUNCTION2));
    EXPECT_THAT(e->right()->op2(), Eq(op::pow));
    EXPECT_THAT(e->right()->left()->name(), StrEq("b"));
    EXPECT_THAT(e->right()->right()->name(), StrEq("c"));
}

TEST(NAME, div_before_add)
{
    tfp::Reference<Expression> e = Expression::parse("a+b/c");
    EXPECT_THAT(e->type(), Eq(Expression::FUNCTION2));
    EXPECT_THAT(e->op2(), Eq(op::add));
    EXPECT_THAT(e->left()->name(), StrEq("a"));
    EXPECT_THAT(e->right()->type(), Eq(Expression::FUNCTION2));
    EXPECT_THAT(e->right()->op2(), Eq(op::div));
    EXPECT_THAT(e->right()->left()->name(), StrEq("b"));
    EXPECT_THAT(e->right()->right()->name(), StrEq("c"));
}

TEST(NAME, power_before_div)
{
    tfp::Reference<Expression> e = Expression::parse("a/b^c");
    EXPECT_THAT(e->type(), Eq(Expression::FUNCTION2));
    EXPECT_THAT(e->op2(), Eq(op::div));
    EXPECT_THAT(e->left()->name(), StrEq("a"));
    EXPECT_THAT(e->right()->type(), Eq(Expression::FUNCTION2));
    EXPECT_THAT(e->right()->op2(), Eq(op::pow));
    EXPECT_THAT(e->right()->left()->name(), StrEq("b"));
    EXPECT_THAT(e->right()->right()->name(), StrEq("c"));
}

TEST(NAME, div_before_sub)
{
    tfp::Reference<Expression> e = Expression::parse("a-b/c");
    EXPECT_THAT(e->type(), Eq(Expression::FUNCTION2));
    EXPECT_THAT(e->op2(), Eq(op::sub));
    EXPECT_THAT(e->left()->name(), StrEq("a"));
    EXPECT_THAT(e->right()->type(), Eq(Expression::FUNCTION2));
    EXPECT_THAT(e->right()->op2(), Eq(op::div));
    EXPECT_THAT(e->right()->left()->name(), StrEq("b"));
    EXPECT_THAT(e->right()->right()->name(), StrEq("c"));
}

TEST(NAME, parenthesis_test)
{
    tfp::Reference<Expression> e = Expression::parse("(a+b)*c");
    EXPECT_THAT(e->type(), Eq(Expression::FUNCTION2));
    EXPECT_THAT(e->op2(), Eq(op::mul));
    EXPECT_THAT(e->right()->name(), StrEq("c"));
    EXPECT_THAT(e->left()->type(), Eq(Expression::FUNCTION2));
    EXPECT_THAT(e->left()->op2(), Eq(op::add));
    EXPECT_THAT(e->left()->left()->name(), StrEq("a"));
    EXPECT_THAT(e->left()->right()->name(), StrEq("b"));
}
