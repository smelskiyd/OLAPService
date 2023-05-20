//
// Created by Danya Smelskiy on 18.05.2023.
//

#pragma once

#include <map>
#include <vector>
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

    const Mapping& getMapping() const;

    virtual Dump dump() const override;

  private:
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
typename Cube<SourceDataType, MeasureType>::Keys Cube<SourceDataType, MeasureType>::getKeys(const SourceDataType& source_data) {
    Keys keys;
    for (const auto& dimension : kDimensions_) {
        keys.push_back(source_data.getDimensionValue(dimension));
    }
    return keys;
}
