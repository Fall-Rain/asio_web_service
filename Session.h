//
// Created by fallrain on 2023/4/20.
//

#ifndef ASIO_DEMO_SESSION_H

#include "boost/asio.hpp"
#include "boost/regex.hpp"
#include "iostream"
#include <boost/asio/ip/tcp.hpp>
#include "boost/algorithm/string.hpp"
#include "business_logic.h"
#include "http_struct.h"
#include "numeric"
#include "boost/beast.hpp"
#include "cctype"

#define ASIO_DEMO_SESSION_H

class Session : public std::enable_shared_from_this<Session> {
public:

    Session(boost::asio::ip::tcp::socket &socket);

    void start();


private:
    void do_read();

    void do_write();

    void process_params();

    void process_headers();

    void process_content_type();

    http_request_struct request;

    http_response_struct response;

    boost::asio::ip::tcp::socket client_socket_;
    boost::asio::streambuf client_buffer_;
//
//    std::map<std::string, std::string> headers;
//    std::string request_body, response_body, method, uri, http_version;
};


#endif //ASIO_DEMO_SESSION_H
