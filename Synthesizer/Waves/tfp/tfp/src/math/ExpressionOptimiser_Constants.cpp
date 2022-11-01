#include "tfp/math/ExpressionOptimiser.hpp"
#include "tfp/math/Expression.hpp"

using namespace tfp;

// ----------------------------------------------------------------------------
bool ExpressionOptimiser::evaluateConstantExpressions(Expression* e)
{
    if (e->type() == Expression::FUNCTION1 &&
        e->right()->type() == Expression::CONSTANT)
    {
        e->set(e->evaluate());
        return true;
    }

    if (e->type() == Expression::FUNCTION2 &&
        e->left()->type() == Expression::CONSTANT &&
        e->right()->type() == Expression::CONSTANT)
    {
        e->set(e->evaluate());
        return true;
    }
    return false;
}

// ----------------------------------------------------------------------------
bool ExpressionOptimiser::combineConstants(Expression* e)
{
    if (e->type() != Expression::FUNCTION2)
        return false;
    if (e->left()->type() != Expression::CONSTANT &&
        e->right()->type() != Expression::CONSTANT)  // Need at least one constant operand
        return false;

    if (e->isOperation(op::add))
    {
        // Travel up the chain of op::adds...
        Expression* top = e->travelUpChain(op::add);
        if (top == NULL)
            return false;

        // And search downwards again for any expression node that has a constant
        Expression* same = top->find<
            Expression::MatchOperandType<Expression::CONSTANT>,
            Expression::RecurseOnOp2<op::add>>(e);
        if (same == NULL)
            return false;

        Expression* lhs      = e->left()->type()    == Expression::CONSTANT ? e->left()     : e->right();
        Expression* rhs      = same->left()->type() == Expression::CONSTANT ? same->left()  : same->right();
        Expression* collapse = same->left()->type() == Expression::CONSTANT ? same->right() : same->left();

        lhs->set(lhs->value() + rhs->value());
        collapse->collapseIntoParent();
        return true;
    }

    if (e->isOperation(op::mul))
    {
        // Travel up the chain of op::muls...
        Expression* top = e->travelUpChain(op::mul);
        if (top == NULL)
            return false;

        // And search downwards again for any expression node that has a constant
        Expression* same = top->find<
            Expression::MatchOperandType<Expression::CONSTANT>,
            Expression::RecurseOnOp2<op::mul>>(e);
        if (same == NULL)
            return false;

        Expression* lhs      = e->left()->type()    == Expression::CONSTANT ? e->left()     : e->right();
        Expression* rhs      = same->left()->type() == Expression::CONSTANT ? same->left()  : same->right();
        Expression* collapse = same->left()->type() == Expression::CONSTANT ? same->right() : same->left();

        lhs->set(lhs->value() * rhs->value());
        collapse->collapseIntoParent();
        return true;
    }

    return false;
}
