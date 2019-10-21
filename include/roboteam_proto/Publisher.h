//
// Created by Lukas Bos on 28/08/2019.
//

#ifndef RTT_PUBLISHER_H
#define RTT_PUBLISHER_H

#include <string>
#include <zmqpp/zmqpp.hpp>

namespace roboteam_proto {
class Publisher {
 private:
  zmqpp::context context;
  zmqpp::socket * socket;
 public:
  explicit Publisher(std::string tcpPort);
  ~Publisher();
  bool send(std::string const& topic, std::string message);
};
}

#endif //RTT_PUBLISHER_H
