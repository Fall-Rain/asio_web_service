//
// Created by fallrain on 2023/4/20.
//

#include "connection.h"

#include "websocket_connection.h"
#include "middleware/cookie_middleware.h"
#include "middleware/cros_middleware.h"
#include "middleware/log_middleware.h"
#include "middleware/middleware_chain.h"
#include "middleware/process_content_type_middleware.h"
#include "middleware/process_params_middleware.h"
#include "middleware/session_middleware.h"
#include <openssl/evp.h>
#include <boost/algorithm/string/case_conv.hpp>
#include <utility>
#include <openssl/sha.h>
#include "boost/regex.hpp"
#include "iostream"
typedef std::string string;


connection::connection(boost::asio::ip::tcp::socket socket, routers &route) : client_socket_(std::move(socket)),
                                                                              route_(route) {
}


void connection::run_middlewares() {
    middleware_chain<
        process_params_middleware,
        process_content_type_middleware,
        log_middleware,
        cros_middleware,
        // websocket_middlesare,
        cookie_middleware,
        session_middleware
    >::run(shared_from_this());
}

void connection::start() {
    do_read();
}


void connection::websocket_handshake() {
    auto secWebsocketKey = request.headers.find("Sec-WebSocket-Key")->second;
    response.http_status = HttpStatusCode::SWITCHING_PROTOCOLS;
    response.headers["Upgrade"] = "websocket";
    response.headers["Connection"] = "Upgrade";
    response.headers["Sec-WebSocket-Accept"] = base64_encode(sha1(secWebsocketKey + magic));
}


void connection::upgrade_to_websocket(websocket_handler websocket_handler) {
    if (!is_websocket_request()) {
        throw std::runtime_error("not websocket request");
    }
    websocket_handshake();
    is_upgrade_to_websocket = true;
    websocket_handler_ = std::move(websocket_handler);
}

void connection::do_read_header() {
    boost::asio::async_read_until(
        client_socket_, client_buffer_, "\r\n\r\n",
        // 读取完成后的回调函数
        [self = shared_from_this()](std::error_code ec, std::size_t bytes_transferred) {
            if (ec) {
                return;
            }
            // 如果没有发生错误
            // 将客户端缓冲区中的数据转换为字符串
            std::string request_header(
                boost::asio::buffers_begin(self->client_buffer_.data()),
                boost::asio::buffers_begin(self->client_buffer_.data()) +
                bytes_transferred);
            self->client_buffer_.consume(bytes_transferred);

            // 分割请求头字符串为单独的行
            std::vector<string> header_vec;
            boost::split(header_vec, request_header, boost::is_any_of("\r\n"),
                         boost::token_compress_on);
            // 解构请求结构体
            auto &[headers, params, form_data, cookie_map, json_map, session_id, method,
                body, uri, http_version,http_body] = self->request;
            http_body = request_header;
            // 分割请求行，提取方法、URI和HTTP版本
            std::vector<string> line;
            boost::split(line, header_vec[0], boost::is_any_of(" "));
            method = stringToHttpMethod.at(line[0]), uri = line[1], http_version = line[2];
            // 解析请求头部并填充请求结构体的headers成员
            std::for_each(header_vec.begin() + 1, header_vec.end() - 1,
                          [&](std::string v) {
                              std::vector<string> header;
                              boost::split(header, v, boost::is_any_of(":"),
                                           boost::token_compress_on);
                              // 移除头部值中的空白字符
                              header[1].erase(std::remove_if(header[1].begin(),
                                                             header[1].end(),
                                                             [](unsigned char c) {
                                                                 return std::isspace(c);
                                                             }), header[1].end());
                              // 填充请求头部到headers映射
                              self->request.headers[header[0]] = header[1];
                          });

            self->do_read_body();
        });
}


