//
// Created by fallrain on 2026/2/26.
//
#include "../Session.h"
#include "middleware.h"

typedef std::string string;

class cookie_middleware : public middleware {
public:
    void handle(std::shared_ptr<Session> session, std::function<void()> next) override {
        auto header_cookie = session->request.headers.find("Cookie");
        // 解析Cookie头部
        if (header_cookie != session->request.headers.end()) {
            std::vector<string> cookie_vec;
            boost::split(cookie_vec, header_cookie->second, boost::is_any_of(";"),
                         boost::token_compress_on);
            std::for_each(cookie_vec.begin(), cookie_vec.end(),
                          [&](const auto &item) {
                              std::vector<string> cookie;
                              boost::split(cookie, item, boost::is_any_of("="),
                                           boost::token_compress_on);
                              // 填充Cookie到cookie_map映射
                              session->request.cookie[cookie[0]] = cookie[1];
                          });
        }
        next();

        // session->response.cookie

        // 如果Cookie不为空，构建Set-Cookie头部
        if (!session->response.cookie.empty()) {
            std::ostringstream ostringstream;
            // ostringstream << "httpOnly;" << "Path=" << session->request.uri << ";";
            for (const auto &item: session->response.cookie) {
                ostringstream << item.first << "=" << item.second << ";";
            }
            session->response.headers["Set-Cookie"] = ostringstream.str();
        }
    }
};
