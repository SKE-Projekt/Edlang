#pragma once
#include "special/return_codes.h"
#include "special/std_libraries.h"
#include "token.h"
#include "exception.h"

class Lexer
{
private:
    int c_sc_pos;
    int line_number;
    std::string source_code;
    std::vector<Token> tokens;

    bool debug;

    std::string parseNumericValue();

    void pushToken(TokenType type_v, std::string body_v)
    {
        this->tokens.push_back(Token(type_v, body_v, this->line_number));
    }

public:
    Lexer(std::string source_code_v, bool debug_v = false)
    {
        this->c_sc_pos = 0;
        this->line_number = 0;
        this->source_code = source_code_v;

        this->debug = debug_v;
    }

    void lex();
};
