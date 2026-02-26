//
// Created by fallrain on 2026/2/25.
//

#include "middleware.h"
#include "../Session.h"
typedef std::string string;

class process_params_middleware : public middleware {
public:
    void handle(std::shared_ptr<Session> session, std::function<void()> next) override {
        size_t pos = session->request.uri.find("?"); // 查找 URI 中的参数部分起始位置
        if (pos == std::string::npos) {
            // 如果没有找到参数部分，直接返回
            next();
            return;
        }
        string query_string = session->request.uri.substr(pos + 1); // 提取参数部分
        std::vector<string> query_line; // 存储每个参数的键值对
        boost::split(query_line, query_string, boost::is_any_of("&")); // 使用 '&' 分割参数
        session->request.params = std::accumulate(query_line.begin(), // 将参数键值对添加到 params 映射中
                                                  query_line.end(),
                                                  std::map<string, string>(),
                                                  [](std::map<std::string, std::string> &result, const string line) {
                                                      std::vector<string> params; // 存储每个参数的键值对
                                                      boost::split(params, line, boost::is_any_of("=")); // 使用 '=' 分割键值对
                                                      if (params.size() == 2) {
                                                          // 如果键值对的大小为 2
                                                          result[params[0]] = params[1]; // 将键值对添加到 params 映射中
                                                      }
                                                      return result;
                                                  });
        next();
    }
};
