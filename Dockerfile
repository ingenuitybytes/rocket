FROM alpine
RUN mkdir /pico
COPY . /pico
CMD [ "echo", "Hello World!" ]