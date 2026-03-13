//
// Created by fallrain on 2026/3/11.
//

#include "websocket_manager.h"

websocket_manager &websocket_manager::instance() {
    static websocket_manager instance_;
    return instance_;
}

void websocket_manager::add(const std::string &id, std::shared_ptr<websocket_connection> connection) {
    std::lock_guard lock(mutex_);
    connections_[id] = connection;
}

void websocket_manager::remove(const std::string &id) {
    std::lock_guard lock(mutex_);
    connections_.erase(id);
}

std::shared_ptr<websocket_connection> websocket_manager::get(const std::string &id) {
    std::lock_guard lock(mutex_);
    auto it = connections_.find(id);
    if (it == connections_.end()) {
        return nullptr;
    }
    return it->second.lock();
}

void websocket_manager::send(std::string &id, const std::string &msg) {
    auto connection = get(id);
    if (connection) {
        connection->send_text(msg);
    }
}



void websocket_manager::broadcast(const std::string &msg, const std::string &id) {
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto &it: connections_) {
        if (it.first != id) {
            it.second.lock()->send_text(msg);
        }
    }
}
