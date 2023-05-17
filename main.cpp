#include "Server.h"

using boost::asio::ip::tcp;

int main() {
    boost::asio::io_context io_context;
    Server server(io_context, 8090);
    server.start();
    io_context.run();
    server.stop();
}

//#include <iostream>
//#include <boost/asio.hpp>
//#include <boost/asio/ssl.hpp>
//
//
//void do_write(boost::asio::ip::tcp::socket &socket) {
//    std::string response = "HTTP/1.1 200 OK\r\n"
//                           "Content-Type: text/html\r\n"
//                           "Content-Length: 12\r\n"
//                           "\r\n"
//                           "Hello, World!";
//    boost::asio::async_write(socket, boost::asio::buffer(response),
//                             [](const boost::system::error_code &error,
//                                std::size_t /*bytes_transferred*/) {
//                                 if (!error) {
//                                     std::cout << "Response sent successfully."
//                                               << std::endl;
//                                 } else {
//                                     std::cout << "Error writing response: "
//                                               << error.message() << std::endl;
//                                 }
//                             });
//}
//
//void do_read(boost::asio::ip::tcp::socket &socket) {
//    boost::asio::streambuf buffer;
//    boost::asio::async_read_until(socket, buffer, "\r\n\r\n",
//                                  [&](boost::system::error_code error,
//                                      std::size_t length) {
//                                      if (!error) {
////                                          std::string body = boost::asio::buffer_cast<const char *>(buffer.data());
////                                      std::cout << std::string(boost::asio::buffers_begin(buffer.data()),
////                                                               boost::asio::buffers_end(buffer.data()));
////                                          std::cout
////                                                  << body << std::endl
////                                                  << length << std::endl
////                                                  << buffer.size() << std::endl;
//                                          do_write(socket);
//                                      } else {
//                                          std::cout << "Error reading response: "
//                                                    << error.message() << std::endl;
//                                      }
//                                  });
//}
//
//void handleConnection(boost::asio::ip::tcp::socket &socket) {
//    do_read(socket);
//}
//
//void startAccept(boost::asio::ip::tcp::acceptor &acceptor) {
//    acceptor.async_accept([&acceptor](const boost::system::error_code &error, boost::asio::ip::tcp::socket socket) {
//        if (!error) {
////            std::cout << "Accepted new connection." << std::endl;
//            handleConnection(socket);
//        } else {
//            std::cout << "Error accepting connection: " << error.message() << std::endl;
//        }
//
//        startAccept(acceptor); // 继续接受下一个连接
//    });
//}
//
//// 在主函数中启动接受连接
//int main() {
//    boost::asio::io_context ioContext;
//    boost::asio::ip::tcp::acceptor acceptor(ioContext);
//
//    boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), 1234);
//    acceptor.open(endpoint.protocol());
//    acceptor.bind(endpoint);
//    acceptor.listen();
//
//    startAccept(acceptor);
//
//    ioContext.run();
//
//    return 0;
//}
