# DuckChat 快速入门指南

本指南将帮助您在5分钟内快速搭建和运行 DuckChat 聊天应用。

**文档版本**: v1.0.0  
**最后更新**: 2025-12-25  
**维护者**: DuckChat Team

## 🚀 一键启动

### 前置要求
- Linux/macOS/Windows 系统
- Git 工具
- 基础的开发环境（GCC/Clang）

### 快速部署
```bash
# 1. 克隆项目
git clone https://github.com/laipuran/DuckChat.git
cd DuckChat

# 2. 一键构建和启动
chmod +x quickstart.sh
./quickstart.sh
```

> 💡 **提示**: `quickstart.sh` 脚本会自动安装依赖、编译项目并启动服务器和客户端。

## 📋 手动安装（如果一键启动失败）

### 步骤1: 安装依赖

#### Ubuntu/Debian
```bash
sudo apt-get update
sudo apt-get install -y g++ make libssl-dev libsqlite3-dev libncursesw5-dev
```

#### CentOS/RHEL
```bash
sudo yum install -y gcc-c++ make openssl-devel sqlite-devel ncurses-devel
```

#### macOS
```bash
brew install gcc make openssl sqlite ncurses
```

### 步骤2: 编译项目
```bash
# 使用构建脚本
chmod +x build.sh
./build.sh all

# 或使用 Makefile
make all
```

### 步骤3: 启动应用

#### 启动服务器
```bash
# 终端1
./server/server
```

#### 启动客户端
```bash
# 终端2
./client/client
```

## 🎮 基础使用

### 首次使用
1. **注册用户**
   - 启动客户端后选择注册选项
   - 输入用户ID和用户名
   - 设置密码

2. **登录系统**
   - 使用注册的用户ID和密码登录

3. **创建聊天室**
   - 输入命令 `:cc <聊天室名称>` 创建聊天室
   - 例如：`:cc 我的聊天室`

4. **发送消息**
   - 按 `m` 键进入消息模式
   - 输入消息内容，按回车发送

### 基础命令
| 命令 | 功能 | 示例 |
|------|------|------|
| `:cc <名称>` | 创建聊天室 | `:cc 技术交流` |
| `:jc <ID>` | 加入聊天室 | `:cc chat123` |
| `:lc <ID>` | 离开聊天室 | `:lc chat123` |
| `:help` | 显示帮助 | `:help` |
| `m` | 进入消息模式 | `m` |
| `TAB` | 切换窗口 | `TAB` |
| `↑/↓` | 滚动消息 | `↑/↓` |
| `q` | 退出程序 | `q` |

## 🔧 常见配置

### 修改端口
编辑 `server/server.cpp` 中的端口设置：
```cpp
address.sin_port = htons(5001);  // 修改为您需要的端口
```

### 修改服务器地址
编辑 `client/client.cpp` 中的服务器地址：
```cpp
server_addr.sin_port = htons(5001);  // 修改为服务器端口
```

### 数据库位置
默认数据库文件位置：`./data/duckchat.db`

## 🐛 常见问题

### 编译错误
```bash
# 如果出现 ncurses 相关错误
sudo apt-get install libncursesw5-dev  # Ubuntu
brew install ncurses                   # macOS
```

### 连接失败
1. 检查服务器是否启动
2. 检查防火墙设置
3. 确认端口没有被占用

### 中文显示问题
```bash
# 确保系统支持UTF-8
export LANG=zh_CN.UTF-8
export LC_ALL=zh_CN.UTF-8
```

## 📚 下一步

### 学习更多
- [系统架构](docs/01-系统架构.md) - 了解整体设计
- [通信协议](docs/02-通信协议.md) - 理解通信机制
- [开发指南](docs/06-开发指南.md) - 学习开发环境搭建

### 高级功能
- 消息加密传输
- 文件传输功能
- 多用户权限管理
- 集群部署

### 参与贡献
1. Fork 项目仓库
2. 创建功能分支
3. 提交代码变更
4. 发起 Pull Request

## 🆘 获取帮助

### 文档资源
- [完整文档](docs/README.md)
- [API参考](docs/)
- [常见问题](TROUBLESHOOTING.md)

### 社区支持
- [GitHub Issues](https://github.com/laipuran/DuckChat/issues)
- [项目主页](https://github.com/laipuran/DuckChat)

### 联系方式
- 项目维护者：DuckChat Team
- 技术支持：通过 GitHub Issues

---

## 🎯 快速检查清单

在开始使用前，请确认：

- [ ] 已安装所有必需依赖
- [ ] 服务器成功启动并监听端口
- [ ] 客户端能够连接到服务器
- [ ] 能够注册和登录用户
- [ ] 能够创建和加入聊天室
- [ ] 能够发送和接收消息

如果以上所有项目都已完成，恭喜您！您已经成功部署了 DuckChat 系统。

---

**祝您使用愉快！** 🎉

---

**最后更新**: 2025-12-25  
**文档版本**: v1.0.0  
**维护者**: DuckChat Team