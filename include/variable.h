#pragma once
#include "special/std_libraries.h"
#include "special/return_codes.h"
#include "expression.h"
#include "exception.h"

const std::string VariableTypeName[] = {
    "INTEGER_TYPE",
    "FLOAT_TYPE",
    "STRING_TYPE",
    "LIST_TYPE",
};

enum VariableType
{
    INTEGER_TYPE,
    FLOAT_TYPE,
    STRING_TYPE,
    LIST_TYPE,
};

class Variable
{
private:
    int line_number;

    int int_val;
    float float_val;
    std::string string_val;
    std::vector<Variable> list_val;

    bool defined_val;

    void raiseExceptionUndefined()
    {
        throw Exception("Użyto zmiennej przed nadaniem jej wartości", UNDEFINED_VARIABLE_USED, this->line_number);
    }

    // TODO
    // Change the name to something better
    void raiseAssignedToUndefinedVar(Variable diff)
    {
        throw Exception("Próba użycia nieokreślonej wartości", UNDEFINED_VARIABLE_USED, diff.getLineNumber());
    }

    void raiseExceptionBadOperation(std::string operation, Variable other)
    {
        throw Exception("Niepoprawna operacja " + operation + " dla typów " + VariableTypeName[this->type] + " i " + VariableTypeName[other.type], BAD_TYPES_FOR_OPERATION, this->line_number);
    }

    bool isOperable(Variable other, std::string operation)
    {
        if (!other.defined_val || !this->defined_val)
        {
            this->raiseAssignedToUndefinedVar(other);
        }

        if (operation == "%")
        {
            return this->type == VariableType::INTEGER_TYPE && other.type == VariableType::INTEGER_TYPE;
        }

        if (this->type == VariableType::STRING_TYPE || other.type == VariableType::STRING_TYPE)
        {
            return this->type == other.type && operation == "+";
        }
        else if (this->type == VariableType::LIST_TYPE)
        {
            if (operation == "+")
                return other.type == VariableType::LIST_TYPE;
            else if (operation == "-")
                return other.type == VariableType::INTEGER_TYPE;
            else
                return false;
        }

        return true;
    }

public:
    VariableType type;

    Variable()
    {
    }

    Variable(VariableType type_v, int line_number_v, bool defined_val_v = true, int int_val_v = -1, float float_val_v = -1, std::string string_val_v = "UNDEFINED", std::vector<Variable> list_val_v = {})
    {
        this->line_number = line_number_v;

        this->type = type_v;
        this->defined_val = defined_val_v;
        this->int_val = int_val_v;
        this->float_val = float_val_v;
        this->string_val = string_val_v;
        this->list_val = list_val_v;
    }

    int getIntVal()
    {
        if (!this->defined_val)
        {
            this->raiseExceptionUndefined();
        }
        return this->int_val;
    }

    float getFloatVal()
    {
        if (!this->defined_val)
        {
            this->raiseExceptionUndefined();
        }
        return this->float_val;
    }

    std::string getStringVal()
    {
        if (!this->defined_val)
        {
            this->raiseExceptionUndefined();
        }
        return this->string_val;
    }

    std::vector<Variable> getListVal()
    {
        if (!this->defined_val)
        {
            this->raiseExceptionUndefined();
        }

        return this->list_val;
    }

    int getLineNumber()
    {
        return this->line_number;
    }

    // TODO
    // Move this to variable.cpp
    void printVariable()
    {
        std::cout << "VARIABLE: " << std::endl;
        std::cout << "{" << std::endl;
        std::cout << "\tTYPE: " << VariableTypeName[this->type] << std::endl;
        std::cout << "\tDEFINED: " << this->defined_val << std::endl;
        std::cout << "\tVALUE: ";
        switch (this->type)
        {
        case VariableType::INTEGER_TYPE:
            std::cout << this->int_val;
            break;
        case VariableType::FLOAT_TYPE:
            std::cout << this->float_val;
            break;
        case VariableType::STRING_TYPE:
            std::cout << this->string_val;
            break;
        case VariableType::LIST_TYPE:
            for (auto e : this->list_val)
                e.printVariable();
            break;
        }
        std::cout << std::endl;
        std::cout << "}" << std::endl;
    }

