#include "tfp/math/ExpressionOptimiser.hpp"
#include "tfp/math/Expression.hpp"

using namespace tfp;

// ----------------------------------------------------------------------------
bool ExpressionOptimiser::simplifySums(Expression* e)
{
    // Travel up the chain of op::adds...
    Expression* top = e->travelUpChain(op::add);
    if (top == NULL)
        return false;

    /*
     * Two possibilities: Either we are a constant/variable/expression that is
     * on the LHS/RHS of op::add, in which case we are looking for a match
     * with ourselves, or, we are an op::mul operator, in which case either our
     * LHS or RHS needs to be matched with another part of the tree so we can
     * increase our factor by 1.
     */
    if (e->isOperation(op::mul))
    {
        // Try matching our LHS
        Expression* same = top->findSameDownChain(op::add, e->left());
        if (same != NULL)
        {
            if (e->right()->type() == Expression::CONSTANT)
                e->right()->set(e->right()->value() + 1.0);
            else
                e->right()->set(op::add, e->right(), Expression::make(1.0));
            same->getOtherOperand()->collapseIntoParent();
            return true;
        }
        // Try matching our RHS
        same = top->findSameDownChain(op::add, e->right());
        if (same != NULL)
        {
            if (e->left()->type() == Expression::CONSTANT)
                e->left()->set(e->left()->value() + 1.0);
            else
                e->left()->set(op::add, e->left(), Expression::make(1.0));
            same->getOtherOperand()->collapseIntoParent();
            return true;
        }
        return false;
    }

    // We are the expression to match
    Expression* same = top->findSameDownChain(op::add, e);
    if (same == NULL)
        return false;

    e->set(op::mul, e, Expression::make(2.0));
    same->getOtherOperand()->collapseIntoParent();
    return true;
}

// ----------------------------------------------------------------------------
bool ExpressionOptimiser::simplifyProducts(Expression* e)
{
    // Travel up the chain of op::muls...
    Expression* top = e->travelUpChain(op::mul);
    if (top == NULL)
        return false;

    /*
     * Two possibilities: Either we are a constant/variable/expression that is
     * on the LHS/RHS of op::mul, in which case we are looking for a match
     * with ourselves, or, we are an op::pow operator, in which case our LHS
     * needs to be matched with another part of the tree so we can increase our
     * exponent by 1.0.
     */
    Expression* toMatch = e;
    if (e->isOperation(op::pow))
        toMatch = e->left();

    // And search downwards again for any expression node that is the same
    // as us
    Expression* same = top->findSameDownChain(op::mul, toMatch);
    if (same == NULL)
        return false;

    if (e->isOperation(op::pow))
    {
        if (e->right()->type() == Expression::CONSTANT)
            e->right()->set(e->right()->value() + 1.0);
        else
            e->right()->set(op::add, e->right(), Expression::make(1.0));
    }
    else
    {
        e->set(op::pow, e, Expression::make(2.0));
    }
    same->getOtherOperand()->collapseIntoParent();
    return true;
}

// ----------------------------------------------------------------------------
bool ExpressionOptimiser::simplifyExponents(Expression* e)
{
    if (e->isOperation(op::pow) == false)
        return false;

    // Travel up the chain of op::muls...
    Expression* top = e->travelUpChain(op::mul);
    if (top == NULL)
        return false;

    // And search downwards again for any expression that has an LHS the same
    // as our LHS
    Expression* same = top->findOpWithSameLHS(op::pow, e->left());
    if (same == NULL)
        return false;

    e->right()->set(op::add, e->right(), same->right());
    same->getOtherOperand()->collapseIntoParent();
    return true;
}
