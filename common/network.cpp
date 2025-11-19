#include <iostream>
#include <sqlite3.h>
#include "network.hpp"
#include"../common/log_helper.hpp"
#include "../third_party/json.hpp"

using namespace std;
using nlohmann::json;

void send_packet(int socket, const ClientPacket &packet)
{
    json serialized = packet;
    string json_string = serialized.dump();

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
    json serialized = packet;
    string json_string = serialized.dump();

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
    recv(socket, &net_length, sizeof(uint32_t), 0);
    uint32_t host_length = ntohl(net_length);

    string json_string;
    json_string.resize(host_length);
    recv(socket, &json_string[0], host_length, 0);
    log(LogLevel::INFO, "Received client packet.");
    return json::parse(json_string).get<ClientPacket>();
}

ServerPacket recv_server_packet(int socket)
{
    uint32_t net_length;
    recv(socket, &net_length, sizeof(uint32_t), 0);
    uint32_t host_length = ntohl(net_length);

    string json_string;
    json_string.resize(host_length);
    recv(socket, &json_string[0], host_length, 0);
    log(LogLevel::INFO, "Received server packet.");
    return json::parse(json_string).get<ServerPacket>();
}