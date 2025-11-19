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
        std::filesystem::remove(db_path);

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
        int result = sqlite3_exec(db, sql, nullptr, nullptr, nullptr);
        // if (result != SQLITE_DONE)
        //     log(LogLevel::ERROR, "Initiating database failed!");
    }
}

bool Database::exist_user(const std::string &user_id)
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
    const std::string &user_id,
    const std::string &username,
    const std::string &password_hash)
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

string Database::get_username(const std::string &user_id)
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

Database::~Database()
{
    if (db)
    {
        sqlite3_close(db);
    }
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