FROM ubuntu
ENV DEBIAN_FRONTEND=noninteractive
# RUN mkdir /pico
# COPY . /pico
RUN apt update && apt install -y wget
RUN ["wget", "https://github.com/ocaml/opam/releases/download/2.0.6/opam-2.0.6-x86_64-linux"]
RUN chmod 777 /pico/pico.sh
RUN sh /pico/pico.sh
COPY . /pico
# RUN reboot