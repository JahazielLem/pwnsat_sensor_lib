cmake_minimum_required(VERSION 3.2)
project(bme280 C)

set(CMAKE_C_STANDARD 11)

# Definir la biblioteca estática
add_library(bme280 STATIC src/bme280.c)

# Incluir directorios de cabeceras para esta biblioteca
target_include_directories(bme280 PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/lib/i2c_tools/include
)

# Buscar y vincular libm y i2c_tools
find_library(MATH_LIBRARY NAMES m)
target_link_libraries(bme280 PUBLIC 
    i2c_tools
    ${MATH_LIBRARY}
)