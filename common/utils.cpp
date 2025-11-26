#include <ctime>
#include <chrono>
#include <string>
#include <iomanip>
#include <random>
#include <iomanip>
#include <sstream>
#include "utils.hpp"

using namespace std;

std::string Utils::get_iso_timestamp()
{
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);

    char buffer[64];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%dT%H:%M:%SZ",
                  std::gmtime(&time_t));
    return std::string(buffer);
}

std::string Utils::get_uuid()
{
    // 获取当前时间戳（毫秒）
    auto now = std::chrono::system_clock::now();
    auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
                         now.time_since_epoch())
                         .count();

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1000, 9999);

    std::stringstream ss;
    ss << std::hex << timestamp << dis(gen);

    std::string result = ss.str();
    return result.substr(0, 12);
}