//
// Created by fallrain on 2026/3/9.
//

#include "router.h"
#include <filesystem>
#include <fstream>

void router::register_route(HttpMethod method, std::string path, http_handler handler) {
    http_routes_[method][path] = handler;
}

void router::register_route(std::string path, websocket_handler websocket_handler) {
    websocket_routes[path] = websocket_handler;
}

void router::post(std::string path, http_handler handler) {
    register_route(HttpMethod::POST, path, handler);
}

void router::get(std::string path, http_handler handler) {
    register_route(HttpMethod::GET, path, handler);
}

void router::ws(std::string path, websocket_handler handler) {
    register_route(path, handler);
}

void router::handle_request(std::shared_ptr<connection> session) {
    auto path = session->request.uri;
    size_t pos = path.find("?");
    if (pos != std::string::npos) {
        path = path.substr(0, pos);
    }

    auto route = http_routes_.find(session->request.method)->second.find(path);
    if (route != http_routes_.find(session->request.method)->second.end()) {
        route->second(session);
        return;
    }
    auto websocket_route = websocket_routes.find(path);
    if (websocket_route != websocket_routes.end()) {
        session->upgrade_to_websocket(websocket_route->second);
        return;
    }
    if (session->request.method == HttpMethod::GET) {
        std::string filePath = root_ + path;
        if (std::filesystem::is_directory(std::filesystem::path(filePath))) {
            filePath += "index.html";
        }

        if (std::filesystem::exists(std::filesystem::path(filePath))) {
            std::ifstream file_stream(filePath);
            if (file_stream.is_open()) {
                std::ostringstream ss;
                ss << file_stream.rdbuf();
                session->response.body = ss.str();
                return;
            }
        }
    }
    if (session->request.method == HttpMethod::OPTIONS) {
        return;
    }
    session->response.http_status = HttpStatusCode::NOT_FOUND;
}

void router::set_root(std::string root) {
    root_ = root;
}
