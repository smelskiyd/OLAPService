//
// Created by Danya Smelskiy on 20.05.2023.
//

#include "OLAPService.h"

#include <iostream>

#include "DiagramBuilder.h"
#include "Requests.h"

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

    const RequestType request_type = ConvertStrToRequestType(request_object.at("request-type").AsString());

    switch (request_type) {
        case RequestType::ADD_RECORD: {
            if (request_object.find("data") == request_object.end()) {
                return "ERROR: No field `data`";
            }

            const std::string data_str = request_object.at("data").AsString();
            const auto record = Record::ParseFromString(data_str);
            if (!record.has_value()) {
                return "ERROR: Failed to parse record";
            }

            std::cout << "Adding new record: {" << record.value() << "}\n";
            database_->addRecord(record.value());
            return "OK";
        }
        case RequestType::GET_DIAGRAM: {
            if (request_object.find("diagram-type") == request_object.end()) {
                return "ERROR: No field `diagram-type`";
            }

            const DiagramType diagram_type = ConvertStrToDiagramType(request_object.at("diagram-type").AsString());

            if (diagram_type == DiagramType::UNDEFINED) {
                return "ERROR: Undefined diagram type";
            }

            return getDiagramDump(diagram_type);
        }
        case RequestType::UNDEFINED: {
            return "ERROR: Undefined request type";
        }
    }

    return "OK";
}
