#!/bin/bash

# 自动化编译脚本
# 使用方法: ./build.sh [server|client|all]

set -e

# 项目根目录
PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# 编译选项
CXX="g++"
CXXFLAGS="-g3 -O0 -std=c++11 -Wall -Wextra"
INCLUDES="-I${PROJECT_ROOT}/common -I${PROJECT_ROOT}/third_party -I${PROJECT_ROOT}/server"
LIBS="-pthread"

# 输出目录
SERVER_OUTPUT="${PROJECT_ROOT}/server/server"
CLIENT_OUTPUT="${PROJECT_ROOT}/client/client"

# 查找所有源文件
find_source_files() {
    local search_dir="$1"
    find "$search_dir" -name "*.cpp" -not -path "*/.*" | sort
}

# 编译服务器
build_server() {
    echo "正在编译服务器..."
    
    # 查找所有服务器相关源文件
    local server_sources=""
    server_sources+="$(find_source_files "${PROJECT_ROOT}/server") "
    server_sources+="$(find_source_files "${PROJECT_ROOT}/common")"
    
    echo "找到的服务器源文件:"
    echo $server_sources | tr ' ' '\n' | sed 's/^/  /'
    
    # 编译
    $CXX $CXXFLAGS $INCLUDES -o "$SERVER_OUTPUT" $server_sources $LIBS
    
    echo "服务器编译完成: $SERVER_OUTPUT"
}

# 编译客户端
build_client() {
    echo "正在编译客户端..."
    
    # 查找所有客户端相关源文件
    local client_sources=""
    client_sources+="$(find_source_files "${PROJECT_ROOT}/client") "
    client_sources+="$(find_source_files "${PROJECT_ROOT}/common")"
    
    echo "找到的客户端源文件:"
    echo $client_sources | tr ' ' '\n' | sed 's/^/  /'
    
    # 编译
    $CXX $CXXFLAGS $INCLUDES -o "$CLIENT_OUTPUT" $client_sources $LIBS
    
    echo "客户端编译完成: $CLIENT_OUTPUT"
}

# 清理编译产物
clean() {
    echo "正在清理编译产物..."
    rm -f "$SERVER_OUTPUT" "$CLIENT_OUTPUT"
    echo "清理完成"
}

# 显示帮助信息
show_help() {
    echo "使用方法: $0 [选项]"
    echo "选项:"
    echo "  server    - 只编译服务器"
    echo "  client    - 只编译客户端"
    echo "  all       - 编译服务器和客户端 (默认)"
    echo "  clean     - 清理编译产物"
    echo "  help      - 显示此帮助信息"
}

# 主逻辑
main() {
    case "${1:-all}" in
        "server")
            build_server
            ;;
        "client")
            build_client
            ;;
        "all")
            build_server
            build_client
            ;;
        "clean")
            clean
            ;;
        "help"|"-h"|"--help")
            show_help
            ;;
        *)
            echo "错误: 未知选项 '$1'"
            show_help
            exit 1
            ;;
    esac
}

# 执行主函数
main "$@"