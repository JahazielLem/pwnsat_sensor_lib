cmake_minimum_required(VERSION 3.2)
project(i2c_tools C)

set(CMAKE_C_STANDARD 11)

# Definir la biblioteca estática
add_library(i2c_tools STATIC src/i2c_tools.c)

# Incluir directorios de cabeceras para esta biblioteca
target_include_directories(i2c_tools PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Buscar y vincular bcm2835
find_library(BCM2835_LIBRARY NAMES bcm2835)
target_link_libraries(i2c_tools PUBLIC ${BCM2835_LIBRARY})