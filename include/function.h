#pragma once
#include "special/std_libraries.h"
#include "special/return_codes.h"
#include "expression.h"
#include "exception.h"
#include "variable.h"

class Function
{
private:
    VariableType return_type;
    std::vector<Expression> args_required;
    std::vector<Expression> body;

public:
    Function();

    Function(Expression function_expr);

    std::vector<Expression> getBody()
    {
        return this->body;
    }

    std::vector<Expression> getArgsReq()
    {
        return this->args_required;
    }

    VariableType getReturnType()
    {
        return this->return_type;
    }

    void operator=(Function other);

    void printFunction();
};
