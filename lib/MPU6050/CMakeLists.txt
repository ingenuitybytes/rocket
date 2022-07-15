cmake_minimum_required(VERSION 3.12)

# Make an automatic library - will be static or dynamic based on user setting
add_library(MPU6050 mpu6050.cpp mpu6050.h)

# This depends on (header only) boost
target_link_libraries(MPU6050 
    pico_stdlib
    hardware_spi)
