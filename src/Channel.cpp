#include "Channel.h"

proto::Channel::Channel(std::string name, std::string port)
  : name(std::move(name)), port(std::move(port))
  { }

proto::Channel::Channel(const proto::Channel & other)
  : name(other.name), port(other.port)
  { }

bool proto::Channel::operator==(const proto::Channel &other) {
    return name == other.name && port == other.port;
}

bool proto::Channel::operator!=(const proto::Channel &other) {
    return !(*this == other);
}
