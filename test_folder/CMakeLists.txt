# Verificar versión de CMake
cmake_minimum_required(VERSION 3.25)

find_package(cJSON REQUIRED)
find_package(unity REQUIRED)

# Crear un ejecutable para las pruebas
add_executable( tests
        test.c
        ../src/cjson_handler.c
        ../src/cJSON.c
        ../src/handlers.c
        ../src/monitor.c
        ../src/shell.c
    )


# Incluir las cabeceras del proyecto principal
target_include_directories(tests PRIVATE ${CMAKE_SOURCE_DIR}/include)

# Linkear las bibliotecas necesarias
target_link_libraries(tests PRIVATE
        unity::unity
        cjson::cjson
)

# Configurar pruebas con CTest
add_test(NAME tests COMMAND tests)
