#include "database.hpp"
#include <filesystem>
#include <iostream>
#include "../common/log_helper.hpp"

using namespace std;

Database::Database(const string &db_path)
{
    int status = sqlite3_open(db_path.data(), &db);
    if (status != SQLITE_OK)
    {
        log(LogLevel::ERROR, sqlite3_errmsg(db));
        filesystem::remove(db_path);

        sqlite3_open(db_path.data(), &db);
    }

    const char *create_tables_sql[] = {
        "CREATE TABLE IF NOT EXISTS users ("
        "user_id TEXT PRIMARY KEY,"
        "username TEXT NOT NULL,"
        "password_hash TEXT NOT NULL);",

        "CREATE TABLE IF NOT EXISTS chats ("
        "chat_id TEXT PRIMARY KEY,"
        "chatname TEXT NOT NULL,"
        "creator_id TEXT NOT NULL,"
        "FOREIGN KEY (creator_id) REFERENCES users(user_id));",

        "CREATE TABLE IF NOT EXISTS chat_members ("
        "chat_id TEXT,"
        "user_id TEXT NOT NULL,"
        "role TEXT DEFAULT 'member',"
        "FOREIGN KEY (chat_id) REFERENCES chats(chat_id),"
        "FOREIGN KEY (user_id) REFERENCES users(user_id),"
        "UNIQUE(chat_id, user_id));",

        "CREATE TABLE IF NOT EXISTS messages ("
        "message_id TEXT PRIMARY KEY,"
        "chat_id TEXT NOT NULL,"
        "sender_id TEXT NOT NULL,"
        "content TEXT NOT NULL,"
        "sent_at DATETIME DEFAULT CURRENT_TIMESTAMP,"
        "FOREIGN KEY (chat_id) REFERENCES chats(chat_id),"
        "FOREIGN KEY (sender_id) REFERENCES users(user_id));"};

    for (const char *sql : create_tables_sql)
    {
        sqlite3_exec(db, sql, nullptr, nullptr, nullptr);
    }
}

Database::~Database()
{
    if (db)
    {
        sqlite3_close(db);
    }
}

bool Database::exist_user(const string &user_id)
{
    sqlite3_stmt *statement;
    sqlite3_prepare_v2(db, "SELECT COUNT(*) FROM users WHERE user_id = ?",
                       -1, &statement, nullptr);

    sqlite3_bind_text(statement, 1, user_id.c_str(), -1, SQLITE_TRANSIENT);

    int count = 0;
    if (sqlite3_step(statement) == SQLITE_ROW)
    {
        count = sqlite3_column_int(statement, 0);
    }

    sqlite3_finalize(statement);
    return count;
}

bool Database::add_user(
    const string &user_id,
    const string &username,
    const string &password_hash)
{
    sqlite3_stmt *statement;
    const char *sql = "INSERT INTO users(user_id, username, password_hash) VALUES(?, ?, ?)";
    sqlite3_prepare_v2(db, sql, -1, &statement, nullptr);

    sqlite3_bind_text(statement, 1, user_id.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 2, username.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 3, password_hash.c_str(), -1, SQLITE_TRANSIENT);

    int flag = sqlite3_step(statement);
    sqlite3_finalize(statement);

    return flag == SQLITE_DONE;
}

string Database::get_username(const string &user_id)
{
    sqlite3_stmt *statement;
    sqlite3_prepare_v2(db, "SELECT username FROM users WHERE user_id = ?",
                       -1, &statement, nullptr);

    sqlite3_bind_text(statement, 1, user_id.c_str(), -1, SQLITE_TRANSIENT);

    string result;
    if (sqlite3_step(statement) == SQLITE_ROW)
    {
        const u_char *username = sqlite3_column_text(statement, 0);
        if (username != nullptr)
            result = reinterpret_cast<const char *>(username);
    }

    sqlite3_finalize(statement);
    return result;
}

