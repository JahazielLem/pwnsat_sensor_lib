cmake_minimum_required(VERSION 3.2)
project(mpu6050 C)

set(CMAKE_C_STANDARD 11)

# Definir la biblioteca estática
add_library(mpu6050 STATIC src/mpu6050.c)

# Incluir directorios de cabeceras para esta biblioteca
target_include_directories(mpu6050 PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/lib/i2c_tools/include
)

# Vincular con i2c_tools
target_link_libraries(mpu6050 PUBLIC i2c_tools)