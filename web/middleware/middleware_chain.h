//
// Created by fallrain on 2026/3/9.
//

#ifndef ASIO_DEMO_MIDDLEWARE_CHAIN_H
#define ASIO_DEMO_MIDDLEWARE_CHAIN_H
#include <boost/type.hpp>

#include "../router.h"

template<typename... middlewares>
class middleware_chain;

template<typename First, typename... Rest>
class middleware_chain<First, Rest...> {
public:
    template<typename Session>
    static void run(Session session) {
        First middleware;
        middleware.handle(session, [session]() {
            middleware_chain<Rest...>::run(session);
        });
    }
};

template<>
class middleware_chain<> {
public:
    static void run(std::shared_ptr<connection> session) {
        try {
            session->route_.handle_request(session);
            // session->response = business_logic::process_request(session->request);
        } catch (const std::exception &e) {
            session->response.body = e.what();
        }
    }
};

#endif //ASIO_DEMO_MIDDLEWARE_CHAIN_H
