#pragma once
#include "special/std_libraries.h"
#include "special/return_codes.h"
#include "expression.h"
#include "exception.h"
#include "variable.h"

class Eval
{
private:
    int line_number;
    std::vector<Expression> expressions;

    bool debug;

    Expression nextExpr()
    {
        if (this->expressions.empty())
        {
            throw Exception("Spodziewano się kolejnego wyrażenia", EXPR_IS_MISSING, this->line_number);
        }

        auto expr = this->expressions.back();
        this->expressions.pop_back();
        this->line_number = expr.getLineNumber();
        return expr;
    }

    Variable evalLiteralExpression(Expression expr);
    Variable evalMathOperatorExpression(Expression expr);

public:
    Eval(std::vector<Expression> expressions_v, bool debug_v = false)
    {
        this->line_number = 1;
        this->expressions = expressions_v;

        this->debug = debug_v;
    }

    void eval();
    Variable evalExpr(Expression expr, bool scoped = false);
};
