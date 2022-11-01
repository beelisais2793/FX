#pragma once

#include "tfp/config.hpp"
#include "tfp/util/Reference.hpp"
#include "tfp/math/TransferFunction.hxx"
#include <string>
#include <vector>

namespace tfp {

class Expression;
class VariableTable;

class TFP_PUBLIC_API ExpressionManipulator
{
public:
    struct NonConstantExponentException : public std::logic_error
    {
        NonConstantExponentException(const char* msg) : std::logic_error(
            std::string("Variable depends on a non-constant expression! ") + msg
        ) {}
    };

    bool recursivelyCall(bool (ExpressionManipulator::*optfunc)(Expression*,const char*),
                         Expression* e, const char* variable, bool* hasVariable=NULL);

    bool manipulateIntoRationalFunction(Expression* e, const char* variable);
    bool enforceProductLHS(Expression* e, const char* variable);
    bool enforceConstantExponent(Expression* e, const char* variable);
    bool expandConstantExponentsIntoProducts(Expression* e, const char* variable);
    bool factorNegativeExponents(Expression* e, const char* variable);
    bool eliminateDivisionsAndSubtractions(Expression* e, const char* variable);
    bool expand(Expression* e, const char* variable);

    bool factorIn(Expression* e, Expression* toFactor, const Expression* ignore=NULL);
private:
};

class TFP_PUBLIC_API TFManipulator : public ExpressionManipulator
{
public:

    struct TFCoefficients
    {
        typedef std::vector< tfp::Reference<Expression> > Coefficients;
        Coefficients numerator;
        Coefficients denominator;
    };

    Expression* findOrAddLatestDivision(Expression* e);
    bool manipulateIntoRationalFunction(Expression* e, const char* variable);
    bool factorNegativeExponentsToNumerator(Expression* e, Expression* numerator, const char* variable);

    /*!
     * @brief Attempts to manipulate the expression into the standard "Transfer
     * function form" and returns two arrays containing the expressions for
     * each coefficient of the numerator and denominator polynomials.
     *
     *          b0 + b1*s^1 + b2*s^2 + ...
     *   T(s) = --------------------------
     *          a0 + a1*s^1 + a2*s^2 + ...
     *
     * There are all sorts of reasons why this might fail, but if it succeeds,
     * it means you have expressions for all of the polynomial coefficients,
     * which allows you to change variables and re-evaluate the new coefficient
     * values without needing to do this transformation each time. The
     * coefficients are required to build a TransferFunction object, which is
     * required for calculating impulse/step responses, frequency responses,
     * pole-zero diagrams, etc.
     *
     * If this function doesn't succeed, which will be the case if any term
     * that contains the specified variable (usually "s") is combined with an
     * operation that is *not* mul/div/add/sub with a variable operand (such as
     * the expression s^a) -- or in other words, if the operation containing
     * the specified variable is not reducible to polynomial form -- the
     * expression will be in a modified state different from the initial
     * condition, but mathematically equivalent. In this case, no expressions
     * exist for the polynomial coefficients and the transfer function will
     * have to perform this manipulation every time a variable changes.
     *
     * It is worth noting that DPSFGs will always produce expressions reducible
     * to polynomial expressions. User input may not.
     */
    TFCoefficients calculateTransferFunctionCoefficients(Expression* e, const char* variable);

    /*!
     * @brief Evaluates all coefficient expressions and constructs a transfer
     * function.
     */
    tfp::TransferFunction<double> calculateTransferFunction(const TFCoefficients& tfe,
                                                            const VariableTable* vt=NULL);
private:
};

}
