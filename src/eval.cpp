#include "../include/eval.h"

void Eval::eval()
{
    while (!this->expressions.empty())
    {
        bool scoped = false;
        auto expr = this->nextExpr();

        if (expr.getType() == ExpressionType::IF_EXPR)
            scoped = true;

        auto result = this->evalExpr(expr, scoped);
        if (this->debug)
        {
            result.printVariable();
            this->getScope().printScope();
        }
    }
}

Variable Eval::evalExpr(Expression expr, bool scoped)
{
    if (this->debug)
    {
        std::cout << "EVALUATING" << std::endl;
        expr.printExpression();
    }

    switch (expr.getType())
    {
    case ExpressionType::NUMERIC_LITERAL:
    case ExpressionType::STRING_LITERAL:
    case ExpressionType::LIST_LITERAL:
        return this->evalLiteralExpression(expr);
        break;
    case ExpressionType::SYMBOLIC_VALUE:
        return this->searchForVariable(expr.getValue(), expr.getLineNumber());
        break;
    case ExpressionType::EXPR_MATH_OPERATOR:
        return this->evalMathOperatorExpression(expr);
        break;
    case ExpressionType::EXPR_LOGIC_OPERATOR:
        return this->evalLogicOperatorExpression(expr);
        break;
    case ExpressionType::EXPR_INDEX_OPERATOR:
        return this->evalIndexOperatorExpression(expr);
        break;
    case ExpressionType::DECLARATION:
        return this->evalVariableDeclaration(expr);
        break;
    case ExpressionType::FUNCTION_DECLARATION:
        return this->evalFunctionDeclaration(expr);
        break;
    case ExpressionType::FUNCTION_CALL:
        return this->evalFunctionCall(expr);
        break;
    case ExpressionType::EXPR_ASSIGNMENT:
        return this->evalVariableAssignment(expr);
        break;
    case ExpressionType::IF_BLOCK_EXPR:
        return this->evalIfBlock(expr);
        break;
    case ExpressionType::LOOP:
        return this->evalLoop(expr);
        break;
    default:
        throw Exception("Nieobsługiwane wyrażenie", UNHANDLED_EXPRESSION, expr.getLineNumber());
    }
}

// TODO
// Fix naming conventions
Variable Eval::evalLoop(Expression expr)
{
    auto cond_expr = expr.getChild(0);
    auto cond = this->evalExpr(cond_expr);

    if (cond.type != VariableType::INTEGER_TYPE)
    {
        throw Exception("Warunek musi być wyrażeniem całkowitoliczbowym", INCORRECT_TYPE, cond_expr.getLineNumber());
    }

    Variable return_var;
    auto body_expr = expr.getChild(1).getChildren();

    while (cond.getIntVal())
    {
        this->addNewScope();
        for (int i = 0; i < body_expr.size() - 1; ++i)
            this->evalExpr(body_expr[i]);
        return_var = this->evalExpr(body_expr.back());

        cond = this->evalExpr(cond_expr);
        if (cond.type != VariableType::INTEGER_TYPE)
        {
            throw Exception("Warunek musi być wyrażeniem całkowitoliczbowym", INCORRECT_TYPE, cond_expr.getLineNumber());
        }
        this->removeLastScope();
    }
    return return_var;
}

