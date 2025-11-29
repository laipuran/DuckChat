#pragma once
#include "../third_party/json.hpp"
#include <vector>
#include <string>
using Json = nlohmann::json;

struct ChatInfo
{
    std::string chat_id;
    std::string role;
    std::string chatname;
    
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(ChatInfo, chat_id, role, chatname);

};

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
    std::string password_hash; // 同时用于 LOGIN

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
        username, password_hash, user_id,
        chat_id, chatname, message_id, message)
};

struct Message
{
    std::string message_id;
    std::string user_id;
    std::string username;
    std::string content;
    std::string timestamp;

    Message();
    Message(const ClientPacket &packet, const std::string &timestamp);
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Message, message_id, user_id, username, content, timestamp);
};

enum class ServerMessage
{
    REGISTER_RESPONSE,
    LOGIN_RESPONSE,
    CREATE_CHAT_RESPONSE,
    JOIN_CHAT_RESPONSE,

    RETURN_CHATS,
    RETURN_MESSAGES,

    NEW_MESSAGE
};

enum class ServerStatus
{
    SUCCESS,
    USER_NOT_FOUND,
    INVALID_PASSWORD,

    USER_EXISTS,

    CHAT_NOT_FOUND,

    CHAT_EXISTS,
};

struct ServerPacket
{
    ServerMessage request;

    ServerStatus status;
    std::string user_id;
    std::string username;
    std::vector<ChatInfo> chats;
    std::vector<Message> messages;

    NLOHMANN_JSON_SERIALIZE_ENUM(
        ServerMessage,
        {
            {ServerMessage::REGISTER_RESPONSE, "REGISTER_RESPONSE"},
            {ServerMessage::LOGIN_RESPONSE, "LOGIN_RESPONSE"},
            {ServerMessage::CREATE_CHAT_RESPONSE, "CREATE_CHAT_RESPONSE"},
            {ServerMessage::JOIN_CHAT_RESPONSE, "JOIN_CHAT_RESPONSE"},
            {ServerMessage::RETURN_CHATS, "RETURN_CHATS"},
            {ServerMessage::RETURN_MESSAGES, "RETURN_MESSAGES"},
            {ServerMessage::NEW_MESSAGE, "NEW_MESSAGE"},

        })
    NLOHMANN_JSON_SERIALIZE_ENUM(
        ServerStatus,
        {
            {ServerStatus::SUCCESS, "SUCCESS"},
            {ServerStatus::USER_NOT_FOUND, "USER_NOT_FOUND"},
            {ServerStatus::INVALID_PASSWORD, "INVALID_PASSWORD"},
            {ServerStatus::USER_EXISTS, "USER_EXISTS"},
            {ServerStatus::CHAT_NOT_FOUND, "CHAT_NOT_FOUND"},
            {ServerStatus::CHAT_EXISTS, "CHAT_EXISTS"},
        })
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(
        ServerPacket, request, status,
        user_id, username, chats, messages)
};
