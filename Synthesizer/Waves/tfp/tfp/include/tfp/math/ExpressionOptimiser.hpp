#pragma once

#include "tfp/config.hpp"
#include "tfp/util/Reference.hpp"

namespace tfp {

class Expression;

class TFP_PUBLIC_API ExpressionOptimiser
{
public:
    bool everything(Expression* e);
    bool constants(Expression* e);
    bool uselessOperations(Expression* e);
    bool simplify(Expression* e);

private:
    bool recursivelyCall(bool (ExpressionOptimiser::*optfunc)(Expression*), Expression* e);

    // UselessOperations
    bool removeMultipleNegates(Expression* e);
    bool removeUselessAdditions(Expression* e);
    bool removeUselessSubtractions(Expression* e);
    bool removeUselessProducts(Expression* e);
    bool removeUselessDivisions(Expression* e);
    bool removeUselessExponents(Expression* e);

    // Simplifications
    bool simplifySums(Expression* e);
    bool simplifyProducts(Expression* e);
    bool simplifyExponents(Expression* e);

    // Constants
    bool evaluateConstantExpressions(Expression* e);
    bool combineConstants(Expression* e);
};

}
