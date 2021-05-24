FROM ubuntu:latest

LABEL Maintainer Roboteam <roboteamtwente@gmail.com>

# Set environment variables / arguments
ARG DEBIAN_FRONTEND=noninteractive

# Update lists
RUN /bin/bash -c "apt-get update -y"

# Upgrade packages, -y marks everything "yes"
RUN /bin/bash -c "apt-get dist-upgrade -y" 

# Install dependencies
RUN /bin/bash -c "apt-get -y install \
    ccache \
    clang \
    xvfb \
    ninja-build \
    autoconf \
    automake \
    libtool \
    libtbb-dev \
    apt-utils \
    libboost-all-dev \
    libeigen3-dev \
    curl \
    make \
    g++ \
    unzip \
    cmake \
    libqt5charts5-dev \
    libsdl2-dev \
    qtdeclarative5-dev \
    git \
    "

# Nlohmann-json
RUN /bin/bash -c "mkdir -p /usr/include/nlohmann && \
    curl https://raw.githubusercontent.com/nlohmann/json/develop/single_include/nlohmann/json.hpp > /usr/include/nlohmann/json.hpp \
    "

# Setup protobuf
RUN /bin/bash -c "git clone https://github.com/protocolbuffers/protobuf.git /opt/protobuf &&\
    cd /opt/protobuf &&\
    git submodule update --init --recursive &&\
    ./autogen.sh &&\
    ./configure &&\
    make -j $(nproc)&&\
    make install &&\
    ldconfig"

# Setup gtest
RUN /bin/bash -c "git clone https://github.com/google/googletest.git /opt/googletest &&\
    cd /opt/googletest &&\
    mkdir install &&\
    cd install &&\
    cmake .. &&\
    make -j $(nproc)&&\
    make install"

# Clone the repos and 
RUN /bin/bash -c "git clone https://github.com/RoboTeamTwente/roboteam_suite /opt/roboteam/roboteam_suite --recursive --branch=robocup/2021"

RUN /bin/bash -c "cd /opt/roboteam/roboteam_suite &&\ 
    ls \
    "

# Copy over the run.sh
COPY run.sh /runs.sh

ENTRYPOINT [ "/run.sh" ]