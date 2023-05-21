//
// Created by Danya Smelskiy on 20.05.2023.
//

#include "OLAPService.h"

#include <iostream>

#include "DiagramBuilder.h"

OLAPService::OLAPService(const std::string& db_storage_path) {
    initDatabase(db_storage_path);

    diagrams_builder_ = std::make_unique<DiagramBuilder>();
    diagrams_builder_->buildDiagrams(*database_);
}

bool OLAPService::initDatabase(const std::string& db_storage_path) {
    database_ = std::make_unique<Database>(db_storage_path);
    if (!database_->isValid()) {
        std::cout << "Failed to initialize database" << std::endl;
        database_.reset();
        return false;
    }

    return true;
}

std::string OLAPService::getDiagramDump(DiagramType diagram_type) const {
    return diagrams_builder_->getDiagramDump(diagram_type).toJson();
}

std::string OLAPService::handleRequest(const Json::Node& request) {
    if (!request.IsMap()) {
        return "ERROR: Bad request format";
    }

    const auto& request_object = request.AsMap();
    if (request_object.find("request-type") == request_object.end()) {
        return "ERROR: No field `request-type`";
    }

    const std::string request_type = request_object.at("request-type").AsString();

    std::cout << "Request type: " << request_type << std::endl;

    return "OK";
}
