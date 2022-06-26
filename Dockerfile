FROM alpine
RUN mkdir /pico
COPY . /pico
RUN apt install wget
RUN wget -O /pico/pico.sh https://raw.githubusercontent.com/pico-8/pico-8/master/scripts/pico.sh
RUN chmod +x /pico/pico.sh
RUN /pico/pico.sh
RUN reboot