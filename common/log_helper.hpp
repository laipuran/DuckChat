#pragma once
#include <iostream>
#include <string>

enum class LogLevel
{
    INFO,
    ERROR
};

inline void log(LogLevel level, std::string log)
{
    switch (level)
    {
    case LogLevel::INFO:
        std::cout << "[INF]";
        break;
    case LogLevel::ERROR:
        std::cout << "[ERR]";
        break;
    default:
        break;
    }
    std::cout << log << std::endl;
}