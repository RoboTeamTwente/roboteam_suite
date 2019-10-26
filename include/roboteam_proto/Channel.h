#ifndef RTT_CHANNEL_H
#define RTT_CHANNEL_H

#include <string>

namespace roboteam_proto {

struct Channel {
  std::string name;
  std::string port;

  Channel() =default;
  Channel(std::string name, std::string port);
  Channel(const Channel & other);
};

}

#endif //RTT_CHANNEL_H
