//
// Created by fallrain on 2026/2/26.
//

#include "middleware.h"
#include "../Session.h"
typedef std::string string;

class session_middleware : public middleware {
    void handle(std::shared_ptr<Session> session, std::function<void()> next) override;
};
