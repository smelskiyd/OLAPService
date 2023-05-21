//
// Created by Danya Smelskiy on 20.05.2023.
//

#include "OLAPService.h"

#include <iostream>

#include "DiagramBuilder.h"
#include "Requests.h"

OLAPService::OLAPService(const std::string& db_storage_path) : kDBStoragePath(db_storage_path) {
    if (!reload()) {
        std::cerr << "Failed to load database from file: " << kDBStoragePath << std::endl;
        exit(1);
    }
}

bool OLAPService::reload() {
    bool result = initDatabase(kDBStoragePath);

    diagrams_builder_ = std::make_unique<DiagramBuilder>();
    diagrams_builder_->buildDiagrams(*database_);
    return result;
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
        case RequestType::RELOAD: {
            std::cout << "Reloading database" << '\n';
            if (!reload()) {
                return "ERROR: Failed to reload database";
            }
            return "OK";
        }
        case RequestType::SAVE: {
            std::cout << "Saving database" << '\n';
            database_->save();
            return "OK";
        }
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
        case RequestType::REMOVE_RECORD: {
            if (request_object.find("data") == request_object.end()) {
                return "ERROR: No field `data`";
            }

            const std::string data_str = request_object.at("data").AsString();
            const auto record = Record::ParseFromString(data_str);
            if (!record.has_value()) {
                return "ERROR: Failed to parse record";
            }

            std::cout << "Removing record: {" << record.value() << "}\n";
            if (database_->removeRecord(record.value())) {
                return "OK";
            } else {
                return "ERROR: There is no such record";
            }
        }
        case RequestType::GET_RECORDS: {
            std::vector<Json::Node> records;

            for (const auto& record : database_->getRecords()) {
                std::stringstream sstr;
                sstr << record;
                records.emplace_back(sstr.str());
            }

            Json::Node records_json(records);
            std::stringstream sstr;
            sstr << records_json;
            return sstr.str();
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
