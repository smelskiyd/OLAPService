//
// Created by Danya Smelskiy on 18.05.2023.
//

#include <iostream>
#include <string>

#include "database/Database.h"
#include "service/OLAPService.h"
#include "olap/Cube.h"
#include "olap/Measure.h"

#include "service/Aggregators.h"

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
    OLAPService service("/Users/smelskiyd/CLionProjects/OLAP/storage/records");

    std::cout << service.getDiagramDump(DiagramType::PRICE_PER_DATA) << std::endl;
    std::cout << service.getDiagramDump(DiagramType::RECORDS_PER_DATA) << std::endl;

    return 0;
}
