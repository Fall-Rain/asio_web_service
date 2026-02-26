//
// Created by fallrain on 2023/5/18.
//
#include "http_struct.h"

#include <boost/beast/http/message.hpp>

const std::map<HttpStatusCode, std::string> httpStatusToString = {
    {HttpStatusCode::OK, "OK"},
    {HttpStatusCode::BAD_REQUEST, "Bad Request"},
    {HttpStatusCode::UNAUTHORIZED, "Unauthorized"},
    {HttpStatusCode::FORBIDDEN, "Forbidden"},
    {HttpStatusCode::NOT_FOUND, "Not Found"},
    {HttpStatusCode::INTERNAL_SERVER_ERROR, "Internal Server Error"},
    {HttpStatusCode::FOUND, "Found"}
};
const std::map<ContentType, std::string> contentTypeToString = {
    {ContentType::TEXT_PLAIN, "text/plain"},
    {ContentType::TEXT_HTML, "text/html;charset=UTF-8"},
    {ContentType::APPLICATION_JSON, "application/json"},
    {ContentType::APPLICATION_XML, "application/xml"}
};


const std::map<std::string, HttpMethod> stringToHttpMethod = {
    {"GET", HttpMethod::GET},
    {"POST", HttpMethod::POST},
    {"HEAD", HttpMethod::HEAD},
    {"OPTIONS", HttpMethod::OPTIONS},
    {"PUT", HttpMethod::PUT},
    {"DELECT", HttpMethod::DELECT},
    {"TRACE", HttpMethod::TRACE},
    {"CONNECT", HttpMethod::CONNECT}
};

const std::map<HttpMethod, std::string> httpMethodToString = {
    {HttpMethod::GET, "GET"},
    {HttpMethod::POST, "POST"},
    {HttpMethod::HEAD, "HEAD"},
    {HttpMethod::OPTIONS, "OPTIONS"},
    {HttpMethod::PUT, "PUT"},
    {HttpMethod::DELECT, "DELECT"},
    {HttpMethod::TRACE, "TRACE"},
    {HttpMethod::CONNECT, "CONNECT"}
};

std::string http_request_struct::to_http_string() {
    return http_body;
    // std::ostringstream http_string;
    // http_string << httpMethodToString.at(method) << " " << uri << " " << http_version << "\r\n";
    // for (auto header: headers) {
    // http_string << header.first << ": " << header.second << "\r\n";
    // }
    // http_string << "\r\n" << body << std::endl << std::endl;
    // return http_string.str();
}

std::string http_response_struct::to_http_string() {
    // 创建输出流
    std::ostringstream response_stream;
    // 获取状态码和状态字符串
    int status_code = static_cast<int>(http_status);
    std::string status_string = httpStatusToString.at(http_status);
    std::string contentType = contentTypeToString.at(content_type);

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
    return response_stream.str(); // 获取完整的响应字符串
}


http_response_struct::http_response_struct(std::string body) : body(body) {
}

http_response_struct::http_response_struct(std::string body, ContentType content_type) : body(body),
    content_type(content_type) {
}

http_response_struct::http_response_struct(HttpStatusCode http_status) : http_status(http_status) {
}


http_response_struct::http_response_struct() {
}
