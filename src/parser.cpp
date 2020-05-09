#include "../include/parser.h"

std::map<std::string, bool> TYPE_NAMES = {
    {"Int", 1},
    {"Float", 1},
    {"String", 1},
};

std::set<std::string> KEYWORDS = {
    "Int",
    "Float",
    "String",
    "If",
    "ElseIf",
    "Else",
    "EndIf"
    "Function",
    "EndFunction"};

Expression Parser::getNextExpression(Expression prev, bool function_args)
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

    auto token = this->nextToken(__LINE__);
    if (token.type == TokenType::END_OF_STATEMENT)
    {
        if (prev.getType() != ExpressionType::EMPTY)
            this->tokens.push_back(token);
        if (function_args)
        {
            throw Exception("Niepoprawny argument podany", BAD_FUNCTION_CALL, this->last_token.line_number);
        }
        return prev;
    }

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
            if (this->debug || true)
            {

                std::cout << "PREV: " << std::endl;
                prev.printExpression();
                token.printToken();
            }
            throw Exception("Niespodziewany token " + token.body, UNEXPECTED_TOKEN_IN_EXPR, token.line_number);
        }
    }
    else if (token.type == TokenType::NEXT_OPERATOR)
    {
        if (!function_args)
        {
            throw Exception("Niespodziewanie natknięto się na ,", UNEXPECTED_TOKEN_IN_EXPR, this->last_token.line_number);
        }
        return prev;
    }
    else if (token.type == TokenType::STRING_VALUE)
    {
        if (prev.getType() == ExpressionType::EMPTY || prev.getType() == ExpressionType::EXPR_ASSIGNMENT)
        {
            auto expr = Expression(ExpressionType::STRING_LITERAL, token.line_number, token.body);
            return this->getNextExpression(expr, function_args);
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
            throw Exception("Niespodziewany wartość tekstowa", UNEXPECTED_TOKEN_IN_EXPR, token.line_number);
        }
    }
    else if (token.type == TokenType::MATH_OPERATOR)
    {
        auto expr = Expression(ExpressionType::EXPR_MATH_OPERATOR, token.line_number, token.body);
        auto next_expr = getNextExpression(expr);

        if (!prev.returnsValue() || !next_expr.returnsValue())
        {
            prev.printExpression();
            next_expr.printExpression();
            throw Exception("2Podane wyrażenie jest nieprzypisywalne", EXPR_NOT_EVALUATING, token.line_number);
        }

        expr.addChild(prev);
        expr.addChild(next_expr);

        if (tokens.back().type == TokenType::END_OF_STATEMENT)
            return expr;
        return this->getNextExpression(expr, function_args);
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
            if (prev.getType() != ExpressionType::EMPTY || function_args)
            {
                throw Exception("Deklaracja nie może być cześcią innego wyrażenia", UNEXPECTED_TOKEN_IN_EXPR, token.line_number);
            }

            return expr;
        }
        else if (expr.getType() == ExpressionType::EXPR_ASSIGNMENT)
        {
            return expr;
        }
        else if (expr.getType() == ExpressionType::IF_BLOCK_EXPR)
        {
            return this->getNextExpression(expr);
        }
        else if (expr.getType() == ExpressionType::FUNCTION_DECLARATION)
        {
            if (prev.getType() != ExpressionType::EMPTY || function_args)
            {
                throw Exception("Deklaracja nie może być cześcią innego wyrażenia", UNEXPECTED_TOKEN_IN_EXPR, token.line_number);
            }

            return expr;
        }
        else if (expr.getType() == ExpressionType::SYMBOLIC_VALUE || expr.getType() == ExpressionType::FUNCTION_CALL)
        {
            return this->getNextExpression(expr);
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

Expression Parser::parseSymbolicToken(Token first_token)
{
    if (first_token.isSymbolicValue("Int") || first_token.isSymbolicValue("Float") || first_token.isSymbolicValue("String"))
    {
        auto name_token = this->nextToken(__LINE__);
        auto next_token = this->nextToken(__LINE__);

        if (name_token.type != TokenType::SYMBOLIC_NAME)
        {
            throw Exception("Niepoprawna nazwa w deklaracji zmiennej", BAD_VARIABLE_DECLARATION, name_token.line_number);
        }
        else if (TYPE_NAMES.count(name_token.body))
        {
            throw Exception("Używanie słów kluczowych jako nazw zmiennych jest niedozowolone", BAD_VARIABLE_DECLARATION, name_token.line_number);
        }

        auto expr = Expression(ExpressionType::DECLARATION, first_token.line_number);
        expr.addChild(Expression(ExpressionType::TYPE, first_token.line_number, first_token.body));
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

            if (!assign_val.returnsValue())
            {
                throw Exception("Podane wyrażenie jest nieprzypisywalne", EXPR_NOT_EVALUATING, next_token.line_number);
            }

            assign_expr.addChild(assign_val);
            expr.addChild(assign_expr);
        }
        else
        {
            throw Exception("Niespodziewany token " + next_token.body, UNEXPECTED_TOKEN_IN_EXPR, next_token.line_number);
        }

        return expr;
    }
    else if (first_token.isSymbolicValue("Function"))
    {
        auto name_token = this->nextToken(__LINE__);
        if (name_token.type != TokenType::SYMBOLIC_NAME || KEYWORDS.find(name_token.body) != KEYWORDS.end())
        {
            throw Exception("Niepoprawna nazwa funkcji " + name_token.body, BAD_FUNCTION_DECLARATION, name_token.line_number);
        }
        auto name_expr = Expression(ExpressionType::SYMBOLIC_VALUE, name_token.line_number, name_token.body);

        auto args_expr = this->getArgsDeclaration();

        auto type_op_token = this->nextToken(__LINE__);
        auto type_token = this->nextToken(__LINE__);
        if (type_op_token.type != TokenType::TYPE_OPERATOR || type_token.type != TokenType::SYMBOLIC_NAME || !TYPE_NAMES.count(type_token.body))
        {
            throw Exception("Niepoprawny typ zwrotny funkcji", BAD_FUNCTION_DECLARATION, type_token.line_number);
        }
        auto type_expr = Expression(ExpressionType::TYPE, type_token.line_number, type_token.body);

        auto body_expr = this->getFunctionBody();

        auto expr = Expression(ExpressionType::FUNCTION_DECLARATION, first_token.line_number);
        expr.addChild(type_expr);
        expr.addChild(name_expr);
        expr.addChild(args_expr);
        expr.addChild(body_expr);

        return expr;
    }
    else if (first_token.isSymbolicValue("If"))
    {
        this->tokens.push_back(first_token);
        return this->getIfBlock();
    }
    else
    {
        auto next_token = this->nextToken(__LINE__);
        if (next_token.type == TokenType::ASSIGNMENT)
        {
            auto expr = Expression(ExpressionType::EXPR_ASSIGNMENT, next_token.line_number);
            expr.addChild(Expression(ExpressionType::SYMBOLIC_VALUE, first_token.line_number, first_token.body));
            auto assign_val = this->getNextExpression(expr);

            if (!assign_val.returnsValue())
            {
                throw Exception("Podane wyrażenie jest nieprzypisywalne", EXPR_NOT_EVALUATING, next_token.line_number);
            }

            expr.addChild(assign_val);
            return expr;
        }
        else if (next_token.type == TokenType::L_PARENTHESIS)
        {
            auto expr = Expression(ExpressionType::FUNCTION_CALL, first_token.line_number);
            auto name_expr = Expression(ExpressionType::SYMBOLIC_VALUE, first_token.line_number, first_token.body);
            std::cout << "OKEJ" << std::endl;
            auto args_provided = this->getArgsProvided();

            expr.addChild(name_expr);
            expr.addChild(args_provided);
            return expr;
        }
        else
        {
            this->tokens.push_back(next_token);
            auto expr = Expression(ExpressionType::SYMBOLIC_VALUE, first_token.line_number, first_token.body);
            return expr;
        }
    }

    throw Exception("Nieobsługiwana wartość symboliczna " + first_token.body, UNEXPECTED_SYMBOLIC_NAME, first_token.line_number);
}

