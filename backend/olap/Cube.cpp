//
// Created by Danya Smelskiy on 20.05.2023.
//

#include "Cube.h"

namespace {

template<typename T>
std::string ToJson(const std::vector<T>& vec);

std::string ToJson(const std::pair<CubeBase::Keys, std::string>& pair);

std::string ToJson(const std::string& str) {
    return "\"" + str + "\"";
}

template<typename T>
std::string ToJson(const std::vector<T>& vec) {
    std::string json = "[";
    for (int i = 0; i < vec.size(); ++i) {
        if (i) {
            json += ", ";
        }
        json += ToJson(vec[i]);
    }
    return json + "]";
}

std::string ToJson(const std::pair<CubeBase::Keys, std::string>& pair) {
    std::string json;
    json += ToJson("keys") + ": " + ToJson(pair.first) + ", ";
    json += ToJson("value") + ": " + ToJson(pair.second);
    return "{" + json + "}";
}

}  // namespace

std::string CubeBase::Dump::toJson() const {
    std::string json;
    json += ToJson("dimensions") + ": " + ToJson(dimensions_name) + ", ";
    json += ToJson("cube") + ": " + ToJson(values);
    return "{" + json + "}";
}
