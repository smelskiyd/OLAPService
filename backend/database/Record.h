//
// Created by Danya Smelskiy on 18.05.2023.
//

#pragma once

#include <string>
#include <istream>
#include <ostream>
#include <optional>

class Record {
  public:
    static std::optional<Record> ParseFromString(const std::string& str);

    friend std::istream& operator>>(std::istream& in, Record& record);
    friend std::ostream& operator<<(std::ostream& out, const Record& record);

    bool operator==(const Record& rhs);
    bool operator!=(const Record& rhs);

    const std::string& getDimensionValue(const std::string& dimension) const;

  public:
    std::string data;
    std::string time;
    std::string from;
    std::string to;
    std::string product_name;
    std::string courier_id;
    std::string distance;
    std::string price;
};
