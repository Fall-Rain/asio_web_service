#include "Server.h"

using boost::asio::ip::tcp;

int main() {
    boost::asio::io_context io_context;
    Server server(io_context, 8090);
    server.start();
    io_context.run();
    server.stop();
}
