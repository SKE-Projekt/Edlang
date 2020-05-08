#include "../include/parser.h"

std::map<std::string, Types> TYPE_NAMES = {
    {"Int", Types::INTEGER_TYPE},
    {"Float", Types::FLOATING_TYPE},
    {"String", Types::STRING_TYPE},
};

Expression Parser::parseSymbolicToken(Token first_token)
{
    if (first_token.body == "Int" || first_token.body == "Float" || first_token.body == "String")
    {
        auto name_token = this->nextToken();
        auto next_token = this->nextToken();

        if (name_token.type != TokenType::SYMBOLIC_NAME)
        {
            throw Exception("Niepoprawna nazwa w deklaracji zmiennej", BAD_VARIABLE_DECLARATION, name_token.line_number);
        }
        else if (TYPE_NAMES.count(name_token.body))
        {
            throw Exception("Używanie słów kluczowych jako nazw zmiennych jest niedozowolone", BAD_VARIABLE_DECLARATION, name_token.line_number);
        }

        auto expr = Expression(ExpressionType::DECLARATION, first_token.line_number, first_token.body);
        expr.addChild(Expression(ExpressionType::SYMBOLIC_VALUE, name_token.line_number, name_token.body));

        if (next_token.type == TokenType::END_OF_STATEMENT)
        {
            this->tokens.push_back(next_token);
            return expr;
        }
        else if (next_token.type == TokenType::ASSIGNMENT)
        {
            auto assign_expr = Expression(ExpressionType::EXPR_ASSIGNMENT, next_token.line_number);
            auto assign_val = this->getNextExpression(assign_expr);
            assign_expr.addChild(assign_val);
            expr.addChild(assign_expr);
        }
        else
        {
            throw Exception("Niespodziewany token " + next_token.body, UNEXPECTED_TOKEN_IN_EXPR, next_token.line_number);
        }

        return expr;
    }
    else
    {
        auto next_token = this->nextToken();
        if (next_token.type == TokenType::ASSIGNMENT)
        {
            auto expr = Expression(ExpressionType::EXPR_ASSIGNMENT, next_token.line_number);
            expr.addChild(Expression(ExpressionType::SYMBOLIC_VALUE, first_token.line_number, first_token.body));
            auto assign_val = this->getNextExpression(expr);
            expr.addChild(assign_val);
            return expr;
        }
    }

    throw Exception("Nieobsługiwana wartość symboliczna " + first_token.body, UNEXPECTED_SYMBOLIC_NAME, first_token.line_number);
}

Expression Parser::getParenthesisedExpression()
{
    int parenthesis_depth = 1;
    std::vector<Token> parenthesised_tokens;
    while (!this->tokens.empty())
    {
        auto token = this->tokens.back();
        this->tokens.pop_back();
        if (token.type == TokenType::R_PARENTHESIS && (--parenthesis_depth == 0))
        {
            parenthesised_tokens.push_back(Token(TokenType::END_OF_STATEMENT, ";", token.line_number));
            Parser parentheisis_parser(parenthesised_tokens);
            parentheisis_parser.parse();

            if (parentheisis_parser.expressions.empty())
            {
                throw Exception("Wyrażenie nawiasowe jest puste", BAD_PARENTHESIS_EXPR, this->last_token.line_number);
            }
            return parentheisis_parser.expressions.back();
        }
        else if (token.type == TokenType::L_PARENTHESIS)
        {
            ++parenthesis_depth;
            parenthesised_tokens.push_back(token);
        }
        else if (token.type == TokenType::END_OF_STATEMENT)
        {
            throw Exception("Nie znaleziono zamknięcia nawiasu", UNEXPECTED_TOKEN_IN_EXPR, token.line_number);
        }
        else
        {
            parenthesised_tokens.push_back(token);
        }
    }

    throw Exception("Wyrażenie nawiasowe jest niepoprawne", BAD_PARENTHESIS_EXPR, this->last_token.line_number);
}

