#include "tfp/math/ExpressionOptimiser.hpp"
#include "tfp/math/Expression.hpp"

using namespace tfp;

// ----------------------------------------------------------------------------
bool ExpressionOptimiser::removeMultipleNegates(Expression* e)
{
    if (e->type() != Expression::FUNCTION1) return false;
    if (e->isOperation(op::negate) == false || e->hasRHSOperation(op::negate) == false)
        return false;

    e->right()->right()->collapseIntoParent();
    e->right()->collapseIntoParent();
    return true;
}

// ----------------------------------------------------------------------------
bool ExpressionOptimiser::removeUselessAdditions(Expression* e)
{
    if (e->isOperation(op::add) == false)
        return false;

    if (e->left()->type() == Expression::CONSTANT && e->left()->value() == 0.0)
    {
        e->right()->collapseIntoParent();
        return true;
    }
    if (e->right()->type() == Expression::CONSTANT && e->right()->value() == 0.0)
    {
        e->left()->collapseIntoParent();
        return true;
    }
    return false;
}

// ----------------------------------------------------------------------------
bool ExpressionOptimiser::removeUselessSubtractions(Expression* e)
{
    if (e->isOperation(op::sub) == false)
        return false;

    if (e->left()->type() == Expression::CONSTANT && e->left()->value() == 0.0)
    {
        e->set(op::negate, e->right());
        return true;
    }
    if (e->right()->type() == Expression::CONSTANT && e->right()->value() == 0.0)
    {
        e->left()->collapseIntoParent();
        return true;
    }
    return false;
}

// ----------------------------------------------------------------------------
bool ExpressionOptimiser::removeUselessProducts(Expression* e)
{
    if (e->isOperation(op::mul) == false)
        return false;

    if (e->left()->type() == Expression::CONSTANT)
    {
        if (e->left()->value() == 1.0)
        {
            e->right()->collapseIntoParent();
            return true;
        }
        if (e->left()->value() == -1.0)
        {
            e->set(op::negate, e->right());
            return true;;
        }
    }

    if (e->right()->type() == Expression::CONSTANT)
    {
        if (e->right()->value() == 1.0)
        {
            e->left()->collapseIntoParent();
            return true;
        }
        else if (e->right()->value() == -1.0)
        {
            e->set(op::negate, e->left());
            return true;
        }
    }
    return false;
}

// ----------------------------------------------------------------------------
bool ExpressionOptimiser::removeUselessDivisions(Expression* e)
{
    if (e->isOperation(op::div) == false)
        return false;

    if (e->right()->type() == Expression::CONSTANT)
    {
        if (e->right()->value() == 1.0)
        {
            e->left()->collapseIntoParent();
            return true;
        }
        if (e->right()->value() == -1.0)
        {
            e->set(op::negate, e->left());
            return true;
        }
    }

    if (e->left()->isSameAs(e->right()))
    {
        e->set(1.0);
        return true;
    }

    return false;
}

// ----------------------------------------------------------------------------
bool ExpressionOptimiser::removeUselessExponents(Expression* e)
{
    if (e->isOperation(op::pow) == false)
        return false;

    if (e->right()->type() == Expression::CONSTANT && e->right()->value() == 1.0)
    {
        e->left()->collapseIntoParent();
        return true;
    }
    return false;
}
