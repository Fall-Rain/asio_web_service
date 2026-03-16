#include "dao.h"
#include "web/Server.h"
#include "web/result.h"
#include "web/routers.h"
#include  <iostream>

#include "websocket_manager.h"
#include "web/session_manage.h"

int main() {
    routers routers;

    routers.post("/login", [](const std::shared_ptr<connection> &session) {
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

    routers.post("/body", [](const std::shared_ptr<connection> &session) {
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

    routers.post("/sendBroadcastMessage", [](const std::shared_ptr<connection> &connection) {
        auto message = connection->request.ptree.get<std::string>("message");
        websocket_manager::instance().broadcast(message, connection->request.session_id);
        connection->response = {
            result("发送成功", 200).to_json_string(),
            ContentType::APPLICATION_JSON
        };
    });

    routers.post("/sendMessage", [](const std::shared_ptr<connection> &connection) {
        auto message = connection->request.ptree.get<std::string>("message");
        websocket_manager::instance().send(connection->request.session_id, message);
        connection->response = {
            result("发送成功",200).to_json_string(),
            ContentType::APPLICATION_JSON
        };
    });

    routers.ws("/chat", [](const std::shared_ptr<websocket_connection> &ws) {
        ws->on_message([ws](const std::string &message) {
            std::cout << "收到消息：" << message << std::endl;
            ws->send_text(message);
        });
        ws->on_handshake([ws](const http_request_struct &http_request_struct) {
            auto it = ws->http_session_->find("username");
            if (it == ws->http_session_->end()) {
                ws->send_close();
                return;
            }
            websocket_manager::instance().add(ws->http_request_.session_id, ws);
            ws->send_text("欢迎" + it->second + "登录websocket");
        });

        ws->on_close([ws] {
            websocket_manager::instance().remove(ws->http_request_.session_id);
            ws->send_text("再见");
            std::cout << "连接断开" << std::endl;
        });
    });

    Server server(8090, routers);
    server.start();
    server.stop();
}
