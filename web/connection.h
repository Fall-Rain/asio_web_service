//
// Created by fallrain on 2023/4/20.
//

#ifndef ASIO_DEMO_SESSION_H

#define ASIO_DEMO_SESSION_H

#include "boost/asio.hpp"

#include "boost/algorithm/string.hpp"
#include "http_protocol.h"
#include "websocket_connection.h"
#include <boost/asio/ip/tcp.hpp>
#include "session_manage.h"

class router;

class connection : public std::enable_shared_from_this<connection> {
public:
    connection(boost::asio::ip::tcp::socket socket, router &route);

    using websocket_handler = std::function<void(std::shared_ptr<websocket_connection>)>;


    void start();

    void run_middlewares();

    // 请求头
    http_request_struct request = {};
    // 应大头
    http_response_struct response;

    // std::shared_ptr<std::unordered_map<std::string, std::string>> http_session;

    http_session_ptr http_session;

    // 写入请求
    void do_write();

    void websocket_handshake();

    router &route_;

    void upgrade_to_websocket(websocket_handler websocket_handler);

    bool is_websocket_request();

    websocket_handler websocket_handler_;

private:
    // 读取请求
    void do_read();

    // 读取头
    void do_read_header();

    // 读取请求体
    void do_read_body();


    std::string sha1(const std::string &input);

    std::string base64_encode(const std::string &input);

    std::string magic = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

    // 客户端socket
    boost::asio::ip::tcp::socket client_socket_;
    // 客服端的buffer
    boost::asio::streambuf client_buffer_;


    bool is_upgrade_to_websocket = false;
};

#endif // ASIO_DEMO_SESSION_H
