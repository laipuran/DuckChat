#include "window_manager.hpp"
#include "chat_manager.hpp"
#include <sstream>
#include <unistd.h>
#include <string>
#include <ncurses.h>

using namespace std;

void WindowManager::initiate()
{
    // 初始化 ncurses
    initscr();
    cbreak();
    // noecho();
    keypad(stdscr, TRUE);

    int scr_line, scr_col;
    getmaxyx(stdscr, scr_line, scr_col);

    // 创建窗口
    chat_list_window = newwin(scr_line - 1, scr_col / 3, 0, 0);
    message_window = newwin(scr_line - 1, scr_col - scr_col / 3, 0, scr_col / 3);
    input_window = newwin(1, scr_col, scr_line - 1, 0);

    // 绘制边框
    box(chat_list_window, '|', '+');
    box(message_window, '|', '+');
    box(input_window, '|', '+');

    // 刷新所有窗口
    touchwin(chat_list_window);
    touchwin(message_window);
    touchwin(input_window);

    wnoutrefresh(chat_list_window);
    wnoutrefresh(message_window);
    wnoutrefresh(input_window);
    doupdate();
}

void WindowManager::handle_input()
{
    while (true)
    {
        char ch = getch();
        switch (ch)
        {
        case 'q':
            printf("Triggering Exit");
            return;
            break;
        case 'm':
        {
            char message[256];
            scanf("%[^\n]", message);
            chat_manager->add_message(string(message));
            break;
        }
        case ':':
        {
            char command_line[256];
            scanf("%[^\n]", command_line);
            
            string cmd_str = string(command_line);
            istringstream iss(cmd_str);
            string command, parameter;
            iss >> command;
            iss >> parameter;
            
            if (command == "cc")
            {
                if (chat_manager) {
                    chat_manager->create_chat(parameter);
                }
            }
            else if (command == "jc")
            {
                if (chat_manager) {
                    chat_manager->join_chat(parameter);
                }
            }
            break;
        }
        default:
            break;
        }
    }

    endwin();
}

void WindowManager::refresh_windows()
{
    if (chat_list_window)
    {
        touchwin(chat_list_window);
        wnoutrefresh(chat_list_window);
    }
    if (message_window)
    {
        touchwin(message_window);
        wnoutrefresh(message_window);
    }
    if (input_window)
    {
        touchwin(input_window);
        wnoutrefresh(input_window);
    }
    doupdate();
}

void WindowManager::render_chat_history(const std::vector<Message> &messages)
{
    for (Message message : messages)
    {
        render_new_message(message);
    }
}

void WindowManager::render_new_message(const Message &message)
{
    string out_str = message.username + ":" + message.content;
    printf("%s", out_str.c_str());
}

void WindowManager::render_chats(const std::vector<ChatInfo> &chats)
{
    for (ChatInfo info : chats)
    {
        render_new_chat(info);
    }
}

void WindowManager::render_new_chat(const ChatInfo &chat_info)
{
    string out_str = chat_info.chatname;
    printf("%s", out_str.c_str());
}