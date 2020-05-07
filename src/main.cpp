#include "../include/special/std_libraries.h"
#include "../include/exception.h"

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
		if (argc != 2)
			throw Exception("Nie podano pliku źródłowego", NO_SOURCE_FILE);

		std::string source_code = readSourceCode(argv[1]);
		std::cout << source_code << std::endl;
	}
	catch (Exception e)
	{
		e.Print();
		return e.getReturnCode();
	}

	return SUCCESS;
}
