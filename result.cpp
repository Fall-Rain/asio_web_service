//
// Created by fallrain on 2023/6/15.
//

#include "result.h"

result::result(std::string message, int code) : message(message), code(code) {}

result::result(std::string message, std::variant<boost::property_tree::ptree, std::string, std::nullptr_t> body,
               int code) : message(message), body(body), code(code) {}

result::result() {}

boost::property_tree::ptree result::to_json() {
    boost::property_tree::ptree ptree;
    ptree.put("message", message);
    if (std::holds_alternative<boost::property_tree::ptree>(body)) {
        ptree.add_child("body", std::get<boost::property_tree::ptree>(body));
    } else if (std::holds_alternative<std::string>(body)) {
        ptree.put("body", std::get<std::string>(body));
    }
    ptree.put("code", code);
    return ptree;
}

std::string result::to_json_string() {
    std::stringstream stringstream;
    boost::property_tree::write_json(stringstream, to_json());
    return stringstream.str();

}