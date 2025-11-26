#pragma once
#include <cstring>
#include <sqlite3.h>
#include "../common/protocal.hpp"
#include"../common/protocal.hpp"

class Database
{
private:
    sqlite3 *db;

public:
    Database(const std::string &db_path);
    ~Database();

    bool exist_user(const std::string &user_id);
    bool add_user(
        const std::string &user_id,
        const std::string &username,
        const std::string &password_hash);

    std::string get_username(const std::string &user_id);
    std::string get_password_hash(const std::string &user_id);

    bool add_message(
        const std::string &user_id,
        const std::string &chat_id,
        const std::string &message_id,
        const std::string &message);

    bool delete_message(
        const std::string &message_id);

    bool chat_exist(const std::string &chat_id);

    std::string get_chatname(const std::string chat_id);

    std::vector<ChatInfo> list_user_chats(
        const std::string &user_id);

    std::vector<Message> fetch_chat_messages(
        const std::string &chat_id);

    bool add_chat(
        const std::string &user_id,
        const std::string &chat_id,
        const std::string &chatname);

    std::vector<std::string> get_chat_members(const std::string &chat_id);

    bool add_chat_member(
        const std::string &user_id,
        const std::string &chat_id,
        const std::string &role);

    bool leave_chat(
        const std::string &user_id,
        const std::string &chat_id);
};