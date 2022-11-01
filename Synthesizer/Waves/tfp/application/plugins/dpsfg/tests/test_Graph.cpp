#include "gmock/gmock.h"
#include "model/Connection.hpp"
#include "model/Graph.hpp"
#include "model/Node.hpp"
#include "tfp/math/Expression.hpp"
#include "tfp/util/Reference.hpp"
#include "tfp/math/VariableTable.hpp"

#define NAME dpsfg_Graph

using namespace testing;
using namespace dpsfg;
using namespace tfp;

TEST(NAME, single_node_forward_path_returns_identity_TF)
{
    Graph graph;
    Node* n = graph.createNode("n");
    graph.setForwardPath(n, n);
    TransferFunction<double> tf = graph.calculateTransferFunction();

    EXPECT_THAT(tf.numerator().size(), Eq(0));
    EXPECT_THAT(tf.denominator().size(), Eq(0));
    EXPECT_THAT(tf.factor(), Eq(1));
}

TEST(NAME, default_connections_are_identity_TF)
{
    Graph graph;
    Node* n1 = graph.createNode("n1");
    Node* n2 = graph.createNode("n2");
    Node* n3 = graph.createNode("n3");
    n1->connectTo(n2);
    n2->connectTo(n3);
    graph.setForwardPath(n1, n3);

    TransferFunction<double> tf = graph.calculateTransferFunction();
    EXPECT_THAT(tf.numerator().size(), Eq(0));
    EXPECT_THAT(tf.denominator().size(), Eq(0));
    EXPECT_THAT(tf.factor(), Eq(1));
}

TEST(NAME, find_single_forward_path)
{
    Graph graph;
    Node* n1 = graph.createNode("n1");
    Node* n2 = graph.createNode("n2");
    Node* n3 = graph.createNode("n3");
    Connection* c1 = n1->connectTo(n2);
    Connection* c2 = n2->connectTo(n3);

    Graph::PathList paths;
    Graph::PathList loops;
    graph.setForwardPath(n1, n3);
    graph.findForwardPathsAndLoops(&paths, &loops);
    EXPECT_THAT(loops.size(), Eq(0u));
    ASSERT_THAT(paths.size(), Eq(1u));
    ASSERT_THAT(paths[0].size(), Eq(2u));
    EXPECT_THAT(paths[0][0].connection_, Eq(c1));
    EXPECT_THAT(paths[0][1].connection_, Eq(c2));
}

TEST(NAME, find_multiple_forward_paths)
{
    Graph graph;
    Node* n1 = graph.createNode("n1");
    Node* n2 = graph.createNode("n2");
    Node* n3 = graph.createNode("n3");
    Node* n4 = graph.createNode("n4");
    Node* n5 = graph.createNode("n5");
    Node* n6 = graph.createNode("n6");
    Node* n7 = graph.createNode("n7");
    Connection* c1 = n1->connectTo(n2);
    Connection* c2 = n2->connectTo(n3);
    Connection* c3 = n3->connectTo(n4);
    Connection* c4 = n4->connectTo(n5);
    Connection* c5 = n2->connectTo(n6);
    Connection* c6 = n6->connectTo(n7);
    Connection* c7 = n7->connectTo(n5);

    /*
     *            --> n6 --> n7 --
     *          /                  v
     * n1 --> n2 --> n3 --> n4 --> n5
     *
     */

    Graph::PathList paths;
    Graph::PathList loops;
    graph.setForwardPath(n1, n5);
    graph.findForwardPathsAndLoops(&paths, &loops);
    EXPECT_THAT(loops.size(), Eq(0u));
    ASSERT_THAT(paths.size(), Eq(2u));

    ASSERT_THAT(paths[0].size(), Eq(4u));
    EXPECT_THAT(paths[0][0].connection_, Eq(c1));
    EXPECT_THAT(paths[0][1].connection_, Eq(c2));
    EXPECT_THAT(paths[0][2].connection_, Eq(c3));
    EXPECT_THAT(paths[0][3].connection_, Eq(c4));

    ASSERT_THAT(paths[1].size(), Eq(4u));
    EXPECT_THAT(paths[1][0].connection_, Eq(c1));
    EXPECT_THAT(paths[1][1].connection_, Eq(c5));
    EXPECT_THAT(paths[1][2].connection_, Eq(c6));
    EXPECT_THAT(paths[1][3].connection_, Eq(c7));
}

