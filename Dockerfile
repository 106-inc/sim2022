FROM ubuntu:22.04

RUN apt-get update && \
    \
    apt-get install -y g++-10 && \
    apt-get install -y clang++-12 && \
    \
    apt-get install -y git && \
    apt-get install -y ninja-build && \
    apt-get install -y make && \
    apt-get install -y cmake && \
    \
    apt-get install -y python3.10 && \
    apt-get install -y python3-dev && \
    apt-get install -y python3-pip && \
    pip3 install pyyaml && \
    \
    apt-get install -y libspdlog-dev && \
    apt-get install -y libgtest-dev
