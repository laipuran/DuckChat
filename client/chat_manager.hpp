#pragma once

#include <string>
#include <vector>
#include <tuple>
#include "../common/protocal.hpp"
#include "window_manager.hpp"

class Chat
{
public:
    std::string chat_id;
    std::string chatname;
    std::vector<std::string> members;
    std::vector<Message> messages;

    void add_message(const Message &message);
    void remove_message(const std::string &message_id);
    std::vector<Message> get_messages();
};

class ChatManager
{
private:
    int server_sock;
    Chat current_chat;
    std::vector<ChatInfo> chat_list;
    std::string current_chat_id;
    WindowManager *window_manager;
    std::string current_user_id;
    std::string current_username;

public:
    ChatManager(int server_sock, WindowManager *window_manager, std::string user_id, std::string username)
        : server_sock(server_sock), window_manager(window_manager), current_user_id(user_id), current_username(username) {};
    ChatManager() {};
    void initiate();

    void handle_chat_lists(const ServerPacket &packet);
    void handle_chat_history(const ServerPacket &packet);
    void handle_new_message(const ServerPacket &packet);
    void handle_new_chat(const ServerPacket &packet);
    void handle_recall_message(const ServerPacket &packet);

    void create_chat(const std::string &chatname);
    void join_chat(const std::string &chat_id);
    void leave_chat(const std::string &chat_id);

    void add_message(const std::string &message);
    void recall_message(const std::string &message_id);
    
    // 访问器方法
    std::vector<Message> get_current_chat_messages();
    std::vector<ChatInfo> get_chat_list();
};