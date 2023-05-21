project(Back-End)

# Common
set(COMMON_LIB_SRCS
        common/Json.cpp)

add_library(common-lib STATIC ${COMMON_LIB_SRCS})
target_include_directories(common-lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/common)

# OLAP
set(SRCS
        database/Database.cpp
        database/Record.cpp
        service/OLAPService.cpp
        service/DiagramBuilder.cpp
        olap/Cube.cpp)

add_library(olap-lib STATIC ${SRCS})
target_include_directories(olap-lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(olap-lib PUBLIC common-lib)

# Server
set(SERVER_LIB_SRCS
        server/Server.cpp
        server/Client.cpp
        server/ChatMessage.cpp)

add_library(server-lib STATIC ${SERVER_LIB_SRCS})
target_link_libraries(server-lib PUBLIC common-lib olap-lib)

add_executable(RunServer run_server.cpp)
target_link_libraries(RunServer PRIVATE server-lib)

add_executable(RunClient run_client.cpp)
target_link_libraries(RunClient PRIVATE server-lib)

# OLAP Server
add_executable(OLAP-Server main.cpp)
target_link_libraries(OLAP-Server PRIVATE olap-lib server-lib)
