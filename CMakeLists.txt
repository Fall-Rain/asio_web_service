cmake_minimum_required(VERSION 3.26)
project(asio_demo)

find_package(Boost REQUIRED COMPONENTS system thread)

find_package(OpenSSL REQUIRED)


set(CMAKE_CXX_STANDARD 17)

# 将 Boost 头文件目录添加到搜索路径
#include_directories(${Boost_INCLUDE_DIRS})

# 将 Boost 库目录添加到链接路径
#link_directories(${Boost_LIBRARY_DIRS})


add_executable(asio_demo
        main.cpp web/ThreadPool.cpp web/ThreadPool.h web/connection.cpp web/connection.h web/Server.cpp web/Server.h web/http_protocol.h web/http_protocol.cpp entity/user.cpp entity/user.h web/result.cpp web/result.h
        dao.cpp
        dao.h
        web/middleware/process_params_middleware.h
        web/middleware/process_content_type_middleware.h
        web/middleware/cros_middleware.h
        web/middleware/cookie_middleware.h
        web/middleware/session_middleware.h
        web/middleware/log_middleware.h
        web/websocket_connection.cpp
        web/websocket_connection.h
        web/websocket_frame.cpp
        web/websocket_frame.h
        web/middleware/middleware_chain.h
        web/routers.cpp
        web/routers.h
        web/session_manage.cpp
        web/session_manage.h
        websocket_manager.cpp
        websocket_manager.h
        web/stream/asio_stream.cpp
        web/stream/asio_stream.h
        web/stream/basic_stream.cpp
        web/stream/basic_stream.h
)
target_link_libraries(asio_demo
        Boost::system
        Boost::thread
        OpenSSL::SSL
        OpenSSL::Crypto
)
