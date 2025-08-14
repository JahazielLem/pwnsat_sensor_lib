cmake_minimum_required(VERSION 3.2)
project(coreflight C)

set(CMAKE_C_STANDARD 11)

# Incluir directorios de cabeceras
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/lib/bme280/include
    ${CMAKE_SOURCE_DIR}/lib/i2c_tools/include
    ${CMAKE_SOURCE_DIR}/lib/mpu6050/include
)

# Buscar bibliotecas externas
find_library(BCM2835_LIBRARY NAMES bcm2835)
find_library(MATH_LIBRARY NAMES m)

# Incluir subdirectorios de las bibliotecas
add_subdirectory(lib/i2c_tools)
add_subdirectory(lib/bme280)
add_subdirectory(lib/mpu6050)

# Definir el ejecutable principal
add_executable(coreflight src/main.c)

# Vincular el ejecutable con las bibliotecas
target_link_libraries(coreflight 
    i2c_tools
    bme280
    mpu6050
    ${BCM2835_LIBRARY}
    ${MATH_LIBRARY}
)