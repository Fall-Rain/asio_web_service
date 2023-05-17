//
// Created by fallrain on 2023/4/20.
//

#include "Server.h"

Server::Server(boost::asio::io_context &io_context, std::uint16_t port) :
        acceptor_(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)),
        pool(std::thread::hardware_concurrency()) {
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
//                    pool.post([&socket] {
//                        std::make_shared<Session>(socket)->start();
//                    });

                    pool.post([session = std::make_shared<Session>(socket)] {
                        session->start();
                    });

//                    std::make_shared<Session>(
//                            static_cast<boost::asio::io_context &>(acceptor_.get_executor().context()))->start(socket);

//                    boost::asio::post(pool,
//                                      [=]() {
//                                          std::make_shared<Session>(
//                                                  static_cast<boost::asio::io_context &>(acceptor_.get_executor().context()))->start(
//                                                  socket);
//                                      },
//                                      std::move(socket));
//                    boost::asio::post(pool, [] {});
                }
                do_accept();
            });
}

