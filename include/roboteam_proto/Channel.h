#ifndef RTT_CHANNEL_H
#define RTT_CHANNEL_H

#include <string>

namespace proto {

struct Channel {
  std::string name;
  std::string port;

  Channel() =default;
  Channel(std::string name, std::string port);
  Channel(const Channel & other);

  bool operator == (const Channel & other);
  bool operator != (const Channel & other);
};

} // proto

#endif //RTT_CHANNEL_H
