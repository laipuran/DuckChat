#include "window_manager.hpp"
#include <ncurses.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <locale>
#include <sstream>
#include <string>
#include "chat_manager.hpp"

using namespace std;

// 初始化静态变量
bool WindowManager::first_initiation = true;

void WindowManager::initiate() {
    // 设置locale以支持UTF-8
    setlocale(LC_ALL, "C.UTF-8");

    // 初始化 ncursesw (宽字符版本)
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    start_color(); // 启用颜色支持

    // 启用宽字符支持
    use_default_colors();

    // 初始化颜色对
    init_pair(1, COLOR_RED, COLOR_BLACK);   // 错误信息
    init_pair(2, COLOR_GREEN, COLOR_BLACK); // 成功信息
    init_pair(3, COLOR_CYAN, COLOR_BLACK);  // 状态信息
    init_pair(4, COLOR_WHITE, COLOR_BLUE);  // 活动窗口高亮

    int scr_line, scr_col;
    getmaxyx(stdscr, scr_line, scr_col);

    // 创建窗口 - 调整布局为类vim风格
    chat_list_window = newwin(scr_line - 2, scr_col / 4, 0, 0);
    message_window = newwin(scr_line - 2, scr_col - scr_col / 4, 0, scr_col / 4);
    status_window = newwin(1, scr_col, scr_line - 2, 0);
    input_window = newwin(1, scr_col, scr_line - 1, 0);

    // 绘制边框
    box(chat_list_window, '|', '-');
    box(message_window, '|', '-');

    // 状态栏和输入栏不需要边框

    // 在聊天列表窗口添加标题
    wattron(chat_list_window, A_BOLD);
    mvwprintw(chat_list_window, 0, 2, "聊天列表");
    wattroff(chat_list_window, A_BOLD);

    // 在消息窗口添加标题
    wattron(message_window, A_BOLD);
    mvwprintw(message_window, 0, 2, "消息");
    wattroff(message_window, A_BOLD);

    // 初始化状态栏
    wattron(status_window, COLOR_PAIR(3));
    mvwprintw(status_window, 0, 0, "欢迎使用DuckChat - 按'm'发送消息, ':'输入命令, 'q'退出");
    wattroff(status_window, COLOR_PAIR(3));

    // 初始化滚动位置
    message_scroll_pos = 0;
    chat_scroll_pos = 0;

    // 计算最大显示行数
    max_message_lines = scr_line - 4; // 减去边框和标题
    max_chat_lines = scr_line - 4;    // 减去边框和标题

    // 初始化窗口模式
    current_window_mode = WindowMode::MESSAGE;
    current_input_mode = InputMode::MESSAGE;

    // 刷新所有窗口
    touchwin(chat_list_window);
    touchwin(message_window);
    touchwin(status_window);
    touchwin(input_window);

    wnoutrefresh(chat_list_window);
    wnoutrefresh(message_window);
    wnoutrefresh(status_window);
    wnoutrefresh(input_window);
    doupdate();

    // 只在第一次初始化时显示字符画
    if (first_initiation) {
        show_splash_art();
        first_initiation = false;
    } else {
        // 第二次及以后的初始化，直接显示聊天列表
        if (chat_list_window)
            render_chats(chat_manager->get_chat_list());
    }
}

