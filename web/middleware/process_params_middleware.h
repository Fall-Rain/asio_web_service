//
// Created by fallrain on 2026/2/25.
//

#include "middleware.h"
#include "../Session.h"
typedef std::string string;

class process_params_middleware : public middleware {
public:
    void handle(std::shared_ptr<Session> session, std::function<void()> next) override;
};
