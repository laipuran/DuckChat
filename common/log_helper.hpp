#pragma once
#include <iostream>
#include <string>
#include <cstdlib>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <memory>

enum class LogLevel
{
    INFO,
    ERROR
};

// 获取日志输出流
inline std::ostream& get_log_stream() {
    const std::string ENVIRONMENT = std::getenv("ENVIRONMENT") ? std::getenv("ENVIRONMENT") : "DEVELOPMENT";
    
    if (ENVIRONMENT == "DEVELOPMENT") {
        // 开发环境：使用静态文件流
        static std::ofstream log_file("data/duckchat.log", std::ios::app);
        return log_file;
    } else {
        // 生产环境：根据日志级别返回相应的标准流
        return std::cout;
    }
}

// 获取错误日志输出流
inline std::ostream& get_error_log_stream() {
    const std::string ENVIRONMENT = std::getenv("ENVIRONMENT") ? std::getenv("ENVIRONMENT") : "DEVELOPMENT";
    
    if (ENVIRONMENT == "DEVELOPMENT") {
        // 开发环境：使用静态文件流
        static std::ofstream log_file("data/duckchat.log", std::ios::app);
        return log_file;
    } else {
        // 生产环境：使用标准错误流
        return std::cerr;
    }
}

inline void log(LogLevel level, std::string log_msg)
{
    // 获取当前时间
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    // 格式化时间字符串
    std::stringstream time_ss;
    time_ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    time_ss << "." << std::setfill('0') << std::setw(3) << ms.count();
    
    // 根据日志级别选择输出流
    std::ostream& stream = (level == LogLevel::ERROR) ? get_error_log_stream() : get_log_stream();
    
    // 输出日志
    stream << "[" << time_ss.str() << "] ["
           << (level == LogLevel::INFO ? "INF" : "ERR") << "] "
           << log_msg << std::endl;
}