string Database::get_password_hash(const string &user_id)
{
    sqlite3_stmt *statement;
    sqlite3_prepare_v2(db, "SELECT password_hash FROM users WHERE user_id = ?",
                       -1, &statement, nullptr);

    sqlite3_bind_text(statement, 1, user_id.c_str(), -1, SQLITE_TRANSIENT);

    string result;
    if (sqlite3_step(statement) == SQLITE_ROW)
    {
        const u_char *password = sqlite3_column_text(statement, 0);
        if (password != nullptr)
            result = reinterpret_cast<const char *>(password);
    }

    sqlite3_finalize(statement);
    return result;
}

bool Database::add_message(
    const string &user_id,
    const string &chat_id,
    const string &message_id,
    const string &message)
{
    sqlite3_stmt *statement;
    const char *sql = "INSERT INTO messages(message_id, chat_id, sender_id, content) VALUES(?, ?, ?, ?)";
    sqlite3_prepare_v2(db, sql, -1, &statement, nullptr);

    sqlite3_bind_text(statement, 1, message_id.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 2, chat_id.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 3, user_id.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 4, message.c_str(), -1, SQLITE_TRANSIENT);

    int flag = sqlite3_step(statement);
    sqlite3_finalize(statement);

    return flag == SQLITE_DONE;
}

bool Database::delete_message(const string &message_id)
{
    sqlite3_stmt *statement;
    const char *sql = "DELETE FROM messages WHERE message_id = ?";
    sqlite3_prepare_v2(db, sql, -1, &statement, nullptr);

    sqlite3_bind_text(statement, 1, message_id.c_str(), -1, SQLITE_TRANSIENT);

    int flag = sqlite3_step(statement);
    sqlite3_finalize(statement);

    return flag == SQLITE_DONE;
}

bool Database::chat_exist(const string &chat_id)
{
    sqlite3_stmt *statement;
    sqlite3_prepare_v2(db, "SELECT COUNT(*) FROM chats WHERE chat_id = ?",
                       -1, &statement, nullptr);

    sqlite3_bind_text(statement, 1, chat_id.c_str(), -1, SQLITE_TRANSIENT);

    int count = 0;
    if (sqlite3_step(statement) == SQLITE_ROW)
    {
        count = sqlite3_column_int(statement, 0);
    }

    sqlite3_finalize(statement);
    return count;
}

std::string Database::get_chatname(const std::string chat_id)
{
    sqlite3_stmt *statement;
    sqlite3_prepare_v2(db, "SELECT chatname FROM chats WHERE chat_id = ?",
                       -1, &statement, nullptr);

    sqlite3_bind_text(statement, 1, chat_id.c_str(), -1, SQLITE_TRANSIENT);

    string result;
    if (sqlite3_step(statement) == SQLITE_ROW)
    {
        const u_char *chatname = sqlite3_column_text(statement, 0);
        if (chatname != nullptr)
            result = reinterpret_cast<const char *>(chatname);
    }

    sqlite3_finalize(statement);
    return result;
}

vector<ChatInfo> Database::list_user_chats(const string &user_id)
{
    sqlite3_stmt *statement;
    const char *sql =
        {"SELECT cm.chat_id, cm.role, c.chatname"
         "FROM chat_members cm"
         "JOIN chats c ON cm.chat_id = c.chat_id"
         "WHERE cm.user_id = ? "};
    sqlite3_prepare_v2(db, sql, -1, &statement, nullptr);
    sqlite3_bind_text(statement, 1, user_id.c_str(), -1, SQLITE_TRANSIENT);

    vector<ChatInfo> chats;
    while (sqlite3_step(statement) == SQLITE_ROW)
    {
        ChatInfo chat_info;
        chat_info.chat_id = reinterpret_cast<const char *>(sqlite3_column_text(statement, 0));
        chat_info.role = reinterpret_cast<const char *>(sqlite3_column_text(statement, 1));
        chat_info.chatname = reinterpret_cast<const char *>(sqlite3_column_text(statement, 2));

        chats.push_back(chat_info);
    }
    return chats;
}

