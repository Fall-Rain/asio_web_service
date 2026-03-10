//
// Created by fallrain on 2026/2/26.
//

#include "../Session.h"
#include "../http_session_manage.h"
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
typedef std::string string;

class session_middleware {
public:
  void handle(std::shared_ptr<Session> session, std::function<void()> next) {
    if (session->request.method == HttpMethod::OPTIONS) {
      next();
      return;
    }
    string session_id;
    bool need_set_cookie = false;
    auto it = session->request.cookie.find("session");
    if (it == session->request.cookie.end()) {
      need_set_cookie = true;
      session_id = generate_uuid();
    } else {
      session_id = it->second;
    }
    session->http_session =
        &http_session_manage::instance().get_session(session_id);

    // if (it == session->request.cookie.end()) {
    //     session_id = business_logic::create_session_map();
    //     need_set_cookie = true;
    // } else {
    //     session_id = it->second;
    //     auto sesstion_map = business_logic::get_session_map(session_id);
    //     if (!sesstion_map) {
    //         session_id = business_logic::create_session_map();
    //         need_set_cookie = true;
    //     }
    // }
    next();
    if (need_set_cookie) {
      session->response.cookie["session"] = session_id;
    }
  };

private:
  std::string generate_uuid() {
    boost::uuids::random_generator gen;
    boost::uuids::uuid uuid = gen();
    return boost::uuids::to_string(uuid);
  }
};
