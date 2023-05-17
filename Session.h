//
// Created by fallrain on 2023/4/20.
//

#ifndef ASIO_DEMO_SESSION_H

#include "boost/asio.hpp"
#include "boost/regex.hpp"
#include "iostream"
#include <boost/asio/ip/tcp.hpp>
#include "boost/algorithm/string.hpp"

#define ASIO_DEMO_SESSION_H


class Session : public std::enable_shared_from_this<Session> {
public:

    Session(boost::asio::ip::tcp::socket &socket);

    void start();

private:
    void do_read();

    void do_write();

    boost::asio::ip::tcp::socket client_socket_;
    boost::asio::streambuf client_buffer_;
    std::map<std::string, std::string> header_map;
    std::string request_body;
};


#endif //ASIO_DEMO_SESSION_H
