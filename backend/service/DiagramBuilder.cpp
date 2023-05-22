//
// Created by Danya Smelskiy on 20.05.2023.
//

#include "DiagramBuilder.h"

#include <iostream>

#include "Aggregators.h"

void DiagramBuilder::buildDiagrams(const Database& database) {
    total_price_per_data_.process(database.getRecords(), RecordTotalPriceAggregator());
    products_count_.process(database.getRecords(), RecordCountAggregator());
    deliveries_per_data_and_time_.process(database.getRecords(), RecordCountAggregator());
    deliveries_per_distance_and_time_.process(database.getRecords(), RecordCountAggregator());
    price_range_per_data_.process(database.getRecords(), RecordPriceRangeAggregator());
    couriers_revenue_.process(database.getRecords(), RecordTotalPriceAggregator());
    records_per_data_.process(database.getRecords(), RecordDefaultAggregator());
    full_cube_.process(database.getRecords(), RecordDefaultAggregator());
}

CubeBase::Dump DiagramBuilder::getDiagramDump(DiagramType diagram_type) const {
    switch (diagram_type) {
        case DiagramType::TOTAL_PRICE_PER_DATA: {
            return total_price_per_data_.dump();
        }
        case DiagramType::PRODUCTS_COUNT: {
            return products_count_.dump();
        }
        case DiagramType::DELIVERIES_PER_DATA_AND_TIME: {
            return deliveries_per_data_and_time_.dump();
        }
        case DiagramType::DELIVERIES_PER_DISTANCE_AND_TIME: {
            return deliveries_per_distance_and_time_.dump();
        }
        case DiagramType::PRICE_RANGE_PER_DATA: {
            return price_range_per_data_.dump();
        }
        case DiagramType::COURIERS_REVENUE: {
            return couriers_revenue_.dump();
        }
        case DiagramType::RECORDS_PER_DATA: {
            return records_per_data_.dump();
        }
        case DiagramType::COUNT:
        case DiagramType::UNDEFINED: {
            std::cerr << "Undefined diagram type" << std::endl;
            return {};
        }
    }
}

CubeBase::Dump DiagramBuilder::getSlice(const std::vector<std::pair<std::string, std::string>>& slice) const {
    return full_cube_.getSlice(slice).dump();
}
