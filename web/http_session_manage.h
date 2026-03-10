//
// Created by fallrain on 2026/3/10.
//

#ifndef ASIO_DEMO_HTTP_SESSION_MANAGE_H
#define ASIO_DEMO_HTTP_SESSION_MANAGE_H
#include <mutex>
#include <string>
#include <unordered_map>

class http_session_manage {
public:
  static http_session_manage &instance() {
    static http_session_manage instance_;
    return instance_;
  }

  std::unordered_map<std::string, std::string> &
  get_session(const std::string &session_id) {
    std::lock_guard lock(mutex_);
    return sessions_[session_id];
  }

  void remove_session(const std::string &session_id) {
    std::lock_guard lock(mutex_);
    sessions_.erase(session_id);
  }

private:
  std::unordered_map<std::string, std::unordered_map<std::string, std::string>>
      sessions_;
  http_session_manage() = default;
  std::mutex mutex_;
};

#endif // ASIO_DEMO_HTTP_SESSION_MANAGE_H
