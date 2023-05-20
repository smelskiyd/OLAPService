cmake_minimum_required(VERSION 3.20)
project(OLAP)

set(CMAKE_CXX_STANDARD 17)

add_library(common-lib STATIC common/Json.cpp)
target_include_directories(common-lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/common)

set(SERVER_LIB_SRCS
        server/Server.cpp
        server/Client.cpp
        server/ChatMessage.cpp)

add_library(server-lib STATIC ${SERVER_LIB_SRCS})
target_link_libraries(server-lib PUBLIC common-lib)

set(SRCS
        database/Database.cpp
        database/Record.cpp
        service/OLAPService.cpp
        service/DiagramBuilder.cpp
        olap/Cube.cpp)

add_library(olap_lib STATIC ${SRCS})
target_include_directories(olap_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(OLAP main.cpp)
target_link_libraries(OLAP PRIVATE olap_lib server-lib)

add_executable(RunServer run_server.cpp)
target_link_libraries(RunServer PRIVATE server-lib)

add_executable(RunClient run_client.cpp)
target_link_libraries(RunClient PRIVATE server-lib)
