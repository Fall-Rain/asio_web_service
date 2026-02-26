//
// Created by fallrain on 2023/5/18.
//
#include <iostream>
#include <utility>
#include "business_logic.h"

#include <optional>

std::map<HttpMethod, std::map<std::string, FunctionPtr> > business_logic::function_map = {
    {HttpMethod::GET, {}},
    {HttpMethod::POST, {}},
    {HttpMethod::HEAD, {}},
    {HttpMethod::OPTIONS, {}},
    {HttpMethod::PUT, {}},
    {HttpMethod::DELECT, {}},
    {HttpMethod::TRACE, {}},
    {HttpMethod::CONNECT, {}}
};

std::map<std::string, std::map<std::string, std::string> > business_logic::session_map;

std::string business_logic::root;

void business_logic::set_root(const std::string &value) {
    root = value; // 设置根路径
}

void business_logic::register_handle(HttpMethod httpMethod, const std::string &handle_name, FunctionPtr function) {
    function_map[httpMethod][handle_name] = std::move(function);
}

http_response_struct business_logic::process_request(const http_request_struct &request) {
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
        for (const auto &item: session_map[request.session_id]) {
            std::cout << item.first << "=" << item.second << std::endl;
        }

        std::string filePath = root + uri;
        if (std::filesystem::is_directory(std::filesystem::path(filePath))) {
            filePath += "index.html";
        }

        if (std::filesystem::exists(std::filesystem::path(filePath))) {
            std::ifstream file_stream(filePath);
            if (file_stream.is_open()) {
                std::ostringstream ss;
                ss << file_stream.rdbuf();
                return {ss.str()};
            } else {
                std::cout << "Failed to open file: " << filePath << std::endl;
            }
        }
    }
    if (request.method == HttpMethod::OPTIONS) {
        return {};
    }

    return {HttpStatusCode::NOT_FOUND};
}

std::string business_logic::create_session_map() {
    boost::uuids::uuid uuid = boost::uuids::random_generator()();
    std::string uuidStr = boost::uuids::to_string(uuid);
    session_map[uuidStr] = {};
    return uuidStr;
}

std::optional<std::reference_wrapper<std::map<std::string, std::string> > > business_logic::get_session_map(
    const std::string &cookie_id) {
    auto session = session_map.find(cookie_id);
    if (session != session_map.end()) {
        return session->second;
    }
    return std::nullopt;
    // return session_map[cookie_id];
}
