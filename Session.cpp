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
                                          self->request.method = line[0];
                                          self->request.uri = line[1];
                                          self->request.http_version = line[2];
                                          std::for_each(header_vec.begin() + 1, header_vec.end() - 1,
                                                        [&](std::string v) {
                                                            std::vector<string> header;
                                                            boost::split(header, v, boost::is_any_of(":"),
                                                                         boost::token_compress_on);

                                                            self->request.headers.insert(
                                                                    std::pair<string, string>(header[0], header[1]));
                                                        });


//                                          std::cout << "请求头" << std::endl;
//                                          std::cout << "method:" << self->request.method << std::endl;
//                                          std::cout << "uri:" << self->request.uri << std::endl;
//                                          std::cout << "http_version:" << self->request.http_version << std::endl;
//                                          for (auto &item: self->request.headers) {
//                                              std::cout << item.first << " => " << item.second << std::endl;
//                                          }
//
//                                          std::cout << std::endl;

                                          auto content_length = self->request.headers.find("Content-Length");
                                          if (content_length != self->request.headers.end()) {
                                              std::size_t excess_data_length =
                                                      self->client_buffer_.size() - bytes_transferred;
                                              std::vector<char> excess_data(excess_data_length);
                                              boost::asio::buffer_copy(boost::asio::buffer(excess_data),
                                                                       self->client_buffer_.data() + bytes_transferred);
                                              self->request.body = string(excess_data.data(), excess_data_length);
                                              size_t content_length = std::stoi(
                                                      self->request.headers.find("Content-Length")->second);
                                              if (content_length - self->request.body.size() > 0) {
                                                  std::vector<char> buffer(content_length - self->request.body.size());
                                                  boost::system::error_code error;
                                                  size_t bytes_read = boost::asio::read(self->client_socket_,
                                                                                        boost::asio::buffer(buffer),
                                                                                        boost::asio::transfer_exactly(
                                                                                                content_length -
                                                                                                self->request.body.size()),
                                                                                        error);
                                                  if (!error) {
                                                      self->request.body.append(buffer.data(), bytes_read);
                                                  } else {
                                                      std::cout << "Error reading request body: " << error.message()
                                                                << std::endl;

                                                  }
                                              }
//                                              std::cout << "请求体：" << std::endl;
//                                              std::cout << self->request.body << std::endl;
                                          }
//                                          std::cout << "请求头：" << std::endl;
//                                          std::cout
//                                                  << self->request.method << " "
//                                                  << self->request.uri << " "
//                                                  << self->request.http_version << " "
//                                                  << std::endl;
//                                          for (auto &item: self->request.headers) {
//                                              std::cout << item.first << ":" << item.second << std::endl;
//                                          }
//                                          std::cout << "请求体：" << std::endl;
//                                          std::cout << self->request.body << std::endl;
                                          self->response = business_logic::process_request(self->request);
                                          self->do_write();
                                      }
                                  }
    );
}


void Session::do_write() {
    auto self = shared_from_this();
    http_response_struct response = self->response;
    std::ostringstream response_stream;
    int status_code = static_cast<int>( response.http_status);
    string status_string = httpStatusToString[response.http_status];
    string contentType = contentTypeToString[response.content_type];


    response_stream << "HTTP/1.1" << " " << status_code << " " << status_string << "\r\n";
    response_stream << "Content-Type: " << contentType << ";charset=UTF-8" << "\r\n";
    for (auto &item: response.headers) {
        response_stream << item.first << ":" << item.second << "\r\n";
    }
    response_stream << "Content-Length:" << response.body.size() << "\r\n";
    response_stream << "\r\n";
    response_stream << response.body;
    string response_str = response_stream.str();
    boost::asio::async_write(client_socket_, boost::asio::buffer(response_str),
                             [self = shared_from_this()](std::error_code ec, std::size_t length) {
                                 if (!ec) {
                                 } else {
                                     std::cerr << "write to remote server error: " << ec.message() << std::endl;
                                 }
                             });
}