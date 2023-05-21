//
// Created by Danya Smelskiy on 18.05.2023.
//

#include <iostream>
#include <string>

#include "service/OLAPService.h"
#include "server/Server.h"

#define DEFAULT_PORT 8888

int main() {
    std::unique_ptr<OLAPService> service =
        std::make_unique<OLAPService>("/Users/smelskiyd/CLionProjects/OLAP/backend/storage/records");

    Server server;
    server.init(std::move(service));

    server.run(DEFAULT_PORT);

    return 0;
}
