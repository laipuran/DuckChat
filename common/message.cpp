#include "protocal.hpp"
#include "utils.hpp"

Message::Message() {}

Message::Message(const ClientPacket &packet, const std::string &timestamp)
    : message_id(packet.message_id), user_id(packet.user_id),
      username(packet.username), content(packet.message),
      timestamp(timestamp) {}