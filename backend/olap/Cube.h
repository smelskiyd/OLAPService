//
// Created by Danya Smelskiy on 18.05.2023.
//

#pragma once

#include <map>
#include <set>
#include <vector>
#include <iostream>
#include <sstream>

class CubeBase {
  public:
    using Dimensions = std::vector<std::string>;
    using Keys = std::vector<std::string>;

    struct Dump {
        std::string toJson() const;

        Dimensions dimensions_name;
        std::vector<std::pair<Keys, std::string>> values;
    };

  public:
    virtual ~CubeBase() = default;

    virtual Dump dump() const = 0;
    virtual Dump dump(bool need_sort) const = 0;
};

template<typename SourceDataType, typename MeasureType>
class Cube : CubeBase {
  public:
    using Mapping = std::map<Keys, MeasureType>;

  public:
    Cube(const Cube& cube);
    Cube(const Dimensions& dimensions);

    template<typename Aggregator>
    void process(const std::vector<SourceDataType>& source_data, Aggregator aggregator);

    Cube getSlice(const std::vector<std::pair<std::string, std::string>>& slice_arguments) const;

    const Mapping& getMapping() const;

    virtual Dump dump() const override;
    virtual Dump dump(bool need_sort) const override;

  private:
    Cube(const Dimensions& dimensions, const Mapping& mapping);

    Keys getKeys(const SourceDataType& source_data);

    const Dimensions kDimensions_;

    Mapping cube_;
};

template<typename SourceDataType, typename MeasureType>
Cube<SourceDataType, MeasureType>::Cube(const Cube& cube) :
    kDimensions_(cube.kDimensions_),
    cube_(cube.cube_) {
}

template<typename SourceDataType, typename MeasureType>
Cube<SourceDataType, MeasureType>::Cube(const Dimensions& dimensions) :
    kDimensions_(dimensions) {
}

template<typename SourceDataType, typename MeasureType>
Cube<SourceDataType, MeasureType>::Cube(const Dimensions& dimensions, const Mapping& mapping) :
    kDimensions_(dimensions), cube_(mapping) {
}

template<typename SourceDataType, typename MeasureType>
template<typename Aggregator>
void Cube<SourceDataType, MeasureType>::process(const std::vector<SourceDataType>& source_data,
                                                Aggregator aggregator) {
    std::map<Keys, std::vector<SourceDataType>> raw_mapping;
    for (const auto& data : source_data) {
        raw_mapping[getKeys(data)].push_back(data);
    }

    cube_.clear();
    for (const auto& [keys, value] : raw_mapping) {
        cube_[keys] = aggregator(value);
    }
}

template<typename SourceDataType, typename MeasureType>
Cube<SourceDataType, MeasureType> Cube<SourceDataType, MeasureType>::getSlice(
        const std::vector<std::pair<std::string, std::string>>& slice_arguments) const {
    Mapping new_cube = cube_;

    std::map<std::string, std::set<std::string>> filters;
    for (const auto& slice : slice_arguments) {
        filters[slice.first].insert(slice.second);
    }

    for (const auto& slice : filters) {
        const auto& dimension_name = slice.first;
        const auto& keys = slice.second;

        int pos = -1;
        for (int i = 0; i < kDimensions_.size(); ++i) {
            if (kDimensions_[i] == dimension_name) {
                pos = i;
                break;
            }
        }
        assert(pos >= 0);

        for (auto iter = new_cube.begin(); iter != new_cube.end();) {
            if (keys.find(iter->first[pos]) == keys.end()) {
                iter = new_cube.erase(iter);
            } else {
                ++iter;
            }
        }
    }

    return Cube(kDimensions_, new_cube);
}

template<typename SourceDataType, typename MeasureType>
const typename Cube<SourceDataType, MeasureType>::Mapping& Cube<SourceDataType, MeasureType>::getMapping() const {
    return cube_;
}

template<typename SourceDataType, typename MeasureType>
CubeBase::Dump Cube<SourceDataType, MeasureType>::dump() const {
    Dump dump;
    dump.dimensions_name = kDimensions_;

    for (const auto& [keys, value] : cube_) {
        std::stringstream sstr;
        sstr << value;

        dump.values.push_back({keys, sstr.str()});
    }

    return dump;
}

template<typename SourceDataType, typename MeasureType>
CubeBase::Dump Cube<SourceDataType, MeasureType>::dump(bool need_sort) const {
    if (!need_sort) {
        return dump();
    }

    Dump dump;
    dump.dimensions_name = kDimensions_;

    std::vector<std::pair<Keys, MeasureType>> sorted_results;
    for (const auto& [keys, value] : cube_) {
        sorted_results.push_back({keys, value});
    }
    std::sort(sorted_results.begin(), sorted_results.end(),
              [](const std::pair<Keys, MeasureType>& lhs, const std::pair<Keys, MeasureType>& rhs) {
        return lhs.second > rhs.second;
    });

    for (const auto& [keys, value] : sorted_results) {
        std::stringstream sstr;
        sstr << value;

        dump.values.push_back({keys, sstr.str()});
    }

    return dump;
}

template<typename SourceDataType, typename MeasureType>
typename Cube<SourceDataType, MeasureType>::Keys Cube<SourceDataType, MeasureType>::getKeys(const SourceDataType& source_data) {
    Keys keys;
    for (const auto& dimension : kDimensions_) {
        keys.push_back(source_data.getDimensionValue(dimension));
    }
    return keys;
}
