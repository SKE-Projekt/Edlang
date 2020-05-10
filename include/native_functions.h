#pragma once
#include "special/std_libraries.h"
#include "special/return_codes.h"
#include "expression.h"
#include "exception.h"
#include "variable.h"

Variable nativePrintFunction(std::vector<Variable> args, int line_number);
Variable nativeLenFunction(std::vector<Variable> args, int line_number);
