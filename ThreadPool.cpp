//
// Created by fallrain on 2023/4/20.
//

#include "ThreadPool.h"

ThreadPool::ThreadPool(std::size_t size) : work_(io_context_) {
    for (std::size_t i = 0; i < size; ++i) {
        threads_.emplace_back([this] {
            io_context_.run();
        });
    }
}

void ThreadPool::stop() {
    io_context_.stop();
    for (auto &thread: threads_) {
        thread.join();
    }
}