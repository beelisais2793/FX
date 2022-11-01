#pragma once

#include "tfp/config.hpp"
#include <string>

namespace tfp {

class Expression;

class ExpressionParser
{
public:
    enum Token
    {
        TOK_NULL,
        TOK_ERROR,
        TOK_END,
        TOK_SEP,
        TOK_OPEN,
        TOK_CLOSE,
        TOK_NUMBER,
        TOK_SYMBOL,
        TOK_INFIX
    };

    bool isAtEnd();
    bool isSymbol();
    bool isNumber();
    bool isOperator();

    void nextToken();

    Expression* base();
    Expression* power();
    Expression* factor();
    Expression* term();
    Expression* expr();
    Expression* list();

    Expression* parse(const char* str);

    Token type_;
    double value_;
    std::string name_;
    double (*function_)(double,double);

    const char* start_;
    const char* next_;
};

}
