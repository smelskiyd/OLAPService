//
// Created by Danya Smelskiy on 21.05.2023.
//

#include "Types.h"

DiagramType ConvertStrToDiagramType(const std::string& diagram_type_str) {
    if (diagram_type_str == "TOTAL_PRICE_PER_DATA") {
        return DiagramType::TOTAL_PRICE_PER_DATA;
    } else if (diagram_type_str == "RECORDS_PER_DATA") {
        return DiagramType::RECORDS_PER_DATA;
    } else {
        return DiagramType::UNDEFINED;
    }
}
