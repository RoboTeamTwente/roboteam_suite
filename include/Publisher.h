//
// Created by Lukas Bos on 28/08/2019.
//

#ifndef RTT_PUBLISHER_H
#define RTT_PUBLISHER_H

#include <string>
#include <zmqpp/zmqpp.hpp>

namespace roboteam_proto {
class Publisher {
  const std::string PUBLISH_ENDPOINT = "tcp://127.0.0.1:5555";

 private:
  zmqpp::context context;
  zmqpp::socket *socket;
  std::string topic = "";
 public:
  explicit Publisher(std::string const & topic);
  void send(std::string message);
};
}

#endif //RTT_PUBLISHER_H
