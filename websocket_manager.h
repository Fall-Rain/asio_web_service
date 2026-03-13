//
// Created by fallrain on 2026/3/11.
//

#ifndef ASIO_DEMO_WEBSOCKET_MANAGE_H
#define ASIO_DEMO_WEBSOCKET_MANAGE_H
#include <memory>
#include <mutex>
#include <unordered_map>

#include "web/websocket_connection.h"


class websocket_manager {
public:
    static websocket_manager &instance();

    void add(const std::string &id, std::shared_ptr<websocket_connection> connection);

    void remove(const std::string &id);

    std::shared_ptr<websocket_connection> get(const std::string &id);

    void send(std::string &id, const std::string &msg);

    void broadcast(const std::string &msg, const std::string &id);

private:
    std::mutex mutex_;
    std::unordered_map<std::string, std::weak_ptr<websocket_connection> > connections_;
};


#endif //ASIO_DEMO_WEBSOCKET_MANAGE_H