void WindowManager::handle_input() {
    while (true) {
        int ch = getch();
        switch (ch) {
            case 'q':
                show_status("正在退出...", false);
                endwin();
                return;
                break;
            case 'm': {
                current_input_mode = InputMode::MESSAGE;
                show_status("输入消息内容:", false);
                string message = get_input();
                if (!message.empty() && chat_manager) {
                    chat_manager->add_message(message);
                    show_status("消息已发送", false);
                }
                current_input_mode = InputMode::COMMAND;
                break;
            }
            case 'l': {
                show_status("确认退出？(y/n)");
                char flag = getch();
                if (flag == 'y' || flag == 'Y') {
                    chat_manager->leave_chat(
                        (chat_manager->get_chat_list())[selected_chat_index].chat_id);
                    show_status("离开聊天", false);
                }
                break;
            }
            case ':': {
                current_input_mode = InputMode::COMMAND;
                string command_line = get_input();

                if (!command_line.empty()) {
                    istringstream iss(command_line);
                    string command, parameter;
                    iss >> command;
                    iss >> parameter;

                    if (command == "cc") {
                        if (chat_manager && !parameter.empty()) {
                            chat_manager->create_chat(parameter);
                            show_status("创建聊天: " + parameter, false);
                        } else {
                            show_status("用法: :cc <聊天名称>", true);
                        }
                    } else if (command == "jc") {
                        if (chat_manager && !parameter.empty()) {
                            chat_manager->join_chat(parameter);
                            show_status("加入聊天: " + parameter, false);
                        } else {
                            show_status("用法: :jc <聊天ID>", true);
                        }
                    } else if (command == "lc") {
                        if (chat_manager) {
                            chat_manager->leave_chat(parameter);
                            show_status("离开聊天", false);
                        }
                    } else if (command == "rm") {
                        if (chat_manager) {
                            chat_manager->recall_message(parameter);
                            show_status("撤回消息", false);
                        }
                    } else if (command == "help") {
                        show_status(
                            "命令: :cc <名称>创建, :jc <ID>加入, :lc <ID>离开, m发送消息, q退出",
                            false);
                    } else {
                        show_status("未知命令: " + command, true);
                    }
                }
                break;
            }
            case '\t': // TAB键切换窗口
                switch_active_window();
                break;
            case KEY_UP:
                if (current_window_mode == WindowMode::MESSAGE) {
                    scroll_messages(-1);
                } else {
                    scroll_chats(-1);

                    auto chat_list = chat_manager->get_chat_list();
                    if (chat_list.size() == 0)
                        break;
                    selected_chat_index = (selected_chat_index - 1) % chat_list.size();
                    chat_manager->handle_switch_chat(selected_chat_index);
                }
                break;
            case KEY_DOWN:
                if (current_window_mode == WindowMode::MESSAGE) {
                    scroll_messages(1);
                } else {
                    scroll_chats(1);

                    auto chat_list = chat_manager->get_chat_list();
                    if (chat_list.size() == 0)
                        break;
                    selected_chat_index =
                        (selected_chat_index + 1) % chat_manager->get_chat_list().size();
                    chat_manager->handle_switch_chat(selected_chat_index);
                }
                break;
            case KEY_LEFT:
                if (current_window_mode == WindowMode::MESSAGE) {
                    current_window_mode = WindowMode::CHAT_LIST;
                    show_status("切换到聊天列表模式", false);
                    refresh_windows();
                }
                break;
            case KEY_RIGHT:
                if (current_window_mode == WindowMode::CHAT_LIST) {
                    current_window_mode = WindowMode::MESSAGE;
                    show_status("切换到消息模式", false);
                    refresh_windows();
                }
                break;
            case KEY_RESIZE:
                // 处理终端大小变化
                clear();
                refresh();
                initiate();
                show_status("终端大小已调整", false);
                if (chat_manager) {
                    std::vector<Message> current_messages =
                        chat_manager->get_current_chat_messages();
                    render_chat_history(current_messages);
                }
                if (chat_list_window)
                    render_chats(chat_manager->get_chat_list());
                break;
            default:
                // 显示按键提示
                if (ch != ERR) {
                    clear_status();
                }
                break;
        }
    }
}

void WindowManager::refresh_windows() {
    if (chat_list_window) {
        touchwin(chat_list_window);
        wnoutrefresh(chat_list_window);
    }
    if (message_window) {
        touchwin(message_window);
        wnoutrefresh(message_window);
    }
    if (status_window) {
        touchwin(status_window);
        wnoutrefresh(status_window);
    }
    if (input_window) {
        touchwin(input_window);
        wnoutrefresh(input_window);
    }
    doupdate();
}

// 处理单条消息，将其转换为多行显示
vector<string> process_message_lines(const Message& msg, int max_width) {
    std::vector<std::string> lines;

    // 格式化消息头（时间戳 + 用户名）
    std::string header = "";
    if (!msg.timestamp.empty() && msg.timestamp.length() >= 5) {
        header = "[" + msg.timestamp.substr(11, 5) + ":" + msg.message_id + "] ";
    }
    header += msg.username + ": ";

    // 计算内容可用的最大宽度
    int content_max_width = max_width - header.length() - 4; // 减去边框和内边距

    // 如果消息内容不超过一行，直接返回
    if ((int)msg.content.length() <= content_max_width) {
        lines.push_back(header + msg.content);
        return lines;
    }

    // 需要换行处理
    std::string remaining_content = msg.content;

    // 第一行（包含头部）
    lines.push_back(header + remaining_content.substr(0, content_max_width));
    remaining_content = remaining_content.substr(content_max_width);

    // 后续行（只包含内容，需要缩进）
    std::string indent(header.length(), ' ');

    while (!remaining_content.empty()) {
        int line_length = std::min(content_max_width, (int)remaining_content.length());

        // 确保不会在UTF-8字符中间截断
        while (line_length > 0 && (remaining_content[line_length - 1] & 0x80) != 0) {
            line_length--;
        }

        lines.push_back(indent + remaining_content.substr(0, line_length));
        remaining_content = remaining_content.substr(line_length);
    }

    return lines;
}

