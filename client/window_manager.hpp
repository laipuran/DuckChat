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
    WINDOW *status_window;
    
    // 滚动相关变量
    int message_scroll_pos;
    int chat_scroll_pos;
    int max_message_lines;
    int max_chat_lines;
    
    // 窗口模式
    WindowMode current_window_mode;
    InputMode current_input_mode;

public:
    ChatManager *chat_manager;

    void initiate();
    void handle_input();
    void handle_simple_input();
    void send_message(const ClientPacket &packet);
    void refresh_windows();
    
    // 滚动相关方法
    void scroll_messages(int direction);
    void scroll_chats(int direction);
    void switch_active_window();
    
    // 状态栏方法
    void show_status(const std::string& message, bool is_error = false);
    void clear_status();

    void render_chat_history(const std::vector<Message> &messages);
    void render_new_message(const Message &message);
    void render_chats(const std::vector<ChatInfo> &chats);
    void render_new_chat(const ChatInfo &chat_info);
    
    // 输入处理改进
    std::string get_input();
};