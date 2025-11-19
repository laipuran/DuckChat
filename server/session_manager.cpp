#include <thread>
#include <memory>
#include "session_manager.hpp"

using namespace std;

SessionManager::SessionManager() : database(Database("duckchat.db")) {};

void SessionManager::create_and_start_session(int socket)
{
    std::lock_guard<std::mutex> lock(sessions_mutex);
    unique_ptr<Session> session(new Session(socket, this));
    Session *session_ptr = session.get();

    this->sessions.push_back(move(session));
    thread new_session_thread(&Session::handle_session, session_ptr);
    new_session_thread.detach();
}

void SessionManager::close_session(int socket)
{
    // TODO: 实现会话关闭逻辑
    (void)socket; // 避免未使用参数警告
}

Database* SessionManager::get_database(){
    return &database;
}