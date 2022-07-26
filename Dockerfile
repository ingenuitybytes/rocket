FROM ubuntu:latest
ENV PICO_SDK_PATH="/pico/pico-sdk"
RUN apt update && apt install -y git && apt install -y cmake gcc-arm-none-eabi libnewlib-arm-none-eabi build-essential libstdc++-arm-none-eabi-newlib python3

RUN cd / && mkdir pico
RUN cd pico && git clone -b master https://github.com/raspberrypi/pico-sdk.git

WORKDIR /pico/pico-sdk
RUN git submodule update --init

COPY . /pico

