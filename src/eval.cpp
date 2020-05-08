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
    case ExpressionType::EXPR_MATH_OPERATOR:
        return this->evalMathOperatorExpression(expr);
        break;
    default:
        throw Exception("Nieobsługiwane wyrażenie", UNHANDLED_EXPRESSION, expr.getLineNumber());
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
    else if (expr.getValue() == "%")
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
