
#include "Server.h"
#include "user.h"

int main() {
    business_logic::register_handle(HttpMethod::POST, "/login",
                                    [](http_request_struct request) -> http_response_struct {
                                        http_response_struct response;
                                        response.content_type = ContentType::APPLICATION_JSON;
                                        std::stringstream stringstream;
                                        std::string username = request.ptree.get<std::string>("username");
                                        std::string password = request.ptree.get<std::string>("password");
                                        boost::property_tree::ptree ptree;
                                        if (username == "admin" && password == "admin") {
                                            business_logic::get_session_map(
                                                    request.session_id)["username"] = "管理员";

                                            ptree.put("message", "登录成功");
                                            ptree.put("username", "管理员");
                                            ptree.put("code", 200);
                                        } else {
                                            ptree.put("message", "用户名密码错误");
                                            ptree.put("code", 401);
                                        }
                                        boost::property_tree::write_json(stringstream, ptree);
                                        response.body = stringstream.str();
                                        return response;
                                    });
    business_logic::register_handle(HttpMethod::POST, "/body", [](http_request_struct request) -> http_response_struct {
        std::map<std::string, std::string> session = business_logic::get_session_map(request.session_id);
        if (session.find("username") == session.end()) {
            http_response_struct response;
            boost::property_tree::ptree ptree;
            ptree.put("code", 401);
            ptree.put("message", "用户未登录");
            std::stringstream stringstream;
            boost::property_tree::write_json(stringstream, ptree);
            return response;
        }
        http_response_struct response;
        response.content_type = ContentType::APPLICATION_JSON;
        boost::property_tree::ptree ptree, childen;
        std::vector<user> userVec = {
                {"0", "admin", "admin"},
                {"1", "user1", "user1"},
                {"2", "user2", "user2"},
                {"3", "user3", "user3"},
                {"4", "user4", "user4"}
        };
        for (auto &item: userVec) {
            childen.push_back(std::make_pair("", item.to_json()));
        }
        ptree.add_child("userList", childen);
        std::stringstream stringstream;
        boost::property_tree::write_json(stringstream, ptree);
        response.body = stringstream.str();
        return response;
    });

    boost::asio::io_context io_context;
    Server server(io_context, 8090);
    server.start();
    io_context.run();
    server.stop();
}
