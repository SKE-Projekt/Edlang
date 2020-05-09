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
        return this->evalLiteralExpression(expr);
        break;
    case ExpressionType::SYMBOLIC_VALUE:
        return this->searchForVariable(expr.getValue(), expr.getLineNumber());
    case ExpressionType::EXPR_MATH_OPERATOR:
        return this->evalMathOperatorExpression(expr);
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
    default:
        throw Exception("Nieobsługiwane wyrażenie", UNHANDLED_EXPRESSION, expr.getLineNumber());
    }
}

Variable Eval::evalFunctionCall(Expression expr)
{
    auto name_expr = expr.getChild(0);
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
    args_provided.printExpression();
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

Variable Eval::evalVariableAssignment(Expression expr)
{
    auto l_val_expr = expr.getChild(0);
    auto r_val_expr = expr.getChild(1);

    auto r_val = this->evalExpr(r_val_expr);
    this->searchForVariable(l_val_expr.getValue(), l_val_expr.getLineNumber(), true, r_val);

    return r_val;
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
    else
    {
        return Variable(VariableType::STRING_TYPE, expr.getLineNumber(), true, -1, -1, expr.getValue());
    }
}
