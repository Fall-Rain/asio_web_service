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
        main.cpp web/ThreadPool.cpp web/ThreadPool.h web/Session.cpp web/Session.h web/Server.cpp web/Server.h web/business_logic.h web/business_logic.cpp web/http_struct.h web/http_struct.cpp entity/user.cpp entity/user.h web/result.cpp web/result.h
        dao.cpp
        dao.h)
target_link_libraries(asio_demo
        Boost::system
        Boost::thread
        OpenSSL::SSL
        OpenSSL::Crypto
)
