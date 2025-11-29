#include "chat_manager.hpp"
#include <string>
#include<iostream>
#include "../common/network.hpp"
#include "../common/utils.hpp"
#include "../common/log_helper.hpp"

using namespace std;

void Chat::add_message(const Message &message)
{
    messages.push_back(message);
}

void Chat::remove_message(const std::string &message_id)
{
    messages.erase(
        std::remove_if(messages.begin(), messages.end(),
                       [&message_id](const Message &msg)
                       {
                           return msg.message_id == message_id;
                       }),
        messages.end());
}

std::vector<Message> Chat::get_messages()
{
    return messages;
}

void ChatManager::initiate()
{
    log(LogLevel::INFO, "初始化聊天管理器，用户ID: " + current_user_id);
    
    ClientPacket chats_request;
    chats_request.request = ClientMessage::LIST_CHATS;
    chats_request.user_id = current_user_id;
    send_packet(server_sock, chats_request);

    ServerPacket chats_packet = recv_server_packet(server_sock);
    handle_chat_lists(chats_packet);
    
    log(LogLevel::INFO, "聊天管理器初始化完成");
}

void ChatManager::handle_chat_lists(const ServerPacket &packet)
{
    chat_list = packet.chats;
    window_manager->render_chats(packet.chats);
}

void ChatManager::handle_chat_history(const ServerPacket &packet)
{
    current_chat.messages = packet.messages;
    window_manager->render_chat_history(packet.messages);
}

void ChatManager::add_message(const std::string &message)
{
    log(LogLevel::INFO, "发送消息到聊天室 " + current_chat_id + ": " + message);
    
    ClientPacket packet;
    packet.request = ClientMessage::MESSAGE;
    packet.user_id = current_user_id;
    packet.chat_id = current_chat_id;
    packet.message = message;
    packet.username = current_username;
    packet.message_id = Utils::get_uuid();

    send_packet(server_sock, packet);

    // 添加消息到本地聊天历史并重新渲染整个聊天历史
    Message new_message(packet, Utils::get_iso_timestamp());
    current_chat.add_message(new_message);
    window_manager->render_chat_history(current_chat.get_messages());
}

void ChatManager::handle_new_message(const ServerPacket &packet)
{
    if (packet.status == ServerStatus::SUCCESS)
    {
        // 检查message_list是否为空，避免数组越界访问
        if (packet.messages.empty())
        {
            log(LogLevel::ERROR, "收到新消息但消息列表为空");
            return;
        }
        Message message = packet.messages[0];
        if (packet.chats.size() == 0)
            return;
        if (packet.chats[0].chat_id != current_chat_id) {
            log(LogLevel::INFO, "收到非当前聊天室的消息，忽略");
            return;
        }
        
        log(LogLevel::INFO, "收到新消息: " + message.username + ": " + message.content);
        current_chat.add_message(message);

        // 重新渲染整个聊天历史，而不是只渲染新消息
        window_manager->render_chat_history(current_chat.get_messages());
    } else {
        log(LogLevel::ERROR, "处理新消息失败，状态码: " + std::to_string(static_cast<int>(packet.status)));
    }
}

void ChatManager::handle_new_chat(const ServerPacket &packet)
{
    // 检查chats向量是否为空，避免数组越界访问
    if (packet.chats.empty()) {
        log(LogLevel::ERROR, "收到新聊天响应但聊天列表为空");
        return;
    }

    chat_list.push_back(packet.chats[0]);
    current_chat_id = packet.chats[0].chat_id;
    window_manager->render_chats(chat_list);

    log(LogLevel::INFO, "成功加入聊天室: " + current_chat_id);
    window_manager->show_status("加入"+current_chat_id+"成功！");

    ClientPacket request_packet;
    request_packet.request = ClientMessage::FETCH_MESSAGES;
    request_packet.chat_id = current_chat_id;

    send_packet(server_sock, request_packet);
    window_manager->render_chats(chat_list);
}

void ChatManager::create_chat(const std::string &chatname)
{
    log(LogLevel::INFO, "创建聊天室: " + chatname);
    
    ClientPacket packet;
    packet.request = ClientMessage::CREATE_CHAT;
    packet.user_id = current_user_id;
    packet.chatname = chatname;
    send_packet(server_sock, packet);
}

void ChatManager::leave_chat(const std::string &chat_id)
{
    std::string target_chat_id = chat_id.empty() ? current_chat_id : chat_id;
    log(LogLevel::INFO, "离开聊天室: " + target_chat_id);
    
    ClientPacket packet;
    packet.request = ClientMessage::LEAVE_CHAT;
    packet.chat_id = target_chat_id;
    packet.user_id = current_user_id;
    send_packet(server_sock, packet);
}

void ChatManager::recall_message(const std::string &message_id)
{
    log(LogLevel::INFO, "撤回消息: " + message_id);
    
    ClientPacket packet;
    packet.request = ClientMessage::RECALL;
    packet.user_id = current_user_id;
    packet.message_id = message_id;
    packet.chat_id = current_chat_id;
    send_packet(server_sock, packet);
}

void ChatManager::join_chat(const string &chat_id)
{
    log(LogLevel::INFO, "加入聊天室: " + chat_id);
    
    ClientPacket packet;
    packet.request = ClientMessage::JOIN_CHAT;
    packet.chat_id = chat_id;
    packet.user_id = current_user_id;
    send_packet(server_sock, packet);
}

std::vector<Message> ChatManager::get_current_chat_messages()
{
    return current_chat.get_messages();
}

std::vector<ChatInfo> ChatManager::get_chat_list()
{
    return chat_list;
}
