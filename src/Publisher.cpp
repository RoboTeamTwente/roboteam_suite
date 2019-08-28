//
// Created by Lukas Bos on 28/08/2019.
//

#include "Publisher.h"
#include <string>

roboteam_proto::Publisher::Publisher(std::string const& topic) {
  socket = new zmqpp::socket(context, zmqpp::socket_type::pub);
  socket->bind(PUBLISH_ENDPOINT);
  this->topic = topic;
}

void roboteam_proto::Publisher::send(std::string message) {
  std::string data = topic + message;
  zmqpp::message transmission;
  transmission << data;
  socket->send(transmission);
  // std::cout << "sending data: " << message << " to topic: " << topic << std::endl;
}