    Variable operator+(Variable &diff)
    {
        if (!this->isOperable(diff, "+"))
        {
            this->raiseExceptionBadOperation("+", diff);
        }

        if (this->type == VariableType::STRING_TYPE)
        {
            return Variable(VariableType::STRING_TYPE, this->line_number, true, -1, -1, this->string_val + diff.getStringVal());
        }

        else if (this->type == VariableType::INTEGER_TYPE)
        {
            if (diff.type == VariableType::INTEGER_TYPE)
            {
                return Variable(VariableType::INTEGER_TYPE, this->line_number, true, this->int_val + diff.int_val);
            }
            else
            {
                return Variable(VariableType::INTEGER_TYPE, this->line_number, true, this->int_val + diff.float_val);
            }
        }
        else if (this->type == VariableType::LIST_TYPE)
        {
            if (diff.type == VariableType::LIST_TYPE)
            {
                for (auto e : diff.getListVal())
                    this->list_val.push_back(e);
            }
            else
            {
                this->list_val.push_back(diff);
            }
            return *this;
        }
        else
        {
            if (diff.type == VariableType::INTEGER_TYPE)
            {
                return Variable(VariableType::FLOAT_TYPE, this->line_number, true, -1, this->float_val + diff.int_val);
            }
            else
            {
                return Variable(VariableType::FLOAT_TYPE, this->line_number, true, -1, this->float_val + diff.float_val);
            }
        }
        this->raiseExceptionBadOperation("+", diff);
        return Variable(VariableType::FLOAT_TYPE, this->line_number, true, -1, this->float_val / diff.float_val);
    }

    Variable operator!()
    {
        switch (this->type)
        {
        case VariableType::INTEGER_TYPE:
            this->int_val = this->int_val == 0;
            break;
        case VariableType::FLOAT_TYPE:
            this->int_val = this->float_val == 0.0;
            break;
        case VariableType::STRING_TYPE:
            this->raiseExceptionBadOperation("!", *this);
            break;
        case VariableType::LIST_TYPE:
            this->raiseExceptionBadOperation("!", *this);
            break;
        }

        this->type == VariableType::INTEGER_TYPE;
        return *this;
    }

    Variable operator==(Variable diff)
    {
        if (this->type != diff.type)
        {
            this->raiseExceptionBadOperation("eq", diff);
        }

        switch (this->type)
        {
        case VariableType::INTEGER_TYPE:
            return Variable(VariableType::INTEGER_TYPE, this->line_number, true, this->int_val == diff.getIntVal());
            break;
        case VariableType::FLOAT_TYPE:
            return Variable(VariableType::INTEGER_TYPE, this->line_number, true, this->float_val == diff.getFloatVal());
            break;
        case VariableType::STRING_TYPE:
            return Variable(VariableType::INTEGER_TYPE, this->line_number, true, this->string_val == diff.getStringVal());
            break;
        default:
            auto other_list = diff.getListVal();
            bool equal = true;
            if (this->list_val.size() != other_list.size())
                equal = false;
            else
            {
                for (int i = 0; i < this->list_val.size(); ++i)
                {
                    if (this->list_val[i].type != other_list[i].type)
                    {
                        equal = false;
                        break;
                    }
                    else if ((this->list_val[i] == other_list[i]).getIntVal() == 0)
                    {
                        equal = false;
                        break;
                    }
                }
            }

            return Variable(VariableType::INTEGER_TYPE, this->line_number, true, equal);
            break;
        }
    }

    Variable operator!=(Variable diff)
    {
        if (this->type != diff.type)
        {
            this->raiseExceptionBadOperation("neq", diff);
        }

        switch (this->type)
        {
        case VariableType::INTEGER_TYPE:
            return Variable(VariableType::INTEGER_TYPE, this->line_number, true, this->int_val != diff.getIntVal());
            break;
        case VariableType::FLOAT_TYPE:
            return Variable(VariableType::INTEGER_TYPE, this->line_number, true, this->float_val != diff.getFloatVal());
            break;
        case VariableType::STRING_TYPE:
            return Variable(VariableType::INTEGER_TYPE, this->line_number, true, this->string_val != diff.getStringVal());
            break;
        default:
            auto other_list = diff.getListVal();
            bool equal = true;
            if (this->list_val.size() != other_list.size())
                equal = false;
            else
            {
                for (int i = 0; i < this->list_val.size(); ++i)
                {
                    if (this->list_val[i].type != other_list[i].type)
                    {
                        equal = false;
                        break;
                    }
                    else if ((this->list_val[i] == other_list[i]).getIntVal() == 0)
                    {
                        equal = false;
                        break;
                    }
                }
            }

            return Variable(VariableType::INTEGER_TYPE, this->line_number, true, equal == false);
            break;
        }
    }

