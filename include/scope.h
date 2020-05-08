#pragma once
#include "special/std_libraries.h"
#include "special/return_codes.h"
#include "expression.h"
#include "exception.h"
#include "variable.h"

class Scope
{
private:
    std::map<std::string, Variable> variables;

public:
    Scope(){};

    void printScope()
    {
        std::cout << "SCOPE: " << std::endl;
        std::cout << "VARIABLES:" << std::endl;
        for (auto v : this->variables)
        {
            std::cout << v.first << ": " << std::endl;
            v.second.printVariable();
        }
    }

    bool symbolExists(std::string symbol)
    {
        return this->variables.count(symbol);
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

    Variable getVariable(std::string symbol, int line_number)
    {
        if (!this->symbolIsVariable(symbol))
        {
            throw Exception("Próba użycia wartości nieznanego symbolu" + symbol, SYMBOL_NOT_DECLARED, line_number);
        }

        return this->variables[symbol];
    }

    void assignVariable(std::string symbol, Variable var)
    {
        if (!this->symbolIsVariable(symbol))
        {
            throw Exception("Próba nadania wartości nieznanemu symbolowi " + symbol, SYMBOL_NOT_DECLARED, var.getLineNumber());
        }
        this->variables[symbol].assign(var);
    }

    bool symbolIsVariable(std::string symbol)
    {
        return this->variables.count(symbol);
    }
};
