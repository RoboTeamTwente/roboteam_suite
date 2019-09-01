# roboteam_proto
Protobuf and ZMQ bindings for RoboteamTwente

# new structure for roboteam twente

Roboteam proto allows us to move from structure 1 to structure 2, and acts as an in-between structure to eventually move to structure 3. 

### Structure 1:
The structure used for robocup 2018-2019. the main problem is the unnecessary vision repository, which does not do that much, but does slow down the system a little bit. Since we want to stop using ROS this repository has no significant use anymore and we can reduce complexity in our repositories.
![structure 1](https://i.imgur.com/yB2jTnA.png)

### Structure 2:
Structure 2 is much cleaner. without ROS a replacement for ros params is found by publishing settings which can be set in the interface from roboteam AI. This gives flexibility in configuration. Also roboteam_World is now only acting as a kalman filter and data channel. The output of world_state is not `us` and `them` anymore, but `blue` and `yellow`. This provides more flexibilty for later on, for example when using an autoref or when using two AI's at once (and displaying them in one interface). The main problem with this structure is that a lot of complexity is hidden in roboteam_ai. 
![structure 2](https://i.imgur.com/EK3Sd0p.png)



# usage
Easiest way to run things is to make some compound executables in CLion that boot everything you need. Make sure you have grsim or ssl-vision running and are listening to the right ports.


running one team:
```
./kalman_world
./roboteam_ai
./roboteam_robothub 
```
running two teams:
```
./kalman_world
./roboteam_ai
./roboteam_robothub 
./roboteam_ai 1
./roboteam_robothub 1
```


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