    Variable operator>(Variable diff)
    {
        if (this->type != diff.type)
        {
            this->raiseExceptionBadOperation(">", diff);
        }

        switch (this->type)
        {
        case VariableType::INTEGER_TYPE:
            return Variable(VariableType::INTEGER_TYPE, this->line_number, true, this->int_val > diff.getIntVal());
            break;
        case VariableType::FLOAT_TYPE:
            return Variable(VariableType::INTEGER_TYPE, this->line_number, true, this->float_val > diff.getFloatVal());
            break;
        default:
            return Variable(VariableType::INTEGER_TYPE, this->line_number, true, this->string_val > diff.getStringVal());
            break;
        }
    }

    Variable operator<(Variable diff)
    {
        if (this->type != diff.type)
        {
            this->raiseExceptionBadOperation("<", diff);
        }

        switch (this->type)
        {
        case VariableType::INTEGER_TYPE:
            return Variable(VariableType::INTEGER_TYPE, this->line_number, true, this->int_val < diff.getIntVal());
            break;
        case VariableType::FLOAT_TYPE:
            return Variable(VariableType::INTEGER_TYPE, this->line_number, true, this->float_val < diff.getFloatVal());
            break;
        default:
            return Variable(VariableType::INTEGER_TYPE, this->line_number, true, this->string_val < diff.getStringVal());
            break;
        }
    }

    Variable operator<=(Variable diff)
    {
        if (this->type != diff.type)
        {
            this->raiseExceptionBadOperation("leq", diff);
        }

        switch (this->type)
        {
        case VariableType::INTEGER_TYPE:
            return Variable(VariableType::INTEGER_TYPE, this->line_number, true, this->int_val <= diff.getIntVal());
            break;
        case VariableType::FLOAT_TYPE:
            return Variable(VariableType::INTEGER_TYPE, this->line_number, true, this->float_val <= diff.getFloatVal());
            break;
        default:
            return Variable(VariableType::INTEGER_TYPE, this->line_number, true, this->string_val <= diff.getStringVal());
            break;
        }
    }

    Variable operator>=(Variable diff)
    {
        if (this->type != diff.type)
        {
            this->raiseExceptionBadOperation("geq", diff);
        }

        switch (this->type)
        {
        case VariableType::INTEGER_TYPE:
            return Variable(VariableType::INTEGER_TYPE, this->line_number, true, this->int_val >= diff.getIntVal());
            break;
        case VariableType::FLOAT_TYPE:
            return Variable(VariableType::INTEGER_TYPE, this->line_number, true, this->float_val >= diff.getFloatVal());
            break;
        default:
            return Variable(VariableType::INTEGER_TYPE, this->line_number, true, this->string_val >= diff.getStringVal());
            break;
        }
    }

    Variable operator-(Variable &diff)
    {
        if (!this->isOperable(diff, "-"))
        {
            this->raiseExceptionBadOperation("-", diff);
        }
        else if (this->type == VariableType::LIST_TYPE)
        {
            int val = diff.getIntVal();
            while (!this->list_val.empty() && (val--))
            {
                this->list_val.pop_back();
            }
            return *this;
        }
        else if (this->type == VariableType::INTEGER_TYPE)
        {
            if (diff.type == VariableType::INTEGER_TYPE)
            {
                return Variable(VariableType::INTEGER_TYPE, this->line_number, true, this->int_val - diff.int_val);
            }
            else
            {
                return Variable(VariableType::INTEGER_TYPE, this->line_number, true, this->int_val - diff.float_val);
            }
        }
        else
        {
            if (diff.type == VariableType::INTEGER_TYPE)
            {
                return Variable(VariableType::FLOAT_TYPE, this->line_number, true, -1, this->float_val - diff.int_val);
            }
            else
            {
                return Variable(VariableType::FLOAT_TYPE, this->line_number, true, -1, this->float_val - diff.float_val);
            }
        }
        this->raiseExceptionBadOperation("-", diff);
        return Variable(VariableType::FLOAT_TYPE, this->line_number, true, -1, this->float_val / diff.float_val);
    }

