
#include "Server.h"

int main() {
    business_logic::register_handle(HttpMethod::POST, "/json", [](http_request_struct request) -> http_response_struct {
        http_response_struct response;
        response.content_type = ContentType::APPLICATION_JSON;
        std::stringstream stringstream;
        std::string username = request.ptree.get<std::string>("username");
        std::string password = request.ptree.get<std::string>("password");
        boost::property_tree::ptree ptree;
        if (username == "admin" && password == "admin") {
            ptree.put("message", "登录成功");
        } else {
            ptree.put("message", "登录失败");
        }
        boost::property_tree::write_json(stringstream, ptree);
        response.body = stringstream.str();
        return response;
    });

    business_logic::register_handle(HttpMethod::POST, "/xml", [](http_request_struct request) -> http_response_struct {
        http_response_struct response;
        response.content_type = ContentType::APPLICATION_XML;
        std::stringstream stringstream;
        std::string username = request.ptree.get<std::string>("root.username");
        std::string password = request.ptree.get<std::string>("root.password");
        boost::property_tree::ptree ptree;
        if (username == "admin" && password == "admin") {
            ptree.put("message", "登录成功");
        } else {
            ptree.put("message", "登录失败");
        }
        boost::property_tree::write_xml(stringstream, ptree);
        response.body = stringstream.str();
        return response;
    });

    business_logic::register_handle(HttpMethod::POST, "/form", [](http_request_struct request) -> http_response_struct {
        http_response_struct response;
        response.content_type = ContentType::APPLICATION_JSON;
        boost::property_tree::ptree ptree;
        for (auto &item: request.form_data) {
            ptree.put(item.first, item.second);
        }
        std::stringstream stringstream;
        boost::property_tree::write_json(stringstream, ptree);
        response.body = stringstream.str();
        return response;
    });
    business_logic::register_handle(HttpMethod::POST, "/body", [](http_request_struct request) -> http_response_struct {
        http_response_struct response;
        response.content_type = ContentType::APPLICATION_JSON;
        boost::property_tree::ptree ptree;
        for (auto &item: request.params) {
            ptree.put(item.first, item.second);
        }
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
