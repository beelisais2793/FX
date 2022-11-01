#include "gmock/gmock.h"
#include "tfp/math/Expression.hpp"
#include "tfp/math/VariableTable.hpp"

#define NAME Expression

using namespace testing;
using namespace tfp;

TEST(NAME, generate_variable_table)
{
    Reference<Expression> e = Expression::parse("a+b^c*d");
    Reference<VariableTable> vt = e->generateVariableTable();
    EXPECT_THAT(vt->valueOf("a"), DoubleEq(0));
    EXPECT_THAT(vt->valueOf("b"), DoubleEq(0));
    EXPECT_THAT(vt->valueOf("c"), DoubleEq(1));
    EXPECT_THAT(vt->valueOf("d"), DoubleEq(1));
}

TEST(NAME, evaluate_constant_expression)
{
    Reference<Expression> e = Expression::parse("(2+3*4)^2 + 4");
    EXPECT_THAT(e->evaluate(), DoubleEq(200));
}

TEST(NAME, evaluate_with_variables)
{
    Reference<Expression> e = Expression::parse("(a+3*c)^d + e");
    Reference<VariableTable> vt = new VariableTable;
    vt->set("a", 2);
    vt->set("c", 4);
    vt->set("d", 2);
    vt->set("e", 5);
    EXPECT_THAT(e->evaluate(vt), DoubleEq(201));
}

TEST(NAME, evaluate_with_missing_variable_fails)
{
    Reference<Expression> e = Expression::parse("a");
    EXPECT_DEATH(e->evaluate(), ".*");
}

TEST(NAME, clone_actually_deep_copies)
{
    Reference<Expression> e1 = Expression::parse("a+b+c");
    Reference<Expression> e2 = e1->clone();
    ASSERT_THAT(e1->op2(), Eq(e2->op2()));
    ASSERT_THAT(e1->right(), Ne(e2->right()));
    ASSERT_THAT(e1->right()->name(), StrEq(e2->right()->name()));
    ASSERT_THAT(e1->left(), Ne(e2->left()));
    ASSERT_THAT(e1->left()->op2(), Eq(e2->left()->op2()));
    ASSERT_THAT(e1->left()->left(), Ne(e2->left()->left()));
    ASSERT_THAT(e1->left()->left()->name(), StrEq(e2->left()->left()->name()));
    ASSERT_THAT(e1->left()->right()->name(), StrEq(e2->left()->right()->name()));
}

TEST(NAME, substitute_simple)
{
    Reference<Expression> e1 = Expression::make("a");
    Reference<VariableTable> vt = new VariableTable;
    vt->set("a", "b");
    vt->set("b", "c");
    e1->insertSubstitutions(vt);
    ASSERT_THAT(e1->name(), StrEq("c"));
}

TEST(NAME, substitute_with_cycles_fails)
{
    Reference<Expression> e1 = Expression::make("a");
    Reference<VariableTable> vt = new VariableTable;
    vt->set("a", "b");
    vt->set("b", "c");
    vt->set("c", "d");
    vt->set("d", "b");
    EXPECT_THROW(e1->insertSubstitutions(vt), std::runtime_error);
}
