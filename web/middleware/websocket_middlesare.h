//
// Created by fallrain on 2026/3/4.
//

#ifndef ASIO_DEMO_WEBSOCKET_MIDDLESARE_H
#define ASIO_DEMO_WEBSOCKET_MIDDLESARE_H
#include "middleware.h"
#include "../http_struct.h"

class websocket_middlesare : public middleware {
public:
    void handle(std::shared_ptr<Session> session, std::function<void()> next) override;

private:
    bool is_websocket_request(const http_request_struct request);

    std::string sha1(const std::string &input);

    std::string base64_encode(const std::string &input);

    std::string magic = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
};


#endif //ASIO_DEMO_WEBSOCKET_MIDDLESARE_H