Variable Eval::evalFunctionCall(Expression expr)
{
    auto name_expr = expr.getChild(0);

    if (name_expr.getValue() == "Print")
    {
        auto args_provided = expr.getChild(1);
        std::vector<Variable> args_to_provide;
        for (auto a : args_provided.getChildren())
        {
            args_to_provide.push_back(this->evalExpr(a.getChild(0)));
        }

        return nativePrintFunction(args_to_provide, expr.getLineNumber());
    }
    else if (name_expr.getValue() == "Read")
    {
        auto args_provided = expr.getChild(1);
        std::vector<Expression> args_to_provide;
        for (auto a : args_provided.getChildren())
        {
            if (a.getChild(0).getType() != ExpressionType::SYMBOLIC_VALUE)
            {
                throw Exception("Próba przypisania wartości do wartości", BAD_FUNCTION_CALL, expr.getLineNumber());
            }

            args_to_provide.push_back(a.getChild(0));
        }

        Variable return_var;
        for (auto s : args_to_provide)
        {
            auto symbol = s.getValue();
            return_var = this->searchForVariableRef(symbol, s.getLineNumber()).readNatively();
        }
        return return_var;
    }
    else if (name_expr.getValue() == "Len")
    {
        auto args_provided = expr.getChild(1);
        std::vector<Variable> args_to_provide;
        for (auto a : args_provided.getChildren())
        {
            args_to_provide.push_back(this->evalExpr(a.getChild(0)));
        }

        return nativeLenFunction(args_to_provide, expr.getLineNumber());
    }

    auto args_provided = expr.getChild(1);
    auto func = this->searchForFunction(name_expr.getValue(), name_expr.getLineNumber());
    Variable return_val;
    this->addNewScope();

    std::vector<std::pair<Variable, std::string>> args_to_declare;
    auto args_required = func.getArgsReq();
    for (auto ar : args_required)
    {
        if (ar.getChild(0).getValue() == "Int")
        {
            args_to_declare.push_back({Variable(VariableType::INTEGER_TYPE, ar.getLineNumber()), ar.getChild(1).getValue()});
        }
        else if (ar.getChild(0).getValue() == "Float")
        {
            args_to_declare.push_back({Variable(VariableType::FLOAT_TYPE, ar.getLineNumber()), ar.getChild(1).getValue()});
        }
        else
        {
            args_to_declare.push_back({Variable(VariableType::STRING_TYPE, ar.getLineNumber()), ar.getChild(1).getValue()});
        }
    }

    // TODO
    // Fix line numbers
    std::vector<Variable> args_to_provide;
    for (auto a : args_provided.getChildren())
    {
        args_to_provide.push_back(this->evalExpr(a.getChild(0)));
    }
    if (args_to_provide.size() != args_to_declare.size())
    {
        throw Exception("Niepoprawna liczba argumentów podana do funkcji(otrzymano " + std::to_string(args_to_provide.size()) + " zamiast " + std::to_string(args_to_declare.size()) + ")", BAD_FUNCTION_CALL, name_expr.getLineNumber());
    }

    for (int i = 0; i < args_to_declare.size(); ++i)
    {
        if (args_to_provide[i].type != args_to_declare[i].first.type)
        {
            throw Exception("Typ " + std::to_string(i + 1) + " argumentu nie zgadza się", BAD_FUNCTION_CALL, name_expr.getLineNumber());
        }

        this->getScope().declareVariable(args_to_declare[i].second, args_to_provide[i]);
        this->getScope().assignVariable(args_to_declare[i].second, args_to_provide[i]);
    }

    for (auto e : func.getBody())
        return_val = this->evalExpr(e);

    this->removeLastScope();

    if (return_val.type != func.getReturnType())
    {
        throw Exception("Typ zwrócony przez funkcje nie pasuje(" + VariableTypeName[return_val.type] + " zamiast " + VariableTypeName[func.getReturnType()] + ")", BAD_FUNCTION_CALL, func.getBody().back().getLineNumber());
    }

    return return_val;
}

Variable Eval::evalFunctionDeclaration(Expression expr)
{
    Function func = Function(expr);
    this->getScope().declareFunction(expr);
    return Variable(VariableType::INTEGER_TYPE, expr.getLineNumber(), false);
}

Variable Eval::evalIfBlock(Expression expr)
{
    std::vector<Expression> cases;
    for (int i = 0; i < expr.childCount(); ++i)
        cases.push_back(expr.getChild(i));

    Expression expr_to_run = Expression(ExpressionType::EMPTY, expr.getLineNumber());
    for (auto c : cases)
    {
        // Checking whether or not the cond is true
        auto cond = c.getChild(0);

        if (cond.getType() == ExpressionType::EMPTY)
        {
            // else block
            expr_to_run = c;
            break;
        }

        auto cond_var = this->evalExpr(cond);
        bool is_true = false;
        switch (cond_var.type)
        {
        case VariableType::INTEGER_TYPE:
            is_true = (cond_var.getIntVal() != 0);
            break;
        case VariableType::FLOAT_TYPE:
            is_true = (cond_var.getFloatVal() != 0.0);
            break;
        case VariableType::STRING_TYPE:
            is_true = (cond_var.getStringVal() != "");
            break;
        }

        if (is_true)
        {
            expr_to_run = c;
            break;
        }
    }

    if (expr_to_run.getType() == ExpressionType::EMPTY || expr_to_run.childCount() < 2)
    {
        return Variable(VariableType::INTEGER_TYPE, expr.getLineNumber(), false);
    }

    this->addNewScope();
    for (int i = 1; i < expr_to_run.childCount() - 1; ++i)
    {
        this->evalExpr(expr_to_run.getChild(i));
    }
    auto return_var = this->evalExpr(expr_to_run.getChild(expr_to_run.childCount() - 1));
    this->removeLastScope();

    return return_var;
}

