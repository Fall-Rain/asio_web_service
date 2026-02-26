//
// Created by fallrain on 2026/2/26.
//

#include "middleware.h"
#include "../Session.h"

class log_middleware : public middleware {
    void handle(std::shared_ptr<Session> session, std::function<void()> next) override {
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
};
