//
// Created by Danya Smelskiy on 20.05.2023.
//

#include "DiagramBuilder.h"

#include <iostream>

#include "Aggregators.h"

void DiagramBuilder::buildDiagrams(const Database& database) {
    total_price_per_data_.process(database.getRecords(), RecordTotalPriceAggregator());
    records_per_data_.process(database.getRecords(), RecordDefaultAggregator());
}

CubeBase::Dump DiagramBuilder::getDiagramDump(DiagramType diagram_type) const {
    switch (diagram_type) {
        case DiagramType::TOTAL_PRICE_PER_DATA: {
            return total_price_per_data_.dump();
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
