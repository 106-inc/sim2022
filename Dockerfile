FROM ubuntu:22.04

RUN apt-get update && apt-get install -y ninja-build
RUN apt-get update && apt-get install -y libspdlog-dev
RUN apt-get update && apt-get install -y libgtest-dev
