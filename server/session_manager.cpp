#include <thread>
#include <memory>
#include <unistd.h>
#include "log_helper.hpp"
#include "session_manager.hpp"

using namespace std;

SessionManager::SessionManager() : database(Database("duckchat.db")) {};

void SessionManager::create_and_start_session(int socket)
{
    std::lock_guard<std::mutex> lock(sessions_mutex);
    unique_ptr<Session> session(new Session(socket, this));
    Session *session_ptr = session.get();

    sessions[socket] = move(session);
    thread new_session_thread(&Session::handle_session, session_ptr);
    new_session_thread.detach();
}

void SessionManager::close_session(int socket)
{
    lock_guard<mutex> lock(sessions_mutex);

    auto it = sessions.find(socket);
    if (it != sessions.end())
    {
        log(LogLevel::INFO, "Session closed for: " + it->second->user_id);
        sessions.erase(it);
        close(socket);
    }
}

Database *SessionManager::get_database()
{
    return &database;
}