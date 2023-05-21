//
// Created by Danya Smelskiy on 21.05.2023.
//

#include "Requests.h"

RequestType ConvertStrToRequestType(const std::string& request_str) {
    if (request_str == "SAVE") {
        return RequestType::SAVE;
    } else if (request_str == "ADD_RECORD") {
        return RequestType::ADD_RECORD;
    } else if (request_str == "REMOVE_RECORD") {
        return RequestType::REMOVE_RECORD;
    } else if (request_str == "GET_DIAGRAM") {
        return RequestType::GET_DIAGRAM;
    } else {
        return RequestType::UNDEFINED;
    }
}
