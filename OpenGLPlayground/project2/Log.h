#pragma once
#ifndef OPENGLPLAYGROUND_LOG_H
#define OPENGLPLAYGROUND_LOG_H

#include <string>
#include <fmt/format.h>

class Log
{
public:
    template<typename... Args>
    static void Info(const std::string &fmt, Args&&... args);
    template<typename... Args>
    static void Error(const std::string &fmt, Args&&... args);

    template<typename... Args>
    static void InfoTag(const std::string &tag, const std::string &fmt, Args&&... args);
    template<typename... Args>
    static void ErrorTag(const std::string &tag, const std::string &fmt, Args&&... args);

    static void Output(const std::string &str);
    static void OutputLine(const std::string &str) { Output(str + "\n"); }
    static void OutputError(const std::string &str);
    static void OutputErrorLine(const std::string &str) { OutputError(str + "\n"); }
};

template<typename... Args>
void Log::Info(const std::string &fmt, Args&&... args)
{
    std::string s = "[Info] ";
    s += fmt::format(fmt, std::forward<Args>(args)...);
    OutputLine(s);
}

template<typename... Args>
void Log::Error(const std::string &fmt, Args&&... args)
{
    std::string s = "[Error] ";
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

template<typename... Args>
void Log::ErrorTag(const std::string &tag, const std::string &fmt, Args&&... args)
{
    std::string s = "[Error] ";
    s += fmt::format("[{}] ", tag);
    s += fmt::format(fmt, std::forward<Args>(args)...);
    OutputLine(s);
}

// TODO: refactor

#define INFO(...) Log::Info(__VA_ARGS__)
#define ERROR(...) Log::Error(__VA_ARGS__)
#define INFO_TAG(...) Log::InfoTag(__VA_ARGS__)
#define ERROR_TAG(...) Log::ErrorTag(__VA_ARGS__)

#endif //OPENGLPLAYGROUND_LOG_H
