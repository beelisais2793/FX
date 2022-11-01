#pragma once

#include "tfp/util/Reference.hpp"
#include "tfp/math/Expression.hpp"
#include <QString>

// remove when moving implementations into cpp file
#include "model/Node.hpp"

namespace dpsfg {

class Node;

class Connection
{
public:
    void setExpression(tfp::Expression* expression) { expression_ = expression; }
    tfp::Expression* getExpression() const { return expression_; }
    void setTargetNode(Node* node) { outgoingNode_ = node; }
    Node* getTargetNode() const { return outgoingNode_; }

private:
    Node*                      outgoingNode_;
    tfp::Reference<tfp::Expression> expression_;
};

}