//////////////// FUNCTION CALL
Expression Parser::getNexedExpr()
{
    std::vector<Token> nexed_tokens;
    auto token = this->nextToken(__LINE__);
    while (token.type != TokenType::NEXT_OPERATOR && token.type != TokenType::R_PARENTHESIS)
    {
        nexed_tokens.push_back(token);
        token = this->nextToken(__LINE__);
    }
    this->tokens.push_back(token);
    nexed_tokens.push_back(Token(TokenType::END_OF_STATEMENT, ";", this->last_token.line_number));

    Parser nexed_parser(nexed_tokens);
    nexed_parser.parse();

    if (nexed_parser.getExprs().empty())
    {
        throw Exception("Niepoprawne wyrażenie podane jako arugment", BAD_FUNCTION_CALL, this->last_token.line_number);
    }

    return nexed_parser.getExprs().back();
}

Expression Parser::getArgsProvided()
{
    auto expr = Expression(ExpressionType::ARG_BLOCK_PROVIDED, this->last_token.line_number);

    bool should_get_next = true;
    while (true)
    {
        auto next_token = this->nextToken(__LINE__);
        std::cout << "NEXT_TOKEN" << std::endl;
        next_token.printToken();
        if (next_token.type == TokenType::R_PARENTHESIS)
        {
            if (should_get_next)
            {
                throw Exception("Nie znalezion kolejnego argumentu w wywołaniu funkcji", BAD_FUNCTION_DECLARATION, next_token.line_number);
            }
            break;
        }
        else if (next_token.type == TokenType::NEXT_OPERATOR)
        {
            should_get_next = true;
            next_token.printToken();
            throw Exception("Błedne wyrażenie podane jako arugment", BAD_FUNCTION_CALL, next_token.line_number);
        }
        else
        {
            this->tokens.push_back(next_token);
        }

        if (should_get_next)
        {
            should_get_next = false;

            auto arg_expr = Expression(ExpressionType::ARG_PROVIDED, this->last_token.line_number);
            auto arg_val_expr = this->getNexedExpr();
            arg_expr.addChild(arg_val_expr);
            expr.addChild(arg_expr);
        }
        else
        {
            break;
        }
    }

    return expr;
}

