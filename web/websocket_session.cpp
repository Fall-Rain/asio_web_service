//
// Created by fallrain on 2026/3/4.
//

#include "websocket_session.h"

#include <boost/asio/read.hpp>
#include <boost/beast/http/message.hpp>
#include <utility>

#include "Server.h"
#include "iostream"

// websocket_session::websocket_session(boost::asio::ip::tcp::socket &socket, http_request_struct http_request_struct)
//     : socket_(std::move(socket)) {
//     on_handshake_(http_request_struct);
// }



websocket_session::websocket_session(boost::asio::ip::tcp::socket socket, http_request_struct http_request_struct,
                                     websocket_handler websocket_handler,
                                     std::shared_ptr<std::unordered_map<std::string, std::string> > http_session)
    : socket_(std::move(socket)),
      websocket_handler_(std::move(websocket_handler)),
      http_request_(std::move(http_request_struct)),
      http_session_(http_session) {
}

void websocket_session::start() {
    websocket_handler_(shared_from_this());
    if (on_handshake_) {
        on_handshake_(http_request_);
    }
    on_read_frame();
}

void websocket_session::on_message(message_callback message_callback) {
    on_message_ = std::move(message_callback);
}

void websocket_session::on_handshake(handshake_callback handshake_callback) {
    on_handshake_ = std::move(handshake_callback);
}

void websocket_session::on_close(close_callback close_callback) {
    on_close_ = close_callback;
}

void websocket_session::send_text(const std::string &msg) {
    std::vector<uint8_t> data(msg.begin(), msg.end());
    send_frame(websocket_opcode::text, data);
}

void websocket_session::send_binary(const std::vector<uint8_t> &data) {
    send_frame(websocket_opcode::binary, data);
}

void websocket_session::send_close(uint16_t code) {
    std::vector<uint8_t> payload;
    payload.push_back((code >> 8) & 0xFF);
    payload.push_back(code & 0xFF);
    send_frame(websocket_opcode::close, payload);
}

void websocket_session::on_read_frame() {
    buffer_.resize(2);
    boost::asio::async_read(
        socket_, boost::asio::buffer(buffer_),
        [self = shared_from_this()](boost::system::error_code ec,
                                    std::size_t length) {
            if (ec) {
                return;
            }
            self->handler_header();
        });
}

void websocket_session::handler_header() {
    fin_ = (buffer_[0] & 0x80) >> 7;
    opcode_ = static_cast<websocket_opcode>(buffer_[0] & 0x0f);
    mask_ = (buffer_[1] & 0x80) >> 7;
    payload_length_ = buffer_[1] & 0x7f;

    if (payload_length_ <= 125) {
        read_mask();
    } else {
        read_extended_length();
    }
}

void websocket_session::read_mask() {
    if (!mask_) {
        read_payload();
        return;
    }
    mask_key_.resize(4);
    boost::asio::async_read(
        socket_, boost::asio::buffer(mask_key_),
        [self = shared_from_this()](boost::system::error_code ec, size_t size) {
            if (ec) {
                return;
            }
            self->read_payload();
        });
}

void websocket_session::read_payload() {
    payload_.resize(payload_length_);
    boost::asio::async_read(
        socket_, boost::asio::buffer(payload_),
        [self = shared_from_this()](boost::system::error_code ec, std::size_t) {
            if (ec) {
                return;
            }
            if (self->mask_) {
                for (uint64_t i = 0; i < self->payload_length_; ++i) {
                    self->payload_[i] ^= self->mask_key_[i % 4];
                }
            }

            std::cout << self->payload_length_ << std::endl;
            self->dispatch(self->opcode_);
            self->on_read_frame();
        });
}

void websocket_session::read_extended_length() {
    size_t ext_bytes = (payload_length_ == 126) ? 2 : 8;
    buffer_.resize(ext_bytes);
    boost::asio::async_read(socket_, boost::asio::buffer(buffer_),
                            [self = shared_from_this(), ext_bytes](
                        boost::system::error_code ec, std::size_t) {
                                if (ec) {
                                    return;
                                }
                                self->payload_length_ = 0;
                                for (size_t i = 0; i < ext_bytes; ++i) {
                                    self->payload_length_ =
                                            (self->payload_length_ << 8) |
                                            self->buffer_[i];
                                }
                                self->read_mask();
                            });
}

void websocket_session::send_frame(websocket_opcode opcode,
                                   const std::vector<uint8_t> &payload) {
    std::vector<uint8_t> frame;
    uint8_t fin_opencode = 0x80 | static_cast<uint8_t>(opcode);
    frame.push_back(fin_opencode);
    uint64_t len = payload.size();
    if (len <= 125) {
        frame.push_back(static_cast<uint8_t>(len));
    } else if (len <= 0xffff) {
        frame.push_back(126);
        frame.push_back((len >> 8) & 0xff);
        frame.push_back(len & 0xff);
    } else {
        frame.push_back(127);
        for (int i = 7; i >= 0; --i) {
            frame.push_back((len >> (8 * i)) & 0xFF);
        }
    }
    frame.insert(frame.end(), payload.begin(), payload.end());

    bool writing = !write_queue_.empty();
    write_queue_.push_back(std::move(frame));
    if (!writing) {
        do_write();
    }
}

void websocket_session::dispatch(websocket_opcode opcode) {
    switch (opcode) {
        case websocket_opcode::text: {
            std::string message(payload_.begin(), payload_.end());
            on_message_(message);
            break;
        }
        case websocket_opcode::binary:
            break;
        case websocket_opcode::ping:
            send_frame(websocket_opcode::pong, payload_);
            break;
        case websocket_opcode::close:
            send_close();
            on_close_();
            socket_.close();
            break;
        default:
            break;
    }
}

void websocket_session::do_write() {
    boost::asio::async_write(
        socket_, boost::asio::buffer(write_queue_.front()),
        [self = shared_from_this()](boost::system::error_code ec, std::size_t) {
            if (ec) {
                return;
            }
            self->write_queue_.pop_front();
            if (!self->write_queue_.empty())
                self->do_write();
        });
}
