#include <iostream>
#include <mutex>
#include <vector>
#include <thread>
#include <cstring>
#include<signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "../third_party/json.hpp"
#include "../common/protocal.hpp"
#include "../common/network.hpp"
#include "session.hpp"
#include "../common/log_helper.hpp"
#include "session_manager.hpp"

using nlohmann::json;
using namespace std;

SessionManager session_manager;
int main()
{
    signal(SIGPIPE, SIG_IGN);
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(5001);

    bind(server_fd, (sockaddr *)&address, sizeof(address));

    listen(server_fd, 10);
    int addrlen = sizeof(address);

    while (true)
    {
        int client_sock = accept(server_fd, (sockaddr *)&address, (socklen_t *)&addrlen);

        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &address.sin_addr, client_ip, INET_ADDRSTRLEN);
        int client_port = ntohs(address.sin_port);
        log(LogLevel::INFO, "Client Address: " + string(client_ip) +":"+ to_string(client_port));

        session_manager.create_and_start_session(client_sock);
    }

    return 0;
}