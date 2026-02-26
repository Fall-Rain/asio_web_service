//
// Created by fallrain on 2023/4/20.
//

#include "Server.h"
#include "middleware/cookie_middleware.h"

Server::Server(boost::asio::io_context &io_context, std::uint16_t port) : acceptor_(io_context,
                                                                              boost::asio::ip::tcp::endpoint(
                                                                                  boost::asio::ip::tcp::v4(), port)),
                                                                          pool(std::thread::hardware_concurrency(),
                                                                               io_context) {
}

void Server::start() {
    do_accept();
}

void Server::stop() {
    acceptor_.close();
    pool.stop();
}

void Server::do_accept() {
    acceptor_.async_accept(
        [this](std::error_code ec, boost::asio::ip::tcp::socket socket) {
            if (!ec) {
                pool.post([session = std::make_shared<Session>(socket)] {
                    session->start();
                });
            }
            do_accept();
        });
}
