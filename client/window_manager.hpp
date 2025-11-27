#pragma once

#include <string>
#include <ncurses.h>
#include "../common/protocal.hpp"

class ChatManager;
class Chat;

enum class WindowMode
{
    CHAT_LIST,
    MESSAGE
};

enum class InputMode
{
    COMMAND,
    MESSAGE
};

class WindowManager
{
private:
    WINDOW *message_window;
    WINDOW *chat_list_window;
    WINDOW *input_window;

public:
    ChatManager *chat_manager;

    void initiate();
    void handle_input();
    void handle_simple_input();
    void send_message(const ClientPacket &packet);
    void refresh_windows();

    void render_chat_history(const std::vector<Message> &messages);
    void render_new_message(const Message &message);
    void render_chats(const std::vector<ChatInfo> &chats);
    void render_new_chat(const ChatInfo &chat_info);
};