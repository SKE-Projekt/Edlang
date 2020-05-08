#include "../include/expression.h"

void Expression::printExpression(int depth)
{
    for (int i = 0; i < depth; ++i)
        std::cout << "\t";
    std::cout << "EXPRESSION(" << ExpressionTypeName[this->type] << ")" << std::endl;
    for (int i = 0; i < depth; ++i)
        std::cout << "\t";
    std::cout << "{" << std::endl;
    for (int i = 0; i < (depth + 1); ++i)
        std::cout << "\t";
    std::cout << "VALUE : " << this->value << std::endl;
    for (int i = 0; i < (depth + 1); ++i)
        std::cout << "\t";
    std::cout << "LINE_NO : " << this->line_number << std::endl;
    for (auto &c : this->children)
    {
        c.printExpression(depth + 1);
    }
    for (int i = 0; i < depth; ++i)
        std::cout << "\t";
    std::cout << "}" << std::endl;
}
