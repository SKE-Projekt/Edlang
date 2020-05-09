#pragma once
#include "special/std_libraries.h"
#include "special/return_codes.h"
#include "expression.h"
#include "exception.h"
#include "variable.h"
#include "function.h"

class Scope
{
private:
    bool shared;
    std::map<std::string, Variable> variables;
    std::map<std::string, Function> functions;

public:
    Scope() { this->shared = true; }
    Scope(bool shared_v)
    {
        this->shared = shared_v;
    }

    void printScope()
    {
        std::cout << "SCOPE: " << std::endl;
        std::cout << "VARIABLES:" << std::endl;
        for (auto v : this->variables)
        {
            std::cout << v.first << ": " << std::endl;
            v.second.printVariable();
        }
        std::cout << "FUNCTIONS:" << std::endl;
        for (auto f : this->functions)
        {
            std::cout << f.first << ": " << std::endl;
            f.second.printFunction();
        }
    }

    bool symbolExists(std::string symbol)
    {
        return this->variables.count(symbol) || this->functions.count(symbol);
    }

    void declareVariable(std::string symbol, Variable var)
    {
        if (this->symbolExists(symbol))
        {
            throw Exception("Próba re-deklaracji nazwy " + symbol, REDECLARATION_OF_SYMBOL, var.getLineNumber());
        }
        this->variables[symbol] = var;
        this->variables[symbol].undefy();
    }

    void declareFunction(Expression expr)
    {
        std::string symbol = expr.getChild(1).getValue();
        if (this->symbolExists(symbol))
        {
            throw Exception("Próba re-deklaracji nazwy " + symbol, REDECLARATION_OF_SYMBOL, expr.getLineNumber());
        }

        this->functions[symbol] = Function(expr);
    }

    Variable getVariable(std::string symbol, int line_number)
    {
        if (!this->symbolIsVariable(symbol))
        {
            if (this->symbolIsFunction(symbol))
            {
                throw Exception("Próba użycia wartości funkcji jako zmiennej" + symbol, BAD_FUNCTION_USAGE, line_number);
            }
            throw Exception("Próba użycia wartości nieznanego symbolu " + symbol, SYMBOL_NOT_DECLARED, line_number);
        }

        return this->variables[symbol];
    }

    Function getFunction(std::string symbol, int line_number)
    {
        if (!this->symbolIsFunction(symbol))
        {
            if (this->symbolIsVariable(symbol))
            {
                throw Exception("Próba użycia wartości funkcji jako zmiennej" + symbol, BAD_FUNCTION_USAGE, line_number);
            }
            throw Exception("Próba użycia wartości nieznanego symbolu" + symbol, SYMBOL_NOT_DECLARED, line_number);
        }

        return this->functions[symbol];
    }

    void assignVariable(std::string symbol, Variable var)
    {
        if (!this->symbolIsVariable(symbol))
        {
            throw Exception("Próba nadania wartości nieznanemu symbolowi " + symbol, SYMBOL_NOT_DECLARED, var.getLineNumber());
        }
        this->variables[symbol].assign(var);
    }

    bool symbolIsFunction(std::string symbol)
    {
        return this->functions.count(symbol);
    }

    bool symbolIsVariable(std::string symbol)
    {
        return this->variables.count(symbol);
    }
};
