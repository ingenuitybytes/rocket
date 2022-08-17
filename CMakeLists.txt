cmake_minimum_required(VERSION 3.12)

# Pull in SDK (must be before project)
include(pico_sdk_import.cmake)

project(rocket C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

if (PICO_SDK_VERSION_STRING VERSION_LESS "1.3.0")
    message(FATAL_ERROR "Raspberry Pi Pico SDK version 1.3.0 (or later) required. Your version is ${PICO_SDK_VERSION_STRING}")
endif()

set(ROCKET_PATH ${PROJECT_SOURCE_DIR})

# Initialize the SDK
pico_sdk_init()

# Tell CMake where to find other source code
add_subdirectory(lib)

add_compile_options(-Wall
        -Wno-format          # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
        -Wno-unused-function # we have some for the docs that aren't called
        -Wno-maybe-uninitialized
        )

        
add_executable(gc
    gc.cpp
)

# pull in common dependencies
target_link_libraries(gc
    pico_stdlib
    hardware_pwm
    hardware_spi
    hardware_i2c
    BMP280
    MPU6050
    FatFs_SPI
    RF24
    )

# enable usb output, disable uart output
pico_enable_stdio_usb(gc 1)
pico_enable_stdio_uart(gc 0)

# create map/bin/hex/uf2 file etc.
pico_add_extra_outputs(gc)

add_executable(fc
    fc.cpp
)

# pull in common dependencies
target_link_libraries(fc
    pico_stdlib
    hardware_pwm
    hardware_spi
    hardware_i2c
    BMP280
    MPU6050
    FatFs_SPI
    RF24
    )

# enable usb output, disable uart output
pico_enable_stdio_usb(fc 1)
pico_enable_stdio_uart(fc 0)

# create map/bin/hex/uf2 file etc.
pico_add_extra_outputs(fc)

add_executable(picow_access_point_background
        picow_access_point.c
        dhcpserver/dhcpserver.c
        )

target_include_directories(picow_access_point_background PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/.. # for our common lwipopts
        ${CMAKE_CURRENT_LIST_DIR}/dhcpserver
        )

target_link_libraries(picow_access_point_background
        pico_cyw43_arch_lwip_threadsafe_background
        pico_stdlib
        )

pico_add_extra_outputs(picow_access_point_background)

add_executable(picow_access_point_poll
        picow_access_point.c
        dhcpserver/dhcpserver.c
        )
target_include_directories(picow_access_point_poll PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/.. # for our common lwipopts
        ${CMAKE_CURRENT_LIST_DIR}/dhcpserver
        )
target_link_libraries(picow_access_point_poll
        pico_cyw43_arch_lwip_poll
        pico_stdlib
        )

pico_add_extra_outputs(picow_access_point_poll)