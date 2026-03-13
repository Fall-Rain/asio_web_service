//
// Created by fallrain on 2026/3/9.
//

#ifndef ASIO_DEMO_ROUTE_H
#define ASIO_DEMO_ROUTE_H
#include <functional>
#include <memory>

#include "connection.h"

using http_handler = std::function<void(std::shared_ptr<connection>)>;
using websocket_handler = std::function<void(std::shared_ptr<websocket_connection>)>;

class routers {
public:
    void set_root(std::string root);

    void get(const std::string& path, http_handler handler);

    void post(const std::string& path, http_handler handler);

    void ws(const std::string& path, websocket_handler handler);

    void handle_request(const std::shared_ptr<connection>& session);

private:
    void register_route(HttpMethod method, const std::string &path,
                        http_handler handler);

    void register_route(const std::string& path, websocket_handler websocket_handler);

    std::map<std::string, websocket_handler> websocket_routes = {};

    std::map<HttpMethod, std::map<std::string, http_handler> > http_routes_ = {
        {HttpMethod::CONNECT, {}}, {HttpMethod::DELECT, {}},
        {HttpMethod::GET, {}}, {HttpMethod::HEAD, {}},
        {HttpMethod::OPTIONS, {}}, {HttpMethod::POST, {}},
        {HttpMethod::PUT, {}}, {HttpMethod::TRACE, {}}
    };
    std::string root_;
};

#endif // ASIO_DEMO_ROUTE_H
