#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sqlite3.h>
#include <signal.h>
#include "network.hpp"
#include "../common/log_helper.hpp"
#include "../third_party/json.hpp"

using namespace std;
using nlohmann::json;

void send_packet(int socket, const ClientPacket &packet)
{
    signal(SIGPIPE, SIG_IGN);
    json serialized = packet;
    string json_string = serialized.dump();
    log(LogLevel::INFO, "Sending: " + json_string);

    uint32_t length = htonl(json_string.length());
    try
    {
        send(socket, &length, sizeof(length), 0);
        send(socket, json_string.c_str(), json_string.length(), 0);
    }
    catch (const exception &e)
    {
        log(LogLevel::ERROR, e.what());
    }
}

void send_packet(int socket, const ServerPacket &packet)
{
    signal(SIGPIPE, SIG_IGN);
    json serialized = packet;
    string json_string = serialized.dump();
    log(LogLevel::INFO, "Sending: " + json_string);

    uint32_t length = htonl(json_string.length());
    try
    {
        send(socket, &length, sizeof(length), 0);
        send(socket, json_string.c_str(), json_string.length(), 0);
    }
    catch (const exception &e)
    {
        log(LogLevel::ERROR, e.what());
    }
}

ClientPacket recv_client_packet(int socket)
{
    uint32_t net_length;
    int result = recv(socket, &net_length, sizeof(uint32_t), 0);

    if (result == 0)
    {
        log(LogLevel::INFO, "Connection closed by client.");
        return ClientPacket();
    }
    if (result == -1)
    {
        log(LogLevel::ERROR, "Receive error: " + string(strerror(errno)));
        return ClientPacket();
    }

    uint32_t host_length = ntohl(net_length);

    const uint32_t MAX_PACKET_SIZE = 1024 * 1024;
    if (host_length > MAX_PACKET_SIZE)
    {
        log(LogLevel::ERROR, "Packet too large: " + to_string(host_length));
        return ClientPacket();
    }

    string json_string;
    json_string.resize(host_length);
    result = recv(socket, &json_string[0], host_length, 0);

    if (result == -1)
        return ClientPacket();
    log(LogLevel::INFO, "Received client packet." + json_string);

    ClientPacket packet;
    try
    {
        packet = json::parse(json_string).get<ClientPacket>();
    }
    catch (const exception &e)
    {
        log(LogLevel::ERROR, e.what());
        return ClientPacket();
    }
    return packet;
}

ServerPacket recv_server_packet(int socket)
{
    uint32_t net_length;
    int result = recv(socket, &net_length, sizeof(uint32_t), 0);

    if (result == 0)
    {
        log(LogLevel::INFO, "Connection closed by client.");
        return ServerPacket();
    }
    if (result == -1)
    {
        log(LogLevel::ERROR, "Receive error: " + string(strerror(errno)));
        return ServerPacket();
    }

    uint32_t host_length = ntohl(net_length);

    const uint32_t MAX_PACKET_SIZE = 1024 * 1024;
    if (host_length > MAX_PACKET_SIZE)
    {
        log(LogLevel::ERROR, "Packet too large: " + to_string(host_length));
        return ServerPacket();
    }

    string json_string;
    json_string.resize(host_length);
    result = recv(socket, &json_string[0], host_length, 0);

    if (result == -1)
        return ServerPacket();
    log(LogLevel::INFO, "Received server packet: " + json_string);

    ServerPacket packet;
    try
    {
        packet = json::parse(json_string).get<ServerPacket>();
    }
    catch (const exception &e)
    {
        log(LogLevel::ERROR, e.what());
        return ServerPacket();
    }
    return packet;
}

bool check_online(int socket)
{
    int error = 0;
    socklen_t len = sizeof(error);
    int result = getsockopt(socket, SOL_SOCKET, SO_ERROR, &error, &len);

    if (result != 0 || error != 0)
    {
        return false;
    }
    return true;
}