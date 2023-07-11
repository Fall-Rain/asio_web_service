
#include "Server.h"
#include "user.h"
#include "result.h"

int main() {
    std::vector<user> userVec = {
            {"0", "admin", "admin", "管理员"},
            {"1", "user1", "user1", "用户1"},
            {"2", "user2", "user2", "用户2"},
            {"3", "user3", "user3", "用户3"},
            {"4", "user4", "user4", "用户3"}
    };
    business_logic::register_handle(HttpMethod::POST, "/login",
                                    [&userVec](http_request_struct request) -> http_response_struct {
                                        std::stringstream stringstream;
                                        std::string username = request.ptree.get<std::string>("username");
                                        std::string password = request.ptree.get<std::string>("password");
                                        for (auto &item: userVec) {
                                            if (item.username == username && item.password == item.password) {
                                                business_logic::get_session_map(
                                                        request.session_id)["username"] = item.username;
                                                boost::property_tree::ptree ptree;
                                                return http_response_struct(
                                                        result("登录成功", item.to_json(), 200).to_json_string(),
                                                        ContentType::APPLICATION_JSON);
                                            }
                                        }
                                        return http_response_struct(
                                                result("用户名密码错误", 401).to_json_string(),
                                                ContentType::APPLICATION_JSON);
                                    });
    business_logic::register_handle(HttpMethod::POST, "/body",
                                    [&userVec](http_request_struct request) -> http_response_struct {
                                        std::map<std::string, std::string> session = business_logic::get_session_map(
                                                request.session_id);
                                        if (session.find("username") == session.end()) {
                                            return http_response_struct(result("用户未登录", 401).to_json_string(),
                                                                        ContentType::APPLICATION_JSON);
                                        }

                                        boost::property_tree::ptree ptree, childen;
                                        for (auto &item: userVec) {
                                            childen.push_back(std::make_pair("", item.to_json()));
                                        }
                                        ptree.add_child("userList", childen);

                                        return http_response_struct(result("获取成功", ptree, 200).to_json_string(),
                                                                    ContentType::APPLICATION_JSON);
                                    });

    boost::asio::io_context io_context;
    Server server(io_context, 8090);
    server.start();
    io_context.run();
    server.stop();
}
