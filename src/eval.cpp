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
        return this->getScope().getVariable(expr.getValue(), expr.getLineNumber());
    case ExpressionType::EXPR_MATH_OPERATOR:
        return this->evalMathOperatorExpression(expr);
        break;
    case ExpressionType::DECLARATION:
        return this->evalVariableDeclaration(expr);
        break;
    case ExpressionType::EXPR_ASSIGNMENT:
        return this->evalVariableAssignment(expr);
        break;
    default:
        throw Exception("Nieobsługiwane wyrażenie", UNHANDLED_EXPRESSION, expr.getLineNumber());
    }
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
        std::cout << "TEST:" << std::endl;
        assign_expr.printExpression();
        return this->evalExpr(assign_expr);
    }

    return var;
}

Variable Eval::evalVariableAssignment(Expression expr)
{
    auto l_val_expr = expr.getChild(0);
    auto r_val_expr = expr.getChild(1);

    auto r_val = this->evalExpr(r_val_expr);
    this->getScope().assignVariable(l_val_expr.getValue(), r_val);

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
