
#include "Server.h"

int main() {
    business_logic::register_handle("/json", [](http_request_struct request) {
        http_response_struct response;
        std::string uri = request.uri;
        uri = uri.substr(1, uri.size() - 1);
        response.body = "{\n  \"username\": \"admin\",\n  \"password\": \"admin\"\n}";
        response.content_type = ContentType::APPLICATION_JSON;
        return response;
    });
    business_logic::register_handle("/body", [](http_request_struct request) {
        http_response_struct response;
        response.body = "body";
        return response;
    });
    business_logic::register_handle("/user1", [](http_request_struct request) {
        http_response_struct response;
        response.body = "user1";
        return response;
    });
    business_logic::register_handle("/user2", [](http_request_struct request) {
        http_response_struct response;
        response.body = "user2";
        return response;
    });
    boost::asio::io_context io_context;
    Server server(io_context, 8090);
    server.start();
    io_context.run();
    server.stop();
}
