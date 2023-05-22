//
// Created by Danya Smelskiy on 21.05.2023.
//

#include "Types.h"

DiagramType ConvertStrToDiagramType(const std::string& diagram_type_str) {
    if (diagram_type_str == "TOTAL_PRICE_PER_DATA") {
        return DiagramType::TOTAL_PRICE_PER_DATA;
    } else if (diagram_type_str == "PRODUCTS_COUNT") {
        return DiagramType::PRODUCTS_COUNT;
    } else if (diagram_type_str == "DELIVERIES_PER_DATA_AND_TIME") {
        return DiagramType::DELIVERIES_PER_DATA_AND_TIME;
    } else if (diagram_type_str == "DELIVERIES_PER_DISTANCE_AND_TIME") {
        return DiagramType::DELIVERIES_PER_DISTANCE_AND_TIME;
    } else if (diagram_type_str == "RECORDS_PER_DATA") {
        return DiagramType::RECORDS_PER_DATA;
    } else {
        return DiagramType::UNDEFINED;
    }
}
