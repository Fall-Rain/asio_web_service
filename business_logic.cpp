//
// Created by fallrain on 2023/5/18.
//
#include "business_logic.h"

std::map<std::string, FunctionPtr> business_logic::function_map;

void business_logic::register_handle(std::string handle_name, FunctionPtr function) {
    function_map[handle_name] = function;
}

http_response_struct business_logic::process_request(http_request_struct request) {
    auto func = function_map.find(request.uri);
    if (func != function_map.end()) {
        return func->second(request);
    } else {
        http_response_struct response;
        response.http_status = HttpStatusCode::NOT_FOUND;
        return response;
    }
}

