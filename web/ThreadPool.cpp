//
// Created by fallrain on 2023/4/20.
//

#include "ThreadPool.h"

ThreadPool::ThreadPool(std::size_t size, boost::asio::io_context &io_context) : io_context_(io_context),
    work_(boost::asio::make_work_guard(io_context)) {
    for (std::size_t i = 0; i < size; ++i) {
        threads_.emplace_back([this] { io_context_.run(); });
    }
}


void ThreadPool::stop() {
    for (auto &thread: threads_) {
        if (thread.joinable()) {
            thread.join();
        }
    }
}
