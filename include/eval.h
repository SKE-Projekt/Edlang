#pragma once
#include "special/std_libraries.h"
#include "special/return_codes.h"
#include "native_functions.h"
#include "expression.h"
#include "exception.h"
#include "variable.h"
#include "function.h"
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

    void addNewScope(bool is_shared = true)
    {
        this->scopes.push_back(Scope(is_shared));
    }
    void removeLastScope()
    {
        this->scopes.pop_back();
    }

    Variable searchForVariable(std::string symbol, int line_number, bool assign = false, Variable var = Variable())
    {
        for (int i = (this->scopes.size() - 1); i >= 0; --i)
        {

            if (this->scopes[i].symbolIsFunction(symbol))
            {
                throw Exception(symbol + " jest funkcją nie zmienną", SYMBOL_NOT_DECLARED, line_number);
            }
            if (this->scopes[i].symbolIsVariable(symbol))
            {
                if (assign)
                {
                    this->scopes[i].assignVariable(symbol, var);
                    return var;
                }
                return this->scopes[i].getVariable(symbol, line_number);
            }
        }

        throw Exception("Nieznana nazwa " + symbol, SYMBOL_NOT_DECLARED, line_number);
    }
    Variable &searchForVariableRef(std::string symbol, int line_number)
    {
        for (int i = (this->scopes.size() - 1); i >= 0; --i)
        {

            if (this->scopes[i].symbolIsFunction(symbol))
            {
                throw Exception(symbol + " jest funkcją nie zmienną", SYMBOL_NOT_DECLARED, line_number);
            }
            if (this->scopes[i].symbolIsVariable(symbol))
            {
                return this->scopes[i].getVariableRef(symbol, line_number);
            }
        }

        throw Exception("Nieznana nazwa " + symbol, SYMBOL_NOT_DECLARED, line_number);
    }

    Function searchForFunction(std::string symbol, int line_number)
    {
        for (int i = (this->scopes.size() - 1); i >= 0; --i)
        {

            if (this->scopes[i].symbolIsVariable(symbol))
            {
                throw Exception(symbol + " jest zmienną nie funkcją", SYMBOL_NOT_DECLARED, line_number);
            }
            if (this->scopes[i].symbolIsFunction(symbol))
            {
                return this->scopes[i].getFunction(symbol, line_number);
            }
        }

        throw Exception("Nieznana nazwa " + symbol, SYMBOL_NOT_DECLARED, line_number);
    }

    Variable evalIfBlock(Expression expr);
    Variable evalLiteralExpression(Expression expr);
    Variable evalMathOperatorExpression(Expression expr);
    Variable evalLogicOperatorExpression(Expression expr);
    Variable evalVariableDeclaration(Expression expr);
    Variable evalVariableAssignment(Expression expr);
    Variable evalFunctionDeclaration(Expression expr);
    Variable evalFunctionCall(Expression expr);

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
        return scopes.back();
    }

    void eval();
    Variable evalExpr(Expression expr, bool scoped = false);
};
