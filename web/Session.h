//
// Created by fallrain on 2023/4/20.
//

#ifndef ASIO_DEMO_SESSION_H

#include "boost/asio.hpp"
#include "boost/regex.hpp"
#include "iostream"
#include <boost/asio/ip/tcp.hpp>
#include "boost/algorithm/string.hpp"
#include "business_logic.h"
#include "http_struct.h"
#define ASIO_DEMO_SESSION_H

template<typename Derived>
struct Middleware {
    template<typename SessionType>
    void operator()(SessionType &session) {
        static_cast<Derived *>(this)->handle(session);
    }
};

class Session : public std::enable_shared_from_this<Session> {
public:
    Session(boost::asio::ip::tcp::socket &socket);

    void start();

    void run_middlewares();

    //请求头
    http_request_struct request;
    //应大头
    http_response_struct response;

    //写入请求
    void do_write();

    bool upgrade_to_websocket = false;

private:
    //读取请求
    void do_read();

    //读取头
    void do_read_header();

    //读取请求体
    void do_read_body();


    //客户端socket
    boost::asio::ip::tcp::socket client_socket_;
    //客服端的buffer
    boost::asio::streambuf client_buffer_;
};

#endif //ASIO_DEMO_SESSION_H
