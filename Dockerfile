FROM alpine:latest
RUN mkdir /pico
WORKDIR /pico
COPY . /pico/