// 将所有消息转换为行向量
vector<string> split_messages(const std::vector<Message>& messages, int max_width) {
    std::vector<std::string> all_lines;

    for (const auto& msg : messages) {
        auto msg_lines = process_message_lines(msg, max_width);
        all_lines.insert(all_lines.end(), msg_lines.begin(), msg_lines.end());

        // 在消息之间添加空行（可选）
        if (&msg != &messages.back()) {
            all_lines.push_back("");
        }
    }

    return all_lines;
}

void WindowManager::render_chat_history(const std::vector<Message>& messages) {
    // 验证窗口指针
    if (!message_window) {
        return;
    }

    // 获取窗口尺寸并验证
    int max_y = getmaxy(message_window);
    int max_x = getmaxx(message_window);
    if (max_y < 4 || max_x < 8) {
        return; // 窗口太小，无法显示内容
    }

    // 使用常量替代魔法数字
    const int BORDER_PADDING = 4;
    const int TITLE_ROW = 0;
    const int CONTENT_START_ROW = 2;

    // 清空消息窗口内容区域（保留边框和标题）
    werase(message_window);
    box(message_window, '|', '-');

    // 使用高亮显示标题
    wattron(message_window, A_BOLD);
    mvwprintw(message_window, TITLE_ROW, 2, "消息");
    wattroff(message_window, A_BOLD);

    // 如果没有消息，显示提示信息
    if (messages.empty()) {
        wattron(message_window, COLOR_PAIR(3)); // 使用青色显示提示
        mvwprintw(message_window, CONTENT_START_ROW, 2, "(暂无消息)");
        wattroff(message_window, COLOR_PAIR(3));
        wrefresh(message_window);
        return;
    }

    // 将所有消息转换为行向量
    std::vector<std::string> all_lines = split_messages(messages, max_x - BORDER_PADDING);

    // 计算可见的行范围
    int start_idx = max(0, (int)all_lines.size() - max_message_lines - message_scroll_pos);
    int end_idx = min((int)all_lines.size(), start_idx + max_message_lines) - 2;

    // 显示行
    for (int i = start_idx; i < end_idx; i++) {
        // 检查是否是最新消息的最后一行
        bool is_latest_line = (i == (int)all_lines.size() - 1) && (message_scroll_pos == 0);

        if (is_latest_line) {
            wattron(message_window, COLOR_PAIR(2));
        }

        mvwprintw(message_window, i - start_idx + CONTENT_START_ROW, 2, "%s", all_lines[i].c_str());

        if (is_latest_line) {
            wattroff(message_window, COLOR_PAIR(2));
        }
    }

    // 显示滚动指示器
    if (message_scroll_pos > 0) {
        wattron(message_window, COLOR_PAIR(3)); // 使用青色显示滚动指示器
        mvwprintw(message_window, 1, max_x - 5, "▲");
        wattroff(message_window, COLOR_PAIR(3));
    }
    if (end_idx < (int)all_lines.size()) {
        wattron(message_window, COLOR_PAIR(3));
        mvwprintw(message_window, max_y - 2, max_x - 5, "▼");
        wattroff(message_window, COLOR_PAIR(3));
    }

    // 如果有新消息且滚动位置在底部，显示新消息指示器
    if (message_scroll_pos == 0 && !messages.empty()) {
        wattron(message_window, COLOR_PAIR(2) | A_BOLD); // 绿色加粗
        mvwprintw(message_window, max_y - 2, max_x - 10, "[新消息]");
        wattroff(message_window, COLOR_PAIR(2) | A_BOLD);
    }

    wrefresh(message_window);
}

