#include "cros_middleware.h"
//
// Created by fallrain on 2026/3/4.
//
void cros_middleware::handle(std::shared_ptr<Session> session, std::function<void()> next) {
    next();
    session->response.headers["Access-Control-Allow-Origin"] = "http://localhost:5173"; // 允许所有域访问
    session->response.headers["Access-Control-Allow-Methods"] = "GET, POST, OPTIONS";
    session->response.headers["Access-Control-Allow-Headers"] =
            "Content-Type, Authorization";
    // Access-Control-Allow-Credentials: true
    session->response.headers["Access-Control-Allow-Credentials"] = "true";
}
