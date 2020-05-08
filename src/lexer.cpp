#include "../include/lexer.h"

std::pair<int, std::string> Lexer::parseStringValue(std::string end_char)
{
    int lines = 0;
    std::string string_value = "";
    this->c_sc_pos++;
    while ((this->c_sc_pos < this->source_code.size()))
    {
        std::string curr_c = this->source_code.substr(this->c_sc_pos, 1);

        if (curr_c == end_char)
        {
            this->c_sc_pos++;
            return {lines, string_value};
        }
        else if (curr_c == "\n")
        {
            lines += 1;
            string_value += curr_c;
        }
        else
        {
            string_value += curr_c;
        }

        this->c_sc_pos++;
    }
    throw Exception("Niezamknięty string", BAD_STRING_LITERAL, this->line_number);
}

std::string Lexer::parseNumericValue()
{
    int numberOfDots = 1;
    std::string number = "";
    std::regex number_regex("[.0-9]");

    while ((this->c_sc_pos < this->source_code.size()))
    {
        std::string curr_c = this->source_code.substr(this->c_sc_pos, 1);
        if (std::regex_match(curr_c, number_regex))
        {
            if (curr_c == "." && --numberOfDots < 0)
            {
                throw Exception("Niepoprawny znak w wartości liczbowej", INCORRECT_CHAR_IN_NUMERIC_VALUE, this->line_number);
            }
            number += curr_c;
        }
        else
        {
            break;
        }
        this->c_sc_pos++;
    }

    return number;
}

std::string Lexer::parseSymbolicName()
{
    std::regex symbolic_regex("[_a-zA-Z0-9]");
    std::string symbolic_name = "";

    while ((this->c_sc_pos < this->source_code.size()))
    {
        std::string curr_c = this->source_code.substr(this->c_sc_pos, 1);
        if (std::regex_match(curr_c, symbolic_regex))
        {
            symbolic_name += curr_c;
        }
        else
        {
            break;
        }
        this->c_sc_pos++;
    }

    return symbolic_name;
}

void Lexer::lex()
{
    std::regex number_regex("[.0-9]");
    std::regex symbol_regex("[a-zA-Z_]");
    std::regex whitespace_regex("[ \t]");
    std::string operators = "+-/*%";

    this->line_number = 1;
    while (this->c_sc_pos < this->source_code.size())
    {
        std::string curr_c = this->source_code.substr(this->c_sc_pos, 1);

        if (curr_c == ";")
        {
            this->pushToken(TokenType::END_OF_STATEMENT, ";");
            this->c_sc_pos++;
        }
        else if (curr_c == "\n")
        {
            this->line_number++;
            this->c_sc_pos++;
            continue;
        }
        else if (std::regex_match(curr_c, whitespace_regex))
        {
            this->c_sc_pos++;
            continue;
        }
        else if (curr_c == "(")
        {
            this->pushToken(TokenType::L_PARENTHESIS, "(");
            this->c_sc_pos++;
        }
        else if (curr_c == ")")
        {
            this->pushToken(TokenType::R_PARENTHESIS, ")");
            this->c_sc_pos++;
        }
        else if (operators.find(curr_c) != operators.npos)
        {
            this->pushToken(TokenType::MATH_OPERATOR, curr_c);
            this->c_sc_pos++;
        }
        else if (std::regex_match(curr_c, number_regex))
        {
            this->pushToken(TokenType::NUMERIC_VALUE, this->parseNumericValue());
        }
        else if (std::regex_match(curr_c, symbol_regex))
        {
            this->pushToken(TokenType::SYMBOLIC_NAME, this->parseSymbolicName());
        }
        else if (curr_c == "\"" || curr_c == "\'")
        {
            auto string_val = this->parseStringValue(curr_c);
            this->pushToken(TokenType::STRING_VALUE, string_val.second);
            this->line_number += string_val.first;
        }
        else
        {
            throw Exception("Znaleziono nierozpoznany znak (" + curr_c + ")", UNRECOGNIZED_CHAR, this->line_number);
        }

        if (this->debug)
        {
            this->tokens.back().printToken();
        }
    }
}
