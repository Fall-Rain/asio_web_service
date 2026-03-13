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
#include "websocket_frame.h"
#include "http_protocol.h"

class websocket_connection : public std::enable_shared_from_this<websocket_connection> {
public:
    using message_callback = std::function<void(std::string)>;

    using handshake_callback = std::function<void(http_request_struct)>;

    using close_callback = std::function<void()>;

    using websocket_handler = std::function<void(std::shared_ptr<websocket_connection>)>;

    websocket_connection(
        boost::asio::ip::tcp::socket socket,
        http_request_struct http_request_struct,
        websocket_handler websocket_handler,
        std::shared_ptr<std::unordered_map<std::string, std::string> > http_session_);

    void start();

    void on_message(message_callback message_callback);

    void on_handshake(handshake_callback handshake_callback);

    void on_close(close_callback close_callback);

    void send_text(const std::string &msg);

    void send_binary(const std::vector<uint8_t> &data);

    void send_close(uint16_t code = 1000);

    std::shared_ptr<std::unordered_map<std::string, std::string> > http_session_;

    http_request_struct http_request_;
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

    message_callback on_message_;
    handshake_callback on_handshake_;
    close_callback on_close_;
    websocket_handler websocket_handler_;
};


#endif //ASIO_DEMO_WEBSOCKET_SESSION_H
