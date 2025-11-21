#pragma once
#include <string>
#include <vector>
#include<mutex>
#include "session.hpp"

class SessionManager
{
private:
    Database database;
    std::mutex sessions_mutex;
    std::unordered_map<int, std::unique_ptr<Session>> sessions;

public:
    SessionManager();

    void create_and_start_session(int socket);
    void close_session(int socket);
    Database *get_database();
};