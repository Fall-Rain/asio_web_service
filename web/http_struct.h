//
// Created by fallrain on 2023/5/18.
//

#ifndef ASIO_DEMO_HTTP_STRUCT_H
#define ASIO_DEMO_HTTP_STRUCT_H

#include <boost/property_tree/ptree.hpp>
#include "map"


enum class HttpStatusCode {
    OK = 200,
    FOUND = 302,
    BAD_REQUEST = 400,
    UNAUTHORIZED = 401,
    FORBIDDEN = 403,
    NOT_FOUND = 404,
    INTERNAL_SERVER_ERROR = 500
};


enum class ContentType {
    TEXT_PLAIN,
    TEXT_HTML,
    APPLICATION_JSON,
    APPLICATION_XML
};

enum class HttpMethod {
    GET, POST, HEAD, OPTIONS, PUT, DELECT, TRACE, CONNECT
};

extern const std::map<HttpStatusCode, std::string> httpStatusToString;
extern const std::map<ContentType, std::string> contentTypeToString;
extern const std::map<HttpMethod, std::string> httpMethodToString;
extern const std::map<std::string, HttpMethod> stringToHttpMethod;


struct http_request_struct {
    std::map<std::string, std::string> headers;
    std::map<std::string, std::string> params;
    std::map<std::string, std::string> form_data;
    std::map<std::string, std::string> cookie;
    boost::property_tree::ptree ptree;
    std::string session_id;
    HttpMethod method;
    std::string body;
    std::string uri;
    std::string http_version;
    std::string http_body;
    std::string to_http_string();
};

struct http_response_struct {
    http_response_struct(std::string body);

    http_response_struct(std::string body, ContentType content_type);

    http_response_struct(HttpStatusCode http_status);

    http_response_struct();

    std::string to_http_string();

    std::map<std::string, std::string> headers;
    std::map<std::string, std::string> cookie;
    std::string body;
    ContentType content_type = ContentType::TEXT_HTML;
    HttpStatusCode http_status = HttpStatusCode::OK;
};


#endif //ASIO_DEMO_HTTP_STRUCT_H
