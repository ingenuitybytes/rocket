cmake_minimum_required(VERSION 3.12)

# Pull in SDK (must be before project)
include(pico_sdk_import.cmake)

project(main C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialize the SDK
pico_sdk_init()

add_executable(main
      main.cpp
      )

# Tell CMake where to find other source code
add_subdirectory(lib)

# pull in common dependencies
target_link_libraries(
      main 
      pico_stdlib
      hardware_spi
      hardware_i2c
      BMP280
      FatFs_SPI
      RF24)

# enable usb output, disable uart output
pico_enable_stdio_usb(main 1)
pico_enable_stdio_uart(main 0)

# create map/bin/hex/uf2 file etc.
pico_add_extra_outputs(main)