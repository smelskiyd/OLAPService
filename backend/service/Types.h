//
// Created by Danya Smelskiy on 20.05.2023.
//

#pragma once

#include <string>
#include <iostream>

enum class DiagramType {
    TOTAL_PRICE_PER_DATA,
    PRODUCTS_COUNT,
    DELIVERIES_PER_DATA_AND_TIME,
    DELIVERIES_PER_DISTANCE_AND_TIME,
    PRICE_RANGE_PER_DATA,
    COURIERS_REVENUE,
    RECORDS_PER_DATA,
    COUNT,
    UNDEFINED
};

DiagramType ConvertStrToDiagramType(const std::string& diagram_type_str);

struct PriceRange {
    double min_price;
    double max_price;

    friend std::ostream& operator<<(std::ostream& out, const PriceRange& price_range);

    bool operator<(const PriceRange& rhs) const;
    bool operator>(const PriceRange& rhs) const;
};
