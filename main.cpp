#include "dao.h"
#include "web/Server.h"
#include "web/result.h"
#include "web/route.h"

int main()
{
    route route;

    route.post("/login", [](std::shared_ptr<Session> session)
    {
        auto username = session->request.ptree.get<std::string>("username");
        auto password = session->request.ptree.get<std::string>("password");
        auto user = dao::getone(username);
        if (!user)
        {
            session->response = {
                result("用户名密码错误", 401).to_json_string(),
                ContentType::APPLICATION_JSON
            };
            return;
        }
        if (user->password != password)
        {
            session->response = {
                result("用户名密码错误", 401).to_json_string(),
                ContentType::APPLICATION_JSON
            };
            return;
        }
        (*session->http_session)["username"] = user->username;
        session->response = {
            result("登录成功", user->to_json(), 200).to_json_string(),
            ContentType::APPLICATION_JSON
        };
    });

    route.post("/body", [](std::shared_ptr<Session> session)
    {
        if ((*session->http_session).find("username") ==
            (*session->http_session).end())
        {
            session->response = {
                result("用户未登录", 401).to_json_string(),
                ContentType::APPLICATION_JSON
            };
        }

        session->response = {
            result("获取成功", dao::getList(), 200).to_json_string(),
            ContentType::APPLICATION_JSON
        };
    });

    boost::asio::io_context io_context;
    Server server(io_context, 8090, route);
    server.start();
    io_context.run();
    server.stop();
}
