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
#include "boost/network/protocol/http/client.hpp"
#define ASIO_DEMO_SESSION_H

class Session : public std::enable_shared_from_this<Session> {
public:

    Session(boost::asio::ip::tcp::socket &socket);

    void start();


private:
    //读取请求
    void do_read();
    //写入请求
    void do_write();
    //处理参数
    void process_params();

    //处理内容
    void process_content_type();
    //请求头
    http_request_struct request;
    //应大头
    http_response_struct response;
    //客户端socket
    boost::asio::ip::tcp::socket client_socket_;
    //客服端的buffer
    boost::asio::streambuf client_buffer_;
//
//    std::map<std::string, std::string> headers;
//    std::string request_body, response_body, method, uri, http_version;
};


#endif //ASIO_DEMO_SESSION_H
