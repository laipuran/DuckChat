#pragma once
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "protocal.hpp"

// 原有函数
void send_packet(int socket, const ClientPacket &client_packet);
void send_packet(int socket, const ServerPacket &server_packet);
ClientPacket recv_client_packet(int socket);
ServerPacket recv_server_packet(int socket);
bool check_online(int socket);

// 内部辅助函数
bool send_all(int socket, const void* data, size_t length);
bool recv_all(int socket, void* data, size_t length);