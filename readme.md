# 🦆 Duck Chat

一个基于C++的简单聊天室应用，支持多用户实时通信、消息持久化和UTF-8中文支持。

## ✨ 特性

- 🚀 **高性能**: 基于C++17开发，多线程网络通信
- 💬 **实时聊天**: 支持多用户同时在线聊天
- 🇨🇳 **中文支持**: 完整的UTF-8编码支持
- 💾 **消息持久化**: SQLite数据库存储聊天记录
- 🔒 **安全通信**: OpenSSL加密传输
- 🖥️ **跨平台**: 支持Linux/macOS/Windows
- 🧪 **高质量代码**: 完整的CI/CD流程和代码质量检查

## 🏗️ 项目架构

```
DuckChat/
├── client/                 # 客户端代码
│   ├── client.cpp          # 客户端主程序
│   ├── chat_manager.cpp    # 聊天管理器
│   └── window_manager.cpp  # 界面管理器
├── server/                 # 服务器代码
│   ├── server.cpp          # 服务器主程序
│   ├── session.cpp         # 会话管理
│   ├── session_manager.cpp # 会话管理器
│   └── database.cpp        # 数据库操作
├── common/                 # 公共代码
│   ├── network.cpp         # 网络通信
│   ├── message.cpp         # 消息处理
│   └── utils.cpp           # 工具函数
├── third_party/            # 第三方库
│   └── json.hpp           # JSON库
└── docs/                   # 项目文档
```

## 🚀 快速开始

### 环境要求

- **编译器**: GCC 7+ 或 Clang 6+ (支持C++17)
- **系统**: Linux/macOS/Windows
- **依赖库**:
  - OpenSSL (加密通信)
  - SQLite3 (数据存储)
  - ncursesw (客户端界面)

### 安装依赖

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

### 编译运行

#### 方法1: 使用构建脚本
```bash
# 克隆项目
git clone https://github.com/laipuran/DuckChat.git
cd DuckChat

# 编译所有组件
chmod +x build.sh
./build.sh all

# 或者分别编译
./build.sh server  # 编译服务器
./build.sh client  # 编译客户端
./build.sh clean   # 清理编译产物
```

#### 方法2: 使用Makefile
```bash
make all           # 编译所有
make server        # 编译服务器
make client        # 编译客户端
make clean         # 清理
make help          # 查看帮助
```

### 运行应用

1. **启动服务器**
```bash
./server/server
```

2. **启动客户端** (新终端)
```bash
./client/client
```

3. **开始聊天** 🎉

## 📖 使用说明

### 服务器端
- 服务器默认监听端口 `5001`
- 支持多客户端同时连接
- 自动处理消息广播和持久化

### 客户端
- 启动后输入用户名
- 使用方向键或快捷键操作界面
- 支持发送中英文消息
- 自动接收其他用户消息

### 快捷键
- `↑/↓` - 浏览历史消息
- `Enter` - 发送消息
- `Ctrl+C` - 退出程序

## 🧪 开发和测试

### 本地开发
```bash
# 安装开发工具
sudo apt-get install clang-format cppcheck clang-tidy valgrind

# 代码格式化
find . -name "*.cpp" -o -name "*.hpp" | xargs clang-format -i

# 静态代码分析
cppcheck --enable=all --std=c++17 server/ client/ common/

# 内存检查
valgrind --leak-check=full ./server/server
```

### 运行测试
```bash
# 基本功能测试
./build.sh all
./server/server &
./client/client
```

## 🔄 CI/CD 流程

本项目配置了完整的GitHub Actions工作流：

### CI/CD 状态

