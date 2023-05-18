//
// Created by fallrain on 2023/5/18.
//

#ifndef ASIO_DEMO_HTTP_STRUCT_H
#define ASIO_DEMO_HTTP_STRUCT_H

#include "map"


enum class HttpStatusCode {
    OK = 200,
    BAD_REQUEST = 400,
    UNAUTHORIZED = 401,
    FORBIDDEN = 403,
    NOT_FOUND = 404,
    INTERNAL_SERVER_ERROR = 500
};

extern std::map<HttpStatusCode, std::string> httpStatusToString;

enum class ContentType {
    TEXT_PLAIN,
    TEXT_HTML,
    APPLICATION_JSON,
    APPLICATION_XML
};

extern std::map<ContentType, std::string> contentTypeToString;


struct http_request_struct {
    std::map<std::string, std::string> headers;
    std::string body;
    std::string method, uri, http_version;
};

struct http_response_struct {
    std::map<std::string, std::string> headers;
    std::string body;
    ContentType content_type = ContentType::TEXT_HTML;
    HttpStatusCode http_status = HttpStatusCode::OK;
};


#endif //ASIO_DEMO_HTTP_STRUCT_H
