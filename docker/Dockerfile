FROM ubuntu:18.04

LABEL Maintainer Roboteam <roboteamtwente@gmail.com>

# Update lists
RUN /bin/bash -c "apt-get update"

# Upgrade packages, -y marks everything "yes"
RUN /bin/bash -c "apt-get dist-upgrade -y" 

# Install dependencies
RUN /bin/bash -c "apt-get -y install \
    libzmq3-dev \ 
    libzmqpp-dev \
    clang \
    xvfb \
    ninja-build \
    libarmadillo-dev \
    autoconf \
    automake \
    libtool \
    curl \
    make \
    g++ \
    unzip \
    cmake \
    libqt5charts5-dev \
    libsdl2-dev \
    qtdeclarative5-dev \
    git"

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

# Clone the repos and checkout master on everything
RUN /bin/bash -c "git clone https://github.com/RoboTeamTwente/roboteam_suite /opt/roboteam/roboteam_suite --recurse-submodules &&\
    cd /opt/roboteam/roboteam_suite &&\
    git submodule update --init --recursive &&\
    git submodule foreach git checkout master"

# Add tests to the container.
RUN mkdir -p /opt/robotests
# Copy over tests folder
COPY tests /opt/robotests
# Copy over divider
COPY divider.sh /divider.sh
# Set the entrypoint for when your docker container runs
ENTRYPOINT [ "/divider.sh" ]