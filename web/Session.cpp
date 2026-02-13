//
// Created by fallrain on 2023/4/20.
//

#include "Session.h"


typedef std::string string;

Session::Session(boost::asio::ip::tcp::socket &socket) : client_socket_(std::move(socket)) {
}


void Session::start() {
    do_read();
}


// 处理请求的参数部分，从 URI 中解析参数
void Session::process_params() {
    auto self = shared_from_this(); // 获取当前 Session 的 shared_ptr
    size_t pos = self->request.uri.find("?"); // 查找 URI 中的参数部分起始位置
    if (pos == std::string::npos) {
        // 如果没有找到参数部分，直接返回
        return;
    }
    string query_string = self->request.uri.substr(pos + 1); // 提取参数部分
    std::vector<string> query_line; // 存储每个参数的键值对
    boost::split(query_line, query_string, boost::is_any_of("&")); // 使用 '&' 分割参数
    self->request.params = std::accumulate(query_line.begin(), // 将参数键值对添加到 params 映射中
                                           query_line.end(),
                                           std::map<string, string>(),
                                           [](std::map<std::string, std::string> &result, const string line) {
                                               std::vector<string> params; // 存储每个参数的键值对
                                               boost::split(params, line, boost::is_any_of("=")); // 使用 '=' 分割键值对
                                               if (params.size() == 2) {
                                                   // 如果键值对的大小为 2
                                                   result[params[0]] = params[1]; // 将键值对添加到 params 映射中
                                               }
                                               return result;
                                           });
}


// 处理请求的Content-Type，根据不同的类型解析请求体
void Session::process_content_type() {
    auto self = shared_from_this(); // 获取当前 Session 的 shared_ptr
    string body = self->request.body; // 获取请求体
    if (body.empty()) {
        // 如果请求体为空，则直接返回
        return;
    }
    std::map<std::string, std::string> headers = self->request.headers; // 获取请求头
    auto content_type = headers.find("Content-Type"); // 查找 Content-Type 头部
    if (content_type == headers.end()) {
        // 如果未找到 Content-Type 头部，则直接返回
        return;
    }
    ContentType contentType; // 定义 ContentType 枚举
    // 遍历 ContentType 映射，找到对应的 ContentType
    for (const auto &item: contentTypeToString) {
        if (item.second == content_type->second) {
            contentType = item.first;
            break;
        }
    }

    std::stringstream stringstream(body); // 创建 stringstream 用于解析请求体
    switch (contentType) {
        case ContentType::APPLICATION_XML: // 如果请求体是 XML 格式
            boost::property_tree::read_xml(stringstream, self->request.ptree); // 解析 XML 数据
            return;
        case ContentType::APPLICATION_JSON: // 如果请求体是 JSON 格式
            boost::property_tree::read_json(stringstream, self->request.ptree); // 解析 JSON 数据
            return;
    }

    boost::regex contentTypeRegex(R"(multipart/form-data;\s*boundary=(.*))");
    // 正则表达式匹配 multipart/form-data 类型的 Content-Type
    boost::smatch match;
    if (boost::regex_search(content_type->second, match, contentTypeRegex)) {
        // 如果 Content-Type 匹配成功
        if (match.size() > 1) {
            // 如果匹配结果的大小大于 1
            string boundary = match[1].str(); // 获取边界字符串
            std::vector<string> params; // 存储字段参数
            std::size_t pos = 0;
            std::size_t lastPos = 0;
            while ((pos = body.find(boundary, lastPos)) != std::string::npos) {
                // 循环查找边界
                string field_name; // 字段名
                string header = body.substr(lastPos, pos - lastPos); // 获取字段头部信息
                std::string::size_type name_pos = header.find("name=\""); // 查找字段名的起始位置
                if (name_pos != std::string::npos) {
                    // 如果找到字段名
                    std::string::size_type name_end_pos = header.find('\"', name_pos + 6); // 查找字段名的结束位置
                    field_name = header.substr(name_pos + 6, name_end_pos - name_pos - 6); // 提取字段名
                    std::cout << "field_name：" << field_name << std::endl; // 输出字段名
                    // 查找字段值的开始位置
                    std::string::size_type value_start_pos = body.find("\r\n\r\n", lastPos);
                    if (value_start_pos == string::npos) {
                        // 如果找不到字段值的起始位置
                        std::cerr << "Invalid part format: missing value start" << std::endl; // 输出错误信息
                        continue;
                    } else {
                        value_start_pos += 4;
                    }

                    // 查找字段值的结束位置
                    std::string::size_type value_end_pos = body.find(boundary, value_start_pos);
                    if (value_start_pos == string::npos) {
                        // 如果找不到字段值的结束位置
                        std::cerr << "Invalid part format: missing value end" << std::endl; // 输出错误信息
                        continue;
                    }
                    // 提取字段的值数据
                    std::string value = body.substr(value_start_pos, value_end_pos - value_start_pos - 4); // 提取字段的值
                    self->request.form_data[field_name] = value; // 将字段名和值添加到 form_data 映射中
                }
                lastPos = pos + boundary.length(); // 更新上一个边界的位置
            }
        }
        return;
    }
}

