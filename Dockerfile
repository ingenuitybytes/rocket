FROM ubuntu:latest
ENV PICO_SDK_PATH="/pico/pico-sdk"
RUN apt update && apt install -y git && apt install -y cmake gcc-arm-none-eabi libnewlib-arm-none-eabi build-essential libstdc++-arm-none-eabi-newlib python3 minicom udev

RUN cd / && mkdir pico
RUN cd pico && git clone -b master https://github.com/raspberrypi/pico-sdk.git && git clone -b master https://github.com/Tutu-Inc/Rocket && git clone -b master https://github.com/Tutu-Inc/Usb-mount

WORKDIR /pico/pico-sdk
RUN git submodule update --init
WORKDIR /pico/Usb-mount
RUN sh setup.sh && c.. && rm -r Usb-mount
WORKDIR /pico/Rocket
