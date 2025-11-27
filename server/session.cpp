#include <iostream>
#include <chrono>
#include "session_manager.hpp"
#include "session.hpp"
#include "../common/log_helper.hpp"
#include "../common/utils.hpp"
using namespace std;

void Session::handle_session()
{
    while (true)
    {
        if (!check_online(socket))
        {
            session_manager->close_session(socket);
            break;
        }
        ClientPacket received_packet = recv_client_packet(socket);
        if (received_packet.request == ClientMessage::REGISTER && received_packet.user_id == "")
        {
            session_manager->close_session(socket);
            break;
        }
        switch (received_packet.request)
        {
        case ClientMessage::LOGIN:
        {
            ServerStatus status = handle_login(received_packet);
            ServerPacket packet;
            packet.status = status;
            packet.request = ServerMessage::LOGIN_RESPONSE;
            if (status != ServerStatus::SUCCESS)
            {
                log(LogLevel::ERROR, "user_id: " + received_packet.user_id + " login failed!");
            }
            else
            {
                packet.username = username;
                packet.user_id = user_id;
                log(LogLevel::INFO, "user_id: " + user_id + " login success!");
            }

            send_packet(socket, packet);
            break;
        }
        case ClientMessage::REGISTER:
        {
            ServerStatus status = handle_register(received_packet);
            ServerPacket packet;
            packet.username = username;
            packet.user_id = user_id;
            packet.request = ServerMessage::REGISTER_RESPONSE;
            packet.status = status;

            send_packet(socket, packet);
            break;
        }
        case ClientMessage::MESSAGE:
        {
            handle_message(received_packet);
            log(LogLevel::INFO, user_id + ": " + received_packet.message);
            break;
        }
        case ClientMessage::RECALL:
        {
            ServerStatus status = handle_recall_message(received_packet);
            ServerPacket packet;
            packet.request = ServerMessage::NEW_MESSAGE;  // 使用现有的响应类型
            packet.status = status;
            send_packet(socket, packet);
            break;
        }
        case ClientMessage::LEAVE_CHAT:
        {
            ServerStatus status = handle_leave_chat(received_packet);
            ServerPacket packet;
            packet.request = ServerMessage::JOIN_CHAT_RESPONSE;  // 复用现有响应类型
            packet.status = status;
            packet.chat_id = received_packet.chat_id;
            send_packet(socket, packet);
            break;
        }
        case ClientMessage::CREATE_CHAT:
        {
            string uuid = Utils::get_uuid();
            ServerStatus status = handle_new_chat(received_packet, uuid);
            ServerPacket packet;
            packet.request = ServerMessage::CREATE_CHAT_RESPONSE;
            packet.status = status;
            packet.chat_id = uuid;
            packet.chatname = received_packet.chatname;
            // 修复：添加user_id和username到响应中
            packet.user_id = user_id;
            packet.username = username;
            ChatInfo info;
            info.chat_id = uuid;
            info.chatname = received_packet.chatname;
            info.role = "member";
            packet.chats.push_back(info);
            send_packet(socket, packet);
            break;
        }
        case ClientMessage::JOIN_CHAT:
        {
            ServerStatus status = handle_join_chat(received_packet);
            ServerPacket packet;
            packet.request = ServerMessage::JOIN_CHAT_RESPONSE;
            packet.status = status;
            packet.chat_id = received_packet.chat_id;
            packet.chatname = session_manager->get_database()->get_chatname(received_packet.chat_id);
            // 修复：添加user_id和username到响应中
            packet.user_id = user_id;
            packet.username = username;
            ChatInfo info;
            info.chat_id = received_packet.chat_id;
            info.chatname = received_packet.chatname;
            info.role = "member";
            packet.chats.push_back(info);
            send_packet(socket, packet);
            break;
        }
        case ClientMessage::LIST_CHATS:
        {
            ServerPacket packet;
            packet.request = ServerMessage::RETURN_CHATS;
            packet.status = ServerStatus::SUCCESS;  // 修复：设置状态码
            // 修复：添加user_id和username到响应中
            packet.user_id = user_id;
            packet.username = username;
            packet.chats = session_manager->get_database()->list_user_chats(received_packet.user_id);
            send_packet(socket, packet);
            break;
        }
        case ClientMessage::FETCH_MESSAGES:
        {
            ServerPacket packet;
            packet.request = ServerMessage::RETURN_MESSAGES;
            packet.status = ServerStatus::SUCCESS;  // 修复：设置状态码
            // 修复：添加user_id和username到响应中
            packet.user_id = user_id;
            packet.username = username;
            packet.message_list = session_manager->get_database()->fetch_chat_messages(received_packet.chat_id);
            send_packet(socket, packet);
            break;
        }
        default:
            break;
        }
    }
}

