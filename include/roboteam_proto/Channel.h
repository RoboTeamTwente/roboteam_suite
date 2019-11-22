#ifndef RTT_CHANNEL_H
#define RTT_CHANNEL_H

#include <string>

namespace proto {

struct Channel {
  std::string name;
  std::string ip;
  std::string port;

  Channel() =default;
  Channel(std::string name, std::string ip, std::string port);
  Channel(const Channel & other);

  std::string getSubscribeAddress();
  std::string getPublishAddress();

  bool operator == (const Channel & other);
  bool operator != (const Channel & other);
};

} // proto

#endif //RTT_CHANNEL_H
