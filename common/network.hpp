#pragma once
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "protocal.hpp"

void send_packet(int socket, const ClientPacket &client_packet);
void send_packet(int socket, const ServerPacket &server_packet);
ClientPacket recv_client_packet(int socket);
ServerPacket recv_server_packet(int socket);
bool check_online(int socket);