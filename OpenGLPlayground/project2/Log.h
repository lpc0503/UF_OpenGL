#pragma once
#ifndef OPENGLPLAYGROUND_LOG_H
#define OPENGLPLAYGROUND_LOG_H

#include <string>
#include <iostream>

#include <fmt/format.h>

class Log
{
public:
    template<typename... Args>
    static void Info(const std::string &fmt, Args&&... args);

    template<typename... Args>
    static void InfoTag(const std::string &tag, const std::string &fmt, Args&&... args);

    static void Output(const std::string &str);
    static void OutputLine(const std::string &str);
};

template<typename... Args>
void Log::Info(const std::string &fmt, Args&&... args)
{
    std::string s = "[Info] ";
    s += fmt::format(fmt, std::forward<Args>(args)...);
    OutputLine(s);
}

template<typename... Args>
void Log::InfoTag(const std::string &tag, const std::string &fmt, Args&&... args)
{
    std::string s = "[Info] ";
    s += fmt::format("[{}] ", tag);
    s += fmt::format(fmt, std::forward<Args>(args)...);
    OutputLine(s);
}


#define INFO(...) Log::Info(__VA_ARGS__)

#define INFO_TAG(...) Log::InfoTag(__VA_ARGS__)

#endif //OPENGLPLAYGROUND_LOG_H
