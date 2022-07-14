cmake_minimum_required(VERSION 3.12)

# Make an automatic library - will be static or dynamic based on user setting
add_library(BMP280 bmp280.cpp bmp280.h)

# This depends on (header only) boost
target_link_libraries(BMP280 
    pico_stdlib
    hardware_spi
    hardware_i2c)
