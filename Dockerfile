# 阶段 1: 公共构建阶段
FROM debian:stable AS builder

# 设置工作目录
WORKDIR /app

# 配置国内APT源
RUN sed -i 's/deb.debian.org/mirrors.ustc.edu.cn/g' /etc/apt/sources.list && \
    apt-get update

# 安装所有编译依赖
RUN apt-get install -y \
    g++ \
    make \
    libssl-dev \
    libsqlite3-dev \
    libncursesw5-dev \
    pkg-config \
    && rm -rf /var/lib/apt/lists/*

# 复制项目文件
COPY . .

# 赋予构建脚本执行权限
RUN chmod +x build.sh config.sh

# 编译服务器和客户端
RUN ./build.sh all

# 阶段 2A: Server 运行时镜像
FROM debian:stable-slim AS server-runtime

# 设置工作目录
WORKDIR /app

# 配置国内APT源
RUN sed -i 's/deb.debian.org/mirrors.ustc.edu.cn/g' /etc/apt/sources.list && \
    apt-get update

# 安装 Server 运行时需要的库
RUN apt-get install -y \
    libssl3 \
    libsqlite3-0 \
    && rm -rf /var/lib/apt/lists/*

# 创建非root用户
RUN useradd -m -u 1000 duckchat && \
    chown -R duckchat:duckchat /app

# 从构建阶段复制 Server 可执行文件
COPY --from=builder /app/server/server /usr/local/bin/server
COPY --from=builder /app/server /app/server

# 设置执行权限
RUN chmod +x /usr/local/bin/server

# 切换到非root用户
USER duckchat

# 暴露服务器端口（假设使用8080端口）
EXPOSE 5001

# 设置环境变量
ENV SERVER_HOST=0.0.0.0
ENV SERVER_PORT=5001

# 健康检查
HEALTHCHECK --interval=30s --timeout=3s --start-period=5s --retries=3 \
    CMD netstat -ln | grep :5001 || exit 1

VOLUME [ "/data" ]

# 启动 Server 进程
ENTRYPOINT ["/usr/local/bin/server"]

# 阶段 2B: Client 运行时镜像
FROM debian:stable-slim AS client-runtime

# 设置工作目录
WORKDIR /app

# 配置国内APT源
RUN sed -i 's/deb.debian.org/mirrors.ustc.edu.cn/g' /etc/apt/sources.list && \
    apt-get update

# 安装 Client 运行时需要的库
RUN apt-get install -y \
    libncursesw5 \
    libssl3 \
    libsqlite3-0 \
    && rm -rf /var/lib/apt/lists/*

# 创建非root用户
RUN useradd -m -u 1000 duckchat && \
    chown -R duckchat:duckchat /app

# 从构建阶段复制 Client 可执行文件
COPY --from=builder /app/client/client /usr/local/bin/client
COPY --from=builder /app/client /app/client

# 设置执行权限
RUN chmod +x /usr/local/bin/client

# 切换到非root用户
USER duckchat

# 设置环境变量
ENV CLIENT_HOST=localhost
ENV CLIENT_PORT=5001

# 启动 Client 进程
ENTRYPOINT ["/usr/local/bin/client"]