//
// Created by Danya Smelskiy on 20.05.2023.
//

#pragma once

#include <ostream>

template<typename Measure>
struct MeasureList {
    MeasureList() = default;
    explicit MeasureList(const std::vector<Measure>& source_data) : data(source_data) {}
    explicit MeasureList(std::vector<Measure>&& source_data) : data(std::move(source_data)) {}

    std::vector<Measure> data;
};

template<typename Measure>
std::ostream& operator<<(std::ostream& out, const MeasureList<Measure>& list) {
    out << "{" << '\n';
    for (size_t i = 0; i < list.data.size(); ++i) {
        out << "\t{" << list.data[i] << "}";
        if (i + 1 != list.data.size()) {
            out << ",";
        }
        out << '\n';
    }
    out << "}";
    return out;
}
