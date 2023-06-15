//
// Created by fallrain on 2023/5/18.
//
#include "http_struct.h"

std::map<HttpStatusCode, std::string> httpStatusToString = {
        {HttpStatusCode::OK,                    "OK"},
        {HttpStatusCode::BAD_REQUEST,           "Bad Request"},
        {HttpStatusCode::UNAUTHORIZED,          "Unauthorized"},
        {HttpStatusCode::FORBIDDEN,             "Forbidden"},
        {HttpStatusCode::NOT_FOUND,             "Not Found"},
        {HttpStatusCode::INTERNAL_SERVER_ERROR, "Internal Server Error"},
        {HttpStatusCode::FOUND,                 "Found"}
};
std::map<ContentType, std::string> contentTypeToString = {
        {ContentType::TEXT_PLAIN,       "text/plain"},
        {ContentType::TEXT_HTML,        "text/html;charset=UTF-8"},
        {ContentType::APPLICATION_JSON, "application/json"},
        {ContentType::APPLICATION_XML,  "application/xml"}
};


std::map<std::string, HttpMethod> stringToHttpMethod = {
        {"GET",     HttpMethod::GET},
        {"POST",    HttpMethod::POST},
        {"HEAD",    HttpMethod::HEAD},
        {"OPTION",  HttpMethod::OPTION},
        {"PUT",     HttpMethod::PUT},
        {"DELECT",  HttpMethod::DELECT},
        {"TRACE",   HttpMethod::TRACE},
        {"CONNECT", HttpMethod::CONNECT}
};

