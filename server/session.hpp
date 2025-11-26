#pragma once
#include <string>
#include "database.hpp"
#include "../common/protocal.hpp"
#include "../common/network.hpp"

class SessionManager;

class Session
{
private:
    int socket;
    SessionManager *session_manager;

public:
    std::string username;
    std::string user_id;

    Session(int socket, SessionManager *session_manager)
        : socket(socket), session_manager(session_manager) {};
    ~Session() = default;

    void handle_session();

    ServerStatus handle_login(const ClientPacket &packet);
    ServerStatus handle_register(const ClientPacket &packet);
    ServerStatus handle_new_chat(const ClientPacket &packet, const std::string &uuid);
    ServerStatus handle_join_chat(const ClientPacket &packet);

    void handle_message(const ClientPacket &packet);
};