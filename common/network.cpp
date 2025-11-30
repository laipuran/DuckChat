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

// 辅助函数：确保发送所有数据
bool send_all(int socket, const void* data, size_t length) {
    size_t total_sent = 0;
    const char* ptr = static_cast<const char*>(data);
    
    while (total_sent < length) {
        ssize_t sent = send(socket, ptr + total_sent, length - total_sent, 0);
        if (sent <= 0) {
            if (sent == -1 && errno == EINTR) continue;
            log(LogLevel::ERROR, "Send error: " + string(strerror(errno)));
            return false;
        }
        total_sent += sent;
    }
    return true;
}

// 辅助函数：确保接收所有数据
bool recv_all(int socket, void* data, size_t length) {
    size_t total_received = 0;
    char* ptr = static_cast<char*>(data);
    
    while (total_received < length) {
        ssize_t received = recv(socket, ptr + total_received, length - total_received, 0);
        if (received <= 0) {
            if (received == -1 && errno == EINTR) continue;
            log(LogLevel::ERROR, "Receive error: " + string(strerror(errno)));
            return false;
        }
        total_received += received;
    }
    return true;
}

void send_packet(int socket, const ClientPacket &packet)
{
    signal(SIGPIPE, SIG_IGN);
    json serialized = packet;
    string json_string = serialized.dump();
    log(LogLevel::INFO, "Sending: " + json_string);

    uint32_t length = htonl(json_string.length());
    try
    {
        if (!send_all(socket, &length, sizeof(length))) {
            log(LogLevel::ERROR, "Failed to send length header");
            return;
        }
        if (!send_all(socket, json_string.c_str(), json_string.length())) {
            log(LogLevel::ERROR, "Failed to send packet data");
            return;
        }
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
        if (!send_all(socket, &length, sizeof(length))) {
            log(LogLevel::ERROR, "Failed to send length header");
            return;
        }
        if (!send_all(socket, json_string.c_str(), json_string.length())) {
            log(LogLevel::ERROR, "Failed to send packet data");
            return;
        }
    }
    catch (const exception &e)
    {
        log(LogLevel::ERROR, e.what());
    }
}

ClientPacket recv_client_packet(int socket)
{
    uint32_t net_length;
    if (!recv_all(socket, &net_length, sizeof(uint32_t))) {
        log(LogLevel::ERROR, "Failed to receive length header");
        return ClientPacket();
    }

    uint32_t host_length = ntohl(net_length);

    if (host_length > MAX_PACKET_SIZE)
    {
        log(LogLevel::ERROR, "Packet too large: " + to_string(host_length) + " bytes, max allowed: " + to_string(MAX_PACKET_SIZE));
        return ClientPacket();
    }

    string json_string;
    json_string.resize(host_length);
    
    if (!recv_all(socket, &json_string[0], host_length)) {
        log(LogLevel::ERROR, "Failed to receive packet data");
        return ClientPacket();
    }
    
    log(LogLevel::INFO, "Received client packet: " + json_string);

    ClientPacket packet;
    try
    {
        packet = json::parse(json_string).get<ClientPacket>();
    }
    catch (const exception &e)
    {
        log(LogLevel::ERROR, "JSON parse error: " + string(e.what()));
        return ClientPacket();
    }
    return packet;
}

ServerPacket recv_server_packet(int socket)
{
    uint32_t net_length;
    if (!recv_all(socket, &net_length, sizeof(uint32_t))) {
        log(LogLevel::ERROR, "Failed to receive length header");
        return ServerPacket();
    }

    uint32_t host_length = ntohl(net_length);

    if (host_length > MAX_PACKET_SIZE)
    {
        log(LogLevel::ERROR, "Packet too large: " + to_string(host_length) + " bytes, max allowed: " + to_string(MAX_PACKET_SIZE));
        return ServerPacket();
    }

    string json_string;
    json_string.resize(host_length);
    
    if (!recv_all(socket, &json_string[0], host_length)) {
        log(LogLevel::ERROR, "Failed to receive packet data");
        return ServerPacket();
    }
    
    log(LogLevel::INFO, "Received server packet: " + json_string);

    ServerPacket packet;
    try
    {
        packet = json::parse(json_string).get<ServerPacket>();
    }
    catch (const exception &e)
    {
        log(LogLevel::ERROR, "JSON parse error: " + string(e.what()));
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