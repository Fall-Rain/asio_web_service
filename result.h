//
// Created by fallrain on 2023/6/15.
//

#ifndef ASIO_DEMO_RESULT_H

#include <variant>
#include "string"
#include "boost/property_tree/json_parser.hpp"

#define ASIO_DEMO_RESULT_H


class result {
public:
    result(std::string message, std::variant<boost::property_tree::ptree, std::string, std::nullptr_t> body, int code);

    result(std::string message, int code);

    result();

    std::string message;
    std::variant<boost::property_tree::ptree, std::string, std::nullptr_t> body;
//    boost::property_tree::ptree body;
    int code;

    boost::property_tree::ptree to_json();

    std::string to_json_string();
};


#endif //ASIO_DEMO_RESULT_H
