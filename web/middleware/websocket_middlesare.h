//
// Created by fallrain on 2026/3/4.
//

#ifndef ASIO_DEMO_WEBSOCKET_MIDDLESARE_H
#define ASIO_DEMO_WEBSOCKET_MIDDLESARE_H
#include <openssl/evp.h>
#include <boost/algorithm/string/case_conv.hpp>
#include <openssl/sha.h>
#include "../http_struct.h"

class websocket_middlesare {
public:
    void handle(std::shared_ptr<Session> session, std::function<void()> next) {
        if (!is_websocket_request(session->request)) {
            next();
            return;
        }
        auto secWebsocketKey = session->request.headers.find("Sec-WebSocket-Key")->second;
        session->response.http_status = HttpStatusCode::SWITCHING_PROTOCOLS;
        session->response.headers["Upgrade"] = "websocket";
        session->response.headers["Connection"] = "Upgrade";
        session->response.headers["Sec-WebSocket-Accept"] = base64_encode(sha1(secWebsocketKey + magic));
    };

private:
    bool is_websocket_request(const http_request_struct request) {
        if (request.method != HttpMethod::GET) {
            return false;
        }
        if (request.headers.find("Upgrade") == request.headers.end()
            || request.headers.find("Connection") == request.headers.end()
            || request.headers.find("Sec-WebSocket-Key") == request.headers.end()
            || request.headers.find("Sec-WebSocket-Version") == request.headers.end()) {
            return false;
        }
        auto upgrade = request.headers.find("Upgrade")->second;
        auto connection = request.headers.find("Connection")->second;
        auto version = request.headers.find("Sec-WebSocket-Version")->second;
        boost::to_lower(upgrade);
        if (upgrade != "websocket") {
            return false;
        }
        if (connection.find("Upgrade") == std::string::npos) {
            return false;
        }
        if (version != "13") {
            return false;
        }
        return true;
    }

    std::string sha1(const std::string &input) {
        unsigned char hash[SHA_DIGEST_LENGTH];

        SHA1(reinterpret_cast<const unsigned char *>(input.c_str()),
             input.size(),
             hash);

        return std::string(reinterpret_cast<char *>(hash),
                           SHA_DIGEST_LENGTH);
    }

    std::string base64_encode(const std::string &input) {
        int len = 4 * ((input.size() + 2) / 3);
        std::string output(len, '\0');

        int out_len = EVP_EncodeBlock(
            reinterpret_cast<unsigned char *>(&output[0]),
            reinterpret_cast<const unsigned char *>(input.data()),
            input.size());

        output.resize(out_len);
        return output;
    }

    std::string magic = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
};


#endif //ASIO_DEMO_WEBSOCKET_MIDDLESARE_H