// 执行读取操作，从客户端读取请求
void Session::do_read() {
    // 异步读取直到遇到"\r\n\r\n"，表示请求头结束
    boost::asio::async_read_until(client_socket_, client_buffer_, "\r\n\r\n",
                                  // 读取完成后的回调函数
                                  [self = shared_from_this()](std::error_code ec, std::size_t bytes_transferred) {
                                      if (!ec) {
                                          // 如果没有发生错误
                                          // 将客户端缓冲区中的数据转换为字符串
                                          std::string request_header(
                                              boost::asio::buffers_begin(self->client_buffer_.data()),
                                              boost::asio::buffers_begin(self->client_buffer_.data()) +
                                              bytes_transferred);
                                          // 分割请求头字符串为单独的行
                                          std::vector<string> header_vec;
                                          boost::split(header_vec, request_header, boost::is_any_of("\r\n"),
                                                       boost::token_compress_on);
                                          // 解构请求结构体
                                          auto &[headers, params, form_data, cookie_map, json_map, session_id, method,
                                              body, uri, http_version] = self->request;
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

                                          // 解析Cookie头部
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
                                                                // 填充Cookie到cookie_map映射
                                                                cookie_map[cookie[0]] = cookie[1];
                                                            });
                                          }
                                          // 检查是否存在会话ID，如果不存在则创建一个新的会话ID
                                          if (cookie_map.find("session") == cookie_map.end()) {
                                              session_id = business_logic::create_session_map();
                                          } else {
                                              session_id = cookie_map["session"];
                                          }
                                          // 检查是否有请求体
                                          auto content_length = headers.find("Content-Length");
                                          if (content_length != headers.end()) {
                                              // 计算请求体中剩余的数据长度
                                              std::size_t excess_data_length =
                                                      self->client_buffer_.size() - bytes_transferred;
                                              std::vector<char> excess_data(excess_data_length);
                                              // 将多余的数据复制到excess_data中
                                              boost::asio::buffer_copy(boost::asio::buffer(excess_data),
                                                                       self->client_buffer_.data() + bytes_transferred);
                                              // 将多余的数据添加到请求体中
                                              body = string(excess_data.data(), excess_data_length);
                                              // 计算需要读取的数据长度
                                              size_t content_length = std::stoi(
                                                  headers.find("Content-Length")->second);
                                              if (content_length - body.size() > 0) {
                                                  std::vector<char> buffer(content_length - body.size());
                                                  boost::system::error_code error;
                                                  // 从套接字中读取剩余的请求体数据
                                                  size_t bytes_read = boost::asio::read(self->client_socket_,
                                                      boost::asio::buffer(buffer),
                                                      boost::asio::transfer_exactly(
                                                          content_length -
                                                          body.size()),
                                                      error);
                                                  if (!error) {
                                                      // 将读取的数据附加到请求体中
                                                      body.append(buffer.data(), bytes_read);
                                                  } else {
                                                      // 输出错误信息
                                                      std::cout << "Error reading request body: " << error.message()
                                                              << std::endl;
                                                  }
                                              }
                                          }
                                          // 处理请求的参数和内容类型
                                          self->process_params();
                                          self->process_content_type();
                                          try {
                                              // 处理请求并获取响应
                                              self->response = business_logic::process_request(self->request);
                                              self->response.headers["Access-Control-Allow-Origin"] = "*"; // 允许所有域访问
                                              self->response.headers["Access-Control-Allow-Methods"] =
                                                      "GET, POST, OPTIONS";
                                              self->response.headers["Access-Control-Allow-Headers"] =
                                                      "Content-Type, Authorization";
                                              // 如果请求中不包含会话ID，则将会话ID添加到响应的Cookie中
                                              if (self->request.cookie.find("session") == cookie_map.end()) {
                                                  self->response.cookie["session"] = session_id;
                                              }
                                          } catch (const std::exception &e) {
                                              // 如果发生异常，将异常信息添加到响应体中
                                              self->response.body = e.what();
                                          }
                                          // 执行写入操作，将响应发送给客户端
                                          self->do_write();
                                      }
                                  }
    );
}


// 执行写入操作，将响应发送给客户端
void Session::do_write() {
    // 解构响应结构体
    auto &[headers, cookie, body, content_type, http_status] = shared_from_this()->response;
    // 创建输出流
    std::ostringstream response_stream;
    // 获取状态码和状态字符串
    int status_code = static_cast<int>(http_status);
    string status_string = httpStatusToString[http_status];
    string contentType = contentTypeToString[content_type];
    // 如果Cookie不为空，构建Set-Cookie头部
    if (!cookie.empty()) {
        std::ostringstream ostringstream;
        for (const auto &item: cookie) {
            ostringstream << item.first << "=" << item.second << ";";
        }
        ostringstream.str().pop_back();
        headers["Set-Cookie"] = ostringstream.str();
    }

    // 构建响应头部
    response_stream << "HTTP/1.1" << " " << status_code << " " << status_string << "\r\n";
    response_stream << "Content-Type: " << contentType << "\r\n";
    response_stream << "Server: c++ server" << "\r\n";
    for (auto &item: headers) {
        response_stream << item.first << ": " << item.second << "\r\n";
    }
    // 如果内容类型为TEXT_HTML，则添加Content-Length头部
    if (content_type == ContentType::TEXT_HTML) {
        response_stream << "Content-Length:" << body.size() << "\r\n";
    }
    response_stream << "\r\n"; // 空行，分隔头部和主体
    response_stream << body; // 添加主体内容
    string response_str = response_stream.str(); // 获取完整的响应字符串

    std::cout << response_str << std::endl;
    // 异步写入响应到客户端
    boost::asio::async_write(client_socket_, boost::asio::buffer(response_str),
                             [self = shared_from_this()](std::error_code ec, std::size_t length) {
                                 if (!ec) {
                                 } else {
                                     // 如果发生错误，输出错误信息
                                     std::cerr << "write to remote server error: " << ec.message() << std::endl;
                                 }
                             });
}
