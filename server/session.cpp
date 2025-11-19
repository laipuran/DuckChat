#include <iostream>
#include "session_manager.hpp"
#include "session.hpp"
#include "../common/log_helper.hpp"
using namespace std;

void Session::handle_session()
{
    while (true)
    {
        ClientPacket packet = recv_client_packet(this->socket);
        switch (packet.request)
        {
        case ClientMessage::LOGIN:
        {
            ServerStatus status = handle_login(packet, *this);
            ServerPacket packet;
            packet.request = ServerMessage::LOGIN_RESPONSE;
            if (status != ServerStatus::SUCCESS)
            {
                packet.status = status;
                log(LogLevel::ERROR, "Login Failed!");
            }
            else
                packet.username = username;

            send_packet(socket, packet);
            break;
        }
        case ClientMessage::REGISTER:
        {
            ServerStatus status = handle_register(packet, *this);
            ServerPacket packet;
            packet.request = ServerMessage::REGISTER_RESPONSE;
            packet.status = status;

            send_packet(socket, packet);
            break;
        }
        case ClientMessage::MESSAGE:
        {
            log(LogLevel::INFO, user_id + ": " + packet.message);
            break;
        }

        default:
            break;
        }
    }
}

ServerStatus Session::handle_login(const ClientPacket &packet, Session &session)
{
    Database *db = session_manager->get_database();

    if (!db->exist_user(packet.user_id))
        return ServerStatus::USER_NOT_FOUND;

    if (packet.password_hash == db->get_password_hash(packet.user_id))
    {
        session.user_id = packet.user_id;
        session.username = db->get_username(packet.user_id);
        return ServerStatus::SUCCESS;
    }
    else
        return ServerStatus::INVALID_PASSWORD;
}

ServerStatus Session::handle_register(const ClientPacket &packet, Session &session)
{
    Database *db = session_manager->get_database();

    if (db->exist_user(packet.user_id))
        return ServerStatus::User_EXISTS;
    else
    {
        db->add_user(packet.user_id, packet.username, packet.password_hash);
        session.user_id = packet.user_id;
        session.username = packet.username;
        return ServerStatus::SUCCESS;
    }
}