void WindowManager::render_chats(const std::vector<ChatInfo>& chats) {
    if (!chat_list_window)
        return;

    // 清空聊天列表窗口内容区域（保留边框和标题）
    werase(chat_list_window);
    box(chat_list_window, '|', '-');
    mvwprintw(chat_list_window, 0, 2, "聊天列表");

    if (selected_chat_index > (int)chats.size()) {
        selected_chat_index = -1;
    }

    // 计算可见的聊天范围
    int start_idx = chat_scroll_pos;
    int end_idx = min((int)chats.size(), start_idx + max_chat_lines);

    // 显示聊天列表
    for (int i = start_idx; i < end_idx; i++) {
        const ChatInfo& chat = chats[i];
        string display_line = chat.chatname + " " + chat.chat_id;

        // 如果是当前选中的聊天，高亮显示
        if (i == start_idx && current_window_mode == WindowMode::CHAT_LIST) {
            wattron(chat_list_window, COLOR_PAIR(4) | A_BOLD);
        }

        // 限制显示长度
        int max_len = getmaxx(chat_list_window) - 4;
        if ((int)display_line.length() > max_len) {
            display_line = display_line.substr(0, max_len - 3) + "...";
        }

        mvwprintw(chat_list_window, i - start_idx + 2, 2, "%s", display_line.c_str());

        if (i == start_idx && current_window_mode == WindowMode::CHAT_LIST) {
            wattroff(chat_list_window, COLOR_PAIR(4) | A_BOLD);
        }
    }

    // 显示滚动指示器
    if (chat_scroll_pos > 0) {
        mvwprintw(chat_list_window, 1, getmaxx(chat_list_window) - 5, "▲");
    }
    if (end_idx < (int)chats.size()) {
        mvwprintw(chat_list_window,
                  getmaxy(chat_list_window) - 2,
                  getmaxx(chat_list_window) - 5,
                  "▼");
    }

    wrefresh(chat_list_window);
}

// 滚动消息历史
void WindowManager::scroll_messages(int direction) {
    if (direction > 0) {
        // 向下滚动
        message_scroll_pos = max(0, message_scroll_pos - 1);
    } else {
        // 向上滚动
        message_scroll_pos += 1;
    }

    // 重新渲染当前聊天的消息历史
    if (chat_manager) {
        // 获取当前聊天的消息并重新渲染
        std::vector<Message> current_messages = chat_manager->get_current_chat_messages();
        render_chat_history(current_messages);
    }
}

// 滚动聊天列表
void WindowManager::scroll_chats(int direction) {
    if (direction > 0) {
        // 向下滚动
        chat_scroll_pos = max(0, chat_scroll_pos - 1);
    } else {
        // 向上滚动
        chat_scroll_pos += 1;
    }

    // 重新渲染聊天列表
    if (chat_manager) {
        render_chats(chat_manager->get_chat_list());
    }
}

// 切换活动窗口
void WindowManager::switch_active_window() {
    if (current_window_mode == WindowMode::MESSAGE) {
        current_window_mode = WindowMode::CHAT_LIST;
        show_status("切换到聊天列表模式", false);
    } else {
        current_window_mode = WindowMode::MESSAGE;
        show_status("切换到消息模式", false);
    }

    refresh_windows();
}

// 显示状态信息
void WindowManager::show_status(const std::string& message, bool is_error) {
    if (!status_window)
        return;

    // 清空状态栏
    werase(status_window);

    // 设置颜色
    if (is_error) {
        wattron(status_window, COLOR_PAIR(1));
    } else {
        wattron(status_window, COLOR_PAIR(3));
    }

    // 显示消息
    mvwprintw(status_window, 0, 0, "%s", message.c_str());

    // 关闭颜色属性
    if (is_error) {
        wattroff(status_window, COLOR_PAIR(1));
    } else {
        wattroff(status_window, COLOR_PAIR(3));
    }

    wrefresh(status_window);
}

// 清空状态栏
void WindowManager::clear_status() {
    if (!status_window)
        return;

    werase(status_window);
    wattron(status_window, COLOR_PAIR(3));
    mvwprintw(status_window, 0, 0, "欢迎使用DuckChat - 按'm'发送消息, ':'输入命令, 'q'退出");
    wattroff(status_window, COLOR_PAIR(3));

    wrefresh(status_window);
}

// 改进的输入处理方法
std::string WindowManager::get_input() {
    if (!input_window)
        return "";

    char buffer[1024];

    // 清空输入窗口
    werase(input_window);
    box(input_window, '|', '-');

    // 显示提示符
    if (current_input_mode == InputMode::COMMAND) {
        wattron(input_window, COLOR_PAIR(3));
        mvwprintw(input_window, 0, 1, ":");
        wattroff(input_window, COLOR_PAIR(3));
        wmove(input_window, 0, 2);
    } else {
        wattron(input_window, COLOR_PAIR(3));
        mvwprintw(input_window, 0, 1, ">");
        wattroff(input_window, COLOR_PAIR(3));
        wmove(input_window, 0, 2);
    }

    // 启用回显并获取输入
    echo();
    wgetnstr(input_window, buffer, sizeof(buffer) - 1);
    noecho();

    return string(buffer);
}

