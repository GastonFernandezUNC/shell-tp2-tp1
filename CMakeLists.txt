cmake_minimum_required(VERSION 3.25 FATAL_ERROR)

project(
    "Shell"
    VERSION 1.0
    LANGUAGES C
)

include(CTest)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_FLAGS_DEBUG "-g")

include_directories(include)

find_package(cJSON REQUIRED)
find_package(unity REQUIRED)

add_executable(${PROJECT_NAME} src/main.c src/handlers.c src/monitor.c src/cjson_handler.c)

target_link_libraries(${PROJECT_NAME} PRIVATE cjson::cjson)
target_link_libraries(${PROJECT_NAME} PRIVATE unity::unity)