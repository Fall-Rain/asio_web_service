//
// Created by fallrain on 2023/4/20.
//

#include "Session.h"


typedef std::string string;

Session::Session(boost::asio::ip::tcp::socket &socket) : client_socket_(std::move(socket)) {}


void Session::start() {
    do_read();
}

void Session::do_read() {
    boost::asio::async_read_until(client_socket_, client_buffer_, "\r\n\r\n",
                                  [self = shared_from_this()](std::error_code ec, std::size_t bytes_transferred) {
                                      if (!ec) {
                                          std::string request_header(
                                                  boost::asio::buffers_begin(self->client_buffer_.data()),
                                                  boost::asio::buffers_begin(self->client_buffer_.data()) +
                                                  bytes_transferred);
                                          std::vector<string> header_vec;
                                          boost::split(header_vec, request_header, boost::is_any_of("\r\n"),
                                                       boost::token_compress_on);

                                          std::vector<string> line;
                                          boost::split(line, header_vec[0], boost::is_any_of(" "));
                                          self->method = line[0];
                                          self->uri = line[1];
                                          self->http_version = line[2];
                                          std::for_each(header_vec.begin() + 1, header_vec.end() - 1,
                                                        [&](std::string v) {
                                                            std::vector<string> header;
                                                            boost::split(header, v, boost::is_any_of(":"),
                                                                         boost::token_compress_on);
                                                            self->headers.insert(
                                                                    std::pair<string, string>(header[0], header[1]));
                                                        });


                                          std::cout << "请求头" << std::endl;
                                          std::cout << "method:" << self->method << std::endl;
                                          std::cout << "uri:" << self->uri << std::endl;
                                          std::cout << "http_version:" << self->http_version << std::endl;
                                          for (auto &item: self->headers) {
                                              std::cout << item.first << " => " << item.second << std::endl;
                                          }

                                          std::cout << std::endl;

                                          auto content_length = self->headers.find("Content-Length");
                                          if (content_length != self->headers.end()) {
                                              std::size_t excess_data_length =
                                                      self->client_buffer_.size() - bytes_transferred;
                                              std::vector<char> excess_data(excess_data_length);
                                              boost::asio::buffer_copy(boost::asio::buffer(excess_data),
                                                                       self->client_buffer_.data() + bytes_transferred);
                                              self->request_body = string(excess_data.data(), excess_data_length);
                                              size_t content_length = std::stoi(
                                                      self->headers.find("Content-Length")->second);
                                              if (content_length - self->request_body.size() > 0) {
                                                  std::vector<char> buffer(content_length - self->request_body.size());
                                                  boost::system::error_code error;
                                                  size_t bytes_read = boost::asio::read(self->client_socket_,
                                                                                        boost::asio::buffer(buffer),
                                                                                        boost::asio::transfer_exactly(
                                                                                                content_length -
                                                                                                self->request_body.size()),
                                                                                        error);
                                                  if (!error) {
                                                      self->request_body.append(buffer.data(), bytes_read);
                                                  } else {
                                                      std::cout << "Error reading request body: " << error.message()
                                                                << std::endl;

                                                  }
                                              }
                                              std::cout << "请求体：" << std::endl;
                                              std::cout << self->request_body << std::endl;
                                          }
                                          self->response_body = business_logic::process_request(self->uri)(
                                                  self->request_body, self->headers);
//                                          auto process_request = business_logic::process_request(self->uri);
//                                          if (process_request != NULL) {
//                                              self->response_body = process_request(self->request_body, self->headers);
//                                          } else {
//                                              self->response_body = self->request_body;
//                                          }
                                          self->do_write();
                                      }
                                  }
    );
}


void Session::do_write() {
    auto self = shared_from_this();
    std::ostringstream response_stream;
    response_stream << "HTTP/1.1 200 OK\r\n";
    response_stream << "Content-Type: text/html\r\n";
    response_stream << "Content-Length:" << self->response_body.size() << "\r\n";
    response_stream << "\r\n";
    response_stream << self->response_body;

    boost::asio::async_write(client_socket_, boost::asio::buffer(response_stream.str()),
                             [self = shared_from_this()](std::error_code ec, std::size_t length) {
                                 if (!ec) {
                                 } else {
                                     std::cerr << "write to remote server error: " << ec.message() << std::endl;
                                 }
                             });
}