//
// Created by Danya Smelskiy on 21.05.2023.
//

#pragma once

#include <string>

enum class RequestType {
    SAVE,
    ADD_RECORD,
    REMOVE_RECORD,
    GET_DIAGRAM,
    UNDEFINED
};

RequestType ConvertStrToRequestType(const std::string& request_str);
