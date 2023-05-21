//
// Created by Danya Smelskiy on 18.05.2023.
//

#include <iostream>
#include <string>

#include "service/OLAPService.h"
#include "server/Server.h"

#define DEFAULT_PORT 8888

// [Data, Time] -> Total Price
//void Graph1(const Database& database) {
//    Cube<Record, double> my_cube({"data", "time"});
//    my_cube.process(database.getRecords(), RecordTotalPriceAggregator());
//    my_cube.print();
//}
//
// [Courier, Distance] -> Average Price
//void Graph2(const Database& database) {
//    Cube<Record, double> my_cube({"courier", "distance"});
//    my_cube.process(database.getRecords(), RecordAveragePriceAggregator());
//    my_cube.print();
//}

int main() {
    std::unique_ptr<OLAPService> service =
        std::make_unique<OLAPService>("/Users/smelskiyd/CLionProjects/OLAP/backend/storage/records");

    Server server;
    server.init(std::move(service));

    server.run(DEFAULT_PORT);

//    std::cout << service.getDiagramDump(DiagramType::PRICE_PER_DATA) << std::endl;
//    std::cout << service.getDiagramDump(DiagramType::RECORDS_PER_DATA) << std::endl;

    return 0;
}