Variable Eval::evalVariableDeclaration(Expression expr)
{
    auto type_expr = expr.getChild(0);
    auto symbol_expr = expr.getChild(1);

    VariableType type = VariableType::INTEGER_TYPE;
    if (type_expr.getValue() == "Float")
        type = VariableType::FLOAT_TYPE;
    else if (type_expr.getValue() == "String")
        type = VariableType::STRING_TYPE;
    else if (type_expr.getValue() == "List")
        type = VariableType::LIST_TYPE;

    Variable var = Variable(type, expr.getLineNumber(), false);

    auto &scope = this->getScope();
    scope.declareVariable(symbol_expr.getValue(), var);

    if (expr.childCount() == 3)
    {
        auto assign_val = expr.getChild(2).getChild(0);
        auto assign_expr = Expression(ExpressionType::EXPR_ASSIGNMENT, assign_val.getLineNumber());
        assign_expr.addChild(symbol_expr);
        assign_expr.addChild(assign_val);
        return this->evalExpr(assign_expr);
    }

    return var;
}

Variable &Eval::evalIndexedVariableAssignment(Expression expr)
{
    auto index = this->evalExpr(expr.getChild(0));
    auto to_var = expr.getChild(1);

    if (index.type != VariableType::INTEGER_TYPE)
    {
        throw Exception("Nieprawidłowy typ indeksu", BAD_INDEX, to_var.getLineNumber());
    }

    if (to_var.getType() == ExpressionType::EXPR_INDEX_OPERATOR)
    {

        auto &var = this->evalIndexedVariableAssignment(to_var);
        auto &var_ref = var.getListValRef();
        if (var_ref.size() <= index.getIntVal())
        {
            throw Exception("Indeks wykracza poza rozmiar listy", BAD_INDEX, index.getLineNumber());
        }
        return var.getListValRef()[index.getIntVal()];
    }
    else if (to_var.getType() != ExpressionType::SYMBOLIC_VALUE)
    {
        throw Exception("Przypisac mozna jedynie do zmiennej typu List", BAD_INDEX, to_var.getLineNumber());
    }

    auto &var = this->searchForVariableRef(to_var.getValue(), to_var.getLineNumber());
    auto &var_ref = var.getListValRef();
    if (var_ref.size() <= index.getIntVal())
    {
        throw Exception("Indeks wykracza", BAD_INDEX, index.getLineNumber());
    }
    return var.getListValRef()[index.getIntVal()];
}

Variable Eval::evalVariableAssignment(Expression expr)
{
    auto l_val_expr = expr.getChild(0);
    auto r_val_expr = expr.getChild(1);

    if (l_val_expr.getType() == ExpressionType::EXPR_INDEX_OPERATOR)
    {
        auto &variable_in_question = evalIndexedVariableAssignment(l_val_expr);
        variable_in_question.assign(this->evalExpr(r_val_expr));
        return variable_in_question;
    }

    auto r_val = this->evalExpr(r_val_expr);
    this->searchForVariable(l_val_expr.getValue(), l_val_expr.getLineNumber(), true, r_val);

    return r_val;
}

