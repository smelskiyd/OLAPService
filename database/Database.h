//
// Created by Danya Smelskiy on 18.05.2023.
//

#pragma once

#include <vector>

#include "Record.h"

class Database {
  public:
    using RecordsList = std::vector<Record>;

    Database(const std::string& storage_path);

    bool isValid() const;

    const RecordsList& getRecords() const;
    size_t getRecordsCount() const;

    void addRecord(const Record& new_record);
    void removeRecord(const Record& record);

    bool save() const;

    void print() const;

  private:
    void read(const std::string& storage_path);
    void read(std::istream& storage);

    const std::string kStoragePath;

    bool is_initialized_correctly_{false};
    RecordsList records_;
};
