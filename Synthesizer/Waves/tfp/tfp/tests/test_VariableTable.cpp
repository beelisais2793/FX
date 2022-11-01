#include "gmock/gmock.h"
#include "tfp/math/Expression.hpp"
#include "tfp/math/VariableTable.hpp"
#include "tfp/util/Reference.hpp"

#define NAME VariableTable

using namespace testing;
using namespace tfp;

TEST(NAME, add_and_get_constant_values)
{
    VariableTable vt;
    vt.set("a", 4.0);
    vt.set("b", "6.0");
    EXPECT_THAT(vt.valueOf("a"), DoubleEq(4.0));
    EXPECT_THAT(vt.valueOf("b"), DoubleEq(6.0));
}

TEST(NAME, get_value_of_dependent_expressions)
{
    VariableTable vt;
    vt.set("a", "1/b+c");
    vt.set("b", 4.0);
    vt.set("c", "2*d");
    vt.set("d", "2");
    EXPECT_THAT(vt.valueOf("a"), DoubleEq(4.25));
}

TEST(NAME, getting_value_of_non_existent_expression_throws)
{
    VariableTable vt;
    EXPECT_THROW(vt.valueOf("a"), VariableTable::MissingEntryException);
}

TEST(NAME, removing_entires_works)
{
    VariableTable vt;
    vt.set("a", 4.0);
    ASSERT_THAT(vt.get("a"), NotNull());
    ASSERT_THAT(vt.get("a")->value(), DoubleEq(4.0));
    vt.remove("a");
    ASSERT_THAT(vt.get("a"), IsNull());
}

TEST(NAME, removing_non_existent_entries_works)
{
    VariableTable vt;
    ASSERT_NO_THROW(vt.remove("a"));
}

TEST(NAME, clear_works)
{
    VariableTable vt;
    vt.set("a", 4.0);
    vt.set("b", 6.0);
    vt.clear();
    ASSERT_THAT(vt.get("a"), IsNull());
    ASSERT_THAT(vt.get("b"), IsNull());
}

TEST(NAME, merge_two_tables_with_overlap)
{
    VariableTable vt1;
    VariableTable vt2;
    vt1.set("a", 4.0);
    vt1.set("b", 5.0);
    vt2.set("b", 6.0);
    vt2.set("c", 7.0);
    vt1.merge(vt2);
    ASSERT_THAT(vt1.valueOf("a"), DoubleEq(4.0));
    ASSERT_THAT(vt1.valueOf("b"), DoubleEq(6.0));
    ASSERT_THAT(vt1.valueOf("c"), DoubleEq(7.0));
    ASSERT_THAT(vt2.valueOf("b"), DoubleEq(6.0));
    ASSERT_THAT(vt2.valueOf("c"), DoubleEq(7.0));
}

TEST(NAME, merge_empty_tables_doesnt_fuck_shit_up)
{
    VariableTable vt1;
    VariableTable vt2;
    ASSERT_NO_THROW(vt1.merge(vt2));
}

TEST(NAME, cyclic_lookup_throws)
{
    VariableTable vt;
    vt.set("a", "a");
    vt.set("b", "c");
    vt.set("c", "d");
    vt.set("d", "b");
    EXPECT_THROW(vt.valueOf("a"), VariableTable::CyclicDependencyException);
    EXPECT_THROW(vt.valueOf("c"), VariableTable::CyclicDependencyException);
}
