#include "dao.h"
#include "web/Server.h"
#include "web/result.h"
#include "web/route.h"
#include  <iostream>

#include "web/http_session_manage.h"

int main() {
    route route;

    route.post("/login", [](std::shared_ptr<Session> session) {
        auto username = session->request.ptree.get<std::string>("username");
        auto password = session->request.ptree.get<std::string>("password");
        auto user = dao::getone(username);
        if (!user) {
            session->response = {
                result("用户名密码错误", 401).to_json_string(),
                ContentType::APPLICATION_JSON
            };
            return;
        }
        if (user->password != password) {
            session->response = {
                result("用户名密码错误", 401).to_json_string(),
                ContentType::APPLICATION_JSON
            };
            return;
        }

        session->http_session->operator[]("username") = user->username;
        session->response = {
            result("登录成功", user->to_json(), 200).to_json_string(),
            ContentType::APPLICATION_JSON
        };
    });

    route.post("/body", [](std::shared_ptr<Session> session) {
        auto http_session = session->http_session->find("username");
        if (http_session == session->http_session->end()) {
            session->response = {
                result("用户未登录", 401).to_json_string(),
                ContentType::APPLICATION_JSON
            };
            return;
        }

        session->response = {
            result("获取成功", dao::getList(), 200).to_json_string(),
            ContentType::APPLICATION_JSON
        };
    });


    route.ws("/chat", [](std::shared_ptr<websocket_session> ws) {
        ws->on_message([ws](const std::string &message) {
            std::cout << "收到消息：" << message << std::endl;
            ws->send_text(message);
        });
        ws->on_handshake([ws](http_request_struct http_request_struct) {
            ws->send_text("欢迎" + ws->http_session_->find("username")->second + "登录websocket");
        });

        ws->on_close([ws] {
            ws->send_text("再见");
            std::cout << "连接断开" << std::endl;
        });
    });

    boost::asio::io_context io_context;
    Server server(io_context, 8090, route);
    server.start();
    io_context.run();
    server.stop();
}
