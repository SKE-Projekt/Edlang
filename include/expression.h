#pragma once
#include "special/return_codes.h"
#include "special/std_libraries.h"
#include "exception.h"

const std::string ExpressionTypeName[] = {
    "EMPTY",
    "NUMERIC_LITERAL",
    "STRING_LITERAL",
    "LIST_LITERAL",
    "MATH_OPERATOR",
    "LOGIC_OPERATOR",
    "DECLARATION",
    "ASSIGNMENT",
    "SYMBOLIC_VALUE",
    "IF_EXPR",
    "IF_BLOCK_EXPR",
    "ENDIF_EXPR",
    "TYPE",
    "ARG_DECLARED",
    "ARG_PROVIDED",
    "ARG_BLOCK_DECLARED",
    "ARG_BLOCK_PROVIDED",
    "FUNCTION_BODY",
    "FUNCTION_DECLARATION",
    "FUNCTION_CALL",
    "LOOP",
    "LOOP_BODY",
};

enum ExpressionType
{
    EMPTY,
    NUMERIC_LITERAL,
    STRING_LITERAL,
    LIST_LITERAL,
    EXPR_MATH_OPERATOR,
    EXPR_LOGIC_OPERATOR,
    DECLARATION,
    EXPR_ASSIGNMENT,
    SYMBOLIC_VALUE,
    IF_EXPR,
    IF_BLOCK_EXPR,
    ENDIF_EXPR,
    TYPE,
    ARG_DECLARED,
    ARG_PROVIDED,
    ARG_BLOCK_DECLARED,
    ARG_BLOCK_PROVIDED,
    FUNCTION_BODY,
    FUNCTION_DECLARATION,
    FUNCTION_CALL,
    LOOP,
    LOOP_BODY
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

    bool returnsValue()
    {
        switch (this->type)
        {
        case ExpressionType::NUMERIC_LITERAL:
        case ExpressionType::STRING_LITERAL:
        case ExpressionType::SYMBOLIC_VALUE:
        case ExpressionType::EXPR_MATH_OPERATOR:
        case ExpressionType::EXPR_ASSIGNMENT:
        case ExpressionType::IF_BLOCK_EXPR:
        case ExpressionType::FUNCTION_CALL:
        case ExpressionType::EXPR_LOGIC_OPERATOR:
        case ExpressionType::LIST_LITERAL:
            return true;
            break;
        default:
            return false;
        }
    }

    int childCount()
    {
        return this->children.size();
    }

    int getLineNumber()
    {
        return this->line_number;
    }

    std::string getValue()
    {
        return this->value;
    };

    Expression getChild(int idx)
    {
        if (this->children.size() <= idx)
        {
            throw Exception("Niewłaściwa budowa wyrażenia", INCORRECT_EXPRESSION_STRUCTURE, this->line_number);
        }
        return this->children[idx];
    }

    std::vector<Expression> getChildren()
    {
        return this->children;
    }
};

class OperatorExpression : Expression
{
};
