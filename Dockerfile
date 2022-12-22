FROM ubuntu:22.04

RUN apt-get update; \
    apt-get install -y git; \
    apt-get install -y ninja-build; \
    apt-get install -y libspdlog-dev; \
    apt-get install -y libgtest-dev
