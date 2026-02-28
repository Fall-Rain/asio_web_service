//
// Created by fallrain on 2023/4/20.
//

#include "Session.h"

#include "middleware/cookie_middleware.h"
#include "middleware/cros_middleware.h"
#include "middleware/log_middleware.h"
#include "middleware/process_content_type_middleware.h"
#include "middleware/process_params_middleware.h"
#include "middleware/session_middleware.h"

typedef std::string string;


Session::Session(boost::asio::ip::tcp::socket &socket) : client_socket_(std::move(socket)) {
    add_middleware(std::make_shared<process_params_middleware>());
    add_middleware(std::make_shared<process_content_type_middleware>());
    add_middleware(std::make_shared<log_middleware>());
    add_middleware(std::make_shared<cros_middleware>());
    add_middleware(std::make_shared<cookie_middleware>());
    add_middleware(std::make_shared<session_middleware>());
}


void Session::start() {
    do_read();
}


void Session::add_middleware(std::shared_ptr<middleware> m) {
    middlewares_.push_back(m);
}

void Session::run_middlewares() {
    run_next(0);
}


void Session::run_next(size_t index) {
    if (index < middlewares_.size()) {
        middlewares_[index].get()->handle(shared_from_this(), [this,index] {
            run_next(index + 1);
        });
    } else {
        try {
            response = business_logic::process_request(request);
        } catch (const std::exception &e) {
            response.body = e.what();
        }
    }
}

void Session::do_read_header() {
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

void Session::do_read_body() {
    auto self = shared_from_this();
    auto &[headers, params, form_data, cookie_map, json_map, session_id, method,
        body, uri, http_version,http_body] = self->request;
    // 检查是否有请求体
    auto content_length_it = headers.find("Content-Length");
    if (content_length_it == headers.end()) {
        self->run_middlewares();
        self->do_write();
        return;
    }

    std::size_t content_length = std::stoi(content_length_it->second);

    if (self->client_buffer_.size() >= content_length) {
        body.assign(
            boost::asio::buffers_begin(self->client_buffer_.data()),
            boost::asio::buffers_begin(self->client_buffer_.data()) +
            content_length);
        self->client_buffer_.consume(content_length);
        self->request.http_body += body;
        self->run_middlewares();
        self->do_write();
        return;
    }

    boost::asio::async_read(
        self->client_socket_, self->client_buffer_,
        boost::asio::transfer_exactly(content_length),
        [self,content_length](std::error_code ec, std::size_t size) {
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
        });
}

// 执行读取操作，从客户端读取请求
void Session::do_read() {
    do_read_header();
}


// 执行写入操作，将响应发送给客户端
void Session::do_write() {
    // 异步写入响应到客户端
    boost::asio::async_write(client_socket_, boost::asio::buffer(shared_from_this()->response.to_http_string()),
                             [self = shared_from_this()](std::error_code ec, std::size_t length) {
                                 if (ec) {
                                     std::cerr << "write to remote server error: " << ec.message() << std::endl;
                                 }
                             });
}
