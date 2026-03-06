//
// Created by fallrain on 2026/2/26.
//
#include "../Session.h"
#include "middleware.h"

typedef std::string string;

class cookie_middleware : public middleware {
public:
    void handle(std::shared_ptr<Session> session, std::function<void()> next) override;
};
