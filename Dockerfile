FROM ubuntu:latest

RUN apt update && \
    apt install -y \
        build-essential \
        git \
        zip \
        unzip \
        curl \
        cmake

COPY . /app

RUN cd app && make setup && make server

CMD [ "bash", "-c", "/app/server" ]
