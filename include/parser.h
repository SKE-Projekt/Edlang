#pragma once
#include "special/return_codes.h"
#include "special/std_libraries.h"
#include "types.h"
#include "token.h"
#include "exception.h"
#include "expression.h"

class Parser
{
private:
    Token last_token;
    std::vector<Token> tokens;
    std::vector<Expression> expressions;

    bool debug;

    Expression getIfBlock();
    Expression getIfExpression(bool is_else);
    Expression getParenthesisedExpression();
    Expression parseSymbolicToken(Token first_token);

    Expression getArgsDeclaration();
    Expression getFunctionBody();

    Token nextToken()
    {
        if (!this->tokens.empty())
        {
            auto token = this->tokens.back();
            this->tokens.pop_back();

            this->last_token = token;
            return token;
        }

        throw Exception("Niepełne wyrażenie", EXPECTED_MORE_TOKENS, this->last_token.line_number);
    }

public:
    Parser(std::vector<Token> tokens_v, bool debug_v = false, bool reverse = true) : last_token(TokenType::END_OF_STATEMENT, "", 1)
    {
        this->tokens = tokens_v;
        this->debug = debug_v;

        if (reverse)
            std::reverse(this->tokens.begin(), this->tokens.end());
    }

    Expression getNextExpression(Expression prev = Expression(ExpressionType::EMPTY, 0), bool priority = false);
    void parse();

    void debugTokens()
    {
        for (auto t : this->tokens)
            t.printToken();
    }

    std::vector<Expression> getExprs()
    {
        return this->expressions;
    }
};