TEST(NAME, find_single_loop)
{
    Graph graph;
    Node* n1 = graph.createNode("n1");
    Node* n2 = graph.createNode("n2");
    Node* n3 = graph.createNode("n3");
    Connection* c1 = n1->connectTo(n2);
    Connection* c2 = n2->connectTo(n3);
    Connection* c3 = n3->connectTo(n2);

    Graph::PathList paths;
    Graph::PathList loops;
    graph.setForwardPath(n1, n3);
    graph.findForwardPathsAndLoops(&paths, &loops);
    ASSERT_THAT(loops.size(), Eq(1u));
    ASSERT_THAT(paths.size(), Eq(1u));

    ASSERT_THAT(paths[0].size(), Eq(2u));
    EXPECT_THAT(paths[0][0].connection_, Eq(c1));
    EXPECT_THAT(paths[0][1].connection_, Eq(c2));

    ASSERT_THAT(loops[0].size(), Eq(2u));
    EXPECT_THAT(loops[0][0].connection_, Eq(c2));
    EXPECT_THAT(loops[0][1].connection_, Eq(c3));
}

TEST(NAME, determinant_with_no_loops)
{
    Graph graph;
    Node* n1 = graph.createNode("n1");
    Node* n2 = graph.createNode("n2");
    n1->connectTo(n2)->setExpression(Expression::make("a"));

    Graph::PathList paths;
    Graph::PathList loops;
    graph.setForwardPath(n1, n2);
    graph.findForwardPathsAndLoops(&paths, &loops);
    Reference<Expression> e = graph.calculateDeterminant(loops);

    EXPECT_THAT(e->type(), Eq(Expression::CONSTANT));
    EXPECT_THAT(e->value(), DoubleEq(1));
}

TEST(NAME, determinant_with_one_loop)
{
    Graph graph;
    Node* n1 = graph.createNode("n1");
    Node* n2 = graph.createNode("n2");
    n1->connectTo(n2)->setExpression(Expression::make("a"));
    n2->connectTo(n1)->setExpression(Expression::make("b"));

    Graph::PathList paths;
    Graph::PathList loops;
    graph.setForwardPath(n1, n2);
    graph.findForwardPathsAndLoops(&paths, &loops);
    Reference<Expression> e = graph.calculateDeterminant(loops);

    EXPECT_THAT(e->op2(), Eq(op::sub));
    EXPECT_THAT(e->left()->value(), DoubleEq(1.0));
    EXPECT_THAT(e->right()->op2(), Eq(op::mul));
    EXPECT_THAT(e->right()->left()->name(), StrEq("a"));
    EXPECT_THAT(e->right()->right()->name(), StrEq("b"));
}

TEST(NAME, determinant_with_two_touching_loops)
{
    Graph graph;
    Node* n1 = graph.createNode("n1");
    Node* n2 = graph.createNode("n2");
    Node* n3 = graph.createNode("n3");
    n1->connectTo(n2)->setExpression(Expression::make("a"));
    n2->connectTo(n3)->setExpression(Expression::make("b"));
    n3->connectTo(n2)->setExpression(Expression::make("c"));
    n2->connectTo(n1)->setExpression(Expression::make("d"));

    // L1: b*c
    // L2: a*d

    Graph::PathList paths;
    Graph::PathList loops;
    graph.setForwardPath(n1, n2);
    graph.findForwardPathsAndLoops(&paths, &loops);
    Reference<Expression> e = graph.calculateDeterminant(loops);

    // Easier to just insert numbers and evaluate
    // Determinant should be: 1 - b*c - a*d
    Reference<VariableTable> vt = e->generateVariableTable();
    e->dump("determinant_with_two_touching_loops.dot");
    vt->set("a", 7.0);
    vt->set("b", 13.0);
    vt->set("c", 17.0);
    vt->set("d", 19.0);
    EXPECT_THAT(e->evaluate(vt), DoubleEq(-353));
}

