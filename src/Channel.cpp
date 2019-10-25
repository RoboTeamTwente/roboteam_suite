#include "Channel.h"

roboteam_proto::Channel::Channel(std::string name, std::string port)
  : name(std::move(name)), port(std::move(port))
  { }

roboteam_proto::Channel::Channel(const roboteam_proto::Channel & other)
  : name(other.name), port(other.port)
  { }
