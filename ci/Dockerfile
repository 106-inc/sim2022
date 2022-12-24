FROM ubuntu:22.04

COPY ./requirements.txt /opt

RUN apt-get update                     && \
    apt-get install -y                    \
                                          \
    g++-10                                \
    clang++-12                            \
                                          \
    git                                   \
    ninja-build                           \
    make                                  \
    cmake                                 \
                                          \
    python3.10                            \
    python3-dev                           \
    python3-pip                           \
                                          \
    libspdlog-dev                         \
    libgtest-dev                       && \
                                          \
    pip3 install -r /opt/requirements.txt

WORKDIR /tmp

COPY ./riscv32-unknown-elf-gcc /usr/bin
