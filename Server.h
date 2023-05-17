//
// Created by fallrain on 2023/4/20.
//

#ifndef ASIO_DEMO_SERVER_H

#include "Session.h"
#include "ThreadPool.h"

#define ASIO_DEMO_SERVER_H


class Server {
public:
    Server(boost::asio::io_context &io_context, std::uint16_t port);

    void start();

    void stop();

private:

    void do_accept();

    boost::asio::ip::tcp::acceptor acceptor_;
    ThreadPool pool;
};


#endif //ASIO_DEMO_SERVER_H