Variable Eval::evalLogicOperatorExpression(Expression expr)
{
    // Check special case for neq var;
    if (expr.getValue() == "neq" && expr.getChild(0).getType() == ExpressionType::EMPTY)
    {
        auto r_val_expr = expr.getChild(1);
        auto r_val = this->evalExpr(r_val_expr);

        return !r_val;
    }

    // Everything is normal
    auto l_val_expr = expr.getChild(0);
    auto r_val_expr = expr.getChild(1);

    auto l_val = this->evalExpr(l_val_expr);
    auto r_val = this->evalExpr(r_val_expr);

    if (expr.getValue() == ">")
        return l_val > r_val;
    else if (expr.getValue() == "<")
        return l_val < r_val;
    else if (expr.getValue() == "leq")
        return l_val <= r_val;
    else if (expr.getValue() == "geq")
        return l_val >= r_val;
    else if (expr.getValue() == "neq")
        return l_val != r_val;
    else if (expr.getValue() == "and" || expr.getValue() == "or")
    {
        if (l_val.type != VariableType::INTEGER_TYPE || r_val.type != VariableType::INTEGER_TYPE)
        {
            throw Exception("Operator " + expr.getValue() + " może być użyty tylko w wypadku wartości całkowitych", INCORRECT_TYPE, expr.getLineNumber());
        }

        if (expr.getValue() == "and")
        {
            return Variable(VariableType::INTEGER_TYPE, expr.getLineNumber(), true, l_val.getIntVal() && r_val.getIntVal());
        }
        else
        {
            return Variable(VariableType::INTEGER_TYPE, expr.getLineNumber(), true, l_val.getIntVal() || r_val.getIntVal());
        }
    }
    else
        return l_val == r_val;
}

Variable Eval::evalIndexOperatorExpression(Expression expr)
{
    auto l_val_expr = expr.getChild(0);
    auto r_val_expr = expr.getChild(1);

    auto l_val = this->evalExpr(l_val_expr);
    auto r_val = this->evalExpr(r_val_expr);

    if (l_val.type != VariableType::INTEGER_TYPE)
    {
        throw Exception("Niepoprawny typ [" + VariableTypeName[l_val.type] + "] został użyty jako indeks", BAD_INDEX, l_val_expr.getLineNumber());
    }

    switch (r_val.type)
    {
    case VariableType::STRING_TYPE:
        if (l_val.getIntVal() >= r_val.getStringVal().size())
        {
            throw Exception("Indeks[" + std::to_string(l_val.getIntVal()) + "] wykracza rozmiar elementu", BAD_INDEX, l_val.getLineNumber());
        }
        return Variable(VariableType::STRING_TYPE, r_val.getLineNumber(), true, -1, -1, r_val.getStringVal().substr(l_val.getIntVal(), 1));
        break;
    case VariableType::LIST_TYPE:
        if (l_val.getIntVal() >= r_val.getListVal().size())
        {
            throw Exception("Indeks[" + std::to_string(l_val.getIntVal()) + "] wykracza rozmiar elementu", BAD_INDEX, l_val.getLineNumber());
        }
        return r_val.getListVal()[l_val.getIntVal()];
        break;
    default:
        throw Exception("Nie można uzyskać indeksowanego elementu typu [" + VariableTypeName[r_val.type] + "]", BAD_INDEX, r_val_expr.getLineNumber());
    }
}

Variable Eval::evalMathOperatorExpression(Expression expr)
{
    auto l_val_expr = expr.getChild(0);
    auto r_val_expr = expr.getChild(1);

    auto l_val = this->evalExpr(l_val_expr);
    auto r_val = this->evalExpr(r_val_expr);

    if (expr.getValue() == "+")
        return l_val + r_val;
    else if (expr.getValue() == "-")
        return l_val - r_val;
    else if (expr.getValue() == "*")
        return l_val * r_val;
    else if (expr.getValue() == "/")
        return l_val / r_val;
    else
        return l_val % r_val;
}

Variable Eval::evalLiteralExpression(Expression expr)
{
    if (expr.getType() == ExpressionType::NUMERIC_LITERAL)
    {
        auto value = expr.getValue();
        if (value.find(".") != value.npos)
        {
            // is float
            if (value.front() == '.')
            {
                value = "0" + value;
            }
            else if (value.back() == '.')
            {
                value += "0";
            }

            float val = std::stof(value);

            return Variable(VariableType::FLOAT_TYPE, expr.getLineNumber(), true, -1, val);
        }
        else
        {
            int val = std::stoi(expr.getValue());
            return Variable(VariableType::INTEGER_TYPE, expr.getLineNumber(), true, val);
        }
    }
    else if (expr.getType() == ExpressionType::LIST_LITERAL)
    {
        std::vector<Variable> list_val;
        for (auto e : expr.getChildren())
            list_val.push_back(this->evalExpr(e));
        return Variable(VariableType::LIST_TYPE, expr.getLineNumber(), true, -1, -1, "UNDEFINED", list_val);
    }
    else
    {
        return Variable(VariableType::STRING_TYPE, expr.getLineNumber(), true, -1, -1, expr.getValue());
    }
}
