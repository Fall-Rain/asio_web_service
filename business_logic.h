//
// Created by fallrain on 2023/5/18.
//

#ifndef ASIO_DEMO_BUSINESS_LOGIC_H
#define ASIO_DEMO_BUSINESS_LOGIC_H

#include <functional>
#include "map"


typedef std::function<std::string(const std::string &, std::map<std::string, std::string>)> FunctionPtr;


class business_logic {
public:
    static void register_handle(std::string handle_name, FunctionPtr function);

    static FunctionPtr process_request(std::string handle_name);

private:
    static std::map<std::string, FunctionPtr> function_map;
};



#endif //ASIO_DEMO_BUSINESS_LOGIC_H
