#pragma once
#include "special/return_codes.h"
#include "special/std_libraries.h"
#include "exception.h"

const std::string ExpressionTypeName[] = {
    "EMPTY",
    "NUMERIC_LITERAL",
    "STRING_LITERAL",
    "MATH_OPERATOR",
    "DECLARATION",
    "ASSIGNMENT",
    "SYMBOLIC_VALUE",
};

enum ExpressionType
{
    EMPTY,
    NUMERIC_LITERAL,
    STRING_LITERAL,
    EXPR_MATH_OPERATOR,
    DECLARATION,
    EXPR_ASSIGNMENT,
    SYMBOLIC_VALUE,
};

class Expression
{
protected:
    int line_number;
    std::string value;
    ExpressionType type;

    std::vector<Expression> children;

public:
    Expression(ExpressionType type_v, int line_number_v, std::string value_v = "")
    {
        this->type = type_v;
        this->value = value_v;
        this->line_number = line_number_v;
    }

    ExpressionType getType()
    {
        return this->type;
    }

    void addChild(Expression expr)
    {
        this->children.push_back(expr);
    }

    void printExpression(int depth = 0);
};

class OperatorExpression : Expression
{
};