TEST(NAME, determinant_with_two_non_touching_loops)
{
    Graph graph;
    Node* n1 = graph.createNode("n1");
    Node* n2 = graph.createNode("n2");
    Node* n3 = graph.createNode("n3");
    Node* n4 = graph.createNode("n4");
    n1->connectTo(n2)->setExpression(Expression::make("P1"));
    n2->connectTo(n3)->setExpression(Expression::make("L11"));
    n3->connectTo(n2)->setExpression(Expression::make("L12"));
    n1->connectTo(n4)->setExpression(Expression::make("L21"));
    n4->connectTo(n1)->setExpression(Expression::make("L22"));

    /*
     *                   P1
     * ---------------------------------------
     * 1 - L11*L12 - L21*L22 + L11*L12*L21*L22
     */

    Graph::PathList paths;
    Graph::PathList loops;
    graph.setForwardPath(n1, n2);
    graph.findForwardPathsAndLoops(&paths, &loops);
    Reference<Expression> e = graph.calculateDeterminant(loops);

    // Easier to just insert numbers and evaluate
    // Determinant should be: 1 - b*c - a*d
    Reference<VariableTable> vt = e->generateVariableTable();
    e->dump("determinant_with_two_non_touching_loops.dot");
    vt->set("L11", 13.0);
    vt->set("L12", 17.0);
    vt->set("L21", 19.0);
    vt->set("L22", 23.0);
    EXPECT_THAT(e->evaluate(vt), DoubleEq(95920.0));
}

TEST(NAME, determinant_with_three_non_touching_loops)
{
    Graph graph;
    Node* n1 = graph.createNode("n1");
    Node* n2 = graph.createNode("n2");
    Node* n3 = graph.createNode("n3");
    n1->connectTo(n2)->setExpression(Expression::make("P11"));
    n2->connectTo(n3)->setExpression(Expression::make("P12"));
    n1->connectTo(n1)->setExpression(Expression::make("L1"));
    n2->connectTo(n2)->setExpression(Expression::make("L2"));
    n3->connectTo(n3)->setExpression(Expression::make("L3"));

    /*
     * 1 - L1 - L2 - L3 + L1*L2 + L1*L3 + L2*L3 - L1*L2*L3
     */

    Graph::PathList paths;
    Graph::PathList loops;
    graph.setForwardPath(n1, n3);
    graph.findForwardPathsAndLoops(&paths, &loops);
    Reference<Expression> e = graph.calculateDeterminant(loops);

    // Easier to just insert numbers and evaluate
    // Determinant should be: 1 - b*c - a*d
    Reference<VariableTable> vt = e->generateVariableTable();
    e->dump("determinant_with_three_non_touching_loops.dot");
    vt->set("L1", 7.0);
    vt->set("L2", 13.0);
    vt->set("L3", 17.0);
    EXPECT_THAT(e->evaluate(vt), DoubleEq(-1152));
}

TEST(NAME, mason_complicated_test)
{
    /*
     *  f       g               i
     *  <       <               <
     * / \     / \             / \
     * \ /  a  \ /  b       c  \ /  d
     *  o--->---o--->---o--->---o--->---o
     *   \      |   h   |              /
     *    \      \__<__/              /
     *     \            e            /
     *      \___________>___________/
     *
     *
     *          abcd + e(1 - g - hb - i + gi + bhi)
     * ----------------------------------------------------------
     * 1 - f - g - bh - i + fg + fhb + fi + gi + bhi - fgi - fhbi
     */
    Graph graph;
    Node* n1 = graph.createNode("n1");
    Node* n2 = graph.createNode("n2");
    Node* n3 = graph.createNode("n3");
    Node* n4 = graph.createNode("n4");
    Node* n5 = graph.createNode("n5");
    // paths
    n1->connectTo(n2)->setExpression(Expression::make("a"));
    n2->connectTo(n3)->setExpression(Expression::make("b"));
    n3->connectTo(n4)->setExpression(Expression::make("c"));
    n4->connectTo(n5)->setExpression(Expression::make("d"));
    n1->connectTo(n5)->setExpression(Expression::make("e"));
    // loops
    n1->connectTo(n1)->setExpression(Expression::make("f"));
    n2->connectTo(n2)->setExpression(Expression::make("g"));
    n4->connectTo(n4)->setExpression(Expression::make("i"));
    n3->connectTo(n2)->setExpression(Expression::make("h"));

    graph.setForwardPath(n1, n5);
    Reference<Expression> e = graph.mason();
    graph.dump("mason_complicated_test_graph.dot");
    e->dump("mason_complicated_test.dot");

    Reference<VariableTable> vt = e->generateVariableTable();
    vt->set("a", 3);
    vt->set("b", 5);
    vt->set("c", 7);
    vt->set("d", 11);
    vt->set("e", 13);
    vt->set("f", 17);
    vt->set("g", 19);
    vt->set("h", 23);
    vt->set("i", 27);
    EXPECT_THAT(e->evaluate(vt), DoubleEq(46109.0 / -55328.0));
}
