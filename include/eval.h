#pragma once
#include "special/std_libraries.h"
#include "special/return_codes.h"
#include "expression.h"
#include "exception.h"
#include "variable.h"
#include "scope.h"

class Eval
{
private:
    std::vector<Scope> scopes;

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

    Variable evalIfBlock(Expression expr);
    Variable evalLiteralExpression(Expression expr);
    Variable evalMathOperatorExpression(Expression expr);
    Variable evalVariableDeclaration(Expression expr);
    Variable evalVariableAssignment(Expression expr);

public:
    Eval(std::vector<Expression> expressions_v, bool debug_v = false)
    {
        this->line_number = 1;
        this->expressions = expressions_v;

        std::reverse(this->expressions.begin(), this->expressions.end());

        this->debug = debug_v;
        this->scopes.push_back(Scope());
    }

    Scope &getScope()
    {
        return scopes[this->scopes.size() - 1];
    }

    void
    eval();
    Variable evalExpr(Expression expr, bool scoped = false);
};
