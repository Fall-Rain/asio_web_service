//
// Created by fallrain on 2023/6/8.
//

#include "user.h"

user::user(std::string id, std::string username, std::string password, std::string name) : id(id), username(username),
                                                                                           password(password),
                                                                                           name(name) {}

boost::property_tree::ptree user::to_json() {
    boost::property_tree::ptree ptree;

    ptree.put("id", id);
    ptree.put("username", username);
    ptree.put("password", "******");
    ptree.put("name", name);
    return ptree;
}