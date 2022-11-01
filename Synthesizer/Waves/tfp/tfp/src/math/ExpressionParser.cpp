#include "tfp/math/ExpressionParser.hpp"
#include "tfp/math/Expression.hpp"
#include <cstring>

using namespace tfp;

// ----------------------------------------------------------------------------
bool ExpressionParser::isAtEnd()
{
    if (*next_ == '\0')
        return true;
    return false;
}

// ----------------------------------------------------------------------------
bool ExpressionParser::isSymbol()
{
    if (isalpha(*next_))
        return true;
    if (*next_ == '_')
        return true;
    return false;
}

// ----------------------------------------------------------------------------
bool ExpressionParser::isNumber()
{
    return isdigit(*next_) || *next_ == '.';
}

// ----------------------------------------------------------------------------
bool ExpressionParser::isOperator()
{
    if (strchr("+-*/^", *next_))
        return true;
    return false;
}

// ----------------------------------------------------------------------------
void ExpressionParser::nextToken()
{
    type_ = TOK_NULL;

    do
    {
        if (*next_ == '\0')
        {
            type_ = TOK_END;
            return;
        }

        if (isNumber())
        {
            type_ = TOK_NUMBER;
            value_ = strtod(next_, (char**)&next_);
            continue;
        }

        if (isSymbol())
        {
            type_ = TOK_SYMBOL;
            name_.clear();
            do
                name_ += *next_++;
            while (isSymbol() || isdigit(*next_));
            continue;
        }

        switch (*next_++)
        {
            case '+': type_ = TOK_INFIX; function_ = op::add; break;
            case '-': type_ = TOK_INFIX; function_ = op::sub; break;
            case '*': type_ = TOK_INFIX; function_ = op::mul; break;
            case '/': type_ = TOK_INFIX; function_ = op::div; break;
            case '^': type_ = TOK_INFIX; function_ = op::pow; break;
            case '%': type_ = TOK_INFIX; function_ = op::mod; break;
            case '(': type_ = TOK_OPEN; break;
            case ')': type_ = TOK_CLOSE; break;
            case ',': type_ = TOK_SEP; break;
            case ' ': case '\t': case '\n': case '\r': continue;
            default: type_ = TOK_ERROR; break;
        }
    } while (type_ == TOK_NULL);
}

// ----------------------------------------------------------------------------
Expression* ExpressionParser::base()
{
    switch (type_)
    {
        case TOK_NUMBER:
        {
            double value = value_;
            nextToken();
            return Expression::make(value);
        }

        case TOK_SYMBOL:
        {
            std::string name = name_;
            nextToken();
            return Expression::make(name.c_str());
        }

        case TOK_OPEN:
        {
            nextToken();
            Expression* ret = expr();
            if (type_ != TOK_CLOSE)
                type_ = TOK_ERROR;
            else
                nextToken();
            return ret;
        }

        default:
            return new Expression;
    }
}

// ----------------------------------------------------------------------------
Expression* ExpressionParser::power()
{
    int sign = 1;
    while (type_ == TOK_INFIX && (function_ == op::add || function_ == op::sub))
    {
        if (function_ == op::sub)
            sign = -sign;
        nextToken();
    }

    if (sign == 1)
        return base();
    else
        return Expression::make(op::negate, base());
}

// ----------------------------------------------------------------------------
Expression* ExpressionParser::factor()
{
    Expression* ret = power();

    while (type_ == TOK_INFIX && (function_ == op::pow))
    {
        double (*f)(double,double) = function_;
        nextToken();
        ret = Expression::make(f, ret, power());
    }

    return ret;
}

// ----------------------------------------------------------------------------
Expression* ExpressionParser::term()
{
    Expression* ret = factor();

    while (type_ == TOK_INFIX && (function_ == op::mul || function_ == op::div || function_ == op::mod))
    {
        double (*f)(double,double) = function_;
        nextToken();
        ret = Expression::make(f, ret, factor());
    }

    return ret;
}

// ----------------------------------------------------------------------------
Expression* ExpressionParser::expr()
{
    Expression* ret = term();

    while (type_ == TOK_INFIX && (function_ == op::add || function_ == op::sub))
    {
        double (*f)(double,double) = function_;
        nextToken();
        ret = Expression::make(f, ret, term());
    }

    return ret;
}

// ----------------------------------------------------------------------------
Expression* ExpressionParser::list()
{
    Expression* ret = expr();

    while (type_ == TOK_SEP)
    {
        nextToken();
        ret = Expression::make(op::comma, ret, expr());
    }

    return ret;
}

// ----------------------------------------------------------------------------
Expression* ExpressionParser::parse(const char* str)
{
    start_ = next_ = str;
    nextToken();

    Expression* result = expr();
    if (type_ != TOK_END)
    {
        delete result;
        return NULL;
    }

    return result;
}
