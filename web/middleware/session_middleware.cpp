#include "session_middleware.h"
//
// Created by fallrain on 2026/3/4.
//
void session_middleware::handle(std::shared_ptr<Session> session, std::function<void()> next) {
    if (session->request.method == HttpMethod::OPTIONS) {
        next();
        return;
    }
    string session_id;
    bool need_set_cookie = false;
    auto it = session->request.cookie.find("session");

    if (it == session->request.cookie.end()) {
        session_id = business_logic::create_session_map();
        need_set_cookie = true;
    } else {
        session_id = it->second;
        auto sesstion_map = business_logic::get_session_map(session_id);
        if (!sesstion_map) {
            session_id = business_logic::create_session_map();
            need_set_cookie = true;
        }
    }
    session->request.session_id = session_id;

    next();
    if (need_set_cookie) {
        session->response.cookie["session"] = session_id;
    }
}
