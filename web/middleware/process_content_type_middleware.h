//
// Created by fallrain on 2026/2/25.
//
#include "../Session.h"
#include "middleware.h"
typedef std::string string;

class process_content_type_middleware : public middleware {
public:
    void handle(std::shared_ptr<Session> session, std::function<void()> next) override;
};
