//
// Created by Danya Smelskiy on 20.05.2023.
//

#pragma once

#include "database/Database.h"

#include "Types.h"
#include "DiagramBuilder.h"

class OLAPService {
  public:
    OLAPService(const std::string& db_storage_path);

    std::string getDiagramDump(DiagramType diagram_type) const;

    std::string handleRequest(const std::string& request);

  private:
    bool initDatabase(const std::string& db_storage_path);

    std::unique_ptr<Database> database_;
    std::unique_ptr<DiagramBuilder> diagrams_builder_;
};
