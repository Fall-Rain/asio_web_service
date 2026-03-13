//
// Created by fallrain on 2026/3/9.
//

#include "routers.h"
#include <filesystem>
#include <fstream>
#include <utility>

void routers::register_route(HttpMethod method, const std::string& path, http_handler handler) {
    http_routes_[method][path] = std::move(handler);
}

void routers::register_route(const std::string& path, websocket_handler websocket_handler) {
    websocket_routes[path] = std::move(websocket_handler);
}

void routers::post(const std::string& path, http_handler handler) {
    register_route(HttpMethod::POST, path, std::move(handler));
}

void routers::get(const std::string& path, http_handler handler) {
    register_route(HttpMethod::GET, path, std::move(handler));
}

void routers::ws(const std::string& path, websocket_handler handler) {
    register_route(path, std::move(handler));
}

void routers::handle_request(const std::shared_ptr<connection>& session) {
    auto path = session->request.uri;
    size_t pos = path.find('?');
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

void routers::set_root(std::string root) {
    root_ = std::move(root);
}
