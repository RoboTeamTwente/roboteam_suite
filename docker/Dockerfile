FROM ubuntu:18.04

LABEL Maintainer Roboteam <roboteamtwente@gmail.com>
RUN apt-get update 
RUN apt-get dist-upgrade -y 
RUN apt-get -y install libzmq3-dev libzmqpp-dev ninja-build libarmadillo-dev autoconf automake libtool curl make g++ unzip cmake libqt5charts5-dev libsdl2-dev qtdeclarative5-dev git

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

# Add tests to the container.
RUN mkdir -p /opt/robotests
COPY tests /opt/robotests
COPY divider.sh /divider.sh
# ENTRYPOINT [ "/divider.sh" ]