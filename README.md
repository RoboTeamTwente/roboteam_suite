# roboteam_proto
Protobuf and ZMQ bindings for RoboteamTwente


# installation on Linux

install protobuf 3.9.1
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

Install ZMQ
```
apt-get install libzmq3-dev libzmqpp-dev
```

# installation on macOS
Make sure you have homebrew installed
```
brew install zmq zmqpp armadillo protobuf
```
