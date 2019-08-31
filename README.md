# roboteam_proto
Protobuf and ZMQ bindings for RoboteamTwente


# Installation
## List of dependencies

- CMake 3.10
- Protobuf 3.9.1 (https://developers.google.com/protocol-buffers/)
- ZeroMQ 4.2.5 (https://zeromq.org/)
- zmqpp 4.1.2 (https://github.com/zeromq/zmqpp) 
- Armadillo 9.6 (http://arma.sourceforge.net/)


## Installation on Linux (tested on Ubuntu 18.04 Bionic Beaver)

### Install protobuf 3.9.1
```
$ sudo apt-get install autoconf automake libtool curl make g++ unzip

$ git clone https://github.com/protocolbuffers/protobuf.git
$ cd protobuf
$ git submodule update --init --recursive
$ ./autogen.sh

$ ./configure
$ make
$ make check
$ sudo make install
$ sudo ldconfig # refresh shared library cache.
```

### Install ZMQ
```
$ sudo apt-get install libzmq3-dev libzmqpp-dev
```

### install Armadillo
```
sudo apt-get install libarmadillo-dev
```

## installation on macOS (tested on macOS 10.14 Mojave)
Make sure you already have the following:
- CMake (3.10 or higher)
- HomeBrew

```
$ brew install zmq zmqpp armadillo protobuf
```
