#include "../include/native_functions.h"

Variable nativePrintFunction(std::vector<Variable> values, int line_number)
{
    for (auto e : values)
    {
        e.printNatively();
    }
    std::cout << std::endl;

    return Variable(VariableType::INTEGER_TYPE, line_number, true, 1);
}

Variable nativeLenFunction(std::vector<Variable> args, int line_number)
{
    if (args.size() == 0)
    {
        throw Exception("", BAD_FUNCTION_USAGE, line_number);
    }

    std::vector<Variable> values;
    for (auto v : args)
    {
        if (v.type != VariableType::STRING_TYPE && v.type != VariableType::LIST_TYPE)
        {
            throw Exception("Funkcja Len przyjmuje jedynie wartości będące stringiem lub listą", BAD_FUNCTION_USAGE, line_number);
        }

        if (v.type == VariableType::LIST_TYPE)
            values.push_back(Variable(VariableType::INTEGER_TYPE, line_number, true, v.getListVal().size()));
        if (v.type == VariableType::STRING_TYPE)
            values.push_back(Variable(VariableType::INTEGER_TYPE, line_number, true, v.getStringVal().size()));
    }

    if (args.size() == 1)
    {
        return values.back();
    }

    return Variable(VariableType::LIST_TYPE, line_number, true, -1, -1, "UNDEFINED", values);
}
