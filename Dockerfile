FROM ubuntu
ENV DEBIAN_FRONTEND=noninteractive
# RUN mkdir /pico
# COPY . /pico
RUN apt update && apt install -y wget
RUN ["wget", "https://raw.githubusercontent.com/raspberrypi/pico-setup/master/pico_setup.sh"]
RUN chmod 777 pico_setup.sh
RUN ./pico_setup.sh
COPY . /pico
# RUN reboot