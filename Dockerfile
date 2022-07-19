FROM ubuntu:latest
ENV DEBIAN_FRONTEND=noninteractive
RUN apt update && apt install -y git && apt install -y cmake gcc-arm-none-eabi libnewlib-arm-none-eabi build-essential libstdc++-arm-none-eabi-newlib

RUN cd /
RUN mkdir pico
RUN cd pico && git clone -b master https://github.com/raspberrypi/pico-sdk.git
ENV PICO_SDK_PATH="/pico/pico-sdk"

# RUN git submodules init

COPY . /pico