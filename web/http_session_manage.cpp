//
// Created by fallrain on 2026/3/10.
//

#include "http_session_manage.h"

http_session_manage &http_session_manage::instance() {
    static http_session_manage instance_;
    return instance_;
}

http_session_ptr http_session_manage::get_session(const std::string &session_id) {
    std::lock_guard lock(mutex_);
    auto it = sessions_.find(session_id);
    if (it != sessions_.end()) {
        return it->second;
    }
    auto session = std::make_shared<http_session_data>();
    sessions_[session_id] = session;
    return session;
}


void http_session_manage::remove_session(const std::string &session_id) {
    std::lock_guard lock(mutex_);
    sessions_.erase(session_id);
}
