
#include "Server.h"
int main() {
    business_logic::register_handle("/user", [](const std::string body, std::map<std::string, std::string> headers) {
        return "hello user";
    });
    business_logic::register_handle("/body", [](const std::string body, std::map<std::string, std::string> headers) {
        return "hello body";
    });
    business_logic::register_handle("/user1", [](const std::string body, std::map<std::string, std::string> headers) {
        return "hello user1";
    });
    business_logic::register_handle("/user2", [](const std::string body, std::map<std::string, std::string> headers) {
        return "hello user2";
    });
    boost::asio::io_context io_context;
    Server server(io_context, 8090);
    server.start();
    io_context.run();
    server.stop();
}
