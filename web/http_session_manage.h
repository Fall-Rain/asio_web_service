//
// Created by fallrain on 2026/3/10.
//

#ifndef ASIO_DEMO_HTTP_SESSION_MANAGE_H
#define ASIO_DEMO_HTTP_SESSION_MANAGE_H
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
using http_session_data = std::unordered_map<std::string, std::string>;

using http_session_ptr = std::shared_ptr<http_session_data>;

class http_session_manage {
public:
    static http_session_manage &instance();

    http_session_ptr get_session(const std::string &session_id);

    void remove_session(const std::string &session_id);

private:
    // std::unordered_map<std::string, std::unordered_map<std::string, std::string> >
    // sessions_;

    std::unordered_map<std::string, http_session_ptr> sessions_;


    http_session_manage() = default;

    std::mutex mutex_;
};

#endif // ASIO_DEMO_HTTP_SESSION_MANAGE_H