void connection::do_read_body() {
    auto &[headers, params, form_data, cookie_map, json_map, session_id, method,
        body, uri, http_version,http_body] = request;
    // 检查是否有请求体
    auto content_length_it = headers.find("Content-Length");
    if (content_length_it == headers.end()) {
        run_middlewares();
        do_write();
        if (is_upgrade_to_websocket) {
            auto ws = std::make_shared<websocket_connection>(
                std::move(client_socket_),
                request,
                websocket_handler_,
                http_session);
            ws->start();
        }
        return;
    }


    std::size_t content_length = std::stoi(content_length_it->second);

    if (client_buffer_.size() >= content_length) {
        body.assign(
            boost::asio::buffers_begin(client_buffer_.data()),
            boost::asio::buffers_begin(client_buffer_.data()) +
            content_length);
        client_buffer_.consume(content_length);
        request.http_body += body;
        run_middlewares();
        do_write();
        if (is_upgrade_to_websocket) {
            auto ws = std::make_shared<websocket_connection>(
                std::move(client_socket_),
                request,
                websocket_handler_,
                http_session);
            ws->start();
        }
        return;
    }

    boost::asio::async_read(
        client_socket_, client_buffer_,
        boost::asio::transfer_exactly(content_length),
        [self = shared_from_this(),content_length](std::error_code ec, std::size_t size) {
            if (ec) return;
            auto &body = self->request.body;

            body.assign(
                boost::asio::buffers_begin(
                    self->client_buffer_.data()),
                boost::asio::buffers_begin(
                    self->client_buffer_.data()) +
                content_length
            );
            self->client_buffer_.consume(content_length);
            self->request.http_body += body;
            self->run_middlewares();
            self->do_write();
            if (self->is_upgrade_to_websocket) {
                auto ws = std::make_shared<websocket_connection>(
                    std::move(self->client_socket_),
                    self->request,
                    self->websocket_handler_,
                    self->http_session);
                ws->start();
            }
        });
}

// 执行读取操作，从客户端读取请求
void connection::do_read() {
    do_read_header();
}


bool connection::is_websocket_request() {
    if (request.method != HttpMethod::GET) {
        return false;
    }
    if (request.headers.find("Upgrade") == request.headers.end()
        || request.headers.find("Connection") == request.headers.end()
        || request.headers.find("Sec-WebSocket-Key") == request.headers.end()
        || request.headers.find("Sec-WebSocket-Version") == request.headers.end()) {
        return false;
    }
    auto upgrade = request.headers.find("Upgrade")->second;
    auto connection = request.headers.find("Connection")->second;
    auto version = request.headers.find("Sec-WebSocket-Version")->second;
    boost::to_lower(upgrade);
    if (upgrade != "websocket") {
        return false;
    }
    if (connection.find("Upgrade") == std::string::npos) {
        return false;
    }
    if (version != "13") {
        return false;
    }
    is_upgrade_to_websocket = true;
    return true;
}

std::string connection::base64_encode(const std::string &input) {
    int len = 4 * ((input.size() + 2) / 3);
    std::string output(len, '\0');

    int out_len = EVP_EncodeBlock(
        reinterpret_cast<unsigned char *>(&output[0]),
        reinterpret_cast<const unsigned char *>(input.data()),
        input.size());

    output.resize(out_len);
    return output;
}

std::string connection::sha1(const std::string &input) {
    unsigned char hash[SHA_DIGEST_LENGTH];

    SHA1(reinterpret_cast<const unsigned char *>(input.c_str()),
         input.size(),
         hash);

    return std::string(reinterpret_cast<char *>(hash),
                       SHA_DIGEST_LENGTH);
}

// 执行写入操作，将响应发送给客户端
void connection::do_write() {
    auto self = shared_from_this();
    // 异步写入响应到客户端
    boost::asio::async_write(client_socket_, boost::asio::buffer(self->response.to_http_string()),
                             [self = shared_from_this()](std::error_code ec, std::size_t length) {
                                 if (ec) {
                                     std::cerr << "write to remote server error: " << ec.message() << std::endl;
                                 }
                                 if (self->should_keep_alive()) {
                                     self->reset_request();
                                     self->do_read();
                                 } else {
                                     self->client_socket_.close();
                                     
                                 }
                                 // self->client_socket_.close();
                             });
}

bool connection::should_keep_alive() {
    auto conn = request.headers.find("Connection");

    if (request.http_version == "HTTP/1.1") {
        if (conn != request.headers.end()) {
            std::string v = conn->second;
            boost::to_lower(v);
            if (v == "close")
                return false;
        }
        return true;
    }

    if (request.http_version == "HTTP/1.0") {
        if (conn != request.headers.end()) {
            std::string v = conn->second;
            boost::to_lower(v);
            if (v == "keep-alive")
                return true;
        }
        return false;
    }
    return false;
}

void connection::reset_request() {
    request = {};
    response = {};
}
