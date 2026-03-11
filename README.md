# OHD HTTP Server

A high-performance C++ Web framework built on top of Boost.Asio.

一个基于 Boost.Asio 实现的轻量级 C++ Web 框架，支持：

* HTTP Server
* Middleware
* Router
* WebSocket
* Session
* 高性能异步 IO

该项目主要用于探索 Web 框架底层实现，并尝试用现代 C++ 构建一个简洁优雅的 Web Server。

---

# Features

* 基于 Boost.Asio 的异步网络模型
* 零运行时开销的模板 Middleware
* 类似 Spring / Express 的路由系统
* WebSocket 支持
* 高性能线程池
* 可扩展的 Stream 抽象
* 模块化架构设计

---

# Architecture

项目结构：

```
server
│
├── entity
│   └── user
│
├── web
│   ├── Server
│   ├── connection
│   ├── router
│   ├── http_protocol
│   ├── websocket_connection
│   ├── websocket_frame
│   ├── asio_stream
│   └── ThreadPool
│
├── middleware
│   ├── log_middleware
│   ├── cors_middleware
│   ├── cookie_middleware
│   ├── session_middleware
│   ├── process_params_middleware
│   └── process_content_type_middleware
│
└── websocket_manager
```

请求处理流程：

```
accept
  ↓
connection
  ↓
parse http
  ↓
middleware chain
  ↓
router
  ↓
handler
```

---

# Quick Start

### 1 Clone

```bash
git clone https://github.com/yourname/ohd-http-server
cd ohd-http-server
```

### 2 Build

依赖：

* C++20
* Boost
* CMake

```bash
mkdir build
cd build
cmake ..
make
```

### 3 Run

```
./server
```

---

# Example

```cpp
#include "Server.h"

int main()
{
    router r;

    r.GET("/hello", [](auto &req, auto &res)
    {
        res.send("Hello World");
    });

    server s(8080, r);
    s.run();
}
```

访问：

```
http://localhost:8080/hello
```

---

# Middleware

支持类似 Web 框架的 Middleware 机制：

```cpp
middleware_chain<
    log_middleware,
    cors_middleware,
    session_middleware
>
```

执行流程：

```
before -> handler -> after
```

所有中间件在 **编译期展开**，无额外运行时开销。

---

# WebSocket

支持 WebSocket：

```cpp
router.ws("/chat", [](websocket_session &ws)
{
    ws.on_message([](auto msg)
    {
        ws.send(msg);
    });
});
```

---

# Stream Abstraction

框架内部抽象了 Stream 层：

```
tcp_stream
ssl_stream
websocket_stream
```

使得框架不依赖特定网络实现。

---

# Roadmap

未来计划：

* HTTP/1.1 KeepAlive
* SSL / TLS 支持
* HTTP/2
* 更高效 Router（Trie）
* C++20 协程支持
* 插件系统

---

# License

MIT License