ServerStatus Session::handle_login(const ClientPacket &packet)
{
    Database *db = session_manager->get_database();

    if (!db->exist_user(packet.user_id))
        return ServerStatus::USER_NOT_FOUND;

    if (packet.password_hash == db->get_password_hash(packet.user_id))
    {
        user_id = packet.user_id;
        username = db->get_username(packet.user_id);
        return ServerStatus::SUCCESS;
    }
    else
        return ServerStatus::INVALID_PASSWORD;
}

ServerStatus Session::handle_register(const ClientPacket &packet)
{
    Database *db = session_manager->get_database();

    if (db->exist_user(packet.user_id))
        return ServerStatus::USER_EXISTS;
    else
    {
        db->add_user(packet.user_id, packet.username, packet.password_hash);
        user_id = packet.user_id;
        username = packet.username;
        return ServerStatus::SUCCESS;
    }
}

void Session::handle_message(const ClientPacket &packet)
{
    Database *db = session_manager->get_database();

    if (!db->chat_exist(packet.chat_id))
        return;

    db->add_message(packet.user_id, packet.chat_id, packet.message_id, packet.message);
    vector<string> members = db->get_chat_members(packet.chat_id);
    for (string member : members)
    {
        if (member == packet.user_id)
            continue;

        Message message(packet, Utils::get_iso_timestamp());
        ServerPacket response_packet;
        response_packet.request = ServerMessage::NEW_MESSAGE;
        response_packet.status = ServerStatus::SUCCESS;
        response_packet.message_list.push_back(message);
        
        // 修复：需要根据用户ID找到对应的socket
        int target_socket = session_manager->get_socket_by_user_id(member);
        if (target_socket != -1) {
            send_packet(target_socket, response_packet);
        } else {
            log(LogLevel::ERROR, "Cannot find socket for user_id: " + member);
        }
    }
}

ServerStatus Session::handle_new_chat(const ClientPacket &packet, const string &uuid)
{
    Database *db = session_manager->get_database();
    if (db->chat_exist(uuid))
        return ServerStatus::CHAT_EXISTS;

    db->add_chat(packet.user_id, uuid, packet.chatname);
    db->add_chat_member(packet.user_id, uuid, "admin");
    return ServerStatus::SUCCESS;
}

ServerStatus Session::handle_join_chat(const ClientPacket &packet)
{
    Database *db = session_manager->get_database();
    if (db->chat_exist(packet.chat_id) == false)
        return ServerStatus::CHAT_NOT_FOUND;

    db->add_chat_member(packet.user_id, packet.chat_id, "member");
    return ServerStatus::SUCCESS;
}

ServerStatus Session::handle_recall_message(const ClientPacket &packet)
{
    Database *db = session_manager->get_database();
    
    // 删除消息
    bool success = db->delete_message(packet.message_id);
    if (success) {
        log(LogLevel::INFO, "Message " + packet.message_id + " recalled by " + packet.user_id);
        return ServerStatus::SUCCESS;
    } else {
        log(LogLevel::ERROR, "Failed to recall message " + packet.message_id);
        return ServerStatus::CHAT_NOT_FOUND;  // 使用现有状态码表示消息不存在
    }
}

ServerStatus Session::handle_leave_chat(const ClientPacket &packet)
{
    Database *db = session_manager->get_database();
    
    // 退出聊天室
    bool success = db->leave_chat(packet.user_id, packet.chat_id);
    if (success) {
        log(LogLevel::INFO, "User " + packet.user_id + " left chat " + packet.chat_id);
        return ServerStatus::SUCCESS;
    } else {
        log(LogLevel::ERROR, "Failed to leave chat " + packet.chat_id);
        return ServerStatus::CHAT_NOT_FOUND;  // 使用现有状态码表示聊天室不存在
    }
}
