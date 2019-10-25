//
// Created by Lukas Bos on 28/08/2019.
//

#ifndef RTT_PUBLISHER_H
#define RTT_PUBLISHER_H

#include <string>
#include <zmqpp/zmqpp.hpp>
#include <google/protobuf/message.h>
#include "Channel.h"

namespace roboteam_proto {

template <class T>
class Publisher {
 private:
  zmqpp::context context;
  zmqpp::socket * socket;
  Channel channel;

 public:
  Publisher(const Publisher & copy) = delete; // you cannot copy this object
  explicit Publisher(const Channel & channel) : channel(channel) {
    std::cout << "[Roboteam_proto] Starting publisher for channel " << channel.name << std::endl;
    socket = new zmqpp::socket(context, zmqpp::socket_type::pub);
    socket->bind(channel.port);
  }

  ~Publisher() {
    std::cout << "[Roboteam_proto] Stopping publisher for channel " << channel.name << std::endl;
    socket->close();
    delete socket;
  }

  bool send(const T & message) {
    zmqpp::message transmission;
    transmission << message.SerializeAsString();
    return socket->send(transmission, true);
  }
};
}

#endif //RTT_PUBLISHER_H