void WindowManager::show_splash_art() {
    // 清屏并显示字符画
    clear();
    refresh();
    
    // 获取终端尺寸
    int scr_line, scr_col;
    getmaxyx(stdscr, scr_line, scr_col);
    
    // 根据窗口宽度决定显示内容
    if (scr_line < 4) {
        // 窗口太窄，只显示简单的DuckChat文字
        mvprintw(scr_line / 2, (scr_col - 8) / 2, "DuckChat");
        
        mvprintw(scr_line / 2 + 2, (scr_col - 18) / 2, "按任意键继续...");
    } else if (scr_col < 60 || scr_line < 10) {
        // 窗口较窄，只显示鸭子部分
        const char* duck_art[] = {
            "                 __",
            "               <(o )___",
            "                ( ._> /",
            "                 `---'"
        };
        
        int art_height = sizeof(duck_art) / sizeof(duck_art[0]);
        int start_line = (scr_line - art_height) / 2;
        
        for (int i = 0; i < art_height; i++) {
            int start_col = (scr_col - strlen(duck_art[2])) / 2;
            mvprintw(start_line + i, start_col, "%s", duck_art[i]);
        }
        
        mvprintw(start_line + art_height + 2, (scr_col - 18) / 2, "按任意键继续...");
    } else {
        // 窗口足够宽，显示完整字符画
        const char* full_art[] = {
            "                 __",
            "               <(o )___",
            "                ( ._> /",
            "                 `---'",
            "    ____             __   ________          __",
            "   / __ \\__  _______/ /__/ ____/ /_  ____ _/ /_",
            "  / / / / / / / ___/ //_/ /   / __ \\/ __ `/ __/",
            " / /_/ / /_/ / /__/ ,< / /___/ / / / /_/ / /_",
            "/_____/\\__,_/\\___/_/|_|\\____/_/ /_/\\__,_/\\__/"
        };
        
        int art_height = sizeof(full_art) / sizeof(full_art[0]);
        int start_line = (scr_line - art_height) / 2;
        
        for (int i = 0; i < art_height; i++) {
            int start_col = (scr_col - strlen(full_art[8])) / 2;
            mvprintw(start_line + i, start_col, "%s", full_art[i]);
        }
        
        mvprintw(start_line + art_height + 2, (scr_col - 30) / 2, "欢迎使用 DuckChat! 按任意键继续...");
    }
    
    refresh();
    
    // 等待用户按任意键
    noecho();
    getch();
    echo();
    
    // 重新初始化界面
    clear();
    refresh();
    
    // 重新创建窗口
    chat_list_window = newwin(scr_line - 2, scr_col / 4, 0, 0);
    message_window = newwin(scr_line - 2, scr_col - scr_col / 4, 0, scr_col / 4);
    status_window = newwin(1, scr_col, scr_line - 2, 0);
    input_window = newwin(1, scr_col, scr_line - 1, 0);
    
    // 绘制边框
    box(chat_list_window, '|', '-');
    box(message_window, '|', '-');
    
    // 在聊天列表窗口添加标题
    wattron(chat_list_window, A_BOLD);
    mvwprintw(chat_list_window, 0, 2, "聊天列表");
    wattroff(chat_list_window, A_BOLD);
    
    // 在消息窗口添加标题
    wattron(message_window, A_BOLD);
    mvwprintw(message_window, 0, 2, "消息");
    wattroff(message_window, A_BOLD);
    
    // 初始化状态栏
    wattron(status_window, COLOR_PAIR(3));
    mvwprintw(status_window, 0, 0, "欢迎使用DuckChat - 按'm'发送消息, ':'输入命令, 'q'退出");
    wattroff(status_window, COLOR_PAIR(3));
    
    // 刷新所有窗口
    touchwin(chat_list_window);
    touchwin(message_window);
    touchwin(status_window);
    touchwin(input_window);
    
    wnoutrefresh(chat_list_window);
    wnoutrefresh(message_window);
    wnoutrefresh(status_window);
    wnoutrefresh(input_window);
    doupdate();
    
    // 重新渲染聊天列表
    if (chat_list_window && chat_manager) {
        render_chats(chat_manager->get_chat_list());
    }
}