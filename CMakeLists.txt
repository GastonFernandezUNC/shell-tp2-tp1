# Verify cmake version
cmake_minimum_required(VERSION 3.25)

# Create a project
project (
    "Shell"
    VERSION 1.0.0
    LANGUAGES C
)

# Enable testing
include(CTest)
enable_testing()

# Flags for the compiler
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_FLAGS_DEBUG "-g -O0 -Wall -Wextra -Wpedantic -Werror -Wunused-parameter -Wmissing-prototypes -Wstrict-prototypes")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -fprofile-arcs -ftest-coverage")

# Include headers
include_directories(include)
add_subdirectory(metrics-tp1-so1)
add_subdirectory(test_folder)

# Conan setup
find_package(cJSON REQUIRED)
find_package(unity REQUIRED)

# Add executables
add_executable(${PROJECT_NAME} src/main.c
        src/cjson_handler.c
        src/handlers.c
        src/monitor.c
        src/shell.c
    )

target_link_libraries(${PROJECT_NAME} PRIVATE cjson::cjson)
target_link_libraries(${PROJECT_NAME} PRIVATE unity::unity)

# Flags for coverage
set(CMAKE_C_FLAGS_DEBUG  "${CMAKE_C_FLAGS} -fprofile-arcs -ftest-coverage")
