//
// Created by Danya Smelskiy on 21.05.2023.
//

#include "Requests.h"

RequestType ConvertStrToRequestType(const std::string& request_str) {
    if (request_str == "GET_DIAGRAM") {
        return RequestType::GET_DIAGRAM;
    } else {
        return RequestType::UNDEFINED;
    }
}
