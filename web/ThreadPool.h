//
// Created by fallrain on 2023/4/20.
//

#ifndef ASIO_DEMO_THREADPOOL_H

#include "boost/asio.hpp"

#define ASIO_DEMO_THREADPOOL_H


class ThreadPool {
public:
    ThreadPool(std::size_t size, boost::asio::io_context &io_context);

    template<typename T>
    void post(T &&task) {
        boost::asio::post(io_context_, std::forward<T>(task));
    }

    void stop();

private:
    boost::asio::io_context &io_context_;
    boost::asio::executor_work_guard<boost::asio::io_context::executor_type> work_;
    std::vector<std::thread> threads_;
};


#endif //ASIO_DEMO_THREADPOOL_H
