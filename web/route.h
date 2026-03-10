//
// Created by fallrain on 2026/3/9.
//

#ifndef ASIO_DEMO_ROUTE_H
#define ASIO_DEMO_ROUTE_H
#include <functional>
#include <memory>

#include "Session.h"

using http_handler = std::function<void(std::shared_ptr<Session>)>;
using websocket_handler = std::function<void(std::shared_ptr<websocket_session>)>;

class route {
public:
    void set_root(std::string root);

    void get(std::string path, http_handler handler);

    void post(std::string path, http_handler handler);

    void ws(std::string path, websocket_handler handler);

    void handle_request(std::shared_ptr<Session> session);

private:
    void register_route(HttpMethod method, std::string path,
                        http_handler handler);

    void register_route(std::string path, websocket_handler websocket_handler);

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
