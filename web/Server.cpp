//
// Created by fallrain on 2023/4/20.
//

#include "Server.h"
#include "middleware/cookie_middleware.h"


Server::Server(std::uint16_t port, router &route) : io_context_(std::make_shared<boost::asio::io_context>()),
                                                    acceptor_(*io_context_,
                                                              boost::asio::ip::tcp::endpoint(
                                                                  boost::asio::ip::tcp::v4(), port)),
                                                    pool(std::thread::hardware_concurrency(), *io_context_),
                                                    route_(route) {
}

void Server::start() {
    do_accept();
    io_context_->run();
}

void Server::stop() {
    acceptor_.close();
    pool.stop();
    io_context_->stop();
}


void Server::do_accept() {
    acceptor_.async_accept(
        [this](std::error_code ec, boost::asio::ip::tcp::socket socket) {
            if (!ec) {
                pool.post([session = std::make_shared<connection>(std::move(socket), route_)] {
                    session->start();
                });
            }
            do_accept();
        });
}
