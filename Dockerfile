FROM ubuntu
ENV DEBIAN_FRONTEND=noninteractive
RUN apt update && apt install -y git && apt install -y cmake gcc-arm-none-eabi libnewlib-arm-none-eabi build-essential libstdc++-arm-none-eabi-newlib

RUN cd /
RUN mkdir pico
RUN cd pico && git clone -b master https://github.com/raspberrypi/pico-sdk.git
RUN cd pico-sdk
# && git submodule update --init
RUN cd .. && git clone -b master https://github.com/raspberrypi/pico-examples.git

COPY . /pico
# RUN reboot