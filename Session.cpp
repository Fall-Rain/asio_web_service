//
// Created by fallrain on 2023/4/20.
//

#include "Session.h"


typedef std::string string;

Session::Session(boost::asio::ip::tcp::socket &socket) : client_socket_(std::move(socket)) {}


void Session::start() {
    do_read();
}


void Session::process_params() {
    auto self = shared_from_this();
    size_t pos = self->request.uri.find("?");
    if (pos == std::string::npos) {
        return;
    }
    string query_string = self->request.uri.substr(pos + 1);
    std::vector<string> query_line;
    boost::split(query_line, query_string, boost::is_any_of("&"));
    self->request.params = std::accumulate(query_line.begin(),
                                           query_line.end(),
                                           std::map<string, string>(),
                                           [](std::map<std::string, std::string> &result, const string line) {
                                               std::vector<string> params;
                                               boost::split(params, line, boost::is_any_of("="));
                                               if (params.size() == 2) {
                                                   result[params[0]] = params[1];
                                               }
                                               return result;
                                           });
}


void Session::process_content_type() {
    auto self = shared_from_this();
    string body = self->request.body;
    if (body.empty()) {
        return;
    }
    std::map<std::string, std::string> headers = self->request.headers;
    auto content_type = headers.find("Content-Type");
    if (content_type == headers.end()) {
        return;
    }
    ContentType contentType;
    for (const auto &item: contentTypeToString) {
        if (item.second == content_type->second) {
            contentType = item.first;
            break;
        }
    }

    std::stringstream stringstream(body);
    switch (contentType) {
        case ContentType::APPLICATION_XML:
            boost::property_tree::read_xml(stringstream, self->request.ptree);
            return;
        case ContentType::APPLICATION_JSON:
            boost::property_tree::read_json(stringstream, self->request.ptree);
            return;
    }

    boost::regex contentTypeRegex(R"(multipart/form-data;\s*boundary=(.*))");
    boost::smatch match;
    if (boost::regex_search(content_type->second, match, contentTypeRegex)) {
        if (match.size() > 1) {
            string boundary = match[1].str();
            std::vector<string> params;
            std::size_t pos = 0;
            std::size_t lastPos = 0;
            while ((pos = body.find(boundary, lastPos)) != std::string::npos) {
                string field_name;
                string header = body.substr(lastPos, pos - lastPos);
                std::string::size_type name_pos = header.find("name=\"");
                if (name_pos != std::string::npos) {
                    std::string::size_type name_end_pos = header.find('\"', name_pos + 6);
                    field_name = header.substr(name_pos + 6, name_end_pos - name_pos - 6);
                    std::cout << "field_name：" << field_name << std::endl;
                    // 查找字段值的开始位置
                    std::string::size_type value_start_pos = body.find("\r\n\r\n", lastPos);
                    if (value_start_pos == string::npos) {
                        std::cerr << "Invalid part format: missing value start" << std::endl;
                        continue;
                    } else {
                        value_start_pos += 4;
                    }

                    // 查找字段值的结束位置
                    std::string::size_type value_end_pos = body.find(boundary, value_start_pos);
                    if (value_start_pos == string::npos) {
                        std::cerr << "Invalid part format: missing value end" << std::endl;
                        continue;
                    }
                    // 提取字段的值数据
                    std::string value = body.substr(value_start_pos, value_end_pos - value_start_pos - 4);
                    self->request.form_data[field_name] = value;
                }
                lastPos = pos + boundary.length();
            }
        }
        return;
    }
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
                                          auto &[headers, params, form_data, cookie_map, json_map, session_id, method, body, uri, http_version] = self->request;
                                          std::vector<string> line;
                                          boost::split(line, header_vec[0], boost::is_any_of(" "));
                                          method = stringToHttpMethod[line[0]], uri = line[1], http_version = line[2];
                                          std::for_each(header_vec.begin() + 1, header_vec.end() - 1,
                                                        [&](std::string v) {
                                                            std::vector<string> header;
                                                            boost::split(header, v, boost::is_any_of(":"),
                                                                         boost::token_compress_on);
                                                            header[1].erase(std::remove_if(header[1].begin(),
                                                                                           header[1].end(),
                                                                                           [](unsigned char c) {
                                                                                               return std::isspace(c);
                                                                                           }), header[1].end());
                                                            self->request.headers[header[0]] = header[1];
//                                                            self->request.headers.insert(
//                                                                    std::pair<string, string>(header[0], header[1]));
                                                        });

                                          auto header_cookie = headers.find("Cookie");
                                          if (header_cookie != headers.end()) {
                                              std::vector<string> cookie_vec;
                                              boost::split(cookie_vec, header_cookie->second, boost::is_any_of(";"),
                                                           boost::token_compress_on);
                                              std::for_each(cookie_vec.begin(), cookie_vec.end(),
                                                            [&](const auto &item) {
                                                                std::vector<string> cookie;
                                                                boost::split(cookie, item, boost::is_any_of("="),
                                                                             boost::token_compress_on);
                                                                cookie_map[cookie[0]] = cookie[1];
//                                                                cookie_map.insert(std::pair<string, string>(cookie[0],
//                                                                                                            cookie[1]));
                                                            });
                                          }
                                          if (cookie_map.find("session") == cookie_map.end()) {
                                              session_id = business_logic::create_session_map();
//                                              cookie_map.insert(std::pair<string, string>("session",
//                                                                                          business_logic::create_cookie_map()));
                                          } else {
                                              session_id = cookie_map["session"];
                                          }
                                          auto content_length = headers.find("Content-Length");
                                          if (content_length != headers.end()) {
                                              std::size_t excess_data_length =
                                                      self->client_buffer_.size() - bytes_transferred;
                                              std::vector<char> excess_data(excess_data_length);
                                              boost::asio::buffer_copy(boost::asio::buffer(excess_data),
                                                                       self->client_buffer_.data() + bytes_transferred);
                                              body = string(excess_data.data(), excess_data_length);
                                              size_t content_length = std::stoi(
                                                      headers.find("Content-Length")->second);
                                              if (content_length - body.size() > 0) {
                                                  std::vector<char> buffer(content_length - body.size());
                                                  boost::system::error_code error;
                                                  size_t bytes_read = boost::asio::read(self->client_socket_,
                                                                                        boost::asio::buffer(buffer),
                                                                                        boost::asio::transfer_exactly(
                                                                                                content_length -
                                                                                                body.size()),
                                                                                        error);
                                                  if (!error) {
                                                      body.append(buffer.data(), bytes_read);
                                                  } else {
                                                      std::cout << "Error reading request body: " << error.message()
                                                                << std::endl;

                                                  }
                                              }
                                          }

                                          self->process_params();
                                          self->process_content_type();
//                                          std::cout << "请求头" << std::endl;
//                                          for (const auto &item: headers) {
//                                              std::cout << item.first << ":" << item.second << std::endl;
//                                          }
                                          try {
                                              self->response = business_logic::process_request(self->request);
                                              if (self->request.cookie.find("session") == cookie_map.end()) {
                                                  self->response.cookie["session"] = session_id;
                                              }

                                          } catch (const std::exception &e) {
                                              self->response.body = e.what();
                                          }


                                          self->do_write();
                                      }
                                  }
    );
}


