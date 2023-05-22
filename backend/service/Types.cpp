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
    } else if (diagram_type_str == "PRICE_RANGE_PER_DATA") {
        return DiagramType::PRICE_RANGE_PER_DATA;
    } else if (diagram_type_str == "COURIERS_REVENUE") {
        return DiagramType::COURIERS_REVENUE;
    } else if (diagram_type_str == "RECORDS_PER_DATA") {
        return DiagramType::RECORDS_PER_DATA;
    } else {
        return DiagramType::UNDEFINED;
    }
}

std::ostream& operator<<(std::ostream& out, const PriceRange& price_range) {
    out << "[" << price_range.min_price << ", " << price_range.max_price << "]";
    return out;
}

bool PriceRange::operator<(const PriceRange& rhs) const {
    return min_price < rhs.min_price || (min_price == rhs.min_price && max_price < rhs.max_price);
}

bool PriceRange::operator>(const PriceRange& rhs) const {
    return !(*this < rhs);
}
