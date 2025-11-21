#include <ctime>
#include<chrono>
#include<string>
#include"utils.hpp"

using namespace std;

std::string Utils::get_iso_timestamp() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    
    char buffer[64];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%dT%H:%M:%SZ", 
                 std::gmtime(&time_t));
    return std::string(buffer);
}