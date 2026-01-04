#pragma once
#include <iostream>
#include <string>
#include <cstdlib>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <sstream>

enum class LogLevel
{
    INFO,
    ERROR
};

const std::string ENVIRONMENT = std::getenv("ENVIRONMENT");

inline void log(LogLevel level, std::string log)
{
    if (ENVIRONMENT == "PRODUCTION") return;
    // 获取当前时间
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    // 格式化时间字符串
    std::stringstream time_ss;
    time_ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    time_ss << "." << std::setfill('0') << std::setw(3) << ms.count();
    
    // 打开日志文件（追加模式）
    std::ofstream log_file("data/duckchat.db", std::ios::app);
    if (log_file.is_open())
    {
        switch (level)
        {
        case LogLevel::INFO:
            log_file << "[" << time_ss.str() << "] [INF] ";
            break;
        case LogLevel::ERROR:
            log_file << "[" << time_ss.str() << "] [ERR] ";
            break;
        default:
            break;
        }
        log_file << log << std::endl;
        log_file.close();
    }
    
    // // 同时保持控制台输出
    // switch (level)
    // {
    // case LogLevel::INFO:
    //     std::cout << "[INF]";
    //     break;
    // case LogLevel::ERROR:
    //     std::cout << "[ERR]";
    //     break;
    // default:
    //     break;
    // }
    // std::cout << log << std::endl;
}