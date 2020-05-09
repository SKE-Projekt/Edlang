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
