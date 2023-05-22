//
// Created by Danya Smelskiy on 20.05.2023.
//

#pragma once

#include "database/Record.h"
#include "olap/Measure.h"

struct RecordDefaultAggregator {
    MeasureList<Record> operator()(const std::vector<Record>& records) {
        return MeasureList<Record>(records);
    }
};

struct RecordTotalPriceAggregator {
    double operator()(const std::vector<Record>& records) {
        double total_price = 0;
        for (const auto& record : records) {
            total_price += std::stod(record.getDimensionValue("price"));
        }
        return total_price;
    }
};

struct RecordCountAggregator {
    uint64_t operator()(const std::vector<Record>& records) {
        return static_cast<uint64_t>(records.size());
    }
};

struct RecordAveragePriceAggregator {
    double operator()(const std::vector<Record>& records) {
        double total_price = 0;
        for (const auto& record : records) {
            total_price += std::stod(record.getDimensionValue("price"));
        }
        return total_price / static_cast<double>(records.size());
    }
};

struct RecordDistanceAggregator {
    double operator()(const std::vector<Record>& records) {
        double total_price = 0;
        for (const auto& record : records) {
            total_price += std::stod(record.getDimensionValue("distance"));
        }
        return total_price;
    }
};

struct RecordPriceRangeAggregator {
    PriceRange operator()(const std::vector<Record>& records) {
        PriceRange result{0., 0.};
        result.max_price = records.empty() ? 0. : std::stod(records[0].price);
        result.min_price = records.empty() ? 0. : std::stod(records[0].price);

        for (const auto& record : records) {
            const double current_price = std::stod(record.price);
            if (current_price < result.min_price) {
                result.min_price = current_price;
            }
            if (current_price > result.max_price) {
                result.max_price = current_price;
            }
        }

        return result;
    }
};
