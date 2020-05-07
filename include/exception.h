#pragma once
#include "special/std_libraries.h"
#include "special/return_codes.h"

class Exception
{
private:
    int return_code;
    int line_number;
    std::string message;

public:
    Exception(std::string message_v, int return_code_v, int line_number_v = -1)
    {
        this->message = message_v;
        this->return_code = return_code_v;
        this->line_number = line_number_v;
    }

    void printMessage()
    {
        std::cout << "[ERROR CODE " << this->return_code << "] " << this->message;
        if (this->line_number != -1)
            std::cout << " w linijce " << this->line_number;
        std::cout << std::endl;
    }

    int getReturnCode()
    {
        return this->return_code;
    }
};
