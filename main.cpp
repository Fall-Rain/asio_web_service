#include "web/Server.h"
//#include "entity/user.h"
#include "web/result.h"
#include "dao.h"

int main() {
    business_logic::register_handle(HttpMethod::POST, "/login",
                                    [](const http_request_struct &request) -> http_response_struct {
                                        std::stringstream stringstream;
                                        auto username = request.ptree.get<std::string>("username");
                                        auto password = request.ptree.get<std::string>("password");
                                        auto user = dao::getone(username);
                                        if (!user) {
                                            return {
                                                result("用户名密码错误", 401).to_json_string(),
                                                ContentType::APPLICATION_JSON
                                            };
                                        }
                                        if (user->password == password) {
                                            // business_logic::get_session_map(request.session_id)["username"] = user->
                                            // username;
                                            business_logic::get_session_map(request.session_id)->get()["username"] =
                                                    user->username;
                                            return {
                                                result("登录成功", user->to_json(), 200).to_json_string(),
                                                ContentType::APPLICATION_JSON
                                            };
                                        } else {
                                            return {
                                                result("用户名密码错误", 401).to_json_string(),
                                                ContentType::APPLICATION_JSON
                                            };
                                        }
                                    });
    business_logic::register_handle(HttpMethod::POST, "/body",
                                    [](const http_request_struct &request) -> http_response_struct {
                                        auto session = business_logic::get_session_map(
                                            request.session_id);
                                        if (session->get().find("username") == session->get().end()) {
                                            return {
                                                result("用户未登录", 401).to_json_string(),
                                                ContentType::APPLICATION_JSON
                                            };
                                        }
                                        std::cout << "当前登录用户" << session->get()["username"];
                                        return {
                                            result("获取成功", dao::getList(), 200).to_json_string(),
                                            ContentType::APPLICATION_JSON
                                        };
                                    });

    business_logic::set_root("/var/www/html");
    boost::asio::io_context io_context;
    Server server(io_context, 8090);
    server.start();
    io_context.run();
    server.stop();
}
