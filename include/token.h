#include "special/std_libraries.h"

const std::string TokenTypeName[] = {
    "END_OF_STATEMENT",
    "NUMERIC_VALUE",
    "OPERATOR_PLUS",
    "OPERATOR_MINUS",
    "OPERATOR_MULITPLY",
    "OPERATOR_DIVIDE",
    "OPERATOR_MODULO",
    "L_PARENTHESIS",
    "R_PARENTHESIS",
    "SYMBOLIC_NAME",
};

enum TokenType
{
    END_OF_STATEMENT,
    NUMERIC_VALUE,
    OPERATOR_PLUS,
    OPERATOR_MINUS,
    OPERATOR_MULITPLY,
    OPERATOR_DIVIDE,
    OPERATOR_MODULO,
    L_PARENTHESIS,
    R_PARENTHESIS,
    SYMBOLIC_NAME,
};

struct Token
{
    TokenType type;
    std::string body;

    int line_number;

    Token(TokenType type_v, std::string body_v, int line_number_v)
    {
        this->type = type_v;
        this->body = body_v;
        this->line_number = line_number_v;
    }

    void printToken()
    {
        std::cout << "TOKEN { TYPE: " << TokenTypeName[this->type] << " ; BODY: " << this->body << " ; LINE : " << this->line_number << " }" << std::endl;
    }
};
