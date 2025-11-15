# 自动化Makefile - 自动发现所有源文件
# 使用方法:
#   make server  - 编译服务器
#   make client  - 编译客户端  
#   make all     - 编译所有（默认）
#   make clean   - 清理编译产物
#   make help    - 显示帮助

# 编译器设置
CXX = g++
CXXFLAGS = -g3 -O0 -std=c++11 -Wall -Wextra
INCLUDES = -I./common -I./third_party -I./server
LIBS = -pthread

# 输出文件
SERVER_OUTPUT = server/server
CLIENT_OUTPUT = client/client

# 自动发现源文件
SERVER_SOURCES = $(shell find server -name "*.cpp" 2>/dev/null | sort)
CLIENT_SOURCES = $(shell find client -name "*.cpp" 2>/dev/null | sort)
COMMON_SOURCES = $(shell find common -name "*.cpp" 2>/dev/null | sort)

# 所有源文件
ALL_SERVER_SOURCES = $(SERVER_SOURCES) $(COMMON_SOURCES)
ALL_CLIENT_SOURCES = $(CLIENT_SOURCES) $(COMMON_SOURCES)

# 默认目标
.PHONY: all server client clean help

all: server client

server:
	@echo "正在编译服务器..."
	@echo "找到的服务器源文件:"
	@echo "$(ALL_SERVER_SOURCES)" | tr ' ' '\n' | sed 's/^/  /'
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(SERVER_OUTPUT) $(ALL_SERVER_SOURCES) $(LIBS)
	@echo "服务器编译完成: $(SERVER_OUTPUT)"

client:
	@echo "正在编译客户端..."
	@echo "找到的客户端源文件:"
	@echo "$(ALL_CLIENT_SOURCES)" | tr ' ' '\n' | sed 's/^/  /'
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(CLIENT_OUTPUT) $(ALL_CLIENT_SOURCES) $(LIBS)
	@echo "客户端编译完成: $(CLIENT_OUTPUT)"

clean:
	@echo "正在清理编译产物..."
	@rm -f $(SERVER_OUTPUT) $(CLIENT_OUTPUT)
	@echo "清理完成"

help:
	@echo "使用方法: make [目标]"
	@echo "目标:"
	@echo "  server  - 编译服务器"
	@echo "  client  - 编译客户端"
	@echo "  all     - 编译服务器和客户端（默认）"
	@echo "  clean   - 清理编译产物"
	@echo "  help    - 显示此帮助信息"
	@echo ""
	@echo "当前发现的源文件:"
	@echo "服务器源文件: $(SERVER_SOURCES)"
	@echo "客户端源文件: $(CLIENT_SOURCES)"
	@echo "公共源文件: $(COMMON_SOURCES)"

# 调试信息
debug-info:
	@echo "编译器: $(CXX)"
	@echo "编译选项: $(CXXFLAGS)"
	@echo "包含路径: $(INCLUDES)"
	@echo "链接库: $(LIBS)"
	@echo ""
	@echo "服务器输出: $(SERVER_OUTPUT)"
	@echo "客户端输出: $(CLIENT_OUTPUT)"
	@echo ""
	@echo "服务器源文件:"
	@echo "$(ALL_SERVER_SOURCES)" | tr ' ' '\n' | sed 's/^/  /'
	@echo ""
	@echo "客户端源文件:"
	@echo "$(ALL_CLIENT_SOURCES)" | tr ' ' '\n' | sed 's/^/  /'