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
#include "boost/uuid/uuid.hpp"
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <filesystem>

typedef std::function<http_response_struct(http_request_struct)> FunctionPtr;

class business_logic {
public:

    static void register_handle(HttpMethod httpMethod, const std::string &handle_name, FunctionPtr function);

    static http_response_struct process_request(const http_request_struct &request);

    static std::string create_session_map();

    static std::map <std::string, std::string> &get_session_map(const std::string &cookie_id);

    static void set_root(std::string root);

private:

    static std::string root;
    static std::map <std::string, std::map<std::string, std::string >> session_map;

    static std::map <HttpMethod, std::map<std::string, FunctionPtr>> function_map;
};


#endif //ASIO_DEMO_BUSINESS_LOGIC_H
