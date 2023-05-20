//
// Created by Danya Smelskiy on 18.05.2023.
//

#include "Database.h"

#include <fstream>
#include <iostream>

Database::Database(const std::string& storage_path) :
        kStoragePath(storage_path) {
    read(storage_path);
}

bool Database::isValid() const {
    return is_initialized_correctly_;
}

const Database::RecordsList& Database::getRecords() const {
    return records_;
}

size_t Database::getRecordsCount() const {
    return records_.size();
}

void Database::addRecord(const Record& new_record) {
    records_.push_back(new_record);
}

void Database::removeRecord(const Record& record) {
    const auto it = std::find(records_.begin(), records_.end(), record);
    records_.erase(it);
}

bool Database::save() const {
    std::ofstream out(kStoragePath);
    if (!out.is_open()) {
        std::cout << "Failed to save database" << std::endl;
        return false;
    }

    for (const auto& record : records_) {
        out << record << '\n';
    }

    return true;
}

void Database::print() const {
    printf("Database has %zu records.\n", getRecordsCount());

    for (const auto& record : records_) {
        std::cout << record << '\n';
    }
}

void Database::read(const std::string& storage_path) {
    std::ifstream storage;

    storage.open(storage_path);
    if (!storage.is_open()) {
        fprintf(stderr, "Failed to open database storage: %s", storage_path.c_str());
        is_initialized_correctly_ = false;
        return;
    }

    read(storage);
}

void Database::read(std::istream& storage) {
    Record record;
    while (storage >> record) {
        records_.push_back(record);
    }
    is_initialized_correctly_ = true;
}
