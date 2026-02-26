//
// Created by fallrain on 2026/2/25.
//
#include "../Session.h"
#include "middleware.h"
typedef std::string string;

class process_content_type_middleware : public middleware {
public:
    void handle(std::shared_ptr<Session> session, std::function<void()> next) override {
        string body = session->request.body; // 获取请求体
        if (body.empty()) {
            // 如果请求体为空，则直接返回
            next();
            return;
        }
        std::map<std::string, std::string> headers = session->request.headers; // 获取请求头
        auto content_type = headers.find("Content-Type"); // 查找 Content-Type 头部
        if (content_type == headers.end()) {
            // 如果未找到 Content-Type 头部，则直接返回
            next();
            return;
        }
        ContentType contentType; // 定义 ContentType 枚举
        // 遍历 ContentType 映射，找到对应的 ContentType
        for (const auto &item: contentTypeToString) {
            if (item.second == content_type->second) {
                contentType = item.first;
                break;
            }
        }

        std::stringstream stringstream(body); // 创建 stringstream 用于解析请求体
        switch (contentType) {
            case ContentType::APPLICATION_XML: // 如果请求体是 XML 格式
                boost::property_tree::read_xml(stringstream, session->request.ptree); // 解析 XML 数据
                next();
                return;
            case ContentType::APPLICATION_JSON: // 如果请求体是 JSON 格式
                boost::property_tree::read_json(stringstream, session->request.ptree); // 解析 JSON 数据
                next();
                return;
        }

        boost::regex contentTypeRegex(R"(multipart/form-data;\s*boundary=(.*))");
        // 正则表达式匹配 multipart/form-data 类型的 Content-Type
        boost::smatch match;
        if (boost::regex_search(content_type->second, match, contentTypeRegex)) {
            // 如果 Content-Type 匹配成功
            if (match.size() > 1) {
                // 如果匹配结果的大小大于 1
                string boundary = match[1].str(); // 获取边界字符串
                std::vector<string> params; // 存储字段参数
                std::size_t pos = 0;
                std::size_t lastPos = 0;
                while ((pos = body.find(boundary, lastPos)) != std::string::npos) {
                    // 循环查找边界
                    string field_name; // 字段名
                    string header = body.substr(lastPos, pos - lastPos); // 获取字段头部信息
                    std::string::size_type name_pos = header.find("name=\""); // 查找字段名的起始位置
                    if (name_pos != std::string::npos) {
                        // 如果找到字段名
                        std::string::size_type name_end_pos = header.find('\"', name_pos + 6); // 查找字段名的结束位置
                        field_name = header.substr(name_pos + 6, name_end_pos - name_pos - 6); // 提取字段名
                        std::cout << "field_name：" << field_name << std::endl; // 输出字段名
                        // 查找字段值的开始位置
                        std::string::size_type value_start_pos = body.find("\r\n\r\n", lastPos);
                        if (value_start_pos == string::npos) {
                            // 如果找不到字段值的起始位置
                            std::cerr << "Invalid part format: missing value start" << std::endl; // 输出错误信息
                            continue;
                        } else {
                            value_start_pos += 4;
                        }

                        // 查找字段值的结束位置
                        std::string::size_type value_end_pos = body.find(boundary, value_start_pos);
                        if (value_start_pos == string::npos) {
                            // 如果找不到字段值的结束位置
                            std::cerr << "Invalid part format: missing value end" << std::endl; // 输出错误信息
                            continue;
                        }
                        // 提取字段的值数据
                        std::string value = body.substr(value_start_pos, value_end_pos - value_start_pos - 4); // 提取字段的值
                        session->request.form_data[field_name] = value; // 将字段名和值添加到 form_data 映射中
                    }
                    lastPos = pos + boundary.length(); // 更新上一个边界的位置
                }
            }
        }
        next();
    }
};
