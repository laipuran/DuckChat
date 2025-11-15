#pragma once
#include<string>
#include<vector>
#include"session.hpp"

class SessionManager{
    private:
    std::vector<std::unique_ptr<Session>> sessions;

    public:
    void create_and_start_session(int socket);
    void close_session(int socket);
};