void Session::do_write() {
    auto &[headers, cookie, body, content_type, http_status] = shared_from_this()->response;
    std::ostringstream response_stream;
    int status_code = static_cast<int>( http_status);
    string status_string = httpStatusToString[http_status];
    string contentType = contentTypeToString[content_type];
    if (!cookie.empty()) {
        std::ostringstream ostringstream;
        for (const auto &item: cookie) {
            ostringstream << item.first << "=" << item.second << ";";
        }
        ostringstream.str().pop_back();
        headers["Set-Cookie"] = ostringstream.str();
    }

    response_stream << "HTTP/1.1" << " " << status_code << " " << status_string << "\r\n";
//    response_stream << "HTTP/1.1" << " " << status_code << "\r\n";
    response_stream << "Content-Type: " << contentType << "\r\n";
    response_stream << "Server: c++ server" << "\r\n";
    for (auto &item: headers) {
        response_stream << item.first << ":" << item.second << "\r\n";
    }
    if (content_type == ContentType::TEXT_HTML) {
        response_stream << "Content-Length:" << body.size() << "\r\n";
    }
    response_stream << "\r\n";
    response_stream << body;
    string response_str = response_stream.str();
//    std::cout << response_str << std::endl;
    boost::asio::async_write(client_socket_, boost::asio::buffer(response_str),
                             [self = shared_from_this()](std::error_code ec, std::size_t length) {
                                 if (!ec) {
                                 } else {
                                     std::cerr << "write to remote server error: " << ec.message() << std::endl;
                                 }
                             });
}