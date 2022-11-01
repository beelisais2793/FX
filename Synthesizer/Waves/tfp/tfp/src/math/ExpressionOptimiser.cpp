#include "tfp/math/ExpressionOptimiser.hpp"
#include "tfp/math/Expression.hpp"

using namespace tfp;

// ----------------------------------------------------------------------------
bool ExpressionOptimiser::everything(Expression* e)
{
    bool mutated = false;
    while (constants(e) |
           uselessOperations(e) |
           simplify(e))
    {
        mutated = true;
    }

    return mutated;
}

// ----------------------------------------------------------------------------
bool ExpressionOptimiser::constants(Expression* e)
{
    bool mutated = false;
    while (recursivelyCall(&ExpressionOptimiser::evaluateConstantExpressions, e) |
           recursivelyCall(&ExpressionOptimiser::combineConstants, e))
    {
        mutated = true;
    }

    return mutated;
}

// ----------------------------------------------------------------------------
bool ExpressionOptimiser::uselessOperations(Expression* e)
{
    bool mutated = false;
    while (
        recursivelyCall(&ExpressionOptimiser::removeMultipleNegates, e) |
        recursivelyCall(&ExpressionOptimiser::removeUselessAdditions, e) |
        recursivelyCall(&ExpressionOptimiser::removeUselessSubtractions, e) |
        recursivelyCall(&ExpressionOptimiser::removeUselessProducts, e) |
        recursivelyCall(&ExpressionOptimiser::removeUselessDivisions, e) |
        recursivelyCall(&ExpressionOptimiser::removeUselessExponents, e))
    {
        mutated = true;
    }

    return mutated;
}

// ----------------------------------------------------------------------------
bool ExpressionOptimiser::simplify(Expression* e)
{
    bool mutated = false;
    while (recursivelyCall(&ExpressionOptimiser::simplifySums, e) |
           recursivelyCall(&ExpressionOptimiser::simplifyProducts, e) |
           recursivelyCall(&ExpressionOptimiser::simplifyExponents, e))
    {
        mutated = true;
    }

    return mutated;
}

// ----------------------------------------------------------------------------
bool ExpressionOptimiser::recursivelyCall(bool (ExpressionOptimiser::*optfunc)(Expression*), Expression* e)
{
    WeakReference<Expression> self(e);

    bool mutated = false;
    if (e->left() && recursivelyCall(optfunc, e->left()))
    {
        mutated = true;
        if (self.expired())
            return true;
    }
    if (e->right() && recursivelyCall(optfunc, e->right()))
    {
        mutated = true;
        if (self.expired())
            return true;
    }
    return mutated | (this->*optfunc)(e);
}
