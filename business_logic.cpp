//
// Created by fallrain on 2023/5/18.
//
#include "business_logic.h"
std::map<std::string, FunctionPtr> business_logic::function_map;
void business_logic::register_handle(std::string handle_name, FunctionPtr function) {
    function_map[handle_name] = function;
}

FunctionPtr business_logic::process_request(std::string handle_name) {
    auto func = function_map.find(handle_name);
    if (func != function_map.end()) {
        return func->second;
    } else {
        return [](const std::string body, std::map<std::string, std::string> headers) {
            return body;
        };
    }
}
