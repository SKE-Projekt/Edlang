#pragma once

// TODO
// Fix them up, tidy them. Make sure all are ACTUALLY USEFULL

// Definitions of return codes for the language
const int SUCCESS = 0;

// Errors
const int NO_SOURCE_FILE = 1;
const int BAD_SOURCE_FILE = 2;
const int BAD_ARGUMENT_VALUE = 3;
const int NO_INPUT_PROVIDED = 4;

const int UNRECOGNIZED_CHAR = 11;
const int INCORRECT_CHAR_IN_NUMERIC_VALUE = 12;
const int UNHANDLED_OPERATOR = 13;
const int EXPECTED_EOF_TOKEN = 14;
const int UNHANDLED_EXPRESSION = 15;
const int UNEXPECTED_TOKEN_IN_EXPR = 16;
const int BAD_PARENTHESIS_EXPR = 17;
const int BAD_STRING_LITERAL = 18;
const int EXPECTED_MORE_TOKENS = 19;
const int BAD_VARIABLE_DECLARATION = 20;
const int UNEXPECTED_SYMBOLIC_NAME = 21;
const int EXPR_NOT_EVALUATING = 22;
const int BAD_IF_EXPR = 23;
const int BAD_FUNCTION_DECLARATION = 24;

const int EXPR_IS_MISSING = 25;
const int UNDEFINED_VARIABLE_USED = 26;
const int INCORRECT_EXPRESSION_STRUCTURE = 27;
const int BAD_TYPES_FOR_OPERATION = 28;
const int REDECLARATION_OF_SYMBOL = 29;
const int SYMBOL_NOT_DECLARED = 30;

const int BAD_FUNCTION_CALL = 31;
const int EMPTY_FUNCTION_BODY = 32;
const int BAD_FUNCTION_USAGE = 33;

const int INCORRECT_TYPE = 34;
const int LOOP_COND_NOT_FOUND = 35;

const int BAD_INDEX = 36;

const int READ_FROM_EMPTY_INPUT = 37;
