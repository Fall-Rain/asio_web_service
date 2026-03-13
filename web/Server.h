//
// Created by fallrain on 2023/4/20.
//

#ifndef ASIO_DEMO_SERVER_H

#include "connection.h"
#include "ThreadPool.h"

#define ASIO_DEMO_SERVER_H


class Server {
public:
    // Server(boost::asio::io_context &io_context, std::uint16_t port, route &route);

    Server(std::uint16_t port, routers &route);

    void start();

    void stop();

private:
    void do_accept();
    std::shared_ptr<boost::asio::io_context> io_context_;
    boost::asio::ip::tcp::acceptor acceptor_;
    ThreadPool pool;
    routers &route_;
};


#endif //ASIO_DEMO_SERVER_H
