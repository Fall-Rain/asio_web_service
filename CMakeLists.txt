cmake_minimum_required(VERSION 3.26)
project(asio_demo)

find_package(Boost REQUIRED COMPONENTS system thread)

find_package(OpenSSL REQUIRED)

find_package (cppnetlib 0.13.0 REQUIRED)

set(CMAKE_CXX_STANDARD 17)

# 将 Boost 头文件目录添加到搜索路径
#include_directories(${Boost_INCLUDE_DIRS})

# 将 Boost 库目录添加到链接路径
#link_directories(${Boost_LIBRARY_DIRS})


add_executable(asio_demo main.cpp ThreadPool.cpp ThreadPool.h Session.cpp Session.h Server.cpp Server.h business_logic.h business_logic.cpp http_struct.h http_struct.cpp user.cpp user.h result.cpp result.h)
target_link_libraries(asio_demo
        Boost::system
        Boost::thread
        OpenSSL::SSL
        OpenSSL::Crypto
        cppnetlib-uri
        cppnetlib-client-connections
        cppnetlib-server-parsers
)
