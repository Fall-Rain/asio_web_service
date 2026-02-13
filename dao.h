//
// Created by fallrain on 2024/5/31.
//

#ifndef ASIO_DEMO_DAO_H

#include "string"
#include "entity/user.h"
#include "vector"
#include "optional"

#define ASIO_DEMO_DAO_H


class dao {
public:
    static std::optional<user> getone(const std::string &username);

    static boost::property_tree::ptree getList();

    static void insertUser(const user &user);

    static void deleteUser(std::string id);

private:
    static std::vector<user> userVector;
};


#endif //ASIO_DEMO_DAO_H
