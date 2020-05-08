#pragma once

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
