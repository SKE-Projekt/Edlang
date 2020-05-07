#include "../include/lexer.h"

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

void Lexer::lex()
{
    std::regex number_regex("[.0-9]");

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
        else if (std::regex_match(curr_c, number_regex))
        {
            this->pushToken(TokenType::NUMERIC_VALUE, this->parseNumericValue());
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
