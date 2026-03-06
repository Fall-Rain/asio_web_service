//
// Created by fallrain on 2026/3/4.
//

#include "log_middleware.h"

void log_middleware::handle(std::shared_ptr<Session> session, std::function<void()> next) {
    std::cout << "请求："
            << std::endl
            << session->request.to_http_string()
            << std::endl
            << std::endl;
    next();
    std::cout << "应答："
            << std::endl
            << session->response.to_http_string()
            << std::endl
            << std::endl;
}
