#pragma once
#include "special/std_libraries.h"

const std::string TokenTypeName[] = {
    "END_OF_STATEMENT",
    "NUMERIC_VALUE",
    "STRING_VALUE",
    "MATH_OPERATOR",
    "LOGIC_OPERATOR",
    "ASSIGNMENT",
    "L_PARENTHESIS",
    "R_PARENTHESIS",
    "L_SQR_PARENTHESIS",
    "R_SQR_PARENTHESIS",
    "L_BRACKET",
    "R_BRACKET",
    "NEXT_OPERATOR",
    "TYPE_OPERATOR",
    "SYMBOLIC_NAME",
};

enum TokenType
{
    END_OF_STATEMENT,
    NUMERIC_VALUE,
    STRING_VALUE,
    MATH_OPERATOR,
    LOGIC_OPERATOR,
    ASSIGNMENT,
    L_PARENTHESIS,
    R_PARENTHESIS,
    L_SQR_PARENTHESIS,
    R_SQR_PARENTHESIS,
    NEXT_OPERATOR,
    L_BRACKET,
    R_BRACKET,
    TYPE_OPERATOR,
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

    bool isSymbolicValue(std::string value)
    {
        return this->type == TokenType::SYMBOLIC_NAME && this->body == value;
    }
};
