//
// Created by fallrain on 2023/6/8.
//

#ifndef ASIO_DEMO_USER_H

#include "string"
#include "boost/property_tree/json_parser.hpp"

#define ASIO_DEMO_USER_H


class user {
public:
    user(std::string id, std::string username, std::string password, std::string name);

    boost::property_tree::ptree to_json();

    std::string id;
    std::string username;
    std::string password;
    std::string name;
};


#endif //ASIO_DEMO_USER_H
