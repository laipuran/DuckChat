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
    std::vector<std::unique_ptr<Session>> sessions;

public:
    SessionManager();

    void create_and_start_session(int socket);
    void close_session(int socket);
    Database *get_database();
};