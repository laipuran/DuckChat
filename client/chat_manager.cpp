#include "chat_manager.hpp"
#include <string>
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
    current_chat.messages = packet.message_list;
    window_manager->render_chat_history(packet.message_list);
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
    window_manager->render_new_message(Message(packet, Utils::get_iso_timestamp()));
}

void ChatManager::handle_new_message(const ServerPacket &packet)
{
    if (packet.status == ServerStatus::SUCCESS)
    {
        current_chat.add_message(packet.message_list[0]);
        current_chat_id = packet.chat_id;
        window_manager->render_new_message(packet.message_list[0]);
    }
}

void ChatManager::handle_new_chat(const ServerPacket &packet)
{
    chat_list.push_back(packet.chats[0]);
    window_manager->render_new_chat(packet.chats[0]);
}

void ChatManager::create_chat(const std::string &chatname)
{
    ClientPacket packet;
    packet.request = ClientMessage::CREATE_CHAT;
    packet.user_id = current_user_id;
    packet.chatname = chatname;
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
