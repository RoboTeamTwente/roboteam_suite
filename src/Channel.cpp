#include "Channel.h"

proto::Channel::Channel(std::string name, std::string ip, std::string port)
  : name(std::move(name)), ip(std::move(ip)), port(std::move(port))
  { }

proto::Channel::Channel(const proto::Channel & other)
  : name(other.name), port(other.port), ip(other.ip)
  { }

bool proto::Channel::operator==(const proto::Channel &other) {
    return name == other.name && port == other.port;
}

bool proto::Channel::operator!=(const proto::Channel &other) {
    return !(*this == other);
}

std::string proto::Channel::getSubscribeAddress() {
    return getAddress(ip, port);
}

std::string proto::Channel::getPublishAddress() {
    return getAddress("*", port);
}

std::string proto::Channel::getAddress(const std::string & _ip, const std::string & _port) {
    return "tcp://" + _ip + ":" + _port;
}
