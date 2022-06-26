FROM ubuntu
ENV DEBIAN_FRONTEND=noninteractive
# RUN mkdir /pico
# COPY . /pico
RUN apt update && apt install -y wget
RUN wget https://raw.githubusercontent.com/pico-8/pico-8/master/scripts/pico.sh
RUN chmod +x /pico/pico.sh && /pico/pico.sh
COPY . /pico
# RUN reboot