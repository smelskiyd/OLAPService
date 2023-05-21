//
// Created by Danya Smelskiy on 20.05.2023.
//

#pragma once

#include <string>

enum class DiagramType {
    TOTAL_PRICE_PER_DATA,
    RECORDS_PER_DATA,
    COUNT,
    UNDEFINED
};

DiagramType ConvertStrToDiagramType(const std::string& diagram_type_str);
