#include <arpa/inet.h>
#include <ncurses.h>
#include <netinet/in.h>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <signal.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <queue>
#include <sstream>
#include <thread>
#include "../common/log_helper.hpp"
#include "../common/network.hpp"
#include "../common/protocal.hpp"
#include "../third_party/json.hpp"
#include "chat_manager.hpp"
#include "window_manager.hpp"

using namespace std;
using nlohmann::json;

int server_fd;
WindowManager window_manager;
ChatManager chat_manager;

void handle_server_receive();
string sha256(const std::string& str);

int main() {
    log(LogLevel::INFO, "客户端启动");

    cout<<"请输入服务器地址(ip或域名)："<<endl;
    string server_addr;
    getline(cin, server_addr);
    cout<<"请输入端口："<<endl;
    int port;
    cin>>port;
    cin.ignore();

    // 使用getaddrinfo进行DNS解析
    struct addrinfo hints, *result;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;  // IPv4
    hints.ai_socktype = SOCK_STREAM;

    int status = getaddrinfo(server_addr.c_str(), to_string(port).c_str(), &hints, &result);
    if (status != 0) {
        cerr << "错误：无法解析服务器地址 " << server_addr << " : " << gai_strerror(status) << endl;
        return 1;
    }

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serv_addr = *(sockaddr_in*)result->ai_addr;
    freeaddrinfo(result);  // 释放内存

    while (true) {
        int result = connect(server_fd, (sockaddr*)&serv_addr, sizeof(serv_addr));
        if (result == 0)
            break;
        sleep(1);
    }
    log(LogLevel::INFO, "已连接到服务器");

    ServerPacket received_packet;

#pragma region
    while (true) {
        int op;
        log(LogLevel::INFO, "Register(0) or Login(1):");
        cout << "Register(0) or Login(1):" << endl;
        cin >> op;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        if (op == 0) {
            string user_id, username, password;
            log(LogLevel::INFO, "Input user id:");
            cout << "Input user id:" << endl;
            getline(cin, user_id);
            log(LogLevel::INFO, "Input username:");
            cout << "Input username:" << endl;
            getline(cin, username);
            log(LogLevel::INFO, "Input password:");
            cout << "Input password:" << endl;
            getline(cin, password);

            ClientPacket packet;
            packet.request = ClientMessage::REGISTER;
            packet.user_id = user_id;
            packet.username = username;
            packet.password_hash = sha256(password);

            send_packet(server_fd, packet);

            received_packet = recv_server_packet(server_fd);
            if (received_packet.status == ServerStatus::SUCCESS && received_packet.user_id != "") {
                log(LogLevel::INFO, "注册成功");
                cout << "Register success" << endl;
                break;
            }
        } else if (op == 1) {
            string user_id, password;
            log(LogLevel::INFO, "Input user id:");
            cout << "Input user id:" << endl;
            getline(cin, user_id);
            log(LogLevel::INFO, "Input password:");
            cout << "Input password:" << endl;
            getline(cin, password);

            ClientPacket packet;
            packet.request = ClientMessage::LOGIN;
            packet.user_id = user_id;
            packet.password_hash = sha256(password);

            send_packet(server_fd, packet);

            received_packet = recv_server_packet(server_fd);
            if (received_packet.status == ServerStatus::SUCCESS && received_packet.user_id != "") {
                log(LogLevel::INFO, "登录成功");
                cout << "Login success" << endl;
                break;
            }
        }
    }
#pragma endregion
#pragma region
    // #pragma region Skipping Login
    //     ClientPacket packet;
    //     packet.request = ClientMessage::LOGIN;
    //     packet.user_id = "duck";
    //     packet.password_hash = sha256("duck");

    //     send_packet(server_fd, packet);

    //     received_packet = recv_server_packet(server_fd);
    //     if (received_packet.status == ServerStatus::SUCCESS)
    //     {
    //         cout << "Login success" << endl;
    //     }
#pragma endregion

    thread client_thread_receive(handle_server_receive);
    client_thread_receive.detach();

    chat_manager =
        ChatManager(server_fd, &window_manager, received_packet.user_id, received_packet.username);
    chat_manager.initiate();
    window_manager.chat_manager = &chat_manager;
    window_manager.initiate();
    window_manager.handle_input();
}

string sha256(const std::string& str) {
    EVP_MD_CTX* mdctx = EVP_MD_CTX_new();
    const EVP_MD* md = EVP_sha256();
    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int hash_len;

    EVP_DigestInit_ex(mdctx, md, NULL);
    EVP_DigestUpdate(mdctx, str.c_str(), str.length());
    EVP_DigestFinal_ex(mdctx, hash, &hash_len);
    EVP_MD_CTX_free(mdctx);

    std::stringstream ss;
    for (unsigned int i = 0; i < hash_len; i++) {
        ss << hex << setw(2) << setfill('0') << (int)hash[i];
    }
    return ss.str();
}

void handle_server_receive() {
    sleep(1);
    while (true) {
        ServerPacket packet = recv_server_packet(server_fd);

        switch (packet.request) {
            case ServerMessage::NEW_MESSAGE:
                chat_manager.handle_new_message(packet);
                break;
            case ServerMessage::RETURN_CHATS:
                chat_manager.handle_chat_lists(packet);
                break;
            case ServerMessage::RETURN_MESSAGES:
                chat_manager.handle_chat_history(packet);
                break;
            case ServerMessage::JOIN_CHAT_RESPONSE:
                chat_manager.handle_new_chat(packet);
                break;
            case ServerMessage::CREATE_CHAT_RESPONSE:
                chat_manager.handle_new_chat(packet);
                break;
            default:
                break;
        }
    }
}