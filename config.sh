#!/bin/bash
# 项目构建配置文件
# 统一管理编译选项和第三方库依赖

# 编译器设置
CXX="g++"
CXXFLAGS="-g3 -O0 -std=c++17 -Wall -Wextra"

# 包含路径
INCLUDES="-I$(dirname "$0")/common -I$(dirname "$0")/third_party -I$(dirname "$0")/server"

# 第三方库配置
OPENSSL_LIBS="-lssl -lcrypto"
SQLITE_LIBS="-lsqlite3"
NCURSES_LIBS="-lncurses"

# 组合库配置
THIRD_PARTY_LIBS="$OPENSSL_LIBS $SQLITE_LIBS $NCURSES_LIBS"

# 输出文件路径
SERVER_OUTPUT="$(dirname "$0")/server/server"
CLIENT_OUTPUT="$(dirname "$0")/client/client"