#include "Log.h"

void Log::Output(const std::string &str)
{
    std::cout << str;
}

void Log::OutputLine(const std::string &str)
{
    Output(str + "\n");
}
