#pragma once
#include "../third_party/json.hpp"
#include <cstring>
#include <vector>
using Json = nlohmann::json;

enum class ClientMessage
{
    REGISTER,
    LOGIN,
    MESSAGE,
    RECALL,
    LIST_CHATS,
    FETCH_MESSAGES,
    CREATE_CHAT,
    JOIN_CHAT,
    LEAVE_CHAT,
    LOGOUT,
};

struct ClientPacket
{
    ClientMessage request;

    std::string username;
    std::string password; // 同时用于 LOGIN

    std::string user_id; // 用于所有消息发送和撤回

    std::string chat_id; // 同时用于 FETCH_MESSAGES
    std::string chatname;

    std::string message_id; // 同时用于 RECALL
    std::string message;

    NLOHMANN_JSON_SERIALIZE_ENUM(
        ClientMessage,
        {
            {ClientMessage::REGISTER, "REGISTER"},
            {ClientMessage::LOGIN, "LOGIN"},
            {ClientMessage::MESSAGE, "MESSAGE"},
            {ClientMessage::RECALL, "RECALL"},
            {ClientMessage::LIST_CHATS, "LIST_CHATS"},
            {ClientMessage::FETCH_MESSAGES, "FETCH_MESSAGES"},
            {ClientMessage::CREATE_CHAT, "CREATE_CHAT"},
            {ClientMessage::JOIN_CHAT, "JOIN_CHAT"},
            {ClientMessage::LEAVE_CHAT, "LEAVE_CHAT"},
            {ClientMessage::LOGOUT, "LOGOUT"},
        })
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(
        ClientPacket, request,
        username, password, user_id,
        chat_id, chatname, message_id, message)
};

enum class ServerMessage
{
    REGISTER_RESPONSE,
    LOGIN_RESPONSE,
    CREATE_CHAT_RESPONSE,
    JOIN_CHAT_RESPONSE,
    RETURN_MESSAGES
};

struct ServerPacket
{
    ServerMessage request;

    int status;
    std::string user_id;
    std::string username;
    std::string chat_id;
    std::vector<ClientPacket> message_list;

    NLOHMANN_JSON_SERIALIZE_ENUM(
        ServerMessage,
        {
            {ServerMessage::REGISTER_RESPONSE, "REGISTER_RESPONSE"},
            {ServerMessage::LOGIN_RESPONSE, "LOGIN_RESPONSE"},
            {ServerMessage::CREATE_CHAT_RESPONSE, "CREATE_CHAT_RESPONSE"},
            {ServerMessage::JOIN_CHAT_RESPONSE, "JOIN_CHAT_RESPONSE"},
            {ServerMessage::RETURN_MESSAGES, "RETURN_MESSAGES"},
        })
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(
        ServerPacket, request, status,
        user_id, username, chat_id,
        message_list)
};
