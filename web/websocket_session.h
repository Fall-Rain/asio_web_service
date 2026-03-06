//
// Created by fallrain on 2026/3/4.
//

#ifndef ASIO_DEMO_WEBSOCKET_SESSION_H
#define ASIO_DEMO_WEBSOCKET_SESSION_H
#include <deque>
#include <memory>
#include <boost/asio/streambuf.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <vector>
#include "websocket_struct.h"


class websocket_session : public std::enable_shared_from_this<websocket_session> {
public:
    websocket_session(boost::asio::ip::tcp::socket &socket);

    void start();

    void send_text(const std::string &msg);

    void send_binary(const std::vector<uint8_t> &data);

    void send_close(uint16_t code = 1000);

private:
    boost::asio::ip::tcp::socket socket_;

    std::vector<uint8_t> buffer_;
    std::vector<uint8_t> payload_;
    std::vector<uint8_t> mask_key_;
    std::deque<std::vector<uint8_t> > write_queue_;


    uint8_t fin_ = 0;
    websocket_opcode opcode_ = websocket_opcode::text;
    bool mask_ = false;
    uint64_t payload_length_ = 0;


    void on_read_frame();

    void handler_header();

    void read_mask();

    void read_extended_length();

    void read_payload();

    void send_frame(websocket_opcode opencode, const std::vector<uint8_t> &payload);

    void dispatch(websocket_opcode opcode);

    void do_write();
};


#endif //ASIO_DEMO_WEBSOCKET_SESSION_H
