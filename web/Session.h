//
// Created by fallrain on 2023/4/20.
//

#ifndef ASIO_DEMO_SESSION_H

#define ASIO_DEMO_SESSION_H

#include "boost/asio.hpp"

#include "boost/algorithm/string.hpp"
#include "http_struct.h"
#include "websocket_session.h"
#include <boost/asio/ip/tcp.hpp>

class route;

class Session : public std::enable_shared_from_this<Session> {
public:
  Session(boost::asio::ip::tcp::socket socket, route &route);

  void start();

  void run_middlewares();

  // 请求头
  http_request_struct request = {};
  // 应大头
  http_response_struct response;

  std::unordered_map<std::string, std::string> *http_session = nullptr;
  // 写入请求
  void do_write();

  std::shared_ptr<websocket_session> upgrade_to_websocket();

  void websocket_handshake();
  route &route_;

private:
  // 读取请求
  void do_read();

  // 读取头
  void do_read_header();

  // 读取请求体
  void do_read_body();

  bool is_websocket_request();

  std::string sha1(const std::string &input);

  std::string base64_encode(const std::string &input);

  std::string magic = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

  // 客户端socket
  boost::asio::ip::tcp::socket client_socket_;
  // 客服端的buffer
  boost::asio::streambuf client_buffer_;
};

#endif // ASIO_DEMO_SESSION_H