Expression Parser::getNextExpression(Expression prev, bool priority)
{
    if (this->tokens.empty())
    {
        if (this->debug)
        {
            std::cout << "----------DEBUG---------" << std::endl;
            prev.printExpression();
        }
        throw Exception("Nie znaleziono zakończenia wyrażenia", EXPECTED_EOF_TOKEN, this->last_token.line_number);
    }

    auto token = this->tokens.back();
    last_token = token;
    if (token.type == TokenType::END_OF_STATEMENT)
    {
        if (prev.getType() == ExpressionType::EMPTY)
            this->tokens.pop_back();
        return prev;
    }

    this->tokens.pop_back();
    if (token.type == TokenType::NUMERIC_VALUE)
    {
        if (prev.getType() == ExpressionType::EMPTY || prev.getType() == ExpressionType::EXPR_ASSIGNMENT)
        {
            auto expr = Expression(ExpressionType::NUMERIC_LITERAL, token.line_number, token.body);
            return this->getNextExpression(expr);
        }
        else if (prev.getType() == ExpressionType::EXPR_MATH_OPERATOR)
        {
            auto expr = Expression(ExpressionType::NUMERIC_LITERAL, token.line_number, token.body);
            return expr;
        }
        else
        {
            if (this->debug)
            {

                std::cout << "PREV: " << std::endl;
                token.printToken();
            }
            throw Exception("Niespodziewany token " + token.body, UNEXPECTED_TOKEN_IN_EXPR, token.line_number);
        }
    }
    else if (token.type == TokenType::STRING_VALUE)
    {
        if (prev.getType() == ExpressionType::EMPTY || prev.getType() == ExpressionType::EXPR_ASSIGNMENT)
        {
            auto expr = Expression(ExpressionType::STRING_LITERAL, token.line_number, token.body);
            return this->getNextExpression(expr);
        }
        else if (prev.getType() == ExpressionType::EXPR_MATH_OPERATOR)
        {
            auto expr = Expression(ExpressionType::STRING_LITERAL, token.line_number, token.body);
            return expr;
        }
        else
        {
            if (this->debug)
            {

                std::cout << "PREV: " << std::endl;
                token.printToken();
            }
            throw Exception("LOL" + token.body, UNEXPECTED_TOKEN_IN_EXPR, token.line_number);
        }
    }
    else if (token.type == TokenType::MATH_OPERATOR)
    {
        auto expr = Expression(ExpressionType::EXPR_MATH_OPERATOR, token.line_number, token.body);
        auto next_expr = getNextExpression(expr);
        expr.addChild(prev);
        expr.addChild(next_expr);

        if (tokens.back().type == TokenType::END_OF_STATEMENT)
            return expr;
        return this->getNextExpression(expr);
    }
    else if (token.type == TokenType::L_PARENTHESIS)
    {
        auto expr = this->getParenthesisedExpression();
        return this->getNextExpression(expr);
    }
    else if (token.type == TokenType::R_PARENTHESIS)
    {
        throw Exception("Znaleziono zamknięcie nieotwartego nawiasu", UNEXPECTED_TOKEN_IN_EXPR, token.line_number);
    }
    else if (token.type == TokenType::SYMBOLIC_NAME)
    {
        auto expr = this->parseSymbolicToken(token);
        if (expr.getType() == ExpressionType::DECLARATION)
        {
            if (prev.getType() != ExpressionType::EMPTY)
            {
                throw Exception("Deklaracja nie może być cześcią innego wyrażenia", UNEXPECTED_TOKEN_IN_EXPR, token.line_number);
            }

            return expr;
        }
        else if (expr.getType() == ExpressionType::EXPR_ASSIGNMENT)
        {
            return expr;
        }
    }
    throw Exception("Nieobsługiwane wyrażenie " + token.body, UNHANDLED_EXPRESSION, token.line_number);
}

void Parser::parse()
{
    while (!this->tokens.empty())
    {
        auto expr = this->getNextExpression();
        if (expr.getType() != ExpressionType::EMPTY)
            this->expressions.push_back(expr);
        if ((this->debug == true) && (expr.getType() != ExpressionType::EMPTY))
        {
            expr.printExpression();
        }
    }
}