//////////////// FUNCTION DECLARATIONS

Expression Parser::getFunctionBody()
{
    std::vector<Token> function_tokens;
    auto token = this->nextToken(__LINE__);
    int line_number = token.line_number;
    while (!token.isSymbolicValue("EndFunction"))
    {
        function_tokens.push_back(token);
        token = this->nextToken(__LINE__);
    }

    std::cout << "FUNCTION BODY TOKENS: " << std::endl;
    for (auto t : function_tokens)
    {
        t.printToken();
    }

    Parser function_parser(function_tokens, true);
    function_parser.parse();

    auto expr = Expression(ExpressionType::FUNCTION_BODY, line_number);
    for (auto c : function_parser.getExprs())
        expr.addChild(c);
    std::cout << "WSZYSTKO OKEJ" << std::endl;
    return expr;
}

Expression Parser::getArgsDeclaration()
{
    auto expr = Expression(ExpressionType::ARG_BLOCK_DECLARED, this->last_token.line_number);

    if (this->nextToken(__LINE__).type != TokenType::L_PARENTHESIS)
    {
        throw Exception("Definicja funkcji wymaga podania argumentów", BAD_FUNCTION_DECLARATION, this->last_token.line_number);
    }

    bool should_get_next = false;
    while (true)
    {
        auto type_token = this->nextToken(__LINE__);
        if (type_token.type == TokenType::R_PARENTHESIS)
        {
            if (should_get_next)
            {
                throw Exception("Nie znalezion kolejnego argumentu", BAD_FUNCTION_DECLARATION, type_token.line_number);
            }
            break;
        }
        auto name_token = this->nextToken(__LINE__);
        if (type_token.type != TokenType::SYMBOLIC_NAME || !TYPE_NAMES.count(type_token.body))
        {
            throw Exception("Niepoprawny typ argumentu", BAD_FUNCTION_DECLARATION, type_token.line_number);
        }
        if (name_token.type != TokenType::SYMBOLIC_NAME || KEYWORDS.find(name_token.body) != KEYWORDS.end())
        {
            throw Exception("Niepoprawna nazwa argumentu " + name_token.body, BAD_FUNCTION_DECLARATION, name_token.line_number);
        }

        if (this->tokens.empty())
            this->nextToken(__LINE__);
        if (this->tokens.back().type == TokenType::NEXT_OPERATOR)
        {
            this->nextToken(__LINE__);
            should_get_next = true;
        }
        else
        {
            should_get_next = false;
        }

        auto arg_expr = Expression(ExpressionType::ARG_DECLARED, this->last_token.line_number);
        arg_expr.addChild(Expression(ExpressionType::TYPE, type_token.line_number, type_token.body));
        arg_expr.addChild(Expression(ExpressionType::SYMBOLIC_VALUE, name_token.line_number, name_token.body));

        expr.addChild(arg_expr);
    }

    return expr;
}

