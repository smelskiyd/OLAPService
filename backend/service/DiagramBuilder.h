//
// Created by Danya Smelskiy on 20.05.2023.
//

#pragma once

#include "database/Database.h"
#include "olap/Cube.h"
#include "olap/Measure.h"

#include "Types.h"

class DiagramBuilder {
  public:
    DiagramBuilder() = default;

    void buildDiagrams(const Database& database);

    CubeBase::Dump getDiagramDump(DiagramType diagram_type) const;
    CubeBase::Dump getSlice(const std::vector<std::pair<std::string, std::string>>& slice) const;

  private:
    Cube<Record, MeasureList<Record>> full_cube_{{"data", "time", "from", "to", "product_name",
                                                  "courier_id", "distance", "price"}};
    Cube<Record, double> total_price_per_data_{{"data"}};
    Cube<Record, uint64_t> deliveries_per_data_and_time_{{"data", "time"}};
    Cube<Record, uint64_t> deliveries_per_distance_and_time_{{"distance", "time"}};
    Cube<Record, uint64_t> products_count_{{"product_name"}};
    Cube<Record, PriceRange> price_range_per_data_{{"data"}};
    Cube<Record, double> couriers_revenue_{{"courier_id"}};
    Cube<Record, MeasureList<Record>> records_per_data_{{"data"}};
};
