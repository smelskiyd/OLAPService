//
// Created by Danya Smelskiy on 18.05.2023.
//

#include "Record.h"

std::istream& operator>>(std::istream& in, Record& record) {
    in >> record.data
        >> record.time
        >> record.from
        >> record.to
        >> record.product_name
        >> record.courier_id
        >> record.distance
        >> record.price;

    return in;
}

std::ostream& operator<<(std::ostream& out, const Record& record) {
    out << record.data << " "
        << record.time << " "
        << record.from << " "
        << record.to << " "
        << record.product_name << " "
        << record.courier_id << " "
        << record.distance << " "
        << record.price;

    return out;
}

bool Record::operator==(const Record& rhs) {
    return data == rhs.data &&
           time == rhs.time &&
           from == rhs.from &&
           to == rhs.to &&
           product_name == rhs.product_name &&
           courier_id == rhs.courier_id &&
           distance == rhs.distance &&
           price == rhs.price;
}

bool Record::operator!=(const Record& rhs) {
    return !(*this == rhs);
}

const std::string& Record::getDimensionValue(const std::string& dimension) const {
    if (dimension == "data") {
        return data;
    } else if (dimension == "time") {
        return time;
    } else if (dimension == "from") {
        return from;
    } else if (dimension == "to") {
        return to;
    } else if (dimension == "product_name") {
        return product_name;
    } else if (dimension == "courier_id") {
        return courier_id;
    } else if (dimension == "distance") {
        return distance;
    } else if (dimension == "price") {
        return price;
    } else {
        assert(false);
    }
}
