//
// Created by fallrain on 2023/5/18.
//

#ifndef ASIO_DEMO_BUSINESS_LOGIC_H
#define ASIO_DEMO_BUSINESS_LOGIC_H

#include <functional>
#include "algorithm"
#include "map"
#include "http_struct.h"
#include "boost/property_tree/json_parser.hpp"
#include "boost/property_tree/xml_parser.hpp"

typedef std::function<http_response_struct(http_request_struct)> FunctionPtr;

class business_logic {
public:

    static void register_handle(HttpMethod httpMethod, std::string handle_name, FunctionPtr function);

    static http_response_struct process_request(http_request_struct request);

private:
    static std::map<HttpMethod, std::map<std::string, FunctionPtr>> function_map;
};


#endif //ASIO_DEMO_BUSINESS_LOGIC_H
