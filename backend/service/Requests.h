//
// Created by Danya Smelskiy on 21.05.2023.
//

#pragma once

#include <string>

enum class RequestType {
    RELOAD,
    SAVE,
    ADD_RECORD,
    REMOVE_RECORD,
    GET_RECORDS,
    GET_SLICE,
    GET_DIAGRAM,
    UNDEFINED
};

RequestType ConvertStrToRequestType(const std::string& request_str);