[![Build and Test](https://github.com/laipuran/DuckChat/actions/workflows/build-and-test.yml/badge.svg)](https://github.com/laipuran/DuckChat/actions/workflows/build-and-test.yml)
[![Code Quality](https://github.com/laipuran/DuckChat/actions/workflows/code-quality.yml/badge.svg)](https://github.com/laipuran/DuckChat/actions/workflows/code-quality.yml)
[![Memory Check](https://github.com/laipuran/DuckChat/actions/workflows/memory-check.yml/badge.svg)](https://github.com/laipuran/DuckChat/actions/workflows/memory-check.yml)
[![Documentation](https://github.com/laipuran/DuckChat/actions/workflows/docs.yml/badge.svg)](https://github.com/laipuran/DuckChat/actions/workflows/docs.yml)
[![Benchmark](https://github.com/laipuran/DuckChat/actions/workflows/benchmark.yml/badge.svg)](https://github.com/laipuran/DuckChat/actions/workflows/benchmark.yml)

### 工作流说明
- **构建测试**: 多平台、多编译器构建验证
- **代码质量**: 格式检查、静态分析、代码规范
- **内存检查**: 内存泄漏检测、地址消毒器
- **文档生成**: 自动生成API文档并部署
- **性能测试**: 编译时间、运行时性能对比

详细配置说明请参考：[GitHub Actions 设置指南](GitHub-Actions-Setup.md)

## 📚 文档

### 📖 项目文档
- [文档导航](docs/README.md) - 完整的文档索引和阅读指南
- [文档索引](docs/DOCUMENTATION_INDEX.md) - 所有文档的详细索引
- [文档整理总结](docs/文档整理总结.md) - 文档体系整理过程和成果

### 🏗️ 核心设计文档
- [系统架构](docs/01-系统架构.md) - 系统整体架构和核心组件设计
- [通信协议](docs/02-通信协议.md) - 客户端与服务器通信协议定义
- [数据库设计](docs/03-数据库设计.md) - 数据库表结构和操作规范

### 💻 实现指南
- [服务器实现](docs/04-服务器实现.md) - 服务器端具体实现逻辑
- [客户端实现](docs/05-客户端实现.md) - 客户端具体实现逻辑

### 🛠️ 开发文档
- [开发指南](docs/06-开发指南.md) - 开发环境搭建和编码规范
- [部署说明](docs/07-部署说明.md) - 系统部署和配置说明

### 🚀 快速开始
- [快速开始](QUICKSTART.md) - 5分钟快速搭建和运行指南

### 🆘 支持文档
- [常见问题](TROUBLESHOOTING.md) - 常见问题解答和故障排除
- [变更日志](CHANGELOG.md) - 版本变更记录和更新历史

### 📋 项目管理
- [待办清单](docs/待办清单.md) - 项目开发任务和进度跟踪
- [项目总结](PROJECT_SUMMARY.md) - 项目完成情况和技术总结

## 🤝 贡献指南

我们欢迎所有形式的贡献！

### 贡献流程
1. Fork 本仓库
2. 创建特性分支 (`git checkout -b feature/AmazingFeature`)
3. 提交更改 (`git commit -m 'Add some AmazingFeature'`)
4. 推送到分支 (`git push origin feature/AmazingFeature`)
5. 创建 Pull Request

### 代码规范
- 遵循 [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html)
- 使用 clang-format 格式化代码
- 确保通过所有CI检查
- 添加适当的注释和文档

### Pull Request 检查清单
- [ ] 代码符合项目风格规范
- [ ] 已通过本地测试
- [ ] 已更新相关文档
- [ ] 无内存泄漏问题
- [ ] 兼容现有功能

## 🐛 问题反馈

如果你发现任何问题或有改进建议，请：

1. 查看 [Issues](https://github.com/laipuran/DuckChat/issues) 确认问题未被报告
2. 创建新的 Issue，详细描述问题
3. 提供复现步骤和环境信息
4. 添加相关标签

## 📄 许可证

本项目采用 MIT 许可证 - 查看 [LICENSE](LICENSE) 文件了解详情

## 🙏 致谢

感谢以下开源项目：
- [nlohmann/json](https://github.com/nlohmann/json) - JSON库
- [OpenSSL](https://www.openssl.org/) - 加密通信
- [SQLite](https://www.sqlite.org/) - 数据库
- [ncurses](https://www.gnu.org/software/ncurses/) - 终端界面

## 📞 联系方式

- 项目主页: https://github.com/laipuran/DuckChat
- 问题反馈: https://github.com/laipuran/DuckChat/issues
- 文档: https://laipuran.github.io/DuckChat/

---

**最后更新**: 2025-12-25
**项目版本**: v1.0.0
**维护者**: DuckChat Team

⭐ 如果这个项目对你有帮助，请给我们一个星标！