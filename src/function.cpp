#include "../include/function.h"

Function::Function(Expression function_expr)
{
    auto type_expr = function_expr.getChild(0);
    if (type_expr.getValue() == "Int")
        this->return_type = VariableType::INTEGER_TYPE;
    else if (type_expr.getValue() == "Float")
        this->return_type = VariableType::FLOAT_TYPE;
    else
        this->return_type = VariableType::STRING_TYPE;

    this->args_required = function_expr.getChild(2).getChildren();

    auto body_expr = function_expr.getChild(3);
    if (body_expr.childCount() == 0)
    {
        throw Exception("Ciało funkcji nie może być puste", EMPTY_FUNCTION_BODY, function_expr.getLineNumber());
    }

    this->body = body_expr.getChildren();
}

Function::Function()
{
    this->return_type = VariableType::INTEGER_TYPE;
}

void Function::operator=(Function other)
{
    this->return_type = other.return_type;
    this->args_required = other.args_required;
    this->body = other.body;
}

void Function::printFunction()
{
    std::cout << "FUNCTION: " << std::endl;
    std::cout << "{" << std::endl;
    std::cout << "\tTYPE: " << VariableTypeName[this->return_type] << std::endl;
    std::cout << "\tARG_COUNT: " << this->args_required.size() << std::endl;
    std::cout << "\tBODY_COUNT: " << this->body.size() << std::endl;
    std::cout << "}" << std::endl;
}