//////////////// PARENTHESIS

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

////////////////////////// IF EXPRESSIONS
Expression Parser::getIfBlock()
{
    int depth = 0;
    auto expr = Expression(ExpressionType::IF_BLOCK_EXPR, this->last_token.line_number);
    bool last_is_else = false;

    do
    {
        bool is_else = false;

        if ((this->tokens.back().isSymbolicValue("If") || this->tokens.back().isSymbolicValue("ElseIf") || this->tokens.back().isSymbolicValue("Else")))
        {
            if (this->tokens.back().isSymbolicValue("Else"))
                is_else = true;
            this->tokens.pop_back();
        }
        else
            break;

        if (last_is_else)
        {
            throw Exception("Else musi znajdować się na końcu bloku warunkowego", BAD_IF_EXPR, this->last_token.line_number);
        }

        last_is_else = is_else;
        expr.addChild(this->getIfExpression(is_else));
    } while (!this->tokens.empty());

    return expr;
}

Expression Parser::getIfExpression(bool is_else)
{
    std::vector<Token> if_tokens;
    auto expr = Expression(ExpressionType::IF_EXPR, this->last_token.line_number);

    // FIX
    // check if token is (
    Expression cond = Expression(ExpressionType::EMPTY, this->last_token.line_number, "ELSE");
    if (is_else == false)
    {
        this->tokens.pop_back();
        cond = this->getParenthesisedExpression();
        if (!cond.returnsValue())
        {
            throw Exception("Podane wyrażenie jest nieprzypisywalne", EXPR_NOT_EVALUATING, this->last_token.line_number);
        }
    }

    int depth = 0;
    auto next_token = this->nextToken(__LINE__);
    while (!next_token.isSymbolicValue("EndIf") && !next_token.isSymbolicValue("ElseIf") && !next_token.isSymbolicValue("Else") || (depth != 0))
    {
        if (next_token.isSymbolicValue("If"))
            depth += 1;
        else if (next_token.isSymbolicValue("EndIf"))
            --depth;
        if_tokens.push_back(next_token);
        next_token = this->nextToken(__LINE__);
    }

    if (next_token.isSymbolicValue("ElseIf") || next_token.isSymbolicValue("Else"))
    {
        if (is_else && next_token.body == "Else")
        {
            throw Exception("Więcej niż jeden Else w bloku warunkowym", BAD_IF_EXPR, next_token.line_number);
        }
        this->tokens.push_back(next_token);
    }
    else
    {
        this->tokens.push_back(Token(TokenType::END_OF_STATEMENT, ";", next_token.line_number));
    }

    Parser if_parser(if_tokens);
    if_parser.parse();

    expr.addChild(cond);
    for (auto e : if_parser.getExprs())
        expr.addChild(e);
    return expr;
}
