#include "chat_manager.hpp"
#include <string>
#include<iostream>
#include "../common/network.hpp"
#include "../common/utils.hpp"

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
    ClientPacket chats_request;
    chats_request.request = ClientMessage::LIST_CHATS;
    chats_request.user_id = current_user_id;
    send_packet(server_sock, chats_request);

    ServerPacket chats_packet = recv_server_packet(server_sock);
    handle_chat_lists(chats_packet);
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
            return;
        }
        Message message = packet.messages[0];
        if (packet.chats.size() == 0)
            return;
        if (packet.chats[0].chat_id != current_chat_id)
            return;
        current_chat.add_message(message);
        current_chat_id = packet.chats[0].chat_id;

        // 重新渲染整个聊天历史，而不是只渲染新消息
        window_manager->render_chat_history(current_chat.get_messages());
    }
}

void ChatManager::handle_new_chat(const ServerPacket &packet)
{
    // 检查chats向量是否为空，避免数组越界访问
    if (packet.chats.empty())
        return;

    chat_list.push_back(packet.chats[0]);
    current_chat_id = packet.chats[0].chat_id;
    window_manager->render_new_chat(packet.chats[0]);

    window_manager->show_status("加入"+current_chat_id+"成功！");

    ClientPacket request_packet;
    request_packet.request = ClientMessage::FETCH_MESSAGES;
    request_packet.chat_id = current_chat_id;

    send_packet(server_sock, request_packet);
    window_manager->render_chats(chat_list);
}

void ChatManager::create_chat(const std::string &chatname)
{
    ClientPacket packet;
    packet.request = ClientMessage::CREATE_CHAT;
    packet.user_id = current_user_id;
    packet.chatname = chatname;
    send_packet(server_sock, packet);
}

void ChatManager::leave_chat(const std::string &chat_id)
{
    ClientPacket packet;
    packet.request = ClientMessage::LEAVE_CHAT;
    packet.chat_id = chat_id.empty() ? current_chat_id : chat_id;
    packet.user_id = current_user_id;
    send_packet(server_sock, packet);
}

void ChatManager::recall_message(const std::string &message_id)
{
    ClientPacket packet;
    packet.request = ClientMessage::RECALL;
    packet.user_id = current_user_id;
    packet.message_id = message_id;
    packet.chat_id = current_chat_id;
    send_packet(server_sock, packet);
}

void ChatManager::join_chat(const string &chat_id)
{
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
