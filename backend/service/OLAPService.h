//
// Created by Danya Smelskiy on 20.05.2023.
//

#pragma once

#include "common/json.h"

#include "database/Database.h"

#include "Types.h"
#include "DiagramBuilder.h"

class OLAPService {
  public:
    OLAPService(const std::string& db_storage_path);

    bool reload();

    std::string handleRequest(const Json::Node& request);

  private:
    std::string getDiagramDump(DiagramType diagram_type, bool need_sort) const;
    std::string getSlice(const std::vector<std::pair<std::string, std::string>>& slice) const;

    bool initDatabase(const std::string& db_storage_path);

    const std::string kDBStoragePath;

    std::unique_ptr<Database> database_;
    std::unique_ptr<DiagramBuilder> diagrams_builder_;
};
