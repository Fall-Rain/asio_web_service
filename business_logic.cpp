//
// Created by fallrain on 2023/5/18.
//
#include <iostream>
#include "business_logic.h"

std::map<HttpMethod, std::map<std::string, FunctionPtr>> business_logic::function_map = {
        {HttpMethod::GET,     {}},
        {HttpMethod::POST,    {}},
        {HttpMethod::HEAD,    {}},
        {HttpMethod::OPTION,  {}},
        {HttpMethod::PUT,     {}},
        {HttpMethod::DELECT,  {}},
        {HttpMethod::TRACE,   {}},
        {HttpMethod::CONNECT, {}}
};

void business_logic::register_handle(HttpMethod httpMethod, std::string handle_name, FunctionPtr function) {
    function_map[httpMethod][handle_name] = function;
}

http_response_struct business_logic::process_request(http_request_struct request) {
    std::string uri = request.uri;
    size_t pos = uri.find("?");
    if (pos != std::string::npos) {
        auto func = request.uri.substr(0, pos);
        uri = uri.substr(0, pos);
    }
    auto func = function_map.find(request.method)->second.find(uri);
    if (func != function_map.find(request.method)->second.end()) {
        return func->second(request);
    }

    if (request.method == HttpMethod::GET) {
        std::string filePath = "/var/www/html" + uri;
        std::ifstream file_stream(filePath);
        if (file_stream.good()) {
            if (file_stream.is_open()) {
                http_response_struct response;
                response.body.assign(std::istreambuf_iterator<char>(file_stream), std::istreambuf_iterator<char>());
                file_stream.close();
                return response;
            } else {
                std::cout << "Failed to open file: " << filePath << std::endl;
            }
        }
    }

    http_response_struct response;
    response.http_status = HttpStatusCode::NOT_FOUND;
    return response;

}