vector<Message> Database::fetch_chat_messages(const string &chat_id)
{
    sqlite3_stmt *statement;
    const char *sql =
        {"SELECT m.message_id, m.sender_id,"
         "m.content, m.sent_at, u.username"
         "FROM messages m"
         "JOIN users u ON m.sender_id = u.user_id"
         "WHERE m.chat_id = ? "
         "ORDER BY m.sent_at ASC;"};

    sqlite3_prepare_v2(db, sql, -1, &statement, nullptr);
    sqlite3_bind_text(statement, 1, chat_id.c_str(), -1, SQLITE_TRANSIENT);

    vector<Message> messages;
    while (sqlite3_step(statement) == SQLITE_ROW)
    {
        Message message;
        message.message_id = reinterpret_cast<const char *>(sqlite3_column_text(statement, 0));
        message.user_id = reinterpret_cast<const char *>(sqlite3_column_text(statement, 1));
        message.content = reinterpret_cast<const char *>(sqlite3_column_text(statement, 2));
        message.timestamp = reinterpret_cast<const char *>(sqlite3_column_text(statement, 3));
        message.username = reinterpret_cast<const char *>(sqlite3_column_text(statement, 4));

        messages.push_back(message);
    }
    return messages;
}

bool Database::add_chat(
    const std::string &user_id,
    const std::string &chat_id,
    const std::string &chatname)
{
    sqlite3_stmt *statement;
    const char *sql = "INSERT INTO chats(chat_id, chatname, creator_id) VALUES(?, ?, ?)";
    sqlite3_prepare_v2(db, sql, -1, &statement, nullptr);

    sqlite3_bind_text(statement, 1, chat_id.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 2, chatname.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 3, user_id.c_str(), -1, SQLITE_TRANSIENT);

    int flag = sqlite3_step(statement);
    sqlite3_finalize(statement);

    return flag == SQLITE_DONE;
}

std::vector<std::string> Database::get_chat_members(const std::string &chat_id)
{
    sqlite3_stmt *statement;
    sqlite3_prepare_v2(db, "SELECT user_id FROM chat_members WHERE chat_id = ?",
                       -1, &statement, nullptr);

    sqlite3_bind_text(statement, 1, chat_id.c_str(), -1, SQLITE_TRANSIENT);

    vector<string> result;
    while (sqlite3_step(statement) == SQLITE_ROW)  // 修复：使用while循环读取所有成员
    {
        const u_char *user_id = sqlite3_column_text(statement, 0);
        if (user_id != nullptr)
            result.push_back(reinterpret_cast<const char *>(user_id));
    }

    sqlite3_finalize(statement);
    return result;
}

bool Database::add_chat_member(
    const std::string &user_id,
    const std::string &chat_id,
    const string &role)
{
    sqlite3_stmt *statement;
    const char *sql = "INSERT INTO chat_members(chat_id, user_id, role) VALUES(?, ?, ?)";
    sqlite3_prepare_v2(db, sql, -1, &statement, nullptr);

    sqlite3_bind_text(statement, 1, chat_id.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 2, user_id.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 3, role.c_str(), -1, SQLITE_TRANSIENT);

    int flag = sqlite3_step(statement);
    sqlite3_finalize(statement);

    return flag == SQLITE_DONE;
}

bool Database::leave_chat(
    const std::string &user_id,
    const std::string &chat_id)
{
    sqlite3_stmt *statement;
    const char *sql = "DELETE FROM chat_members WHERE user_id = ? AND chat_id = ?";
    sqlite3_prepare_v2(db, sql, -1, &statement, nullptr);

    sqlite3_bind_text(statement, 1, user_id.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 2, chat_id.c_str(), -1, SQLITE_TRANSIENT);

    int flag = sqlite3_step(statement);
    sqlite3_finalize(statement);

    return flag == SQLITE_DONE;
}