    Variable operator*(Variable &diff)
    {
        if (!this->isOperable(diff, "*"))
        {
            this->raiseExceptionBadOperation("*", diff);
        }

        else if (this->type == VariableType::INTEGER_TYPE)
        {
            if (diff.type == VariableType::INTEGER_TYPE)
            {
                return Variable(VariableType::INTEGER_TYPE, this->line_number, true, this->int_val * diff.int_val);
            }
            else
            {
                return Variable(VariableType::INTEGER_TYPE, this->line_number, true, this->int_val * diff.float_val);
            }
        }
        else
        {
            if (diff.type == VariableType::INTEGER_TYPE)
            {
                return Variable(VariableType::FLOAT_TYPE, this->line_number, true, -1, this->float_val * diff.int_val);
            }
            else
            {
                return Variable(VariableType::FLOAT_TYPE, this->line_number, true, -1, this->float_val * diff.float_val);
            }
        }
        this->raiseExceptionBadOperation("*", diff);
        return Variable(VariableType::FLOAT_TYPE, this->line_number, true, -1, this->float_val / diff.float_val);
    }

    Variable operator/(Variable &diff)
    {
        if (!this->isOperable(diff, "/"))
        {
            this->raiseExceptionBadOperation("/", diff);
        }

        else if (this->type == VariableType::INTEGER_TYPE)
        {
            if (diff.type == VariableType::INTEGER_TYPE)
            {
                return Variable(VariableType::INTEGER_TYPE, this->line_number, true, this->int_val / diff.int_val);
            }
            else
            {
                return Variable(VariableType::INTEGER_TYPE, this->line_number, true, this->int_val / diff.float_val);
            }
        }
        else
        {
            if (diff.type == VariableType::INTEGER_TYPE)
            {
                return Variable(VariableType::FLOAT_TYPE, this->line_number, true, -1, this->float_val / diff.int_val);
            }
            else
            {
                return Variable(VariableType::FLOAT_TYPE, this->line_number, true, -1, this->float_val / diff.float_val);
            }
        }

        this->raiseExceptionBadOperation("/", diff);
        return Variable(VariableType::FLOAT_TYPE, this->line_number, true, -1, this->float_val / diff.float_val);
    }

    Variable operator%(Variable &diff)
    {
        if (!this->isOperable(diff, "%"))
        {
            this->raiseExceptionBadOperation("%", diff);
        }

        return Variable(VariableType::INTEGER_TYPE, this->line_number, true, this->int_val % diff.int_val);
    }

    Variable assign(Variable diff)
    {
        if (!diff.defined_val)
        {
            this->raiseAssignedToUndefinedVar(diff);
        }

        if (this->type != diff.type)
        {
            this->raiseExceptionBadOperation("=", diff);
        }

        this->defined_val = true;
        switch (this->type)
        {
        case VariableType::INTEGER_TYPE:
            this->int_val = diff.int_val;
            break;
        case VariableType::FLOAT_TYPE:
            this->float_val = diff.float_val;
            break;
        case VariableType::STRING_TYPE:
            this->string_val = diff.string_val;
            break;
        case VariableType::LIST_TYPE:
            this->list_val = diff.list_val;
            break;
        }

        return *this;
    }

    Variable readNatively()
    {
        this->defined_val = true;
        switch (this->type)
        {
        case VariableType::INTEGER_TYPE:
            std::cin >> this->int_val;
            break;
        case VariableType::FLOAT_TYPE:
            std::cin >> this->float_val;
            break;
        case VariableType::STRING_TYPE:
            std::cin >> this->string_val;
            break;
        case VariableType::LIST_TYPE:
            for (auto e : this->list_val)
                e.readNatively();
            break;
        }

        return *this;
    }

    void printNatively()
    {
        if (!this->defined_val)
        {
            this->raiseExceptionUndefined();
        }

        switch (this->type)
        {
        case VariableType::INTEGER_TYPE:
            std::cout << this->int_val;
            break;
        case VariableType::FLOAT_TYPE:
            std::cout << this->float_val;
            break;
        case VariableType::STRING_TYPE:
            std::cout << this->string_val;
            break;
        case VariableType::LIST_TYPE:
            std::cout << "{";
            for (int i = 0; i < this->list_val.size() - 1; ++i)
            {
                this->list_val[i].printNatively();
                std::cout << ", ";
            }
            if (!this->list_val.empty())
            {
                this->list_val.back().printNatively();
            }
            std::cout << "}";
        }
    }

    void undefy()
    {
        this->defined_val = false;
    }
};
