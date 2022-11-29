#include "Log.h"

#include <iostream>

void Log::Output(const std::string &str)
{
    std::cout << str;
}

void Log::OutputError(const std::string &str)
{
    std::cerr << str;
}
