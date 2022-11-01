#include "gmock/gmock.h"
#include "model/Connection.hpp"
#include "model/Graph.hpp"
#include "model/Node.hpp"
#include "tfp/util/Reference.hpp"
#include "tfp/math/VariableTable.hpp"

#define NAME dpsfg_Node

using namespace testing;
using namespace dpsfg;

TEST(NAME, node_connect_to_another_node)
{
    Graph graph;
    Node* n1 = graph.createNode("n1");
    Node* n2 = graph.createNode("n2");
    Connection* c = n1->connectTo(n2);

    ASSERT_THAT(c, NotNull());
    EXPECT_THAT(c->getTargetNode(), Eq(n2));
    ASSERT_THAT(n1->getOutgoingConnections().size(), Eq(1u));
    EXPECT_THAT(n1->getOutgoingConnections()[0].get(), Eq(c));
    EXPECT_THAT(n2->getOutgoingConnections().size(), Eq(0u));
}

TEST(NAME, node_connect_from_another_node)
{
    Graph graph;
    Node* n1 = graph.createNode("n1");
    Node* n2 = graph.createNode("n2");
    Connection* c = n2->connectFrom(n1);

    ASSERT_THAT(c, NotNull());
    EXPECT_THAT(c->getTargetNode(), Eq(n2));
    ASSERT_THAT(n1->getOutgoingConnections().size(), Eq(1u));
    EXPECT_THAT(n1->getOutgoingConnections()[0].get(), Eq(c));
    EXPECT_THAT(n2->getOutgoingConnections().size(), Eq(0u));
}

TEST(NAME, node_connect_to_self)
{
    Graph graph;
    Node* n = graph.createNode("n");
    Connection* c = n->connectTo(n);

    ASSERT_THAT(c, NotNull());
    EXPECT_THAT(c->getTargetNode(), Eq(n));
    ASSERT_THAT(n->getOutgoingConnections().size(), Eq(1u));
    EXPECT_THAT(n->getOutgoingConnections()[0].get(), Eq(c));

    n->disconnectFrom(n);  // avoid memory leaks
}

TEST(NAME, two_node_loop)
{
    Graph graph;
    Node* n1 = graph.createNode("n1");
    Node* n2 = graph.createNode("n2");
    n1->connectTo(n2);
    n1->connectFrom(n2);

    EXPECT_THAT(n1->getOutgoingConnections()[0]->getTargetNode()
                  ->getOutgoingConnections()[0]->getTargetNode(), Eq(n1));

    n1->disconnectFrom(n2); // avoid memory leaks
}
