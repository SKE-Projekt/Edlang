#include "special/std_libraries.h"
#include "special/return_codes.h"
#include "expression.h"
#include "exception.h"

const std::string VariableTypeName[] = {
    "INTEGER_TYPE",
    "FLOAT_TYPE",
    "STRING_TYPE",
};

enum VariableType
{
    INTEGER_TYPE,
    FLOAT_TYPE,
    STRING_TYPE,
};

class Variable
{
private:
    int line_number;

    int int_val;
    float float_val;
    std::string string_val;

    bool defined_val;

    void raiseExceptionUndefined()
    {
        throw Exception("Użyto zmiennej przed nadaniem jej wartości", UNDEFINED_VARIABLE_USED, this->line_number);
    }

    void raiseExceptionBadOperation(std::string operation, Variable other)
    {
        throw Exception("Niepoprawna operacja " + operation + " dla typów " + VariableTypeName[this->type] + " i " + VariableTypeName[other.type], BAD_TYPES_FOR_OPERATION, this->line_number);
    }

    bool isOperable(Variable other, std::string operation)
    {
        if (operation == "%")
        {
            return this->type == VariableType::INTEGER_TYPE && other.type == VariableType::INTEGER_TYPE;
        }

        if (this->type == VariableType::STRING_TYPE || other.type == VariableType::STRING_TYPE)
        {
            return this->type == other.type && operation == "+";
        }

        return true;
    }

public:
    VariableType type;
    Variable(VariableType type_v, int line_number_v, bool defined_val_v = true, int int_val_v = -1, float float_val_v = -1, std::string string_val_v = "UNDEFINED")
    {
        this->line_number = line_number_v;

        this->type = type_v;
        this->defined_val = defined_val_v;
        this->int_val = int_val_v;
        this->float_val = float_val_v;
        this->string_val = string_val_v;
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

    void printVariable()
    {
        std::cout << "ZMIENNA: " << std::endl;
        std::cout << "{" << std::endl;
        std::cout << "\tTYP: " << VariableTypeName[this->type] << std::endl;
        std::cout << "\tZDEFINIOWANA: " << this->defined_val << std::endl;
        std::cout << "\tWARTOSC: ";
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
    }
};
