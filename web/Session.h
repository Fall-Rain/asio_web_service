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
#include "numeric"
#include "boost/beast.hpp"
#include "cctype"
#include "middleware/middleware.h"
#define ASIO_DEMO_SESSION_H

class Session : public std::enable_shared_from_this<Session> {
public:
    Session(boost::asio::ip::tcp::socket &socket);

    void start();

    void add_middleware(std::shared_ptr<middleware> middleware_);

    void run_middlewares();


    //请求头
    http_request_struct request;
    //应大头
    http_response_struct response;

private:
    //读取请求
    void do_read();

    void do_read_header();
    void do_read_body();

    //写入请求
    void do_write();

    //客户端socket
    boost::asio::ip::tcp::socket client_socket_;
    //客服端的buffer
    boost::asio::streambuf client_buffer_;

    //处理参数
    // void process_params();

    //处理内容
    // void process_content_type();


    std::vector<std::shared_ptr<middleware> > middlewares_;

    void run_next(size_t index);
};


#endif //ASIO_DEMO_SESSION_H
