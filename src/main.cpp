#include "../include/special/std_libraries.h"
#include "../include/exception.h"
#include "../include/parser.h"
#include "../include/lexer.h"
#include "../include/eval.h"

std::string readSourceCode(std::string source_code_file)
{
	std::ifstream source_code_input(source_code_file);
	if (!source_code_input)
		throw Exception("Podany plik źródłowy jest nieprawidłowy", BAD_SOURCE_FILE);

	std::stringstream buffer;
	buffer << source_code_input.rdbuf();
	return buffer.str();
}

int main(int argc, char *argv[])
{
	try
	{
		if (argc < 2)
			throw Exception("Nie podano pliku źródłowego", NO_SOURCE_FILE);
		std::string source_code = readSourceCode(argv[1]);

		bool debug = false;
		if (argc >= 3)
		{
			if (argv[2] == std::string("DEBUG_ON"))
			{
				debug = true;
			}
			else if (argv[2] == std::string("DEBUG_OFF"))
			{
				debug = false;
			}
			else
			{
				std::cout << argv[2] << " : " << (argv[2] == std::string("DEBUG_ON")) << std::endl;
				throw Exception("Niepoprawna wartość argumentu na pozycji 2", BAD_ARGUMENT_VALUE);
			}
		}

		Lexer lexer(source_code, debug);
		lexer.lex();

		Parser parser(lexer.getTokens(), debug);
		parser.parse();

		Eval eval(parser.getExprs(), debug);
		eval.eval();
	}
	catch (Exception e)
	{
		e.printMessage();
		return e.getReturnCode();
	}

	return SUCCESS;
}
