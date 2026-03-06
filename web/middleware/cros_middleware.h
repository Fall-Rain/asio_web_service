//
// Created by fallrain on 2026/2/25.
//

#include "middleware.h"
#include "../Session.h"

class cros_middleware : public middleware {
    void handle(std::shared_ptr<Session> session, std::function<void()> next) override;
};
