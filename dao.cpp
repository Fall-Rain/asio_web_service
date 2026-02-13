//
// Created by fallrain on 2024/5/31.
//

#include "dao.h"

std::vector<user> dao::userVector = {
        {"0", "admin", "admin", "管理员"},
        {"1", "user1", "user1", "用户1"},
        {"2", "user2", "user2", "用户2"},
        {"3", "user3", "user3", "用户3"},
        {"4", "user4", "user4", "用户3"}
};

std::optional<user> dao::getone(const std::string &username) {
    for (const auto &item: userVector) {
        if (item.username == username) {
            return item;
        }
    }
    return std::nullopt;
}

boost::property_tree::ptree dao::getList() {
    boost::property_tree::ptree ptree, childen;
    for (auto &item: userVector) {
        childen.push_back(std::make_pair("", item.to_json()));
    }
    ptree.add_child("userList", childen);
    return ptree;
}

void dao::insertUser(const user &user) {
    userVector.insert(userVector.begin(), user);
}

void dao::deleteUser(std::string id) {
    userVector.erase(std::remove_if(userVector.begin(), userVector.end(), [&id](const user &user) {
        return user.id == id;
    }), userVector.end());
}

