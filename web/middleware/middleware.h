//
// Created by fallrain on 2026/2/26.
//

#ifndef ASIO_DEMO_MIDDLEWARE_H
#define ASIO_DEMO_MIDDLEWARE_H
#include <functional>
#include <memory>

class Session;

class middleware {
public:
    virtual ~middleware() = default;

    virtual void handle(std::shared_ptr<Session> session, std::function<void()> next) =0;
};
#endif //ASIO_DEMO_MIDDLEWARE_H
